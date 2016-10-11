/**
 *
 *  Copyright (C) 2016 Roman Pauer
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

#include <stdint.h>

#pragma pack(4)

typedef union _pixel_format_disp_ {
    struct {
        uint32_t b:5;
        uint32_t g:6;
        uint32_t r:5;
        uint32_t a:16;
    } s;
    uint32_t pix;
    uint16_t pix2;
} pixel_format_disp;

#pragma pack()

#include "palettecommon.h"

void Set_Palette_Value(uint32_t index, uint32_t r, uint32_t g, uint32_t b)
{
    Game_Palette[index].s.r = r >> 3;
    Game_Palette[index].s.g = g >> 2;
    Game_Palette[index].s.b = b >> 3;

    Set_Palette_Value2(index, r, g, b);
}

