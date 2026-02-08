/**
 *
 *  Copyright (C) 2021-2026 Roman Pauer
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

#include <stdint.h>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../platform.h"

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
// C11
static _Thread_local uint32_t current_SEH_frame = 0;
#elif defined(__cplusplus) && __cplusplus >= 201103L
// C++11
static thread_local uint32_t current_SEH_frame = 0;
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC) || defined(__IBMC__) || defined(__IBMCPP__) || defined(__ibmxl__) || defined(__GNUC__) || defined(__llvm__) || (defined(__INTEL_COMPILER) && defined(__linux__))
static __thread uint32_t current_SEH_frame = 0;
#elif defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(_WIN32)) || defined(__BORLANDC__) || defined(__DMC__)
static __declspec(thread) uint32_t current_SEH_frame = 0;
#else
#error thread local variables are not supported
#endif


static void *vectored_exception_handle = NULL;


static LONG CALLBACK exception_handler(PEXCEPTION_POINTERS ExceptionInfo)
{
    if (current_SEH_frame == 0)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    fprintf(stderr, "vectored exception handler: 0x%x\n", (unsigned int)ExceptionInfo->ExceptionRecord->ExceptionCode);
    TerminateProcess(GetCurrentProcess(), 0);
    return 0;
}


void X86_InitializeExceptions(void) __attribute__((noinline));
void X86_InitializeExceptions(void)
{
    vectored_exception_handle = AddVectoredExceptionHandler(1, exception_handler);
}


uint32_t CCALL X86_ReadFsDword(uint32_t addr)
{
    uint32_t *address;

    if (addr == 0)
    {
        if (current_SEH_frame != 0)
        {
            return current_SEH_frame;
        }
    }

    address = (uint32_t *) (addr + (uintptr_t)NtCurrentTeb());

    return *address;
}

void CCALL X86_WriteFsDword(uint32_t addr, uint32_t value)
{
    uint32_t *address;

    address = (uint32_t *) (addr + (uintptr_t)NtCurrentTeb());

    if (addr == 0)
    {
        if (value == *address)
        {
            current_SEH_frame = 0;
        }
        else
        {
            current_SEH_frame = value;
        }
    }
    else
    {
        *address = value;
    }
}

