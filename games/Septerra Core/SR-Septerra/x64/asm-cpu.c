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
#include "x64_stack.h"
#include <stdlib.h>
#include <stdint.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
// C11
static _Thread_local _stack *thread_stack = NULL;
#elif defined(__cplusplus) && __cplusplus >= 201103L
// C++11
static thread_local _stack *thread_stack = NULL;
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC) || defined(__IBMC__) || defined(__IBMCPP__) || defined(__ibmxl__) || defined(__GNUC__) || defined(__llvm__) || (defined(__INTEL_COMPILER) && defined(__linux__))
static __thread _stack *thread_stack = NULL;
#elif defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(_WIN32)) || defined(__BORLANDC__) || defined(__DMC__)
static __declspec(thread) _stack *thread_stack = NULL;
#else
#error thread local variables are not supported
#endif

EXTERNC _stack *x86_initialize_stack(void)
{
    _stack *stack;
    void *stack_bottom;

    stack = thread_stack;
    if (stack != NULL) return stack;

    stack_bottom = x86_malloc(1024 * 1024 - 8);
    if (stack_bottom == NULL) exit(2);

    stack = (_stack *)((1024 * 1022 + (uintptr_t)stack_bottom) & ~(uintptr_t)127);
    stack->stack_bottom = stack_bottom;
    stack->stack_top = (void *)(1024 * 1020 + (uintptr_t)stack_bottom);

    stack->esp = stack->stack_top;

    thread_stack = stack;
    return stack;
}

EXTERNC void x86_deinitialize_stack(void)
{
    _stack *stack;
    void *stack_bottom;

    stack = thread_stack;
    if (stack == NULL) return;

    if (stack->stack_bottom != NULL)
    {
        stack_bottom = stack->stack_bottom;
        stack->stack_bottom = NULL;
        x86_free(stack_bottom);
    }

    thread_stack = NULL;
}

