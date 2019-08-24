@part of static recompiler -- do not edit

@@
@@  Copyright (C) 2016-2019 Roman Pauer
@@
@@  Permission is hereby granted, free of charge, to any person obtaining a copy of
@@  this software and associated documentation files (the "Software"), to deal in
@@  the Software without restriction, including without limitation the rights to
@@  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
@@  of the Software, and to permit persons to whom the Software is furnished to do
@@  so, subject to the following conditions:
@@
@@  The above copyright notice and this permission notice shall be included in all
@@  copies or substantial portions of the Software.
@@
@@  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
@@  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
@@  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
@@  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
@@  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
@@  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
@@  SOFTWARE.
@@

.include "arm.inc"
.include "armconf.inc"
.include "asm_pushx.inc"

.extern X86_InterruptProcedure

.section .note.GNU-stack,"",%progbits
.section .text

.global x86_int

x86_int:

@ tmp1 = interrupt number
@ [esp] = return address

        PUSHFD
        PUSHAD

        and a1, tmp1, #0xff

    @ remember original esp value
        mov a2, esp

.ifndef ALLOW_UNALIGNED_STACK
    @ reserve 4 bytes on stack
        sub lr, esp, #4
    @ align stack to 8 bytes
        bic esp, lr, #7
    @ save original esp value on stack
        str a2, [esp]
.endif

        bl X86_InterruptProcedure

.ifndef ALLOW_UNALIGNED_STACK
    @ restore original esp value from stack
        ldr esp, [esp]
.endif

        POPAD
        POPFD

        ldr lr, [esp], #4
        bx lr

@ end procedure x86_int

