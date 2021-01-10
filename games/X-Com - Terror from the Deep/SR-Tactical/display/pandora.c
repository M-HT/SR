/**
 *
 *  Copyright (C) 2016-2021 Roman Pauer
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

static int DisplayMode;

int Change_Display_Mode(int direction);

static uint16_t interpolation_matrix[256*256];
#define IM_LOOKUP(a, b) ( (uint32_t) interpolation_matrix[(((uint32_t) (a)) << 8) + ((uint32_t) (b))] )

static void Set_Palette_Value2(uint32_t index, uint32_t r, uint32_t g, uint32_t b)
{
    uint16_t *val1, *val2;
    pixel_format_disp pixel;

#define INTERPOLATE(a, b) (((a) + (b)) / 2)

    val1 = &(interpolation_matrix[index * 256]);
    val2 = &(interpolation_matrix[index]);

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
static void Flip_320x200x8_to_800x480x16_interpolated_lt(uint8_t *src, uint16_t *dst)
{
    int y, x, counter2, counter3;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

#define INTERPOLATE(a1, b1, c1, d1) \
    { \
        pixel.s.r = ((uint32_t)Game_Palette_Or[(a1)].s.r + (uint32_t)Game_Palette_Or[(b1)].s.r + (uint32_t)Game_Palette_Or[(c1)].s.r + (uint32_t)Game_Palette_Or[(d1)].s.r) >> 5; \
        pixel.s.g = ((uint32_t)Game_Palette_Or[(a1)].s.g + (uint32_t)Game_Palette_Or[(b1)].s.g + (uint32_t)Game_Palette_Or[(c1)].s.g + (uint32_t)Game_Palette_Or[(d1)].s.g) >> 4; \
        pixel.s.b = ((uint32_t)Game_Palette_Or[(a1)].s.b + (uint32_t)Game_Palette_Or[(b1)].s.b + (uint32_t)Game_Palette_Or[(c1)].s.b + (uint32_t)Game_Palette_Or[(d1)].s.b) >> 5; \
    }

    counter2 = 1;
    counter3 = 3;

    for (y = 200; y !=0; y--)
    {
        for (x = 320; x != 0; x-=16)
        {
            register uint32_t src1, src2;

            src1 = SRC[0];

            // 1st quad
            DST[0] = DST[400] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[1] = DST[401] = IM_LOOKUP(BYTE0(src1), BYTE1(src1)) | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[2] = DST[402] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            DST[3] = DST[403] = Game_Palette[BYTE2(src1)].pix | (IM_LOOKUP(BYTE2(src1), BYTE3(src1)) << 16);
            src2 = SRC[1];

            // 2nd quad
            DST[4] = DST[404] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[5] = DST[405] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            DST[6] = DST[406] = IM_LOOKUP(BYTE0(src2), BYTE1(src2)) | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[7] = DST[407] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);

            // 3rd quad
            DST[8] = DST[408] = Game_Palette[BYTE2(src2)].pix | (IM_LOOKUP(BYTE2(src2), BYTE3(src2)) << 16);
            src1 = SRC[2];
            DST[9] = DST[409] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[10] = DST[410] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            DST[11] = DST[411] = IM_LOOKUP(BYTE0(src1), BYTE1(src1)) | (Game_Palette[BYTE1(src1)].pix << 16);

            // 4th quad
            DST[12] = DST[412] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            DST[13] = DST[413] = Game_Palette[BYTE2(src1)].pix | (IM_LOOKUP(BYTE2(src1), BYTE3(src1)) << 16);
            src2 = SRC[3];
            DST[14] = DST[414] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[15] = DST[415] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);

            // 5th quad
            DST[16] = DST[416] = IM_LOOKUP(BYTE0(src2), BYTE1(src2)) | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[17] = DST[417] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            DST[18] = DST[418] = Game_Palette[BYTE2(src2)].pix | (IM_LOOKUP(BYTE2(src2), BYTE3(src2)) << 16);
            DST[19] = DST[419] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);


            dst+=40;
            src+=16;
        }
        dst += 800;

        counter2--;
        if (counter2 == 0)
        {
            register pixel_format_disp pixel;

            counter2 = counter3;
            counter3 = 5 - counter3;

            src-=320;

            pixel.pix = 0;

            for (x = 320; x != 0; x-=8)
            {
                register uint32_t src1, src2;

                src1 = SRC[0];
                src2 = SRC[320/4];

                DST[0] = IM_LOOKUP(BYTE0(src1), BYTE0(src2)) | (IM_LOOKUP(BYTE0(src1), BYTE0(src2)) << 16);
                INTERPOLATE(BYTE0(src1), BYTE0(src2), BYTE1(src1), BYTE1(src2))
                DST[1] = pixel.pix | (IM_LOOKUP(BYTE1(src1), BYTE1(src2)) << 16);
                DST[2] = IM_LOOKUP(BYTE1(src1), BYTE1(src2)) | (IM_LOOKUP(BYTE2(src1), BYTE2(src2)) << 16);
                INTERPOLATE(BYTE2(src1), BYTE2(src2), BYTE3(src1), BYTE3(src2))
                DST[3] = IM_LOOKUP(BYTE2(src1), BYTE2(src2)) | (pixel.pix << 16);
                DST[4] = IM_LOOKUP(BYTE3(src1), BYTE3(src2)) | (IM_LOOKUP(BYTE3(src1), BYTE3(src2)) << 16);

                src1 = SRC[1];
                src2 = SRC[(320/4) + 1];

                DST[5] = IM_LOOKUP(BYTE0(src1), BYTE0(src2)) | (IM_LOOKUP(BYTE0(src1), BYTE0(src2)) << 16);
                INTERPOLATE(BYTE0(src1), BYTE0(src2), BYTE1(src1), BYTE1(src2))
                DST[6] = pixel.pix | (IM_LOOKUP(BYTE1(src1), BYTE1(src2)) << 16);
                DST[7] = IM_LOOKUP(BYTE1(src1), BYTE1(src2)) | (IM_LOOKUP(BYTE2(src1), BYTE2(src2)) << 16);
                INTERPOLATE(BYTE2(src1), BYTE2(src2), BYTE3(src1), BYTE3(src2))
                DST[8] = IM_LOOKUP(BYTE2(src1), BYTE2(src2)) | (pixel.pix << 16);
                DST[9] = IM_LOOKUP(BYTE3(src1), BYTE3(src2)) | (IM_LOOKUP(BYTE3(src1), BYTE3(src2)) << 16);

                src+=8;
                dst+=20;
            }
        }
    }

#undef INTERPOLATE
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

    color1 = Game_Palette_Or[src[0]].c[color_index]; // original
    color2 = Game_Palette_Or[src[1]].c[color_index]; // neighbour

    pattern = ((color1 & 0x80) >> 3) | ((color2 & 0x80) >> 6);
    pattern |= (Game_Palette_Or[src[-320]].c[color_index] & 0x80) >> 2; // previous
    pattern |= (Game_Palette_Or[src[-319]].c[color_index] & 0x80) >> 5; // previous - neighbour
    pattern |= (Game_Palette_Or[src[320]].c[color_index] & 0x80) >> 4; // next
    pattern |= (Game_Palette_Or[src[321]].c[color_index] & 0x80) >> 7; // next - neighbour

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

static uint32_t Get_pixel_value_interpolated4(uint8_t *src, int color_index)
{
    register uint32_t pattern, color1, color2;

    color1 = Game_Palette_Or[src[0]].c[color_index]; // original
    color2 = Game_Palette_Or[src[320]].c[color_index]; // neighbour

    pattern = ((color1 & 0x80) >> 3) | ((color2 & 0x80) >> 6);
    pattern |= (Game_Palette_Or[src[-1]].c[color_index] & 0x80) >> 2; // previous
    pattern |= (Game_Palette_Or[src[319]].c[color_index] & 0x80) >> 5; // previous - neighbour
    pattern |= (Game_Palette_Or[src[1]].c[color_index] & 0x80) >> 4; // next
    pattern |= (Game_Palette_Or[src[321]].c[color_index] & 0x80) >> 7; // next - neighbour

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

static uint32_t Get_pixel_value_interpolated5(uint8_t *src, int color_index, uint32_t color1, uint32_t color2)
{
    register uint32_t pattern;

    pattern = (((color1 << (3 - (color_index & 1))) & 0x80) >> 3)  // original
	        | (((color2 << (3 - (color_index & 1))) & 0x80) >> 6); // neighbour
    pattern |= (Game_Palette_Or[src[0]].c[color_index] & 0x80) >> 2; // previous
    pattern |= (Game_Palette_Or[src[1]].c[color_index] & 0x80) >> 5; // previous - neighbour
    pattern |= (Game_Palette_Or[src[320]].c[color_index] & 0x80) >> 4; // next
    pattern |= (Game_Palette_Or[src[321]].c[color_index] & 0x80) >> 7; // next - neighbour

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

static void Flip_320x200x8_to_800x480x16_interpolated5(uint8_t *src, uint16_t *dst)
{
    int y, x, counter2, counter3;
    pixel_format_disp pixel;

#define INTERPOLATE_COLOR(_pixel, color, color_index, shift, a, b, index, get_pixel_value) \
    { \
        if (((Game_Palette_Or[(a)].s.color & 0x80) == (Game_Palette_Or[(b)].s.color & 0x80)) || \
            ((Game_Palette_Or[(a)].s.color & 0x80) != ((Game_Palette_Or[(a)].s.color - Game_Palette_Or[(b)].s.color) & 0x80)) \
           ) \
        { \
            _pixel.s.color = ((uint32_t)Game_Palette_Or[(a)].s.color + (uint32_t)Game_Palette_Or[(b)].s.color) >> (1 + (shift)); \
        } \
        else \
        { \
            _pixel.s.color = get_pixel_value(&(src[index]), color_index) >> (shift); \
        } \
    }
#define INTERPOLATE3(_a, _b, index) \
    { \
        INTERPOLATE_COLOR(pixel, r, 0, 3, _a, _b, index, Get_pixel_value_interpolated3) \
        INTERPOLATE_COLOR(pixel, g, 1, 2, _a, _b, index, Get_pixel_value_interpolated3) \
        INTERPOLATE_COLOR(pixel, b, 2, 3, _a, _b, index, Get_pixel_value_interpolated3) \
    }
#define INTERPOLATE4(_pixel, _a, _b, index) \
    { \
        INTERPOLATE_COLOR(_pixel, r, 0, 3, _a, _b, index, Get_pixel_value_interpolated4) \
        INTERPOLATE_COLOR(_pixel, g, 1, 2, _a, _b, index, Get_pixel_value_interpolated4) \
        INTERPOLATE_COLOR(_pixel, b, 2, 3, _a, _b, index, Get_pixel_value_interpolated4) \
    }
#define INTERPOLATE_COLOR5(color, color_index, shift, index) \
    { \
        if (((oldpixel.s.color & (0x80 >> (shift))) == (pixel.s.color & (0x80 >> (shift)))) || \
            ((oldpixel.s.color & (0x80 >> (shift))) != ((oldpixel.s.color - pixel.s.color) & (0x80 >> (shift)))) \
           ) \
        { \
            newpixel.s.color = ((uint32_t)oldpixel.s.color + (uint32_t)pixel.s.color) >> 1; \
        } \
        else \
        { \
            newpixel.s.color = Get_pixel_value_interpolated5(&(src[index]), color_index, oldpixel.s.color, pixel.s.color); \
        } \
    }
#define INTERPOLATE5(index) \
    { \
        INTERPOLATE_COLOR5(r, 0, 3, index) \
        INTERPOLATE_COLOR5(g, 1, 2, index) \
        INTERPOLATE_COLOR5(b, 2, 3, index) \
    }
#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    pixel.pix = 0;

    // first line - use linear interpolation
    for (x = 320; x != 0; x-=4)
    {
        register uint32_t src1;

        src1 = SRC[0];

        DST[0] = DST[400] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
        DST[1] = DST[401] = IM_LOOKUP(BYTE0(src1), BYTE1(src1)) | (Game_Palette[BYTE1(src1)].pix << 16);
        DST[2] = DST[402] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
        DST[3] = DST[403] = Game_Palette[BYTE2(src1)].pix | (IM_LOOKUP(BYTE2(src1), BYTE3(src1)) << 16);
        DST[4] = DST[404] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

        dst+=10;
        src+=4;
    }
    dst += 800;

    counter2 = 1;
    counter3 = 2;

    for (y = 198; y !=0; y--)
    {
        for (x = 320; x != 0; x-=16)
        {
            register uint32_t src1, src2;

            src1 = SRC[0];

            // 1st quad
            DST[0] = DST[400] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            INTERPOLATE3(BYTE0(src1), BYTE1(src1), 0)
            DST[1] = DST[401] = pixel.pix | (Game_Palette[BYTE1(src1)].pix << 16);
            DST[2] = DST[402] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            INTERPOLATE3(BYTE2(src1), BYTE3(src1), 2)
            DST[3] = DST[403] = Game_Palette[BYTE2(src1)].pix | (pixel.pix << 16);
            src2 = SRC[1];

            // 2nd quad
            DST[4] = DST[404] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[5] = DST[405] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
            INTERPOLATE3(BYTE0(src2), BYTE1(src2), 4)
            DST[6] = DST[406] = pixel.pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[7] = DST[407] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);

            // 3rd quad
            INTERPOLATE3(BYTE2(src2), BYTE3(src2), 6)
            DST[8] = DST[408] = Game_Palette[BYTE2(src2)].pix | (pixel.pix << 16);
            src1 = SRC[2];
            DST[9] = DST[409] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
            DST[10] = DST[410] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
            INTERPOLATE3(BYTE0(src1), BYTE1(src1), 8)
            DST[11] = DST[411] = pixel.pix | (Game_Palette[BYTE1(src1)].pix << 16);

            // 4th quad
            DST[12] = DST[412] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
            INTERPOLATE3(BYTE2(src1), BYTE3(src1), 10)
            DST[13] = DST[413] = Game_Palette[BYTE2(src1)].pix | (pixel.pix << 16);
            src2 = SRC[3];
            DST[14] = DST[414] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
            DST[15] = DST[415] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE0(src2)].pix << 16);

            // 5th quad
            INTERPOLATE3(BYTE0(src2), BYTE1(src2), 12)
            DST[16] = DST[416] = pixel.pix | (Game_Palette[BYTE1(src2)].pix << 16);
            DST[17] = DST[417] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
            INTERPOLATE3(BYTE2(src2), BYTE3(src2), 14)
            DST[18] = DST[418] = Game_Palette[BYTE2(src2)].pix | (pixel.pix << 16);
            DST[19] = DST[419] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);


            dst+=40;
            src+=16;
        }
        dst += 800;

        counter2--;
        if (counter2 == 0)
        {
            pixel_format_disp oldpixel, newpixel;

            counter2 = counter3;
            counter3 = 5 - counter3;

            src-=320;

			newpixel.pix = 0;

            // first column - use linear interpolation
            {
                oldpixel.pix = IM_LOOKUP(src[0], src[320]);
            }

            for (x = 318; x != 0; x-=2)
            {
                dst[0] = dst[1] = oldpixel.pix;

                INTERPOLATE4(pixel, src[1], src[321], 1)

                dst[3] = dst[4] = pixel.pix;

                // calculate dst[2] based on oldpixel, pixel, src[0], src[1], src[320], src[321]
                INTERPOLATE5(0)
				dst[2] = newpixel.pix;

                dst+=5;
                src+=2;

                INTERPOLATE4(oldpixel, src[0], src[320], 0)
            }

            // last column - use linear interpolation
            {
                dst[0] = dst[1] = oldpixel.pix;

                pixel.pix = IM_LOOKUP(src[1], src[321]);

                dst[3] = dst[4] = pixel.pix;

                // calculate dst[2] based on oldpixel, pixel, src[0], src[1], src[320], src[321]
                INTERPOLATE5(0)
				dst[2] = newpixel.pix;

                dst+=5;
                src+=2;
            }
        }
    }

    // last line - use linear interpolation
    for (x = 320; x != 0; x-=4)
    {
        register uint32_t src1;

        src1 = SRC[0];

        DST[0] = DST[400] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
        DST[1] = DST[401] = IM_LOOKUP(BYTE0(src1), BYTE1(src1)) | (Game_Palette[BYTE1(src1)].pix << 16);
        DST[2] = DST[402] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
        DST[3] = DST[403] = Game_Palette[BYTE2(src1)].pix | (IM_LOOKUP(BYTE2(src1), BYTE3(src1)) << 16);
        DST[4] = DST[404] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

        dst+=10;
        src+=4;
    }
    //dst += 800;

#undef DST
#undef SRC
#undef INTERPOLATE5
#undef INTERPOLATE_COLOR5
#undef INTERPOLATE4
#undef INTERPOLATE3
#undef INTERPOLATE_COLOR
}

/*
static void Flip_320x200x8_to_640x480x16_in_800x480_interpolated_lt(uint8_t *src, uint16_t *dst)
{
    register int x, counter2;
    int y, counter3;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    dst += 80;

    counter2 = 1;
    counter3 = 3;

    for (y = 200; y !=0; y--)
    {
        for (x = 320; x != 0; x-=32)
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
            DST[31] = DST[431] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

            dst+=64;
            src+=32;
        }

        dst += 960;

        counter2--;
        if (counter2 == 0)
        {
            counter2 = counter3;
            counter3 = 5 - counter3;

            src-=320;

            for (x = 320; x != 0; x-=4)
            {
                register uint32_t src1, src2;

                src1 = SRC[0];
                src2 = SRC[320/4];

                DST[0] = IM_LOOKUP(BYTE0(src1), BYTE0(src2)) | (IM_LOOKUP(BYTE0(src1), BYTE0(src2)) << 16);
                DST[1] = IM_LOOKUP(BYTE1(src1), BYTE1(src2)) | (IM_LOOKUP(BYTE1(src1), BYTE1(src2)) << 16);
                DST[2] = IM_LOOKUP(BYTE2(src1), BYTE2(src2)) | (IM_LOOKUP(BYTE2(src1), BYTE2(src2)) << 16);
                DST[3] = IM_LOOKUP(BYTE3(src1), BYTE3(src2)) | (IM_LOOKUP(BYTE3(src1), BYTE3(src2)) << 16);

                dst+=8;
                src+=4;
            }

            dst += 160;
        }

    }
#undef DST
#undef SRC
}
*/

