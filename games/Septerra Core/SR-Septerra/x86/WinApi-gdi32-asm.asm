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

%include "asm-calls.inc"

%ifidn __OUTPUT_FORMAT__, win32
    %define BitBlt_c _BitBlt_c
    %define CreateCompatibleDC_c _CreateCompatibleDC_c
    %define CreateFontIndirectA_c _CreateFontIndirectA_c
    %define CreatePolygonRgn_c _CreatePolygonRgn_c
    %define CreateSolidBrush_c _CreateSolidBrush_c
    %define DeleteDC_c _DeleteDC_c
    %define DeleteObject_c _DeleteObject_c
    %define FillRgn_c _FillRgn_c
    %define GetStockObject_c _GetStockObject_c
    %define OffsetRgn_c _OffsetRgn_c
    %define SelectObject_c _SelectObject_c
    %define SetBkMode_c _SetBkMode_c
    %define SetTextColor_c _SetTextColor_c
    %define TextOutA_c _TextOutA_c
%endif

extern BitBlt_c
extern CreateCompatibleDC_c
extern CreateFontIndirectA_c
extern CreatePolygonRgn_c
extern CreateSolidBrush_c
extern DeleteDC_c
extern DeleteObject_c
extern FillRgn_c
extern GetStockObject_c
extern OffsetRgn_c
extern SelectObject_c
extern SetBkMode_c
extern SetTextColor_c
extern TextOutA_c

global BitBlt_asm2c
global CreateCompatibleDC_asm2c
global CreateFontIndirectA_asm2c
global CreatePolygonRgn_asm2c
global CreateSolidBrush_asm2c
global DeleteDC_asm2c
global DeleteObject_asm2c
global FillRgn_asm2c
global GetStockObject_asm2c
global OffsetRgn_asm2c
global SelectObject_asm2c
global SetBkMode_asm2c
global SetTextColor_asm2c
global TextOutA_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
BitBlt_asm2c:

; [esp + 9*4] = DWORD rop
; [esp + 8*4] = int   y1
; [esp + 7*4] = int   x1
; [esp + 6*4] = HDC   hdcSrc
; [esp + 5*4] = int   cy
; [esp + 4*4] = int   cx
; [esp + 3*4] = int   y
; [esp + 2*4] = int   x
; [esp +   4] = HDC   hdc
; [esp      ] = return address

        Call_Asm_Stack9 BitBlt_c

        retn 9*4

; end procedure BitBlt_asm2c


align 16
CreateCompatibleDC_asm2c:

; [esp +   4] = HDC   hdc
; [esp      ] = return address

        Call_Asm_Stack1 CreateCompatibleDC_c

        retn 4

; end procedure CreateCompatibleDC_asm2c


align 16
CreateFontIndirectA_asm2c:

; [esp +   4] = const LOGFONTA *lplf
; [esp      ] = return address

        Call_Asm_Stack1 CreateFontIndirectA_c

        retn 4

; end procedure CreateFontIndirectA_asm2c


align 16
CreatePolygonRgn_asm2c:

; [esp + 3*4] = int         iMode
; [esp + 2*4] = int         cPoint
; [esp +   4] = const POINT *pptl
; [esp      ] = return address

        Call_Asm_Stack3 CreatePolygonRgn_c

        retn 3*4

; end procedure CreatePolygonRgn_asm2c


align 16
CreateSolidBrush_asm2c:

; [esp +   4] = COLORREF color
; [esp      ] = return address

        Call_Asm_Stack1 CreateSolidBrush_c

        retn 4

; end procedure CreateSolidBrush_asm2c


align 16
DeleteDC_asm2c:

; [esp +   4] = HDC hdc
; [esp      ] = return address

        Call_Asm_Stack1 DeleteDC_c

        retn 4

; end procedure DeleteDC_asm2c


align 16
DeleteObject_asm2c:

; [esp +   4] = HGDIOBJ ho
; [esp      ] = return address

        Call_Asm_Stack1 DeleteObject_c

        retn 4

; end procedure DeleteObject_asm2c


align 16
FillRgn_asm2c:

; [esp + 3*4] = HBRUSH hbr
; [esp + 2*4] = HRGN   hrgn
; [esp +   4] = HDC    hdc
; [esp      ] = return address

        Call_Asm_Stack3 FillRgn_c

        retn 3*4

; end procedure FillRgn_asm2c


align 16
GetStockObject_asm2c:

; [esp +   4] = int i
; [esp      ] = return address

        Call_Asm_Stack1 GetStockObject_c

        retn 4

; end procedure GetStockObject_asm2c


align 16
OffsetRgn_asm2c:

; [esp + 3*4] = int  y
; [esp + 2*4] = int  x
; [esp +   4] = HRGN hrgn
; [esp      ] = return address

        Call_Asm_Stack3 OffsetRgn_c

        retn 3*4

; end procedure OffsetRgn_asm2c


align 16
SelectObject_asm2c:

; [esp + 2*4] = HGDIOBJ h
; [esp +   4] = HDC     hdc
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
SetTextColor_asm2c:

; [esp + 2*4] = COLORREF color
; [esp +   4] = HDC      hdc
; [esp      ] = return address

        Call_Asm_Stack2 SetTextColor_c

        retn 2*4

; end procedure SetTextColor_asm2c


align 16
TextOutA_asm2c:

; [esp + 5*4] = int    c
; [esp + 4*4] = LPCSTR lpString
; [esp + 3*4] = int    y
; [esp + 2*4] = int    x
; [esp +   4] = HDC    hdc
; [esp      ] = return address

        Call_Asm_Stack5 TextOutA_c

        retn 5*4

; end procedure TextOutA_asm2c

