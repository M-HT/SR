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

.extern draw_3dscene

.extern loc_8B6BB

.global draw_3dscene_proc
.global _draw_3dscene_proc

.global sub_8B6BB
.global _sub_8B6BB

.section .text

draw_3dscene_proc:
_draw_3dscene_proc:

@ [esp      ] = return address

        stmfd esp!, {eflags}

        @call draw_3dscene
        bl draw_3dscene

        ldmfd esp!, {eflags}

        @retn
        ldmfd esp!, {eip}

@ end procedure draw_3dscene_proc


sub_8B6BB:
_sub_8B6BB:

#input:
# r0 = handle
# lr - return address
#

        stmfd sp!, {v1-v8,lr}

        mov eax, r0

        ADR lr, Game_RunTimer_Asm_after_call
        stmfd esp!, {lr}
        LDR eflags, =0x3202
        b loc_8B6BB
    Game_RunTimer_Asm_after_call:

        mov r0, eax

        ldmfd sp!, {v1-v8,pc}

@ end procedure sub_8B6BB
