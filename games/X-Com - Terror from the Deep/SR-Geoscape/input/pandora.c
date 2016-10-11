/**
 *
 *  Copyright (C) 2016 Roman Pauer
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
#include "../Geoscape-proc-events.h"
#include <time.h>
#include <sys/stat.h>

// definitions
#define GAME_KEYBOARD_DPAD          (0)
#define GAME_TOUCHSCREEN_DPAD       (1)
#define GAME_TOUCHSCREEN_ABXY       (2)

#define BUTTON_Y               (0)
#define BUTTON_X               (1)
#define BUTTON_A               (2)
#define BUTTON_B               (3)
#define BUTTON_START           (4)
#define BUTTON_SELECT          (5)
#define BUTTON_L               (6)
#define BUTTON_R               (7)
#define BUTTON_UP              (8)
#define BUTTON_DOWN            (9)
#define BUTTON_LEFT            (10)
#define BUTTON_RIGHT           (11)
#define BUTTON_LAST            (12)

#define PANDORA_BUTTON_UP              (SDLK_UP)
#define PANDORA_BUTTON_DOWN            (SDLK_DOWN)
#define PANDORA_BUTTON_LEFT            (SDLK_LEFT)
#define PANDORA_BUTTON_RIGHT           (SDLK_RIGHT)
#define PANDORA_BUTTON_Y               (SDLK_PAGEUP)
#define PANDORA_BUTTON_X               (SDLK_PAGEDOWN)
#define PANDORA_BUTTON_A               (SDLK_HOME)
#define PANDORA_BUTTON_B               (SDLK_END)
#define PANDORA_BUTTON_L               (SDLK_RSHIFT)
#define PANDORA_BUTTON_R               (SDLK_RCTRL)
#define PANDORA_BUTTON_START           (SDLK_LALT)
#define PANDORA_BUTTON_SELECT          (SDLK_LCTRL)

typedef int (*Action_function)(int pressed, int key, SDL_Event *event);


// variables
static volatile int Game_PButton[BUTTON_LAST];	/* pandora buttons */
static int Game_ModButton;				/* modifier key */

static volatile int Game_DKeyboard;		/* is dpad acting as keyboard ? */
static int Game_RMBModifier;
static int Game_RMBPressed;

static int Game_RotateOrigMousePos[2];			// Used to store previous mouse cursor position before earth rotation macros.
static volatile int Game_CurrentlyRotating;			// Flag indicates whether we're currently rotating

static int Game_InputMode;

static Action_function Action_button[BUTTON_LAST], Action_button_R[BUTTON_LAST];

static int Action_button_Key[BUTTON_LAST], Action_button_R_Key[BUTTON_LAST];


