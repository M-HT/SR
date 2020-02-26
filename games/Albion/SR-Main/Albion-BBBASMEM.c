/**
 *
 *  Copyright (C) 2018-2020 Roman Pauer
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

#include "Game_defs.h"
#include "Albion-BBBASMEM.h"
#include "Albion-BBERROR.h"
#include <stdio.h>
#include <string.h>
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif


typedef struct {
    uint16_t error_number;
    uint16_t error_parameter;
    uint16_t line_number;
    uint16_t reserved;
    const char *filename;
} BASEMEM_ErrorStruct;

typedef struct {
    unsigned int flags;
    void *mem_ptr;
    unsigned int length;
} BASEMEM_RegionStruct;

#define MAX_REGIONS 50

static const char *BASEMEM_error_str[] = {
    "Illegal error code.",
    "No free entry in table.",
    "Out of DOS memory (DPMI error code : %#x).",
    "Out of XMS memory (DPMI error code : %#x).",
    "Could not allocate LDT segment descriptor (DPMI error code : %#x).",
    "Could not set segment base (DPMI error code : %#x).",
    "Could not set segment upper limit (DPMI error code : %#x).",
    "Could not find entry.",
    "Could not free DOS memory (DPMI error code : %#x).",
    "Could not free LDT segment descriptor (DPMI error code : %#x).",
    "Could not free XMS memory (DPMI error code : %#x).",
    "Couldn't resize DOS memory (DPMI error code : %#x).",
    "Could not resize XMS memory (DPMI error code : %#x).",
    "Unsupported memory type.",
    "Could not lock memory region (DPMI error code : %#x)",
    "Could not unlock memory region (DPMI error code : %#x)",
    "Illegal parameters."
};

static int BASEMEM_initialized = 0;
static BASEMEM_RegionStruct BASEMEM_regions[MAX_REGIONS];
static int BASEMEM_page_size;


static void BASEMEM_PushError(unsigned int error_number, int error_parameter, int line_number, const char *filename_ptr);
static void BASEMEM_LocalPrintError(char *buffer, const uint8_t *data);


int BASEMEM_Init(void)
{
    int index;

    if (!BASEMEM_initialized)
    {
        for (index = 0; index < MAX_REGIONS; index++)
        {
            BASEMEM_regions[index].flags = 0;
        }

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
        {
            SYSTEM_INFO siSysInfo;
            GetSystemInfo(&siSysInfo);

            BASEMEM_page_size = siSysInfo.dwPageSize;
        }
#else
        BASEMEM_page_size = sysconf(_SC_PAGESIZE);
        if (BASEMEM_page_size == -1) BASEMEM_page_size = 4096;
#endif

        BASEMEM_initialized = 1;
    }

    return 1;
}

void BASEMEM_Exit(void)
{
    int index;

    if (BASEMEM_initialized)
    {
        for (index = 0; index < MAX_REGIONS; index++)
        {
            if (BASEMEM_regions[index].flags & BASEMEM_MEMORY_ALLOCATED)
            {
                BASEMEM_Free(BASEMEM_regions[index].mem_ptr);
            }
        }

        BASEMEM_initialized = 0;
    }
}

unsigned int BASEMEM_GetFreeMemSize(unsigned int memory_flags)
{
    switch (memory_flags & 0xff)
    {
        case BASEMEM_XMS_MEMORY:     // XMS memory
            return GAME_MAX_FREE_MEMORY - 65536;
        case BASEMEM_DOS_MEMORY:     // DOS memory
            return 6570 << 4;
        default:
            BASEMEM_PushError(13, 0, 353, "bbbasmem.c");
            return 0;
    }
}

#if 0
static unsigned int BASEMEM_GetFreeVirtualMemSize(unsigned int memory_flags)
{
    return BASEMEM_GetFreeMemSize(memory_flags);
}

static unsigned int BASEMEM_GetFreePhysicalMemSize(unsigned int memory_flags)
{
    return BASEMEM_GetFreeMemSize(memory_flags);
}
#endif

void *BASEMEM_Alloc(unsigned int size, unsigned int memory_flags)
{
    int index, free_region_index;
    void *mem_ptr;

    if (size == 0)
    {
        return NULL;
    }

    free_region_index = -1;
    for (index = 0; index < MAX_REGIONS; index++)
    {
        if (BASEMEM_regions[index].flags == 0)
        {
            free_region_index = index;
            break;
        }
    }

    if (free_region_index == -1)
    {
        BASEMEM_PushError(1, 0, 617, "bbbasmem.c");
        return NULL;
    }

    switch (memory_flags & 0xff)
    {
        case BASEMEM_XMS_MEMORY:     // XMS memory
            size = (size + 0xFFF) & ~0xFFF;
            mem_ptr = malloc(size);
            if (mem_ptr == NULL)
            {
                BASEMEM_PushError(3, 0, 685, "bbbasmem.c");
                return NULL;
            }
            break;
        case BASEMEM_DOS_MEMORY:     // DOS memory
            size = (size + 15) & ~15;
            mem_ptr = malloc(size);
            if (mem_ptr == NULL)
            {
                BASEMEM_PushError(2, 0, 648, "bbbasmem.c");
                return NULL;
            }
            break;
        default:
            BASEMEM_PushError(13, 0, 802, "bbbasmem.c");
            return NULL;
    }

    memory_flags |= BASEMEM_MEMORY_ALLOCATED;
    BASEMEM_regions[free_region_index].flags = memory_flags;
    BASEMEM_regions[free_region_index].mem_ptr = mem_ptr;
    BASEMEM_regions[free_region_index].length = size;

    if (memory_flags & BASEMEM_LOCK_MEMORY)
    {
        if (!BASEMEM_LockRegion(mem_ptr, size))
        {
            BASEMEM_Free(mem_ptr);
            return NULL;
        }

        BASEMEM_regions[free_region_index].flags |= BASEMEM_MEMORY_LOCKED;
    }

    if (memory_flags & BASEMEM_ZERO_MEMORY)
    {
        BASEMEM_FillMemByte(mem_ptr, size, 0);
    }
    else
    {
        BASEMEM_FillMemByte(mem_ptr, size, 0xCC);
    }

    return mem_ptr;
}

int BASEMEM_Free(void *mem_ptr)
{
    int index, found_region_index;

    found_region_index = -1;
    for (index = 0; index < MAX_REGIONS; index++)
    {
        if (BASEMEM_regions[index].flags & BASEMEM_MEMORY_ALLOCATED)
        {
            if (BASEMEM_regions[index].mem_ptr == mem_ptr)
            {
                found_region_index = index;
                break;
            }
        }
    }

    if (found_region_index == -1)
    {
        BASEMEM_PushError(7, 0, 914, "bbbasmem.c");
        return 0;
    }

    BASEMEM_FillMemByte(BASEMEM_regions[found_region_index].mem_ptr, BASEMEM_regions[found_region_index].length, 0xCC);

    if (BASEMEM_regions[found_region_index].flags & BASEMEM_MEMORY_LOCKED)
    {
        BASEMEM_UnlockRegion(BASEMEM_regions[found_region_index].mem_ptr, BASEMEM_regions[found_region_index].length);
        BASEMEM_regions[found_region_index].flags &= ~BASEMEM_MEMORY_LOCKED;
    }

    switch (BASEMEM_regions[found_region_index].flags & 0xff)
    {
        case BASEMEM_XMS_MEMORY:     // XMS memory
        case BASEMEM_DOS_MEMORY:     // DOS memory
            free(BASEMEM_regions[found_region_index].mem_ptr);
            break;
        default:
            BASEMEM_PushError(13, 0, 1009, "bbbasmem.c");
            break;
    }

    BASEMEM_regions[found_region_index].flags = 0;
    return 1;
}

#if 0
static void *BASEMEM_Realloc(void *mem_ptr, unsigned int size)
{
    int index, found_region_index;
    unsigned int old_size;
    void *new_mem_ptr;

    found_region_index = -1;
    for (index = 0; index < MAX_REGIONS; index++)
    {
        if (BASEMEM_regions[index].flags & BASEMEM_MEMORY_ALLOCATED)
        {
            if (BASEMEM_regions[index].mem_ptr == mem_ptr)
            {
                found_region_index = index;
                break;
            }
        }
    }

    if (found_region_index == -1)
    {
        BASEMEM_PushError(7, 0, 1078, "bbbasmem.c");
        return NULL;
    }

    old_size = BASEMEM_regions[found_region_index].length;
    if (old_size > size)
    {
        BASEMEM_FillMemByte((void *) (((uintptr_t)BASEMEM_regions[found_region_index].mem_ptr) + size), old_size - size, 0xCC);
    }

    if (BASEMEM_regions[found_region_index].flags & BASEMEM_MEMORY_LOCKED)
    {
        BASEMEM_UnlockRegion(BASEMEM_regions[found_region_index].mem_ptr, BASEMEM_regions[found_region_index].length);
        BASEMEM_regions[found_region_index].flags &= ~BASEMEM_MEMORY_LOCKED;
    }

    switch (BASEMEM_regions[found_region_index].flags & 0xff)
    {
        case BASEMEM_XMS_MEMORY:     // XMS memory
            size = (size + 0xFFF) & ~0xFFF;
            new_mem_ptr = realloc(BASEMEM_regions[found_region_index].mem_ptr, size);
            if (new_mem_ptr == NULL)
            {
                BASEMEM_PushError(12, 0, 1175, "bbbasmem.c");
                return NULL;
            }
            break;
        case BASEMEM_DOS_MEMORY:     // DOS memory
            size = (size + 15) & ~15;
            new_mem_ptr = realloc(BASEMEM_regions[found_region_index].mem_ptr, size);
            if (new_mem_ptr == NULL)
            {
                BASEMEM_PushError(11, 0, 1135, "bbbasmem.c");
                return NULL;
            }
            break;
        default:
            BASEMEM_PushError(13, 0, 1254, "bbbasmem.c");
            return NULL;
    }

    if (BASEMEM_regions[found_region_index].flags & BASEMEM_LOCK_MEMORY)
    {
        if (BASEMEM_LockRegion(new_mem_ptr, size))
        {
            BASEMEM_regions[found_region_index].flags |= BASEMEM_MEMORY_LOCKED;
        }
        else
        {
            return NULL;
        }
    }

    if (old_size < size)
    {
        if (BASEMEM_regions[found_region_index].flags & BASEMEM_ZERO_MEMORY)
        {
            BASEMEM_FillMemByte((void *) (((uintptr_t)new_mem_ptr) + old_size), size - old_size, 0);
        }
        else
        {
            BASEMEM_FillMemByte((void *) (((uintptr_t)new_mem_ptr) + old_size), size - old_size, 0xCC);
        }
    }

    BASEMEM_regions[found_region_index].mem_ptr = new_mem_ptr;
    BASEMEM_regions[found_region_index].length = size;

    return new_mem_ptr;
}
#endif

int BASEMEM_LockRegion(void *mem_ptr, unsigned int length)
{
    return 1;
}

int BASEMEM_UnlockRegion(void *mem_ptr, unsigned int length)
{
    return 1;
}

void BASEMEM_FillMemByte(void *dst, unsigned int length, int c)
{
    if (dst != NULL)
    {
        memset(dst, c, length);
    }
    else
    {
        BASEMEM_PushError(16, 0, 1482, "bbbasmem.c");
    }
}

void BASEMEM_FillMemLong(void *dst, unsigned int length, unsigned int c)
{
    if (dst != NULL)
    {
        uint32_t *dst4;
        unsigned int index;

        dst4 = (uint32_t *) dst;
        length >>= 2;

        for (index = 0; index < length; index++)
        {
            *dst4 = c;
            dst4++;
        }
    }
    else
    {
        BASEMEM_PushError(16, 0, 1482, "bbbasmem.c");
    }
}

void BASEMEM_CopyMem(const void *src, void *dst, unsigned int length)
{
    if ((src != NULL) && (dst != NULL))
    {
        memmove(dst, src, length);
    }
    else
    {
        BASEMEM_PushError(16, 0, 1576, "bbbasmem.c");
    }
}

void *BASEMEM_AlignMemptr(void *mem_ptr)
{
    return (void *) ( (((uintptr_t)mem_ptr) + 7) & ~((uintptr_t)7) );
}

static void BASEMEM_PushError(unsigned int error_number, int error_parameter, int line_number, const char *filename_ptr)
{
    BASEMEM_ErrorStruct data;

    data.error_number = error_number;
    data.error_parameter = error_parameter;
    data.line_number = line_number;
    data.filename = filename_ptr;

    ERROR_PushError(BASEMEM_LocalPrintError, "BASEMEM", sizeof(data), (const uint8_t *) &data);
}

static void BASEMEM_LocalPrintError(char *buffer, const uint8_t *data)
{
    unsigned int error_number, error_parameter;
    char tempbuf[100];

#define DATA (((BASEMEM_ErrorStruct *)data))
    error_number = DATA->error_number;
    if (error_number > 16)
    {
        error_number = 0;
    }

    error_parameter = DATA->error_parameter;
    if (!(error_parameter & 0x8000))
    {
        error_parameter = 0;
    }

    snprintf(tempbuf, 100, BASEMEM_error_str[error_number], error_parameter);

    if ((DATA->line_number != 0) && (DATA->filename != NULL))
    {
        snprintf(buffer, 200, "%s\n(line %u of file %s)", tempbuf, DATA->line_number, DATA->filename);
    }
    else
    {
        strncpy(buffer, tempbuf, 199);
    }
#undef DATA
}

void BASEMEM_PrintReport(FILE *fp)
{
    if (fp == NULL)
    {
        fp = stdout;
    }

    fprintf(fp, "\nBASEMEM report\n\n");
    fprintf(fp, "Page size    : %u\n", BASEMEM_page_size);
    fprintf(fp, "Largest available free block in bytes  : %lu\n", (unsigned long int)GAME_MAX_FREE_MEMORY);
    fprintf(fp, "\n");
}

