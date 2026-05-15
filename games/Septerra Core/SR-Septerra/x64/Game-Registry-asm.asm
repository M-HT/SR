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

extern Registry_SetValueDword
extern Registry_GetValueDword
extern Registry_GetValueString

global Registry_SetValueDword_asm2c
global Registry_GetValueDword_asm2c
global Registry_GetValueString_asm2c

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
Registry_SetValueDword_asm2c:

; [esp + 2*4] = uint32_t Value
; [esp +   4] = const char *ValueName
; [esp      ] = return address

        Call_Asm_Stack2 Registry_SetValueDword

; end procedure Registry_SetValueDword_asm2c


align 16
Registry_GetValueDword_asm2c:

; [esp + 2*4] = uint32_t *Value
; [esp +   4] = const char *ValueName
; [esp      ] = return address

        Call_Asm_Stack2 Registry_GetValueDword

; end procedure Registry_GetValueDword_asm2c


align 16
Registry_GetValueString_asm2c:

; [esp + 2*4] = char *Value
; [esp +   4] = const char *ValueName
; [esp      ] = return address

        Call_Asm_Stack2 Registry_GetValueString

; end procedure Registry_GetValueString_asm2c


%ifidn __OUTPUT_FORMAT__, win64
section_end:

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
%endif
