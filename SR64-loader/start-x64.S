//
// Copyright (C) 2022 Roman Pauer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

.global _start
.global _start3
.extern _start2

.type _start, %function
_start:
// rdx = void *func
// rsp = uint64_t *stack

        mov %rdx, %rdi
        mov %rsp, %rsi
        call _start2

// end procedure _start

.type _start2, %function
_start3:
// rdi = void *func
// rsi = void *stack
// rdx = void *entry

        mov %rsi, %rsp
        mov %rdx, %rax
        mov %rdi, %rdx
        jmp *%rax

// end procedure _start3


.section .note.GNU-stack,"",%progbits
