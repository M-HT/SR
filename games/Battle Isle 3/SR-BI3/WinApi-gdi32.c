/**
 *
 *  Copyright (C) 2021-2026 Roman Pauer
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

#include "WinApi-gdi32.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void * CCALL CreateFontIndirectA_c(const void * lplf)
{
    return CreateFontIndirectA((CONST LOGFONTA *)lplf);
}

uint32_t CCALL DeleteObject_c(void * ho)
{
    return DeleteObject((HGDIOBJ)ho);
}

uint32_t CCALL GdiFlush_c(void)
{
    return GdiFlush();
}

int32_t CCALL GetDeviceCaps_c(void * hdc, int32_t index)
{
    return GetDeviceCaps((HDC)hdc, index);
}

uint32_t CCALL GetTextExtentPointA_c(void * hdc, const char * lpString, int32_t c, void * lpsz)
{
    return GetTextExtentPointA((HDC)hdc, lpString, c, (LPSIZE)lpsz);
}

void * CCALL SelectObject_c(void * hdc, void * h)
{
    return SelectObject((HDC)hdc, (HGDIOBJ)h);
}

int32_t CCALL SetBkMode_c(void * hdc, int32_t mode)
{
    return SetBkMode((HDC)hdc, mode);
}

uint32_t CCALL TextOutA_c(void * hdc, int32_t x, int32_t y, const char * lpString, int32_t c)
{
    return TextOutA((HDC)hdc, x, y, lpString, c);
}

