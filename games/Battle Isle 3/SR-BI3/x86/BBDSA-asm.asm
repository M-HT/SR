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
    %define DSA_ReuseStaticColors_c _DSA_ReuseStaticColors_c
    %define DSA_FreeStaticColors_c _DSA_FreeStaticColors_c
    %define DSAWIN_PrepareInit_c _DSAWIN_PrepareInit_c
    %define DSAWIN_GetMainWindowHandle_c _DSAWIN_GetMainWindowHandle_c
    %define DSAWIN_GetInstance_c _DSAWIN_GetInstance_c
    %define DSA_Init_c _DSA_Init_c
    %define DSA_Exit_c _DSA_Exit_c
    %define DSA_OpenScreen_c _DSA_OpenScreen_c
    %define DSA_CloseScreen_c _DSA_CloseScreen_c
    %define DSA_CopyMainOPMToScreen_c _DSA_CopyMainOPMToScreen_c
    %define DSA_StretchOPMToScreen_c _DSA_StretchOPMToScreen_c
    %define DSA_CopyOPMToScreenEx_c _DSA_CopyOPMToScreenEx_c
    %define DSA_TotalRepaint_c _DSA_TotalRepaint_c
    %define DSA_MoveScreen_c _DSA_MoveScreen_c
    %define DSA_ResizeScreen_c _DSA_ResizeScreen_c
    %define DSA_DrawSizingScreen_c _DSA_DrawSizingScreen_c
    %define DSA_EnterResizingMode_c _DSA_EnterResizingMode_c
    %define DSA_LeaveResizingMode_c _DSA_LeaveResizingMode_c
    %define DSA_GetDSAMetrics_c _DSA_GetDSAMetrics_c
    %define DSA_SetDSAPos_c _DSA_SetDSAPos_c
    %define DSA_GetScreenExtends_c _DSA_GetScreenExtends_c
    %define DSA_GetActiveScreen_c _DSA_GetActiveScreen_c
    %define DSA_SetActiveScreen_c _DSA_SetActiveScreen_c
    %define DSA_GetLastTouchedScreen_c _DSA_GetLastTouchedScreen_c
    %define DSA_CopyPartOPMToScreen_c _DSA_CopyPartOPMToScreen_c
    %define DSA_ScreenVisibility_c _DSA_ScreenVisibility_c
    %define DSA_LoadBackground_c _DSA_LoadBackground_c
    %define DSA_FixBackground_c _DSA_FixBackground_c
    %define DSA_SetCapture_c _DSA_SetCapture_c
    %define DSA_SetBackgroundInRAM_c _DSA_SetBackgroundInRAM_c
    %define DSA_SetPal_c _DSA_SetPal_c
    %define DSA_ActivatePal_c _DSA_ActivatePal_c
    %define DSA_PreventPaletteRemapping_c _DSA_PreventPaletteRemapping_c
    %define DSA_GetBackgroundOffset_c _DSA_GetBackgroundOffset_c
    %define DSA_SetBackground2Black_c _DSA_SetBackground2Black_c
    %define DSA_MarkBitmapAsDirty_c _DSA_MarkBitmapAsDirty_c
%endif

extern DSA_ReuseStaticColors_c
extern DSA_FreeStaticColors_c
extern DSAWIN_PrepareInit_c
extern DSAWIN_GetMainWindowHandle_c
extern DSAWIN_GetInstance_c
extern DSA_Init_c
extern DSA_Exit_c
extern DSA_OpenScreen_c
extern DSA_CloseScreen_c
extern DSA_CopyMainOPMToScreen_c
extern DSA_StretchOPMToScreen_c
extern DSA_CopyOPMToScreenEx_c
extern DSA_TotalRepaint_c
extern DSA_MoveScreen_c
extern DSA_ResizeScreen_c
extern DSA_DrawSizingScreen_c
extern DSA_EnterResizingMode_c
extern DSA_LeaveResizingMode_c
extern DSA_GetDSAMetrics_c
extern DSA_SetDSAPos_c
extern DSA_GetScreenExtends_c
extern DSA_GetActiveScreen_c
extern DSA_SetActiveScreen_c
extern DSA_GetLastTouchedScreen_c
extern DSA_CopyPartOPMToScreen_c
extern DSA_ScreenVisibility_c
extern DSA_LoadBackground_c
extern DSA_FixBackground_c
extern DSA_SetCapture_c
extern DSA_SetBackgroundInRAM_c
extern DSA_SetPal_c
extern DSA_ActivatePal_c
extern DSA_PreventPaletteRemapping_c
extern DSA_GetBackgroundOffset_c
extern DSA_SetBackground2Black_c
extern DSA_MarkBitmapAsDirty_c

