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

%ifidn __OUTPUT_FORMAT__, win32
    %define Game_TimerTick _Game_TimerTick
    %define Game_TimerRun _Game_TimerRun

    %define Game_Sync _Game_Sync
    %define Game_RunTimer _Game_RunTimer
%endif

extern Game_TimerTick
extern Game_TimerRun

extern Game_Sync
extern Game_RunTimer

global SR_Call_Asm_Float1
global _SR_Call_Asm_Float1

global SR_Sync
global _SR_Sync

global SR_CheckTimer
global _SR_CheckTimer

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
SR_Call_Asm_Float1:
_SR_Call_Asm_Float1:

; eax          = parameter
; [esp +    4] = procedure address
; [esp       ] = return address


        push ecx
        push edx

; eax          = parameter
; [esp +  3*4] = procedure address
; [esp +  2*4] = return address

    ; remember original esp value
        mov ecx, esp
    ; reserve 8 bytes on stack
        sub esp, byte 8
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 1*4], ecx

    ; put function argument to stack
        mov [esp], eax ; parameter

    ; stack is aligned to 16 bytes

        call dword [ecx + 3*4] ; procedure address

    ; restore original esp value from stack
        mov esp, [esp + 1*4]

        pop edx
        pop ecx

        retn 4

; end procedure SR_InterruptProcedure_Asm

align 16
SR_Sync:
_SR_Sync:

; [esp       ] = return address

        pushfd
        push eax
        push ecx
        push edx

; [esp +  4*4] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 4 bytes on stack
        sub esp, byte 4
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp], eax

    ; stack is aligned to 16 bytes

        call Game_Sync

    ; restore original esp value from stack
        mov esp, [esp]

        pop edx
        pop ecx
        pop eax
        popfd

        retn

; end procedure SR_Sync

align 16
SR_CheckTimer:
_SR_CheckTimer:

; [esp       ] = return address

        pushfd
        push eax

        mov eax, [Game_TimerRun]
        cmp eax, [Game_TimerTick]
        jne short SR_CheckTimer_RunTimer

        pop eax
        popfd

        retn

    SR_CheckTimer_RunTimer:
        push ecx
        push edx

    ; remember original esp value
        mov eax, esp
    ; reserve 4 bytes on stack
        sub esp, byte 4
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp], eax

    ; stack is aligned to 16 bytes

        call Game_RunTimer

    ; restore original esp value from stack
        mov esp, [esp]

        pop edx
        pop ecx
        pop eax
        popfd

        retn

; end procedure SR_CheckTimer

