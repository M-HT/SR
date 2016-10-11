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

%include "misc.inc"

%ifndef ELF
    %define draw_3dscene _draw_3dscene
%endif

extern draw_3dscene

extern loc_8B6BB

global draw_3dscene_proc
global _draw_3dscene_proc

global sub_8B6BB
global _sub_8B6BB

%ifdef ELF
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
draw_3dscene_proc:
_draw_3dscene_proc:

; [esp      ] = return address


        push eax
        push ecx
        push edx

; [esp + 3*4] = return address

        call draw_3dscene

        pop edx
        pop ecx
        pop eax

        retn

; end procedure draw_3dscene_proc


align 16
sub_8B6BB:
_sub_8B6BB:

; [esp +   4] = handle
; [esp      ] = return address


        push ebx
        push esi
        push edi
        push ebp

; [esp + 5*4] = handle
; [esp + 4*4] = return address

        mov eax, [esp + 5*4]

        call loc_8B6BB

        pop ebp
        pop edi
        pop esi
        pop ebx

        retn

; end procedure sub_8B6BB
