/**
 *
 *  Copyright (C) 2016-2024 Roman Pauer
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
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-proc-vfs.h"
#include "Albion-proc.h"
#include "Game_thread.h"
#include "virtualfs.h"

static void Game_Set_errno_val(void)
{
    errno_val = Game_errno();
}

static void Game_Set_errno_error(int errornum)
{
    errno_val = (errornum >= 0 && errornum < 256)?(errno_rtable[errornum]):(errornum);
}


static int file_pattern_match(const char *filename, const char *pattern)
{
    int i, asterisk;

    if (pattern[0] == '*' && pattern[1] == '.' && pattern[2] == '*' && pattern[3] == 0 )
    {
        return 1;
    }

new_segment:
    asterisk = 0;

    if (*pattern == '*')
    {
        asterisk = 1;
        do
        {
            pattern++;
        } while (*pattern == '*');
    }

test_match:

    for (i = 0; (pattern[i] != 0) && (pattern[i] != '*'); i++)
    {
        if (filename[i] != pattern[i])
        {
            if (filename[i] == 0) return 0;
            if ( (pattern[i] == '?') && (filename[i] != '.') ) continue;
            if (!asterisk) return 0;
            filename++;
            goto test_match;
        }
    }

    if (pattern[i] == '*')
    {
        filename += i;
        pattern += i;
        goto new_segment;
    }
    if (filename[i] == 0) return 1;
    if ( (i != 0) && (pattern[i - 1] == '*') ) return 1;
    if (!asterisk) return 0;
    filename++;
    goto test_match;

    return 1;
}

/*

wildcard matching:
http://xoomer.alice.it/acantato/dev/wildcard/wildmatch.html

*/

