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
    %define FX_Init_c _FX_Init_c
    %define FX_Exit_c _FX_Exit_c
    %define FX_ReserveDevices_c _FX_ReserveDevices_c
    %define FX_ReadLib_c _FX_ReadLib_c
    %define FX_FreeLib_c _FX_FreeLib_c
    %define FX_StopAllSamples_c _FX_StopAllSamples_c
    %define FX_PlaySample_c _FX_PlaySample_c
    %define FX_SetVolume_c _FX_SetVolume_c
    %define FM_IsError_c _FM_IsError_c
%endif

extern FX_Init_c
extern FX_Exit_c
extern FX_ReserveDevices_c
extern FX_ReadLib_c
extern FX_FreeLib_c
extern FX_StopAllSamples_c
extern FX_PlaySample_c
extern FX_SetVolume_c
extern FM_IsError_c

global FX_Init
global FX_Exit
global FX_ReserveDevices
global FX_ReadLib
global FX_FreeLib
global FX_StopAllSamples
global FX_PlaySample
global FX_SetVolume
global FM_IsError

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
FX_Init:

; [esp      ] = return address

        Call_Asm_Stack0 FX_Init_c

        retn

; end procedure FX_Init


align 16
FX_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 FX_Exit_c

        retn

; end procedure FX_Exit


align 16
FX_ReserveDevices:

; [esp +   4] = int reserve
; [esp      ] = return address

        Call_Asm_Stack1 FX_ReserveDevices_c

        retn

; end procedure FX_ReserveDevices


align 16
FX_ReadLib:

; [esp +   4] = const char * path
; [esp      ] = return address

        Call_Asm_Stack1 FX_ReadLib_c

        retn

; end procedure FX_ReadLib


align 16
FX_FreeLib:

; [esp +   4] = int lib_handle
; [esp      ] = return address

        Call_Asm_Stack1 FX_FreeLib_c

        retn

; end procedure FX_FreeLib


align 16
FX_StopAllSamples:

; [esp      ] = return address

        Call_Asm_Stack0 FX_StopAllSamples_c

        retn

; end procedure FX_StopAllSamples


align 16
FX_PlaySample:

; [esp + 5*4] = int times_play
; [esp + 4*4] = int volume
; [esp + 3*4] = int priority
; [esp + 2*4] = int sample_number
; [esp +   4] = int lib_handle
; [esp      ] = return address

        Call_Asm_Stack5 FX_PlaySample_c

        retn

; end procedure FX_PlaySample


align 16
FX_SetVolume:

; [esp +   4] = unsigned int volume
; [esp      ] = return address

        Call_Asm_Stack1 FX_SetVolume_c

        retn

; end procedure FX_SetVolume


align 16
FM_IsError:

; [esp      ] = return address

        Call_Asm_Stack0 FM_IsError_c

        retn

; end procedure FM_IsError


