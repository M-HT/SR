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

#if !defined(_BBAVI_H_INCLUDED_)
#define _BBAVI_H_INCLUDED_

#include <stdint.h>
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

int CCALL AVI_Init_c(void);
void CCALL AVI_Exit_c(void);
void CCALL AVI_SetDestortionLevel_c(int destortionLevel);
void CCALL AVI_SystemTask_c(void);
void * CCALL AVI_OpenVideo_c(const char *path, const uint8_t *param2);
void CCALL AVI_CloseVideo_c(void *video);
int CCALL AVI_PlayVideo_c(void *video, int x, int y, int param4, int param5, int volume, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif /* _BBAVI_H_INCLUDED_ */
