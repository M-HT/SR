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

#include "../Game_defs.h"
#include "../Game_vars.h"
#include "palette16bgra.h"
#include "overlay.h"

static int DisplayMode;

int Change_Display_Mode(int direction);

static uint16_t interpolation_matrix2[256*256];
#define IM2_LOOKUP(a, b) ( (uint32_t) interpolation_matrix2[(((uint32_t) (a)) << 8) + ((uint32_t) (b))] )

static void Set_Palette_Value2(uint32_t index, uint32_t r, uint32_t g, uint32_t b)
{
    uint16_t *val1, *val2;
    pixel_format_disp pixel;

#define MAXDIFF 128
#define INTERPOLATE(a, b) (((a) >= (b)) ? ( ((a) - (b) >= MAXDIFF) ? ((3 * (a) + (b)) / 4) : (((a) + (b)) / 2) ) : ( ((b) - (a) >= MAXDIFF) ? ((3 * (b) + (a)) / 4) : (((a) + (b)) / 2) ) )

    val1 = &(interpolation_matrix2[index * 256]);
    val2 = &(interpolation_matrix2[index]);

    pixel.pix = 0;

    for (index = 0; index < 256; index++)
    {
        pixel.s.r = INTERPOLATE(r, (uint32_t) Game_Palette_Or[index].s.r) >> 3;
        pixel.s.g = INTERPOLATE(g, (uint32_t) Game_Palette_Or[index].s.g) >> 2;
        pixel.s.b = INTERPOLATE(b, (uint32_t) Game_Palette_Or[index].s.b) >> 3;

        *val1 = *val2 = pixel.pix;

        val1++;
        val2 += 256;
    }

#undef INTERPOLATE
#undef MAXDIFF
}

#define BYTE0(u32) ( (u32) & 0xff )
#define BYTE1(u32) ( ((u32) & 0xff00) >> 8 )
#define BYTE2(u32) ( ((u32) & 0xff0000) >> 16 )
#define BYTE3(u32) ( (u32) >> 24 )

/*
static void Flip_360x240x8_to_800x480x16(uint8_t *src, uint16_t *dst)
{
    register int y, x;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    for (y = 240; y !=0; y--)
    {
        for (x = 360; x != 0; x-=36)
        {
            register uint32_t src1, src2;
            //
            // 2232223 2222 3222322 2232223 2222 3222322
            //

            // 1st quad
            src1 = SRC[0];
            DST[0] = DST[400] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[1] = DST[401] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[2] = DST[402] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[1];
            DST[3] = DST[403] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            // 2nd quad
            DST[4] = DST[404] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[5] = DST[405] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[6] = DST[406] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            DST[7] = DST[407] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[2];

            // 3rd quad
            DST[8] = DST[408] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[9] = DST[409] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[10] = DST[410] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[11] = DST[411] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[3];

            // 4th quad
            DST[12] = DST[412] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[13] = DST[413] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[14] = DST[414] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[15] = DST[415] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);

            // 5th quad
            DST[16] = DST[416] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            src1 = SRC[4];
            DST[17] = DST[417] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[18] = DST[418] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[19] = DST[419] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);

            // 6th quad
            DST[20] = DST[420] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[5];
            DST[21] = DST[421] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[22] = DST[422] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[23] = DST[423] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);

            // 7th quad
            DST[24] = DST[424] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[6];
            DST[25] = DST[425] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[26] = DST[426] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[27] = DST[427] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);

            // 8th quad
            DST[28] = DST[428] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[29] = DST[429] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[7];
            DST[30] = DST[430] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[31] = DST[431] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);

            // 9th quad
            DST[32] = DST[432] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[33] = DST[433] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[8];
            DST[34] = DST[434] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[35] = DST[435] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);

            // 10th quad
            DST[36] = DST[436] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[37] = DST[437] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[38] = DST[438] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            DST[39] = DST[439] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            dst+=80;
            src+=36;
        }
        dst += 800;
    }
#undef DST
#undef SRC
}
*/

/*
..    ...
.x => ..x
..    ...

.x    . x
.x => ..x
..    ...

x.    x .
.x => .ox
..    ...

xx    xxx
.x => ..x
..    ...

..    ...
.x => ..x
.x    . x

.x    . x
.x => ..x
.x    . x

x.    x .
.x => .ox
.x    . x

xx    xxx
.x => ..x
.x    . x

..    ...
.x => .ox
x.    x .

.x    . x
.x => .ox
x.    x .

x.    x .
.x => .ox
x.    x .

xx    xxx
.x => .ox
x.    x .

..    ...
.x => ..x
xx    xxx

.x    . x
.x => ..x
xx    xxx

x.    x .
.x => .ox
xx    xxx

xx    xxx
.x => ..x
xx    xxx
*/
static uint32_t Get_pixel_value_interpolated3(uint8_t *src, int color_index)
{
    register uint32_t pattern, color1, color2;

    color1 = Game_Palette_Or[src[0]].c[color_index];
    color2 = Game_Palette_Or[src[1]].c[color_index];

    pattern = ((color1 & 0x80) >> 3) | ((color2 & 0x80) >> 6);
    pattern |= (Game_Palette_Or[src[-360]].c[color_index] & 0x80) >> 2;
    pattern |= (Game_Palette_Or[src[-359]].c[color_index] & 0x80) >> 5;
    pattern |= (Game_Palette_Or[src[360]].c[color_index] & 0x80) >> 4;
    pattern |= (Game_Palette_Or[src[361]].c[color_index] & 0x80) >> 7;

    switch (pattern)
    {
        case 0b000010:
        case 0b000110:
        case 0b100110:
        case 0b000011:
        case 0b000111:
        case 0b100111:
        case 0b001011:
        case 0b001111:
        case 0b101111:
            return color1;
        case 0b010000:
        case 0b110000:
        case 0b110100:
        case 0b011000:
        case 0b111000:
        case 0b111100:
        case 0b011001:
        case 0b111001:
        case 0b111101:
            return color2;
        default:
            return (color1 + color2) / 2;
    }
}

