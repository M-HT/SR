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

uint32_t X86_ReadMemProcedure(
    const uint32_t Address,
    const uint32_t MemSize
);

void X86_WriteMemProcedure(
    const uint32_t Address,
    const uint32_t MemSize,
    const uint32_t _eax
);

#ifdef __cplusplus
}
#endif

EXTERNC uint32_t x86_mov_reg_mem_8(uint32_t address)
{
    return X86_ReadMemProcedure(address, 1) & 0xff;
}

EXTERNC uint32_t x86_mov_reg_mem_16(uint32_t address)
{
    return X86_ReadMemProcedure(address, 2) & 0xffff;
}

EXTERNC uint32_t x86_mov_reg_mem_32(uint32_t address)
{
    return X86_ReadMemProcedure(address, 4);
}



EXTERNC void x86_mov_mem_reg_8(uint32_t address, uint32_t value)
{
    X86_WriteMemProcedure(address, 1, value & 0xff);
}

EXTERNC void x86_mov_mem_reg_16(uint32_t address, uint32_t value)
{
    X86_WriteMemProcedure(address, 2, value & 0xffff);
}

EXTERNC void x86_mov_mem_reg_32(uint32_t address, uint32_t value)
{
    X86_WriteMemProcedure(address, 4, value);
}

