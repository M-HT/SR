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
#include "palette32bgra.h"
#include "overlay.h"
#include <memory.h>

static int DisplayMode;

static int ScaleOutput, ScaledWidth, ScaledHeight, Fullscreen;
static uint32_t *ScaleSrc;

#if defined(USE_SDL2) || defined(ALLOW_OPENGL)
static pixel_format_disp Game_PaletteAlpha[256];
#endif
static uint32_t interpolation_matrix2[256*256];
#define IM2_LOOKUP(a, b) ( interpolation_matrix2[(((uint32_t) (a)) << 8) + ((uint32_t) (b))] )

static void Set_Palette_Value2(uint32_t index, uint32_t r, uint32_t g, uint32_t b)
{
    uint32_t *val1, *val2;
    pixel_format_disp pixel;

#if defined(USE_SDL2) || defined(ALLOW_OPENGL)
    Game_PaletteAlpha[index].s.r = r;
    Game_PaletteAlpha[index].s.g = g;
    Game_PaletteAlpha[index].s.b = b;
    Game_PaletteAlpha[index].s.a = 255;
#endif

#define MAXDIFF 128
#define INTERPOLATE(a, b) (((a) >= (b)) ? ( ((a) - (b) >= MAXDIFF) ? ((3 * (a) + (b)) / 4) : (((a) + (b)) / 2) ) : ( ((b) - (a) >= MAXDIFF) ? ((3 * (b) + (a)) / 4) : (((a) + (b)) / 2) ) )

    val1 = &(interpolation_matrix2[index * 256]);
    val2 = &(interpolation_matrix2[index]);

    pixel.pix = 0;

    for (index = 0; index < 256; index++)
    {
        pixel.s.r = INTERPOLATE(r, (uint32_t) Game_Palette_Or[index].s.r);
        pixel.s.g = INTERPOLATE(g, (uint32_t) Game_Palette_Or[index].s.g);
        pixel.s.b = INTERPOLATE(b, (uint32_t) Game_Palette_Or[index].s.b);

        *val1 = *val2 = pixel.pix;

        val1++;
        val2 += 256;
    }

#undef INTERPOLATE
#undef MAXDIFF
}

