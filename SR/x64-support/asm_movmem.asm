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

extern X86_ReadMemProcedure
extern X86_WriteMemProcedure

global x86_mov_reg_mem_8
global x86_mov_reg_mem_16
global x86_mov_reg_mem_32

global x86_mov_mem_reg_8
global x86_mov_mem_reg_16
global x86_mov_mem_reg_32

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
x86_mov_reg_mem_8:

; r10d = address
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r10d ; address
    ; second function argument
        mov edx, 1 ; mem size
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r10d ; address
    ; second function argument
        mov esi, 1 ; mem size
%endif

    ; stack is aligned to 16 bytes
        call X86_ReadMemProcedure

    ; return value
        mov r9, rax

%ifidn __OUTPUT_FORMAT__, win64
    ; restore original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov r11d, [r8]

        POP32 eax, ecx, edx
%else
    ; restore original esp value
        mov r8, [rsp]
        mov r11d, [r8]

        POP32 eax, ecx, edx, esi, edi
%endif
        POPFD

; r9b = value
        RET

; end procedure x86_mov_reg_mem_8

align 16
x86_mov_reg_mem_16:

; r10d = address
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r10d ; address
    ; second function argument
        mov edx, 2 ; mem size
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r10d ; address
    ; second function argument
        mov esi, 2 ; mem size
%endif

    ; stack is aligned to 16 bytes
        call X86_ReadMemProcedure

    ; return value
        mov r9, rax

%ifidn __OUTPUT_FORMAT__, win64
    ; restore original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov r11d, [r8]

        POP32 eax, ecx, edx
%else
    ; restore original esp value
        mov r8, [rsp]
        mov r11d, [r8]

        POP32 eax, ecx, edx, esi, edi
%endif
        POPFD

; r9w = value
        RET

; end procedure x86_mov_reg_mem_16

align 16
x86_mov_reg_mem_32:

; r10d = address
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r10d ; address
    ; second function argument
        mov edx, 4 ; mem size
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r10d ; address
    ; second function argument
        mov esi, 4 ; mem size
%endif

    ; stack is aligned to 16 bytes
        call X86_ReadMemProcedure

    ; return value
        mov r9, rax

%ifidn __OUTPUT_FORMAT__, win64
    ; restore original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov r11d, [r8]

        POP32 eax, ecx, edx
%else
    ; restore original esp value
        mov r8, [rsp]
        mov r11d, [r8]

        POP32 eax, ecx, edx, esi, edi
%endif
        POPFD

; r9d = value
        RET

; end procedure x86_mov_reg_mem_32


align 16
x86_mov_mem_reg_8:

; r9b = value
; r10d = address
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r10d ; address
    ; second function argument
        mov edx, 1 ; mem size
    ; third function argument
        movzx r8, r9b
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r10d ; address
    ; second function argument
        mov esi, 1 ; mem size
    ; third function argument
        movzx rdx, r9b
%endif

    ; stack is aligned to 16 bytes
        call X86_WriteMemProcedure

%ifidn __OUTPUT_FORMAT__, win64
    ; restore original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov r11d, [r8]

        POP32 eax, ecx, edx
%else
    ; restore original esp value
        mov r8, [rsp]
        mov r11d, [r8]

        POP32 eax, ecx, edx, esi, edi
%endif
        POPFD

        RET

; end procedure x86_mov_mem_reg_8

align 16
x86_mov_mem_reg_16:

; r9w = value
; r10d = address
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r10d ; address
    ; second function argument
        mov edx, 2 ; mem size
    ; third function argument
        movzx r8, r9w
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r10d ; address
    ; second function argument
        mov esi, 2 ; mem size
    ; third function argument
        movzx rdx, r9w
%endif

    ; stack is aligned to 16 bytes
        call X86_WriteMemProcedure

%ifidn __OUTPUT_FORMAT__, win64
    ; restore original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov r11d, [r8]

        POP32 eax, ecx, edx
%else
    ; restore original esp value
        mov r8, [rsp]
        mov r11d, [r8]

        POP32 eax, ecx, edx, esi, edi
%endif
        POPFD

        RET

; end procedure x86_mov_mem_reg_16

align 16
x86_mov_mem_reg_32:

; r9d = value
; r10d = address
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r10d ; address
    ; second function argument
        mov edx, 4 ; mem size
    ; third function argument
        mov r8d, r9d
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r10d ; address
    ; second function argument
        mov esi, 4 ; mem size
    ; third function argument
        mov edx, r9d
%endif

    ; stack is aligned to 16 bytes
        call X86_WriteMemProcedure

%ifidn __OUTPUT_FORMAT__, win64
    ; restore original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov r11d, [r8]

        POP32 eax, ecx, edx
%else
    ; restore original esp value
        mov r8, [rsp]
        mov r11d, [r8]

        POP32 eax, ecx, edx, esi, edi
%endif
        POPFD

        RET

; end procedure x86_mov_mem_reg_32

%ifidn __OUTPUT_FORMAT__, win64
section_end:

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
%endif
