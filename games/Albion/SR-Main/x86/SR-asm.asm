;;
;;  Copyright (C) 2016 Roman Pauer
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

%ifndef ELF
    %define Game_TimerTick _Game_TimerTick
    %define Game_TimerRun _Game_TimerRun

    %define Game_Sync _Game_Sync
    %define Game_RunTimer _Game_RunTimer
    %define Game_RunTimerDelay _Game_RunTimerDelay
%endif

extern Game_TimerTick
extern Game_TimerRun

extern Game_Sync
extern Game_RunTimer
extern Game_RunTimerDelay

global SR_Call_Asm_Float1
global _SR_Call_Asm_Float1

global SR_Sync
global _SR_Sync

global SR_CheckTimer
global _SR_CheckTimer

global SR_RunTimerDelay
global _SR_RunTimerDelay

%ifdef ELF
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

        push eax ; parameter

        call dword [esp + 4*4] ; procedure address

        add esp, byte 4

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

        call Game_Sync

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

        call Game_RunTimer

        pop edx
        pop ecx
        pop eax
        popfd

        retn

; end procedure SR_CheckTimer

align 16
SR_RunTimerDelay:
_SR_RunTimerDelay:

; [esp       ] = return address

        pushfd
        push eax
        push ecx
        push edx

; [esp +  4*4] = return address

        call Game_RunTimerDelay

        pop edx
        pop ecx
        pop eax
        popfd

        retn

; end procedure SR_RunTimerDelay