#if defined(USE_SDL2) || defined(ALLOW_OPENGL)
static void Flip_360x240x8_to_360x240x32_advanced(uint8_t *src, uint32_t *dst1, uint32_t *dst2, int *dst2_used)
{
    int counter, DrawOverlay;

    DrawOverlay = Get_DrawOverlay(src, 1);
    *dst2_used = DrawOverlay;

    if (DrawOverlay)
    {
        uint8_t *zalsrc, *src2, *orig;
        uint32_t *zaldst1, *zaldst2;
        int x, y, ViewportX2;

        // display part above the viewport
        if (Game_OverlayDisplay.ViewportY != 0)
        {
            for (counter = 360 * Game_OverlayDisplay.ViewportY; counter != 0; counter -= 8)
            {
                dst1[0] = Game_PaletteAlpha[src[0]].pix;
                dst1[1] = Game_PaletteAlpha[src[1]].pix;
                dst1[2] = Game_PaletteAlpha[src[2]].pix;
                dst1[3] = Game_PaletteAlpha[src[3]].pix;
                dst1[4] = Game_PaletteAlpha[src[4]].pix;
                dst1[5] = Game_PaletteAlpha[src[5]].pix;
                dst1[6] = Game_PaletteAlpha[src[6]].pix;
                dst1[7] = Game_PaletteAlpha[src[7]].pix;

                src += 8;
                dst1 += 8;
            }

            for (counter = Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * Game_OverlayDisplay.ViewportY; counter != 0; counter -= 8)
            {
                dst2[0] = 0;
                dst2[1] = 0;
                dst2[2] = 0;
                dst2[3] = 0;
                dst2[4] = 0;
                dst2[5] = 0;
                dst2[6] = 0;
                dst2[7] = 0;

                dst2 += 8;
            }
        }

        zalsrc = src;
        zaldst1 = dst1;
        zaldst2 = dst2;

        // display part left of the viewport
        if (Game_OverlayDisplay.ViewportX != 0)
        {
            for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                for (x = Game_OverlayDisplay.ViewportX; x >= 8; x -= 8)
                {
                    dst1[0] = Game_PaletteAlpha[src[0]].pix;
                    dst1[1] = Game_PaletteAlpha[src[1]].pix;
                    dst1[2] = Game_PaletteAlpha[src[2]].pix;
                    dst1[3] = Game_PaletteAlpha[src[3]].pix;
                    dst1[4] = Game_PaletteAlpha[src[4]].pix;
                    dst1[5] = Game_PaletteAlpha[src[5]].pix;
                    dst1[6] = Game_PaletteAlpha[src[6]].pix;
                    dst1[7] = Game_PaletteAlpha[src[7]].pix;

                    src += 8;
                    dst1 += 8;
                }

                for (; x != 0; x--)
                {
                    dst1[0] = Game_PaletteAlpha[src[0]].pix;

                    src++;
                    dst1++;
                }

                src += (360 - Game_OverlayDisplay.ViewportX);
                dst1 += (360 - Game_OverlayDisplay.ViewportX);
            }

            for (y = Scaler_ScaleFactor * Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                for (x = Scaler_ScaleFactor * Game_OverlayDisplay.ViewportX; x >= 8; x -= 8)
                {
                    dst2[0] = 0;
                    dst2[1] = 0;
                    dst2[2] = 0;
                    dst2[3] = 0;
                    dst2[4] = 0;
                    dst2[5] = 0;
                    dst2[6] = 0;
                    dst2[7] = 0;

                    dst2 += 8;
                }

                for (; x != 0; x--)
                {
                    dst2[0] = 0;

                    dst2++;
                }

                dst2 += Scaler_ScaleFactor * (360 - Game_OverlayDisplay.ViewportX);
            }
        }

        // display part right of the viewport
        if ((Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth) != 360)
        {
            src = zalsrc + (Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth);
            dst1 = zaldst1 + (Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth);
            dst2 = zaldst2 + Scaler_ScaleFactor * (Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth);
            ViewportX2 = 360 - (Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth);

            for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                for (x = ViewportX2; x >= 8; x -= 8)
                {
                    dst1[0] = Game_PaletteAlpha[src[0]].pix;
                    dst1[1] = Game_PaletteAlpha[src[1]].pix;
                    dst1[2] = Game_PaletteAlpha[src[2]].pix;
                    dst1[3] = Game_PaletteAlpha[src[3]].pix;
                    dst1[4] = Game_PaletteAlpha[src[4]].pix;
                    dst1[5] = Game_PaletteAlpha[src[5]].pix;
                    dst1[6] = Game_PaletteAlpha[src[6]].pix;
                    dst1[7] = Game_PaletteAlpha[src[7]].pix;

                    src += 8;
                    dst1 += 8;
                }

                for (; x != 0; x--)
                {
                    dst1[0] = Game_PaletteAlpha[src[0]].pix;

                    src++;
                    dst1++;
                }

                src += (360 - ViewportX2);
                dst1 += (360 - ViewportX2);
            }

            for (y = Scaler_ScaleFactor * Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                for (x = Scaler_ScaleFactor * ViewportX2; x >= 8; x -= 8)
                {
                    dst2[0] = 0;
                    dst2[1] = 0;
                    dst2[2] = 0;
                    dst2[3] = 0;
                    dst2[4] = 0;
                    dst2[5] = 0;
                    dst2[6] = 0;
                    dst2[7] = 0;

                    dst2 += 8;
                }

                for (; x != 0; x--)
                {
                    dst2[0] = 0;

                    dst2++;
                }

                dst2 += Scaler_ScaleFactor * (360 - ViewportX2);
            }
        }

        // display part below the viewport
        src = zalsrc + (360 * Game_OverlayDisplay.ViewportHeight);
        dst1 = zaldst1 + (360 * Game_OverlayDisplay.ViewportHeight);
        dst2 = zaldst2 + Scaler_ScaleFactor * Scaler_ScaleFactor * (360 * Game_OverlayDisplay.ViewportHeight);

        for (counter = 360 * (240 - (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight)); counter != 0; counter -= 8)
        {
            dst1[0] = Game_PaletteAlpha[src[0]].pix;
            dst1[1] = Game_PaletteAlpha[src[1]].pix;
            dst1[2] = Game_PaletteAlpha[src[2]].pix;
            dst1[3] = Game_PaletteAlpha[src[3]].pix;
            dst1[4] = Game_PaletteAlpha[src[4]].pix;
            dst1[5] = Game_PaletteAlpha[src[5]].pix;
            dst1[6] = Game_PaletteAlpha[src[6]].pix;
            dst1[7] = Game_PaletteAlpha[src[7]].pix;

            src += 8;
            dst1 += 8;
        }

        for (counter = Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (240 - (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight)); counter != 0; counter -= 8)
        {
            dst2[0] = 0;
            dst2[1] = 0;
            dst2[2] = 0;
            dst2[3] = 0;
            dst2[4] = 0;
            dst2[5] = 0;
            dst2[6] = 0;
            dst2[7] = 0;

            dst2 += 8;
        }

        // the viewport
        src = zalsrc + Game_OverlayDisplay.ViewportX;
        dst1 = zaldst1 + Game_OverlayDisplay.ViewportX;
        orig = Game_OverlayDisplay.ScreenViewpartOriginal + 360 * Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportX;

        for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
        {
            for (x = Game_OverlayDisplay.ViewportWidth; x >= 8; x -= 8)
            {
                dst1[0] = (src[0] == orig[0])?0:Game_PaletteAlpha[src[0]].pix;
                dst1[1] = (src[1] == orig[1])?0:Game_PaletteAlpha[src[1]].pix;
                dst1[2] = (src[2] == orig[2])?0:Game_PaletteAlpha[src[2]].pix;
                dst1[3] = (src[3] == orig[3])?0:Game_PaletteAlpha[src[3]].pix;
                dst1[4] = (src[4] == orig[4])?0:Game_PaletteAlpha[src[4]].pix;
                dst1[5] = (src[5] == orig[5])?0:Game_PaletteAlpha[src[5]].pix;
                dst1[6] = (src[6] == orig[6])?0:Game_PaletteAlpha[src[6]].pix;
                dst1[7] = (src[7] == orig[7])?0:Game_PaletteAlpha[src[7]].pix;

                src += 8;
                orig += 8;
                dst1 += 8;
            }

            for (; x != 0; x--)
            {
                dst1[0] = (src[0] == orig[0])?0:Game_PaletteAlpha[src[0]].pix;

                src++;
                orig++;
                dst1++;
            }

            src += (360 - Game_OverlayDisplay.ViewportWidth);
            orig += (360 - Game_OverlayDisplay.ViewportWidth);
            dst1 += (360 - Game_OverlayDisplay.ViewportWidth);
        }

        dst1 = zaldst1 + Game_OverlayDisplay.ViewportX;
        dst2 = zaldst2 + Scaler_ScaleFactor * Game_OverlayDisplay.ViewportX;
        src2 = Game_OverlayDisplay.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * Game_OverlayDisplay.ViewportY + Scaler_ScaleFactor * Game_OverlayDisplay.ViewportX;

        counter = Scaler_ScaleFactor;
        for (y = Scaler_ScaleFactor * Game_OverlayDisplay.ViewportHeight; y != 0; y--)
        {
            int counter2, same;

            counter2 = Scaler_ScaleFactor;
            same = (dst1[0] == 0)?1:0;

            for (x = Scaler_ScaleFactor * Game_OverlayDisplay.ViewportWidth; x != 0; x--)
            {
                dst2[0] = (same)?Game_PaletteAlpha[src2[0]].pix:0;

                src2++;
                dst2++;

                counter2--;
                if (counter2 == 0)
                {
                    counter2 = Scaler_ScaleFactor;
                    dst1++;
                    same = (dst1[0] == 0)?1:0;
                }
            }

            src2 += Scaler_ScaleFactor * (360 - Game_OverlayDisplay.ViewportWidth);
            dst2 += Scaler_ScaleFactor * (360 - Game_OverlayDisplay.ViewportWidth);

            counter--;
            if (counter == 0)
            {
                counter = Scaler_ScaleFactor;
                dst1 += (360 - Game_OverlayDisplay.ViewportWidth);
            }
            else
            {
                dst1 -= Game_OverlayDisplay.ViewportWidth;
            }
        }

        // markers
        if (DrawOverlay & 1)
        {
            dst1 = zaldst1 + 360 * (Game_OverlayDisplay.ViewportHeight - 2) + (Game_OverlayDisplay.ViewportX + 1);

            for (x = 8; x != 0; x--)
            {
                dst1[0] = 0;
                dst1++;
            }

            dst2 = zaldst2 + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (Game_OverlayDisplay.ViewportHeight - 2) + Scaler_ScaleFactor * (Game_OverlayDisplay.ViewportX + 1);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2) + Scaler_ScaleFactor * (Game_OverlayDisplay.ViewportX + 1);

            for (y = Scaler_ScaleFactor; y != 0; y--)
            {
                for (x = Scaler_ScaleFactor * 8; x != 0; x--)
                {
                    dst2[0] = Game_PaletteAlpha[src2[0]].pix;
                    src2++;
                    dst2++;
                }

                src2 += Scaler_ScaleFactor * 352;
                dst2 += Scaler_ScaleFactor * 352;
            }
        }

        if (DrawOverlay & 2)
        {
            dst1 = zaldst1 + 360 * (Game_OverlayDisplay.ViewportHeight - 2) + (Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth - 10);

            for (x = 8; x != 0; x--)
            {
                dst1[0] = 0;
                dst1++;
            }

            dst2 = zaldst2 + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (Game_OverlayDisplay.ViewportHeight - 2) + Scaler_ScaleFactor * (Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth - 10);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2) + Scaler_ScaleFactor * (Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth - 10);

            for (y = Scaler_ScaleFactor; y != 0; y--)
            {
                for (x = Scaler_ScaleFactor * 8; x != 0; x--)
                {
                    dst2[0] = Game_PaletteAlpha[src2[0]].pix;
                    src2++;
                    dst2++;
                }

                src2 += Scaler_ScaleFactor * 352;
                dst2 += Scaler_ScaleFactor * 352;
            }
        }
    }
    else
    {
        for (counter = 360*240; counter != 0; counter -= 8)
        {
            dst1[0] = Game_Palette[src[0]].pix;
            dst1[1] = Game_Palette[src[1]].pix;
            dst1[2] = Game_Palette[src[2]].pix;
            dst1[3] = Game_Palette[src[3]].pix;
            dst1[4] = Game_Palette[src[4]].pix;
            dst1[5] = Game_Palette[src[5]].pix;
            dst1[6] = Game_Palette[src[6]].pix;
            dst1[7] = Game_Palette[src[7]].pix;

            src += 8;
            dst1 += 8;
        }
    }
}
#endif

