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

#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-proc-vfs.h"
#include "display/overlay.h"

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)) || defined(GP2X)
#undef ZLIB_DYNAMIC
#else
#define ZLIB_DYNAMIC 1
#endif

#ifdef ZLIB_DYNAMIC
#include <dlfcn.h>

static int (*zlib_deflateInit2_) (z_streamp strm, int  level, int  method,
                                      int windowBits, int memLevel,
                                      int strategy, const char *version,
                                      int stream_size);

#define zlib_deflateInit2(strm, level, method, windowBits, memLevel, strategy) \
        zlib_deflateInit2_((strm),(level),(method),(windowBits),(memLevel),\
                      (strategy), ZLIB_VERSION, (int)sizeof(z_stream))

static int (*zlib_deflate) (z_streamp strm, int flush);
static int (*zlib_deflateEnd) (z_streamp strm);

static uLong (*zlib_crc32)   (uLong crc, const Bytef *buf, uInt len);

static void *zlib_handle;

static int zlib_state = 0;
#else
#define zlib_deflateInit2(strm, level, method, windowBits, memLevel, strategy) deflateInit2((strm),(level),(method),(windowBits),(memLevel),(strategy))
#define zlib_deflate(strm, flush) deflate((strm),(flush))
#define zlib_deflateEnd(strm) deflateEnd((strm))
#endif


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

