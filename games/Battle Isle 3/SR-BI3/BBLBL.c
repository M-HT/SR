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

#include "BBLBL.h"
#include "BBMEM.h"
#include "BBDOS.h"
#include "BBDEPACK.h"
#include <stddef.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#define GETLE32(_buf,_index) ( ((uint32_t)_buf[_index]) | (((uint32_t)_buf[(_index) + 1]) << 8) | (((uint32_t)_buf[(_index) + 2]) << 16) | (((uint32_t)_buf[(_index) + 3]) << 24) )
#define GETLE16(_buf,_index) ( ((uint32_t)_buf[_index]) | (((uint32_t)_buf[(_index) + 1]) << 8) )


typedef struct _LBL_Entry {
    unsigned int offset;
    unsigned int size;
    unsigned int uncompressed_size;
    uint8_t name[8];
} LBL_Entry;

typedef struct _LBL_Library {
    unsigned int entries_offset;
    //unsigned int field_4;
    unsigned int num_entries;
    //unsigned int field_A;
    LBL_Entry *entries;
    int file_handle;
    int current_entry;
    char path[MAX_PATH]; // change: increase path length
} LBL_Library;


static int LBL_initialized = 0;


int LBL_Init_c(void)
{
    LBL_initialized++;
    return 1;
}

void LBL_Exit_c(void)
{
    if (LBL_initialized > 0)
    {
        LBL_initialized--;
    }
}

void *LBL_OpenLib_c(const char *path, int param2)
{
    LBL_Library *library;
    uint8_t buffer12[12];
    unsigned int index;

    if (!LBL_initialized)
    {
        return NULL;
    }

    library = (LBL_Library *)MEM_malloc_c(sizeof(LBL_Library), NULL, NULL, 0, 0);
    if (library == NULL)
    {
        return NULL;
    }

    library->file_handle = DOS_Open_c(path, DOS_OPEN_MODE_READ);
    if (library->file_handle == -1)
    {
        MEM_free_c(library);
        return NULL;
    }

    if (DOS_Read_c(library->file_handle, buffer12, 12) != 12)
    {
        DOS_Close_c(library->file_handle);
        MEM_free_c(library);
        return NULL;
    }

    library->entries_offset = GETLE32(buffer12, 0);
    //library->field_4 = GETLE32(buffer12, 4);
    library->num_entries = GETLE16(buffer12, 8);
    //library->field_A = GETLE16(buffer12, 10);

    library->entries = (LBL_Entry *)MEM_malloc_c(sizeof(LBL_Entry) * (library->num_entries + 1), NULL, NULL, 0, 0);
    if (library->entries == NULL)
    {
        DOS_Close_c(library->file_handle);
        MEM_free_c(library);
        return NULL;
    }

    // change: replace nonsense condition
    //if ((DOS_Seek_c(library->file_handle, DOS_SEEK_SET, library->entries_offset) & 0xff) == -1)
    if (!DOS_Seek_c(library->file_handle, DOS_SEEK_SET, library->entries_offset))
    {
        DOS_Close_c(library->file_handle);
        MEM_free_c(library->entries);
        MEM_free_c(library);
        return NULL;
    }

    for (index = 0; index < library->num_entries; index++)
    {
        if (DOS_Read_c(library->file_handle, buffer12, 12) != 12)
        {
            DOS_Close_c(library->file_handle);
            MEM_free_c(library->entries);
            MEM_free_c(library);
            return NULL;
        }

        library->entries[index].offset = GETLE32(buffer12, 8);
        library->entries[index].size = 0;
        library->entries[index].uncompressed_size = 0;
        memmove(library->entries[index].name, buffer12, 8);
    }

    for (index = 0; index < library->num_entries; index++)
    {
        if (index >= (library->num_entries - 1))
        {
            library->entries[index].size = library->entries_offset - library->entries[index].offset;
        }
        else
        {
            library->entries[index].size = library->entries[index + 1].offset - library->entries[index].offset;
        }

        if (param2 & 0xff)
        {
            // change: replace nonsense condition
            //if ((DOS_Seek_c(library->file_handle, DOS_SEEK_SET, library->entries[index].offset) & 0xff) == -1)
            if (!DOS_Seek_c(library->file_handle, DOS_SEEK_SET, library->entries[index].offset))
            {
                DOS_Close_c(library->file_handle);
                MEM_free_c(library->entries);
                MEM_free_c(library);
                return NULL;
            }

            if (DOS_Read_c(library->file_handle, buffer12, 8) != 8)
            {
                DOS_Close_c(library->file_handle);
                MEM_free_c(library->entries);
                MEM_free_c(library);
                return NULL;
            }

            if (0 == memcmp(buffer12, "TPWM", 4))
            {
                library->entries[index].uncompressed_size = GETLE32(buffer12, 4);
            }
        }
    }

    strncpy(library->path, path, MAX_PATH); // change: increase path length
    library->current_entry = -1;

    DOS_Close_c(library->file_handle);
    library->file_handle = -1;

    return library;
}

