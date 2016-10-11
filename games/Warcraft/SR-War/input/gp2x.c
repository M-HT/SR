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
#include <string.h>

#if (SDL_MAJOR_VERSION > 1 || SDL_MAJOR_VERSION == 1 && (SDL_MINOR_VERSION > 2 || SDL_MINOR_VERSION == 2 && SDL_PATCHLEVEL >= 9 ) )
    #include <SDL/SDL_gp2x.h>
#endif


// definitions
// GAME_JOYSTICK != JOYSTICK_NONE
#define CURSOR_KEY_UP            (0)
#define CURSOR_KEY_DOWN          (2)
#define CURSOR_KEY_LEFT          (1)
#define CURSOR_KEY_RIGHT         (3)

// GAME_JOYSTICK = JOYSTICK_GP2X
#define GP2X_BUTTON_UP              (0)
#define GP2X_BUTTON_DOWN            (4)
#define GP2X_BUTTON_LEFT            (2)
#define GP2X_BUTTON_RIGHT           (6)
#define GP2X_BUTTON_UPLEFT          (1)
#define GP2X_BUTTON_UPRIGHT         (7)
#define GP2X_BUTTON_DOWNLEFT        (3)
#define GP2X_BUTTON_DOWNRIGHT       (5)
#define GP2X_BUTTON_CLICK           (18)
#define GP2X_BUTTON_A               (12)
#define GP2X_BUTTON_B               (13)
#define GP2X_BUTTON_X               (14)
#define GP2X_BUTTON_Y               (15)
#define GP2X_BUTTON_L               (10)
#define GP2X_BUTTON_R               (11)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_SELECT          (9)
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)

// variables
static uint32_t Game_SelectGroupOnMove;

// GAME_JOYSTICK != JOYSTICK_NONE
static volatile int Game_JButton[20];	/* joystick buttons */
static int CursorKeyState[4];			/* cursor keys state */

static volatile int Game_JKeyboard;		/* is joystick acting as keyboard ? */

// GAME_JOYSTICK = JOYSTICK_GP2X
static void (*Action_button_A)(int pressed, int key);	/* procedure for A button */
static void (*Action_button_B)(int pressed, int key);	/* procedure for B button */
static void (*Action_button_X)(int pressed, int key);	/* procedure for X button */
static void (*Action_button_Y)(int pressed, int key);	/* procedure for Y button */
static void (*Action_button_Select)(int pressed, int key);	/* procedure for Select button */
static void (*Action_button_VolUp)(int pressed, int key);	/* procedure for Volume+ button */
static void (*Action_button_VolDown)(int pressed, int key);	/* procedure for Volume- button */

static void (*Action_button_R_A)(int pressed, int key);	/* procedure for R+A button */
static void (*Action_button_R_B)(int pressed, int key);	/* procedure for R+B button */
static void (*Action_button_R_X)(int pressed, int key);	/* procedure for R+X button */
static void (*Action_button_R_Y)(int pressed, int key);	/* procedure for R+Y button */
static void (*Action_button_R_Select)(int pressed, int key);	/* procedure for R+Select button */
static void (*Action_button_R_VolUp)(int pressed, int key);		/* procedure for R+Volume+ button */
static void (*Action_button_R_VolDown)(int pressed, int key);	/* procedure for R+Volume- button */

static int Action_button_A_Key, Action_button_B_Key, Action_button_X_Key,
           Action_button_Y_Key, Action_button_Select_Key,
           Action_button_VolUp_Key, Action_button_VolDown_Key;

static int Action_button_R_A_Key, Action_button_R_B_Key, Action_button_R_X_Key,
           Action_button_R_Y_Key, Action_button_R_Select_Key,
           Action_button_R_VolUp_Key, Action_button_R_VolDown_Key;