static void Flip_360x240x8_to_720x480x32(uint8_t *src, uint32_t *dst)
{
    int x, y, DrawOverlay, ViewportX2;

#define WRITE_PIXEL2(_x) dst[2 * (_x)] = dst[1 + (2 * (_x))] = dst[720 + (2 * (_x))] = dst[721 + (2 * (_x))] = Game_Palette[src[(_x)]].pix;

    DrawOverlay = Get_DrawOverlay(src, 1);

    if (DrawOverlay)
    {
        uint8_t *zalsrc, *src2, *orig;
        uint32_t *zaldst;

        // display part above the viewport
        for (y = Game_OverlayDisplay.ViewportY; y != 0; y--)
        {
            for (x = 360; x != 0; x-=9)
            {
                WRITE_PIXEL2(0)
                WRITE_PIXEL2(1)
                WRITE_PIXEL2(2)
                WRITE_PIXEL2(3)
                WRITE_PIXEL2(4)
                WRITE_PIXEL2(5)
                WRITE_PIXEL2(6)
                WRITE_PIXEL2(7)
                WRITE_PIXEL2(8)

                src+=9;
                dst+=18;
            }
            dst+=720;
        }

        zalsrc = src;
        zaldst = dst;

        // display part left of the viewport
        if (Game_OverlayDisplay.ViewportX != 0)
        {
            for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                for (x = Game_OverlayDisplay.ViewportX; x != 0; x--)
                {
                    WRITE_PIXEL2(0)

                    src++;
                    dst+=2;
                }
                src+=(360-Game_OverlayDisplay.ViewportX);
                dst+=720+(720-2*Game_OverlayDisplay.ViewportX);
            }
        }

        // display part right of the viewport
        if ((Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth) != 360)
        {
            src = zalsrc + Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth;
            dst = zaldst + 2 * (Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth);
            ViewportX2 = 360 - (Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth);
            for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
            {
                for (x = ViewportX2; x != 0; x--)
                {
                    WRITE_PIXEL2(0)

                    src++;
                    dst+=2;
                }
                src+=(360-ViewportX2);
                dst+=720+(720-2*ViewportX2);
            }
        }

        // display part below the viewport
        src = zalsrc + 360 * Game_OverlayDisplay.ViewportHeight;
        dst = zaldst + 720 * 2 * Game_OverlayDisplay.ViewportHeight;
        for (y = 240 - (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight); y != 0; y--)
        {
            for (x = 360; x != 0; x-=9)
            {
                WRITE_PIXEL2(0)
                WRITE_PIXEL2(1)
                WRITE_PIXEL2(2)
                WRITE_PIXEL2(3)
                WRITE_PIXEL2(4)
                WRITE_PIXEL2(5)
                WRITE_PIXEL2(6)
                WRITE_PIXEL2(7)
                WRITE_PIXEL2(8)

                src+=9;
                dst+=18;
            }
            dst+=720;
        }

        // the viewport
        src = zalsrc + Game_OverlayDisplay.ViewportX;
        orig = Game_OverlayDisplay.ScreenViewpartOriginal + 360 * Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportX;
        dst = zaldst + 2 * Game_OverlayDisplay.ViewportX;
        src2 = Game_OverlayDisplay.ScreenViewpartOverlay + Game_OverlayDisplay.ViewportY*2 * 800 + Game_OverlayDisplay.ViewportX*2;
        for (y = Game_OverlayDisplay.ViewportHeight; y != 0; y--)
        {
            for (x = Game_OverlayDisplay.ViewportWidth; x != 0; x--)
            {
                if (*src == *orig)
                {
                    dst[0] = Game_Palette[src2[0]].pix;
                    dst[1] = Game_Palette[src2[1]].pix;
                    dst[720] = Game_Palette[src2[800]].pix;
                    dst[721] = Game_Palette[src2[801]].pix;
                }
                else
                {
                    WRITE_PIXEL2(0)
                }

                src++;
                orig++;
                src2+=2;
                dst+=2;
            }
            src+=(360 - Game_OverlayDisplay.ViewportWidth);
            orig+=(360 - Game_OverlayDisplay.ViewportWidth);
            src2+=800 + (800 - 2*Game_OverlayDisplay.ViewportWidth);
            dst+=720 + (720 - 2 * Game_OverlayDisplay.ViewportWidth);
        }

        if (DrawOverlay & 1)
        {
            dst = zaldst + 720 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + 2 * (Game_OverlayDisplay.ViewportX + 1);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + (Game_OverlayDisplay.ViewportX + 1)*2;
            for (x = 8; x != 0; x--)
            {
                dst[0] = Game_Palette[src2[0]].pix;
                dst[1] = Game_Palette[src2[1]].pix;
                dst[720] = Game_Palette[src2[800]].pix;
                dst[721] = Game_Palette[src2[801]].pix;

                src2+=2;
                dst+=2;
            }
        }

        if (DrawOverlay & 2)
        {
            dst = zaldst + 720 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + 2 * Game_OverlayDisplay.ViewportX + 2 * (Game_OverlayDisplay.ViewportWidth - 10);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + Game_OverlayDisplay.ViewportX*2 + 2*(Game_OverlayDisplay.ViewportWidth - 10);
            for (x = 8; x != 0; x--)
            {
                dst[0] = Game_Palette[src2[0]].pix;
                dst[1] = Game_Palette[src2[1]].pix;
                dst[720] = Game_Palette[src2[800]].pix;
                dst[721] = Game_Palette[src2[801]].pix;

                src2+=2;
                dst+=2;
            }
        }
    }
    else
    {
        for (y = 240; y != 0; y--)
        {
            for (x = 360; x != 0; x-=9)
            {
                WRITE_PIXEL2(0)
                WRITE_PIXEL2(1)
                WRITE_PIXEL2(2)
                WRITE_PIXEL2(3)
                WRITE_PIXEL2(4)
                WRITE_PIXEL2(5)
                WRITE_PIXEL2(6)
                WRITE_PIXEL2(7)
                WRITE_PIXEL2(8)

                src+=9;
                dst+=18;
            }
            dst+=720;
        }
    }

