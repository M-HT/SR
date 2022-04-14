/**
 *
 *  Copyright (C) 2016-2022 Roman Pauer
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

#include <string.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
#else
    #include <SDL/SDL.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Intro-proc-events.h"
#include "main.h"

void Game_ProcessKEvents()
{
    int finish;
    uint32_t VSyncTick, key_code, ascii_code, scancode;
    SDL_Event *cevent;

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

    const static uint8_t bios_scancode_table[128] = {
           0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /*   0- 15 */
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /*  16- 31 */
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /*  32- 47 */
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /*  48- 63 */
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /*  64- 79 */
        0x50, 0x51, 0x52, 0x53,    0,    0,    0, 0x85, 0x86,    0,    0, 0xec, 0xed, 0xee,    0,    0, /*  80- 95 */
           0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, /*  96-111 */
           0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, /* 112-127 */
    };

    VSyncTick = Game_VSyncTick;
    finish = 0;

    while (!finish && Game_KQueueWrite != Game_KQueueRead)
    {
        cevent = &(Game_EventKQueue[Game_KQueueRead]);

        switch(cevent->type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            #if !defined(USE_SDL2)
                if ((cevent->key.keysym.unicode > 0) && (cevent->key.keysym.unicode < 128))
                {
                    scancode = scancode_table[cevent->key.keysym.unicode];
                    ascii_code = cevent->key.keysym.unicode;
                }
                else
            #endif
                if (cevent->key.keysym.sym < 128)
                {
                    scancode = scancode_table[cevent->key.keysym.sym];

                    if (((cevent->key.keysym.mod & KMOD_SHIFT) == 0) != ((cevent->key.keysym.mod & KMOD_CAPS) == 0))
                    {
                        if (cevent->key.keysym.sym >= SDLK_a && cevent->key.keysym.sym <= SDLK_z)
                        {
                            ascii_code = cevent->key.keysym.sym - 32;
                        }
                        else
                        {
                            ascii_code = cevent->key.keysym.sym;
                        }
                    }
                    else
                    {
                        ascii_code = cevent->key.keysym.sym;
                    }
                }
                else
                {
                    ascii_code = 0;
                    scancode = 0;

                    switch((int) cevent->key.keysym.sym)
                    {
                    #ifdef USE_SDL2
                        case SDLK_KP_0:
                    #else
                        case SDLK_KP0:
                    #endif
                            scancode = 0x52;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '0';

                            break;
                    #ifdef USE_SDL2
                        case SDLK_KP_1:
                    #else
                        case SDLK_KP1:
                    #endif
                            scancode = 0x4f;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '1';

                            break;
                    #ifdef USE_SDL2
                        case SDLK_KP_2:
                    #else
                        case SDLK_KP2:
                    #endif
                            scancode = 0x50;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '2';

                            break;
                    #ifdef USE_SDL2
                        case SDLK_KP_3:
                    #else
                        case SDLK_KP3:
                    #endif
                            scancode = 0x51;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '3';

                            break;
                    #ifdef USE_SDL2
                        case SDLK_KP_4:
                    #else
                        case SDLK_KP4:
                    #endif
                            scancode = 0x4b;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '4';

                            break;
                    #ifdef USE_SDL2
                        case SDLK_KP_5:
                    #else
                        case SDLK_KP5:
                    #endif
                            scancode = 0x4c;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '5';

                            break;
                    #ifdef USE_SDL2
                        case SDLK_KP_6:
                    #else
                        case SDLK_KP6:
                    #endif
                            scancode = 0x4d;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '6';

                            break;
                    #ifdef USE_SDL2
                        case SDLK_KP_7:
                    #else
                        case SDLK_KP7:
                    #endif
                            scancode = 0x47;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '7';

                            break;
                    #ifdef USE_SDL2
                        case SDLK_KP_8:
                    #else
                        case SDLK_KP8:
                    #endif
                            scancode = 0x48;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '8';

                            break;
                    #ifdef USE_SDL2
                        case SDLK_KP_9:
                    #else
                        case SDLK_KP9:
                    #endif
                            scancode = 0x49;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '9';

                            break;
                        case SDLK_KP_PERIOD:
                            scancode = 0x53;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '.';

                            break;
                        case SDLK_KP_DIVIDE:
                            scancode = 0x35;
                            ascii_code = '/';

                        case SDLK_KP_MULTIPLY:
                            scancode = 0x37;
                            ascii_code = '*';

                            break;
                        case SDLK_KP_MINUS:
                            scancode = 0x4a;
                            ascii_code = '-';

                            break;
                        case SDLK_KP_PLUS:
                            scancode = 0x4e;
                            ascii_code = '+';

                            break;
                        case SDLK_KP_ENTER:
                            scancode = 0x1c;
                            ascii_code = 0x1c;

                            break;
                        case SDLK_KP_EQUALS:
                            scancode = 0x0d;
                            ascii_code = '=';

                            break;
                        case SDLK_UP:
                            scancode = 0x48;

                            break;
                        case SDLK_DOWN:
                            scancode = 0x50;

                            break;
                        case SDLK_RIGHT:
                            scancode = 0x4d;

                            break;
                        case SDLK_LEFT:
                            scancode = 0x4b;

                            break;
                        case SDLK_INSERT:
                            scancode = 0x52;

                            break;
                        case SDLK_HOME:
                            scancode = 0x47;

                            break;
                        case SDLK_END:
                            scancode = 0x4f;

                            break;
                        case SDLK_PAGEUP:
                            scancode = 0x49;

                            break;
                        case SDLK_PAGEDOWN:
                            scancode = 0x51;

                            break;
                        case SDLK_F1:
                            scancode = 0x3b;

                            break;
                        case SDLK_F2:
                            scancode = 0x3c;

                            break;
                        case SDLK_F3:
                            scancode = 0x3d;

                            break;
                        case SDLK_F4:
                            scancode = 0x3e;

                            break;
                        case SDLK_F5:
                            scancode = 0x3f;

                            break;
                        case SDLK_F6:
                            scancode = 0x40;

                            break;
                        case SDLK_F7:
                            scancode = 0x41;

                            break;
                        case SDLK_F8:
                            scancode = 0x42;

                            break;
                        case SDLK_F9:
                            scancode = 0x43;

                            break;
                        case SDLK_F10:
                            scancode = 0x44;

                            break;
                        case SDLK_F11:
                            scancode = 0x57;

                            break;
                        case SDLK_F12:
                            scancode = 0x58;

                            break;
                    #ifdef USE_SDL2
                        case SDLK_NUMLOCKCLEAR:
                    #else
                        case SDLK_NUMLOCK:
                    #endif
                            scancode = 0x45;

                            break;
                        case SDLK_CAPSLOCK:
                            scancode = 0x3a;

                            break;
                    #ifdef USE_SDL2
                        case SDLK_SCROLLLOCK:
                    #else
                        case SDLK_SCROLLOCK:
                    #endif
                            scancode = 0x46;

                            break;
                        case SDLK_RSHIFT:
                            scancode = 0x36;

                            break;
                        case SDLK_LSHIFT:
                            scancode = 0x2a;

                            break;
                        case SDLK_RCTRL:
                            scancode = 0x1d;

                            break;
                        case SDLK_LCTRL:
                            scancode = 0x1d;

                            break;
                        case SDLK_RALT:
                            scancode = 0x38;

                            break;
                        case SDLK_LALT:
                            scancode = 0x38;

                            break;
                    #ifdef USE_SDL2
                        case SDLK_LGUI:
                    #else
                        case SDLK_LSUPER:
                    #endif
                            scancode = 0x5b;

                            break;
                    #ifdef USE_SDL2
                        case SDLK_RGUI:
                    #else
                        case SDLK_RSUPER:
                    #endif
                            scancode = 0x5c;

                            break;
                        case SDLK_MODE:
                            scancode = 0x5d;

                            break;
                        case SDLK_MENU:
                            scancode = 0x38;

                            break;

                        default:
                            goto _after_switch1;
                    }
                }
                if (cevent->key.state == SDL_RELEASED)
                {
                    scancode |= 0x80;
                }

                if (cevent->key.state == SDL_PRESSED)
                {
                    key_code = (scancode << 24) | (bios_scancode_table[scancode & 0x7f] << 8) | ascii_code;

                    if ( ( (Game_KBufferWrite + 1) & (GAME_KBUFFER_LENGTH - 1) ) == Game_KBufferRead )
                    {
#if defined(__DEBUG__)
                        fprintf(stderr, "keyboard buffer overflow\n");
#endif
                    }
                    else
                    {
                        Game_KBuffer[Game_KBufferWrite] = key_code;

                        Game_KBufferWrite = (Game_KBufferWrite + 1) & (GAME_KBUFFER_LENGTH - 1);
                    }


                }

                break;
                // case SDL_KEYDOWN, SDL_KEYUP:
        } // switch(event.type)

    _after_switch1:
        Game_KQueueRead = (Game_KQueueRead + 1) & (GAME_KQUEUE_LENGTH - 1);
    //_after_switch2:
        if (VSyncTick != Game_VSyncTick) finish = 1;
    }

}

