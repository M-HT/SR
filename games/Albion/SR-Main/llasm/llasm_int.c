//part of static recompiler -- do not edit

/**
 *
 *  Copyright (C) 2019 Roman Pauer
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

#include "llasm_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

void x86_pushad(CPU);
void x86_popad(CPU);
void x86_popfd(CPU);

void X86_InterruptProcedure(const uint8_t IntNum, void *regs);

#ifdef __cplusplus
}
#endif

EXTERNC void x86_int(CPU, uint32_t intno)
{
        //PUSHFD
        esp -= 4;
        *((uint32_t *)REG2PTR(esp)) = eflags;
        //PUSHAD
        x86_pushad(cpu);

        X86_InterruptProcedure(intno & 0xff, REG2PTR(esp));

        //POPAD
        x86_popad(cpu);
        //POPFD
        x86_popfd(cpu);
}

