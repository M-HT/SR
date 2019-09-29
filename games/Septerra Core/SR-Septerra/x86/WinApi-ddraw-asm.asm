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
    %define DirectDrawCreate_c _DirectDrawCreate_c

    %define IDirectDraw_QueryInterface_c _IDirectDraw_QueryInterface_c
    %define IDirectDraw_AddRef_c _IDirectDraw_AddRef_c
    %define IDirectDraw_Release_c _IDirectDraw_Release_c
    %define IDirectDraw_Compact_c _IDirectDraw_Compact_c
    %define IDirectDraw_CreateClipper_c _IDirectDraw_CreateClipper_c
    %define IDirectDraw_CreatePalette_c _IDirectDraw_CreatePalette_c
    %define IDirectDraw_CreateSurface_c _IDirectDraw_CreateSurface_c
    %define IDirectDraw_DuplicateSurface_c _IDirectDraw_DuplicateSurface_c
    %define IDirectDraw_EnumDisplayModes_c _IDirectDraw_EnumDisplayModes_c
    %define IDirectDraw_EnumSurfaces_c _IDirectDraw_EnumSurfaces_c
    %define IDirectDraw_FlipToGDISurface_c _IDirectDraw_FlipToGDISurface_c
    %define IDirectDraw_GetCaps_c _IDirectDraw_GetCaps_c
    %define IDirectDraw_GetDisplayMode_c _IDirectDraw_GetDisplayMode_c
    %define IDirectDraw_GetFourCCCodes_c _IDirectDraw_GetFourCCCodes_c
    %define IDirectDraw_GetGDISurface_c _IDirectDraw_GetGDISurface_c
    %define IDirectDraw_GetMonitorFrequency_c _IDirectDraw_GetMonitorFrequency_c
    %define IDirectDraw_GetScanLine_c _IDirectDraw_GetScanLine_c
    %define IDirectDraw_GetVerticalBlankStatus_c _IDirectDraw_GetVerticalBlankStatus_c
    %define IDirectDraw_Initialize_c _IDirectDraw_Initialize_c
    %define IDirectDraw_RestoreDisplayMode_c _IDirectDraw_RestoreDisplayMode_c
    %define IDirectDraw_SetCooperativeLevel_c _IDirectDraw_SetCooperativeLevel_c
    %define IDirectDraw_SetDisplayMode_c _IDirectDraw_SetDisplayMode_c
    %define IDirectDraw_WaitForVerticalBlank_c _IDirectDraw_WaitForVerticalBlank_c

    %define IDirectDrawSurface_QueryInterface_c _IDirectDrawSurface_QueryInterface_c
    %define IDirectDrawSurface_AddRef_c _IDirectDrawSurface_AddRef_c
    %define IDirectDrawSurface_Release_c _IDirectDrawSurface_Release_c
    %define IDirectDrawSurface_AddAttachedSurface_c _IDirectDrawSurface_AddAttachedSurface_c
    %define IDirectDrawSurface_AddOverlayDirtyRect_c _IDirectDrawSurface_AddOverlayDirtyRect_c
    %define IDirectDrawSurface_Blt_c _IDirectDrawSurface_Blt_c
    %define IDirectDrawSurface_BltBatch_c _IDirectDrawSurface_BltBatch_c
    %define IDirectDrawSurface_BltFast_c _IDirectDrawSurface_BltFast_c
    %define IDirectDrawSurface_DeleteAttachedSurface_c _IDirectDrawSurface_DeleteAttachedSurface_c
    %define IDirectDrawSurface_EnumAttachedSurfaces_c _IDirectDrawSurface_EnumAttachedSurfaces_c
    %define IDirectDrawSurface_EnumOverlayZOrders_c _IDirectDrawSurface_EnumOverlayZOrders_c
    %define IDirectDrawSurface_Flip_c _IDirectDrawSurface_Flip_c
    %define IDirectDrawSurface_GetAttachedSurface_c _IDirectDrawSurface_GetAttachedSurface_c
    %define IDirectDrawSurface_GetBltStatus_c _IDirectDrawSurface_GetBltStatus_c
    %define IDirectDrawSurface_GetCaps_c _IDirectDrawSurface_GetCaps_c
    %define IDirectDrawSurface_GetClipper_c _IDirectDrawSurface_GetClipper_c
    %define IDirectDrawSurface_GetColorKey_c _IDirectDrawSurface_GetColorKey_c
    %define IDirectDrawSurface_GetDC_c _IDirectDrawSurface_GetDC_c
    %define IDirectDrawSurface_GetFlipStatus_c _IDirectDrawSurface_GetFlipStatus_c
    %define IDirectDrawSurface_GetOverlayPosition_c _IDirectDrawSurface_GetOverlayPosition_c
    %define IDirectDrawSurface_GetPalette_c _IDirectDrawSurface_GetPalette_c
    %define IDirectDrawSurface_GetPixelFormat_c _IDirectDrawSurface_GetPixelFormat_c
    %define IDirectDrawSurface_GetSurfaceDesc_c _IDirectDrawSurface_GetSurfaceDesc_c
    %define IDirectDrawSurface_Initialize_c _IDirectDrawSurface_Initialize_c
    %define IDirectDrawSurface_IsLost_c _IDirectDrawSurface_IsLost_c
    %define IDirectDrawSurface_Lock_c _IDirectDrawSurface_Lock_c
    %define IDirectDrawSurface_ReleaseDC_c _IDirectDrawSurface_ReleaseDC_c
    %define IDirectDrawSurface_Restore_c _IDirectDrawSurface_Restore_c
    %define IDirectDrawSurface_SetClipper_c _IDirectDrawSurface_SetClipper_c
    %define IDirectDrawSurface_SetColorKey_c _IDirectDrawSurface_SetColorKey_c
    %define IDirectDrawSurface_SetOverlayPosition_c _IDirectDrawSurface_SetOverlayPosition_c
    %define IDirectDrawSurface_SetPalette_c _IDirectDrawSurface_SetPalette_c
    %define IDirectDrawSurface_Unlock_c _IDirectDrawSurface_Unlock_c
    %define IDirectDrawSurface_UpdateOverlay_c _IDirectDrawSurface_UpdateOverlay_c
    %define IDirectDrawSurface_UpdateOverlayDisplay_c _IDirectDrawSurface_UpdateOverlayDisplay_c
    %define IDirectDrawSurface_UpdateOverlayZOrder_c _IDirectDrawSurface_UpdateOverlayZOrder_c

    %define IDirectDrawVtbl_asm2c _IDirectDrawVtbl_asm2c
    %define IDirectDrawSurfaceVtbl_asm2c _IDirectDrawSurfaceVtbl_asm2c
