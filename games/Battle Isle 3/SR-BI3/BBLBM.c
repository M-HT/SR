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

#include "BBLBM.h"
#include "BBMEM.h"
#include "BBDSA.h"
#include "BBOPM.h"
#include "BBDOS.h"
#include <stddef.h>
#include <string.h>


typedef struct {
    const uint8_t *buffer;
    int length;
    const uint8_t *bmhd_chunk;
    const uint8_t *body_chunk;
    const uint8_t *cmap_chunk;
} LBM_struct;


static int LBM_serieNumber = 0;
static char LBM_autoSaveName[13];
static char LBM_serieName[13];


static const uint8_t *LBM_FindChunk_c(LBM_struct *lbmdata, const char *chunk_id)
{
    int bufferindex, idindex, matchlength;

    for (bufferindex = 0; bufferindex < lbmdata->length; bufferindex++)
    {
        matchlength = 0;
        for (idindex = 0; idindex < 4; idindex++)
        {
            if (lbmdata->buffer[bufferindex + idindex] == chunk_id[idindex])
            {
                matchlength++;
            }
        }

        if (matchlength == 4)
        {
            return &(lbmdata->buffer[bufferindex]);
        }
    }

    return NULL;
}

static void LBM_GetPal_c(LBM_struct *lbmdata, DSA_Palette *palette)
{
    const uint8_t *cmap_ptr;
    int index;

    cmap_ptr = lbmdata->cmap_chunk + 8;
    for (index = 0; index < 256; index++)
    {
        palette->palPalEntry[index].peRed   = cmap_ptr[0];
        palette->palPalEntry[index].peGreen = cmap_ptr[1];
        palette->palPalEntry[index].peBlue  = cmap_ptr[2];
        cmap_ptr += 3;
    }
}

static void LBM_DisplayLBMinOPM_c(LBM_struct *lbmdata, OPM_struct *pixel_map)
{
    const uint8_t *bmhd_ptr, *body_ptr;
    int width, height, x, y, copy_length, index;
    uint8_t value;

    bmhd_ptr = lbmdata->bmhd_chunk + 8;
    // change: removed pointless abs
    //width = abs(bmhd_ptr[1] + (bmhd_ptr[0] << 8));
    width = bmhd_ptr[1] + (bmhd_ptr[0] << 8);
    // change: removed pointless abs
    //height = abs(bmhd_ptr[3] + (bmhd_ptr[2] << 8));
    height = bmhd_ptr[3] + (bmhd_ptr[2] << 8);

    body_ptr = lbmdata->body_chunk + 8;

    if (bmhd_ptr[10])
    {
        // compressed data

        x = 0;
        y = 0;
        do
        {
            copy_length = (int8_t)*body_ptr;
            body_ptr++;

            if (copy_length < 0)
            {
                if (copy_length > -128)
                {
                    value = *body_ptr;
                    body_ptr++;
                    copy_length = -copy_length;
                    for (index = 0; index <= copy_length; index++)
                    {
                        OPM_SetPixel_c(pixel_map, x, y, value);
                        x++;
                    }
                }
            }
            else
            {
                for (index = 0; index <= copy_length; index++)
                {
                    OPM_SetPixel_c(pixel_map, x, y, *body_ptr);
                    x++;
                    body_ptr++;
                }
            }

            if (x == width)
            {
                x = 0;
                y++;
            }
        }
        while (y < height);
    }
    else
    {
        // uncompressed data

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                OPM_SetPixel_c(pixel_map, x, y, *body_ptr);
                body_ptr++;
            }
        }
    }
}