static void Flip_360x240x8_to_800x480x16_interpolated3_helper(uint8_t *src, uint16_t *dst, int height, int borders)
{
    int x;
    pixel_format_disp pixel;

#define INTERPOLATE_COLOR(color, color_index, shift, a, b, index) \
    { \
        if (((Game_Palette_Or[(a)].s.color & 0x80) == (Game_Palette_Or[(b)].s.color & 0x80)) || \
            ((Game_Palette_Or[(a)].s.color & 0x80) != ((Game_Palette_Or[(a)].s.color - Game_Palette_Or[(b)].s.color) & 0x80)) \
           ) \
        { \
            pixel.s.color = ((uint32_t)Game_Palette_Or[(a)].s.color + (uint32_t)Game_Palette_Or[(b)].s.color) >> (1 + (shift)); \
        } \
        else \
        { \
            pixel.s.color = Get_pixel_value_interpolated3(&(src[index]), color_index) >> (shift); \
        } \
    }
#define INTERPOLATE(_a, _b, index) \
    { \
        INTERPOLATE_COLOR(r, 0, 3, _a, _b, index) \
        INTERPOLATE_COLOR(g, 1, 2, _a, _b, index) \
        INTERPOLATE_COLOR(b, 2, 3, _a, _b, index) \
    }
#define INTERPOLATE1(a1, b1) \
    { \
        pixel.s.r = ((uint32_t)Game_Palette_Or[(a1)].s.r + (uint32_t)Game_Palette_Or[(b1)].s.r) >> 4; \
        pixel.s.g = ((uint32_t)Game_Palette_Or[(a1)].s.g + (uint32_t)Game_Palette_Or[(b1)].s.g) >> 3; \
        pixel.s.b = ((uint32_t)Game_Palette_Or[(a1)].s.b + (uint32_t)Game_Palette_Or[(b1)].s.b) >> 4; \
    }
#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    pixel.pix = 0;

    if (borders & 1)
    {
        // first line - use linear interpolation
        for (x = 360; x != 0; x-=9)
        {
            DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
            DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
            INTERPOLATE1(src[1], src[2])
            DST[2] = DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
            DST[3] = DST[403] = Game_Palette[src[2]].pix | (Game_Palette[src[3]].pix << 16);
            DST[4] = DST[404] = Game_Palette[src[3]].pix | (Game_Palette[src[4]].pix << 16);
            DST[5] = DST[405] = Game_Palette[src[4]].pix | (Game_Palette[src[5]].pix << 16);
            DST[6] = DST[406] = Game_Palette[src[5]].pix | (Game_Palette[src[6]].pix << 16);
            INTERPOLATE1(src[6], src[7])
            DST[7] = DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);
            DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
            DST[9] = DST[409] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);

            dst+=20;
            src+=9;
        }
        dst += 800;
        height--;
    }

    if (borders & 2)
    {
        height--;
    }

    for (; height !=0; height--)
    {
        for (x = 360; x != 0; x-=36)
        {
            register uint32_t src1, src2;
            /*
            455444554 455444554 455444554 455444554
            */

            // 1st quad
            src1 = SRC[0];
            DST[0] = DST[400] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[1] = DST[401] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            INTERPOLATE(BYTE1(src1), BYTE2(src1), 1)
            DST[2] = DST[402] = pixel.pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[1];
            DST[3] = DST[403] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            // 2nd quad
            DST[4] = DST[404] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[5] = DST[405] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[6] = DST[406] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            INTERPOLATE(BYTE2(src2), BYTE3(src2), 6)
            DST[7] = DST[407] = Game_Palette[BYTE2(src2)].pix | (pixel.pix << 16);
            src1 = SRC[2];

            // 3rd quad
            DST[8] = DST[408] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[9] = DST[409] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[10] = DST[410] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[11] = DST[411] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);

            // 4th quad
            INTERPOLATE(BYTE2(src1), BYTE3(src1), 10)
            DST[12] = DST[412] = pixel.pix | (Game_Palette[BYTE3(src1)].pix << 16);
            src2 = SRC[3];
            DST[13] = DST[413] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[14] = DST[414] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[15] = DST[415] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);

            // 5th quad
            DST[16] = DST[416] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            src1 = SRC[4];
            INTERPOLATE(BYTE3(src2), BYTE0(src1), 15)
            DST[17] = DST[417] = Game_Palette[BYTE3(src2)].pix | (pixel.pix << 16);
            DST[18] = DST[418] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[19] = DST[419] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);

            // 6th quad
            DST[20] = DST[420] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[5];
            DST[21] = DST[421] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            INTERPOLATE(BYTE3(src1), BYTE0(src2), 19)
            DST[22] = DST[422] = pixel.pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[23] = DST[423] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);

            // 7th quad
            DST[24] = DST[424] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[6];
            DST[25] = DST[425] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[26] = DST[426] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            INTERPOLATE(BYTE0(src1), BYTE1(src1), 24)
            DST[27] = DST[427] = Game_Palette[BYTE0(src1)].pix | (pixel.pix << 16);

            // 8th quad
            DST[28] = DST[428] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[29] = DST[429] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[7];
            DST[30] = DST[430] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[31] = DST[431] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);

            // 9th quad
            INTERPOLATE(BYTE0(src2), BYTE1(src2), 28)
            DST[32] = DST[432] = pixel.pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[33] = DST[433] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[8];
            DST[34] = DST[434] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[35] = DST[435] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);

            // 10th quad
            DST[36] = DST[436] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            INTERPOLATE(BYTE1(src1), BYTE2(src1), 33)
            DST[37] = DST[437] = Game_Palette[BYTE1(src1)].pix | (pixel.pix << 16);
            DST[38] = DST[438] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            DST[39] = DST[439] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            dst+=80;
            src+=36;
        }
        dst += 800;
    }

    if (borders & 2)
    {
        // last line - use linear interpolation
        for (x = 360; x != 0; x-=9)
        {
            DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
            DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
            INTERPOLATE1(src[1], src[2])
            DST[2] = DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
            DST[3] = DST[403] = Game_Palette[src[2]].pix | (Game_Palette[src[3]].pix << 16);
            DST[4] = DST[404] = Game_Palette[src[3]].pix | (Game_Palette[src[4]].pix << 16);
            DST[5] = DST[405] = Game_Palette[src[4]].pix | (Game_Palette[src[5]].pix << 16);
            DST[6] = DST[406] = Game_Palette[src[5]].pix | (Game_Palette[src[6]].pix << 16);
            INTERPOLATE1(src[6], src[7])
            DST[7] = DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);
            DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
            DST[9] = DST[409] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);

            dst+=20;
            src+=9;
        }
        //dst += 800;
    }
}

