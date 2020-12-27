/**
 *
 *  Copyright (C) 2016-2020 Roman Pauer
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
#include <memory.h>
#include <string.h>

#if (SDL_MAJOR_VERSION > 1 || SDL_MAJOR_VERSION == 1 && (SDL_MINOR_VERSION > 2 || SDL_MINOR_VERSION == 2 && SDL_PATCHLEVEL >= 9 ) )
    #include <SDL/SDL_gp2x.h>
#endif

static uint32_t Game_DisplayStretched;	/* is display stretched to whole screen ? */
static int Game_VideoTVOut;				/* using TV out ? */
static int Game_UsingTouchscreen;			/* Playing on GP2X with touchscreen? */

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

static void Flip_320x200x8_to_320x200x16(uint8_t *src, uint16_t *dst)
{
    register int counter;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    for (counter = 320*200; counter != 0; counter-=32)
    {
        register uint32_t src1, src2;

        // 1st quad
        src1 = SRC[0];
        DST[0] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[1];
        DST[1] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[2] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[2];
        DST[3] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 2nd quad
        DST[4] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[3];
        DST[5] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[6] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[4];
        DST[7] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 3rd quad
        DST[8] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[5];
        DST[9] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[10] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[6];
        DST[11] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 4th quad
        DST[12] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[7];
        DST[13] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[14] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        DST[15] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        dst+=32;
        src+=32;
    }
#undef DST
#undef SRC
}

/*
static void Flip_320x200x8_to_320x240x16(uint8_t *src, uint16_t *dst)
{
    register int counter, counter2;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    counter2 = 3*(320 / 32);
    for (counter = 320*240; counter != 0; counter-=32)
    {
        register uint32_t src1, src2;

        // 1st quad
        src1 = SRC[0];
        DST[0] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[1];
        DST[1] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[2] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[2];
        DST[3] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 2nd quad
        DST[4] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[3];
        DST[5] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[6] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[4];
        DST[7] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 3rd quad
        DST[8] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[5];
        DST[9] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[10] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[6];
        DST[11] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 4th quad
        DST[12] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[7];
        DST[13] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[14] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        DST[15] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        dst+=32;
        src+=32;

        counter2--;
        if (counter2 == 0)
        {
            counter2 = 6*(320 / 32);
            src-=320;
        }
    }
#undef DST
#undef SRC
}
*/

/*
static void Flip_320x200x8_to_320x240x16_interpolated(uint8_t *src, uint16_t *dst)
{
    register int counter, counter2;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

#define interpolate_color(s1, s2) \
    { \
        register uint32_t color1, color2; \
        \
        color1 = Game_Palette_Or[s1].pix; \
        color2 = Game_Palette_Or[s2].pix; \
        \
        pixel.s.r = ((color1 & 0xff) + (color2 & 0xff)) >> (1 + 3 + 0); \
        pixel.s.g = ((color1 & 0xff00) + (color2 & 0xff00)) >> (1 + 2 + 8); \
        pixel.s.b = ((color1 & 0xff0000) + (color2 & 0xff0000)) >> (1 + 3 + 16); \
    }

    counter2 = 3*(320 / 32);
    for (counter = 320*200; counter != 0; counter-=32)
    {
        register uint32_t src1, src2;

        // 1st quad
        src1 = SRC[0];
        DST[0] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[1];
        DST[1] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[2] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[2];
        DST[3] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 2nd quad
        DST[4] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[3];
        DST[5] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[6] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[4];
        DST[7] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 3rd quad
        DST[8] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[5];
        DST[9] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[10] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[6];
        DST[11] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 4th quad
        DST[12] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[7];
        DST[13] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[14] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        DST[15] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        dst+=32;
        src+=32;

        counter2--;
        if (counter2 == 0)
        {
            register pixel_format_disp pixel;
            src-=320;

            pixel.pix = 0;


            for (counter2 = 320; counter2 != 0; counter2-=4)
            {
                src1 = SRC[0];
                src2 = SRC[320/4];

                interpolate_color(BYTE0(src1), BYTE0(src2))

                dst[0] = pixel.pix;

                interpolate_color(BYTE1(src1), BYTE1(src2))

                dst[1] = pixel.pix;

                interpolate_color(BYTE2(src1), BYTE2(src2))

                dst[2] = pixel.pix;

                interpolate_color(BYTE3(src1), BYTE3(src2))

                dst[3] = pixel.pix;

                dst+=4;
                src+=4;
            }

            counter2 = 5*(320 / 32);
        }
    }
#undef interpolate_color
#undef DST
#undef SRC
}
*/

