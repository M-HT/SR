/**
 *
 *  Copyright (C) 2016-2024 Roman Pauer
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

#if !defined(_EMU_X86_H_INCLUDED_)
#define _EMU_X86_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

int emu_x86_initialize(unsigned int rate, char const *drivers_cat, char const *mt32_roms, int opl3_emulator);
int emu_x86_setvolume(unsigned char volume);
int emu_x86_playsequence(void const *sequence, int size);
int emu_x86_getdata(void *buffer, int size);
int emu_x86_rewindsequence(void);
int emu_x86_stopsequence(void);
void emu_x86_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* _EMU_X86_H_INCLUDED_ */