static void Flip_320x200x8_to_640x480x16_in_800x480_interpolated4(uint8_t *src, uint16_t *dst)
{
    register int x, counter2;
    int y, counter3;

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
            pixel.s.color = Get_pixel_value_interpolated4(&(src[index]), color_index) >> (shift); \
        } \
    }
#define INTERPOLATE(_a, _b, index) \
    { \
        INTERPOLATE_COLOR(r, 0, 3, _a, _b, index) \
        INTERPOLATE_COLOR(g, 1, 2, _a, _b, index) \
        INTERPOLATE_COLOR(b, 2, 3, _a, _b, index) \
    }

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    dst += 80;

    counter2 = 2;
    counter3 = 2;

    for (y = 200; y !=0; y--)
    {
        for (x = 320; x != 0; x-=32)
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
            DST[31] = DST[431] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

            dst+=64;
            src+=32;
        }

        dst += 960;

        counter2--;
        if (counter2 == 0)
        {
            register pixel_format_disp pixel;

            counter2 = counter3;
            counter3 = 5 - counter3;

            src-=320;

            // first column - use linear interpolation
            {
                DST[0] = IM_LOOKUP(src[0], src[320]) | (IM_LOOKUP(src[0], src[320]) << 16);
                src++;
                dst+=2;
            }

            pixel.pix = 0;

            for (x = 318; x != 0; x--)
            {
                INTERPOLATE(src[0], src[320], 0)

                DST[0] = pixel.pix | (pixel.pix << 16);

                src++;
                dst+=2;
            }

            // last column - use linear interpolation
            {
                DST[0] = IM_LOOKUP(src[0], src[320]) | (IM_LOOKUP(src[0], src[320]) << 16);
                src++;
                dst+=162;
            }
        }

    }
