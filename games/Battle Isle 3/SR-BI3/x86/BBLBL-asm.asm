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
    %define LBL_Init_c _LBL_Init_c
    %define LBL_Exit_c _LBL_Exit_c
    %define LBL_OpenLib_c _LBL_OpenLib_c
    %define LBL_CloseLib_c _LBL_CloseLib_c
    %define LBL_ReadEntry_c _LBL_ReadEntry_c
    %define LBL_GetEntrySize_c _LBL_GetEntrySize_c
    %define LBL_CloseFile_c _LBL_CloseFile_c
    %define LBL_GetNOFEntries_c _LBL_GetNOFEntries_c
%endif

extern LBL_Init_c
extern LBL_Exit_c
extern LBL_OpenLib_c
extern LBL_CloseLib_c
extern LBL_ReadEntry_c
extern LBL_GetEntrySize_c
extern LBL_CloseFile_c
extern LBL_GetNOFEntries_c

global LBL_Init
global LBL_Exit
global LBL_OpenLib
global LBL_CloseLib
global LBL_ReadEntry
global LBL_GetEntrySize
global LBL_CloseFile
global LBL_GetNOFEntries

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
LBL_Init:

; [esp      ] = return address

        Call_Asm_Stack0 LBL_Init_c

        retn

; end procedure LBL_Init


align 16
LBL_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 LBL_Exit_c

        retn

; end procedure LBL_Exit


align 16
LBL_OpenLib:

; [esp + 2*4] = int param2
; [esp +   4] = const char * path
; [esp      ] = return address

        Call_Asm_Stack2 LBL_OpenLib_c

        retn

; end procedure LBL_OpenLib


align 16
LBL_CloseLib:

; [esp +   4] = void * lib
; [esp      ] = return address

        Call_Asm_Stack1 LBL_CloseLib_c

        retn

; end procedure LBL_CloseLib


align 16
LBL_ReadEntry:

; [esp + 5*4] = void * entry_metadata
; [esp + 4*4] = int close_file
; [esp + 3*4] = unsigned int entry_number
; [esp + 2*4] = void * entry_data
; [esp +   4] = void * lib
; [esp      ] = return address

        Call_Asm_Stack5 LBL_ReadEntry_c

        retn

; end procedure LBL_ReadEntry


align 16
LBL_GetEntrySize:

; [esp + 2*4] = unsigned int entry_number
; [esp +   4] = void * lib
; [esp      ] = return address

        Call_Asm_Stack2 LBL_GetEntrySize_c

        retn

; end procedure LBL_GetEntrySize


align 16
LBL_CloseFile:

; [esp +   4] = void * lib
; [esp      ] = return address

        Call_Asm_Stack1 LBL_CloseFile_c

        retn

; end procedure LBL_CloseFile


align 16
LBL_GetNOFEntries:

; [esp +   4] = void * lib
; [esp      ] = return address

        Call_Asm_Stack1 LBL_GetNOFEntries_c

        retn

; end procedure LBL_GetNOFEntries


