@part of static recompiler -- do not edit

@@
@@  Copyright (C) 2016 Roman Pauer
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
.include "asm_xti.inc"


.section .text

.global x86_pushaw
.global x86_popaw
.global x86_popfd

x86_pushaw:

        mov tmp3, eax, lsl #16
        mov tmpadr, ecx, lsl #16
        mov tmp1, edx, lsl #16
        mov tmp2, ebx, lsl #16
        orr tmp3, tmp3, tmpadr, lsr #16
        orr tmp1, tmp1, tmp2, lsr #16
        mov tmpadr, esp, lsl #16

        stmfd esp!, {tmp1,tmp3}

        mov tmp2, edi, lsl #16
        mov tmp3, ebp, lsl #16
        mov tmp1, esi, lsl #16
        orr tmp3, tmpadr, tmp3, lsr #16
        orr tmp1, tmp1, tmp2, lsr #16

        stmfd esp!, {tmp1,tmp3}

        bx lr

@ end procedure x86_pushaw


x86_popaw:

        mov tmpadr, #0xff
        ldmfd esp!, {tmp1,tmp3}
        orr tmpadr, tmpadr, #0xff00

        @bic esp, esp, tmpadr
        bic ebp, ebp, tmpadr
        bic esi, esi, tmpadr
        bic edi, edi, tmpadr
        @orr esp, esp, tmp3, lsr #16
        orr esi, esi, tmp1, lsr #16
        and tmp3, tmp3, tmpadr
        and tmp1, tmp1, tmpadr
        orr ebp, ebp, tmp3
        orr edi, edi, tmp1

        ldmfd esp!, {tmp1,tmp3}

        bic eax, eax, tmpadr
        bic ecx, ecx, tmpadr
        bic edx, edx, tmpadr
        bic ebx, ebx, tmpadr
        orr eax, eax, tmp3, lsr #16
        orr edx, edx, tmp1, lsr #16
        and tmp3, tmp3, tmpadr
        and tmp1, tmp1, tmpadr
        orr ecx, ecx, tmp3
        orr ebx, ebx, tmp1

        bx lr

@ end procedure x86_popaw

x86_popfd:

        ldr eflags, [esp], #4
        mrs tmp3, cpsr
        bic tmp3, tmp3, #0xf0000000
        and tmp2, eflags, #0x00c0
        orr tmp3, tmp3, tmp2, lsl #24
        and tmp2, eflags, #0x0001
        orr tmp3, tmp3, tmp2, lsl #29
        and tmp2, eflags, #0x0800
        orr tmp3, tmp3, tmp2, lsl #17
        LDR tmp2, =X86_InterruptFlag
        msr cpsr_f, tmp3
        and tmp3, eflags, #0x0200
        mov tmp3, tmp3, lsr #9
        str tmp3, [tmp2]

        bx lr

@ end procedure x86_popfd
