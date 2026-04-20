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

%macro  Game_Set_errno 2

%ifnum %1
	%if %1 >= 0
		mov dword [errno_val], %1
	%endif
%else
	%ifidn __OUTPUT_FORMAT__, win64
		mov [rsp+FIRST_PARAMETER_OFFSET+8+%2], eax ; store register in reserved slots

		call Game_errno
		mov [errno_val], eax

		mov eax, [rsp+FIRST_PARAMETER_OFFSET+8+%2] ; load register from reserved slots
	%else
		sub rsp, byte 16
		mov [rsp], eax

		call Game_errno
		mov [errno_val], eax

		mov eax, [rsp]
		add rsp, byte 16
	%endif
%endif

%endmacro


%macro  Game_Call_Prologue_0 0

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


%macro  Game_Call_Epilogue_0 1-2 0

		Game_Set_errno %1,0

%ifidn __OUTPUT_FORMAT__, win64
		mov r10, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
		mov r11d, [r10] ; load 32-bit stack pointer

		mov r8d, [r11d]
		add r11d, byte 4+%2
%else
		mov r10, [rsp] ; load first function parameter from stack
		mov r11d, [r10] ; load 32-bit stack pointer

		mov esi, [r11d]
		mov edi, [r11d+4]
		mov r8d, [r11d+2*4]
		add r11d, byte 3*4+%2
%endif

		jmp r8

%endmacro


%macro  Game_Call_Prologue_1 1

%ifidn __OUTPUT_FORMAT__, win64
		mov r10, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
		mov [rsp+FIRST_PARAMETER_OFFSET+8], %1 ; store register in reserved slots
		mov [r10], r11d ; store 32-bit stack pointer
%else
		sub r11d, byte 3*4
		mov [r11d], %1
		mov [r11d+4], esi
		mov [r11d+2*4], edi

		mov r10, [rsp] ; load first function parameter from stack
		mov [r10], r11d ; store 32-bit stack pointer
%endif

%endmacro


%macro  Game_Call_Epilogue_1 2-3 0

		Game_Set_errno %2,4

%ifidn __OUTPUT_FORMAT__, win64
		mov r10, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
		mov %1, [rsp+FIRST_PARAMETER_OFFSET+8] ; load register from reserved slots
		mov r11d, [r10] ; load 32-bit stack pointer

		mov r8d, [r11d]
		add r11d, byte 4+%3
%else
		mov r10, [rsp] ; load first function parameter from stack
		mov r11d, [r10] ; load 32-bit stack pointer

		mov %1, [r11d]
		mov esi, [r11d+4]
		mov edi, [r11d+2*4]
		mov r8d, [r11d+3*4]
		add r11d, byte 4*4+%3
%endif

		jmp r8

%endmacro


%macro  Game_Call_Prologue_2 2

%ifidn __OUTPUT_FORMAT__, win64
		mov r10, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
		mov [rsp+FIRST_PARAMETER_OFFSET+8], %1 ; store registers in reserved slots
		mov [rsp+FIRST_PARAMETER_OFFSET+8+4], %2
		mov [r10], r11d ; store 32-bit stack pointer
%else
		sub r11d, byte 4*4
		mov [r11d], %1
		mov [r11d+4], %2
		mov [r11d+2*4], esi
		mov [r11d+3*4], edi

		mov r10, [rsp] ; load first function parameter from stack
		mov [r10], r11d ; store 32-bit stack pointer
%endif

%endmacro


%macro  Game_Call_Epilogue_2 3-4 0

		Game_Set_errno %3,8

%ifidn __OUTPUT_FORMAT__, win64
		mov r10, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
		mov %1, [rsp+FIRST_PARAMETER_OFFSET+8] ; load registers from reserved slots
		mov %2, [rsp+FIRST_PARAMETER_OFFSET+8+4]
		mov r11d, [r10] ; load 32-bit stack pointer

		mov r8d, [r11d]
		add r11d, byte 4+%4
%else
		mov r10, [rsp] ; load first function parameter from stack
		mov r11d, [r10] ; load 32-bit stack pointer

		mov %1, [r11d]
		mov %2, [r11d+4]
		mov esi, [r11d+2*4]
		mov edi, [r11d+3*4]
		mov r8d, [r11d+4*4]
		add r11d, byte 5*4+%4
%endif

		jmp r8

%endmacro


%macro  Game_Call_Prologue_3 3

%ifidn __OUTPUT_FORMAT__, win64
		mov r10, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
		mov [rsp+FIRST_PARAMETER_OFFSET+8], %1 ; store registers in reserved slots
		mov [rsp+FIRST_PARAMETER_OFFSET+8+4], %2
		mov [rsp+FIRST_PARAMETER_OFFSET+8+8], %3
		mov [r10], r11d ; store 32-bit stack pointer
%else
		sub r11d, byte 5*4
		mov [r11d], %1
		mov [r11d+4], %2
		mov [r11d+2*4], %3
		mov [r11d+3*4], esi
		mov [r11d+4*4], edi

		mov r10, [rsp] ; load first function parameter from stack
		mov [r10], r11d ; store 32-bit stack pointer
%endif

%endmacro


%macro  Game_Call_Epilogue_3 4-5 0

		Game_Set_errno %4,12

