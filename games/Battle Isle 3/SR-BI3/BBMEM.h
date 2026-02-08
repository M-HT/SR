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

#if !defined(_BBMEM_H_INCLUDED_)
#define _BBMEM_H_INCLUDED_

#include <stdint.h>
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

int CCALL MEM_Init_c(void);
void CCALL MEM_Exit_c(void);
void * CCALL MEM_malloc_c(unsigned int size, const char *module_name, const char *object_type, unsigned int line, int type);
void CCALL MEM_free_c(void *mem_ptr);
void CCALL MEM_Take_Snapshot_c(const char *name);
void CCALL MEM_Check_Snapshot_c(void);
void CCALL MEM_Dump_c(void *hFile);
void CCALL MEM_SwitchSecurity_c(unsigned int security);
void * CCALL BASEMEM_Alloc_c(unsigned int size);
int CCALL BASEMEM_Free_c(void *mem_ptr);
void CCALL BASEMEM_CopyMem_c(const void *src, void *dst, unsigned int length);
void CCALL BASEMEM_FillMemByte_c(void *dst, unsigned int length, int c);
void CCALL BBMEM_FreeMemory_c(int type);
void * CCALL BBMEM_GetPoolPointer_c(void);

#ifdef __cplusplus
}
#endif

#endif /* _BBMEM_H_INCLUDED_ */
