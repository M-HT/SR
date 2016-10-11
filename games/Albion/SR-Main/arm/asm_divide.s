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


.extern div_64
.extern div_32

.extern idiv_64
.extern idiv_32

.section .text

.global x86_div_64
.global x86_div_32
.global x86_div_16
.global x86_idiv_64
.global x86_idiv_32
.global x86_idiv_16

# unsigned divide 64bit number by 32bit number
#
# entry:
# edx:eax = dividend
# tmp3 = divisor
# result:
# eax = quotient
# edx = remainder


x86_div_64:

        stmfd esp!, {eflags, lr}

        mov a1, eax

        cmp edx, #0
        bne x86_div_64_div64

        mov a2, tmp3
        bl div_32

        ldmfd esp!, {eflags, lr}

        mov eax, a1
        mov edx, a2

        bx lr

    x86_div_64_div64:

        mov a2, edx
        bl div_64

        ldmfd esp!, {eflags, lr}

        mov eax, a1
        mov edx, a2

        bx lr

@ end procedure x86_div_64


# unsigned divide 32bit number by 32bit number
#
# entry:
# tmp1 = dividend
# tmp2 = divisor
# result:
# tmp1 = quotient
# tmp2 = remainder


x86_div_32:

        stmfd esp!, {eflags, lr}

        bl div_32

        ldmfd esp!, {eflags, lr}
        bx lr

@ end procedure x86_div_32


# unsigned divide 16bit number by 32bit number
#
# entry:
# tmp1 = dividend (upper 16 bits are ignored)
# tmp2 = divisor
# result:
# tmp1 = quotient
# tmp2 = remainder


x86_div_16:

        mov a1, tmp1, lsl #16

        stmfd esp!, {eflags, lr}

        mov a1, a1, lsr #16
        bl div_32

        ldmfd esp!, {eflags, lr}
        bx lr

@ end procedure x86_div_16


# signed divide 64bit number by 32bit number
#
# entry:
# edx:eax = dividend
# tmp3 = divisor
# result:
# eax = quotient
# edx = remainder


x86_idiv_64:

        stmfd esp!, {eflags, lr}

        mov a1, eax

        cmp edx, eax, asr #31
        bne x86_idiv_64_div64

        mov a2, tmp3
        bl idiv_32

        ldmfd esp!, {eflags, lr}

        mov eax, a1
        mov edx, a2

        bx lr

    x86_idiv_64_div64:

        mov a2, edx
        bl idiv_64

        ldmfd esp!, {eflags, lr}

        mov eax, a1
        mov edx, a2

        bx lr

@ end procedure x86_idiv_64



# signed divide 32bit number by 32bit number
#
# entry:
# tmp1 = dividend
# tmp2 = divisor
# result:
# tmp1 = quotient
# tmp2 = remainder


x86_idiv_32:

        stmfd esp!, {eflags, lr}

        bl idiv_32

        ldmfd esp!, {eflags, lr}
        bx lr

@ end procedure x86_idiv_32


# signed divide 16bit number by 32bit number
#
# entry:
# tmp1 = dividend (upper 16 bits are ignored)
# tmp2 = divisor
# result:
# tmp1 = quotient
# tmp2 = remainder


x86_idiv_16:

        mov a1, tmp1, lsl #16

        stmfd esp!, {eflags, lr}

        mov a1, a1, asr #16
        bl idiv_32

        ldmfd esp!, {eflags, lr}
        bx lr

@ end procedure x86_idiv_16

