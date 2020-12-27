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

static int Game_TvoutUnscaled;           /* Before, when using Tvout, albion would try to use the full
                                            360-wide resolution provided.  This doesn't work on my TV
                                            (and probably not for a lot out there) and just causes a large
                                            portion of the screen to not be seen (even larger than is
                                            normal for tv-out).  We now have this as optional behavior.
                                            The default is to scale the image to a 320-wide
                                            screen just as is done for the LCD of the GP2X.  */

static int DisplayMode;

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

static void Flip_360x240x8_to_360x240x16(uint8_t *src, uint16_t *dst)
{
    register int counter;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    for (counter = 360*240; counter != 0; counter-=32)
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
static void Flip_360x240x8_to_320x240x16(uint8_t *src, uint16_t *dst)
{
    register int counter;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    for (counter = 320*240; counter != 0; counter-=32)
    {
        register uint32_t src1, src2;

        // 1st quad
        src1 = SRC[0];
        DST[0] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[1];
        DST[1] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        src1 = SRC[2];
        DST[2] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
        DST[3] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);

        // 2nd quad
        src2 = SRC[3];
        DST[4] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
        DST[5] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
        src1 = SRC[4];
        DST[6] = Game_Palette[BYTE2(src2)].pix | (Game_Palette[BYTE3(src2)].pix << 16);
        DST[7] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);

        // 3rd quad
        src2 = SRC[5];
        DST[8] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        src1 = SRC[6];
        DST[9] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);
        DST[10] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);
        src2 = SRC[7];
        DST[11] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);

        // 4th quad
        DST[12] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
        src1 = SRC[8];
        DST[13] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);
        DST[14] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        DST[15] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

        dst+=32;
        src+=36;
    }
#undef DST
#undef SRC
}
*/

/*
static void Flip_360x240x8_to_320x240x16_interpolated2_old(uint8_t *src, uint16_t *dst)
{
    register int counter;
    register pixel_format_disp pixel;

    const static uint32_t maxdiff = 128;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

#define interpolate_color(s1, s2, c, shift, shift2) \
    { \
        register uint32_t min, max; \
        \
        max = ( (s1) >= (s2) )?( s1 ):( s2 ); \
        min = ( (s1) >= (s2) )?( s2 ):( s1 ); \
        \
        pixel.s.c = ( (max - min > (maxdiff << shift2))?( max ):( (max + min) / 2 ) ) >> (shift + shift2); \
    }

    pixel.pix = 0;

    for (counter = 320*240; counter != 0; counter-=32)
    {
        register uint32_t src1, src2;

        // 1st quad
        src1 = SRC[0];
        DST[0] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[1];
        DST[1] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

        {
            register uint32_t color1, color2;

            color1 = Game_Palette_Or[BYTE0(src2)].pix;
            color2 = Game_Palette_Or[BYTE1(src2)].pix;

            interpolate_color(color1 & 0xff, color2 & 0xff, r, 3, 0)
            interpolate_color(color1 & 0xff00, color2 & 0xff00, g, 2, 8)
            interpolate_color(color1 & 0xff0000, color2 & 0xff0000, b, 3, 16)
        }

        src1 = SRC[2];
        DST[2] = pixel.pix | (Game_Palette[BYTE2(src2)].pix << 16);
        DST[3] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);

        // 2nd quad
        src2 = SRC[3];
        DST[4] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
        DST[5] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);

        {
            register uint32_t color1, color2;

            color1 = Game_Palette_Or[BYTE1(src2)].pix;
            color2 = Game_Palette_Or[BYTE2(src2)].pix;

            interpolate_color(color1 & 0xff, color2 & 0xff, r, 3, 0)
            interpolate_color(color1 & 0xff00, color2 & 0xff00, g, 2, 8)
            interpolate_color(color1 & 0xff0000, color2 & 0xff0000, b, 3, 16)
        }

        src1 = SRC[4];
        DST[6] = pixel.pix | (Game_Palette[BYTE3(src2)].pix << 16);
        DST[7] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);

        // 3rd quad
        src2 = SRC[5];
        DST[8] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        src1 = SRC[6];
        DST[9] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);

        {
            register uint32_t color1, color2;

            color1 = Game_Palette_Or[BYTE2(src2)].pix;
            color2 = Game_Palette_Or[BYTE3(src2)].pix;

            interpolate_color(color1 & 0xff, color2 & 0xff, r, 3, 0)
            interpolate_color(color1 & 0xff00, color2 & 0xff00, g, 2, 8)
            interpolate_color(color1 & 0xff0000, color2 & 0xff0000, b, 3, 16)
        }

        DST[10] = pixel.pix | (Game_Palette[BYTE0(src1)].pix << 16);
        src2 = SRC[7];
        DST[11] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);

        // 4th quad
        DST[12] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
        src1 = SRC[8];
        DST[13] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);

        {
            register uint32_t color1, color2;

            color1 = Game_Palette_Or[BYTE3(src2)].pix;
            color2 = Game_Palette_Or[BYTE0(src1)].pix;

            interpolate_color(color1 & 0xff, color2 & 0xff, r, 3, 0)
            interpolate_color(color1 & 0xff00, color2 & 0xff00, g, 2, 8)
            interpolate_color(color1 & 0xff0000, color2 & 0xff0000, b, 3, 16)
        }

        DST[14] = pixel.pix | (Game_Palette[BYTE1(src1)].pix << 16);
        DST[15] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

        dst+=32;
        src+=36;
    }

#undef interpolate_color
#undef DST
#undef SRC
}
*/