%ifidn __OUTPUT_FORMAT__, win64
		mov r10, [rsp+FIRST_PARAMETER_OFFSET] ; load first function parameter from reserved slot
		mov %1, [rsp+FIRST_PARAMETER_OFFSET+8] ; load registers from reserved slots
		mov %2, [rsp+FIRST_PARAMETER_OFFSET+8+4]
		mov %3, [rsp+FIRST_PARAMETER_OFFSET+8+8]
		mov r11d, [r10] ; load 32-bit stack pointer

		mov r8d, [r11d]
		add r11d, byte 4+%5
%else
		mov r10, [rsp] ; load first function parameter from stack
		mov r11d, [r10] ; load 32-bit stack pointer

		mov %1, [r11d]
		mov %2, [r11d+4]
		mov %3, [r11d+2*4]
		mov esi, [r11d+3*4]
		mov edi, [r11d+4*4]
		mov r8d, [r11d+5*4]
		add r11d, byte 6*4+%5
%endif

		jmp r8

%endmacro


%macro  Game_Call_Asm_VariableStack1 1-2 'get_errno_val'

		Game_Call_Prologue_2 ecx,edx

%ifidn __OUTPUT_FORMAT__, win64
		mov ecx, [r11d+4]
		lea edx, [r11d+2*4]
%else
		mov edi, [r11d+5*4]
		lea esi, [r11d+6*4]
%endif

		call %1

		Game_Call_Epilogue_2 ecx,edx,%2

%endmacro


%macro  Game_Call_Asm_VariableStack2 1-2 'get_errno_val'

		Game_Call_Prologue_2 ecx,edx

%ifidn __OUTPUT_FORMAT__, win64
		mov ecx, [r11d+4]
		mov edx, [r11d+2*4]
		lea r8d, [r11d+3*4]
%else
		mov edi, [r11d+5*4]
		mov esi, [r11d+6*4]
		lea edx, [r11d+7*4]
%endif

		call %1

		Game_Call_Epilogue_2 ecx,edx,%2

%endmacro


%macro  Game_Call_Asm_Stack1 1-2 'get_errno_val'

		Game_Call_Prologue_2 ecx,edx

%ifidn __OUTPUT_FORMAT__, win64
		mov ecx, [r11d+4]
%else
		mov edi, [r11d+5*4]
%endif

		call %1

		Game_Call_Epilogue_2 ecx,edx,%2

%endmacro


%macro  Game_Call_Asm_Stack2 1-2 'get_errno_val'

		Game_Call_Prologue_2 ecx,edx

%ifidn __OUTPUT_FORMAT__, win64
		mov ecx, [r11d+4]
		mov edx, [r11d+2*4]
%else
		mov edi, [r11d+5*4]
		mov esi, [r11d+6*4]
%endif

		call %1

		Game_Call_Epilogue_2 ecx,edx,%2

%endmacro


%macro  Game_Call_Asm_Stack3 1-2 'get_errno_val'

		Game_Call_Prologue_2 ecx,edx

%ifidn __OUTPUT_FORMAT__, win64
		mov ecx, [r11d+4]
		mov edx, [r11d+2*4]
		mov r8d, [r11d+3*4]
%else
		mov edi, [r11d+5*4]
		mov esi, [r11d+6*4]
		mov edx, [r11d+7*4]
%endif

		call %1

		Game_Call_Epilogue_2 ecx,edx,%2

%endmacro


%macro  Game_Call_Asm_Reg0 1-2 'get_errno_val'

		Game_Call_Prologue_2 ecx,edx

		call %1

		Game_Call_Epilogue_2 ecx,edx,%2

%endmacro


%macro  Game_Call_Asm_Reg1 1-2 'get_errno_val'

		Game_Call_Prologue_2 ecx,edx

%ifidn __OUTPUT_FORMAT__, win64
		mov ecx, eax
%else
		mov edi, eax
%endif

		call %1

		Game_Call_Epilogue_2 ecx,edx,%2

%endmacro


%macro  Game_Call_Asm_Reg2 1-2 'get_errno_val'

		Game_Call_Prologue_1 ecx

%ifidn __OUTPUT_FORMAT__, win64
		mov ecx, eax
		mov edx, edx
%else
		mov edi, eax
		mov esi, edx
%endif

		call %1

		Game_Call_Epilogue_1 ecx,%2

%endmacro


%macro  Game_Call_Asm_Reg3 1-2 'get_errno_val'

		Game_Call_Prologue_1 ecx

%ifidn __OUTPUT_FORMAT__, win64
		mov ecx, eax
		mov edx, edx
		mov r8d, ebx
%else
		mov edi, eax
		mov esi, edx
		mov edx, ebx
%endif

		call %1

		Game_Call_Epilogue_1 ecx,%2

%endmacro


%macro  Game_Call_Asm_Reg4 1-2 'get_errno_val'

		Game_Call_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
		mov r9d, ecx ; 4th parameter
		mov ecx, eax
		mov edx, edx
		mov r8d, ebx
%else
		mov edi, eax
		mov esi, edx
		mov edx, ebx
		mov ecx, ecx
%endif

		call %1

		Game_Call_Epilogue_0 %2

%endmacro


%macro  Game_Call_Asm_Reg5 1-2 'get_errno_val'

		Game_Call_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
		mov r9d, ecx ; 4th parameter
		mov ecx, eax
		mov edx, edx
		mov r8d, ebx
		mov eax, [r11d+4]
		mov [rsp], eax
%else
		mov edi, eax
		mov esi, edx
		mov edx, ebx
		mov ecx, ecx
		mov r8d, [r11d+3*4]
%endif

		call %1

		Game_Call_Epilogue_0 %2,4

%endmacro

