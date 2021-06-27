;;
;;  Copyright (C) 2020-2021 Roman Pauer
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

%include "asm-calls.inc"

%ifidn __OUTPUT_FORMAT__, win32
    %define BLEV_Init_c _BLEV_Init_c
    %define BLEV_Exit_c _BLEV_Exit_c
    %define BLEV_ClearAllEvents_c _BLEV_ClearAllEvents_c
    %define BLEV_PutEvent_c _BLEV_PutEvent_c
    %define BLEV_GetEvent_c _BLEV_GetEvent_c
    %define BLEV_PeekEvent_c _BLEV_PeekEvent_c
%endif

extern BLEV_Init_c
extern BLEV_Exit_c
extern BLEV_ClearAllEvents_c
extern BLEV_PutEvent_c
extern BLEV_GetEvent_c
extern BLEV_PeekEvent_c

global BLEV_Init
global BLEV_Exit
global BLEV_ClearAllEvents
global BLEV_PutEvent
global BLEV_GetEvent
global BLEV_PeekEvent

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
BLEV_Init:

; [esp      ] = return address

        Call_Asm_Stack0 BLEV_Init_c

        retn

; end procedure BLEV_Init


align 16
BLEV_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 BLEV_Exit_c

        retn

; end procedure BLEV_Exit


align 16
BLEV_ClearAllEvents:

; [esp      ] = return address

        Call_Asm_Stack0 BLEV_ClearAllEvents_c

        retn

; end procedure BLEV_ClearAllEvents


align 16
BLEV_PutEvent:

; [esp +   4] = const BLEV_Event * event
; [esp      ] = return address

        Call_Asm_Stack1 BLEV_PutEvent_c

        retn

; end procedure BLEV_PutEvent


align 16
BLEV_GetEvent:

; [esp +   4] = BLEV_Event * event
; [esp      ] = return address

        Call_Asm_Stack1 BLEV_GetEvent_c

        retn

; end procedure BLEV_GetEvent


align 16
BLEV_PeekEvent:

; [esp +   4] = BLEV_Event * event
; [esp      ] = return address

        Call_Asm_Stack1 BLEV_PeekEvent_c

        retn

; end procedure BLEV_PeekEvent