static void Flip_360x240x8_to_800x480x16_interpolated3(uint8_t *src, uint16_t *dst)
{
    int x, y, DrawOverlay, ViewportX_9, ViewportWidth_9, ViewportX2_9;

    DrawOverlay = Get_DrawOverlay(src, 1);

    if (DrawOverlay)
    {
        uint8_t *zalsrc, *src2, *tmpsrc2, *orig;
        uint16_t *zaldst, *tmpdst;
        uint32_t srcval1, srcval2;
        pixel_format_disp pixel;

        // display part above the viewport
        if (Game_OverlayDisplay.ViewportY != 0)
        {
            Flip_360x240x8_to_800x480x16_interpolated3_helper(src, dst, Game_OverlayDisplay.ViewportY, 1);
            src += 360 * Game_OverlayDisplay.ViewportY;
            dst += 800 * 2 * Game_OverlayDisplay.ViewportY;
        }

        zalsrc = src;
        zaldst = dst;

        pixel.pix = 0;

        // display part left of the viewport
        ViewportX_9 = Game_OverlayDisplay.ViewportX - (Game_OverlayDisplay.ViewportX % 9);
        if (ViewportX_9 != 0)
        {
            y = Game_OverlayDisplay.ViewportHeight;
            // first line
            if (Game_OverlayDisplay.ViewportY == 0)
            {
                y--;

                tmpdst = dst;
                for (x = ViewportX_9; x != 0; x-=9)
                {
                    /*
                    455444554
                    */

                    // 1st quad
                    DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
                    DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
                    INTERPOLATE1(src[1], src[2])
                    DST[2] = DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                    DST[3] = DST[403] = Game_Palette[src[2]].pix | (Game_Palette[src[3]].pix << 16);

                    // 2nd quad
                    DST[4] = DST[404] = Game_Palette[src[3]].pix | (Game_Palette[src[4]].pix << 16);
                    DST[5] = DST[405] = Game_Palette[src[4]].pix | (Game_Palette[src[5]].pix << 16);
                    DST[6] = DST[406] = Game_Palette[src[5]].pix | (Game_Palette[src[6]].pix << 16);
                    INTERPOLATE1(src[6], src[7])
                    DST[7] = DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);

                    // 3rd quad
                    DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
                    DST[9] = DST[409] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);

                    src+=9;
                    dst+=20;
                }
                src+=(360-ViewportX_9);
                dst = tmpdst + 2*800;
            }

            for (; y != 0; y--)
            {
                tmpdst = dst;
                for (x = ViewportX_9; x != 0; x-=9)
                {
                    /*
                    455444554
                    */

                    // 1st quad
                    DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
                    DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
                    INTERPOLATE(src[1], src[2], 1)
                    DST[2] = DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                    DST[3] = DST[403] = Game_Palette[src[2]].pix | (Game_Palette[src[3]].pix << 16);

                    // 2nd quad
                    DST[4] = DST[404] = Game_Palette[src[3]].pix | (Game_Palette[src[4]].pix << 16);
                    DST[5] = DST[405] = Game_Palette[src[4]].pix | (Game_Palette[src[5]].pix << 16);
                    DST[6] = DST[406] = Game_Palette[src[5]].pix | (Game_Palette[src[6]].pix << 16);
                    INTERPOLATE(src[6], src[7], 6)
                    DST[7] = DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);

                    // 3rd quad
                    DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
                    DST[9] = DST[409] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);

                    src+=9;
                    dst+=20;
                }
                src+=(360-ViewportX_9);
                dst = tmpdst + 2*800;
            }
        }

        // display part right of the viewport
        ViewportWidth_9 = Game_OverlayDisplay.ViewportWidth + (Game_OverlayDisplay.ViewportX - ViewportX_9);
        if ( (ViewportWidth_9 % 9) != 0)
        {
            ViewportWidth_9 = ViewportWidth_9 + 9 - (ViewportWidth_9 % 9);
        }
        if ((ViewportX_9 + ViewportWidth_9) != 360)
        {
            src = zalsrc + ViewportX_9 + ViewportWidth_9;
            dst = zaldst + (((ViewportX_9 + ViewportWidth_9) * 20) / 9);
            ViewportX2_9 = 360 - (ViewportX_9 + ViewportWidth_9);

            y = Game_OverlayDisplay.ViewportHeight;
            // first line
            if (Game_OverlayDisplay.ViewportY == 0)
            {
                y--;

                tmpdst = dst;
                for (x = ViewportX2_9; x != 0; x-=9)
                {
                    /*
                    455444554
                    */

                    // 1st quad
                    DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
                    DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
                    INTERPOLATE1(src[1], src[2])
                    DST[2] = DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                    DST[3] = DST[403] = Game_Palette[src[2]].pix | (Game_Palette[src[3]].pix << 16);

                    // 2nd quad
                    DST[4] = DST[404] = Game_Palette[src[3]].pix | (Game_Palette[src[4]].pix << 16);
                    DST[5] = DST[405] = Game_Palette[src[4]].pix | (Game_Palette[src[5]].pix << 16);
                    DST[6] = DST[406] = Game_Palette[src[5]].pix | (Game_Palette[src[6]].pix << 16);
                    INTERPOLATE1(src[6], src[7])
                    DST[7] = DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);

                    // 3rd quad
                    DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
                    DST[9] = DST[409] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);

                    src+=9;
                    dst+=20;
                }
                src+=(360-ViewportX2_9);
                dst = tmpdst + 2*800;
            }

            for (; y != 0; y--)
            {
                tmpdst = dst;
                for (x = ViewportX2_9; x != 0; x-=9)
                {
                    /*
                    455444554
                    */

                    // 1st quad
                    DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
                    DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
                    INTERPOLATE(src[1], src[2], 1)
                    DST[2] = DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                    DST[3] = DST[403] = Game_Palette[src[2]].pix | (Game_Palette[src[3]].pix << 16);

                    // 2nd quad
                    DST[4] = DST[404] = Game_Palette[src[3]].pix | (Game_Palette[src[4]].pix << 16);
                    DST[5] = DST[405] = Game_Palette[src[4]].pix | (Game_Palette[src[5]].pix << 16);
                    DST[6] = DST[406] = Game_Palette[src[5]].pix | (Game_Palette[src[6]].pix << 16);
                    INTERPOLATE(src[6], src[7], 6)
                    DST[7] = DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);

                    // 3rd quad
                    DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
                    DST[9] = DST[409] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);

                    src+=9;
                    dst+=20;
                }
                src+=(360-ViewportX2_9);
                dst = tmpdst + 2*800;
            }
        }

        // display part below the viewport
        Flip_360x240x8_to_800x480x16_interpolated3_helper(zalsrc + 360 * Game_OverlayDisplay.ViewportHeight, zaldst + 800 * 2 * Game_OverlayDisplay.ViewportHeight, 240 - (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight), 2);

        // the viewport
        src = zalsrc + ViewportX_9;
        orig = Game_OverlayDisplay.ScreenViewpartOriginal + 360 * Game_OverlayDisplay.ViewportY + ViewportX_9;
        dst = zaldst + ((ViewportX_9 * 20) / 9);
        src2 = Game_OverlayDisplay.ScreenViewpartOverlay + Game_OverlayDisplay.ViewportY*2 * 800 + ((ViewportX_9 * 20) / 9);

        y = Game_OverlayDisplay.ViewportHeight;
        // first line
        if (Game_OverlayDisplay.ViewportY == 0)
        {
            y--;

            tmpsrc2 = src2;
            tmpdst = dst;
            for (x = ViewportWidth_9; x != 0; x-=9)
            {
                /*
                455444554
                */

                if (src[0] == orig[0])
                {
                    srcval1 = *((uint16_t *)&(src2[0]));
                    srcval2 = *((uint16_t *)&(src2[800]));
                    DST[0] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[400] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
                }

                if (src[1] == orig[1])
                {
                    if (src[2] == orig[2])
                    {
                        srcval1 = *((uint16_t *)&(src2[2])) | (((uint32_t)(*((uint16_t *)&(src2[4])))) << 16);
                        DST[1] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        DST[2] = Game_Palette[BYTE2(srcval1)].pix | (Game_Palette[BYTE3(srcval1)].pix << 16);
                        srcval2 = *((uint16_t *)&(src2[802])) | (((uint32_t)(*((uint16_t *)&(src2[804])))) << 16);
                        dst[6] = Game_Palette[src2[6]].pix;
                        DST[401] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                        DST[402] = Game_Palette[BYTE2(srcval2)].pix | (Game_Palette[BYTE3(srcval2)].pix << 16);
                        dst[806] = Game_Palette[src2[806]].pix;
                    }
                    else
                    {
                        srcval1 = *((uint16_t *)&(src2[2]));
                        DST[1] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        INTERPOLATE1(src2[4], src[2])
                        srcval2 = *((uint16_t *)&(src2[802]));
                        DST[2] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                        DST[401] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                        INTERPOLATE1(src2[804], src[2])
                        DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                        dst[6] = dst[806] = Game_Palette[src[2]].pix;
                    }
                }
                else
                {
                    if (src[2] == orig[2])
                    {
                        srcval1 = *((uint16_t *)&(src2[4]));
                        DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
                        INTERPOLATE1(src[1], BYTE0(srcval1))
                        srcval2 = *((uint16_t *)&(src2[804]));
                        DST[2] = pixel.pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        INTERPOLATE1(src[1], BYTE0(srcval2))
                        DST[402] = pixel.pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                        dst[6] = Game_Palette[src2[6]].pix;
                        dst[806] = Game_Palette[src2[806]].pix;
                    }
                    else
                    {
                        DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
                        INTERPOLATE1(src[1], src[2])
                        DST[2] = DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                        dst[6] = dst[806] = Game_Palette[src[2]].pix;
                    }
                }

                if (src[3] == orig[3])
                {
                    dst[7] = Game_Palette[src2[7]].pix;
                    dst[8] = Game_Palette[src2[8]].pix;
                    dst[807] = Game_Palette[src2[807]].pix;
                    dst[808] = Game_Palette[src2[808]].pix;
                }
                else
                {
                    dst[7] = dst[8] = dst[807] = dst[808] = Game_Palette[src[3]].pix;
                }

                if (src[4] == orig[4])
                {
                    dst[9] = Game_Palette[src2[9]].pix;
                    dst[10] = Game_Palette[src2[10]].pix;
                    dst[809] = Game_Palette[src2[809]].pix;
                    dst[810] = Game_Palette[src2[810]].pix;
                }
                else
                {
                    dst[9] = dst[10] = dst[809] = dst[810] = Game_Palette[src[4]].pix;
                }

                if (src[5] == orig[5])
                {
                    dst[11] = Game_Palette[src2[11]].pix;
                    dst[12] = Game_Palette[src2[12]].pix;
                    dst[811] = Game_Palette[src2[811]].pix;
                    dst[812] = Game_Palette[src2[812]].pix;
                }
                else
                {
                    dst[11] = dst[12] = dst[811] = dst[812] = Game_Palette[src[5]].pix;
                }

                if (src[6] == orig[6])
                {
                    if (src[7] == orig[7])
                    {
                        dst[13] = Game_Palette[src2[13]].pix;
                        srcval1 = *((uint16_t *)&(src2[14])) | (((uint32_t)(*((uint16_t *)&(src2[16])))) << 16);
                        dst[813] = Game_Palette[src2[813]].pix;
                        srcval2 = *((uint16_t *)&(src2[814])) | (((uint32_t)(*((uint16_t *)&(src2[816])))) << 16);
                        DST[7] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        DST[8] = Game_Palette[BYTE2(srcval1)].pix | (Game_Palette[BYTE3(srcval1)].pix << 16);
                        DST[407] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                        DST[408] = Game_Palette[BYTE2(srcval2)].pix | (Game_Palette[BYTE3(srcval2)].pix << 16);
                    }
                    else
                    {
                        dst[13] = Game_Palette[src2[13]].pix;
                        srcval1 = *((uint16_t *)&(src2[14]));
                        INTERPOLATE1(BYTE1(srcval1), src[7])
                        DST[7] = Game_Palette[BYTE0(srcval1)].pix | (pixel.pix << 16);
                        dst[813] = Game_Palette[src2[813]].pix;
                        srcval2 = *((uint16_t *)&(src2[814]));
                        INTERPOLATE1(BYTE1(srcval2), src[7])
                        DST[407] = Game_Palette[BYTE0(srcval2)].pix | (pixel.pix << 16);
                        DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
                    }
                }
                else
                {
                    if (src[7] == orig[7])
                    {
                        dst[13] = dst[813] = Game_Palette[src[6]].pix;
                        INTERPOLATE1(src[6], src2[15])
                        srcval1 = *((uint16_t *)&(src2[16]));
                        DST[7] = Game_Palette[src[6]].pix | (pixel.pix << 16);
                        DST[8] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        INTERPOLATE1(src[6], src2[815])
                        srcval2 = *((uint16_t *)&(src2[816]));
                        DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);
                        DST[408] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                    }
                    else
                    {
                        dst[13] = dst[813] = Game_Palette[src[6]].pix;
                        INTERPOLATE1(src[6], src[7])
                        DST[7] = DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);
                        DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
                    }
                }

                if (src[8] == orig[8])
                {
                    srcval1 = *((uint16_t *)&(src2[18]));
                    srcval2 = *((uint16_t *)&(src2[818]));
                    DST[9] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[409] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[9] = DST[409] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);
                }

                src+=9;
                orig+=9;
                src2+=20;
                dst+=20;
            }
            src+=(360-ViewportWidth_9);
            orig+=(360-ViewportWidth_9);
            src2 = tmpsrc2 + 2*800;
            dst = tmpdst + 2*800;
        }

        for (; y != 0; y--)
        {
            tmpsrc2 = src2;
            tmpdst = dst;
            for (x = ViewportWidth_9; x != 0; x-=9)
            {
                /*
                455444554
                */

                if (src[0] == orig[0])
                {
                    srcval1 = *((uint16_t *)&(src2[0]));
                    srcval2 = *((uint16_t *)&(src2[800]));
                    DST[0] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[400] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
                }

                if (src[1] == orig[1])
                {
                    if (src[2] == orig[2])
                    {
                        srcval1 = *((uint16_t *)&(src2[2])) | (((uint32_t)(*((uint16_t *)&(src2[4])))) << 16);
                        DST[1] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        DST[2] = Game_Palette[BYTE2(srcval1)].pix | (Game_Palette[BYTE3(srcval1)].pix << 16);
                        srcval2 = *((uint16_t *)&(src2[802])) | (((uint32_t)(*((uint16_t *)&(src2[804])))) << 16);
                        dst[6] = Game_Palette[src2[6]].pix;
                        DST[401] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                        DST[402] = Game_Palette[BYTE2(srcval2)].pix | (Game_Palette[BYTE3(srcval2)].pix << 16);
                        dst[806] = Game_Palette[src2[806]].pix;
                    }
                    else
                    {
                        srcval1 = *((uint16_t *)&(src2[2]));
                        DST[1] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        INTERPOLATE1(src2[4], src[2])
                        srcval2 = *((uint16_t *)&(src2[802]));
                        DST[2] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                        DST[401] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                        INTERPOLATE1(src2[804], src[2])
                        DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                        dst[6] = dst[806] = Game_Palette[src[2]].pix;
                    }
                }
                else
                {
                    if (src[2] == orig[2])
                    {
                        srcval1 = *((uint16_t *)&(src2[4]));
                        DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
                        INTERPOLATE1(src[1], BYTE0(srcval1))
                        srcval2 = *((uint16_t *)&(src2[804]));
                        DST[2] = pixel.pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        INTERPOLATE1(src[1], BYTE0(srcval2))
                        DST[402] = pixel.pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                        dst[6] = Game_Palette[src2[6]].pix;
                        dst[806] = Game_Palette[src2[806]].pix;
                    }
                    else
                    {
                        DST[1] = DST[401] = Game_Palette[src[1]].pix | (Game_Palette[src[1]].pix << 16);
                        INTERPOLATE(src[1], src[2], 1)
                        DST[2] = DST[402] = pixel.pix | (Game_Palette[src[2]].pix << 16);
                        dst[6] = dst[806] = Game_Palette[src[2]].pix;
                    }
                }

                if (src[3] == orig[3])
                {
                    dst[7] = Game_Palette[src2[7]].pix;
                    dst[8] = Game_Palette[src2[8]].pix;
                    dst[807] = Game_Palette[src2[807]].pix;
                    dst[808] = Game_Palette[src2[808]].pix;
                }
                else
                {
                    dst[7] = dst[8] = dst[807] = dst[808] = Game_Palette[src[3]].pix;
                }

                if (src[4] == orig[4])
                {
                    dst[9] = Game_Palette[src2[9]].pix;
                    dst[10] = Game_Palette[src2[10]].pix;
                    dst[809] = Game_Palette[src2[809]].pix;
                    dst[810] = Game_Palette[src2[810]].pix;
                }
                else
                {
                    dst[9] = dst[10] = dst[809] = dst[810] = Game_Palette[src[4]].pix;
                }

                if (src[5] == orig[5])
                {
                    dst[11] = Game_Palette[src2[11]].pix;
                    dst[12] = Game_Palette[src2[12]].pix;
                    dst[811] = Game_Palette[src2[811]].pix;
                    dst[812] = Game_Palette[src2[812]].pix;
                }
                else
                {
                    dst[11] = dst[12] = dst[811] = dst[812] = Game_Palette[src[5]].pix;
                }

                if (src[6] == orig[6])
                {
                    if (src[7] == orig[7])
                    {
                        dst[13] = Game_Palette[src2[13]].pix;
                        srcval1 = *((uint16_t *)&(src2[14])) | (((uint32_t)(*((uint16_t *)&(src2[16])))) << 16);
                        dst[813] = Game_Palette[src2[813]].pix;
                        srcval2 = *((uint16_t *)&(src2[814])) | (((uint32_t)(*((uint16_t *)&(src2[816])))) << 16);
                        DST[7] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        DST[8] = Game_Palette[BYTE2(srcval1)].pix | (Game_Palette[BYTE3(srcval1)].pix << 16);
                        DST[407] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                        DST[408] = Game_Palette[BYTE2(srcval2)].pix | (Game_Palette[BYTE3(srcval2)].pix << 16);
                    }
                    else
                    {
                        dst[13] = Game_Palette[src2[13]].pix;
                        srcval1 = *((uint16_t *)&(src2[14]));
                        INTERPOLATE1(BYTE1(srcval1), src[7])
                        DST[7] = Game_Palette[BYTE0(srcval1)].pix | (pixel.pix << 16);
                        dst[813] = Game_Palette[src2[813]].pix;
                        srcval2 = *((uint16_t *)&(src2[814]));
                        INTERPOLATE1(BYTE1(srcval2), src[7])
                        DST[407] = Game_Palette[BYTE0(srcval2)].pix | (pixel.pix << 16);
                        DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
                    }
                }
                else
                {
                    if (src[7] == orig[7])
                    {
                        dst[13] = dst[813] = Game_Palette[src[6]].pix;
                        INTERPOLATE1(src[6], src2[15])
                        srcval1 = *((uint16_t *)&(src2[16]));
                        DST[7] = Game_Palette[src[6]].pix | (pixel.pix << 16);
                        DST[8] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        INTERPOLATE1(src[6], src2[815])
                        srcval2 = *((uint16_t *)&(src2[816]));
                        DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);
                        DST[408] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                    }
                    else
                    {
                        dst[13] = dst[813] = Game_Palette[src[6]].pix;
                        INTERPOLATE(src[6], src[7], 6)
                        DST[7] = DST[407] = Game_Palette[src[6]].pix | (pixel.pix << 16);
                        DST[8] = DST[408] = Game_Palette[src[7]].pix | (Game_Palette[src[7]].pix << 16);
                    }
                }

                if (src[8] == orig[8])
                {
                    srcval1 = *((uint16_t *)&(src2[18]));
                    srcval2 = *((uint16_t *)&(src2[818]));
                    DST[9] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[409] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[9] = DST[409] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);
                }

                src+=9;
                orig+=9;
                src2+=20;
                dst+=20;
            }
            src+=(360-ViewportWidth_9);
            orig+=(360-ViewportWidth_9);
            src2 = tmpsrc2 + 2*800;
            dst = tmpdst + 2*800;
        }

        if (DrawOverlay & 1)
        {
            dst = zaldst + 800 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + (((Game_OverlayDisplay.ViewportX + 1) * 20) / 9);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + (((Game_OverlayDisplay.ViewportX + 1) * 20) / 9);
            for (x = 10; x != 0; x--)
            {
                dst[0] = Game_Palette[src2[0]].pix;
                dst[1] = Game_Palette[src2[1]].pix;
                dst[800] = Game_Palette[src2[800]].pix;
                dst[801] = Game_Palette[src2[801]].pix;

                src2+=2;
                dst+=2;
            }
        }

        if (DrawOverlay & 2)
        {
            dst = zaldst + 800 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + (((Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth - 10) * 20) / 9);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + (((Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth - 10) * 20) / 9);
            for (x = 10; x != 0; x--)
            {
                dst[0] = Game_Palette[src2[0]].pix;
                dst[1] = Game_Palette[src2[1]].pix;
                dst[800] = Game_Palette[src2[800]].pix;
                dst[801] = Game_Palette[src2[801]].pix;

                src2+=2;
                dst+=2;
            }
        }
    }
    else
    {
        Flip_360x240x8_to_800x480x16_interpolated3_helper(src, dst, 240, 3);
    }