%endif

extern DirectDrawCreate_c

extern IDirectDraw_QueryInterface_c
extern IDirectDraw_AddRef_c
extern IDirectDraw_Release_c
extern IDirectDraw_Compact_c
extern IDirectDraw_CreateClipper_c
extern IDirectDraw_CreatePalette_c
extern IDirectDraw_CreateSurface_c
extern IDirectDraw_DuplicateSurface_c
extern IDirectDraw_EnumDisplayModes_c
extern IDirectDraw_EnumSurfaces_c
extern IDirectDraw_FlipToGDISurface_c
extern IDirectDraw_GetCaps_c
extern IDirectDraw_GetDisplayMode_c
extern IDirectDraw_GetFourCCCodes_c
extern IDirectDraw_GetGDISurface_c
extern IDirectDraw_GetMonitorFrequency_c
extern IDirectDraw_GetScanLine_c
extern IDirectDraw_GetVerticalBlankStatus_c
extern IDirectDraw_Initialize_c
extern IDirectDraw_RestoreDisplayMode_c
extern IDirectDraw_SetCooperativeLevel_c
extern IDirectDraw_SetDisplayMode_c
extern IDirectDraw_WaitForVerticalBlank_c

extern IDirectDrawSurface_QueryInterface_c
extern IDirectDrawSurface_AddRef_c
extern IDirectDrawSurface_Release_c
extern IDirectDrawSurface_AddAttachedSurface_c
extern IDirectDrawSurface_AddOverlayDirtyRect_c
extern IDirectDrawSurface_Blt_c
extern IDirectDrawSurface_BltBatch_c
extern IDirectDrawSurface_BltFast_c
extern IDirectDrawSurface_DeleteAttachedSurface_c
extern IDirectDrawSurface_EnumAttachedSurfaces_c
extern IDirectDrawSurface_EnumOverlayZOrders_c
extern IDirectDrawSurface_Flip_c
extern IDirectDrawSurface_GetAttachedSurface_c
extern IDirectDrawSurface_GetBltStatus_c
extern IDirectDrawSurface_GetCaps_c
extern IDirectDrawSurface_GetClipper_c
extern IDirectDrawSurface_GetColorKey_c
extern IDirectDrawSurface_GetDC_c
extern IDirectDrawSurface_GetFlipStatus_c
extern IDirectDrawSurface_GetOverlayPosition_c
extern IDirectDrawSurface_GetPalette_c
extern IDirectDrawSurface_GetPixelFormat_c
extern IDirectDrawSurface_GetSurfaceDesc_c
extern IDirectDrawSurface_Initialize_c
extern IDirectDrawSurface_IsLost_c
extern IDirectDrawSurface_Lock_c
extern IDirectDrawSurface_ReleaseDC_c
extern IDirectDrawSurface_Restore_c
extern IDirectDrawSurface_SetClipper_c
extern IDirectDrawSurface_SetColorKey_c
extern IDirectDrawSurface_SetOverlayPosition_c
extern IDirectDrawSurface_SetPalette_c
extern IDirectDrawSurface_Unlock_c
extern IDirectDrawSurface_UpdateOverlay_c
extern IDirectDrawSurface_UpdateOverlayDisplay_c
extern IDirectDrawSurface_UpdateOverlayZOrder_c

