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
    %define RNG_Init_c _RNG_Init_c
    %define RNG_Exit_c _RNG_Exit_c
    %define RNG_NewBuffer_c _RNG_NewBuffer_c
    %define RNG_DelBuffer_c _RNG_DelBuffer_c
    %define RNG_In_c _RNG_In_c
    %define RNG_Out_c _RNG_Out_c
    %define RNG_Peek_c _RNG_Peek_c
    %define RNG_Replace_c _RNG_Replace_c
    %define RNG_PutFirst_c _RNG_PutFirst_c
%endif

extern RNG_Init_c
extern RNG_Exit_c
extern RNG_NewBuffer_c
extern RNG_DelBuffer_c
extern RNG_In_c
extern RNG_Out_c
extern RNG_Peek_c
extern RNG_Replace_c
extern RNG_PutFirst_c

global RNG_Init
global RNG_Exit
global RNG_NewBuffer
global RNG_DelBuffer
global RNG_In
global RNG_Out
global RNG_Peek
global RNG_Replace
global RNG_PutFirst

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
RNG_Init:

; [esp      ] = return address

        Call_Asm_Stack0 RNG_Init_c

        retn

; end procedure RNG_Init


align 16
RNG_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 RNG_Exit_c

        retn

; end procedure RNG_Exit


align 16
RNG_NewBuffer:

; [esp + 2*4] = int num_elements
; [esp +   4] = int element_size
; [esp      ] = return address

        Call_Asm_Stack2 RNG_NewBuffer_c

        retn

; end procedure RNG_NewBuffer


align 16
RNG_DelBuffer:

; [esp +   4] = void * buffer
; [esp      ] = return address

        Call_Asm_Stack1 RNG_DelBuffer_c

        retn

; end procedure RNG_DelBuffer


align 16
RNG_In:

; [esp + 2*4] = const void * element
; [esp +   4] = void * buffer
; [esp      ] = return address

        Call_Asm_Stack2 RNG_In_c

        retn

; end procedure RNG_In


align 16
RNG_Out:

; [esp + 2*4] = void * element
; [esp +   4] = void * buffer
; [esp      ] = return address

        Call_Asm_Stack2 RNG_Out_c

        retn

; end procedure RNG_Out


align 16
RNG_Peek:

; [esp + 2*4] = void * element
; [esp +   4] = void * buffer
; [esp      ] = return address

        Call_Asm_Stack2 RNG_Peek_c

        retn

; end procedure RNG_Peek


align 16
RNG_Replace:

; [esp + 2*4] = const void * element
; [esp +   4] = void * buffer
; [esp      ] = return address

        Call_Asm_Stack2 RNG_Replace_c

        retn

; end procedure RNG_Replace


align 16
RNG_PutFirst:

; [esp + 2*4] = const void * element
; [esp +   4] = void * buffer
; [esp      ] = return address

        Call_Asm_Stack2 RNG_PutFirst_c

        retn

; end procedure RNG_PutFirst


