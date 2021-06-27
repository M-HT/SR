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
    %define OPM_Init_c _OPM_Init_c
    %define OPM_Exit_c _OPM_Exit_c
    %define OPM_New_c _OPM_New_c
    %define OPM_Del_c _OPM_Del_c
    %define OPM_CreateVirtualOPM_c _OPM_CreateVirtualOPM_c
    %define OPM_CreateSecondaryOPM_c _OPM_CreateSecondaryOPM_c
    %define OPM_SetPixel_c _OPM_SetPixel_c
    %define OPM_HorLine_c _OPM_HorLine_c
    %define OPM_VerLine_c _OPM_VerLine_c
    %define OPM_FillBox_c _OPM_FillBox_c
    %define OPM_CopyGFXOPM_c _OPM_CopyGFXOPM_c
    %define OPM_CopyOPMOPM_c _OPM_CopyOPMOPM_c
    %define OPM_AccessBitmap_c _OPM_AccessBitmap_c
%endif

extern OPM_Init_c
extern OPM_Exit_c
extern OPM_New_c
extern OPM_Del_c
extern OPM_CreateVirtualOPM_c
extern OPM_CreateSecondaryOPM_c
extern OPM_SetPixel_c
extern OPM_HorLine_c
extern OPM_VerLine_c
extern OPM_FillBox_c
extern OPM_CopyGFXOPM_c
extern OPM_CopyOPMOPM_c
extern OPM_AccessBitmap_c

global OPM_Init
global OPM_Exit
global OPM_New
global OPM_Del
global OPM_CreateVirtualOPM
global OPM_CreateSecondaryOPM
global OPM_SetPixel
global OPM_HorLine
global OPM_VerLine
global OPM_FillBox
global OPM_CopyGFXOPM
global OPM_CopyOPMOPM
global OPM_AccessBitmap

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
OPM_Init:

; [esp      ] = return address

        Call_Asm_Stack0 OPM_Init_c

        retn

; end procedure OPM_Init


align 16
OPM_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 OPM_Exit_c

        retn

; end procedure OPM_Exit


align 16
OPM_New:

; [esp + 5*4] = uint8_t * buffer
; [esp + 4*4] = OPM_struct * pixel_map
; [esp + 3*4] = unsigned int bytes_per_pixel
; [esp + 2*4] = unsigned int height
; [esp +   4] = unsigned int width
; [esp      ] = return address

        Call_Asm_Stack5 OPM_New_c

        retn

; end procedure OPM_New


align 16
OPM_Del:

; [esp +   4] = OPM_struct * pixel_map
; [esp      ] = return address

        Call_Asm_Stack1 OPM_Del_c

        retn

; end procedure OPM_Del


align 16
OPM_CreateVirtualOPM:

; [esp + 6*4] = int virtual_height
; [esp + 5*4] = int virtual_width
; [esp + 4*4] = int virtual_y
; [esp + 3*4] = int virtual_x
; [esp + 2*4] = OPM_struct * virtual_pixel_map
; [esp +   4] = OPM_struct * base_pixel_map
; [esp      ] = return address

        Call_Asm_Stack6 OPM_CreateVirtualOPM_c

        retn

; end procedure OPM_CreateVirtualOPM


align 16
OPM_CreateSecondaryOPM:

; [esp + 5*4] = uint8_t * buffer
; [esp + 4*4] = OPM_struct * pixel_map
; [esp + 3*4] = int bytes_per_pixel
; [esp + 2*4] = int height
; [esp +   4] = int width
; [esp      ] = return address

        Call_Asm_Stack5 OPM_CreateSecondaryOPM_c

        retn

; end procedure OPM_CreateSecondaryOPM


align 16
OPM_SetPixel:

; [esp + 4*4] = uint8_t color
; [esp + 3*4] = int y
; [esp + 2*4] = int x
; [esp +   4] = OPM_struct * pixel_map
; [esp      ] = return address

        Call_Asm_Stack4 OPM_SetPixel_c

        retn

; end procedure OPM_SetPixel


align 16
OPM_HorLine:

; [esp + 5*4] = uint8_t color
; [esp + 4*4] = unsigned int length
; [esp + 3*4] = int y
; [esp + 2*4] = int x
; [esp +   4] = OPM_struct * pixel_map
; [esp      ] = return address

        Call_Asm_Stack5 OPM_HorLine_c

        retn

; end procedure OPM_HorLine


align 16
OPM_VerLine:

; [esp + 5*4] = uint8_t color
; [esp + 4*4] = unsigned int length
; [esp + 3*4] = int y
; [esp + 2*4] = int x
; [esp +   4] = OPM_struct * pixel_map
; [esp      ] = return address

        Call_Asm_Stack5 OPM_VerLine_c

        retn

; end procedure OPM_VerLine


align 16
OPM_FillBox:

; [esp + 6*4] = uint8_t color
; [esp + 5*4] = unsigned int height
; [esp + 4*4] = unsigned int width
; [esp + 3*4] = int y
; [esp + 2*4] = int x
; [esp +   4] = OPM_struct * pixel_map
; [esp      ] = return address

        Call_Asm_Stack6 OPM_FillBox_c

        retn

; end procedure OPM_FillBox


align 16
OPM_CopyGFXOPM:

; [esp + 5*4] = uint8_t value_add
; [esp + 4*4] = int pos_y
; [esp + 3*4] = int pos_x
; [esp + 2*4] = GFX_struct * gfx
; [esp +   4] = OPM_struct * pixel_map
; [esp      ] = return address

        Call_Asm_Stack5 OPM_CopyGFXOPM_c

        retn

; end procedure OPM_CopyGFXOPM


align 16
OPM_CopyOPMOPM:

; [esp + 8*4] = int dst_y
; [esp + 7*4] = int dst_x
; [esp + 6*4] = int copy_height
; [esp + 5*4] = int copy_width
; [esp + 4*4] = int src_y
; [esp + 3*4] = int src_x
; [esp + 2*4] = OPM_struct * dst_pixel_map
; [esp +   4] = OPM_struct * src_pixel_map
; [esp      ] = return address

        Call_Asm_Stack8 OPM_CopyOPMOPM_c

        retn

; end procedure OPM_CopyOPMOPM


align 16
OPM_AccessBitmap:

; [esp +   4] = OPM_struct * pixel_map
; [esp      ] = return address

        Call_Asm_Stack1 OPM_AccessBitmap_c

        retn

; end procedure OPM_AccessBitmap


