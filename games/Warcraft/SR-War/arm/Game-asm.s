@@
@@  Copyright (C) 2016-2021 Roman Pauer
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

.extern Game_ESP_Original_Value
.extern Game_MouseTable

.extern main_
.extern update_timer
.extern keyboard_interrupt

.global Game_ExitMain_Asm
.global _Game_ExitMain_Asm
.global _Z17Game_ExitMain_Asmv
.global __Z17Game_ExitMain_Asmv

.global Game_StopMain_Asm
.global _Game_StopMain_Asm
.global _Z17Game_StopMain_Asmv
.global __Z17Game_StopMain_Asmv

.global Game_Main_Asm
.global _Game_Main_Asm
.global _Z13Game_Main_AsmiPPc
.global __Z13Game_Main_AsmiPPc

.global Game_MouseMove
.global _Game_MouseMove
.global _Z14Game_MouseMovejjj
.global __Z14Game_MouseMovejjj

.global Game_MouseButton
.global _Game_MouseButton
.global _Z16Game_MouseButtonjjjj
.global __Z16Game_MouseButtonjjjj

.global Game_RunTimer_Asm
.global _Game_RunTimer_Asm
.global _Z17Game_RunTimer_Asmv
.global __Z17Game_RunTimer_Asmv

.global Game_RunInt9_Asm
.global _Game_RunInt9_Asm
.global _Z16Game_RunInt9_Asmv
.global __Z16Game_RunInt9_Asmv

.global Game_RunAILcallback_Asm
.global _Game_RunAILcallback_Asm
.global _Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_
.global __Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_


.section .note.GNU-stack,"",%progbits
.section .text

.type Game_ExitMain_Asm, %function
.type _Game_ExitMain_Asm, %function
.type _Z17Game_ExitMain_Asmv, %function
.type __Z17Game_ExitMain_Asmv, %function
Game_ExitMain_Asm:
_Game_ExitMain_Asm:
_Z17Game_ExitMain_Asmv:
__Z17Game_ExitMain_Asmv:

        CLI

        LDR lr, =Game_ESP_Original_Value
        ldr sp, [lr]

        mov r0, eax

        ldmfd sp!, {v1-v8,pc}

# end procedure Game_ExitMain_Asm

.type Game_StopMain_Asm, %function
.type _Game_StopMain_Asm, %function
.type _Z17Game_StopMain_Asmv, %function
.type __Z17Game_StopMain_Asmv, %function
Game_StopMain_Asm:
_Game_StopMain_Asm:
_Z17Game_StopMain_Asmv:
__Z17Game_StopMain_Asmv:

        CLI

        LDR lr, =Game_ESP_Original_Value
        ldr sp, [lr]

        mov r0, #1

        ldmfd sp!, {v1-v8,pc}

# end procedure Game_StopMain_Asm

.type Game_Main_Asm, %function
.type _Game_Main_Asm, %function
.type _Z13Game_Main_AsmiPPc, %function
.type __Z13Game_Main_AsmiPPc, %function
Game_Main_Asm:
_Game_Main_Asm:
_Z13Game_Main_AsmiPPc:
__Z13Game_Main_AsmiPPc:

#input:
# r0 - argc
# r1 - argv
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
        b main_
# end procedure Game_Main_Asm


.type Game_MouseMove, %function
.type _Game_MouseMove, %function
.type _Z14Game_MouseMovejjj, %function
.type __Z14Game_MouseMovejjj, %function
Game_MouseMove:
_Game_MouseMove:
_Z14Game_MouseMovejjj:
__Z14Game_MouseMovejjj:

#input:
# r0 = button state
# r1 = x position
# r2 = y position
# lr - return address
#

        LDR r3, =Game_MouseTable
        ldr r3, [r3]

        cmp r3, #0

        moveq r0, #1
        bxeq lr

        stmfd sp!, {v1-v8,lr}

        mov eax, #1				@ condition mask
        mov ebx, r0				@ button state
        mov ecx, r1, lsl #1		@ cursor column (double)
        mov edx, r2				@ cursor row
        mov esi, #0				@ horizontal mickey count
        mov edi, #0				@ vertical mickey count


        mov lr, #0
        stmfd esp!, {lr}		@ emulate far call
        ADR lr, Game_MouseMove_after_call
        stmfd esp!, {lr}
        LDR eflags, =0x3202
        bx r3
    Game_MouseMove_after_call:

        mov r0, #0
        ldmfd sp!, {v1-v8,pc}