// functions
void EmulateKey(int type, SDLKey key)
{
    SDL_Event pump_event;

    pump_event.type = type;
    pump_event.key.state = (type == SDL_KEYUP)?SDL_RELEASED:SDL_PRESSED;
    pump_event.key.keysym.sym = key;
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


// GAME_JOYSTICK != JOYSTICK_NONE
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

static void EmulateSelectGroup(void)
{
    Game_SelectGroupOnMove = 0;

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

static void ChangeCursorKeys(void)
{
    int NewState[4];

    NewState[CURSOR_KEY_UP] = 0;
    NewState[CURSOR_KEY_LEFT] = 0;
    NewState[CURSOR_KEY_DOWN] = 0;
    NewState[CURSOR_KEY_RIGHT] = 0;

    if (Game_JButton[GP2X_BUTTON_UP]) NewState[CURSOR_KEY_UP] = 1;
    if (Game_JButton[GP2X_BUTTON_DOWN]) NewState[CURSOR_KEY_DOWN] = 1;
    if (Game_JButton[GP2X_BUTTON_LEFT]) NewState[CURSOR_KEY_LEFT] = 1;
    if (Game_JButton[GP2X_BUTTON_RIGHT]) NewState[CURSOR_KEY_RIGHT] = 1;

    if (Game_JButton[GP2X_BUTTON_UPLEFT])
    {
        if ( !Game_JButton[GP2X_BUTTON_UP] && !Game_JButton[GP2X_BUTTON_LEFT] )
        {
            NewState[CURSOR_KEY_UP] = 1;
            NewState[CURSOR_KEY_LEFT] = 1;
        }
    }

    if (Game_JButton[GP2X_BUTTON_UPRIGHT])
    {
        if ( !Game_JButton[GP2X_BUTTON_UP] && !Game_JButton[GP2X_BUTTON_RIGHT] )
        {
            NewState[CURSOR_KEY_UP] = 1;
            NewState[CURSOR_KEY_RIGHT] = 1;
        }
    }

    if (Game_JButton[GP2X_BUTTON_DOWNLEFT])
    {
        if ( !Game_JButton[GP2X_BUTTON_DOWN] && !Game_JButton[GP2X_BUTTON_LEFT] )
        {
            NewState[CURSOR_KEY_DOWN] = 1;
            NewState[CURSOR_KEY_LEFT] = 1;
        }
    }

    if (Game_JButton[GP2X_BUTTON_DOWNRIGHT])
    {
        if ( !Game_JButton[GP2X_BUTTON_DOWN] && !Game_JButton[GP2X_BUTTON_RIGHT] )
        {
            NewState[CURSOR_KEY_DOWN] = 1;
            NewState[CURSOR_KEY_RIGHT] = 1;
        }
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

static void ReleaseCursorKeys(void)
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

static void Action_none(int pressed, int key)
{
}

// GAME_JOYSTICK = JOYSTICK_GP2X
static void Action_left_mouse_button(int pressed, int key)
{
    if (!Game_Paused)
    {
        EmulateMouseButton((pressed)?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);
    }
    else
    {
#if !defined(SDLK_ENTER)
    #define SDLK_ENTER SDLK_RETURN
#endif
        if (pressed == 0)
        {
            EmulateKey(SDL_KEYUP, SDLK_ENTER);
        }
    }
}

static void Action_right_mouse_button(int pressed, int key)
{
    if (!Game_Paused)
    {
        EmulateMouseButton((pressed)?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT);
    }
}

static void Action_key(int pressed, int key)
{
    if (!Game_Paused)
    {
        EmulateKey((pressed)?SDL_KEYDOWN:SDL_KEYUP, (SDLKey) key);
    }
}

static void Action_macro_key_left_mouse_button(int pressed, int key)
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
            EmulateKey(SDL_KEYUP, SDLK_ENTER);
        }
    }
}

static void Action_combo_left_mouse_button_select_group(int pressed, int key)
{
    if (!Game_Paused)
    {
        if (pressed)
        {
            if (Game_JButton[GP2X_BUTTON_UP] ||
                Game_JButton[GP2X_BUTTON_DOWN] ||
                Game_JButton[GP2X_BUTTON_LEFT] ||
                Game_JButton[GP2X_BUTTON_RIGHT] ||
                Game_JButton[GP2X_BUTTON_UPLEFT] ||
                Game_JButton[GP2X_BUTTON_UPRIGHT] ||
                Game_JButton[GP2X_BUTTON_DOWNLEFT] ||
                Game_JButton[GP2X_BUTTON_DOWNRIGHT]
               )
            {
                Game_SelectGroupOnMove = 0;

                EmulateKey(SDL_KEYDOWN, SDLK_LCTRL);

                EmulateDelay(30);

                EmulateMouseButton(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT);

                EmulateDelay(30);

                EmulateKey(SDL_KEYUP, SDLK_LCTRL);
            }
            else
            {
                EmulateMouseButton(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT);
                Game_SelectGroupOnMove = 1;
            }
        }
        else
        {
            Game_SelectGroupOnMove = 0;
            EmulateMouseButton(SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);
        }

    }
    else
    {
        if (pressed == 0)
        {
            EmulateKey(SDL_KEYUP, SDLK_ENTER);
        }
    }
}

//senquack - volume increase/decrease actions
static void Action_volume_increase(int pressed, int key)
{
    if (pressed)
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Warcraft: Begin increasing GP2X volume..\n");
#endif
        Game_VolumeDelta = 1;			// Signifies code in game loop should increase volume
    }
    else
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Warcraft: Cease increasing GP2X volume..\n");
#endif
        Game_VolumeDelta = 0;
    }
}

