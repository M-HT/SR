/**
 *
 *  Copyright (C) 2025-2026 Roman Pauer
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

#if !defined(_GAME_MEMORY_H_INCLUDED_)
#define _GAME_MEMORY_H_INCLUDED_

#ifdef __cplusplus

int x86_init_malloc(void);
void x86_deinit_malloc(void);

void *x86_malloc(unsigned int size);
void x86_free(void *ptr);
void *x86_calloc(unsigned int nmemb, unsigned int size);
void *x86_realloc(void *ptr, unsigned int size);

void *map_memory_32bit(unsigned int size, int only_address_space);
void unmap_memory_32bit(void *mem, unsigned int size);

#ifdef PTROFS_64BIT

int initialize_pointer_offset(void);

#endif

#else

#include <stdlib.h>

#define x86_malloc malloc
#define x86_free free
#define x86_calloc calloc
#define x86_realloc realloc

#endif

#endif /* _GAME_MEMORY_H_INCLUDED_ */

