/**
 *
 *  Copyright (C) 2019-2025 Roman Pauer
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

#ifdef DEBUG_CLIB
#include <inttypes.h>
#endif
#include "CLIB-asm-x86.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


void *_alloca_probe_c(uint32_t size)
{
#ifdef DEBUG_CLIB
    eprintf("_alloca_probe: %i\n", size);
#endif

    void * volatile addr;
    unsigned int index;

    addr = alloca(size);

    for (index = 0; index < size; index += 4096)
    {
        *(uint8_t *)((size - 1) - index + (uintptr_t)addr) = 0;
    }

    // return value will be ignored
    return addr;
}


int32_t printf2_c(const char *format, va_list ap)
{
    int res;

#ifdef DEBUG_CLIB
    eprintf("printf: 0x%" PRIxPTR " (%s) - ", (uintptr_t) format, format);
#endif

    res = vprintf(format, ap);

#ifdef DEBUG_CLIB
    eprintf("%i\n", res);
#endif

    return res;
}

int32_t sprintf2_c(char *str, const char *format, va_list ap)
{
    int res;

#ifdef DEBUG_CLIB
    eprintf("sprintf: 0x%" PRIxPTR ", 0x%" PRIxPTR " (%s) - ", (uintptr_t) str, (uintptr_t) format, format);
#endif

    res = vsprintf(str, format, ap);

#ifdef DEBUG_CLIB
    eprintf("%i (%s)\n", res, str);
#endif

    return res;
}


int64_t _ftol_c(double *num)
{
#if defined(_MSC_VER) && _MSC_VER < 1800
    return (int64_t) *num;
#else
    return (int64_t) trunc(*num);
#endif
}

