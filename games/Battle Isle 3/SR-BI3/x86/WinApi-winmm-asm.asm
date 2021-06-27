;;
;;  Copyright (C) 2021 Roman Pauer
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

%include "asm-calls.inc"

%ifidn __OUTPUT_FORMAT__, win32
    %define mciGetErrorStringA_c _mciGetErrorStringA_c
    %define mciSendCommandA_c _mciSendCommandA_c
%endif

extern mciGetErrorStringA_c
extern mciSendCommandA_c

global mciGetErrorStringA_asm2c
global mciSendCommandA_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
mciGetErrorStringA_asm2c:

; [esp + 3*4] = UINT cchText
; [esp + 2*4] = LPSTR pszText
; [esp +   4] = MCIERROR mcierr
; [esp      ] = return address

        Call_Asm_Stack3 mciGetErrorStringA_c

        retn 3*4

; end procedure mciGetErrorStringA_asm2c


align 16
mciSendCommandA_asm2c:

; [esp + 4*4] = DWORD_PTR dwParam2
; [esp + 3*4] = DWORD_PTR dwParam1
; [esp + 2*4] = UINT uMsg
; [esp +   4] = MCIDEVICEID mciId
; [esp      ] = return address

        Call_Asm_Stack4 mciSendCommandA_c

        retn 4*4

; end procedure mciSendCommandA_asm2c