static uint8_t *fill_png_pixel_data(uint8_t *src, int image_mode, int DrawOverlay, int remaining, uint8_t *curptr)
{
    int x, y, ret, overlay_y;
    uint8_t value;
    uint16_t *dst16;
    uint8_t *dst8, *src2, *orig;
    z_stream strm;
    uint8_t pixel_data[724*2];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.next_in = NULL;

    if (Z_OK != zlib_deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY))
    {
        return NULL;
    }

    strm.avail_out = remaining;
    strm.next_out = curptr;

    ret = 0; // silence warning

    if (image_mode == 0)
    {
        pixel_data[3] = 0; // filter for line

        if (DrawOverlay & 3)
        {
            overlay_y = Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2;
        }
        else
        {
            overlay_y = -1;
        }

        for (y = 0; y < 240; y++)
        {
            if (y == overlay_y)
            {
                memcpy(&(pixel_data[4]), src, 360);
                src += 360;

                if (DrawOverlay & 1)
                {
                    dst8 = &(pixel_data[4 + Game_OverlayDraw.ViewportX + 1]);
                    src2 = Game_OverlayDraw.ScreenViewpartOverlay + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2)*2 * 800 + (Game_OverlayDraw.ViewportX + 1)*2;
                    for (x = 8; x != 0; x--)
                    {
                        dst8[0] = src2[0];

                        src2+=2;
                        dst8++;
                    }
                }

                if (DrawOverlay & 2)
                {
                    dst8 = &(pixel_data[4 + Game_OverlayDraw.ViewportX + Game_OverlayDraw.ViewportWidth - 10]);
                    src2 = Game_OverlayDraw.ScreenViewpartOverlay + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2)*2 * 800 + Game_OverlayDraw.ViewportX*2 + 2*(Game_OverlayDraw.ViewportWidth - 10);
                    for (x = 8; x != 0; x--)
                    {
                        dst8[0] = src2[0];

                        src2+=2;
                        dst8++;
                    }
                }


                // write line with filter
                strm.avail_in = 361;
                strm.next_in = &(pixel_data[3]);

                ret = zlib_deflate(&strm, Z_NO_FLUSH);
                if (ret == Z_STREAM_ERROR)
                {
                    zlib_deflateEnd(&strm);
                    return NULL;
                }

                if (strm.avail_in != 0)
                {
                    zlib_deflateEnd(&strm);
                    return NULL;
                }
            }
            else
            {
                // write filter
                strm.avail_in = 1;
                strm.next_in = &(pixel_data[3]);

                ret = zlib_deflate(&strm, Z_NO_FLUSH);
                if (ret == Z_STREAM_ERROR)
                {
                    zlib_deflateEnd(&strm);
                    return NULL;
                }

                if (strm.avail_in != 0)
                {
                    zlib_deflateEnd(&strm);
                    return NULL;
                }


                // write line
                strm.avail_in = 360;
                strm.next_in = src;
                src += 360;

                ret = zlib_deflate(&strm, (y == 239)?Z_FINISH:Z_NO_FLUSH);
                if (ret == Z_STREAM_ERROR)
                {
                    zlib_deflateEnd(&strm);
                    return NULL;
                }

                if (strm.avail_in != 0)
                {
                    zlib_deflateEnd(&strm);
                    return NULL;
                }
            }
        }
    }
    else if (image_mode == 1)
    {
        pixel_data[3] = 0; // filter for first line
        pixel_data[3 + 724] = 2;  // filter for second line
        memset(&(pixel_data[4 + 724]), 0, 720); // second line

        for (y = 240; y != 0; y--)
        {
            // fill first line
            dst16 = (uint16_t *)&(pixel_data[4]);
            for (x = 360; x != 0; x--)
            {
                value = *src;
                src++;

                *dst16 = value | (value << 8);
                dst16++;
            }

            // write first line with filter
            strm.avail_in = 721;
            strm.next_in = &(pixel_data[3]);

            ret = zlib_deflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            if (strm.avail_in != 0)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            // write second line with filter
            strm.avail_in = 721;
            strm.next_in = &(pixel_data[3 + 724]);

            ret = zlib_deflate(&strm, (y == 1)?Z_FINISH:Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            if (strm.avail_in != 0)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }
        }
    }
    else
    {
        pixel_data[3] = 0; // filter for first line
        pixel_data[3 + 724] = 0;  // filter for second line

        // part above the viewport
        for (y = Game_OverlayDraw.ViewportY; y != 0; y--)
        {
            // fill first line
            dst16 = (uint16_t *)&(pixel_data[4]);
            for (x = 360; x != 0; x--)
            {
                value = *src;
                src++;

                *dst16 = value | (value << 8);
                dst16++;
            }

            // write first line with filter
            strm.avail_in = 721;
            strm.next_in = &(pixel_data[3]);

            ret = zlib_deflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            if (strm.avail_in != 0)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            // write first line with filter again
            strm.avail_in = 721;
            strm.next_in = &(pixel_data[3]);

            ret = zlib_deflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            if (strm.avail_in != 0)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }
        }


        orig = Game_OverlayDraw.ScreenViewpartOriginal + 360 * Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportX;
        src2 = Game_OverlayDraw.ScreenViewpartOverlay + Game_OverlayDraw.ViewportY*2 * 800 + Game_OverlayDraw.ViewportX*2;

        // the viewport
        for (y = Game_OverlayDraw.ViewportHeight; y != 0; y--)
        {
            dst8 = &(pixel_data[4]);

            // part left of the viewport
            for (x = Game_OverlayDraw.ViewportX; x != 0; x--)
            {
                dst8[0] = dst8[1] = dst8[724] = dst8[724 + 1] = *src;
                dst8 += 2;
                src++;
            }

            // the viewport
            for (x = Game_OverlayDraw.ViewportWidth; x != 0; x--)
            {
                if (*src == *orig)
                {
                    dst8[0] = src2[0];
                    dst8[1] = src2[1];
                    dst8[724] = src2[800];
                    dst8[724 + 1] = src2[801];
                }
                else
                {
                    dst8[0] = dst8[1] = dst8[724] = dst8[724 + 1] = *src;
                }

                dst8 += 2;
                src++;
                orig++;
                src2 += 2;
            }

            orig += 360 - Game_OverlayDraw.ViewportWidth;
            src2 += 800 + (800 - 2*Game_OverlayDraw.ViewportWidth);

            // part right of the viewport
            for (x = 360 - (Game_OverlayDraw.ViewportX + Game_OverlayDraw.ViewportWidth); x != 0; x--)
            {
                dst8[0] = dst8[1] = dst8[724] = dst8[724 + 1] = *src;
                dst8 += 2;
                src++;
            }

            if (y == 1)
            {
                if (DrawOverlay & 1)
                {
                    dst8 = &(pixel_data[4 + 2 * (Game_OverlayDraw.ViewportX + 1)]);
                    src2 = Game_OverlayDraw.ScreenViewpartOverlay + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2)*2 * 800 + (Game_OverlayDraw.ViewportX + 1)*2;
                    for (x = 8; x != 0; x--)
                    {
                        dst8[0] = src2[0];
                        dst8[1] = src2[1];
                        dst8[724] = src2[800];
                        dst8[724 + 1] = src2[801];

                        src2+=2;
                        dst8+=2;
                    }
                }

                if (DrawOverlay & 2)
                {
                    dst8 = &(pixel_data[4 + 2 * Game_OverlayDraw.ViewportX + 2 * (Game_OverlayDraw.ViewportWidth - 10)]);
                    src2 = Game_OverlayDraw.ScreenViewpartOverlay + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2)*2 * 800 + Game_OverlayDraw.ViewportX*2 + 2*(Game_OverlayDraw.ViewportWidth - 10);
                    for (x = 8; x != 0; x--)
                    {
                        dst8[0] = src2[0];
                        dst8[1] = src2[1];
                        dst8[724] = src2[800];
                        dst8[724 + 1] = src2[801];

                        src2+=2;
                        dst8+=2;
                    }
                }
            }

            // write first line with filter
            strm.avail_in = 721;
            strm.next_in = &(pixel_data[3]);

            ret = zlib_deflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            if (strm.avail_in != 0)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            // write second line with filter
            strm.avail_in = 721;
            strm.next_in = &(pixel_data[3 + 724]);

            ret = zlib_deflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            if (strm.avail_in != 0)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }
        }

        // part below the viewport
        for (y = 240 - (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight); y != 0; y--)
        {
            // fill first line
            dst16 = (uint16_t *)&(pixel_data[4]);
            for (x = 360; x != 0; x--)
            {
                value = *src;
                src++;

                *dst16 = value | (value << 8);
                dst16++;
            }

            // write first line with filter
            strm.avail_in = 721;
            strm.next_in = &(pixel_data[3]);

            ret = zlib_deflate(&strm, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            if (strm.avail_in != 0)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            // write first line with filter again
            strm.avail_in = 721;
            strm.next_in = &(pixel_data[3]);

            ret = zlib_deflate(&strm, (y == 1)?Z_FINISH:Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }

            if (strm.avail_in != 0)
            {
                zlib_deflateEnd(&strm);
                return NULL;
            }
        }
    }

    if (ret != Z_STREAM_END)
    {
        zlib_deflateEnd(&strm);
        return NULL;
    }

    curptr += remaining - strm.avail_out;

    zlib_deflateEnd(&strm);

    return curptr;
}

