/**
 *
 *  Copyright (C) 2018-2023 Roman Pauer
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

#if !defined(_ALBION_BBOPM_H_INCLUDED_)
#define _ALBION_BBOPM_H_INCLUDED_

#include "ptr32.h"

#define BBOPM_ALLOCATED_BUFFER 1
#define BBOPM_MODIFIED 2
#define BBOPM_UNKNOWN4 4
#define BBOPM_VIRTUAL 8
#define BBOPM_TRANSPARENCY 0x10

// todo: align struct members
#pragma pack(1)

typedef struct _OPM_Struct {
    uint32_t flags;
    int16_t width;
    int16_t height;
    int16_t bytes_per_pixel;
    PTR32(uint8_t) buffer;
    uint32_t size;
    int16_t bytes_per_pixel2;
    int16_t stride;
    int16_t origin_x;
    int16_t origin_y;
    int16_t clip_x;
    int16_t clip_y;
    int16_t clip_width;
    int16_t clip_height;
    uint16_t transparent_color;
    int16_t virtual_x;
    int16_t virtual_y;
    PTR32(struct _OPM_Struct) base_pixel_map;
} OPM_Struct;

#pragma pack()


#ifdef __cplusplus
extern "C" {
#endif

// note: parameters smaller than 32 bits are extended to 32 bits by callers in the original asm code

extern int32_t OPM_New(uint32_t width, uint32_t height, uint32_t bytes_per_pixel, OPM_Struct *pixel_map, uint8_t *buffer);
extern void OPM_Del(OPM_Struct *pixel_map);
extern void OPM_SetVirtualClipStart(OPM_Struct *virtual_pixel_map, int32_t clip_x, int32_t clip_y);
extern void OPM_CreateVirtualOPM(OPM_Struct *base_pixel_map, OPM_Struct *virtual_pixel_map, int32_t virtual_x, int32_t virtual_y, int32_t virtual_width, int32_t virtual_height);
extern void OPM_SetPixel(OPM_Struct *pixel_map, int32_t x, int32_t y, uint8_t color);
extern void OPM_HorLine(OPM_Struct *pixel_map, int32_t x, int32_t y, int32_t length, uint8_t color);
extern void OPM_VerLine(OPM_Struct *pixel_map, int32_t x, int32_t y, int32_t length, uint8_t color);
extern void OPM_Box(OPM_Struct *pixel_map, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color);
extern void OPM_FillBox(OPM_Struct *pixel_map, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color);
extern void OPM_CopyOPMOPM(OPM_Struct *src_pixel_map, OPM_Struct *dst_pixel_map, int32_t src_x, int32_t src_y, int32_t src_width, int32_t src_height, int32_t dst_x, int32_t dst_y);


#ifdef __cplusplus
}
#endif

#endif /* _ALBION_BBOPM_H_INCLUDED_ */
