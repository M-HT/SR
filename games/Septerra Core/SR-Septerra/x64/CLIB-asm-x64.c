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
#ifdef DEBUG_CLIB
#include <inttypes.h>
#endif
#include "CLIB-asm-x64.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "../platform.h"


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


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

