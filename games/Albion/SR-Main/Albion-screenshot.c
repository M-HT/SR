/**
 *
 *  Copyright (C) 2016 Roman Pauer
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

#include <stdio.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-proc-vfs.h"
#include "display/overlay.h"

extern uint8_t loc_17E164[1024];
extern uint16_t loc_182010;

static inline uint8_t *write_16be(uint8_t *ptr, uint16_t value)
{
    ptr[0] = value >> 8;
    ptr[1] = value & 0xff;
    return ptr + 2;
}

static inline uint8_t *write_16le(uint8_t *ptr, uint16_t value)
{
    ptr[0] = value & 0xff;
    ptr[1] = value >> 8;
    return ptr + 2;
}

static inline uint8_t *write_32be(uint8_t *ptr, uint32_t value)
{
    ptr[0] = value >> 24;
    ptr[1] = (value >> 16) & 0xff;
    ptr[2] = (value >> 8) & 0xff;
    ptr[3] = value & 0xff;
    return ptr + 4;
}

static inline uint8_t *write_32le(uint8_t *ptr, uint32_t value)
{
    ptr[0] = value & 0xff;
    ptr[1] = (value >> 8) & 0xff;
    ptr[2] = (value >> 16) & 0xff;
    ptr[3] = value >> 24;
    return ptr + 4;
}

// http://www.shikadi.net/moddingwiki/LBM_Format

void Game_save_screenshot(const char *filename)
{
    uint8_t *screenshot_src, *buffer, *curptr, *chunk_size_ptr[2];
    unsigned int width, height, width_in_file, palette_index;
    FILE *f;
    int image_mode, DrawOverlay;

    screenshot_src = &(Game_FrameBuffer[loc_182010 * 360 * 240]);
    DrawOverlay = Get_DrawOverlay(screenshot_src, 0);

    if ((Render_Width == 720) && (Game_ScreenshotFormat != 0))
    {
        if (DrawOverlay)
        {
            image_mode = 2;
        }
        else
        {
            image_mode = 1;
        }
    }
    else
    {
        image_mode = 0;
    }

    if (image_mode)
    {
        width = 720;
        height = 480;
    }
    else
    {
        width = 360;
        height = 240;
    }

    if (Game_ScreenshotFormat == 2)
    {
        width_in_file = ((width + 15) >> 4) * 2 * 8;
    }
    else if (Game_ScreenshotFormat == 4)
    {
        width_in_file = ((width + 3) >> 2) * 4;
    }
    else
    {
        width_in_file = width;
    }

    buffer = (uint8_t *) malloc(width_in_file * height + 2000);
    if (buffer == NULL) return;

    curptr = buffer;

    if ((Game_ScreenshotFormat == 0) || (Game_ScreenshotFormat == 1) || (Game_ScreenshotFormat == 2))
    {
        //LBM

        // write "FORM" chunk id
        memcpy(curptr, "FORM", 4);
        curptr += 4;

        // space for "FORM" chunk size
        chunk_size_ptr[0] = curptr;
        curptr += 4;

        // write "PBM " id
        memcpy(curptr, "PBM ", 4);
        curptr += 4;

        // write "BMHD" chunk id
        memcpy(curptr, "BMHD", 4);
        curptr += 4;

        // space for "BMHD" chunk size
        chunk_size_ptr[1] = curptr;
        curptr += 4;

        // write image width (16-bit Big Endian)
        curptr = write_16be(curptr, width);

        // write image height (16-bit Big Endian)
        curptr = write_16be(curptr, height);

        // write xOrigin (16-bit Big Endian)
        curptr = write_16be(curptr, 0);

        // write yOrigin (16-bit Big Endian)
        curptr = write_16be(curptr, 0);

        // write numPlanes (image bpp) (8-bit)
        *curptr = 8;
        curptr++;

        // write mask (8-bit)
        *curptr = 0;
        curptr++;

        // write compression (8-bit)
        *curptr = 0;
        curptr++;

        // write padding (8-bit)
        *curptr = 0;
        curptr++;

        // write Transparent colour (16-bit Big Endian)
        curptr = write_16be(curptr, 0);

        if (Game_ScreenshotFormat == 0)
        {
            // write ??? (16-bit Big Endian)
            curptr = write_16be(curptr, 0);
        }

        // write xAspect (8-bit)
        *curptr = (Game_ScreenshotFormat == 0)?10:8;
        curptr++;

        // write yAspect (8-bit)
        *curptr = (Game_ScreenshotFormat == 0)?11:9;
        curptr++;

        // write pageWidth (16-bit Big Endian)
        curptr = write_16be(curptr, (image_mode)?640:320);

        // write pageHeight (16-bit Big Endian)
        curptr = write_16be(curptr, (image_mode)?480:((Game_ScreenshotFormat == 0)?200:240));

        // write "BMHD" chunk size (32-bit Big Endian)
        write_32be(chunk_size_ptr[1], (curptr - chunk_size_ptr[1]) - 4);

        // write "CMAP" chunk id
        memcpy(curptr, "CMAP", 4);
        curptr += 4;

        // space for "CMAP" chunk size
        chunk_size_ptr[1] = curptr;
        curptr += 4;

        // fill "CMAP" chunk
        for (palette_index = 0; palette_index < 256; palette_index++)
        {
            curptr[0] = loc_17E164[4 * palette_index];     // b
            curptr[1] = loc_17E164[4 * palette_index + 1]; // g
            curptr[2] = loc_17E164[4 * palette_index + 2]; // r
            curptr += 3;
        }

        // write "CMAP" chunk size (32-bit Big Endian)
        write_32be(chunk_size_ptr[1], (curptr - chunk_size_ptr[1]) - 4);

        // write "BODY" chunk id
        memcpy(curptr, "BODY", 4);
        curptr += 4;

        // space for "BODY" chunk size
        chunk_size_ptr[1] = curptr;
        curptr += 4;
    }
    else if (Game_ScreenshotFormat == 3)
    {
        // TGA

        // write ID length (8-bit)
        *curptr = 0;
        curptr++;

        // write Color map type (8-bit)
        *curptr = 1;
        curptr++;

        // write Image type (8-bit)
        *curptr = 1;
        curptr++;

        // write Color map specification: First entry index (16-bit Little Endian)
        curptr = write_16le(curptr, 0);

        // write Color map specification: Color map length (16-bit Little Endian)
        curptr = write_16le(curptr, 256);

        // write Color map specification: Color map entry size (8-bit)
        *curptr = 24;
        curptr++;

        // write Image specification: X-origin (16-bit Little Endian)
        curptr = write_16le(curptr, 0);

        // write Image specification: Y-origin (16-bit Little Endian)
        curptr = write_16le(curptr, 0);

        // write Image specification: Image width (16-bit Little Endian)
        curptr = write_16le(curptr, width);

        // write Image specification: Image height (16-bit Little Endian)
        curptr = write_16le(curptr, height);

        // write Image specification: Pixel depth (8-bit)
        *curptr = 8;
        curptr++;

        // write Image specification: Image descriptor (8-bit)
        *curptr = 0x20;
        curptr++;

        // fill palette data
        for (palette_index = 0; palette_index < 256; palette_index++)
        {
            curptr[0] = loc_17E164[4 * palette_index + 2]; // r
            curptr[1] = loc_17E164[4 * palette_index + 1]; // g
            curptr[2] = loc_17E164[4 * palette_index];     // b
            curptr += 3;
        }
    }
    else if (Game_ScreenshotFormat == 4)
    {
        // BMP

        // write header
        memcpy(curptr, "BM", 2);
        curptr += 2;

        // space for file size
        chunk_size_ptr[0] = curptr;
        curptr += 4;

        // write Reserved (16-bit Little Endian)
        curptr = write_16le(curptr, 0);

        // write Reserved (16-bit Little Endian)
        curptr = write_16le(curptr, 0);

        // write The offset of bitmap image data (32-bit Little Endian)
        curptr = write_32le(curptr, 14 + 40 + 1024);

        // write DIB header size (32-bit Little Endian)
        curptr = write_32le(curptr, 40);

        // write The bitmap width in pixels (32-bit Little Endian)
        curptr = write_32le(curptr, width);

        // write The bitmap height in pixels (32-bit Little Endian)
        curptr = write_32le(curptr, -(int32_t)height);

        // write The number of color planes (16-bit Little Endian)
        curptr = write_16le(curptr, 1);

        // write The number of bits per pixel (16-bit Little Endian)
        curptr = write_16le(curptr, 8);

        // write The compression method (32-bit Little Endian)
        curptr = write_32le(curptr, 0);

        // write The image size (32-bit Little Endian)
        curptr = write_32le(curptr, 0);

        // write The horizontal resolution of the image (32-bit Little Endian)
        curptr = write_32le(curptr, 0);

        // write The vertical resolution of the image (32-bit Little Endian)
        curptr = write_32le(curptr, 0);

        // write The number of colors in the color palette (32-bit Little Endian)
        curptr = write_32le(curptr, 0);

        // write The number of important colors used (32-bit Little Endian)
        curptr = write_32le(curptr, 0);

        // fill palette data
        for (palette_index = 0; palette_index < 256; palette_index++)
        {
            curptr[0] = loc_17E164[4 * palette_index + 2]; // r
            curptr[1] = loc_17E164[4 * palette_index + 1]; // g
            curptr[2] = loc_17E164[4 * palette_index];     // b
            curptr[3] = 0;                                 // a
            curptr += 4;
        }
    }

    // fill pixel data
    {
        int x, y;
        uint8_t *src, *orig, *src2, *dst;

        if (image_mode == 0)
        {
            if (width_in_file == width)
            {
                memcpy(curptr, screenshot_src, width * height);
                curptr += width * height;
            }
            else
            {
                src = screenshot_src;

                for (y = height; y != 0; y--)
                {
                    memcpy(curptr, src, width);
                    curptr += width;
                    src += width;

                    memset(curptr, 0, width_in_file - width);
                    curptr += width_in_file - width;
                }
            }

            if (DrawOverlay & 1)
            {
                dst = curptr - width_in_file * height + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) * width_in_file + Game_OverlayDraw.ViewportX + 1;
                src2 = Game_OverlayDraw.ScreenViewpartOverlay + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2)*2 * 800 + (Game_OverlayDraw.ViewportX + 1)*2;
                for (x = 8; x != 0; x--)
                {
                    dst[0] = src2[0];

                    src2+=2;
                    dst++;
                }
            }

            if (DrawOverlay & 2)
            {
                dst = curptr - width_in_file * height + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) * width_in_file + Game_OverlayDraw.ViewportX + Game_OverlayDraw.ViewportWidth - 10;
                src2 = Game_OverlayDraw.ScreenViewpartOverlay + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2)*2 * 800 + Game_OverlayDraw.ViewportX*2 + 2*(Game_OverlayDraw.ViewportWidth - 10);
                for (x = 8; x != 0; x--)
                {
                    dst[0] = src2[0];

                    src2+=2;
                    dst++;
                }
            }
        }
        else if (image_mode == 1)
        {
            src = screenshot_src;
            for (y = 240; y != 0; y--)
            {
                for (x = 360; x != 0; x--)
                {
                    curptr[0] = curptr[1] = curptr[width_in_file] = curptr[width_in_file + 1] = *src;
                    curptr += 2;
                    src++;
                }

                for (x = width_in_file - width; x != 0; x--)
                {
                    curptr[0] = curptr[width_in_file] = 0;
                    curptr++;
                }

                curptr += width_in_file;
            }
        }
        else
        {
            src = screenshot_src;

            // part above the viewport
            for (y = Game_OverlayDraw.ViewportY; y != 0; y--)
            {
                for (x = 360; x != 0; x--)
                {
                    curptr[0] = curptr[1] = curptr[width_in_file] = curptr[width_in_file + 1] = *src;
                    curptr += 2;
                    src++;
                }

                for (x = width_in_file - width; x != 0; x--)
                {
                    curptr[0] = curptr[width_in_file] = 0;
                    curptr++;
                }

                curptr += width_in_file;
            }

            orig = Game_OverlayDraw.ScreenViewpartOriginal + 360 * Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportX;
            src2 = Game_OverlayDraw.ScreenViewpartOverlay + Game_OverlayDraw.ViewportY*2 * 800 + Game_OverlayDraw.ViewportX*2;

            // the viewport
            for (y = Game_OverlayDraw.ViewportHeight; y != 0; y--)
            {
                // part left of the viewport
                for (x = Game_OverlayDraw.ViewportX; x != 0; x--)
                {
                    curptr[0] = curptr[1] = curptr[width_in_file] = curptr[width_in_file + 1] = *src;
                    curptr += 2;
                    src++;
                }

                // the viewport
                for (x = Game_OverlayDraw.ViewportWidth; x != 0; x--)
                {
                    if (*src == *orig)
                    {
                        curptr[0] = src2[0];
                        curptr[1] = src2[1];
                        curptr[width_in_file] = src2[800];
                        curptr[width_in_file + 1] = src2[801];
                    }
                    else
                    {
                        curptr[0] = curptr[1] = curptr[width_in_file] = curptr[width_in_file + 1] = *src;
                    }

                    curptr += 2;
                    src++;
                    orig++;
                    src2 += 2;
                }

                orig += 360 - Game_OverlayDraw.ViewportWidth;
                src2 += 800 + (800 - 2*Game_OverlayDraw.ViewportWidth);

                // part right of the viewport
                for (x = 360 - (Game_OverlayDraw.ViewportX + Game_OverlayDraw.ViewportWidth); x != 0; x--)
                {
                    curptr[0] = curptr[1] = curptr[width_in_file] = curptr[width_in_file + 1] = *src;
                    curptr += 2;
                    src++;
                }

                for (x = width_in_file - width; x != 0; x--)
                {
                    curptr[0] = curptr[width_in_file] = 0;
                    curptr++;
                }

                curptr += width_in_file;
            }

            if (DrawOverlay & 1)
            {
                dst = curptr - 2 * width_in_file * 2 + 2 * (Game_OverlayDraw.ViewportX + 1);
                src2 = Game_OverlayDraw.ScreenViewpartOverlay + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2)*2 * 800 + (Game_OverlayDraw.ViewportX + 1)*2;
                for (x = 8; x != 0; x--)
                {
                    dst[0] = src2[0];
                    dst[1] = src2[1];
                    dst[width_in_file] = src2[800];
                    dst[width_in_file + 1] = src2[801];

                    src2+=2;
                    dst+=2;
                }
            }

            if (DrawOverlay & 2)
            {
                dst = curptr - 2 * width_in_file * 2 + 2 * Game_OverlayDraw.ViewportX + 2 * (Game_OverlayDraw.ViewportWidth - 10);
                src2 = Game_OverlayDraw.ScreenViewpartOverlay + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2)*2 * 800 + Game_OverlayDraw.ViewportX*2 + 2*(Game_OverlayDraw.ViewportWidth - 10);
                for (x = 8; x != 0; x--)
                {
                    dst[0] = src2[0];
                    dst[1] = src2[1];
                    dst[width_in_file] = src2[800];
                    dst[width_in_file + 1] = src2[801];

                    src2+=2;
                    dst+=2;
                }
            }

            // part below the viewport
            for (y = 240 - (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight); y != 0; y--)
            {
                for (x = 360; x != 0; x--)
                {
                    curptr[0] = curptr[1] = curptr[width_in_file] = curptr[width_in_file + 1] = *src;
                    curptr += 2;
                    src++;
                }

                for (x = width_in_file - width; x != 0; x--)
                {
                    curptr[0] = curptr[width_in_file] = 0;
                    curptr++;
                }

                curptr += width_in_file;
            }
        }
    }

    if ((Game_ScreenshotFormat == 0) || (Game_ScreenshotFormat == 1) || (Game_ScreenshotFormat == 2))
    {
        // write "BODY" chunk size (32-bit Big Endian)
        write_32be(chunk_size_ptr[1], (curptr - chunk_size_ptr[1]) - 4);

        // write "FORM" chunk size (32-bit Big Endian)
        write_32be(chunk_size_ptr[0], (curptr - chunk_size_ptr[0]) - 4);
    }
    else if (Game_ScreenshotFormat == 4)
    {
        // write file size (32-bit Little Endian)
        write_32le(chunk_size_ptr[0], curptr - buffer);
    }

    f = Game_fopen(filename, "wb");
    if (f != NULL)
    {
        fwrite(buffer, 1, curptr - buffer, f);
        fclose(f);
    }

    free(buffer);
}

