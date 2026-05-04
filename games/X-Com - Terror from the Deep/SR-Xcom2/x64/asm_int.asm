;part of static recompiler -- do not edit

;;
;;  Copyright (C) 2016-2025 Roman Pauer
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

%include "asm_call.inc"
%include "asm_pushx.inc"
%include "asm_unwind.inc"

extern X86_InterruptProcedure

global x86_int

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
x86_int:

; r9d = interrupt number
; [esp] = return address

        PUSHFD
        PUSHAD
%ifidn __OUTPUT_FORMAT__, win64
    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r9d ; interrupt number
    ; second function argument
        mov edx, r11d
%else
    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r9d ; interrupt number
    ; second function argument
        mov esi, r11d
%endif

    ; stack is aligned to 16 bytes
        call X86_InterruptProcedure

    ; restore original esp value
%ifidn __OUTPUT_FORMAT__, win64
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
%else
        mov r8, [rsp]
%endif
        mov r11d, [r8]

        POPAD
        POPFD

        RET

; end procedure x86_int

%ifidn __OUTPUT_FORMAT__, win64
section_end:

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
%endif
