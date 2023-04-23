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

EXTERNC void x86_rep_stosb(CPU)
{
    int32_t dir;

    if (ecx == 0) return;

    dir = (eflags & DF)?-1:1;

    do {
        *((uint8_t *)REG2PTR(edi)) = eax;
        edi += dir;

        ecx--;
    } while (ecx != 0);
}

EXTERNC void x86_rep_stosd(CPU)
{
    uint32_t dstptr, counter, srcvalue;

    counter = ecx;
    if (counter == 0) return;

    dstptr = edi;
    srcvalue = eax;

    if ((eflags & DF) == 0)
    {
        if ((dstptr & 3) == 0)
        {
            do {
                *((uint32_t *)REG2PTR(dstptr)) = srcvalue;
                dstptr += 4;

                counter--;
            } while (counter != 0);
        }
        else
        {
            do {
                UNALIGNED_WRITE_32(dstptr, srcvalue)
                dstptr += 4;

                counter--;
            } while (counter != 0);
        }
    }
    else
    {
        if ((dstptr & 3) == 0)
        {
            do {
                *((uint32_t *)REG2PTR(dstptr)) = srcvalue;
                dstptr -= 4;

                counter--;
            } while (counter != 0);
        }
        else
        {
            do {
                UNALIGNED_WRITE_32(dstptr, srcvalue)
                dstptr -= 4;

                counter--;
            } while (counter != 0);
        }
    }

    ecx = counter;
    edi = dstptr;
}

EXTERNC void x86_rep_stosw(CPU)
{
    int32_t dir;

    if (ecx == 0) return;

    dir = (eflags & DF)?-2:2;

    if ((edi & 1) == 0)
    {
        do {
            *((uint16_t *)REG2PTR(edi)) = eax;
            edi += dir;

            ecx--;
        } while (ecx != 0);
    }
    else
    {
        do {
            UNALIGNED_WRITE_16(edi, eax)
            edi += dir;

            ecx--;
        } while (ecx != 0);
    }
}

