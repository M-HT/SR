/**
 *
 *  Copyright (C) 2019-2021 Roman Pauer
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

extern _cpu *x86_initialize_cpu(void);
extern void x86_deinitialize_cpu(void);

extern void c_WinMain_(CPU);

#ifdef __cplusplus
}
#endif

EXTERNC int WinMain_asm(void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow)
{
    _cpu *cpu;
    int retval;

    cpu = x86_initialize_cpu();

    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = nCmdShow;
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = (uintptr_t)lpCmdLine;
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = (uintptr_t)hPrevInstance;
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = (uintptr_t)hInstance;

    // stdcall (4 parameters)
    c_WinMain_(cpu);

    retval = eax;

    x86_deinitialize_cpu();

    return retval;
}

