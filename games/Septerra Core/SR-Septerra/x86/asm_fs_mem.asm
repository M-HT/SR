;part of static recompiler -- do not edit

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

%ifidn __OUTPUT_FORMAT__, win32
    %define X86_ReadFsDword _X86_ReadFsDword
    %define X86_WriteFsDword _X86_WriteFsDword
%endif

extern X86_ReadFsDword
extern X86_WriteFsDword

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

global x86_read_fs_dword
global x86_write_fs_dword

align 16
x86_read_fs_dword:

; [esp + 4] = uint32_t addr
; [esp    ] = return address

        pushfd
        push eax
        push ecx
        push edx

; [esp + 5*4] = uint32_t addr
; [esp   4*4] = return address

    ; remember original esp value
        mov eax, esp
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        push eax
    ; add dummy values to stack, so that stack is aligned to 16 bytes before call
        sub esp, byte 2*4

    ; push function arguments to stack
        push dword [eax + 5*4]
    ; stack is aligned to 16 bytes

        call X86_ReadFsDword

    ; restore original esp value from stack
        mov esp, [esp + 3*4]

        mov [esp + 5*4], eax

; [esp + 5*4] = uint32_t value
; [esp   4*4] = return address

        pop edx
        pop ecx
        pop eax
        popfd

        retn

; end procedure x86_read_fs_dword


align 16
x86_write_fs_dword:

; [esp + 2*4] = uint32_t value
; [esp +   4] = uint32_t addr
; [esp      ] = return address

        pushfd
        push eax
        push ecx
        push edx

; [esp + 6*4] = uint32_t value
; [esp + 5*4] = uint32_t addr
; [esp + 4*4] = return address

    ; remember original esp value
        mov eax, esp
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        push eax
    ; add dummy values to stack, so that stack is aligned to 16 bytes before call
        sub esp, byte 4

    ; push function arguments to stack
        push dword [eax + 6*4]
        push dword [eax + 5*4]
    ; stack is aligned to 16 bytes

        call X86_WriteFsDword

    ; restore original esp value from stack
        mov esp, [esp + 3*4]

        pop edx
        pop ecx
        pop eax
        popfd

        retn 2*4

; end procedure x86_write_fs_dword


