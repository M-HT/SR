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

extern X86_InPortProcedure
extern X86_OutPortProcedure

global x86_in_al_imm
global x86_in_ax_imm
global x86_in_eax_imm
global x86_in_al_dx
global x86_in_ax_dx
global x86_in_eax_dx

global x86_out_imm_al
global x86_out_imm_ax
global x86_out_imm_eax
global x86_out_dx_al
global x86_out_dx_ax
global x86_out_dx_eax

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
x86_in_al_imm:

; r9d = port number
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r9d ; port number
    ; second function argument
        mov edx, 1 ; register length
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r9d ; port number
    ; second function argument
        mov esi, 1 ; register length
%endif

    ; stack is aligned to 16 bytes
        call X86_InPortProcedure

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

        mov al, r9b

        RET

; end procedure x86_in_al_imm

align 16
x86_in_ax_imm:

; r9d = port number
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r9d ; port number
    ; second function argument
        mov edx, 2 ; register length
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r9d ; port number
    ; second function argument
        mov esi, 2 ; register length
%endif

    ; stack is aligned to 16 bytes
        call X86_InPortProcedure

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

        mov ax, r9w

        RET

; end procedure x86_in_ax_imm

align 16
x86_in_eax_imm:

; r9d = port number
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r9d ; port number
    ; second function argument
        mov edx, 4 ; register length
%else
        PUSH32 ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r9d ; port number
    ; second function argument
        mov esi, 4 ; register length
%endif

    ; stack is aligned to 16 bytes
        call X86_InPortProcedure

%ifidn __OUTPUT_FORMAT__, win64
    ; restore original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov r11d, [r8]

        POP32 ecx, edx
%else
    ; restore original esp value
        mov r8, [rsp]
        mov r11d, [r8]

        POP32 ecx, edx, esi, edi
%endif
        POPFD

        RET

; end procedure x86_in_eax_imm

align 16
x86_in_al_dx:

; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        movzx rcx, dx ; port number
    ; second function argument
        mov edx, 1 ; register length
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        movzx rdi, dx ; port number
    ; second function argument
        mov esi, 1 ; register length
%endif

    ; stack is aligned to 16 bytes
        call X86_InPortProcedure

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

        mov al, r9b

        RET

; end procedure x86_in_al_dx

align 16
x86_in_ax_dx:

; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        movzx rcx, dx ; port number
    ; second function argument
        mov edx, 2 ; register length
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        movzx rdi, dx ; port number
    ; second function argument
        mov esi, 2 ; register length
%endif

    ; stack is aligned to 16 bytes
        call X86_InPortProcedure

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

        mov ax, r9w

        RET

; end procedure x86_in_ax_dx

align 16
x86_in_eax_dx:

; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        movzx rcx, dx ; port number
    ; second function argument
        mov edx, 4 ; register length
%else
        PUSH32 ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        movzx rdi, dx ; port number
    ; second function argument
        mov esi, 4 ; register length
%endif

    ; stack is aligned to 16 bytes
        call X86_InPortProcedure

%ifidn __OUTPUT_FORMAT__, win64
    ; restore original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov r11d, [r8]

        POP32 ecx, edx
%else
    ; restore original esp value
        mov r8, [rsp]
        mov r11d, [r8]

        POP32 ecx, edx, esi, edi
%endif
        POPFD

        RET

; end procedure x86_in_eax_dx

align 16
x86_out_imm_al:

; r9d = port number
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r9d ; port number
    ; second function argument
        mov edx, 1 ; register length
    ; third function argument
        movzx r8, al
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r9d ; port number
    ; second function argument
        mov esi, 1 ; register length
    ; third function argument
        movzx rdx, al
%endif

    ; stack is aligned to 16 bytes
        call X86_OutPortProcedure

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

; end procedure x86_out_imm_al

align 16
x86_out_imm_ax:

; r9d = port number
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r9d ; port number
    ; second function argument
        mov edx, 2 ; register length
    ; third function argument
        movzx r8, ax
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r9d ; port number
    ; second function argument
        mov esi, 2 ; register length
    ; third function argument
        movzx rdx, ax
%endif

    ; stack is aligned to 16 bytes
        call X86_OutPortProcedure

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

; end procedure x86_out_imm_ax

align 16
x86_out_imm_eax:

; r9d = port number
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r9d ; port number
    ; second function argument
        mov edx, 4 ; register length
    ; third function argument
        mov r8d, eax
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r9d ; port number
    ; second function argument
        mov esi, 4 ; register length
    ; third function argument
        mov edx, eax
%endif

    ; stack is aligned to 16 bytes
        call X86_OutPortProcedure

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

; end procedure x86_out_imm_eax

align 16
x86_out_dx_al:

; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        movzx rcx, dx ; port number
    ; second function argument
        mov edx, 1 ; register length
    ; third function argument
        movzx r8, al
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        movzx rdi, dx ; port number
    ; second function argument
        mov esi, 1 ; register length
    ; third function argument
        movzx rdx, al
%endif

    ; stack is aligned to 16 bytes
        call X86_OutPortProcedure

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

; end procedure x86_out_dx_al

align 16
x86_out_dx_ax:

; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        movzx rcx, dx ; port number
    ; second function argument
        mov edx, 2 ; register length
    ; third function argument
        movzx r8, ax
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        movzx rdi, dx ; port number
    ; second function argument
        mov esi, 2 ; register length
    ; third function argument
        movzx rdx, ax
%endif

    ; stack is aligned to 16 bytes
        call X86_OutPortProcedure

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

; end procedure x86_out_dx_ax

align 16
x86_out_dx_eax:

; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        movzx rcx, dx ; port number
    ; second function argument
        mov edx, 4 ; register length
    ; third function argument
        mov r8d, eax
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        movzx rdi, dx ; port number
    ; second function argument
        mov esi, 4 ; register length
    ; third function argument
        mov edx, eax
%endif

    ; stack is aligned to 16 bytes
        call X86_OutPortProcedure

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

; end procedure x86_out_dx_eax

%ifidn __OUTPUT_FORMAT__, win64
section_end:

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
%endif
