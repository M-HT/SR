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

#include "llasm_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

extern _cpu *x86_initialize_cpu(void);
extern void x86_deinitialize_cpu(void);

extern void CCALL c_GetRecordName_(CPU);
extern void CCALL c_MessageProc_c2asm(CPU);
extern void CCALL c_WinMain_(CPU);
extern void CCALL c_RunWndProc_c2asm(CPU);
extern void CCALL c_run_thread_c2asm(CPU);

#ifdef __cplusplus
}
#endif

EXTERNC const char * CCALL GetRecordName_asm(uint32_t RecordKey)
{
    _cpu *cpu;

    cpu = x86_initialize_cpu();

    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = RecordKey;

    c_GetRecordName_(cpu);

    esp += 4;

    return (const char *)REG2PTR(eax);
}

EXTERNC uint32_t CCALL MessageProc_asm(const char *MessageText, uint32_t MessageType, uint32_t MessageCode, uint32_t (*MessageProc)(const char *, uint32_t, uint32_t))
{
    _cpu *cpu;

    cpu = x86_initialize_cpu();

    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = PTR2REG(MessageProc);
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = MessageCode;
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = MessageType;
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = PTR2REG(MessageText);

    c_MessageProc_c2asm(cpu);

    esp += 4*4;

    return eax;
}

EXTERNC int CCALL WinMain_asm(void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow)
{
    _cpu *cpu;
    int retval;

    cpu = x86_initialize_cpu();

    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = nCmdShow;
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = PTR2REG(lpCmdLine);
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = PTR2REG(hPrevInstance);
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = PTR2REG(hInstance);

    // stdcall (4 parameters)
    c_WinMain_(cpu);

    retval = eax;

    x86_deinitialize_cpu();

    return retval;
}

EXTERNC uint32_t CCALL RunWndProc_asm(void *hwnd, uint32_t uMsg, uint32_t wParam, uint32_t lParam, uint32_t (*WndProc)(void *, uint32_t, uint32_t, uint32_t))
{
    _cpu *cpu;

    cpu = x86_initialize_cpu();

    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = PTR2REG(WndProc);
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = lParam;
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = wParam;
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = uMsg;
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = PTR2REG(hwnd);

    // stdcall (4 parameters)
    c_RunWndProc_c2asm(cpu);

    // restore esp (5th parameter)
    esp += 4;

    return eax;
}

EXTERNC void CCALL run_thread_asm(void *arglist, void(*start_address)(void *))
{
    _cpu *cpu;

    cpu = x86_initialize_cpu();

    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = PTR2REG(start_address);
    esp -= 4;
    *((uint32_t *)REG2PTR(esp)) = PTR2REG(arglist);

    // ccall
    c_run_thread_c2asm(cpu);

    // no need to restore esp, because cpu will be deleted
    //esp += 2*4;

    x86_deinitialize_cpu();
}

