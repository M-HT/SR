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

extern uint32_t X86_InterruptFlag;


EXTERNC void x86_pushad(CPU)
{
    uint32_t tmp1, *ptr;

    tmp1 = esp;
    esp -= 8*4;
    ptr = (uint32_t *)REG2PTR(esp);

    ptr[0] = edi;
    ptr[1] = esi;
    ptr[2] = ebp;
    ptr[3] = tmp1;
    ptr[4] = ebx;
    ptr[5] = edx;
    ptr[6] = ecx;
    ptr[7] = eax;
}

EXTERNC void x86_pushaw(CPU)
{
    uint32_t tmp1, tmp2, tmp3, tmp4, *ptr;

    tmp1 = (edi & 0xffff) | (esi << 16);
    tmp2 = (ebp & 0xffff) | (esp << 16);
    tmp3 = (ebx & 0xffff) | (edx << 16);
    tmp4 = (ecx & 0xffff) | (eax << 16);

    esp -= 8*2;
    ptr = (uint32_t *)REG2PTR(esp);

    ptr[0] = tmp1;
    ptr[1] = tmp2;
    ptr[2] = tmp3;
    ptr[3] = tmp4;
}

EXTERNC void x86_popad(CPU)
{
    uint32_t *ptr;

    ptr = (uint32_t *)REG2PTR(esp);

    edi = ptr[0];
    esi = ptr[1];
    ebp = ptr[2];
    ebx = ptr[4];
    edx = ptr[5];
    ecx = ptr[6];
    eax = ptr[7];

    esp += 8*4;
}

EXTERNC void x86_popaw(CPU)
{
    uint32_t tmp1, tmp2, tmp3, tmp4, *ptr;

    ptr = (uint32_t *)REG2PTR(esp);

    tmp1 = ptr[0];
    tmp2 = ptr[1];
    tmp3 = ptr[2];
    tmp4 = ptr[3];

    esp += 8*2;

    edi = (edi & 0xffff0000) | (tmp1 & 0xffff);
    ebp = (ebp & 0xffff0000) | (tmp2 & 0xffff);
    ebx = (ebx & 0xffff0000) | (tmp3 & 0xffff);
    ecx = (ecx & 0xffff0000) | (tmp4 & 0xffff);

    esi = (edi & 0xffff0000) | (tmp1 >> 16);
    edx = (ebx & 0xffff0000) | (tmp3 >> 16);
    eax = (ecx & 0xffff0000) | (tmp4 >> 16);
}

EXTERNC void x86_popfd(CPU)
{
    eflags = *((uint32_t *)REG2PTR(esp));
    esp += 4;

    X86_InterruptFlag = (eflags & IF)?1:0;
}

