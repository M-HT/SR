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

static int Input_GameController;
static int Controller_Deadzone;

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


void EmulateKey(int type, int key)
{
    SDL_Event pump_event;

    pump_event.type = type;
    pump_event.key.state = (type == SDL_KEYUP)?SDL_RELEASED:SDL_PRESSED;
#if SDL_VERSION_ATLEAST(2,0,0)
    pump_event.key.repeat = 0;
    pump_event.key.keysym.sym = (SDL_Keycode) key;
#else
    pump_event.key.keysym.sym = (SDLKey) key;
    pump_event.key.keysym.unicode = 0;
#endif
    pump_event.key.keysym.mod = KMOD_NONE;

    SDL_PushEvent(&pump_event);
}

static void EmulateDelay(int delay)
{
    SDL_Event pump_event;

    pump_event.type = SDL_USEREVENT;
    pump_event.user.code = EC_DELAY;
    pump_event.user.data1 = (void *)(intptr_t) delay;
    SDL_PushEvent(&pump_event);
}


static void EmulateMouseButton(int type, int button)
{
    SDL_Event pump_event;

    pump_event.type = SDL_USEREVENT;
    pump_event.user.code = EC_INPUT_MOUSE_BUTTON;
    pump_event.user.data1 = (void *)(intptr_t) button;
    pump_event.user.data2 = (void *)(intptr_t) type;

    SDL_PushEvent(&pump_event);
}