#undef DST
#undef SRC
#undef INTERPOLATE
#undef INTERPOLATE_COLOR
}

static void Flip_320x200x8_to_640x400x16_in_800x480(uint8_t *src, uint16_t *dst)
{
    register int y, x;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    dst += 80 + 40*800;

    for (y = 200; y !=0; y--)
    {
        for (x = 320; x != 0; x-=32)
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
            DST[31] = DST[431] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

            dst+=64;
            src+=32;
        }

        dst += 960;
    }
#undef DST
#undef SRC
}


void Init_Display(void)
{
    Display_FSType = 0;
    Font_Size_Shift = 2;
    DisplayMode = 1;
}

void Init_Display2(void)
{
    Init_Palette();

    memset(&(interpolation_matrix[0]), 0, sizeof(interpolation_matrix));

    Game_AdvancedScaling = 0;

    Display_Width = 800;
    Display_Height = 480;
    Display_Bitsperpixel = 16;
    Display_Fullscreen = 1;
    Display_MouseLocked = 0;
    //Picture_Width = 640;
    //Picture_Height = 400;
    //Picture_Position_UL_X = 80;
    //Picture_Position_UL_Y = 40;
    //Picture_Position_BR_X = 719;
    //Picture_Position_BR_Y = 439;
    //Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_320x200x8_to_640x400x16_in_800x480;

    Change_Display_Mode(0);
}

