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
    %define video_RegisterClass_PRE_Video _video_RegisterClass_PRE_Video
    %define video_Open_PRE_Video _video_Open_PRE_Video
    %define video_Close_PRE_Video _video_Close_PRE_Video
    %define video_Play_PRE_Video _video_Play_PRE_Video
    %define video_RegisterClass_POST_Video _video_RegisterClass_POST_Video
    %define video_Open_POST_Video _video_Open_POST_Video
    %define video_Close_POST_Video _video_Close_POST_Video
    %define video_Play_POST_Video _video_Play_POST_Video
    %define video_RegisterClass_SS_Video _video_RegisterClass_SS_Video
%endif

extern video_RegisterClass_PRE_Video
extern video_Open_PRE_Video
extern video_Close_PRE_Video
extern video_Play_PRE_Video
extern video_RegisterClass_POST_Video
extern video_Open_POST_Video
extern video_Close_POST_Video
extern video_Play_POST_Video
extern video_RegisterClass_SS_Video

global video_RegisterClass_PRE_Video_asm2c
global video_Open_PRE_Video_asm2c
global video_Close_PRE_Video_asm2c
global video_Play_PRE_Video_asm2c
global video_RegisterClass_POST_Video_asm2c
global video_Open_POST_Video_asm2c
global video_Close_POST_Video_asm2c
global video_Play_POST_Video_asm2c
global video_RegisterClass_SS_Video_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
video_RegisterClass_PRE_Video_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 video_RegisterClass_PRE_Video

        retn

; end procedure video_RegisterClass_PRE_Video_asm2c


align 16
video_Open_PRE_Video_asm2c:

; [esp +   4] = const char *path
; [esp      ] = return address

        Call_Asm_Stack1 video_Open_PRE_Video

        retn 4

; end procedure video_Open_PRE_Video_asm2c


align 16
video_Close_PRE_Video_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 video_Close_PRE_Video

        retn

; end procedure video_Close_PRE_Video_asm2c


align 16
video_Play_PRE_Video_asm2c:

; [esp +   4] = uint32_t zoomed
; [esp      ] = return address

        Call_Asm_Stack1 video_Play_PRE_Video

        retn 4

; end procedure video_Play_PRE_Video_asm2c


align 16
video_RegisterClass_POST_Video_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 video_RegisterClass_POST_Video

        retn

; end procedure video_RegisterClass_POST_Video_asm2c


align 16
video_Open_POST_Video_asm2c:

; [esp +   4] = const char *path
; [esp      ] = return address

        Call_Asm_Stack1 video_Open_POST_Video

        retn 4

; end procedure video_Open_POST_Video_asm2c


align 16
video_Close_POST_Video_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 video_Close_POST_Video

        retn

; end procedure video_Close_POST_Video_asm2c


align 16
video_Play_POST_Video_asm2c:

; [esp +   4] = uint32_t zoomed
; [esp      ] = return address

        Call_Asm_Stack1 video_Play_POST_Video

        retn 4

; end procedure video_Play_POST_Video_asm2c


align 16
video_RegisterClass_SS_Video_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 video_RegisterClass_SS_Video

        retn

; end procedure video_RegisterClass_SS_Video_asm2c

