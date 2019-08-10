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

%include "misc.inc"

%ifidn __OUTPUT_FORMAT__, win32
    %define Game_acos2 _Game_acos2
    %define Game_asin2 _Game_asin2
%endif

extern Game_acos2
extern Game_asin2


global SR_acos2
global SR_asin2
global SR_asin2_2


%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
SR_acos2:

; st0   = x
; [esp] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 16 bytes on stack
        sub esp, byte 16
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; save original esp value on stack
        mov [esp + 4], eax
        lea eax, [esp + 8]
        fstp qword [eax]
        mov [esp], eax
    ; stack is aligned to 16 bytes

        call Game_acos2

        fld qword [esp+8]

    ; restore original esp value from stack
        mov esp, [esp + 4]

        retn

; end procedure SR_acos2

align 16
SR_asin2:

; st0   = x
; [esp] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 16 bytes on stack
        sub esp, byte 16
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; save original esp value on stack
        mov [esp + 4], eax
        lea eax, [esp + 8]
        fstp qword [eax]
        mov [esp], eax
    ; stack is aligned to 16 bytes

        call Game_asin2

        fld qword [esp+8]

    ; restore original esp value from stack
        mov esp, [esp + 4]

        retn

; end procedure SR_asin2

align 16
SR_asin2_2:

; st1   = y
; st0   = x
; [esp] = return address

    ; remember original esp value
        mov eax, esp
    ; reserve 24 bytes on stack
        sub esp, byte 24
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; save original esp value on stack
        mov [esp + 4], eax
        lea eax, [esp + 8]
        fstp qword [eax]
        fstp qword [eax+8]
        mov [esp], eax
    ; stack is aligned to 16 bytes

        call Game_asin2

        fld qword [esp+16]
        fld qword [esp+8]

    ; restore original esp value from stack
        mov esp, [esp + 4]

        retn

; end procedure SR_asin2_2

