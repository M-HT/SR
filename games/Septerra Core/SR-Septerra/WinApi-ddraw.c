/**
 *
 *  Copyright (C) 2019-2023 Roman Pauer
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

#include <inttypes.h>
#include "WinApi-ddraw.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "Game-Config.h"


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


#define make_hresult(s,f,c) ((uint32_t)(((unsigned long)(s)<<31)|((unsigned long)(f)<<16)|((unsigned long)(c))))

#define E_NOINTERFACE ((uint32_t)0x80004002L)
#define E_POINTER ((uint32_t)0x80004003L)
#define E_OUTOFMEMORY ((uint32_t)0x8007000EL)
#define E_INVALIDARG ((uint32_t)0x80070057L)

#define DD_OK 0

#define _FACDD  0x876
#define MAKE_DDHRESULT( code )  make_hresult( 1, _FACDD, code )

#define DDERR_OUTOFMEMORY                       E_OUTOFMEMORY
#define DDERR_INVALIDPARAMS                     E_INVALIDARG
#define DDERR_OUTOFVIDEOMEMORY                  MAKE_DDHRESULT( 380 )
#define DDERR_SURFACEBUSY                       MAKE_DDHRESULT( 430 )
#define DDERR_SURFACELOST                       MAKE_DDHRESULT( 450 )
#define DDERR_UNSUPPORTEDMODE                   MAKE_DDHRESULT( 590 )

#define DDSCL_FULLSCREEN                        0x00000001l
#define DDSCL_EXCLUSIVE                         0x00000010l

#define DDSCAPS_RESERVED1                       0x00000001l
#define DDSCAPS_ALPHA                           0x00000002l
#define DDSCAPS_BACKBUFFER                      0x00000004l
#define DDSCAPS_COMPLEX                         0x00000008l
#define DDSCAPS_FLIP                            0x00000010l
#define DDSCAPS_FRONTBUFFER                     0x00000020l
#define DDSCAPS_OFFSCREENPLAIN                  0x00000040l
#define DDSCAPS_OVERLAY                         0x00000080l
#define DDSCAPS_PALETTE                         0x00000100l
#define DDSCAPS_PRIMARYSURFACE                  0x00000200l
#define DDSCAPS_RESERVED3                       0x00000400l
#define DDSCAPS_PRIMARYSURFACELEFT              0x00000000l
#define DDSCAPS_SYSTEMMEMORY                    0x00000800l
#define DDSCAPS_TEXTURE                         0x00001000l
#define DDSCAPS_3DDEVICE                        0x00002000l
#define DDSCAPS_VIDEOMEMORY                     0x00004000l
#define DDSCAPS_VISIBLE                         0x00008000l
#define DDSCAPS_WRITEONLY                       0x00010000l
#define DDSCAPS_ZBUFFER                         0x00020000l
#define DDSCAPS_OWNDC                           0x00040000l
#define DDSCAPS_LIVEVIDEO                       0x00080000l
#define DDSCAPS_HWCODEC                         0x00100000l
#define DDSCAPS_MODEX                           0x00200000l
#define DDSCAPS_MIPMAP                          0x00400000l
#define DDSCAPS_RESERVED2                       0x00800000l
#define DDSCAPS_ALLOCONLOAD                     0x04000000l
#define DDSCAPS_VIDEOPORT                       0x08000000l
#define DDSCAPS_LOCALVIDMEM                     0x10000000l
#define DDSCAPS_NONLOCALVIDMEM                  0x20000000l
#define DDSCAPS_STANDARDVGAMODE                 0x40000000l
#define DDSCAPS_OPTIMIZED                       0x80000000l

#define DDSD_CAPS               0x00000001l     // default
#define DDSD_HEIGHT             0x00000002l
#define DDSD_WIDTH              0x00000004l
#define DDSD_PITCH              0x00000008l
#define DDSD_BACKBUFFERCOUNT    0x00000020l
#define DDSD_ZBUFFERBITDEPTH    0x00000040l
#define DDSD_ALPHABITDEPTH      0x00000080l
#define DDSD_LPSURFACE          0x00000800l
#define DDSD_PIXELFORMAT        0x00001000l
#define DDSD_CKDESTOVERLAY      0x00002000l
#define DDSD_CKDESTBLT          0x00004000l
#define DDSD_CKSRCOVERLAY       0x00008000l
#define DDSD_CKSRCBLT           0x00010000l
#define DDSD_MIPMAPCOUNT        0x00020000l
#define DDSD_REFRESHRATE        0x00040000l
#define DDSD_LINEARSIZE         0x00080000l
#define DDSD_TEXTURESTAGE       0x00100000l
#define DDSD_FVF                0x00200000l
#define DDSD_SRCVBHANDLE        0x00400000l
#define DDSD_DEPTH              0x00800000l
#define DDSD_ALL                0x00fff9eel

#define DDPF_PALETTEINDEXED8                    0x00000020l
#define DDPF_RGB                                0x00000040l

#define DDLOCK_WAIT                             0x00000001L

#define DDBLT_ALPHADEST                         0x00000001l
#define DDBLT_ALPHADESTCONSTOVERRIDE            0x00000002l
#define DDBLT_ALPHADESTNEG                      0x00000004l
#define DDBLT_ALPHADESTSURFACEOVERRIDE          0x00000008l
#define DDBLT_ALPHAEDGEBLEND                    0x00000010l
#define DDBLT_ALPHASRC                          0x00000020l
#define DDBLT_ALPHASRCCONSTOVERRIDE             0x00000040l
#define DDBLT_ALPHASRCNEG                       0x00000080l
#define DDBLT_ALPHASRCSURFACEOVERRIDE           0x00000100l
#define DDBLT_ASYNC                             0x00000200l
#define DDBLT_COLORFILL                         0x00000400l
#define DDBLT_DDFX                              0x00000800l
#define DDBLT_DDROPS                            0x00001000l
#define DDBLT_KEYDEST                           0x00002000l
#define DDBLT_KEYDESTOVERRIDE                   0x00004000l
#define DDBLT_KEYSRC                            0x00008000l
#define DDBLT_KEYSRCOVERRIDE                    0x00010000l
#define DDBLT_ROP                               0x00020000l
#define DDBLT_ROTATIONANGLE                     0x00040000l
#define DDBLT_ZBUFFER                           0x00080000l
#define DDBLT_ZBUFFERDESTCONSTOVERRIDE          0x00100000l
#define DDBLT_ZBUFFERDESTOVERRIDE               0x00200000l
#define DDBLT_ZBUFFERSRCCONSTOVERRIDE           0x00400000l
#define DDBLT_ZBUFFERSRCOVERRIDE                0x00800000l
#define DDBLT_WAIT                              0x01000000l
#define DDBLT_DEPTHFILL                         0x02000000l
#define DDBLT_DONOTWAIT                         0x08000000l

#define SRCCOPY             0x00CC0020


struct IDirectDraw_c {
    PTR32(void) lpVtbl;
    uint32_t RefCount;
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_Window *Window;
    SDL_Renderer *Renderer;
    SDL_Texture *Texture[3];
#else
    SDL_Surface *Screen;
#endif
};

struct IDirectDrawSurface_c {
    PTR32(void) lpVtbl;
    uint32_t RefCount;
#if SDL_VERSION_ATLEAST(2,0,0)
    int current_texture;
    SDL_Renderer *Renderer;
    SDL_Texture *Texture[3];
#endif
    SDL_Surface *Surface;
    int primary, backbuffer, mustlock, was_flipped;
    struct IDirectDrawSurface_c *lpBackbuffer;
};


typedef struct _ddcolorkey {
    uint32_t dwColorSpaceLowValue;
    uint32_t dwColorSpaceHighValue;
} ddcolorkey, *lpddcolorkey;

typedef struct _rect {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} rect, *lprect;

typedef struct _ddpixelformat {
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    union
    {
        uint32_t dwRGBBitCount;
        uint32_t dwYUVBitCount;
        uint32_t dwAlphaBitDepth;
    };
    union
    {
        uint32_t dwRBitMask;
        uint32_t dwYBitMask;
    };
    union
    {
        uint32_t dwGBitMask;
        uint32_t dwUBitMask;
    };
    union
    {
        uint32_t dwBBitMask;
        uint32_t dwVBitMask;
    };
    union
    {
        uint32_t dwRGBAlphaBitMask;
    };
} ddpixelformat, *lpddpixelformat;

typedef struct _ddscaps {
    uint32_t dwCaps;
} ddscaps, *lpddscaps;

typedef struct _ddscaps2 {
    uint32_t dwCaps;
    uint32_t dwCaps2;
    uint32_t dwCaps3;
    uint32_t dwCaps4;
} ddscaps2, *lpddscaps2;

typedef struct _ddsurfacedesc {
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwHeight;
    uint32_t dwWidth;
    int32_t lPitch;
    uint32_t dwBackBufferCount;
    uint32_t dwRefreshRate;
    uint32_t dwAlphaBitDepth;
    uint32_t dwReserved;
    PTR32(void) lpSurface;
    ddcolorkey ddckCKDestOverlay;
    ddcolorkey ddckCKDestBlt;
    ddcolorkey ddckCKSrcOverlay;
    ddcolorkey ddckCKSrcBlt;
    ddpixelformat ddpfPixelFormat;
    ddscaps ddsCaps;
} ddsurfacedesc, *lpddsurfacedesc;

typedef struct _ddbltfx
{
    uint32_t dwSize;
    uint32_t dwDDFX;
    uint32_t dwROP;
    uint32_t dwDDROP;
    uint32_t dwRotationAngle;
    uint32_t dwZBufferOpCode;
    uint32_t dwZBufferLow;
    uint32_t dwZBufferHigh;
    uint32_t dwZBufferBaseDest;
    uint32_t dwZDestConstBitDepth;
    union
    {
        uint32_t dwZDestConst;
        PTR32(struct IDirectDrawSurface_c) lpDDSZBufferDest;
    };
    uint32_t dwZSrcConstBitDepth;
    union
    {
        uint32_t dwZSrcConst;
        PTR32(struct IDirectDrawSurface_c) lpDDSZBufferSrc;
    };
    uint32_t dwAlphaEdgeBlendBitDepth;
    uint32_t dwAlphaEdgeBlend;
    uint32_t dwReserved;
    uint32_t dwAlphaDestConstBitDepth;
    union
    {
        uint32_t dwAlphaDestConst;
        PTR32(struct IDirectDrawSurface_c) lpDDSAlphaDest;
    };
    uint32_t dwAlphaSrcConstBitDepth;
    union
    {
        uint32_t dwAlphaSrcConst;
        PTR32(struct IDirectDrawSurface_c) lpDDSAlphaSrc;
    };
    union
    {
        uint32_t dwFillColor;
        uint32_t dwFillDepth;
        uint32_t dwFillPixel;
        PTR32(struct IDirectDrawSurface_c) lpDDSPattern;
    };
    ddcolorkey ddckDestColorkey;
    ddcolorkey ddckSrcColorkey;
} ddbltfx;


extern uint32_t IDirectDrawVtbl_asm2c;
extern uint32_t IDirectDrawSurfaceVtbl_asm2c;

extern int sdl_versionnum;


#if SDL_VERSION_ATLEAST(2,0,0)
#ifdef __cplusplus
extern "C"
#endif
SDL_Renderer *GetSurfaceRenderer(struct IDirectDrawSurface_c *lpThis)
{
    return lpThis->Renderer;
}
#endif


uint32_t DirectDrawCreate_c(void *lpGUID, PTR32(struct IDirectDraw_c) *lplpDD, void *pUnkOuter)
{
    struct IDirectDraw_c *lpDD_c;

#ifdef DEBUG_DDRAW
    eprintf("DirectDrawCreate: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t)lpGUID, (uintptr_t)lplpDD, (uintptr_t)pUnkOuter);
#endif

    if ((lpGUID != NULL) || (pUnkOuter != NULL))
    {
        return DDERR_INVALIDPARAMS;
    }

    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        eprintf("Error: SDL_InitSubSystem: Video: %s\n", SDL_GetError());
        exit(1);
    }

    lpDD_c = (struct IDirectDraw_c *)malloc(sizeof(struct IDirectDraw_c));

    if (lpDD_c == NULL)
    {
        return DDERR_OUTOFMEMORY;
    }

    lpDD_c->lpVtbl = &IDirectDrawVtbl_asm2c;
    lpDD_c->RefCount = 1;

    *lplpDD = lpDD_c;

#ifdef DEBUG_DDRAW
    eprintf("OK: 0x%" PRIxPTR "\n", (uintptr_t)lpDD_c);
#endif

    return DD_OK;
}


uint32_t IDirectDraw_QueryInterface_c(struct IDirectDraw_c *lpThis, void * riid, PTR32(void)* ppvObj)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDraw_QueryInterface: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR "\n", (uintptr_t)lpThis, (uintptr_t)riid, (uintptr_t)ppvObj);
#endif

    if (ppvObj == NULL) return E_POINTER;
    return E_NOINTERFACE;
}

uint32_t IDirectDraw_AddRef_c(struct IDirectDraw_c *lpThis)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDraw_AddRef: 0x%" PRIxPTR " - ", (uintptr_t)lpThis);
#endif

    if (lpThis != NULL)
    {
        lpThis->RefCount++;
#ifdef DEBUG_DDRAW
        eprintf("%i\n", lpThis->RefCount);
#endif
        return lpThis->RefCount;
    }

#ifdef DEBUG_DDRAW
    eprintf("%i\n", 0);
#endif

    return 0;
}

uint32_t IDirectDraw_Release_c(struct IDirectDraw_c *lpThis)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDraw_Release: 0x%" PRIxPTR " - ", (uintptr_t)lpThis);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("%i\n", 0);
#endif
        return 0;
    }

    lpThis->RefCount--;
#ifdef DEBUG_DDRAW
    eprintf("%i\n", lpThis->RefCount);
#endif
    if (lpThis->RefCount == 0)
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        if (lpThis->Texture[0] != NULL)
        {
            SDL_DestroyTexture(lpThis->Texture[2]);
            lpThis->Texture[2] = NULL;
            SDL_DestroyTexture(lpThis->Texture[1]);
            lpThis->Texture[1] = NULL;
            SDL_DestroyTexture(lpThis->Texture[0]);
            lpThis->Texture[0] = NULL;
        }
        if (lpThis->Renderer != NULL)
        {
            SDL_DestroyRenderer(lpThis->Renderer);
            lpThis->Renderer = NULL;
        }
        if (lpThis->Window != NULL)
        {
            SDL_DestroyWindow(lpThis->Window);
            lpThis->Window = NULL;
        }
#endif
        free(lpThis);
        return 0;
    }
    return lpThis->RefCount;
}

uint32_t IDirectDraw_Compact_c(struct IDirectDraw_c *lpThis)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_Compact");
    exit(1);
//    return IDirectDraw_Compact(lpThis->lpObject);
}

uint32_t IDirectDraw_CreateClipper_c(struct IDirectDraw_c *lpThis, uint32_t param1, PTR32(void)* param2, void * param3)
{
    eprintf("Unsupported method: %s\n", "IDirectDraw_CreateClipper");
    exit(1);
}

uint32_t IDirectDraw_CreatePalette_c(struct IDirectDraw_c *lpThis, uint32_t param1, void * param2, PTR32(void)* param3, void * param4)
{
    eprintf("Unsupported method: %s\n", "IDirectDraw_CreatePalette");
    exit(1);
}

uint32_t IDirectDraw_CreateSurface_c(struct IDirectDraw_c *lpThis, struct _ddsurfacedesc *lpDDSurfaceDesc, PTR32(struct IDirectDrawSurface_c)* lplpDDSurface, void * pUnkOuter)
{
    struct IDirectDrawSurface_c *lpDDS_c;

#ifdef DEBUG_DDRAW
    eprintf("IDirectDraw_CreateSurface: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t)lpThis, (uintptr_t)lpDDSurfaceDesc, (uintptr_t)lplpDDSurface, (uintptr_t)pUnkOuter);
#endif

    if ((lpThis == NULL) || (lpDDSurfaceDesc == NULL) || (lplpDDSurface == NULL) || (pUnkOuter != NULL))
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

#ifdef DEBUG_DDRAW
    eprintf("%i, %i, %i, %i, 0x%" PRIxPTR ", 0x%x, 0x%x - ", lpDDSurfaceDesc->dwSize, lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight, lpDDSurfaceDesc->dwBackBufferCount, (uintptr_t)lpDDSurfaceDesc->lpSurface, lpDDSurfaceDesc->dwFlags, lpDDSurfaceDesc->ddsCaps.dwCaps);
#endif

    if ((lpDDSurfaceDesc->dwSize != 108) || ((lpDDSurfaceDesc->dwFlags & DDSD_CAPS) == 0))
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

    if (lpDDSurfaceDesc->ddsCaps.dwCaps == (DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP))
    {
        if ((lpDDSurfaceDesc->dwFlags != (DDSD_CAPS | DDSD_BACKBUFFERCOUNT)) ||
            (lpDDSurfaceDesc->dwBackBufferCount != 1)
           )
        {
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_INVALIDPARAMS;
        }

        lpDDS_c = (struct IDirectDrawSurface_c *)malloc(sizeof(struct IDirectDrawSurface_c));

        if (lpDDS_c == NULL)
        {
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_OUTOFMEMORY;
        }

        lpDDS_c->lpVtbl = &IDirectDrawSurfaceVtbl_asm2c;
        lpDDS_c->RefCount = 1;
        lpDDS_c->primary = 1;
        lpDDS_c->backbuffer = 0;
        lpDDS_c->was_flipped = 0;

#if SDL_VERSION_ATLEAST(2,0,0)
        lpDDS_c->current_texture = 0;
        lpDDS_c->Renderer = lpThis->Renderer;
        lpDDS_c->Texture[0] = lpThis->Texture[0];
        lpDDS_c->Texture[1] = lpThis->Texture[1];
        lpDDS_c->Texture[2] = lpThis->Texture[2];
        lpDDS_c->Surface = NULL;
        lpDDS_c->mustlock = 1;
#else
        lpDDS_c->Surface = lpThis->Screen;
        lpDDS_c->mustlock = SDL_MUSTLOCK(lpDDS_c->Surface);
#endif

        lpDDS_c->lpBackbuffer = (struct IDirectDrawSurface_c *)malloc(sizeof(struct IDirectDrawSurface_c));

        if (lpDDS_c->lpBackbuffer == NULL)
        {
            free(lpDDS_c);
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_OUTOFMEMORY;
        }

        lpDDS_c->lpBackbuffer->lpVtbl = &IDirectDrawSurfaceVtbl_asm2c;
        lpDDS_c->lpBackbuffer->RefCount = 1;
        lpDDS_c->lpBackbuffer->primary = 0;
        lpDDS_c->lpBackbuffer->backbuffer = 1;
        lpDDS_c->lpBackbuffer->was_flipped = 0;
        lpDDS_c->lpBackbuffer->lpBackbuffer = NULL;

#if SDL_VERSION_ATLEAST(2,0,0)
        Uint32 format;
        int width, height;

        if (0 == SDL_QueryTexture(lpThis->Texture[0], &format, NULL, &width, &height))
        {
            int bpp;
            Uint32 Rmask, Gmask, Bmask, Amask;
            if (SDL_PixelFormatEnumToMasks(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask))
            {
                lpDDS_c->lpBackbuffer->Surface = SDL_CreateRGBSurface(
                    SDL_SWSURFACE,
                    width,
                    height,
                    bpp,
                    Rmask,
                    Gmask,
                    Bmask,
                    Amask
                );
            }
        }
#else
        lpDDS_c->lpBackbuffer->Surface = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            lpThis->Screen->w,
            lpThis->Screen->h,
            lpThis->Screen->format->BitsPerPixel,
            lpThis->Screen->format->Rmask,
            lpThis->Screen->format->Gmask,
            lpThis->Screen->format->Bmask,
            lpThis->Screen->format->Amask
        );
#endif

        if (lpDDS_c->lpBackbuffer->Surface == NULL)
        {
            free(lpDDS_c->lpBackbuffer);
            free(lpDDS_c);
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_OUTOFVIDEOMEMORY;
        }

        lpDDS_c->lpBackbuffer->mustlock = SDL_MUSTLOCK(lpDDS_c->lpBackbuffer->Surface);

#if SDL_VERSION_ATLEAST(2,0,0)
        lpDDS_c->Surface = SDL_CreateRGBSurfaceFrom(
            NULL,
            lpDDS_c->lpBackbuffer->Surface->w,
            lpDDS_c->lpBackbuffer->Surface->h,
            lpDDS_c->lpBackbuffer->Surface->format->BitsPerPixel,
            lpDDS_c->lpBackbuffer->Surface->pitch,
            lpDDS_c->lpBackbuffer->Surface->format->Rmask,
            lpDDS_c->lpBackbuffer->Surface->format->Gmask,
            lpDDS_c->lpBackbuffer->Surface->format->Bmask,
            lpDDS_c->lpBackbuffer->Surface->format->Amask
        );

        if (lpDDS_c->Surface == NULL)
        {
            SDL_FreeSurface(lpDDS_c->lpBackbuffer->Surface);
            free(lpDDS_c->lpBackbuffer);
            free(lpDDS_c);
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_OUTOFVIDEOMEMORY;
        }
#endif

        *lplpDDSurface = lpDDS_c;

#ifdef DEBUG_DDRAW
        eprintf("OK: 0x%" PRIxPTR " (0x%" PRIxPTR ")\n", (uintptr_t)lpDDS_c, (uintptr_t)lpDDS_c->lpBackbuffer);
#endif
        return DD_OK;
    }

    if (lpDDSurfaceDesc->ddsCaps.dwCaps == (DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY))
    {
        if (lpDDSurfaceDesc->dwFlags != (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT))
        {
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_INVALIDPARAMS;
        }

#if defined(__DEBUG__)
        eprintf("%i, 0x%x, %i, 0x%x, 0x%x, 0x%x - ", lpDDSurfaceDesc->ddpfPixelFormat.dwSize, lpDDSurfaceDesc->ddpfPixelFormat.dwFlags, lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount, lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask, lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask, lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask);
#endif

        if ((lpDDSurfaceDesc->ddpfPixelFormat.dwSize != 32) ||
            (lpDDSurfaceDesc->ddpfPixelFormat.dwFlags != DDPF_RGB)
           )
        {
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_INVALIDPARAMS;
        }

        lpDDS_c = (struct IDirectDrawSurface_c *)malloc(sizeof(struct IDirectDrawSurface_c));

        if (lpDDS_c == NULL)
        {
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_OUTOFMEMORY;
        }

        lpDDS_c->lpVtbl = &IDirectDrawSurfaceVtbl_asm2c;
        lpDDS_c->RefCount = 1;
        lpDDS_c->primary = 0;
        lpDDS_c->backbuffer = 0;
        lpDDS_c->was_flipped = 0;
        lpDDS_c->lpBackbuffer = NULL;

        lpDDS_c->Surface = SDL_CreateRGBSurface(
            SDL_SWSURFACE,
            lpDDSurfaceDesc->dwWidth,
            lpDDSurfaceDesc->dwHeight,
            lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount,
            lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask,
            lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask,
            lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask,
            0
        );

        if (lpDDS_c->Surface == NULL)
        {
            free(lpDDS_c);
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_OUTOFVIDEOMEMORY;
        }

        lpDDS_c->mustlock = SDL_MUSTLOCK(lpDDS_c->Surface);

        *lplpDDSurface = lpDDS_c;

#ifdef DEBUG_DDRAW
        eprintf("OK: 0x%" PRIxPTR "\n", (uintptr_t)lpDDS_c);
#endif
        return DD_OK;
    }

    eprintf("Unsupported method: %s\n", "IDirectDraw_CreateSurface");
    exit(1);
}

uint32_t IDirectDraw_DuplicateSurface_c(struct IDirectDraw_c *lpThis, struct IDirectDrawSurface_c * param1, PTR32(struct IDirectDrawSurface_c)* param2)
{
    eprintf("Unsupported method: %s\n", "IDirectDraw_DuplicateSurface");
    exit(1);
}

uint32_t IDirectDraw_EnumDisplayModes_c(struct IDirectDraw_c *lpThis, uint32_t param1, void * param2, void * param3, void * param4)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_EnumDisplayModes");
    exit(1);
//    return IDirectDraw_EnumDisplayModes(lpThis->lpObject, param1, (LPDDSURFACEDESC)param2, param3, (LPDDENUMMODESCALLBACK)param4);
}

uint32_t IDirectDraw_EnumSurfaces_c(struct IDirectDraw_c *lpThis, uint32_t param1, void * param2, void * param3, void * param4)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_EnumSurfaces");
    exit(1);
//    return IDirectDraw_EnumSurfaces(lpThis->lpObject, param1, (LPDDSURFACEDESC)param2, param3, (LPDDENUMSURFACESCALLBACK)param4);
}

uint32_t IDirectDraw_FlipToGDISurface_c(struct IDirectDraw_c *lpThis)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDraw_FlipToGDISurface: 0x%" PRIxPTR "\n", (uintptr_t) lpThis);
#endif

    return DD_OK;
}

uint32_t IDirectDraw_GetCaps_c(struct IDirectDraw_c *lpThis, void * param1, void * param2)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_GetCaps");
    exit(1);
//    return IDirectDraw_GetCaps(lpThis->lpObject, (LPDDCAPS)param1, (LPDDCAPS)param2);
}

uint32_t IDirectDraw_GetDisplayMode_c(struct IDirectDraw_c *lpThis, void * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_GetDisplayMode");
    exit(1);
//    return IDirectDraw_GetDisplayMode(lpThis->lpObject, (LPDDSURFACEDESC)param1);
}

uint32_t IDirectDraw_GetFourCCCodes_c(struct IDirectDraw_c *lpThis, uint32_t * param1, uint32_t * param2)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_GetFourCCCodes");
    exit(1);
//    return IDirectDraw_GetFourCCCodes(lpThis->lpObject, (LPDWORD)param1, (LPDWORD)param2);
}

uint32_t IDirectDraw_GetGDISurface_c(struct IDirectDraw_c *lpThis, PTR32(struct IDirectDrawSurface_c)* param1)
{
    eprintf("Unsupported method: %s\n", "IDirectDraw_GetGDISurface");
    exit(1);
}

uint32_t IDirectDraw_GetMonitorFrequency_c(struct IDirectDraw_c *lpThis, uint32_t * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_GetMonitorFrequency");
    exit(1);
//    return IDirectDraw_GetMonitorFrequency(lpThis->lpObject, (LPDWORD)param1);
}

uint32_t IDirectDraw_GetScanLine_c(struct IDirectDraw_c *lpThis, uint32_t * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_GetScanLine");
    exit(1);
//    return IDirectDraw_GetScanLine(lpThis->lpObject, (LPDWORD)param1);
}

uint32_t IDirectDraw_GetVerticalBlankStatus_c(struct IDirectDraw_c *lpThis, uint32_t * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_GetVerticalBlankStatus");
    exit(1);
//    return IDirectDraw_GetVerticalBlankStatus(lpThis->lpObject, (LPBOOL)param1);
}

uint32_t IDirectDraw_Initialize_c(struct IDirectDraw_c *lpThis, void * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_Initialize");
    exit(1);
//    return IDirectDraw_Initialize(lpThis->lpObject, (GUID FAR *)param1);
}

uint32_t IDirectDraw_RestoreDisplayMode_c(struct IDirectDraw_c *lpThis)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_RestoreDisplayMode");
    exit(1);
//    return IDirectDraw_RestoreDisplayMode(lpThis->lpObject);
}

uint32_t IDirectDraw_SetCooperativeLevel_c(struct IDirectDraw_c *lpThis, void * hWnd, uint32_t dwFlags)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDraw_SetCooperativeLevel: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%x - ", (uintptr_t)lpThis, (uintptr_t)hWnd, dwFlags);
#endif

    if ((lpThis == NULL) || (hWnd == NULL))
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

    if (dwFlags != (DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE))
    {
        eprintf("Unsupported method: %s\n", "IDirectDraw_SetCooperativeLevel");
        exit(1);
    }

#ifdef DEBUG_DDRAW
    eprintf("OK\n");
#endif
    return DD_OK;
}

uint32_t IDirectDraw_SetDisplayMode_c(struct IDirectDraw_c *lpThis, uint32_t dwWidth, uint32_t dwHeight, uint32_t dwBPP)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDraw_SetDisplayMode: 0x%" PRIxPTR ", %i, %i, %i - ", (uintptr_t)lpThis, dwWidth, dwHeight, dwBPP);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    if (dwBPP != 16)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_UNSUPPORTEDMODE;
    }

    lpThis->Window = SDL_CreateWindow("Septerra Core", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (Display_Width != 0)?Display_Width:dwWidth, (Display_Height != 0)?Display_Height:dwHeight, ((Display_Resizable && (Display_Mode == 0))?SDL_WINDOW_RESIZABLE:0) | ((Display_Mode == 1)?SDL_WINDOW_FULLSCREEN_DESKTOP:0) | ((Display_Mode == 2)?SDL_WINDOW_FULLSCREEN:0));
    if (lpThis->Window == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_UNSUPPORTEDMODE;
    }

    lpThis->Renderer = SDL_CreateRenderer(lpThis->Window, -1, (Display_VSync)?SDL_RENDERER_PRESENTVSYNC:0);
    if (lpThis->Renderer == NULL)
    {
        SDL_DestroyWindow(lpThis->Window);
        lpThis->Window = NULL;
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_UNSUPPORTEDMODE;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, (Display_ScalingQuality)?"linear":"nearest");

#if SDL_VERSION_ATLEAST(2,0,5)
    if (Display_IntegerScaling)
    {
        if (sdl_versionnum >= SDL_VERSIONNUM(2,0,5))
        {
            SDL_RenderSetIntegerScale(lpThis->Renderer, SDL_TRUE);
        }
    }
#endif

    if (0 != SDL_RenderSetLogicalSize(lpThis->Renderer, dwWidth, dwHeight))
    {
        SDL_DestroyRenderer(lpThis->Renderer);
        lpThis->Renderer = NULL;
        SDL_DestroyWindow(lpThis->Window);
        lpThis->Window = NULL;
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_UNSUPPORTEDMODE;
    }

    Uint32 window_format, texture_format;
    window_format = SDL_GetWindowPixelFormat(lpThis->Window);
    texture_format = SDL_PIXELFORMAT_RGB565;
    if (window_format != SDL_PIXELFORMAT_UNKNOWN)
    {
        int bpp;
        Uint32 Rmask, Gmask, Bmask, Amask;
        if (SDL_PixelFormatEnumToMasks(window_format, &bpp, &Rmask, &Gmask, &Bmask, &Amask))
        {
            texture_format = (Rmask & 1)?(int)SDL_PACKEDORDER_XBGR:(int)SDL_PIXELFORMAT_RGB565;
        }
    }

    int index;
    for (index = 0; index < 3; index++)
    {
        lpThis->Texture[index] = SDL_CreateTexture(lpThis->Renderer, texture_format, SDL_TEXTUREACCESS_STREAMING, dwWidth, dwHeight);
    }

    if ((lpThis->Texture[0] == NULL) || (lpThis->Texture[1] == NULL) || (lpThis->Texture[2] == NULL))
    {
        for (index = 2; index >= 0; index--)
        {
            if (lpThis->Texture[index] != NULL)
            {
                SDL_DestroyTexture(lpThis->Texture[index]);
                lpThis->Texture[index] = NULL;
            }
        }

        SDL_DestroyRenderer(lpThis->Renderer);
        lpThis->Renderer = NULL;
        SDL_DestroyWindow(lpThis->Window);
        lpThis->Window = NULL;
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_UNSUPPORTEDMODE;
    }
#else
#ifdef PANDORA
    lpThis->Screen = SDL_SetVideoMode(dwWidth, dwHeight, dwBPP, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
#else
    lpThis->Screen = SDL_SetVideoMode(dwWidth, dwHeight, dwBPP, SDL_HWSURFACE);
#endif
    if (lpThis->Screen == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_UNSUPPORTEDMODE;
    }

    SDL_WM_SetCaption("Septerra Core", NULL);
#endif

#if !SDL_VERSION_ATLEAST(2,0,0)
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
#endif

    // Septerra Core waits for focus on start
    {
        SDL_Event event;

#if SDL_VERSION_ATLEAST(2,0,0)
        event.type = SDL_WINDOWEVENT;
        event.window.event = SDL_WINDOWEVENT_FOCUS_GAINED;
#else
        event.type = SDL_ACTIVEEVENT;
        event.active.gain = 1;
        event.active.state = SDL_APPINPUTFOCUS;
#endif

        SDL_PushEvent(&event);
    }

#ifdef DEBUG_DDRAW
    eprintf("OK\n");
#endif

    return DD_OK;
}

uint32_t IDirectDraw_WaitForVerticalBlank_c(struct IDirectDraw_c *lpThis, uint32_t param1, void * param2)
{
    eprintf("Unimplemented: %s\n", "IDirectDraw_WaitForVerticalBlank");
    exit(1);
//    return IDirectDraw_WaitForVerticalBlank(lpThis->lpObject, param1, (HANDLE)param2);
}


uint32_t IDirectDrawSurface_QueryInterface_c(struct IDirectDrawSurface_c *lpThis, void * riid, PTR32(void)* ppvObj)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_QueryInterface: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR "\n", (uintptr_t)lpThis, (uintptr_t)riid, (uintptr_t)ppvObj);
#endif

    if (ppvObj == NULL) return E_POINTER;
    return E_NOINTERFACE;
}

uint32_t IDirectDrawSurface_AddRef_c(struct IDirectDrawSurface_c *lpThis)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_AddRef: 0x%" PRIxPTR " - ", (uintptr_t)lpThis);
#endif

    if (lpThis != NULL)
    {
        lpThis->RefCount++;
#ifdef DEBUG_DDRAW
        eprintf("%i\n", lpThis->RefCount);
#endif
        return lpThis->RefCount;
    }

#ifdef DEBUG_DDRAW
    eprintf("%i\n", 0);
#endif

    return 0;
}

uint32_t IDirectDrawSurface_Release_c(struct IDirectDrawSurface_c *lpThis)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_Release: 0x%" PRIxPTR " - ", (uintptr_t)lpThis);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("%i\n", 0);
#endif
        return 0;
    }

    lpThis->RefCount--;
#ifdef DEBUG_DDRAW
    eprintf("%i\n", lpThis->RefCount);
#endif
    if (lpThis->RefCount == 0)
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        if (lpThis->Surface != NULL)
        {
            SDL_FreeSurface(lpThis->Surface);
            lpThis->Surface = NULL;
        }
#else
        if ((lpThis->Surface != NULL) && !lpThis->primary)
        {
            SDL_FreeSurface(lpThis->Surface);
            lpThis->Surface = NULL;
        }
#endif

        if (lpThis->lpBackbuffer != NULL)
        {
            IDirectDrawSurface_Release_c(lpThis->lpBackbuffer);
            lpThis->lpBackbuffer = NULL;
        }
        free(lpThis);
        return 0;
    }
    return lpThis->RefCount;
}

uint32_t IDirectDrawSurface_AddAttachedSurface_c(struct IDirectDrawSurface_c *lpThis, struct IDirectDrawSurface_c * param1)
{
    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_AddAttachedSurface");
    exit(1);
}

uint32_t IDirectDrawSurface_AddOverlayDirtyRect_c(struct IDirectDrawSurface_c *lpThis, void * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_AddOverlayDirtyRect");
    exit(1);
//    return IDirectDrawSurface_AddOverlayDirtyRect(lpThis->lpObject, (LPRECT)param1);
}

uint32_t IDirectDrawSurface_Blt_c(struct IDirectDrawSurface_c *lpThis, struct _rect *lpDestRect, struct IDirectDrawSurface_c * lpDDSrcSurface, struct _rect *lpSrcRect, uint32_t dwFlags, struct _ddbltfx * lpDDBltFX)
{
    SDL_Rect dstrect, srcrect;
    int fill_first;

#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_Blt: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%x, 0x%" PRIxPTR " - ", (uintptr_t)lpThis, (uintptr_t)lpDestRect, (uintptr_t)lpDDSrcSurface, (uintptr_t)lpSrcRect, dwFlags, (uintptr_t)lpDDBltFX);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

    if (((dwFlags == (DDBLT_ASYNC | DDBLT_COLORFILL)) && (lpDDBltFX != NULL)) ||
        ((dwFlags == (DDBLT_WAIT | DDBLT_COLORFILL)) && (lpDDBltFX != NULL))
       )
    {
#ifdef DEBUG_DDRAW
        eprintf("fill %s: 0x%x - ", ((lpThis->primary)?"frontbuffer":((lpThis->backbuffer)?"backbuffer":"surface")), lpDDBltFX->dwFillColor);

        if (lpDestRect != NULL)
        {
            eprintf("[%i x %i - %i x %i] - ", lpDestRect->left, lpDestRect->top, lpDestRect->right, lpDestRect->bottom);
        }
        else
        {
            eprintf("[whole surface] - ");
        }
#endif

        if (lpDestRect != NULL)
        {
            dstrect.x = lpDestRect->left;
            dstrect.y = lpDestRect->top;
            dstrect.w = lpDestRect->right - lpDestRect->left;
            dstrect.h = lpDestRect->bottom - lpDestRect->top;
        }

#if SDL_VERSION_ATLEAST(2,0,0)
        if (lpThis->primary)
        {
            Uint8 r, g, b;

            SDL_GetRGB(lpDDBltFX->dwFillColor, lpThis->lpBackbuffer->Surface->format, &r, &g, &b);

            SDL_SetRenderDrawColor(lpThis->Renderer, r, g, b, 255);

            if (0 != SDL_RenderDrawRect(lpThis->Renderer, (lpDestRect != NULL)?&dstrect:NULL))
            {
#ifdef DEBUG_DDRAW
                eprintf("error\n");
#endif
                return DDERR_SURFACEBUSY;
            }

            SDL_RenderPresent(lpThis->Renderer);

            // clear next frame
            SDL_SetRenderDrawColor(lpThis->Renderer, 0, 0, 0, 255);
            SDL_RenderClear(lpThis->Renderer);

            lpThis->was_flipped = 0;

#ifdef DEBUG_DDRAW
            eprintf("OK\n");
#endif

            return DD_OK;
        }
#endif

#if !SDL_VERSION_ATLEAST(2,0,0)
        if (lpThis->primary)
        {
            if (lpThis->was_flipped && (lpDestRect != NULL) && (lpThis->Surface->flags & SDL_DOUBLEBUF))
            {
                SDL_BlitSurface(lpThis->lpBackbuffer->Surface, NULL, lpThis->Surface, NULL);
                lpThis->was_flipped--;
            }
            else
            {
                lpThis->was_flipped = 0;
            }
        }
#endif

        if (0 != SDL_FillRect(lpThis->Surface, (lpDestRect != NULL)?&dstrect:NULL, lpDDBltFX->dwFillColor))
        {
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_SURFACEBUSY;
        }

#if !SDL_VERSION_ATLEAST(2,0,0)
        if (lpThis->primary)
        {
            if ((lpDestRect != NULL) && !(lpThis->Surface->flags & SDL_DOUBLEBUF))
            {
                SDL_UpdateRect(lpThis->Surface, dstrect.x, dstrect.y, dstrect.w, dstrect.h);
            }
            else
            {
                SDL_Flip(lpThis->Surface);
            }
        }
#endif

#ifdef DEBUG_DDRAW
        eprintf("OK\n");
#endif

        return DD_OK;
    }

    if (((dwFlags == 0) && (lpDDBltFX == NULL)) ||
        ((dwFlags == (DDBLT_ROP | DDBLT_WAIT)) && (lpDDBltFX != NULL) && (lpDDBltFX->dwROP == SRCCOPY)) ||
        ((dwFlags == (DDBLT_KEYSRCOVERRIDE | DDBLT_WAIT)) && (lpDDBltFX != NULL))
       )
    {
        if (lpDDSrcSurface == NULL)
        {
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_INVALIDPARAMS;
        }

#ifdef DEBUG_DDRAW
        eprintf("copy %s to %s - ", ((lpDDSrcSurface->primary)?"frontbuffer":((lpDDSrcSurface->backbuffer)?"backbuffer":"surface")), ((lpThis->primary)?"frontbuffer":((lpThis->backbuffer)?"backbuffer":"surface")));

        if (lpSrcRect != NULL)
        {
            eprintf("[%i x %i - %i x %i] - ", lpSrcRect->left, lpSrcRect->top, lpSrcRect->right, lpSrcRect->bottom);
        }
        else
        {
            eprintf("[whole surface] - ");
        }

        if (lpDestRect != NULL)
        {
            eprintf("[%i x %i - %i x %i] - ", lpDestRect->left, lpDestRect->top, lpDestRect->right, lpDestRect->bottom);
        }
        else
        {
            eprintf("[whole surface] - ");
        }
#endif

        if (lpDDSrcSurface->primary)
        {
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_SURFACEBUSY;
        }

        if (lpSrcRect != NULL)
        {
            srcrect.x = lpSrcRect->left;
            srcrect.y = lpSrcRect->top;
            srcrect.w = lpSrcRect->right - lpSrcRect->left;
            srcrect.h = lpSrcRect->bottom - lpSrcRect->top;
        }

        fill_first = 0;
        if (lpDestRect != NULL)
        {
            dstrect.x = lpDestRect->left;
            dstrect.y = lpDestRect->top;
            dstrect.w = lpDestRect->right - lpDestRect->left;
            dstrect.h = lpDestRect->bottom - lpDestRect->top;

            if (lpSrcRect != NULL)
            {
                if (srcrect.w > dstrect.w) srcrect.w = dstrect.w;
                if (srcrect.h > dstrect.h) srcrect.h = dstrect.h;

                if ((dstrect.w > srcrect.w) ||
                    (dstrect.h > srcrect.h)
                   )
                {
                    fill_first = 1;
                }
            }
            else
            {
                srcrect.x = 0;
                srcrect.y = 0;
                srcrect.w = dstrect.w;
                srcrect.h = dstrect.h;

                if ((dstrect.w > lpDDSrcSurface->Surface->w) ||
                    (dstrect.h > lpDDSrcSurface->Surface->h)
                   )
                {
                    fill_first = 1;
                }
            }
        }

#if SDL_VERSION_ATLEAST(2,0,0)
        if (lpThis->primary)
        {
            if (0 != SDL_LockTexture(lpThis->Texture[lpThis->current_texture], NULL, (void **)&(lpThis->Surface->pixels), &(lpThis->Surface->pitch)))
            {
#ifdef DEBUG_DDRAW
                eprintf("error\n");
#endif
                return DDERR_SURFACEBUSY;
            }

            lpThis->was_flipped = 0;
        }
#endif

#if !SDL_VERSION_ATLEAST(2,0,0)
        if (lpThis->primary)
        {
            if (lpThis->was_flipped && (lpDestRect != NULL) && (lpThis->Surface->flags & SDL_DOUBLEBUF))
            {
                SDL_BlitSurface(lpThis->lpBackbuffer->Surface, NULL, lpThis->Surface, NULL);
                lpThis->was_flipped--;
            }
            else
            {
                lpThis->was_flipped = 0;
            }
        }
#endif

        if (dwFlags & DDBLT_KEYSRCOVERRIDE)
        {
#if SDL_VERSION_ATLEAST(2,0,0)
            SDL_SetColorKey(lpDDSrcSurface->Surface, SDL_TRUE, lpDDBltFX->ddckSrcColorkey.dwColorSpaceLowValue);
#else
            SDL_SetColorKey(lpDDSrcSurface->Surface, SDL_SRCCOLORKEY, lpDDBltFX->ddckSrcColorkey.dwColorSpaceLowValue);
#endif
        }
        else
        {
            if (fill_first)
            {
                SDL_FillRect(lpThis->Surface, &dstrect, 0);
            }
        }

        if (0 != SDL_BlitSurface(lpDDSrcSurface->Surface, ((lpSrcRect != NULL) || (lpDestRect != NULL))?&srcrect:NULL, lpThis->Surface, (lpDestRect != NULL)?&dstrect:NULL))
        {
            if (dwFlags & DDBLT_KEYSRCOVERRIDE)
            {
                SDL_SetColorKey(lpDDSrcSurface->Surface, 0, 0);
            }
#if SDL_VERSION_ATLEAST(2,0,0)
            if (lpThis->primary)
            {
                SDL_UnlockTexture(lpThis->Texture[lpThis->current_texture]);
            }
#endif
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_SURFACEBUSY;
        }

        if (lpThis->primary)
        {
#if SDL_VERSION_ATLEAST(2,0,0)
            SDL_UnlockTexture(lpThis->Texture[lpThis->current_texture]);

            SDL_RenderCopy(lpThis->Renderer, lpThis->Texture[lpThis->current_texture], NULL, NULL);
            SDL_RenderPresent(lpThis->Renderer);

            // clear next frame
            SDL_SetRenderDrawColor(lpThis->Renderer, 0, 0, 0, 255);
            SDL_RenderClear(lpThis->Renderer);
#else
            if ((lpDestRect != NULL) && !(lpThis->Surface->flags & SDL_DOUBLEBUF))
            {
                SDL_UpdateRect(lpThis->Surface, dstrect.x, dstrect.y, dstrect.w, dstrect.h);
            }
            else
            {
                SDL_Flip(lpThis->Surface);
            }
#endif
        }

        if (dwFlags & DDBLT_KEYSRCOVERRIDE)
        {
            SDL_SetColorKey(lpDDSrcSurface->Surface, 0, 0);
        }

#ifdef DEBUG_DDRAW
        eprintf("OK\n");
#endif

        return DD_OK;
    }

    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_Blt");
    exit(1);
}

uint32_t IDirectDrawSurface_BltBatch_c(struct IDirectDrawSurface_c *lpThis, void * param1, uint32_t param2, uint32_t param3)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_BltBatch");
    exit(1);
//    return IDirectDrawSurface_BltBatch(lpThis->lpObject, (LPDDBLTBATCH)param1, param2, param3);
}

uint32_t IDirectDrawSurface_BltFast_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, uint32_t param2, struct IDirectDrawSurface_c * param3, void * param4, uint32_t param5)
{
    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_BltFast");
    exit(1);
}

uint32_t IDirectDrawSurface_DeleteAttachedSurface_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, struct IDirectDrawSurface_c * param2)
{
    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_DeleteAttachedSurface");
    exit(1);
}

uint32_t IDirectDrawSurface_EnumAttachedSurfaces_c(struct IDirectDrawSurface_c *lpThis, void * param1, void * param2)
{
    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_EnumAttachedSurfaces");
    exit(1);
}

uint32_t IDirectDrawSurface_EnumOverlayZOrders_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, void * param2, void * param3)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_EnumOverlayZOrders");
    exit(1);
//    return IDirectDrawSurface_EnumOverlayZOrders(lpThis->lpObject, param1, param2, (LPDDENUMSURFACESCALLBACK)param3);
}

#if SDL_VERSION_ATLEAST(2,0,0)
static void CopyBackbufferSurfaceToTexture(SDL_Surface *Surface, SDL_Texture *Texture)
{
    uint8_t *dst, *src;
    int dstpitch, srcpitch, copylength, height;

    if (0 == SDL_LockTexture(Texture, NULL, (void **)&dst, &dstpitch))
    {
        if (0 == SDL_LockSurface(Surface))
        {
            src = (uint8_t *) Surface->pixels;
            srcpitch = Surface->pitch;

            if (srcpitch == dstpitch)
            {
                memcpy(dst, src, Surface->h * srcpitch);
            }
            else
            {
                copylength = Surface->w * Surface->format->BytesPerPixel;

                for (height = Surface->h; height != 0; height--)
                {
                    memcpy(dst, src, copylength);

                    dst += dstpitch;
                    src += srcpitch;
                }
            }

            SDL_UnlockSurface(Surface);
        }

        SDL_UnlockTexture(Texture);
    }
}
#endif

uint32_t IDirectDrawSurface_Flip_c(struct IDirectDrawSurface_c *lpThis, struct IDirectDrawSurface_c * lpDDSurfaceTargetOverride, uint32_t dwFlags)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_Flip: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%x - ", (uintptr_t)lpThis, (uintptr_t)lpDDSurfaceTargetOverride, dwFlags);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

#ifdef DEBUG_DDRAW
    eprintf("flip %s - ", ((lpThis->primary)?"frontbuffer":((lpThis->backbuffer)?"backbuffer":"surface")));
#endif

    if (!lpThis->primary)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

    if (lpDDSurfaceTargetOverride == NULL)
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        lpThis->current_texture++;
        if (lpThis->current_texture > 2)
        {
            lpThis->current_texture = 0;
        }

        CopyBackbufferSurfaceToTexture(lpThis->lpBackbuffer->Surface, lpThis->Texture[lpThis->current_texture]);
        SDL_RenderCopy(lpThis->Renderer, lpThis->Texture[lpThis->current_texture], NULL, NULL);
        SDL_RenderPresent(lpThis->Renderer);

        SDL_Delay(Display_DelayAfterFlip);

        // clear next frame
        SDL_SetRenderDrawColor(lpThis->Renderer, 0, 0, 0, 255);
        SDL_RenderClear(lpThis->Renderer);
#else
        SDL_BlitSurface(lpThis->lpBackbuffer->Surface, NULL, lpThis->Surface, NULL);
        SDL_Flip(lpThis->Surface);

        SDL_Delay(Display_DelayAfterFlip);
#endif

        lpThis->was_flipped = 2;

#ifdef DEBUG_DDRAW
        eprintf("OK\n");
#endif

        return DD_OK;
    }

    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_Flip");
    exit(1);
}

uint32_t IDirectDrawSurface_GetAttachedSurface_c(struct IDirectDrawSurface_c *lpThis, struct _ddscaps2 *lpDDSCaps, PTR32(struct IDirectDrawSurface_c)* lplpDDAttachedSurface)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_GetAttachedSurface: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t)lpThis, (uintptr_t)lpDDSCaps, (uintptr_t)lplpDDAttachedSurface);
#endif

    if ((lpThis == NULL) || (lplpDDAttachedSurface == NULL))
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

    if (lpDDSCaps->dwCaps == DDSCAPS_BACKBUFFER)
    {
        if ((!lpThis->primary) || (lpThis->lpBackbuffer == NULL))
        {
#ifdef DEBUG_DDRAW
            eprintf("error\n");
#endif
            return DDERR_INVALIDPARAMS;
        }

        *lplpDDAttachedSurface = lpThis->lpBackbuffer;

#ifdef DEBUG_DDRAW
        eprintf("OK: 0x%" PRIxPTR "\n", (uintptr_t)lpThis->lpBackbuffer);
#endif

        return DD_OK;
    }

    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_GetAttachedSurface");
    exit(1);
}

uint32_t IDirectDrawSurface_GetBltStatus_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_GetBltStatus");
    exit(1);
//    return IDirectDrawSurface_GetBltStatus(lpThis->lpObject, param1);
}

uint32_t IDirectDrawSurface_GetCaps_c(struct IDirectDrawSurface_c *lpThis, void * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_GetCaps");
    exit(1);
//    return IDirectDrawSurface_GetCaps(lpThis->lpObject, (LPDDSCAPS)param1);
}

uint32_t IDirectDrawSurface_GetClipper_c(struct IDirectDrawSurface_c *lpThis, PTR32(void)* param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_GetClipper");
    exit(1);
//    return IDirectDrawSurface_GetClipper(lpThis->lpObject, (LPDIRECTDRAWCLIPPER FAR*)param1);
}

uint32_t IDirectDrawSurface_GetColorKey_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, void * param2)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_GetColorKey");
    exit(1);
//    return IDirectDrawSurface_GetColorKey(lpThis->lpObject, param1, (LPDDCOLORKEY)param2);
}

uint32_t IDirectDrawSurface_GetDC_c(struct IDirectDrawSurface_c *lpThis, PTR32(void)* lphDC)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_GetDC: 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t)lpThis, (uintptr_t)lphDC);
#endif

    if ((lpThis == NULL) || (lphDC == NULL))
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

    // Septerra Core uses this, to write copyright notice to screen when doing screenshot
    return DDERR_SURFACEBUSY;
}

uint32_t IDirectDrawSurface_GetFlipStatus_c(struct IDirectDrawSurface_c *lpThis, uint32_t dwFlags)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_GetFlipStatus: 0x%" PRIxPTR ", 0x%x\n", (uintptr_t)lpThis, dwFlags);
#endif

    return DD_OK;
}

uint32_t IDirectDrawSurface_GetOverlayPosition_c(struct IDirectDrawSurface_c *lpThis, int32_t * param1, int32_t * param2)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_GetOverlayPosition");
    exit(1);
//    return IDirectDrawSurface_GetOverlayPosition(lpThis->lpObject, (LPLONG)param1, (LPLONG)param2);
}

uint32_t IDirectDrawSurface_GetPalette_c(struct IDirectDrawSurface_c *lpThis, PTR32(void)* param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_GetPalette");
    exit(1);
//    return IDirectDrawSurface_GetPalette(lpThis->lpObject, (LPDIRECTDRAWPALETTE FAR*)param1);
}

uint32_t IDirectDrawSurface_GetPixelFormat_c(struct IDirectDrawSurface_c *lpThis, void * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_GetPixelFormat");
    exit(1);
//    return IDirectDrawSurface_GetPixelFormat(lpThis->lpObject, (LPDDPIXELFORMAT)param1);
}

static uint32_t GetSurfaceDesc(struct IDirectDrawSurface_c *lpThis, struct _ddsurfacedesc *lpDDSurfaceDesc)
{
    SDL_Surface *Surface;

    if (lpDDSurfaceDesc->dwSize != 108)
    {
        return DDERR_INVALIDPARAMS;
    }

    Surface = (lpThis->primary)?lpThis->lpBackbuffer->Surface:lpThis->Surface;

    lpDDSurfaceDesc->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_BACKBUFFERCOUNT | DDSD_REFRESHRATE | DDSD_PIXELFORMAT;
    lpDDSurfaceDesc->dwHeight = Surface->h;
    lpDDSurfaceDesc->dwWidth = Surface->w;
    lpDDSurfaceDesc->lPitch = Surface->pitch;
    lpDDSurfaceDesc->dwBackBufferCount = (lpThis->lpBackbuffer != NULL)?1:0;
    lpDDSurfaceDesc->dwRefreshRate = 60;
    lpDDSurfaceDesc->ddsCaps.dwCaps =
        ((lpThis->primary)?DDSCAPS_PRIMARYSURFACE:((lpThis->backbuffer)?DDSCAPS_BACKBUFFER:DDSCAPS_OFFSCREENPLAIN)) |
#if SDL_VERSION_ATLEAST(2,0,0)
        ((lpThis->primary)?DDSCAPS_VIDEOMEMORY:DDSCAPS_SYSTEMMEMORY)
#else
        ((lpThis->Surface->flags & SDL_HWSURFACE)?DDSCAPS_VIDEOMEMORY:DDSCAPS_SYSTEMMEMORY)
#endif
    ;
    lpDDSurfaceDesc->ddpfPixelFormat.dwSize = 32;
    lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = (Surface->format->BitsPerPixel == 8)?(DDPF_PALETTEINDEXED8 | DDPF_RGB):(DDPF_RGB);
    lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = Surface->format->BitsPerPixel;
    lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = Surface->format->Rmask;
    lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = Surface->format->Gmask;
    lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = Surface->format->Bmask;

    if (!lpThis->mustlock)
    {
        lpDDSurfaceDesc->dwFlags |= DDSD_LPSURFACE;
        lpDDSurfaceDesc->lpSurface = Surface->pixels;
    }

    return DD_OK;
}

uint32_t IDirectDrawSurface_GetSurfaceDesc_c(struct IDirectDrawSurface_c *lpThis, struct _ddsurfacedesc *lpDDSurfaceDesc)
{
    uint32_t result;

#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_GetSurfaceDesc: 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t)lpThis, (uintptr_t)lpDDSurfaceDesc);
#endif

    if ((lpThis == NULL) || (lpDDSurfaceDesc == NULL))
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

#ifdef DEBUG_DDRAW
    eprintf("%i - ", lpDDSurfaceDesc->dwSize);
#endif

    result = GetSurfaceDesc(lpThis, lpDDSurfaceDesc);

#ifdef DEBUG_DDRAW
    if (result == DD_OK)
    {
        eprintf("OK\n");
    }
    else
    {
        eprintf("error\n");
    }
#endif

    return result;
}

uint32_t IDirectDrawSurface_Initialize_c(struct IDirectDrawSurface_c *lpThis, struct IDirectDraw_c * param1, void * param2)
{
    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_Initialize");
    exit(1);
}

uint32_t IDirectDrawSurface_IsLost_c(struct IDirectDrawSurface_c *lpThis)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_IsLost: 0x%" PRIxPTR " - ", (uintptr_t)lpThis);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    if ((lpThis->primary && (lpThis->Texture[0] == NULL)) ||
        (!lpThis->primary && (lpThis->Surface == NULL))
       )
#else
    if (lpThis->Surface == NULL)
#endif
    {
#ifdef DEBUG_DDRAW
        eprintf("lost\n");
#endif
        return DDERR_SURFACELOST;
    }
    else
    {
#ifdef DEBUG_DDRAW
        eprintf("OK\n");
#endif
        return DD_OK;
    }
}

uint32_t IDirectDrawSurface_Lock_c(struct IDirectDrawSurface_c *lpThis, void * lpDestRect, struct _ddsurfacedesc *lpDDSurfaceDesc, uint32_t dwFlags, void * hEvent)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_Lock: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%x, 0x%" PRIxPTR " - ", (uintptr_t) lpThis, (uintptr_t) lpDestRect, (uintptr_t) lpDDSurfaceDesc, dwFlags, (uintptr_t) hEvent);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

#ifdef DEBUG_DDRAW
    eprintf("lock %s - ", ((lpThis->primary)?"frontbuffer":((lpThis->backbuffer)?"backbuffer":"surface")));
#endif

    if ((lpDestRect == NULL) && ((dwFlags == 0) || (dwFlags == DDLOCK_WAIT)) && (hEvent == NULL))
    {
        if (lpDDSurfaceDesc != NULL)
        {
            uint32_t result;

            result = GetSurfaceDesc(lpThis, lpDDSurfaceDesc);

            if (result != DD_OK)
            {
#ifdef DEBUG_DDRAW
                eprintf("error\n");
#endif
                return result;
            }
        }


        if (lpThis->mustlock)
        {
            SDL_Surface *Surface;

            Surface = (lpThis->primary)?lpThis->lpBackbuffer->Surface:lpThis->Surface;

            if (0 != SDL_LockSurface(Surface))
            {
#ifdef DEBUG_DDRAW
                eprintf("error\n");
#endif
                return DDERR_SURFACEBUSY;
            }

            if (lpDDSurfaceDesc != NULL)
            {
                lpDDSurfaceDesc->dwFlags |= DDSD_LPSURFACE;
                lpDDSurfaceDesc->lpSurface = Surface->pixels;
            }
        }

#ifdef DEBUG_DDRAW
        eprintf("OK\n");
#endif
        return DD_OK;
    }

    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_Lock");
    exit(1);
}

uint32_t IDirectDrawSurface_ReleaseDC_c(struct IDirectDrawSurface_c *lpThis, void * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_ReleaseDC");
    exit(1);
//    return IDirectDrawSurface_ReleaseDC(lpThis->lpObject, (HDC)param1);
}

uint32_t IDirectDrawSurface_Restore_c(struct IDirectDrawSurface_c *lpThis)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_Restore");
    exit(1);
//    return IDirectDrawSurface_Restore(lpThis->lpObject);
}

uint32_t IDirectDrawSurface_SetClipper_c(struct IDirectDrawSurface_c *lpThis, void * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_SetClipper");
    exit(1);
//    return IDirectDrawSurface_SetClipper(lpThis->lpObject, (LPDIRECTDRAWCLIPPER)param1);
}

uint32_t IDirectDrawSurface_SetColorKey_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, void * param2)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_SetColorKey");
    exit(1);
//    return IDirectDrawSurface_SetColorKey(lpThis->lpObject, param1, (LPDDCOLORKEY)param2);
}

uint32_t IDirectDrawSurface_SetOverlayPosition_c(struct IDirectDrawSurface_c *lpThis, int32_t param1, int32_t param2)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_SetOverlayPosition");
    exit(1);
//    return IDirectDrawSurface_SetOverlayPosition(lpThis->lpObject, param1, param2);
}

uint32_t IDirectDrawSurface_SetPalette_c(struct IDirectDrawSurface_c *lpThis, void * param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_SetPalette");
    exit(1);
//    return IDirectDrawSurface_SetPalette(lpThis->lpObject, (LPDIRECTDRAWPALETTE)param1);
}

uint32_t IDirectDrawSurface_Unlock_c(struct IDirectDrawSurface_c *lpThis, void * lpRect)
{
#ifdef DEBUG_DDRAW
    eprintf("IDirectDrawSurface_Unlock: 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t) lpThis, (uintptr_t) lpRect);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DDRAW
        eprintf("error\n");
#endif
        return DDERR_INVALIDPARAMS;
    }

    if (lpThis->primary)
    {
        lpThis->was_flipped = 0;
    }

    //if (lpRect == NULL)
    {
        if (lpThis->mustlock)
        {
            if (lpThis->primary)
            {
                SDL_UnlockSurface(lpThis->lpBackbuffer->Surface);
            }
            else
            {
                SDL_UnlockSurface(lpThis->Surface);
            }
        }

        /*if (lpThis->primary)
        {
            SDL_Flip(lpThis->Surface);
        }*/

#ifdef DEBUG_DDRAW
        eprintf("OK\n");
#endif
        return DD_OK;
    }

    /*eprintf("Unsupported method: %s\n", "IDirectDrawSurface_Unlock");
    exit(1);*/
}

uint32_t IDirectDrawSurface_UpdateOverlay_c(struct IDirectDrawSurface_c *lpThis, void * param1, struct IDirectDrawSurface_c * param2, void * param3, uint32_t param4, void * param5)
{
    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_UpdateOverlay");
    exit(1);
}

uint32_t IDirectDrawSurface_UpdateOverlayDisplay_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1)
{
    eprintf("Unimplemented: %s\n", "IDirectDrawSurface_UpdateOverlayDisplay");
    exit(1);
//    return IDirectDrawSurface_UpdateOverlayDisplay(lpThis->lpObject, param1);
}

uint32_t IDirectDrawSurface_UpdateOverlayZOrder_c(struct IDirectDrawSurface_c *lpThis, uint32_t param1, struct IDirectDrawSurface_c * param2)
{
    eprintf("Unsupported method: %s\n", "IDirectDrawSurface_UpdateOverlayZOrder");
    exit(1);
}

