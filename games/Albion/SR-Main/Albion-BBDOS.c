/**
 *
 *  Copyright (C) 2018-2025 Roman Pauer
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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
#include <io.h>
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include "Albion-BBDOS.h"
#include "Albion-BBERROR.h"
#include "Albion-BBBASMEM.h"
#include "Game_defs.h"
// todo: remove
#include "Game_vars.h"
#include "virtualfs.h"

#if !defined(O_RDONLY)
    #define O_RDONLY _O_RDONLY
#endif
#if !defined(O_WRONLY)
    #define O_WRONLY _O_WRONLY
#endif
#if !defined(O_RDWR)
    #define O_RDWR _O_RDWR
#endif
#if !defined(O_APPEND)
    #define O_APPEND _O_APPEND
#endif
#if !defined(O_CREAT)
    #define O_CREAT _O_CREAT
#endif
#if !defined(O_TRUNC)
    #define O_TRUNC _O_TRUNC
#endif
#if !defined(O_BINARY)
    #if defined(_O_BINARY)
        #define O_BINARY _O_BINARY
    #else
        #define O_BINARY 0
    #endif
#endif


typedef struct {
    PTR32(const char) text;
    PTR32(const char) filename;
    int32_t data;
    int16_t oserror;
} DOS_ErrorStruct;

typedef struct {
    unsigned int flags;
    int fd;
} DOS_EntryStruct;

#define NUM_ENTRIES 50
#define HISTORY_SIZE 32

static int DOS_initialized = 0;
static int DOS_lseek_origins[3] = {SEEK_CUR, SEEK_SET, SEEK_END};
static unsigned int DOS_history_index = 0;
#if 0
static char DOS_getcwd_buffer[144];
#endif
static char DOS_filenames[NUM_ENTRIES][128];
static DOS_EntryStruct DOS_file_entries[NUM_ENTRIES];
static char DOS_filenames_history[HISTORY_SIZE][128];


static void DOS_LocalPrintError(char *buffer, const uint8_t *data);


int32_t DOS_Init(void)
{
    int file_handle;

    DOS_history_index = 0;
    if (!BASEMEM_Init())
    {
        DOS_ErrorStruct data;

        data.text = "DOS_Init: Cannot init BASEMEM";
        data.filename = " ";
        data.data = 0;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return 0;
    }

    if (DOS_initialized)
    {
        return 1;
    }

    for (file_handle = 0; file_handle < NUM_ENTRIES; file_handle++)
    {
        DOS_file_entries[file_handle].flags = 0;
    }

    DOS_initialized = 1;
    return 1;
}

void DOS_Exit(void)
{
    if (DOS_initialized)
    {
        int file_handle;

        for (file_handle = 0; file_handle < NUM_ENTRIES; file_handle++)
        {
            if (DOS_file_entries[file_handle].flags & 1)
            {
                DOS_Close(file_handle);
            }
        }

        DOS_initialized = 0;
    }
}

int32_t DOS_Open(const char *path, uint32_t mode)
{
    int file_handle, open_flags;
    char temp_str[MAX_PATH];
    int fd;
    file_entry *realdir;
    int vfs_err;


    mode = (uint16_t) mode;

    for (file_handle = 0; file_handle < NUM_ENTRIES; file_handle++)
    {
        if (DOS_file_entries[file_handle].flags == 0)
        {
            break;
        }
    }

    if (file_handle >= NUM_ENTRIES)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_Open: No free entry";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = 0;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return -1;
    }

    switch (mode)
    {
        case DOS_OPEN_MODE_READ:
            open_flags = O_BINARY | O_RDONLY;
            break;
        case DOS_OPEN_MODE_CREATE:
            open_flags = O_BINARY | O_WRONLY | O_CREAT | O_TRUNC;
            break;
        case DOS_OPEN_MODE_RDWR:
            open_flags = O_BINARY | O_RDWR;
            break;
        case DOS_OPEN_MODE_APPEND:
            open_flags = O_BINARY | O_WRONLY | O_APPEND;
            break;
        default:
            open_flags = O_RDONLY;
            break;
    }

    vfs_err = vfs_get_real_name(path, (char *) &temp_str, &realdir);

    if (open_flags | (O_APPEND | O_CREAT))
    {
        fd = open((const char *) &temp_str, open_flags, 0x1FF);
    }
    else
    {
        fd = open((const char *) &temp_str, open_flags);
    }

    if (fd == -1)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_Open: open() oserror";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = 0;
        data.oserror = errno;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return -1;
    }

    if (vfs_err)
    {
        vfs_add_file(realdir, (const char *) &temp_str, 0);
    }

    DOS_file_entries[file_handle].flags = mode | 1;
    DOS_file_entries[file_handle].fd = fd;
    strcpy(DOS_filenames[file_handle], path);

#if 0
    if (0 == strcmp(path, "XLDLIBS\\CURRENT\\MERCHDT2.XLD"))
    {
        off_t origpos, endpos;

        origpos = lseek(fd, 0, SEEK_CUR);
        if (origpos != -1)
        {
            endpos = lseek(fd, 0, SEEK_END);
            lseek(fd, origpos, SEEK_SET);
        }
    }
#endif

    return (int16_t) file_handle;
}

int32_t DOS_Close(int32_t file_handle)
{
    file_handle = (int16_t) file_handle;

    if (!(DOS_file_entries[file_handle].flags & 1))
    {
        DOS_ErrorStruct data;

        data.text = "DOS_Close: File is not open";
        data.filename = " ";
        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return 0;
    }

    close(DOS_file_entries[file_handle].fd);
    DOS_file_entries[file_handle].flags = 0;
    return 1;
}

int32_t DOS_Read(int32_t file_handle, void *buffer, uint32_t length)
{
    int retval;

    file_handle = (int16_t) file_handle;

    if (!(DOS_file_entries[file_handle].flags & 1))
    {
        DOS_ErrorStruct data;

        data.text = "DOS_Read: File not open";
        data.filename = " ";
        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return -1;
    }

    retval = read(DOS_file_entries[file_handle].fd, buffer, length);

    if (retval != (int)length)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_Read: read() oserror";

        strcpy(DOS_filenames_history[DOS_history_index], DOS_filenames[file_handle]);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = file_handle;
        data.oserror = errno;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return -1;
    }

    return retval;
}

int32_t DOS_Write(int32_t file_handle, const void *buffer, uint32_t length)
{
    int retval;

    file_handle = (int16_t) file_handle;

    if (!(DOS_file_entries[file_handle].flags & 1))
    {
        DOS_ErrorStruct data;

        data.text = "DOS_Write: File not open";
        data.filename = " ";
        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return -1;
    }

    retval = write(DOS_file_entries[file_handle].fd, buffer, length);

    if (retval == -1)
    {
        int err;
        DOS_ErrorStruct data;

        err = errno;

        // todo: remove
        errno_val = ((err >= 0) && (err < 256))?(errno_rtable[err]):(err);

        data.text = "DOS_Write: write() oserror";

        strcpy(DOS_filenames_history[DOS_history_index], DOS_filenames[file_handle]);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = file_handle;
        data.oserror = err;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return -1;
    }

    return retval;
}

int32_t DOS_Seek(int32_t file_handle, int32_t origin, int32_t offset)
{
    off_t curpos;

    file_handle = (int16_t) file_handle;

    if (!(DOS_file_entries[file_handle].flags & 1))
    {
        DOS_ErrorStruct data;

        data.text = "DOS_Seek: File not open";
        data.filename = " ";
        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return 0;
    }

    curpos = lseek(DOS_file_entries[file_handle].fd, offset, DOS_lseek_origins[origin]);
    if (curpos < 0 || curpos > 2147483647)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_Seek: SetFPos() oserror";

        strcpy(DOS_filenames_history[DOS_history_index], DOS_filenames[file_handle]);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = file_handle;
        data.oserror = errno;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return 0;
    }

    return 1;
}

#if 0
static void *DOS_ReadFile(const char *path, void *buffer, unsigned int buf_len)
{
    int file_handle, length, buf_allocated;

    buf_allocated = 0;
    length = DOS_GetFileLength(path);
    if (length < 0)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_ReadFile: DOS_GetFileLength() error";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = 0;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return 0;
    }

    if (buffer == NULL)
    {
        buffer = BASEMEM_Alloc(length, BASEMEM_XMS_MEMORY | BASEMEM_ZERO_MEMORY);
        if (buffer == NULL)
        {
            DOS_ErrorStruct data;

            data.text = "DOS_ReadFile: No Mem for File Buf";

            strcpy(DOS_filenames_history[DOS_history_index], path);
            data.filename = DOS_filenames_history[DOS_history_index];
            DOS_history_index++;
            if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

            data.data = length;
            data.oserror = 0;

            ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

            return 0;
        }

        buf_allocated |= 1;
    }

    file_handle = DOS_Open(path, DOS_OPEN_MODE_READ);
    if (file_handle < 0)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_ReadFile: xxx DOS_Open() error";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        if (buf_allocated & 1)
        {
            BASEMEM_Free(buffer);
        }

        return 0;
    }

    if (DOS_Read(file_handle, buffer, length) < 0)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_ReadFile: DOS_Read() error";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        DOS_Close(file_handle);
        return 0;
    }

    DOS_Close(file_handle);
    return buffer;
}

static int DOS_WriteFile(const char *path, const void *buffer, unsigned int length)
{
    int file_handle;

    file_handle = DOS_Open(path, DOS_OPEN_MODE_CREATE);
    if (file_handle < 0)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_WriteFile: DOS_Open() error";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return 0;
    }

    if (DOS_Write(file_handle, buffer, length) < 0)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_WriteFile: DOS_Write() error";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        DOS_Close(file_handle);
        return 0;
    }

    DOS_Close(file_handle);
    return 1;
}
#endif

int32_t DOS_GetFileLength(const char *path)
{
    int file_handle;
    off_t origpos, endpos;

    file_handle = DOS_Open(path, DOS_OPEN_MODE_READ);
    if (file_handle < 0)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_GetFileLength: Open error";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return -1;
    }

    origpos = lseek(DOS_file_entries[file_handle].fd, 0, SEEK_CUR);
    if (origpos == -1)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_GetFileLength: filelength error";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return -1;
    }

    endpos = lseek(DOS_file_entries[file_handle].fd, 0, SEEK_END);
    lseek(DOS_file_entries[file_handle].fd, origpos, SEEK_SET);

    DOS_Close(file_handle);
    return (endpos < 0 || endpos > 2147483647) ? -1 : endpos;
}

int32_t DOS_exists(const char *path)
{
    char temp_str[MAX_PATH];
    int fd;
    file_entry *realdir;
    int vfs_err;

    vfs_err = vfs_get_real_name(path, (char *) &temp_str, &realdir);

    fd = open((const char *) &temp_str, O_RDONLY | O_BINARY);
    if (fd == -1)
    {
        if (errno == ENOENT)
        {
            return 0;
        }
        else
        {
            DOS_ErrorStruct data;

            data.text = "DOS_exists: unknown error";

            strcpy(DOS_filenames_history[DOS_history_index], path);
            data.filename = DOS_filenames_history[DOS_history_index];
            DOS_history_index++;
            if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

            data.data = 0;
            data.oserror = errno;

            ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

            return 0;
        }
    }
    else
    {
        close(fd);

        if (vfs_err)
        {
            vfs_add_file(realdir, (const char *) &temp_str, 0);
        }

        return 1;
    }
}

#if 0
static const char *DOS_getcurrentdir(void)
{
    file_entry *cur_dir;
    int len, err;
    const char *retval;

    cur_dir = vfs_get_current_dir();
    if (cur_dir->dos_fullname[1] == ':' && cur_dir->dos_fullname[2] == 0)
    {
        DOS_getcwd_buffer[0] = cur_dir->dos_fullname[0];
        DOS_getcwd_buffer[1] = ':';
        DOS_getcwd_buffer[2] = '\\';
        DOS_getcwd_buffer[3] = 0;

        retval = DOS_getcwd_buffer;
    }
    else
    {
        len = strlen(cur_dir->dos_fullname);
        if (len < 144)
        {
            strcpy(DOS_getcwd_buffer, cur_dir->dos_fullname);
            retval = DOS_getcwd_buffer;
        }
        else
        {
            err = ERANGE;
            retval = NULL;
        }
    }

    if (retval == NULL)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_getcurrentdir: unknown error";

        strcpy(DOS_filenames_history[DOS_history_index], "---");
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = 0;
        data.oserror = err;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return NULL;
    }

    return retval;
}
#endif

int32_t DOS_setcurrentdir(const char *path)
{
    file_entry *new_dir;

#if 0
    if (strlen(path) > 2)
    {
        if (path[1] == ':')
        {
            // _dos_setdrive
        }
    }
#endif

    new_dir = vfs_set_current_dir(path);

    if (new_dir == NULL)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_setcurrentdir: unknown error";

        strcpy(DOS_filenames_history[DOS_history_index], path);
        data.filename = DOS_filenames_history[DOS_history_index];
        DOS_history_index++;
        if (DOS_history_index >= HISTORY_SIZE) DOS_history_index = 0;

        data.data = 0;
        data.oserror = ENOENT;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return 0;
    }

    return 1;
}

#if 0
static int DOS_eof(int file_handle)
{
    off_t origpos, endpos;

    file_handle = (int16_t) file_handle;

    if (!(DOS_file_entries[file_handle].flags & 1))
    {
        DOS_ErrorStruct data;

        data.text = "DOS_eof: File not open";
        data.filename = " ";
        data.data = file_handle;
        data.oserror = errno;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return 0;
    }

    origpos = lseek(DOS_file_entries[file_handle].fd, 0, SEEK_CUR);
    if (origpos == -1)
    {
        DOS_ErrorStruct data;

        data.text = "DOS_eof: invalid File Handle";
        data.filename = " ";
        data.data = file_handle;
        data.oserror = errno;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return -1;
    }


    endpos = lseek(DOS_file_entries[file_handle].fd, 0, SEEK_END);
    lseek(DOS_file_entries[file_handle].fd, origpos, SEEK_SET);

    return (origpos == endpos)?1:0;
}
#endif

int32_t DOS_GetSeekPosition(int32_t file_handle)
{
    off_t curpos;

    file_handle = (int16_t) file_handle;

    if (!(DOS_file_entries[file_handle].flags & 1))
    {
        DOS_ErrorStruct data;

        data.text = "DOS_GetSeekPosition: File not open";
        data.filename = " ";
        data.data = file_handle;
        data.oserror = 0;

        ERROR_PushError(DOS_LocalPrintError, "BBDOS Library", sizeof(data), (const uint8_t *) &data);

        return 0;
    }

    curpos = lseek(DOS_file_entries[file_handle].fd, 0, SEEK_CUR);
    return (curpos < 0 || curpos > 2147483647) ? -1 : curpos;
}

static void DOS_LocalPrintError(char *buffer, const uint8_t *data)
{
#define DATA (((DOS_ErrorStruct *)data))
    sprintf(buffer, " %s - FILENAME: %s - DATA: %ld - OSERROR: %d", (const char *)DATA->text, (const char *)DATA->filename, (long int)DATA->data, (int)DATA->oserror);
#undef DATA
}