global DSA_ReuseStaticColors
global DSA_FreeStaticColors
global DSAWIN_PrepareInit
global DSAWIN_GetMainWindowHandle
global DSAWIN_GetInstance
global DSA_Init
global DSA_Exit
global DSA_OpenScreen
global DSA_CloseScreen
global DSA_CopyMainOPMToScreen
global DSA_StretchOPMToScreen
global DSA_CopyOPMToScreenEx
global DSA_TotalRepaint
global DSA_MoveScreen
global DSA_ResizeScreen
global DSA_DrawSizingScreen
global DSA_EnterResizingMode
global DSA_LeaveResizingMode
global DSA_GetDSAMetrics
global DSA_SetDSAPos
global DSA_GetScreenExtends
global DSA_GetActiveScreen
global DSA_SetActiveScreen
global DSA_GetLastTouchedScreen
global DSA_CopyPartOPMToScreen
global DSA_ScreenVisibility
global DSA_LoadBackground
global DSA_FixBackground
global DSA_SetCapture
global DSA_SetBackgroundInRAM
global DSA_SetPal
global DSA_ActivatePal
global DSA_PreventPaletteRemapping
global DSA_GetBackgroundOffset
global DSA_SetBackground2Black
global DSA_MarkBitmapAsDirty

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
DSA_ReuseStaticColors:

; [esp +   4] = unsigned int osVersions
; [esp      ] = return address

        Call_Asm_Stack1 DSA_ReuseStaticColors_c

        retn

; end procedure DSA_ReuseStaticColors


align 16
DSA_FreeStaticColors:

; [esp +   4] = unsigned int osVersions
; [esp      ] = return address

        Call_Asm_Stack1 DSA_FreeStaticColors_c

        retn

; end procedure DSA_FreeStaticColors


align 16
DSAWIN_PrepareInit:

; [esp +   4] = void * hInstance
; [esp      ] = return address

        Call_Asm_Stack1 DSAWIN_PrepareInit_c

        retn

; end procedure DSAWIN_PrepareInit


align 16
DSAWIN_GetMainWindowHandle:

; [esp      ] = return address

        Call_Asm_Stack0 DSAWIN_GetMainWindowHandle_c

        retn

; end procedure DSAWIN_GetMainWindowHandle


align 16
DSAWIN_GetInstance:

; [esp      ] = return address

        Call_Asm_Stack0 DSAWIN_GetInstance_c

        retn

; end procedure DSAWIN_GetInstance


align 16
DSA_Init:

; [esp      ] = return address

        Call_Asm_Stack0 DSA_Init_c

        retn

; end procedure DSA_Init


align 16
DSA_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 DSA_Exit_c

        retn

; end procedure DSA_Exit


align 16
DSA_OpenScreen:

; [esp + 7*4] = int type
; [esp + 6*4] = int y
; [esp + 5*4] = int x
; [esp + 4*4] = const char * windowName
; [esp + 3*4] = int param3
; [esp + 2*4] = OPM_struct * opm
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack7 DSA_OpenScreen_c

        retn

; end procedure DSA_OpenScreen


align 16
DSA_CloseScreen:

; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack1 DSA_CloseScreen_c

        retn

; end procedure DSA_CloseScreen


align 16
DSA_CopyMainOPMToScreen:

; [esp + 2*4] = int onlyModified
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack2 DSA_CopyMainOPMToScreen_c

        retn

; end procedure DSA_CopyMainOPMToScreen


align 16
DSA_StretchOPMToScreen:

; [esp + 10*4] = int heightSrc
; [esp + 9*4] = int widthSrc
; [esp + 8*4] = int ySrc
; [esp + 7*4] = int xSrc
; [esp + 6*4] = OPM_struct * opm
; [esp + 5*4] = int heightDst
; [esp + 4*4] = int widthDst
; [esp + 3*4] = int yDst
; [esp + 2*4] = int xDst
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack10 DSA_StretchOPMToScreen_c

        retn

; end procedure DSA_StretchOPMToScreen


align 16
DSA_CopyOPMToScreenEx:

; [esp + 8*4] = int ySrc
; [esp + 7*4] = int xSrc
; [esp + 6*4] = OPM_struct * opm
; [esp + 5*4] = int height
; [esp + 4*4] = int width
; [esp + 3*4] = int yDst
; [esp + 2*4] = int xDst
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack8 DSA_CopyOPMToScreenEx_c

        retn

; end procedure DSA_CopyOPMToScreenEx


align 16
DSA_TotalRepaint:

; [esp      ] = return address

        Call_Asm_Stack0 DSA_TotalRepaint_c

        retn

; end procedure DSA_TotalRepaint


align 16
DSA_MoveScreen:

; [esp + 3*4] = int changeY
; [esp + 2*4] = int changeX
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack3 DSA_MoveScreen_c

        retn

; end procedure DSA_MoveScreen


align 16
DSA_ResizeScreen:

; [esp + 3*4] = int redraw
; [esp + 2*4] = OPM_struct * opm
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack3 DSA_ResizeScreen_c

        retn

; end procedure DSA_ResizeScreen


align 16
DSA_DrawSizingScreen:

; [esp + 2*4] = int16_t * rect
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack2 DSA_DrawSizingScreen_c

        retn

