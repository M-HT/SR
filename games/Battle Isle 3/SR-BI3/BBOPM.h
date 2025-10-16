/**
 *
 *  Copyright (C) 2020-2025 Roman Pauer
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

#if !defined(_BBOPM_H_INCLUDED_)
#define _BBOPM_H_INCLUDED_

#include <stdint.h>
#include "platform.h"

#define BBOPM_OWNED_BUFFER 1
#define BBOPM_MODIFIED 2
#define BBOPM_CREATED 4
#define BBOPM_VIRTUAL 8
#define BBOPM_SECONDARY 0x10


#pragma pack(1)

typedef struct PACKED _OPM_struct {
    uint32_t flags;
    int16_t width;
    int16_t height;
    int16_t bytes_per_pixel;
    uint8_t *buffer;
    uint32_t size;
    uint16_t bytes_per_pixel2;
    int16_t stride;
    int16_t origin_x;
    int16_t origin_y;
    int16_t clip_x;
    int16_t clip_y;
    int16_t clip_width;
    int16_t clip_height;
    int16_t virtual_x;
    int16_t virtual_y;
    struct _OPM_struct *base_pixel_map;
    void *hDC;
    void *hBitmap;
    void *hSelected;
    uint32_t access_offset;
} OPM_struct;

typedef struct PACKED _GFX_struct {
    uint32_t unknown_00;
    uint32_t unknown_04;
    uint32_t unknown_08;
    uint8_t transparent_color;
    uint8_t unknown_0D;
    int16_t pos_x;
    int16_t pos_y;
    uint16_t width;
    uint16_t height;
    uint32_t unknown_16;
    uint8_t data[1];
} GFX_struct;

#pragma pack()


#ifdef __cplusplus
extern "C" {
#endif

int OPM_Init_c(void);
void OPM_Exit_c(void);
int OPM_New_c(unsigned int width, unsigned int height, unsigned int bytes_per_pixel, OPM_struct *pixel_map, uint8_t *buffer);
void OPM_Del_c(OPM_struct *pixel_map);
void OPM_CreateVirtualOPM_c(OPM_struct *base_pixel_map, OPM_struct *virtual_pixel_map, int virtual_x, int virtual_y, int virtual_width, int virtual_height);
int OPM_CreateSecondaryOPM_c(int width, int height, int bytes_per_pixel, OPM_struct *pixel_map, uint8_t *buffer);
void OPM_SetPixel_c(OPM_struct *pixel_map, int x, int y, uint8_t color);
void OPM_HorLine_c(OPM_struct *pixel_map, int x, int y, unsigned int length, uint8_t color);
void OPM_VerLine_c(OPM_struct *pixel_map, int x, int y, unsigned int length, uint8_t color);
void OPM_FillBox_c(OPM_struct *pixel_map, int x, int y, unsigned int width, unsigned int height, uint8_t color);
void OPM_CopyGFXOPM_c(OPM_struct *pixel_map, GFX_struct *gfx, int pos_x, int pos_y, uint8_t value_add);
void OPM_CopyOPMOPM_c(OPM_struct *src_pixel_map, OPM_struct *dst_pixel_map, int src_x, int src_y, int copy_width, int copy_height, int dst_x, int dst_y);
void OPM_AccessBitmap_c(OPM_struct *pixel_map);

#ifdef __cplusplus
}
#endif

#endif /* _BBOPM_H_INCLUDED_ */
