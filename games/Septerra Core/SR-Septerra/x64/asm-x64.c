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

#include "x64_stack.h"
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

extern _stack *x86_initialize_stack(void);
extern void x86_deinitialize_stack(void);

extern const char * CCALL c_GetRecordName_(_stack *stack, uint32_t RecordKey);
extern uint32_t CCALL c_MessageProc_c2asm(_stack *stack, const char *MessageText, uint32_t MessageType, uint32_t MessageCode, uint32_t (*MessageProc)(const char *, uint32_t, uint32_t));
extern int CCALL c_WinMain_(_stack *stack, void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow);
extern uint32_t CCALL c_RunWndProc_c2asm(_stack *stack, void *hwnd, uint32_t uMsg, uint32_t wParam, uint32_t lParam, uint32_t (*WndProc)(void *, uint32_t, uint32_t, uint32_t));
extern void CCALL c_run_thread_c2asm(_stack *stack, void *arglist, void(*start_address)(void *));

#ifdef __cplusplus
}
#endif


EXTERNC const char * CCALL GetRecordName_asm(uint32_t RecordKey)
{
    _stack *stack;

    stack = x86_initialize_stack();

    return c_GetRecordName_(stack, RecordKey);
}

EXTERNC uint32_t CCALL MessageProc_asm(const char *MessageText, uint32_t MessageType, uint32_t MessageCode, uint32_t (*MessageProc)(const char *, uint32_t, uint32_t))
{
    _stack *stack;

    stack = x86_initialize_stack();

    return c_MessageProc_c2asm(stack, MessageText, MessageType, MessageCode, MessageProc);
}

EXTERNC int CCALL WinMain_asm(void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow)
{
    _stack *stack;
    int retval;

    stack = x86_initialize_stack();

    retval = c_WinMain_(stack, hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    x86_deinitialize_stack();

    return retval;
}

EXTERNC uint32_t CCALL RunWndProc_asm(void *hwnd, uint32_t uMsg, uint32_t wParam, uint32_t lParam, uint32_t (*WndProc)(void *, uint32_t, uint32_t, uint32_t))
{
    _stack *stack;

    stack = x86_initialize_stack();

    return c_RunWndProc_c2asm(stack, hwnd, uMsg, wParam, lParam, WndProc);
}

EXTERNC void CCALL run_thread_asm(void *arglist, void(*start_address)(void *))
{
    _stack *stack;

    stack = x86_initialize_stack();

    c_run_thread_c2asm(stack, arglist, start_address);

    x86_deinitialize_stack();
}

