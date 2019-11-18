/**
 *
 *  Copyright (C) 2018-2019 Roman Pauer
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

#include "Albion-BBOPM.h"
#include "Albion-BBBASMEM.h"
#include "Albion-BBERROR.h"
#include <stdio.h>
#include <string.h>


typedef struct {
    const char *text;
    int data1;
    int data2;
} OPM_ErrorStruct;


static void OPM_LocalPrintError(char *buffer, const uint8_t *data);

static void ASM_DrawFilledRectangle(uint8_t *dst, uint8_t color, unsigned int stridediff, int width, int height);
static void ASM_DrawVerticalLine(uint8_t *dst, uint8_t color, int linelength, unsigned int stride);
static void ASM_DrawHorizontalLine(uint8_t *dst, uint8_t color, int linelength, unsigned int stride);
static void ASM_CopyRectangle(uint8_t *dst, uint8_t *src, unsigned int srcstridediff, unsigned int dststridediff, int width, int height);
static void ASM_CopyRectangleWithTransparency(uint8_t *dst, uint8_t *src, uint8_t transparent_color, unsigned int srcstridediff, unsigned int dststridediff, int width, int height);


int OPM_New(unsigned int width, unsigned int height, unsigned int bytes_per_pixel, OPM_Struct *pixel_map, uint8_t *buffer)
{
    unsigned int size;

    pixel_map->flags = 0;
    pixel_map->view_x = 0;
    pixel_map->view_y = 0;

    size = width * height * bytes_per_pixel;
    pixel_map->size = size;
    if (buffer == NULL)
    {
        pixel_map->buffer = (uint8_t *) BASEMEM_Alloc(size, BASEMEM_XMS_MEMORY | BASEMEM_ZERO_MEMORY);
        if (pixel_map->buffer == NULL)
        {
            OPM_ErrorStruct data;

            data.text = "OPM_New: Cannot Allocate Mem for PixelMap width,height ";
            data.data1 = width;
            data.data2 = height;

            ERROR_PushError(OPM_LocalPrintError, "BBOPM Library", sizeof(data), (const uint8_t *) &data);

            return 0;
        }

        memset(pixel_map->buffer, 0, size);
        pixel_map->flags |= BBOPM_ALLOCATED_BUFFER;
    }
    else
    {
        pixel_map->buffer = buffer;
    }

    pixel_map->width = width;
    pixel_map->height = height;
    pixel_map->bytes_per_pixel = bytes_per_pixel;
    pixel_map->clip_x = 0;
    pixel_map->clip_y = 0;
    pixel_map->clip_width = width;
    pixel_map->clip_height = height;
    pixel_map->origin_x = 0;
    pixel_map->origin_y = 0;
    pixel_map->bytes_per_pixel2 = bytes_per_pixel;
    pixel_map->stride = width * bytes_per_pixel;
    pixel_map->flags |= BBOPM_UNKNOWN4 | BBOPM_MODIFIED;

    return 1;
}

void OPM_Del(OPM_Struct *pixel_map)
{
    if (!(pixel_map->flags & BBOPM_VIEW))
    {
        if (pixel_map->flags & BBOPM_ALLOCATED_BUFFER)
        {
            BASEMEM_Free(pixel_map->buffer);
        }
    }

    pixel_map->flags = 0;
}

void OPM_SetViewClipStart(OPM_Struct *view_pixel_map, int clip_x, int clip_y)
{
    OPM_Struct *base_pixel_map;
    int clip_width, clip_height;

    if (!(view_pixel_map->flags & BBOPM_VIEW)) return;

    base_pixel_map = view_pixel_map->base_pixel_map;
    clip_width = view_pixel_map->width;
    clip_height = view_pixel_map->height;

    if (clip_x < 0)
    {
        clip_width += clip_x;
        clip_x = 0;
    }

    if (clip_x + clip_width >= base_pixel_map->width)
    {
        clip_width = base_pixel_map->width - clip_x;
    }

    if (clip_y < 0)
    {
        clip_height += clip_y;
        clip_y = 0;
    }

    if (clip_y + clip_height >= base_pixel_map->height)
    {
        clip_height = base_pixel_map->height - clip_y;
    }

    view_pixel_map->clip_x = clip_x;
    view_pixel_map->clip_y = clip_y;
    view_pixel_map->clip_width = clip_width;
    view_pixel_map->clip_height = clip_height;

    view_pixel_map->buffer = base_pixel_map->buffer + view_pixel_map->stride * clip_y + clip_x;
}

void OPM_CreateVirtualOPM(OPM_Struct *base_pixel_map, OPM_Struct *view_pixel_map, int view_x, int view_y, int view_width, int view_height)
{
    view_pixel_map->flags = BBOPM_VIEW | BBOPM_UNKNOWN4 | BBOPM_MODIFIED;
    view_pixel_map->view_x = view_x;
    view_pixel_map->view_y = view_y;
    view_pixel_map->width = view_width;
    view_pixel_map->height = view_height;
    view_pixel_map->origin_x = 0;
    view_pixel_map->origin_y = 0;

    if (view_x < base_pixel_map->clip_x)
    {
        view_width -= base_pixel_map->clip_x - view_x;
        view_pixel_map->origin_x = -(base_pixel_map->clip_x - view_x);
        view_x = base_pixel_map->clip_x;
        if (view_width < 0)
        {
            view_width = 0;
        }
    }

    if (view_x + view_width > base_pixel_map->clip_x + base_pixel_map->clip_width)
    {
        view_width -= (view_x + view_width) - (base_pixel_map->clip_x + base_pixel_map->clip_width);
        if (view_width < 0)
        {
            view_width = 0;
            view_x = base_pixel_map->clip_x + base_pixel_map->clip_width;
        }
    }

    if (view_y < base_pixel_map->clip_y)
    {
        view_height -= base_pixel_map->clip_y - view_y;
        view_pixel_map->origin_y = -(base_pixel_map->clip_y - view_y);
        view_y = base_pixel_map->clip_y;
        if (view_height < 0)
        {
            view_height = 0;
        }
    }

    if (view_y + view_height > base_pixel_map->clip_y + base_pixel_map->clip_height)
    {
        view_height -= (view_y + view_height) - (base_pixel_map->clip_y + base_pixel_map->clip_height);
        if (view_height < 0)
        {
            view_height = 0;
            view_y = base_pixel_map->clip_y + base_pixel_map->clip_height;
        }
    }

    view_pixel_map->base_pixel_map = base_pixel_map;
    view_pixel_map->bytes_per_pixel = base_pixel_map->bytes_per_pixel;
    view_pixel_map->clip_x = 0;
    view_pixel_map->clip_y = 0;
    view_pixel_map->clip_width = view_width;
    view_pixel_map->clip_height = view_height;
    view_pixel_map->bytes_per_pixel2 = base_pixel_map->bytes_per_pixel;
    view_pixel_map->stride = base_pixel_map->stride;
    view_pixel_map->buffer = base_pixel_map->buffer + view_pixel_map->stride * view_y + view_x;
}

void OPM_SetPixel(OPM_Struct *pixel_map, int x, int y, uint8_t color)
{
    x += pixel_map->origin_x;
    y += pixel_map->origin_y;

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

#if 0
static uint8_t OPM_GetPixel(OPM_Struct *pixel_map, int x, int y)
{
    x += pixel_map->origin_x;
    y += pixel_map->origin_y;

    if ((x >= pixel_map->clip_x) &&
        (x < pixel_map->clip_x + pixel_map->clip_width) &&
        (y >= pixel_map->clip_y) &&
        (y < pixel_map->clip_y + pixel_map->clip_height)
       )
    {
        return pixel_map->buffer[y * pixel_map->stride + x];
    }
    else
    {
        return 0;
    }
}
#endif

void OPM_HorLine(OPM_Struct *pixel_map, int x, int y, int length, uint8_t color)
{
    x += pixel_map->origin_x;
    y += pixel_map->origin_y;

    if (y < pixel_map->clip_y) return;
    if (y >= pixel_map->clip_y + pixel_map->clip_height) return;

    if (x < pixel_map->clip_x)
    {
        length -= pixel_map->clip_x - x;
        x = pixel_map->clip_x;
        if (length <= 0) return;
    }

    if (x + length > pixel_map->clip_x + pixel_map->clip_width)
    {
        length -= (x + length) - (pixel_map->clip_x + pixel_map->clip_width);
        if (length <= 0) return;
    }

    ASM_DrawHorizontalLine(pixel_map->buffer + y * pixel_map->stride + x, color, length, pixel_map->stride);
    pixel_map->flags |= BBOPM_MODIFIED;
}

void OPM_VerLine(OPM_Struct *pixel_map, int x, int y, int length, uint8_t color)
{
    x += pixel_map->origin_x;
    y += pixel_map->origin_y;

    if (x < pixel_map->clip_x) return;
    if (x >= pixel_map->clip_x + pixel_map->clip_width) return;

    if (y < pixel_map->clip_y)
    {
        length -= pixel_map->clip_y - y;
        y = pixel_map->clip_y;
        if (length <= 0) return;
    }

    if (y + length > pixel_map->clip_y + pixel_map->clip_height)
    {
        length -= (y + length) - (pixel_map->clip_y + pixel_map->clip_height);
        if (length <= 0) return;
    }

    ASM_DrawVerticalLine(pixel_map->buffer + y * pixel_map->stride + x, color, length, pixel_map->stride);
    pixel_map->flags |= BBOPM_MODIFIED;
}

void OPM_Box(OPM_Struct *pixel_map, int x, int y, int width, int height, uint8_t color)
{
    int draw_left_line, draw_right_line, draw_top_line, draw_bottom_line;

    draw_left_line = draw_right_line = draw_top_line = draw_bottom_line = 1;

    x += pixel_map->origin_x;
    y += pixel_map->origin_y;

    if (x < pixel_map->clip_x)
    {
        width -= pixel_map->clip_x - x;
        x = pixel_map->clip_x;
        draw_left_line = 0;
        if (width <= 0) return;
    }

    if (x + width > pixel_map->clip_x + pixel_map->clip_width)
    {
        width -= (x + width) - (pixel_map->clip_x + pixel_map->clip_width);
        draw_right_line = 0;
        if (width <= 0) return;
    }

    if (y < pixel_map->clip_y)
    {
        height -= pixel_map->clip_y - y;
        y = pixel_map->clip_y;
        draw_top_line = 0;
        if (height <= 0) return;
    }

    if (y + height > pixel_map->clip_y + pixel_map->clip_height)
    {
        height -= (y + height) - (pixel_map->clip_y + pixel_map->clip_height);
        draw_bottom_line = 0;
        if (height <= 0) return;
    }

    if (draw_left_line)
    {
        ASM_DrawVerticalLine(pixel_map->buffer + y * pixel_map->stride + x, color, height, pixel_map->stride);
    }
    if (draw_right_line)
    {
        ASM_DrawVerticalLine(pixel_map->buffer + y * pixel_map->stride + x + width - 1, color, height, pixel_map->stride);
    }
    if (draw_top_line)
    {
        ASM_DrawHorizontalLine(pixel_map->buffer + y * pixel_map->stride + x, color, width, pixel_map->stride);
    }
    if (draw_bottom_line)
    {
        ASM_DrawHorizontalLine(pixel_map->buffer + (y + height - 1) * pixel_map->stride + x, color, width, pixel_map->stride);
    }
    pixel_map->flags |= BBOPM_MODIFIED;
}

void OPM_FillBox(OPM_Struct *pixel_map, int x, int y, int width, int height, uint8_t color)
{
    x += pixel_map->origin_x;
    y += pixel_map->origin_y;

    if (x < pixel_map->clip_x)
    {
        width -= pixel_map->clip_x - x;
        x = pixel_map->clip_x;
        if (width <= 0) return;
    }

    if (x + width > pixel_map->clip_x + pixel_map->clip_width)
    {
        width -= (x + width) - (pixel_map->clip_x + pixel_map->clip_width);
        if (width <= 0) return;
    }

    if (y < pixel_map->clip_y)
    {
        height -= pixel_map->clip_y - y;
        y = pixel_map->clip_y;
        if (height <= 0) return;
    }

    if (y + height > pixel_map->clip_y + pixel_map->clip_height)
    {
        height -= (y + height) - (pixel_map->clip_y + pixel_map->clip_height);
        if (height <= 0) return;
    }

    ASM_DrawFilledRectangle(pixel_map->buffer + y * pixel_map->stride + x, color, pixel_map->stride - width, width, height);
    pixel_map->flags |= BBOPM_MODIFIED;
}

void OPM_CopyOPMOPM(OPM_Struct *src_pixel_map, OPM_Struct *dst_pixel_map, int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y)
{
    int clip_x, clip_y, clip_endx, clip_endy, add_x, add_y;
    uint8_t *src, *dst;

    src_x += src_pixel_map->origin_x;
    src_y += src_pixel_map->origin_y;
    dst_x += dst_pixel_map->origin_x;
    dst_y += dst_pixel_map->origin_y;

    clip_x = src_pixel_map->clip_x;
    clip_y = src_pixel_map->clip_y;
    clip_endx = clip_x + src_pixel_map->clip_width;
    clip_endy = clip_y + src_pixel_map->clip_height;

    add_x = 0;
    add_y = 0;

    if (src_x < clip_x)
    {
        if (clip_x - src_x > src_width) return;
        src_width -= clip_x - src_x;
        if (src_width <= 0) return;
        add_x += clip_x - src_x;
        src_x = clip_x;
    }

    if (src_y < clip_y)
    {
        if (clip_y - src_y > src_height) return;
        src_height -= clip_y - src_y;
        if (src_height <= 0) return;
        add_y += clip_y - src_y;
        src_y = clip_y;
    }

    if (src_x + src_width > clip_endx)
    {
        if (src_x > clip_endx) return;
        src_width -= (src_x + src_width) - clip_endx;
        if (src_width <= 0) return;
    }

    if (src_y + src_height > clip_endy)
    {
        if (src_y > clip_endy) return;
        src_height -= (src_y + src_height) - clip_endy;
        if (src_height <= 0) return;
    }

    dst_x += add_x;
    dst_y += add_y;

    clip_x = dst_pixel_map->clip_x;
    clip_y = dst_pixel_map->clip_y;
    clip_endx = clip_x + dst_pixel_map->clip_width;
    clip_endy = clip_y + dst_pixel_map->clip_height;

    add_x = 0;
    add_y = 0;

    if (dst_x < clip_x)
    {
        if (clip_x - dst_x > src_width) return;
        src_width -= clip_x - dst_x;
        if (src_width <= 0) return;
        add_x += clip_x - dst_x;
        dst_x = clip_x;
    }

    if (dst_y < clip_y)
    {
        if (clip_y - dst_y > src_height) return;
        src_height -= clip_y - dst_y;
        if (src_height <= 0) return;
        add_y += clip_y - dst_y;
        dst_y = clip_y;
    }

    if (dst_x + src_width > clip_endx)
    {
        if (dst_x > clip_endx) return;
        src_width -= (dst_x + src_width) - clip_endx;
        if (src_width <= 0) return;
    }

    if (dst_y + src_height > clip_endy)
    {
        if (dst_y > clip_endy) return;
        src_height -= (dst_y + src_height) - clip_endy;
        if (src_height <= 0) return;
    }

    src_x += add_x;
    src_y += add_y;

    src = src_pixel_map->buffer + src_pixel_map->stride * src_y + src_x;
    dst = dst_pixel_map->buffer + dst_pixel_map->stride * dst_y + dst_x;

    if (src_pixel_map->flags & BBOPM_TRANSPARENCY)
    {
        ASM_CopyRectangleWithTransparency(dst, src, src_pixel_map->transparent_color, src_pixel_map->stride - src_width, dst_pixel_map->stride - src_width, src_width, src_height);
    }
    else
    {
        ASM_CopyRectangle(dst, src, src_pixel_map->stride - src_width, dst_pixel_map->stride - src_width, src_width, src_height);
    }
    dst_pixel_map->flags |= BBOPM_MODIFIED;
}

static void OPM_LocalPrintError(char *buffer, const uint8_t *data)
{
#define DATA (((OPM_ErrorStruct *)data))
    sprintf(buffer, "ERROR!: %s  %ld, %ld", DATA->text, DATA->data1, DATA->data2);
#undef DATA
}


static void ASM_DrawFilledRectangle(uint8_t *dst, uint8_t color, unsigned int stridediff, int width, int height)
{
    if (width >= 8)
    {
        do
        {
            memset(dst, color, width);
            dst += width;
            dst += stridediff;
            height--;
        } while (height > 0);
    }
    else
    {
        do
        {
            int count;
            for (count = width; count != 0; count--)
            {
                *dst = color;
                dst++;
            }
            dst += stridediff;
            height--;
        } while (height > 0);
    }
}

static void ASM_DrawVerticalLine(uint8_t *dst, uint8_t color, int linelength, unsigned int stride)
{
    for (; linelength != 0; linelength--)
    {
        *dst = color;
        dst += stride;
    }
}

static void ASM_DrawHorizontalLine(uint8_t *dst, uint8_t color, int linelength, unsigned int stride)
{
    if (linelength >= 12)
    {
        memset(dst, color, linelength);
    }
    else
    {
        for (; linelength != 0; linelength--)
        {
            *dst = color;
            dst++;
        }
    }
}

static void ASM_CopyRectangle(uint8_t *dst, uint8_t *src, unsigned int srcstridediff, unsigned int dststridediff, int width, int height)
{
    if (width >= 8)
    {
        do
        {
            memcpy(dst, src, width);
            src += width;
            dst += width;
            src += srcstridediff;
            dst += dststridediff;
            height--;
        } while (height > 0);
    }
    else
    {
        do
        {
            int count;
            for (count = width; count != 0; count--)
            {
                *dst = *src;
                src++;
                dst++;
            }
            src += srcstridediff;
            dst += dststridediff;
            height--;
        } while (height > 0);
    }
}

static void ASM_CopyRectangleWithTransparency(uint8_t *dst, uint8_t *src, uint8_t transparent_color, unsigned int srcstridediff, unsigned int dststridediff, int width, int height)
{
    do
    {
        int count;
        for (count = width; count != 0; count--)
        {
            uint8_t value;
            value = *src;
            if (value != transparent_color)
            {
                *dst = value;
            }
            src++;
            dst++;
        }
        src += srcstridediff;
        dst += dststridediff;
        height--;
    } while (height > 0);
}

