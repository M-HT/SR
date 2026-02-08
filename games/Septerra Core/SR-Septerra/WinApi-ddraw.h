/**
 *
 *  Copyright (C) 2019-2026 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#if !defined(_WINAPI_DDRAW_H_INCLUDED_)
#define _WINAPI_DDRAW_H_INCLUDED_

#include "ptr32.h"
#include "platform.h"

struct IDirectDraw_c;
struct IDirectDrawSurface_c;

struct _ddsurfacedesc;
struct _rect;
struct _ddbltfx;
struct _ddscaps2;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t CCALL DirectDrawCreate_c(void *lpGUID, PTR32(struct IDirectDraw_c) *lplpDD, void *pUnkOuter);

uint32_t CCALL IDirectDraw_QueryInterface_c(struct IDirectDraw_c *lpThis, void * riid, PTR32(void)* ppvObj);
uint32_t CCALL IDirectDraw_AddRef_c(struct IDirectDraw_c *lpThis);
uint32_t CCALL IDirectDraw_Release_c(struct IDirectDraw_c *lpThis);
uint32_t CCALL IDirectDraw_Compact_c(struct IDirectDraw_c *lpThis);
uint32_t CCALL IDirectDraw_CreateClipper_c(struct IDirectDraw_c *lpThis, uint32_t param1, PTR32(void)* param2, void * param3);
uint32_t CCALL IDirectDraw_CreatePalette_c(struct IDirectDraw_c *lpThis, uint32_t param1, void * param2, PTR32(void)* param3, void * param4);
uint32_t CCALL IDirectDraw_CreateSurface_c(struct IDirectDraw_c *lpThis, struct _ddsurfacedesc *lpDDSurfaceDesc, PTR32(struct IDirectDrawSurface_c)* lplpDDSurface, void * pUnkOuter);
uint32_t CCALL IDirectDraw_DuplicateSurface_c(struct IDirectDraw_c *lpThis, struct IDirectDrawSurface_c * param1, PTR32(struct IDirectDrawSurface_c)* param2);
uint32_t CCALL IDirectDraw_EnumDisplayModes_c(struct IDirectDraw_c *lpThis, uint32_t param1, void * param2, void * param3, void * param4);
uint32_t CCALL IDirectDraw_EnumSurfaces_c(struct IDirectDraw_c *lpThis, uint32_t param1, void * param2, void * param3, void * param4);
uint32_t CCALL IDirectDraw_FlipToGDISurface_c(struct IDirectDraw_c *lpThis);
uint32_t CCALL IDirectDraw_GetCaps_c(struct IDirectDraw_c *lpThis, void * param1, void * param2);
uint32_t CCALL IDirectDraw_GetDisplayMode_c(struct IDirectDraw_c *lpThis, void * param1);
uint32_t CCALL IDirectDraw_GetFourCCCodes_c(struct IDirectDraw_c *lpThis, uint32_t * param1, uint32_t * param2);
uint32_t CCALL IDirectDraw_GetGDISurface_c(struct IDirectDraw_c *lpThis, PTR32(struct IDirectDrawSurface_c)* param1);
uint32_t CCALL IDirectDraw_GetMonitorFrequency_c(struct IDirectDraw_c *lpThis, uint32_t * param1);
uint32_t CCALL IDirectDraw_GetScanLine_c(struct IDirectDraw_c *lpThis, uint32_t * param1);
uint32_t CCALL IDirectDraw_GetVerticalBlankStatus_c(struct IDirectDraw_c *lpThis, uint32_t * param1);
uint32_t CCALL IDirectDraw_Initialize_c(struct IDirectDraw_c *lpThis, void * param1);
uint32_t CCALL IDirectDraw_RestoreDisplayMode_c(struct IDirectDraw_c *lpThis);
uint32_t CCALL IDirectDraw_SetCooperativeLevel_c(struct IDirectDraw_c *lpThis, void * hWnd, uint32_t dwFlags);
uint32_t CCALL IDirectDraw_SetDisplayMode_c(struct IDirectDraw_c *lpThis, uint32_t dwWidth, uint32_t dwHeight, uint32_t dwBPP);
uint32_t CCALL IDirectDraw_WaitForVerticalBlank_c(struct IDirectDraw_c *lpThis, uint32_t param1, void * param2);

uint32_t CCALL IDirectDrawSurface_QueryInterface_c(struct IDirectDrawSurface_c *lpThis, void * riid, PTR32(void)* ppvObj);
uint32_t CCALL IDirectDrawSurface_AddRef_c(struct IDirectDrawSurface_c *lpThis);
uint32_t CCALL IDirectDrawSurface_Release_c(struct IDirectDrawSurface_c *lpThis);
uint32_t CCALL IDirectDrawSurface_AddAttachedSurface_c(struct IDirectDrawSurface_c *lpThis, struct IDirectDrawSurface_c * param1);
uint32_t CCALL IDirectDrawSurface_AddOverlayDirtyRect_c(struct IDirectDrawSurface_c *lpThis, void * param1);
uint32_t CCALL IDirectDrawSurface_Blt_c(struct IDirectDrawSurface_c *lpThis, struct _rect *lpDestRect, struct IDirectDrawSurface_c * lpDDSrcSurface, struct _rect *lpSrcRect, uint32_t dwFlags, struct _ddbltfx * lpDDBltFX);
uint32_t CCALL IDirectDrawSurface_BltBatch_c(struct IDirectDrawSurface_c *lpThis, void * param1, uint32_t param2, uint32_t param3);
uint32_t CCALL IDirectDrawSurface_BltFast_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, uint32_t param2, struct IDirectDrawSurface_c * param3, void * param4, uint32_t param5);
uint32_t CCALL IDirectDrawSurface_DeleteAttachedSurface_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, struct IDirectDrawSurface_c * param2);
uint32_t CCALL IDirectDrawSurface_EnumAttachedSurfaces_c(struct IDirectDrawSurface_c *lpThis, void * param1, void * param2);
uint32_t CCALL IDirectDrawSurface_EnumOverlayZOrders_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, void * param2, void * param3);
uint32_t CCALL IDirectDrawSurface_Flip_c(struct IDirectDrawSurface_c *lpThis, struct IDirectDrawSurface_c * lpDDSurfaceTargetOverride, uint32_t dwFlags);
uint32_t CCALL IDirectDrawSurface_GetAttachedSurface_c(struct IDirectDrawSurface_c *lpThis, struct _ddscaps2 *lpDDSCaps, PTR32(struct IDirectDrawSurface_c)* lplpDDAttachedSurface);
uint32_t CCALL IDirectDrawSurface_GetBltStatus_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1);
uint32_t CCALL IDirectDrawSurface_GetCaps_c(struct IDirectDrawSurface_c *lpThis, void * param1);
uint32_t CCALL IDirectDrawSurface_GetClipper_c(struct IDirectDrawSurface_c *lpThis, PTR32(void)* param1);
uint32_t CCALL IDirectDrawSurface_GetColorKey_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, void * param2);
uint32_t CCALL IDirectDrawSurface_GetDC_c(struct IDirectDrawSurface_c *lpThis, PTR32(void)* lphDC);
uint32_t CCALL IDirectDrawSurface_GetFlipStatus_c(struct IDirectDrawSurface_c *lpThis, uint32_t dwFlags);
uint32_t CCALL IDirectDrawSurface_GetOverlayPosition_c(struct IDirectDrawSurface_c *lpThis, int32_t * param1, int32_t * param2);
uint32_t CCALL IDirectDrawSurface_GetPalette_c(struct IDirectDrawSurface_c *lpThis, PTR32(void)* param1);
uint32_t CCALL IDirectDrawSurface_GetPixelFormat_c(struct IDirectDrawSurface_c *lpThis, void * param1);
uint32_t CCALL IDirectDrawSurface_GetSurfaceDesc_c(struct IDirectDrawSurface_c *lpThis, struct _ddsurfacedesc *lpDDSurfaceDesc);
uint32_t CCALL IDirectDrawSurface_Initialize_c(struct IDirectDrawSurface_c *lpThis, struct IDirectDraw_c * param1, void * param2);
uint32_t CCALL IDirectDrawSurface_IsLost_c(struct IDirectDrawSurface_c *lpThis);
uint32_t CCALL IDirectDrawSurface_Lock_c(struct IDirectDrawSurface_c *lpThis, void * lpDestRect, struct _ddsurfacedesc *lpDDSurfaceDesc, uint32_t dwFlags, void * hEvent);
uint32_t CCALL IDirectDrawSurface_ReleaseDC_c(struct IDirectDrawSurface_c *lpThis, void * param1);
uint32_t CCALL IDirectDrawSurface_Restore_c(struct IDirectDrawSurface_c *lpThis);
uint32_t CCALL IDirectDrawSurface_SetClipper_c(struct IDirectDrawSurface_c *lpThis, void * param1);
uint32_t CCALL IDirectDrawSurface_SetColorKey_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, void * param2);
uint32_t CCALL IDirectDrawSurface_SetOverlayPosition_c(struct IDirectDrawSurface_c *lpThis, int32_t param1, int32_t param2);
uint32_t CCALL IDirectDrawSurface_SetPalette_c(struct IDirectDrawSurface_c *lpThis, void * param1);
uint32_t CCALL IDirectDrawSurface_Unlock_c(struct IDirectDrawSurface_c *lpThis, void * lpRect);
uint32_t CCALL IDirectDrawSurface_UpdateOverlay_c(struct IDirectDrawSurface_c *lpThis, void * param1, struct IDirectDrawSurface_c * param2, void * param3, uint32_t param4, void * param5);
uint32_t CCALL IDirectDrawSurface_UpdateOverlayDisplay_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1);
uint32_t CCALL IDirectDrawSurface_UpdateOverlayZOrder_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, struct IDirectDrawSurface_c * param2);

#ifdef __cplusplus
}
#endif

#endif

