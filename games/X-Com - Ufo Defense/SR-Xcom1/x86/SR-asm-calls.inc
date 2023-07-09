;;
;;  Copyright (C) 2016-2023 Roman Pauer
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

%macro  Game_Call_Asm_VariableStack1 1-2 'get_errno_val'

        push ecx
        push edx

    ; remember original esp value
        mov ecx, esp
    ; reserve 12 bytes on stack
        sub esp, byte 12
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 2*4], ecx
    ; adjust stack for function argument, so that stack is aligned to 16 bytes before call
        add esp, byte 8

    ; push function arguments to stack
        lea edx, [ecx + 4*4]
        push edx
        push dword [ecx + 3*4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 2*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        pop edx
        pop ecx

        retn

%endmacro


%macro  Game_Call_Asm_VariableStack2 1-2 'get_errno_val'

        push ecx
        push edx

    ; remember original esp value
        mov ecx, esp
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        push ecx

    ; push function arguments to stack
        lea edx, [ecx + 5*4]
        push edx
        push dword [ecx + 4*4]
        push dword [ecx + 3*4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 3*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        pop edx
        pop ecx

        retn

%endmacro


%macro  Game_Call_Asm_Stack1 1-2 'get_errno_val'

        push ecx
        push edx

    ; remember original esp value
        mov ecx, esp
    ; reserve 8 bytes on stack
        sub esp, byte 8
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 1*4], ecx
    ; adjust stack for function argument, so that stack is aligned to 16 bytes before call
        add esp, byte 4

    ; push function arguments to stack
        push dword [ecx + 3*4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 1*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        pop edx
        pop ecx

        retn

%endmacro


%macro  Game_Call_Asm_Stack2 1-2 'get_errno_val'

        push ecx
        push edx

    ; remember original esp value
        mov ecx, esp
    ; reserve 12 bytes on stack
        sub esp, byte 12
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 2*4], ecx
    ; adjust stack for function argument, so that stack is aligned to 16 bytes before call
        add esp, byte 8

    ; push function arguments to stack
        push dword [ecx + 4*4]
        push dword [ecx + 3*4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 2*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        pop edx
        pop ecx

        retn

%endmacro


%macro  Game_Call_Asm_Stack3 1-2 'get_errno_val'

        push ecx
        push edx

    ; remember original esp value
        mov ecx, esp
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        push ecx

    ; push function arguments to stack
        push dword [ecx + 5*4]
        push dword [ecx + 4*4]
        push dword [ecx + 3*4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 3*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        pop edx
        pop ecx

        retn

%endmacro


%macro  Game_Call_Asm_Reg0 1-2 'get_errno_val'

        push ecx
        push edx

    ; remember original esp value
        mov ecx, esp
    ; reserve 4 bytes on stack
        sub esp, byte 4
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp], ecx

    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
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

    ; remember original esp value
        mov ecx, esp
    ; reserve 8 bytes on stack
        sub esp, byte 8
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; put function argument to stack
        mov [esp], eax

    ; save original esp value on stack
        mov [esp + 1*4], ecx

    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 1*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        pop edx
        pop ecx

        retn

%endmacro


%macro  Game_Call_Asm_Reg2 1-2 'get_errno_val'

        push ecx

    ; remember original esp value
        mov ecx, esp
    ; reserve 12 bytes on stack
        sub esp, byte 12
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; put function arguments to stack
        mov [esp], eax
        mov [esp + 1*4], edx

    ; save original esp value on stack
        mov [esp + 2*4], ecx

    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 2*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        pop ecx

        retn

%endmacro


%macro  Game_Call_Asm_Reg3 1-2 'get_errno_val'

        push ecx

    ; remember original esp value
        mov ecx, esp
    ; reserve 16 bytes on stack
        sub esp, byte 16
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; put function arguments to stack
        mov [esp], eax
        mov [esp + 1*4], edx
        mov [esp + 2*4], ebx

    ; save original esp value on stack
        mov [esp + 3*4], ecx

    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 3*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        pop ecx

        retn

%endmacro


%macro  Game_Call_Asm_Reg4 1-2 'get_errno_val'

    ; save original ecx value on stack
        push ecx
    ; remember original esp value
        lea ecx, [esp + 4]
    ; reserve 16 bytes on stack
        sub esp, byte 16
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; put function arguments to stack
        mov [esp], eax
        mov [esp + 1*4], edx
        mov [esp + 2*4], ebx

    ; read original ecx value from stack
        mov eax, [ecx - 4]

    ; put function argument to stack
        mov [esp + 3*4], eax

    ; save original esp value on stack
        mov [esp + 4*4], ecx

    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 4*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        retn

%endmacro


%macro  Game_Call_Asm_Reg5 1-2 'get_errno_val'

    ; save original ecx value on stack
        push ecx
    ; remember original esp value
        lea ecx, [esp + 4]
    ; reserve 20 bytes on stack
        sub esp, byte 20
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; put function arguments to stack
        mov [esp], eax
        mov [esp + 1*4], edx
        mov [esp + 2*4], ebx

    ; read original ecx value from stack
        mov eax, [ecx - 4]

    ; read original function argument from stack
        mov edx, [ecx + 4]

    ; put function arguments to stack
        mov [esp + 3*4], eax
        mov [esp + 4*4], edx

    ; save original esp value on stack
        mov [esp + 5*4], ecx

    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 5*4]

%ifnum %2
    %if %2 >= 0
        push %2
        call Game_Set_errno_num_Asm
    %endif
%else
        call Game_Set_errno_Asm
%endif

        retn 4

%endmacro

