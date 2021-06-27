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
    %define LBM_DisplayLBM_c _LBM_DisplayLBM_c
    %define LBM_StartSerie_c _LBM_StartSerie_c
    %define LBM_AutoSave_c _LBM_AutoSave_c
%endif

extern LBM_DisplayLBM_c
extern LBM_StartSerie_c
extern LBM_AutoSave_c

global LBM_DisplayLBM
global LBM_StartSerie
global LBM_AutoSave

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
LBM_DisplayLBM:

; [esp + 4*4] = unsigned int flags
; [esp + 3*4] = uint8_t * palette
; [esp + 2*4] = void * pixel_map
; [esp +   4] = const char * path
; [esp      ] = return address

        Call_Asm_Stack4 LBM_DisplayLBM_c

        retn

; end procedure LBM_DisplayLBM


align 16
LBM_StartSerie:

; [esp +   4] = int number
; [esp      ] = return address

        Call_Asm_Stack1 LBM_StartSerie_c

        retn

; end procedure LBM_StartSerie


align 16
LBM_AutoSave:

; [esp +   4] = void * pixel_map
; [esp      ] = return address

        Call_Asm_Stack1 LBM_AutoSave_c

        retn

; end procedure LBM_AutoSave


