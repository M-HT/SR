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

.macro pushfd2
    mrs tmpadr, cpsr
    stmfd esp!, {tmpadr, eflags}
.endm

.macro popfd3lr
    ldmfd esp!, {tmpadr, eflags, lr}
    msr cpsr_f, tmpadr
.endm


.extern X86_ReadMemProcedure
.extern X86_WriteMemProcedure

.section .note.GNU-stack,"",%progbits
.section .text

.global x86_mov_reg_mem_8
.global x86_mov_reg_mem_16
.global x86_mov_reg_mem_32

.global x86_mov_mem_reg_8
.global x86_mov_mem_reg_16
.global x86_mov_mem_reg_32

x86_mov_reg_mem_8:

@ tmp1 = address
@ [esp] = return address

        pushfd2

        mov tmp2, #1                @ register length

        bl X86_ReadMemProcedure

        and tmp1, tmp1, #0x00ff

        popfd3lr

        bx lr

@ end procedure x86_mov_reg_mem_8

x86_mov_reg_mem_16:

@ tmp1 = address
@ [esp] = return address

        pushfd2

        mov tmp2, #2                @ register length

        bl X86_ReadMemProcedure

        mov tmp1, tmp1, lsl #16
        mov tmp1, tmp1, lsr #16

        popfd3lr

        bx lr

@ end procedure x86_mov_reg_mem_16

x86_mov_reg_mem_32:

@ tmp1 = address
@ [esp] = return address

        pushfd2

        mov tmp2, #4                @ register length

        bl X86_ReadMemProcedure

        popfd3lr

        bx lr

@ end procedure x86_mov_reg_mem_32


x86_mov_mem_reg_8:

@ tmp2 = value
@ tmp1 = address
@ [esp] = return address

        pushfd2

        and tmp3, tmp2, #0xff
        mov tmp2, #1                @ register length

        bl X86_WriteMemProcedure

        popfd3lr

        bx lr

@ end procedure x86_mov_mem_reg_8

x86_mov_mem_reg_16:

@ tmp2 = value
@ tmp1 = address
@ [esp] = return address

        pushfd2

        mov tmp3, tmp2, lsl #16
        mov tmp3, tmp3, lsr #16
        mov tmp2, #2                @ register length

        bl X86_WriteMemProcedure

        popfd3lr

        bx lr

@ end procedure x86_mov_mem_reg_16

x86_mov_mem_reg_32:

@ tmp2 = value
@ tmp1 = address
@ [esp] = return address

        pushfd2

        mov tmp3, tmp2
        mov tmp2, #4                @ register length

        bl X86_WriteMemProcedure

        popfd3lr

        bx lr

@ end procedure x86_mov_mem_reg_32

