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

#if !defined(_SMK_DEFS_H_INCLUDED_)
#define _SMK_DEFS_H_INCLUDED_

#include <stdint.h>

#define DR_640x480x32 1
#define DR_320x240x16 3
#define DR_800x480x16 4

#define PALETTE_RGB 0
#define PALETTE_BGR 1

#if defined(GP2X)
	#define DISPLAY_RES DR_320x240x16
	#define DISPLAY_PALETTE PALETTE_RGB
	#define DISPLAY_FULLSCREEN 0
#elif defined(PANDORA)
	#define DISPLAY_RES DR_800x480x16
	#define DISPLAY_PALETTE PALETTE_RGB
	#define DISPLAY_FULLSCREEN 1
#elif defined(__WIN32__)
	#define DISPLAY_RES DR_640x480x32
	#define DISPLAY_PALETTE PALETTE_RGB
	#define DISPLAY_FULLSCREEN 0
#else
	#define DISPLAY_RES DR_640x480x32
	#define DISPLAY_PALETTE PALETTE_RGB
	#define DISPLAY_FULLSCREEN 0
#endif



#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_X               (14)
#define GP2X_BUTTON_Y               (15)
#define GP2X_BUTTON_L               (10)
#define GP2X_BUTTON_R               (11)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

typedef enum _FrameState_ {
	Frame_Empty,
	Frame_Reading,
	Frame_Full
} FrameState;


typedef struct _FrameData_ {
	FrameState state;
} FrameData;


#if (DISPLAY_RES == DR_640x480x32)

	#if (DISPLAY_PALETTE == PALETTE_RGB)
typedef union _pixel_format_ {
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	} s;
	uint32_t pix;
} pixel_format;
	#else
typedef union _pixel_format_ {
	struct {
		uint8_t a;
		uint8_t b;
		uint8_t g;
		uint8_t r;
	} s;
	uint32_t pix;
} pixel_format;
	#endif

#elif (DISPLAY_RES == DR_320x240x16) || (DISPLAY_RES == DR_800x480x16)

	#if (DISPLAY_PALETTE == PALETTE_RGB)
typedef union _pixel_format_ {
	struct {
		uint32_t r:5;
		uint32_t g:6;
		uint32_t b:5;
		uint32_t a:8;
	} s;
	uint32_t pix;
} pixel_format;
	#else
typedef union _pixel_format_ {
	struct {
		uint32_t b:5;
		uint32_t g:6;
		uint32_t r:5;
		uint32_t a:8;
	} s;
	uint32_t pix;
} pixel_format;
	#endif

#endif


#endif /* _SMK_DEFS_H_INCLUDED_ */
