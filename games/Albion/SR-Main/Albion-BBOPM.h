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

#if !defined(_ALBION_BBOPM_H_INCLUDED_)
#define _ALBION_BBOPM_H_INCLUDED_

#include <stdint.h>

#define BBOPM_ALLOCATED_BUFFER 1
#define BBOPM_MODIFIED 2
#define BBOPM_UNKNOWN4 4
#define BBOPM_VIEW 8
#define BBOPM_TRANSPARENCY 0x10

// todo: align struct members
#pragma pack(1)

typedef struct __attribute__ ((__packed__)) _OPM_Struct {
    uint32_t flags;
    int16_t width;
    int16_t height;
    int16_t bytes_per_pixel;
    uint8_t *buffer;
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
    int16_t view_x;
    int16_t view_y;
    struct _OPM_Struct *base_pixel_map;
} OPM_Struct;

#pragma pack()


#ifdef __cplusplus
extern "C" {
#endif

extern int OPM_New(unsigned int width, unsigned int height, unsigned int bytes_per_pixel, OPM_Struct *pixel_map, uint8_t *buffer);
extern void OPM_Del(OPM_Struct *pixel_map);
extern void OPM_SetViewClipStart(OPM_Struct *view_pixel_map, int clip_x, int clip_y);
extern void OPM_CreateVirtualOPM(OPM_Struct *base_pixel_map, OPM_Struct *view_pixel_map, int view_x, int view_y, int view_width, int view_height);
extern void OPM_SetPixel(OPM_Struct *pixel_map, int x, int y, uint8_t color);
extern void OPM_HorLine(OPM_Struct *pixel_map, int x, int y, int length, uint8_t color);
extern void OPM_VerLine(OPM_Struct *pixel_map, int x, int y, int length, uint8_t color);
extern void OPM_Box(OPM_Struct *pixel_map, int x, int y, int width, int height, uint8_t color);
extern void OPM_FillBox(OPM_Struct *pixel_map, int x, int y, int width, int height, uint8_t color);
extern void OPM_CopyOPMOPM(OPM_Struct *src_pixel_map, OPM_Struct *dst_pixel_map, int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y);


#ifdef __cplusplus
}
#endif

#endif /* _ALBION_BBOPM_H_INCLUDED_ */