int CCALL LBM_DisplayLBM_c(const char *path, void *pixel_map, uint8_t *palette, unsigned int flags)
{
    LBM_struct lbmdata;
    const uint8_t *bmhd_ptr;
    int width, height;

    if (flags & 0x10000)
    {
        lbmdata.length = 100000000;
        lbmdata.buffer = (uint8_t *)path;
    }
    else
    {
        lbmdata.length = DOS_GetFileLength_c(path);
        if (lbmdata.length < 0)
        {
            return 0;
        }

        lbmdata.buffer = (uint8_t *)DOS_ReadFile_c(path, NULL);
        if (lbmdata.buffer == NULL)
        {
            return 0;
        }
    }

    lbmdata.bmhd_chunk = LBM_FindChunk_c(&lbmdata, "BMHD");
    if (lbmdata.bmhd_chunk == NULL)
    {
        return 0;
    }

    lbmdata.body_chunk = LBM_FindChunk_c(&lbmdata, "BODY");
    if (lbmdata.body_chunk == NULL)
    {
        return 0;
    }

    lbmdata.cmap_chunk = LBM_FindChunk_c(&lbmdata, "CMAP");
    if (lbmdata.cmap_chunk == NULL)
    {
        return 0;
    }

    if (palette != NULL)
    {
        LBM_GetPal_c(&lbmdata, (DSA_Palette *)palette);
    }

    bmhd_ptr = lbmdata.bmhd_chunk + 8;
    // change: removed pointless abs
    //width = abs(bmhd_ptr[1] + (bmhd_ptr[0] << 8));
    width = bmhd_ptr[1] + (bmhd_ptr[0] << 8);
    // change: removed pointless abs
    //height = abs(bmhd_ptr[3] + (bmhd_ptr[2] << 8));
    height = bmhd_ptr[3] + (bmhd_ptr[2] << 8);

    if (flags & 1)
    {
        if (!OPM_New_c(width, height, 1, (OPM_struct *)pixel_map, NULL))
        {
            return 0;
        }
    }
    else if (flags & 2)
    {
        ((OPM_struct *)pixel_map)->width = width;
        ((OPM_struct *)pixel_map)->height = height;
        ((OPM_struct *)pixel_map)->stride = width;
        ((OPM_struct *)pixel_map)->size = width * height;
        ((OPM_struct *)pixel_map)->clip_x = 0;
        ((OPM_struct *)pixel_map)->clip_y = 0;
        ((OPM_struct *)pixel_map)->clip_width = width;
        ((OPM_struct *)pixel_map)->clip_height = height;
    }

    LBM_DisplayLBMinOPM_c(&lbmdata, (OPM_struct *)pixel_map);

    if (!(flags & 0x10000))
    {
        MEM_free_c((void *)lbmdata.buffer);
    }

    return 1;
}

static uint8_t *LBM_WriteBE16_c(uint8_t *buffer, uint16_t value)
{
    buffer[0] = value >> 8;
    buffer[1] = value & 0xff;

    return buffer + 2;
}

static uint8_t *LBM_WriteBE32_c(uint8_t *buffer, uint32_t value)
{
    buffer[0] = value >> 24;
    buffer[2] = (value >> 16) & 0xff;
    buffer[3] = (value >> 8) & 0xff;
    buffer[4] = value & 0xff;

    return buffer + 4;
}

static uint8_t *LBM_WriteChunkID_c(uint8_t *buffer, const char *chunkname)
{
    int index;

    for (index = 0; index < 4; index++)
    {
        *buffer = chunkname[index];
        buffer++;
    }

    return buffer;
}

