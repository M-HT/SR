/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#define _FILE_OFFSET_BITS 64
#define _TIME_BITS 64
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif
#include <sys/types.h>
#include <ctype.h>
#include "virtualfs.h"
#include "Game_defs.h"

file_entry Game_CDir, *Game_Current_Dir;

int vfs_init(int relative)
{
    char cur_dir[MAX_PATH];

    Game_CDir.dos_name[0] = 'C';
    Game_CDir.dos_name[1] = ':';
    Game_CDir.dos_name[2] = 0;

    Game_CDir.real_name[0] = '.';
    Game_CDir.real_name[1] = 0;

    Game_CDir.dos_fullname = strdup(Game_CDir.dos_name);

    if (relative)
    {
        Game_CDir.real_fullname = strdup(Game_CDir.real_name);
    }
    else
    {
        if ( getcwd(cur_dir, MAX_PATH) == NULL )
        {
            return -1;
        }

        Game_CDir.real_fullname = strdup(cur_dir);
    }

    Game_CDir.attributes = 1;
    Game_CDir.dir_visited = 0;

    Game_CDir.parent = &Game_CDir;
    Game_CDir.next = NULL;
    Game_CDir.prev = NULL;
    Game_CDir.first_child = NULL;

    Game_Current_Dir = &Game_CDir;

    vfs_visit_dir(&Game_CDir);

    return 0;
}

void vfs_delete_entry(file_entry *entry)
{
    file_entry *child;

    child = entry->first_child;
    while (child != NULL)
    {
        file_entry *next_child;

        next_child = child->next;

        vfs_delete_entry(child);

        child = next_child;
    }

    if (entry->dos_fullname != NULL)
    {
        free(entry->dos_fullname);
        entry->dos_fullname = NULL;
    }
    if (entry->real_fullname != NULL)
    {
        free(entry->real_fullname);
        entry->real_fullname = NULL;
    }

    if (entry->prev != NULL)
    {
        entry->prev->next = entry->next;
    }
    else
    {
        if (entry->parent != entry)
        {
            entry->parent->first_child = entry->next;
        }
    }

    if (entry->next != NULL)
    {
        entry->next->prev = entry->prev;
    }

    if (entry->parent != entry)
    {
        free(entry);
    }
}

