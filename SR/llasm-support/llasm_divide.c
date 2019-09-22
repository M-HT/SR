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


EXTERNC void x86_div_64_64(CPU, uint32_t *div)
{
    uint64_t dividend, divisor, result;

    dividend = div[0] | (((uint64_t)div[1]) << 32);
    divisor = div[2] | (((uint64_t)div[3]) << 32);

    result = dividend / divisor;

    eax = (uint32_t) result;
    edx = (uint32_t) (result >> 32);
}

EXTERNC void x86_div_64(CPU, uint32_t divisor)
{
    if (edx != 0)
    {
        uint64_t dividend;

        dividend = (((uint64_t)edx) << 32) | eax;

        eax = (uint32_t) (dividend / divisor);
        edx = (uint32_t) (dividend % divisor);
    }
    else
    {
        edx = (uint32_t) (eax % divisor);
        eax = (uint32_t) (eax / divisor);
    }
}


EXTERNC void x86_div_32(CPU, uint32_t divisor)
{
    uint32_t dividend, quotient, remainder;

    dividend = (edx << 16) | (eax & 0xffff);

    quotient = (uint32_t) (dividend / divisor);
    remainder = (uint32_t) (dividend % divisor);

    eax = (eax & 0xffff0000) | (quotient & 0xffff);
    edx = (edx & 0xffff0000) | (remainder & 0xffff);
}


EXTERNC void x86_div_16(CPU, uint32_t divisor)
{
    uint16_t dividend;
    uint32_t quotient, remainder;

    dividend = (uint16_t)(eax & 0xffff);

    quotient = (uint32_t) (dividend / divisor);
    remainder = (uint32_t) (dividend % divisor);

    eax = (eax & 0xffff0000) | (quotient & 0xff) | ((remainder & 0xff) << 8);
}


EXTERNC void x86_idiv_64_64(CPU, uint32_t *div)
{
    int64_t dividend, divisor, result;

    dividend = div[0] | (((uint64_t)div[1]) << 32);
    divisor = div[2] | (((uint64_t)div[3]) << 32);

    result = dividend / divisor;

    eax = (uint32_t) result;
    edx = (uint32_t) (result >> 32);
}

EXTERNC void x86_idiv_64(CPU, int32_t divisor)
{
    if ((int32_t)edx != (((int32_t)eax) >> 31))
    {
        int64_t dividend;

        dividend = (((uint64_t)edx) << 32) | eax;

        eax = (int32_t) (dividend / divisor);
        edx = (int32_t) (dividend % divisor);
    }
    else
    {
        edx = (int32_t) (((int32_t)eax) % divisor);
        eax = (int32_t) (((int32_t)eax) / divisor);
    }
}


EXTERNC void x86_idiv_32(CPU, int32_t divisor)
{
    int32_t dividend, quotient, remainder;

    dividend = (edx << 16) | (eax & 0xffff);

    quotient = (int32_t) (dividend / divisor);
    remainder = (int32_t) (dividend % divisor);

    eax = (eax & 0xffff0000) | (quotient & 0xffff);
    edx = (edx & 0xffff0000) | (remainder & 0xffff);
}


EXTERNC void x86_idiv_16(CPU, int32_t divisor)
{
    int16_t dividend;
    int32_t quotient, remainder;

    dividend = (int16_t)(eax & 0xffff);

    quotient = (int32_t) (dividend / divisor);
    remainder = (int32_t) (dividend % divisor);

    eax = (eax & 0xffff0000) | (quotient & 0xff) | ((remainder & 0xff) << 8);
}