void *LBL_ReadEntry_c(void *lib, void *entry_data, unsigned int entry_number, int close_file, void *entry_name)
{
    LBL_Library *library;
    void *compressed_data;
    unsigned int size;
    void *data;

    if (!LBL_initialized)
    {
        return NULL;
    }

    library = (LBL_Library *)lib;

    if (entry_number >= library->num_entries)
    {
        return NULL;
    }

    if (entry_name != NULL)
    {
        memmove(entry_name, library->entries[entry_number].name, 8);
    }

    if (library->entries[entry_number].uncompressed_size != 0)
    {
        size = library->entries[entry_number].uncompressed_size;
    }
    else
    {
        size = library->entries[entry_number].size;
    }

    if (library->file_handle == -1)
    {
        library->file_handle = DOS_Open_c(library->path, DOS_OPEN_MODE_READ);
        if (library->file_handle == -1)
        {
            return NULL;
        }
    }

    if (library->current_entry != entry_number)
    {
        // change: replace nonsense condition
        //if((DOS_Seek_c(library->file_handle, DOS_SEEK_SET, library->entries[entry_number].offset) & 0xff) == -1)
        if(!DOS_Seek_c(library->file_handle, DOS_SEEK_SET, library->entries[entry_number].offset))
        {
            DOS_Close_c(library->file_handle);
            library->file_handle = -1;
            return NULL;
        }
    }

    library->current_entry = entry_number + 1;

    if (entry_data != NULL)
    {
        data = entry_data;
    }
    else
    {
        data = MEM_malloc_c(size, NULL, NULL, 0, 0);
        if (data == NULL)
        {
            DOS_Close_c(library->file_handle);
            library->file_handle = -1;
            return NULL;
        }
    }

    if (library->entries[entry_number].uncompressed_size != 0)
    {
        compressed_data = MEM_malloc_c(library->entries[entry_number].size, NULL, NULL, 0, 0);
        if (compressed_data == NULL)
        {
            DOS_Close_c(library->file_handle);
            library->file_handle = -1;
            if (data != entry_data)
            {
                MEM_free_c(data);
            }
            return NULL;
        }

        if (DOS_Read_c(library->file_handle, compressed_data, library->entries[entry_number].size) != library->entries[entry_number].size)
        {
            DOS_Close_c(library->file_handle);
            library->file_handle = -1;
            if (data != entry_data)
            {
                MEM_free_c(data);
            }
            return NULL;
        }

        DEPACK_c((const uint8_t *)compressed_data, (uint8_t *)data);
        MEM_free_c(compressed_data);
    }
    else
    {
        if (DOS_Read_c(library->file_handle, data, library->entries[entry_number].size) != library->entries[entry_number].size)
        {
            DOS_Close_c(library->file_handle);
            library->file_handle = -1;
            if (data != entry_data)
            {
                MEM_free_c(data);
            }
            return NULL;
        }
    }

    if (close_file & 0xff)
    {
        DOS_Close_c(library->file_handle);
        library->file_handle = -1;
        library->current_entry = -1;
    }

    return data;
}

void LBL_CloseLib_c(void *lib)
{
    LBL_Library *library;

    if (!LBL_initialized)
    {
        return;
    }

    library = (LBL_Library *)lib;

    if (library->file_handle != -1)
    {
        DOS_Close_c(library->file_handle);
    }
    MEM_free_c(library->entries);
    MEM_free_c(library);
}

int LBL_GetEntrySize_c(void *lib, unsigned int entry_number)
{
    LBL_Library *library;

    if (!LBL_initialized)
    {
        return -1;
    }

    library = (LBL_Library *)lib;

    if (entry_number >= library->num_entries)
    {
        return -1;
    }

    if (library->entries[entry_number].uncompressed_size != 0)
    {
        return library->entries[entry_number].uncompressed_size;
    }
    else
    {
        return library->entries[entry_number].size;
    }
}

void LBL_CloseFile_c(void *lib)
{
    LBL_Library *library;

    if (!LBL_initialized)
    {
        return;
    }

    library = (LBL_Library *)lib;

    if (library->file_handle == -1)
    {
        return;
    }

    DOS_Close_c(library->file_handle);
    library->file_handle = -1;
    library->current_entry = -1;
}

int LBL_GetNOFEntries_c(void *lib)
{
    LBL_Library *library;

    library = (LBL_Library *)lib;

    return library->num_entries;
}

