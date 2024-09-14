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


static SDL_Joystick *stick = NULL;
static int key_l2 = 0;
static int mouse_x = 0;
static int mouse_y = 0;
static int mouse_right_button_mod;


void EmulateKey(int type, int key)
{
    SDL_Event pump_event;

    pump_event.type = type;
    pump_event.key.state = (type == SDL_KEYUP)?SDL_RELEASED:SDL_PRESSED;
    pump_event.key.repeat = 0;
    pump_event.key.keysym.sym = (SDL_Keycode) key;
    pump_event.key.keysym.mod = KMOD_NONE;

    SDL_PushEvent(&pump_event);
}

void Init_Input(void)
{
}

void Init_Input2(void)
{
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);

    stick = SDL_JoystickOpen(0);
    if (stick == NULL)
    {
        SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }
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
    switch(_event->type)
    {
        case SDL_MOUSEMOTION:
            mouse_x = _event->motion.x;
            mouse_y = _event->motion.y;
            break;

        case SDL_JOYAXISMOTION:
        case SDL_JOYBALLMOTION:
        case SDL_JOYHATMOTION:
            return 1;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            if (stick != NULL)
            {
                if (_event->jbutton.button == 8) // L2 button on Pyra
                {
                    key_l2 = (_event->type == SDL_JOYBUTTONDOWN)?1:0;
                }
            }
            return 1;

        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        case SDL_FINGERMOTION:
            if (Game_Renderer != NULL)
            {
                if (_event->type == SDL_FINGERMOTION)
                {
                    mouse_x = SDL_floorf(_event->tfinger.x * Display_Width);
                    mouse_y = SDL_floorf(_event->tfinger.y * Display_Height);

                    _event->type = SDL_MOUSEMOTION;
                    _event->motion.windowID = _event->tfinger.windowID;
                    _event->motion.which = SDL_TOUCH_MOUSEID;
                    _event->motion.state = 0;
                    _event->motion.x = mouse_x;
                    _event->motion.y = mouse_y;
                    _event->motion.xrel = 0;
                    _event->motion.yrel = 0;
                }
                else
                {
                    _event->button.windowID = _event->tfinger.windowID;
                    _event->button.which = SDL_TOUCH_MOUSEID;
#if SDL_VERSION_ATLEAST(2,0,2)
                    _event->button.clicks = 1;
#endif
                    _event->button.x = mouse_x;
                    _event->button.y = mouse_y;

                    if (_event->type == SDL_FINGERDOWN)
                    {
                        mouse_right_button_mod = (key_l2) ? 1 : 0;
                        _event->type = SDL_MOUSEBUTTONDOWN;
                        _event->button.button = (mouse_right_button_mod) ? SDL_BUTTON_RIGHT : SDL_BUTTON_LEFT;
                        _event->button.state = SDL_PRESSED;
                    }
                    else
                    {
                        _event->type = SDL_MOUSEBUTTONUP;
                        _event->button.button = (mouse_right_button_mod) ? SDL_BUTTON_RIGHT : SDL_BUTTON_LEFT;
                        _event->button.state = SDL_RELEASED;
                        mouse_right_button_mod = 0;
                    }
                }
            }
            break;

        default:
            break;
    }

    return 0;
}

int Handle_Input_Event2(SDL_Event *_event)
{
    return 0;
}

void Handle_Timer_Input_Event(void)
{
}

