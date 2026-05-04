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

%include "x64inc.inc"

extern Game_SDLTicks
extern Game_LastAudio

extern Game_SlowDownMainLoop
extern Game_SlowDownScrolling
extern Game_Sync
extern Game_RunTimerDelay
extern Game_RunTimer

global SR_SlowDownMainLoop

global SR_SlowDownScrolling

global SR_Sync

global SR_RunTimerDelay

global SR_CheckTimer

%include "SR-asm-calls.inc"

%ifidn __OUTPUT_FORMAT__, elf64
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif
%ifidn __OUTPUT_FORMAT__, win64
section_prolog:
        SECTION_PROLOG
%endif

align 16
SR_SlowDownMainLoop:

; [r11d] = return address

        Game_Call_Prologue_3 eax,ecx,edx

        call Game_SlowDownMainLoop

        Game_Call_Epilogue_3 eax,ecx,edx,-1

; end procedure SR_SlowDownMainLoop

align 16
SR_SlowDownScrolling:

; [r11d] = return address

        Game_Call_Prologue_3 eax,ecx,edx

        call Game_SlowDownScrolling

        Game_Call_Epilogue_3 eax,ecx,edx,-1

; end procedure SR_SlowDownScrolling

align 16
SR_Sync:

; [r11d] = return address

        Game_Call_Prologue_3 eax,ecx,edx

        call Game_Sync

        Game_Call_Epilogue_3 eax,ecx,edx,-1

; end procedure SR_Sync

align 16
SR_RunTimerDelay:

; [r11d] = return address

        Game_Call_Prologue_3 eax,ecx,edx

        call Game_RunTimerDelay

        Game_Call_Epilogue_3 eax,ecx,edx,-1

; end procedure SR_RunTimerDelay

%ifidn __OUTPUT_FORMAT__, win64
section_end:
%endif

align 16
SR_CheckTimer:

; [rsp] = return address

%ifidn __OUTPUT_FORMAT__, win64
        push r13 ; save non-volatile register on stack
    .prolog_1:
        mov r13, rsp ; set frame pointer (stack is aligned to 16 bytes)
    .end_prolog: ; end of prolog for unwinding
%endif

        pushf

        mov r8d, [Game_LastAudio]
        cmp r8d, [Game_SDLTicks]
        jne short SR_CheckTimer_RunTimer

        popf

%ifidn __OUTPUT_FORMAT__, win64
    ; start of epilog for unwinding
        pop r13 ; restore non-volatile register from stack
%endif

        ret

    SR_CheckTimer_RunTimer:

%ifidn __OUTPUT_FORMAT__, win64
        mov r10, [rsp+FIRST_PARAMETER_OFFSET+3*8] ; load first function parameter from reserved slot
        mov [rsp+FIRST_PARAMETER_OFFSET+4*8], eax ; store registers in reserved slots
        mov [rsp+FIRST_PARAMETER_OFFSET+4*8+4], ecx
        mov [rsp+FIRST_PARAMETER_OFFSET+4*8+2*4], edx
        mov [r10], r11d ; store 32-bit stack pointer

        sub rsp, 5*8 ; reserve slots for four function parameters and align stack to 16 bytes

        call Game_RunTimer

        add rsp, 5*8

        mov r10, [rsp+FIRST_PARAMETER_OFFSET+3*8] ; load first function parameter from reserved slot
        mov eax, [rsp+FIRST_PARAMETER_OFFSET+4*8] ; load registers from reserved slots
        mov ecx, [rsp+FIRST_PARAMETER_OFFSET+4*8+4]
        mov edx, [rsp+FIRST_PARAMETER_OFFSET+4*8+2*4]
        mov r11d, [r10] ; load 32-bit stack pointer

        popf

    ; start of epilog for unwinding
        pop r13 ; restore non-volatile register from stack
        ret
    .end_function: ; end of function for unwinding
%else
        PUSH32 eax, ecx, edx, esi, edi

        mov r10, [rsp+2*8] ; load first function parameter from stack
        mov [r10], r11d ; store 32-bit stack pointer

        call Game_RunTimer

        mov r10, [rsp+2*8] ; load first function parameter from stack
        mov r11d, [r10] ; load 32-bit stack pointer

        POP32 eax, ecx, edx, esi, edi

        popf

        ret
%endif

; end procedure SR_CheckTimer

%ifidn __OUTPUT_FORMAT__, win64

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
        P_UNWIND_INFO SR_CheckTimer, SR_CheckTimer_RunTimer.end_function, x_SR_CheckTimer
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
align 8
x_SR_CheckTimer:
    ; UNWIND_INFO
        db 1 ; Version (1) & Flags (0)
        db SR_CheckTimer.end_prolog - SR_CheckTimer ; Size of prolog
        db 2 ; Count of unwind codes
        db 0x0d ; Frame Register (13 = r13) & Frame Register offset (scaled) (0)
    ; UNWIND_CODE[2]
        db SR_CheckTimer.end_prolog - SR_CheckTimer ; Offset in prolog
        db 0x03 ; Unwind operation code (3 = UWOP_SET_FPREG)
        db SR_CheckTimer.prolog_1 - SR_CheckTimer ; Offset in prolog
        db 0xd0 ; Unwind operation code (0 = UWOP_PUSH_NONVOL) & Operation info (13 = r13)
%endif
