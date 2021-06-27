/**
 *
 *  Copyright (C) 2020-2021 Roman Pauer
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

#include "BBTXT.h"
#include "BBOPM.h"
#include "BBLBL.h"
#include "BBMEM.h"
#include <stddef.h>


typedef struct {
    unsigned int unknown_00;
    GFX_struct *char_gfx[256];
} TXT_struct;


static int TXT_initialized = 0;
static TXT_struct *TXT_fonts[32];


int TXT_Init_c(void)
{
    int index;

    if (!TXT_initialized)
    {
        for (index = 0; index < 32; index++)
        {
            TXT_fonts[index] = NULL;
        }
    }

    TXT_initialized++;
    return 1;
}

void TXT_Exit_c(void)
{
    //int index;

    TXT_initialized--;
    if (TXT_initialized > 0)
    {
        return;
    }

    TXT_UnloadAllFonts_c();
    /*for (index = 0; index < 32; index++)
    {
        if (TXT_fonts[index] != NULL)
        {
            TXT_UnloadFont_c(index);
        }
    }*/
}

int TXT_LoadFont_c(const char *path)
{
    int error, font_handle, index, num_entries;
    void *lib;
    TXT_struct *font;
    GFX_struct *gfx;
    uint8_t metadata[8];

    error = 0;
    for (font_handle = 0; font_handle < 32; font_handle++)
    {
        if (TXT_fonts[font_handle] == NULL) break;
    }

    if (font_handle >= 32)
    {
        return -1;
    }

    lib = LBL_OpenLib_c(path, 0);
    if (lib == NULL)
    {
        return -1;
    }

    font = (TXT_struct *)MEM_malloc_c(sizeof(TXT_struct), NULL, NULL, 0, 0);
    if (font == NULL)
    {
        LBL_CloseLib_c(lib);
        return -1;
    }

    for (index = 0; index < 256; index++)
    {
        font->char_gfx[index] = NULL;
    }

    num_entries = LBL_GetNOFEntries_c(lib);

    for (index = 0; index < num_entries; index++)
    {
        gfx = (GFX_struct *)LBL_ReadEntry_c(lib, NULL, index, 0, metadata);
        if (gfx == NULL)
        {
            error = 1;
            break;
        }

        font->char_gfx[metadata[0]] = gfx;
        font->unknown_00 = metadata[1];

        gfx->transparent_color = 0xff;
    }

    if (error || (font->char_gfx[32] == NULL))
    {
        for (index = 0; index < 256; index++)
        {
            if (font->char_gfx[index] != NULL)
            {
                MEM_free_c(font->char_gfx[index]);
            }
        }

        MEM_free_c(font);
        LBL_CloseLib_c(lib);
        return -1;
    }


    for (index = 0; index < 256; index++)
    {
        if (font->char_gfx[index] == NULL)
        {
            font->char_gfx[index] = font->char_gfx[32];
        }
    }

    TXT_fonts[font_handle] = font;
    LBL_CloseLib_c(lib);

    return font_handle;
}

static void TXT_UnloadFont_c(int font_handle)
{
    TXT_struct *font;
    int index;

    if ((font_handle < 0) || (font_handle >= 32))
    {
        return;
    }

    font = TXT_fonts[font_handle];
    if (font == NULL)
    {
        return;
    }

    for (index = 0; index < 256; index++)
    {
        if ((index != 32) && (font->char_gfx[index] == font->char_gfx[32]))
        {
            font->char_gfx[index] = NULL;
        }
    }

    for (index = 0; index < 256; index++)
    {
        if (font->char_gfx[index] != NULL)
        {
            MEM_free_c(font->char_gfx[index]);
        }
    }

    MEM_free_c(font);
    TXT_fonts[font_handle] = NULL;
}

void TXT_WriteString_c(const char *text, int font_handle, void *dst_pixel_map, int x, int y, int width, int height, uint8_t color_add)
{
    TXT_struct *font;
    OPM_struct *pixel_map;
    int16_t orig_clip_x, orig_clip_y, orig_clip_width, orig_clip_height;
    int index;
    GFX_struct *gfx;

    if ((font_handle < 0) || (font_handle >= 32))
    {
        return;
    }

    font = (TXT_struct *)TXT_fonts[font_handle];
    if (font == NULL)
    {
        return;
    }

    if ((text == NULL) || (dst_pixel_map == NULL))
    {
        return;
    }

    pixel_map = (OPM_struct *)dst_pixel_map;

    orig_clip_x = pixel_map->clip_x;
    orig_clip_y = pixel_map->clip_y;
    orig_clip_width = pixel_map->clip_width;
    orig_clip_height = pixel_map->clip_height;

    pixel_map->clip_x = x;
    pixel_map->clip_y = y;
    pixel_map->clip_width = width;
    pixel_map->clip_height = height;

    if (pixel_map->clip_x < 0) pixel_map->clip_x = 0;
    if (pixel_map->clip_y < 0) pixel_map->clip_y = 0;
    if (pixel_map->clip_x > pixel_map->width) pixel_map->clip_x = pixel_map->width;
    if (pixel_map->clip_y > pixel_map->height) pixel_map->clip_y = pixel_map->height;
    if (pixel_map->clip_width < 0) pixel_map->clip_width = 0;
    if (pixel_map->clip_height < 0) pixel_map->clip_height = 0;
    if (pixel_map->clip_width > pixel_map->width) pixel_map->clip_width = pixel_map->width;
    if (pixel_map->clip_height > pixel_map->height) pixel_map->clip_height = pixel_map->height;

    for (index = 0; text[index] != 0; index++)
    {
        gfx = font->char_gfx[(uint8_t)text[index]];
        OPM_CopyGFXOPM_c(pixel_map, gfx, x, y, color_add);
        x += gfx->width;
    }

    pixel_map->clip_x = orig_clip_x;
    pixel_map->clip_y = orig_clip_y;
    pixel_map->clip_width = orig_clip_width;
    pixel_map->clip_height = orig_clip_height;
}

void TXT_UnloadAllFonts_c(void)
{
    int index;

    for (index = 0; index < 32; index++)
    {
        if (TXT_fonts[index] != NULL)
        {
            TXT_UnloadFont_c(index);
        }
    }
}

