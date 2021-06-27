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
    %define midi_OpenSDIMusic _midi_OpenSDIMusic
    %define midi_GetSDIMusicID _midi_GetSDIMusicID
    %define midi_PlaySDIMusic _midi_PlaySDIMusic
    %define midi_CloseSDIMusic _midi_CloseSDIMusic
    %define midi_IsPlaying _midi_IsPlaying
    %define midi_OpenTestMusic _midi_OpenTestMusic
    %define midi_PlayTestMusic _midi_PlayTestMusic
    %define midi_CloseTestMusic _midi_CloseTestMusic
    %define midi_GetErrorString _midi_GetErrorString
%endif

extern midi_OpenSDIMusic
extern midi_GetSDIMusicID
extern midi_PlaySDIMusic
extern midi_CloseSDIMusic
extern midi_IsPlaying
extern midi_OpenTestMusic
extern midi_PlayTestMusic
extern midi_CloseTestMusic
extern midi_GetErrorString

global midi_OpenSDIMusic_asm2c
global midi_GetSDIMusicID_asm2c
global midi_PlaySDIMusic_asm2c
global midi_CloseSDIMusic_asm2c
global midi_IsPlaying_asm2c
global midi_OpenTestMusic_asm2c
global midi_PlayTestMusic_asm2c
global midi_CloseTestMusic_asm2c
global midi_GetErrorString_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
midi_OpenSDIMusic_asm2c:

; [esp +   4] = const char *filename
; [esp      ] = return address

        Call_Asm_Stack1 midi_OpenSDIMusic

        retn 4

; end procedure midi_OpenSDIMusic_asm2c


align 16
midi_GetSDIMusicID_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 midi_GetSDIMusicID

        retn

; end procedure midi_GetSDIMusicID_asm2c


align 16
midi_PlaySDIMusic_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 midi_PlaySDIMusic

        retn

; end procedure midi_PlaySDIMusic_asm2c


align 16
midi_CloseSDIMusic_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 midi_CloseSDIMusic

        retn

; end procedure midi_CloseSDIMusic_asm2c


align 16
midi_IsPlaying_asm2c:

; [esp +   4] = unsigned int musicID
; [esp      ] = return address

        Call_Asm_Stack1 midi_IsPlaying

        retn 4

; end procedure midi_IsPlaying_asm2c


align 16
midi_OpenTestMusic_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 midi_OpenTestMusic

        retn

; end procedure midi_OpenTestMusic_asm2c


align 16
midi_PlayTestMusic_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 midi_PlayTestMusic

        retn

; end procedure midi_PlayTestMusic_asm2c


align 16
midi_CloseTestMusic_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 midi_CloseTestMusic

        retn

; end procedure midi_CloseTestMusic_asm2c


align 16
midi_GetErrorString_asm2c:

; [esp + 3*4] = unsigned int length
; [esp + 2*4] = char *text
; [esp +   4] = int error
; [esp      ] = return address

        Call_Asm_Stack3 midi_GetErrorString

        retn 3*4

; end procedure midi_GetErrorString_asm2c

