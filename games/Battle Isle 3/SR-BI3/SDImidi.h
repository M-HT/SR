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

#if !defined(_SDIMIDI_H_INCLUDED_)
#define _SDIMIDI_H_INCLUDED_

int midi_PluginStartup(void);
void midi_PluginShutdown(void);

#ifdef __cplusplus
extern "C" {
#endif

int midi_OpenSDIMusic(const char *filename);
unsigned int midi_GetSDIMusicID(void);
int midi_PlaySDIMusic(void);
int midi_CloseSDIMusic(void);
int midi_IsPlaying(unsigned int musicID);

int midi_OpenTestMusic(void);
int midi_PlayTestMusic(void);
int midi_CloseTestMusic(void);

int midi_GetErrorString(int error, char *text, unsigned int length);

#ifdef __cplusplus
}
#endif

#endif /* _SDIMIDI_H_INCLUDED_ */
