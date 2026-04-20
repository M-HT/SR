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

#include "../Game_defs.h"
#include "../Game_memory.h"
#include "x64_stack.h"
#include <stdlib.h>

static _stack *global_stack = NULL;
static void *global_stack_block = NULL;

extern "C" uint32_t X86_InterruptFlag;
extern "C" uint8_t stack_start;

EXTERNC _stack *x86_initialize_stack(void)
{
    _stack *stack;
    void *stack_block;

    stack = global_stack;
    if (stack != NULL) return stack;

    stack_block = x86_malloc(4088);
    if (stack_block == NULL) exit(2);

    stack = (_stack *)((2048 + (uintptr_t)stack_block) & ~(uintptr_t)127);
    global_stack_block = stack_block;

    stack->stack_bottom = NULL;
    stack->stack_top = &stack_start;

    stack->esp = stack->stack_top;
    X86_InterruptFlag = 1;

    global_stack = stack;
    return stack;
}

EXTERNC void x86_deinitialize_stack(void)
{
    _stack *stack;
    void *stack_block;

    stack = global_stack;
    if (stack == NULL) return;

    if (global_stack_block != NULL)
    {
        stack_block = global_stack_block;
        global_stack_block = NULL;
        x86_free(stack_block);
    }

    X86_InterruptFlag = 0;

    global_stack = NULL;
}

