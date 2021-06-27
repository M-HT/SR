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
    %define SYSTEM_SetInitValues_c _SYSTEM_SetInitValues_c
    %define SYSTEM_SystemTask_c _SYSTEM_SystemTask_c
    %define SYSTEM_Init_c _SYSTEM_Init_c
    %define SYSTEM_Exit_c _SYSTEM_Exit_c
    %define SYSTEM_GetTicks_c _SYSTEM_GetTicks_c
    %define SYSTEM_IsApplicationActive_c _SYSTEM_IsApplicationActive_c
    %define SYSTEM_WaitTicks_c _SYSTEM_WaitTicks_c
    %define SYSTEM_EnterCriticalSection_c _SYSTEM_EnterCriticalSection_c
    %define SYSTEM_LeaveCriticalSection_c _SYSTEM_LeaveCriticalSection_c
    %define SYSTEM_InCriticalSection_c _SYSTEM_InCriticalSection_c
    %define SYSTEM_GetOS_c _SYSTEM_GetOS_c
%endif

extern SYSTEM_SetInitValues_c
extern SYSTEM_SystemTask_c
extern SYSTEM_Init_c
extern SYSTEM_Exit_c
extern SYSTEM_GetTicks_c
extern SYSTEM_IsApplicationActive_c
extern SYSTEM_WaitTicks_c
extern SYSTEM_EnterCriticalSection_c
extern SYSTEM_LeaveCriticalSection_c
extern SYSTEM_InCriticalSection_c
extern SYSTEM_GetOS_c

global SYSTEM_SetInitValues
global SYSTEM_SystemTask
global SYSTEM_Init
global SYSTEM_Exit
global SYSTEM_GetTicks
global SYSTEM_IsApplicationActive
global SYSTEM_WaitTicks
global SYSTEM_EnterCriticalSection
global SYSTEM_LeaveCriticalSection
global SYSTEM_InCriticalSection
global SYSTEM_GetOS

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
SYSTEM_SetInitValues:

; [esp + 2*4] = const char * value
; [esp +   4] = int type
; [esp      ] = return address

        Call_Asm_Stack2 SYSTEM_SetInitValues_c

        retn

; end procedure SYSTEM_SetInitValues


align 16
SYSTEM_SystemTask:

; [esp      ] = return address

        Call_Asm_Stack0 SYSTEM_SystemTask_c

        retn

; end procedure SYSTEM_SystemTask


align 16
SYSTEM_Init:

; [esp      ] = return address

        Call_Asm_Stack0 SYSTEM_Init_c

        retn

; end procedure SYSTEM_Init


align 16
SYSTEM_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 SYSTEM_Exit_c

        retn

; end procedure SYSTEM_Exit


align 16
SYSTEM_GetTicks:

; [esp      ] = return address

        Call_Asm_Stack0 SYSTEM_GetTicks_c

        retn

; end procedure SYSTEM_GetTicks


align 16
SYSTEM_IsApplicationActive:

; [esp      ] = return address

        Call_Asm_Stack0 SYSTEM_IsApplicationActive_c

        retn

; end procedure SYSTEM_IsApplicationActive


align 16
SYSTEM_WaitTicks:

; [esp +   4] = unsigned int ticks
; [esp      ] = return address

        Call_Asm_Stack1 SYSTEM_WaitTicks_c

        retn

; end procedure SYSTEM_WaitTicks


align 16
SYSTEM_EnterCriticalSection:

; [esp      ] = return address

        Call_Asm_Stack0 SYSTEM_EnterCriticalSection_c

        retn

; end procedure SYSTEM_EnterCriticalSection


align 16
SYSTEM_LeaveCriticalSection:

; [esp      ] = return address

        Call_Asm_Stack0 SYSTEM_LeaveCriticalSection_c

        retn

; end procedure SYSTEM_LeaveCriticalSection


align 16
SYSTEM_InCriticalSection:

; [esp      ] = return address

        Call_Asm_Stack0 SYSTEM_InCriticalSection_c

        retn

; end procedure SYSTEM_InCriticalSection


align 16
SYSTEM_GetOS:

; [esp      ] = return address

        Call_Asm_Stack0 SYSTEM_GetOS_c

        retn

; end procedure SYSTEM_GetOS


