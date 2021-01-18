/**
 *
 *  Copyright (C) 2019-2021 Roman Pauer
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

#include "WinApi-user32.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

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
    void *hwnd;
    uint32_t message;
    uint32_t wParam;
    int32_t  lParam;
    uint32_t time;
    point    pt;
} msg, *lpmsg;

typedef struct {
    uint32_t style;
    void *lpfnWndProc;
    int32_t cbClsExtra;
    int32_t cbWndExtra;
    void *hInstance;
    void *hIcon;
    void *hCursor;
    void *hbrBackground;
    const char *lpszMenuName;
    const char *lpszClassName;
} wndclassa;


static void *lpfnWndProc = NULL;
static int cursor_visibility = 0;
static unsigned int mouse_buttons = 0;
static unsigned int keyboard_mods = 0;
static int mouse_x = 0;
static int mouse_y = 0;
static unsigned int message_time = 0;
static int mouse_right_button_mod = 0;
#if !SDL_VERSION_ATLEAST(2,0,2)
static const int mouse_doubleclick_max_distance = 10;
static const int mouse_doubleclick_max_time = 500;

static int mouse_doubleclick = 0;
static uint32_t mouse_last_time[3], mouse_current_time[3];
static int mouse_last_x[3], mouse_last_y[3], mouse_current_x[3], mouse_current_y[3];
static int mouse_last_peep[3];
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t RunWndProc_asm(void *hwnd, uint32_t uMsg, uint32_t wParam, uint32_t lParam, uint32_t (*WndProc)(void *, uint32_t, uint32_t, uint32_t));
#ifdef __cplusplus
}
#endif


static int find_event(SDL_Event *event, int remove, int wait)
{
    SDL_PumpEvents();

    while (1)
    {
        int num_events, keep_event;
#if !SDL_VERSION_ATLEAST(2,0,2)
        int mouse_button;
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

            return 0;
        }


        keep_event = 0;
        switch (event->type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            // key events
            keep_event = 1;
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
            keep_event = 1;
            mouse_buttons = event->motion.state;
            if (mouse_right_button_mod && (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)))
            {
                mouse_buttons = (mouse_buttons & ~(SDL_BUTTON(SDL_BUTTON_LEFT))) | SDL_BUTTON(SDL_BUTTON_RIGHT);
            }
            mouse_x = event->motion.x;
            mouse_y = event->motion.y;
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
#if SDL_VERSION_ATLEAST(2,0,2)
            if ((event->button.button == SDL_BUTTON_LEFT) ||
                (event->button.button == SDL_BUTTON_MIDDLE) ||
                (event->button.button == SDL_BUTTON_RIGHT)
               )
            {
                keep_event = 1;
            }
#else
            mouse_doubleclick = 0;

            if (event->button.button == SDL_BUTTON_LEFT) mouse_button = 0;
            else if (event->button.button == SDL_BUTTON_RIGHT) mouse_button = 1;
            else if (event->button.button == SDL_BUTTON_MIDDLE) mouse_button = 2;
            else mouse_button = -1;
            if (mouse_button >= 0)
            {
                keep_event = 1;
            }
#endif
            if (event->button.state == SDL_PRESSED)
            {
                mouse_buttons |= SDL_BUTTON(event->button.button);
#if !SDL_VERSION_ATLEAST(2,0,2)
                if (mouse_button >= 0)
                {
                    if (!mouse_last_peep[mouse_button])
                    {
#ifdef _WIN32
                        mouse_current_time[mouse_button] = GetTickCount();
#else
                        mouse_current_time[mouse_button] = Winapi_GetTicks();
#endif
                        mouse_current_x[mouse_button] = event->button.x;
                        mouse_current_y[mouse_button] = event->button.y;
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
#endif
            }
            else
            {
                mouse_buttons &= ~SDL_BUTTON(event->button.button);
            }
            mouse_x = event->button.x;
            mouse_y = event->button.y;
            break;

        case SDL_JOYAXISMOTION:
            // joystick axis motion event
            break;

        case SDL_JOYBALLMOTION:
            // joystick trackball motion event
            break;

        case SDL_JOYHATMOTION:
            // joystick hat motion event
            break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            // joystick button event
            break;

        case SDL_QUIT:
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
            break;

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            // game controller button event
            break;

        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
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
        case SDL_JOYDEVICEREMOVED:
            // joystick device event
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

static void translate_event(lpmsg lpMsg, SDL_Event *event)
{
    int doubleclick, vkey, scancode;

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

    memset(lpMsg, 0, sizeof(msg));

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
                    vkey = VK_UP;
                    scancode = 0x48;
                    break;

                case SDLK_DOWN:
                    vkey = VK_DOWN;
                    scancode = 0x50;
                    break;

                case SDLK_RIGHT:
                    vkey = VK_RIGHT;
                    scancode = 0x4d;
                    break;

                case SDLK_LEFT:
                    vkey = VK_LEFT;
                    scancode = 0x4b;
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

            lpMsg->message = (event->type == SDL_KEYUP)?WM_KEYUP:WM_KEYDOWN;
            lpMsg->wParam = vkey;
            lpMsg->lParam = scancode << 16;
            if (event->type == SDL_KEYUP)
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
#if SDL_VERSION_ATLEAST(2,0,0)
                if (event->key.repeat)
                {
                    lpMsg->lParam |= 1 | (1 << 30);
                }
#endif
            }

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
        if (event->button.clicks > 1)
        {
            doubleclick = 1;
        }
#else
        if (mouse_doubleclick)
        {
            doubleclick = 1;
        }
#endif

        switch (event->button.button)
        {
        case SDL_BUTTON_LEFT:
            lpMsg->message = (event->button.state == SDL_PRESSED)?(doubleclick?WM_LBUTTONDBLCLK:WM_LBUTTONDOWN):WM_LBUTTONUP;
            break;
        case SDL_BUTTON_MIDDLE:
            lpMsg->message = (event->button.state == SDL_PRESSED)?(doubleclick?WM_MBUTTONDBLCLK:WM_MBUTTONDOWN):WM_MBUTTONUP;
            break;
        case SDL_BUTTON_RIGHT:
            lpMsg->message = (event->button.state == SDL_PRESSED)?(doubleclick?WM_RBUTTONDBLCLK:WM_RBUTTONDOWN):WM_RBUTTONUP;
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
        break;

    case SDL_QUIT:
        //lpMsg->message = WM_QUIT;
        lpMsg->message = WM_SYSCOMMAND;
        lpMsg->wParam = SC_CLOSE;
        break;

    case SDL_USEREVENT:
        if (event->user.code == 0)
        {
            lpMsg->message = WM_QUIT;
            lpMsg->wParam = (uintptr_t)event->user.data1;
        }
        else if (event->user.code == 1)
        {
            lpMsg->message = WM_NULL;
        }
        else
        {
            lpMsg->message = WM_APP + event->user.code - 2;
        }
        break;

#if SDL_VERSION_ATLEAST(2,0,0)
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

    return (void *)1;
}

uint32_t DefWindowProcA_c(void *hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam)
{
#ifdef DEBUG_USER32
    eprintf("DefWindowProcA: 0x%x, 0x%x, 0x%x, 0x%x\n", (uintptr_t)hWnd, Msg, wParam, lParam);
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
    eprintf("GetCursorPos: 0x%x - ", (uintptr_t)lpPoint);
#endif

    SDL_GetMouseState(&x, &y);

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
    eprintf("GetMessageA: %i, %i, %i\n", (intptr_t)hWnd, wMsgFilterMin, wMsgFilterMax);
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

        translate_event((lpmsg)lpMsg, &event);

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
    if ( (((uint32_t)lpCursorName) & 0xffff0000) == 0 )
    {
        eprintf("LoadCursorA: %i, %i\n", (int) hInstance, (int32_t) lpCursorName);
    }
    else
    {
        eprintf("LoadCursorA: %i, %s\n", (int) hInstance, lpCursorName);
    }
#endif

    if ((hInstance == NULL) && ((((uint32_t)lpCursorName) & 0xffff0000) == 0))
    {
        uint32_t predefined_cursor;
        SDL_Cursor* cursor;

        predefined_cursor = (uint32_t)lpCursorName;

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
    if ( (((uint32_t)lpIconName) & 0xffff0000) == 0 )
    {
        eprintf("LoadIconA: %i, %i\n", (int) hInstance, (int32_t) lpIconName);
    }
    else
    {
        eprintf("LoadIconA: %i, %s\n", (int) hInstance, lpIconName);
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
    eprintf("LoadImageA: %i, %s, %i, %i, %i, %i\n", (int) hinst, lpszName, uType, cxDesired, cyDesired, fuLoad);
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
    eprintf("OffsetRect: 0x%x, %i, %i\n", (intptr_t)lprc, dx, dy);
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
    eprintf("PeekMessageA: %i, %i, %i, %i\n", (intptr_t)hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
#endif

    if ((hWnd == NULL) && (wMsgFilterMin == 0) && (wMsgFilterMax == 0) && ((wRemoveMsg == 0) || (wRemoveMsg == 1)))
    {
        SDL_Event event;

        // Send Septerra Core to sleep to prevent it from consuming too much cpu
#ifdef _WIN32
        Sleep(1);
#else
        struct timespec _tp;

        _tp.tv_sec = 0;
        _tp.tv_nsec = 1000000;

        nanosleep(&_tp, NULL);
#endif

        if (!find_event(&event, wRemoveMsg, 0)) return 0;

        if (lpMsg != NULL)
        {
            translate_event((lpmsg)lpMsg, &event);
        }

        return 1;
    }

    eprintf("Unsupported method: %s\n", "PeekMessageA");
    exit(1);
}

uint32_t PostMessageA_c(void *hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam)
{
#ifdef DEBUG_USER32
    eprintf("PostMessageA: 0x%x, %i, %i, %i\n", (uintptr_t)hWnd, Msg, wParam, lParam);
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
    event.user.data1 = (void *)nExitCode;
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
    eprintf("SetCursor: 0x%x\n", (uintptr_t) hCursor);
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
    SDL_WarpMouseInWindow(NULL, X, Y);
#else
    SDL_WarpMouse(X, Y);
#endif
    return 1;
}

void *SetFocus_c(void *hWnd)
{
#ifdef DEBUG_USER32
    eprintf("SetFocus: %i\n", (int) hWnd);
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
    eprintf("ShowWindow: %i, %i\n", (int)hWnd, nCmdShow);
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
    eprintf("UpdateWindow: %i\n", (int)hWnd);
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