void vfs_visit_dir(file_entry *vdir)
{
    {
        file_entry *child;

        child = vdir->first_child;
        while (child != NULL)
        {
            file_entry *next_child;

            next_child = child->next;

            vfs_delete_entry(child);

            child = next_child;
        }
    }

    vdir->dir_visited = 0;

    {
        file_entry *new_child, *last_child;
        int vdir_doslength, vdir_reallength, file_reallength;
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
        intptr_t dir;
        struct _finddata_t file;
#else
        DIR *dir;
        struct dirent *file;
        struct stat file_stat;
#endif
        char new_filename[MAX_PATH];

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
        if (strlen(vdir->real_fullname) >= MAX_PATH - 4) return;
        strcpy(new_filename, vdir->real_fullname);
        strcat(new_filename, "\\*.*");
        dir = _findfirst(new_filename, &file);
        if (dir < 0) return;
#else
        dir = opendir(vdir->real_fullname);
        if (dir == NULL) return;
#endif

        last_child = NULL;
        vdir_doslength = strlen(vdir->dos_fullname);
        vdir_reallength = strlen(vdir->real_fullname);

        strcpy(new_filename, vdir->real_fullname);
        new_filename[vdir_reallength] = '/';

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define FILE_NAME file.name
    #define CLOSE_DIR(a) _findclose(a)
        do
#else
    #define FILE_NAME file->d_name
    #define CLOSE_DIR(a) closedir(a)
        for (file = readdir(dir); file != NULL; file = readdir(dir))
#endif
        {
            // skip '.' and '..'
            if (FILE_NAME[0] == '.' &&
                (FILE_NAME[1] == 0 || (FILE_NAME[1] == '.' && FILE_NAME[2] == 0))
                )
            {
                continue;
            }

            // skip long names
            file_reallength = strlen(FILE_NAME);
            if (file_reallength > 12) continue;

            {
                char *ext;

                ext = strchr(FILE_NAME, '.');

                if (ext == NULL)
                {
                    // file without extension

                    // skip long names
                    if (file_reallength > 8) continue;
                }
                else
                {
                    // file with extension

                    // skip long names
                    if (ext - FILE_NAME > 8) continue;
                    ext++;

                    // skip file with more extensions
                    if ( strchr(ext, '.') != NULL ) continue;

                    // skip long extensions
                    if (strlen(ext) > 3) continue;
                }
            }

            strcpy(&(new_filename[vdir_reallength + 1]), FILE_NAME);

#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
            if (stat(new_filename, &file_stat)) continue;

            // only read regular files and directories
            if (!S_ISREG(file_stat.st_mode) && !S_ISDIR(file_stat.st_mode)) continue;
#endif

            // allocate mem for new file entry
            new_child = (file_entry *) malloc(sizeof(file_entry));
            if (new_child == NULL)
            {
                CLOSE_DIR(dir);
                return;
            }

            memset(new_child, 0, sizeof(file_entry));

            // set real and dos name
            strcpy(new_child->real_name, FILE_NAME);

            {
                int i;

                for (i = 0; new_child->real_name[i] != 0; i++)
                {
                    new_child->dos_name[i] = toupper(new_child->real_name[i]);
                }
                new_child->dos_name[i] = 0;
            }

            // set real and dos full name

            new_child->dos_fullname = (char *) malloc(file_reallength + vdir_doslength + 2);
            new_child->real_fullname = (char *) malloc(file_reallength + vdir_reallength + 2);
            if (new_child->dos_fullname == NULL ||
                new_child->real_fullname == NULL
                )
            {
                CLOSE_DIR(dir);
                return;
            }

            strcpy(new_child->dos_fullname, vdir->dos_fullname);
            new_child->dos_fullname[vdir_doslength] = '\\';
            strcpy(&(new_child->dos_fullname[vdir_doslength + 1]), new_child->dos_name);

            strcpy(new_child->real_fullname, new_filename);

            // set attributes
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
            new_child->attributes = (file.attrib & _A_SUBDIR)?1:0;
#else
            new_child->attributes = (S_ISDIR(file_stat.st_mode))?1:0;
#endif

            // set pointers
            new_child->parent = vdir;
            new_child->prev = last_child;

            if (last_child != NULL)
            {
                last_child->next = new_child;
            }
            else
            {
                vdir->first_child = new_child;
            }

            last_child = new_child;
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
        } while (_findnext(dir, &file) >= 0);
#else
        }
#endif

        CLOSE_DIR(dir);
    }
#undef FILE_NAME
#undef CLOSE_DIR

    vdir->dir_visited = 1;
}