/*
static void Flip_320x200x8_to_320x240x16_interpolated_lt(uint8_t *src, uint16_t *dst)
{
    register int counter, counter2;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    counter2 = 3*(320 / 32);
    for (counter = 320*200; counter != 0; counter-=32)
    {
        register uint32_t src1, src2;

        // 1st quad
        src1 = SRC[0];
        DST[0] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[1];
        DST[1] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[2] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[2];
        DST[3] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 2nd quad
        DST[4] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[3];
        DST[5] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[6] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[4];
        DST[7] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 3rd quad
        DST[8] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[5];
        DST[9] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[10] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[6];
        DST[11] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 4th quad
        DST[12] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[7];
        DST[13] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[14] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        DST[15] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        dst+=32;
        src+=32;

        counter2--;
        if (counter2 == 0)
        {
            src-=320;

            for (counter2 = 320; counter2 != 0; counter2-=4)
            {
                src1 = SRC[0];
                src2 = SRC[320/4];

                DST[0] = IM_LOOKUP(BYTE0(src1), BYTE0(src2)) | (IM_LOOKUP(BYTE1(src1), BYTE1(src2)) << 16);
                DST[1] = IM_LOOKUP(BYTE2(src1), BYTE2(src2)) | (IM_LOOKUP(BYTE3(src1), BYTE3(src2)) << 16);

                dst+=4;
                src+=4;
            }

            counter2 = 5*(320 / 32);
        }
    }
#undef DST
#undef SRC
}
*/

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
        case 0x02:
        case 0x06:
        case 0x26:
        case 0x03:
        case 0x07:
        case 0x27:
        case 0x0b:
        case 0x0f:
        case 0x2f:
            return color1;
        case 0x10:
        case 0x30:
        case 0x34:
        case 0x18:
        case 0x38:
        case 0x3c:
        case 0x19:
        case 0x39:
        case 0x3d:
            return color2;
        default:
            return (color1 + color2) / 2;
    }
}

static void Flip_320x200x8_to_320x240x16_interpolated4(uint8_t *src, uint16_t *dst)
{
    register int counter, counter2;

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

    counter2 = 3*(320 / 32);
    for (counter = 320*200; counter != 0; counter-=32)
    {
        register uint32_t src1, src2;

        // 1st quad
        src1 = SRC[0];
        DST[0] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[1];
        DST[1] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[2] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[2];
        DST[3] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 2nd quad
        DST[4] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[3];
        DST[5] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[6] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[4];
        DST[7] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 3rd quad
        DST[8] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[5];
        DST[9] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[10] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        src1 = SRC[6];
        DST[11] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        // 4th quad
        DST[12] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[7];
        DST[13] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        DST[14] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        DST[15] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);

        dst+=32;
        src+=32;

        counter2--;
        if (counter2 == 0)
        {
            register pixel_format_disp pixel;

            src-=320;

            // first column - use linear interpolation
            {
                dst[0] = IM_LOOKUP(src[0], src[320]);
                src++;
                dst++;
            }

            pixel.pix = 0;

            for (counter2 = 318; counter2 != 0; counter2--)
            {
                INTERPOLATE(src[0], src[320], 0)

                dst[0] = pixel.pix;

                src++;
                dst++;
            }

            // last column - use linear interpolation
            {
                dst[0] = IM_LOOKUP(src[0], src[320]);
                src++;
                dst++;
            }

            counter2 = 5*(320 / 32);
        }
    }
