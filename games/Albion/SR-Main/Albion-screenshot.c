/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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

#define _FILE_OFFSET_BITS 64
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
    #include <malloc.h>
#else
    #include <unistd.h>
    #include <dirent.h>
    #include <alloca.h>
#endif

#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include "Game_defs.h"
#include "Game_scalerplugin.h"
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

#ifdef __cplusplus
#define ZLIB_TYPEOF(t) decltype(t)
#else
#define ZLIB_TYPEOF(t) void*
#endif


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
#define zlib_crc32(crc, buf, len) crc32((crc), (buf), (len))
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

#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
static uint8_t *fill_png_pixel_data_advanced(uint32_t *src, int remaining, uint8_t *curptr)
{
    int x, y, ret;
    uint8_t *dst8;
    z_stream strm;
    uint8_t *pixel_data;

    pixel_data = (uint8_t *) alloca(360 * 3 * Scaler_ScaleFactor + 4);

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

    pixel_data[3] = 0; // filter for line

    for (y = Scaler_ScaleFactor * 240; y != 0; y--)
    {
        dst8 = &(pixel_data[4]);

        for (x = Scaler_ScaleFactor * 360; x != 0; x--)
        {
            dst8[0] = (*src >> 16) & 0xff;
            dst8[1] = (*src >> 8) & 0xff;
            dst8[2] = *src & 0xff;
            dst8 += 3;
            src++;
        }

        // write second line with filter
        strm.avail_in = 360 * 3 * Scaler_ScaleFactor + 1;
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

    if (ret != Z_STREAM_END)
    {
        zlib_deflateEnd(&strm);
        return NULL;
    }

    curptr += remaining - strm.avail_out;

    zlib_deflateEnd(&strm);

    return curptr;
}
#endif

static uint8_t *fill_png_pixel_data(uint8_t *src, int image_mode, int DrawOverlay, int remaining, uint8_t *curptr)
{
    int x, y, ret, overlay_y;
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
    int counter, counter2;
#endif
    uint8_t value;
    uint16_t *dst16;
    uint8_t *dst8, *src2, *orig;
    z_stream strm;
    uint8_t *pixel_data;

    switch (image_mode)
    {
        case 0:
        case 4:
            pixel_data = (uint8_t *) alloca(364);
            break;
        case 1:
        case 2:
            pixel_data = (uint8_t *) alloca(724*2);
            break;
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
        case 5:
            pixel_data = (uint8_t *) alloca((360 * Scaler_ScaleFactor + 4) * 2);
            break;
        case 6:
            pixel_data = (uint8_t *) alloca((360 * Scaler_ScaleFactor + 4) * Scaler_ScaleFactor);
            break;
#endif
        default:
            pixel_data = NULL;
            break;
    }

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

    if ((image_mode == 0) || (image_mode == 4))
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
                unsigned int src2_stride, src2_factor;

                memcpy(&(pixel_data[4]), src, 360);
                src += 360;

#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
                if (image_mode != 0)
                {
                    src2_stride = Scaler_ScaleFactor * 360;
                    src2_factor = Scaler_ScaleFactor;
                }
                else
#endif
                {
                    src2_stride = 800;
                    src2_factor = 2;
                }

                if (DrawOverlay & 1)
                {
                    dst8 = &(pixel_data[4 + (Game_OverlayDraw.ViewportX + 1)]);
                    src2 = Game_OverlayDraw.ScreenViewpartOverlay + src2_factor * src2_stride * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + src2_factor * (Game_OverlayDraw.ViewportX + 1);
                    for (x = 8; x != 0; x--)
                    {
                        dst8[0] = src2[0];

                        src2 += src2_factor;
                        dst8++;
                    }
                }

                if (DrawOverlay & 2)
                {
                    dst8 = &(pixel_data[4 + Game_OverlayDraw.ViewportX + (Game_OverlayDraw.ViewportWidth - 10)]);
                    src2 = Game_OverlayDraw.ScreenViewpartOverlay + src2_factor * src2_stride * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + Game_OverlayDraw.ViewportX*2 + src2_factor * (Game_OverlayDraw.ViewportWidth - 10);
                    for (x = 8; x != 0; x--)
                    {
                        dst8[0] = src2[0];

                        src2 += src2_factor;
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
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
    else if (image_mode == 5)
    {
        pixel_data[3] = 0; // filter for first line
        pixel_data[3 + (360 * Scaler_ScaleFactor + 4)] = 2;  // filter for next lines
        memset(&(pixel_data[4 + (360 * Scaler_ScaleFactor + 4)]), 0, 360 * Scaler_ScaleFactor); // next lines

        for (y = 240; y != 0; y--)
        {
            // fill first line
            dst8 = &(pixel_data[4]);
            for (x = 360; x != 0; x--)
            {
                value = *src;
                src++;

                for (counter = Scaler_ScaleFactor; counter != 0; counter--)
                {
                    *dst8 = value;
                    dst8++;
                }
            }

            // write first line with filter
            strm.avail_in = 360 * Scaler_ScaleFactor + 1;
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

            // write next lines with filter
            for (counter = Scaler_ScaleFactor - 1; counter != 0; counter--)
            {
                strm.avail_in = 360 * Scaler_ScaleFactor + 1;
                strm.next_in = &(pixel_data[3 + (360 * Scaler_ScaleFactor + 4)]);

                ret = zlib_deflate(&strm, ((y == 1) && (counter == 1))?Z_FINISH:Z_NO_FLUSH);
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
#endif
    else if (image_mode == 2)
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
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
    else if (image_mode == 6)
    {
        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
        {
            pixel_data[3 + counter * (360 * Scaler_ScaleFactor + 4)] = 0;  // filter for line
        }

        // part above the viewport
        for (y = Game_OverlayDraw.ViewportY; y != 0; y--)
        {
            // fill first line
            dst8 = &(pixel_data[4]);
            for (x = 360; x != 0; x--)
            {
                value = *src;
                src++;

                for (counter = Scaler_ScaleFactor; counter != 0; counter--)
                {
                    *dst8 = value;
                    dst8++;
                }
            }

            // write lines with filter
            for (counter = Scaler_ScaleFactor; counter != 0; counter--)
            {
                strm.avail_in = 360 * Scaler_ScaleFactor + 1;
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
        }


        orig = Game_OverlayDraw.ScreenViewpartOriginal + 360 * Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportX;
        src2 = Game_OverlayDraw.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * Game_OverlayDraw.ViewportY + Scaler_ScaleFactor * Game_OverlayDraw.ViewportX;

        // the viewport
        for (y = Game_OverlayDraw.ViewportHeight; y != 0; y--)
        {
            dst8 = &(pixel_data[4]);

            // part left of the viewport
            for (x = Game_OverlayDraw.ViewportX; x != 0; x--)
            {
                value = *src;
                src++;

                for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                {
                    for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                    {
                        dst8[counter + counter2 * (360 * Scaler_ScaleFactor + 4)] = value;
                    }
                }

                dst8 += Scaler_ScaleFactor;
            }

            // the viewport
            for (x = Game_OverlayDraw.ViewportWidth; x != 0; x--)
            {
                value = *src;
                src++;
                orig++;

                if (value == orig[-1])
                {
                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                        {
                            dst8[counter + counter2 * (360 * Scaler_ScaleFactor + 4)] = src2[counter + counter2 * (360 * Scaler_ScaleFactor)];
                        }
                    }
                }
                else
                {
                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                        {
                            dst8[counter + counter2 * (360 * Scaler_ScaleFactor + 4)] = value;
                        }
                    }
                }

                dst8 += Scaler_ScaleFactor;
                src2 += Scaler_ScaleFactor;
            }

            orig += 360 - Game_OverlayDraw.ViewportWidth;
            src2 += (Scaler_ScaleFactor - 1) * Scaler_ScaleFactor * 360 + Scaler_ScaleFactor * (360 - Game_OverlayDraw.ViewportWidth);

            // part right of the viewport
            for (x = 360 - (Game_OverlayDraw.ViewportX + Game_OverlayDraw.ViewportWidth); x != 0; x--)
            {
                value = *src;
                src++;

                for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                {
                    for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                    {
                        dst8[counter + counter2 * (360 * Scaler_ScaleFactor + 4)] = value;
                    }
                }

                dst8 += Scaler_ScaleFactor;
            }

            if (y == 1)
            {
                if (DrawOverlay & 1)
                {
                    dst8 = &(pixel_data[4 + 2 * (Game_OverlayDraw.ViewportX + 1)]);
                    src2 = Game_OverlayDraw.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + Scaler_ScaleFactor * (Game_OverlayDraw.ViewportX + 1);
                    for (x = 8; x != 0; x--)
                    {
                        for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                        {
                            for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                            {
                                dst8[counter + counter2 * (360 * Scaler_ScaleFactor + 4)] = src2[counter + counter2 * (360 * Scaler_ScaleFactor)];
                            }
                        }

                        src2 += Scaler_ScaleFactor;
                        dst8 += Scaler_ScaleFactor;
                    }
                }

                if (DrawOverlay & 2)
                {
                    dst8 = &(pixel_data[4 + 2 * Game_OverlayDraw.ViewportX + 2 * (Game_OverlayDraw.ViewportWidth - 10)]);
                    src2 = Game_OverlayDraw.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + Game_OverlayDraw.ViewportX*2 + Scaler_ScaleFactor * (Game_OverlayDraw.ViewportWidth - 10);
                    for (x = 8; x != 0; x--)
                    {
                        for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                        {
                            for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                            {
                                dst8[counter + counter2 * (360 * Scaler_ScaleFactor + 4)] = src2[counter + counter2 * (360 * Scaler_ScaleFactor)];
                            }
                        }

                        src2 += Scaler_ScaleFactor;
                        dst8 += Scaler_ScaleFactor;
                    }
                }
            }

            // write lines with filter
            for (counter = 0; counter < Scaler_ScaleFactor; counter++)
            {
                strm.avail_in = 360 * Scaler_ScaleFactor + 1;
                strm.next_in = &(pixel_data[3 + counter * (360 * Scaler_ScaleFactor + 4)]);

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
        }

        // part below the viewport
        for (y = 240 - (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight); y != 0; y--)
        {
            // fill first line
            dst8 = &(pixel_data[4]);
            for (x = 360; x != 0; x--)
            {
                value = *src;
                src++;

                for (counter = Scaler_ScaleFactor; counter != 0; counter--)
                {
                    *dst8 = value;
                    dst8++;
                }
            }

            // write lines with filter
            for (counter = Scaler_ScaleFactor; counter != 0; counter--)
            {
                strm.avail_in = 360 * Scaler_ScaleFactor + 1;
                strm.next_in = &(pixel_data[3]);

                ret = zlib_deflate(&strm, ((y == 1) && (counter == 1))?Z_FINISH:Z_NO_FLUSH);
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
#endif

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

#ifdef __cplusplus
extern "C"
#endif
void Game_save_screenshot(const char *filename)
{
    uint8_t *screenshot_src, *buffer, *curptr, *chunk_size_ptr[2];
    uint32_t *buf_scaled;
    unsigned int width, height, width_in_file, palette_index;
    FILE *f;
    int image_mode, DrawOverlay;
    const char *extension;
    char *filename2;

    int x, y;
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
    int counter, counter2;
    uint8_t value;
#endif
    uint8_t *src, *orig, *src2, *dst;

#ifdef ZLIB_DYNAMIC
    if (Game_ScreenshotFormat == 5)
    {
        if (zlib_state < 0) return;
        else if (zlib_state == 0)
        {
            fprintf(stderr, "%s: loading shared object: %s\n", "zlib", "libz.so");
            zlib_handle = dlopen("libz.so", RTLD_LAZY);
            if (zlib_handle == NULL)
            {
                fprintf(stderr, "%s: load error: %s\n", "zlib", dlerror());

                fprintf(stderr, "%s: loading shared object: %s\n", "zlib", "./libz.so");
                zlib_handle = dlopen("./libz.so", RTLD_LAZY);
            }
            if (zlib_handle == NULL)
            {
                fprintf(stderr, "%s: load error: %s\n", "zlib", dlerror());
                zlib_state = -1;
                return;
            }

            zlib_deflateInit2_ = (ZLIB_TYPEOF(zlib_deflateInit2_)) dlsym(zlib_handle, "deflateInit2_");
            zlib_deflate = (ZLIB_TYPEOF(zlib_deflate)) dlsym(zlib_handle, "deflate");
            zlib_deflateEnd = (ZLIB_TYPEOF(zlib_deflateEnd)) dlsym(zlib_handle, "deflateEnd");
            zlib_crc32 = (ZLIB_TYPEOF(zlib_crc32)) dlsym(zlib_handle, "crc32");

            if ((zlib_deflateInit2_ == NULL) || (zlib_deflate == NULL) || (zlib_deflateEnd == NULL) || (zlib_crc32 == NULL))
            {
                fprintf(stderr, "%s: error: %s\n", "zlib", "functions not available in shared object");
                dlclose(zlib_handle);
                zlib_state = -1;
                return;
            }

            fprintf(stderr, "%s: OK\n", "zlib");
            zlib_state = 1;
        }
    }
#endif

    screenshot_src = &(Game_FrameBuffer[loc_182010 * 360 * 240]);
    DrawOverlay = Get_DrawOverlay(screenshot_src, &Game_OverlayDraw);

#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
    if (Game_AdvancedScaling)
    {
        if ((Game_ScreenshotEnhancedResolution != 0) && (Scaler_ScaleFactor > 1))
        {
            width = Scaler_ScaleFactor * 360;
            height = Scaler_ScaleFactor * 240;

            if (Scaler_ScaleTextureData && (Game_ScreenshotFormat > 2))
            {
                image_mode = 11;
            }
            else if (DrawOverlay)
            {
                image_mode = 6;
            }
            else
            {
                image_mode = 5;
            }
        }
        else
        {
            width = 360;
            height = 240;

            image_mode = 4;
        }
    }
    else
#endif
    {
        if ((Render_Width == 720) && (Game_ScreenshotEnhancedResolution != 0))
        {
            width = 720;
            height = 480;

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
            width = 360;
            height = 240;

            image_mode = 0;
        }
    }

    if (Game_ScreenshotFormat == 2)
    {
        width_in_file = ((width + 15) >> 4) * 2 * 8;
    }
    else if (Game_ScreenshotFormat == 4)
    {
        width_in_file = ((((image_mode == 11)?(3*width):width) + 3) >> 2) * 4;
    }
    else if (Game_ScreenshotFormat == 5)
    {
        width_in_file = ((image_mode == 11)?(3*width):width) + 1;
    }
    else
    {
        width_in_file = (image_mode == 11)?(3*width):width;
    }

    buffer = (uint8_t *) malloc(width_in_file * height + 2000);
    if (buffer == NULL) return;

    if (image_mode == 11)
    {
        buf_scaled = (uint32_t *) malloc(width * height * sizeof(uint32_t));
        if (buf_scaled == NULL)
        {
            free(buffer);
            return;
        }
    }
    else buf_scaled = NULL;

    curptr = buffer;

    switch (Game_ScreenshotFormat)
    {
        case 0:
        case 1:
        case 2:
            extension = ".lbm";
            break;
        case 3:
            extension = ".tga";
            break;
        case 4:
            extension = ".bmp";
            break;
        case 5:
            extension = ".png";
            break;
        default:
            extension = NULL;
            break;
    }

    if (Game_ScreenshotAutomaticFilename)
    {
        static int lastnum = 0;

        filename2 = (char *) malloc(10 + 4 + 1 + 3 + 1);
        if (filename2 == NULL)
        {
            if (buf_scaled != NULL) free(buf_scaled);
            free(buffer);
            return;
        }

        if (lastnum != 0)
        {
            sprintf(filename2, "Screenshot%04d%s", lastnum, extension);
            lastnum++;

            // check if file exists
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
            if (INVALID_FILE_ATTRIBUTES != GetFileAttributesA(filename2))
#else
            if (0 == access(filename2, F_OK))
#endif
            {
                lastnum = 0;
            }
        }

        if (lastnum == 0)
        {
            // find file with highest number
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
            HANDLE hFindFile;
            WIN32_FIND_DATAA FindFileData;

            sprintf(filename2, "Screenshot*%s", extension);

            hFindFile = FindFirstFileA(filename2, &FindFileData);
            if (hFindFile != INVALID_HANDLE_VALUE)
            {
                do
                {
                    int filenum;

                    filenum = atoi(&(FindFileData.cFileName[10]));
                    if (filenum >= lastnum)
                    {
                        lastnum = filenum + 1;
                    }
                } while (FindNextFileA(hFindFile, &FindFileData));

                FindClose(hFindFile);
            }
#else
            DIR *dir;
            struct dirent *entry;

            dir = opendir("./");

            if (dir != NULL)
            {
                while (1)
                {
                    int filenum;

                    entry = readdir(dir);
                    if (entry == NULL) break;

                    if (0 != strncasecmp(entry->d_name, "Screenshot", 10)) continue;

                    if (0 != strcasecmp(&(entry->d_name[strlen(entry->d_name) - 4]), extension)) continue;

                    filenum = atoi(&(entry->d_name[10]));
                    if (filenum >= lastnum)
                    {
                        lastnum = filenum + 1;
                    }
                }

                closedir(dir);
            }
#endif
            // todo: find lastnum

            sprintf(filename2, "Screenshot%04d%s", lastnum, extension);
            lastnum++;
        }
    }
    else
    {
        if (NULL == strchr(filename, '.'))
        {
            filename2 = (char *) malloc(8 + 1 + 3 + 1);
            if (filename2 != NULL)
            {
                strncpy(filename2, filename, 8);
                filename2[8] = 0;
                strcat(filename2, extension);
            }
        }
        else
        {
            filename2 = NULL;
        }
    }

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
        *curptr = (image_mode == 11)?0:1;
        curptr++;

        // write Image type (8-bit)
        *curptr = (image_mode == 11)?2:1;
        curptr++;

        // write Color map specification: First entry index (16-bit Little Endian)
        curptr = write_16le(curptr, 0);

        // write Color map specification: Color map length (16-bit Little Endian)
        curptr = write_16le(curptr, (image_mode == 11)?0:256);

        // write Color map specification: Color map entry size (8-bit)
        *curptr = (image_mode == 11)?0:24;
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
        *curptr = (image_mode == 11)?24:8;
        curptr++;

        // write Image specification: Image descriptor (8-bit)
        *curptr = 0x20;
        curptr++;

        if (image_mode != 11)
        {
            // fill palette data
            for (palette_index = 0; palette_index < 256; palette_index++)
            {
                curptr[0] = loc_17E164[4 * palette_index + 2]; // b
                curptr[1] = loc_17E164[4 * palette_index + 1]; // g
                curptr[2] = loc_17E164[4 * palette_index];     // r
                curptr += 3;
            }
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
        curptr = write_32le(curptr, (image_mode == 11)?(14 + 40):(14 + 40 + 1024));

        // write DIB header size (32-bit Little Endian)
        curptr = write_32le(curptr, 40);

        // write The bitmap width in pixels (32-bit Little Endian)
        curptr = write_32le(curptr, width);

        // write The bitmap height in pixels (32-bit Little Endian)
        curptr = write_32le(curptr, -(int32_t)height);

        // write The number of color planes (16-bit Little Endian)
        curptr = write_16le(curptr, 1);

        // write The number of bits per pixel (16-bit Little Endian)
        curptr = write_16le(curptr, (image_mode == 11)?24:8);

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

        if (image_mode != 11)
        {
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
        *curptr = (image_mode == 11)?2:3;
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
        curptr = write_32be(curptr,
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
            (Game_AdvancedScaling)?(Scaler_ScaleTextureData * 320):
#endif
            640
        );

        // write Pixels per unit, Y axis
        curptr = write_32be(curptr,
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
            (Game_AdvancedScaling)?(Scaler_ScaleTextureData * 240):
#endif
            480
        );

        // write Unit specifier
        *curptr = 0;
        curptr++;

        // write "pHYs" chunk crc32
        curptr = write_32be(curptr, zlib_crc32(0, curptr - (4 + 9), 4 + 9));

        if (image_mode != 11)
        {
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
        }


        // "IDAT" chunk

        // space for "IDAT" chunk size
        chunk_size_ptr[0] = curptr;
        curptr += 4;

        // write "IDAT" chunk id
        memcpy(curptr, "IDAT", 4);
        curptr += 4;
    }

    // fill pixel data
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
    if (image_mode == 11)
    {
        uint32_t *palette, *buf_unscaled, *src32, *dst32;

        // temporary data
        palette = (uint32_t *) ((3 + (uintptr_t)curptr) & ~(uintptr_t)3);
        buf_unscaled = palette + 256;

        // palette data
        for (palette_index = 0; palette_index < 256; palette_index++)
        {
            palette[palette_index] =  loc_17E164[4 * palette_index + 2]        // b
                                   | (loc_17E164[4 * palette_index + 1] <<  8) // g
                                   | (loc_17E164[4 * palette_index    ] << 16) // r
                                   | 0xff000000;                               // a
        }

        if (DrawOverlay)
        {
            src = screenshot_src;
            dst32 = buf_unscaled;

            // part above the viewport
            for (counter = 360 * Game_OverlayDraw.ViewportY; counter != 0; counter--)
            {
                dst32[0] = palette[src[0]];

                src++;
                dst32++;
            }

            orig = Game_OverlayDraw.ScreenViewpartOriginal + 360 * Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportX;

            // the viewport
            for (y = Game_OverlayDraw.ViewportHeight; y != 0; y--)
            {
                // part left of the viewport
                for (x = Game_OverlayDraw.ViewportX; x != 0; x--)
                {
                    dst32[0] = palette[src[0]];
                    src++;
                    dst32++;
                }

                // the viewport
                for (x = Game_OverlayDraw.ViewportWidth; x != 0; x--)
                {
                    value = *src;
                    src++;
                    orig++;
                    dst32[0] = (value == orig[-1])?0:palette[value];
                    dst32++;
                }

                orig += 360 - Game_OverlayDraw.ViewportWidth;

                // part right of the viewport
                for (x = 360 - (Game_OverlayDraw.ViewportX + Game_OverlayDraw.ViewportWidth); x != 0; x--)
                {
                    dst32[0] = palette[src[0]];
                    src++;
                    dst32++;
                }
            }

            // part below the viewport
            for (counter = 360 * (240 - (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight)); counter != 0; counter--)
            {
                dst32[0] = palette[src[0]];

                src++;
                dst32++;
            }

            // markers
            if (DrawOverlay & 1)
            {
                dst32 = buf_unscaled + 360 * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + (Game_OverlayDraw.ViewportX + 1);

                for (x = 8; x != 0; x--)
                {
                    dst32[0] = 0;
                    dst32++;
                }
            }

            if (DrawOverlay & 2)
            {
                dst32 = buf_unscaled + 360 * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + (Game_OverlayDraw.ViewportX + Game_OverlayDraw.ViewportWidth - 10);

                for (x = 8; x != 0; x--)
                {
                    dst32[0] = 0;
                    dst32++;
                }
            }
        }
        else
        {
            for (counter = 0; counter < 360*240; counter++)
            {
                buf_unscaled[counter] = palette[screenshot_src[counter]];
            }
        }

        ScalerPlugin_scale(Scaler_ScaleFactor, buf_unscaled, buf_scaled, 360, 240, 0);

        if (DrawOverlay)
        {
            src32 = buf_unscaled + 360 * Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportX;
            dst32 = buf_scaled + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * Game_OverlayDraw.ViewportY + Scaler_ScaleFactor * Game_OverlayDraw.ViewportX;
            src2 = Game_OverlayDraw.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * Game_OverlayDraw.ViewportY + Scaler_ScaleFactor * Game_OverlayDraw.ViewportX;

            // the viewport
            for (y = Game_OverlayDraw.ViewportHeight; y != 0; y--)
            {
                // the viewport
                for (x = Game_OverlayDraw.ViewportWidth; x != 0; x--)
                {
                    src32++;
                    if (src32[-1] == 0)
                    {
                        for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                        {
                            for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                            {
                                dst32[counter + counter2 * Scaler_ScaleFactor * 360] = palette[src2[counter + counter2 * Scaler_ScaleFactor * 360]];
                            }
                        }
                    }

                    dst32 += Scaler_ScaleFactor;
                    src2 += Scaler_ScaleFactor;
                }

                src32 += 360 - Game_OverlayDraw.ViewportWidth;
                dst32 += (Scaler_ScaleFactor - 1) * Scaler_ScaleFactor * 360 + Scaler_ScaleFactor * (360 - Game_OverlayDraw.ViewportWidth);
                src2 += (Scaler_ScaleFactor - 1) * Scaler_ScaleFactor * 360 + Scaler_ScaleFactor * (360 - Game_OverlayDraw.ViewportWidth);
            }
        }

        if (Game_ScreenshotFormat == 5)
        {
            curptr = fill_png_pixel_data_advanced(buf_scaled, width_in_file * height + (2000 - 16) - (curptr - buffer), curptr);
            if (curptr == NULL)
            {
                if (filename2 != NULL) free(filename2);
                if (buf_scaled != NULL) free(buf_scaled);
                free(buffer);
                return;
            }
        }
        else
        {
            src32 = buf_scaled;
            for (y = height; y != 0; y--)
            {
                for (x = width; x != 0; x--)
                {
                    curptr[0] = *src32 & 0xff;
                    curptr[1] = (*src32 >> 8) & 0xff;
                    curptr[2] = (*src32 >> 16) & 0xff;
                    curptr += 3;
                    src32++;
                }

                memset(curptr, 0, width_in_file - 3 * width);
                curptr += width_in_file - 3 * width;
            }
        }

    }
    else
#endif
    if (Game_ScreenshotFormat == 5)
    {
        curptr = fill_png_pixel_data(screenshot_src, image_mode, DrawOverlay, width_in_file * height + (2000 - 16) - (curptr - buffer), curptr);
        if (curptr == NULL)
        {
            if (filename2 != NULL) free(filename2);
            if (buf_scaled != NULL) free(buf_scaled);
            free(buffer);
            return;
        }
    }
    else
    {
        if ((image_mode == 0) || (image_mode == 4))
        {
            unsigned int src2_stride, src2_factor;

#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
            if (image_mode != 0)
            {
                src2_stride = Scaler_ScaleFactor * 360;
                src2_factor = Scaler_ScaleFactor;
            }
            else
#endif
            {
                src2_stride = 800;
                src2_factor = 2;
            }

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
                dst = curptr - width_in_file * height + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) * width_in_file + (Game_OverlayDraw.ViewportX + 1);
                src2 = Game_OverlayDraw.ScreenViewpartOverlay + src2_factor * src2_stride * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + src2_factor * (Game_OverlayDraw.ViewportX + 1);
                for (x = 8; x != 0; x--)
                {
                    dst[0] = src2[0];

                    src2 += src2_factor;
                    dst++;
                }
            }

            if (DrawOverlay & 2)
            {
                dst = curptr - width_in_file * height + (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) * width_in_file + Game_OverlayDraw.ViewportX + (Game_OverlayDraw.ViewportWidth - 10);
                src2 = Game_OverlayDraw.ScreenViewpartOverlay + src2_factor * src2_stride * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + Game_OverlayDraw.ViewportX*2 + src2_factor * (Game_OverlayDraw.ViewportWidth - 10);
                for (x = 8; x != 0; x--)
                {
                    dst[0] = src2[0];

                    src2 += src2_factor;
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
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
        else if (image_mode == 5)
        {
            src = screenshot_src;
            for (y = 240; y != 0; y--)
            {
                for (x = 360; x != 0; x--)
                {
                    value = *src;
                    src++;

                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                        {
                            curptr[counter + counter2 * width_in_file] = value;
                        }
                    }

                    curptr += Scaler_ScaleFactor;
                }

                for (x = width_in_file - width; x != 0; x--)
                {
                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        curptr[counter2 * width_in_file] = 0;
                    }

                    curptr++;
                }

                curptr += (Scaler_ScaleFactor - 1) * width_in_file;
            }
        }
#endif
        else if (image_mode == 2)
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
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
        else if (image_mode == 6)
        {
            src = screenshot_src;

            // part above the viewport
            for (y = Game_OverlayDraw.ViewportY; y != 0; y--)
            {
                for (x = 360; x != 0; x--)
                {
                    value = *src;
                    src++;

                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                        {
                            curptr[counter + counter2 * width_in_file] = value;
                        }
                    }

                    curptr += Scaler_ScaleFactor;
                }

                for (x = width_in_file - width; x != 0; x--)
                {
                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        curptr[counter2 * width_in_file] = 0;
                    }

                    curptr++;
                }

                curptr += (Scaler_ScaleFactor - 1) * width_in_file;
            }

            orig = Game_OverlayDraw.ScreenViewpartOriginal + 360 * Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportX;
            src2 = Game_OverlayDraw.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * Game_OverlayDraw.ViewportY + Scaler_ScaleFactor * Game_OverlayDraw.ViewportX;

            // the viewport
            for (y = Game_OverlayDraw.ViewportHeight; y != 0; y--)
            {
                // part left of the viewport
                for (x = Game_OverlayDraw.ViewportX; x != 0; x--)
                {
                    value = *src;
                    src++;

                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                        {
                            curptr[counter + counter2 * width_in_file] = value;
                        }
                    }

                    curptr += Scaler_ScaleFactor;
                }

                // the viewport
                for (x = Game_OverlayDraw.ViewportWidth; x != 0; x--)
                {
                    value = *src;
                    src++;
                    orig++;

                    if (value == orig[-1])
                    {
                        for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                        {
                            for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                            {
                                curptr[counter + counter2 * width_in_file] = src2[counter + counter2 * (Scaler_ScaleFactor * 360)];
                            }
                        }
                    }
                    else
                    {
                        for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                        {
                            for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                            {
                                curptr[counter + counter2 * width_in_file] = value;
                            }
                        }
                    }

                    curptr += Scaler_ScaleFactor;
                    src2 += Scaler_ScaleFactor;
                }

                orig += 360 - Game_OverlayDraw.ViewportWidth;
                src2 += (Scaler_ScaleFactor - 1 ) * Scaler_ScaleFactor * 360 + Scaler_ScaleFactor * (360 - Game_OverlayDraw.ViewportWidth);

                // part right of the viewport
                for (x = 360 - (Game_OverlayDraw.ViewportX + Game_OverlayDraw.ViewportWidth); x != 0; x--)
                {
                    value = *src;
                    src++;

                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                        {
                            curptr[counter + counter2 * width_in_file] = value;
                        }
                    }

                    curptr += Scaler_ScaleFactor;
                }

                for (x = width_in_file - width; x != 0; x--)
                {
                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        curptr[counter2 * width_in_file] = 0;
                    }

                    curptr++;
                }

                curptr += (Scaler_ScaleFactor - 1) * width_in_file;
            }

            if (DrawOverlay & 1)
            {
                dst = curptr - 2 * width_in_file * 2 + 2 * (Game_OverlayDraw.ViewportX + 1);
                src2 = Game_OverlayDraw.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + Scaler_ScaleFactor * (Game_OverlayDraw.ViewportX + 1);
                for (x = 8; x != 0; x--)
                {
                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                        {
                            dst[counter + counter2 * width_in_file] = src2[counter + counter2 * (Scaler_ScaleFactor * 360)];
                        }
                    }

                    src2 += Scaler_ScaleFactor;
                    dst += Scaler_ScaleFactor;
                }
            }

            if (DrawOverlay & 2)
            {
                dst = curptr - 2 * width_in_file * 2 + 2 * Game_OverlayDraw.ViewportX + 2 * (Game_OverlayDraw.ViewportWidth - 10);
                src2 = Game_OverlayDraw.ScreenViewpartOverlay + Scaler_ScaleFactor * 360 * Scaler_ScaleFactor * (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight - 2) + Game_OverlayDraw.ViewportX*2 + Scaler_ScaleFactor * (Game_OverlayDraw.ViewportWidth - 10);
                for (x = 8; x != 0; x--)
                {
                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                        {
                            dst[counter + counter2 * width_in_file] = src2[counter + counter2 * (Scaler_ScaleFactor * 360)];
                        }
                    }

                    src2 += Scaler_ScaleFactor;
                    dst += Scaler_ScaleFactor;
                }
            }

            // part below the viewport
            for (y = 240 - (Game_OverlayDraw.ViewportY + Game_OverlayDraw.ViewportHeight); y != 0; y--)
            {
                for (x = 360; x != 0; x--)
                {
                    value = *src;
                    src++;

                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        for (counter = 0; counter < Scaler_ScaleFactor; counter++)
                        {
                            curptr[counter + counter2 * width_in_file] = value;
                        }
                    }

                    curptr += Scaler_ScaleFactor;
                }

                for (x = width_in_file - width; x != 0; x--)
                {
                    for (counter2 = 0; counter2 < Scaler_ScaleFactor; counter2++)
                    {
                        curptr[counter2 * width_in_file] = 0;
                    }

                    curptr++;
                }

                curptr += (Scaler_ScaleFactor - 1) * width_in_file;
            }
        }
#endif
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

    if (Game_ScreenshotAutomaticFilename)
    {
        f = fopen(filename2, "wb");
    }
    else
    {
        f = Game_fopen((filename2 != NULL)?filename2:filename, "wb");
    }
    if (f != NULL)
    {
        fwrite(buffer, 1, curptr - buffer, f);
        fclose(f);
    }

    if (filename2 != NULL) free(filename2);
    if (buf_scaled != NULL) free(buf_scaled);
    free(buffer);
}

