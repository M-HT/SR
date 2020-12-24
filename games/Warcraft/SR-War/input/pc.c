/**
 *
 *  Copyright (C) 2016-2020 Roman Pauer
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

#define SELECT_GROUP_INACTIVE 0
#define SELECT_GROUP_POSSIBLE 1
#define SELECT_GROUP_ACTIVE 2
#define SELECT_GROUP_STOPPED 3
#define SELECT_GROUP_IMPOSSIBLE 4

// variables
static int Game_MouseHelper, Game_HelperMouseMiddleButton, Game_HelperMouseRightButton;
static uint32_t Game_SelectGroupOnMove;
static int Game_SelectGroupX, Game_SelectGroupY, Game_SelectGroupTreshold;

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
#ifdef USE_SDL2
    pump_event.key.keysym.sym = (SDL_Keycode) key;
#else
    pump_event.key.keysym.sym = (SDLKey) key;
#endif
    pump_event.key.keysym.mod = KMOD_NONE;

    SDL_PushEvent(&pump_event);
}

static void EmulateDelay(int delay)
{
    SDL_Event pump_event;

    pump_event.type = SDL_USEREVENT;
    pump_event.user.code = EC_DELAY;
    pump_event.user.data1 = (void *) delay;
    SDL_PushEvent(&pump_event);
}


static void EmulateMouseButton(int type, int button)
{
    SDL_Event pump_event;

    pump_event.type = SDL_USEREVENT;
    pump_event.user.code = EC_INPUT_MOUSE_BUTTON;
    pump_event.user.data1 = (void *) button;
    pump_event.user.data2 = (void *) type;

    SDL_PushEvent(&pump_event);
}

static void EmulateSelectGroup(void)
{
    Game_SelectGroupOnMove = SELECT_GROUP_ACTIVE;

    if (!Game_Paused)
    {
        EmulateMouseButton(SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);

        EmulateDelay(30);

        EmulateKey(SDL_KEYDOWN, SDLK_LCTRL);

        EmulateDelay(30);

        EmulateMouseButton(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT);

        EmulateDelay(30);

        EmulateKey(SDL_KEYUP, SDLK_LCTRL);
    }
}

static void EmulateRepairStop(void)
{
    EmulateKey(SDL_KEYDOWN, SDLK_r);
    EmulateKey(SDL_KEYDOWN, SDLK_s);

    EmulateDelay(30);

    EmulateKey(SDL_KEYUP, SDLK_r);
    EmulateKey(SDL_KEYUP, SDLK_s);

    EmulateDelay(30);

    EmulateMouseButton(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT);
}

static void EmulateHarvestTransportAttackMoveWall(void)
{
    EmulateKey(SDL_KEYDOWN, SDLK_h);
    EmulateKey(SDL_KEYDOWN, SDLK_t);
    EmulateKey(SDL_KEYDOWN, SDLK_a);
    EmulateKey(SDL_KEYDOWN, SDLK_m);
    EmulateKey(SDL_KEYDOWN, SDLK_w);

    EmulateDelay(30);

    EmulateKey(SDL_KEYUP, SDLK_h);
    EmulateKey(SDL_KEYUP, SDLK_t);
    EmulateKey(SDL_KEYUP, SDLK_a);
    EmulateKey(SDL_KEYUP, SDLK_m);
    EmulateKey(SDL_KEYUP, SDLK_w);

    EmulateDelay(30);

    EmulateMouseButton(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT);

    EmulateDelay(30);
}


void Init_Input(void)
{
    Game_Joystick = 0;

    Game_MouseHelper = 0;

    Game_HelperMouseMiddleButton = 0;
    Game_HelperMouseRightButton = 0;

    Game_SelectGroupOnMove = SELECT_GROUP_INACTIVE;
    Game_SelectGroupTreshold = 6;
}

void Init_Input2(void)
{
}

int Config_Input(char *str, char *param)
{
    int num_int;

    if ( strncasecmp(str, "Input_", 6) == 0)	// str begins with "Input_"
    {
        str += 6;

        if ( strcasecmp(str, "MouseHelper") == 0)	// str equals "MouseHelper"
        {
            if ( strcasecmp(param, "on") == 0)	// param equals "on"
            {
                Game_MouseHelper = 1;
            }
            else if ( strcasecmp(param, "off") == 0) // param equals "off"
            {
                Game_MouseHelper = 0;
            }
        }
        else if ( strcasecmp(str, "SelectGroupTreshold") == 0)	// str equals "SelectGroupTreshold"
        {
            // Select group treshold

            num_int = 0;
            sscanf(param, "%i", &num_int);
            if (num_int < 0)
            {
                Game_SelectGroupTreshold = 0;
            }
            else if (num_int > 20)
            {
                Game_SelectGroupTreshold = 20;
            }
            else
            {
                Game_SelectGroupTreshold = num_int;
            }
        }
    }
    else
    {
        return 0;
    }

    return 1;
}

void Cleanup_Input(void)
{
    Game_HelperMouseMiddleButton = 0;
    Game_HelperMouseRightButton = 0;
    Game_SelectGroupOnMove = SELECT_GROUP_INACTIVE;
}

int Handle_Input_Event(SDL_Event *_event)
{
    switch(_event->type)
    {
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
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
#endif
        case SDL_USEREVENT:
            switch (_event->user.code)
            {
                case EC_INPUT_MOUSE_BUTTON:
                    {
                        int mousex, mousey;

                        SDL_GetMouseState(&mousex, &mousey);

                        _event->type = (int) _event->user.data2;
                        _event->button.button = (int) _event->user.data1;
                        _event->button.state = (_event->type == SDL_MOUSEBUTTONUP)?SDL_RELEASED:SDL_PRESSED;
                        _event->button.x = mousex;
                        _event->button.y = mousey;
                    }
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (Game_MouseHelper)
            {
                switch (_event->button.button)
                {
                    case SDL_BUTTON_LEFT:
                        if (!Game_Paused)
                        {
                            if (Game_SelectGroupOnMove == SELECT_GROUP_STOPPED)
                            {
                                Game_SelectGroupOnMove = SELECT_GROUP_IMPOSSIBLE;
                            }
                            else
                            {
                                Game_SelectGroupOnMove = SELECT_GROUP_POSSIBLE;
                                Game_SelectGroupX = _event->motion.x;
                                Game_SelectGroupY = _event->motion.y;
                            }
                        }
                        break;
                    case SDL_BUTTON_MIDDLE:
                        if (!Game_Paused)
                        {
                            Game_HelperMouseMiddleButton = 1;
                            EmulateRepairStop();
                            return 1;
                        }
                        break;
                    case SDL_BUTTON_RIGHT:
                        if (!Game_Paused)
                        {
                            Game_HelperMouseRightButton = 1;
                            EmulateHarvestTransportAttackMoveWall();
                            return 1;
                        }
                        break;
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (Game_MouseHelper)
            {
                switch (_event->button.button)
                {
                    case SDL_BUTTON_MIDDLE:
                    case SDL_BUTTON_RIGHT:
                        if ((_event->button.button == SDL_BUTTON_MIDDLE) && Game_HelperMouseMiddleButton)
                        {
                            Game_HelperMouseMiddleButton = 0;
                            _event->button.button = SDL_BUTTON_LEFT;
                        }
                        else if ((_event->button.button == SDL_BUTTON_RIGHT) && Game_HelperMouseRightButton)
                        {
                            Game_HelperMouseRightButton = 0;
                            _event->button.button = SDL_BUTTON_LEFT;
                        }
                        else
                        {
                            break;
                        }
                    case SDL_BUTTON_LEFT:
                        if ((Game_SelectGroupOnMove == SELECT_GROUP_ACTIVE) && !Game_Paused)
                        {
                            Game_SelectGroupOnMove = SELECT_GROUP_STOPPED;
                        }
                        else
                        {
                            Game_SelectGroupOnMove = SELECT_GROUP_INACTIVE;
                        }
                        break;
                }
            }
            break;
        case SDL_MOUSEMOTION:
            if (Game_MouseHelper)
            {
                if (Game_SelectGroupOnMove == SELECT_GROUP_POSSIBLE)
                {
                    if ((abs(Game_SelectGroupX - _event->motion.x) > Game_SelectGroupTreshold) ||
                        (abs(Game_SelectGroupY - _event->motion.y) > Game_SelectGroupTreshold)
                       )
                    {
                        EmulateSelectGroup();
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

