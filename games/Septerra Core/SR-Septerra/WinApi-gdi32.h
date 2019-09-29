/**
 *
 *  Copyright (C) 2019 Roman Pauer
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

uint32_t BitBlt_c(void *hdcDest, int32_t nXDest, int32_t nYDest, int32_t nWidth, int32_t nHeight, void *hdcSrc, int32_t nXSrc, int32_t nYSrc, uint32_t dwRop);
void *CreateCompatibleDC_c(void *hdc);
void *CreateFontIndirectA_c(void *lplf);
void *CreatePolygonRgn_c(void *lppt, int32_t cPoints, int32_t fnPolyFillMode);
void *CreateSolidBrush_c(uint32_t crColor);
uint32_t DeleteDC_c(void *hdc);
uint32_t DeleteObject_c(void *hObject);
uint32_t FillRgn_c(void *hdc, void *hrgn, void *hbr);
void *GetStockObject_c(int32_t fnObject);
int32_t OffsetRgn_c(void *hrgn, int32_t nXOffset, int32_t nYOffset);
void *SelectObject_c(void *hdc, void *hgdiobj);
int32_t SetBkMode_c(void *hdc, int32_t iBkMode);
uint32_t SetTextColor_c(void *hdc, uint32_t crColor);
uint32_t TextOutA_c(void *hdc, int32_t nXStart, int32_t nYStart, const char *lpString, int32_t cbString);

#ifdef __cplusplus
}
#endif

#endif