#undef WRITE_PIXEL2
}

/*
static void Flip_360x240x8_to_640x480x32_in_720x480(uint8_t *src, uint32_t *dst)
{
    int x, y;

#define WRITE_PIXEL2(x1, x2) dst[(x2)] = dst[1 + (x2)] = dst[720 + (x2)] = dst[721 + (x2)] = Game_Palette[src[(x1)]].pix;
#define WRITE_PIXEL1(x1, x2) dst[(x2)] = dst[720 + (x2)] = Game_Palette[src[(x1)]].pix;

    dst += 40;

    for (y = 240; y != 0; y--)
    {
        for (x = 360; x != 0; x-=9)
        {
            //
            // 221222122
            //

            WRITE_PIXEL2(0, 0)
            WRITE_PIXEL2(1, 2)
            WRITE_PIXEL1(2, 4)
            WRITE_PIXEL2(3, 5)
            WRITE_PIXEL2(4, 7)
            WRITE_PIXEL2(5, 9)
            WRITE_PIXEL1(6, 11)
            WRITE_PIXEL2(7, 12)
            WRITE_PIXEL2(8, 14)

            src+=9;
            dst+=16;
        }
        dst+=800;
    }

#undef WRITE_PIXEL1
#undef WRITE_PIXEL2
}
*/

/*
static void Flip_360x240x8_to_640x480x32_in_720x480_interpolated(uint8_t *src, uint32_t *dst)
{
    int x, y;

// http://www.compuphase.com/graphic/scale3.htm
#define AVERAGE(a, b) ( ((((a) ^ (b)) & 0xfffefefeL) >> 1) + ((a) & (b)) )
#define WRITE_PIXEL2(x1, x2) dst[(x2)] = dst[1 + (x2)] = dst[720 + (x2)] = dst[721 + (x2)] = Game_Palette[src[(x1)]].pix;
#define WRITE_PIXEL3(x1, x2) \
    { \
        dst[(x2)] = dst[720 + (x2)] = Game_Palette[src[(x1)]].pix; \
        dst[2 + (x2)] = dst[722 + (x2)] = Game_Palette[src[1 + (x1)]].pix; \
        dst[1 + (x2)] = dst[721 + (x2)] = AVERAGE(Game_Palette[src[(x1)]].pix, Game_Palette[src[1 + (x1)]].pix); \
    }

    dst += 40;

    for (y = 240; y != 0; y--)
    {
        for (x = 360; x != 0; x-=9)
        {
            //
            // 2322232
            //

            WRITE_PIXEL2(0, 0)
            WRITE_PIXEL3(1, 2)
            WRITE_PIXEL2(3, 5)
            WRITE_PIXEL2(4, 7)
            WRITE_PIXEL2(5, 9)
            WRITE_PIXEL3(6, 11)
            WRITE_PIXEL2(8, 14)

            src+=9;
            dst+=16;
        }
        dst+=800;
    }

#undef WRITE_PIXEL3
#undef WRITE_PIXEL2
#undef AVERAGE
}
*/

