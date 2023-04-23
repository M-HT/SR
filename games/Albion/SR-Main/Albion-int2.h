/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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

#if !defined(_ALBION_INT2_H_INCLUDED_)
#define _ALBION_INT2_H_INCLUDED_

#pragma pack(4)

typedef struct _Game_SREGS_ {
  uint16_t es;
  uint16_t cs;
  uint16_t ss;
  uint16_t ds;
  uint16_t fs;
  uint16_t gs;
} Game_SREGS;

typedef struct _Game_DWORDREGS_ {
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t esi;
  uint32_t edi;
  uint32_t cflag;
} Game_DWORDREGS;

typedef struct _Game_WORDREGS_ {
  uint16_t ax, _upper_ax;
  uint16_t bx, _upper_bx;
  uint16_t cx, _upper_cx;
  uint16_t dx, _upper_dx;
  uint16_t si, _upper_si;
  uint16_t di, _upper_di;
  uint32_t cflag;
} Game_WORDREGS;

typedef struct _Game_BYTEREGS_ {
  uint8_t al;
  uint8_t ah;
  uint16_t _upper_ax;
  uint8_t bl;
  uint8_t bh;
  uint16_t _upper_bx;
  uint8_t cl;
  uint8_t ch;
  uint16_t _upper_cx;
  uint8_t dl;
  uint8_t dh;
  uint16_t _upper_dx;
  uint16_t si, _upper_si;
  uint16_t di, _upper_di;
  uint32_t cflag;
} Game_BYTEREGS;

typedef union _Game_REGS_ {
  Game_DWORDREGS d;
  Game_WORDREGS w;
  Game_BYTEREGS h;
} Game_REGS;

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t Game_int386x(
    const uint32_t IntNum,
    const Game_REGS *in_regs,
    Game_REGS *out_regs,
    Game_SREGS *seg_regs
);

#ifdef __cplusplus
}
#endif

#endif /* _ALBION_INT2_H_INCLUDED_ */