int Config_Display(char *str, char *param)
{
    if ( strcasecmp(str, "Display_Mode") == 0)	// str equals "Display_Mode"
    {
        if ( strcasecmp(param, "double_pixels") == 0)	// param equals "double_pixels"
        {
            // Double pixels - 640x400
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

void Reposition_Display(void)
{
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
            Render_Width = 640;
            Render_Height = 400;
            Picture_Width = 640;
            Picture_Height = 400;
            Picture_Position_UL_X = 80;
            Picture_Position_UL_Y = 40;
            Picture_Position_BR_X = 719;
            Picture_Position_BR_Y = 439;
            Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_320x200x8_to_640x400x16_in_800x480;

            ClearScreen = 1;
            break;
        case 1:
            Render_Width = 800;
            Render_Height = 480;
            Picture_Width = 800;
            Picture_Height = 480;
            Picture_Position_UL_X = 0;
            Picture_Position_UL_Y = 0;
            Picture_Position_BR_X = 799;
            Picture_Position_BR_Y = 479;
            Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_320x200x8_to_800x480x16_interpolated5;

            break;
        case 2:
            Render_Width = 640;
            Render_Height = 480;
            Picture_Width = 640;
            Picture_Height = 480;
            Picture_Position_UL_X = 80;
            Picture_Position_UL_Y = 0;
            Picture_Position_BR_X = 719;
            Picture_Position_BR_Y = 479;
            Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_320x200x8_to_640x480x16_in_800x480_interpolated4;

            ClearScreen = 1;
            break;
    }

    return ClearScreen;
}

