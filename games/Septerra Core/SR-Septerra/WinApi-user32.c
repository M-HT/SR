/**
 *
 *  Copyright (C) 2019-2024 Roman Pauer
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
#include "WinApi-user32.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "Game-Config.h"
#include "VKfont.h"

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
#define WINAPI_NODEF_DEFINITIONS
#endif
#include "WinApi.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#else
#include <time.h>
#include "CLIB.h"
unsigned int Winapi_GetTicks(void);
#endif

#define eprintf(...) fprintf(stderr,__VA_ARGS__)


#ifndef _WIN32
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define SM_SWAPBUTTON 23

#define MB_TYPEMASK          0x0000000FL
#define MB_ICONMASK          0x000000F0L
#define MB_DEFMASK           0x00000F00L
#define MB_MODEMASK          0x00003000L

#define MB_OK                0x00000000L
#define MB_OKCANCEL          0x00000001L
#define MB_ABORTRETRYIGNORE  0x00000002L
#define MB_YESNOCANCEL       0x00000003L
#define MB_YESNO             0x00000004L
#define MB_RETRYCANCEL       0x00000005L
#define MB_CANCELTRYCONTINUE 0x00000006L

#define MB_ICONHAND          0x00000010L
#define MB_ICONEXCLAMATION   0x00000030L
#define MB_ICONASTERISK      0x00000040L

#define MB_DEFBUTTON2        0x00000100L
#define MB_DEFBUTTON3        0x00000200L

#define MB_SYSTEMMODAL       0x00001000L

#define IDOK        1
#define IDCANCEL    2
#define IDABORT     3
#define IDRETRY     4
#define IDIGNORE    5
#define IDYES       6
#define IDNO        7
#define IDTRYAGAIN 10
#define IDCONTINUE 11


#define WM_NULL 0x0000
#define WM_SETFOCUS 0x0007
#define WM_KILLFOCUS 0x0008
#define WM_QUIT 0x0012
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_SYSCOMMAND 0x0112
#define WM_TIMER 0x0113
#define WM_MOUSEMOVE 0x0200
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_LBUTTONDBLCLK 0x0203
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP 0x0205
#define WM_RBUTTONDBLCLK 0x0206
#define WM_MBUTTONDOWN 0x0207
#define WM_MBUTTONUP 0x0208
#define WM_MBUTTONDBLCLK 0x0209
#define WM_USER 0x0400
#define WM_APP 0x8000

#define SC_MINIMIZE 0xF020
#define SC_CLOSE 0xF060
#define SC_RESTORE 0xF120


#define MK_CONTROL 0x0008
#define MK_LBUTTON 0x0001
#define MK_MBUTTON 0x0010
#define MK_RBUTTON 0x0002
#define MK_SHIFT 0x0004
#define MK_XBUTTON1 0x0020
#define MK_XBUTTON2 0x0040


#define VK_TAB 0x09
#define VK_CLEAR 0x0C
#define VK_RETURN 0x0D
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_CAPITAL 0x14
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_LWIN 0x5B
#define VK_RWIN 0x5C
#define VK_APPS 0x5D
#define VK_NUMPAD0 0x60
#define VK_NUMPAD1 0x61
#define VK_NUMPAD2 0x62
#define VK_NUMPAD3 0x63
#define VK_NUMPAD4 0x64
#define VK_NUMPAD5 0x65
#define VK_NUMPAD6 0x66
#define VK_NUMPAD7 0x67
#define VK_NUMPAD8 0x68
#define VK_NUMPAD9 0x69
#define VK_MULTIPLY 0x6A
#define VK_ADD 0x6B
#define VK_SUBTRACT 0x6D
#define VK_DECIMAL 0x6E
#define VK_DIVIDE 0x6F
#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7a
#define VK_F12 0x7b
#define VK_F13 0x7c
#define VK_F14 0x7d
#define VK_F15 0x7e
#define VK_F16 0x7f
#define VK_F17 0x80
#define VK_F18 0x81
#define VK_F19 0x82
#define VK_F20 0x83
#define VK_F21 0x84
#define VK_F22 0x85
#define VK_F23 0x86
#define VK_F24 0x87
#define VK_NUMLOCK 0x90
#define VK_SCROLL 0x91



#define IMAGE_BITMAP        0
#define LR_LOADFROMFILE     0x0010
#endif


typedef struct {
    uint32_t x;
    uint32_t y;
} point, *lppoint;

typedef struct {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} rect, *lprect;

typedef struct {
    PTR32(void) hwnd;
    uint32_t message;
    uint32_t wParam;
    int32_t  lParam;
    uint32_t time;
    point    pt;
} msg, *lpmsg;

typedef struct {
    uint32_t style;
    PTR32(void) lpfnWndProc;
    int32_t cbClsExtra;
    int32_t cbWndExtra;
    PTR32(void) hInstance;
    PTR32(void) hIcon;
    PTR32(void) hCursor;
    PTR32(void) hbrBackground;
    PTR32(const char) lpszMenuName;
    PTR32(const char) lpszClassName;
} wndclassa;


extern uint32_t SoundEngine_Counter;

int sdl_versionnum = 0;
static void *lpfnWndProc = NULL;
static int cursor_visibility = 0;
static unsigned int mouse_buttons = 0;
static unsigned int keyboard_mods = 0;
static int mouse_x = 0;
static int mouse_y = 0;
static unsigned int message_time = 0;
static int mouse_right_button_mod = 0;
// doubleclick detection - BEGIN
static const int mouse_doubleclick_max_distance = 10;
static const int mouse_doubleclick_max_time = 500;

static int mouse_doubleclick = 0;
static uint32_t mouse_last_time[3], mouse_current_time[3];
static int mouse_last_x[3], mouse_last_y[3], mouse_current_x[3], mouse_current_y[3];
static int mouse_last_peep[3];
// doubleclick detection - END

#if SDL_VERSION_ATLEAST(2,0,0)
static SDL_Window *mouse_window = NULL;
static SDL_Renderer *mouse_renderer = NULL;
static int mouse_clip_w = 0;
static int mouse_clip_h = 0;
#endif

static SDL_Joystick *joystick = NULL;
#if SDL_VERSION_ATLEAST(2,0,0)
static SDL_GameController *controller = NULL;
static int controller_base_axis;
#endif
static unsigned int joystick_hat_position = 0;
static int controller_axis_x = 0;
static int controller_axis_y = 0;
static int controller_mouse_motion = 0;
static int controller_frac_x;
static int controller_frac_y;
static uint32_t controller_mouse_last_time;
static SDL_Event priority_event;
static SDL_Surface *virtual_keyboard_surface = NULL;
#if SDL_VERSION_ATLEAST(2,0,0)
static SDL_Texture *virtual_keyboard_texture = NULL;
static SDL_Surface *virtual_keyboard_texture_surface = NULL;
#endif
static int virtual_keyboard_state = 0;
static unsigned int virtual_keyboard_disabled = 0;
static int virtual_keyboard_pos_x;
static int virtual_keyboard_pos_y;
static int virtual_keyboard_altlayout;

#ifdef DISPLAY_FULL_VIRTUAL_KEYBOARD
#define VK_LAYOUT_WIDTH 13
static const uint8_t virtual_keyboard_layout[2][4][VK_LAYOUT_WIDTH] = { {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M'},
    {'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'},
    {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '='},
    {',', '.', '/', ';', '\'', '[', ']', '\\', ' ', '\b', '\177', '\r', '\n'},
}, {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm'},
    {'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'},
    {'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+'},
    {'<', '>', '?', ':', '"', '{', '}', '|', ' ', '\b', '\177', '\r', '\n'},
}, };
#else
#define VK_LAYOUT_WIDTH 10
static const uint8_t virtual_keyboard_layout[2][4][VK_LAYOUT_WIDTH] = { {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'},
    {'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T'},
    {'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4'},
    {'5', '6', '7', '8', '9', '0', '\b', '\177', '\r', '\n'},
}, {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'},
    {'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't'},
    {'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4'},
    {'5', '6', '7', '8', '9', '0', ' ', '\177', '\r', '\n'},
}, };
#endif

static uint8_t virtual_keyboard_buffer[2 * VK_LAYOUT_WIDTH];
static int virtual_keyboard_buflen;


#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t RunWndProc_asm(void *hwnd, uint32_t uMsg, uint32_t wParam, uint32_t lParam, uint32_t (*WndProc)(void *, uint32_t, uint32_t, uint32_t));
#ifdef __cplusplus
}
#endif


#ifdef _WIN32
static LONG(__stdcall *NtDelayExecution)(BOOL Alertable, PLARGE_INTEGER DelayInterval);
static LONG(__stdcall *NtSetTimerResolution)(IN ULONG RequestedResolution, IN BOOLEAN Set, OUT PULONG ActualResolution);
static LONG(__stdcall *NtQueryTimerResolution)(OUT PULONG CoarsestResolution, OUT PULONG FinestResolution, OUT PULONG ActualResolution);

static int use_nt_delay;
static ULONG original_timer_resolution;

static UINT timer_period;

static void restore_timer_resolution(void)
{
    ULONG actual_resolution;

    NtSetTimerResolution(original_timer_resolution, TRUE, &actual_resolution);
}

static void end_timer_period(void)
{
    timeEndPeriod(timer_period);
}

#ifdef __cplusplus
extern "C"
#endif
void init_sleepmode(void)
{
    use_nt_delay = 0;

    if (CPU_SleepMode)
    {
        HMODULE ntdll_handle;

        ntdll_handle = GetModuleHandleA("ntdll.dll");
        if (ntdll_handle != NULL)
        {
            NtDelayExecution = (LONG(__stdcall*)(BOOL, PLARGE_INTEGER)) GetProcAddress(ntdll_handle, "NtDelayExecution");
            NtSetTimerResolution = (LONG(__stdcall*)(ULONG, BOOLEAN, PULONG)) GetProcAddress(ntdll_handle, "NtSetTimerResolution");
            NtQueryTimerResolution = (LONG(__stdcall*)(PULONG, PULONG, PULONG)) GetProcAddress(ntdll_handle, "NtQueryTimerResolution");

            if ((NtDelayExecution != NULL) && (NtSetTimerResolution != NULL) && (NtQueryTimerResolution != NULL))
            {
                ULONG coarsest_resolution, finest_resolution, actual_resolution, requested_resolution;

                if (0 <= NtQueryTimerResolution(&coarsest_resolution, &finest_resolution, &actual_resolution))
                {
                    use_nt_delay = 1;

                    requested_resolution = (finest_resolution < 5000)?5000:finest_resolution;

                    if (requested_resolution < actual_resolution)
                    {
                        original_timer_resolution = actual_resolution;

                        if (0 <= NtSetTimerResolution(requested_resolution, TRUE, &actual_resolution))
                        {
                            if (actual_resolution != original_timer_resolution)
                            {
                                atexit(restore_timer_resolution);
                            }
                        }
                    }
                }
            }
        }
    }

    if (CPU_SleepMode && !use_nt_delay)
    {
        TIMECAPS tc;

        if (MMSYSERR_NOERROR == timeGetDevCaps(&tc, sizeof(tc)))
        {
            timer_period = tc.wPeriodMin;
            if (MMSYSERR_NOERROR == timeBeginPeriod(tc.wPeriodMin))
            {
                atexit(end_timer_period);
            }
        }
    }
}
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
static void warp_mouse(int x, int y)
{
    if (mouse_renderer != NULL)
    {
#if SDL_VERSION_ATLEAST(2,0,18)
        if (sdl_versionnum >= SDL_VERSIONNUM(2,0,18))
        {
            SDL_RenderLogicalToWindow(mouse_renderer, 0.5f + x, 0.5f + y, &x, &y);
        }
        else
#endif
        {
            float scaleX, scaleY;
            SDL_Rect viewport;
            int output_w, output_h, window_w, window_h;

            SDL_RenderGetScale(mouse_renderer, &scaleX, &scaleY);
            SDL_RenderGetViewport(mouse_renderer, &viewport);
            SDL_GetRendererOutputSize(mouse_renderer, &output_w, &output_h);
            SDL_GetWindowSize(mouse_window, &window_w, &window_h);

            x = (int) (((x + viewport.x) * scaleX * window_w + output_w - scaleX) / output_w);
            y = (int) (((y + viewport.y) * scaleY * window_h + output_h - scaleY) / output_h);
        }
        SDL_WarpMouseInWindow(mouse_window, x, y);
    }
}
#endif

#ifdef __cplusplus
extern "C"
#endif
void disable_virtual_keyboard(int disable)
{
    if (disable)
    {
        virtual_keyboard_state = 0;
        virtual_keyboard_disabled++;
    }
    else
    {
        if (virtual_keyboard_disabled)
        {
            virtual_keyboard_disabled--;
        }
    }
}

#ifdef __cplusplus
extern "C"
#endif
void delete_virtual_keyboard(void)
{
    virtual_keyboard_state = 0;
    if (virtual_keyboard_surface != NULL)
    {
        SDL_FreeSurface(virtual_keyboard_surface);
        virtual_keyboard_surface = NULL;
    }
#if SDL_VERSION_ATLEAST(2,0,0)
    if (virtual_keyboard_texture != NULL)
    {
        SDL_DestroyTexture(virtual_keyboard_texture);
        virtual_keyboard_texture = NULL;
    }
    if (virtual_keyboard_texture_surface != NULL)
    {
        SDL_FreeSurface(virtual_keyboard_texture_surface);
        virtual_keyboard_texture_surface = NULL;
    }
#endif
}

static void vk_draw_chr(SDL_Surface *surface, uint8_t chr, int x, int y)
{
    const uint8_t *src;
    uint8_t *dst, prev_value, value;
    int offset, sx, sy;

    if (vkfont_chr_index[chr] < 0) return;

    src = vkfont_chr_data + vkfont_chr_size * vkfont_chr_index[chr];
    dst = ((uint8_t *)surface->pixels) + y * surface->pitch + (x >> 3);

    offset = x & 7;

    if (offset == 0)
    {
        for (sy = 0; sy < vkfont_chr_height; sy++)
        {
            for (sx = 0; sx < vkfont_chr_width_bytes; sx++)
            {
                dst[sx] |= src[sx];
            }
            src += vkfont_chr_width_bytes;
            dst += surface->pitch;
        }
    }
    else if (((8 - (vkfont_chr_width_pixels & 7)) & 7) >= offset)
    {
        for (sy = 0; sy < vkfont_chr_height; sy++)
        {
            prev_value = 0;
            for (sx = 0; sx < vkfont_chr_width_bytes; sx++)
            {
                value = src[sx];
                dst[sx] |= (value >> offset) | (prev_value << (8 - offset));
                prev_value = value;
            }
            src += vkfont_chr_width_bytes;
            dst += surface->pitch;
        }
    }
    else
    {
        for (sy = 0; sy < vkfont_chr_height; sy++)
        {
            prev_value = 0;
            for (sx = 0; sx < vkfont_chr_width_bytes; sx++)
            {
                value = src[sx];
                dst[sx] |= (value >> offset) | (prev_value << (8 - offset));
                prev_value = value;
            }
            dst[vkfont_chr_width_bytes] |= prev_value << (8 - offset);
            src += vkfont_chr_width_bytes;
            dst += surface->pitch;
        }
    }
}

static void vk_invert_area(SDL_Surface *surface, int x, int y, int w, int h)
{
    uint8_t *dst, mask1, mask2;
    int sy, sx, bytes;

    dst = ((uint8_t *)surface->pixels) + y * surface->pitch + (x >> 3);

    mask1 = 0xff >> (x & 7);
    mask2 = 0xff << ((8 - ((x + w) & 7)) & 7);

    if (w <= (8 - (x & 7)))
    {
        mask1 &= mask2;

        for (sy = 0; sy < h; sy++)
        {
            *dst ^= mask1;
            dst += surface->pitch;
        }
    }
    else if (w <= (16 - (x & 7)))
    {
        for (sy = 0; sy < h; sy++)
        {
            dst[0] ^= mask1;
            dst[1] ^= mask2;
            dst += surface->pitch;
        }
    }
    else
    {
        bytes = (w + (x & 7) - 1) >> 3;

        for (sy = 0; sy < h; sy++)
        {
            dst[0] ^= mask1;
            for (sx = 1; sx < bytes; sx++)
            {
                dst[sx] ^= 0xff;
            }
            dst[bytes] ^= mask2;
            dst += surface->pitch;
        }
    }
}

#ifdef __cplusplus
extern "C"
#endif
int display_virtual_keyboard(
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_Renderer *renderer
#else
    SDL_Surface *surface, SDL_Rect *update_area
#endif
)
{
    SDL_Rect vkrect;
    int row, column;

    if (!virtual_keyboard_state) return 0;

    vkrect.w = (vkfont_chr_width_pixels + 2 * 2) * VK_LAYOUT_WIDTH + 4 * 2;
    vkrect.h = (vkfont_chr_height + 2 * 2) * 6 + 4 * 2;
    vkrect.x = (640 - vkrect.w) >> 1;
    vkrect.y = (480 - vkrect.h) >> 1;

    if (virtual_keyboard_state < 0)
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderFillRect(renderer, &vkrect);
#else
        SDL_FillRect(surface, &vkrect, 0);
#endif

        virtual_keyboard_state = 0;
    }
    else
    {
        if (virtual_keyboard_surface == NULL)
        {
            virtual_keyboard_surface = SDL_CreateRGBSurface(0, vkrect.w, vkrect.h, 1, 0, 0, 0, 0);
            if (virtual_keyboard_surface == NULL)
            {
                return 0;
            }

            virtual_keyboard_surface->format->palette->colors[0].r = 0;
            virtual_keyboard_surface->format->palette->colors[0].g = 0;
            virtual_keyboard_surface->format->palette->colors[0].b = 0;
            virtual_keyboard_surface->format->palette->colors[1].r = 255;
            virtual_keyboard_surface->format->palette->colors[1].g = 255;
            virtual_keyboard_surface->format->palette->colors[1].b = 255;
#if SDL_VERSION_ATLEAST(2,0,0)
            virtual_keyboard_surface->format->palette->colors[0].a = 128;
            virtual_keyboard_surface->format->palette->colors[1].a = 255;
#endif
        }
#if SDL_VERSION_ATLEAST(2,0,0)
        if (virtual_keyboard_texture == NULL)
        {
            Uint32 format, Rmask, Gmask, Bmask, Amask;
            SDL_RendererInfo info;
            int index, bpp;

            format = SDL_PIXELFORMAT_ABGR8888;
            if (SDL_GetRendererInfo(renderer, &info) <= 0)
            {
                for (index = 0; index < (int)info.num_texture_formats; ++index)
                {
                    if (info.texture_formats[index] == SDL_PIXELFORMAT_ARGB8888 ||
                        info.texture_formats[index] == SDL_PIXELFORMAT_ABGR8888 ||
                        info.texture_formats[index] == SDL_PIXELFORMAT_RGB888 ||
                        info.texture_formats[index] == SDL_PIXELFORMAT_BGR888) {
                        format = info.texture_formats[index];
                        break;
                    }
                }
            }

            SDL_PixelFormatEnumToMasks(format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
            virtual_keyboard_texture_surface = SDL_CreateRGBSurfaceFrom(NULL, vkrect.w, vkrect.h, bpp, 0, Rmask, Gmask, Bmask, Amask);
            if (virtual_keyboard_texture_surface == NULL)
            {
                return 0;
            }

            virtual_keyboard_texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING, vkrect.w, vkrect.h);
            if (virtual_keyboard_texture == NULL)
            {
                SDL_FreeSurface(virtual_keyboard_texture_surface);
                virtual_keyboard_texture_surface = NULL;
                return 0;
            }

            if (Amask)
            {
                SDL_SetTextureBlendMode(virtual_keyboard_texture, SDL_BLENDMODE_BLEND);
            }
        }
#endif

        memset(virtual_keyboard_surface->pixels, 0, virtual_keyboard_surface->h * virtual_keyboard_surface->pitch);

        for (row = 0; row < 4; row++)
        {
            for (column = 0; column < VK_LAYOUT_WIDTH; column++)
            {
                vk_draw_chr(virtual_keyboard_surface, virtual_keyboard_layout[virtual_keyboard_altlayout][row][column], (vkfont_chr_width_pixels + 2 * 2) * column + 2 + 4, (vkfont_chr_height + 2 * 2) * row + 2 + 4);
            }
        }

        if (virtual_keyboard_buflen <= VK_LAYOUT_WIDTH)
        {
            for (column = 0; column < virtual_keyboard_buflen; column++)
            {
                vk_draw_chr(virtual_keyboard_surface, virtual_keyboard_buffer[column], (vkfont_chr_width_pixels + 2 * 2) * column + 2 + 4, (vkfont_chr_height + 2 * 2) * 5 + 2 + 4);
            }
        }
        else
        {
            for (column = 0; column < VK_LAYOUT_WIDTH; column++)
            {
                vk_draw_chr(virtual_keyboard_surface, virtual_keyboard_buffer[column], (vkfont_chr_width_pixels + 2 * 2) * column + 2 + 4, (vkfont_chr_height + 2 * 2) * 4 + 2 + 4);
            }
            for (column = VK_LAYOUT_WIDTH; column < virtual_keyboard_buflen; column++)
            {
                vk_draw_chr(virtual_keyboard_surface, virtual_keyboard_buffer[column], (vkfont_chr_width_pixels + 2 * 2) * (column - VK_LAYOUT_WIDTH) + 2 + 4, (vkfont_chr_height + 2 * 2) * 5 + 2 + 4);
            }
        }

        if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
        {
            vk_invert_area(virtual_keyboard_surface, (vkfont_chr_width_pixels + 2 * 2) * (VK_LAYOUT_WIDTH - 2) + 4, (vkfont_chr_height + 2 * 2) * 3 + 4, (vkfont_chr_width_pixels + 2 * 2) * 2, vkfont_chr_height + 2 * 2);
        }
        else
        {
            vk_invert_area(virtual_keyboard_surface, (vkfont_chr_width_pixels + 2 * 2) * virtual_keyboard_pos_x + 4, (vkfont_chr_height + 2 * 2) * virtual_keyboard_pos_y + 4, vkfont_chr_width_pixels + 2 * 2, vkfont_chr_height + 2 * 2);
        }

#if SDL_VERSION_ATLEAST(2,0,0)
        if (SDL_LockTexture(virtual_keyboard_texture, NULL, &virtual_keyboard_texture_surface->pixels, &virtual_keyboard_texture_surface->pitch) >= 0)
        {
            SDL_UpperBlit(virtual_keyboard_surface, NULL, virtual_keyboard_texture_surface, NULL);
            SDL_UnlockTexture(virtual_keyboard_texture);
        }

        SDL_RenderCopy(renderer, virtual_keyboard_texture, NULL, &vkrect);
#else
        SDL_UpperBlit(virtual_keyboard_surface, NULL, surface, &vkrect);
#endif
    }

#if !SDL_VERSION_ATLEAST(2,0,0)
    if (update_area != NULL)
    {
        if (vkrect.x > update_area->x) vkrect.x = update_area->x;
        if (vkrect.y > update_area->y) vkrect.y = update_area->y;
        if ((vkrect.x + vkrect.w) < (update_area->x + update_area->w)) vkrect.w = update_area->x + update_area->w - vkrect.x;
        if ((vkrect.y + vkrect.h) < (update_area->y + update_area->h)) vkrect.h = update_area->y + update_area->h - vkrect.y;

        SDL_UpdateRect(surface, vkrect.x, vkrect.y, vkrect.w, vkrect.h);
    }
#endif

    return 1;
}

static void open_controller_or_joystick(int device_index)
{
    int num_joysticks, index;

    num_joysticks = SDL_NumJoysticks();

#if SDL_VERSION_ATLEAST(2,0,0)
    // prefer game controllers over joysticks
    for (index = 0; index < num_joysticks; index++)
    {
        if ((device_index >= 0) && (index != device_index))
        {
            continue;
        }

        if (SDL_IsGameController(index))
        {
            controller = SDL_GameControllerOpen(index);
            if (controller != NULL)
            {
#if SDL_VERSION_ATLEAST(2,0,14)
                if (sdl_versionnum >= SDL_VERSIONNUM(2,0,14))
                {
                    if (SDL_GameControllerHasAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) && SDL_GameControllerHasAxis(controller, SDL_CONTROLLER_AXIS_LEFTY))
                    {
                        controller_base_axis = SDL_CONTROLLER_AXIS_LEFTX;
                    }
                    else if (SDL_GameControllerHasAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) && SDL_GameControllerHasAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY))
                    {
                        controller_base_axis = SDL_CONTROLLER_AXIS_RIGHTX;
                    }
                    else
                    {
                        SDL_GameControllerClose(controller);
                        controller = NULL;
                        continue;
                    }
                }
                else
#endif
                controller_base_axis = SDL_CONTROLLER_AXIS_LEFTX;
                printf("Using controller: %s (%s)\n", SDL_GameControllerName(controller), SDL_JoystickName(SDL_GameControllerGetJoystick(controller)));
                break;
            }
        }
    }

    if (controller == NULL)
#endif
    for (index = 0; index < num_joysticks; index++)
    {
        if ((device_index >= 0) && (index != device_index))
        {
            continue;
        }

        joystick = SDL_JoystickOpen(index);
        if (joystick != NULL)
        {
            if (SDL_JoystickNumAxes(joystick) < 2)
            {
                SDL_JoystickClose(joystick);
                joystick = NULL;
                continue;
            }

            printf("Using joystick: %s\n",
#if SDL_VERSION_ATLEAST(2,0,0)
                SDL_JoystickName(joystick)
#else
                SDL_JoystickName(index)
#endif
            );
            break;
        }
    }
}

static void handle_mouse_button(int pressed, int mouse_button, int event_button, int force_detect, int remove)
{
    if (pressed)
    {
        mouse_buttons |= SDL_BUTTON(event_button);

#if SDL_VERSION_ATLEAST(2,0,2)
        if (force_detect || (sdl_versionnum < SDL_VERSIONNUM(2,0,2)))
#endif
        {
            if (mouse_button >= 0)
            {
                if (!mouse_last_peep[mouse_button])
                {
#ifdef _WIN32
                    mouse_current_time[mouse_button] = GetTickCount();
#else
                    mouse_current_time[mouse_button] = Winapi_GetTicks();
#endif
                    mouse_current_x[mouse_button] = mouse_x;
                    mouse_current_y[mouse_button] = mouse_y;
                }

                if ((((uint32_t)(mouse_current_time[mouse_button] - mouse_last_time[mouse_button])) < mouse_doubleclick_max_time) &&
                    (abs(mouse_current_x[mouse_button] - mouse_last_x[mouse_button]) < mouse_doubleclick_max_distance) &&
                    (abs(mouse_current_y[mouse_button] - mouse_last_y[mouse_button]) < mouse_doubleclick_max_distance)
                   )
                {
                    mouse_doubleclick = 1;
                }

                if (remove)
                {
                    mouse_last_time[mouse_button] = mouse_current_time[mouse_button];
                    mouse_last_x[mouse_button] = mouse_current_x[mouse_button];
                    mouse_last_y[mouse_button] = mouse_current_y[mouse_button];
                }
                mouse_last_peep[mouse_button] = (remove)?0:1;
            }
        }
    }
    else
    {
        mouse_buttons &= ~SDL_BUTTON(event_button);
    }
}

static int check_controller_event(SDL_Event *event, int remove)
{
    int x, y, dx, dy, newx, newy;
    uint32_t diff;

    x = controller_axis_x;
    if ((x <= Controller_Deadzone) && (x >= -Controller_Deadzone)) x = 0;
    y = controller_axis_y;
    if ((y <= Controller_Deadzone) && (y >= -Controller_Deadzone)) y = 0;

    if (virtual_keyboard_state)
    {
        if ((x <= 8192) && (x >= -8192)) x = 0;
        if ((y <= 8192) && (y >= -8192)) y = 0;

        if ((x != 0) || (y != 0))
        {
            diff = SoundEngine_Counter - controller_mouse_last_time;

            if (diff >= 10)
            {
                controller_mouse_last_time = SoundEngine_Counter;

                if (y > 0)
                {
                    virtual_keyboard_pos_y = (virtual_keyboard_pos_y < 3) ? virtual_keyboard_pos_y + 1 : 0;
                }
                else if (y < 0)
                {
                    virtual_keyboard_pos_y = (virtual_keyboard_pos_y) ? virtual_keyboard_pos_y - 1 : 3;
                }

                if (x > 0)
                {
                    if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
                    {
                        virtual_keyboard_pos_x = 0;
                    }
                    else
                    {
                        virtual_keyboard_pos_x = (virtual_keyboard_pos_x < (VK_LAYOUT_WIDTH - 1)) ? virtual_keyboard_pos_x + 1 : 0;
                    }
                }
                else if (x < 0)
                {
                    if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
                    {
                        virtual_keyboard_pos_x = (VK_LAYOUT_WIDTH - 3);
                    }
                    else
                    {
                        virtual_keyboard_pos_x = (virtual_keyboard_pos_x) ? virtual_keyboard_pos_x - 1 : (VK_LAYOUT_WIDTH - 1);
                    }
                }
            }
        }

        return 0;
    }

    if ((x != 0) || (y != 0))
    {
        diff = SoundEngine_Counter - controller_mouse_last_time;

        if (diff > 0)
        {
            if (!controller_mouse_motion)
            {
                diff = 1;
                controller_frac_x = 0;
                controller_frac_y = 0;
            }

            controller_mouse_motion = 1;
            controller_mouse_last_time = SoundEngine_Counter;

            dx = x * diff + controller_frac_x;
            newx = dx >> 12;
            controller_frac_x = dx - (newx << 12);
            newx += mouse_x;

            dy = y * diff + controller_frac_y;
            newy = dy >> 12;
            controller_frac_y = dy - (newy << 12);
            newy += mouse_y;

#if SDL_VERSION_ATLEAST(2,0,0)
            if (Display_Mode)
            {
                if (newx < 0) newx = 0;
                else if (newx >= mouse_clip_w) newx = mouse_clip_w - 1;

                if (newy < 0) newy = 0;
                else if (newy >= mouse_clip_h) newy = mouse_clip_h - 1;
            }
            else
#endif
            {
                if (newx < 0) newx = 0;
                else if (newx >= 640) newx = 639;

                if (newy < 0) newy = 0;
                else if (newy >= 480) newy = 479;
            }


            event->type = SDL_MOUSEMOTION;
            event->motion.state = mouse_buttons;
            event->motion.x = newx;
            event->motion.y = newy;
            event->motion.xrel = newx - mouse_x;
            event->motion.yrel = newy - mouse_y;

            mouse_x = newx;
            mouse_y = newy;

            if (!remove)
            {
                priority_event = *event;
            }

            return 1;
        }
    }
    else
    {
        controller_mouse_motion = 0;
    }

    return 0;
}

static int find_event(SDL_Event *event, int remove, int wait)
{
    int pump_events, can_sleep;

    if (priority_event.type)
    {
        *event = priority_event;
        if (remove)
        {
            priority_event.type = 0;
        }
        return 1;
    }

    pump_events = 1;
    can_sleep = (CPU_SleepMode != 2)?1:0;

    while (1)
    {
        int num_events, keep_event;
        int mouse_button;
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_Window *window;
        SDL_Renderer *renderer;
#endif


#if SDL_VERSION_ATLEAST(2,0,0)
        num_events = SDL_PeepEvents(event, 1, (remove)?SDL_GETEVENT:SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
#else
        num_events = SDL_PeepEvents(event, 1, (remove)?SDL_GETEVENT:SDL_PEEKEVENT, SDL_ALLEVENTS);
#endif
        if (num_events < 0) return 0; // error
        if (num_events == 0) // no events
        {
            if (wait)
            {
                if (SDL_WaitEvent(NULL)) continue;
            }
            else if (pump_events)
            {
                pump_events = 0;
                SDL_PumpEvents();
                continue;
            }
            else if (can_sleep)
            {
                if (Display_DelayAfterFlip
#if SDL_VERSION_ATLEAST(2,0,0)
                    || Display_VSync
#endif
                )
                {
                    static Uint32 last_time;
                    Uint32 current_time;

                    current_time = SDL_GetTicks();
                    if (current_time != last_time)
                    {
                        last_time = current_time;
                        can_sleep = 0;
                    }
                }

                if (can_sleep)
                {
                    // Send Septerra Core to sleep to prevent it from consuming too much cpu
#ifdef _WIN32
                    if (use_nt_delay)
                    {
                        LARGE_INTEGER delay;

                        delay.QuadPart = -5000;
                        NtDelayExecution(FALSE, &delay);
                    }
                    else
                    {
                        Sleep(1);
                    }
#else
                    struct timespec _tp;

                    _tp.tv_sec = 0;
                    _tp.tv_nsec = (CPU_SleepMode == 0)?1000000:500000;

                    nanosleep(&_tp, NULL);
#endif
                }
            }

            if (check_controller_event(event, remove))
            {
                return 1;
            }

            return 0;
        }


        keep_event = 0;
        switch (event->type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            // key events
            if (!virtual_keyboard_state)
            {
                keep_event = 1;
            }
            keyboard_mods = event->key.keysym.mod;
            {
                int modvalue;
                switch(event->key.keysym.sym)
                {
                case SDLK_RSHIFT:
                    modvalue = KMOD_RSHIFT;
                    break;

                case SDLK_LSHIFT:
                    modvalue = KMOD_LSHIFT;
                    break;

                case SDLK_RCTRL:
                    modvalue = KMOD_RCTRL;
                    break;

                case SDLK_LCTRL:
                    modvalue = KMOD_LCTRL;
                    break;

                case SDLK_RALT:
                    modvalue = KMOD_RALT;
                    break;

                case SDLK_LALT:
                    modvalue = KMOD_LALT;
                    break;

#if SDL_VERSION_ATLEAST(2,0,0)
                case SDLK_LGUI:
                    modvalue = KMOD_LGUI;
                    break;

                case SDLK_RGUI:
                    modvalue = KMOD_RGUI;
                    break;
#endif

                default:
                    modvalue = KMOD_NONE;
                    break;
                }

                if (event->type == SDL_KEYDOWN)
                {
                    keyboard_mods |= modvalue;
                }
                else
                {
                    keyboard_mods &= ~modvalue;
                }
            }
            break;

        case SDL_MOUSEMOTION:
            // mouse motion events
            if (!virtual_keyboard_state)
            {
                keep_event = 1;
            }
            mouse_buttons = event->motion.state;
            if (mouse_right_button_mod && (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)))
            {
                mouse_buttons = (mouse_buttons & ~(SDL_BUTTON(SDL_BUTTON_LEFT))) | SDL_BUTTON(SDL_BUTTON_RIGHT);
            }
            mouse_x = event->motion.x;
            mouse_y = event->motion.y;
#if SDL_VERSION_ATLEAST(2,0,0)
            if (Display_Mode)
            {
                int newx, newy;

                if (mouse_x < 0) mouse_x = 0;
                else if (mouse_x >= mouse_clip_w) mouse_x = mouse_clip_w - 1;

                if (mouse_y < 0) mouse_y = 0;
                else if (mouse_y >= mouse_clip_h) mouse_y = mouse_clip_h - 1;

                newx = event->motion.x;
                newy = event->motion.y;
                if (event->motion.xrel > 0)
                {
                    if (newx < 0) newx = event->motion.xrel;
                }
                else if (event->motion.xrel < 0)
                {
                    if (newx >= mouse_clip_w) newx = mouse_clip_w + event->motion.xrel - 1;
                }

                if (event->motion.yrel > 0)
                {
                    if (newy < 0) newy = event->motion.yrel;
                }
                else if (event->motion.yrel < 0)
                {
                    if (newy >= mouse_clip_h) newy = mouse_clip_h + event->motion.yrel - 1;
                }

                if ((newx != event->motion.x) || (newy != event->motion.y))
                {
                    warp_mouse(newx, newy);
                }
            }
#endif
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            // mouse button events
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                if (event->button.state == SDL_PRESSED)
                {
#ifdef PANDORA
                    if (keyboard_mods & KMOD_RSHIFT) // L button on Pandora
                    {
                        event->button.button = SDL_BUTTON_RIGHT;
                        mouse_right_button_mod = 1;
                    }
#endif
                }
                else
                {
                    if (mouse_right_button_mod)
                    {
                        event->button.button = SDL_BUTTON_RIGHT;
                        if (remove) mouse_right_button_mod = 0;
                    }
                }
            }

            mouse_doubleclick = 0;

            if (event->button.button == SDL_BUTTON_LEFT) mouse_button = 0;
            else if (event->button.button == SDL_BUTTON_RIGHT) mouse_button = 1;
            else if (event->button.button == SDL_BUTTON_MIDDLE) mouse_button = 2;
            else mouse_button = -1;
            if (mouse_button >= 0)
            {
                if (!virtual_keyboard_state)
                {
                    keep_event = 1;
                }
            }

            mouse_x = event->button.x;
            mouse_y = event->button.y;
            handle_mouse_button((event->button.state == SDL_PRESSED)?1:0, mouse_button, event->button.button, 0, remove);
#if SDL_VERSION_ATLEAST(2,0,0)
            if (Display_Mode)
            {
                if (mouse_x < 0) mouse_x = 0;
                else if (mouse_x >= mouse_clip_w) mouse_x = mouse_clip_w - 1;

                if (mouse_y < 0) mouse_y = 0;
                else if (mouse_y >= mouse_clip_h) mouse_y = mouse_clip_h - 1;
            }
#endif
            break;

        case SDL_JOYAXISMOTION:
            // joystick axis motion event
            if (joystick != NULL)
            {
                if (event->jaxis.axis == 0)
                {
                    controller_axis_x = event->jaxis.value;
                }
                else if (event->jaxis.axis == 1)
                {
                    controller_axis_y = event->jaxis.value;
                }

                if (check_controller_event(event, remove))
                {
                    keep_event = 1;
                }
            }
            break;

        case SDL_JOYBALLMOTION:
            // joystick trackball motion event
            break;

        case SDL_JOYHATMOTION:
            // joystick hat motion event
            if (joystick != NULL)
            {
                if (event->jhat.hat == 0)
                {
                    if ((event->jhat.value & 0x0f) != joystick_hat_position)
                    {
                        if (virtual_keyboard_state)
                        {
                            if ((event->jhat.value & 1) && !(joystick_hat_position & 1)) // UP
                            {
                                virtual_keyboard_pos_y = (virtual_keyboard_pos_y) ? virtual_keyboard_pos_y - 1 : 3;
                            }
                            if ((event->jhat.value & 2) && !(joystick_hat_position & 2)) // RIGHT
                            {
                                if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
                                {
                                    virtual_keyboard_pos_x = 0;
                                }
                                else
                                {
                                    virtual_keyboard_pos_x = (virtual_keyboard_pos_x < (VK_LAYOUT_WIDTH - 1)) ? virtual_keyboard_pos_x + 1 : 0;
                                }
                            }
                            if ((event->jhat.value & 4) && !(joystick_hat_position & 4)) // DOWN
                            {
                                virtual_keyboard_pos_y = (virtual_keyboard_pos_y < 3) ? virtual_keyboard_pos_y + 1 : 0;
                            }
                            if ((event->jhat.value & 8) && !(joystick_hat_position & 8)) // LEFT
                            {
                                if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
                                {
                                    virtual_keyboard_pos_x = VK_LAYOUT_WIDTH - 3;
                                }
                                else
                                {
                                    virtual_keyboard_pos_x = (virtual_keyboard_pos_x) ? virtual_keyboard_pos_x - 1 : (VK_LAYOUT_WIDTH - 1);
                                }
                            }
                        }
                        else
                        {
                            keep_event = 1;
                        }
                        event->jhat.value = (event->jhat.value & 0x0f) | (joystick_hat_position << 4); // store previous hat position in upper nibble
                        if (remove || !keep_event)
                        {
                            joystick_hat_position = event->jhat.value & 0x0f;
                        }
                    }
                }
            }
            break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            // joystick button event
            if (joystick != NULL)
            {
                mouse_button = -1;
                switch (event->jbutton.button)
                {
                case 0:
                case 1:
                    if (virtual_keyboard_state)
                    {
                        if (event->type == SDL_JOYBUTTONDOWN)
                        {
                            if (event->jbutton.button == 0)
                            {
                                if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 3)))
                                {
                                    if (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2))
                                    {
                                        if (virtual_keyboard_state > 0)
                                        {
                                            virtual_keyboard_state = -1;
                                            if (virtual_keyboard_buflen)
                                            {
                                                priority_event.type = SDL_USEREVENT;
                                                priority_event.user.code = 4;
                                                priority_event.user.data1 = NULL;
                                                priority_event.user.data2 = NULL;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        if (virtual_keyboard_buflen)
                                        {
                                            virtual_keyboard_buflen--;
                                        }
                                    }
                                }
                                else
                                {
                                    if (virtual_keyboard_buflen < 2 * VK_LAYOUT_WIDTH)
                                    {
                                        virtual_keyboard_buffer[virtual_keyboard_buflen] = virtual_keyboard_layout[virtual_keyboard_altlayout][virtual_keyboard_pos_y][virtual_keyboard_pos_x];
                                        virtual_keyboard_buflen++;
                                    }
                                }
                            }
                            else
                            {
                                if (virtual_keyboard_buflen)
                                {
                                    virtual_keyboard_buflen--;
                                }
                            }
                        }
                    }
                    else
                    {
                        mouse_button = event->jbutton.button;
                    }
                    break;
                case 2:
                    if (event->type == SDL_JOYBUTTONDOWN)
                    {
                        if (!virtual_keyboard_disabled)
                        {
                            if (virtual_keyboard_state <= 0)
                            {
                                virtual_keyboard_state = 1;
                                virtual_keyboard_pos_x = 0;
                                virtual_keyboard_pos_y = 0;
                                virtual_keyboard_altlayout = 0;
                                virtual_keyboard_buflen = 0;
                            }
                            else
                            {
                                virtual_keyboard_state = -1;
                            }
                        }
                    }
                    break;
                case 3:
                    if (virtual_keyboard_state)
                    {
                        if (event->type == SDL_JOYBUTTONDOWN)
                        {
                            virtual_keyboard_altlayout = 1 - virtual_keyboard_altlayout;
                        }
                    }
                    else
                    {
                        keep_event = 1;
                    }
                    break;
                default:
                    break;
                }

                if (mouse_button >= 0)
                {
                    keep_event = 1;
                    mouse_doubleclick = 0;
                    handle_mouse_button((event->type == SDL_JOYBUTTONDOWN)?1:0, mouse_button, (mouse_button)?SDL_BUTTON_RIGHT:SDL_BUTTON_LEFT, 1, remove);
                }
            }
            break;

        case SDL_QUIT:
            virtual_keyboard_state = 0;
            keep_event = 1;
            break;

        case SDL_SYSWMEVENT:
            // system specific event
            break;

        case SDL_USEREVENT:
            // user events
            if ((event->user.code >= 0) && (event->user.code <= 3))
            {
                keep_event = 1;
            }
            break;

#if SDL_VERSION_ATLEAST(2,0,0)
#if SDL_VERSION_ATLEAST(2,0,4)
        case SDL_AUDIODEVICEADDED:
        case SDL_AUDIODEVICEREMOVED:
            // audio device event
            break;
#endif

        case  SDL_CONTROLLERAXISMOTION:
            // game controller axis motion event
            if (controller != NULL)
            {
                if (event->caxis.axis == controller_base_axis)
                {
                    controller_axis_x = event->caxis.value;
                }
                else if (event->caxis.axis == (controller_base_axis + 1))
                {
                    controller_axis_y = event->caxis.value;
                }

                if (check_controller_event(event, remove))
                {
                    keep_event = 1;
                }
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            // game controller button event
            if (controller != NULL)
            {
                mouse_button = -1;
                switch (event->cbutton.button)
                {
                case SDL_CONTROLLER_BUTTON_A:
                case SDL_CONTROLLER_BUTTON_X:
                    if (virtual_keyboard_state)
                    {
                        if (event->type == SDL_CONTROLLERBUTTONDOWN)
                        {
                            if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 3)))
                            {
                                if (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2))
                                {
                                    if (virtual_keyboard_state > 0)
                                    {
                                        virtual_keyboard_state = -1;
                                        if (virtual_keyboard_buflen)
                                        {
                                            priority_event.type = SDL_USEREVENT;
                                            priority_event.user.code = 4;
                                            priority_event.user.data1 = NULL;
                                            priority_event.user.data2 = NULL;
                                        }
                                    }
                                }
                                else
                                {
                                    if (virtual_keyboard_buflen)
                                    {
                                        virtual_keyboard_buflen--;
                                    }
                                }
                            }
                            else
                            {
                                if (virtual_keyboard_buflen < 2 * VK_LAYOUT_WIDTH)
                                {
                                    virtual_keyboard_buffer[virtual_keyboard_buflen] = virtual_keyboard_layout[virtual_keyboard_altlayout][virtual_keyboard_pos_y][virtual_keyboard_pos_x];
                                    virtual_keyboard_buflen++;
                                }
                            }
                        }
                    }
                    else
                    {
                        mouse_button = 0;
                    }
                    break;
                case SDL_CONTROLLER_BUTTON_B:
                case SDL_CONTROLLER_BUTTON_Y:
                    if (virtual_keyboard_state)
                    {
                        if (event->type == SDL_CONTROLLERBUTTONDOWN)
                        {
                            if (virtual_keyboard_buflen)
                            {
                                virtual_keyboard_buflen--;
                            }
                        }
                    }
                    else
                    {
                        mouse_button = 1;
                    }
                    break;

                case SDL_CONTROLLER_BUTTON_START:
                    if (event->type == SDL_CONTROLLERBUTTONDOWN)
                    {
                        if (!virtual_keyboard_disabled)
                        {
                            if (virtual_keyboard_state <= 0)
                            {
                                virtual_keyboard_state = 1;
                                virtual_keyboard_pos_x = 0;
                                virtual_keyboard_pos_y = 0;
                                virtual_keyboard_altlayout = 0;
                                virtual_keyboard_buflen = 0;
                            }
                            else
                            {
                                virtual_keyboard_state = -1;
                            }
                        }
                    }
                    break;

                case SDL_CONTROLLER_BUTTON_BACK:
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    if (virtual_keyboard_state)
                    {
                        if (event->type == SDL_CONTROLLERBUTTONDOWN)
                        {
                            switch (event->cbutton.button)
                            {
                            case SDL_CONTROLLER_BUTTON_BACK:
                                virtual_keyboard_altlayout = 1 - virtual_keyboard_altlayout;
                                break;
                            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                                virtual_keyboard_pos_y = (virtual_keyboard_pos_y) ? virtual_keyboard_pos_y - 1 : 3;
                                break;
                            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                                virtual_keyboard_pos_y = (virtual_keyboard_pos_y < 3) ? virtual_keyboard_pos_y + 1 : 0;
                                break;
                            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                                if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
                                {
                                    virtual_keyboard_pos_x = (VK_LAYOUT_WIDTH - 3);
                                }
                                else
                                {
                                    virtual_keyboard_pos_x = (virtual_keyboard_pos_x) ? virtual_keyboard_pos_x - 1 : (VK_LAYOUT_WIDTH - 1);
                                }
                                break;
                            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                                if ((virtual_keyboard_pos_y == 3) && (virtual_keyboard_pos_x >= (VK_LAYOUT_WIDTH - 2)))
                                {
                                    virtual_keyboard_pos_x = 0;
                                }
                                else
                                {
                                    virtual_keyboard_pos_x = (virtual_keyboard_pos_x < (VK_LAYOUT_WIDTH - 1)) ? virtual_keyboard_pos_x + 1 : 0;
                                }
                                break;
                            }
                        }
                    }
                    else
                    {
                        keep_event = 1;
                    }
                    break;

                default:
                    break;
                }

                if (mouse_button >= 0)
                {
                    keep_event = 1;
                    mouse_doubleclick = 0;
                    handle_mouse_button((event->type == SDL_CONTROLLERBUTTONDOWN)?1:0, mouse_button, (mouse_button)?SDL_BUTTON_RIGHT:SDL_BUTTON_LEFT, 1, remove);
                }
            }
            break;

        case SDL_CONTROLLERDEVICEADDED:
            // controller device event
            if (Input_GameController && (joystick == NULL)
#if SDL_VERSION_ATLEAST(2,0,0)
                && (controller == NULL)
#endif
            )
            {
                open_controller_or_joystick(event->cdevice.which);
            }
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            // controller device event
            if (controller != NULL)
            {
                if (SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)) == event->cdevice.which)
                {
                    SDL_GameControllerClose(controller);
                    controller = NULL;

                    controller_axis_x = 0;
                    controller_axis_y = 0;
                    controller_mouse_motion = 0;

                    if (virtual_keyboard_state)
                    {
                        virtual_keyboard_state = -1;
                    }

                    printf("Controller disconnected\n");
                }
            }
            break;
        case SDL_CONTROLLERDEVICEREMAPPED:
            // controller device event
            break;

        case SDL_DOLLARGESTURE:
        case SDL_DOLLARRECORD:
            // complex gesture event
            break;

        case SDL_DROPFILE:
#if SDL_VERSION_ATLEAST(2,0,5)
        case SDL_DROPTEXT:
        case SDL_DROPBEGIN:
        case SDL_DROPCOMPLETE:
#endif
            // event used to request a file open by the system
            break;

        case SDL_FINGERMOTION:
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
            // finger touch event
            break;

        case SDL_JOYDEVICEADDED:
            // joystick device event
            if (Input_GameController && (joystick == NULL)
#if SDL_VERSION_ATLEAST(2,0,0)
                && (controller == NULL)
#endif
            )
            {
                open_controller_or_joystick(event->jdevice.which);
            }
            break;
        case SDL_JOYDEVICEREMOVED:
            // joystick device event
            if (joystick != NULL)
            {
                if (SDL_JoystickInstanceID(joystick) == event->jdevice.which)
                {
                    SDL_JoystickClose(joystick);
                    joystick = NULL;

                    joystick_hat_position = 0;
                    controller_axis_x = 0;
                    controller_axis_y = 0;
                    controller_mouse_motion = 0;

                    if (virtual_keyboard_state)
                    {
                        virtual_keyboard_state = -1;
                    }

                    printf("Joystick disconnected\n");
                }
            }
            break;

        case SDL_MOUSEWHEEL:
            // mouse wheel event
            break;

        case SDL_MULTIGESTURE:
            // multiple finger gesture event
            break;

        case SDL_TEXTEDITING:
            // keyboard text editing event
            break;

        case SDL_TEXTINPUT:
            // keyboard text input event
            break;

        case SDL_WINDOWEVENT:
            // window state change event
            switch (event->window.event)
            {
            case SDL_WINDOWEVENT_MINIMIZED:
            case SDL_WINDOWEVENT_RESTORED:
            case SDL_WINDOWEVENT_FOCUS_GAINED:
            case SDL_WINDOWEVENT_FOCUS_LOST:
                keep_event = 1;
                break;
            case SDL_WINDOWEVENT_SHOWN:
                window = SDL_GetWindowFromID(event->motion.windowID);
                renderer = (window != NULL)?SDL_GetRenderer(window):NULL;
                if (renderer != NULL)
                {
                    mouse_window = window;
                    mouse_renderer = renderer;
                    SDL_RenderGetLogicalSize(renderer, &mouse_clip_w, &mouse_clip_h);
                }
                break;
            default:
                break;
            }
            break;

#else
        case SDL_ACTIVEEVENT:
            // Application loses/gains visibility
            switch (event->active.state)
            {
            case SDL_APPMOUSEFOCUS:
                break;
            case SDL_APPINPUTFOCUS:
            case SDL_APPACTIVE:
                keep_event = 1;
                break;
            default:
                break;
            }
            break;

        case SDL_EVENT_RESERVEDA:
        case SDL_EVENT_RESERVEDB:
            // reserved for future use
            break;

        case SDL_VIDEORESIZE:
        case SDL_VIDEOEXPOSE:
            break;

        case SDL_EVENT_RESERVED2:
        case SDL_EVENT_RESERVED3:
        case SDL_EVENT_RESERVED4:
        case SDL_EVENT_RESERVED5:
        case SDL_EVENT_RESERVED6:
        case SDL_EVENT_RESERVED7:
            // reserved for future use
            break;
#endif

        default:
            // user events
            break;
        }

        if (keep_event) return 1;

        if (!remove)
        {
#if SDL_VERSION_ATLEAST(2,0,0)
            num_events = SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
#else
            num_events = SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_ALLEVENTS);
#endif
            if (num_events <= 0) return 0; // error or no events
        }
    };
}

static void translate_key(lpmsg lpMsg, int released, int vkey, int scancode, int repeat)
{
    lpMsg->message = (released)?WM_KEYUP:WM_KEYDOWN;
    lpMsg->wParam = vkey;
    lpMsg->lParam = scancode << 16;
    if (released)
    {
        lpMsg->lParam |= 1 | (1 << 30) | (1 << 31);
        // missing bits (unused by Septerra Core):
        // bit 24 - Indicates whether the key is an extended key, such as the right-hand ALT and CTRL keys that appear on an enhanced 101- or 102-key keyboard. The value is 1 if it is an extended key; otherwise, it is 0.
    }
    else
    {
        // missing bits (unused by Septerra Core):
        // bits 0-15 - The repeat count for the current message.
        // bit 24 - Indicates whether the key is an extended key, such as the right-hand ALT and CTRL keys that appear on an enhanced 101- or 102-key keyboard. The value is 1 if it is an extended key; otherwise, it is 0.
        // bit 30 - The previous key state.
        if (repeat)
        {
            lpMsg->lParam |= 1 | (1 << 30);
        }
    }
}

static void translate_mouse_button(lpmsg lpMsg, int pressed, int mouse_button, int doubleclick)
{
    switch (mouse_button)
    {
    case 0:
        lpMsg->message = (pressed)?(doubleclick?WM_LBUTTONDBLCLK:WM_LBUTTONDOWN):WM_LBUTTONUP;
        break;
    case 1:
        lpMsg->message = (pressed)?(doubleclick?WM_RBUTTONDBLCLK:WM_RBUTTONDOWN):WM_RBUTTONUP;
        break;
    case 2:
        lpMsg->message = (pressed)?(doubleclick?WM_MBUTTONDBLCLK:WM_MBUTTONDOWN):WM_MBUTTONUP;
        break;
    }

    if (mouse_buttons & SDL_BUTTON_LMASK) lpMsg->wParam |= MK_LBUTTON;
    if (mouse_buttons & SDL_BUTTON_RMASK) lpMsg->wParam |= MK_RBUTTON;
    if (mouse_buttons & SDL_BUTTON_MMASK) lpMsg->wParam |= MK_MBUTTON;
    if (mouse_buttons & SDL_BUTTON_X1MASK) lpMsg->wParam |= MK_XBUTTON1;
    if (mouse_buttons & SDL_BUTTON_X2MASK) lpMsg->wParam |= MK_XBUTTON2;
    if (keyboard_mods & KMOD_SHIFT) lpMsg->wParam |= MK_SHIFT;
    if (keyboard_mods & KMOD_CTRL) lpMsg->wParam |= MK_CONTROL;

    lpMsg->lParam = (mouse_x & 0xffff) | ((mouse_y & 0xffff) << 16);
}

static void translate_event(lpmsg lpMsg, SDL_Event *event, int remove)
{
    int doubleclick, mouse_button, vkey, scancode, index, released;

    const static uint8_t vkey_table[128] = {
           0,    0,    0,    0,    0,    0,    0,    0, 0x08, 0x09,    0,    0, 0x0c, 0x0d,    0,    0, /*   0- 15 */
           0,    0,    0, 0x13,    0,    0,    0,    0,    0,    0,    0, 0x1b,    0,    0,    0,    0, /*  16- 31 */
        0x20, 0x31, 0xde, 0x33, 0x34, 0x35, 0x37, 0xde, 0x39, 0x30, 0x38, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, /*  32- 47 */
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xba, 0xba, 0xbc, 0xbb, 0xbe, 0xbf, /*  48- 63 */
        0x32, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /*  64- 79 */
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0xdb, 0xdc, 0xdd, 0x36, 0xbd, /*  80- 95 */
        0xc0, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /*  96-111 */
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0xdb, 0xdc, 0xdd, 0xc0, 0x2e, /* 112-127 */
    };

    const static uint8_t scancode_table[128] = {
           0,    0,    0,    0,    0,    0,    0,    0, 0x0e, 0x0f,    0,    0,    0, 0x1c,    0,    0, /*   0- 15 */
           0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 0x01,    0,    0,    0,    0, /*  16- 31 */
        0x39, 0x02, 0x28, 0x04, 0x05, 0x06, 0x08, 0x28, 0x0a, 0x0b, 0x09, 0x0d, 0x33, 0x0c, 0x34, 0x35, /*  32- 47 */
        0x0b, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x27, 0x27, 0x33, 0x0d, 0x34, 0x35, /*  48- 63 */
        0x03, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /*  64- 79 */
        0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x07, 0x0c, /*  80- 95 */
        0x29, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26, 0x32, 0x31, 0x18, /*  96-111 */
        0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c, 0x1a, 0x2b, 0x1b, 0x29, 0x53, /* 112-127 */
    };

    const static uint8_t vk_shift_table[128] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*   0- 15 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  16- 31 */
        0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, /*  32- 47 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, /*  48- 63 */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  64- 79 */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, /*  80- 95 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  96-111 */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, /* 112-127 */
    };

    memset((void *)lpMsg, 0, sizeof(msg));

    switch (event->type)
    {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            // key events
#if !SDL_VERSION_ATLEAST(2,0,0)
            /*if ((event->key.keysym.unicode > 0) && (event->key.keysym.unicode < 128))
            {
                vkey = vkey_table[event->key.keysym.unicode];
                scancode = scancode_table[event->key.keysym.unicode];
            }
            else*/
#endif
            if (event->key.keysym.sym < 128)
            {
                vkey = vkey_table[event->key.keysym.sym];
                scancode = scancode_table[event->key.keysym.sym];

                if (Keys_SwitchWSAD)
                {
                    switch((int) event->key.keysym.sym)
                    {
                    case SDLK_a:
                        vkey = VK_LEFT;
                        scancode = 0x4b;
                        break;
                    case SDLK_d:
                        vkey = VK_RIGHT;
                        scancode = 0x4d;
                        break;
                    case SDLK_s:
                        vkey = VK_DOWN;
                        scancode = 0x50;
                        break;
                    case SDLK_w:
                        vkey = VK_UP;
                        scancode = 0x48;
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                vkey = 0;
                scancode = 0;

                switch((int) event->key.keysym.sym)
                {
                case SDLK_KP_PERIOD:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_DECIMAL:VK_DELETE;
                    scancode = 0x53;
                    break;

                case SDLK_KP_DIVIDE:
                    vkey = VK_DIVIDE;
                    scancode = 0x35;
                    break;

                case SDLK_KP_MULTIPLY:
                    vkey = VK_MULTIPLY;
                    scancode = 0x37;
                    break;

                case SDLK_KP_MINUS:
                    vkey = VK_SUBTRACT;
                    scancode = 0x4a;
                    break;

                case SDLK_KP_PLUS:
                    vkey = VK_ADD;
                    scancode = 0x4e;
                    break;

                case SDLK_KP_ENTER:
                    vkey = VK_RETURN;
                    scancode = 0x1c;
                    break;

                case SDLK_KP_EQUALS:
                    vkey = 0xbd;
                    scancode = 0x0d;
                    break;


                case SDLK_UP:
                    if (Keys_SwitchArrowKeys)
                    {
                        vkey = vkey_table['W'];
                        scancode = scancode_table['W'];
                    }
                    else
                    {
                        vkey = VK_UP;
                        scancode = 0x48;
                    }
                    break;

                case SDLK_DOWN:
                    if (Keys_SwitchArrowKeys)
                    {
                        vkey = vkey_table['S'];
                        scancode = scancode_table['S'];
                    }
                    else
                    {
                        vkey = VK_DOWN;
                        scancode = 0x50;
                    }
                    break;

                case SDLK_RIGHT:
                    if (Keys_SwitchArrowKeys)
                    {
                        vkey = vkey_table['D'];
                        scancode = scancode_table['D'];
                    }
                    else
                    {
                        vkey = VK_RIGHT;
                        scancode = 0x4d;
                    }
                    break;

                case SDLK_LEFT:
                    if (Keys_SwitchArrowKeys)
                    {
                        vkey = vkey_table['A'];
                        scancode = scancode_table['A'];
                    }
                    else
                    {
                        vkey = VK_LEFT;
                        scancode = 0x4b;
                    }
                    break;

                case SDLK_INSERT:
                    vkey = VK_INSERT;
                    scancode = 0x52;
                    break;

                case SDLK_HOME:
                    vkey = VK_HOME;
                    scancode = 0x47;
                    break;

                case SDLK_END:
                    vkey = VK_END;
                    scancode = 0x4f;
                    break;

                case SDLK_PAGEUP:
                    vkey = VK_PRIOR;
                    scancode = 0x49;
                    break;

                case SDLK_PAGEDOWN:
                    vkey = VK_NEXT;
                    scancode = 0x51;
                    break;


                case SDLK_F1:
                    vkey = VK_F1;
                    scancode = 0x3b;
                    break;

                case SDLK_F2:
                    vkey = VK_F2;
                    scancode = 0x3c;
                    break;

                case SDLK_F3:
                    vkey = VK_F3;
                    scancode = 0x3d;
                    break;

                case SDLK_F4:
                    vkey = VK_F4;
                    scancode = 0x3e;
                    break;

                case SDLK_F5:
                    vkey = VK_F5;
                    scancode = 0x3f;
                    break;

                case SDLK_F6:
                    vkey = VK_F6;
                    scancode = 0x40;
                    break;

                case SDLK_F7:
                    vkey = VK_F7;
                    scancode = 0x41;
                    break;

                case SDLK_F8:
                    vkey = VK_F8;
                    scancode = 0x42;
                    break;

                case SDLK_F9:
                    vkey = VK_F9;
                    scancode = 0x43;
                    break;

                case SDLK_F10:
                    vkey = VK_F10;
                    scancode = 0x44;
                    break;

                case SDLK_F11:
                    vkey = VK_F11;
                    scancode = 0x57;
                    break;

                case SDLK_F12:
                    vkey = VK_F12;
                    scancode = 0x58;
                    break;

                case SDLK_F13:
                    vkey = VK_F13;
                    break;

                case SDLK_F14:
                    vkey = VK_F14;
                    break;

                case SDLK_F15:
                    vkey = VK_F15;
                    break;


                case SDLK_CAPSLOCK:
                    vkey = VK_CAPITAL;
                    scancode = 0x3a;
                    break;

                case SDLK_RSHIFT:
                    vkey = VK_SHIFT;
                    scancode = 0x36;
                    break;

                case SDLK_LSHIFT:
                    vkey = VK_SHIFT;
                    scancode = 0x2a;
                    break;

                case SDLK_RCTRL:
                    vkey = VK_CONTROL;
                    scancode = 0x1d;
                    break;

                case SDLK_LCTRL:
                    vkey = VK_CONTROL;
                    scancode = 0x1d;
                    break;

                case SDLK_RALT:
                    vkey = VK_MENU;
                    scancode = 0x38;
                    break;

                case SDLK_LALT:
                    vkey = VK_MENU;
                    scancode = 0x38;
                    break;

                case SDLK_MODE:
                    vkey = 0xa5;
                    scancode = 0x5d;
                    break;

                case SDLK_MENU:
                    vkey = VK_APPS;
                    scancode = 0x38;
                    break;


#if SDL_VERSION_ATLEAST(2,0,0)
                case SDLK_PAUSE:
                    vkey = 0x13;
                    break;

                case SDLK_DELETE:
                    vkey = VK_DELETE;
                    scancode = 0x53;
                    break;


                case SDLK_KP_1:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD1:VK_END;
                    scancode = 0x4f;
                    break;

                case SDLK_KP_2:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD2:VK_DOWN;
                    scancode = 0x50;
                    break;

                case SDLK_KP_3:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD3:VK_NEXT;
                    scancode = 0x51;
                    break;

                case SDLK_KP_4:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD4:VK_LEFT;
                    scancode = 0x4b;
                    break;

                case SDLK_KP_5:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD5:VK_CLEAR;
                    scancode = 0x4c;
                    break;

                case SDLK_KP_6:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD6:VK_RIGHT;
                    scancode = 0x4d;
                    break;

                case SDLK_KP_7:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD7:VK_HOME;
                    scancode = 0x47;
                    break;

                case SDLK_KP_8:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD8:VK_UP;
                    scancode = 0x48;
                    break;

                case SDLK_KP_9:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD9:VK_PRIOR;
                    scancode = 0x49;
                    break;

                case SDLK_KP_0:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD0:VK_INSERT;
                    scancode = 0x52;
                    break;


                case SDLK_NUMLOCKCLEAR:
                    vkey = VK_NUMLOCK;
                    scancode = 0x45;
                    break;

                case SDLK_SCROLLLOCK:
                    vkey = VK_SCROLL;
                    scancode = 0x46;
                    break;

                case SDLK_LGUI:
                    vkey = VK_LWIN;
                    scancode = 0x5b;
                    break;

                case SDLK_RGUI:
                    vkey = VK_RWIN;
                    scancode = 0x5c;
                    break;


                case SDLK_F16:
                    vkey = VK_F16;
                    break;

                case SDLK_F17:
                    vkey = VK_F17;
                    break;

                case SDLK_F18:
                    vkey = VK_F18;
                    break;

                case SDLK_F19:
                    vkey = VK_F19;
                    break;

                case SDLK_F20:
                    vkey = VK_F20;
                    break;

                case SDLK_F21:
                    vkey = VK_F21;
                    break;

                case SDLK_F22:
                    vkey = VK_F22;
                    break;

                case SDLK_F23:
                    vkey = VK_F23;
                    break;

                case SDLK_F24:
                    vkey = VK_F24;
                    break;


                case SDLK_RETURN2:
                    vkey = VK_RETURN;
                    scancode = 0x1c;
                    break;

#else
                case SDLK_KP0:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD0:VK_INSERT;
                    scancode = 0x52;
                    break;

                case SDLK_KP1:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD1:VK_END;
                    scancode = 0x4f;
                    break;

                case SDLK_KP2:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD2:VK_DOWN;
                    scancode = 0x50;
                    break;

                case SDLK_KP3:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD3:VK_NEXT;
                    scancode = 0x51;
                    break;

                case SDLK_KP4:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD4:VK_LEFT;
                    scancode = 0x4b;
                    break;

                case SDLK_KP5:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD5:VK_CLEAR;
                    scancode = 0x4c;
                    break;

                case SDLK_KP6:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD6:VK_RIGHT;
                    scancode = 0x4d;
                    break;

                case SDLK_KP7:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD7:VK_HOME;
                    scancode = 0x47;
                    break;

                case SDLK_KP8:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD8:VK_UP;
                    scancode = 0x48;
                    break;

                case SDLK_KP9:
                    vkey = (event->key.keysym.mod & KMOD_NUM)?VK_NUMPAD9:VK_PRIOR;
                    scancode = 0x49;
                    break;


                case SDLK_NUMLOCK:
                    vkey = VK_NUMLOCK;
                    scancode = 0x45;
                    break;

                case SDLK_SCROLLOCK:
                    vkey = VK_SCROLL;
                    scancode = 0x46;
                    break;

                case SDLK_LSUPER:
                    vkey = VK_LWIN;
                    scancode = 0x5b;
                    break;

                case SDLK_RSUPER:
                    vkey = VK_RWIN;
                    scancode = 0x5c;
                    break;

#endif
                }
            }

            translate_key(lpMsg, (event->type == SDL_KEYUP)?1:0, vkey, scancode,
#if SDL_VERSION_ATLEAST(2,0,0)
                event->key.repeat
#else
                0
#endif
            );

            break;

    case SDL_MOUSEMOTION:
        // mouse motion events
        lpMsg->message = WM_MOUSEMOVE;

        if (mouse_buttons & SDL_BUTTON_LMASK) lpMsg->wParam |= MK_LBUTTON;
        if (mouse_buttons & SDL_BUTTON_RMASK) lpMsg->wParam |= MK_RBUTTON;
        if (mouse_buttons & SDL_BUTTON_MMASK) lpMsg->wParam |= MK_MBUTTON;
        if (mouse_buttons & SDL_BUTTON_X1MASK) lpMsg->wParam |= MK_XBUTTON1;
        if (mouse_buttons & SDL_BUTTON_X2MASK) lpMsg->wParam |= MK_XBUTTON2;
        if (keyboard_mods & KMOD_SHIFT) lpMsg->wParam |= MK_SHIFT;
        if (keyboard_mods & KMOD_CTRL) lpMsg->wParam |= MK_CONTROL;

        lpMsg->lParam = (mouse_x & 0xffff) | ((mouse_y & 0xffff) << 16);
        break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        // mouse button events

        doubleclick = 0;
#if SDL_VERSION_ATLEAST(2,0,2)
        if (sdl_versionnum >= SDL_VERSIONNUM(2,0,2))
        {
            if (event->button.clicks > 1)
            {
                doubleclick = 1;
            }
        }
        else
#endif
        {
            if (mouse_doubleclick)
            {
                doubleclick = 1;
            }
        }

        switch (event->button.button)
        {
        case SDL_BUTTON_LEFT:
            mouse_button = 0;
            break;
        case SDL_BUTTON_MIDDLE:
            mouse_button = 2;
            break;
        case SDL_BUTTON_RIGHT:
            mouse_button = 1;
            break;
        default:
            mouse_button = -1;
            break;
        }

        translate_mouse_button(lpMsg, (event->button.state == SDL_PRESSED)?1:0, mouse_button, doubleclick);
        break;

    case SDL_JOYHATMOTION:
        // joystick hat motion event
        released = 0;
        for (index = 0; index < 4; index++)
        {
            // search for released key
            if ((event->jhat.value & (0x10 << index)) && !(event->jhat.value & (1 << index)))
            {
                released = 1;
                event->jhat.value &= ~(0x10 << index);
                break;
            }
        }
        if (!released)
        {
            released = 1;
            for (index = 0; index < 4; index++)
            {
                // search for pressed key
                if ((event->jhat.value & (1 << index)) && !(event->jhat.value & (0x10 << index)))
                {
                    released = 0;
                    event->jhat.value |= (0x10 << index);
                    break;
                }
            }
        }

        switch (index)
        {
        case 0:
            vkey = VK_UP;
            scancode = 0x48;
            break;
        case 1:
            vkey = VK_RIGHT;
            scancode = 0x4d;
            break;
        case 2:
            vkey = VK_DOWN;
            scancode = 0x50;
            break;
        case 3:
            vkey = VK_LEFT;
            scancode = 0x4b;
            break;
        default:
            vkey = 0;
            scancode = 0;
            break;
        }

        translate_key(lpMsg, released, vkey, scancode, 0);

        if (remove && ((event->jhat.value & 0x0f) != (event->jhat.value >> 4)))
        {
            priority_event = *event;
        }
        break;

    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        // joystick button event
        mouse_button = -1;
        vkey = 0;
        scancode = 0;

        switch (event->jbutton.button)
        {
        case 0:
        case 1:
            mouse_button = event->jbutton.button;
            break;
        case 3:
            vkey = VK_TAB;
            scancode = 0x0f;
            break;
        default:
            break;
        }

        if (vkey)
        {
            translate_key(lpMsg, (event->type == SDL_JOYBUTTONUP)?1:0, vkey, scancode, 0);
        }
        else
        {
            translate_mouse_button(lpMsg, (event->type == SDL_JOYBUTTONDOWN)?1:0, mouse_button, mouse_doubleclick);
        }
        break;

    case SDL_QUIT:
        //lpMsg->message = WM_QUIT;
        lpMsg->message = WM_SYSCOMMAND;
        lpMsg->wParam = SC_CLOSE;
        break;

    case SDL_USEREVENT:
        switch (event->user.code)
        {
        case 0:
            lpMsg->message = WM_QUIT;
            lpMsg->wParam = (uintptr_t)event->user.data1;
            break;
        case 1:
            lpMsg->message = WM_NULL;
            break;
        case 2:
        case 3:
            lpMsg->message = WM_APP + event->user.code - 2;
            break;
        case 4:
            index = virtual_keyboard_buffer[(uintptr_t)event->user.data1];
            switch ((uintptr_t)event->user.data2)
            {
            case 0:
                translate_key(lpMsg, 1, VK_SHIFT, 0x36, 0); // release right shift
                if (remove)
                {
                    event->user.data2 = (void *)(intptr_t)1;
                }
                break;
            case 1:
                translate_key(lpMsg, (vk_shift_table[index])?0:1, VK_SHIFT, 0x2a, 0); // press/release left shift
                if (remove)
                {
                    event->user.data2 = (void *)(intptr_t)2;
                }
                break;
            case 2:
                translate_key(lpMsg, 0, vkey_table[index], scancode_table[index], 0); // press key
                if (remove)
                {
                    event->user.data2 = (void *)(intptr_t)3;
                }
                break;
            case 3:
                translate_key(lpMsg, 1, vkey_table[index], scancode_table[index], 0); // release key
                if (remove)
                {
                    if (1 + (intptr_t)event->user.data1 < virtual_keyboard_buflen)
                    {
                        event->user.data1 = (void *)(1 + (uintptr_t)event->user.data1);
                        event->user.data2 = (void *)(intptr_t)((vk_shift_table[index] == vk_shift_table[virtual_keyboard_buffer[(uintptr_t)event->user.data1]])?2:1);
                    }
                    else
                    {
                        event->user.data2 = (void *)(intptr_t)((vk_shift_table[index])?4:5);
                    }
                }
                break;
            case 4:
                translate_key(lpMsg, 1, VK_SHIFT, 0x2a, 0); // release left shift
                if (remove)
                {
                    event->user.data2 = (void *)(intptr_t)5;
                }
                break;
            }

            if (remove)
            {
                if ((uintptr_t)event->user.data2 < 5)
                {
                    priority_event = *event;
                }
            }
            break;
        }
        break;

#if SDL_VERSION_ATLEAST(2,0,0)
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
        // game controller button event
        mouse_button = -1;
        vkey = 0;
        scancode = 0;

        switch (event->cbutton.button)
        {
        case SDL_CONTROLLER_BUTTON_A:
        case SDL_CONTROLLER_BUTTON_X:
            mouse_button = 0;
            break;

        case SDL_CONTROLLER_BUTTON_B:
        case SDL_CONTROLLER_BUTTON_Y:
            mouse_button = 1;
            break;

        case SDL_CONTROLLER_BUTTON_BACK:
            vkey = VK_TAB;
            scancode = 0x0f;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            vkey = VK_UP;
            scancode = 0x48;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            vkey = VK_DOWN;
            scancode = 0x50;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            vkey = VK_LEFT;
            scancode = 0x4b;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            vkey = VK_RIGHT;
            scancode = 0x4d;
            break;

        default:
            break;
        }

        if (vkey)
        {
            translate_key(lpMsg, (event->type == SDL_CONTROLLERBUTTONUP)?1:0, vkey, scancode, 0);
        }
        else
        {
            translate_mouse_button(lpMsg, (event->type == SDL_CONTROLLERBUTTONDOWN)?1:0, mouse_button, mouse_doubleclick);
        }

        break;

    case SDL_WINDOWEVENT:
        // window state change event
        switch (event->window.event)
        {
        case SDL_WINDOWEVENT_MINIMIZED:
            lpMsg->message = WM_SYSCOMMAND;
            lpMsg->wParam = SC_MINIMIZE;
            break;
        case SDL_WINDOWEVENT_RESTORED:
            lpMsg->message = WM_SYSCOMMAND;
            lpMsg->wParam = SC_RESTORE;
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            lpMsg->message = WM_SETFOCUS;
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            lpMsg->message = WM_KILLFOCUS;
            break;
        default:
            break;
        }
        break;
#else
    case SDL_ACTIVEEVENT:
        // Application loses/gains visibility
        switch (event->active.state)
        {
        case SDL_APPINPUTFOCUS:
            lpMsg->message = (event->active.gain)?WM_SETFOCUS:WM_KILLFOCUS;
            break;
        case SDL_APPACTIVE:
            lpMsg->message = WM_SYSCOMMAND;
            lpMsg->wParam = (event->active.gain)?SC_RESTORE:SC_MINIMIZE;
        default:
            break;
        }
        break;
#endif

    default:
        break;
    }
}


uint32_t AdjustWindowRectEx_c(void *lpRect, uint32_t dwStyle, uint32_t bMenu, uint32_t dwExStyle)
{
    eprintf("Unimplemented: %s\n", "AdjustWindowRectEx");
    exit(1);
//    return AdjustWindowRectEx((LPRECT)lpRect, dwStyle, bMenu, dwExStyle);
}

uint32_t ClipCursor_c(const void *lpRect)
{
    return 1;
//    return ClipCursor((const RECT *)lpRect);
}

void *CreateWindowExA_c(uint32_t dwExStyle, const char *lpClassName, const char *lpWindowName, uint32_t dwStyle, int32_t x, int32_t y, int32_t nWidth, int32_t nHeight, void *hWndParent, void *hMenu, void *hInstance, void *lpParam)
{
#ifdef DEBUG_USER32
    eprintf("CreateWindowExA: 0x%x, %s, %s, 0x%x, %i, %i, %i, %i\n", dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight);
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_version linked;

    SDL_GetVersion(&linked);
    sdl_versionnum = SDL_VERSIONNUM(linked.major, linked.minor, linked.patch);
#else
    const SDL_version *linked;

    linked = SDL_Linked_Version();
    sdl_versionnum = SDL_VERSIONNUM(linked->major, linked->minor, linked->patch);
#endif

    priority_event.type = 0;
    controller_mouse_last_time = SoundEngine_Counter;

    if (Input_GameController && (joystick == NULL)
#if SDL_VERSION_ATLEAST(2,0,0)
        && (controller == NULL)
#endif
    )
    {
        SDL_InitSubSystem(SDL_INIT_JOYSTICK
#if SDL_VERSION_ATLEAST(2,0,0)
            | SDL_INIT_GAMECONTROLLER
#endif
        );

#if SDL_VERSION_ATLEAST(2,0,2)
        if (sdl_versionnum >= SDL_VERSIONNUM(2,0,2))
        {
            SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
        }
#endif

        open_controller_or_joystick(-1);

#if !SDL_VERSION_ATLEAST(2,0,0)
        if (joystick == NULL)
        {
            SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
        }
#endif
    }

    return (void *)1;
}

uint32_t DefWindowProcA_c(void *hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam)
{
#ifdef DEBUG_USER32
    eprintf("DefWindowProcA: 0x%" PRIxPTR ", 0x%x, 0x%x, 0x%x\n", (uintptr_t)hWnd, Msg, wParam, lParam);
#endif

    return 0;
}

uint32_t DestroyWindow_c(void *hWnd)
{
    eprintf("Unimplemented: %s\n", "DestroyWindow");
    exit(1);
//    return DestroyWindow((HWND)hWnd);
}

uint32_t DispatchMessageA_c(void *lpMsg)
{
#ifdef DEBUG_USER32
    eprintf("DispatchMessageA - ");
#endif

    if (lpMsg == NULL)
    {
#ifdef DEBUG_USER32
        eprintf("error\n");
#endif
        return 0;
    }

#ifdef DEBUG_USER32
    eprintf("0x%x, 0x%x, 0x%x\n", ((lpmsg)lpMsg)->message, ((lpmsg)lpMsg)->wParam, ((lpmsg)lpMsg)->lParam);
#endif

    if ((((lpmsg)lpMsg)->message == WM_TIMER) && (((lpmsg)lpMsg)->lParam != 0))
    {
        eprintf("Unimplemented: %s\n", "DispatchMessageA");
        exit(1);
    }
    else if (lpfnWndProc == NULL)
    {
        if (((lpmsg)lpMsg)->message == WM_QUIT)
        {
            exit(((lpmsg)lpMsg)->wParam);
        }

        return 0;
    }
    else
    {
        return RunWndProc_asm(((lpmsg)lpMsg)->hwnd, ((lpmsg)lpMsg)->message, ((lpmsg)lpMsg)->wParam, ((lpmsg)lpMsg)->lParam, (uint32_t (*)(void *, uint32_t, uint32_t, uint32_t))lpfnWndProc);
    }
}

int16_t GetAsyncKeyState_c(int32_t vKey)
{
#ifdef DEBUG_USER32
    eprintf("GetAsyncKeyState: %i\n", vKey);
#endif

    if (vKey == VK_SHIFT)
    {
        return (keyboard_mods & KMOD_SHIFT)?0x8000:0;
    }

    eprintf("Unimplemented: %s\n", "GetAsyncKeyState");
    exit(1);
}

uint32_t GetCursorPos_c(void *lpPoint)
{
    int x, y;

#ifdef DEBUG_USER32
    eprintf("GetCursorPos: 0x%" PRIxPTR " - ", (uintptr_t)lpPoint);
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
    // SDL_GetMouseState returns real window coordinates instead of renderer logical coordinates
    // also last event coordinates are probably more appropriate for Septerra Core
    x = mouse_x;
    y = mouse_y;
#else
    SDL_GetMouseState(&x, &y);
#endif

    ((lppoint)lpPoint)->x = x;
    ((lppoint)lpPoint)->y = y;

#ifdef DEBUG_USER32
    eprintf("OK: %i x %i", x, y);
#endif

    return 1;
}

int16_t GetKeyState_c(int32_t nVirtKey)
{
#ifdef DEBUG_USER32
    eprintf("GetKeyState: %i\n", nVirtKey);
#endif

    if (nVirtKey == VK_CAPITAL)
    {
        return (keyboard_mods & KMOD_CAPS)?1:0;
    }

    eprintf("Unimplemented: %s\n", "GetKeyState");
    exit(1);
}

uint32_t GetMessageA_c(void *lpMsg, void *hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax)
{
#ifdef DEBUG_USER32
    eprintf("GetMessageA: 0x%" PRIxPTR ", %i, %i\n", (intptr_t)hWnd, wMsgFilterMin, wMsgFilterMax);
#endif

    if (lpMsg == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return -1;
    }

    if ((hWnd == NULL) && (wMsgFilterMin == 0) && (wMsgFilterMax == 0))
    {
        SDL_Event event;
        if (!find_event(&event, 1, 1))
        {
            Winapi_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return -1;
        }

        translate_event((lpmsg)lpMsg, &event, 1);

#ifdef _WIN32
        message_time = GetTickCount();
#else
        message_time = Winapi_GetTicks();
#endif

        return (((lpmsg)lpMsg)->message == WM_QUIT)?0:1;
    }

    eprintf("Unsupported method: %s\n", "GetMessageA");
    exit(1);
}

uint32_t GetMessagePos_c(void)
{
#ifdef DEBUG_USER32
    eprintf("GetMessagePos\n");
#endif

    return (mouse_x & 0xffff) | ((mouse_y & 0xffff) << 16);
}

uint32_t GetMessageTime_c(void)
{
#ifdef DEBUG_USER32
    eprintf("GetMessageTime\n");
#endif

    return message_time;
}

int32_t GetSystemMetrics_c(int32_t nIndex)
{
#ifdef DEBUG_USER32
    eprintf("GetSystemMetrics: %i\n", nIndex);
#endif

    switch (nIndex)
    {
        case SM_CXSCREEN:
            return 640;
        case SM_CYSCREEN:
            return 480;
        case SM_SWAPBUTTON:
            return 0;
        default:
            exit(1);
    }
}

void *LoadCursorA_c(void *hInstance, const char *lpCursorName)
{
#ifdef DEBUG_USER32
    if ( (((uintptr_t)lpCursorName) & ~(uintptr_t)0xffff) == 0 )
    {
        eprintf("LoadCursorA: 0x%" PRIxPTR ", %i\n", (uintptr_t) hInstance, (int32_t)(intptr_t) lpCursorName);
    }
    else
    {
        eprintf("LoadCursorA: 0x%" PRIxPTR ", %s\n", (uintptr_t) hInstance, lpCursorName);
    }
#endif

    if ((hInstance == NULL) && ((((uintptr_t)lpCursorName) & ~(uintptr_t)0xffff) == 0))
    {
        uint32_t predefined_cursor;
        SDL_Cursor* cursor;

        predefined_cursor = (uintptr_t)lpCursorName;

        if (predefined_cursor == 32514) // IDC_WAIT
        {
        // Septerra Core loads and sets cursor only once and doesn't free it, so SDL cursor can be used directly
#if SDL_VERSION_ATLEAST(2,0,0)
            cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
#else
            const static Uint8 data[2 * 23] = {
                0x7f, 0xfc,     /* 0111 1111 1111 1100 */   /*  .............   */
                0x80, 0x02,     /* 1000 0000 0000 0010 */   /* .XXXXXXXXXXXXX.  */
                0xbf, 0xfa,     /* 1011 1111 1111 1010 */   /* .X...........X.  */
                0xb5, 0x5a,     /* 1011 0101 0101 1010 */   /* .X..X.X.X.X..X.  */
                0xaa, 0xaa,     /* 1010 1010 1010 1010 */   /* .X.X.X.X.X.X.X.  */
                0xb5, 0x5a,     /* 1011 0101 0101 1010 */   /* .X..X.X.X.X..X.  */
                0x9a, 0xb2,     /* 1001 1010 1011 0010 */   /* .XX..X.X.X..XX.  */
                0x4d, 0x64,     /* 0100 1101 0110 0100 */   /*  .XX..X.X..XX.   */
                0x26, 0xc8,     /* 0010 0110 1100 1000 */   /*   .XX..X..XX.    */
                0x13, 0x90,     /* 0001 0011 1001 0000 */   /*    .XX...XX.     */
                0x0b, 0xa0,     /* 0000 1011 1010 0000 */   /*     .X...X.      */
                0x0a, 0xa0,     /* 0000 1010 1010 0000 */   /*     .X.X.X.      */
                0x0b, 0xa0,     /* 0000 1011 1010 0000 */   /*     .X...X.      */
                0x13, 0x90,     /* 0001 0011 1001 0000 */   /*    .XX...XX.     */
                0x26, 0xc8,     /* 0010 0110 1100 1000 */   /*   .XX..X..XX.    */
                0x4f, 0x64,     /* 0100 1111 0110 0100 */   /*  .XX....X..XX.   */
                0x9f, 0xf2,     /* 1001 1111 1111 0010 */   /* .XX.........XX.  */
                0xba, 0xba,     /* 1011 1010 1011 1010 */   /* .X...X.X.X...X.  */
                0xb5, 0x5a,     /* 1011 0101 0101 1010 */   /* .X..X.X.X.X..X.  */
                0xaa, 0xaa,     /* 1010 1010 1010 1010 */   /* .X.X.X.X.X.X.X.  */
                0x95, 0x52,     /* 1001 0101 0101 0010 */   /* .XX.X.X.X.X.XX.  */
                0x80, 0x02,     /* 1000 0000 0000 0010 */   /* .XXXXXXXXXXXXX.  */
                0x7f, 0xfc,     /* 0111 1111 1111 1100 */   /*  .............   */
            };
            const static Uint8 mask[2 * 23] = {
                0x7f, 0xfc,     /* 0111 1111 1111 1100 */   /*  .............   */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .XXXXXXXXXXXXX.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .X...........X.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .X..X.X.X.X..X.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .X.X.X.X.X.X.X.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .X..X.X.X.X..X.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .XX..X.X.X..XX.  */
                0x7f, 0xfc,     /* 0111 1111 1111 1100 */   /*  .XX..X.X..XX.   */
                0x3f, 0xf8,     /* 0011 1111 1111 1000 */   /*   .XX..X..XX.    */
                0x1f, 0xf0,     /* 0001 1111 1111 0000 */   /*    .XX...XX.     */
                0x0f, 0xe0,     /* 0000 1111 1110 0000 */   /*     .X...X.      */
                0x0f, 0xe0,     /* 0000 1111 1110 0000 */   /*     .X.X.X.      */
                0x0f, 0xe0,     /* 0000 1111 1110 0000 */   /*     .X...X.      */
                0x1f, 0xf0,     /* 0001 1111 1111 0000 */   /*    .XX...XX.     */
                0x3f, 0xf8,     /* 0011 1111 1111 1000 */   /*   .XX..X..XX.    */
                0x7f, 0xfc,     /* 0111 1111 1111 1100 */   /*  .XX....X..XX.   */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .XX.........XX.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .X...X.X.X...X.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .X..X.X.X.X..X.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .X.X.X.X.X.X.X.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .XX.X.X.X.X.XX.  */
                0xff, 0xfe,     /* 1111 1111 1111 1110 */   /* .XXXXXXXXXXXXX.  */
                0x7f, 0xfc,     /* 0111 1111 1111 1100 */   /*  .............   */
            };

            cursor = SDL_CreateCursor((Uint8 *) data, (Uint8 *) mask, 16, 23, 7, 11);
#endif
            if (cursor != NULL) return cursor;
        }
    }

    Winapi_SetLastError(ERROR_INVALID_PARAMETER);
    return NULL;
}

void *LoadIconA_c(void *hInstance, const char *lpIconName)
{
#ifdef DEBUG_USER32
    if ( (((uintptr_t)lpIconName) & ~(uintptr_t)0xffff) == 0 )
    {
        eprintf("LoadIconA: 0x%" PRIxPTR ", %i\n", (uintptr_t) hInstance, (int32_t)(intptr_t) lpIconName);
    }
    else
    {
        eprintf("LoadIconA: 0x%" PRIxPTR ", %s\n", (uintptr_t) hInstance, lpIconName);
    }
#endif

    // Septerra Core only uses icon for RegisterClassA
    Winapi_SetLastError(ERROR_INVALID_PARAMETER);
    return NULL;
}

void *LoadImageA_c(void *hinst, const char *lpszName, uint32_t uType, int32_t cxDesired, int32_t cyDesired, uint32_t fuLoad)
{
#ifndef _WIN32
    char buf[8192];
#endif

#ifdef DEBUG_USER32
    eprintf("LoadImageA: 0x%" PRIxPTR ", %s, %i, %i, %i, %i\n", (uintptr_t) hinst, lpszName, uType, cxDesired, cyDesired, fuLoad);
#endif

    if ((uType == IMAGE_BITMAP) && (fuLoad == LR_LOADFROMFILE))
    {
        if (lpszName == NULL)
        {
            Winapi_SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }


#ifdef _WIN32
        if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(lpszName))
#else
        if (!CLIB_FindFile(lpszName, buf))
#endif
        {
            Winapi_SetLastError(ERROR_FILE_NOT_FOUND);
            return NULL;
        }
    }

    eprintf("Unsupported method: %s\n", "LoadImageA");
    exit(1);
}

