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
    %define FGT_SystemTask_End _FGT_SystemTask_End
    %define FGT_CheckTicksDelay _FGT_CheckTicksDelay
%endif

extern FGT_SystemTask_End
extern FGT_CheckTicksDelay

global FGT_SystemTask_End_asm2c
global FGT_CheckTicksDelay_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
FGT_SystemTask_End_asm2c:

; [esp +   4] = int flushGdi
; [esp      ] = return address

        Call_Asm_Stack1 FGT_SystemTask_End

        retn 4

; end procedure FGT_SystemTask_End_asm2c


align 16
FGT_CheckTicksDelay_asm2c:

; [esp +   4] = int index
; [esp      ] = return address

        Call_Asm_Stack1 FGT_CheckTicksDelay

        retn 4

; end procedure FGT_CheckTicksDelay_asm2c

