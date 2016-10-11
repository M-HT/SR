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

#if !defined(_EMU_MT32_H_INCLUDED_)
#define _EMU_MT32_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int emu_mt32_init(unsigned int samplerate, char const *mt32_roms);
void emu_mt32_shutdown(void);

void emu_mt32_write_330(uint8_t value);
void emu_mt32_write_331(uint8_t value);

uint8_t emu_mt32_read_330(void);
uint8_t emu_mt32_read_331(void);

void emu_mt32_getsamples(int16_t *samples, int numsamples);

#ifdef __cplusplus
}
#endif

#endif /* _EMU_MT32_H_INCLUDED_ */

