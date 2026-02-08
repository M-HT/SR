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

#if defined(DEBUG_GDI32)
#include <inttypes.h>
#endif
#include "WinApi-gdi32.h"
#include "WinApi.h"
#include <stdio.h>
#include <stdlib.h>


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


uint32_t CCALL BitBlt_c(void *hdcDest, int32_t nXDest, int32_t nYDest, int32_t nWidth, int32_t nHeight, void *hdcSrc, int32_t nXSrc, int32_t nYSrc, uint32_t dwRop)
{
    eprintf("Unimplemented: %s\n", "BitBlt");
    exit(1);
//    return BitBlt((HDC)hdcDest, nXDest, nYDest, nWidth, nHeight, (HDC)hdcSrc, nXSrc, nYSrc, dwRop);
}

void * CCALL CreateCompatibleDC_c(void *hdc)
{
    eprintf("Unimplemented: %s\n", "CreateCompatibleDC");
    exit(1);
//    return CreateCompatibleDC((HDC)hdc);
}

void * CCALL CreateFontIndirectA_c(void *lplf)
{
    eprintf("Unimplemented: %s\n", "CreateFontIndirectA");
    exit(1);
//    return CreateFontIndirectA((const LOGFONTA *)lplf);
}

void * CCALL CreatePolygonRgn_c(void *lppt, int32_t cPoints, int32_t fnPolyFillMode)
{
#if defined(DEBUG_GDI32)
    eprintf("CreatePolygonRgn: 0x%" PRIxPTR ", %i, %i\n", (uintptr_t)lppt, cPoints, fnPolyFillMode);
#endif

    // Septerra Core doesn't seem to use the result
    return NULL;
}

void * CCALL CreateSolidBrush_c(uint32_t crColor)
{
    eprintf("Unimplemented: %s\n", "CreateSolidBrush");
    exit(1);
//    return CreateSolidBrush(crColor);
}

uint32_t CCALL DeleteDC_c(void *hdc)
{
    eprintf("Unimplemented: %s\n", "DeleteDC");
    exit(1);
//    return DeleteDC((HDC)hdc);
}

uint32_t CCALL DeleteObject_c(void *hObject)
{
#if defined(DEBUG_GDI32)
    eprintf("DeleteObject: 0x%" PRIxPTR "\n", (uintptr_t)hObject);
#endif

    if (hObject == NULL) return 0;

    eprintf("Unimplemented: %s\n", "DeleteObject");
    exit(1);
//    return DeleteObject((HGDIOBJ)hObject);
}

uint32_t CCALL FillRgn_c(void *hdc, void *hrgn, void *hbr)
{
    eprintf("Unimplemented: %s\n", "FillRgn");
    exit(1);
//    return FillRgn((HDC)hdc, (HRGN)hrgn, (HBRUSH)hbr);
}

void * CCALL GetStockObject_c(int32_t fnObject)
{
#if defined(DEBUG_GDI32)
    eprintf("GetStockObject: %i\n", fnObject);
#endif

    // Septerra Core only uses stock object for RegisterClassA
    Winapi_SetLastError(ERROR_INVALID_PARAMETER);
    return NULL;
}

int32_t CCALL OffsetRgn_c(void *hrgn, int32_t nXOffset, int32_t nYOffset)
{
    eprintf("Unimplemented: %s\n", "OffsetRgn");
    exit(1);
//    return OffsetRgn((HRGN)hrgn, nXOffset, nYOffset);
}

void * CCALL SelectObject_c(void *hdc, void *hgdiobj)
{
    eprintf("Unimplemented: %s\n", "SelectObject");
    exit(1);
//    return SelectObject((HDC)hdc, (HGDIOBJ)hgdiobj);
}

int32_t CCALL SetBkMode_c(void *hdc, int32_t iBkMode)
{
    eprintf("Unimplemented: %s\n", "SetBkMode");
    exit(1);
//    return SetBkMode((HDC)hdc, iBkMode);
}

uint32_t CCALL SetTextColor_c(void *hdc, uint32_t crColor)
{
    eprintf("Unimplemented: %s\n", "SetTextColor");
    exit(1);
//    return SetTextColor((HDC)hdc, crColor);
}

uint32_t CCALL TextOutA_c(void *hdc, int32_t nXStart, int32_t nYStart, const char *lpString, int32_t cbString)
{
    eprintf("Unimplemented: %s\n", "TextOutA");
    exit(1);
//    return TextOutA((HDC)hdc, nXStart, nYStart, lpString, cbString);
}

