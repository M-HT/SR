/**
 *
 *  Copyright (C) 2020-2026 Roman Pauer
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

#include "BBMEM.h"
#include "BBDSA.h"
#include "BBDBG.h"
#include <string.h>
#include <malloc.h>
#include "WinApi-wing32.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#undef ORIGINAL_MEM_ALLOC


#ifdef ORIGINAL_MEM_ALLOC
typedef struct _MEM_struct {
    unsigned int size;
    struct _MEM_struct *ptr;
    unsigned int type;
} MEM_struct;
#endif


static int BBMEM_initialized = 0;
static void *BBMEM_PoolPointer = NULL;
#if 0
static HDC BBMEM_hWinGDC[20];
#endif


static void BBMEM_DumpLine_c(void *hFile, const char *str)
{
    // do nothing
}

int CCALL MEM_Init_c(void)
{
#if 0
    int index, index2;
#endif

    if (!BBMEM_initialized)
    {
        BBMEM_PoolPointer = malloc(0x100000);
        if (BBMEM_PoolPointer == NULL)
        {
            DBG_Panic_c("C:\\DATA\\BBLIB\\SRC\\BASE\\BBMem.c", 568);
            return 0;
        }

#if 0
        for (index = 0; index < 20; index++)
        {
            BBMEM_hWinGDC[index] = (HDC)WinGCreateDC_c();
            if (BBMEM_hWinGDC[i] == NULL)
            {
                for (index2 = index - 1; index2 >= 0; index2--)
                {
                    DeleteDC(BBMEM_hWinGDC[index2]);
                }

                free(BBMEM_PoolPointer);
                DBG_Panic_c("C:\\DATA\\BBLIB\\SRC\\BASE\\BBMem.c", 588);
                return 0;
            }
        }
#endif
    }

    BBMEM_initialized++;
    return 1;
}

void CCALL MEM_Exit_c(void)
{
    BBMEM_initialized--;
    if (BBMEM_initialized <= 0)
    {
        BBMEM_FreeMemory_c(0);
    }
}

void * CCALL MEM_malloc_c(unsigned int size, const char *module_name, const char *object_type, unsigned int line, int type)
{
#ifdef ORIGINAL_MEM_ALLOC
    MEM_struct *mem;
#else
    void *mem;
#endif

    if (size == 0)
    {
        return NULL;
    }

#ifdef ORIGINAL_MEM_ALLOC
    mem = (MEM_struct *)malloc(size + sizeof(MEM_struct));
#else
    mem = malloc(size);
#endif
    if (mem == NULL)
    {
        BBMEM_FreeMemory_c(1);
#ifdef ORIGINAL_MEM_ALLOC
        mem = (MEM_struct *)malloc(size + sizeof(MEM_struct));
#else
        mem = malloc(size);
#endif
    }

    if (mem == NULL)
    {
        MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), "WINDOWS meldet zuwenig freien Speicher.Beenden Sie einige Anwendungen und starten Sie Battle Isle 3 erneut. Ihr momentaner Spielstand ist in Slot 9 oder 10 gesichert.", "MEMORY HANDLER", MB_OK | MB_ICONERROR);
        DBG_Panic_c("C:\\DATA\\BBLIB\\SRC\\BASE\\BBMem.c", 908);
        return NULL;
    }

#ifdef ORIGINAL_MEM_ALLOC
    mem->size = size;
    mem->type = 0;
    mem->ptr = mem;

    return (void *)(sizeof(MEM_struct) + (uintptr_t)mem);
#else
    return mem;
#endif
}

void CCALL MEM_free_c(void *mem_ptr)
{
#ifdef ORIGINAL_MEM_ALLOC
    MEM_struct *mem;
#endif

    if (mem_ptr == NULL)
    {
        return;
    }

#ifdef ORIGINAL_MEM_ALLOC
    mem = (MEM_struct *)(((uintptr_t)mem_ptr) - sizeof(MEM_struct));
    free(mem);
#else
    free(mem_ptr);
#endif
}

void CCALL MEM_Take_Snapshot_c(const char *name)
{
    // do nothing
}

void CCALL MEM_Check_Snapshot_c(void)
{
    // do nothing
}

void CCALL MEM_Dump_c(void *hFile)
{
    DWORD ticks;

    BBMEM_DumpLine_c(hFile, "---------------- LAST MEMORY DUMP BEGIN --------------------");
    BBMEM_DumpLine_c(hFile, NULL);

    ticks = GetTickCount();
    BBMEM_DumpLine_c(hFile, "Current Time in ms: ");
    BBMEM_DumpLine_c(hFile, (const char *)(ticks | 0x80000000));
    BBMEM_DumpLine_c(hFile, NULL);


    ticks = GetTickCount();
    BBMEM_DumpLine_c(hFile, NULL);
    BBMEM_DumpLine_c(hFile, "Current Time in ms: ");
    BBMEM_DumpLine_c(hFile, (const char *)(ticks | 0x80000000));
    BBMEM_DumpLine_c(hFile, NULL);

    BBMEM_DumpLine_c(hFile, "---------------- LAST MEMORY DUMP END --------------------");
    BBMEM_DumpLine_c(hFile, NULL);

    if (hFile != NULL)
    {
        FlushFileBuffers(hFile);
    }
}

void CCALL MEM_SwitchSecurity_c(unsigned int security)
{
    // do nothing
}

void * CCALL BASEMEM_Alloc_c(unsigned int size)
{
    return MEM_malloc_c(size, "BASEMEM", NULL, 0, 1);
}

int CCALL BASEMEM_Free_c(void *mem_ptr)
{
    MEM_free_c(mem_ptr);
    return 1;
}

void CCALL BASEMEM_CopyMem_c(const void *src, void *dst, unsigned int length)
{
    memmove(dst, src, length);
}

void CCALL BASEMEM_FillMemByte_c(void *dst, unsigned int length, int c)
{
    memset(dst, c, length);
}

void CCALL BBMEM_FreeMemory_c(int type)
{
#if 0
    int index;
#endif

    if (BBMEM_PoolPointer != NULL)
    {
        free(BBMEM_PoolPointer);
    }
    BBMEM_PoolPointer = NULL;

#if 0
    for (index = 0; index < 20; index++)
    {
        if (BBMEM_hWinGDC[index] != NULL)
        {
            DeleteDC(BBMEM_hWinGDC[index]);
        }
        BBMEM_hWinGDC[index] = NULL;
    }
#endif

    if (type == 1)
    {
        MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), "Your system is low on memory. Save your work and quit the applicatijon at once!", "MEMORY OBSERVER", MB_OK | MB_ICONERROR);
    }

    if (type == 2)
    {
        MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), "Your system is low on WinG - DCs. Save your work and quit the applicatijon at once!", "MEMORY OBSERVER", MB_OK | MB_ICONERROR);
    }
}

void * CCALL BBMEM_GetPoolPointer_c(void)
{
    return BBMEM_PoolPointer;
}

