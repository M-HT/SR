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
#if !(defined(__GNUC__) && defined(_WIN64))
#include <setjmp.h>
#if defined(_MSC_VER) && defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#endif
#include "../Game_defs.h"
#include "x64_stack.h"


#ifdef __cplusplus
extern "C" {
#endif

extern _stack *x86_initialize_stack(void);
extern void x86_deinitialize_stack(void);

extern int CCALL c_main_(_stack *stack, int argc, char *argv[], void *main_proc);
extern void CCALL c_update_timer(_stack *stack, void *timer_proc);

#ifdef __cplusplus
}
#endif

extern "C" uint32_t X86_InterruptFlag;

#if defined(__GNUC__) && defined(_WIN64)
static intptr_t exit_env[5];
#else
static jmp_buf exit_env;
#if defined(_MSC_VER) && defined(_WIN64)
static int (*dyn_intrinsic_setjmp)(jmp_buf, void *);
static void (*dyn_longjmp)(jmp_buf, int);
#endif
#endif

static int main_return_value;


EXTERNC void CCALL Game_ExitMain_Asm(int32_t status)
{
    main_return_value = status;
#if defined(__GNUC__) && defined(_WIN64)
    __builtin_longjmp(exit_env, 1);
#elif defined(_MSC_VER) && defined(_WIN64)
    (dyn_longjmp != NULL) ? dyn_longjmp(exit_env, 1) : longjmp(exit_env, 1);
#else
    longjmp(exit_env, 1);
#endif
}

void CCALL Game_StopMain_Asm(void)
{
    main_return_value = 1;
#if defined(__GNUC__) && defined(_WIN64)
    __builtin_longjmp(exit_env, 1);
#elif defined(_MSC_VER) && defined(_WIN64)
    (dyn_longjmp != NULL) ? dyn_longjmp(exit_env, 1) : longjmp(exit_env, 1);
#else
    longjmp(exit_env, 1);
#endif
}

int CCALL Game_Main_Asm(int argc, char *argv[], void *main_proc)
{
#if defined(__GNUC__) && defined(_WIN64)
    if (__builtin_setjmp(exit_env) == 0)
#elif defined(_MSC_VER) && defined(_WIN64)
    HMODULE hLib;

    hLib = GetModuleHandleW(L"ucrtbase.dll");
    if (hLib != NULL)
    {
        dyn_intrinsic_setjmp = (int (*)(jmp_buf, void *))GetProcAddress(hLib, "__intrinsic_setjmp");
        dyn_longjmp = (void (*)(jmp_buf, int))GetProcAddress(hLib, "longjmp");
    }
    if (hLib == NULL || dyn_intrinsic_setjmp == NULL || dyn_longjmp == NULL)
    {
        dyn_intrinsic_setjmp = NULL;
        dyn_longjmp = NULL;
    }

    if (((dyn_intrinsic_setjmp != NULL) ? dyn_intrinsic_setjmp(exit_env, NULL) : setjmp(exit_env)) == 0)
#else
    if (setjmp(exit_env) == 0)
#endif
    {
        _stack *stack;
        int retval;

        stack = x86_initialize_stack();

        retval = c_main_(stack, argc, argv, main_proc);

        x86_deinitialize_stack();

        return retval;
    }
    else
    {
        x86_deinitialize_stack();

        return main_return_value;
    }
}

void CCALL Game_RunTimer_Asm(void *timer_proc)
{
    _stack *stack;
    uint32_t old_InterruptFlag;

    stack = x86_initialize_stack();

    old_InterruptFlag = X86_InterruptFlag;

    c_update_timer(stack, timer_proc);

    X86_InterruptFlag = old_InterruptFlag;
}