int32_t Game_access(const char *path, int32_t mode)
{
    char temp_str[MAX_PATH];
    int ret;
    file_entry *realdir;
    int vfs_err;

#if defined(__DEBUG__)
    fprintf(stderr, "access: original name: %s\n", path);
#endif

    vfs_err = vfs_get_real_name(path, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
    fprintf(stderr, "access: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

    if (mode == 0)
    {
        if (vfs_err == 0)
        {
            return 0;
        }
        else
        {
            Game_Set_errno_error(ENOENT);
            return -1;
        }
    }
    else
    {
        ret = access((char *) &temp_str, mode);
        Game_Set_errno_val();

        return ret;
    }
}

int32_t Game_chdir(const char *path)
{
    file_entry *new_dir;

#if defined(__DEBUG__)
    fprintf(stderr, "chdir: path: %s\n", path);
#endif

    new_dir = vfs_set_current_dir(path);

    if (new_dir != NULL)
    {
        return 0;
    }
    else
    {
        Game_Set_errno_error(ENOENT);
        return -1;
    }
}

char *Game_getcwd(char *buf, int32_t size)
{
    file_entry *cur_dir;
    int len, addbackslash;

    if (size < 0)
    {
        Game_Set_errno_error(EINVAL);
        return NULL;
    }

    cur_dir = vfs_get_current_dir();

    len = strlen(cur_dir->dos_fullname);

    if (cur_dir->dos_fullname[1] == ':' && cur_dir->dos_fullname[2] == 0)
    {
        addbackslash = 1;
    }
    else
    {
        addbackslash = 0;
    }

    if (buf != NULL)
    {
        if ((uint32_t)size < (size_t) len + addbackslash + 1)
        {
            Game_Set_errno_error(ERANGE);
            return NULL;
        }
    }
    else
    {
        buf = (char *) malloc(len + addbackslash + 1);

        if (buf == NULL)
        {
            Game_Set_errno_error(ENOMEM);
            return NULL;
        }
    }

    strcpy(buf, cur_dir->dos_fullname);

    if (addbackslash)
    {
        buf[len] = '\\';
        buf[len + 1] = 0;
    }

#if defined(__DEBUG__)
    fprintf(stderr, "getcwd: path: %s\n", buf);
    fprintf(stderr, "getcwd: realpath: %s\n", cur_dir->real_fullname);
#endif

    return buf;
}

FILE *Game_fopen(const char *filename, const char *mode)
{
    char temp_str[MAX_PATH];
    FILE *ret;
    file_entry *realdir;
    int vfs_err;

#if defined(__DEBUG__)
    fprintf(stderr, "fopen: original name: %s\n", filename);
#endif

    vfs_err = vfs_get_real_name(filename, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
    fprintf(stderr, "fopen: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

    ret = fopen((char *) &temp_str, mode);
    Game_Set_errno_val();

    if (vfs_err && ret != NULL)
    {
        vfs_add_file(realdir, (char *) &temp_str, 0);
    }

    return ret;
}

int32_t Game_open(const char *pathname, int32_t flags, uint32_t mode)
{
    char temp_str[MAX_PATH];
    int ret;
    file_entry *realdir;
    int vfs_err;

#if defined(__DEBUG__)
    fprintf(stderr, "open: original name: %s\n", pathname);
#endif

    vfs_err = vfs_get_real_name(pathname, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
    fprintf(stderr, "open: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

    ret = open((char *) &temp_str, flags, mode);
    Game_Set_errno_val();

    if (vfs_err && ret != -1)
    {
        vfs_add_file(realdir, (char *) &temp_str, 0);
    }

    return ret;
}

int32_t Game_mkdir(const char *pathname)
{
    char temp_str[MAX_PATH];
    int ret;
    file_entry *realdir;
    int vfs_err;

#if defined(__DEBUG__)
    fprintf(stderr, "mkdir: original name: %s\n", pathname);
#endif

    vfs_err = vfs_get_real_name(pathname, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
    fprintf(stderr, "mkdir: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

#if defined(__MINGW32__)
    ret = mkdir((char *) &temp_str);
#else
    ret = mkdir((char *) &temp_str, 0777);
#endif
    Game_Set_errno_val();

    if (vfs_err && ret == 0)
    {
        vfs_add_file(realdir, (char *) &temp_str, 1);
    }

    return ret;
}

int32_t Game_unlink(const char *pathname)
{
    char temp_str[MAX_PATH];
    int ret;
    file_entry *realdir;
    int vfs_err;

#if defined(__DEBUG__)
    fprintf(stderr, "unlink: original name: %s\n", pathname);
#endif

    vfs_err = vfs_get_real_name(pathname, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
    fprintf(stderr, "unlink: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

    ret = unlink((char *) &temp_str);
    Game_Set_errno_val();

    if ( (vfs_err == 0) && (ret == 0) )
    {
        vfs_delete_entry(realdir);
    }

    return ret;
}

int32_t Game_rename(const char *oldpath, const char *newpath)
{
    char temp_str_old[MAX_PATH], temp_str_new[MAX_PATH];
    int ret;
    file_entry *realdir_old, *realdir_new;
    int vfs_err_old, vfs_err_new;

#if defined(__DEBUG__)
    fprintf(stderr, "rename: original names: %s --> %s\n", oldpath, newpath);
#endif

    vfs_err_old = vfs_get_real_name(oldpath, (char *) &temp_str_old, &realdir_old);
    vfs_err_new = vfs_get_real_name(newpath, (char *) &temp_str_new, &realdir_new);

#if defined(__DEBUG__)
    fprintf(stderr, "rename: real names: %s (%i) --> %s (%i)\n", (char *) &temp_str_old, vfs_err_old, (char *) &temp_str_new, vfs_err_new);
#endif

    if (vfs_err_old == 0)
    {
        ret = rename((char *) &temp_str_old, (char *) &temp_str_new);
        Game_Set_errno_val();

        if (ret == 0)
        {
            if (vfs_err_new)
            {
                vfs_add_file(realdir_new, (char *) &temp_str_new, realdir_old->attributes);
            }

            vfs_delete_entry(realdir_old);
        }

        return ret;
    }
    else
    {
        Game_Set_errno_error(EACCES);
        return -1;
    }
}

struct watcom_dirent *Game_opendir(const char *dirname)
{
    char temp_str[MAX_PATH], orig_directory[MAX_PATH];
    const char *pattern;
    struct watcom_dirent *ret;
    file_entry *realdir;
    int vfs_err, readdirtype;

    if (dirname == NULL) return NULL;

    // check for wildcards
    if (strchr(dirname, '*') != NULL)
    {
        readdirtype = 1;
    }
    else if (strchr(dirname, '?') != NULL)
    {
        readdirtype = 1;
    }
    else
    {
        readdirtype = 0;
    }

    if (readdirtype == 0)
    {
        // no wildcards
#if defined(__DEBUG__)
        fprintf(stderr, "opendir: original name: %s\n", dirname);
#endif

        vfs_err = vfs_get_real_name(dirname, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
        fprintf(stderr, "opendir: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

        if (vfs_err)
        {
            Game_Set_errno_error(ENOENT);
            return NULL;
        }

        if (realdir->attributes & 1)
        {
            // directory
            vfs_visit_dir(realdir);

            if (realdir->first_child == NULL)
            {
                Game_Set_errno_error(ENOENT);
                return NULL;
            }
        }

        // allocate and fill return structure
        ret = (struct watcom_dirent *) malloc(sizeof(struct watcom_dirent));

        if (ret == NULL)
        {
            Game_Set_errno_error(ENOMEM);
            return NULL;
        }

        memset((void *)ret, 0, sizeof(struct watcom_dirent));

        ret->u.v.realdir = realdir;
        ret->u.v.isfirst = 1;
        ret->u.v.readdirtype = 0;

        ret->d_first = 1;

        ret->d_openpath = strdup(dirname);
        if (ret->d_openpath == NULL)
        {
            free(ret);
            Game_Set_errno_error(ENOMEM);
            return NULL;
        }

        ret->u.v.pattern = NULL;

        return ret;
    }
    else
    {
        // wildcards

        // find last slash or backslash in dirname
        {
            const char *slash;

            slash = strrchr(dirname, '/');
            pattern = strrchr(dirname, '\\');

            if (pattern == NULL)
            {
                pattern = slash;
            }
            else
            {
                if (slash > pattern)
                {
                    pattern = slash;
                }
            }
        }

        // copy dirname without last entry (delimited by '\' or '/') to orig_directory
        if (pattern == NULL)
        {
            orig_directory[0] = 0;
        }
        else
        {
            strncpy(orig_directory, dirname, MAX_PATH - 1);
            orig_directory[MAX_PATH - 1] = 0;

            if (pattern - dirname < MAX_PATH)
            {
                orig_directory[pattern - dirname] = 0;
            }
        }

#if defined(__DEBUG__)
        fprintf(stderr, "opendir: original name: %s\n", orig_directory);
#endif

        vfs_err = vfs_get_real_name(orig_directory, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
        fprintf(stderr, "opendir: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

        if (vfs_err)
        {
            Game_Set_errno_error(ENOENT);
            return NULL;
        }

        if (realdir->attributes & 1)
        {
            // directory
            vfs_visit_dir(realdir);

            realdir = realdir->first_child;
        }
        else
        {
            // file
            Game_Set_errno_error(ENOENT);
            return NULL;
        }

        // copy pattern to orig_directory
        strncpy(orig_directory, ( (pattern != NULL)?(pattern + 1):dirname ), MAX_PATH - 1);
        orig_directory[MAX_PATH - 1] = 0;

        // convert pattern to uppercase
        {
            int i;

            for (i = 0; orig_directory[i] != 0; i++)
            {
                orig_directory[i] = toupper(orig_directory[i]);
            }
        }

        // find first name matching pattern
        while (realdir != NULL && !file_pattern_match(realdir->dos_name, orig_directory))
        {
            realdir = realdir->next;
        }

        if (realdir == NULL)
        {
            Game_Set_errno_error(ENOENT);
            return NULL;
        }

        // allocate and fill return structure
        ret = (struct watcom_dirent *) malloc(sizeof(struct watcom_dirent));

        if (ret == NULL)
        {
            Game_Set_errno_error(ENOMEM);
            return NULL;
        }

        memset((void *)ret, 0, sizeof(struct watcom_dirent));

        ret->u.v.realdir = realdir;
        ret->u.v.isfirst = 0;
        ret->u.v.readdirtype = 1;

        ret->d_first = 1;

        ret->d_openpath = strdup(dirname);
        if (ret->d_openpath == NULL)
        {
            free(ret);
            Game_Set_errno_error(ENOMEM);
            return NULL;
        }

        ret->u.v.pattern = strdup(orig_directory);

        if (ret->u.v.pattern == NULL)
        {
            free(ret->d_openpath);
            free(ret);
            Game_Set_errno_error(ENOMEM);
            return NULL;
        }

        return ret;
    }
}

#define WATCOM_A_NORMAL       0x00    /* Normal file - read/write permitted */
#define WATCOM_A_RDONLY       0x01    /* Read-only file */
#define WATCOM_A_HIDDEN       0x02    /* Hidden file */
#define WATCOM_A_SYSTEM       0x04    /* System file */
#define WATCOM_A_VOLID        0x08    /* Volume-ID entry */
#define WATCOM_A_SUBDIR       0x10    /* Subdirectory */
#define WATCOM_A_ARCH         0x20    /* Archive file */

struct watcom_dirent *Game_readdir(struct watcom_dirent *dirp)
{
    file_entry *current_entry;
    struct stat statbuf;
    struct tm *tmbuf;

#define REALDIR ((file_entry *)(void *) (dirp->u.v.realdir))

    if (dirp == NULL) return NULL;

    if (dirp->u.v.readdirtype == 0)
    {
        // no wildcards

        if (REALDIR == NULL)
        {
            return NULL;
        }

        // todo: '.' and '..' are not needed in fallout
        if (dirp->u.v.isfirst)
        {
            dirp->u.v.isfirst = 0;

            if (REALDIR->attributes & 1)
            {
                current_entry = REALDIR->first_child;
                dirp->u.v.realdir = current_entry->next;
            }
            else
            {
                current_entry = REALDIR;
                dirp->u.v.realdir = NULL;
            }
        }
        else
        {
            current_entry = REALDIR;
            dirp->u.v.realdir = REALDIR->next;
        }
    }
    else
    {
        // wildcards

        if (REALDIR == NULL)
        {
            return NULL;
        }

        current_entry = REALDIR;
        dirp->u.v.realdir = REALDIR->next;

        while (REALDIR != NULL && !file_pattern_match(REALDIR->dos_name, dirp->u.v.pattern))
        {
            dirp->u.v.realdir = REALDIR->next;
        }
    }

    dirp->d_attr = (current_entry->attributes & 1)?WATCOM_A_SUBDIR:WATCOM_A_NORMAL;


    if ( stat(current_entry->real_fullname, &statbuf) )
    {
        dirp->ut.d_time = 0;
        dirp->ud.d_date = 0;
        dirp->d_size = 0;
    }
    else
    {
        tmbuf = localtime(&(statbuf.st_mtime));

        dirp->ut.vt.twosecs = tmbuf->tm_sec / 2;
        dirp->ut.vt.minutes = tmbuf->tm_min;
        dirp->ut.vt.hours   = tmbuf->tm_hour;

        dirp->ud.vd.day     = tmbuf->tm_mday;
        dirp->ud.vd.month   = tmbuf->tm_mon + 1;
        dirp->ud.vd.year    = tmbuf->tm_year - 80;

        dirp->d_size = statbuf.st_size;
    }

    strncpy(dirp->d_name, current_entry->dos_name, 13);
    dirp->d_name[12] = 0;

    dirp->d_first = 0;

    dirp->v_cur_res = current_entry;

    // todo: other unused fields

    return dirp;

#undef REALDIR
}

int32_t Game_closedir(struct watcom_dirent *dirp)
{
    if (dirp == NULL) return 0;

    if (dirp->u.v.pattern != NULL)
    {
        free(dirp->u.v.pattern);
        dirp->u.v.pattern = NULL;
    }

    if (dirp->d_openpath != NULL)
    {
        free(dirp->d_openpath);
        dirp->d_openpath = NULL;
    }

    dirp->u.v.realdir = NULL;

    free(dirp);

    return 0;
}

static void Conv_find(struct watcom_find_t *buffer, struct watcom_dirent *direntp)
{
    // file attributes
    buffer->attrib = direntp->d_attr;

    // file's modification time and date
    buffer->wr_time = direntp->ut.d_time;
    buffer->wr_date = direntp->ud.d_date;

    // file size
    buffer->size = direntp->d_size;

    // file name
    strcpy(buffer->name, direntp->d_name);

    // todo: other unused fields
}

#define DOS_FIND_SIGNATURE1 0xBDAECFAB
#define DOS_FIND_SIGNATURE2 0xED

uint32_t Game_dos_findfirst(const char *path, const uint32_t attributes, struct watcom_find_t *buffer)
{
    struct watcom_dirent *direntp;
#if defined(__DEBUG__)
    fprintf(stderr, "findfirst: %s\n", path);
#endif

    if (buffer->u.v.dirp != NULL)
    {
        if (buffer->u.v.signature1 == DOS_FIND_SIGNATURE1 &&
            buffer->u.v.signature2 == DOS_FIND_SIGNATURE2
           )
        {
            Game_closedir(buffer->u.v.dirp);
        }
        buffer->u.v.dirp = NULL;
    }

    buffer->u.v.signature1 = 0;
    buffer->u.v.signature2 = 0;

    if (attributes & WATCOM_A_VOLID)
    {
        Game_Set_errno_error(ENOENT);
        return 0xffffffff;
    }

    buffer->u.v.find_attrib = attributes & (WATCOM_A_HIDDEN | WATCOM_A_SYSTEM | WATCOM_A_SUBDIR);

    buffer->u.v.dirp = Game_opendir(path);

    if (buffer->u.v.dirp == NULL)
    {
        Game_Set_errno_error(ENOENT);
        return 0xffffffff;
    }

    buffer->u.v.signature1 = DOS_FIND_SIGNATURE1;
    buffer->u.v.signature2 = DOS_FIND_SIGNATURE2;

    direntp = Game_readdir(buffer->u.v.dirp);

    if (direntp == NULL)
    {
        Game_closedir(buffer->u.v.dirp);
        buffer->u.v.dirp = NULL;

        buffer->u.v.signature1 = 0;
        buffer->u.v.signature2 = 0;

        Game_Set_errno_error(ENOENT);

        return 0xffffffff;
    }
    else
    {
        Conv_find(buffer, direntp);

        if ( (uint8_t) ( buffer->attrib & (~buffer->u.v.find_attrib) & (WATCOM_A_HIDDEN | WATCOM_A_SYSTEM | WATCOM_A_SUBDIR) ) )
        {
            return Game_dos_findnext(buffer);
        }
        else
        {
#if defined(__DEBUG__)
            fprintf(stderr, "found file: %s\n", buffer->name);
#endif

            return 0;
        }
    }

}

uint32_t Game_dos_findnext(struct watcom_find_t *buffer)
{
    struct watcom_dirent *direntp;

#if defined(__DEBUG__)
    fprintf(stderr, "findnext\n");
#endif

    direntp = Game_readdir(buffer->u.v.dirp);

    while (direntp != NULL)
    {
        Conv_find(buffer, direntp);

        if ( (uint8_t) ( buffer->attrib & (~buffer->u.v.find_attrib) & (WATCOM_A_HIDDEN | WATCOM_A_SYSTEM | WATCOM_A_SUBDIR) ) )
        {
            direntp = Game_readdir(buffer->u.v.dirp);
        }
        else
        {
#if defined(__DEBUG__)
            fprintf(stderr, "found file: %s\n", buffer->name);
#endif

            return 0;
        }
    }

    Game_closedir(buffer->u.v.dirp);
    buffer->u.v.dirp = NULL;

    buffer->u.v.signature1 = 0;
    buffer->u.v.signature2 = 0;

    Game_Set_errno_error(ENOENT);

    return 0xffffffff;

}

uint32_t Game_dos_findclose(struct watcom_find_t *buffer)
{
#if defined(__DEBUG__)
    fprintf(stderr, "findclose\n");
#endif

    if (buffer->u.v.dirp != NULL)
    {
        Game_closedir(buffer->u.v.dirp);
        buffer->u.v.dirp = NULL;
    }

    buffer->u.v.signature1 = 0;
    buffer->u.v.signature2 = 0;

    return 0;
}

