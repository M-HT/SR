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

.extern x86_pushaw
.extern x86_popaw
.extern x86_popfd

.macro PUSH_REG reg_name
        str \reg_name, [esp, #-4]!
.endm

.macro PUSHAD
        mov tmp3, esp
        stmfd esp!, {tmp3,ebx,edx,ecx,eax}
        stmfd esp!, {edi,esi,ebp}
.endm

.macro PUSHAW
        bl x86_pushaw
.endm

.macro PUSHFD
        mrs tmp3, cpsr
        bic eflags, eflags, #0x00c1
        bic eflags, eflags, #0x0800
        and tmp2, tmp3, #0xc0000000
        orr eflags, eflags, tmp2, lsr #24
        and tmp2, tmp3, #0x20000000
        orr eflags, eflags, tmp2, lsr #29
        and tmp2, tmp3, #0x10000000
        orr eflags, eflags, tmp2, lsr #17
        str eflags, [esp, #-4]!
.endm


.macro POP_REG reg_name
        ldr \reg_name, [esp], #4
.endm

.macro POPAD
        ldmfd esp!, {edi,esi,ebp}
        ldmfd esp!, {tmp3,ebx,edx,ecx,eax}
.endm

.macro POPAW
        bl x86_popaw
.endm

.macro POPFD
        bl x86_popfd
.endm

