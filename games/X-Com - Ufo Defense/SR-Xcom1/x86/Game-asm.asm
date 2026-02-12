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


global Game_ExitMain_Asm
global _Game_ExitMain_Asm

global Game_StopMain_Asm
global _Game_StopMain_Asm

global Game_Main_Asm
global _Game_Main_Asm

global Game_RunTimer_Asm
global _Game_RunTimer_Asm

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
Game_ExitMain_Asm:
_Game_ExitMain_Asm:

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

; [esp + 3*4] = main_proc
; [esp + 2*4] = argv
; [esp +   4] = argc
; [esp      ] = return address

        push ebx
        push esi
        push edi
        push ebp

; [esp + 7*4] = main_proc
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
        mov ebx, [esp + 7*4]

        STI

        push Game_ExitMain_Asm
        jmp ebx
; end procedure Game_Main_Asm

align 16
Game_RunTimer_Asm:
_Game_RunTimer_Asm:

; [esp +   4] = timer_proc
; [esp      ] = return address

        push ebx
        push esi
        push edi
        push ebp

; [esp + 5*4] = timer_proc
; [esp + 4*4] = return address
; [esp + 3*4] = saved ebx
; [esp + 2*4] = saved esi
; [esp +   4] = saved edi
; [esp      ] = saved ebp

        mov ebx, [esp + 5*4]

        call ebx

        pop ebp
        pop edi
        pop esi
        pop ebx

        retn

; end procedure Game_RunTimer_Asm


%ifidn __OUTPUT_FORMAT__, elf32
section .bss nobits alloc noexec write align=4
%else
section .bss bss align=4
%endif

Game_ESP_Original_Value:
resd 1