#undef DST
#undef SRC
#undef INTERPOLATE1
#undef INTERPOLATE
#undef INTERPOLATE_COLOR
}

static void Flip_360x240x8_to_720x480x16_in_800x480_helper(uint8_t *src, uint16_t *dst, int height)
{
#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    for (; height !=0; height--)
    {
        register int x;

        for (x = 360; x != 0; x-=36)
        {
            register uint32_t src1, src2;

            // 1st quad
            src1 = SRC[0];
            DST[0] = DST[400] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[1] = DST[401] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[2] = DST[402] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[1];
            DST[3] = DST[403] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            // 2nd quad
            DST[4] = DST[404] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[5] = DST[405] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[6] = DST[406] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[2];
            DST[7] = DST[407] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

            // 3rd quad
            DST[8] = DST[408] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[9] = DST[409] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[10] = DST[410] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[3];
            DST[11] = DST[411] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            // 4th quad
            DST[12] = DST[412] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[13] = DST[413] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[14] = DST[414] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[4];
            DST[15] = DST[415] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

            // 5th quad
            DST[16] = DST[416] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[17] = DST[417] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[18] = DST[418] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[5];
            DST[19] = DST[419] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            // 6th quad
            DST[20] = DST[420] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[21] = DST[421] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[22] = DST[422] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[6];
            DST[23] = DST[423] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

            // 7th quad
            DST[24] = DST[424] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[25] = DST[425] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[26] = DST[426] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[7];
            DST[27] = DST[427] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            // 8th quad
            DST[28] = DST[428] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[29] = DST[429] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[30] = DST[430] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[8];
            DST[31] = DST[431] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

            // 9th quad
            DST[32] = DST[432] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[33] = DST[433] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[34] = DST[434] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            DST[35] = DST[435] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            dst+=72;
            src+=36;
        }
        dst += 800 + (800 - 720);
    }
#undef DST
#undef SRC
}