// convert device coordinates to picture coordinates
static int32_t Game_Device2PictureX(int32_t devicex)
{
    if (devicex < Picture_Position_UL_X) devicex = Picture_Position_UL_X;
    else if (devicex > Picture_Position_BR_X) devicex = Picture_Position_BR_X;

    return devicex - Picture_Position_UL_X;
}

static int32_t Game_Device2PictureY(int32_t devicey)
{
    if (devicey < Picture_Position_UL_Y) devicey = Picture_Position_UL_Y;
    else if (devicey > Picture_Position_BR_Y) devicey = Picture_Position_BR_Y;

    return devicey - Picture_Position_UL_Y;
}

// convert picture coordinates to device coordinates
static int32_t Game_Picture2DeviceX(int32_t picturex)
{
    picturex += Picture_Position_UL_X;

    if (picturex < 0) picturex = 0;
    else if (picturex >= (int32_t) Display_Width) picturex = Display_Width - 1;

    return picturex;
}

static int32_t Game_Picture2DeviceY(int32_t picturey)
{
    picturey += Picture_Position_UL_Y;

    if (picturey < 0) picturey = 0;
    else if (picturey >= (int32_t) Display_Height) picturey = Display_Height - 1;

    return picturey;
}

void Game_GetGameMouse(int *mousex, int *mousey)
{
    int mx, my;

    SDL_GetMouseState(&mx, &my);
    *mousex = (Game_Device2PictureX(mx) * Game_VideoAspectX + 32767) >> 16;
    *mousey = (Game_Device2PictureY(my) * Game_VideoAspectY + 32767) >> 16;
}

void Game_RepositionMouse(int mousex, int mousey)
{
    SDL_Event event;

    event.type = SDL_USEREVENT;
    event.user.code = EC_MOUSE_SET;
    event.user.data1 = (void *)(intptr_t) Game_Picture2DeviceX((mousex * Game_VideoAspectXR + 32767) >> 16);
    event.user.data2 = (void *)(intptr_t) Game_Picture2DeviceY((mousey * Game_VideoAspectYR + 32767) >> 16);

    SDL_PushEvent(&event);
}