// http://www.shikadi.net/moddingwiki/LBM_Format

void Game_save_screenshot(const char *filename)
{
    uint8_t *screenshot_src, *buffer, *curptr, *chunk_size_ptr[2];
    unsigned int width, height, width_in_file, palette_index;
    FILE *f;
    int image_mode, DrawOverlay;

#ifdef ZLIB_DYNAMIC
    if (Game_ScreenshotFormat == 5)
    {
        if (zlib_state < 0) return;
        else if (zlib_state == 0)
        {
            zlib_handle = dlopen("libz.so", RTLD_LAZY);
            if (zlib_handle == NULL)
            {
                zlib_handle = dlopen("./libz.so", RTLD_LAZY);
            }
            if (zlib_handle == NULL)
            {
                zlib_state = -1;
                return;
            }

            zlib_deflateInit2_ = dlsym(zlib_handle, "deflateInit2_");
            zlib_deflate = dlsym(zlib_handle, "deflate");
            zlib_deflateEnd = dlsym(zlib_handle, "deflateEnd");
            zlib_crc32 = dlsym(zlib_handle, "crc32");

            if ((zlib_deflateInit2_ == NULL) || (zlib_deflate == NULL) || (zlib_deflateEnd == NULL) || (zlib_crc32 == NULL))
            {
                dlclose(zlib_handle);
                zlib_state = -1;
                return;
            }

            zlib_state = 1;
        }
    }
#endif

    screenshot_src = &(Game_FrameBuffer[loc_182010 * 360 * 240]);
    DrawOverlay = Get_DrawOverlay(screenshot_src, 0);

    if ((Render_Width == 720) && (Game_ScreenshotEnhancedResolution != 0))
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
    else if (Game_ScreenshotFormat == 5)
    {
        width_in_file = width + 1;
    }
    else
    {
        width_in_file = width;
    }

    buffer = (uint8_t *) malloc(width_in_file * height + 2000);
    if (buffer == NULL) return;

    curptr = buffer;

    // silence warning
    chunk_size_ptr[0] = chunk_size_ptr[1] = NULL;

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
            curptr[0] = loc_17E164[4 * palette_index];     // r
            curptr[1] = loc_17E164[4 * palette_index + 1]; // g
            curptr[2] = loc_17E164[4 * palette_index + 2]; // b
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
            curptr[0] = loc_17E164[4 * palette_index + 2]; // b
            curptr[1] = loc_17E164[4 * palette_index + 1]; // g
            curptr[2] = loc_17E164[4 * palette_index];     // r
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
            curptr[0] = loc_17E164[4 * palette_index + 2]; // b
            curptr[1] = loc_17E164[4 * palette_index + 1]; // g
            curptr[2] = loc_17E164[4 * palette_index];     // r
            curptr[3] = 0;                                 // a
            curptr += 4;
        }
    }
    else if (Game_ScreenshotFormat == 5)
    {
        // PNG

        // write header
        curptr[0] = 0x89;
        curptr[1] = 'P';
        curptr[2] = 'N';
        curptr[3] = 'G';
        curptr[4] = 0x0d;
        curptr[5] = 0x0a;
        curptr[6] = 0x1a;
        curptr[7] = 0x0a;
        curptr += 8;


        // "IHDR" chunk

        // write "IHDR" chunk size
        curptr = write_32be(curptr, 13);

        // write "IHDR" chunk id
        memcpy(curptr, "IHDR", 4);
        curptr += 4;

        // write Width
        curptr = write_32be(curptr, width);

        // write Height
        curptr = write_32be(curptr, height);

        // write Bit depth
        *curptr = 8;
        curptr++;

        // write Color type
        *curptr = 3;
        curptr++;

        // write Compression method
        *curptr = 0;
        curptr++;

        // write Filter method
        *curptr = 0;
        curptr++;

        // write Interlace method
        *curptr = 0;
        curptr++;

        // write "IHDR" chunk crc32
        curptr = write_32be(curptr, zlib_crc32(0, curptr - (4 + 13), 4 + 13));


        // "pHYs" chunk

        // write "pHYs" chunk size
        curptr = write_32be(curptr, 9);

        // write "pHYs" chunk id
        memcpy(curptr, "pHYs", 4);
        curptr += 4;

        // write Pixels per unit, X axis
        curptr = write_32be(curptr, 640);

        // write Pixels per unit, Y axis
        curptr = write_32be(curptr, 480);

        // write Unit specifier
        *curptr = 0;
        curptr++;

        // write "pHYs" chunk crc32
        curptr = write_32be(curptr, zlib_crc32(0, curptr - (4 + 9), 4 + 9));


        // "sBIT" chunk

        // write "sBIT" chunk size
        curptr = write_32be(curptr, 3);

        // write "sBIT" chunk id
        memcpy(curptr, "sBIT", 4);
        curptr += 4;

        // write Significant bits for red component of the palette entries
        *curptr = 6;
        curptr++;

        // write Significant bits for green component of the palette entries
        *curptr = 6;
        curptr++;

        // write Significant bits for blue component of the palette entries
        *curptr = 6;
        curptr++;

        // write "sBIT" chunk crc32
        curptr = write_32be(curptr, zlib_crc32(0, curptr - (4 + 3), 4 + 3));


        // "PLTE" chunk

        // write "PLTE" chunk size
        curptr = write_32be(curptr, 256 * 3);

        // write "PLTE" chunk id
        memcpy(curptr, "PLTE", 4);
        curptr += 4;

        // fill palette data
        for (palette_index = 0; palette_index < 256; palette_index++)
        {
            curptr[0] = loc_17E164[4 * palette_index];     // r
            curptr[1] = loc_17E164[4 * palette_index + 1]; // g
            curptr[2] = loc_17E164[4 * palette_index + 2]; // b
            curptr += 3;
        }

        // write "PLTE" chunk crc32
        curptr = write_32be(curptr, zlib_crc32(0, curptr - (4 + 256 * 3), 4 + 256 * 3));


        // "IDAT" chunk

        // space for "IDAT" chunk size
        chunk_size_ptr[0] = curptr;
        curptr += 4;

        // write "IDAT" chunk id
        memcpy(curptr, "IDAT", 4);
        curptr += 4;
    }

    // fill pixel data
    if (Game_ScreenshotFormat == 5)
    {
        curptr = fill_png_pixel_data(screenshot_src, image_mode, DrawOverlay, 721*480 + (2000 - 16) - (curptr - buffer), curptr);
        if (curptr == NULL)
        {
            free(buffer);
            return;
        }
    }
    else
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
    else if (Game_ScreenshotFormat == 5)
    {
        // write "IDAT" chunk size
        write_32be(chunk_size_ptr[0], (curptr - chunk_size_ptr[0]) - 8);

        // write "IDAT" chunk crc32
        curptr = write_32be(curptr, zlib_crc32(0, chunk_size_ptr[0] + 4, curptr - (chunk_size_ptr[0] + 4)));


        // "IEND" chunk

        // write "IEND" chunk size
        curptr = write_32be(curptr, 0);

        // write "IEND" chunk id
        memcpy(curptr, "IEND", 4);
        curptr += 4;

        // write "IEND" chunk crc32
        curptr = write_32be(curptr, zlib_crc32(0, curptr - (4 + 0), 4 + 0));
    }

    f = Game_fopen(filename, "wb");
    if (f != NULL)
    {
        fwrite(buffer, 1, curptr - buffer, f);
        fclose(f);
    }

    free(buffer);
}

