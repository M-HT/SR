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
    %define Game_SDLTicks _Game_SDLTicks
    %define Game_LastAudio _Game_LastAudio

    %define Game_SlowDownMainLoop _Game_SlowDownMainLoop
    %define Game_SlowDownScrolling _Game_SlowDownScrolling
    %define Game_Sync _Game_Sync
    %define Game_RunTimer _Game_RunTimer
    %define Game_RunTimerDelay _Game_RunTimerDelay
%endif

extern Game_SDLTicks
extern Game_LastAudio

extern Game_SlowDownMainLoop
extern Game_SlowDownScrolling
extern Game_Sync
extern Game_RunTimer
extern Game_RunTimerDelay

global SR_SlowDownMainLoop
global _SR_SlowDownMainLoop

global SR_SlowDownScrolling
global _SR_SlowDownScrolling

global SR_Sync
global _SR_Sync

global SR_CheckTimer
global _SR_CheckTimer

global SR_RunTimerDelay
global _SR_RunTimerDelay

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
SR_SlowDownMainLoop:
_SR_SlowDownMainLoop:

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

        call Game_SlowDownMainLoop

    ; restore original esp value from stack
        mov esp, [esp]

        pop edx
        pop ecx
        pop eax
        popfd

        retn

; end procedure SR_SlowDownMainLoop

align 16
SR_SlowDownScrolling:
_SR_SlowDownScrolling:

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

        call Game_SlowDownScrolling

    ; restore original esp value from stack
        mov esp, [esp]

        pop edx
        pop ecx
        pop eax
        popfd

        retn

; end procedure SR_SlowDownScrolling

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

        mov eax, [Game_LastAudio]
        cmp eax, [Game_SDLTicks]
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

align 16
SR_RunTimerDelay:
_SR_RunTimerDelay:

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

        call Game_RunTimerDelay

    ; restore original esp value from stack
        mov esp, [esp]

        pop edx
        pop ecx
        pop eax
        popfd

        retn

; end procedure SR_RunTimerDelay

