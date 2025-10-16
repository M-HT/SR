/**
 *
 *  Copyright (C) 2020-2025 Roman Pauer
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

#include <stdio.h>
#include <string.h>
#include "Game_vars.h"
#include "Game_scalerplugin.h"
#include "Game_thread.h"
#include "Albion-smk.h"
#include "Albion-proc.h"
#include "Albion-proc-vfs.h"
#include "Albion-music-midiplugin.h"
#include "Albion-music-midiplugin2.h"
#include "smack.h"


static int firstMenu = 1;

static SmackStruct *Smack;
static SmackFrame *Frame;

static Game_Flip_Procedure blit_normal_height, blit_double_height;

static int SMK_double_pixels;

static SDL_sem *SMK_FuncSem;
static volatile int main_result;

#if SDL_VERSION_ATLEAST(2,0,0)
static SDL_Texture *SMK_Texture[3];
static SDL_Texture *SMK_ScaledTexture[3];
#elif defined(ALLOW_OPENGL)
static GLuint SMK_GLTexture[3];
static GLuint SMK_GLFramebuffer[3];
static GLuint SMK_GLScaledTexture[3];

static PFNGLGENFRAMEBUFFERSEXTPROC gl_glGenFramebuffersEXT;
static PFNGLDELETEFRAMEBUFFERSEXTPROC gl_glDeleteFramebuffersEXT;
static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC gl_glFramebufferTexture2DEXT;
static PFNGLBINDFRAMEBUFFEREXTPROC gl_glBindFramebufferEXT;
#endif
#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
static void *SMK_TextureData;
static void *SMK_ScaledTextureData;
static int SMK_CurrentTexture;
static int SMK_ScaleFactor;
static int SMK_ScaledTextureWidth;
static int SMK_ScaledTextureHeight;
#endif

static Uint32 palette_out[256];

static uint8_t *audio_buf;
static uint32_t audio_len, audio_read, audio_write;
static SDL_AudioCVT audio_cvt;
static uint8_t *audio_cvtbuf;
static int audio_directcvt;


#if defined(GP2X) && defined(__GNUC__)
static void __attribute__ ((noinline, naked)) BufferToScreen320x240x16Asm(uint16_t *dst, uint8_t *src, uint32_t *pal, uint32_t Height, uint32_t Width, uint32_t WidthDiff)
{
//input:
// r0 - dst
// r1 - src
// r2 - palette
// r3 - Height
// [sp] - Width
// [sp+4] - screen->w
//
//temp:
// ip - Width
// lr - screen->w - Width
// v1 - horizontal counter
// v2 - *src
// v3 - *pal
// v4 - *dst
// v5 - *dst2
	asm(
		"stmfd sp!, {v1-v5, lr}" "\n"
		"ldr ip, [sp, #(6*4)]" "\n"
		"ldr lr, [sp, #(7*4)]" "\n"

		"rsb v1, r3, #240" "\n"
		"mul v2, v1, lr" "\n"
		"add r0, r0, v2" "\n"
		"rsb v1, ip, #320" "\n"
		"add r0, r0, v1" "\n"

		"sub lr, lr, ip" "\n"

		"1:" "\n"
		"mov v1, ip, lsr #2" "\n"

		"2:" "\n"
		"ldr v2, [r1], #4" "\n"
		"and v4, v2, #0x00ff" "\n"
		"ldr v4, [r2, v4, lsl #2]" "\n"

		"and v3, v2, #0x00ff00" "\n"
		"ldr v3, [r2, v3, lsr #6]" "\n"
		"orr v4, v4, v3, lsl #16" "\n"

		"and v5, v2, #0x00ff0000" "\n"
		"ldr v5, [r2, v5, lsr #14]" "\n"

		"mov v3, v2, lsr #24" "\n"
		"ldr v3, [r2, v3, lsl #2]" "\n"
		"orr v5, v5, v3, lsl #16" "\n"

		"stmia r0!, {v4, v5}" "\n"

		"subS v1, v1, #1" "\n"
		"bne 2b" "\n"

		"add r0, r0, lr" "\n"

		"subS r3, r3, #1" "\n"
		"bne 1b" "\n"

		//exit
		"ldmfd sp!, {v1-v5, pc}" "\n"
	);
}

static void __attribute__ ((noinline, naked)) BufferToScreen320x240x16DoubleAsm(uint16_t *dst, uint8_t *src, uint32_t *pal, uint32_t Height, uint32_t Width, uint32_t WidthDiff)
{
//input:
// r0 - dst
// r1 - src
// r2 - palette
// r3 - Height
// [sp] - Width
// [sp+4] - screen->w
//
//temp:
// ip - Width
// lr - screen->w - Width
// v1 - horizontal counter
// v2 - *src
// v3 - *pal
// v4 - *dst
// v5 - *dst2
// v6 - dst+w
	asm(
		"stmfd sp!, {v1-v6, lr}" "\n"
		"ldr ip, [sp, #(7*4)]" "\n"
		"ldr lr, [sp, #(8*4)]" "\n"

		"rsb v1, r3, #120" "\n"
		"mul v2, v1, lr" "\n"
		"add r0, r0, v2, lsl #1" "\n"
		"rsb v1, ip, #320" "\n"
		"add r0, r0, v1" "\n"

		"add v6, r0, lr" "\n"

		"rsb lr, ip, lr, lsl #1" "\n"

		"1:" "\n"
		"mov v1, ip, lsr #2" "\n"

		"2:" "\n"
		"ldr v2, [r1], #4" "\n"
		"and v4, v2, #0x00ff" "\n"
		"ldr v4, [r2, v4, lsl #2]" "\n"

		"and v3, v2, #0x00ff00" "\n"
		"ldr v3, [r2, v3, lsr #6]" "\n"
		"orr v4, v4, v3, lsl #16" "\n"

		"and v5, v2, #0x00ff0000" "\n"
		"ldr v5, [r2, v5, lsr #14]" "\n"

		"mov v3, v2, lsr #24" "\n"
		"ldr v3, [r2, v3, lsl #2]" "\n"
		"orr v5, v5, v3, lsl #16" "\n"

		"stmia r0!, {v4, v5}" "\n"
		"stmia v6!, {v4, v5}" "\n"

		"subS v1, v1, #1" "\n"
		"bne 2b" "\n"

		"add r0, r0, lr" "\n"
		"add v6, v6, lr" "\n"

		"subS r3, r3, #1" "\n"
		"bne 1b" "\n"

		//exit
		"ldmfd sp!, {v1-v6, pc}" "\n"
	);
}
#endif

static void Blit_32(SDL_Surface *surface, uint32_t *pdst)
{
    int smk_width, smk_height, dst_pitch, x, y;
    uint8_t *psrc;

    smk_width = Smack->PaddedWidth;
    smk_height = Smack->PaddedHeight;

    if (surface != NULL)
    {
        dst_pitch = surface->pitch;
        pdst = (uint32_t *)((((surface->h - smk_height) / 2) * dst_pitch) + (((surface->w - smk_width) / 2) * 4) + (uintptr_t)surface->pixels);
    }
    else
    {
        dst_pitch = 320*4;
        pdst = (uint32_t *)((((200 - smk_height) / 2) * dst_pitch) + (((320 - smk_width) / 2) * 4) + (uintptr_t)pdst);
    }

    psrc = Frame->Video;
    for (y = 0; y < smk_height; y++)
    {
        for (x = 0; x < smk_width; x++)
        {
            pdst[x] = palette_out[*psrc];
            psrc++;
        }
        pdst = (uint32_t *)(dst_pitch + (uintptr_t)pdst);
    }
}

static void Blit_16(SDL_Surface *surface, uint16_t *pdst)
{
#if defined(GP2X) && defined(__GNUC__)
    BufferToScreen320x240x16Asm
    (
        (uint16_t *) surface->pixels,
        Frame->Video,
        (uint32_t *) palette_out,
        Smack->PaddedHeight,
        Smack->PaddedWidth,
        surface->w
    );
#else
    int smk_width, smk_height, dst_pitch, x, y;
    uint8_t *psrc;

    smk_width = Smack->PaddedWidth;
    smk_height = Smack->PaddedHeight;

    if (surface != NULL)
    {
        dst_pitch = surface->pitch;
        pdst = (uint16_t *)((((surface->h - smk_height) / 2) * dst_pitch) + (((surface->w - smk_width) / 2) * 2) + (uintptr_t)surface->pixels);
    }
    else
    {
        dst_pitch = 320*2;
        pdst = (uint16_t *)((((200 - smk_height) / 2) * dst_pitch) + (((320 - smk_width) / 2) * 2) + (uintptr_t)pdst);
    }

    psrc = Frame->Video;
    for (y = 0; y < smk_height; y++)
    {
        for (x = 0; x < smk_width; x++)
        {
            pdst[x] = palette_out[*psrc];
            psrc++;
        }
        pdst = (uint16_t *)(dst_pitch + (uintptr_t)pdst);
    }
#endif
}

static void Blit_32_double_height(SDL_Surface *surface, uint32_t *pdst)
{
    int smk_width, smk_height, dst_pitch, x, y;
    uint8_t *psrc;
    uint32_t *pdst2;

    smk_width = Smack->PaddedWidth;
    smk_height = Smack->PaddedHeight;

    if (surface != NULL)
    {
        dst_pitch = surface->pitch;
        pdst = (uint32_t *)((((surface->h - 2 * smk_height) / 2) * dst_pitch) + (((surface->w - smk_width) / 2) * 4) + (uintptr_t)surface->pixels);
    }
    else
    {
        dst_pitch = 320*4;
        pdst = (uint32_t *)((((200 - 2 * smk_height) / 2) * dst_pitch) + (((320 - smk_width) / 2) * 4) + (uintptr_t)pdst);
    }

    psrc = Frame->Video;
    pdst2 = (uint32_t *)(dst_pitch + (uintptr_t)pdst);
    for (y = 0; y < smk_height; y++)
    {
        for (x = 0; x < smk_width; x++)
        {
            pdst[x] = pdst2[x] = palette_out[*psrc];
            psrc++;
        }
        pdst = (uint32_t *)(2 * dst_pitch + (uintptr_t)pdst);
        pdst2 = (uint32_t *)(dst_pitch + (uintptr_t)pdst);
    }
}

static void Blit_16_double_height(SDL_Surface *surface, uint16_t *pdst)
{
#if defined(GP2X) && defined(__GNUC__)
    BufferToScreen320x240x16DoubleAsm
    (
        (uint16_t *) surface->pixels,
        Frame->Video,
        (uint32_t *) palette_out,
        Smack->PaddedHeight,
        Smack->PaddedWidth,
        surface->w
    );
#else
    int smk_width, smk_height, dst_pitch, x, y;
    uint8_t *psrc;
    uint16_t *pdst2;

    smk_width = Smack->PaddedWidth;
    smk_height = Smack->PaddedHeight;

    if (surface != NULL)
    {
        dst_pitch = surface->pitch;
        pdst = (uint16_t *)((((surface->h - 2 * smk_height) / 2) * dst_pitch) + (((surface->w - smk_width) / 2) * 2) + (uintptr_t)surface->pixels);
    }
    else
    {
        dst_pitch = 320*2;
        pdst = (uint16_t *)((((200 - 2 * smk_height) / 2) * dst_pitch) + (((320 - smk_width) / 2) * 2) + (uintptr_t)pdst);
    }

    psrc = Frame->Video;
    pdst2 = (uint16_t *)(dst_pitch + (uintptr_t)pdst);
    for (y = 0; y < smk_height; y++)
    {
        for (x = 0; x < smk_width; x++)
        {
            pdst[x] = pdst2[x] = palette_out[*psrc];
            psrc++;
        }
        pdst = (uint16_t *)(2 * dst_pitch + (uintptr_t)pdst);
        pdst2 = (uint16_t *)(dst_pitch + (uintptr_t)pdst);
    }
#endif
}

static void Blit_32_double_pixels(SDL_Surface *surface, uint32_t *pdst)
{
    int smk_width, smk_height, dst_pitch, x, y;
    uint8_t *psrc;
    uint32_t *pdst2;

    smk_width = Smack->PaddedWidth;
    smk_height = Smack->PaddedHeight;

    if (surface != NULL)
    {
        dst_pitch = surface->pitch;
        pdst = (uint32_t *)((((surface->h - 2 * smk_height) / 2) * dst_pitch) + (((surface->w - 2 * smk_width) / 2) * 4) + (uintptr_t)surface->pixels);
    }
    else
    {
        dst_pitch = 640*4;
        pdst = (uint32_t *)((((400 - 2 * smk_height) / 2) * dst_pitch) + (((640 - 2 * smk_width) / 2) * 4) + (uintptr_t)pdst);
    }

    psrc = Frame->Video;
    pdst2 = (uint32_t *)(dst_pitch + (uintptr_t)pdst);
    for (y = 0; y < smk_height; y++)
    {
        for (x = 0; x < smk_width; x++)
        {
            pdst[2 * x] = pdst[2 * x + 1] = pdst2[2 * x] = pdst2[2 * x + 1] = palette_out[*psrc];
            psrc++;
        }
        pdst = (uint32_t *)(2 * dst_pitch + (uintptr_t)pdst);
        pdst2 = (uint32_t *)(dst_pitch + (uintptr_t)pdst);
    }
}

static void Blit_16_double_pixels(SDL_Surface *surface, uint16_t *pdst)
{
    int smk_width, smk_height, dst_pitch, x, y;
    uint8_t *psrc;
    uint16_t *pdst2;

    smk_width = Smack->PaddedWidth;
    smk_height = Smack->PaddedHeight;

    if (surface != NULL)
    {
        dst_pitch = surface->pitch;
        pdst = (uint16_t *)((((surface->h - 2 * smk_height) / 2) * dst_pitch) + (((surface->w - 2 * smk_width) / 2) * 2) + (uintptr_t)surface->pixels);
    }
    else
    {
        dst_pitch = 640*2;
        pdst = (uint16_t *)((((400 - 2 * smk_height) / 2) * dst_pitch) + (((640 - 2 * smk_width) / 2) * 2) + (uintptr_t)pdst);
    }

    psrc = Frame->Video;
    pdst2 = (uint16_t *)(dst_pitch + (uintptr_t)pdst);
    for (y = 0; y < smk_height; y++)
    {
        for (x = 0; x < smk_width; x++)
        {
            pdst[2 * x] = pdst[2 * x + 1] = pdst2[2 * x] = pdst2[2 * x + 1] = palette_out[*psrc];
            psrc++;
        }
        pdst = (uint16_t *)(2 * dst_pitch + (uintptr_t)pdst);
        pdst2 = (uint16_t *)(dst_pitch + (uintptr_t)pdst);
    }
}

static void Blit_32_double_height_double_pixels(SDL_Surface *surface, uint32_t *pdst)
{
    int smk_width, smk_height, dst_pitch, x, y;
    uint8_t *psrc;
    uint32_t *pdst2, *pdst3, *pdst4;

    smk_width = Smack->PaddedWidth;
    smk_height = Smack->PaddedHeight;

    if (surface != NULL)
    {
        dst_pitch = surface->pitch;
        pdst = (uint32_t *)((((surface->h - 4 * smk_height) / 2) * dst_pitch) + (((surface->w - 2 * smk_width) / 2) * 4) + (uintptr_t)surface->pixels);
    }
    else
    {
        dst_pitch = 640*4;
        pdst = (uint32_t *)((((400 - 4 * smk_height) / 2) * dst_pitch) + (((640 - 2 * smk_width) / 2) * 4) + (uintptr_t)pdst);
    }

    psrc = Frame->Video;
    pdst2 = (uint32_t *)(dst_pitch + (uintptr_t)pdst);
    pdst3 = (uint32_t *)(dst_pitch + (uintptr_t)pdst2);
    pdst4 = (uint32_t *)(dst_pitch + (uintptr_t)pdst3);
    for (y = 0; y < smk_height; y++)
    {
        for (x = 0; x < smk_width; x++)
        {
            pdst[2 * x] = pdst[2 * x + 1] = pdst2[2 * x] = pdst2[2 * x + 1] = pdst3[2 * x] = pdst3[2 * x + 1] = pdst4[2 * x] = pdst4[2 * x + 1] = palette_out[*psrc];
            psrc++;
        }
        pdst = (uint32_t *)(4 * dst_pitch + (uintptr_t)pdst);
        pdst2 = (uint32_t *)(dst_pitch + (uintptr_t)pdst);
        pdst3 = (uint32_t *)(dst_pitch + (uintptr_t)pdst2);
        pdst4 = (uint32_t *)(dst_pitch + (uintptr_t)pdst3);
    }
}

static void Blit_16_double_height_double_pixels(SDL_Surface *surface, uint16_t *pdst)
{
    int smk_width, smk_height, dst_pitch, x, y;
    uint8_t *psrc;
    uint16_t *pdst2, *pdst3, *pdst4;

    smk_width = Smack->PaddedWidth;
    smk_height = Smack->PaddedHeight;

    if (surface != NULL)
    {
        dst_pitch = surface->pitch;
        pdst = (uint16_t *)((((surface->h - 4 * smk_height) / 2) * dst_pitch) + (((surface->w - 2 * smk_width) / 2) * 2) + (uintptr_t)surface->pixels);
    }
    else
    {
        dst_pitch = 640*2;
        pdst = (uint16_t *)((((400 - 4 * smk_height) / 2) * dst_pitch) + (((640 - 2 * smk_width) / 2) * 2) + (uintptr_t)pdst);
    }

    psrc = Frame->Video;
    pdst2 = (uint16_t *)(dst_pitch + (uintptr_t)pdst);
    pdst3 = (uint16_t *)(dst_pitch + (uintptr_t)pdst2);
    pdst4 = (uint16_t *)(dst_pitch + (uintptr_t)pdst3);
    for (y = 0; y < smk_height; y++)
    {
        for (x = 0; x < smk_width; x++)
        {
            pdst[2 * x] = pdst[2 * x + 1] = pdst2[2 * x] = pdst2[2 * x + 1] = pdst3[2 * x] = pdst3[2 * x + 1] = pdst4[2 * x] = pdst4[2 * x + 1] = palette_out[*psrc];
            psrc++;
        }
        pdst = (uint16_t *)(4 * dst_pitch + (uintptr_t)pdst);
        pdst2 = (uint16_t *)(dst_pitch + (uintptr_t)pdst);
        pdst3 = (uint16_t *)(dst_pitch + (uintptr_t)pdst2);
        pdst4 = (uint16_t *)(dst_pitch + (uintptr_t)pdst3);
    }
}

static void eventloop_initialize(void)
{
#if SDL_VERSION_ATLEAST(2,0,0)
    int index;
    SDL_RendererInfo info;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, (Game_ScalingQuality)?"linear":"nearest");

    if (Scaler_ScaleTexture)
    {
        int horizontal_factor, vertical_factor;

        if (!SMK_ScaleFactor)
        {
            horizontal_factor = vertical_factor = 2;

            while ((horizontal_factor + 1) * 320 <= (int)Picture_Width) horizontal_factor++;
            while ((vertical_factor + 1) * 200 <= (int)Picture_Height) vertical_factor++;

            if (horizontal_factor > GAME_MAX_3D_ENGINE_FACTOR) horizontal_factor = GAME_MAX_3D_ENGINE_FACTOR;
            if (vertical_factor > GAME_MAX_3D_ENGINE_FACTOR) vertical_factor = GAME_MAX_3D_ENGINE_FACTOR;

            if (0 == SDL_GetRendererInfo(Game_Renderer, &info))
            {
                if (info.max_texture_width > (int)Picture_Width)
                {
                    while (horizontal_factor * 320 > info.max_texture_width) horizontal_factor--;
                }
                if (info.max_texture_height > (int)Picture_Height)
                {
                    while (vertical_factor * 200 > info.max_texture_height) vertical_factor--;
                }
            }

            SMK_ScaleFactor = 1;
        }
        else
        {
            horizontal_factor = vertical_factor = SMK_ScaleFactor;
        }

        SMK_ScaledTextureWidth = horizontal_factor * 320;
        SMK_ScaledTextureHeight = vertical_factor * 200;

        for (index = 0; index < 3; index++)
        {
            SMK_ScaledTexture[index] = SDL_CreateTexture(Game_Renderer, (Display_Bitsperpixel == 32)?SDL_PIXELFORMAT_ARGB8888:SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_TARGET, SMK_ScaledTextureWidth, SMK_ScaledTextureHeight);
        }

        if ((SMK_ScaledTexture[0] == NULL) || (SMK_ScaledTexture[1] == NULL) || (SMK_ScaledTexture[2] == NULL))
        {
            for (index = 2; index >= 0; index--)
            {
                if (SMK_ScaledTexture[index] != NULL)
                {
                    SDL_DestroyTexture(SMK_ScaledTexture[index]);
                    SMK_ScaledTexture[index] = NULL;
                }
            }

            goto main_init_exit;
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    }

    if (Scaler_ScaleTextureData)
    {
        if (!SMK_ScaleFactor)
        {
            int max_factor;

            SMK_ScaleFactor = 2;

            while (((SMK_ScaleFactor + 1) * 320 <= (int)Picture_Width) ||
                   ((SMK_ScaleFactor + 1) * 200 <= (int)Picture_Height)
                  ) SMK_ScaleFactor++;

            max_factor = ScalerPlugin_get_maximum_scale_factor();
            if (Scaler_ScaleFactor > max_factor) Scaler_ScaleFactor = max_factor;

            if (0 == SDL_GetRendererInfo(Game_Renderer, &info))
            {
                if (info.max_texture_width > (int)Picture_Width)
                {
                    while (SMK_ScaleFactor * 320 > info.max_texture_width) SMK_ScaleFactor--;
                }
                if (info.max_texture_height > (int)Picture_Height)
                {
                    while (SMK_ScaleFactor * 200 > info.max_texture_height) SMK_ScaleFactor--;
                }
            }
        }
    }
    else
    {
        SMK_ScaleFactor = (SMK_double_pixels)?2:1;
    }

    for (index = 0; index < 3; index++)
    {
        SMK_Texture[index] = SDL_CreateTexture(Game_Renderer, (Display_Bitsperpixel == 32)?SDL_PIXELFORMAT_ARGB8888:SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, SMK_ScaleFactor * 320, SMK_ScaleFactor * 200);
    }

    if (Scaler_ScaleTextureData)
    {
        SMK_ScaledTextureData = malloc(SMK_ScaleFactor * 320 * SMK_ScaleFactor * 200 * Display_Bitsperpixel / 8);
    }

    if ((SMK_Texture[0] == NULL) || (SMK_Texture[1] == NULL) || (SMK_Texture[2] == NULL) || (Scaler_ScaleTextureData && (SMK_ScaledTextureData == NULL)))
    {
        if (SMK_ScaledTextureData != NULL)
        {
            free(SMK_ScaledTextureData);
            SMK_ScaledTextureData = NULL;
        }

        for (index = 2; index >= 0; index--)
        {
            if (SMK_Texture[index] != NULL)
            {
                SDL_DestroyTexture(SMK_Texture[index]);
                SMK_Texture[index] = NULL;
            }
        }

        if (SMK_ScaledTexture[0] != NULL)
        {
            SDL_DestroyTexture(SMK_ScaledTexture[2]);
            SMK_ScaledTexture[2] = NULL;
            SDL_DestroyTexture(SMK_ScaledTexture[1]);
            SMK_ScaledTexture[1] = NULL;
            SDL_DestroyTexture(SMK_ScaledTexture[0]);
            SMK_ScaledTexture[0] = NULL;
        }

        goto main_init_exit;
    }
#else
#ifdef ALLOW_OPENGL
    if (Game_UseOpenGL)
    {
        int index;
        GLint scaling_quality, max_texture_size;

        // flush GL errors
        while(glGetError() != GL_NO_ERROR);

        scaling_quality = (Game_ScalingQuality)?GL_LINEAR:GL_NEAREST;

        if (Scaler_ScaleTexture)
        {
            int horizontal_factor, vertical_factor;

            gl_glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) SDL_GL_GetProcAddress("glGenFramebuffersEXT");
            gl_glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) SDL_GL_GetProcAddress("glDeleteFramebuffersEXT");
            gl_glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) SDL_GL_GetProcAddress("glFramebufferTexture2DEXT");
            gl_glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) SDL_GL_GetProcAddress("glBindFramebufferEXT");

            if (!SMK_ScaleFactor)
            {
                horizontal_factor = vertical_factor = 2;

                while ((horizontal_factor + 1) * 320 <= (int)Picture_Width) horizontal_factor++;
                while ((vertical_factor + 1) * 200 <= (int)Picture_Height) vertical_factor++;

                if (horizontal_factor > GAME_MAX_3D_ENGINE_FACTOR) horizontal_factor = GAME_MAX_3D_ENGINE_FACTOR;
                if (vertical_factor > GAME_MAX_3D_ENGINE_FACTOR) vertical_factor = GAME_MAX_3D_ENGINE_FACTOR;

                max_texture_size = 0;
                glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
                if (max_texture_size > (int)Picture_Width)
                {
                    while (horizontal_factor * 320 > max_texture_size) horizontal_factor--;
                }
                if (max_texture_size > (int)Picture_Height)
                {
                    while (vertical_factor * 200 > max_texture_size) vertical_factor--;
                }

                SMK_ScaleFactor = 1;
            }
            else
            {
                horizontal_factor = vertical_factor = SMK_ScaleFactor;
            }

            SMK_ScaledTextureWidth = horizontal_factor * 320;
            SMK_ScaledTextureHeight = vertical_factor * 200;

            glGenTextures(3, &(SMK_GLScaledTexture[0]));
            if (glGetError() != GL_NO_ERROR) goto main_init_exit;

            for (index = 0; index < 3; index++)
            {
                glBindTexture(GL_TEXTURE_2D, SMK_GLScaledTexture[index]);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaling_quality);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaling_quality);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SMK_ScaledTextureWidth, SMK_ScaledTextureHeight, 0, GL_BGRA, (Display_Bitsperpixel == 32)?GL_UNSIGNED_INT_8_8_8_8_REV:GL_UNSIGNED_SHORT_5_6_5_REV, NULL);
            }

            gl_glGenFramebuffersEXT(3, &(SMK_GLFramebuffer[0]));

            for (index = 0; index < 3; index++)
            {
                gl_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, SMK_GLFramebuffer[index]);

                gl_glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, SMK_GLScaledTexture[index], 0);
            }

            scaling_quality = GL_NEAREST;
        }

        if (Scaler_ScaleTextureData)
        {
            if (!SMK_ScaleFactor)
            {
                int max_factor;

                SMK_ScaleFactor = 2;

                while (((SMK_ScaleFactor + 1) * 320 <= (int)Picture_Width) ||
                       ((SMK_ScaleFactor + 1) * 200 <= (int)Picture_Height)
                      ) SMK_ScaleFactor++;

                max_factor = ScalerPlugin_get_maximum_scale_factor();
                if (Scaler_ScaleFactor > max_factor) Scaler_ScaleFactor = max_factor;

                max_texture_size = 0;
                glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
                if (max_texture_size > (int)Picture_Width)
                {
                    while (SMK_ScaleFactor * 320 > max_texture_size) SMK_ScaleFactor--;
                }
                if (max_texture_size > (int)Picture_Height)
                {
                    while (SMK_ScaleFactor * 200 > max_texture_size) SMK_ScaleFactor--;
                }
            }
        }
        else
        {
            SMK_ScaleFactor = (SMK_double_pixels)?2:1;
        }

        if (Scaler_ScaleTextureData)
        {
            SMK_ScaledTextureData = malloc(SMK_ScaleFactor * 320 * SMK_ScaleFactor * 200 * Display_Bitsperpixel / 8);
        }

        glGenTextures(3, &(SMK_GLTexture[0]));
        if ((!Scaler_ScaleTexture) && (glGetError() != GL_NO_ERROR)) goto main_init_exit;

        for (index = 0; index < 3; index++)
        {
            glBindTexture(GL_TEXTURE_2D, SMK_GLTexture[index]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaling_quality);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaling_quality);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SMK_ScaleFactor * 320, SMK_ScaleFactor * 200, 0, GL_BGRA, (Display_Bitsperpixel == 32)?GL_UNSIGNED_INT_8_8_8_8_REV:GL_UNSIGNED_SHORT_5_6_5_REV, Scaler_ScaleTextureData ? SMK_ScaledTextureData : SMK_TextureData);
        }

        if ((glGetError() != GL_NO_ERROR) || (Scaler_ScaleTextureData && (SMK_ScaledTextureData == NULL)))
        {
            if (SMK_ScaledTextureData != NULL)
            {
                free(SMK_ScaledTextureData);
                SMK_ScaledTextureData = NULL;
            }

            glBindTexture(GL_TEXTURE_2D, 0);

            glDeleteTextures(3, &(SMK_GLTexture[0]));
            SMK_GLTexture[0] = 0;

            if (SMK_GLFramebuffer[0] != 0)
            {
                gl_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
                gl_glDeleteFramebuffersEXT(3, &(SMK_GLFramebuffer[0]));
                SMK_GLFramebuffer[0] = 0;
            }

            if (SMK_GLScaledTexture[0] != 0)
            {
                glDeleteTextures(3, &(SMK_GLScaledTexture[0]));
                SMK_GLScaledTexture[0] = 0;
            }

            // flush GL errors
            while(glGetError() != GL_NO_ERROR);

            goto main_init_exit;
        }
    }
    else
#endif
    {
        // clear screen
        int clear_screen;
        for (clear_screen = 3; clear_screen != 0; clear_screen--)
        {
            SDL_FillRect(Game_Screen, NULL, 0);
            SDL_Flip(Game_Screen);
        }
    }
#endif

    main_result = 1;

#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
main_init_exit:
#endif
    SDL_SemPost(SMK_FuncSem);
}

static void eventloop_deinitialize(void)
{
#if SDL_VERSION_ATLEAST(2,0,0)
    if (SMK_ScaledTextureData != NULL)
    {
        free(SMK_ScaledTextureData);
        SMK_ScaledTextureData = NULL;
    }

    if (SMK_Texture[0] != NULL)
    {
        SDL_DestroyTexture(SMK_Texture[2]);
        SMK_Texture[2] = NULL;
        SDL_DestroyTexture(SMK_Texture[1]);
        SMK_Texture[1] = NULL;
        SDL_DestroyTexture(SMK_Texture[0]);
        SMK_Texture[0] = NULL;
    }

    if (SMK_ScaledTexture[0] != NULL)
    {
        SDL_DestroyTexture(SMK_ScaledTexture[2]);
        SMK_ScaledTexture[2] = NULL;
        SDL_DestroyTexture(SMK_ScaledTexture[1]);
        SMK_ScaledTexture[1] = NULL;
        SDL_DestroyTexture(SMK_ScaledTexture[0]);
        SMK_ScaledTexture[0] = NULL;
    }
#elif defined(ALLOW_OPENGL)
    if (SMK_ScaledTextureData != NULL)
    {
        free(SMK_ScaledTextureData);
        SMK_ScaledTextureData = NULL;
    }

    if ((SMK_GLTexture[0] != 0) || (SMK_GLScaledTexture[0] != 0))
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (SMK_GLTexture[0] != 0)
    {
        glDeleteTextures(3, &(SMK_GLTexture[0]));
        SMK_GLTexture[0] = 0;
    }

    if (SMK_GLFramebuffer[0] != 0)
    {
        gl_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        gl_glDeleteFramebuffersEXT(3, &(SMK_GLFramebuffer[0]));
        SMK_GLFramebuffer[0] = 0;
    }

    if (SMK_GLScaledTexture[0] != 0)
    {
        glDeleteTextures(3, &(SMK_GLScaledTexture[0]));
        SMK_GLScaledTexture[0] = 0;
    }
#endif

    SDL_SemPost(SMK_FuncSem);
}

static void eventloop_flip(void)
{
#if SDL_VERSION_ATLEAST(2,0,0)
    if (Scaler_ScaleTextureData)
    {
        SDL_UpdateTexture(SMK_Texture[SMK_CurrentTexture], NULL, SMK_ScaledTextureData, SMK_ScaleFactor * 320 * Display_Bitsperpixel / 8);
    }
    else
    {
        SDL_UpdateTexture(SMK_Texture[SMK_CurrentTexture], NULL, SMK_TextureData, ((SMK_double_pixels)?640:320) * Display_Bitsperpixel / 8);
    }

    SDL_RenderClear(Game_Renderer);

    if (Scaler_ScaleTexture)
    {
        SDL_SetRenderTarget(Game_Renderer, SMK_ScaledTexture[SMK_CurrentTexture]);
        SDL_RenderCopy(Game_Renderer, SMK_Texture[SMK_CurrentTexture], NULL, NULL);

        SDL_SetRenderTarget(Game_Renderer, NULL);
        SDL_RenderCopy(Game_Renderer, SMK_ScaledTexture[SMK_CurrentTexture], NULL, NULL);
    }
    else
    {
        SDL_RenderCopy(Game_Renderer, SMK_Texture[SMK_CurrentTexture], NULL, NULL);
    }

    SDL_RenderPresent(Game_Renderer);

    SMK_CurrentTexture++;
    if (SMK_CurrentTexture > 2)
    {
        SMK_CurrentTexture = 0;
    }
#elif defined(ALLOW_OPENGL)
    if (Game_UseOpenGL)
    {
        static const GLfloat QuadVertices[2*4] = {
            -1.0f,  1.0f,
             1.0f,  1.0f,
             1.0f, -1.0f,
            -1.0f, -1.0f,
        };
        static const GLfloat QuadTexCoords[2*4] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };
        static const GLfloat QuadScaleTexCoords[2*4] = {
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
        };

        glBindTexture(GL_TEXTURE_2D, SMK_GLTexture[SMK_CurrentTexture]);

        if (Scaler_ScaleTextureData)
        {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SMK_ScaleFactor * 320, SMK_ScaleFactor * 200, GL_BGRA, (Display_Bitsperpixel == 32)?GL_UNSIGNED_INT_8_8_8_8_REV:GL_UNSIGNED_SHORT_5_6_5_REV, SMK_ScaledTextureData);
        }
        else
        {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ((SMK_double_pixels)?640:320), ((SMK_double_pixels)?400:200), GL_BGRA, (Display_Bitsperpixel == 32)?GL_UNSIGNED_INT_8_8_8_8_REV:GL_UNSIGNED_SHORT_5_6_5_REV, SMK_TextureData);
        }

        glEnable(GL_TEXTURE_2D);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        if (Scaler_ScaleTexture)
        {
            gl_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, SMK_GLFramebuffer[SMK_CurrentTexture]);

            glViewport(0, 0, SMK_ScaledTextureWidth, SMK_ScaledTextureHeight);

            glVertexPointer(2, GL_FLOAT, 0, &(QuadVertices[0]));
            glTexCoordPointer(2, GL_FLOAT, 0, &(QuadScaleTexCoords[0]));
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            gl_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

            glViewport(Picture_Position_UL_X, Picture_Position_UL_Y, Picture_Width, Picture_Height);

            glBindTexture(GL_TEXTURE_2D, SMK_GLScaledTexture[SMK_CurrentTexture]);
        }

        glVertexPointer(2, GL_FLOAT, 0, &(QuadVertices[0]));
        glTexCoordPointer(2, GL_FLOAT, 0, &(QuadTexCoords[0]));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glDisable(GL_TEXTURE_2D);

        SDL_GL_SwapBuffers();

        SMK_CurrentTexture++;
        if (SMK_CurrentTexture > 2)
        {
            SMK_CurrentTexture = 0;
        }
    }
#endif
}

static int initialize_display(void)
{
    SDL_Event event;

#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
    if (Game_AdvancedScaling)
    {
        SMK_ScaleFactor = Game_ScaleFactor;

        SMK_double_pixels = 0;
    }
    else
#endif
    {
        SMK_double_pixels = ((Display_Width >= 640) && (Display_Height >= 400))?1:0;
    }

    if (SMK_double_pixels)
    {
        if (Display_Bitsperpixel == 32)
        {
            blit_normal_height = (Game_Flip_Procedure)Blit_32_double_pixels;
            blit_double_height = (Game_Flip_Procedure)Blit_32_double_height_double_pixels;
        }
        else
        {
            blit_normal_height = (Game_Flip_Procedure)Blit_16_double_pixels;
            blit_double_height = (Game_Flip_Procedure)Blit_16_double_height_double_pixels;
        }
    }
    else
    {
        if (Display_Bitsperpixel == 32)
        {
            blit_normal_height = (Game_Flip_Procedure)Blit_32;
            blit_double_height = (Game_Flip_Procedure)Blit_32_double_height;
        }
        else
        {
            blit_normal_height = (Game_Flip_Procedure)Blit_16;
            blit_double_height = (Game_Flip_Procedure)Blit_16_double_height;
        }
    }

    SMK_FuncSem = SDL_CreateSemaphore(0);
    if (SMK_FuncSem == NULL) return 0;


#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
    SMK_TextureData = NULL;
    SMK_CurrentTexture = 0;
#if !SDL_VERSION_ATLEAST(2,0,0)
    if (Game_UseOpenGL)
#endif
    {
        SMK_TextureData = malloc(((SMK_double_pixels)?640*400:320*200) * (Display_Bitsperpixel / 8));
        if (SMK_TextureData == NULL) goto init_exit1;

        memset(SMK_TextureData, 0, ((SMK_double_pixels)?640*400:320*200) * (Display_Bitsperpixel / 8));
    }
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
    SMK_Texture[0] = NULL;
#elif defined(ALLOW_OPENGL)
    SMK_GLTexture[0] = 0;
#endif

    if (Thread_Exit) goto init_exit2;

    main_result = 0;

    event.type = SDL_USEREVENT;
    event.user.code = EC_SMK_FUNCTION;
    event.user.data1 = (void *) &eventloop_initialize;
    event.user.data2 = NULL;

    SDL_PushEvent(&event);

    SDL_SemWait(SMK_FuncSem);

    if (!main_result) goto init_exit2;

    return 1;

init_exit2:
#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
    if (SMK_TextureData != NULL)
    {
        free(SMK_TextureData);
        SMK_TextureData = NULL;
    }
#endif

#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
init_exit1:
#endif
    SDL_DestroySemaphore(SMK_FuncSem);
    SMK_FuncSem = NULL;
    return 0;
}

static void deinitialize_display(void)
{
    if (SMK_FuncSem != NULL)
    {
        SDL_Event event;

        if (Thread_Exit) return;

        event.type = SDL_USEREVENT;
        event.user.code = EC_SMK_FUNCTION;
        event.user.data1 = (void *) &eventloop_deinitialize;
        event.user.data2 = NULL;

        SDL_PushEvent(&event);

        SDL_SemWait(SMK_FuncSem);
    }

#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
    if (SMK_TextureData != NULL)
    {
        free(SMK_TextureData);
        SMK_TextureData = NULL;
    }
#endif

    if (SMK_FuncSem != NULL)
    {
        SDL_DestroySemaphore(SMK_FuncSem);
        SMK_FuncSem = NULL;
    }
}

static void BufferToScreen(void)
{
    int index;
    SDL_Event event;

    // set palette
    if (Smack->FrameTypes[Smack->CurrentFrame] & 1)
    {
#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
#if !SDL_VERSION_ATLEAST(2,0,0)
        if (Game_UseOpenGL)
#endif
        {
            if (Display_Bitsperpixel == 32)
            {
                for (index = 0; index < 256; index++)
                {
                    palette_out[index] = ((*(Frame->Palette))[index].r << 16) | ((*(Frame->Palette))[index].g << 8) | ((*(Frame->Palette))[index].b);
                }
            }
            else
            {
                for (index = 0; index < 256; index++)
                {
                    palette_out[index] = (((*(Frame->Palette))[index].r & 0xf8) << 8) | (((*(Frame->Palette))[index].g & 0xfc) << 3) | ((*(Frame->Palette))[index].b >> 3);
                }
            }
        }
#if !SDL_VERSION_ATLEAST(2,0,0)
        else
#endif
#endif

#if !SDL_VERSION_ATLEAST(2,0,0)
        {
            for (index = 0; index < 256; index++)
            {
                palette_out[index] = SDL_MapRGB(Game_Screen->format, (*(Frame->Palette))[index].r, (*(Frame->Palette))[index].g, (*(Frame->Palette))[index].b);
            }
        }
#endif
    }


    // display frame

#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
#if !SDL_VERSION_ATLEAST(2,0,0)
    if (Game_UseOpenGL)
#endif
    {
        if (Smack->Flags & 4)
        {
            blit_double_height(NULL, SMK_TextureData);
        }
        else
        {
            blit_normal_height(NULL, SMK_TextureData);
        }

        if (Scaler_ScaleTextureData)
        {
            ScalerPlugin_scale(SMK_ScaleFactor, SMK_TextureData, SMK_ScaledTextureData, 320, 200, 1);
        }
    }
#if !SDL_VERSION_ATLEAST(2,0,0)
    else
#endif
#endif

#if !SDL_VERSION_ATLEAST(2,0,0)
    {
        SDL_LockSurface(Game_Screen);

        if (Smack->Flags & 4)
        {
            blit_double_height(Game_Screen, NULL);
        }
        else
        {
            blit_normal_height(Game_Screen, NULL);
        }

        SDL_UnlockSurface(Game_Screen);
        SDL_Flip(Game_Screen);
    }
#endif

    if (Thread_Exit) return;

    event.type = SDL_USEREVENT;
    event.user.code = EC_SMK_FUNCTION;
    event.user.data1 = (void *) &eventloop_flip;
    event.user.data2 = NULL;

    SDL_PushEvent(&event);
}

static void audio_player(void *udata, Uint8 *stream, int len)
{
    int len2;

    if (audio_read + len > audio_len)
    {
        len2 = audio_len - audio_read;

        memcpy(stream, &(audio_buf[audio_read]), len2);

        audio_read = len - len2;

        memcpy(&(stream[len2]), audio_buf, audio_read);
    }
    else
    {
        memcpy(stream, &(audio_buf[audio_read]), len);
        audio_read += len;
        if (audio_read >= audio_len) audio_read -= audio_len;
    }
}

static void play_smk(const char *filename)
{
    FILE *f;
    char filepath[260];

    int audio_track; // audio track to be played
    int frame_delay; // frame delay (in ms)
    int index;
    Uint32 lastticks, ticks;
    int delay;

#if SDL_VERSION_ATLEAST(2,0,0)
    if (Game_Window == NULL) return;
#else
    if (Game_Screen == NULL) return;
#endif

    // stop timer, events, ...
    SMK_Playing = 1;

    // hide virtual keyboard
    VK_Visible = 0;

    // open video file
    strcpy(filepath, "C:\\");
    strcat(filepath, filename);

    f = Game_fopen(filepath, "rb");
    if (f == NULL)
    {
        strcpy(filepath, "C:\\VIDEO\\");
        strcat(filepath, filename);

        f = Game_fopen(filepath, "rb");
    }

    if (f == NULL)
    {
        if (snprintf(filepath, sizeof(filepath), "C:\\%s%s", Albion_CDPath, filename) < (int)sizeof(filepath))
        {
            f = Game_fopen(filepath, "rb");
        }
    }

    if (f == NULL)
    {
        if (snprintf(filepath, sizeof(filepath), "C:\\%sVIDEO\\%s", Albion_CDPath, filename) < (int)sizeof(filepath))
        {
            f = Game_fopen(filepath, "rb");
        }
    }

    if (f == NULL)
    {
        // video file not found
        goto SMK_exit1;
    }

    // open Smacker file
    Smack = SmackOpen(f);
    if (Smack == NULL)
    {
        goto SMK_exit2;
    }

    // check smack parameters
    if (Smack->PaddedWidth == 0 ||
        Smack->PaddedWidth > 320 ||
        Smack->PaddedHeight == 0 ||
        Smack->PaddedHeight > ( (Smack->Flags & 4)?120u:240u ) ||
        Smack->Frames == 0)
    {
        goto SMK_exit3;
    }

    // choose audio track
    audio_track = -1;
    if (Game_Sound || Game_Music)
    {
        for (index = 0; index < 7; index++)
        {
            if (Smack->AudioSize[index] != 0 &&
                (Smack->AudioRate[index] & 0x40000000) != 0) // data present
            {
                audio_track = index;
                break;
            }
        }
    }

    // initialize audio conversion
    audio_buf = NULL;
    audio_cvtbuf = NULL;
    if (audio_track != -1)
    {
        if (0 > SDL_BuildAudioCVT(&audio_cvt,
                                  (Smack->AudioRate[audio_track] & 0x20000000)?AUDIO_S16LSB:AUDIO_U8,
                                  (Smack->AudioRate[audio_track] & 0x10000000)?2:1,
                                  Smack->AudioRate[audio_track] & 0x00ffffff,
                                  Game_AudioFormat,
                                  Game_AudioChannels,
                                  Game_AudioRate)
                                 )
        {
            audio_track = -1;
        }
    }

    // more initialization

    // allocate framebuffer with sound
    Frame = SmackAllocateFrame(Smack, NULL, 0, 0, (audio_track == -1)?0:1, 0, 0, 0);
    if (Frame == NULL)
    {
        goto SMK_exit3;
    }

    // allocate audio buffer
    if (audio_track != -1)
    {
        audio_directcvt = ((audio_cvt.len_mult == 1) && (audio_cvt.len_ratio == 1.0f))?1:0;

        audio_len = (3 + (int)(3 * Smack->AudioSize[audio_track] * audio_cvt.len_ratio)) & ~3;
        audio_buf = (uint8_t *) malloc(audio_len);

        if (audio_buf == NULL) goto SMK_exit4;

        if (!audio_directcvt)
        {
            audio_cvtbuf = (uint8_t *) malloc((Smack->AudioSize[audio_track] + 4) * audio_cvt.len_mult);
            if (audio_cvtbuf == NULL) goto SMK_exit4;
        }
    }

    // calculate frame delay (in ms) */
    if (Smack->FrameRate > 0) frame_delay = Smack->FrameRate;
    else if (Smack->FrameRate < 0) frame_delay = Smack->FrameRate / -100;
    else frame_delay = 100;

    // wait one frame before start
    SDL_Delay(frame_delay);

    // initialize display
    if (!initialize_display())
    {
        goto SMK_exit4;
    }

    // prebuffer frames
    SmackDecodeFrame(Smack, Frame, Frame, audio_track);
    if ( SmackError() )
    {
        goto SMK_exit5;
    }

    BufferToScreen();

    if (audio_track != -1)
    {
        audio_read = 0;

        if (audio_directcvt)
        {
            audio_write = Frame->AudioLength;

            audio_cvt.buf = audio_buf;
            audio_cvt.len = audio_write;

            memcpy(audio_buf, Frame->Audio, audio_cvt.len);

            SDL_ConvertAudio(&audio_cvt);
        }
        else
        {
            audio_cvt.buf = audio_cvtbuf;
            audio_cvt.len = Frame->AudioLength;

            memcpy(audio_cvtbuf, Frame->Audio, audio_cvt.len);

            SDL_ConvertAudio(&audio_cvt);

            audio_write = audio_cvt.len_cvt;

            memcpy(audio_buf, audio_cvtbuf, audio_write);
        }
    }

    SmackNextFrame(Smack);

    // start playing
    if (audio_track != -1)
    {
        Mix_HookMusic(&audio_player, NULL);
    }
    lastticks = SDL_GetTicks();
    delay = 0;

    while (1)
    {
        if (Thread_Exit) break;

        // check events
        while (Game_KQueueWrite != Game_KQueueRead)
        {
            SDL_Event *cevent;

            cevent = &(Game_EventKQueue[Game_KQueueRead]);

            switch(cevent->type)
            {
                case SDL_KEYUP:
                    if ((cevent->key.keysym.sym == SDLK_ESCAPE) ||
                        (cevent->key.keysym.sym == SDLK_TAB) ||
                        (cevent->key.keysym.sym == SDLK_F15)
                       )
                    {
                        goto SMK_exit5;
                    }
                    break;
            }

            Game_KQueueRead = (Game_KQueueRead + 1) & (GAME_KQUEUE_LENGTH - 1);
        }

        // calculate delay
        ticks = SDL_GetTicks();
        delay -= (ticks - lastticks);
        lastticks = ticks;

        if (delay > 0)
        {
            SDL_Delay(1);
            continue;
        }

        delay += frame_delay;

        if (Smack->CurrentFrame == 0) break;

        SmackDecodeFrame(Smack, Frame, Frame, audio_track);

        if ( SmackError() ) break;

        if (audio_track != -1)
        {
            unsigned int frame_audio_len, frame_audio_len2;

            frame_audio_len = Frame->AudioLength;

            if (frame_audio_len != 0)
            {
                if (audio_directcvt)
                {
                    if (audio_write + frame_audio_len > audio_len)
                    {
                        frame_audio_len2 = audio_len - audio_write;

                        audio_cvt.buf = &(audio_buf[audio_write]);
                        audio_cvt.len = frame_audio_len2;

                        memcpy(&(audio_buf[audio_write]), Frame->Audio, frame_audio_len2);

                        SDL_ConvertAudio(&audio_cvt);

                        audio_write = frame_audio_len - frame_audio_len2;

                        audio_cvt.buf = audio_buf;
                        audio_cvt.len = audio_write;

                        memcpy(audio_buf, &(Frame->Audio[frame_audio_len2]), audio_write);

                        SDL_ConvertAudio(&audio_cvt);
                    }
                    else
                    {
                        audio_cvt.buf = &(audio_buf[audio_write]);
                        audio_cvt.len = frame_audio_len;

                        memcpy(&(audio_buf[audio_write]), Frame->Audio, frame_audio_len);
                        audio_write += frame_audio_len;
                        if (audio_write >= audio_len) audio_write -= audio_len;

                        SDL_ConvertAudio(&audio_cvt);
                    }
                }
                else
                {
                    audio_cvt.buf = audio_cvtbuf;
                    audio_cvt.len = frame_audio_len;

                    memcpy(audio_cvtbuf, Frame->Audio, audio_cvt.len);

                    SDL_ConvertAudio(&audio_cvt);

                    frame_audio_len = audio_cvt.len_cvt;

                    if (audio_write + frame_audio_len > audio_len)
                    {
                        frame_audio_len2 = audio_len - audio_write;

                        memcpy(&(audio_buf[audio_write]), audio_cvtbuf, frame_audio_len2);

                        audio_write = frame_audio_len - frame_audio_len2;

                        memcpy(audio_buf, &(audio_cvtbuf[frame_audio_len2]), audio_write);
                    }
                    else
                    {
                        memcpy(&(audio_buf[audio_write]), audio_cvtbuf, frame_audio_len);
                        audio_write += frame_audio_len;
                        if (audio_write >= audio_len) audio_write -= audio_len;
                    }
                }

            }
        }


        BufferToScreen();

        SmackNextFrame(Smack);
    }