/*
static void Flip_360x240x8_to_640x480x32_in_720x480_interpolated2(uint8_t *src, uint32_t *dst)
{
    int x, y;
    pixel_format_disp pixel;

#define MAXDIFF 128
#define INTERPOLATE(a, b) (((a) >= (b)) ? ( ((a) - (b) >= MAXDIFF) ? ((3 * ((uint32_t)(a)) + (b)) / 4) : ((((uint32_t)(a)) + (b)) / 2) ) : ( ((b) - (a) >= MAXDIFF) ? ((3 * ((uint32_t)(b)) + (a)) / 4) : ((((uint32_t)(a)) + (b)) / 2) ) )
#define WRITE_PIXEL2(x1, x2) dst[(x2)] = dst[1 + (x2)] = dst[720 + (x2)] = dst[721 + (x2)] = Game_Palette[src[(x1)]].pix;
#define WRITE_PIXEL3(x1, x2) \
    { \
        pixel1.pix = Game_Palette[src[(x1)]].pix; \
        pixel2.pix = Game_Palette[src[1 + (x1)]].pix; \
        dst[(x2)] = dst[720 + (x2)] = pixel1.pix; \
        dst[2 + (x2)] = dst[722 + (x2)] = pixel2.pix; \
        pixel.s.r = INTERPOLATE(pixel1.s.r, pixel2.s.r); \
        pixel.s.g = INTERPOLATE(pixel1.s.g, pixel2.s.g); \
        pixel.s.b = INTERPOLATE(pixel1.s.b, pixel2.s.b); \
        dst[1 + (x2)] = dst[721 + (x2)] = pixel.pix; \
    }

    pixel.pix = 0;
    dst += 40;

    for (y = 240; y != 0; y--)
    {
        for (x = 360; x != 0; x-=9)
        {
            //
            // 2322232
            //

            pixel_format_disp pixel1, pixel2;

            WRITE_PIXEL2(0, 0)
            WRITE_PIXEL3(1, 2)
            WRITE_PIXEL2(3, 5)
            WRITE_PIXEL2(4, 7)
            WRITE_PIXEL2(5, 9)
            WRITE_PIXEL3(6, 11)
            WRITE_PIXEL2(8, 14)

            src+=9;
            dst+=16;
        }
        dst+=800;
    }

#undef WRITE_PIXEL3
#undef WRITE_PIXEL2
#undef INTERPOLATE
#undef MAXDIFF
}
*/

