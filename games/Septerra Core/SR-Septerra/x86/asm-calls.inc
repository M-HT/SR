;;
;;  Copyright (C) 2019 Roman Pauer
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

%macro  Call_Asm_Float1_Int 1

; st0   = num
; [esp] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 16 bytes on stack
        sub esp, byte 16
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

        mov [esp + 4], eax
        lea eax, [esp + 8]
        fstp qword [eax]
        mov [esp], eax
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 4]

%endmacro

%macro  Call_Asm_Float1_Float 1

; st0   = num
; [esp] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 16 bytes on stack
        sub esp, byte 16
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

        mov [esp + 4], eax
        lea eax, [esp + 8]
        fstp qword [eax]
        mov [esp], eax
    ; stack is aligned to 16 bytes

        call %1

        fld qword [esp+8]

    ; restore original esp value from stack
        mov esp, [esp + 4]

%endmacro

%macro  Call_Asm_Float2_Float 1

; st1   = num2
; st0   = num1
; [esp] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 24 bytes on stack
        sub esp, byte 24
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

        mov [esp + 4], eax
        lea eax, [esp + 8]
        fstp qword [eax]
        fstp qword [eax + 8]
        mov [esp], eax
    ; stack is aligned to 16 bytes

        call %1

        fld qword [esp+8]

    ; restore original esp value from stack
        mov esp, [esp + 4]

%endmacro

%macro  Call_Asm_VariableStack2 1

; [esp + 3*4] = ...
; [esp + 2*4] = 2nd parameter
; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        push eax

    ; push function arguments to stack
        lea ecx, [eax + 3*4]
        push ecx
        push dword [eax + 2*4]
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 3*4]

%endmacro

%macro  Call_Asm_Stack0 1

; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 4 bytes on stack
        sub esp, byte 4
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp], eax

    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp]

%endmacro

%macro  Call_Asm_Stack1 1

; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 8 bytes on stack
        sub esp, byte 8
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 1*4], eax
    ; adjust stack for function argument, so that stack is aligned to 16 bytes before call
        add esp, byte 4

    ; push function arguments to stack
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 1*4]

%endmacro

%macro  Call_Asm_Stack2 1

; [esp + 2*4] = 2nd parameter
; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 12 bytes on stack
        sub esp, byte 12
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 2*4], eax
    ; adjust stack for function arguments, so that stack is aligned to 16 bytes before call
        add esp, byte 8

    ; push function arguments to stack
        push dword [eax + 2*4]
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 2*4]

%endmacro

%macro  Call_Asm_Stack3 1

; [esp + 3*4] = 3rd parameter
; ...
; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        push eax

    ; push function arguments to stack
        push dword [eax + 3*4]
        push dword [eax + 2*4]
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 3*4]

%endmacro

%macro  Call_Asm_Stack4 1

; [esp + 4*4] = 4th parameter
; ...
; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 4 bytes on stack
        sub esp, byte 4
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp], eax

    ; push function arguments to stack
    ; stack is aligned to 16 bytes
        push dword [eax + 4*4]
        push dword [eax + 3*4]
        push dword [eax + 2*4]
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 4*4]

%endmacro

%macro  Call_Asm_Stack5 1

; [esp + 5*4] = 5th parameter
; ...
; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 8 bytes on stack
        sub esp, byte 8
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 1*4], eax
    ; adjust stack for function argument, so that stack is aligned to 16 bytes before call
        add esp, byte 4

    ; push function arguments to stack
        push dword [eax + 5*4]
    ; stack is aligned to 16 bytes
        push dword [eax + 4*4]
        push dword [eax + 3*4]
        push dword [eax + 2*4]
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 5*4]

%endmacro

%macro  Call_Asm_Stack6 1

; [esp + 6*4] = 6th parameter
; ...
; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 12 bytes on stack
        sub esp, byte 12
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 2*4], eax
    ; adjust stack for function arguments, so that stack is aligned to 16 bytes before call
        add esp, byte 8

    ; push function arguments to stack
        push dword [eax + 6*4]
        push dword [eax + 5*4]
    ; stack is aligned to 16 bytes
        push dword [eax + 4*4]
        push dword [eax + 3*4]
        push dword [eax + 2*4]
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 6*4]

%endmacro

%macro  Call_Asm_Stack7 1

; [esp + 7*4] = 7th parameter
; ...
; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        push eax

    ; push function arguments to stack
        push dword [eax + 7*4]
        push dword [eax + 6*4]
        push dword [eax + 5*4]
    ; stack is aligned to 16 bytes
        push dword [eax + 4*4]
        push dword [eax + 3*4]
        push dword [eax + 2*4]
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 7*4]

%endmacro

%macro  Call_Asm_Stack8 1

; [esp + 8*4] = 8th parameter
; ...
; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 4 bytes on stack
        sub esp, byte 4
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp], eax

    ; push function arguments to stack
    ; stack is aligned to 16 bytes
        push dword [eax + 8*4]
        push dword [eax + 7*4]
        push dword [eax + 6*4]
        push dword [eax + 5*4]
    ; stack is aligned to 16 bytes
        push dword [eax + 4*4]
        push dword [eax + 3*4]
        push dword [eax + 2*4]
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 8*4]

%endmacro

%macro  Call_Asm_Stack9 1

; [esp + 9*4] = 9th parameter
; ...
; [esp +   4] = 1st parameter
; [esp      ] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 8 bytes on stack
        sub esp, byte 8
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 1*4], eax
    ; adjust stack for function argument, so that stack is aligned to 16 bytes before call
        add esp, byte 4

    ; push function arguments to stack
        push dword [eax + 9*4]
    ; stack is aligned to 16 bytes
        push dword [eax + 8*4]
        push dword [eax + 7*4]
        push dword [eax + 6*4]
        push dword [eax + 5*4]
    ; stack is aligned to 16 bytes
        push dword [eax + 4*4]
        push dword [eax + 3*4]
        push dword [eax + 2*4]
        push dword [eax +   4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 9*4]

%endmacro

%macro  Call_Asm_Stack12 1

; [esp + 12*4] = 12th parameter
; ...
; [esp +    4] = 1st parameter
; [esp       ] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 4 bytes on stack
        sub esp, byte 4
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp], eax

    ; push function arguments to stack
    ; stack is aligned to 16 bytes
        push dword [eax + 12*4]
        push dword [eax + 11*4]
        push dword [eax + 10*4]
        push dword [eax +  9*4]
    ; stack is aligned to 16 bytes
        push dword [eax +  8*4]
        push dword [eax +  7*4]
        push dword [eax +  6*4]
        push dword [eax +  5*4]
    ; stack is aligned to 16 bytes
        push dword [eax +  4*4]
        push dword [eax +  3*4]
        push dword [eax +  2*4]
        push dword [eax +    4]
    ; stack is aligned to 16 bytes

        call %1

    ; restore original esp value from stack
        mov esp, [esp + 12*4]

%endmacro

