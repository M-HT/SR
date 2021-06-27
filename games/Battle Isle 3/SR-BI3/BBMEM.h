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

#if !defined(_BBMEM_H_INCLUDED_)
#define _BBMEM_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int MEM_Init_c(void);
void MEM_Exit_c(void);
void *MEM_malloc_c(unsigned int size, const char *module_name, const char *object_type, unsigned int line, int type);
void MEM_free_c(void *mem_ptr);
void MEM_Take_Snapshot_c(const char *name);
void MEM_Check_Snapshot_c(void);
void MEM_Dump_c(void *hFile);
void MEM_SwitchSecurity_c(unsigned int security);
void *BASEMEM_Alloc_c(unsigned int size);
int BASEMEM_Free_c(void *mem_ptr);
void BASEMEM_CopyMem_c(const void *src, void *dst, unsigned int length);
void BASEMEM_FillMemByte_c(void *dst, unsigned int length, int c);
void BBMEM_FreeMemory_c(int type);
void *BBMEM_GetPoolPointer_c(void);

#ifdef __cplusplus
}
#endif

#endif /* _BBMEM_H_INCLUDED_ */
