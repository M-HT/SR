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

.ifdef USE_R9

.macro  mem_read_word_reg dst, tmpreg1, tmpreg2
        ldrb \dst, [tmpadr]
        ldrb \tmpreg1, [tmpadr, #1]
        ldrb \tmpreg2, [tmpadr, #2]
        ldrb tmp9, [tmpadr, #3]
        orr \dst, \dst, \tmpreg1, lsl #8
        orr \dst, \dst, \tmpreg2, lsl #16
        orr \dst, \dst, tmp9, lsl #24

        add lr, lr, #4
        bx lr
.endm

.else

.macro  mem_read_word_reg dst, tmpreg1, tmpreg2
        ldrb \dst, [tmpadr]
        ldrb \tmpreg1, [tmpadr, #1]
        ldrb \tmpreg2, [tmpadr, #2]
        orr \dst, \dst, \tmpreg1, lsl #8
        ldrb \tmpreg1, [tmpadr, #3]
        orr \dst, \dst, \tmpreg2, lsl #16
        orr \dst, \dst, \tmpreg1, lsl #24

        add lr, lr, #4
        bx lr
.endm

.endif

.macro  mem_write_word_reg src, tmpreg
        strb \src, [tmpadr]
        mov \tmpreg, \src, lsr #8
        strb \tmpreg, [tmpadr, #1]
        mov \tmpreg, \src, lsr #16
        strb \tmpreg, [tmpadr, #2]
        mov \tmpreg, \src, lsr #24
        strb \tmpreg, [tmpadr, #3]

        add lr, lr, #4
        bx lr
.endm



.section .note.GNU-stack,"",%progbits
.section .text

.global mem_read_word_tmp1
.global mem_read_word_tmp2
.global mem_read_word_tmp3
@tmpadr
.global mem_read_word_edi
.global mem_read_word_esi
.global mem_read_word_ebp
.global mem_read_word_ebx
.global mem_read_word_edx
.global mem_read_word_tmp9
.global mem_read_word_ecx
.global mem_read_word_eax
.global mem_read_word_eflags
.global mem_read_word_esp
@tmplr
.global mem_read_word_eip

.global mem_read_dword_tmp1_tmp2

.global mem_write_word_tmp1
.global mem_write_word_tmp2
.global mem_write_word_tmp3
@tmpadr
.global mem_write_word_edi
.global mem_write_word_esi
.global mem_write_word_ebp
.global mem_write_word_ebx
.global mem_write_word_edx
.global mem_write_word_tmp9
.global mem_write_word_ecx
.global mem_write_word_eax
.global mem_write_word_eflags
.global mem_write_word_esp
@tmplr
@eip

.global mem_write_dword_tmp1_tmp2

mem_read_word_tmp1:
        mem_read_word_reg tmp1, tmp2, tmp3

mem_read_word_tmp2:
        mem_read_word_reg tmp2, tmp1, tmp3

mem_read_word_tmp3:
        mem_read_word_reg tmp3, tmp1, tmp2

mem_read_word_edi:
        mem_read_word_reg edi, tmp2, tmp3

mem_read_word_esi:
        mem_read_word_reg esi, tmp2, tmp3

mem_read_word_ebp:
        mem_read_word_reg ebp, tmp2, tmp3

mem_read_word_ebx:
        mem_read_word_reg ebx, tmp2, tmp3

mem_read_word_edx:
        mem_read_word_reg edx, tmp2, tmp3

mem_read_word_tmp9:

        ldrb tmp9, [tmpadr]
        ldrb tmp2, [tmpadr, #1]
        ldrb tmp3, [tmpadr, #2]
        orr tmp9, tmp9, tmp2, lsl #8
        ldrb tmp2, [tmpadr, #3]
        orr tmp9, tmp9, tmp3, lsl #16
        orr tmp9, tmp9, tmp2, lsl #24

        add lr, lr, #4
        bx lr

@ end procedure mem_read_word_tmp9

mem_read_word_ecx:
        mem_read_word_reg ecx, tmp2, tmp3

mem_read_word_eax:
        mem_read_word_reg eax, tmp2, tmp3

mem_read_word_eflags:
        mem_read_word_reg eflags, tmp2, tmp3

mem_read_word_esp:
        mem_read_word_reg esp, tmp2, tmp3

mem_read_word_eip:

.ifdef USE_R9
        ldrb tmplr, [tmpadr]
        ldrb tmp2, [tmpadr, #1]
        ldrb tmp3, [tmpadr, #2]
        ldrb tmp9, [tmpadr, #3]
        orr tmplr, tmplr, tmp2, lsl #8
        orr tmplr, tmplr, tmp3, lsl #16
        orr tmplr, tmplr, tmp9, lsl #24
.else
        ldrb tmplr, [tmpadr]
        ldrb tmp2, [tmpadr, #1]
        ldrb tmp3, [tmpadr, #2]
        orr tmplr, tmplr, tmp2, lsl #8
        ldrb tmp2, [tmpadr, #3]
        orr tmplr, tmplr, tmp3, lsl #16
        orr tmplr, tmplr, tmp2, lsl #24
.endif

        bx tmplr

@ end procedure mem_read_word_eip


mem_read_dword_tmp1_tmp2:

.ifdef USE_R9
        ldrb tmp1, [tmpadr]
        ldrb tmp2, [tmpadr, #1]
        ldrb tmp3, [tmpadr, #2]
        ldrb tmp9, [tmpadr, #3]
        orr tmp1, tmp1, tmp2, lsl #8
        orr tmp1, tmp1, tmp3, lsl #16
        orr tmp1, tmp1, tmp9, lsl #24

        ldrb tmp2, [tmpadr, #4]
        ldrb tmp3, [tmpadr, #5]
        ldrb tmp9, [tmpadr, #6]
        orr tmp2, tmp2, tmp3, lsl #8
        ldrb tmp3, [tmpadr, #7]
        orr tmp2, tmp2, tmp9, lsl #16
        orr tmp2, tmp2, tmp3, lsl #24
.else
        ldrb tmp1, [tmpadr]
        ldrb tmp3, [tmpadr, #1]
        ldrb tmp2, [tmpadr, #2]
        orr tmp1, tmp1, tmp3, lsl #8
        ldrb tmp3, [tmpadr, #3]
        orr tmp1, tmp1, tmp2, lsl #16
        orr tmp1, tmp1, tmp3, lsl #24

        ldrb tmp2, [tmpadr, #4]
        ldrb tmp3, [tmpadr, #5]
        orr tmp2, tmp2, tmp3, lsl #8
        ldrb tmp3, [tmpadr, #6]
        orr tmp2, tmp2, tmp3, lsl #16
        ldrb tmp3, [tmpadr, #7]
        orr tmp2, tmp2, tmp3, lsl #24
.endif

        add lr, lr, #8
        bx lr

@ end procedure mem_read_dword_tmp1_tmp2


mem_write_word_tmp1:
        mem_write_word_reg tmp1, tmp3

mem_write_word_tmp2:
        mem_write_word_reg tmp2, tmp3

mem_write_word_tmp3:
        mem_write_word_reg tmp3, tmp2

mem_write_word_edi:
        mem_write_word_reg edi, tmp3

mem_write_word_esi:
        mem_write_word_reg esi, tmp3

mem_write_word_ebp:
        mem_write_word_reg ebp, tmp3

mem_write_word_ebx:
        mem_write_word_reg ebx, tmp3

mem_write_word_edx:
        mem_write_word_reg edx, tmp3

mem_write_word_tmp9:
        mem_write_word_reg tmp9, tmp3

mem_write_word_ecx:
        mem_write_word_reg ecx, tmp3

mem_write_word_eax:
        mem_write_word_reg eax, tmp3

mem_write_word_eflags:
        mem_write_word_reg eflags, tmp3

mem_write_word_esp:
        mem_write_word_reg esp, tmp3


mem_write_dword_tmp1_tmp2:

        strb tmp1, [tmpadr]
        mov tmp3, tmp1, lsr #8
        strb tmp3, [tmpadr, #1]
        mov tmp3, tmp1, lsr #16
        strb tmp3, [tmpadr, #2]
        mov tmp3, tmp1, lsr #24
        strb tmp3, [tmpadr, #3]

        strb tmp2, [tmpadr, #4]
        mov tmp3, tmp2, lsr #8
        strb tmp3, [tmpadr, #5]
        mov tmp3, tmp2, lsr #16
        strb tmp3, [tmpadr, #6]
        mov tmp3, tmp2, lsr #24
        strb tmp3, [tmpadr, #7]

        add lr, lr, #8
        bx lr

@ end procedure mem_write_dword_tmp1_tmp2
