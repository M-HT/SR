;;
;;  Copyright (C) 2020-2021 Roman Pauer
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
    %define TXT_Init_c _TXT_Init_c
    %define TXT_Exit_c _TXT_Exit_c
    %define TXT_LoadFont_c _TXT_LoadFont_c
    %define TXT_WriteString_c _TXT_WriteString_c
    %define TXT_UnloadAllFonts_c _TXT_UnloadAllFonts_c
%endif

extern TXT_Init_c
extern TXT_Exit_c
extern TXT_LoadFont_c
extern TXT_WriteString_c
extern TXT_UnloadAllFonts_c

global TXT_Init
global TXT_Exit
global TXT_LoadFont
global TXT_WriteString
global TXT_UnloadAllFonts

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
TXT_Init:

; [esp      ] = return address

        Call_Asm_Stack0 TXT_Init_c

        retn

; end procedure TXT_Init


align 16
TXT_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 TXT_Exit_c

        retn

; end procedure TXT_Exit


align 16
TXT_LoadFont:

; [esp +   4] = const char * path
; [esp      ] = return address

        Call_Asm_Stack1 TXT_LoadFont_c

        retn

; end procedure TXT_LoadFont


align 16
TXT_WriteString:

; [esp + 8*4] = uint8_t color_add
; [esp + 7*4] = int height
; [esp + 6*4] = int width
; [esp + 5*4] = int y
; [esp + 4*4] = int x
; [esp + 3*4] = void * dst_pixel_map
; [esp + 2*4] = int font_handle
; [esp +   4] = const char * text
; [esp      ] = return address

        Call_Asm_Stack8 TXT_WriteString_c

        retn

; end procedure TXT_WriteString


align 16
TXT_UnloadAllFonts:

; [esp      ] = return address

        Call_Asm_Stack0 TXT_UnloadAllFonts_c

        retn

; end procedure TXT_UnloadAllFonts


