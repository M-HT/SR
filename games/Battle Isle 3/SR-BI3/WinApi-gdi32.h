/**
 *
 *  Copyright (C) 2021 Roman Pauer
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

#if !defined(_WINAPI_GDI32_H_INCLUDED_)
#define _WINAPI_GDI32_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void * CreateFontIndirectA_c(const void * lplf);
uint32_t DeleteObject_c(void * ho);
uint32_t GdiFlush_c(void);
int32_t GetDeviceCaps_c(void * hdc, int32_t index);
uint32_t GetTextExtentPointA_c(void * hdc, const char * lpString, int32_t c, void * lpsz);
void * SelectObject_c(void * hdc, void * h);
int32_t SetBkMode_c(void * hdc, int32_t mode);
uint32_t TextOutA_c(void * hdc, int32_t x, int32_t y, const char * lpString, int32_t c);

#ifdef __cplusplus
}
#endif

#endif