uint32_t MessageBoxA_c(void *hWnd, const char *lpText, const char *lpCaption, uint32_t uType)
{
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_MessageBoxData data;
    SDL_MessageBoxButtonData buttons[3];

    data.window = NULL;
    data.title = lpCaption;
    data.message = lpText;
    data.buttons = buttons;
    data.colorScheme = NULL;

    switch (uType & MB_TYPEMASK)
    {
        case MB_OK:
            data.numbuttons = 1;
            buttons[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[0].buttonid = IDOK;
            buttons[0].text = "OK";
            break;
        case MB_OKCANCEL:
            data.numbuttons = 2;
            buttons[0].flags = 0;
            buttons[0].buttonid = IDOK;
            buttons[0].text = "OK";
            buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].buttonid = IDCANCEL;
            buttons[1].text = "Cancel";
            break;
        case MB_ABORTRETRYIGNORE:
            data.numbuttons = 3;
            buttons[0].flags = 0;
            buttons[0].buttonid = IDABORT;
            buttons[0].text = "Abort";
            buttons[1].flags = 0;
            buttons[1].buttonid = IDRETRY;
            buttons[1].text = "Retry";
            buttons[2].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].buttonid = IDIGNORE;
            buttons[2].text = "Ignore";
            break;
        case MB_YESNOCANCEL:
            data.numbuttons = 3;
            buttons[0].flags = 0;
            buttons[0].buttonid = IDYES;
            buttons[0].text = "Yes";
            buttons[1].flags = 0;
            buttons[1].buttonid = IDNO;
            buttons[1].text = "No";
            buttons[2].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].buttonid = IDCANCEL;
            buttons[2].text = "Cancel";
            break;
        case MB_YESNO:
            data.numbuttons = 2;
            buttons[0].flags = 0;
            buttons[0].buttonid = IDYES;
            buttons[0].text = "Yes";
            buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].buttonid = IDNO;
            buttons[1].text = "No";
            break;
        case MB_RETRYCANCEL:
            data.numbuttons = 2;
            buttons[0].flags = 0;
            buttons[0].buttonid = IDRETRY;
            buttons[0].text = "Retry";
            buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[1].buttonid = IDCANCEL;
            buttons[1].text = "Cancel";
            break;
        case MB_CANCELTRYCONTINUE:
            data.numbuttons = 3;
            buttons[0].flags = 0;
            buttons[0].buttonid = IDCANCEL;
            buttons[0].text = "Cancel";
            buttons[1].flags = 0;
            buttons[1].buttonid = IDTRYAGAIN;
            buttons[1].text = "Try Again";
            buttons[2].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            buttons[2].buttonid = IDCONTINUE;
            buttons[2].text = "Continue";
            break;
        default:
            data.numbuttons = 0;
            break;
    }

    switch (uType & MB_ICONMASK)
    {
        case MB_ICONHAND:
            data.flags = SDL_MESSAGEBOX_ERROR;
            break;
        case MB_ICONEXCLAMATION:
            data.flags = SDL_MESSAGEBOX_WARNING;
            break;
        case MB_ICONASTERISK:
            data.flags = SDL_MESSAGEBOX_INFORMATION;
            break;
        default:
            data.flags = 0;
            break;
    }

    switch (uType & MB_DEFMASK)
    {
        case MB_DEFBUTTON2:
            buttons[1].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            break;
        case MB_DEFBUTTON3:
            buttons[2].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            break;
        default:
            buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            break;
    }

    if ((hWnd == NULL) && ((uType & MB_MODEMASK) == MB_SYSTEMMODAL) && ((uType & ~(MB_TYPEMASK | MB_ICONMASK | MB_DEFMASK | MB_MODEMASK)) == 0))
    {
        if (data.numbuttons == 1)
        {
            if (0 == SDL_ShowSimpleMessageBox(data.flags, data.title, data.message, data.window))
            {
                return IDOK;
            }
        }
        else if (data.numbuttons > 1)
        {
            int buttonid;

            if (0 == SDL_ShowMessageBox(&data, &buttonid))
            {
                return buttonid;
            }
        }
    }
