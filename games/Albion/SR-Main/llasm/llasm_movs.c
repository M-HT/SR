//part of static recompiler -- do not edit

/**
 *
 *  Copyright (C) 2019-2026 Roman Pauer
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

EXTERNC void CCALL x86_rep_movsb(CPU)
{
    int32_t dir;

    if (ecx == 0) return;

    dir = (eflags & DF)?-1:1;

    do {
        *((uint8_t *)REG2PTR(edi)) = *((uint8_t *)REG2PTR(esi));
        esi += dir;
        edi += dir;

        ecx--;
    } while (ecx != 0);
}

EXTERNC void CCALL x86_rep_movsd(CPU)
{
    uint32_t srcptr, dstptr, counter;

    counter = ecx;
    if (counter == 0) return;

    srcptr = esi;
    dstptr = edi;

    if ((eflags & DF) == 0)
    {
        if (((srcptr | dstptr) & 3) == 0)
        {
            do {
                *((uint32_t *)REG2PTR(dstptr)) = *((uint32_t *)REG2PTR(srcptr));
                srcptr += 4;
                dstptr += 4;

                counter--;
            } while (counter != 0);
        }
        else if ((srcptr & 3) == 0)
        {
            do {
                UNALIGNED_WRITE_32(dstptr, *((uint32_t *)REG2PTR(srcptr)))
                srcptr += 4;
                dstptr += 4;

                counter--;
            } while (counter != 0);
        }
        else if ((dstptr & 3) == 0)
        {
            do {
                *((uint32_t *)REG2PTR(dstptr)) = UNALIGNED_READ_32(srcptr);
                srcptr += 4;
                dstptr += 4;

                counter--;
            } while (counter != 0);
        }
        else
        {
            do {
                UNALIGNED_WRITE_32(dstptr, UNALIGNED_READ_32(srcptr))
                srcptr += 4;
                dstptr += 4;

                counter--;
            } while (counter != 0);
        }
    }
    else
    {
        if (((srcptr | dstptr) & 3) == 0)
        {
            do {
                *((uint32_t *)REG2PTR(dstptr)) = *((uint32_t *)REG2PTR(srcptr));
                srcptr -= 4;
                dstptr -= 4;

                counter--;
            } while (counter != 0);
        }
        else if ((srcptr & 3) == 0)
        {
            do {
                UNALIGNED_WRITE_32(dstptr, *((uint32_t *)REG2PTR(srcptr)))
                srcptr -= 4;
                dstptr -= 4;

                counter--;
            } while (counter != 0);
        }
        else if ((dstptr & 3) == 0)
        {
            do {
                *((uint32_t *)REG2PTR(dstptr)) = UNALIGNED_READ_32(srcptr);
                srcptr -= 4;
                dstptr -= 4;

                counter--;
            } while (counter != 0);
        }
        else
        {
            do {
                UNALIGNED_WRITE_32(dstptr, UNALIGNED_READ_32(srcptr))
                srcptr -= 4;
                dstptr -= 4;

                counter--;
            } while (counter != 0);
        }
    }

    ecx = counter;
    esi = srcptr;
    edi = dstptr;
}

EXTERNC void CCALL x86_rep_movsw(CPU)
{
    int32_t dir;

    if (ecx == 0) return;

    dir = (eflags & DF)?-2:2;

    if (((esi | edi) & 1) == 0)
    {
        do {
            *((uint16_t *)REG2PTR(edi)) = *((uint16_t *)REG2PTR(esi));
            esi += dir;
            edi += dir;

            ecx--;
        } while (ecx != 0);
    }
    else if ((esi & 1) == 0)
    {
        do {
            UNALIGNED_WRITE_16(edi, *((uint16_t *)REG2PTR(esi)))
            esi += dir;
            edi += dir;

            ecx--;
        } while (ecx != 0);
    }
    else if ((edi & 1) == 0)
    {
        do {
            *((uint16_t *)REG2PTR(edi)) = UNALIGNED_READ_16(esi);
            esi += dir;
            edi += dir;

            ecx--;
        } while (ecx != 0);
    }
    else
    {
        do {
            UNALIGNED_WRITE_16(edi, UNALIGNED_READ_16(esi))
            esi += dir;
            edi += dir;

            ecx--;
        } while (ecx != 0);
    }
}