SMK_exit5:
    if (audio_track != -1)
    {
        Mix_HookMusic(NULL, NULL);
        if (Game_Music && Game_MidiSubsystem)
        {
            // restore audio
            if (Game_MidiSubsystem <= 10)
            {
                MidiPlugin_Restore();
            }
            else
            {
                MidiPlugin2_Restore();
            }
        }
    }
    deinitialize_display();
SMK_exit4:
    if (audio_cvtbuf != NULL)
    {
        free(audio_cvtbuf);
        audio_cvtbuf = NULL;
    }
    if (audio_buf != NULL)
    {
        free(audio_buf);
        audio_buf = NULL;
    }
    SmackDeallocateFrame(Frame);
SMK_exit3:
    SmackClose(Smack);
SMK_exit2:
    Game_fclose(f);
SMK_exit1:
    if (Thread_Exit)
    {
        Game_StopMain();
    }

    // resume timer, events, ...
    SMK_Playing = 0;
}


void SMK_ShowMenu(void)
{
    if (!firstMenu) return;

    firstMenu = 0;

    if (!Game_PlayIntro) return;

    play_smk("INTRO.SMK");
}

void SMK_PlayIntro(void)
{
    play_smk("INTRO.SMK");
}

void SMK_PlayCredits(void)
{
    play_smk("CREDITS.SMK");
}

