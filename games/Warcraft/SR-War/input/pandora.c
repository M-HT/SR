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

#define _FILE_OFFSET_BITS 64
#include "../Game_defs.h"
#include "../Game_vars.h"
#include <time.h>
#include <sys/stat.h>

// definitions
#define CURSOR_KEY_UP            (0)
#define CURSOR_KEY_DOWN          (2)
#define CURSOR_KEY_LEFT          (1)
#define CURSOR_KEY_RIGHT         (3)

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


#define SELECT_GROUP_INACTIVE 0
#define SELECT_GROUP_POSSIBLE 1
#define SELECT_GROUP_ACTIVE 2
#define SELECT_GROUP_STOPPED 3
#define SELECT_GROUP_IMPOSSIBLE 4

// variables
static int Game_MouseHelper, Game_HelperMouseMiddleButton, Game_HelperMouseRightButton;
static uint32_t Game_SelectGroupOnMove;
static int Game_SelectGroupX, Game_SelectGroupY, Game_SelectGroupTreshold;

static volatile int Game_PButton[BUTTON_LAST];	/* pandora buttons */
static int CursorKeyState[4];			/* cursor keys state */
static int Game_ModButton;				/* modifier key */

static volatile int Game_DKeyboard;		/* is dpad acting as keyboard ? */


static int Game_InputMode;

static Action_function Action_button[BUTTON_LAST], Action_button_R[BUTTON_LAST];

static int Action_button_Key[BUTTON_LAST], Action_button_R_Key[BUTTON_LAST];


// functions
void EmulateKey(int type, int key)
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

static void ChangeCursorKeys()
{
    int NewState[4];

    NewState[CURSOR_KEY_UP] = 0;
    NewState[CURSOR_KEY_LEFT] = 0;
    NewState[CURSOR_KEY_DOWN] = 0;
    NewState[CURSOR_KEY_RIGHT] = 0;

    //senquack
    if (Game_InputMode == GAME_TOUCHSCREEN_ABXY)
    {
        if (Game_PButton[BUTTON_Y]) NewState[CURSOR_KEY_UP] = 1;
        if (Game_PButton[BUTTON_X]) NewState[CURSOR_KEY_DOWN] = 1;
        if (Game_PButton[BUTTON_A]) NewState[CURSOR_KEY_LEFT] = 1;
        if (Game_PButton[BUTTON_B]) NewState[CURSOR_KEY_RIGHT] = 1;
    }
    else
    {
        if (Game_PButton[BUTTON_UP]) NewState[CURSOR_KEY_UP] = 1;
        if (Game_PButton[BUTTON_DOWN]) NewState[CURSOR_KEY_DOWN] = 1;
        if (Game_PButton[BUTTON_LEFT]) NewState[CURSOR_KEY_LEFT] = 1;
        if (Game_PButton[BUTTON_RIGHT]) NewState[CURSOR_KEY_RIGHT] = 1;
    }


    if ( (NewState[CURSOR_KEY_UP] == 0) != (CursorKeyState[CURSOR_KEY_UP] == 0) )
    {
        CursorKeyState[CURSOR_KEY_UP] = NewState[CURSOR_KEY_UP];
        EmulateKey( (NewState[CURSOR_KEY_UP])?SDL_KEYDOWN:SDL_KEYUP, SDLK_UP);
    }

    if ( (NewState[CURSOR_KEY_LEFT] == 0) != (CursorKeyState[CURSOR_KEY_LEFT] == 0) )
    {
        CursorKeyState[CURSOR_KEY_LEFT] = NewState[CURSOR_KEY_LEFT];
        EmulateKey( (NewState[CURSOR_KEY_LEFT])?SDL_KEYDOWN:SDL_KEYUP, SDLK_LEFT);
    }

    if ( (NewState[CURSOR_KEY_DOWN] == 0) != (CursorKeyState[CURSOR_KEY_DOWN] == 0) )
    {
        CursorKeyState[CURSOR_KEY_DOWN] = NewState[CURSOR_KEY_DOWN];
        EmulateKey( (NewState[CURSOR_KEY_DOWN])?SDL_KEYDOWN:SDL_KEYUP, SDLK_DOWN);
    }

    if ( (NewState[CURSOR_KEY_RIGHT] == 0) != (CursorKeyState[CURSOR_KEY_RIGHT] == 0) )
    {
        CursorKeyState[CURSOR_KEY_RIGHT] = NewState[CURSOR_KEY_RIGHT];
        EmulateKey( (NewState[CURSOR_KEY_RIGHT])?SDL_KEYDOWN:SDL_KEYUP, SDLK_RIGHT);
    }
}