static void Flip_360x240x8_to_640x480x32_in_720x480_interpolated2_lt(uint8_t *src, uint32_t *dst)
{
    int x, y, DrawOverlay, ViewportX_9, ViewportWidth_9, ViewportX2_9;

#define WRITE_PIXEL2(x1, x2) dst[(x2)] = dst[1 + (x2)] = dst[720 + (x2)] = dst[721 + (x2)] = Game_Palette[src[(x1)]].pix;
#define WRITE_PIXEL3(x1, x2) \
    { \
        dst[(x2)] = dst[720 + (x2)] = Game_Palette[src[(x1)]].pix; \
        dst[1 + (x2)] = dst[721 + (x2)] = IM2_LOOKUP(src[(x1)], src[1 + (x1)]); \
        dst[2 + (x2)] = dst[722 + (x2)] = Game_Palette[src[1 + (x1)]].pix; \
    }

#define OVERLAY_PIXEL2(x1, x2) \
    { \
        if (src[(x1)] == orig[(x1)]) \
        { \
            dst[(x2)] = Game_Palette[src2[(x2)]].pix; \
            dst[(x2) + 1] = Game_Palette[src2[(x2) + 1]].pix; \
            dst[(x2) + 720] = Game_Palette[src2[(x2) + 800]].pix; \
            dst[(x2) + 721] = Game_Palette[src2[(x2) + 801]].pix; \
        } \
        else \
        { \
            dst[(x2)] = dst[1 + (x2)] = dst[720 + (x2)] = dst[721 + (x2)] = Game_Palette[src[(x1)]].pix; \
        } \
    }
#define OVERLAY_PIXEL3(x1, x2) \
    { \
        if (src[(x1)] == orig[(x1)]) \
        { \
            if (src[(x1) + 1] == orig[(x1) + 1]) \
            { \
                dst[(x2)] = Game_Palette[src2[(x2)]].pix; \
                dst[(x2) + 1] = Game_Palette[src2[(x2) + 1]].pix; \
                dst[(x2) + 2] = Game_Palette[src2[(x2) + 2]].pix; \
                dst[(x2) + 720] = Game_Palette[src2[(x2) + 800]].pix; \
                dst[(x2) + 721] = Game_Palette[src2[(x2) + 801]].pix; \
                dst[(x2) + 722] = Game_Palette[src2[(x2) + 802]].pix; \
            } \
            else \
            { \
                dst[(x2)] = Game_Palette[src2[(x2)]].pix; \
                dst[1 + (x2)] = IM2_LOOKUP(src2[(x2) + 1], src[1 + (x1)]); \
                dst[(x2) + 720] = Game_Palette[src2[(x2) + 800]].pix; \
                dst[721 + (x2)] = IM2_LOOKUP(src2[(x2) + 801], src[1 + (x1)]); \
                dst[2 + (x2)] = dst[722 + (x2)] = Game_Palette[src[1 + (x1)]].pix; \
            } \
        } \
        else \
        { \
            if (src[(x1) + 1] == orig[(x1) + 1]) \
            { \
                dst[(x2)] = dst[720 + (x2)] = Game_Palette[src[(x1)]].pix; \
                dst[1 + (x2)] = IM2_LOOKUP(src[(x1)], src2[(x2) + 801]); \
                dst[(x2) + 2] = Game_Palette[src2[(x2) + 2]].pix; \
                dst[721 + (x2)] = IM2_LOOKUP(src[(x1)], src2[(x2) + 801]); \
                dst[(x2) + 722] = Game_Palette[src2[(x2) + 802]].pix; \
            } \
            else \
            { \
                dst[(x2)] = dst[720 + (x2)] = Game_Palette[src[(x1)]].pix; \
                dst[1 + (x2)] = dst[721 + (x2)] = IM2_LOOKUP(src[(x1)], src[1 + (x1)]); \
                dst[2 + (x2)] = dst[722 + (x2)] = Game_Palette[src[1 + (x1)]].pix; \
            } \
        } \
    }

    DrawOverlay = Get_DrawOverlay(src, 1);

    dst += 40;

    if (DrawOverlay)
    {
        uint8_t *zalsrc, *src2, *tmpsrc2, *orig;
        uint32_t *zaldst, *tmpdst;

        // display part above the viewport
        for (y = Game_OverlayDisplay.ViewportY; y != 0; y--)
        {
            for (x = 360; x != 0; x-=9)
            {
                //
                // 2322232
                //

                WRITE_PIXEL2(0, 0)
                WRITE_PIXEL3(1, 2)
                WRITE_PIXEL2(3, 5)
                WRITE_PIXEL2(4, 7)
                WRITE_PIXEL2(5, 9)
                WRITE_PIXEL3(6, 11)
                WRITE_PIXEL2(8, 14)

                src+=9;
                dst+=16;
            }
            dst+=720+(720-640);
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
                    //
                    // 2322232
                    //

                    WRITE_PIXEL2(0, 0)
                    WRITE_PIXEL3(1, 2)
                    WRITE_PIXEL2(3, 5)
                    WRITE_PIXEL2(4, 7)
                    WRITE_PIXEL2(5, 9)
                    WRITE_PIXEL3(6, 11)
                    WRITE_PIXEL2(8, 14)

                    src+=9;
                    dst+=16;
                }
                src+=(360-ViewportX_9);
                dst = tmpdst + 2*720;
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
                    //
                    // 2322232
                    //

                    WRITE_PIXEL2(0, 0)
                    WRITE_PIXEL3(1, 2)
                    WRITE_PIXEL2(3, 5)
                    WRITE_PIXEL2(4, 7)
                    WRITE_PIXEL2(5, 9)
                    WRITE_PIXEL3(6, 11)
                    WRITE_PIXEL2(8, 14)

                    src+=9;
                    dst+=16;
                }
                src+=(360-ViewportX2_9);
                dst = tmpdst + 2*720;
            }

        }

        // display part below the viewport
        src = zalsrc + 360 * Game_OverlayDisplay.ViewportHeight;
        dst = zaldst + 720 * 2 * Game_OverlayDisplay.ViewportHeight;
        for (y = 240 - (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight); y != 0; y--)
        {
            for (x = 360; x != 0; x-=9)
            {
                //
                // 2322232
                //

                WRITE_PIXEL2(0, 0)
                WRITE_PIXEL3(1, 2)
                WRITE_PIXEL2(3, 5)
                WRITE_PIXEL2(4, 7)
                WRITE_PIXEL2(5, 9)
                WRITE_PIXEL3(6, 11)
                WRITE_PIXEL2(8, 14)

                src+=9;
                dst+=16;
            }
            dst+=720+(720-640);
        }

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
                //
                // 2322232
                //

                OVERLAY_PIXEL2(0, 0)
                OVERLAY_PIXEL3(1, 2)
                OVERLAY_PIXEL2(3, 5)
                OVERLAY_PIXEL2(4, 7)
                OVERLAY_PIXEL2(5, 9)
                OVERLAY_PIXEL3(6, 11)
                OVERLAY_PIXEL2(8, 14)

                src+=9;
                orig+=9;
                src2+=16;
                dst+=16;
            }
            src+=(360-ViewportWidth_9);
            orig+=(360-ViewportWidth_9);
            src2 = tmpsrc2 + 2*800;
            dst = tmpdst + 2*720;
        }

        if (DrawOverlay & 1)
        {
            dst = zaldst + 720 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + (((Game_OverlayDisplay.ViewportX + 1) * 16) / 9);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + (((Game_OverlayDisplay.ViewportX + 1) * 16) / 9);
            for (x = 8; x != 0; x--)
            {
                dst[0] = Game_Palette[src2[0]].pix;
                dst[1] = Game_Palette[src2[1]].pix;
                dst[720] = Game_Palette[src2[800]].pix;
                dst[721] = Game_Palette[src2[801]].pix;

                src2+=2;
                dst+=2;
            }
        }

        if (DrawOverlay & 2)
        {
            dst = zaldst + 720 * 2 * (Game_OverlayDisplay.ViewportHeight - 2) + (((Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth - 10) * 16) / 9);
            src2 = Game_OverlayDisplay.ScreenViewpartOverlay + (Game_OverlayDisplay.ViewportY + Game_OverlayDisplay.ViewportHeight - 2)*2 * 800 + (((Game_OverlayDisplay.ViewportX + Game_OverlayDisplay.ViewportWidth - 10) * 16) / 9);
            for (x = 8; x != 0; x--)
            {
                dst[0] = Game_Palette[src2[0]].pix;
                dst[1] = Game_Palette[src2[1]].pix;
                dst[720] = Game_Palette[src2[800]].pix;
                dst[721] = Game_Palette[src2[801]].pix;

                src2+=2;
                dst+=2;
            }
        }
    }
    else
    {
        for (y = 240; y != 0; y--)
        {
            for (x = 360; x != 0; x-=9)
            {
                //
                // 2322232
                //

                WRITE_PIXEL2(0, 0)
                WRITE_PIXEL3(1, 2)
                WRITE_PIXEL2(3, 5)
                WRITE_PIXEL2(4, 7)
                WRITE_PIXEL2(5, 9)
                WRITE_PIXEL3(6, 11)
                WRITE_PIXEL2(8, 14)

                src+=9;
                dst+=16;
            }
            dst+=720+(720-640);
        }
    }

