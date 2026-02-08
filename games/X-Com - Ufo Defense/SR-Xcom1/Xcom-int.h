/**
 *
 *  Copyright (C) 2016-2026 Roman Pauer
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

#if !defined(_XCOM_INT_H_INCLUDED_)
#define _XCOM_INT_H_INCLUDED_

#include "Game_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Game_register _edi;
    Game_register _esi;
    const Game_register _ebp;
    const Game_register _esp;
    Game_register _ebx;
    Game_register _edx;
    Game_register _ecx;
    Game_register _eax;
    Game_register _eflags;
    const Game_register _eip;
} _cpu_regs;

extern void CCALL X86_InterruptProcedure(
    const uint8_t IntNum,
    _cpu_regs *regs
);


#ifdef __cplusplus
}
#endif

#endif /* _XCOM_INT_H_INCLUDED_ */
