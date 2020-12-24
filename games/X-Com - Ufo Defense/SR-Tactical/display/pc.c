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
#include "palette32bgra.h"
#include <memory.h>

static int ScaleOutput, ScaledWidth, ScaledHeight, Fullscreen;
static uint32_t *ScaleSrc;

static void Set_Palette_Value2(uint32_t index, uint32_t r, uint32_t g, uint32_t b)
{
}

static void Flip_320x200x8_to_640x400x32(uint8_t *src, uint32_t *dst)
{
    int x, y;

#define WRITE_PIXEL2(x) dst[2 * (x)] = dst[1 + (2 * (x))] = dst[640 + (2 * (x))] = dst[641 + (2 * (x))] = Game_Palette[src[(x)]].pix;

    for (y = 200; y != 0; y--)
    {
        for (x = 320; x != 0; x-=8)
        {
            WRITE_PIXEL2(0)
            WRITE_PIXEL2(1)
            WRITE_PIXEL2(2)
            WRITE_PIXEL2(3)
            WRITE_PIXEL2(4)
            WRITE_PIXEL2(5)
            WRITE_PIXEL2(6)
            WRITE_PIXEL2(7)

            src+=8;
            dst+=16;
        }
        dst+=640;
    }

#undef WRITE_PIXEL2
}

#if !defined(ALLOW_OPENGL) && !defined(USE_SDL2)
static void Flip_320x200x8_to_320x200x32(const uint8_t *src, uint32_t *dst)
{
    int counter;

    for (counter = 320*200; counter != 0; counter-=8)
    {
        dst[0] = Game_Palette[src[0]].pix;
        dst[1] = Game_Palette[src[1]].pix;
        dst[2] = Game_Palette[src[2]].pix;
        dst[3] = Game_Palette[src[3]].pix;
        dst[4] = Game_Palette[src[4]].pix;
        dst[5] = Game_Palette[src[5]].pix;
        dst[6] = Game_Palette[src[6]].pix;
        dst[7] = Game_Palette[src[7]].pix;

        src+=8;
        dst+=8;
    }
}