global DirectDrawCreate_asm2c

global IDirectDrawVtbl_asm2c
global IDirectDrawSurfaceVtbl_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
DirectDrawCreate_asm2c:

; [esp + 3*4] = IUnknown     *pUnkOuter
; [esp + 2*4] = LPDIRECTDRAW *lplpDD
; [esp +   4] = GUID         *lpGUID
; [esp      ] = return address

        Call_Asm_Stack3 DirectDrawCreate_c

        retn 3*4

; end procedure DirectDrawCreate_asm2c


align 16
IDirectDraw_QueryInterface_asm2c:

; [esp + 3*4] = LPVOID FAR * ppvObj
; [esp + 2*4] = REFIID riid
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDraw_QueryInterface_c

        retn 3*4

; end procedure IDirectDraw_QueryInterface_asm2c


align 16
IDirectDraw_AddRef_asm2c:

; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectDraw_AddRef_c

        retn 4

; end procedure IDirectDraw_AddRef_asm2c


align 16
IDirectDraw_Release_asm2c:

; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectDraw_Release_c

        retn 4

; end procedure IDirectDraw_Release_asm2c


align 16
IDirectDraw_Compact_asm2c:

; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectDraw_Compact_c

        retn 4

; end procedure IDirectDraw_Compact_asm2c


align 16
IDirectDraw_CreateClipper_asm2c:

; [esp + 4*4] = IUnknown FAR * param3
; [esp + 3*4] = LPDIRECTDRAWCLIPPER FAR* param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack4 IDirectDraw_CreateClipper_c

        retn 4*4

; end procedure IDirectDraw_CreateClipper_asm2c


align 16
IDirectDraw_CreatePalette_asm2c:

