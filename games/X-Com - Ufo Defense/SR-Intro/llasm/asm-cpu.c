#include "llasm_cpu.h"
#include <stdlib.h>

static _cpu *global_cpu_ptr = NULL;
static _cpu global_cpu;

extern uint32_t X86_InterruptFlag;
extern uint8_t stack_start;

EXTERNC _cpu *x86_initialize_cpu(void)
{
    _cpu *cpu;

    cpu = global_cpu_ptr;
    if (cpu != NULL) return cpu;

    cpu = &global_cpu;

    cpu->stack_bottom = NULL;
    cpu->stack_top = &stack_start;

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

