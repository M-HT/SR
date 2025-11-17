;part of static recompiler -- do not edit

;;
;;  Copyright (C) 2019-2025 Roman Pauer
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

extern X86_ReadFsDword
extern X86_WriteFsDword

global x86_read_fs_dword
global x86_write_fs_dword

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
x86_read_fs_dword:

; r10d = uint32_t addr
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r10d ; addr
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r10d ; addr
%endif

    ; stack is aligned to 16 bytes
        call X86_ReadFsDword

    ; return value
        mov r9d, eax

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

; r9d = uint32_t value
        RET

; end procedure x86_read_fs_dword


align 16
x86_write_fs_dword:

; r9d  = uint32_t value
; r10d = uint32_t addr
; [esp] = return address

        PUSHFD
%ifidn __OUTPUT_FORMAT__, win64
        PUSH32 eax, ecx, edx

    ; store original esp value
        mov r8, [rsp+FIRST_PARAMETER_OFFSET]
        mov [r8], r11d

    ; first function argument
        mov ecx, r10d ; addr
    ; second function argument
        mov edx, r9d ; value
%else
        PUSH32 eax, ecx, edx, esi, edi

    ; store original esp value
        mov r8, [rsp]
        mov [r8], r11d

    ; first function argument
        mov edi, r10d ; addr
    ; second function argument
        mov esi, r9d ; value
%endif

    ; stack is aligned to 16 bytes
        call X86_WriteFsDword

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

; end procedure x86_write_fs_dword

%ifidn __OUTPUT_FORMAT__, win64
section_end:

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
%endif
