/**
 *
 *  Copyright (C) 2022 Roman Pauer
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

#include "syscall64.h"

// arm64
// svc #0

//	system call number 	1st parameter 	2nd parameter 	3rd parameter 	4th parameter 	5th parameter 	6th parameter 	result 	result2
//	w8  				x0  			x1  			x2  			x3  			x4  			x5  			x0  	x1

uint64_t syscall0(uint64_t callnum)
{
    register uint64_t _callnum __asm ("x8") = callnum;
    register uint64_t _result __asm ("x0");

    __asm __volatile ("svc #0"
        : "=r" (_result)
        : "r" (_callnum)
        : "cc", "x1"
        );

    return _result;
}

uint64_t syscall1x(uint64_t param1, uint64_t callnum)
{
    register uint64_t _callnum __asm ("x8") = callnum;
    register uint64_t _param1 __asm ("x0") = param1;
    register uint64_t _result __asm ("x0");

    __asm __volatile ("svc #0"
        : "=r" (_result)
        : "r" (_callnum), "r" (_param1)
        : "cc", "x1"
        );

    return _result;
}

uint64_t syscall2x(uint64_t param1, uint64_t param2, uint64_t callnum)
{
    register uint64_t _callnum __asm ("x8") = callnum;
    register uint64_t _param1 __asm ("x0") = param1;
    register uint64_t _param2 __asm ("x1") = param2;
    register uint64_t _result __asm ("x0");

    __asm __volatile ("svc #0"
        : "=r" (_result)
        : "r" (_callnum), "r" (_param1), "r" (_param2)
        : "cc"
        );

    return _result;
}

uint64_t syscall3x(uint64_t param1, uint64_t param2, uint64_t param3, uint64_t callnum)
{
    register uint64_t _callnum __asm ("x8") = callnum;
    register uint64_t _param1 __asm ("x0") = param1;
    register uint64_t _param2 __asm ("x1") = param2;
    register uint64_t _param3 __asm ("x2") = param3;
    register uint64_t _result __asm ("x0");

    __asm __volatile ("svc #0"
        : "=r" (_result)
        : "r" (_callnum), "r" (_param1), "r" (_param2), "r" (_param3)
        : "cc"
        );

    return _result;
}

uint64_t syscall4x(uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t callnum)
{
    register uint64_t _callnum __asm ("x8") = callnum;
    register uint64_t _param1 __asm ("x0") = param1;
    register uint64_t _param2 __asm ("x1") = param2;
    register uint64_t _param3 __asm ("x2") = param3;
    register uint64_t _param4 __asm ("x3") = param4;
    register uint64_t _result __asm ("x0");

    __asm __volatile ("svc #0"
        : "=r" (_result)
        : "r" (_callnum), "r" (_param1), "r" (_param2), "r" (_param3), "r" (_param4)
        : "cc"
        );

    return _result;
}

uint64_t syscall5x(uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5, uint64_t callnum)
{
    register uint64_t _callnum __asm ("x8") = callnum;
    register uint64_t _param1 __asm ("x0") = param1;
    register uint64_t _param2 __asm ("x1") = param2;
    register uint64_t _param3 __asm ("x2") = param3;
    register uint64_t _param4 __asm ("x3") = param4;
    register uint64_t _param5 __asm ("x4") = param5;
    register uint64_t _result __asm ("x0");

    __asm __volatile ("svc #0"
        : "=r" (_result)
        : "r" (_callnum), "r" (_param1), "r" (_param2), "r" (_param3), "r" (_param4), "r" (_param5)
        : "cc"
        );

    return _result;
}

uint64_t syscall6x(uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5, uint64_t param6, uint64_t callnum)
{
    register uint64_t _callnum __asm ("x8") = callnum;
    register uint64_t _param1 __asm ("x0") = param1;
    register uint64_t _param2 __asm ("x1") = param2;
    register uint64_t _param3 __asm ("x2") = param3;
    register uint64_t _param4 __asm ("x3") = param4;
    register uint64_t _param5 __asm ("x4") = param5;
    register uint64_t _param6 __asm ("x5") = param6;
    register uint64_t _result __asm ("x0");

    __asm __volatile ("svc #0"
        : "=r" (_result)
        : "r" (_callnum), "r" (_param1), "r" (_param2), "r" (_param3), "r" (_param4), "r" (_param5), "r" (_param6)
        : "cc"
        );

    return _result;
}

