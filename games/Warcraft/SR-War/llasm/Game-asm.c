/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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

extern void c_main_(CPU);
extern void c_update_timer(CPU);
extern void c_keyboard_interrupt(CPU);

extern void c_RunProcEBP(CPU);

#ifdef __cplusplus
}
#endif

extern uint32_t X86_InterruptFlag;
extern void *Game_MouseTable[8];

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

int Game_Main_Asm(int argc, PTR32(char) argv[])
{
    if (setjmp(exit_env) == 0)
    {
        _cpu *cpu;
        int retval;

        cpu = x86_initialize_cpu();

        eax = argc;
        edx = (uintptr_t)argv;

        c_main_(cpu);

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

uint32_t Game_MouseMove(uint32_t state, uint32_t x, uint32_t y)
{
    _cpu *cpu;
    uint32_t old_eax, old_ecx, old_edx, old_ebx, old_ebp, old_esi, old_edi;
    uint32_t old_InterruptFlag, old_eflags;

    if (Game_MouseTable[0] == NULL)
    {
        return 1;
    }

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

    eax = 1;                            // condition mask
    ebx = state;                        // button state
    ecx = x * 2;                        // cursor column (double)
    edx = y;                            // cursor row
    esi = 0;                            // horizontal mickey count
    edi = 0;                            // vertical mickey count

    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = 0;    // emulate far call

    ebp = (uintptr_t)Game_MouseTable[0];
    c_RunProcEBP(cpu);

    eax = old_eax;
    ecx = old_ecx;
    edx = old_edx;
    ebx = old_ebx;
    ebp = old_ebp;
    esi = old_esi;
    edi = old_edi;

    eflags = old_eflags;
    X86_InterruptFlag = old_InterruptFlag;

    return 0;
}

uint32_t Game_MouseButton(uint32_t state, uint32_t action, uint32_t x, uint32_t y)
{
    _cpu *cpu;
    uint32_t old_eax, old_ecx, old_edx, old_ebx, old_ebp, old_esi, old_edi;
    uint32_t old_InterruptFlag, old_eflags;

    if (Game_MouseTable[action] == NULL)
    {
        return 1;
    }

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

    eax = 1 << action;                  // condition mask (action)
    ebx = state;                        // button state
    ecx = x * 2;                        // cursor column (double)
    edx = y;                            // cursor row
    esi = 0;                            // horizontal mickey count
    edi = 0;                            // vertical mickey count

    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = 0;    // emulate far call

    ebp = (uintptr_t)Game_MouseTable[action];
    c_RunProcEBP(cpu);

    eax = old_eax;
    ecx = old_ecx;
    edx = old_edx;
    ebx = old_ebx;
    ebp = old_ebp;
    esi = old_esi;
    edi = old_edi;

    eflags = old_eflags;
    X86_InterruptFlag = old_InterruptFlag;

    return 0;
}

void Game_RunTimer_Asm(void)
{
    _cpu *cpu;
    uint32_t old_InterruptFlag, old_eflags;

    cpu = x86_initialize_cpu();

    // eax, ebx, ecx, edx, esi, edi, ebp are saved in the called function

    old_InterruptFlag = X86_InterruptFlag;
    old_eflags = eflags;
    eflags = 0x3202;

    c_update_timer(cpu);

    eflags = old_eflags;
    X86_InterruptFlag = old_InterruptFlag;
}


void Game_RunInt9_Asm(void)
{
    _cpu *cpu;
    uint32_t old_InterruptFlag, old_eflags;

    cpu = x86_initialize_cpu();

    // eax, ebx, ecx, edx, esi, edi, ebp are saved in the called function

    old_InterruptFlag = X86_InterruptFlag;
    old_eflags = eflags;
    eflags = 0x3202;

    c_keyboard_interrupt(cpu);

    eflags = old_eflags;
    X86_InterruptFlag = old_InterruptFlag;
}


void Game_RunAILcallback_Asm(AIL_sample_CB callback, AIL_sample *sample)
{
    _cpu *cpu;
    uint32_t old_eax, old_ecx, old_edx, old_ebx, old_ebp, old_esi, old_edi;

    cpu = x86_initialize_cpu();

    old_eax = eax;
    old_ecx = ecx;
    old_edx = edx;
    old_ebx = ebx;
    old_ebp = ebp;
    old_esi = esi;
    old_edi = edi;

    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = (uintptr_t)sample;

    ebp = (uintptr_t)callback;
    c_RunProcEBP(cpu);

    esp += 4;

    eax = old_eax;
    ecx = old_ecx;
    edx = old_edx;
    ebx = old_ebx;
    ebp = old_ebp;
    esi = old_esi;
    edi = old_edi;
}