; [esp + 5*4] = IUnknown FAR * param4
; [esp + 4*4] = LPDIRECTDRAWPALETTE FAR* param3
; [esp + 3*4] = LPPALETTEENTRY param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack5 IDirectDraw_CreatePalette_c

        retn 5*4

; end procedure IDirectDraw_CreatePalette_asm2c


align 16
IDirectDraw_CreateSurface_asm2c:

; [esp + 4*4] = IUnknown FAR * param3
; [esp + 3*4] = LPDIRECTDRAWSURFACE FAR * param2
; [esp + 2*4] = LPDDSURFACEDESC param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack4 IDirectDraw_CreateSurface_c

        retn 4*4

; end procedure IDirectDraw_CreateSurface_asm2c


align 16
IDirectDraw_DuplicateSurface_asm2c:

; [esp + 3*4] = LPDIRECTDRAWSURFACE FAR * param2
; [esp + 2*4] = LPDIRECTDRAWSURFACE param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDraw_DuplicateSurface_c

        retn 3*4

; end procedure IDirectDraw_DuplicateSurface_asm2c


align 16
IDirectDraw_EnumDisplayModes_asm2c:

; [esp + 5*4] = LPDDENUMMODESCALLBACK param4
; [esp + 4*4] = LPVOID param3
; [esp + 3*4] = LPDDSURFACEDESC param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack5 IDirectDraw_EnumDisplayModes_c

        retn 5*4

; end procedure IDirectDraw_EnumDisplayModes_asm2c


align 16
IDirectDraw_EnumSurfaces_asm2c:

; [esp + 5*4] = LPDDENUMSURFACESCALLBACK param4
; [esp + 4*4] = LPVOID param3
; [esp + 3*4] = LPDDSURFACEDESC param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack5 IDirectDraw_EnumSurfaces_c

        retn 5*4

; end procedure IDirectDraw_EnumSurfaces_asm2c


align 16
IDirectDraw_FlipToGDISurface_asm2c:

; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectDraw_FlipToGDISurface_c

        retn 4

; end procedure IDirectDraw_FlipToGDISurface_asm2c


align 16
IDirectDraw_GetCaps_asm2c:

; [esp + 3*4] = LPDDCAPS param2
; [esp + 2*4] = LPDDCAPS param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDraw_GetCaps_c

        retn 3*4

; end procedure IDirectDraw_GetCaps_asm2c


align 16
IDirectDraw_GetDisplayMode_asm2c:

; [esp + 2*4] = LPDDSURFACEDESC param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDraw_GetDisplayMode_c

        retn 2*4

; end procedure IDirectDraw_GetDisplayMode_asm2c


align 16
IDirectDraw_GetFourCCCodes_asm2c:

; [esp + 3*4] = LPDWORD param2
; [esp + 2*4] = LPDWORD param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDraw_GetFourCCCodes_c

        retn 3*4

; end procedure IDirectDraw_GetFourCCCodes_asm2c


align 16
IDirectDraw_GetGDISurface_asm2c:

; [esp + 2*4] = LPDIRECTDRAWSURFACE FAR * param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDraw_GetGDISurface_c

        retn 2*4

; end procedure IDirectDraw_GetGDISurface_asm2c


align 16
IDirectDraw_GetMonitorFrequency_asm2c:

; [esp + 2*4] = LPDWORD param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDraw_GetMonitorFrequency_c

        retn 2*4

; end procedure IDirectDraw_GetMonitorFrequency_asm2c


align 16
IDirectDraw_GetScanLine_asm2c:

; [esp + 2*4] = LPDWORD param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDraw_GetScanLine_c

        retn 2*4

; end procedure IDirectDraw_GetScanLine_asm2c


align 16
IDirectDraw_GetVerticalBlankStatus_asm2c:

; [esp + 2*4] = LPBOOL param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDraw_GetVerticalBlankStatus_c

        retn 2*4

; end procedure IDirectDraw_GetVerticalBlankStatus_asm2c


