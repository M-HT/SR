;;
;;  Copyright (C) 2019-2026 Roman Pauer
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

%macro  Call_Asm_Prologue_0 0

%ifidn __OUTPUT_FORMAT__, win64
        mov r10, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
        mov [r10], r11d ; store 32-bit stack pointer
%else
        sub r11d, byte 2*4
        mov [r11d], esi
        mov [r11d+4], edi

        mov r10, [rsp] ; load first function parameter from stack
        mov [r10], r11d ; store 32-bit stack pointer
%endif

%endmacro


%macro  Call_Asm_Epilogue_0 1

%ifidn __OUTPUT_FORMAT__, win64
        mov r10, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
        mov r11d, [r10] ; load 32-bit stack pointer

        mov r8d, [r11d]
        add r11d, byte 4+(%1*4)
%else
        mov r10, [rsp] ; load first function parameter from stack
        mov r11d, [r10] ; load 32-bit stack pointer

        mov esi, [r11d]
        mov edi, [r11d+4]
        mov r8d, [r11d+2*4]
        add r11d, byte 3*4+(%1*4)
%endif

        jmp r8

%endmacro


%macro  Call_Asm_Float1_Int 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        lea rcx, [rsp+FIRST_PARAMETER_OFFSET+8+8]
        fstp qword [rcx]
%else
        sub rsp, byte 16
        mov rdi, rsp
        fstp qword [rdi]
%endif

        call %1

%ifidn __OUTPUT_FORMAT__, win64
%else
        add rsp, byte 16
%endif

        mov rdx, rax
        shr rdx, 32
        mov eax, eax

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Float1_Float 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        lea rcx, [rsp+FIRST_PARAMETER_OFFSET+8+8]
        fstp qword [rcx]
%else
        sub rsp, byte 16
        mov rdi, rsp
        fstp qword [rdi]
%endif

        call %1

%ifidn __OUTPUT_FORMAT__, win64
        fld qword [rsp+FIRST_PARAMETER_OFFSET+8+8]
%else
        fld qword [rsp]
        add rsp, byte 16
%endif

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Float2_Float 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        lea rcx, [rsp+FIRST_PARAMETER_OFFSET+8+8]
        fstp qword [rcx]
        fstp qword [rcx+8]
%else
        sub rsp, byte 16
        mov rdi, rsp
        fstp qword [rdi]
        fstp qword [rdi+8]
%endif

        call %1

%ifidn __OUTPUT_FORMAT__, win64
        fld qword [rsp+FIRST_PARAMETER_OFFSET+8+8]
%else
        fld qword [rsp]
        add rsp, byte 16
%endif

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_VariableStack2 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
        lea r8d, [r11d+3*4]
%else
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
        lea edx, [r11d+5*4]
%endif

        call %1

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack0 1-2 0

        Call_Asm_Prologue_0

        call %1

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack1 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        mov ecx, [r11d+4]
%else
        mov edi, [r11d+3*4]
%endif

        call %1

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack2 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
%else
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
%endif

        call %1

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack3 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
        mov r8d, [r11d+3*4]
%else
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
        mov edx, [r11d+5*4]
%endif

        call %1

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack4 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
        mov r8d, [r11d+3*4]
        mov r9d, [r11d+4*4]
%else
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
        mov edx, [r11d+5*4]
        mov ecx, [r11d+6*4]
%endif

        call %1

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack5 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        sub rsp, byte 16
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
        mov r8d, [r11d+3*4]
        mov r9d, [r11d+4*4]
        mov eax, [r11d+5*4]
        mov [rsp+4*8], rax
%else
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
        mov edx, [r11d+5*4]
        mov ecx, [r11d+6*4]
        mov r8d, [r11d+7*4]
%endif

        call %1

%ifidn __OUTPUT_FORMAT__, win64
        add rsp, byte 16
%else
%endif

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack6 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        sub rsp, byte 16
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
        mov r8d, [r11d+3*4]
        mov r9d, [r11d+4*4]
        mov eax, [r11d+5*4]
        mov [rsp+4*8], rax
        mov eax, [r11d+6*4]
        mov [rsp+5*8], rax
%else
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
        mov edx, [r11d+5*4]
        mov ecx, [r11d+6*4]
        mov r8d, [r11d+7*4]
        mov r9d, [r11d+8*4]
%endif

        call %1

%ifidn __OUTPUT_FORMAT__, win64
        add rsp, byte 16
