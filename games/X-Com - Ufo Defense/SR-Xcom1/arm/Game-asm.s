@@
@@  Copyright (C) 2016-2026 Roman Pauer
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
.include "asm_xti.inc"

.arm


.global Game_ExitMain_Asm
.global _Game_ExitMain_Asm

.global Game_StopMain_Asm
.global _Game_StopMain_Asm

.global Game_Main_Asm
.global _Game_Main_Asm

.global Game_RunTimer_Asm
.global _Game_RunTimer_Asm


.section .note.GNU-stack,"",%progbits
.section .text

.type Game_ExitMain_Asm, %function
.type _Game_ExitMain_Asm, %function
Game_ExitMain_Asm:
_Game_ExitMain_Asm:

        CLI

        LDR lr, =Game_ESP_Original_Value
        ldr sp, [lr]

        mov r0, eax

        ldmfd sp!, {v1-v8,pc}

# end procedure Game_ExitMain_Asm

.type Game_StopMain_Asm, %function
.type _Game_StopMain_Asm, %function
Game_StopMain_Asm:
_Game_StopMain_Asm:

        CLI

        LDR lr, =Game_ESP_Original_Value
        ldr sp, [lr]

        mov r0, #1

        ldmfd sp!, {v1-v8,pc}

# end procedure Game_Stop_Asm

.type Game_Main_Asm, %function
.type _Game_Main_Asm, %function
Game_Main_Asm:
_Game_Main_Asm:

#input:
# r0 - argc
# r1 - argv
# r2 - main_proc
# lr - return address
#

        stmfd sp!, {v1-v8,lr}

        LDR lr, =Game_ESP_Original_Value
        str sp, [lr]

        mov eax, r0
        mov edx, r1

        STI

        ADR lr, Game_ExitMain_Asm
        stmfd esp!, {lr}
        LDR eflags, =0x3202
        bx r2
# end procedure Game_Main_Asm

.type Game_RunTimer_Asm, %function
.type _Game_RunTimer_Asm, %function
Game_RunTimer_Asm:
_Game_RunTimer_Asm:

#input:
# r0 - timer_proc
# lr - return address
#

        stmfd sp!, {v1-v8,lr}

        ADR lr, Game_RunTimer_Asm_after_call
        stmfd esp!, {lr}
        LDR eflags, =0x3202
        bx r0
    Game_RunTimer_Asm_after_call:

        ldmfd sp!, {v1-v8,pc}

# end procedure Game_RunTimer_Asm


.section .bss

Game_ESP_Original_Value:
.skip 4