static void Flip_360x240x8_to_320x240x16_interpolated2_lt(uint8_t *src, uint16_t *dst)
{
    register int counter;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    for (counter = 320*240; counter != 0; counter-=32)
    {
        register uint32_t src1, src2;

        // 1st quad
        src1 = SRC[0];
        DST[0] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);
        src2 = SRC[1];
        DST[1] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

        src1 = SRC[2];
        DST[2] = IM2_LOOKUP(BYTE0(src2), BYTE1(src2)) | (Game_Palette[BYTE2(src2)].pix << 16);
        DST[3] = Game_Palette[BYTE3(src2)].pix | (Game_Palette[BYTE0(src1)].pix << 16);

        // 2nd quad
        src2 = SRC[3];
        DST[4] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);
        DST[5] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);

        src1 = SRC[4];
        DST[6] = IM2_LOOKUP(BYTE1(src2), BYTE2(src2)) | (Game_Palette[BYTE3(src2)].pix << 16);
        DST[7] = Game_Palette[BYTE0(src1)].pix | (Game_Palette[BYTE1(src1)].pix << 16);

        // 3rd quad
        src2 = SRC[5];
        DST[8] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);
        src1 = SRC[6];
        DST[9] = Game_Palette[BYTE0(src2)].pix | (Game_Palette[BYTE1(src2)].pix << 16);

        DST[10] = IM2_LOOKUP(BYTE2(src2), BYTE3(src2)) | (Game_Palette[BYTE0(src1)].pix << 16);
        src2 = SRC[7];
        DST[11] = Game_Palette[BYTE1(src1)].pix | (Game_Palette[BYTE2(src1)].pix << 16);

        // 4th quad
        DST[12] = Game_Palette[BYTE3(src1)].pix | (Game_Palette[BYTE0(src2)].pix << 16);
        src1 = SRC[8];
        DST[13] = Game_Palette[BYTE1(src2)].pix | (Game_Palette[BYTE2(src2)].pix << 16);

        DST[14] = IM2_LOOKUP(BYTE3(src2), BYTE0(src1)) | (Game_Palette[BYTE1(src1)].pix << 16);
        DST[15] = Game_Palette[BYTE2(src1)].pix | (Game_Palette[BYTE3(src1)].pix << 16);

        dst+=32;
        src+=36;
    }

#undef DST
#undef SRC
}


