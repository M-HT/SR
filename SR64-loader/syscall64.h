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

#if !defined(_SYSCALL64_H_INCLUDED_)
#define _SYSCALL64_H_INCLUDED_

#include <stdint.h>

uint64_t syscall0(uint64_t callnum);

uint64_t syscall1x(uint64_t param1, uint64_t callnum);
#define syscall1(callnum,param1) syscall1x(param1, callnum)

uint64_t syscall2x(uint64_t param1, uint64_t param2, uint64_t callnum);
#define syscall2(callnum,param1,param2) syscall2x(param1, param2, callnum)

uint64_t syscall3x(uint64_t param1, uint64_t param2, uint64_t param3, uint64_t callnum);
#define syscall3(callnum,param1,param2,param3) syscall3x(param1, param2, param3, callnum)

uint64_t syscall4x(uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t callnum);
#define syscall4(callnum,param1,param2,param3,param4) syscall4x(param1, param2, param3, param4, callnum)

uint64_t syscall5x(uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5, uint64_t callnum);
#define syscall5(callnum,param1,param2,param3,param4,param5) syscall5x(param1, param2, param3, param4, param5, callnum)

uint64_t syscall6x(uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5, uint64_t param6, uint64_t callnum);
#define syscall6(callnum,param1,param2,param3,param4,param5,param6) syscall6x(param1, param2, param3, param4, param5, param6, callnum)

#endif

