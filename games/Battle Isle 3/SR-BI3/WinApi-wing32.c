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

#include "WinApi-wing32.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

uint32_t WinGBitBlt_c( void * hdcDst, int32_t xDst, int32_t yDst, int32_t width, int32_t height, void * hdcSrc, int32_t xSrc, int32_t ySrc )
{
    return BitBlt( (HDC)hdcDst, xDst, yDst, width, height, (HDC)hdcSrc, xSrc, ySrc, SRCCOPY );
}

void * WinGCreateBitmap_c( void * hdc, void *bmi, void **bits )
{
    return CreateDIBSection( (HDC)hdc, (BITMAPINFO *)bmi, DIB_RGB_COLORS, bits, NULL, 0 );
}

void * WinGCreateDC_c( void )
{
    return CreateCompatibleDC( NULL );
}

uint32_t WinGSetDIBColorTable_c( void * hdc, uint32_t start, uint32_t end, void *colors )
{
    return SetDIBColorTable( (HDC)hdc, start, end, (RGBQUAD *)colors );
}

uint32_t WinGStretchBlt_c( void * hdcDst, int32_t xDst, int32_t yDst, int32_t widthDst, int32_t heightDst, void * hdcSrc, int32_t xSrc, int32_t ySrc, int32_t widthSrc, int32_t heightSrc )
{
    INT prev_mode;
    BOOL retval;

    prev_mode = SetStretchBltMode( (HDC)hdcDst, COLORONCOLOR );
    retval = StretchBlt( (HDC)hdcDst, xDst, yDst, widthDst, heightDst, (HDC)hdcSrc, xSrc, ySrc, widthSrc, heightSrc, SRCCOPY );
    SetStretchBltMode( (HDC)hdcDst, prev_mode );

    return retval;
}

