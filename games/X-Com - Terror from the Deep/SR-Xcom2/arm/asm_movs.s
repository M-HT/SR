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

.section .note.GNU-stack,"",%progbits
.section .text

.global x86_rep_movsb
.global x86_rep_movsd_1_1
.global x86_rep_movsd_4_4
.global x86_rep_movsw_1_1
.global x86_rep_movsw_2_2

x86_rep_movsb:

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #9      @ tmpadr = 0 or 2
        rsb tmpadr, tmpadr, #1          @ tmpadr = 1 or -1

    1:

        ldrb tmp1, [esi], tmpadr

        subS ecx, ecx, #1

        strb tmp1, [edi], tmpadr

        bne 1b

        bx lr

@ end procedure x86_rep_movsb


x86_rep_movsd_1_1:

        orr tmpadr, esi, edi
        tst tmpadr, #3
        beq x86_rep_movsd_4_4
        tst esi, #3
        beq x86_rep_movsd_4_1
        tst edi, #3
        beq x86_rep_movsd_1_4

        str tmp3, [esp, #-4]!

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #7      @ tmpadr = 0 or 8
        rsb tmpadr, tmpadr, #4          @ tmpadr = 4 or -4

    1:

.ifdef USE_R9
        ldrb tmp2, [esi, #1]
        ldrb tmp3, [esi, #2]
        ldrb tmp9, [esi, #3]
        ldrb tmp1, [esi], tmpadr

        subS ecx, ecx, #1

        strb tmp2, [edi, #1]
        strb tmp3, [edi, #2]
        strb tmp9, [edi, #3]
        strb tmp1, [edi], tmpadr
.else
        ldrb tmp1, [esi, #1]
        ldrb tmp2, [esi, #2]
        ldrb tmp3, [esi, #3]

        subS ecx, ecx, #1

        strb tmp1, [edi, #1]

        ldrb tmp1, [esi], tmpadr

        strb tmp2, [edi, #2]
        strb tmp3, [edi, #3]
        strb tmp1, [edi], tmpadr
.endif

        bne 1b

        ldr tmp3, [esp], #4

        bx lr

@ end procedure x86_rep_movsd_1_1


x86_rep_movsd_1_4:

        str tmp3, [esp, #-4]!

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #7      @ tmpadr = 0 or 8
        rsb tmpadr, tmpadr, #4          @ tmpadr = 4 or -4

    1:

.ifdef USE_R9
        ldrb tmp2, [esi, #1]
        ldrb tmp3, [esi, #2]
        ldrb tmp9, [esi, #3]
        ldrb tmp1, [esi], tmpadr

        subS ecx, ecx, #1

        orr tmp2, tmp2, tmp3, lsl #8
        orr tmp2, tmp2, tmp9, lsl #16
        orr tmp1, tmp1, tmp2, lsl #8

        str tmp1, [edi], tmpadr
.else
        ldrb tmp1, [esi, #1]
        ldrb tmp2, [esi, #2]
        ldrb tmp3, [esi, #3]

        subS ecx, ecx, #1

        orr tmp2, tmp1, tmp2, lsl #8

        ldrb tmp1, [esi], tmpadr

        orr tmp2, tmp2, tmp3, lsl #16
        orr tmp1, tmp1, tmp2, lsl #8

        str tmp1, [edi], tmpadr
.endif

        bne 1b

        ldr tmp3, [esp], #4

        bx lr

@ end procedure x86_rep_movsd_1_4


x86_rep_movsd_4_1:

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #7      @ tmpadr = 0 or 8
        rsb tmpadr, tmpadr, #4          @ tmpadr = 4 or -4

    1:

        ldr tmp1, [esi], tmpadr

        subS ecx, ecx, #1

        mov tmp2, tmp1, lsr #8
        strb tmp2, [edi, #1]
        mov tmp2, tmp1, lsr #16
        strb tmp2, [edi, #2]
        mov tmp2, tmp1, lsr #24
        strb tmp2, [edi, #3]
        strb tmp1, [edi], tmpadr

        bne 1b

        bx lr

@ end procedure x86_rep_movsd_4_1


x86_rep_movsd_4_4:

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #7      @ tmpadr = 0 or 8
        rsb tmpadr, tmpadr, #4          @ tmpadr = 4 or -4

    1:

        ldr tmp1, [esi], tmpadr

        subS ecx, ecx, #1

        str tmp1, [edi], tmpadr

        bne 1b

        bx lr

@ end procedure x86_rep_movsd_4_4


x86_rep_movsw_1_1:

        orr tmpadr, esi, edi
        tst tmpadr, #1
        beq x86_rep_movsw_2_2
        tst esi, #1
        beq x86_rep_movsw_2_1
        tst edi, #1
        beq x86_rep_movsw_1_2

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #8      @ tmpadr = 0 or 4
        rsb tmpadr, tmpadr, #2          @ tmpadr = 2 or -2

    1:

        ldrb tmp2, [esi, #1]
        ldrb tmp1, [esi], tmpadr

        subS ecx, ecx, #1

        strb tmp2, [edi, #1]
        strb tmp1, [edi], tmpadr

        bne 1b

        bx lr

@ end procedure x86_rep_movsw_1_1


x86_rep_movsw_1_2:

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #8      @ tmpadr = 0 or 4
        rsb tmpadr, tmpadr, #2          @ tmpadr = 2 or -2

    1:

        ldrb tmp2, [esi, #1]
        ldrb tmp1, [esi], tmpadr

        subS ecx, ecx, #1

        orr tmp1, tmp1, tmp2, lsl #8

        strh tmp1, [edi], tmpadr

        bne 1b

        bx lr

@ end procedure x86_rep_movsw_1_2


x86_rep_movsw_2_1:

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #8      @ tmpadr = 0 or 4
        rsb tmpadr, tmpadr, #2          @ tmpadr = 2 or -2

    1:

        ldrh tmp1, [esi], tmpadr

        subS ecx, ecx, #1

        mov tmp2, tmp1, lsr #8
        strb tmp2, [edi, #1]
        strb tmp1, [edi], tmpadr

        bne 1b

        bx lr

@ end procedure x86_rep_movsw_2_1


x86_rep_movsw_2_2:

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #8      @ tmpadr = 0 or 4
        rsb tmpadr, tmpadr, #2          @ tmpadr = 2 or -2

    1:

        ldrh tmp1, [esi], tmpadr

        subS ecx, ecx, #1

        strh tmp1, [edi], tmpadr

        bne 1b

        bx lr

@ end procedure x86_rep_movsw_2_2