static void Flip_360x240x8_to_720x480x16_in_800x480(uint8_t *src, uint16_t *dst)
{
    int x, y, DrawOverlay, ViewportX_4, ViewportWidth_4, ViewportX2_4;

#define DST ((uint32_t *) dst)

    DrawOverlay = Get_DrawOverlay(src, 1);

    dst += 40;

    if (DrawOverlay)
    {
        uint8_t *zalsrc, *src2, *orig;
        uint16_t *zaldst;
        uint32_t srcval0, srcval1, srcval2, origval0;

        // display part above the viewport
        if (Game_OverlayDisplay.ViewportY != 0)
        {
            Flip_360x240x8_to_720x480x16_in_800x480_helper(src, dst, Game_OverlayDisplay.ViewportY);
            src += 360 * Game_OverlayDisplay.ViewportY;
            dst += 800 * 2 * Game_OverlayDisplay.ViewportY;
        }

        zalsrc = src;
        zaldst = dst;

        // display part left of the viewport
        ViewportX_4 = Game_OverlayDisplay.ViewportX - (Game_OverlayDisplay.ViewportX % 4);
        if (ViewportX_4 != 0)
        {
            for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                for (x = ViewportX_4; x != 0; x-=4)
                {
                    srcval0 = *((uint32_t *)src);
                    DST[0] = DST[400] = Game_Palette[BYTE0(srcval0)].pix | (Game_Palette[BYTE0(srcval0)].pix << 16);
                    DST[1] = DST[401] = Game_Palette[BYTE1(srcval0)].pix | (Game_Palette[BYTE1(srcval0)].pix << 16);
                    DST[2] = DST[402] = Game_Palette[BYTE2(srcval0)].pix | (Game_Palette[BYTE2(srcval0)].pix << 16);
                    DST[3] = DST[403] = Game_Palette[BYTE3(srcval0)].pix | (Game_Palette[BYTE3(srcval0)].pix << 16);

                    src+=4;
                    dst+=8;
                }
                src+=(360-ViewportX_4);
                dst+=800+(800-2*ViewportX_4);
            }
        }

        // display part right of the viewport
        ViewportWidth_4 = Game_OverlayDisplay.ViewportWidth + (Game_OverlayDisplay.ViewportX - ViewportX_4);
        if ( (ViewportWidth_4 % 4) != 0)
        {
            ViewportWidth_4 = ViewportWidth_4 + 4 - (ViewportWidth_4 % 4);
        }
        if ((ViewportX_4 + ViewportWidth_4) != 360)
        {
            src = zalsrc + ViewportX_4 + ViewportWidth_4;
            dst = zaldst + 2 * (ViewportX_4 + ViewportWidth_4);
            ViewportX2_4 = 360 - (ViewportX_4 + ViewportWidth_4);
            for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                for (x = ViewportX2_4; x != 0; x-=4)
                {
                    srcval0 = *((uint32_t *)src);
                    DST[0] = DST[400] = Game_Palette[BYTE0(srcval0)].pix | (Game_Palette[BYTE0(srcval0)].pix << 16);
                    DST[1] = DST[401] = Game_Palette[BYTE1(srcval0)].pix | (Game_Palette[BYTE1(srcval0)].pix << 16);
                    DST[2] = DST[402] = Game_Palette[BYTE2(srcval0)].pix | (Game_Palette[BYTE2(srcval0)].pix << 16);
                    DST[3] = DST[403] = Game_Palette[BYTE3(srcval0)].pix | (Game_Palette[BYTE3(srcval0)].pix << 16);

                    src+=4;
                    dst+=8;
                }
                src+=(360-ViewportX2_4);
                dst+=800+(800-2*ViewportX2_4);
            }
        }

        // display part below the viewport
        Flip_360x240x8_to_720x480x16_in_800x480_helper(zalsrc + 360 * Game_OverlayDisplay.ViewportHeight, zaldst + 800 * 2 * Game_OverlayDisplay.ViewportHeight, 240 - (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight));

        // the viewport
        src = zalsrc + ViewportX_4;
        orig = Game_OverlayDisplay.ScreenViewpartOriginal + 360 * Game_OverlayDisplay.ViewportY + ViewportX_4;
        dst = zaldst + 2 * ViewportX_4;
        src2 = Game_OverlayDisplay.ScreenViewpartOverlay + Game_OverlayDisplay.ViewportY*2 * 800 + ViewportX_4*2;
        for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
        {
            for (x = ViewportWidth_4; x != 0; x-=4)
            {
                srcval0 = *((uint32_t *)src);
                origval0 = *((uint32_t *)orig);

                if (BYTE0(srcval0) == BYTE0(origval0))
                {
                    srcval1 = *((uint16_t *)&(src2[0]));
                    srcval2 = *((uint16_t *)&(src2[800]));
                    DST[0] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[400] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[0] = DST[400] = Game_Palette[BYTE0(srcval0)].pix | (Game_Palette[BYTE0(srcval0)].pix << 16);
                }
                if (BYTE1(srcval0) == BYTE1(origval0))
                {
                    srcval1 = *((uint16_t *)&(src2[2]));
                    srcval2 = *((uint16_t *)&(src2[802]));
                    DST[1] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[401] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[1] = DST[401] = Game_Palette[BYTE1(srcval0)].pix | (Game_Palette[BYTE1(srcval0)].pix << 16);
                }
                if (BYTE2(srcval0) == BYTE2(origval0))
                {
                    srcval1 = *((uint16_t *)&(src2[4]));
                    srcval2 = *((uint16_t *)&(src2[804]));
                    DST[2] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[402] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[2] = DST[402] = Game_Palette[BYTE2(srcval0)].pix | (Game_Palette[BYTE2(srcval0)].pix << 16);
                }
                if (BYTE3(srcval0) == BYTE3(origval0))
                {
                    srcval1 = *((uint16_t *)&(src2[6]));
                    srcval2 = *((uint16_t *)&(src2[806]));
                    DST[3] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[403] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[3] = DST[403] = Game_Palette[BYTE3(srcval0)].pix | (Game_Palette[BYTE3(srcval0)].pix << 16);
                }

                src+=4;
                orig+=4;
                src2+=8;
                dst+=8;
            }
            src+=(360 - ViewportWidth_4);
            orig+=(360 - ViewportWidth_4);
            src2+=800 + (800 - 2*ViewportWidth_4);
            dst+=800 + (800 - 2 * ViewportWidth_4);
        }

        if (DrawOverlay & 1)
        {
            dst = zaldst + 800 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + 2 * (Game_OverlayDisplay.ViewportX + 1);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + (Game_OverlayDisplay.ViewportX + 1)*2;
            for (x = 8; x != 0; x--)
            {
                srcval1 = *((uint16_t *)src2);
                srcval2 = *((uint16_t *)&(src2[800]));
                DST[0] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                DST[400] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);

                src2+=2;
                dst+=2;
            }
        }

        if (DrawOverlay & 2)
        {
            dst = zaldst + 800 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + 2 * Game_OverlayDisplay.ViewportX + 2 * (Game_OverlayDisplay.ViewportWidth - 10);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + Game_OverlayDisplay.ViewportX*2 + 2*(Game_OverlayDisplay.ViewportWidth - 10);
            for (x = 8; x != 0; x--)
            {
                srcval1 = *((uint16_t *)src2);
                srcval2 = *((uint16_t *)&(src2[800]));
                DST[0] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                DST[400] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);

                src2+=2;
                dst+=2;
            }
        }
    }
    else
    {
        Flip_360x240x8_to_720x480x16_in_800x480_helper(src, dst, 240);
    }

