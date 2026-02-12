/**
 *
 *  Copyright (C) 2016-2026 Roman Pauer
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
#include <string.h>

static int DisplayMode;

static int ScaledWidth, ScaledHeight, Fullscreen;

static pixel_format_disp Game_PaletteAlpha[256];

static void Set_Palette_Value2(uint32_t index, uint32_t r, uint32_t g, uint32_t b)
{
    Game_PaletteAlpha[index].s.r = r;
    Game_PaletteAlpha[index].s.g = g;
    Game_PaletteAlpha[index].s.b = b;
    Game_PaletteAlpha[index].s.a = 255;
}

static void Flip_360x240x8_to_360x240x32_advanced(uint8_t *src, uint32_t *dst1, uint32_t *dst2, int *dst2_used)
{
    int counter, DrawOverlay;
    Game_OverlayInfo OverlayInfo;

    OverlayInfo = Game_OverlayDisplay;
    DrawOverlay = Get_DrawOverlay(src, &OverlayInfo);
    *dst2_used = DrawOverlay;

    if (DrawOverlay)
    {
        uint8_t *zalsrc, *src2, *orig;
        uint32_t *zaldst1, *zaldst2;
        int x, y, ViewportX2;

        // display part above the viewport
        if (OverlayInfo.ViewportY != 0)
        {
            for (counter = 360 * OverlayInfo.ViewportY; counter != 0; counter -= 8)
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

            for (counter = Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * OverlayInfo.ViewportY; counter != 0; counter -= 8)
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
        if (OverlayInfo.ViewportX != 0)
        {
            for (y = OverlayInfo.ViewportHeight; y != 0; y--)
            {
                for (x = OverlayInfo.ViewportX; x >= 8; x -= 8)
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

                src += (360 - OverlayInfo.ViewportX);
                dst1 += (360 - OverlayInfo.ViewportX);
            }

            for (y = Scaler_ScaleFactor * OverlayInfo.ViewportHeight; y != 0; y--)
            {
                for (x = Scaler_ScaleFactor * OverlayInfo.ViewportX; x >= 8; x -= 8)
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

                dst2 += Scaler_ScaleFactor * (360 - OverlayInfo.ViewportX);
            }
        }

        // display part right of the viewport
        if ((OverlayInfo.ViewportX + OverlayInfo.ViewportWidth) != 360)
        {
            src = zalsrc + (OverlayInfo.ViewportX + OverlayInfo.ViewportWidth);
            dst1 = zaldst1 + (OverlayInfo.ViewportX + OverlayInfo.ViewportWidth);
            dst2 = zaldst2 + Scaler_ScaleFactor * (OverlayInfo.ViewportX + OverlayInfo.ViewportWidth);
            ViewportX2 = 360 - (OverlayInfo.ViewportX + OverlayInfo.ViewportWidth);

            for (y = OverlayInfo.ViewportHeight; y != 0; y--)
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

            for (y = Scaler_ScaleFactor * OverlayInfo.ViewportHeight; y != 0; y--)
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
        src = zalsrc + (360 * OverlayInfo.ViewportHeight);
        dst1 = zaldst1 + (360 * OverlayInfo.ViewportHeight);
        dst2 = zaldst2 + Scaler_ScaleFactor * Scaler_ScaleFactor * (360 * OverlayInfo.ViewportHeight);

        for (counter = 360 * (240 - (OverlayInfo.ViewportY + OverlayInfo.ViewportHeight)); counter != 0; counter -= 8)
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

        for (counter = Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (240 - (OverlayInfo.ViewportY + OverlayInfo.ViewportHeight)); counter != 0; counter -= 8)
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
        src = zalsrc + OverlayInfo.ViewportX;
        dst1 = zaldst1 + OverlayInfo.ViewportX;
        orig = OverlayInfo.ScreenViewpartOriginal + 360 * OverlayInfo.ViewportY + OverlayInfo.ViewportX;

        for (y = OverlayInfo.ViewportHeight; y != 0; y--)
        {
            for (x = OverlayInfo.ViewportWidth; x >= 8; x -= 8)
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

            src += (360 - OverlayInfo.ViewportWidth);
            orig += (360 - OverlayInfo.ViewportWidth);
            dst1 += (360 - OverlayInfo.ViewportWidth);
        }

        dst1 = zaldst1 + OverlayInfo.ViewportX;
        dst2 = zaldst2 + Scaler_ScaleFactor * OverlayInfo.ViewportX;
        src2 = OverlayInfo.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * OverlayInfo.ViewportY + Scaler_ScaleFactor * OverlayInfo.ViewportX;

        counter = Scaler_ScaleFactor;
        for (y = Scaler_ScaleFactor * OverlayInfo.ViewportHeight; y != 0; y--)
        {
            int counter2, same;

            counter2 = Scaler_ScaleFactor;
            same = (dst1[0] == 0)?1:0;

            for (x = Scaler_ScaleFactor * OverlayInfo.ViewportWidth; x != 0; x--)
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

            src2 += Scaler_ScaleFactor * (360 - OverlayInfo.ViewportWidth);
            dst2 += Scaler_ScaleFactor * (360 - OverlayInfo.ViewportWidth);

            counter--;
            if (counter == 0)
            {
                counter = Scaler_ScaleFactor;
                dst1 += (360 - OverlayInfo.ViewportWidth);
            }
            else
            {
                dst1 -= OverlayInfo.ViewportWidth;
            }
        }

        // markers
        if (DrawOverlay & 1)
        {
            dst1 = zaldst1 + 360 * (OverlayInfo.ViewportHeight - 2) + (OverlayInfo.ViewportX + 1);

            for (x = 8; x != 0; x--)
            {
                dst1[0] = 0;
                dst1++;
            }

            dst2 = zaldst2 + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (OverlayInfo.ViewportHeight - 2) + Scaler_ScaleFactor * (OverlayInfo.ViewportX + 1);
            src2 = OverlayInfo.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (OverlayInfo.ViewportY + OverlayInfo.ViewportHeight - 2) + Scaler_ScaleFactor * (OverlayInfo.ViewportX + 1);

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
            dst1 = zaldst1 + 360 * (OverlayInfo.ViewportHeight - 2) + (OverlayInfo.ViewportX + OverlayInfo.ViewportWidth - 10);

            for (x = 8; x != 0; x--)
            {
                dst1[0] = 0;
                dst1++;
            }

            dst2 = zaldst2 + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (OverlayInfo.ViewportHeight - 2) + Scaler_ScaleFactor * (OverlayInfo.ViewportX + OverlayInfo.ViewportWidth - 10);
            src2 = OverlayInfo.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (OverlayInfo.ViewportY + OverlayInfo.ViewportHeight - 2) + Scaler_ScaleFactor * (OverlayInfo.ViewportX + OverlayInfo.ViewportWidth - 10);

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

static void Flip_360x240x8_to_720x480x32(uint8_t *src, uint32_t *dst)
{
    int x, y, DrawOverlay, ViewportX2;
    Game_OverlayInfo OverlayInfo;

#define WRITE_PIXEL2(_x) dst[2 * (_x)] = dst[1 + (2 * (_x))] = dst[720 + (2 * (_x))] = dst[721 + (2 * (_x))] = Game_Palette[src[(_x)]].pix;

    OverlayInfo = Game_OverlayDisplay;
    DrawOverlay = Get_DrawOverlay(src, &OverlayInfo);

    if (DrawOverlay)
    {
        uint8_t *zalsrc, *src2, *orig;
        uint32_t *zaldst;

        // display part above the viewport
        for (y = OverlayInfo.ViewportY; y != 0; y--)
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
        if (OverlayInfo.ViewportX != 0)
        {
            for (y = OverlayInfo.ViewportHeight; y != 0; y--)
            {
                for (x = OverlayInfo.ViewportX; x != 0; x--)
                {
                    WRITE_PIXEL2(0)

                    src++;
                    dst+=2;
                }
                src+=(360-OverlayInfo.ViewportX);
                dst+=720+(720-2*OverlayInfo.ViewportX);
            }
        }

        // display part right of the viewport
        if ((OverlayInfo.ViewportX + OverlayInfo.ViewportWidth) != 360)
        {
            src = zalsrc + OverlayInfo.ViewportX + OverlayInfo.ViewportWidth;
            dst = zaldst + 2 * (OverlayInfo.ViewportX + OverlayInfo.ViewportWidth);
            ViewportX2 = 360 - (OverlayInfo.ViewportX + OverlayInfo.ViewportWidth);
            for (y = OverlayInfo.ViewportHeight; y != 0; y--)
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
        src = zalsrc + 360 * OverlayInfo.ViewportHeight;
        dst = zaldst + 720 * 2 * OverlayInfo.ViewportHeight;
        for (y = 240 - (OverlayInfo.ViewportY + OverlayInfo.ViewportHeight); y != 0; y--)
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
        src = zalsrc + OverlayInfo.ViewportX;
        orig = OverlayInfo.ScreenViewpartOriginal + 360 * OverlayInfo.ViewportY + OverlayInfo.ViewportX;
        dst = zaldst + 2 * OverlayInfo.ViewportX;
        src2 = OverlayInfo.ScreenViewpartOverlay + OverlayInfo.ViewportY*2 * 800 + OverlayInfo.ViewportX*2;
        for (y = OverlayInfo.ViewportHeight; y != 0; y--)
        {
            for (x = OverlayInfo.ViewportWidth; x != 0; x--)
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
            src+=(360 - OverlayInfo.ViewportWidth);
            orig+=(360 - OverlayInfo.ViewportWidth);
            src2+=800 + (800 - 2*OverlayInfo.ViewportWidth);
            dst+=720 + (720 - 2 * OverlayInfo.ViewportWidth);
        }

        if (DrawOverlay & 1)
        {
            dst = zaldst + 720 * 2 * (OverlayInfo.ViewportHeight - 2) + 2 * (OverlayInfo.ViewportX + 1);
            src2 = OverlayInfo.ScreenViewpartOverlay + (OverlayInfo.ViewportY + OverlayInfo.ViewportHeight - 2)*2 * 800 + (OverlayInfo.ViewportX + 1)*2;
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
            dst = zaldst + 720 * 2 * (OverlayInfo.ViewportHeight - 2) + 2 * OverlayInfo.ViewportX + 2 * (OverlayInfo.ViewportWidth - 10);
            src2 = OverlayInfo.ScreenViewpartOverlay + (OverlayInfo.ViewportY + OverlayInfo.ViewportHeight - 2)*2 * 800 + OverlayInfo.ViewportX*2 + 2*(OverlayInfo.ViewportWidth - 10);
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

void Init_Display(void)
{
    Display_FSType = 1;
    Game_UseEnhanced3DEngineNewValue = 1;

    ScaledWidth = 720;
    ScaledHeight = 480;
    Fullscreen = 0;
}

void Init_Display2(void)
{
    Init_Palette();

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
    if (Game_AdvancedScaling)
    {
        Render_Width = 360;
        Render_Height = 240;
        Display_Advanced_Flip_Procedure = (Game_Advanced_Flip_Procedure) &Flip_360x240x8_to_360x240x32_advanced;
    }
    else
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
}