align 16
IDirectDraw_Initialize_asm2c:

; [esp + 2*4] = GUID FAR * param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDraw_Initialize_c

        retn 2*4

; end procedure IDirectDraw_Initialize_asm2c


align 16
IDirectDraw_RestoreDisplayMode_asm2c:

; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectDraw_RestoreDisplayMode_c

        retn 4

; end procedure IDirectDraw_RestoreDisplayMode_asm2c


align 16
IDirectDraw_SetCooperativeLevel_asm2c:

; [esp + 3*4] = DWORD param2
; [esp + 2*4] = HWND param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDraw_SetCooperativeLevel_c

        retn 3*4

; end procedure IDirectDraw_SetCooperativeLevel_asm2c


align 16
IDirectDraw_SetDisplayMode_asm2c:

; [esp + 4*4] = DWORD param3
; [esp + 3*4] = DWORD param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack4 IDirectDraw_SetDisplayMode_c

        retn 4*4

; end procedure IDirectDraw_SetDisplayMode_asm2c


align 16
IDirectDraw_WaitForVerticalBlank_asm2c:

; [esp + 3*4] = HANDLE param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDraw *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDraw_WaitForVerticalBlank_c

        retn 3*4

; end procedure IDirectDraw_WaitForVerticalBlank_asm2c


align 16
IDirectDrawSurface_QueryInterface_asm2c:

; [esp + 3*4] = LPVOID FAR * ppvObj
; [esp + 2*4] = REFIID riid
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_QueryInterface_c

        retn 3*4

; end procedure IDirectDrawSurface_QueryInterface_asm2c


align 16
IDirectDrawSurface_AddRef_asm2c:

; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectDrawSurface_AddRef_c

        retn 4

; end procedure IDirectDrawSurface_AddRef_asm2c


align 16
IDirectDrawSurface_Release_asm2c:

; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectDrawSurface_Release_c

        retn 4

; end procedure IDirectDrawSurface_Release_asm2c


align 16
IDirectDrawSurface_AddAttachedSurface_asm2c:

; [esp + 2*4] = LPDIRECTDRAWSURFACE param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_AddAttachedSurface_c

        retn 2*4

; end procedure IDirectDrawSurface_AddAttachedSurface_asm2c


align 16
IDirectDrawSurface_AddOverlayDirtyRect_asm2c:

; [esp + 2*4] = LPRECT param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_AddOverlayDirtyRect_c

        retn 2*4

; end procedure IDirectDrawSurface_AddOverlayDirtyRect_asm2c


align 16
IDirectDrawSurface_Blt_asm2c:

; [esp + 6*4] = LPDDBLTFX param5
; [esp + 5*4] = DWORD param4
; [esp + 4*4] = LPRECT param3
; [esp + 3*4] = LPDIRECTDRAWSURFACE param2
; [esp + 2*4] = LPRECT param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack6 IDirectDrawSurface_Blt_c

        retn 6*4

; end procedure IDirectDrawSurface_Blt_asm2c


align 16
IDirectDrawSurface_BltBatch_asm2c:

; [esp + 4*4] = DWORD param3
; [esp + 3*4] = DWORD param2
; [esp + 2*4] = LPDDBLTBATCH param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack4 IDirectDrawSurface_BltBatch_c

        retn 4*4

; end procedure IDirectDrawSurface_BltBatch_asm2c


align 16
IDirectDrawSurface_BltFast_asm2c:

; [esp + 6*4] = DWORD param5
; [esp + 5*4] = LPRECT param4
; [esp + 4*4] = LPDIRECTDRAWSURFACE param3
; [esp + 3*4] = DWORD param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack6 IDirectDrawSurface_BltFast_c

        retn 6*4

; end procedure IDirectDrawSurface_BltFast_asm2c


align 16
IDirectDrawSurface_DeleteAttachedSurface_asm2c:

; [esp + 3*4] = LPDIRECTDRAWSURFACE param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_DeleteAttachedSurface_c

        retn 3*4

; end procedure IDirectDrawSurface_DeleteAttachedSurface_asm2c


align 16
IDirectDrawSurface_EnumAttachedSurfaces_asm2c:

; [esp + 3*4] = LPDDENUMSURFACESCALLBACK param2
; [esp + 2*4] = LPVOID param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_EnumAttachedSurfaces_c

        retn 3*4

; end procedure IDirectDrawSurface_EnumAttachedSurfaces_asm2c


align 16
IDirectDrawSurface_EnumOverlayZOrders_asm2c:

; [esp + 4*4] = LPDDENUMSURFACESCALLBACK param3
; [esp + 3*4] = LPVOID param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack4 IDirectDrawSurface_EnumOverlayZOrders_c

        retn 4*4

; end procedure IDirectDrawSurface_EnumOverlayZOrders_asm2c


align 16
IDirectDrawSurface_Flip_asm2c:

; [esp + 3*4] = DWORD param2
; [esp + 2*4] = LPDIRECTDRAWSURFACE param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_Flip_c

        retn 3*4

; end procedure IDirectDrawSurface_Flip_asm2c


align 16
IDirectDrawSurface_GetAttachedSurface_asm2c:

; [esp + 3*4] = LPDIRECTDRAWSURFACE FAR * param2
; [esp + 2*4] = LPDDSCAPS param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_GetAttachedSurface_c

        retn 3*4

; end procedure IDirectDrawSurface_GetAttachedSurface_asm2c


align 16
IDirectDrawSurface_GetBltStatus_asm2c:

; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_GetBltStatus_c

        retn 2*4

; end procedure IDirectDrawSurface_GetBltStatus_asm2c


align 16
IDirectDrawSurface_GetCaps_asm2c:

; [esp + 2*4] = LPDDSCAPS param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_GetCaps_c

        retn 2*4

; end procedure IDirectDrawSurface_GetCaps_asm2c


align 16
IDirectDrawSurface_GetClipper_asm2c:

; [esp + 2*4] = LPDIRECTDRAWCLIPPER FAR* param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_GetClipper_c

        retn 2*4

; end procedure IDirectDrawSurface_GetClipper_asm2c


align 16
IDirectDrawSurface_GetColorKey_asm2c:

; [esp + 3*4] = LPDDCOLORKEY param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_GetColorKey_c

        retn 3*4

; end procedure IDirectDrawSurface_GetColorKey_asm2c


align 16
IDirectDrawSurface_GetDC_asm2c:

; [esp + 2*4] = HDC FAR * param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_GetDC_c

        retn 2*4

; end procedure IDirectDrawSurface_GetDC_asm2c


align 16
IDirectDrawSurface_GetFlipStatus_asm2c:

; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_GetFlipStatus_c

        retn 2*4

; end procedure IDirectDrawSurface_GetFlipStatus_asm2c


align 16
IDirectDrawSurface_GetOverlayPosition_asm2c:

; [esp + 3*4] = LPLONG param2
; [esp + 2*4] = LPLONG param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_GetOverlayPosition_c

        retn 3*4

; end procedure IDirectDrawSurface_GetOverlayPosition_asm2c


align 16
IDirectDrawSurface_GetPalette_asm2c:

; [esp + 2*4] = LPDIRECTDRAWPALETTE FAR* param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_GetPalette_c

        retn 2*4

; end procedure IDirectDrawSurface_GetPalette_asm2c


align 16
IDirectDrawSurface_GetPixelFormat_asm2c:

; [esp + 2*4] = LPDDPIXELFORMAT param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_GetPixelFormat_c

        retn 2*4

; end procedure IDirectDrawSurface_GetPixelFormat_asm2c


align 16
IDirectDrawSurface_GetSurfaceDesc_asm2c:

; [esp + 2*4] = LPDDSURFACEDESC param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_GetSurfaceDesc_c

        retn 2*4