# end procedure Game_MouseMove

.type Game_MouseButton, %function
.type _Game_MouseButton, %function
.type _Z16Game_MouseButtonjj, %function
.type __Z16Game_MouseButtonjj, %function
Game_MouseButton:
_Game_MouseButton:
_Z16Game_MouseButtonjjjj:
__Z16Game_MouseButtonjjjj:

#input:
# r0 = button state
# r1 = action
# r2 = x position
# r3 = y position
# lr - return address
#

        LDR ip, =Game_MouseTable
        ldr ip, [ip, r1, lsl #2]

        cmp ip, #0
        moveq r0, #1
        bxeq lr

        stmfd sp!, {v1-v8,lr}

        mov eax, #1				@ condition mask
        mov eax, eax, lsl r1	@ condition mask (action)
        mov ebx, r0				@ button state
        mov ecx, r2, lsl #1		@ cursor column (double)
        mov edx, r3				@ cursor row
        mov esi, #0				@ horizontal mickey count
        mov edi, #0				@ vertical mickey count

        mov r3, ip

        mov lr, #0
        stmfd esp!, {lr}		@ emulate far call
        ADR lr, Game_MouseButton_after_call
        stmfd esp!, {lr}
        LDR eflags, =0x3202
        bx r3
    Game_MouseButton_after_call:

        mov r0, #0
        ldmfd sp!, {v1-v8,pc}

# end procedure Game_MouseButton

.type Game_RunTimer_Asm, %function
.type _Game_RunTimer_Asm, %function
.type _Z17Game_RunTimer_Asmv, %function
.type __Z17Game_RunTimer_Asmv, %function
Game_RunTimer_Asm:
_Game_RunTimer_Asm:
_Z17Game_RunTimer_Asmv:
__Z17Game_RunTimer_Asmv:

        stmfd sp!, {v1-v8,lr}

        ADR lr, Game_RunTimer_Asm_after_call
        stmfd esp!, {lr}
        LDR eflags, =0x3202
        LDR eip, =update_timer
    Game_RunTimer_Asm_after_call:

        ldmfd sp!, {v1-v8,pc}

# end procedure Game_RunTimer_Asm

.type Game_RunInt9_Asm, %function
.type _Game_RunInt9_Asm, %function
.type _Z16Game_RunInt9_Asmv, %function
.type __Z16Game_RunInt9_Asmv, %function
Game_RunInt9_Asm:
_Game_RunInt9_Asm:
_Z16Game_RunInt9_Asmv:
__Z16Game_RunInt9_Asmv:

        stmfd sp!, {v1-v8,lr}

        ADR lr, Game_RunInt9_Asm_after_call
        stmfd esp!, {lr}
        LDR eflags, =0x3202
        b keyboard_interrupt
    Game_RunInt9_Asm_after_call:

        ldmfd sp!, {v1-v8,pc}

# end procedure Game_RunInt9_Asm

.type Game_RunAILcallback_Asm, %function
.type _Game_RunAILcallback_Asm, %function
.type _Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_, %function
.type __Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_, %function
Game_RunAILcallback_Asm:
_Game_RunAILcallback_Asm:
_Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_:
__Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_:

#input:
# r0 = callback address
# r1 = callback parameter
# lr - return address
#

        stmfd sp!, {v1-v8,lr}

        stmfd esp!, {r1}       @ callback parameter

        ADR lr, Game_RunAILcallback_Asm_after_call
        stmfd esp!, {lr}
        LDR eflags, =0x3202
        bx r0                  @ callback address
    Game_RunAILcallback_Asm_after_call:

        add esp, esp, #4

        ldmfd sp!, {v1-v8,pc}

# end procedure Game_RunAILcallback_Asm