// functions
void EmulateKey(int type, SDLKey key)
{
    SDL_Event pump_event;

    pump_event.type = SDL_USEREVENT;
    pump_event.user.code = EC_INPUT_KEY;
    pump_event.user.data1 = (void *) key;
    pump_event.user.data2 = (void *) type;

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

static void WarpGameMouse(int mousex, int mousey)
{
    SDL_Event pump_event;

    pump_event.type = SDL_MOUSEMOTION;
    pump_event.motion.x = Game_Picture2DeviceX((mousex * Game_VideoAspectXR + 32767) >> 16);
    pump_event.motion.y = Game_Picture2DeviceY((mousey * Game_VideoAspectYR + 32767) >> 16);
    SDL_PushEvent(&pump_event);

    SDL_WarpMouse(pump_event.motion.x, pump_event.motion.y);
}

static void GetMousePos(int32_t *ptr)
{
    SDL_Event pump_event;

    pump_event.type = SDL_USEREVENT;
    pump_event.user.code = EC_GET_MOUSE_POS;
    pump_event.user.data1 = (void *) ptr;
    SDL_PushEvent(&pump_event);
}

static void EmulateMouseButton(int type, int button)
{
    SDL_Event pump_event;
    int mousex, mousey;

    SDL_GetMouseState(&mousex, &mousey);

    pump_event.type = type;
    pump_event.button.button = button;
    pump_event.button.state = (type == SDL_MOUSEBUTTONUP)?SDL_RELEASED:SDL_PRESSED;
    pump_event.button.x = mousex;
    pump_event.button.y = mousey;

    SDL_PushEvent(&pump_event);
}

static int Action_left_mouse_button(int pressed, int key, SDL_Event *event)
{
    if (!Game_Paused)
    {
        EmulateMouseButton((pressed)?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);

        return 0;
    }
    else
    {
#if !defined(SDLK_ENTER)
    #define SDLK_ENTER SDLK_RETURN
#endif
        if (pressed == 0)
        {
            event->key.keysym.sym = SDLK_ENTER;
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

static int Action_right_mouse_button(int pressed, int key, SDL_Event *event)
{
    if (!Game_Paused)
    {
        EmulateMouseButton((pressed)?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT);

        return 0;
    }
    else
    {
        return 1;
    }
}

static int Action_key(int pressed, int key, SDL_Event *event)
{
    if ((!Game_Paused) || (key == SDLK_PAUSE))
    {
        event->key.keysym.sym = (SDLKey) key;
        return 0;
    }
    return 1;
}

//senquack - new set of four macros to facilitate rotating the earth
static int Action_rotateup(int pressed, int key, SDL_Event *event)
{
    if (!Game_Paused)
    {
        if (pressed)
        {
            if (!Game_CurrentlyRotating)
            {
                GetMousePos(&(Game_RotateOrigMousePos[0]));
            }

            Game_CurrentlyRotating = 1;

            // Simulate movement to geoscape "Rotate Up" icon
            WarpGameMouse(278, 165);
            EmulateDelay((Game_Main_Loop_VSync_Ticks + 2) * 17);
            EmulateKey(SDL_MOUSEBUTTONDOWN, (SDLKey) SDL_BUTTON_LEFT);
        }
        else
        {
            if ( Game_CurrentlyRotating == 1 )
            {
                Game_CurrentlyRotating = 0;
                EmulateKey(SDL_MOUSEBUTTONUP, (SDLKey) SDL_BUTTON_LEFT);
                EmulateDelay((Game_Main_Loop_VSync_Ticks + 2) * 17);
                SDL_WarpMouse(Game_RotateOrigMousePos[0], Game_RotateOrigMousePos[1]);
            }
        }
    }
    return 1;
}

static int Action_rotatedown(int pressed, int key, SDL_Event *event)
{
    if (!Game_Paused)
    {
        if (pressed)
        {
            if (!Game_CurrentlyRotating)
            {
                GetMousePos(&(Game_RotateOrigMousePos[0]));
            }

            Game_CurrentlyRotating = 2;

            // Simulate movement to geoscape "Rotate Down" icon
            WarpGameMouse(278, 195);
            EmulateDelay((Game_Main_Loop_VSync_Ticks + 2) * 17);
            EmulateKey(SDL_MOUSEBUTTONDOWN, (SDLKey) SDL_BUTTON_LEFT);
        }
        else
        {
            if ( Game_CurrentlyRotating == 2 )
            {
                Game_CurrentlyRotating = 0;
                EmulateKey(SDL_MOUSEBUTTONUP, (SDLKey) SDL_BUTTON_LEFT);
                EmulateDelay((Game_Main_Loop_VSync_Ticks + 2) * 17);
                SDL_WarpMouse(Game_RotateOrigMousePos[0], Game_RotateOrigMousePos[1]);
            }
        }
    }
    return 1;
}

static int Action_rotateleft(int pressed, int key, SDL_Event *event)
{
    if (!Game_Paused)
    {
        if (pressed)
        {
            if (!Game_CurrentlyRotating)
            {
                GetMousePos(&(Game_RotateOrigMousePos[0]));
            }

            Game_CurrentlyRotating = 3;

            // Simulate movement to geoscape "Rotate Left" icon
            WarpGameMouse(264, 181);
            EmulateDelay((Game_Main_Loop_VSync_Ticks + 2) * 17);
            EmulateKey(SDL_MOUSEBUTTONDOWN, (SDLKey) SDL_BUTTON_LEFT);
        }
        else
        {
            if ( Game_CurrentlyRotating == 3 )
            {
                Game_CurrentlyRotating = 0;
                EmulateKey(SDL_MOUSEBUTTONUP, (SDLKey) SDL_BUTTON_LEFT);
                EmulateDelay((Game_Main_Loop_VSync_Ticks + 2) * 17);
                SDL_WarpMouse(Game_RotateOrigMousePos[0], Game_RotateOrigMousePos[1]);
            }
        }
    }
    return 1;
}

static int Action_rotateright(int pressed, int key, SDL_Event *event)
{
    if (!Game_Paused)
    {
        if (pressed)
        {
            if (!Game_CurrentlyRotating)
            {
                GetMousePos(&(Game_RotateOrigMousePos[0]));
            }

            Game_CurrentlyRotating = 4;

            // Simulate movement to geoscape "Rotate Right" icon
            WarpGameMouse(292, 181);
            EmulateDelay((Game_Main_Loop_VSync_Ticks + 2) * 17);
            EmulateKey(SDL_MOUSEBUTTONDOWN, (SDLKey) SDL_BUTTON_LEFT);
        }
        else
        {
            if ( Game_CurrentlyRotating == 4 )
            {
                Game_CurrentlyRotating = 0;
                EmulateKey(SDL_MOUSEBUTTONUP, (SDLKey) SDL_BUTTON_LEFT);
                EmulateDelay((Game_Main_Loop_VSync_Ticks + 2) * 17);
                SDL_WarpMouse(Game_RotateOrigMousePos[0], Game_RotateOrigMousePos[1]);
            }
        }
    }
    return 1;
}

//levelup action in geoscape is used for zooming in
static int Action_levelup(int pressed, int key, SDL_Event *event)
{
    static int32_t orig_pos[2];			// Stores mouse coordinates while macro occurs

    if (!Game_Paused)
    {
        if (pressed)
        {
            GetMousePos(&(orig_pos[0]));			// Store away mouse coordinates before macro

            // Simulate movement to geoscape "Zoom In" icon
            WarpGameMouse(305, 168);
            EmulateKey(SDL_MOUSEBUTTONDOWN, (SDLKey) SDL_BUTTON_LEFT);
        }
        else
        {
            EmulateKey(SDL_MOUSEBUTTONUP, (SDLKey) SDL_BUTTON_LEFT);
            SDL_WarpMouse(orig_pos[0], orig_pos[1]);
        }
    }
    return 1;
}

//leveldown action in geoscape is used for zooming out
static int Action_leveldown(int pressed, int key, SDL_Event *event)
{
    static int32_t orig_pos[2];			// Stores mouse coordinates while macro occurs

    if (!Game_Paused)
    {
        if (pressed)
        {
            GetMousePos(&(orig_pos[0]));			// Store away mouse coordinates before macro

            // Simulate movement to geoscape "Zoom Out" icon
            WarpGameMouse(305, 189);
            EmulateKey(SDL_MOUSEBUTTONDOWN, (SDLKey) SDL_BUTTON_LEFT);
        }
        else
        {
            EmulateKey(SDL_MOUSEBUTTONUP, (SDLKey) SDL_BUTTON_LEFT);
            SDL_WarpMouse(orig_pos[0], orig_pos[1]);
        }
    }
    return 1;
}

//selectnextsoldier action in geoscape is used for setting time to 5 secs
static int Action_selectnextsoldier(int pressed, int key, SDL_Event *event)
{
    static int32_t orig_pos[2];			// Stores mouse coordinates while macro occurs

    if (!Game_Paused)
    {
        if (pressed)
        {
            GetMousePos(&(orig_pos[0]));			// Store away mouse coordinates before macro

            // Simulate movement to geoscape "5 Secs" icon
            WarpGameMouse(272, 118);
            EmulateKey(SDL_MOUSEBUTTONDOWN, (SDLKey) SDL_BUTTON_LEFT);
        }
        else
        {
            EmulateKey(SDL_MOUSEBUTTONUP, (SDLKey) SDL_BUTTON_LEFT);
            SDL_WarpMouse(orig_pos[0], orig_pos[1]);
        }
    }
    return 1;
}

//deselectcurrentsoldier action in geoscape is used for setting time to 1 day
static int Action_deselectcurrentsoldier(int pressed, int key, SDL_Event *event)
{
    static int32_t orig_pos[2];			// Stores mouse coordinates while macro occurs

    if (!Game_Paused)
    {
        if (pressed)
        {
            GetMousePos(&(orig_pos[0]));			// Store away mouse coordinates before macro

            // Simulate movement to geoscape "1 Day" icon
            WarpGameMouse(302, 146);
            EmulateKey(SDL_MOUSEBUTTONDOWN, (SDLKey) SDL_BUTTON_LEFT);
        }
        else
        {
            EmulateKey(SDL_MOUSEBUTTONUP, (SDLKey) SDL_BUTTON_LEFT);
            SDL_WarpMouse(orig_pos[0], orig_pos[1]);
        }
    }
    return 1;
}

static int Action_toggle_scaling(int pressed, int key, SDL_Event *event)
{
    if (pressed == 0)
    {
        Display_ChangeMode = 1;
    }
    return 1;
}

static int Action_none(int pressed, int key, SDL_Event *event)
{
    return 1;
}



void Init_Input(void)
{
    Game_Joystick = 0;

    Game_InputMode = GAME_TOUCHSCREEN_DPAD;
    Game_ModButton = 0;
    Game_DKeyboard = 0;
    Game_RMBModifier = 0;
    Game_RMBPressed = 0;
    Game_CurrentlyRotating = 0;

    memset(Action_button, 0, sizeof(Action_button));
    memset(Action_button_R, 0, sizeof(Action_button_R));
    memset(Action_button_Key, 0, sizeof(Action_button_Key));
    memset(Action_button_R_Key, 0, sizeof(Action_button_R_Key));

    Action_button[BUTTON_START] = &Action_key;
    Action_button[BUTTON_SELECT] = &Action_toggle_scaling;

    Action_button_Key[BUTTON_START] = SDLK_PAUSE;
}

void Init_Input2(void)
{
    int index;

    // without this code, calling localtime in function Game_readdir (called by Game_dos_findfirst) in file Albion-proc-vfs.c crashes on Pandora
    {
        struct stat statbuf;
        struct tm *tmbuf;

        statbuf.st_mtime = 0;
        tmbuf = localtime(&(statbuf.st_mtime));
    }

    if (Game_InputMode != GAME_KEYBOARD_DPAD)
    {
        Game_DKeyboard = 1;
    }

    for (index = 0; index < BUTTON_LAST; index++)
    {
        if (Action_button[index] == NULL) Action_button[index] = Action_none;
        if (Action_button_R[index] == NULL) Action_button_R[index] = Action_button[index];
    }
}

int Config_Input(char *str, char *param)
{
    //int num_int;

    if ( strncasecmp(str, "Input_", 6) == 0)	// str begins with "Input_"
    {
        str += 6;

        if ( strcasecmp(str, "Mode") == 0)	// str equals "Mode"
        {
            if ( strcasecmp(param, "keyboard_dpad") == 0)	// param equals "keyboard_dpad"
            {
                // Keyboard with dpad as direction controls
                Game_InputMode = GAME_KEYBOARD_DPAD;
            }
            else if ( strcasecmp(param, "touchscreen_dpad") == 0) // param equals "touchscreen_dpad"
            {
                // Touchscreen with dpad as direction controls
                Game_InputMode = GAME_TOUCHSCREEN_DPAD;
            }
            else if ( strcasecmp(param, "touchscreen_abxy") == 0) // param equals "touchscreen_abxy"
            {
                // Touchscreen with abxy as direction controls
                Game_InputMode = GAME_TOUCHSCREEN_ABXY;
            }
        }
    }
    else if ( strncasecmp(str, "Button_", 7) == 0 ) // str begins with "Button_"
    {
        // button mapping
        Action_function Action_current;
        int Action_current_Key;

        str += 7;
        Action_current = NULL;
        Action_current_Key = 0;

        // find the correct action
        if ( strncasecmp(param, "Action_", 7) == 0 ) // param begins with "Action_"
        {
            param += 7;

            if ( strncasecmp(param, "key_", 4) == 0 ) // param begins with "key_"
            {
                // key

                param += 4;

                if ( strcasecmp(param, "esc") == 0 ) // param equals "esc"
                {
                    // esc key

                    Action_current = &Action_key;
                    Action_current_Key = SDLK_ESCAPE;
                }
                else if ( strcasecmp(param, "enter") == 0 ) // param equals "enter"
                {
                    // enter key

                    Action_current = &Action_key;
                    Action_current_Key = SDLK_ENTER;
                }
            }
            else if ( strncasecmp(param, "mouse_", 6) == 0 ) // param begins with "mouse_"
            {
                // mouse button

                param += 6;

                if ( strcasecmp(param, "left_button") == 0 ) // param equals "left_button"
                {
                    // left mouse button

                    Action_current = &Action_left_mouse_button;
                }
                else if ( strcasecmp(param, "right_button") == 0 ) // param equals "right_button"
                {
                    // right mouse button

                    Action_current = &Action_right_mouse_button;
                }

            }
            else if ( strcasecmp(param, "virtual_keyboard") == 0 ) // param equals "virtual_keyboard"
            {
                // virtual keyboard

                Action_current = &Action_key;
                Action_current_Key = SDLK_F15;
            }
            /*else if ( strcasecmp(param, "rotateup") == 0 ) // param equals "rotateup"
            {
                // rotate Up macro

                Action_current = &Action_rotateup;
            }
            else if ( strcasecmp(param, "rotatedown") == 0 ) // param equals "rotatedown"
            {
                // rotate Down macro

                Action_current = &Action_rotatedown;
            }
            else if ( strcasecmp(param, "rotateleft") == 0 ) // param equals "rotateleft"
            {
                // rotate Left macro

                Action_current = &Action_rotateleft;
            }
            else if ( strcasecmp(param, "rotateright") == 0 ) // param equals "rotateright"
            {
                // rotate Right macro

                Action_current = &Action_rotateright;
            }*/
            //senquack
            else if ( strcasecmp(param, "levelup") == 0 ) // param equals "levelup"
            {
                // Battlescape View Level Up

                Action_current = &Action_levelup;
            }
            else if ( strcasecmp(param, "leveldown") == 0 ) // param equals "leveldown"
            {
                // Battlescape View Level Down

                Action_current = &Action_leveldown;
            }
            else if ( strcasecmp(param, "selectnextsoldier") == 0 ) // param equals "selectnextsoldier"
            {
                // Battlescape Select Next Soldier

                Action_current = &Action_selectnextsoldier;
            }
            else if ( strcasecmp(param, "deselectcurrentsoldier") == 0 ) // param equals "deselectcurrentsoldier"
            {
                // Battlescape Deselect Current Soldier

                Action_current = &Action_deselectcurrentsoldier;
            }
            else if ( strcasecmp(param, "toggle_scaling") == 0 ) // param equals "toggle_scaling"
            {
                // Toggle scaling

                Action_current = &Action_toggle_scaling;
            }
            else if ( strcasecmp(param, "pause") == 0 ) // param equals "pause"
            {
                // Pause game

                Action_current = &Action_key;
                Action_current_Key = SDLK_PAUSE;
            }
            else if ( strcasecmp(param, "none") == 0 ) // param equals "none"
            {
                // no action

                Action_current = &Action_none;
            }
        }


        // find the correct button
        {
            if ( strcasecmp(str, "A") == 0 ) // str equals "A"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button[BUTTON_A] = Action_current;
                    Action_button_Key[BUTTON_A] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "B") == 0 ) // str equals "B"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button[BUTTON_B] = Action_current;
                    Action_button_Key[BUTTON_B] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "X") == 0 ) // str equals "X"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button[BUTTON_X] = Action_current;
                    Action_button_Key[BUTTON_X] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "Y") == 0 ) // str equals "Y"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button[BUTTON_Y] = Action_current;
                    Action_button_Key[BUTTON_Y] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "Up") == 0 ) // str equals "Up"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button[BUTTON_UP] = Action_current;
                    Action_button_Key[BUTTON_UP] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "Down") == 0 ) // str equals "Down"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button[BUTTON_DOWN] = Action_current;
                    Action_button_Key[BUTTON_DOWN] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "Left") == 0 ) // str equals "Left"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button[BUTTON_LEFT] = Action_current;
                    Action_button_Key[BUTTON_LEFT] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "Right") == 0 ) // str equals "Right"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button[BUTTON_RIGHT] = Action_current;
                    Action_button_Key[BUTTON_RIGHT] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "Select") == 0 ) // str equals "Select"
            {
                    Action_button[BUTTON_SELECT] = Action_current;
                    Action_button_Key[BUTTON_SELECT] = Action_current_Key;
            }
            else if ( strcasecmp(str, "Start") == 0 ) // str equals "Start"
            {
                    Action_button[BUTTON_START] = Action_current;
                    Action_button_Key[BUTTON_START] = Action_current_Key;
            }
            else if ( strcasecmp(str, "R+A") == 0 ) // str equals "R+A"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_A] = Action_current;
                    Action_button_R_Key[BUTTON_A] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "R+B") == 0 ) // str equals "R+B"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_B] = Action_current;
                    Action_button_R_Key[BUTTON_B] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "R+X") == 0 ) // str equals "R+X"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_X] = Action_current;
                    Action_button_R_Key[BUTTON_X] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "R+Y") == 0 ) // str equals "R+Y"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_Y] = Action_current;
                    Action_button_R_Key[BUTTON_Y] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "R+Select") == 0 ) // str equals "R+Select"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_SELECT] = Action_current;
                    Action_button_R_Key[BUTTON_SELECT] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "R+Start") == 0 ) // str equals "R+Start"
            {
                if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_START] = Action_current;
                    Action_button_R_Key[BUTTON_START] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "L+Up") == 0 ) // str equals "L+Up"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_UP] = Action_current;
                    Action_button_R_Key[BUTTON_UP] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "L+Down") == 0 ) // str equals "L+Down"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_DOWN] = Action_current;
                    Action_button_R_Key[BUTTON_DOWN] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "L+Left") == 0 ) // str equals "L+Left"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_LEFT] = Action_current;
                    Action_button_R_Key[BUTTON_LEFT] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "L+Right") == 0 ) // str equals "L+Right"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_RIGHT] = Action_current;
                    Action_button_R_Key[BUTTON_RIGHT] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "L+Select") == 0 ) // str equals "L+Select"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_SELECT] = Action_current;
                    Action_button_R_Key[BUTTON_SELECT] = Action_current_Key;
                }
            }
            else if ( strcasecmp(str, "L+Start") == 0 ) // str equals "L+Start"
            {
                if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                {
                    Action_button_R[BUTTON_START] = Action_current;
                    Action_button_R_Key[BUTTON_START] = Action_current_Key;
                }
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
// GAME_JOYSTICK != JOYSTICK_NONE
    memset((void *) &Game_PButton, 0, sizeof(Game_PButton));
}