#undef DST
}

/*
static void Flip_360x240x8_to_640x480x16_in_800x480(uint8_t *src, uint16_t *dst)
{
    register int y, x;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    dst += 80;

    for (y = 240; y !=0; y--)
    {
        for (x = 360; x != 0; x-=36)
        {
            register uint32_t src1, src2;
            //
            //221222122 221222122 221222122 221222122
            //

            // 1st quad
            src1 = SRC[0];
            DST[0] = DST[400] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[1] = DST[401] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            src2 = SRC[1];
            DST[2] = DST[402] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[3] = DST[403] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);

            // 2nd quad
            DST[4] = DST[404] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[5] = DST[405] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[2];
            DST[6] = DST[406] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[7] = DST[407] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);

            // 3rd quad
            DST[8] = DST[408] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            src2 = SRC[3];
            DST[9] = DST[409] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            DST[10] = DST[410] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[11] = DST[411] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);

            // 4th quad
            DST[12] = DST[412] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[4];
            DST[13] = DST[413] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[14] = DST[414] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[15] = DST[415] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);

            // 5th quad
            DST[16] = DST[416] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[5];
            DST[17] = DST[417] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[18] = DST[418] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[19] = DST[419] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);

            // 6th quad
            src1 = SRC[6];
            DST[20] = DST[420] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[21] = DST[421] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[22] = DST[422] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[23] = DST[423] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);

            // 7th quad
            src2 = SRC[7];
            DST[24] = DST[424] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[25] = DST[425] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[26] = DST[426] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[8];
            DST[27] = DST[427] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

            // 8th quad
            DST[28] = DST[428] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[29] = DST[429] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[30] = DST[430] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            DST[31] = DST[431] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            dst+=64;
            src+=36;
        }
        dst += 960;
    }
#undef DST
#undef SRC
}
*/

static void Flip_360x240x8_to_640x480x16_in_800x480_interpolated2_lt_helper(uint8_t *src, uint16_t *dst, int height)
{
#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    for (; height !=0; height--)
    {
        int x;

        for (x = 360; x != 0; x-=36)
        {
            register uint32_t src1, src2;
            /*
            433444334 433444334 433444334 433444334
            */

            // 1st quad
            src1 = SRC[0];
            DST[0] = DST[400] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[1] = DST[401] = Game_Palette[BYTE1(src1)].pix | (IM2_LOOKUP(BYTE1(src1), BYTE2(src1)) << 16);
            src2 = SRC[1];
            DST[2] = DST[402] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[3] = DST[403] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);

            // 2nd quad
            DST[4] = DST[404] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[5] = DST[405] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            DST[6] = DST[406] = IM2_LOOKUP(BYTE2(src2), BYTE3(src2)) | (Game_Palette[BYTE3(src2)].pix << 16);
            src1 = SRC[2];
            DST[7] = DST[407] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);

            // 3rd quad
            DST[8] = DST[408] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[9] = DST[409] = Game_Palette[BYTE2(src1)].pix | (IM2_LOOKUP(BYTE2(src1), BYTE3(src1)) << 16);
            src2 = SRC[3];
            DST[10] = DST[410] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[11] = DST[411] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);

            // 4th quad
            DST[12] = DST[412] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[4];
            DST[13] = DST[413] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[14] = DST[414] = IM2_LOOKUP(BYTE3(src2), BYTE0(src1)) | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[15] = DST[415] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);

            // 5th quad
            DST[16] = DST[416] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            src2 = SRC[5];
            DST[17] = DST[417] = Game_Palette[BYTE3(src1)].pix | (IM2_LOOKUP(BYTE3(src1), BYTE0(src2)) << 16);
            DST[18] = DST[418] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[19] = DST[419] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);

            // 6th quad
            src1 = SRC[6];
            DST[20] = DST[420] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[21] = DST[421] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[22] = DST[422] = IM2_LOOKUP(BYTE0(src1), BYTE1(src1)) | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[23] = DST[423] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);

            // 7th quad
            src2 = SRC[7];
            DST[24] = DST[424] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[25] = DST[425] = Game_Palette[BYTE0(src2)].pix | (IM2_LOOKUP(BYTE0(src2), BYTE1(src2)) << 16);
            DST[26] = DST[426] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            src1 = SRC[8];
            DST[27] = DST[427] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

            // 8th quad
            DST[28] = DST[428] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[29] = DST[429] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[30] = DST[430] = IM2_LOOKUP(BYTE1(src1), BYTE2(src1)) | (Game_Palette[BYTE2(src1)].pix << 16);
            DST[31] = DST[431] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

            dst+=64;
            src+=36;
        }
        dst += 800 + (800 - 640);
    }
