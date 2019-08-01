;;
;;  Copyright (C) 2016-2019 Roman Pauer
;;
;;  Permission is hereby granted, free of charge, to any person obtaining a copy of
;;  this software and associated documentation files (the "Software"), to deal in
;;  the Software without restriction, including without limitation the rights to
;;  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
;;  of the Software, and to permit persons to whom the Software is furnished to do
;;  so, subject to the following conditions:
;;
;;  The above copyright notice and this permission notice shall be included in all
;;  copies or substantial portions of the Software.
;;
;;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;;  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;;  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;;  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;;  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;;  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;;  SOFTWARE.
;;

%include "misc.inc"
%include "asm_xti.inc"

%ifidn __OUTPUT_FORMAT__, win32
    %define Game_ESP_Original_Value _Game_ESP_Original_Value
    %define Game_MouseTable _Game_MouseTable
%endif

extern Game_ESP_Original_Value
extern Game_MouseTable

extern main_
extern update_timer
extern keyboard_interrupt

global Game_ExitMain_Asm
global _Game_ExitMain_Asm
global _Z17Game_ExitMain_Asmv
global __Z17Game_ExitMain_Asmv

global Game_StopMain_Asm
global _Game_StopMain_Asm
global _Z17Game_StopMain_Asmv
global __Z17Game_StopMain_Asmv

global Game_Main_Asm
global _Game_Main_Asm
global _Z13Game_Main_AsmiPPc
global __Z13Game_Main_AsmiPPc

global Game_MouseMove
global _Game_MouseMove
global _Z14Game_MouseMovejjj
global __Z14Game_MouseMovejjj

global Game_MouseButton
global _Game_MouseButton
global _Z16Game_MouseButtonjjjj
global __Z16Game_MouseButtonjjjj

global Game_RunTimer_Asm
global _Game_RunTimer_Asm
global _Z17Game_RunTimer_Asmv
global __Z17Game_RunTimer_Asmv

global Game_RunInt9_Asm
global _Game_RunInt9_Asm
global _Z16Game_RunInt9_Asmv
global __Z16Game_RunInt9_Asmv

global Game_RunAILcallback_Asm
global _Game_RunAILcallback_Asm
global _Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_
global __Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_


%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
Game_ExitMain_Asm:
_Game_ExitMain_Asm:
_Z17Game_ExitMain_Asmv:
__Z17Game_ExitMain_Asmv:

        CLI

        mov esp, [Game_ESP_Original_Value]

; [esp + 6*4] = argv
; [esp + 5*4] = argc
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp


        pop ebp
        pop edi
        pop esi
        pop ebx

; [esp + 2*4] = argv
; [esp +   4] = argc
; [esp      ] = return address

        retn

; end procedure Game_ExitMain_Asm

align 16
Game_StopMain_Asm:
_Game_StopMain_Asm:
_Z17Game_StopMain_Asmv:
__Z17Game_StopMain_Asmv:

        CLI

        mov esp, [Game_ESP_Original_Value]

; [esp + 6*4] = argv
; [esp + 5*4] = argc
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp


        pop ebp
        pop edi
        pop esi
        pop ebx

; [esp + 2*4] = argv
; [esp +   4] = argc
; [esp      ] = return address

        mov eax, 1

        retn

; end procedure Game_StopMain_Asm

align 16
Game_Main_Asm:
_Game_Main_Asm:
_Z13Game_Main_AsmiPPc:
__Z13Game_Main_AsmiPPc:

; [esp + 2*4] = argv
; [esp +   4] = argc
; [esp      ] = return address

        push ebx
        push esi
        push edi
        push ebp

; [esp + 6*4] = argv
; [esp + 5*4] = argc
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp

        mov [Game_ESP_Original_Value], esp

        mov eax, [esp + 5*4]
        mov edx, [esp + 6*4]

        STI

        push Game_ExitMain_Asm
        jmp main_
; end procedure Game_Main_Asm

align 16
Game_MouseMove:
_Game_MouseMove:
_Z14Game_MouseMovejjj:
__Z14Game_MouseMovejjj:

