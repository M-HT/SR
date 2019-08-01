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
    %define X86_InPortProcedure _X86_InPortProcedure
    %define X86_OutPortProcedure _X86_OutPortProcedure
%endif

extern X86_InPortProcedure
extern X86_OutPortProcedure

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

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

align 16
x86_in_al_imm:

; [esp +    4] = port number
; [esp       ] = return address

        pushfd
        pushad

; [esp + 10*4] = port number
; [esp +  9*4] = return address

        push byte 1				; register length
        push dword [esp + 4*11]	; port number

        call X86_InPortProcedure

        add esp, byte 4*2

        mov [esp + 4*7], al

        popad
        popfd

        retn 4

; end procedure x86_in_al_imm

align 16
x86_in_ax_imm:

; [esp +    4] = port number
; [esp       ] = return address

        pushfd
        pushad

; [esp + 10*4] = port number
; [esp +  9*4] = return address

        push byte 2				; register length
        push dword [esp + 4*11]	; port number

        call X86_InPortProcedure

        add esp, byte 4*2

        mov [esp + 4*7], ax

        popad
        popfd

        retn 4

; end procedure x86_in_ax_imm

align 16
x86_in_eax_imm:

; [esp +    4] = port number
; [esp       ] = return address

        pushfd
        pushad

; [esp + 10*4] = port number
; [esp +  9*4] = return address

        push byte 4				; register length
        push dword [esp + 4*11]	; port number

        call X86_InPortProcedure

        add esp, byte 4*2

        mov [esp + 4*7], eax

        popad
        popfd

        retn 4

; end procedure x86_in_eax_imm

align 16
x86_in_al_dx:

; [esp       ] = return address

        pushfd
        pushad

; [esp +  9*4] = return address

        push byte 1				; register length
        push edx				; port number

        call X86_InPortProcedure

        add esp, byte 4*2

        mov [esp + 4*7], al

        popad
        popfd

        retn

; end procedure x86_in_al_dx

align 16
x86_in_ax_dx:

; [esp       ] = return address

        pushfd
        pushad

; [esp +  9*4] = return address

        push byte 2				; register length
        push edx				; port number

        call X86_InPortProcedure

        add esp, byte 4*2

        mov [esp + 4*7], ax

        popad
        popfd

        retn

; end procedure x86_in_ax_dx

align 16
x86_in_eax_dx:

; [esp       ] = return address

        pushfd
        pushad

; [esp +  9*4] = return address

        push byte 4				; register length
        push edx				; port number

        call X86_InPortProcedure

        add esp, byte 4*2

        mov [esp + 4*7], eax

        popad
        popfd

        retn

; end procedure x86_in_eax_dx

align 16
x86_out_imm_al:

; [esp +    4] = port number
; [esp       ] = return address

        pushfd
        pushad

; [esp + 10*4] = port number
; [esp +  9*4] = return address

        push eax
        push byte 1				; register length
        push dword [esp + 4*12]	; port number

        call X86_OutPortProcedure

        add esp, byte 4*3

        popad
        popfd

        retn 4

; end procedure x86_out_imm_al

align 16
x86_out_imm_ax:

; [esp +    4] = port number
; [esp       ] = return address

        pushfd
        pushad

; [esp + 10*4] = port number
; [esp +  9*4] = return address

        push eax
        push byte 2				; register length
        push dword [esp + 4*12]	; port number

        call X86_OutPortProcedure

        add esp, byte 4*3

        popad
        popfd

        retn 4

; end procedure x86_out_imm_ax

align 16
x86_out_imm_eax:

; [esp +    4] = port number
; [esp       ] = return address

        pushfd
        pushad

; [esp + 10*4] = port number
; [esp +  9*4] = return address

        push eax
        push byte 4				; register length
        push dword [esp + 4*12]	; port number

        call X86_OutPortProcedure

        add esp, byte 4*3

        popad
        popfd

        retn 4

; end procedure x86_out_imm_eax

align 16
x86_out_dx_al:

; [esp       ] = return address

        pushfd
        pushad

; [esp +  9*4] = return address

        push eax
        push byte 1				; register length
        push edx				; port number

        call X86_OutPortProcedure

        add esp, byte 4*3

        popad
        popfd

        retn

; end procedure x86_out_dx_al

align 16
x86_out_dx_ax:

; [esp       ] = return address

        pushfd
        pushad

; [esp    9*4] = return address

        push eax
        push byte 2				; register length
        push edx				; port number

        call X86_OutPortProcedure

        add esp, byte 4*3

        popad
        popfd

        retn

; end procedure x86_out_dx_ax

align 16
x86_out_dx_eax:

; [esp       ] = return address

        pushfd
        pushad

; [esp +  9*4] = return address

        push eax
        push byte 4				; register length
        push edx				; port number

        call X86_OutPortProcedure

        add esp, byte 4*3

        popad
        popfd

        retn

; end procedure x86_out_dx_eax