static void Action_volume_decrease(int pressed, int key)
{
    if (pressed)
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Warcraft: Begin decreasing GP2X volume..\n");
#endif
        Game_VolumeDelta = -1;			// Signifies code in game loop should decrease volume
    }
    else
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Warcraft: Cease decreasing GP2X volume..\n");
#endif
        Game_VolumeDelta = 0;
    }
}


void Init_Input(void)
{
    Game_Joystick = 1;

    Game_SelectGroupOnMove = 0;

// GAME_JOYSTICK != JOYSTICK_NONE
    Game_JKeyboard = 0;


// GAME_JOYSTICK = JOYSTICK_GP2X
    Action_button_A = &Action_key;
    Action_button_B = &Action_key;
    Action_button_X = &Action_right_mouse_button;
    Action_button_Y = &Action_left_mouse_button;
    Action_button_Select = &Action_key;
    Action_button_VolUp = &Action_volume_increase;
    Action_button_VolDown = &Action_volume_decrease;

    Action_button_A_Key = SDLK_m;
    Action_button_B_Key = SDLK_a;
    Action_button_X_Key = 0;
    Action_button_Y_Key = 0;
    Action_button_Select_Key = SDLK_F1;
    Action_button_VolUp_Key = 0;
    Action_button_VolDown_Key = 0;

    Action_button_R_A = &Action_key;
    Action_button_R_B = &Action_key;
    Action_button_R_X = &Action_key;
    Action_button_R_Y = &Action_key;
    Action_button_R_Select = &Action_key;
    Action_button_R_VolUp = &Action_key;
    Action_button_R_VolDown = &Action_key;

    Action_button_R_A_Key = SDLK_s;
    Action_button_R_B_Key = SDLK_BACKSPACE;
    Action_button_R_X_Key = SDLK_ESCAPE;
    Action_button_R_Y_Key = SDLK_RETURN;
    Action_button_R_Select_Key = SDLK_F15;
    Action_button_R_VolUp_Key = SDLK_F8;
    Action_button_R_VolDown_Key = SDLK_F5;
}

void Init_Input2(void)
{
}