; [esp + 3*4] = y position
; [esp + 2*4] = x position
; [esp +   4] = button state
; [esp      ] = return address

        mov edx, [Game_MouseTable]

        or  edx, edx
        jne short Game_MouseMove_continue

        mov eax, 0x01

        retn

    Game_MouseMove_continue:

        push ebx
        push esi
        push edi
        push ebp

; [esp + 7*4] = y position
; [esp + 6*4] = x position
; [esp + 5*4] = button state
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp

%define state esp+5*4
%define xpos esp+6*4
%define ypos esp+7*4

        mov ebp, edx
        mov eax, 1						; condition mask
        mov ebx, [state]				; button state
        xor esi, esi					; horizontal mickey count
        mov ecx, [xpos]					; cursor column
        xor edi, edi					; vertical mickey count
        shl ecx, 1						; double the cursor column
        mov edx, [ypos]					; cursor row

        push byte 0						; emulate far call
        call ebp

        pop ebp
        pop edi
        pop esi
        pop ebx

        xor eax, eax

        retn

%undef ypos
%undef xpos
%undef state

; end procedure Game_MouseMove

align 16
Game_MouseButton:
_Game_MouseButton:
_Z16Game_MouseButtonjjjj:
__Z16Game_MouseButtonjjjj:

; [esp + 4*4] = y position
; [esp + 3*4] = x position
; [esp + 2*4] = action
; [esp +   4] = button state
; [esp      ] = return address

        mov ecx, [esp + 2*4]
        mov eax, 1						; condition mask
        mov edx, [Game_MouseTable + ecx*4]

        or  edx, edx
        jne Game_MouseButton_continue

        retn

    Game_MouseButton_continue:

        push ebx
        push esi
        push edi
        push ebp

; [esp + 8*4] = y position
; [esp + 7*4] = x position
; [esp + 6*4] = action
; [esp + 5*4] = button state
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp

%define state esp+5*4
%define action esp+6*4
%define xpos esp+7*4
%define ypos esp+8*4

        mov ebp, edx
        shl eax, cl						; condition mask (action)
        mov ebx, [state]				; button state
        xor esi, esi					; horizontal mickey count
        mov ecx, [xpos]					; cursor column
        xor edi, edi					; vertical mickey count
        shl ecx, 1						; double the cursor column
        mov edx, [ypos]					; cursor row

        push byte 0						; emulate far call
        call ebp

        pop ebp
        pop edi
        pop esi
        pop ebx

        xor eax, eax

        retn

%undef ypos
%undef xpos
%undef action
%undef state
; end procedure Game_MouseButton

align 16
Game_RunTimer_Asm:
_Game_RunTimer_Asm:
_Z17Game_RunTimer_Asmv:
__Z17Game_RunTimer_Asmv:

        push ebx
        push esi
        push edi
        push ebp

        call update_timer

        pop ebp
        pop edi
        pop esi
        pop ebx

        retn

; end procedure Game_RunTimer_Asm

align 16
Game_RunInt9_Asm:
_Game_RunInt9_Asm:
_Z16Game_RunInt9_Asmv:
__Z16Game_RunInt9_Asmv:

        push ebx
        push esi
        push edi
        push ebp

        call keyboard_interrupt

        pop ebp
        pop edi
        pop esi
        pop ebx

        retn

; end procedure Game_RunInt9_Asm

align 16
Game_RunAILcallback_Asm:
_Game_RunAILcallback_Asm:
_Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_:
__Z23Game_RunAILcallback_AsmPFvP11_AIL_sampleES0_:

; [esp + 2*4] = callback parameter
; [esp +   4] = callback address
; [esp      ] = return address

        push ebx
        push esi
        push edi
        push ebp

; [esp + 6*4] = callback parameter
; [esp + 5*4] = callback address
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp

        push dword [esp + 6*4]  ; callback parameter

        call [esp + 6*4]        ; callback address

        add esp, byte 4

        pop ebp
        pop edi
        pop esi
        pop ebx

        retn

; end procedure Game_RunAILcallback_Asm
