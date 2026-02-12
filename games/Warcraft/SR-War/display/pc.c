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
#include <memory.h>
#include <string.h>

static int ScaledWidth, ScaledHeight, Fullscreen;

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

void Init_Display(void)
{
    Display_FSType = 1;

    ScaledWidth = 640;
    ScaledHeight = 400;
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
            ScaledHeight = 400;
        }
    }
    else
    {
        if (ScaledWidth < 640) ScaledWidth = 640;
        if (ScaledHeight < 400) ScaledHeight = 400;
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
    if (Game_AdvancedScaling)
    {
        Render_Width = 320;
        Render_Height = 200;
        Display_Flip_Procedure = (Game_Flip_Procedure) &Flip_320x200x8_to_320x200x32;
    }
    else
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

