/**
 *
 *  Copyright (C) 2020-2026 Roman Pauer
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

#include "BBOPM.h"
#include "BBLL.h"
#include "BBMEM.h"
#include "BBDSA.h"
#include "WinApi-wing32.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


extern unsigned int DSA_pixelMapList;
extern OPM_struct DSA_pixelMapBG;
extern RGBQUAD DSA_colorTable[256];


typedef union {
    BITMAPINFO bmi;
    struct {
        BITMAPINFOHEADER header2;
        RGBQUAD colors2[256];
    } bmi2;
} myBITMAPINFO;


static int OPM_initialized = 0;
static myBITMAPINFO OPM_bitmapInfo;


int CCALL OPM_Init_c(void)
{
    if (OPM_initialized)
    {
        return 1;
    }

    OPM_initialized = 1;
    return 1;
}

void CCALL OPM_Exit_c(void)
{
    OPM_struct *pixel_map;

    if (!OPM_initialized)
    {
        return;
    }

    LL_Reset_c(DSA_pixelMapList);
    while (1)
    {
        pixel_map = (OPM_struct *)LL_GetData_c(DSA_pixelMapList);
        if (pixel_map == NULL)
        {
            break;
        }

        if ((!(pixel_map->flags & BBOPM_VIRTUAL)) && (pixel_map->flags & BBOPM_OWNED_BUFFER))
        {
            if (pixel_map->flags & BBOPM_SECONDARY)
            {
                MEM_free_c(pixel_map->buffer);
            }
            else
            {
                SelectObject((HDC)pixel_map->hDC, (HGDIOBJ)pixel_map->hSelected);
                DeleteObject((HGDIOBJ)pixel_map->hBitmap);
                DeleteDC((HDC)pixel_map->hDC);
            }
        }

        LL_GotoNext_c(DSA_pixelMapList);
    }

    OPM_initialized = 0;
}

int CCALL OPM_New_c(unsigned int width, unsigned int height, unsigned int bytes_per_pixel, OPM_struct *pixel_map, uint8_t *buffer)
{
    RGBQUAD *colors;
    int index;
    char msg[128];

    width = (uint16_t)width;
    height = (uint16_t)height;
    bytes_per_pixel = (uint16_t)bytes_per_pixel;


    pixel_map->flags = 0;

    colors = &(OPM_bitmapInfo.bmi.bmiColors[0]);
    // change: 255 replaced with 256
    //for (index = 0; index < 255; index++)
    for (index = 0; index < 256; index++)
    {
        colors[index].rgbRed = index;
        colors[index].rgbGreen = index;
        colors[index].rgbBlue = index;
        colors[index].rgbReserved = 0;
    }

    OPM_bitmapInfo.bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    OPM_bitmapInfo.bmi.bmiHeader.biWidth = 1;
    OPM_bitmapInfo.bmi.bmiHeader.biHeight = 1;
    OPM_bitmapInfo.bmi.bmiHeader.biPlanes = 1;
    OPM_bitmapInfo.bmi.bmiHeader.biBitCount = 8;

    if ( buffer != NULL)
    {
        pixel_map->buffer = buffer;
        pixel_map->hDC = NULL;
        pixel_map->hBitmap = NULL;
        pixel_map->hSelected = NULL;
    }
    else
    {
        pixel_map->hDC = WinGCreateDC_c();
        if (pixel_map->hDC == NULL)
        {
            BBMEM_FreeMemory_c(2);
            pixel_map->hDC = WinGCreateDC_c();
        }
        if (pixel_map->hDC == NULL)
        {
            // change: bytes_per_pixel replaced with height
            //wsprintfA(msg, "WinGCreateDC: Widht: %d  Height: %d", width, bytes_per_pixel);
            wsprintfA(msg, "WinGCreateDC: Widht: %d  Height: %d", width, height);
            MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), msg, "WING ERROR", MB_OK | MB_ICONERROR);
            MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), "WINDOWS meldet zuwenig freien Speicher.Beenden Sie einige Anwendungen und starten Sie Battle Isle 3 erneut. Ihr momentaner Spielstand ist in Slot 9 oder 10 gesichert.", "BITMAP 1 HANDLER", MB_OK | MB_ICONERROR);
            return 0;
        }

        OPM_bitmapInfo.bmi.bmiHeader.biWidth = width;
        OPM_bitmapInfo.bmi.bmiHeader.biHeight = -(int)height;

        pixel_map->hBitmap = WinGCreateBitmap_c((HDC)pixel_map->hDC, &(OPM_bitmapInfo.bmi), (void **)&buffer);
        if (pixel_map->hBitmap == NULL)
        {
            BBMEM_FreeMemory_c(1);
            pixel_map->hBitmap = WinGCreateBitmap_c((HDC)pixel_map->hDC, &(OPM_bitmapInfo.bmi), (void **)&buffer);
        }
        if (pixel_map->hBitmap == NULL)
        {
            // change: bytes_per_pixel replaced with height
            //wsprintfA(msg, "WinGCreateBitmap: Widht: %d  Height: %d", width, bytes_per_pixel);
            wsprintfA(msg, "WinGCreateBitmap: Widht: %d  Height: %d", width, height);
            MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), msg, "WING ERROR", MB_OK | MB_ICONERROR);
            MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), "WINDOWS meldet zuwenig freien Speicher.Beenden Sie einige Anwendungen und starten Sie Battle Isle 3 erneut. Ihr momentaner Spielstand ist in Slot 9 oder 10 gesichert.", "BITMAP 2 HANDLER", MB_OK | MB_ICONERROR);
            DeleteDC((HDC)pixel_map->hDC);
            pixel_map->hDC = NULL;
            return 0;
        }
        pixel_map->buffer = buffer;

        pixel_map->hSelected = SelectObject((HDC)pixel_map->hDC, (HGDIOBJ)pixel_map->hBitmap);
        if (pixel_map->hSelected == NULL)
        {
            // change: bytes_per_pixel replaced with height
            //wsprintfA(msg, "SelectObject: Widht: %d  Height: %d", width, bytes_per_pixel);
            wsprintfA(msg, "SelectObject: Widht: %d  Height: %d", width, height);
            MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), msg, "WING ERROR", MB_OK | MB_ICONERROR);
            MessageBoxA((HWND)DSAWIN_GetMainWindowHandle_c(), "WINDOWS meldet zuwenig freien Speicher.Beenden Sie einige Anwendungen und starten Sie Battle Isle 3 erneut. Ihr momentaner Spielstand ist in Slot 9 oder 10 gesichert.", "BITMAP 3 HANDLER", MB_OK | MB_ICONERROR);
            DeleteObject((HGDIOBJ)pixel_map->hBitmap);
            DeleteDC((HDC)pixel_map->hDC);
            pixel_map->hDC = NULL;
            return 0;
        }

        LL_AppendElement_c(DSA_pixelMapList, pixel_map);
        WinGSetDIBColorTable_c((HDC)pixel_map->hDC, 0, 256, DSA_colorTable);
        pixel_map->flags |= BBOPM_OWNED_BUFFER;
    }

    pixel_map->width = width;
    pixel_map->height = height;
    pixel_map->size = width * height;
    pixel_map->bytes_per_pixel = bytes_per_pixel;
    pixel_map->clip_x = 0;
    pixel_map->clip_y = 0;
    pixel_map->clip_width = width;
    pixel_map->clip_height = height;
    pixel_map->origin_x = 0;
    pixel_map->origin_y = 0;
    pixel_map->bytes_per_pixel2 = bytes_per_pixel;
    pixel_map->stride = width * bytes_per_pixel;
    pixel_map->flags |= BBOPM_CREATED | BBOPM_MODIFIED;
    pixel_map->access_offset = 0;

    return 1;
}

void CCALL OPM_Del_c(OPM_struct *pixel_map)
{
    OPM_struct *pm2;

    if ((!(pixel_map->flags & BBOPM_VIRTUAL)) && (pixel_map->flags & BBOPM_OWNED_BUFFER))
    {
        if (pixel_map->flags & BBOPM_SECONDARY)
        {
            MEM_free_c(pixel_map->buffer);
        }
        else
        {
            LL_Reset_c(DSA_pixelMapList);
            while (1)
            {
                pm2 = (OPM_struct *)LL_GetData_c(DSA_pixelMapList);
                if (pm2 == NULL)
                {
                    break;
                }

                if (pm2 == pixel_map)
                {
                    LL_DeleteElement_c(DSA_pixelMapList);
                    break;
                }

                LL_GotoNext_c(DSA_pixelMapList);
            }

            SelectObject((HDC)pixel_map->hDC, (HGDIOBJ)pixel_map->hSelected);
            DeleteObject((HGDIOBJ)pixel_map->hBitmap);
            DeleteDC((HDC)pixel_map->hDC);
        }
    }

    pixel_map->flags = 0;
}

void CCALL OPM_CreateVirtualOPM_c(OPM_struct *base_pixel_map, OPM_struct *virtual_pixel_map, int virtual_x, int virtual_y, int virtual_width, int virtual_height)
{
    virtual_x = (uint16_t)virtual_x;
    virtual_y = (uint16_t)virtual_y;
    virtual_width = (uint16_t)virtual_width;
    virtual_height = (uint16_t)virtual_height;

    virtual_pixel_map->flags = BBOPM_VIRTUAL | BBOPM_CREATED | BBOPM_MODIFIED;
    virtual_pixel_map->virtual_x = virtual_x;
    virtual_pixel_map->virtual_y = virtual_y;
    virtual_pixel_map->base_pixel_map = base_pixel_map;
    virtual_pixel_map->width = virtual_width;
    virtual_pixel_map->height = virtual_height;
    virtual_pixel_map->bytes_per_pixel = base_pixel_map->bytes_per_pixel;
    virtual_pixel_map->size = base_pixel_map->size;
    virtual_pixel_map->clip_x = 0;
    virtual_pixel_map->clip_y = 0;
    virtual_pixel_map->clip_width = virtual_width;
    virtual_pixel_map->clip_height = virtual_height;
    virtual_pixel_map->origin_x = 0;
    virtual_pixel_map->origin_y = 0;
    virtual_pixel_map->bytes_per_pixel2 = base_pixel_map->bytes_per_pixel;
    virtual_pixel_map->stride = base_pixel_map->stride;
    virtual_pixel_map->buffer = &(base_pixel_map->buffer[virtual_y * virtual_pixel_map->stride + virtual_x]);
    virtual_pixel_map->hDC = base_pixel_map->hDC;
    virtual_pixel_map->hBitmap = base_pixel_map->hBitmap;
    virtual_pixel_map->hSelected = base_pixel_map->hSelected;
    virtual_pixel_map->access_offset = 0;
}

int CCALL OPM_CreateSecondaryOPM_c(int width, int height, int bytes_per_pixel, OPM_struct *pixel_map, uint8_t *buffer)
{
    width = (uint16_t)width;
    height = (uint16_t)height;
    bytes_per_pixel = (uint16_t)bytes_per_pixel;

    pixel_map->flags = 0;
    if (buffer != NULL)
    {
        pixel_map->buffer = buffer;
        pixel_map->hDC = NULL;
        pixel_map->hBitmap = NULL;
        pixel_map->hSelected = NULL;
    }
    else
    {
        pixel_map->hDC = NULL;
        pixel_map->hBitmap = NULL;
        pixel_map->hSelected = NULL;
        pixel_map->buffer = (uint8_t *)MEM_malloc_c(height * width * bytes_per_pixel, NULL, NULL, 0, 0);
        if (pixel_map->buffer == NULL)
        {
            return 0;
        }

        pixel_map->flags |= BBOPM_OWNED_BUFFER;
    }
    pixel_map->flags |= BBOPM_SECONDARY;
    pixel_map->width = width;
    pixel_map->height = height;
    pixel_map->size = height * width;
    pixel_map->bytes_per_pixel = bytes_per_pixel;
    pixel_map->clip_x = 0;
    pixel_map->clip_y = 0;
    pixel_map->clip_width = width;
    pixel_map->clip_height = height;
    pixel_map->origin_x = 0;
    pixel_map->origin_y = 0;
    pixel_map->bytes_per_pixel2 = bytes_per_pixel;
    pixel_map->stride = width * bytes_per_pixel;
    pixel_map->access_offset = 0;
    pixel_map->flags |= BBOPM_CREATED | BBOPM_MODIFIED;

    return 1;
}

void CCALL OPM_SetPixel_c(OPM_struct *pixel_map, int x, int y, uint8_t color)
{
    x = (int16_t)(x + pixel_map->origin_x);
    y = (int16_t)(y + pixel_map->origin_y);

    if ((x >= pixel_map->clip_x) &&
        (x < pixel_map->clip_x + pixel_map->clip_width) &&
        (y >= pixel_map->clip_y) &&
        (y < pixel_map->clip_y + pixel_map->clip_height)
       )
    {
        pixel_map->flags |= BBOPM_MODIFIED;
        pixel_map->buffer[y * pixel_map->stride + x] = color;
    }
}

void CCALL OPM_HorLine_c(OPM_struct *pixel_map, int x, int y, unsigned int length, uint8_t color)
{
    int length2, index;

    x = (int16_t)(x + pixel_map->origin_x);
    y = (int16_t)(y + pixel_map->origin_y);
    length = (uint16_t)length;

    if ((y < pixel_map->clip_y) || (y >= pixel_map->clip_y + pixel_map->clip_height) || (x < pixel_map->clip_x))
    {
        return;
    }

    length2 = pixel_map->clip_x + pixel_map->clip_width - x;
    if (length2 >= (int)length)
    {
        length2 = length;
    }

    if (length2 > 0)
    {
        for (index = 0; index < length2; index++)
        {
            pixel_map->buffer[y * pixel_map->stride + (x + index)] = color;
        }

        pixel_map->flags |= BBOPM_MODIFIED;
    }
}

void CCALL OPM_VerLine_c(OPM_struct *pixel_map, int x, int y, unsigned int length, uint8_t color)
{
    int length2, index;

    x = (int16_t)(x + pixel_map->origin_x);
    y = (int16_t)(y + pixel_map->origin_y);
    length = (uint16_t)length;

    if ((x < pixel_map->clip_x) || (x >= pixel_map->clip_x + pixel_map->clip_width) || (y < pixel_map->clip_y))
    {
        return;
    }

    length2 = pixel_map->clip_y + pixel_map->clip_height - y;
    if (length2 >= (int)length)
    {
        length2 = length;
    }

    if (length2 > 0)
    {
        for (index = 0; index < length2; index++)
        {
            pixel_map->buffer[(y + index) * pixel_map->stride + x] = color;
        }

        pixel_map->flags |= BBOPM_MODIFIED;
    }
}

void CCALL OPM_FillBox_c(OPM_struct *pixel_map, int x, int y, unsigned int width, unsigned int height, uint8_t color)
{
    unsigned int index;

    height = (uint16_t)height;

    if (pixel_map == NULL)
    {
        if (!(DSA_pixelMapBG.flags & BBOPM_CREATED))
        {
            return;
        }

        pixel_map = &DSA_pixelMapBG;
    }

    for (index = 0; index < height; index++)
    {
        OPM_HorLine_c(pixel_map, x, y + index, width, color);
    }
}

void CCALL OPM_CopyGFXOPM_c(OPM_struct *pixel_map, GFX_struct *gfx, int pos_x, int pos_y, uint8_t value_add)
{
    int clip_x, clip_y, clip_endx, clip_endy;
    int dst_x, dst_y, dst_width, dst_height;
    int gfx_width, dst_stride, dst_diff, src_diff;
    int counter_x, counter_y;
    uint8_t *src_ptr, *dst_ptr, transparent_color, src_value;

    pos_x = (int16_t)pos_x;
    pos_y = (int16_t)pos_y;


    if (pixel_map == NULL)
    {
        if (!(DSA_pixelMapBG.flags & BBOPM_CREATED))
        {
            return;
        }
        pixel_map = &DSA_pixelMapBG;
    }

    transparent_color = gfx->transparent_color;
    dst_width = gfx->width;
    dst_height = gfx->height;
    gfx_width = dst_width;
    src_ptr = &(gfx->data[0]);

    dst_stride = pixel_map->stride;
    dst_x = pos_x + gfx->pos_x;
    dst_y = pos_y + gfx->pos_y;

    clip_x = pixel_map->clip_x;
    clip_y = pixel_map->clip_y;
    clip_endx = clip_x + pixel_map->clip_width;
    clip_endy = clip_y + pixel_map->clip_height;

    if (dst_x < clip_x)
    {
        if (clip_x - dst_x > dst_width)
        {
            return;
        }
        dst_width -= clip_x - dst_x;
        src_ptr += clip_x - dst_x;
        dst_x = pixel_map->clip_x;
    }

    if (dst_y < clip_y)
    {
        if (clip_y - dst_y > dst_height)
        {
            return;
        }
        dst_height -= clip_y - dst_y;
        src_ptr += gfx_width * (clip_y - dst_y);
        dst_y = pixel_map->clip_y;
    }

    if (dst_x + dst_width > clip_endx)
    {
        if (dst_x > clip_endx)
        {
            return;
        }
        dst_width -= (dst_x + dst_width) - clip_endx;
    }

    if (dst_y + dst_height > clip_endy)
    {
        if (dst_y > clip_endy)
        {
            return;
        }
        dst_height -= (dst_y + dst_height) - clip_endy;
    }

    if ((dst_width < 1) || (dst_height < 1)) return;


    dst_diff = dst_stride - dst_width;
    src_diff = gfx_width - dst_width;
    dst_ptr = &(pixel_map->buffer[dst_y * dst_stride + dst_x]);

    for (counter_y = dst_height; counter_y != 0; counter_y--)
    {
        for (counter_x = dst_width; counter_x != 0; counter_x--)
        {
            src_value = *src_ptr;
            src_ptr++;
            if (src_value != transparent_color)
            {
                *dst_ptr = value_add + src_value;
            }
            dst_ptr++;
        }

        dst_ptr += dst_diff;
        src_ptr += src_diff;
    }

    pixel_map->flags |= BBOPM_MODIFIED;
}

void CCALL OPM_CopyOPMOPM_c(OPM_struct *src_pixel_map, OPM_struct *dst_pixel_map, int src_x, int src_y, int copy_width, int copy_height, int dst_x, int dst_y)
{
    int clip_x, clip_y, clip_endx, clip_endy;
    int src_add_x, src_add_y, src_width, src_height;
    int dst_add_x, dst_add_y, dst_width, dst_height;
    int max_add_x, max_add_y, min_width, min_height;
    int counter, dst_stride, src_stride;
    uint8_t *dst_ptr, *src_ptr;

    src_x = (int16_t)src_x;
    src_y = (int16_t)src_y;
    copy_width = (int16_t)copy_width;
    copy_height = (int16_t)copy_height;
    dst_x = (int16_t)dst_x;
    dst_y = (int16_t)dst_y;


    if (src_pixel_map == NULL)
    {
        if (!(DSA_pixelMapBG.flags & BBOPM_CREATED))
        {
            return;
        }
        src_pixel_map = &DSA_pixelMapBG;
    }

    if (dst_pixel_map == NULL)
    {
        if (!(DSA_pixelMapBG.flags & BBOPM_CREATED))
        {
            return;
        }
        dst_pixel_map = &DSA_pixelMapBG;
    }

    src_width = copy_width;
    src_height = copy_height;
    src_add_x = 0;
    src_add_y = 0;
    dst_width = copy_width;
    dst_height = copy_height;
    dst_add_x = 0;
    dst_add_y = 0;

    clip_x = src_pixel_map->clip_x;
    clip_y = src_pixel_map->clip_y;
    clip_endx = clip_x + src_pixel_map->clip_width;
    clip_endy = clip_y + src_pixel_map->clip_height;

    if (src_x < clip_x)
    {
        if (clip_x - src_x > copy_width)
        {
            return;
        }
        src_width = copy_width - (clip_x - src_x);
        src_add_x = clip_x - src_x;
        src_x = src_pixel_map->clip_x;
    }

    if (src_y < clip_y)
    {
        if (clip_y - src_y > copy_height)
        {
            return;
        }
        src_height = copy_height - (clip_y - src_y);
        src_add_y = clip_y - src_y;
        src_y = src_pixel_map->clip_y;
    }

    if (src_x + src_width > clip_endx)
    {
        if (src_x > clip_endx)
        {
            return;
        }
        src_width -= (src_x + src_width) - clip_endx;
    }

    if (src_y + src_height > clip_endy)
    {
        if (src_y > clip_endy)
        {
            return;
        }
        src_height -= (src_y + src_height) - clip_endy;
    }

    if ((src_width < 1) || (src_height < 1)) return;


    clip_x = dst_pixel_map->clip_x;
    clip_y = dst_pixel_map->clip_y;
    clip_endx = clip_x + dst_pixel_map->clip_width;
    clip_endy = clip_y + dst_pixel_map->clip_height;

    if (dst_x < clip_x)
    {
        if (clip_x - dst_x > copy_width)
        {
            return;
        }
        dst_width = copy_width - (clip_x - dst_x);
        dst_add_x = clip_x - dst_x;
        dst_x = dst_pixel_map->clip_x;
    }

    if (dst_y < clip_y)
    {
        if (clip_y - dst_y > copy_height)
        {
            return;
        }
        dst_height = copy_height - (clip_y - dst_y);
        dst_add_y = clip_y - dst_y;
        dst_y = dst_pixel_map->clip_y;
    }

    if (dst_x + dst_width > clip_endx)
    {
        if (dst_x > clip_endx)
        {
            return;
        }
        dst_width -= (dst_x + dst_width) - clip_endx;
    }

    if (dst_y + dst_height > clip_endy)
    {
        if (dst_y > clip_endy)
        {
            return;
        }
        dst_height -= (dst_y + dst_height) - clip_endy;
    }

    if ((dst_width < 1) || (dst_height < 1)) return;

    max_add_x = (dst_add_x >= src_add_x)?dst_add_x:src_add_x;
    max_add_y = (dst_add_y >= src_add_y)?dst_add_y:src_add_y;
    min_width = (dst_width <= src_width)?dst_width:src_width;
    min_height = (dst_height <= src_height)?dst_height:src_height;

    if ((copy_width <= max_add_x) || (copy_height <= max_add_y)) return;


    dst_stride = dst_pixel_map->stride;
    src_stride = src_pixel_map->stride;
    dst_ptr = &(dst_pixel_map->buffer[(src_add_y + dst_y) * dst_pixel_map->stride + (src_add_x + dst_x)]);
    src_ptr = &(src_pixel_map->buffer[(dst_add_y + src_y) * src_pixel_map->stride + (dst_add_x + src_x)]);

    for (counter = min_height; counter != 0; counter--)
    {
        memcpy(dst_ptr, src_ptr, min_width);
        dst_ptr += dst_stride;
        src_ptr += src_stride;
    }

    dst_pixel_map->flags |= BBOPM_MODIFIED;
}

void CCALL OPM_AccessBitmap_c(OPM_struct *pixel_map)
{
    uint8_t *ptr;
    uint8_t value;

    ptr = &pixel_map->buffer[pixel_map->access_offset];
    value = *ptr;
    *ptr = 0xAA;
    *ptr = value;

    pixel_map->access_offset += 2048;
    if (pixel_map->access_offset >= pixel_map->size)
    {
        pixel_map->access_offset = 0;
    }
}