#undef OVERLAY_PIXEL3
#undef OVERLAY_PIXEL2
#undef WRITE_PIXEL3
#undef WRITE_PIXEL2
}

#if !defined(ALLOW_OPENGL) && !defined(USE_SDL2)
static void Flip_360x240x8_to_WxHx32_bilinear(uint8_t *src, uint32_t *dst)
{
    uint32_t src_y, src_ydelta, src_ypos, src_xdelta, src_xpos, src_xpos_0, dst_xlastsize, height, width, val1, val2, dstval;
    uint32_t *srcline1, *srcline2;

    Flip_360x240x8_to_720x480x32(src, ScaleSrc);

    dst_xlastsize = (((ScaledWidth << 16) / 720) + 0xffff) >> 16;
    src_xdelta = (720 << 16) / ScaledWidth;
    if (720 > ScaledWidth)
    {
        src_xpos_0 = ((720 - ScaledWidth) << 16) / 720;
    }
    else
    {
        src_xpos_0 = 0;
    }

    src_ydelta = (480 << 16) / ScaledHeight;
    if (480 > ScaledHeight)
    {
        src_ypos = ((480 - ScaledHeight) << 16) / 480;
    }
    else
    {
        src_ypos = 0;
    }

    for (height = ScaledHeight; height != 0; height--)
    {
        src_xpos = src_xpos_0;

        src_y = src_ypos >> 16;
        srcline1 = ScaleSrc + 720 * src_y;
        srcline2 = srcline1 + 719;

        if ((src_y < 479) && ((src_ypos & 0xffff) != 0))
        {
            // bilinear
            for (width = ScaledWidth - dst_xlastsize; width != 0; width--)
            {
                uint32_t tmpval1, tmpval2;

                val1 = srcline1[0];
                val2 = srcline1[1];

                // r
                tmpval1 = (((int)(val1 & 0xff)) << 2) + (((((int)(val2 & 0xff)) - ((int)(val1 & 0xff))) * (int)src_xpos) >> 14);
                // g
                tmpval1 |= ( (((int)((val1 >> 8) & 0xff)) << 2) + (((((int)((val2 >> 8) & 0xff)) - ((int)((val1 >> 8) & 0xff))) * (int)src_xpos) >> 14) ) << 10;
                // b
                tmpval1 |= ( (((int)((val1 >> 16) & 0xff)) << 2) + (((((int)((val2 >> 16) & 0xff)) - ((int)((val1 >> 16) & 0xff))) * (int)src_xpos) >> 14) ) << 20;

                val1 = srcline1[720];
                val2 = srcline1[721];

                // r
                tmpval2 = (((int)(val1 & 0xff)) << 2) + (((((int)(val2 & 0xff)) - ((int)(val1 & 0xff))) * (int)src_xpos) >> 14);
                // g
                tmpval2 |= ( (((int)((val1 >> 8) & 0xff)) << 2) + (((((int)((val2 >> 8) & 0xff)) - ((int)((val1 >> 8) & 0xff))) * (int)src_xpos) >> 14) ) << 10;
                // b
                tmpval2 |= ( (((int)((val1 >> 16) & 0xff)) << 2) + (((((int)((val2 >> 16) & 0xff)) - ((int)((val1 >> 16) & 0xff))) * (int)src_xpos) >> 14) ) << 20;

                // r
                dstval = ( ((int)(tmpval1 & 0x3ff)) + (((((int)(tmpval2 & 0x3ff)) - ((int)(tmpval1 & 0x3ff))) * (int)(src_ypos & 0xffff)) >> 16) ) >> 2;
                // g
                dstval |= ( ( ((int)((tmpval1 >> 10) & 0x3ff)) + (((((int)((tmpval2 >> 10) & 0x3ff)) - ((int)((tmpval1 >> 10) & 0x3ff))) * (int)(src_ypos & 0xffff)) >> 16) ) & 0x3fc ) << 6;
                // b
                dstval |= ( ( ((int)((tmpval1 >> 20) & 0x3ff)) + (((((int)((tmpval2 >> 20) & 0x3ff)) - ((int)((tmpval1 >> 20) & 0x3ff))) * (int)(src_ypos & 0xffff)) >> 16) ) & 0x3fc ) << 14;

                *dst = dstval;

                dst++;
                src_xpos += src_xdelta;
                srcline1 += (src_xpos >> 16);
                src_xpos &= 0xffff;
            }

            if (dst_xlastsize != 0)
            {
                val1 = srcline2[0];
                val2 = srcline2[720];

                // r
                dstval = ((int)(val1 & 0xff)) + (((((int)(val2 & 0xff)) - ((int)(val1 & 0xff))) * (int)(src_ypos & 0xffff)) >> 16);
                // g
                dstval |= ( ((int)((val1 >> 8) & 0xff)) + (((((int)((val2 >> 8) & 0xff)) - ((int)((val1 >> 8) & 0xff))) * (int)(src_ypos & 0xffff)) >> 16) ) << 8;
                // b
                dstval |= ( ((int)((val1 >> 16) & 0xff)) + (((((int)((val2 >> 16) & 0xff)) - ((int)((val1 >> 16) & 0xff))) * (int)(src_ypos & 0xffff)) >> 16) ) << 16;
            }
            for (width = dst_xlastsize; width != 0; width--)
            {
                *dst = dstval;

                dst++;
            }
        }
        else
        {
            // linear
            for (width = ScaledWidth - dst_xlastsize; width != 0; width--)
            {
                val1 = srcline1[0];
                val2 = srcline1[1];

                // r
                dstval = ((int)(val1 & 0xff)) + (((((int)(val2 & 0xff)) - ((int)(val1 & 0xff))) * (int)src_xpos) >> 16);
                // g
                dstval |= ( ((int)((val1 >> 8) & 0xff)) + (((((int)((val2 >> 8) & 0xff)) - ((int)((val1 >> 8) & 0xff))) * (int)src_xpos) >> 16) ) << 8;
                // b
                dstval |= ( ((int)((val1 >> 16) & 0xff)) + (((((int)((val2 >> 16) & 0xff)) - ((int)((val1 >> 16) & 0xff))) * (int)src_xpos) >> 16) ) << 16;

                *dst = dstval;

                dst++;
                src_xpos += src_xdelta;
                srcline1 += (src_xpos >> 16);
                src_xpos &= 0xffff;
            }

            dstval = *srcline2;
            for (width = dst_xlastsize; width != 0; width--)
            {
                *dst = dstval;

                dst++;
            }
        }

        src_ypos += src_ydelta;
    }
}
#endif