// senquack - This blits the middle 320 pixels of the 360-pixel-wide framebuffer with
// 				no scaling at all.  It is meant to be turned on when the player has
// 				a long text dialog.. they are centered in the middle anyway so it fits
// 				quite nicely unscaled and is much easier on the eyes.  At 200mhz it takes
// 				less than 3.9ms  (interpolated asm is 4.5ms, so optimization is unnecessary)
/*void Game_Flip_320x240x16_unscaled(uint8_t *src, uint16_t *dst)
{
     int y, x;

     src += 20;

     for (y = 240; y > 0; y--)
     {
          for (x = 20; x > 0; x--)
          {
                // 16 pixels * 20 = 320
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
                *dst = (uint16_t)Game_Palette[*src].pix; dst++; src++;
          }
          src += 40;    // Done with one line, skip past the last 20 of the current line and
                        //	first 20 of the next line..
     }
}*/

static void Flip_360x240x8_to_320x240x16_unscaled(uint8_t *src, uint16_t *dst)
{
    register int y, x;

#define SRC ((uint32_t *) src)
#define DST ((uint32_t *) dst)

    src += 20;

    for (y = 240; y !=0; y--)
    {
        for (x = 320; x != 0; x-=32)
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
        src += 40;
    }
#undef DST
#undef SRC
}


void Init_Display(void)
{
    Game_TvoutUnscaled = 0;
    Display_FSType = 0;
    Font_Size_Shift = 1;
    Game_UseEnhanced3DEngineNewValue = 0;
}

void Init_Display2(void)
{
    Init_Palette();

    memset(&(interpolation_matrix2[0]), 0, sizeof(interpolation_matrix2));

    DisplayMode = 0;
    Display_Width = 320;
    Display_Height = 240;
    Display_Bitsperpixel = 16;
    Display_Fullscreen = 0;
    Display_MouseLocked = 1;
    Render_Width = 320;
    Render_Height = 240;
    Picture_Width = 320;
    Picture_Height = 240;
    Picture_Position_UL_X = 0;
    Picture_Position_UL_Y = 0;
    Picture_Position_BR_X = 319;
    Picture_Position_BR_Y = 239;
    Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_320x240x16_interpolated2_lt;

    if (Game_TvoutUnscaled)
    {
        #if defined(SDL_GP2X__H)
            SDL_Rect size;

            SDL_GP2X_GetPhysicalScreenSize(&size);

            if (size.w == 320)
            {
                Game_TvoutUnscaled = 0;
            }
        #else
            Game_TvoutUnscaled = 0;
        #endif
    }

    if (Game_TvoutUnscaled) // unscaled TV out
    {
        Display_Width = 360;
        Render_Width = 360;
        Picture_Width = 360;
        Picture_Position_BR_X = 359;
        Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_360x240x16;
    }
}

int Config_Display(char *str, char *param)
{
    if ( strcasecmp(str, "Tvout_Scaling") == 0)	// str equals "Tvout_Scaling"
    {
        if ( strcasecmp(param, "on") == 0)	// param equals "on
        {
            // Tv out will be shown with interpolated scaling, like the LCD of the GP2X gets
            // (default behavior)
            Game_TvoutUnscaled = 0;
        }
        else if ( strcasecmp(param, "off") == 0) // param equals "off"
        {
            // Full 360-width framebuffer is shown on tvout (a lot of the image is
            // lost on senquack's TV this way, even more than is normally seen with
            // tv out under SDL.  Using the previous option and Open2X's new TV
            // Daemon (written by moi) gets a perfect image on my NTSC TV.
            Game_TvoutUnscaled = 1;
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

    if (!(Game_TvoutUnscaled)) // not unscaled TV out
    {
        DisplayMode = (DisplayMode + direction + 2) % 2;

        switch (DisplayMode)
        {
            case 0:
                Render_Width = 320;
                Picture_Width = 320;
                Picture_Position_UL_X = 0;
                Picture_Position_BR_X = 319;
                Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_320x240x16_interpolated2_lt;
                break;
            case 1:
                Render_Width = 360;
                Picture_Width = 360;
                Picture_Position_UL_X = -20;
                Picture_Position_BR_X = 339;
                Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_320x240x16_unscaled;
                break;
        }
    }

    return ClearScreen;
}

