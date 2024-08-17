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

#include "../Game_defs.h"
#include "../Game_vars.h"

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static int keypad_xor_value_known = 0;
static int keypad_xor_value = 0;
#endif


void EmulateKey(int type, int key)
{
    SDL_Event pump_event;

    pump_event.type = type;
    pump_event.key.state = (type == SDL_KEYUP)?SDL_RELEASED:SDL_PRESSED;
#if SDL_VERSION_ATLEAST(2,0,0)
    pump_event.key.keysym.sym = (SDL_Keycode) key;
#else
    pump_event.key.keysym.sym = (SDLKey) key;
#endif
    pump_event.key.keysym.mod = KMOD_NONE;

    SDL_PushEvent(&pump_event);
}

void Init_Input(void)
{
    Game_Joystick = 0;
    Game_TouchscreenButtonEvents = 0;
    Game_RMBActive = 0;
}

void Init_Input2(void)
{
}

int Config_Input(char *str, char *param)
{
    return 0;
}

void Cleanup_Input(void)
{
}

int Handle_Input_Event(SDL_Event *_event)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    switch(_event->type)
    {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (!keypad_xor_value_known)
            {
                if (_event->type == SDL_KEYDOWN)
                {
                    keypad_xor_value_known = 1;
                    keypad_xor_value = ( ((GetKeyState(VK_NUMLOCK) & 0x0001)?KMOD_NUM:0) ^ (_event->key.keysym.mod & KMOD_NUM) )
                                     | ( ((GetKeyState(VK_CAPITAL) & 0x0001)?KMOD_CAPS:0) ^ (_event->key.keysym.mod & KMOD_CAPS) )
                                     ;
                }
            }

            _event->key.keysym.mod ^= keypad_xor_value;
        default:
            break;
    }
#endif

    return 0;
}

int Handle_Input_Event2(SDL_Event *_event)
{
    return 0;
}

void Handle_Timer_Input_Event(void)
{
}