; end procedure DSA_DrawSizingScreen


align 16
DSA_EnterResizingMode:

; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack1 DSA_EnterResizingMode_c

        retn

; end procedure DSA_EnterResizingMode


align 16
DSA_LeaveResizingMode:

; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack1 DSA_LeaveResizingMode_c

        retn

; end procedure DSA_LeaveResizingMode


align 16
DSA_GetDSAMetrics:

; [esp + 6*4] = uint8_t * isVisible
; [esp + 5*4] = int32_t * height
; [esp + 4*4] = int32_t * width
; [esp + 3*4] = int32_t * y
; [esp + 2*4] = int32_t * x
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack6 DSA_GetDSAMetrics_c

        retn

; end procedure DSA_GetDSAMetrics


align 16
DSA_SetDSAPos:

; [esp + 4*4] = int repaint
; [esp + 3*4] = int y
; [esp + 2*4] = int x
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack4 DSA_SetDSAPos_c

        retn

; end procedure DSA_SetDSAPos


align 16
DSA_GetScreenExtends:

; [esp + 2*4] = int32_t * height
; [esp +   4] = int32_t * width
; [esp      ] = return address

        Call_Asm_Stack2 DSA_GetScreenExtends_c

        retn

; end procedure DSA_GetScreenExtends


align 16
DSA_GetActiveScreen:

; [esp      ] = return address

        Call_Asm_Stack0 DSA_GetActiveScreen_c

        retn

; end procedure DSA_GetActiveScreen


align 16
DSA_SetActiveScreen:

; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack1 DSA_SetActiveScreen_c

        retn

; end procedure DSA_SetActiveScreen


align 16
DSA_GetLastTouchedScreen:

; [esp      ] = return address

        Call_Asm_Stack0 DSA_GetLastTouchedScreen_c

        retn

; end procedure DSA_GetLastTouchedScreen


align 16
DSA_CopyPartOPMToScreen:

; [esp + 5*4] = int height
; [esp + 4*4] = int width
; [esp + 3*4] = int y
; [esp + 2*4] = int x
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack5 DSA_CopyPartOPMToScreen_c

        retn

; end procedure DSA_CopyPartOPMToScreen


align 16
DSA_ScreenVisibility:

; [esp + 2*4] = int show
; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack2 DSA_ScreenVisibility_c

        retn

; end procedure DSA_ScreenVisibility


align 16
DSA_LoadBackground:

; [esp +   4] = const char * path
; [esp      ] = return address

        Call_Asm_Stack1 DSA_LoadBackground_c

        retn

; end procedure DSA_LoadBackground


align 16
DSA_FixBackground:

; [esp +   4] = int isFixed
; [esp      ] = return address

        Call_Asm_Stack1 DSA_FixBackground_c

        retn

; end procedure DSA_FixBackground


align 16
DSA_SetCapture:

; [esp +   4] = DSA_Screen * screen
; [esp      ] = return address

        Call_Asm_Stack1 DSA_SetCapture_c

        retn

; end procedure DSA_SetCapture


align 16
DSA_SetBackgroundInRAM:

; [esp +   4] = int value
; [esp      ] = return address

        Call_Asm_Stack1 DSA_SetBackgroundInRAM_c

        retn

; end procedure DSA_SetBackgroundInRAM


align 16
DSA_SetPal:

; [esp + 5*4] = unsigned int dst_start_entry
; [esp + 4*4] = unsigned int num_entries
; [esp + 3*4] = unsigned int src_start_entry
; [esp + 2*4] = DSA_Palette * palette
; [esp +   4] = int unused
; [esp      ] = return address

        Call_Asm_Stack5 DSA_SetPal_c

        retn

; end procedure DSA_SetPal


align 16
DSA_ActivatePal:

; [esp      ] = return address

        Call_Asm_Stack0 DSA_ActivatePal_c

        retn

; end procedure DSA_ActivatePal


align 16
DSA_PreventPaletteRemapping:

; [esp +   4] = int valueAdd
; [esp      ] = return address

        Call_Asm_Stack1 DSA_PreventPaletteRemapping_c

        retn

; end procedure DSA_PreventPaletteRemapping


align 16
DSA_GetBackgroundOffset:

; [esp + 2*4] = int32_t * offsetY
; [esp +   4] = int32_t * offsetX
; [esp      ] = return address

        Call_Asm_Stack2 DSA_GetBackgroundOffset_c

        retn

; end procedure DSA_GetBackgroundOffset


align 16
DSA_SetBackground2Black:

; [esp +   4] = int isBlack
; [esp      ] = return address

        Call_Asm_Stack1 DSA_SetBackground2Black_c

        retn

; end procedure DSA_SetBackground2Black


align 16
DSA_MarkBitmapAsDirty:

; [esp + 2*4] = int param2
; [esp +   4] = int param1
; [esp      ] = return address

        Call_Asm_Stack2 DSA_MarkBitmapAsDirty_c

        retn

; end procedure DSA_MarkBitmapAsDirty


