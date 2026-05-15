;;
;;  Copyright (C) 2019-2026 Roman Pauer
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

%include "x64inc.inc"
%include "asm-calls.inc"

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

%ifidn __OUTPUT_FORMAT__, elf64
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif
%ifidn __OUTPUT_FORMAT__, win64
section_prolog:
        SECTION_PROLOG
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

        Call_Asm_Stack9 BitBlt_c,9

; end procedure BitBlt_asm2c


align 16
CreateCompatibleDC_asm2c:

; [esp +   4] = HDC   hdc
; [esp      ] = return address

        Call_Asm_Stack1 CreateCompatibleDC_c,1

; end procedure CreateCompatibleDC_asm2c


align 16
CreateFontIndirectA_asm2c:

; [esp +   4] = const LOGFONTA *lplf
; [esp      ] = return address

        Call_Asm_Stack1 CreateFontIndirectA_c,1

; end procedure CreateFontIndirectA_asm2c


align 16
CreatePolygonRgn_asm2c:

; [esp + 3*4] = int         iMode
; [esp + 2*4] = int         cPoint
; [esp +   4] = const POINT *pptl
; [esp      ] = return address

        Call_Asm_Stack3 CreatePolygonRgn_c,3

; end procedure CreatePolygonRgn_asm2c


align 16
CreateSolidBrush_asm2c:

; [esp +   4] = COLORREF color
; [esp      ] = return address

        Call_Asm_Stack1 CreateSolidBrush_c,1

; end procedure CreateSolidBrush_asm2c


align 16
DeleteDC_asm2c:

; [esp +   4] = HDC hdc
; [esp      ] = return address

        Call_Asm_Stack1 DeleteDC_c,1

; end procedure DeleteDC_asm2c


align 16
DeleteObject_asm2c:

; [esp +   4] = HGDIOBJ ho
; [esp      ] = return address

        Call_Asm_Stack1 DeleteObject_c,1

; end procedure DeleteObject_asm2c


align 16
FillRgn_asm2c:

; [esp + 3*4] = HBRUSH hbr
; [esp + 2*4] = HRGN   hrgn
; [esp +   4] = HDC    hdc
; [esp      ] = return address

        Call_Asm_Stack3 FillRgn_c,3

; end procedure FillRgn_asm2c


align 16
GetStockObject_asm2c:

; [esp +   4] = int i
; [esp      ] = return address

        Call_Asm_Stack1 GetStockObject_c,1

; end procedure GetStockObject_asm2c


align 16
OffsetRgn_asm2c:

; [esp + 3*4] = int  y
; [esp + 2*4] = int  x
; [esp +   4] = HRGN hrgn
; [esp      ] = return address

        Call_Asm_Stack3 OffsetRgn_c,3

; end procedure OffsetRgn_asm2c


align 16
SelectObject_asm2c:

; [esp + 2*4] = HGDIOBJ h
; [esp +   4] = HDC     hdc
; [esp      ] = return address

        Call_Asm_Stack2 SelectObject_c,2

; end procedure SelectObject_asm2c


align 16
SetBkMode_asm2c:

; [esp + 2*4] = int mode
; [esp +   4] = HDC hdc
; [esp      ] = return address

        Call_Asm_Stack2 SetBkMode_c,2

; end procedure SetBkMode_asm2c


align 16
SetTextColor_asm2c:

; [esp + 2*4] = COLORREF color
; [esp +   4] = HDC      hdc
; [esp      ] = return address

        Call_Asm_Stack2 SetTextColor_c,2

; end procedure SetTextColor_asm2c


align 16
TextOutA_asm2c:

; [esp + 5*4] = int    c
; [esp + 4*4] = LPCSTR lpString
; [esp + 3*4] = int    y
; [esp + 2*4] = int    x
; [esp +   4] = HDC    hdc
; [esp      ] = return address

        Call_Asm_Stack5 TextOutA_c,5

; end procedure TextOutA_asm2c


%ifidn __OUTPUT_FORMAT__, win64
section_end:

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
%endif