#endif
    eprintf("MessageBoxA: 0x%x\n\tCaption: %s\n\tText: %s\n", uType, lpCaption, lpText);

    if ((uType & MB_TYPEMASK) == 0)
    {
        return IDOK;
    }

    eprintf("Unsupported method: %s\n", "MessageBoxA");
    exit(1);
}

uint32_t OffsetRect_c(void *lprc, int32_t dx, int32_t dy)
{
#ifdef DEBUG_USER32
    eprintf("OffsetRect: 0x%" PRIxPTR ", %i, %i\n", (intptr_t)lprc, dx, dy);
#endif

    if (lprc == NULL) return 0;

    ((lprect)lprc)->left += dx;
    ((lprect)lprc)->right += dx;
    ((lprect)lprc)->top += dy;
    ((lprect)lprc)->bottom += dy;

    return 1;
}

uint32_t PeekMessageA_c(void *lpMsg, void *hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax, uint32_t wRemoveMsg)
{
#ifdef DEBUG_USER32
    eprintf("PeekMessageA: 0x%" PRIxPTR ", %i, %i, %i\n", (uintptr_t)hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
#endif

    if ((hWnd == NULL) && (wMsgFilterMin == 0) && (wMsgFilterMax == 0) && ((wRemoveMsg == 0) || (wRemoveMsg == 1)))
    {
        SDL_Event event;

        if (!find_event(&event, wRemoveMsg, 0)) return 0;

        if (lpMsg != NULL)
        {
            translate_event((lpmsg)lpMsg, &event, wRemoveMsg);
        }

        return 1;
    }

    eprintf("Unsupported method: %s\n", "PeekMessageA");
    exit(1);
}

uint32_t PostMessageA_c(void *hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam)
{
#ifdef DEBUG_USER32
    eprintf("PostMessageA: 0x%" PRIxPTR ", %i, %i, %i\n", (uintptr_t)hWnd, Msg, wParam, lParam);
#endif

    if (((uintptr_t)hWnd == 1) && (Msg >= WM_USER) && (Msg < WM_APP))
    {
        // Septerra Core posts these messages, but I have no idea what they are for (they're not used by the application)

        Winapi_SetLastError(ERROR_ACCESS_DENIED);
        return 0;
    }

    eprintf("Unsupported method: %s\n", "PostMessageA");
    exit(1);
}

void PostQuitMessage_c(int32_t nExitCode)
{
    SDL_Event event;
#ifdef DEBUG_USER32
    eprintf("PostQuitMessage: %i\n", nExitCode);
#endif

    event.type = SDL_USEREVENT;
    event.user.code = 0;
    event.user.data1 = (void *)(intptr_t)nExitCode;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
}

uint32_t PtInRect_c(const void *lprc, int32_t px, int32_t py)
{
    if (lprc == NULL) return 0;

#define RC ((const int32_t *)lprc)
    if ((px >= RC[0]) && (px < RC[2]) && (py >= RC[1]) && (py < RC[3])) return 1;

    return 0;
#undef RC
}

uint32_t RegisterClassA_c(void *lpWndClass)
{
#ifdef DEBUG_USER32
    eprintf("RegisterClassA\n");
#endif

    if (lpWndClass != NULL)
    {
        lpfnWndProc = ((wndclassa *)lpWndClass)->lpfnWndProc;
    }

    return 1;
}

void *SetCursor_c(void *hCursor)
{
    SDL_Cursor *old_cursor;
#ifdef DEBUG_USER32
    eprintf("SetCursor: 0x%" PRIxPTR "\n", (uintptr_t) hCursor);
#endif

    old_cursor = SDL_GetCursor();
    if (old_cursor != hCursor)
    {
        // Septerra Core loads and sets cursor only once and doesn't free it, so SDL cursor can be used directly
        SDL_SetCursor((SDL_Cursor *)hCursor);
    }

    return old_cursor;
}

uint32_t SetCursorPos_c(int32_t X, int32_t Y)
{
#ifdef DEBUG_USER32
    eprintf("SetCursorPos: %i, %i\n", X, Y);
#endif

#if SDL_VERSION_ATLEAST(2,0,0)
    warp_mouse(X, Y);
#else
    SDL_WarpMouse(X, Y);
#endif
    return 1;
}

void *SetFocus_c(void *hWnd)
{
#ifdef DEBUG_USER32
    eprintf("SetFocus: 0x%" PRIxPTR "\n", (uintptr_t) hWnd);
#endif

    return hWnd;
}

int32_t ShowCursor_c(uint32_t bShow)
{
#ifdef DEBUG_USER32
    eprintf("ShowCursor: %i\n", bShow);
#endif

    if (bShow)
    {
        cursor_visibility++;
        if (cursor_visibility == 0)
        {
            SDL_ShowCursor(SDL_ENABLE);
        }
    }
    else
    {
        cursor_visibility--;
        if (cursor_visibility == -1)
        {
            SDL_ShowCursor(SDL_DISABLE);
        }
    }

    return cursor_visibility;
}

uint32_t ShowWindow_c(void *hWnd, int32_t nCmdShow)
{
#ifdef DEBUG_USER32
    eprintf("ShowWindow: 0x%" PRIxPTR ", %i\n", (uintptr_t)hWnd, nCmdShow);
#endif

    return 0;
}

uint32_t TranslateMessage_c(void *pMsg)
{
#ifdef DEBUG_USER32
    eprintf("TranslateMessage\n");
#endif

    // Septerra Core doesn't process WM_CHAR, WM_DEADCHAR, WM_SYSCHAR or WM_SYSDEADCHAR messages
    return 0;
}

uint32_t UpdateWindow_c(void *hWnd)
{
#ifdef DEBUG_USER32
    eprintf("UpdateWindow: 0x%" PRIxPTR "\n", (uintptr_t)hWnd);
#endif

    return 1;
}

uint32_t ValidateRect_c(void *hWnd, const void *lpRect)
{
    eprintf("Unimplemented: %s\n", "ValidateRect");
    exit(1);
//    return ValidateRect((HWND)hWnd, (const RECT *)lpRect);
}

uint32_t WaitMessage_c(void)
{
#ifdef DEBUG_USER32
    eprintf("WaitMessage\n");
#endif

    if (SDL_WaitEvent(NULL))
    {
        return 1;
    }
    else
    {
        Winapi_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
}