int Handle_Input_Event(SDL_Event *_event)
{
    switch (_event->type)
    {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            _event->key.keysym.mod = (SDLMod) ((unsigned int) _event->key.keysym.mod & ~(KMOD_RSHIFT | KMOD_RCTRL | KMOD_LALT | KMOD_LCTRL));

            switch ((int) _event->key.keysym.sym)
            {
                case PANDORA_BUTTON_UP:
                    Game_PButton[BUTTON_UP] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                    {
                        if ( Game_ModButton )
                        {
                            return Action_button_R[BUTTON_UP] (Game_PButton[BUTTON_UP], Action_button_R_Key[BUTTON_UP], _event);
                        }
                        else
                        {
                            return Action_button[BUTTON_UP] (Game_PButton[BUTTON_UP], Action_button_Key[BUTTON_UP], _event);
                        }
                    }
                    else
                    {
                        if (Game_Paused)
                        {
                            _event->key.keysym.sym = SDLK_UP;
                        }
                        else
                        {
                            if (Game_DKeyboard)
                            {
                                return Action_rotateup(Game_PButton[BUTTON_UP], 0, _event);
                            }
                            else Game_CurrentlyRotating = 0;

                            return 1;
                        }
                    }
                    break;
                case PANDORA_BUTTON_DOWN:
                    Game_PButton[BUTTON_DOWN] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                    {
                        if ( Game_ModButton )
                        {
                            return Action_button_R[BUTTON_DOWN] (Game_PButton[BUTTON_DOWN], Action_button_R_Key[BUTTON_DOWN], _event);
                        }
                        else
                        {
                            return Action_button[BUTTON_DOWN] (Game_PButton[BUTTON_DOWN], Action_button_Key[BUTTON_DOWN], _event);
                        }
                    }
                    else
                    {
                        if (Game_Paused)
                        {
                            _event->key.keysym.sym = SDLK_DOWN;
                        }
                        else
                        {
                            if (Game_DKeyboard)
                            {
                                return Action_rotatedown(Game_PButton[BUTTON_DOWN], 0, _event);
                            }
                            else Game_CurrentlyRotating = 0;

                            return 1;
                        }
                    }
                    break;
                case PANDORA_BUTTON_LEFT:
                    Game_PButton[BUTTON_LEFT] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                    {
                        if ( Game_ModButton )
                        {
                            return Action_button_R[BUTTON_LEFT] (Game_PButton[BUTTON_LEFT], Action_button_R_Key[BUTTON_LEFT], _event);
                        }
                        else
                        {
                            return Action_button[BUTTON_LEFT] (Game_PButton[BUTTON_LEFT], Action_button_Key[BUTTON_LEFT], _event);
                        }
                    }
                    else
                    {
                        if (Game_Paused)
                        {
                            _event->key.keysym.sym = SDLK_LEFT;
                        }
                        else
                        {
                            if (Game_DKeyboard)
                            {
                                return Action_rotateleft(Game_PButton[BUTTON_LEFT], 0, _event);
                            }
                            else Game_CurrentlyRotating = 0;

                            return 1;
                        }
                    }
                    break;
                case PANDORA_BUTTON_RIGHT:
                    Game_PButton[BUTTON_RIGHT] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                    {
                        if ( Game_ModButton )
                        {
                            return Action_button_R[BUTTON_RIGHT] (Game_PButton[BUTTON_RIGHT], Action_button_R_Key[BUTTON_RIGHT], _event);
                        }
                        else
                        {
                            return Action_button[BUTTON_RIGHT] (Game_PButton[BUTTON_RIGHT], Action_button_Key[BUTTON_RIGHT], _event);
                        }
                    }
                    else
                    {
                        if (Game_Paused)
                        {
                            _event->key.keysym.sym = SDLK_RIGHT;
                        }
                        else
                        {
                            if (Game_DKeyboard)
                            {
                                return Action_rotateright(Game_PButton[BUTTON_RIGHT], 0, _event);
                            }
                            return 1;
                        }
                    }
                    break;


                case PANDORA_BUTTON_Y:
                    Game_PButton[BUTTON_Y] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                    {
                        if (Game_Paused)
                        {
                            _event->key.keysym.sym = SDLK_UP;
                        }
                        else
                        {
                            if (Game_DKeyboard)
                            {
                                return Action_rotateup(Game_PButton[BUTTON_Y], 0, _event);
                            }
                            else Game_CurrentlyRotating = 0;

                            return 1;
                        }
                    }
                    else
                    {
                        if ( Game_ModButton )
                        {
                            return Action_button_R[BUTTON_Y] (Game_PButton[BUTTON_Y], Action_button_R_Key[BUTTON_Y], _event);
                        }
                        else
                        {
                            return Action_button[BUTTON_Y] (Game_PButton[BUTTON_Y], Action_button_Key[BUTTON_Y], _event);
                        }
                    }
                    break;
                case PANDORA_BUTTON_X:
                    Game_PButton[BUTTON_X] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                    {
                        if (Game_Paused)
                        {
                            _event->key.keysym.sym = SDLK_DOWN;
                        }
                        else
                        {
                            if (Game_DKeyboard)
                            {
                                return Action_rotatedown(Game_PButton[BUTTON_X], 0, _event);
                            }
                            else Game_CurrentlyRotating = 0;

                            return 1;
                        }
                    }
                    else
                    {
                        if ( Game_ModButton )
                        {
                            return Action_button_R[BUTTON_X] (Game_PButton[BUTTON_X], Action_button_R_Key[BUTTON_X], _event);
                        }
                        else
                        {
                            return Action_button[BUTTON_X] (Game_PButton[BUTTON_X], Action_button_Key[BUTTON_X], _event);
                        }
                    }
                    break;
                case PANDORA_BUTTON_A:
                    Game_PButton[BUTTON_A] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                    {
                        if (Game_Paused)
                        {
                            _event->key.keysym.sym = SDLK_LEFT;
                        }
                        else
                        {
                            if (Game_DKeyboard)
                            {
                                return Action_rotateleft(Game_PButton[BUTTON_A], 0, _event);
                            }
                            else Game_CurrentlyRotating = 0;

                            return 1;
                        }
                    }
                    else
                    {
                        if ( Game_ModButton )
                        {
                            return Action_button_R[BUTTON_A] (Game_PButton[BUTTON_A], Action_button_R_Key[BUTTON_A], _event);
                        }
                        else
                        {
                            return Action_button[BUTTON_A] (Game_PButton[BUTTON_A], Action_button_Key[BUTTON_A], _event);
                        }
                    }
                    break;
                case PANDORA_BUTTON_B:
                    Game_PButton[BUTTON_B] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                    {
                        if (Game_Paused)
                        {
                            _event->key.keysym.sym = SDLK_RIGHT;
                        }
                        else
                        {
                            if (Game_DKeyboard)
                            {
                                return Action_rotateright(Game_PButton[BUTTON_B], 0, _event);
                            }
                            else Game_CurrentlyRotating = 0;

                            return 1;
                        }
                    }
                    else
                    {
                        if ( Game_ModButton )
                        {
                            return Action_button_R[BUTTON_B] (Game_PButton[BUTTON_B], Action_button_R_Key[BUTTON_B], _event);
                        }
                        else
                        {
                            return Action_button[BUTTON_B] (Game_PButton[BUTTON_B], Action_button_Key[BUTTON_B], _event);
                        }
                    }
                    break;

                case PANDORA_BUTTON_L:
                    Game_PButton[BUTTON_L] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
                    {
                        Game_ModButton = Game_PButton[BUTTON_L];
                        return 1;
                    }
                    else if (Game_InputMode == GAME_KEYBOARD_DPAD)
                    {
                        Game_DKeyboard = Game_PButton[BUTTON_L];
                        return 1;
                    }
                    else
                    {
                        Game_RMBModifier = Game_PButton[BUTTON_L];
                        return 1;
                    }

                    break;
                case PANDORA_BUTTON_R:
                    Game_PButton[BUTTON_R] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_InputMode != GAME_TOUCHSCREEN_ABXY)
                    {
                        Game_ModButton = Game_PButton[BUTTON_R];

                        return 1;
                    }
                    else
                    {
                        Game_RMBModifier = Game_PButton[BUTTON_R];
                        return 1;
                    }

                    break;


                case PANDORA_BUTTON_START:
                    Game_PButton[BUTTON_START] = (_event->type == SDL_KEYDOWN)?1:0;
                    if (Game_PButton[BUTTON_R] && Game_PButton[BUTTON_L])
                    {
                        if (!Game_PButton[BUTTON_START])
                        {
                            _event->type = SDL_USEREVENT;
                            _event->user.code = EC_PROGRAM_QUIT;
                            _event->user.data1 = NULL;
                            _event->user.data2 = NULL;
                        }
                        else
                        {
                            return 1;
                        }
                    }
                    else
                    {
                        if ( Game_ModButton )
                        {
                            return Action_button_R[BUTTON_START] (Game_PButton[BUTTON_START], Action_button_R_Key[BUTTON_START], _event);
                        }
                        else
                        {
                            return Action_button[BUTTON_START] (Game_PButton[BUTTON_START], Action_button_Key[BUTTON_START], _event);
                        }
                    }

                    break;
                case PANDORA_BUTTON_SELECT:
                    Game_PButton[BUTTON_SELECT] = (_event->type == SDL_KEYDOWN)?1:0;
                    if ( Game_ModButton )
                    {
                        return Action_button_R[BUTTON_SELECT] (Game_PButton[BUTTON_SELECT], Action_button_R_Key[BUTTON_SELECT], _event);
                    }
                    else
                    {
                        return Action_button[BUTTON_SELECT] (Game_PButton[BUTTON_SELECT], Action_button_Key[BUTTON_SELECT], _event);
                    }
                    break;
            }
            break;
        case SDL_USEREVENT:
            switch (_event->user.code)
            {
                case EC_INPUT_KEY:
                    _event->type = (int) _event->user.data2;
                    switch (_event->type)
                    {
                        case SDL_KEYDOWN:
                        case SDL_KEYUP:
                            _event->key.keysym.sym = (SDLKey) (uintptr_t) _event->user.data1;
                            _event->key.state = (_event->type == SDL_KEYUP)?SDL_RELEASED:SDL_PRESSED;
                            _event->key.keysym.mod = KMOD_NONE;
                            break;
                        case SDL_MOUSEBUTTONDOWN:
                        case SDL_MOUSEBUTTONUP:
                            _event->button.button = (uintptr_t) _event->user.data1;
                            _event->button.state = (_event->type == SDL_MOUSEBUTTONUP)?SDL_RELEASED:SDL_PRESSED;
                            break;
                    }
                    break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (_event->button.button == SDL_BUTTON_LEFT)
            {
                if (Game_RMBModifier)
                {
                    Game_RMBPressed = 1;
                    _event->button.button = SDL_BUTTON_RIGHT;
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (_event->button.button == SDL_BUTTON_LEFT)
            {
                if (Game_RMBPressed)
                {
                    Game_RMBPressed = 0;
                    _event->button.button = SDL_BUTTON_RIGHT;
                }
            }
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
    int deltax, deltay;
    SDL_Event event;

    deltax = 0;
    deltay = 0;

    if (Game_DKeyboard || Game_CurrentlyRotating)
    {
    }
    else
    {
        if (Game_PButton[BUTTON_UP]) deltay = -4;
        if (Game_PButton[BUTTON_DOWN]) deltay = 4;
        if (Game_PButton[BUTTON_LEFT]) deltax = -4;
        if (Game_PButton[BUTTON_RIGHT]) deltax = 4;
    }

    if (deltax != 0 || deltay != 0)
    {
        if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
        {
            if (Game_PButton[BUTTON_L])
            {
                deltax <<= 1;
                deltay <<= 1;
            }
        }
        else
        {
            if (Game_PButton[BUTTON_R])
            {
                deltax <<= 1;
                deltay <<= 1;
            }
        }


        event.type = SDL_USEREVENT;
        event.user.code = EC_MOUSE_MOVE;
        event.user.data1 = (void *) deltax;
        event.user.data2 = (void *) deltay;

        SDL_PushEvent(&event);
    }
}

