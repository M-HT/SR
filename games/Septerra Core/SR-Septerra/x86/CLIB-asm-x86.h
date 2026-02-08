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

#if !defined(_CLIB_ASM_X86_H_INCLUDED_)
#define _CLIB_ASM_X86_H_INCLUDED_

#include <stdint.h>
#include "../platform.h"

#ifdef __cplusplus
extern "C" {
#endif

void * CCALL _alloca_probe_c(uint32_t size);

uint64_t CCALL _aulldiv_c(uint64_t x, uint64_t y);
int64_t CCALL _alldiv_c(int64_t x, int64_t y);
uint64_t CCALL _time64_c(uint64_t *t64);

int32_t CCALL _ftol2_sse_c(double *num);
int64_t CCALL _ftol2_c(double *num);
int64_t CCALL _ftol_c(double *num);

void CCALL _CIcos_c(double *num);
void CCALL _CIsin_c(double *num);
void CCALL _CIatan2_c(double *nums);
void CCALL _CIsqrt_c(double *num);
void CCALL _CIfmod_c(double *nums);
void CCALL _CItan_c(double *num);
void CCALL _CIpow_c(double *nums);

#ifdef __cplusplus
}
#endif

#endif

