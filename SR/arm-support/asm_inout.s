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


.extern X86_InPortProcedure
.extern X86_OutPortProcedure

.section .note.GNU-stack,"",%progbits
.section .text

.global x86_in_al_imm
.global x86_in_ax_imm
.global x86_in_eax_imm
.global x86_in_al_dx
.global x86_in_ax_dx
.global x86_in_eax_dx

.global x86_out_imm_al
.global x86_out_imm_ax
.global x86_out_imm_eax
.global x86_out_dx_al
.global x86_out_dx_ax
.global x86_out_dx_eax

x86_in_al_imm:

@ tmp1 = port number
@ [esp] = return address

        pushfd2

        bic tmp1, tmp1, #0xff000000
        mov tmp2, #1				@ register length
        bic tmp1, tmp1, #0x00ff0000

        bl X86_InPortProcedure

        bic eax, eax, #0x00ff
        and tmp1, tmp1, #0x00ff
        orr eax, eax, tmp1

        popfd3lr

        bx lr

@ end procedure x86_in_al_imm

x86_in_ax_imm:

@ tmp1 = port number
@ [esp] = return address

        pushfd2

        bic tmp1, tmp1, #0xff000000
        mov tmp2, #2				@ register length
        bic tmp1, tmp1, #0x00ff0000

        bl X86_InPortProcedure

        bic eax, eax, #0x00ff
        mov tmp1, tmp1, lsl #16
        bic eax, eax, #0xff00
        orr eax, eax, tmp1, lsr #16

        popfd3lr

        bx lr

@ end procedure x86_in_ax_imm

x86_in_eax_imm:

@ tmp1 = port number
@ [esp] = return address

        pushfd2

        bic tmp1, tmp1, #0xff000000
        mov tmp2, #4				@ register length
        bic tmp1, tmp1, #0x00ff0000

        bl X86_InPortProcedure

        mov eax, tmp1

        popfd3lr

        bx lr

@ end procedure x86_in_eax_imm

x86_in_al_dx:

@ [esp] = return address

        pushfd2

        mov tmp1, edx, lsl #16		@ port number
        mov tmp2, #1				@ register length
        mov tmp1, tmp1, lsr #16

        bl X86_InPortProcedure

        bic eax, eax, #0x00ff
        and tmp1, tmp1, #0x00ff
        orr eax, eax, tmp1

        popfd3lr

        bx lr

@ end procedure x86_in_al_dx

x86_in_ax_dx:

@ [esp] = return address

        pushfd2

        mov tmp1, edx, lsl #16		@ port number
        mov tmp2, #2				@ register length
        mov tmp1, tmp1, lsr #16

        bl X86_InPortProcedure

        bic eax, eax, #0x00ff
        mov tmp1, tmp1, lsl #16
        bic eax, eax, #0xff00
        orr eax, eax, tmp1, lsr #16

        popfd3lr

        bx lr

@ end procedure x86_in_ax_dx

x86_in_eax_dx:

@ [esp] = return address

        pushfd2

        mov tmp1, edx, lsl #16		@ port number
        mov tmp2, #4				@ register length
        mov tmp1, tmp1, lsr #16

        bl X86_InPortProcedure

        mov eax, tmp1

        popfd3lr

        bx lr

@ end procedure x86_in_eax_dx

x86_out_imm_al:

@ tmp1 = port number
@ [esp] = return address

        pushfd2

        bic tmp1, tmp1, #0xff000000
        mov tmp2, #1				@ register length
        bic tmp1, tmp1, #0x00ff0000
        mov tmp3, eax

        bl X86_OutPortProcedure

        popfd3lr

        bx lr

@ end procedure x86_out_imm_al

x86_out_imm_ax:

@ tmp1 = port number
@ [esp] = return address

        pushfd2

        bic tmp1, tmp1, #0xff000000
        mov tmp2, #2				@ register length
        bic tmp1, tmp1, #0x00ff0000
        mov tmp3, eax

        bl X86_OutPortProcedure

        popfd3lr

        bx lr

@ end procedure x86_out_imm_ax

x86_out_imm_eax:

@ tmp1 = port number
@ [esp] = return address

        pushfd2

        bic tmp1, tmp1, #0xff000000
        mov tmp2, #4				@ register length
        bic tmp1, tmp1, #0x00ff0000
        mov tmp3, eax

        bl X86_OutPortProcedure

        popfd3lr

        bx lr

@ end procedure x86_out_imm_eax

x86_out_dx_al:

@ [esp] = return address

        pushfd2

        mov tmp1, edx, lsl #16		@ port number
        mov tmp2, #1				@ register length
        mov tmp1, tmp1, lsr #16
        mov tmp3, eax

        bl X86_OutPortProcedure

        popfd3lr

        bx lr

@ end procedure x86_out_dx_al

x86_out_dx_ax:

@ [esp] = return address

        pushfd2

        mov tmp1, edx, lsl #16		@ port number
        mov tmp2, #2				@ register length
        mov tmp1, tmp1, lsr #16
        mov tmp3, eax

        bl X86_OutPortProcedure

        popfd3lr

        bx lr

@ end procedure x86_out_dx_ax

x86_out_dx_eax:

@ [esp] = return address

        pushfd2

        mov tmp1, edx, lsl #16		@ port number
        mov tmp2, #4				@ register length
        mov tmp1, tmp1, lsr #16
        mov tmp3, eax

        bl X86_OutPortProcedure

        popfd3lr

        bx lr

@ end procedure x86_out_dx_eax

