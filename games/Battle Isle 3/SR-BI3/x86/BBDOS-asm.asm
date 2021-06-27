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
    %define DOS_Init_c _DOS_Init_c
    %define DOS_Exit_c _DOS_Exit_c
    %define DOS_Open_c _DOS_Open_c
    %define DOS_Close_c _DOS_Close_c
    %define DOS_Read_c _DOS_Read_c
    %define DOS_Write_c _DOS_Write_c
    %define DOS_Seek_c _DOS_Seek_c
    %define DOS_ReadFile_c _DOS_ReadFile_c
    %define DOS_GetFileLength_c _DOS_GetFileLength_c
%endif

extern DOS_Init_c
extern DOS_Exit_c
extern DOS_Open_c
extern DOS_Close_c
extern DOS_Read_c
extern DOS_Write_c
extern DOS_Seek_c
extern DOS_ReadFile_c
extern DOS_GetFileLength_c

global DOS_Init
global DOS_Exit
global DOS_Open
global DOS_Close
global DOS_Read
global DOS_Write
global DOS_Seek
global DOS_ReadFile
global DOS_GetFileLength

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
DOS_Init:

; [esp      ] = return address

        Call_Asm_Stack0 DOS_Init_c

        retn

; end procedure DOS_Init


align 16
DOS_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 DOS_Exit_c

        retn

; end procedure DOS_Exit


align 16
DOS_Open:

; [esp + 2*4] = unsigned int mode
; [esp +   4] = const char * path
; [esp      ] = return address

        Call_Asm_Stack2 DOS_Open_c

        retn

; end procedure DOS_Open


align 16
DOS_Close:

; [esp +   4] = int file_handle
; [esp      ] = return address

        Call_Asm_Stack1 DOS_Close_c

        retn

; end procedure DOS_Close


align 16
DOS_Read:

; [esp + 3*4] = unsigned int length
; [esp + 2*4] = void * buffer
; [esp +   4] = int file_handle
; [esp      ] = return address

        Call_Asm_Stack3 DOS_Read_c

        retn

; end procedure DOS_Read


align 16
DOS_Write:

; [esp + 3*4] = unsigned int length
; [esp + 2*4] = const void * buffer
; [esp +   4] = int file_handle
; [esp      ] = return address

        Call_Asm_Stack3 DOS_Write_c

        retn

; end procedure DOS_Write


align 16
DOS_Seek:

; [esp + 3*4] = int offset
; [esp + 2*4] = int origin
; [esp +   4] = int file_handle
; [esp      ] = return address

        Call_Asm_Stack3 DOS_Seek_c

        retn

; end procedure DOS_Seek


align 16
DOS_ReadFile:

; [esp + 2*4] = void * buffer
; [esp +   4] = const char * path
; [esp      ] = return address

        Call_Asm_Stack2 DOS_ReadFile_c

        retn

; end procedure DOS_ReadFile


align 16
DOS_GetFileLength:

; [esp +   4] = const char * path
; [esp      ] = return address

        Call_Asm_Stack1 DOS_GetFileLength_c

        retn

; end procedure DOS_GetFileLength


