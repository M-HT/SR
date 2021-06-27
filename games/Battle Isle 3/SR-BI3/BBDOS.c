/**
 *
 *  Copyright (C) 2020-2021 Roman Pauer
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

#include "BBDOS.h"
#include "BBMEM.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


typedef struct {
    unsigned int flags;
    HFILE handle;
} DOS_struct;


static int DOS_initialized = 0;
static DOS_struct DOS_handles[256];


int DOS_Init_c(void)
{
    int count, index;

    if (DOS_initialized)
    {
        return 1;
    }

    count = SetHandleCount(255);
    for (index = 0; index < count; index++)
    {
        DOS_handles[index].flags = 0;
    }
    for (index = count; index < 256; index++)
    {
        DOS_handles[index].flags = 1;
    }

    DOS_initialized = 1;
    return 1;
}

void DOS_Exit_c(void)
{
    int index;

    if (DOS_initialized)
    {
        DOS_initialized = 0;

        for (index = 0; index < 256; index++)
        {
            if (DOS_handles[index].flags & 1)
            {
                DOS_Close_c(index);
            }
        }
    }
}

int DOS_Open_c(const char *path, unsigned int mode)
{
    int iReadWrite, append, index;

    iReadWrite = OF_READ;
    append = 0;
    if ((mode & DOS_OPEN_MODE_READ) && (mode & DOS_OPEN_MODE_WRITE))
    {
        iReadWrite = OF_READWRITE;
    }
    else if (mode & DOS_OPEN_MODE_WRITE)
    {
        iReadWrite = OF_WRITE;
    }
    else if (mode & DOS_OPEN_MODE_APPEND)
    {
        iReadWrite = OF_WRITE;
        append = 1;
    }

    for (index = 0; index < 256; index++)
    {
        if (DOS_handles[index].flags == 0) break;
    }
    if (index == 256)
    {
        return -1;
    }

    if ((!append) && (iReadWrite != OF_READ))
    {
        DOS_handles[index].handle = _lcreat(path, 0); // 0 =  Normal. Can be read from or written to without restriction.
        if (DOS_handles[index].handle == HFILE_ERROR)
        {
            return -1;
        }
    }
    else
    {
        DOS_handles[index].handle = _lopen(path, iReadWrite);
        if (DOS_handles[index].handle == HFILE_ERROR)
        {
            if (iReadWrite == OF_READ)
            {
                return -1;
            }

            DOS_handles[index].handle = _lcreat(path, 0); // 0 =  Normal. Can be read from or written to without restriction.
            if (DOS_handles[index].handle == HFILE_ERROR)
            {
                return -1;
            }
        }
        else if (append)
        {
            _llseek(DOS_handles[index].handle, 0, 2); // 2 = Moves the pointer from the end of the file.
        }
    }

    DOS_handles[index].flags = mode | 1;
    return index;
}

int DOS_Close_c(int file_handle)
{
    file_handle = (int16_t)file_handle;

    if (!(DOS_handles[file_handle].flags & 1))
    {
        return 0;
    }

    _lclose(DOS_handles[file_handle].handle);
    DOS_handles[file_handle].flags = 0;
    return 1;
}

int DOS_Read_c(int file_handle, void *buffer, unsigned int length)
{
    file_handle = (int16_t)file_handle;

    if (!(DOS_handles[file_handle].flags & 1))
    {
        return -1;
    }

    return _lread(DOS_handles[file_handle].handle, buffer, length);
}

int DOS_Write_c(int file_handle, const void *buffer, unsigned int length)
{
    unsigned int total_length, to_write, written;

    file_handle = (int16_t)file_handle;

    if (!(DOS_handles[file_handle].flags & 1))
    {
        return -1;
    }

    total_length = length;
    while (length != 0)
    {
        to_write = length;
        if (to_write >= 65535)
        {
            to_write = 65535;
        }
        written = _lwrite(DOS_handles[file_handle].handle, buffer, to_write);
        if (written != to_write)
        {
            return -1;
        }
        buffer = (const void *)(written + (uintptr_t)buffer);
        length -= written;
    };

    return total_length;
}

int DOS_Seek_c(int file_handle, int origin, int offset)
{
    LONG res;

    file_handle = (int16_t)file_handle;

    if (!(DOS_handles[file_handle].flags & 1))
    {
        return 0;
    }

    origin = (int16_t)origin;

    if (origin == DOS_SEEK_CUR)
    {
        res = _llseek(DOS_handles[file_handle].handle, offset, 1); // 1 = Moves the file from its current location.
    }
    else if (origin == DOS_SEEK_SET)
    {
        res = _llseek(DOS_handles[file_handle].handle, offset, 0); // 0 = Moves the pointer from the beginning of the file.
    }
    else if (origin == DOS_SEEK_END)
    {
        res = _llseek(DOS_handles[file_handle].handle, offset, 2); // 2 = Moves the pointer from the end of the file.
    }
    else
    {
        res = _llseek(DOS_handles[file_handle].handle, offset, 0); // 0 = Moves the pointer from the beginning of the file.
    }

    return (res != HFILE_ERROR)?1:0;
}

void *DOS_ReadFile_c(const char *path, void *buffer)
{
    int length, memalloc, file_handle;

    length = DOS_GetFileLength_c(path);
    if (length < 0)
    {
        return NULL;
    }

    memalloc = 0;
    if (buffer == NULL)
    {
        buffer = MEM_malloc_c(length, NULL, NULL, 0, 0);
        if (buffer == NULL)
        {
            return NULL;
        }
        memalloc = 1;
    }

    file_handle = DOS_Open_c(path, DOS_OPEN_MODE_READ);
    if (file_handle < 0)
    {
        if (memalloc & 1)
        {
            MEM_free_c(buffer);
        }
        return NULL;
    }

    if (DOS_Read_c(file_handle, buffer, length) < 0)
    {
        DOS_Close_c(file_handle);
        if (memalloc & 1)
        {
            MEM_free_c(buffer);
        }
        return NULL;
    }

    DOS_Close_c(file_handle);
    return buffer;
}

int DOS_WriteFile_c(const char *path, const void *buffer, unsigned int length)
{
    int file_handle;

    file_handle = DOS_Open_c(path, DOS_OPEN_MODE_WRITE);
    if (file_handle < 0)
    {
        return 0;
    }

    if (DOS_Write_c(file_handle, buffer, length) < 0)
    {
        DOS_Close_c(file_handle);
        return 0;
    }

    DOS_Close_c(file_handle);
    return 1;
}

int DOS_GetFileLength_c(const char *path)
{
    LONG res;
    int file_handle;

    file_handle = DOS_Open_c(path, DOS_OPEN_MODE_READ);
    if (file_handle < 0)
    {
        return -1;
    }

    res = _llseek(DOS_handles[file_handle].handle, 0, 2); // 2 = Moves the pointer from the end of the file.
    if (res == HFILE_ERROR)
    {
        DOS_Close_c(file_handle);
        return -1;
    }

    DOS_Close_c(file_handle);
    return res;
}