/*
return value:
0 - dos path found (realdir = found entry)
1 - last entry ('\' delimited) in dos path not found (realdir = last directory parsed found in dos path)
2 - entry before last entry ('\' delimited) in dos path not found (realdir = last directory parsed found in dos path)
*/
int vfs_get_real_name(const char *origdosname, char *buf, file_entry **realdir)
{
    char upperdosname[MAX_PATH], *dosname, *backslash;
    file_entry *parse_dir, *new_parse_dir;
    int ret;

    // convert dos name to uppercase
    {
        int i;

        for (i = 0; origdosname[i] != 0 && i < MAX_PATH - 1; i++)
        {
            if (origdosname[i] == '/')
            {
                upperdosname[i] = '\\';
            }
            else
            {
                upperdosname[i] = toupper(origdosname[i]);
            }
        }
        upperdosname[i] = 0;
    }
    dosname = &(upperdosname[0]);


    // find initial directory for parsing
    if (dosname[0] == '\\')
    {
        parse_dir = &Game_CDir;
        dosname++;
    }
    else if (dosname[0] == 'C' && dosname[1] == ':')
    {
        if (dosname[2] == '\\')
        {
            parse_dir = &Game_CDir;
            dosname+=3;
        }
        else
        {
            parse_dir = Game_Current_Dir;
            dosname+=2;
        }
    }
    else
    {
        parse_dir = Game_Current_Dir;
    }

    // find directory
    for (backslash = strchr(dosname, '\\'); backslash != NULL; backslash = strchr(dosname, '\\'))
    {
        *backslash = 0;

        if (backslash[1] == 0)
        {
            backslash = NULL;
            break;
        }

        if (!parse_dir->dir_visited)
        {
            vfs_visit_dir(parse_dir);
        }

        // check '.' and '..'
        if (dosname[0] == '.')
        {
            if (dosname[1] == 0)
            {
                dosname = backslash + 1;
                continue;
            }
            else if (dosname[1] == '.' && dosname[2] == 0)
            {
                parse_dir = parse_dir->parent;
                dosname = backslash + 1;
                continue;
            }
        }

        for (new_parse_dir = parse_dir->first_child; new_parse_dir != NULL; new_parse_dir = new_parse_dir->next)
        {
            if ( strcmp(dosname, new_parse_dir->dos_name) == 0)
            {
                if ( !(new_parse_dir->attributes & 1) )
                {
                    new_parse_dir = NULL;
                }
                break;
            }
        }

        if (new_parse_dir == NULL)
        {
            *backslash = '\\';
            break;
        }
        else
        {
            parse_dir = new_parse_dir;
            dosname = backslash + 1;
        }
    }

    if (realdir != NULL) *realdir = parse_dir;

    // find file/directory
    if (backslash != NULL)
    {
        ret = 2;
        new_parse_dir = NULL;
    }
    else if (*dosname == 0)
    {
        ret = 0;
        new_parse_dir = parse_dir;
    }
    else
    {
        if (!parse_dir->dir_visited)
        {
            vfs_visit_dir(parse_dir);
        }

        ret = 1;
        for (new_parse_dir = parse_dir->first_child; new_parse_dir != NULL; new_parse_dir = new_parse_dir->next)
        {
            if ( strcmp(dosname, new_parse_dir->dos_name) == 0)
            {
                ret = 0;
                if (realdir != NULL) *realdir = new_parse_dir;
                break;
            }
        }
    }

    if (buf != NULL)
    {
        if (new_parse_dir == NULL)
        {
            // file/directory not found
            char *realname;
            int buflen;

            buf[MAX_PATH - 1] = 0;
            strncpy(buf, parse_dir->real_fullname, MAX_PATH - 1);

            buflen = strlen(buf);

            if (buflen < MAX_PATH - 1)
            {
                realname = &(buf[buflen]);
                *realname = '/';
                realname++;
                buflen++;

                // convert dos name to lowercase
                {
                    int i;

                    // origdosname might have '\' or '/' at the end, but dosname doesn't
                    for (i = 0; dosname[i] != 0 && buflen + i < MAX_PATH - 1; i++)
                    {
                        if (dosname[i] == '\\')
                        {
                            realname[i] = '/';
                        }
                        else
                        {
                            realname[i] = tolower(dosname[i]);
                        }
                    }
                    realname[i] = 0;
                }
            }
        }
        else
        {
            // file/directory found
            buf[MAX_PATH - 1] = 0;
            strncpy(buf, new_parse_dir->real_fullname, MAX_PATH - 1);
        }
    }

    return ret;
}

void vfs_add_file(file_entry *dir, const char *filepath)
{
    const char *filename;
    file_entry *new_child, *last_child;
    int dir_doslength;


    filename = &(filepath[strlen(dir->real_fullname) + 1]);

    if (strchr(filename, '/') == NULL)
    {
        // allocate mem for new file entry
        new_child = (file_entry *) malloc(sizeof(file_entry));
        if (new_child == NULL)
        {
            return;
        }

        memset(new_child, 0, sizeof(file_entry));

        // set real and dos name
        strcpy(new_child->real_name, filename);

        {
            int i;

            for (i = 0; filename[i] != 0; i++)
            {
                new_child->dos_name[i] = toupper(filename[i]);
            }
            new_child->dos_name[i] = 0;
        }

        // set real and dos full name
        new_child->real_fullname = strdup(filepath);

        dir_doslength = strlen(dir->dos_fullname);

        new_child->dos_fullname = (char *) malloc(strlen(filename) + dir_doslength + 2);

        strcpy(new_child->dos_fullname, dir->dos_fullname);
        new_child->dos_fullname[dir_doslength] = '\\';
        strcpy(&(new_child->dos_fullname[dir_doslength + 1]), new_child->dos_name);

        // set attributes
        new_child->attributes = 0;

        // set pointers
        new_child->parent = dir;

        if (dir->first_child == NULL)
        {
            dir->first_child = new_child;
        }
        else
        {
            last_child = dir->first_child;

            while (last_child->next != NULL)
            {
                last_child = last_child->next;
            }

            new_child->prev = last_child;

            last_child->next = new_child;
        }

    }
    else
    {
        // this shouldn't happen
#if defined(__DEBUG__)
        fprintf(stderr, "vfs_add_file: adding file in directory: %s\n", filename);
#endif
    }
}
