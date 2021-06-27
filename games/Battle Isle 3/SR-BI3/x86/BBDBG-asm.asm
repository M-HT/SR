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
    %define DBG_Panic_c _DBG_Panic_c
    %define DBG_Init_c _DBG_Init_c
    %define DBG_Exit_c _DBG_Exit_c
%endif

extern DBG_Panic_c
extern DBG_Init_c
extern DBG_Exit_c

global DBG_Panic
global DBG_Init
global DBG_Exit

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
DBG_Panic:

; [esp + 2*4] = int line
; [esp +   4] = const char * module
; [esp      ] = return address

        Call_Asm_Stack2 DBG_Panic_c

        retn

; end procedure DBG_Panic


align 16
DBG_Init:

; [esp      ] = return address

        Call_Asm_Stack0 DBG_Init_c

        retn

; end procedure DBG_Init


align 16
DBG_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 DBG_Exit_c

        retn

; end procedure DBG_Exit


