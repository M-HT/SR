/**
 *
 *  Copyright (C) 2016-2026 Roman Pauer
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

#if !defined(_ALBION_PROC_FLOAT_H_INCLUDED_)
#define _ALBION_PROC_FLOAT_H_INCLUDED_

#include "Game_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void CCALL Game_FloatProcSin(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSin2(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSin3(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSin4(int32_t *ValuePtr);
extern void CCALL Game_FloatProcCos(int32_t *ValuePtr);
extern void CCALL Game_FloatProcCos2(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSqrt(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSqrt2(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSqrt3(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSqrt4(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSqrt5(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSqrt6(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSqrt7(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSqrt8(int32_t *ValuePtr);
extern void CCALL Game_FloatProcCosSin(int32_t *ValuePtr);
extern void CCALL Game_FloatProcAtan(int32_t *ValuePtr);
extern void CCALL Game_FloatProcSinCos(int32_t *ValuePtr);

#ifdef __cplusplus
}
#endif

#endif /* _ALBION_PROC_FLOAT_H_INCLUDED_ */
