/**
 *
 *  Copyright (C) 2020-2021 Roman Pauer
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

#if !defined(_BBFX_H_INCLUDED_)
#define _BBFX_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int FX_Init_c(void);
void FX_Exit_c(void);
int FX_ReserveDevices_c(int reserve);
int FX_ReadLib_c(const char *path);
void FX_FreeLib_c(int lib_handle);
void FX_StopAllSamples_c(void);
int FX_PlaySample_c(int lib_handle, int sample_number, int priority, int volume, int times_play);
void FX_SetVolume_c(unsigned int volume);
int FM_IsError_c(void);

#ifdef __cplusplus
}
#endif

#endif /* _BBFX_H_INCLUDED_ */