%else
%endif

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack7 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        sub rsp, byte 32
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
        mov r8d, [r11d+3*4]
        mov r9d, [r11d+4*4]
        mov eax, [r11d+5*4]
        mov [rsp+4*8], rax
        mov eax, [r11d+6*4]
        mov [rsp+5*8], rax
        mov eax, [r11d+7*4]
        mov [rsp+6*8], rax
%else
        sub rsp, byte 16
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
        mov edx, [r11d+5*4]
        mov ecx, [r11d+6*4]
        mov r8d, [r11d+7*4]
        mov r9d, [r11d+8*4]
        mov eax, [r11d+9*4]
        mov [rsp], rax
%endif

        call %1

%ifidn __OUTPUT_FORMAT__, win64
        add rsp, byte 32
%else
        add rsp, byte 16
%endif

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack8 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        sub rsp, byte 32
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
        mov r8d, [r11d+3*4]
        mov r9d, [r11d+4*4]
        mov eax, [r11d+5*4]
        mov [rsp+4*8], rax
        mov eax, [r11d+6*4]
        mov [rsp+5*8], rax
        mov eax, [r11d+7*4]
        mov [rsp+6*8], rax
        mov eax, [r11d+8*4]
        mov [rsp+7*8], rax
%else
        sub rsp, byte 16
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
        mov edx, [r11d+5*4]
        mov ecx, [r11d+6*4]
        mov r8d, [r11d+7*4]
        mov r9d, [r11d+8*4]
        mov eax, [r11d+9*4]
        mov [rsp], rax
        mov eax, [r11d+10*4]
        mov [rsp+8], rax
%endif

        call %1

%ifidn __OUTPUT_FORMAT__, win64
        add rsp, byte 32
%else
        add rsp, byte 16
%endif

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack9 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        sub rsp, byte 48
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
        mov r8d, [r11d+3*4]
        mov r9d, [r11d+4*4]
        mov eax, [r11d+5*4]
        mov [rsp+4*8], rax
        mov eax, [r11d+6*4]
        mov [rsp+5*8], rax
        mov eax, [r11d+7*4]
        mov [rsp+6*8], rax
        mov eax, [r11d+8*4]
        mov [rsp+7*8], rax
        mov eax, [r11d+9*4]
        mov [rsp+8*8], rax
%else
        sub rsp, byte 32
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
        mov edx, [r11d+5*4]
        mov ecx, [r11d+6*4]
        mov r8d, [r11d+7*4]
        mov r9d, [r11d+8*4]
        mov eax, [r11d+9*4]
        mov [rsp], rax
        mov eax, [r11d+10*4]
        mov [rsp+8], rax
        mov eax, [r11d+11*4]
        mov [rsp+2*8], rax
%endif

        call %1

%ifidn __OUTPUT_FORMAT__, win64
        add rsp, byte 48
%else
        add rsp, byte 32
%endif

        Call_Asm_Epilogue_0 %2

%endmacro


%macro  Call_Asm_Stack12 1-2 0

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        sub rsp, byte 64
        mov ecx, [r11d+4]
        mov edx, [r11d+2*4]
        mov r8d, [r11d+3*4]
        mov r9d, [r11d+4*4]
        mov eax, [r11d+5*4]
        mov [rsp+4*8], rax
        mov eax, [r11d+6*4]
        mov [rsp+5*8], rax
        mov eax, [r11d+7*4]
        mov [rsp+6*8], rax
        mov eax, [r11d+8*4]
        mov [rsp+7*8], rax
        mov eax, [r11d+9*4]
        mov [rsp+8*8], rax
        mov eax, [r11d+10*4]
        mov [rsp+9*8], rax
        mov eax, [r11d+11*4]
        mov [rsp+10*8], rax
        mov eax, [r11d+12*4]
        mov [rsp+11*8], rax
%else
        sub rsp, byte 48
        mov edi, [r11d+3*4]
        mov esi, [r11d+4*4]
        mov edx, [r11d+5*4]
        mov ecx, [r11d+6*4]
        mov r8d, [r11d+7*4]
        mov r9d, [r11d+8*4]
        mov eax, [r11d+9*4]
        mov [rsp], rax
        mov eax, [r11d+10*4]
        mov [rsp+8], rax
        mov eax, [r11d+11*4]
        mov [rsp+2*8], rax
        mov eax, [r11d+12*4]
        mov [rsp+3*8], rax
        mov eax, [r11d+13*4]
        mov [rsp+4*8], rax
        mov eax, [r11d+14*4]
        mov [rsp+5*8], rax
%endif

        call %1

%ifidn __OUTPUT_FORMAT__, win64
        add rsp, byte 64
%else
        add rsp, byte 48
%endif

        Call_Asm_Epilogue_0 %2

%endmacro

