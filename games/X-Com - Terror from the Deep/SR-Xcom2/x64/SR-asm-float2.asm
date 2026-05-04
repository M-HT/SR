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

extern Game_acos2
extern Game_asin2

global SR_acos2
global SR_asin2
global SR_asin2_2

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
SR_acos2:

; st0    = x
; [r11d] = return address

        Game_Call_Prologue_2 ecx,edx

%ifidn __OUTPUT_FORMAT__, win64
        lea rcx, [rsp+FIRST_PARAMETER_OFFSET+8+8]
        fstp qword [rcx]

        call Game_acos2

        lea rax, [rsp+FIRST_PARAMETER_OFFSET+8+8]
        fld qword [rax]
%else
        sub rsp, byte 16
        mov rdi, rsp
        fstp qword [rdi]

        call Game_acos2

        fld qword [rsp]
        add rsp, byte 16
%endif

        Game_Call_Epilogue_2 ecx,edx,-1

; end procedure SR_acos2

align 16
SR_asin2:

; st0    = x
; [r11d] = return address

        Game_Call_Prologue_2 ecx,edx

%ifidn __OUTPUT_FORMAT__, win64
        lea rcx, [rsp+FIRST_PARAMETER_OFFSET+8+8]
        fstp qword [rcx]

        call Game_asin2

        lea rax, [rsp+FIRST_PARAMETER_OFFSET+8+8]
        fld qword [rax]
%else
        sub rsp, byte 16
        mov rdi, rsp
        fstp qword [rdi]

        call Game_asin2

        fld qword [rsp]
        add rsp, byte 16
%endif

        Game_Call_Epilogue_2 ecx,edx,-1

; end procedure SR_asin2

align 16
SR_asin2_2:

; st1    = y
; st0    = x
; [r11d] = return address

        Game_Call_Prologue_2 ecx,edx

%ifidn __OUTPUT_FORMAT__, win64
        lea rcx, [rsp+FIRST_PARAMETER_OFFSET+8+8]
        fstp qword [rcx]
        fstp qword [rcx+8]

        call Game_asin2

        lea rax, [rsp+FIRST_PARAMETER_OFFSET+8+8]
        fld qword [rax+8]
        fld qword [rax]
%else
        sub rsp, byte 16
        mov rdi, rsp
        fstp qword [rdi]
        fstp qword [rdi+8]

        call Game_asin2

        fld qword [rsp+8]
        fld qword [rsp]
        add rsp, byte 16
%endif

        Game_Call_Epilogue_2 ecx,edx,-1

; end procedure SR_asin2_2

%ifidn __OUTPUT_FORMAT__, win64
section_end:

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
%endif