static void Flip_320x200x8_to_WxHx32_bilinear(uint8_t *src, uint32_t *dst)
{
    uint32_t src_y, src_ydelta, src_ypos, src_xdelta, src_xpos, src_xpos_0, dst_xlastsize, height, width, val1, val2, dstval;
    uint32_t *srcline1, *srcline2;

    Flip_320x200x8_to_320x200x32(src, ScaleSrc);

    dst_xlastsize = (((ScaledWidth << 16) / 640) + 0xffff) >> 16;
    src_xdelta = (640 << 16) / ScaledWidth;
    if (640 > ScaledWidth)
    {
        src_xpos_0 = ((640 - ScaledWidth) << 16) / 640;
    }
    else
    {
        src_xpos_0 = 0;
    }

    src_ydelta = (400 << 16) / ScaledHeight;
    if (400 > ScaledHeight)
    {
        src_ypos = ((400 - ScaledHeight) << 16) / 400;
    }
    else
    {
        src_ypos = 0;
    }

    for (height = ScaledHeight; height != 0; height--)
    {
        src_xpos = src_xpos_0;

        src_y = src_ypos >> 16;
        srcline1 = ScaleSrc + 320 * (src_y >> 1);
        srcline2 = srcline1 + 319;

        if ((src_y < 399) && ((src_ypos & 0xffff) != 0) && ((src_y & 1) != 0))
        {
            // bilinear
            for (width = ScaledWidth - dst_xlastsize; width != 0; width--)
            {
                if ((src_xpos & 0x10000) != 0)
                {
                    uint32_t tmpval1, tmpval2;

                    val1 = srcline1[0];
                    val2 = srcline1[1];

                    // r
                    tmpval1 = (((int)(val1 & 0xff)) << 2) + (((((int)(val2 & 0xff)) - ((int)(val1 & 0xff))) * (int)(src_xpos & 0xffff)) >> 14);
                    // g
                    tmpval1 |= ( (((int)((val1 >> 8) & 0xff)) << 2) + (((((int)((val2 >> 8) & 0xff)) - ((int)((val1 >> 8) & 0xff))) * (int)(src_xpos & 0xffff)) >> 14) ) << 10;
                    // b
                    tmpval1 |= ( (((int)((val1 >> 16) & 0xff)) << 2) + (((((int)((val2 >> 16) & 0xff)) - ((int)((val1 >> 16) & 0xff))) * (int)(src_xpos & 0xffff)) >> 14) ) << 20;

                    val1 = srcline1[320];
                    val2 = srcline1[321];

                    // r
                    tmpval2 = (((int)(val1 & 0xff)) << 2) + (((((int)(val2 & 0xff)) - ((int)(val1 & 0xff))) * (int)(src_xpos & 0xffff)) >> 14);
                    // g
                    tmpval2 |= ( (((int)((val1 >> 8) & 0xff)) << 2) + (((((int)((val2 >> 8) & 0xff)) - ((int)((val1 >> 8) & 0xff))) * (int)(src_xpos & 0xffff)) >> 14) ) << 10;
                    // b
                    tmpval2 |= ( (((int)((val1 >> 16) & 0xff)) << 2) + (((((int)((val2 >> 16) & 0xff)) - ((int)((val1 >> 16) & 0xff))) * (int)(src_xpos & 0xffff)) >> 14) ) << 20;

                    // r
                    dstval = ( ((int)(tmpval1 & 0x3ff)) + (((((int)(tmpval2 & 0x3ff)) - ((int)(tmpval1 & 0x3ff))) * (int)(src_ypos & 0xffff)) >> 16) ) >> 2;
                    // g
                    dstval |= ( ( ((int)((tmpval1 >> 10) & 0x3ff)) + (((((int)((tmpval2 >> 10) & 0x3ff)) - ((int)((tmpval1 >> 10) & 0x3ff))) * (int)(src_ypos & 0xffff)) >> 16) ) & 0x3fc ) << 6;
                    // b
                    dstval |= ( ( ((int)((tmpval1 >> 20) & 0x3ff)) + (((((int)((tmpval2 >> 20) & 0x3ff)) - ((int)((tmpval1 >> 20) & 0x3ff))) * (int)(src_ypos & 0xffff)) >> 16) ) & 0x3fc ) << 14;

                    *dst = dstval;

                    dst++;
                    src_xpos += src_xdelta;
                    srcline1 += (src_xpos >> 17);
                    src_xpos &= 0x1ffff;
                }
                else
                {
                    val1 = srcline1[0];
                    val2 = srcline1[320];

                    // r
                    dstval = ((int)(val1 & 0xff)) + (((((int)(val2 & 0xff)) - ((int)(val1 & 0xff))) * (int)(src_ypos & 0xffff)) >> 16);
                    // g
                    dstval |= ( ((int)((val1 >> 8) & 0xff)) + (((((int)((val2 >> 8) & 0xff)) - ((int)((val1 >> 8) & 0xff))) * (int)(src_ypos & 0xffff)) >> 16) ) << 8;
                    // b
                    dstval |= ( ((int)((val1 >> 16) & 0xff)) + (((((int)((val2 >> 16) & 0xff)) - ((int)((val1 >> 16) & 0xff))) * (int)(src_ypos & 0xffff)) >> 16) ) << 16;

                    *dst = dstval;

                    dst++;
                    src_xpos += src_xdelta;
                    srcline1 += (src_xpos >> 17);
                    src_xpos &= 0x1ffff;
                }
            }

            if (dst_xlastsize != 0)
            {
                val1 = srcline2[0];
                val2 = srcline2[320];

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
                if ((src_xpos & 0x10000) != 0)
                {
                    val1 = srcline1[0];
                    val2 = srcline1[1];

                    // r
                    dstval = ((int)(val1 & 0xff)) + (((((int)(val2 & 0xff)) - ((int)(val1 & 0xff))) * (int)(src_xpos & 0xffff)) >> 16);
                    // g
                    dstval |= ( ((int)((val1 >> 8) & 0xff)) + (((((int)((val2 >> 8) & 0xff)) - ((int)((val1 >> 8) & 0xff))) * (int)(src_xpos & 0xffff)) >> 16) ) << 8;
                    // b
                    dstval |= ( ((int)((val1 >> 16) & 0xff)) + (((((int)((val2 >> 16) & 0xff)) - ((int)((val1 >> 16) & 0xff))) * (int)(src_xpos & 0xffff)) >> 16) ) << 16;

                    *dst = dstval;

                    dst++;
                    src_xpos += src_xdelta;
                    srcline1 += (src_xpos >> 17);
                    src_xpos &= 0x1ffff;
                }
                else
                {
                    *dst = *srcline1;

                    dst++;
                    src_xpos += src_xdelta;
                    srcline1 += (src_xpos >> 17);
                    src_xpos &= 0x1ffff;
                }
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
    Font_Size_Shift = 2;

    ScaleOutput = 0;
    ScaledWidth = 640;
    ScaledHeight = 400;
    Fullscreen = 0;
    ScaleSrc = NULL;
}

void Init_Display2(void)
{
    Init_Palette();

    if ((ScaledWidth != 640) || (ScaledHeight != 400))
    {
        ScaleOutput = 1;

    #if defined(USE_SDL2)
    #elif defined(ALLOW_OPENGL)
        Game_UseOpenGL = 1;
    #else
        ScaleSrc = (uint32_t *) malloc(320*200*4+4);
        if (ScaleSrc == NULL)
        {
            ScaleOutput = 0;
            ScaledWidth = 640;
            ScaledHeight = 400;
        }
    #endif
    }

    Display_Width = ScaledWidth;
    Display_Height = ScaledHeight;
    Display_Bitsperpixel = 32;
    Display_Fullscreen = Fullscreen;
    Display_MouseLocked = 0;
    Render_Width = 640;
    Render_Height = 400;
    Picture_Width = ScaledWidth;
    Picture_Height = ScaledHeight;
    Picture_Position_UL_X = 0;
    Picture_Position_UL_Y = 0;
    Picture_Position_BR_X = ScaledWidth-1;
    Picture_Position_BR_Y = ScaledHeight-1;
#if !defined(ALLOW_OPENGL) && !defined(USE_SDL2)
    if (ScaleOutput)
    {
        Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_320x200x8_to_WxHx32_bilinear;
    }
    else
#endif
    {
        Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_320x200x8_to_640x400x32;
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
            sscanf(param, "%i", &num_int);
            if (num_int > 0)
            {
                ScaledWidth = num_int;
                if (ScaledWidth < 640) ScaledWidth = 640;
            }

            return 1;
        }
        else if ( strcasecmp(str, "ScaledHeight") == 0)	// str equals "ScaledHeight"
        {
            num_int = 0;
            sscanf(param, "%i", &num_int);
            if (num_int > 0)
            {
                ScaledHeight = num_int;
                if (ScaledHeight < 480) ScaledHeight = 480;
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

void Reposition_Display(void)
{
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
    return 0;
}

