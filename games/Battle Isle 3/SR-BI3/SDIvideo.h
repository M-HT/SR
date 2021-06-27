/**
 *
 *  Copyright (C) 2021 Roman Pauer
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

#if !defined(_SDIVIDEO_H_INCLUDED_)
#define _SDIVIDEO_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t video_RegisterClass_PRE_Video(void);
int video_Open_PRE_Video(const char *path);
int video_Close_PRE_Video(void);
int video_Play_PRE_Video(uint32_t zoomed);

uint32_t video_RegisterClass_POST_Video(void);
int video_Open_POST_Video(const char *path);
int video_Close_POST_Video(void);
int video_Play_POST_Video(uint32_t zoomed);

uint32_t video_RegisterClass_SS_Video(void);

#ifdef __cplusplus
}
#endif

#endif /* _SDIVIDEO_H_INCLUDED_ */