static void LBM_SaveLBM_c(const char *path, OPM_struct *pixel_map)
{
    uint8_t *buffer, *cur_ptr, *chunk_size_ptr[2];
    int index;
    OPM_struct pm2;
    DSA_Palette palette;

    buffer = (uint8_t *)MEM_malloc_c(pixel_map->height * pixel_map->width + 2000, NULL, NULL, 0, 0);

    DSA_GetPalette_c(&palette);

    // write "FORM" chunk id
    chunk_size_ptr[0] = LBM_WriteChunkID_c(buffer, "FORM");

    // write "PBM " id
    cur_ptr = LBM_WriteChunkID_c(chunk_size_ptr[0] + 4, "PBM ");

    // write "BMHD" chunk id
    chunk_size_ptr[1] = LBM_WriteChunkID_c(cur_ptr, "BMHD");

    // write image width (16-bit Big Endian)
    cur_ptr = LBM_WriteBE16_c(chunk_size_ptr[1] + 4, pixel_map->width);

    // write image height (16-bit Big Endian)
    cur_ptr = LBM_WriteBE16_c(cur_ptr, pixel_map->height);

    // write xOrigin (16-bit Big Endian)
    cur_ptr = LBM_WriteBE16_c(cur_ptr, 0);

    // write yOrigin (16-bit Big Endian)
    cur_ptr = LBM_WriteBE16_c(cur_ptr, 0);

    // write numPlanes (image bpp) (8-bit)
    cur_ptr[0] = 0;

    // write mask (8-bit)
    cur_ptr[1] = 0;

    // write compression (8-bit)
    cur_ptr[2] = 0;

    // write padding (8-bit)
    cur_ptr[3] = 0;

    // write Transparent colour (16-bit Big Endian)
    cur_ptr = LBM_WriteBE16_c(cur_ptr + 4, 0);

    // write xAspect (8-bit)
    cur_ptr[0] = 10;

    // write yAspect (8-bit)
    cur_ptr[1] = 11;

    // write pageWidth (16-bit Big Endian)
    cur_ptr = LBM_WriteBE16_c(cur_ptr + 2, pixel_map->width);

    // write pageHeight (16-bit Big Endian)
    cur_ptr = LBM_WriteBE16_c(cur_ptr, pixel_map->height);

    // write "BMHD" chunk size (32-bit Big Endian)
    LBM_WriteBE32_c(chunk_size_ptr[1], (cur_ptr - chunk_size_ptr[1]) - 4);

    // write "CMAP" chunk id
    chunk_size_ptr[1] = LBM_WriteChunkID_c(cur_ptr, "CMAP");

    // space for "CMAP" chunk size
    cur_ptr = chunk_size_ptr[1] + 4;

    // fill "CMAP" chunk
    for (index = 0; index < 256; index++)
    {
        cur_ptr[0] = palette.palPalEntry[index].peRed;
        cur_ptr[1] = palette.palPalEntry[index].peGreen;
        cur_ptr[2] = palette.palPalEntry[index].peBlue;
        cur_ptr += 3;
    }

    // write "CMAP" chunk size (32-bit Big Endian)
    LBM_WriteBE32_c(chunk_size_ptr[1], (cur_ptr - chunk_size_ptr[1]) - 4);

    // write "BODY" chunk id
    chunk_size_ptr[1] = LBM_WriteChunkID_c(cur_ptr, "BODY");

    // space for "BODY" chunk size
    cur_ptr = chunk_size_ptr[1] + 4;

    OPM_New_c(pixel_map->width, pixel_map->height, 1, &pm2, cur_ptr);

    // fill "BODY" chunk
    memmove(pm2.buffer, pixel_map->buffer, pixel_map->height * pixel_map->width);
    cur_ptr += pixel_map->height * pixel_map->width;

    // write "BODY" chunk size (32-bit Big Endian)
    LBM_WriteBE32_c(chunk_size_ptr[1], (cur_ptr - chunk_size_ptr[1]) - 4);

    // write "FORM" chunk size (32-bit Big Endian)
    LBM_WriteBE32_c(chunk_size_ptr[0], (cur_ptr - chunk_size_ptr[0]) - 4);

    DOS_WriteFile_c(path, buffer, cur_ptr - buffer);

    OPM_Del_c(&pm2);

    MEM_free_c(buffer);
}

void CCALL LBM_StartSerie_c(int number)
{
    if ((number < 0) || (number > 999))
    {
        return;
    }

    memmove(LBM_serieName, "Dxxx_yyy.LBM", 13);

    if (number < 100)
    {
        LBM_serieName[1] = '0';
    }
    else
    {
        LBM_serieName[1] = (number / 100) + '0';
        number %= 100;
    }

    if (number < 10)
    {
        LBM_serieName[2] = '0';
    }
    else
    {
        LBM_serieName[2] = (number / 10) + '0';
        number %= 10;
    }

    if (number < 1)
    {
        LBM_serieName[3] = '0';
    }
    else
    {
        LBM_serieName[3] = number + '0';
    }

    LBM_serieNumber = 0;
}

void CCALL LBM_AutoSave_c(void *pixel_map)
{
    if ((LBM_serieNumber < 0) || (LBM_serieNumber > 999))
    {
        return;
    }

    memmove(LBM_autoSaveName, LBM_serieName, 13);

    if (LBM_serieNumber < 100)
    {
        LBM_autoSaveName[5] = '0';
    }
    else
    {
        LBM_autoSaveName[5] = (LBM_serieNumber / 100) + '0';
        LBM_serieNumber %= 100;
    }

    if (LBM_serieNumber < 10)
    {
        LBM_autoSaveName[6] = '0';
    }
    else
    {
        LBM_autoSaveName[6] = (LBM_serieNumber / 10) + '0';
        LBM_serieNumber %= 10;
    }

    if (LBM_serieNumber < 1)
    {
        LBM_autoSaveName[7] = '0';
    }
    else
    {
        LBM_autoSaveName[7] = LBM_serieNumber + '0';
    }

    LBM_serieNumber++;

    LBM_SaveLBM_c(LBM_autoSaveName, (OPM_struct *)pixel_map);
}