void Init_Display(void)
{
#if defined(USE_SDL2) || defined(ALLOW_OPENGL)
    Display_FSType = 1;
#else
    Display_FSType = 0;
#endif
    Font_Size_Shift = 2;
    Game_UseEnhanced3DEngineNewValue = 1;

    ScaleOutput = 0;
    ScaledWidth = 720;
    ScaledHeight = 480;
    Fullscreen = 0;
    ScaleSrc = NULL;
}

void Init_Display2(void)
{
    Init_Palette();

    memset(&(interpolation_matrix2[0]), 0, sizeof(interpolation_matrix2));

#if !defined(USE_SDL2) && !defined(ALLOW_OPENGL)
    Game_AdvancedScaling = 0;
#endif

    if (Fullscreen && Display_FSType)
    {
        if ((ScaledWidth == 0) || (ScaledHeight == 0))
        {
            Display_FSType = 2;
            ScaledWidth = 640;
            ScaledHeight = 480;
        }
    }
    else
    {
        if (ScaledWidth < 640) ScaledWidth = 640;
        if (ScaledHeight < 480) ScaledHeight = 480;
    }

    if ((ScaledWidth != 720) || (ScaledHeight != 480) || (Fullscreen && Display_FSType))
    {
        ScaleOutput = 1;

    #if defined(USE_SDL2)
    #elif defined(ALLOW_OPENGL)
        Game_UseOpenGL = 1;
    #else
        ScaleSrc = (uint32_t *) malloc(720*480*4+4);
        if (ScaleSrc == NULL)
        {
            ScaleOutput = 0;
            ScaledWidth = 720;
            ScaledHeight = 480;
        }
    #endif
    }

    DisplayMode = 0;
    Display_Width = ScaledWidth;
    Display_Height = ScaledHeight;
    Display_Bitsperpixel = 32;
    Display_Fullscreen = Fullscreen;
    Display_MouseLocked = 0;
    Render_Width = 720;
    Render_Height = 480;
    Picture_Width = ScaledWidth;
    Picture_Height = ScaledHeight;
    Picture_Position_UL_X = 0;
    Picture_Position_UL_Y = 0;
    Picture_Position_BR_X = ScaledWidth - 1;
    Picture_Position_BR_Y = ScaledHeight - 1;
#if defined(USE_SDL2) || defined(ALLOW_OPENGL)
    if (Game_AdvancedScaling)
    {
        Render_Width = 360;
        Render_Height = 240;
        Display_Advanced_Flip_Procedure = (Game_Advanced_Flip_Procedure) &Flip_360x240x8_to_360x240x32_advanced;
    }
    else
#else
    if (ScaleOutput)
    {
        Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_WxHx32_bilinear;
    }
    else
#endif
    {
        Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_720x480x32;
    }
}

int Config_Display(char *str, char *param)
{
    int num_int;

    if ( strncasecmp(str, "Display_", 8) == 0 ) // str begins with "Display_"
    {
        str += 8;

        if ( strcasecmp(str, "ScaledWidth") == 0)	// str equals "ScaledWidth"
        {
            num_int = 0;
            if (sscanf(param, "%i", &num_int) > 0)
            {
                ScaledWidth = num_int;
                if (ScaledWidth < 0) ScaledWidth = 0;
            }

            return 1;
        }
        else if ( strcasecmp(str, "ScaledHeight") == 0)	// str equals "ScaledHeight"
        {
            num_int = 0;
            if (sscanf(param, "%i", &num_int) > 0)
            {
                ScaledHeight = num_int;
                if (ScaledHeight < 0) ScaledHeight = 0;
            }

            return 1;
        }
        else if ( strcasecmp(str, "Fullscreen") == 0)	// str equals "Fullscreen"
        {
            if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
            {
                Fullscreen = 1;
            }
            else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
            {
                Fullscreen = 0;
            }

            return 1;
        }
    }

    return 0;
}

void Cleanup_Display(void)
{
    if (ScaleSrc != NULL)
    {
        free(ScaleSrc);
        ScaleSrc = NULL;
    }
}

int Change_Display_Mode(int direction)
{
    int ClearScreen;

    if (ScaleOutput || Game_AdvancedScaling) return 0;

    ClearScreen = 0;
    DisplayMode = (DisplayMode + direction + 2) % 2;

    switch (DisplayMode)
    {
        case 0:
            Render_Width = 720;
            Picture_Width = 720;
            Picture_Position_UL_X = 0;
            Picture_Position_BR_X = 719;
            Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_720x480x32;
            break;
        case 1:
            Render_Width = 640;
            Picture_Width = 640;
            Picture_Position_UL_X = 40;
            Picture_Position_BR_X = 679;
            Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_360x240x8_to_640x480x32_in_720x480_interpolated2_lt;

            ClearScreen = 1;
            break;
    }

    return ClearScreen;
}

