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

extern main_
extern keyboard_interrupt
extern update_timer


global c_main_
global c_keyboard_interrupt
global c_update_timer
global c_run_mouse_interrupt_subroutine
global c_run_ailcallback


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

%macro WATCALL_PARAMS_2 0
        ; __watcall
        mov eax, edx
        mov edx, r8d
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

%macro WATCALL_PARAMS_2 0
        ; __watcall
        mov eax, esi
        ;mov edx, edx
%endm

%endif


%ifidn __OUTPUT_FORMAT__, elf64
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
c_main_:

; rdx/r8  = char *argv[]
; rsi/rdx = int argc
; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

        WATCALL_PARAMS_2

        CALL main_

        EPILOG

; end procedure c_main_

align 16
c_keyboard_interrupt:

; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

        CALL keyboard_interrupt

        EPILOG

; end procedure c_keyboard_interrupt

align 16
c_update_timer:

; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

        CALL update_timer

        EPILOG

; end procedure c_update_timer

align 16
c_run_mouse_interrupt_subroutine:

; rdx/r8  = uint32_t *registers
; rsi/rdx = uint32_t subroutine
; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

%ifidn __OUTPUT_FORMAT__, win64
        mov r10d, edx
        mov eax, [r8]
        mov ebx, [r8+1*4]
        mov ecx, [r8+2*4]
        mov edx, [r8+3*4]
        mov esi, [r8+4*4]
        mov edi, [r8+5*4]
%else
        mov r10d, esi
        mov eax, [rdx]
        mov ebx, [rdx+1*4]
        mov ecx, [rdx+2*4]
        mov esi, [rdx+4*4]
        mov edi, [rdx+5*4]
        mov edx, [rdx+3*4]
%endif

        PUSH32 0    ; emulate far call
        CALL r10    ; subroutine

        EPILOG

; end procedure c_run_mouse_interrupt_subroutine

align 16
c_run_ailcallback:

; rdx/r8  = AIL_sample *sample
; rsi/rdx = AIL_sample_CB callback
; rdi/rcx = _stack *stack
; [rsp] = return address

        PROLOG

%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 r8d  ; sample
        CALL rdx    ; callback
%else
        PUSH32 edx  ; sample
        CALL rsi    ; callback
%endif

        lea r11d, [r11d+4]

        EPILOG

; end procedure c_run_ailcallback


%ifidn __OUTPUT_FORMAT__, win64

section .pdata rdata align=4
        P_UNWIND c_main_
        P_UNWIND c_keyboard_interrupt
        P_UNWIND c_update_timer
        P_UNWIND c_run_mouse_interrupt_subroutine
        P_UNWIND c_run_ailcallback

section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO c_main_

%endif

