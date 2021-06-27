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
    %define CreateFontIndirectA_c _CreateFontIndirectA_c
    %define DeleteObject_c _DeleteObject_c
    %define GdiFlush_c _GdiFlush_c
    %define GetDeviceCaps_c _GetDeviceCaps_c
    %define GetTextExtentPointA_c _GetTextExtentPointA_c
    %define SelectObject_c _SelectObject_c
    %define SetBkMode_c _SetBkMode_c
    %define TextOutA_c _TextOutA_c
%endif

extern CreateFontIndirectA_c
extern DeleteObject_c
extern GdiFlush_c
extern GetDeviceCaps_c
extern GetTextExtentPointA_c
extern SelectObject_c
extern SetBkMode_c
extern TextOutA_c

global CreateFontIndirectA_asm2c
global DeleteObject_asm2c
global GdiFlush_asm2c
global GetDeviceCaps_asm2c
global GetTextExtentPointA_asm2c
global SelectObject_asm2c
global SetBkMode_asm2c
global TextOutA_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
CreateFontIndirectA_asm2c:

; [esp +   4] = CONST LOGFONTA * lplf
; [esp      ] = return address

        Call_Asm_Stack1 CreateFontIndirectA_c

        retn 1*4

; end procedure CreateFontIndirectA_asm2c


align 16
DeleteObject_asm2c:

; [esp +   4] = HGDIOBJ ho
; [esp      ] = return address

        Call_Asm_Stack1 DeleteObject_c

        retn 1*4

; end procedure DeleteObject_asm2c


align 16
GdiFlush_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 GdiFlush_c

        retn

; end procedure GdiFlush_asm2c


align 16
GetDeviceCaps_asm2c:

; [esp + 2*4] = int index
; [esp +   4] = HDC hdc
; [esp      ] = return address

        Call_Asm_Stack2 GetDeviceCaps_c

        retn 2*4

; end procedure GetDeviceCaps_asm2c


align 16
GetTextExtentPointA_asm2c:

; [esp + 4*4] = LPSIZE lpsz
; [esp + 3*4] = int c
; [esp + 2*4] = LPCSTR lpString
; [esp +   4] = HDC hdc
; [esp      ] = return address

        Call_Asm_Stack4 GetTextExtentPointA_c

        retn 4*4

; end procedure GetTextExtentPointA_asm2c


align 16
SelectObject_asm2c:

; [esp + 2*4] = HGDIOBJ h
; [esp +   4] = HDC hdc
; [esp      ] = return address

        Call_Asm_Stack2 SelectObject_c

        retn 2*4

; end procedure SelectObject_asm2c


align 16
SetBkMode_asm2c:

; [esp + 2*4] = int mode
; [esp +   4] = HDC hdc
; [esp      ] = return address

        Call_Asm_Stack2 SetBkMode_c

        retn 2*4

; end procedure SetBkMode_asm2c


align 16
TextOutA_asm2c:

; [esp + 5*4] = int c
; [esp + 4*4] = LPCSTR lpString
; [esp + 3*4] = int y
; [esp + 2*4] = int x
; [esp +   4] = HDC hdc
; [esp      ] = return address

        Call_Asm_Stack5 TextOutA_c

        retn 5*4

; end procedure TextOutA_asm2c


