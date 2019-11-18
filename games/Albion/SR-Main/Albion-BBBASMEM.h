/**
 *
 *  Copyright (C) 2018-2019 Roman Pauer
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

#if !defined(_ALBION_BBBASMEM_H_INCLUDED_)
#define _ALBION_BBBASMEM_H_INCLUDED_

#include <stdint.h>
#include <stdio.h>

#define BASEMEM_XMS_MEMORY 1
#define BASEMEM_DOS_MEMORY 2

#define BASEMEM_ZERO_MEMORY 0x100
#define BASEMEM_LOCK_MEMORY 0x200

#define BASEMEM_MEMORY_LOCKED 0x40000000
#define BASEMEM_MEMORY_ALLOCATED 0x80000000

#ifdef __cplusplus
extern "C" {
#endif

int BASEMEM_Init(void);
void BASEMEM_Exit(void);
unsigned int BASEMEM_GetFreeMemSize(unsigned int memory_flags);
void *BASEMEM_Alloc(unsigned int size, unsigned int memory_flags);
int BASEMEM_Free(void *mem_ptr);
int BASEMEM_LockRegion(void *mem_ptr, unsigned int length);
int BASEMEM_UnlockRegion(void *mem_ptr, unsigned int length);
void BASEMEM_FillMemByte(void *dst, unsigned int length, int c);
void BASEMEM_FillMemLong(void *dst, unsigned int length, unsigned int c);
void BASEMEM_CopyMem(const void *src, void *dst, unsigned int length);
void *BASEMEM_AlignMemptr(void *mem_ptr);
void BASEMEM_PrintReport(FILE *fp);


#ifdef __cplusplus
}
#endif

#endif /* _ALBION_BBBASMEM_H_INCLUDED_ */
