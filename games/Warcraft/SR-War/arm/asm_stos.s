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

.section .text

.global x86_rep_stosb
.global x86_rep_stosd_1
.global x86_rep_stosd_4
.global x86_rep_stosw_1
.global x86_rep_stosw_2

x86_rep_stosb:

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #9      @ tmpadr = 0 or 2
        rsb tmpadr, tmpadr, #1          @ tmpadr = 1 or -1

    1:

        strb eax, [edi], tmpadr

        subS ecx, ecx, #1

        bne 1b

        bx lr

@ end procedure x86_rep_stosb


x86_rep_stosd_1:

        tst edi, #3
        beq x86_rep_stosd_4
        tst edi, #1
        beq x86_rep_stosd_2

        mov tmp1, eax, lsr #8
        mov tmp2, eax, lsr #24

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #7      @ tmpadr = 0 or 8
        rsb tmpadr, tmpadr, #4          @ tmpadr = 4 or -4

    1:

        strh tmp1, [edi, #1]
        strb tmp2, [edi, #3]
        strb eax, [edi], tmpadr

        subS ecx, ecx, #1

        bne 1b

        bx lr

@ end procedure x86_rep_stosd_1


x86_rep_stosd_2:

        mov tmp1, eax, lsr #16

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #7      @ tmpadr = 0 or 8
        rsb tmpadr, tmpadr, #4          @ tmpadr = 4 or -4

    1:

        strh tmp1, [edi, #2]
        strh eax, [edi], tmpadr

        subS ecx, ecx, #1

        bne 1b

        bx lr

@ end procedure x86_rep_stosd_2


x86_rep_stosd_4:

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #7      @ tmpadr = 0 or 8
        rsb tmpadr, tmpadr, #4          @ tmpadr = 4 or -4

    1:

        str eax, [edi], tmpadr

        subS ecx, ecx, #1

        bne 1b

        bx lr

@ end procedure x86_rep_stosd_4


x86_rep_stosw_1:

        tst edi, #1
        beq x86_rep_stosw_2

        mov tmp1, eax, lsr #8

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #8      @ tmpadr = 0 or 4
        rsb tmpadr, tmpadr, #2          @ tmpadr = 2 or -2

    1:

        strb tmp1, [edi, #1]
        strb eax, [edi], tmpadr

        subS ecx, ecx, #1

        bne 1b

        bx lr

@ end procedure x86_rep_stosw_1


x86_rep_stosw_2:

        and tmpadr, eflags, #0x0400     @ tmpadr = 0 or 0x400     (direction flag)
        mov tmpadr, tmpadr, lsr #8      @ tmpadr = 0 or 4
        rsb tmpadr, tmpadr, #2          @ tmpadr = 2 or -2

    1:

        strh eax, [edi], tmpadr

        subS ecx, ecx, #1

        bne 1b

        bx lr

@ end procedure x86_rep_stosw_2

