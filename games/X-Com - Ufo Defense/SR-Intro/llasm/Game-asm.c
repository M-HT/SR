#include <stdlib.h>
#include <setjmp.h>
#include "llasm_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

extern _cpu *x86_initialize_cpu(void);
extern void x86_deinitialize_cpu(void);

extern void c_main_(CPU);
extern void c_update_timer(CPU);

#ifdef __cplusplus
}
#endif

extern uint32_t X86_InterruptFlag;

static jmp_buf exit_env;


void Game_StopMain_Asm(void)
{
    longjmp(exit_env, 1);
}

int Game_Main_Asm(int argc, char *argv[])
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

        return 1;
    }
}

void Game_RunTimer_Asm(void)
{
    _cpu *cpu;
    uint32_t old_InterruptFlag, old_eflags;

    cpu = x86_initialize_cpu();

    old_InterruptFlag = X86_InterruptFlag;
    old_eflags = eflags;
    eflags = 0x3202;

    c_update_timer(cpu);

    eflags = old_eflags;
    X86_InterruptFlag = old_InterruptFlag;
}

