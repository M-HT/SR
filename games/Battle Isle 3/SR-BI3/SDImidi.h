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

#if !defined(_SDIMIDI_H_INCLUDED_)
#define _SDIMIDI_H_INCLUDED_

#include "platform.h"

int midi_PluginStartup(void);
void midi_PluginShutdown(void);

#ifdef __cplusplus
extern "C" {
#endif

int CCALL midi_OpenSDIMusic(const char *filename);
unsigned int CCALL midi_GetSDIMusicID(void);
int CCALL midi_PlaySDIMusic(void);
int CCALL midi_CloseSDIMusic(void);
int CCALL midi_IsPlaying(unsigned int musicID);

int CCALL midi_OpenTestMusic(void);
int CCALL midi_PlayTestMusic(void);
int CCALL midi_CloseTestMusic(void);

int CCALL midi_GetErrorString(int error, char *text, unsigned int length);

#ifdef __cplusplus
}
#endif

#endif /* _SDIMIDI_H_INCLUDED_ */