; end procedure IDirectDrawSurface_GetSurfaceDesc_asm2c


align 16
IDirectDrawSurface_Initialize_asm2c:

; [esp + 3*4] = LPDDSURFACEDESC param2
; [esp + 2*4] = LPDIRECTDRAW param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_Initialize_c

        retn 3*4

; end procedure IDirectDrawSurface_Initialize_asm2c


align 16
IDirectDrawSurface_IsLost_asm2c:

; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectDrawSurface_IsLost_c

        retn 4

; end procedure IDirectDrawSurface_IsLost_asm2c


align 16
IDirectDrawSurface_Lock_asm2c:

; [esp + 5*4] = HANDLE param4
; [esp + 4*4] = DWORD param3
; [esp + 3*4] = LPDDSURFACEDESC param2
; [esp + 2*4] = LPRECT param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack5 IDirectDrawSurface_Lock_c

        retn 5*4

; end procedure IDirectDrawSurface_Lock_asm2c


align 16
IDirectDrawSurface_ReleaseDC_asm2c:

; [esp + 2*4] = HDC param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_ReleaseDC_c

        retn 2*4

; end procedure IDirectDrawSurface_ReleaseDC_asm2c


align 16
IDirectDrawSurface_Restore_asm2c:

; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectDrawSurface_Restore_c

        retn 4

; end procedure IDirectDrawSurface_Restore_asm2c


align 16
IDirectDrawSurface_SetClipper_asm2c:

; [esp + 2*4] = LPDIRECTDRAWCLIPPER param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_SetClipper_c

        retn 2*4

; end procedure IDirectDrawSurface_SetClipper_asm2c


align 16
IDirectDrawSurface_SetColorKey_asm2c:

; [esp + 3*4] = LPDDCOLORKEY param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_SetColorKey_c

        retn 3*4

; end procedure IDirectDrawSurface_SetColorKey_asm2c


align 16
IDirectDrawSurface_SetOverlayPosition_asm2c:

; [esp + 3*4] = LONG param2
; [esp + 2*4] = LONG param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_SetOverlayPosition_c

        retn 3*4

; end procedure IDirectDrawSurface_SetOverlayPosition_asm2c


align 16
IDirectDrawSurface_SetPalette_asm2c:

; [esp + 2*4] = LPDIRECTDRAWPALETTE param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_SetPalette_c

        retn 2*4

; end procedure IDirectDrawSurface_SetPalette_asm2c


align 16
IDirectDrawSurface_Unlock_asm2c:

; [esp + 2*4] = LPVOID param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_Unlock_c

        retn 2*4

; end procedure IDirectDrawSurface_Unlock_asm2c


align 16
IDirectDrawSurface_UpdateOverlay_asm2c:

; [esp + 6*4] = LPDDOVERLAYFX param5
; [esp + 5*4] = DWORD param4
; [esp + 4*4] = LPRECT param3
; [esp + 3*4] = LPDIRECTDRAWSURFACE param2
; [esp + 2*4] = LPRECT param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack6 IDirectDrawSurface_UpdateOverlay_c

        retn 6*4

; end procedure IDirectDrawSurface_UpdateOverlay_asm2c


align 16
IDirectDrawSurface_UpdateOverlayDisplay_asm2c:

; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectDrawSurface_UpdateOverlayDisplay_c

        retn 2*4

; end procedure IDirectDrawSurface_UpdateOverlayDisplay_asm2c


align 16
IDirectDrawSurface_UpdateOverlayZOrder_asm2c:

; [esp + 3*4] = LPDIRECTDRAWSURFACE param2
; [esp + 2*4] = DWORD param1
; [esp +   4] = struct IDirectDrawSurface *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectDrawSurface_UpdateOverlayZOrder_c

        retn 3*4

; end procedure IDirectDrawSurface_UpdateOverlayZOrder_asm2c



