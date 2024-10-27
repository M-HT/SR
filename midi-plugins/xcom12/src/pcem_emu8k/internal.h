/**
 *
 *  Copyright (C) 2024 Roman Pauer
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

#if !defined(_INTERNAL_H_INCLUDED_)
#define _INTERNAL_H_INCLUDED_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

static
void pclog(const char *format, ...)
{
#ifdef DEBUG
    va_list ap;
    va_start(ap, format);
    vprintf(buf, format, ap);
    va_end(ap);
#endif
}

#define fatal(format) return;

#define romfopen emu8k_romfopen

static
void io_sethandler(uint16_t base, int size, uint8_t (*inb)(uint16_t addr, void *priv), uint16_t (*inw)(uint16_t addr, void *priv),
                   uint32_t (*inl)(uint16_t addr, void *priv), void (*outb)(uint16_t addr, uint8_t val, void *priv),
                   void (*outw)(uint16_t addr, uint16_t val, void *priv), void (*outl)(uint16_t addr, uint32_t val, void *priv),
                   void *priv)
{
}

extern int emu8k_new_pos;
#define sound_pos_global emu8k_new_pos * 48000 + 0

#include "common.h"

#endif /* _INTERNAL_H_INCLUDED_ */

