/**
 *
 *  Copyright (C) 2019-2026 Roman Pauer
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
#include <inttypes.h>
#include "CLIB-asm-x86.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../platform.h"

#if defined(__SSE2__)
    #include <emmintrin.h>
#endif


typedef union {
    double d;
    struct {
        uint32_t low;
        uint32_t high;
    };
} double_int;


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


void * CCALL _alloca_probe_c(uint32_t size)
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


uint64_t CCALL _aulldiv_c(uint64_t x, uint64_t y)
{
#ifdef DEBUG_CLIB
    eprintf("_aulldiv: 0x%x%x, 0x%x%x\n", (uint32_t) (x >> 32), (uint32_t) x, (uint32_t) (y >> 32), (uint32_t) y);
#endif

    return x / y;
}

int64_t CCALL _alldiv_c(int64_t x, int64_t y)
{
#ifdef DEBUG_CLIB
    eprintf("_alldiv: 0x%x%x, 0x%x%x\n", (uint32_t) (x >> 32), (uint32_t) x, (uint32_t) (y >> 32), (uint32_t) y);
#endif

    return x / y;
}

uint64_t CCALL _time64_c(uint64_t *t64)
{
    time_t t;

#ifdef DEBUG_CLIB
    eprintf("_time64: 0x%" PRIxPTR "\n", (uintptr_t) t64);
#endif

    t = time(NULL);
    if (t64 != NULL)
    {
        *t64 = t;
    }

    return t;
}


int32_t CCALL _ftol2_sse_c(double *num)
{
#if defined(__SSE2__)
    return _mm_cvttsd_si32(_mm_load1_pd(num));
#else
    //return (int32_t) trunc(*num);

    const static double doublemagic = 6755399441055744.0; // 2^52 * 1.5

    double_int result;
    double num2;

    if (*num < 0)
    {
        if (*num <= -2147483648.0) return 0x80000000;

        result.d = *num + doublemagic;  // fast conversion to int,
        num2 = (double)(int32_t)result.low; // result.low contains the result (rounded up or down)

        if (num2 < *num) // compare result with original value and if the result was rounded toward negative infinity, then increase result (truncate toward 0)
        {
            result.low++;
        }

        return (int32_t)result.low;
    }
    else
    {
        if (*num >= 2147483648.0) return 0x80000000;

        result.d = *num + doublemagic;  // fast conversion to int,
        if (0 > (int32_t)result.low) return 0x7fffffff;
        num2 = (double)(int32_t)result.low; // result.low contains the result (rounded up or down)

        if (num2 > *num) // compare result with original value and if the result was rounded toward positive infinity, then decrease result (truncate toward 0)
        {
            result.low--;
        }

        return (int32_t)result.low;
    }
#endif
}

int64_t CCALL _ftol2_c(double *num)
{
#if defined(_MSC_VER) && _MSC_VER < 1800
    return (int64_t) *num;
#else
    double dval;

    dval = trunc(*num);
    return ((dval < 9223372036854775808.0) && (dval > -9223372036854775808.0))?((int64_t) dval):INT64_C(0x8000000000000000);
#endif
}

int64_t CCALL _ftol_c(double *num)
{
#if defined(_MSC_VER) && _MSC_VER < 1800
    return (int64_t) *num;
#else
    double dval;

    dval = trunc(*num);
    return ((dval < 9223372036854775808.0) && (dval > -9223372036854775808.0))?((int64_t) dval):INT64_C(0x8000000000000000);
#endif
}


void CCALL _CIcos_c(double *num)
{
    *num = cos(*num);
}

void CCALL _CIsin_c(double *num)
{
    *num = sin(*num);
}

void CCALL _CIatan2_c(double *nums)
{
    nums[0] = atan2(nums[1], nums[0]);
}

void CCALL _CIsqrt_c(double *num)
{
    *num = sqrt(*num);
}

void CCALL _CIfmod_c(double *nums)
{
    nums[0] = fmod(nums[1], nums[0]);
}

void CCALL _CItan_c(double *num)
{
    *num = tan(*num);
}

void CCALL _CIpow_c(double *nums)
{
    nums[0] = pow(nums[1], nums[0]);
}

