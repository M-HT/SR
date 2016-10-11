;;
;;  Copyright (C) 2016 Roman Pauer
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

%macro  Game_Call_Asm_Stack 1-2 'get_errno_val'

		pop eax
		mov [Game_ReturnAddress], eax

		mov [Game_Registers], ebx
		mov [Game_Registers + 4], ecx
		mov [Game_Registers + 8], edx

		call %1

%ifnum %2
	%if %2 >= 0
		mov dword [errno_val], %2
	%endif
%else
		call Game_Set_errno_Asm
%endif

		mov ebx, [Game_Registers]
		mov ecx, [Game_Registers + 4]
		mov edx, [Game_Registers + 8]

		jmp [Game_ReturnAddress]

%endmacro

%define Game_Call_Asm_Stack0 Game_Call_Asm_Stack


%macro  Game_Call_Asm_Reg0 1-2 'get_errno_val'

		push ecx
		push edx

		call %1

%ifnum %2
	%if %2 >= 0
		mov dword [errno_val], %2
	%endif
%else
		call Game_Set_errno_Asm
%endif

		pop edx
		pop ecx

		retn
%endmacro


%macro  Game_Call_Asm_Reg1 1-2 'get_errno_val'

		push ecx
		push edx
		push eax

		call %1

%ifnum %2
	%if %2 >= 0
		mov dword [errno_val], %2
	%endif
%else
		call Game_Set_errno_Asm
%endif

		add esp, byte 4
		pop edx
		pop ecx

		retn

%endmacro


%macro  Game_Call_Asm_Reg2 1-2 'get_errno_val'

		push ecx
		push edx
		push eax

		call %1

%ifnum %2
	%if %2 >= 0
		mov dword [errno_val], %2
	%endif
%else
		call Game_Set_errno_Asm
%endif

		add esp, byte 2*4
		pop ecx

		retn

%endmacro


%macro  Game_Call_Asm_Reg3 1-2 'get_errno_val'

		push ecx
		push ebx
		push edx
		push eax

		call %1

%ifnum %2
	%if %2 >= 0
		mov dword [errno_val], %2
	%endif
%else
		call Game_Set_errno_Asm
%endif

		add esp, byte 3*4
		pop ecx

		retn

%endmacro


%macro  Game_Call_Asm_Reg4 1-2 'get_errno_val'

		push ecx
		push ebx
		push edx
		push eax

		call %1

%ifnum %2
	%if %2 >= 0
		mov dword [errno_val], %2
	%endif
%else
		call Game_Set_errno_Asm
%endif

		add esp, byte 4*4

		retn

%endmacro


%macro  Game_Call_Asm_Reg5 1-2 'get_errno_val'

		push dword [esp + 4]
		push ecx
		push ebx
		push edx
		push eax

		call %1

%ifnum %2
	%if %2 >= 0
		mov dword [errno_val], %2
	%endif
%else
		call Game_Set_errno_Asm
%endif

		add esp, byte 5*4

		retn 4

%endmacro

