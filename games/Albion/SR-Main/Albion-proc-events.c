/* encoding = UTF-8 */

/**
 *
 *  Copyright (C) 2016-2024 Roman Pauer
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
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-proc-events.h"
#include "input.h"


void Game_ProcessKEvents(void)
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

    const static uint8_t ascii_shift_table[128] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /*   0- 15 */
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /*  16- 31 */
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x22, 0x28, 0x29, 0x2a, 0x2b, 0x3c, 0x5f, 0x3e, 0x3f, /*  32- 47 */
        0x29, 0x21, 0x40, 0x23, 0x24, 0x25, 0x5e, 0x26, 0x2a, 0x28, 0x3a, 0x3a, 0x3c, 0x2b, 0x3e, 0x3f, /*  48- 63 */
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /*  64- 79 */
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x5e, 0x5f, /*  80- 95 */
        0x7e, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /*  96-111 */
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* 112-127 */
    };

    static int alt_code_state = 0;
    static unsigned int alt_code_value;

    VSyncTick = Game_VSyncTick;
    finish = 0;

    while (!finish && Game_KQueueWrite != Game_KQueueRead)
    {
        cevent = &(Game_EventKQueue[Game_KQueueRead]);

        switch(cevent->type)
        {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (cevent->key.keysym.sym == SDLK_PAUSE)
                {
                    if (alt_code_state >= 1) alt_code_state = -1;
                    goto _after_switch1;
                }
            #if !SDL_VERSION_ATLEAST(2,0,0)
                else if ((cevent->key.keysym.unicode > 0) && (cevent->key.keysym.unicode < 128))
                {
                    scancode = scancode_table[cevent->key.keysym.unicode];
                    ascii_code = cevent->key.keysym.unicode;
                }
                else if (cevent->key.keysym.unicode != 0)
                {
                    scancode = 0;
                    ascii_code = 0;

                    if ((ascii_code == 0) && (Albion_Font_Lang != AL_UNKNOWN))
                    {
                        switch (cevent->key.keysym.unicode)
                        {
                            case 0x00E4: // ä
                                ascii_code = 0x84;
                                break;
                            case 0x00C4: // Ä
                                ascii_code = 0x8e;
                                break;
                            case 0x00F6: // ö
                                ascii_code = 0x94;
                                break;
                            case 0x00D6: // Ö
                                ascii_code = 0x99;
                                break;
                            case 0x00FC: // ü
                                ascii_code = 0x81;
                                break;
                            case 0x00DC: // Ü
                                ascii_code = 0x9a;
                                break;
                            case 0x00DF: // ß
                                ascii_code = 0xe1;
                                break;
                            default:
                                break;
                        }
                    }

                    if ((ascii_code == 0) && (Albion_Font_Lang == AL_ENG_FRE))
                    {
                        switch (cevent->key.keysym.unicode)
                        {
                            case 0x00E9: // é
                                ascii_code = 0x82;
                                break;
                            case 0x00E2: // â
                                ascii_code = 0x83;
                                break;
                            case 0x00E0: // à
                                ascii_code = 0x85;
                                break;
                            case 0x00E7: // ç
                                ascii_code = 0x87;
                                break;
                            case 0x00EA: // ê
                                ascii_code = 0x88;
                                break;
                            case 0x00EB: // ë
                                ascii_code = 0x89;
                                break;
                            case 0x00E8: // è
                                ascii_code = 0x8a;
                                break;
                            case 0x00EF: // ï
                                ascii_code = 0x8b;
                                break;
                            case 0x00EE: // î
                                ascii_code = 0x8c;
                                break;
                            case 0x00EC: // ì
                                ascii_code = 0x8d;
                                break;
                            case 0x00F4: // ô
                                ascii_code = 0x93;
                                break;
                            case 0x00F2: // ò
                                ascii_code = 0x95;
                                break;
                            case 0x00FB: // û
                                ascii_code = 0x96;
                                break;
                            case 0x00F9: // ù
                                ascii_code = 0x97;
                                break;
                            case 0x00E1: // á
                                ascii_code = 0xa0;
                                break;
                            case 0x00ED: // í
                                ascii_code = 0xa1;
                                break;
                            case 0x00F3: // ó
                                ascii_code = 0xa2;
                                break;
                            case 0x00FA: // ú
                                ascii_code = 0xa3;
                                break;
                            default:
                                break;
                        }
                    }

                    if ((ascii_code == 0) && (Albion_Font_Lang == AL_CZE))
                    {
                        switch (cevent->key.keysym.unicode)
                        {
                            case 0x00E9: // é
                                ascii_code = 0x82;
                                break;
                            case 0x00E1: // á
                                ascii_code = 0x83;
                                break;
                            case 0x010F: // ď
                                ascii_code = 0x85;
                                break;
                            case 0x010D: // č
                                ascii_code = 0x87;
                                break;
                            case 0x011B: // ě
                                ascii_code = 0x88;
                                break;
                            case 0x0148: // ň
                                ascii_code = 0x89;
                                break;
                            case 0x0159: // ř
                                ascii_code = 0x8a;
                                break;
                            case 0x0161: // š
                                ascii_code = 0x8b;
                                break;
                            case 0x0165: // ť
                                ascii_code = 0x8c;
                                break;
                            case 0x017E: // ž
                                ascii_code = 0x8d;
                                break;
                            case 0x00F3: // ó
                                ascii_code = 0x93;
                                break;
                            case 0x00FA: // ú
                                ascii_code = 0x95;
                                break;
                            case 0x016F: // ů
                                ascii_code = 0x96;
                                break;
                            case 0x00FD: // ý
                                ascii_code = 0x97;
                                break;
                            case 0x00ED: // í
                                ascii_code = 0xa0;
                                break;
                            default:
                                break;
                        }
                    }
                }
            #endif
                else
                {
                    ascii_code = 0;
                }

                if ((ascii_code == 0) && (cevent->key.keysym.sym < 128))
                {
                    ascii_code = cevent->key.keysym.sym;
                    scancode = scancode_table[ascii_code];

                    if (((cevent->key.keysym.mod & KMOD_SHIFT) == 0) != ((cevent->key.keysym.mod & KMOD_CAPS) == 0))
                    {
                        if (ascii_code >= SDLK_a && ascii_code <= SDLK_z)
                        {
                            ascii_code -= 32;
                        }
                    }

                #if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)) && !SDL_VERSION_ATLEAST(2,0,0)
                    if (ascii_code == 61 && cevent->type == SDL_KEYDOWN && cevent->key.keysym.unicode == 0)
                    {
                        // handle dead key (´/ˇ), which doesn't act like dead key
                        ascii_code = 0;
                    }
                #endif

                    if (cevent->key.keysym.mod & KMOD_SHIFT)
                    {
                        ascii_code = ascii_shift_table[ascii_code];
                    }
                }

                if (ascii_code != 0)
                {
                    if (ascii_code == '{' || ascii_code == '}')
                    {
                        // don't use these characters in Albion
                        ascii_code = 0;
                    }

                    if (Game_SwitchWSAD)
                    {
                        switch(ascii_code)
                        {
                        case 'A':
                        case 'a':
                            ascii_code = 0;
                            scancode = 0x4b;
                            break;
                        case 'D':
                        case 'd':
                            ascii_code = 0;
                            scancode = 0x4d;
                            break;
                        case 'S':
                        case 's':
                            ascii_code = 0;
                            scancode = 0x50;
                            break;
                        case 'W':
                        case 'w':
                            ascii_code = 0;
                            scancode = 0x48;
                            break;
                        default:
                            break;
                        }
                    }
                }
                else
                {
                    ascii_code = 0;
                    scancode = 0;

                    switch((int) cevent->key.keysym.sym)
                    {
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_0:
                    #else
                        case SDLK_KP0:
                    #endif
                            scancode = 0x52;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '0';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_1:
                    #else
                        case SDLK_KP1:
                    #endif
                            scancode = 0x4f;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '1';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_2:
                    #else
                        case SDLK_KP2:
                    #endif
                            scancode = 0x50;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '2';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_3:
                    #else
                        case SDLK_KP3:
                    #endif
                            scancode = 0x51;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '3';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_4:
                    #else
                        case SDLK_KP4:
                    #endif
                            scancode = 0x4b;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '4';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_5:
                    #else
                        case SDLK_KP5:
                    #endif
                            scancode = 0x4c;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '5';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_6:
                    #else
                        case SDLK_KP6:
                    #endif
                            scancode = 0x4d;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '6';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_7:
                    #else
                        case SDLK_KP7:
                    #endif
                            scancode = 0x47;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '7';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_KP_8:
                    #else
                        case SDLK_KP8:
                    #endif
                            scancode = 0x48;
                            if (cevent->key.keysym.mod & KMOD_NUM) ascii_code = '8';

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
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

                            break;
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
                            if (Game_SwitchArrowKeys)
                            {
                                ascii_code = 'w';
                                scancode = scancode_table[ascii_code];
                                if (((cevent->key.keysym.mod & KMOD_SHIFT) == 0) != ((cevent->key.keysym.mod & KMOD_CAPS) == 0))
                                {
                                    ascii_code -= 32;
                                }
                            }
                            else
                            {
                                scancode = 0x48;
                            }

                            break;
                        case SDLK_DOWN:
                            if (Game_SwitchArrowKeys)
                            {
                                ascii_code = 's';
                                scancode = scancode_table[ascii_code];
                                if (((cevent->key.keysym.mod & KMOD_SHIFT) == 0) != ((cevent->key.keysym.mod & KMOD_CAPS) == 0))
                                {
                                    ascii_code -= 32;
                                }
                            }
                            else
                            {
                                scancode = 0x50;
                            }

                            break;
                        case SDLK_RIGHT:
                            if (Game_SwitchArrowKeys)
                            {
                                ascii_code = 'd';
                                scancode = scancode_table[ascii_code];
                                if (((cevent->key.keysym.mod & KMOD_SHIFT) == 0) != ((cevent->key.keysym.mod & KMOD_CAPS) == 0))
                                {
                                    ascii_code -= 32;
                                }
                            }
                            else
                            {
                                scancode = 0x4d;
                            }

                            break;
                        case SDLK_LEFT:
                            if (Game_SwitchArrowKeys)
                            {
                                ascii_code = 'a';
                                scancode = scancode_table[ascii_code];
                                if (((cevent->key.keysym.mod & KMOD_SHIFT) == 0) != ((cevent->key.keysym.mod & KMOD_CAPS) == 0))
                                {
                                    ascii_code -= 32;
                                }
                            }
                            else
                            {
                                scancode = 0x4b;
                            }

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
                        case SDLK_F15:
                            if (alt_code_state >= 1) alt_code_state = -1;
                            goto _after_switch1;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_NUMLOCKCLEAR:
                    #else
                        case SDLK_NUMLOCK:
                    #endif
                            scancode = 0x45;

                            break;
                        case SDLK_CAPSLOCK:
                            scancode = 0x3a;

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
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
                        case SDLK_LCTRL:
                            scancode = 0x1d;

                            break;
                        case SDLK_RALT:
                        case SDLK_LALT:
                            scancode = 0x38;

                            if (cevent->type == SDL_KEYDOWN)
                            {
                                alt_code_state = 1;
                                alt_code_value = 0;
                            }
                            else
                            {
                                alt_code_state = (alt_code_state == 2)?3:0;
                            }

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        case SDLK_LGUI:
                    #else
                        case SDLK_LSUPER:
                    #endif
                            scancode = 0x5b;

                            break;
                    #if SDL_VERSION_ATLEAST(2,0,0)
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
                            if (alt_code_state >= 1) alt_code_state = -1;
                            goto _after_switch1;
                    }
                }

                if (cevent->type == SDL_KEYDOWN && alt_code_state >= 1)
                {
                    if (ascii_code >= '0' && ascii_code <= '9')
                    {
                        alt_code_state = 2;
                        alt_code_value = alt_code_value * 10 + (ascii_code - '0');
                        ascii_code = 0;
                    }
                    else
                    {
                        if (scancode != 0x38) alt_code_state = -1;
                    }
                }

                if (cevent->key.state == SDL_RELEASED)
                {
                    scancode |= 0x80;
                }

                if (Game_InterruptTable[9] != 0)
                {
                    keyboard_keys[scancode & 0x7f] = (cevent->key.state == SDL_PRESSED)?1:0;
                }

                if (cevent->key.state == SDL_PRESSED || alt_code_state == 3)
                {
                    if (cevent->key.state == SDL_PRESSED)
                    {
                        key_code = (scancode << 24) | (bios_scancode_table[scancode & 0x7f] << 8) | ascii_code;
                    }
                    else
                    {
                        key_code = alt_code_value & 0xff;
                        alt_code_state = 0;
                    }

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

//senquack - new togglable screen scaling requires some changes
static void Game_CompareMPosition(int x, int y, int *XPosDiff, int *YPosDiff)
{
    int32_t virtx, virty;

    virtx = Game_Device2PictureX(x);
    virty = Game_Device2PictureY(y);

    if (Game_VideoAspectX <= 65536)
    {
        /* screen resolution is bigger than game resolution */

        *XPosDiff = ((virtx * Game_VideoAspectX + 32767) >> 16) - mouse_pos[1];
    }
    else
    {
        /* screen resolution is smaller than game resolution */

        *XPosDiff = virtx - ((mouse_pos[1] * Game_VideoAspectXR + 32767) >> 16);
    }

    if (Game_VideoAspectY <= 65536)
    {
        /* screen resolution is bigger than game resolution */

        *YPosDiff = ((virty * Game_VideoAspectY + 32767) >> 16) - mouse_pos[0];
    }
    else
    {
        /* screen resolution is smaller than game resolution */

        *YPosDiff = virty - ((mouse_pos[0] * Game_VideoAspectYR + 32767) >> 16);
    }
}

static void Game_WarpMouse(int x, int y)
{
    SDL_Event event;

    event.type = SDL_USEREVENT;
    event.user.code = EC_MOUSE_SET;
    event.user.data1 = (void *)(intptr_t) x;
    event.user.data2 = (void *)(intptr_t) y;

    SDL_PushEvent(&event);
}

void Game_RepositionMouse(void)
{
    Game_WarpMouse(Game_Picture2DeviceX((mouse_pos[1] * Game_VideoAspectXR + 32767) >> 16), Game_Picture2DeviceY((mouse_pos[0] * Game_VideoAspectYR + 32767) >> 16));
}

int Game_ProcessMEvents(void)
{
    int finish;
    uint32_t VSyncTick;
    SDL_Event *cevent;

    VSyncTick = Game_VSyncTick;
    finish = 0;

    while (!finish && Game_MQueueWrite != Game_MQueueRead)
    {
        cevent = &(Game_EventMQueue[Game_MQueueRead]);

        switch(cevent->type)
        {
            case SDL_MOUSEMOTION:
                //senquack - when in unscaled display mode, handle things a bit differently
                //	since screen coordinates correspond directly to the game's framebuffer
                //	coordinates, only shifted a bit
                {
                    // Original code:
                    int XPosDiff, YPosDiff, newx, newy;
                    uint32_t ret;

                    newx = cevent->motion.x;
                    newy = cevent->motion.y;

                    if (Display_MouseLocked || Display_Fullscreen)
                    {
                        if (cevent->motion.xrel > 0)
                        {
                            if (newx < Picture_Position_UL_X) newx = Picture_Position_UL_X + cevent->motion.xrel;
                        }
                        else if (cevent->motion.xrel < 0)
                        {
                            if (newx >= Picture_Position_BR_X) newx = Picture_Position_BR_X + cevent->motion.xrel - 1;
                        }

                        if (cevent->motion.yrel > 0)
                        {
                            if (newy < Picture_Position_UL_Y) newy = Picture_Position_UL_Y + cevent->motion.yrel;
                        }
                        else if (cevent->motion.yrel < 0)
                        {
                            if (newy >= Picture_Position_BR_Y) newy = Picture_Position_BR_Y + cevent->motion.yrel - 1;
                        }
                    }

                    Game_CompareMPosition(newx, newy, &XPosDiff, &YPosDiff);

                    if (XPosDiff || YPosDiff)
                    {
                        ret = Game_MouseMove(cevent->motion.state, (Game_Device2PictureX(newx) * Game_VideoAspectX + 32767) >> 16, (Game_Device2PictureY(newy) * Game_VideoAspectY + 32767) >> 16);

                        if ((Display_MouseLocked || Display_Fullscreen) && !ret)
                        {
                            Game_CompareMPosition(newx, newy, &XPosDiff, &YPosDiff);

                            if (XPosDiff > 0)
                            {
                                if (cevent->motion.xrel < 0)
                                {
                                    newx = Game_Picture2DeviceX((mouse_pos[1] * Game_VideoAspectXR) >> 16);
                                }
                            }
                            else if (XPosDiff < 0)
                            {
                                if (cevent->motion.xrel > 0)
                                {
                                    newx = Game_Picture2DeviceX(((mouse_pos[1] + 1) * Game_VideoAspectXR) >> 16) - 1;
                                }
                            }

                            if (YPosDiff > 0)
                            {
                                if (cevent->motion.yrel < 0)
                                {
                                    newy = Game_Picture2DeviceY((mouse_pos[0] * Game_VideoAspectYR) >> 16);
                                }
                            }
                            else if (YPosDiff < 0)
                            {
                                if (cevent->motion.yrel > 0)
                                {
                                    newy = Game_Picture2DeviceY(((mouse_pos[0] + 1) * Game_VideoAspectYR) >> 16) - 1;
                                }
                            }
                        }
                    }

                    if ((newx != cevent->motion.x) || (newy != cevent->motion.y))
                    {
                        Game_WarpMouse(newx, newy);
                    }
                }
                break;
                // case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                //senquack - now we keep track of what mouse buttons are currently pressed so
                //           we can support two ways of using the touchscreen.  The first way
                //           lets users touch/tap the screen and that is LMB, and if they
                //           hold the buttom assigned to RMB while touch/tapping the screen,
                //           that is RMB.  The second way lets users only move the cursor
                //           when touching the touchscreen, and only then the buttons for

                if (cevent->button.button == SDL_BUTTON_LEFT ||
                    cevent->button.button == SDL_BUTTON_RIGHT)
                {
                    int XPosDiff, YPosDiff;
                    uint32_t ret;

                    if (Game_TouchscreenButtonEvents)
                    {
                        // We are using GP2X's touchscreen and the game is configured to
                        // register LMB presses when stylus is pressed and only register
                        // RMB presses when the button for RMB is held and the stylus is
                        // then pressed
                        if (cevent->button.state != SDL_PRESSED)
                        {
                            // When one mouse button goes up, make sure both go up
                            ret = Game_MouseButton(0, 2);
                            ret = Game_MouseButton(0, 4);
                        }
                        else
                        {
                            if (Game_RMBActive)
                            {
                                // Button assigned to RMB is held, force RMB down and force LMB up
                                ret = Game_MouseButton(0, 3); // RMB down
                                ret = Game_MouseButton(0, 2); // LMB up
                            }
                            else
                            {
                                // Button assigned to RMB is not held, force LMB down and force RMB up
                                ret = Game_MouseButton(0, 1); // LMB down
                                ret = Game_MouseButton(0, 4); // RMB up
                            }
                        }
                    }
                    else
                    {
                        ret = Game_MouseButton(0 /*SDL_GetMouseState(NULL, NULL)*/, ((cevent->button.state == SDL_PRESSED)?1:2) + ((cevent->button.button == SDL_BUTTON_LEFT)?0:2));
                    }

                    if ((Display_MouseLocked || Display_Fullscreen) && !ret)
                    {
                        Game_CompareMPosition(cevent->button.x, cevent->button.y, &XPosDiff, &YPosDiff);

                        if (XPosDiff || YPosDiff)
                        {
                            Game_RepositionMouse();
                        }
                    }
                //senquack - added support for toggling scaling using mouse wheel:
                }
        #if SDL_VERSION_ATLEAST(2,0,0)
            // handled below
        #else
                else if (cevent->button.button == SDL_BUTTON_WHEELUP)
                {
                    if (cevent->type == SDL_MOUSEBUTTONUP) {
                        Display_ChangeMode = 1;
                    }
                }
                else if (cevent->button.button == SDL_BUTTON_WHEELDOWN)
                {
                    if (cevent->type == SDL_MOUSEBUTTONUP) {
                        Display_ChangeMode = -1;
                    }
                }
        #endif

                break;
                // case SDL_MOUSEBUTTONUP, SDL_MOUSEBUTTONDOWN:
        #if SDL_VERSION_ATLEAST(2,0,0)
            case SDL_MOUSEWHEEL:
                if (cevent->wheel.y > 0)
                {
                    Display_ChangeMode = 1;
                }
                else if (cevent->wheel.y < 0)
                {
                    Display_ChangeMode = -1;
                }
                break;
                // case SDL_MOUSEWHEEL:
        #endif
        } // switch(event.type)


        if (VSyncTick != Game_VSyncTick) finish = 1;
        Game_MQueueRead = (Game_MQueueRead + 1) & (GAME_MQUEUE_LENGTH - 1);
    }

    return finish;
}

