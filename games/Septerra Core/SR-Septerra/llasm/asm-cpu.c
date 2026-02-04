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

#include "../Game-Memory.h"
#include "llasm_cpu.h"
#include <stdlib.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
// C11
static _Thread_local _cpu *thread_cpu = NULL;
#elif defined(__cplusplus) && __cplusplus >= 201103L
// C++11
static thread_local _cpu *thread_cpu = NULL;
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC) || defined(__IBMC__) || defined(__IBMCPP__) || defined(__ibmxl__) || defined(__GNUC__) || defined(__llvm__) || (defined(__INTEL_COMPILER) && defined(__linux__))
static __thread _cpu *thread_cpu = NULL;
#elif defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(_WIN32)) || defined(__BORLANDC__) || defined(__DMC__)
static __declspec(thread) _cpu *thread_cpu = NULL;
#else
#error thread local variables are not supported
#endif

#ifdef __cplusplus
extern "C" {
#endif

uint32_t X86_InterruptFlag;

#ifdef __cplusplus
}
#endif

#ifdef PTROFS_64BIT
extern uint64_t pointer_offset;
#endif

EXTERNC _cpu *x86_initialize_cpu(void)
{
    _cpu *cpu;
    void *stack_bottom;

    cpu = thread_cpu;
    if (cpu != NULL) return cpu;

    stack_bottom = x86_malloc(1024 * 1024 - 8);
    if (stack_bottom == NULL) exit(2);

    cpu = (_cpu *)((1024 * 1022 + (uintptr_t)stack_bottom) & ~(uintptr_t)127);
    cpu->stack_bottom = stack_bottom;
    cpu->stack_top = (void *)(1024 * 1020 + (uintptr_t)stack_bottom);

    cpu->_st_top = 0;
    cpu->_st_sw_cond = 0;
    cpu->_st_cw = 0x037f;

#ifdef PTROFS_64BIT
    cpu->_pointer_offset = pointer_offset;
#else
    cpu->_reserved1 = 0;
#endif

    esp = PTR2REG(cpu->stack_top);
    eflags = 0x3202;
    X86_InterruptFlag = 1;

    thread_cpu = cpu;
    return cpu;
}

EXTERNC void x86_deinitialize_cpu(void)
{
    _cpu *cpu;
    void *stack_bottom;

    cpu = thread_cpu;
    if (cpu == NULL) return;

    if (cpu->stack_bottom != NULL)
    {
        stack_bottom = cpu->stack_bottom;
        cpu->stack_bottom = NULL;
        x86_free(stack_bottom);
    }

    thread_cpu = NULL;
}