int Config_Input(char *str, char *param)
{
    if ( strncasecmp(str, "Button_", 7) == 0 ) // str begins with "Button_"
    {
        // button mapping
        void (*Action_button)(int pressed, int key);
        int Action_button_Key;

        str += 7;
        Action_button = NULL;
        Action_button_Key = 0;

        // find the correct action
        if ( strncasecmp(param, "Action_", 7) == 0 ) // param begins with "Action_"
        {
            param += 7;

            if ( strncasecmp(param, "key_", 4) == 0 ) // param begins with "key_"
            {
                // key

                param += 4;

                if ( strcasecmp(param, "backspace") == 0 ) // param equals "backspace"
                {
                    // backspace key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_BACKSPACE;
                }
                else if ( strcasecmp(param, "ctrl") == 0 ) // param equals "ctrl"
                {
                    // ctrl key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_LCTRL;
                }
                else if ( strcasecmp(param, "enter") == 0 ) // param equals "enter"
                {
                    // enter key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_RETURN;
                }
                else if ( strcasecmp(param, "esc") == 0 ) // param equals "esc"
                {
                    // esc key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_ESCAPE;
                }
                else if ( strcasecmp(param, "shift") == 0 ) // param equals "shift"
                {
                    // shift key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_LSHIFT;
                }
                else if ( (param[0] == 'f' || param[0] == 'F') && param[1] != 0 && (param[2] == 0 || param[3] == 0))
                {
                    // f keys

                    if (param[2] == 0)
                    {
                        if (param[1] >= '1' && param[1] <= '9')
                        {
                            Action_button = &Action_key;
                            Action_button_Key = SDLK_F1 + param[1] - '1';
                        }
                    }
                    else if (param[2] == '0')
                    {
                        if (param[1] == '1')
                        {
                            Action_button = &Action_key;
                            Action_button_Key = SDLK_F10;
                        }
                    }
                }
                else if (param[0] != 0 && param[1] == 0)
                {
                    // other keys

                    if (param[0] >= 32 && param[0] <= 127)
                    {
                        Action_button = &Action_key;
                        if (param[0] >= 65 && param[0] <= 90)
                        {
                            Action_button_Key = param[0] + SDLK_a - 65;
                        }
                        else
                        {
                            Action_button_Key = param[0];
                        }
                    }
                }
                else if (param[0] == 0)
                {
                    // spacebar key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_SPACE;
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
                        Action_button = &Action_macro_key_left_mouse_button;
                        if (param[0] >= 65 && param[0] <= 90)
                        {
                            Action_button_Key = param[0] + SDLK_a - 65;
                        }
                        else
                        {
                            Action_button_Key = param[0];
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

                    Action_button = &Action_left_mouse_button;
                }
                else if ( strcasecmp(param, "right_button") == 0 ) // param equals "right_button"
                {
                    // right mouse button

                    Action_button = &Action_right_mouse_button;
                }

            }
            else if ( strcasecmp(param, "combo_mouse_left_button_select_group") == 0 ) // param equals "combo_mouse_left_button_select_group"
            {
                // combo left mouse button / select group
                Action_button = &Action_combo_left_mouse_button_select_group;
            }
            else if ( strcasecmp(param, "virtual_keyboard") == 0 ) // param equals "virtual_keyboard"
            {
                // virtual keyboard

                Action_button = &Action_key;
                Action_button_Key = SDLK_F15;
            }
            //senquack
            else if ( strcasecmp(param, "volume_increase") == 0 ) // param equals "volume_increase"
            {
                // Increase GP2X volume

                Action_button = &Action_volume_increase;
            }
            else if ( strcasecmp(param, "volume_decrease") == 0 ) // param equals "volume_decrease"
            {
                // Decrease GP2X volume

                Action_button = &Action_volume_decrease;
            }
            else if ( strcasecmp(param, "none") == 0 ) // param equals "none"
            {
                // no action

                Action_button = &Action_none;
            }
        }


        // find the correct button
        {
            if ( strcasecmp(str, "A") == 0 ) // str equals "A"
            {
                Action_button_A = (Action_button == NULL)?Action_none:Action_button;
                Action_button_A_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "B") == 0 ) // str equals "B"
            {
                Action_button_B = (Action_button == NULL)?Action_none:Action_button;
                Action_button_B_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "X") == 0 ) // str equals "X"
            {
                Action_button_X = (Action_button == NULL)?Action_none:Action_button;
                Action_button_X_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "Y") == 0 ) // str equals "Y"
            {
                Action_button_Y = (Action_button == NULL)?Action_none:Action_button;
                Action_button_Y_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "Select") == 0 ) // str equals "Select"
            {
                Action_button_Select = (Action_button == NULL)?Action_none:Action_button;
                Action_button_Select_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "Vol+") == 0 ) // str equals "Vol+"
            {
                Action_button_VolUp = (Action_button == NULL)?Action_none:Action_button;
                Action_button_VolUp_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "Vol-") == 0 ) // str equals "Vol-"
            {
                Action_button_VolDown = (Action_button == NULL)?Action_none:Action_button;
                Action_button_VolDown_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+A") == 0 ) // str equals "R+A"
            {
                Action_button_R_A = Action_button;
                Action_button_R_A_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+B") == 0 ) // str equals "R+B"
            {
                Action_button_R_B = Action_button;
                Action_button_R_B_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+X") == 0 ) // str equals "R+X"
            {
                Action_button_R_X = Action_button;
                Action_button_R_X_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+Y") == 0 ) // str equals "R+Y"
            {
                Action_button_R_Y = Action_button;
                Action_button_R_Y_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+Select") == 0 ) // str equals "R+Select"
            {
                Action_button_R_Select = Action_button;
                Action_button_R_Select_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+Vol+") == 0 ) // str equals "R+Vol+"
            {
                Action_button_R_VolUp = Action_button;
                Action_button_R_VolUp_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+Vol-") == 0 ) // str equals "R+Vol-"
            {
                Action_button_R_VolDown = Action_button;
                Action_button_R_VolDown_Key = Action_button_Key;
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
    Game_SelectGroupOnMove = 0;

// GAME_JOYSTICK != JOYSTICK_NONE
    memset((void *) &Game_JButton, 0, sizeof(Game_JButton));
    memset(&CursorKeyState, 0, sizeof(CursorKeyState));
}

int Handle_Input_Event(SDL_Event *_event)
{
    return 0;
}

int Handle_Input_Event2(SDL_Event *_event)
{
    SDL_Event event;

    event = *_event;
    switch(event.type)
    {
// GAME_JOYSTICK = JOYSTICK_GP2X
        case SDL_JOYBUTTONDOWN:
            switch (event.jbutton.button)
            {
                case GP2X_BUTTON_A:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_A != NULL)
                        {
                            Action_button_R_A(1, Action_button_R_A_Key);
                        }
                        else
                        {
                            Action_button_A(1, Action_button_A_Key);
                        }
                    }
                    else
                    {
                        Action_button_A(1, Action_button_A_Key);
                    }

                    break;
                case GP2X_BUTTON_B:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_B != NULL)
                        {
                            Action_button_R_B(1, Action_button_R_B_Key);
                        }
                        else
                        {
                            Action_button_B(1, Action_button_B_Key);
                        }
                    }
                    else
                    {
                        Action_button_B(1, Action_button_B_Key);
                    }

                    break;
                case GP2X_BUTTON_X:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_X != NULL)
                        {
                            Action_button_R_X(1, Action_button_R_X_Key);
                        }
                        else
                        {
                            Action_button_X(1, Action_button_X_Key);
                        }
                    }
                    else
                    {
                        Action_button_X(1, Action_button_X_Key);
                    }

                    break;
                case GP2X_BUTTON_Y:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_Y != NULL)
                        {
                            Action_button_R_Y(1, Action_button_R_Y_Key);
                        }
                        else
                        {
                            Action_button_Y(1, Action_button_Y_Key);
                        }
                    }
                    else
                    {
                        Action_button_Y(1, Action_button_Y_Key);
                    }

                    break;
                case GP2X_BUTTON_SELECT:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_Select != NULL)
                        {
                            Action_button_R_Select(1, Action_button_R_Select_Key);
                        }
                        else
                        {
                            Action_button_Select(1, Action_button_Select_Key);
                        }
                    }
                    else
                    {
                        Action_button_Select(1, Action_button_Select_Key);
                    }

                    break;
                case GP2X_BUTTON_VOLUP:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_VolUp != NULL)
                        {
                            Action_button_R_VolUp(1, Action_button_R_VolUp_Key);
                        }
                        else
                        {
                            Action_button_VolUp(1, Action_button_VolUp_Key);
                        }
                    }
                    else
                    {
                        Action_button_VolUp(1, Action_button_VolUp_Key);
                    }

                    break;
                case GP2X_BUTTON_VOLDOWN:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_VolDown != NULL)
                        {
                            Action_button_R_VolDown(1, Action_button_R_VolDown_Key);
                        }
                        else
                        {
                            Action_button_VolDown(1, Action_button_VolDown_Key);
                        }
                    }
                    else
                    {
                        Action_button_VolDown(1, Action_button_VolDown_Key);
                    }

                    break;
                case GP2X_BUTTON_UP:
                case GP2X_BUTTON_DOWN:
                case GP2X_BUTTON_LEFT:
                case GP2X_BUTTON_RIGHT:
                case GP2X_BUTTON_UPLEFT:
                case GP2X_BUTTON_UPRIGHT:
                case GP2X_BUTTON_DOWNLEFT:
                case GP2X_BUTTON_DOWNRIGHT:
                    if (!Game_Paused)
                    {
                        if (Game_SelectGroupOnMove) EmulateSelectGroup();

                        Game_JButton[event.jbutton.button] = 1;
                        if (Game_JKeyboard)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    break;
                case GP2X_BUTTON_R:
                    Game_JButton[GP2X_BUTTON_R] = 1;
                    break;
                case GP2X_BUTTON_L:
                    Game_JButton[GP2X_BUTTON_L] = 1;
                    if (!Game_Paused)
                    {
                        Game_JKeyboard = 1;
                        ChangeCursorKeys();
                    }
                    break;
            }
            break;
            // case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            switch (event.jbutton.button)
            {
                case GP2X_BUTTON_A:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_A != NULL)
                        {
                            Action_button_R_A(0, Action_button_R_A_Key);
                        }
                        else
                        {
                            Action_button_A(0, Action_button_A_Key);
                        }
                    }
                    else
                    {
                        Action_button_A(0, Action_button_A_Key);
                    }

                    break;
                case GP2X_BUTTON_B:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_B != NULL)
                        {
                            Action_button_R_B(0, Action_button_R_B_Key);
                        }
                        else
                        {
                            Action_button_B(0, Action_button_B_Key);
                        }
                    }
                    else
                    {
                        Action_button_B(0, Action_button_B_Key);
                    }

                    break;
                case GP2X_BUTTON_X:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_X != NULL)
                        {
                            Action_button_R_X(0, Action_button_R_X_Key);
                        }
                        else
                        {
                            Action_button_X(0, Action_button_X_Key);
                        }
                    }
                    else
                    {
                        Action_button_X(0, Action_button_X_Key);
                    }

                    break;
                case GP2X_BUTTON_Y:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_Y != NULL)
                        {
                            Action_button_R_Y(0, Action_button_R_Y_Key);
                        }
                        else
                        {
                            Action_button_Y(0, Action_button_Y_Key);
                        }
                    }
                    else
                    {
                        Action_button_Y(0, Action_button_Y_Key);
                    }

                    break;
                case GP2X_BUTTON_SELECT:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_Select != NULL)
                        {
                            Action_button_R_Select(0, Action_button_R_Select_Key);
                        }
                        else
                        {
                            Action_button_Select(0, Action_button_Select_Key);
                        }
                    }
                    else
                    {
                        Action_button_Select(0, Action_button_Select_Key);
                    }

                    break;
                case GP2X_BUTTON_VOLUP:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_VolUp != NULL)
                        {
                            Action_button_R_VolUp(0, Action_button_R_VolUp_Key);
                        }
                        else
                        {
                            Action_button_VolUp(0, Action_button_VolUp_Key);
                        }
                    }
                    else
                    {
                        Action_button_VolUp(0, Action_button_VolUp_Key);
                    }

                    break;
                case GP2X_BUTTON_VOLDOWN:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_VolDown != NULL)
                        {
                            Action_button_R_VolDown(0, Action_button_R_VolDown_Key);
                        }
                        else
                        {
                            Action_button_VolDown(0, Action_button_VolDown_Key);
                        }
                    }
                    else
                    {
                        Action_button_VolDown(0, Action_button_VolDown_Key);
                    }

                    break;
                case GP2X_BUTTON_START:
                    if (Game_JButton[GP2X_BUTTON_R] && Game_JButton[GP2X_BUTTON_L])
                    {
                        SDL_Event pump_event;

                        pump_event.type = SDL_USEREVENT;
                        pump_event.user.code = EC_PROGRAM_QUIT;
                        pump_event.user.data1 = NULL;
                        pump_event.user.data2 = NULL;

                        SDL_PushEvent(&pump_event);
                    }
                    else if (!Game_JButton[GP2X_BUTTON_R] && !Game_JButton[GP2X_BUTTON_L])
                    {
                        ReleaseCursorKeys();
                        EmulateKey(SDL_KEYUP, SDLK_PAUSE);
                    }
                    break;
                case GP2X_BUTTON_UP:
                case GP2X_BUTTON_DOWN:
                case GP2X_BUTTON_LEFT:
                case GP2X_BUTTON_RIGHT:
                case GP2X_BUTTON_UPLEFT:
                case GP2X_BUTTON_UPRIGHT:
                case GP2X_BUTTON_DOWNLEFT:
                case GP2X_BUTTON_DOWNRIGHT:
                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 0;
                        if (Game_JKeyboard)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        switch (event.jbutton.button)
                        {
                            case GP2X_BUTTON_UP:
                                EmulateKey(SDL_KEYUP, SDLK_UP);
                                break;
                            case GP2X_BUTTON_DOWN:
                                EmulateKey(SDL_KEYUP, SDLK_DOWN);
                                break;
                            case GP2X_BUTTON_LEFT:
                                EmulateKey(SDL_KEYUP, SDLK_LEFT);
                                break;
                            case GP2X_BUTTON_RIGHT:
                                EmulateKey(SDL_KEYUP, SDLK_RIGHT);
                                break;
                        }
                    }
                    break;
                case GP2X_BUTTON_R:
                    Game_JButton[GP2X_BUTTON_R] = 0;
                    break;
                case GP2X_BUTTON_L:
                    Game_JButton[GP2X_BUTTON_L] = 0;
                    if (!Game_Paused)
                    {
                        ReleaseCursorKeys();
                        Game_JKeyboard = 0;
                    }
                    break;
            }
            break;
            // case SDL_JOYBUTTONUP:
            default:
                return 0;
    } // switch(event.type)

    return 1;
}