static void EmulateSelectGroup(void)
{
    Game_SelectGroupOnMove = SELECT_GROUP_ACTIVE;

    if (!VK_Visible)
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
                if (Game_SDLVersionNum >= SDL_VERSIONNUM(2,0,14))
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
                fprintf(stderr, "Using controller: %s (%s)\n", SDL_GameControllerName(controller), SDL_JoystickName(SDL_GameControllerGetJoystick(controller)));
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

            fprintf(stderr, "Using joystick: %s\n",
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

void Init_Input(void)
{
    Game_MouseHelper = 0;

    Game_HelperMouseMiddleButton = 0;
    Game_HelperMouseRightButton = 0;

    Game_SelectGroupOnMove = SELECT_GROUP_INACTIVE;
    Game_SelectGroupTreshold = 6;

    Input_GameController = 0;
    Controller_Deadzone = 1000;
}

void Init_Input2(void)
{
    if (Input_GameController)
    {
        SDL_InitSubSystem(SDL_INIT_JOYSTICK
#if SDL_VERSION_ATLEAST(2,0,0)
            | SDL_INIT_GAMECONTROLLER
#endif
        );

#if SDL_VERSION_ATLEAST(2,0,2)
        if (Game_SDLVersionNum >= SDL_VERSIONNUM(2,0,2))
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
        else if ( strcasecmp(str, "GameController") == 0) // str equals "GameController"
        {
            if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
            {
                Input_GameController = 1;
            }
            else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
            {
                Input_GameController = 0;
            }
        }
    }
    else if ( strcasecmp(str, "Controller_Deadzone") == 0) // str equals "Controller_Deadzone"
    {
        num_int = 0;
        sscanf(param, "%i", &num_int);
        if (num_int >= 0)
        {
            Controller_Deadzone = num_int;
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

                        _event->type = (intptr_t) _event->user.data2;
                        _event->button.button = (intptr_t) _event->user.data1;
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
                        if (!VK_Visible)
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
                        if (!VK_Visible)
                        {
                            Game_HelperMouseMiddleButton = 1;
                            EmulateRepairStop();
                            return 1;
                        }
                        break;
                    case SDL_BUTTON_RIGHT:
                        if (!VK_Visible)
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
                        if ((Game_SelectGroupOnMove == SELECT_GROUP_ACTIVE) && !VK_Visible)
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
    switch(_event->type)
    {
        case SDL_JOYAXISMOTION:
            if (joystick != NULL)
            {
                if (_event->jaxis.axis == 0)
                {
                    controller_axis_x = _event->jaxis.value;
                }
                else if (_event->jaxis.axis == 1)
                {
                    controller_axis_y = _event->jaxis.value;
                }
            }
            break;

        case SDL_JOYHATMOTION:
            if (joystick != NULL)
            {
                if (_event->jhat.hat == 0)
                {
                    if ((_event->jhat.value & 0x0f) != joystick_hat_position)
                    {
                        if (!(_event->jhat.value & 1) && (joystick_hat_position & 1)) // UP
                        {
                            EmulateKey(SDL_KEYUP, SDLK_UP);
                        }
                        if (!(_event->jhat.value & 2) && (joystick_hat_position & 2)) // RIGHT
                        {
                            EmulateKey(SDL_KEYUP, SDLK_RIGHT);
                        }
                        if (!(_event->jhat.value & 4) && (joystick_hat_position & 4)) // DOWN
                        {
                            EmulateKey(SDL_KEYUP, SDLK_DOWN);
                        }
                        if (!(_event->jhat.value & 8) && (joystick_hat_position & 8)) // LEFT
                        {
                            EmulateKey(SDL_KEYUP, SDLK_LEFT);
                        }

                        if ((_event->jhat.value & 1) && !(joystick_hat_position & 1)) // UP
                        {
                            EmulateKey(SDL_KEYDOWN, SDLK_UP);
                        }
                        if ((_event->jhat.value & 2) && !(joystick_hat_position & 2)) // RIGHT
                        {
                            EmulateKey(SDL_KEYDOWN, SDLK_RIGHT);
                        }
                        if ((_event->jhat.value & 4) && !(joystick_hat_position & 4)) // DOWN
                        {
                            EmulateKey(SDL_KEYDOWN, SDLK_DOWN);
                        }
                        if ((_event->jhat.value & 8) && !(joystick_hat_position & 8)) // LEFT
                        {
                            EmulateKey(SDL_KEYDOWN, SDLK_LEFT);
                        }

                        joystick_hat_position = _event->jhat.value & 0x0f;
                    }
                }
            }
            break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            if (joystick != NULL)
            {
                switch (_event->jbutton.button)
                {
                case 0:
                    if (!VK_Visible)
                    {
                        EmulateMouseButton((_event->type == SDL_JOYBUTTONDOWN) ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);
                    }
                    else
                    {
                        EmulateKey((_event->type == SDL_JOYBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_RETURN);
                    }
                    break;
                case 1:
                    if (!VK_Visible)
                    {
                        EmulateMouseButton((_event->type == SDL_JOYBUTTONDOWN) ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT);
                    }
                    else
                    {
                        EmulateKey((_event->type == SDL_JOYBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_BACKSPACE);
                    }
                    break;

                case 2:
                    EmulateKey((_event->type == SDL_JOYBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_F15);
                    break;
                case 3:
                    if (!VK_Visible)
                    {
                        EmulateKey((_event->type == SDL_JOYBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_RETURN);
                    }
                    else
                    {
                        EmulateKey((_event->type == SDL_JOYBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_TAB);
                    }
                    break;

                default:
                    break;
                }
            }
            break;

#if SDL_VERSION_ATLEAST(2,0,0)
        case SDL_CONTROLLERAXISMOTION:
            if (controller != NULL)
            {
                if (_event->caxis.axis == controller_base_axis)
                {
                    controller_axis_x = _event->caxis.value;
                }
                else if (_event->caxis.axis == (controller_base_axis + 1))
                {
                    controller_axis_y = _event->caxis.value;
                }
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            if (controller != NULL)
            {
                switch (_event->cbutton.button)
                {
                case SDL_CONTROLLER_BUTTON_A:
                case SDL_CONTROLLER_BUTTON_X:
                    if (!VK_Visible)
                    {
                        EmulateMouseButton((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);
                    }
                    else
                    {
                        EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_RETURN);
                    }
                    break;
                case SDL_CONTROLLER_BUTTON_B:
                case SDL_CONTROLLER_BUTTON_Y:
                    if (!VK_Visible)
                    {
                        EmulateMouseButton((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT);
                    }
                    else
                    {
                        EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_BACKSPACE);
                    }
                    break;

                case SDL_CONTROLLER_BUTTON_START:
                    EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_F15);
                    break;
                case SDL_CONTROLLER_BUTTON_BACK:
                    if (!VK_Visible)
                    {
                        EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_RETURN);
                    }
                    else
                    {
                        EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_TAB);
                    }
                    break;

                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_LSHIFT);
                    break;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    if (Game_MouseHelper)
                    {
                        EmulateMouseButton((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP, SDL_BUTTON_MIDDLE);
                    }
                    else
                    {
                        EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_RCTRL);
                    }
                    break;

                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_UP);
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_DOWN);
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_LEFT);
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    EmulateKey((_event->type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP, SDLK_RIGHT);
                    break;

                default:
                    break;
                }
            }
            break;

        case SDL_CONTROLLERDEVICEADDED:
            if (Input_GameController && (joystick == NULL) && (controller == NULL))
            {
                open_controller_or_joystick(_event->cdevice.which);
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            if (controller != NULL)
            {
                if (SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)) == _event->cdevice.which)
                {
                    SDL_GameControllerClose(controller);
                    controller = NULL;

                    controller_axis_x = 0;
                    controller_axis_y = 0;
                    controller_mouse_motion = 0;

                    VK_Visible = 0;

                    fprintf(stderr, "Controller disconnected\n");
                }
            }
            break;

        case SDL_JOYDEVICEADDED:
            if (Input_GameController && (joystick == NULL) && (controller == NULL))
            {
                open_controller_or_joystick(_event->jdevice.which);
            }
            break;

        case SDL_JOYDEVICEREMOVED:
            if (joystick != NULL)
            {
                if (SDL_JoystickInstanceID(joystick) == _event->jdevice.which)
                {
                    SDL_JoystickClose(joystick);
                    joystick = NULL;

                    joystick_hat_position = 0;
                    controller_axis_x = 0;
                    controller_axis_y = 0;
                    controller_mouse_motion = 0;

                    VK_Visible = 0;

                    fprintf(stderr, "Joystick disconnected\n");
                }
            }
            break;
#endif
        default:
            return 0;
    }

    return 1;
}

void Handle_Timer_Input_Event(void)
{
    int cx, cy, deltax, deltay;
    int64_t tx, ty;
    uint32_t tick, diff;
    SDL_Event event;

    cx = controller_axis_x;
    if ((cx <= Controller_Deadzone) && (cx >= -Controller_Deadzone)) cx = 0;
    cy = controller_axis_y;
    if ((cy <= Controller_Deadzone) && (cy >= -Controller_Deadzone)) cy = 0;

    if (VK_Visible)
    {
        if ((cx < 8191) && (cx > -8192)) cx = 0;
        if ((cy < 8191) && (cy > -8192)) cy = 0;

        if ((cx != 0) || (cy != 0))
        {
            tick = Game_TimerTick;
            diff = tick - controller_mouse_last_time;

            if (diff >= 10)
            {
                controller_mouse_last_time = tick;

                if (cy > 0)
                {
                    EmulateKey(SDL_KEYDOWN, SDLK_DOWN);
                }
                else if (cy < 0)
                {
                    EmulateKey(SDL_KEYDOWN, SDLK_UP);
                }

                if (cx > 0)
                {
                    EmulateKey(SDL_KEYDOWN, SDLK_RIGHT);
                }
                else if (cx < 0)
                {
                    EmulateKey(SDL_KEYDOWN, SDLK_LEFT);
                }
            }
        }

        return;
    }

    if ((cx != 0) || (cy != 0))
    {
        tick = Game_TimerTick;
        diff = tick - controller_mouse_last_time;

        if (diff > 0)
        {
            if (!controller_mouse_motion)
            {
                diff = 1;
                controller_frac_x = 0;
                controller_frac_y = 0;
            }

            controller_mouse_motion = 1;
            controller_mouse_last_time = tick;

            tx = (((int64_t)cx) * diff) * Game_VideoAspectXR + controller_frac_x;
            deltax = tx >> 29;
            controller_frac_x = tx - (((int64_t)deltax) << 29);

            ty = (((int64_t)cy) * diff) * Game_VideoAspectYR + controller_frac_y;
            deltay = ty >> 29;
            controller_frac_y = ty - (((int64_t)deltay) << 29);

            event.type = SDL_USEREVENT;
            event.user.code = EC_MOUSE_MOVE;
            event.user.data1 = (void *)(intptr_t) deltax;
            event.user.data2 = (void *)(intptr_t) deltay;

            SDL_PushEvent(&event);
        }
    }
    else
    {
        controller_mouse_motion = 0;
    }
}

