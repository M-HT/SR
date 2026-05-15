;;
;;  Copyright (C) 2026 Roman Pauer
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

extern GetRecordName_
extern WinMain_


global c_GetRecordName_
global c_MessageProc_c2asm
global c_WinMain_
global c_RunWndProc_c2asm
global c_run_thread_c2asm


%ifidn __OUTPUT_FORMAT__, win64

%macro PROLOG 0
        SECTION_PROLOG
        mov [rsp+FIRST_PARAMETER_OFFSET], rcx ; store first function parameter in reserved slot
        mov r11d, [rcx] ; load 32-bit stack pointer
%endm

%macro EPILOG 0
        mov rcx, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
        mov [rcx], r11d ; store 32-bit stack pointer
        SECTION_EPILOG
    .end_function: ; end of function for unwinding
%endm

%macro P_UNWIND 1
        P_UNWIND_INFO %1, %1.end_function, x_common
%endm

%else

%macro PROLOG 0
        push rbp ; save non-volatile register on stack
        push rbx ; save non-volatile register on stack
        push rdi ; save first function parameter on stack (which also alignes the stack to 16 bytes)
        mov r11d, [rdi] ; load 32-bit stack pointer
%endm

%macro EPILOG 0
        pop rdi ; restore first function parameter from stack
        mov [rdi], r11d ; store 32-bit stack pointer
        pop rbx ; restore non-volatile register from stack
        pop rbp ; restore non-volatile register from stack
        ret
%endm

%endif


%ifidn __OUTPUT_FORMAT__, elf64
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
c_GetRecordName_:

; rsi/rdx = uint32_t RecordKey
; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 edx
%else
        PUSH32 esi
%endif

        CALL GetRecordName_

        add r11d, byte 4

        EPILOG

; end procedure c_GetRecordName_

align 16
c_MessageProc_c2asm:

; r8/[rsp+5*8] = uint32_t (*MessageProc)(const char *, uint32_t, uint32_t)
; rcx/r9  = uint32_t MessageCode
; rdx/r8  = uint32_t MessageType
; rsi/rdx = const char *MessageText
; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

%ifidn __OUTPUT_FORMAT__, win64
        mov r10d, [rsp+FIRST_PARAMETER_OFFSET+4*8]
        PUSH32 r9d, r8d, edx
%else
        mov r10d, r8d
        PUSH32 ecx, edx, esi
%endif

        CALL r10

        add r11d, byte 3*4

        EPILOG

; end procedure c_MessageProc_c2asm

align 16
c_WinMain_:

; r8/[rsp+5*8] = int nCmdShow
; rcx/r9  = char *lpCmdLine
; rdx/r8  = void *hPrevInstance
; rsi/rdx = void *hInstance
; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

%ifidn __OUTPUT_FORMAT__, win64
        mov eax, [rsp+FIRST_PARAMETER_OFFSET+4*8]
        PUSH32 eax, r9d, r8d, edx
%else
        PUSH32 r8d, ecx, edx, esi
%endif

        ; stdcall
        CALL WinMain_

        EPILOG

; end procedure c_WinMain_

align 16
c_RunWndProc_c2asm:

; r9/[rsp+6*8] = uint32_t (*WndProc)(void *, uint32_t, uint32_t, uint32_t)
; r8/[rsp+5*8] = uint32_t lParam
; rcx/r9  = uint32_t wParam
; rdx/r8  = uint32_t uMsg
; rsi/rdx = void *hwnd
; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

%ifidn __OUTPUT_FORMAT__, win64
        mov eax, [rsp+FIRST_PARAMETER_OFFSET+4*8]
        PUSH32 eax, r9d, r8d, edx
        mov r9d, [rsp+FIRST_PARAMETER_OFFSET+5*8]
%else
        PUSH32 r8d, ecx, edx, esi
%endif

        ; stdcall
        CALL r9

        EPILOG

; end procedure c_RunWndProc_c2asm

align 16
c_run_thread_c2asm:

; rdx/r8  = void(*start_address)(void *)
; rsi/rdx = void *arglist
; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

%ifidn __OUTPUT_FORMAT__, win64
        mov r10d, r8d
        PUSH32 edx
        CALL r10
%else
        PUSH32 esi
        CALL rdx
%endif

        add r11d, byte 4

        EPILOG

; end procedure c_run_thread_c2asm


%ifidn __OUTPUT_FORMAT__, win64

section .pdata rdata align=4
        P_UNWIND c_GetRecordName_
        P_UNWIND c_MessageProc_c2asm
        P_UNWIND c_WinMain_
        P_UNWIND c_RunWndProc_c2asm
        P_UNWIND c_run_thread_c2asm

section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO c_GetRecordName_

%endif