void Handle_Timer_Input_Event(void)
{
    int deltax, deltay;
    SDL_Event event;

    deltax = 0;
    deltay = 0;

    if (Game_JKeyboard)
    {
    }
    else
    {
        if (Game_JButton[GP2X_BUTTON_UP]) deltay = -2;
        if (Game_JButton[GP2X_BUTTON_DOWN]) deltay = 2;
        if (Game_JButton[GP2X_BUTTON_LEFT]) deltax = -2;
        if (Game_JButton[GP2X_BUTTON_RIGHT]) deltax = 2;

        if (Game_JButton[GP2X_BUTTON_UPLEFT])
        {
            if (Game_JButton[GP2X_BUTTON_UP]) deltax = -1;
            else if (Game_JButton[GP2X_BUTTON_LEFT]) deltay = -1;
            else
            {
                deltax = -2;
                deltay = -2;
            }
        }

        if (Game_JButton[GP2X_BUTTON_UPRIGHT])
        {
            if (Game_JButton[GP2X_BUTTON_UP]) deltax = 1;
            else if (Game_JButton[GP2X_BUTTON_RIGHT]) deltay = -1;
            else
            {
                deltax = 2;
                deltay = -2;
            }
        }

        if (Game_JButton[GP2X_BUTTON_DOWNLEFT])
        {
            if (Game_JButton[GP2X_BUTTON_DOWN]) deltax = -1;
            else if (Game_JButton[GP2X_BUTTON_LEFT]) deltay = 1;
            else
            {
                deltax = -2;
                deltay = 2;
            }
        }

        if (Game_JButton[GP2X_BUTTON_DOWNRIGHT])
        {
            if (Game_JButton[GP2X_BUTTON_DOWN]) deltax = 1;
            else if (Game_JButton[GP2X_BUTTON_RIGHT]) deltay = 1;
            else
            {
                deltax = 2;
                deltay = 2;
            }
        }
    }

    if (deltax != 0 || deltay != 0)
    {
        if (Game_JButton[GP2X_BUTTON_R])
        {
            deltax <<= 1;
            deltay <<= 1;
        }


        event.type = SDL_USEREVENT;
        event.user.code = EC_MOUSE_MOVE;
        event.user.data1 = (void *) deltax;
        event.user.data2 = (void *) deltay;

        SDL_PushEvent(&event);
    }
}

