;;
;;  Copyright (C) 2016-2026 Roman Pauer
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
    %define Game_MouseTable _Game_MouseTable
%endif

extern Game_MouseTable

extern main_
extern mouse_pos
extern mouse_old_pos
extern update_timer

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
global _Z16Game_MouseButtonjj
global __Z16Game_MouseButtonjj

global Game_RunTimer_Asm
global _Game_RunTimer_Asm
global _Z17Game_RunTimer_Asmv
global __Z17Game_RunTimer_Asmv

global Game_RunProcReg1_Asm
global _Game_RunProcReg1_Asm

global Game_RunProcReg2_Asm
global _Game_RunProcReg2_Asm


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

; end procedure Game_Stop_Asm

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

        mov eax, [mouse_pos + 4]		; albion x
        mov [mouse_old_pos + 2], ax		; my old x
        mov [mouse_old_pos + 6], ax		; my old x ???
        mov eax, [mouse_pos]			; albion y
        mov [mouse_old_pos + 4], ax		; my old y
        mov [mouse_old_pos + 8], ax		; my old y ???

        mov eax, 1						; condition mask
        mov ebx, [state]				; button state
        mov esi, [xpos]					; horizontal mickey count
        mov edi, [ypos]					; vertical mickey count

        push byte 0						; emulate far call
        call edx

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
_Z16Game_MouseButtonjj:
__Z16Game_MouseButtonjj:

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

; [esp + 6*4] = action
; [esp + 5*4] = button state
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp

%define state esp+5*4
%define action esp+6*4

        mov edi, [mouse_pos]			; vertical mickey count
        mov esi, [mouse_pos + 4]		; horizontal mickey count
        shl eax, cl						; condition mask (action)
        mov ebx, [state]				; button state

        push byte 0						; emulate far call
        call edx

        pop ebp
        pop edi
        pop esi
        pop ebx

        xor eax, eax

        retn

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
Game_RunProcReg1_Asm:
_Game_RunProcReg1_Asm:

; [esp + 2*4] = proc parameter 1
; [esp +   4] = proc address
; [esp      ] = return address

        push ebx
        push esi
        push edi
        push ebp

; [esp + 6*4] = proc parameter 1
; [esp + 5*4] = proc address
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp

        mov ecx, [esp + 5*4]  ; proc address
        mov eax, [esp + 6*4]  ; proc parameter 1

        call ecx

        pop ebp
        pop edi
        pop esi
        pop ebx

        retn

; end procedure Game_RunProcReg1_Asm

align 16
Game_RunProcReg2_Asm:
_Game_RunProcReg2_Asm:

; [esp + 3*4] = proc parameter 2
; [esp + 2*4] = proc parameter 1
; [esp +   4] = proc address
; [esp      ] = return address

        push ebx
        push esi
        push edi
        push ebp

; [esp + 7*4] = proc parameter 2
; [esp + 6*4] = proc parameter 1
; [esp + 5*4] = proc address
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp

        mov ecx, [esp + 5*4]  ; proc address
        mov eax, [esp + 6*4]  ; proc parameter 1
        mov edx, [esp + 7*4]  ; proc parameter 2

        call ecx

        pop ebp
        pop edi
        pop esi
        pop ebx

        retn

; end procedure Game_RunProcReg2_Asm


%ifidn __OUTPUT_FORMAT__, elf32
section .bss nobits alloc noexec write align=4
%else
section .bss bss align=4
%endif

Game_ESP_Original_Value:
resd 1

