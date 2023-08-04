/**
 *
 *  Copyright (C) 2019-2023 Roman Pauer
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

#include "../Game_defs.h"
#include "llasm_cpu.h"
#include <stdlib.h>

static _cpu *global_cpu_ptr = NULL;
static _cpu global_cpu;

extern int Game_Executable;
extern uint32_t X86_InterruptFlag;
#if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_GEOSCAPE))
extern uint8_t geoscape_stack_start;
#endif
#if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_TACTICAL))
extern uint8_t tactical_stack_start;
#endif

EXTERNC _cpu *x86_initialize_cpu(void)
{
    _cpu *cpu;

    cpu = global_cpu_ptr;
    if (cpu != NULL) return cpu;

    cpu = &global_cpu;

    cpu->stack_bottom = NULL;

    switch (Game_Executable)
    {
    #if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_GEOSCAPE))
        case EXE_GEOSCAPE:
            cpu->stack_top = &geoscape_stack_start;
            break;
    #endif
    #if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_TACTICAL))
        case EXE_TACTICAL:
            cpu->stack_top = &tactical_stack_start;
            break;
    #endif
        default:
            break;
    }

    cpu->_st_top = 0;
    cpu->_st_sw_cond = 0;
    cpu->_st_cw = 0x037f;

    esp = (uint32_t)(uintptr_t)cpu->stack_top;
    eflags = 0x3202;
    X86_InterruptFlag = 1;

    global_cpu_ptr = cpu;
    return cpu;
}

EXTERNC void x86_deinitialize_cpu(void)
{
    X86_InterruptFlag = 0;

    global_cpu_ptr = NULL;
}

