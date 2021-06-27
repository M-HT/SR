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
    %define AVI_Init_c _AVI_Init_c
    %define AVI_Exit_c _AVI_Exit_c
    %define AVI_SetDestortionLevel_c _AVI_SetDestortionLevel_c
    %define AVI_OpenVideo_c _AVI_OpenVideo_c
    %define AVI_CloseVideo_c _AVI_CloseVideo_c
    %define AVI_PlayVideo_c _AVI_PlayVideo_c
%endif

extern AVI_Init_c
extern AVI_Exit_c
extern AVI_SetDestortionLevel_c
extern AVI_OpenVideo_c
extern AVI_CloseVideo_c
extern AVI_PlayVideo_c

global AVI_Init
global AVI_Exit
global AVI_SetDestortionLevel
global AVI_OpenVideo
global AVI_CloseVideo
global AVI_PlayVideo

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
AVI_Init:

; [esp      ] = return address

        Call_Asm_Stack0 AVI_Init_c

        retn

; end procedure AVI_Init


align 16
AVI_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 AVI_Exit_c

        retn

; end procedure AVI_Exit


align 16
AVI_SetDestortionLevel:

; [esp +   4] = int destortionLevel
; [esp      ] = return address

        Call_Asm_Stack1 AVI_SetDestortionLevel_c

        retn

; end procedure AVI_SetDestortionLevel


align 16
AVI_OpenVideo:

; [esp + 2*4] = const uint8_t * param2
; [esp +   4] = const char * path
; [esp      ] = return address

        Call_Asm_Stack2 AVI_OpenVideo_c

        retn

; end procedure AVI_OpenVideo


align 16
AVI_CloseVideo:

; [esp +   4] = void * video
; [esp      ] = return address

        Call_Asm_Stack1 AVI_CloseVideo_c

        retn

; end procedure AVI_CloseVideo


align 16
AVI_PlayVideo:

; [esp + 7*4] = unsigned int flags
; [esp + 6*4] = int volume
; [esp + 5*4] = int param5
; [esp + 4*4] = int param4
; [esp + 3*4] = int y
; [esp + 2*4] = int x
; [esp +   4] = void * video
; [esp      ] = return address

        Call_Asm_Stack7 AVI_PlayVideo_c

        retn

; end procedure AVI_PlayVideo


