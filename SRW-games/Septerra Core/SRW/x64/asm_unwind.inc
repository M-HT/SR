;part of static recompiler -- do not edit

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

%ifidn __OUTPUT_FORMAT__, win64

%macro SECTION_PROLOG 0
        push rsi ; save non-volatile register on stack
    .prolog_1:
        push rdi ; save non-volatile register on stack
    .prolog_2:
        push rbp ; save non-volatile register on stack
    .prolog_3:
        push rbx ; save non-volatile register on stack
    .prolog_4:
        push r13 ; save non-volatile register on stack
    .prolog_5:
        sub rsp, 4*8 ; reserve slots for four function parameters (stack is aligned to 16 bytes)
    .prolog_6:
        mov r13, rsp ; set frame pointer
    .end_prolog: ; end of prolog for unwinding
%endm

%define FIRST_PARAMETER_OFFSET (10*8)

%macro SECTION_EPILOG 0
    ; start of epilog for unwinding
        add rsp, 4*8 ; deallocate stack
        pop r13 ; restore non-volatile register from stack
        pop rbx ; restore non-volatile register from stack
        pop rbp ; restore non-volatile register from stack
        pop rdi ; restore non-volatile register from stack
        pop rsi ; restore non-volatile register from stack
        ret
%endm

%macro P_UNWIND_INFO 3
        dd %1 wrt ..imagebase ; Function start address
        dd %2 wrt ..imagebase ; Function end address
        dd %3 wrt ..imagebase ; Unwind info address
%endm

%macro X_UNWIND_INFO 1
    ; UNWIND_INFO
        db 1 ; Version (1) & Flags (0)
        db %1.end_prolog - %1 ; Size of prolog
        db 7 ; Count of unwind codes
        db 0x0d ; Frame Register (13 = r13) & Frame Register offset (scaled) (0)
    ; UNWIND_CODE[7]
        db %1.end_prolog - %1 ; Offset in prolog
        db 0x03 ; Unwind operation code (3 = UWOP_SET_FPREG)
        db %1.prolog_6 - %1 ; Offset in prolog
        db 0x32 ; Unwind operation code (2 = UWOP_ALLOC_SMALL) & Operation info (3 = 8*3+8)
        db %1.prolog_5 - %1 ; Offset in prolog
        db 0xd0 ; Unwind operation code (0 = UWOP_PUSH_NONVOL) & Operation info (13 = r13)
        db %1.prolog_4 - %1 ; Offset in prolog
        db 0x30 ; Unwind operation code (0 = UWOP_PUSH_NONVOL) & Operation info (3 = rbx)
        db %1.prolog_3 - %1 ; Offset in prolog
        db 0x50 ; Unwind operation code (0 = UWOP_PUSH_NONVOL) & Operation info (5 = rbp)
        db %1.prolog_2 - %1 ; Offset in prolog
        db 0x70 ; Unwind operation code (0 = UWOP_PUSH_NONVOL) & Operation info (7 = rdi)
        db %1.prolog_1 - %1 ; Offset in prolog
        db 0x60 ; Unwind operation code (0 = UWOP_PUSH_NONVOL) & Operation info (6 = rsi)
        db 0 ; Unused entry (for alignment)
        db 0 ; Unused entry
%endm

%endif