#undef DST
#undef SRC
}

static void Flip_360x240x8_to_640x480x16_in_800x480_interpolated2_lt(uint8_t *src, uint16_t *dst)
{
    int x, y, DrawOverlay, ViewportX_9, ViewportWidth_9, ViewportX2_9;

#define DST ((uint32_t *) dst)

    DrawOverlay = Get_DrawOverlay(src, 1);

    dst += 80;

    if (DrawOverlay)
    {
        uint8_t *zalsrc, *src2, *tmpsrc2, *orig;
        uint16_t *zaldst, *tmpdst;
        uint32_t srcval1, srcval2;

        // display part above the viewport
        if (Game_OverlayDisplay.ViewportY != 0)
        {
            Flip_360x240x8_to_640x480x16_in_800x480_interpolated2_lt_helper(src, dst, Game_OverlayDisplay.ViewportY);
            src += 360 * Game_OverlayDisplay.ViewportY;
            dst += 800 * 2 * Game_OverlayDisplay.ViewportY;
        }

        zalsrc = src;
        zaldst = dst;

        // display part left of the viewport
        ViewportX_9 = Game_OverlayDisplay.ViewportX - (Game_OverlayDisplay.ViewportX % 9);
        if (ViewportX_9 != 0)
        {
            for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                tmpdst = dst;
                for (x = ViewportX_9; x != 0; x-=9)
                {
                    /*
                    433444334
                    */

                    // 1st quad
                    DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
                    DST[1] = DST[401] = Game_Palette[src[1]].pix | (IM2_LOOKUP(src[1], src[2]) << 16);
                    DST[2] = DST[402] = Game_Palette[src[2]].pix | (Game_Palette[src[3]].pix << 16);
                    DST[3] = DST[403] = Game_Palette[src[3]].pix | (Game_Palette[src[4]].pix << 16);

                    // 2nd quad
                    DST[4] = DST[404] = Game_Palette[src[4]].pix | (Game_Palette[src[5]].pix << 16);
                    DST[5] = DST[405] = Game_Palette[src[5]].pix | (Game_Palette[src[6]].pix << 16);
                    DST[6] = DST[406] = IM2_LOOKUP(src[6], src[7]) | (Game_Palette[src[7]].pix << 16);
                    DST[7] = DST[407] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);

                    src+=9;
                    dst+=16;
                }
                src+=(360-ViewportX_9);
                dst = tmpdst + 2*800;
            }
        }

        // display part right of the viewport
        ViewportWidth_9 = Game_OverlayDisplay.ViewportWidth + (Game_OverlayDisplay.ViewportX - ViewportX_9);
        if ( (ViewportWidth_9 % 9) != 0)
        {
            ViewportWidth_9 = ViewportWidth_9 + 9 - (ViewportWidth_9 % 9);
        }
        if ((ViewportX_9 + ViewportWidth_9) != 360)
        {
            src = zalsrc + ViewportX_9 + ViewportWidth_9;
            dst = zaldst + (((ViewportX_9 + ViewportWidth_9) * 16) / 9);
            ViewportX2_9 = 360 - (ViewportX_9 + ViewportWidth_9);
            for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                tmpdst = dst;
                for (x = ViewportX2_9; x != 0; x-=9)
                {
                    /*
                    433444334
                    */

                    // 1st quad
                    DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
                    DST[1] = DST[401] = Game_Palette[src[1]].pix | (IM2_LOOKUP(src[1], src[2]) << 16);
                    DST[2] = DST[402] = Game_Palette[src[2]].pix | (Game_Palette[src[3]].pix << 16);
                    DST[3] = DST[403] = Game_Palette[src[3]].pix | (Game_Palette[src[4]].pix << 16);

                    // 2nd quad
                    DST[4] = DST[404] = Game_Palette[src[4]].pix | (Game_Palette[src[5]].pix << 16);
                    DST[5] = DST[405] = Game_Palette[src[5]].pix | (Game_Palette[src[6]].pix << 16);
                    DST[6] = DST[406] = IM2_LOOKUP(src[6], src[7]) | (Game_Palette[src[7]].pix << 16);
                    DST[7] = DST[407] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);

                    src+=9;
                    dst+=16;
                }
                src+=(360-ViewportX2_9);
                dst = tmpdst + 2*800;
            }
        }

        // display part below the viewport
        Flip_360x240x8_to_640x480x16_in_800x480_interpolated2_lt_helper(zalsrc + 360 * Game_OverlayDisplay.ViewportHeight, zaldst + 800 * 2 * Game_OverlayDisplay.ViewportHeight, 240 - (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight));

        // the viewport
        src = zalsrc + ViewportX_9;
        orig = Game_OverlayDisplay.ScreenViewpartOriginal + 360 * Game_OverlayDisplay.ViewportY + ViewportX_9;
        dst = zaldst + ((ViewportX_9 * 16) / 9);
        src2 = Game_OverlayDisplay.ScreenViewpartOverlay + Game_OverlayDisplay.ViewportY*2 * 800 + ((ViewportX_9 * 16) / 9);
        for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
        {
            tmpsrc2 = src2;
            tmpdst = dst;
            for (x = ViewportWidth_9; x != 0; x-=9)
            {
                /*
                433444334
                */

                if (src[0] == orig[0])
                {
                    srcval1 = *((uint16_t *)&(src2[0]));
                    srcval2 = *((uint16_t *)&(src2[800]));
                    DST[0] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[400] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[0] = DST[400] = Game_Palette[src[0]].pix | (Game_Palette[src[0]].pix << 16);
                }

                if (src[1] == orig[1])
                {
                    if (src[2] == orig[2])
                    {
                        srcval1 = *((uint16_t *)&(src2[2]));
                        DST[1] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        srcval2 = *((uint16_t *)&(src2[802]));
                        dst[4] = Game_Palette[src2[4]].pix;
                        DST[401] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                        dst[804] = Game_Palette[src2[804]].pix;
                    }
                    else
                    {
                        srcval1 = *((uint16_t *)&(src2[2]));
                        srcval2 = *((uint16_t *)&(src2[802]));
                        DST[1] = Game_Palette[BYTE0(srcval1)].pix | (IM2_LOOKUP(BYTE1(srcval1), src[2]) << 16);
                        DST[401] = Game_Palette[BYTE0(srcval2)].pix | (IM2_LOOKUP(BYTE1(srcval2), src[2]) << 16);
                        dst[4] = dst[804] = Game_Palette[src[2]].pix;
                    }
                }
                else
                {
                    if (src[2] == orig[2])
                    {
                        dst[2] = dst[802] = Game_Palette[src[1]].pix;
                        dst[3] = IM2_LOOKUP(src[1], src2[3]);
                        dst[4] = Game_Palette[src2[4]].pix;
                        dst[803] = IM2_LOOKUP(src[1], src2[803]);
                        dst[804] = Game_Palette[src2[804]].pix;
                    }
                    else
                    {
                        DST[1] = DST[401] = Game_Palette[src[1]].pix | (IM2_LOOKUP(src[1], src[2]) << 16);
                        dst[4] = dst[804] = Game_Palette[src[2]].pix;
                    }
                }

                if (src[3] == orig[3])
                {
                    dst[5] = Game_Palette[src2[5]].pix;
                    dst[6] = Game_Palette[src2[6]].pix;
                    dst[805] = Game_Palette[src2[805]].pix;
                    dst[806] = Game_Palette[src2[806]].pix;
                }
                else
                {
                    dst[5] = dst[6] = dst[805] = dst[806] = Game_Palette[src[3]].pix;
                }

                if (src[4] == orig[4])
                {
                    dst[7] = Game_Palette[src2[7]].pix;
                    dst[8] = Game_Palette[src2[8]].pix;
                    dst[807] = Game_Palette[src2[807]].pix;
                    dst[808] = Game_Palette[src2[808]].pix;
                }
                else
                {
                    dst[7] = dst[8] = dst[807] = dst[808] = Game_Palette[src[4]].pix;
                }

                if (src[5] == orig[5])
                {
                    dst[9] = Game_Palette[src2[9]].pix;
                    dst[10] = Game_Palette[src2[10]].pix;
                    dst[809] = Game_Palette[src2[809]].pix;
                    dst[810] = Game_Palette[src2[810]].pix;
                }
                else
                {
                    dst[9] = dst[10] = dst[809] = dst[810] = Game_Palette[src[5]].pix;
                }

                if (src[6] == orig[6])
                {
                    if (src[7] == orig[7])
                    {
                        dst[11] = Game_Palette[src2[11]].pix;
                        srcval1 = *((uint16_t *)&(src2[12]));
                        dst[811] = Game_Palette[src2[811]].pix;
                        srcval2 = *((uint16_t *)&(src2[812]));
                        DST[6] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                        DST[406] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                    }
                    else
                    {
                        dst[11] = Game_Palette[src2[11]].pix;
                        dst[12] = IM2_LOOKUP(src2[12], src[7]);
                        dst[811] = Game_Palette[src2[811]].pix;
                        dst[812] = IM2_LOOKUP(src2[812], src[7]);
                        dst[13] = dst[813] = Game_Palette[src[7]].pix;
                    }
                }
                else
                {
                    if (src[7] == orig[7])
                    {
                        dst[11] = dst[811] = Game_Palette[src[6]].pix;
                        DST[6] = IM2_LOOKUP(src[6], src2[12]) | (Game_Palette[src2[13]].pix << 16);
                        DST[406] = IM2_LOOKUP(src[6], src2[812]) | (Game_Palette[src2[813]].pix << 16);
                    }
                    else
                    {
                        dst[11] = dst[811] = Game_Palette[src[6]].pix;
                        DST[6] = DST[406] = IM2_LOOKUP(src[6], src[7]) | (Game_Palette[src[7]].pix << 16);
                    }
                }

                if (src[8] == orig[8])
                {
                    srcval1 = *((uint16_t *)&(src2[14]));
                    srcval2 = *((uint16_t *)&(src2[814]));
                    DST[7] = Game_Palette[BYTE0(srcval1)].pix | (Game_Palette[BYTE1(srcval1)].pix << 16);
                    DST[407] = Game_Palette[BYTE0(srcval2)].pix | (Game_Palette[BYTE1(srcval2)].pix << 16);
                }
                else
                {
                    DST[7] = DST[407] = Game_Palette[src[8]].pix | (Game_Palette[src[8]].pix << 16);
                }

                src+=9;
                orig+=9;
                src2+=16;
                dst+=16;
            }
            src+=(360-ViewportWidth_9);
            orig+=(360-ViewportWidth_9);
            src2 = tmpsrc2 + 2*800;
            dst = tmpdst + 2*800;
        }

        if (DrawOverlay & 1)
        {
            dst = zaldst + 800 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + (((Game_OverlayDisplay.ViewportX + 1) * 16) / 9);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + (((Game_OverlayDisplay.ViewportX + 1) * 16) / 9);
            for (x = 8; x != 0; x--)
            {
                dst[0] = Game_Palette[src2[0]].pix;
                dst[1] = Game_Palette[src2[1]].pix;
                dst[800] = Game_Palette[src2[800]].pix;
                dst[801] = Game_Palette[src2[801]].pix;

                src2+=2;
                dst+=2;
            }
        }

        if (DrawOverlay & 2)
        {
            dst = zaldst + 800 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + (((Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth - 10) * 16) / 9);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + (((Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth - 10) * 16) / 9);
            for (x = 8; x != 0; x--)
            {
                dst[0] = Game_Palette[src2[0]].pix;
                dst[1] = Game_Palette[src2[1]].pix;
                dst[800] = Game_Palette[src2[800]].pix;
                dst[801] = Game_Palette[src2[801]].pix;

                src2+=2;
                dst+=2;
            }
        }
    }
    else
    {
        Flip_360x240x8_to_640x480x16_in_800x480_interpolated2_lt_helper(src, dst, 240);
    }