static void ReleaseCursorKeys()
{
    if (CursorKeyState[CURSOR_KEY_UP])
    {
        CursorKeyState[CURSOR_KEY_UP] = 0;
        EmulateKey(SDL_KEYUP, SDLK_UP);
    }

    if (CursorKeyState[CURSOR_KEY_LEFT])
    {
        CursorKeyState[CURSOR_KEY_LEFT] = 0;
        EmulateKey(SDL_KEYUP, SDLK_LEFT);
    }

    if (CursorKeyState[CURSOR_KEY_DOWN])
    {
        CursorKeyState[CURSOR_KEY_DOWN] = 0;
        EmulateKey(SDL_KEYUP, SDLK_DOWN);
    }

    if (CursorKeyState[CURSOR_KEY_RIGHT])
    {
        CursorKeyState[CURSOR_KEY_RIGHT] = 0;
        EmulateKey(SDL_KEYUP, SDLK_RIGHT);
    }
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

static int Action_macro_key_left_mouse_button(int pressed, int key, SDL_Event *event)
{
    if (!Game_Paused)
    {
        if (pressed)
        {
            EmulateKey(SDL_KEYDOWN, (SDLKey) key);

            EmulateDelay(40);

            EmulateKey(SDL_KEYUP, (SDLKey) key);

            EmulateDelay(40);
        }
        EmulateMouseButton((pressed)?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);
    }
    else
    {
        if (pressed == 0)
        {
            event->key.keysym.sym = SDLK_ENTER;
            return 0;
        }
    }
    return 1;
}

static int Action_combo_left_mouse_button_select_group(int pressed, int key, SDL_Event *event)
{
    if (!Game_Paused)
    {
        if (pressed)
        {
            if ((Game_InputMode == GAME_KEYBOARD_DPAD) &&
                (Game_PButton[BUTTON_UP] ||
                 Game_PButton[BUTTON_DOWN] ||
                 Game_PButton[BUTTON_LEFT] ||
                 Game_PButton[BUTTON_RIGHT]
                )
               )
            {
                Game_SelectGroupOnMove = SELECT_GROUP_ACTIVE;

                EmulateKey(SDL_KEYDOWN, SDLK_LCTRL);

                EmulateDelay(30);

                EmulateMouseButton(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT);

                EmulateDelay(30);

                EmulateKey(SDL_KEYUP, SDLK_LCTRL);
            }
            else
            {
                EmulateMouseButton(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT);
                Game_SelectGroupOnMove = SELECT_GROUP_POSSIBLE;
            }
        }
        else
        {
            Game_SelectGroupOnMove = SELECT_GROUP_INACTIVE;
            EmulateMouseButton(SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);
        }

    }
    else
    {
        if (pressed == 0)
        {
            event->key.keysym.sym = SDLK_ENTER;
            return 0;
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

    Game_MouseHelper = 0;

    Game_HelperMouseMiddleButton = 0;
    Game_HelperMouseRightButton = 0;

    Game_SelectGroupOnMove = SELECT_GROUP_INACTIVE;
    Game_SelectGroupTreshold = 6;

    Game_InputMode = GAME_TOUCHSCREEN_DPAD;
    Game_ModButton = 0;
    Game_DKeyboard = 0;

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

        statbuf.st_mtime = 0;
        localtime(&(statbuf.st_mtime));
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
    int num_int;

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
        else if ( strcasecmp(str, "MouseHelper") == 0)	// str equals "MouseHelper"
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
                else if ( (param[0] == 'f' || param[0] == 'F') && param[1] != 0 && (param[2] == 0 || param[3] == 0))
                {
                    // f keys

                    if (param[2] == 0)
                    {
                        if (param[1] >= '1' && param[1] <= '9')
                        {
                            Action_current = &Action_key;
                            Action_current_Key = SDLK_F1 + param[1] - '1';
                        }
                    }
                    else if (param[2] == '0')
                    {
                        if (param[1] == '1')
                        {
                            Action_current = &Action_key;
                            Action_current_Key = SDLK_F10;
                        }
                    }
                }
                else if (param[0] != 0 && param[1] == 0)
                {
                    // other keys

                    if (param[0] >= 32 && param[0] <= 127)
                    {
                        Action_current = &Action_key;
                        if (param[0] >= 65 && param[0] <= 90)
                        {
                            Action_current_Key = param[0] + SDLK_a - 65;
                        }
                        else
                        {
                            Action_current_Key = param[0];
                        }
                    }
                }
            }
            else if ( strncasecmp(param, "macro_key_", 10) == 0 ) // param begins with "macro_key_"
            {
                // macro keys

                param += 10;

                if ( strcasecmp(param+1, "_mouse_left_button") == 0 ) // param+1 equals "_mouse_left_button"
                {
                    // key click followed by mouse left button

                    if (param[0] >= 32 && param[0] <= 127)
                    {
                        Action_current = &Action_macro_key_left_mouse_button;
                        if (param[0] >= 65 && param[0] <= 90)
                        {
                            Action_current_Key = param[0] + SDLK_a - 65;
                        }
                        else
                        {
                            Action_current_Key = param[0];
                        }
                    }
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
            else if ( strcasecmp(param, "combo_mouse_left_button_select_group") == 0 ) // param equals "combo_mouse_left_button_select_group"
            {
                // combo left mouse button / select group
                Action_current = &Action_combo_left_mouse_button_select_group;
            }
            else if ( strcasecmp(param, "virtual_keyboard") == 0 ) // param equals "virtual_keyboard"
            {
                // virtual keyboard

                Action_current = &Action_key;
                Action_current_Key = SDLK_F15;
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
    Game_HelperMouseMiddleButton = 0;
    Game_HelperMouseRightButton = 0;
    Game_SelectGroupOnMove = SELECT_GROUP_INACTIVE;

// GAME_JOYSTICK != JOYSTICK_NONE
    memset((void *) &Game_PButton, 0, sizeof(Game_PButton));
    memset(&CursorKeyState, 0, sizeof(CursorKeyState));
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
                            if (Game_InputMode == GAME_KEYBOARD_DPAD)
                            {
                                if ((Game_SelectGroupOnMove == SELECT_GROUP_POSSIBLE) && Game_PButton[BUTTON_UP]) EmulateSelectGroup();
                            }

                            if (Game_DKeyboard)
                            {
                                ChangeCursorKeys();
                            }
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
                            if (Game_InputMode == GAME_KEYBOARD_DPAD)
                            {
                                if ((Game_SelectGroupOnMove == SELECT_GROUP_POSSIBLE) && Game_PButton[BUTTON_DOWN]) EmulateSelectGroup();
                            }

                            if (Game_DKeyboard)
                            {
                                ChangeCursorKeys();
                            }
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
                            if (Game_InputMode == GAME_KEYBOARD_DPAD)
                            {
                                if ((Game_SelectGroupOnMove == SELECT_GROUP_POSSIBLE) && Game_PButton[BUTTON_LEFT]) EmulateSelectGroup();
                            }

                            if (Game_DKeyboard)
                            {
                                ChangeCursorKeys();
                            }
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
                            if (Game_InputMode == GAME_KEYBOARD_DPAD)
                            {
                                if ((Game_SelectGroupOnMove == SELECT_GROUP_POSSIBLE) && Game_PButton[BUTTON_RIGHT]) EmulateSelectGroup();
                            }

                            if (Game_DKeyboard)
                            {
                                ChangeCursorKeys();
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
                                ChangeCursorKeys();
                            }
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
                                ChangeCursorKeys();
                            }
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
                                ChangeCursorKeys();
                            }
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
                                ChangeCursorKeys();
                            }
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
                        if (!Game_Paused)
                        {
                            if (Game_PButton[BUTTON_L])
                            {
                                Game_DKeyboard = 1;
                                ChangeCursorKeys();
                            }
                            else
                            {
                                ReleaseCursorKeys();
                                Game_DKeyboard = 0;
                            }
                        }
                        return 1;
                    }
                    else
                    {
                        _event->key.keysym.sym = SDLK_RSHIFT;
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
                        _event->key.keysym.sym = SDLK_RSHIFT;
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
                    _event->key.keysym.sym = (SDLKey) (uintptr_t) _event->user.data1;
                    _event->key.state = (_event->type == SDL_KEYUP)?SDL_RELEASED:SDL_PRESSED;
                    _event->key.keysym.mod = KMOD_NONE;
                    break;
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
            switch (_event->button.button)
            {
                case SDL_BUTTON_LEFT:
                    if (!Game_Paused)
                    {
                        if (Game_SelectGroupOnMove == SELECT_GROUP_STOPPED)
                        {
                            Game_SelectGroupOnMove = SELECT_GROUP_IMPOSSIBLE;
                        }
                        else if (((Game_InputMode == GAME_TOUCHSCREEN_DPAD) && !Game_PButton[BUTTON_L]) ||
                                 ((Game_InputMode == GAME_TOUCHSCREEN_ABXY) && !Game_PButton[BUTTON_R]) ||
                                 ((Game_InputMode == GAME_KEYBOARD_DPAD) && Game_MouseHelper)
                        )
                        {
                            Game_SelectGroupOnMove = SELECT_GROUP_POSSIBLE;
                            Game_SelectGroupX = _event->motion.x;
                            Game_SelectGroupY = _event->motion.y;
                        }
                    }
                    break;
                case SDL_BUTTON_MIDDLE:
                    if (Game_MouseHelper && (Game_InputMode == GAME_KEYBOARD_DPAD) && !Game_Paused)
                    {
                        Game_HelperMouseMiddleButton = 1;
                        EmulateRepairStop();
                        return 1;
                    }
                    break;
                case SDL_BUTTON_RIGHT:
                    if (Game_MouseHelper && (Game_InputMode == GAME_KEYBOARD_DPAD) && !Game_Paused)
                    {
                        Game_HelperMouseRightButton = 1;
                        EmulateHarvestTransportAttackMoveWall();
                        return 1;
                    }
            }
            break;
        case SDL_MOUSEBUTTONUP:
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
                    if (Game_MouseHelper || (Game_InputMode != GAME_KEYBOARD_DPAD))
                    {
                        if ((Game_SelectGroupOnMove == SELECT_GROUP_ACTIVE) && !Game_Paused)
                        {
                            Game_SelectGroupOnMove = SELECT_GROUP_STOPPED;
                        }
                        else
                        {
                            Game_SelectGroupOnMove = SELECT_GROUP_INACTIVE;
                        }
                    }
                    break;
            }
            break;
        case SDL_MOUSEMOTION:
            if (Game_SelectGroupOnMove == SELECT_GROUP_POSSIBLE)
            {
                if ((abs(Game_SelectGroupX - _event->motion.x) > Game_SelectGroupTreshold) ||
                    (abs(Game_SelectGroupY - _event->motion.y) > Game_SelectGroupTreshold)
                   )
                {
                    EmulateSelectGroup();
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

    if (Game_DKeyboard)
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

