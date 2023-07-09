;part of static recompiler -- do not edit

;;
;;  Copyright (C) 2016-2019 Roman Pauer
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

%ifidn __OUTPUT_FORMAT__, win32
    %define X86_InterruptProcedure _X86_InterruptProcedure
%endif

extern X86_InterruptProcedure

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

global x86_int

align 16
x86_int:

; [esp +    4] = interrupt number
; [esp       ] = return address

        pushfd
        pushad

; [esp + 10*4] = interrupt number
; [esp +  9*4] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 8 bytes on stack
        sub esp, byte 8
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; adjust stack for function arguments, so that stack is aligned to 16 bytes before call
        add esp, byte 8
    ; push function arguments to stack
        push eax
        push dword [eax + 10*4] ; interrupt number
    ; stack is aligned to 16 bytes

        call X86_InterruptProcedure

    ; restore original esp value from stack
        mov esp, [esp + 1*4]

        popad
        popfd

        retn 4

; end procedure x86_int