%ifidn __OUTPUT_FORMAT__, elf32
section .rdata progbits alloc noexec nowrite align=8
%else
section .rdata rdata align=8
%endif

IDirectDrawVtbl_asm2c:
dd IDirectDraw_QueryInterface_asm2c
dd IDirectDraw_AddRef_asm2c
dd IDirectDraw_Release_asm2c
dd IDirectDraw_Compact_asm2c
dd IDirectDraw_CreateClipper_asm2c
dd IDirectDraw_CreatePalette_asm2c
dd IDirectDraw_CreateSurface_asm2c
dd IDirectDraw_DuplicateSurface_asm2c
dd IDirectDraw_EnumDisplayModes_asm2c
dd IDirectDraw_EnumSurfaces_asm2c
dd IDirectDraw_FlipToGDISurface_asm2c
dd IDirectDraw_GetCaps_asm2c
dd IDirectDraw_GetDisplayMode_asm2c
dd IDirectDraw_GetFourCCCodes_asm2c
dd IDirectDraw_GetGDISurface_asm2c
dd IDirectDraw_GetMonitorFrequency_asm2c
dd IDirectDraw_GetScanLine_asm2c
dd IDirectDraw_GetVerticalBlankStatus_asm2c
dd IDirectDraw_Initialize_asm2c
dd IDirectDraw_RestoreDisplayMode_asm2c
dd IDirectDraw_SetCooperativeLevel_asm2c
dd IDirectDraw_SetDisplayMode_asm2c
dd IDirectDraw_WaitForVerticalBlank_asm2c

IDirectDrawSurfaceVtbl_asm2c:
dd IDirectDrawSurface_QueryInterface_asm2c
dd IDirectDrawSurface_AddRef_asm2c
dd IDirectDrawSurface_Release_asm2c
dd IDirectDrawSurface_AddAttachedSurface_asm2c
dd IDirectDrawSurface_AddOverlayDirtyRect_asm2c
dd IDirectDrawSurface_Blt_asm2c
dd IDirectDrawSurface_BltBatch_asm2c
dd IDirectDrawSurface_BltFast_asm2c
dd IDirectDrawSurface_DeleteAttachedSurface_asm2c
dd IDirectDrawSurface_EnumAttachedSurfaces_asm2c
dd IDirectDrawSurface_EnumOverlayZOrders_asm2c
dd IDirectDrawSurface_Flip_asm2c
dd IDirectDrawSurface_GetAttachedSurface_asm2c
dd IDirectDrawSurface_GetBltStatus_asm2c
dd IDirectDrawSurface_GetCaps_asm2c
dd IDirectDrawSurface_GetClipper_asm2c
dd IDirectDrawSurface_GetColorKey_asm2c
dd IDirectDrawSurface_GetDC_asm2c
dd IDirectDrawSurface_GetFlipStatus_asm2c
dd IDirectDrawSurface_GetOverlayPosition_asm2c
dd IDirectDrawSurface_GetPalette_asm2c
dd IDirectDrawSurface_GetPixelFormat_asm2c
dd IDirectDrawSurface_GetSurfaceDesc_asm2c
dd IDirectDrawSurface_Initialize_asm2c
dd IDirectDrawSurface_IsLost_asm2c
dd IDirectDrawSurface_Lock_asm2c
dd IDirectDrawSurface_ReleaseDC_asm2c
dd IDirectDrawSurface_Restore_asm2c
dd IDirectDrawSurface_SetClipper_asm2c
dd IDirectDrawSurface_SetColorKey_asm2c
dd IDirectDrawSurface_SetOverlayPosition_asm2c
dd IDirectDrawSurface_SetPalette_asm2c
dd IDirectDrawSurface_Unlock_asm2c
dd IDirectDrawSurface_UpdateOverlay_asm2c
dd IDirectDrawSurface_UpdateOverlayDisplay_asm2c
dd IDirectDrawSurface_UpdateOverlayZOrder_asm2c