#undef DST
#undef SRC
#undef INTERPOLATE
#undef INTERPOLATE_COLOR
}


void Init_Display(void)
{
    Game_DisplayStretched = 1;
    Display_FSType = 0;
    Font_Size_Shift = 1;

    Game_UsingTouchscreen = 0;

    Game_Delay_Game = -1;
}

void Init_Display2(void)
{
    Init_Palette();

    //senquack
    // Some final tidying up of any mutually-exclusive options:
    if ( Game_UsingTouchscreen )
    {
        Game_DisplayStretched = 1;
    }

    memset(&(interpolation_matrix[0]), 0, sizeof(interpolation_matrix));

#if defined(SDL_GP2X__H)
    {
            SDL_Rect size;

            SDL_GP2X_GetPhysicalScreenSize(&size);

            Game_VideoTVOut = (size.w == 320)?0:1;
    }
#else
    Game_VideoTVOut = 0;
#endif

    if (Game_DisplayStretched && Game_VideoTVOut)
    {
        Game_DisplayStretched = 0;
    }

    Display_Width = 320;
    Display_Bitsperpixel = 16;
    Display_Fullscreen = 0;
    Display_MouseLocked = 1;
    Render_Width = 320;
    Picture_Width = 320;
    Picture_Position_UL_X = 0;
    Picture_Position_UL_Y = 0;
    Picture_Position_BR_X = 319;

    if (Game_DisplayStretched)
    {
        Display_Height = 240;
        Render_Height = 240;
        Picture_Height = 240;
        Picture_Position_BR_Y = 239;
        Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_320x200x8_to_320x240x16_interpolated4;
    }
    else
    {
        Display_Height = 200;
        Render_Height = 200;
        Picture_Height = 200;
        Picture_Position_BR_Y = 199;
        Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_320x200x8_to_320x200x16;

        if (!Game_VideoTVOut)
        {
            Game_Screen = SDL_SetVideoMode (320, 240, 16, SDL_SWSURFACE);

            if (Game_Screen != NULL)
            {
                SDL_Rect rect;

                // clear screen
                rect.x = 0;
                rect.y = 0;
                rect.w = Game_Screen->w;
                rect.h = Game_Screen->h;
                SDL_FillRect(Game_Screen, &rect, 0);
                SDL_Flip(Game_Screen);
            }
        }
    }

}

int Config_Display(char *str, char *param)
{
    if ( strcasecmp(str, "Touchscreen") == 0)	// str equals "Touchscreen"
    {
        if ( strcasecmp(param, "on") == 0)	// param equals "on"
        {
            // Use GP2X touchscreen if any is available
            Game_UsingTouchscreen = 1;
        }
        else if ( strcasecmp(param, "off") == 0) // param equals "off"
        {
            // Don't use GP2X touchscreen if any is available
//				Game_UsingTouchscreen = 0;	/* Default, Don't assign here. Just for clarification.
        }
    }
    else if ( strcasecmp(str, "Display") == 0 ) // str equals "Display"
    {
        if ( strcasecmp(param, "original") == 0 ) // param equals "original"
        {
            // original resolution

            Game_DisplayStretched = 0;

        }
        else if ( strcasecmp(param, "stretched") == 0 ) // param equals "stretched"
        {
            // display stretched to whole screen

            Game_DisplayStretched = 1;

        }

        return 1;
    }

    return 0;
}

void Reposition_Display(void)
{
    #if defined(SDL_GP2X__H)
        if ( (!Game_DisplayStretched) && (!Game_VideoTVOut) )
        {
            SDL_GP2X_MiniDisplay(0, 20);
        }
    #endif
}

void Cleanup_Display(void)
{
}

int Change_Display_Mode(int direction)
{
    return 0;
}

