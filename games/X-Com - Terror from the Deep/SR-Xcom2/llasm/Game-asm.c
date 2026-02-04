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

#include <stdlib.h>
#include <setjmp.h>
#include "../Game_defs.h"
#include "llasm_cpu.h"


#ifdef __cplusplus
extern "C" {
#endif

extern _cpu *x86_initialize_cpu(void);
extern void x86_deinitialize_cpu(void);

extern void c_RunProcEBP(CPU);

#ifdef __cplusplus
}
#endif

EXTERNCVAR uint32_t X86_InterruptFlag;

static jmp_buf exit_env;

static int main_return_value;


EXTERNC void Game_ExitMain_Asm(int32_t status)
{
    main_return_value = status;
    longjmp(exit_env, 1);
}

void Game_StopMain_Asm(void)
{
    main_return_value = 1;
    longjmp(exit_env, 1);
}

int Game_Main_Asm(int argc, char *argv[], void *main_proc)
{
    if (setjmp(exit_env) == 0)
    {
        _cpu *cpu;
        int retval;

        cpu = x86_initialize_cpu();

        eax = argc;
        edx = PTR2REG(argv);

        ebp = PTR2REG(main_proc);
        c_RunProcEBP(cpu);

        retval = eax;

        x86_deinitialize_cpu();

        return retval;
    }
    else
    {
        x86_deinitialize_cpu();

        return main_return_value;
    }
}

void Game_RunTimer_Asm(void *timer_proc)
{
    _cpu *cpu;
    uint32_t old_eax, old_ecx, old_edx, old_ebx, old_ebp, old_esi, old_edi;
    uint32_t old_InterruptFlag, old_eflags;

    cpu = x86_initialize_cpu();

    old_eax = eax;
    old_ecx = ecx;
    old_edx = edx;
    old_ebx = ebx;
    old_ebp = ebp;
    old_esi = esi;
    old_edi = edi;

    old_InterruptFlag = X86_InterruptFlag;
    old_eflags = eflags;
    eflags = 0x3202;

    ebp = PTR2REG(timer_proc);
    c_RunProcEBP(cpu);

    eflags = old_eflags;
    X86_InterruptFlag = old_InterruptFlag;

    eax = old_eax;
    ecx = old_ecx;
    edx = old_edx;
    ebx = old_ebx;
    ebp = old_ebp;
    esi = old_esi;
    edi = old_edi;
}