#undef DST
}

void Init_Display(void)
{
    Font_Size_Shift = 2;
    DisplayMode = 0;
    Game_UseEnhanced3DEngineNewValue = 1;
}

void Init_Display2(void)
{
    Init_Palette();

    memset(&(interpolation_matrix2[0]), 0, sizeof(interpolation_matrix2));

    Display_Width = 800;
    Display_Height = 480;
    Display_Bitsperpixel = 16;
    Display_Fullscreen = 1;
    Display_MouseLocked = 0;
    //Render_Width = 720;
    Render_Height = 480;
    //Picture_Width = 720;
    Picture_Height = 480;
    //Picture_Position_UL_X = 40;
    Picture_Position_UL_Y = 0;
    //Picture_Position_BR_X = 759;
    Picture_Position_BR_Y = 479;
    //Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_720x480x16_in_800x480;

    Change_Display_Mode(0);
}

int Config_Display(char *str, char *param)
{
    if ( strcasecmp(str, "Display_Mode") == 0)	// str equals "Display_Mode"
    {
        if ( strcasecmp(param, "double_pixels") == 0)	// param equals "double_pixels"
        {
            // Double pixels - 720x480
            DisplayMode = 0;
        }
        else if ( strcasecmp(param, "fullscreen") == 0) // param equals "fullscreen"
        {
            // Fullscreen - 800x480
            DisplayMode = 1;
        }
        else if ( strcasecmp(param, "correct_aspect_ratio") == 0) // param equals "correct_aspect_ratio"
        {
            // Correct aspect ratio - 640x480
            DisplayMode = 2;
        }

        return 1;
    }

    return 0;
}

void Cleanup_Display(void)
{
}

int Change_Display_Mode(int direction)
{
    int ClearScreen;

    ClearScreen = 0;
    DisplayMode = (DisplayMode + direction + 3) % 3;

    switch (DisplayMode)
    {
        case 0:
            Render_Width = 720;
            Picture_Width = 720;
            Picture_Position_UL_X = 40;
            Picture_Position_BR_X = 759;
            Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_720x480x16_in_800x480;

            ClearScreen = 1;
            break;
        case 1:
            Render_Width = 800;
            Picture_Width = 800;
            Picture_Position_UL_X = 0;
            Picture_Position_BR_X = 799;
            Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_800x480x16_interpolated3;

            break;
        case 2:
            Render_Width = 640;
            Picture_Width = 640;
            Picture_Position_UL_X = 80;
            Picture_Position_BR_X = 719;
            Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_640x480x16_in_800x480_interpolated2_lt;

            ClearScreen = 1;
            break;
    }

    return ClearScreen;
}

