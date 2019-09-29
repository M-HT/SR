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
    %define ASI_startup_c _ASI_startup_c
    %define ASI_shutdown_c _ASI_shutdown_c
    %define ASI_stream_close_c _ASI_stream_close_c
    %define ASI_stream_process_c _ASI_stream_process_c
%endif

extern ASI_startup_c
extern ASI_shutdown_c
extern ASI_stream_close_c
extern ASI_stream_process_c


global ASI_startup_asm2c
global ASI_shutdown_asm2c
global ASI_stream_close_asm2c
global ASI_stream_process_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
ASI_startup_asm2c:

; [esp] = return address

        Call_Asm_Stack0 ASI_startup_c

        retn

; end procedure ASI_startup_asm2c


align 16
ASI_shutdown_asm2c:

; [esp] = return address

        Call_Asm_Stack0 ASI_shutdown_c

        retn

; end procedure ASI_shutdown_asm2c


align 16
ASI_stream_close_asm2c:

; [esp + 4] = void *stream
; [esp    ] = return address


        Call_Asm_Stack1 ASI_stream_close_c

        retn 4

; end procedure ASI_stream_close_asm2c


align 16
ASI_stream_process_asm2c:

; [esp + 3*4] = int32_t request_size
; [esp + 2*4] = void *buffer
; [esp +   4] = void *stream
; [esp      ] = return address


        Call_Asm_Stack3 ASI_stream_process_c

        retn 3*4

; end procedure ASI_stream_process_asm2c

