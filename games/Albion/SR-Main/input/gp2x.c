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
#define CTRL_KEY                 (4)
#define ALT_KEY                  (5)

// GAME_JOYSTICK = JOYSTICK_GP2X
#define GAME_CURSORBUTTONS_DPAD	(0)
#define GAME_CURSORBUTTONS_ABXY	(1)

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
// GAME_JOYSTICK != JOYSTICK_NONE
static volatile int Game_JButton[20];	/* joystick buttons */
static int CursorKeyState[6];			/* cursor keys state */
static int Game_AltButton;				/* alt key */
//senquack - control button is now handled similary to alt key:
static int Game_CtrlButton;				/* control key */

static volatile int Game_JKeyboard;		/* is joystick acting as keyboard ? */

// GAME_JOYSTICK = JOYSTICK_GP2X
//senquack
static int Game_UsingTouchscreen;        /* Playing on GP2X with touchscreen? */
static int Game_CursorButtons;           /* Will cursor buttons be controlled by
                                     GP2X's stick/dpad or by the
                                     a/b/x/y buttons? */


//senquack - up/down/left/right now remappable: (for left-handed F200 users)
static void (*Action_button_Up)(int pressed, int key);	/* procedure for up button */
static void (*Action_button_Down)(int pressed, int key);	/* procedure for down button */
static void (*Action_button_Left)(int pressed, int key);	/* procedure for left button */
static void (*Action_button_Right)(int pressed, int key);	/* procedure for right button */
static void (*Action_button_A)(int pressed, int key);	/* procedure for A button */
static void (*Action_button_B)(int pressed, int key);	/* procedure for B button */
static void (*Action_button_X)(int pressed, int key);	/* procedure for X button */
static void (*Action_button_Y)(int pressed, int key);	/* procedure for Y button */
//senquack - triggers now mappable:
static void (*Action_button_L)(int pressed, int key);	/* procedure for L trigger */
static void (*Action_button_R)(int pressed, int key);	/* procedure for R trigger */
static void (*Action_button_Select)(int pressed, int key);	/* procedure for Select button */
//senquack - Start now mappable:
static void (*Action_button_Start)(int pressed, int key);	/* procedure for Start button */
static void (*Action_button_VolUp)(int pressed, int key);	/* procedure for Volume+ button */
static void (*Action_button_VolDown)(int pressed, int key);	/* procedure for Volume- button */

static void (*Action_button_R_A)(int pressed, int key);	/* procedure for R+A button */
static void (*Action_button_R_B)(int pressed, int key);	/* procedure for R+B button */
static void (*Action_button_R_X)(int pressed, int key);	/* procedure for R+X button */
static void (*Action_button_R_Y)(int pressed, int key);	/* procedure for R+Y button */
static void (*Action_button_R_Select)(int pressed, int key);	/* procedure for R+Select button */
static void (*Action_button_R_VolUp)(int pressed, int key);		/* procedure for R+Volume+ button */
static void (*Action_button_R_VolDown)(int pressed, int key);	/* procedure for R+Volume- button */

//senquack
static void (*Action_button_L_VolUp)(int pressed, int key);		/* procedure for L+Volume+ button */
static void (*Action_button_L_VolDown)(int pressed, int key);	/* procedure for L+Volume- button */

//senquack - modified:
static int Action_button_A_Key, Action_button_B_Key, Action_button_X_Key,
           Action_button_Y_Key, Action_button_Select_Key, Action_button_Start_Key,
           Action_button_L_Key, Action_button_R_Key,
           Action_button_VolUp_Key, Action_button_VolDown_Key,
           Action_button_Up_Key, Action_button_Down_Key,
           Action_button_Left_Key, Action_button_Right_Key;

//senquack - modified:
static int Action_button_R_A_Key, Action_button_R_B_Key, Action_button_R_X_Key,
           Action_button_R_Y_Key, Action_button_R_Select_Key,
           Action_button_L_VolUp_Key, Action_button_L_VolDown_Key,
           Action_button_R_VolUp_Key, Action_button_R_VolDown_Key;


// functions
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

static void ChangeCursorKeys()
{
    int NewState[6];

    NewState[CURSOR_KEY_UP] = 0;
    NewState[CURSOR_KEY_LEFT] = 0;
    NewState[CURSOR_KEY_DOWN] = 0;
    NewState[CURSOR_KEY_RIGHT] = 0;
    NewState[CTRL_KEY] = 0;
    NewState[ALT_KEY] = 0;

    //senquack
    if (Game_CursorButtons == GAME_CURSORBUTTONS_ABXY)
    {
        if (Game_JButton[GP2X_BUTTON_Y]) NewState[CURSOR_KEY_UP] = 1;
        if (Game_JButton[GP2X_BUTTON_X]) NewState[CURSOR_KEY_DOWN] = 1;
        if (Game_JButton[GP2X_BUTTON_A]) NewState[CURSOR_KEY_LEFT] = 1;
        if (Game_JButton[GP2X_BUTTON_B]) NewState[CURSOR_KEY_RIGHT] = 1;
    }
    else
    {
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
    }

    //senquack - control key now handled similarly to alt key:
//    if (Game_JButton[GP2X_BUTTON_R]) NewState[CTRL_KEY] = 1;
    if (Game_CtrlButton) NewState[CTRL_KEY] = 1;

    if (Game_AltButton) NewState[ALT_KEY] = 1;


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

    if ( (NewState[CTRL_KEY] == 0) != (CursorKeyState[CTRL_KEY] == 0) )
    {
        CursorKeyState[CTRL_KEY] = NewState[CTRL_KEY];
        EmulateKey( (NewState[CTRL_KEY])?SDL_KEYDOWN:SDL_KEYUP, SDLK_LCTRL);
    }

    if ( (NewState[ALT_KEY] == 0) != (CursorKeyState[ALT_KEY] == 0) )
    {
        CursorKeyState[ALT_KEY] = NewState[ALT_KEY];
        EmulateKey( (NewState[ALT_KEY])?SDL_KEYDOWN:SDL_KEYUP, SDLK_LALT);
    }
}

static void ReleaseCursorKeys()
{
    if (CursorKeyState[ALT_KEY])
    {
        CursorKeyState[ALT_KEY] = 0;
        EmulateKey(SDL_KEYUP, SDLK_LALT);
    }

    if (CursorKeyState[CTRL_KEY])
    {
        CursorKeyState[CTRL_KEY] = 0;
        EmulateKey(SDL_KEYUP, SDLK_LCTRL);
    }

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

//senquack - new touchscreen functionality:
static void Action_right_mouse_button(int pressed, int key)
{
    //senquack - keep track of mouse button state for new touchscreen functionality:
    Game_RMBActive = pressed;

    if (!Game_Paused)
    {
        if (!Game_TouchscreenButtonEvents)
        {
            EmulateMouseButton((pressed)?SDL_MOUSEBUTTONDOWN:SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT);
        }
        else
        {
            // We are using GP2X's touchscreen and the game is configured to
            // register LMB presses when stylus is pressed and only register
            // RMB presses when the button for RMB is held and the stylus is
            // then pressed
            // Only emulate button up events:
        }

    }
}

static void Action_key_alt(int pressed, int key)
{
    Game_AltButton = pressed;

    if (!Game_Paused)
    {
        if (Game_JKeyboard)
        {
            ChangeCursorKeys();
        }
    }
}

//senquack - control key now handled similarly to alt key:
static void Action_key_ctrl(int pressed, int key)
{
    Game_CtrlButton = pressed;

    if (!Game_Paused)
    {
        if (Game_JKeyboard)
        {
            ChangeCursorKeys();
        }
    }
}

static void Action_key(int pressed, int key)
{
    if (!Game_Paused)
    {
        EmulateKey((pressed)?SDL_KEYDOWN:SDL_KEYUP, key);
    }
}

//senquack - volume increase/decrease actions
static void Action_volume_increase(int pressed, int key)
{
    if (pressed)
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Albion: Begin increasing GP2X volume..\n");
#endif
        Game_VolumeDelta = 1;			// Signifies code in game loop should increase volume
    }
    else
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Albion: Cease increasing GP2X volume..\n");
#endif
        Game_VolumeDelta = 0;
    }
}

static void Action_volume_decrease(int pressed, int key)
{
    if (pressed)
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Albion: Begin decreasing GP2X volume..\n");
#endif
        Game_VolumeDelta = -1;			// Signifies code in game loop should decrease volume
    }
    else
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Albion: Cease decreasing GP2X volume..\n");
#endif
        Game_VolumeDelta = 0;
    }
}

// GP2X users can toggle screen scaling for better reading of long text dialogs
static void Action_toggle_scaling(int pressed, int key)
{
    if (pressed == 0)
    {
        Display_ChangeMode = 1;
    }
}


void Init_Input(void)
{
    Game_Joystick = 1;

// GAME_JOYSTICK != JOYSTICK_NONE
    Game_AltButton = 0;
    Game_JKeyboard = 0;


// GAME_JOYSTICK = JOYSTICK_GP2X
    //senquack
    Game_TouchscreenButtonEvents = 0;
    Game_RMBActive = 0;
    Game_UsingTouchscreen = 0;
    Game_CursorButtons = GAME_CURSORBUTTONS_DPAD;


    //senquack - not remappable when not using the touchscreen (default config)
    Action_button_Up = &Action_none;
    Action_button_Down = &Action_none;
    Action_button_Left = &Action_none;
    Action_button_Right = &Action_none;

    //senquack - changed button A to switching between scaled/unscaled display modes,
    //           and made R+A control backspace instead
    Action_button_A = &Action_toggle_scaling;
    Action_button_B = &Action_left_mouse_button;
    Action_button_X = &Action_right_mouse_button;
    Action_button_Y = &Action_key;
    Action_button_Select = &Action_key;
    Action_button_Start = &Action_key;
    //senquack - triggers now mappable:
    Action_button_L = &Action_none;
    Action_button_R = &Action_key_ctrl; /* Default to control key as always */
    Action_button_VolUp = &Action_volume_increase;
    Action_button_VolDown = &Action_volume_decrease;

    //senquack - added these:
    Action_button_Start_Key = 0;
    Action_button_Up_Key = 0;
    Action_button_Down_Key = 0;
    Action_button_Left_Key = 0;
    Action_button_Right_Key = 0;
    Action_button_L_Key = 0;
    Action_button_R_Key = 0;

    //    senquack
//    Action_button_A_Key = SDLK_BACKSPACE;
    Action_button_A_Key = 0;
    Action_button_B_Key = 0;
    Action_button_X_Key = 0;
    Action_button_Y_Key = SDLK_y;
    Action_button_Select_Key = SDLK_TAB;
    Action_button_Start_Key = SDLK_ESCAPE;
    Action_button_VolUp_Key = 0;
    Action_button_VolDown_Key = 0;

    //    senquack
//    Action_button_R_A = NULL;
    Action_button_R_A = &Action_key;
    Action_button_R_B = NULL;
    Action_button_R_X = NULL;
    Action_button_R_Y = NULL;
    Action_button_R_Select = &Action_key;
    Action_button_R_VolUp = NULL;
    Action_button_R_VolDown = NULL;
    //senquack
    Action_button_L_VolUp = NULL;
    Action_button_L_VolDown = NULL;

    //senquack
//    Action_button_R_A_Key = 0;
    Action_button_R_A_Key = SDLK_BACKSPACE;
    Action_button_R_B_Key = 0;
    Action_button_R_X_Key = 0;
    Action_button_R_Y_Key = 0;
    Action_button_R_Select_Key = SDLK_F15;
    Action_button_R_VolUp_Key = 0;
    Action_button_R_VolDown_Key = 0;
    //senquack
    Action_button_L_VolUp_Key = 0;
    Action_button_L_VolDown_Key = 0;
}

void Init_Input2(void)
{
// GAME_JOYSTICK = JOYSTICK_GP2X
// senquack
#if defined(SDL_GP2X__H)

    if (SDL_GP2X_MouseType() == 2)
    {
        // When running on an F200, but using the F100 configuration file that disables
        // the touchscreen, disable the touchscreen entirely:
        if (!Game_UsingTouchscreen)
        {
            SDL_GP2X_TouchpadMouseMotionEvents(0);
            Game_TouchscreenButtonEvents = 0;
        }

        if (!Game_TouchscreenButtonEvents)
        {
            // Running on an F200 unit and game is configured to only register mouse events when
            // buttons are pressed on GP2X, and stylus simply moves the cursor around.

            // New SDL option allows us to block SDL from generating mouse button events
            // when touchscreen is pressed.  When using touchscreen, we want button events
            // controlled only by GP2X buttons.  If user has USB mouse connected this will not
            // be called.
            SDL_GP2X_TouchpadMouseButtonEvents(0);
        }
    }
    else
    {
        Game_UsingTouchscreen = 0; 	/* When not on F200s, always disable this */
        Game_TouchscreenButtonEvents = 0; /* Same here */
    }

    if (Game_UsingTouchscreen)
    {
        // When using the touchscreen, cursor key emulation is always on (not modal):
        Game_JKeyboard = 1;
    }

#endif

}

int Config_Input(char *str, char *param)
{
//senquack
    if ( strcasecmp(str, "Touchscreen") == 0)	// str equals "Touchscreen"
    {
        if ( strcasecmp(param, "on") == 0)	// param equals "on"
        {
            // Use GP2X touchscreen if any is available
            Game_UsingTouchscreen = 1;
        }
        else if ( strcasecmp(param, "off") == 0) // param equals "off"
        {
            // Don't use GP2X touchscreen if any is available
            Game_UsingTouchscreen = 0;	// Default
        }
    }
    else if ( strcasecmp(str, "Stylus_Clicks") == 0)	// str equals "Stylus_Clicks"
    {
        if ( strcasecmp(param, "on") == 0)	// param equals "on"
        {
            // When on, this causes LMB clicks to be registered when stylus is pressed
            // (and RMB clicks to be registed when button for RMB is held and stylus is pressed)
            Game_TouchscreenButtonEvents = 1;
        }
        else if ( strcasecmp(param, "off") == 0) // param equals "off"
        {
            // When off, the stylus only moves the cursor around the screen
            Game_TouchscreenButtonEvents = 0;
        }
    }
    else if ( strcasecmp(str, "Cursor_Buttons") == 0)	// str equals "Cursor_Buttons"
    {
        if ( strcasecmp(param, "dpad") == 0)	// param equals "dpad"
        {
            // Cursor keys will be controlled by stick/dpad
            Game_CursorButtons = GAME_CURSORBUTTONS_DPAD;
        }
        else if ( strcasecmp(param, "abxy") == 0) // param equals "abxy"
        {
            // Cursor keys will be controlled by ABXY buttons
            Game_CursorButtons = GAME_CURSORBUTTONS_ABXY;
        }
    }
    else if ( strncasecmp(str, "Button_", 7) == 0 ) // str begins with "Button_"
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

                if ( strcasecmp(param, "alt") == 0 ) // param equals "alt"
                {
                    // alt key

                    Action_button = &Action_key_alt;
                }
                //senquack - control key now handled similarly to alt key:
                if ( strcasecmp(param, "ctrl") == 0 ) // param equals "ctrl"
                {
                    // ctrl key

                    Action_button = &Action_key_ctrl;
                }
                else if ( strcasecmp(param, "backspace") == 0 ) // param equals "backspace"
                {
                    // backspace key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_BACKSPACE;
                }
                else if ( strcasecmp(param, "esc") == 0 ) // param equals "esc"
                {
                    // esc key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_ESCAPE;
                }
                else if ( strcasecmp(param, "pagedown") == 0 ) // param equals "pagedown"
                {
                    // pagedown key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_PAGEDOWN;
                }
                else if ( strcasecmp(param, "pageup") == 0 ) // param equals "pageup"
                {
                    // pageup key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_PAGEUP;
                }
                else if ( strcasecmp(param, "tab") == 0 ) // param equals "tab"
                {
                    // tab key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_TAB;
                }
                else if ( (param[0] == 'f' || param[0] == 'F') && param[1] != 0 && param[2] == 0)
                {
                    // f keys

                    if (param[1] >= '1' && param[1] <= '6')
                    {
                        Action_button = &Action_key;
                        Action_button_Key = SDLK_F1 + param[1] - '1';
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
            //senquack - screen scaling can now be toggled on the fly:
            else if ( strcasecmp(param, "toggle_scaling") == 0 ) // param equals "toggle_scaling"
            {
                // Toggle scaling

                Action_button = &Action_toggle_scaling;
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
            //senquack
            else if ( strcasecmp(str, "Start") == 0 ) // str equals "Start"
            {
                Action_button_Start = (Action_button == NULL)?Action_none:Action_button;
                Action_button_Start_Key = Action_button_Key;
            }
            //senquack - triggers now mappable
            else if ( strcasecmp(str, "L") == 0 ) // str equals "L"
            {
                Action_button_L = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_Key = Action_button_Key;
            }
            //senquack - triggers now mappable
            else if ( strcasecmp(str, "R") == 0 ) // str equals "R"
            {
                Action_button_R = (Action_button == NULL)?Action_none:Action_button;
                Action_button_R_Key = Action_button_Key;
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
            //senquack - added L+vol buttons
            else if ( strcasecmp(str, "L+Vol+") == 0 ) // str equals "L+Vol+"
            {
                Action_button_L_VolUp = Action_button;
                Action_button_L_VolUp_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+Vol-") == 0 ) // str equals "L+Vol-"
            {
                Action_button_L_VolDown = Action_button;
                Action_button_L_VolDown_Key = Action_button_Key;
            }
            //senquack - up/down/left/right remappable (for left-handed touchscreen users)
            else if ( strcasecmp(str, "Up") == 0 ) // str equals "Up"
            {
                Action_button_Up = (Action_button == NULL)?Action_none:Action_button;
                Action_button_Up_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "Down") == 0 ) // str equals "Down"
            {
                Action_button_Down = (Action_button == NULL)?Action_none:Action_button;
                Action_button_Down_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "Left") == 0 ) // str equals "Left"
            {
                Action_button_Left = (Action_button == NULL)?Action_none:Action_button;
                Action_button_Left_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "Right") == 0 ) // str equals "Right"
            {
                Action_button_Right = (Action_button == NULL)?Action_none:Action_button;
                Action_button_Right_Key = Action_button_Key;
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
                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_ABXY)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // Only when cursor keys are being controlled by the dpad/stick
                        // are ABXY remappable:
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
                    }

                    break;
                case GP2X_BUTTON_B:
                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_ABXY)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // Only when cursor keys are being controlled by the dpad/stick
                        // are ABXY remappable:
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
                    }

                    break;
                case GP2X_BUTTON_X:
                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_ABXY)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // Only when cursor keys are being controlled by the dpad/stick
                        // are ABXY remappable:
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
                    }

                    break;
                case GP2X_BUTTON_Y:
                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_ABXY)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // Only when cursor keys are being controlled by the dpad/stick
                        // are ABXY remappable:
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
                //senquack - start is now remappable:
                case GP2X_BUTTON_START:
                    if (!Game_JButton[GP2X_BUTTON_R] && !Game_JButton[GP2X_BUTTON_L])
                    {
                        Action_button_Start(1, Action_button_Start_Key);
                    }
                    break;
                //senquack
                case GP2X_BUTTON_VOLUP:
                    Game_JButton[event.jbutton.button] = 1;

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
                    //senquack
                    else if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        if (Action_button_L_VolUp != NULL)
                        {
                            Action_button_L_VolUp(1, Action_button_L_VolUp_Key);
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
                //senquack
                case GP2X_BUTTON_VOLDOWN:
                    Game_JButton[event.jbutton.button] = 1;

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
                    else if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        if (Action_button_L_VolDown != NULL)
                        {
                            Action_button_L_VolDown(1, Action_button_L_VolDown_Key);
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
                //senquack - up/down/left/right are now mappable:
                case GP2X_BUTTON_UP:
                    //senquack
                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_DPAD)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // When cursor keys are controlled by ABXY buttons,
                        // the DPAD/stick buttons are remappable:
                        Action_button_Up(1, Action_button_Up_Key);
                    }

                    break;
                case GP2X_BUTTON_DOWN:
                    Game_JButton[event.jbutton.button] = 1;

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_DPAD)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // When cursor keys are controlled by ABXY buttons,
                        // the DPAD/stick buttons are remappable:
                        Action_button_Down(1, Action_button_Down_Key);
                    }

                    break;
                case GP2X_BUTTON_LEFT:
                    //senquack
                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_DPAD)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // When cursor keys are controlled by ABXY buttons,
                        // the DPAD/stick buttons are remappable:
                        Action_button_Left(1, Action_button_Left_Key);
                    }

                    break;
                case GP2X_BUTTON_RIGHT:
                    //senquack
                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_DPAD)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // When cursor keys are controlled by ABXY buttons,
                        // the DPAD/stick buttons are remappable:
                        Action_button_Right(1, Action_button_Right_Key);
                    }

                    break;
               //senquack - up/down/left/right are now mappable:
//                    case GP2X_BUTTON_UP:
//                    case GP2X_BUTTON_DOWN:
//                    case GP2X_BUTTON_LEFT:
//                    case GP2X_BUTTON_RIGHT:
                case GP2X_BUTTON_UPLEFT:
                case GP2X_BUTTON_UPRIGHT:
                case GP2X_BUTTON_DOWNLEFT:
                case GP2X_BUTTON_DOWNRIGHT:
                    //senquack
                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                        if (Game_CursorButtons == GAME_CURSORBUTTONS_DPAD)
                        {
                            if (Game_JKeyboard)
                            {
                                ChangeCursorKeys();
                            }
                        }
                    }
                    break;
                case GP2X_BUTTON_R:
                    //senquack - Button R is now mappable and also is no longer the
                    //           de-facto control key, as that is also mappable now
                    Game_JButton[GP2X_BUTTON_R] = 1;
//                        if (!Game_Paused)
//                        {
//                            if (Game_JKeyboard)
//                            {
//                                ChangeCursorKeys();
//                            }
//                        }
                    Action_button_R(1, Action_button_R_Key);

                    break;
                case GP2X_BUTTON_L:
                    //senquack - Button L is now mappable when using touchscreen
                    Game_JButton[GP2X_BUTTON_L] = 1;

                    if (Game_UsingTouchscreen)
                    {
                        Action_button_L(1, Action_button_L_Key);
                    }
                    break;
                default:
                    return 0;
            }
            break;
            // case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            //senquack
            switch (event.jbutton.button)
            {
                case GP2X_BUTTON_A:
                    Game_JButton[event.jbutton.button] = 0;

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_ABXY)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
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
                    }

                    break;
                case GP2X_BUTTON_B:
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_ABXY)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
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
                    }

                    break;
                case GP2X_BUTTON_X:
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

                    if (SMK_Playing)
                    {
                        EmulateKey(SDL_KEYUP, SDLK_ESCAPE);
                        break;
                    }

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_ABXY)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
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
                    }

                    break;
                case GP2X_BUTTON_Y:
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_ABXY)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
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
                    }

                    break;
                case GP2X_BUTTON_SELECT:
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

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
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

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
                    else if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        if (Action_button_L_VolUp != NULL)
                        {
                            Action_button_L_VolUp(0, Action_button_L_VolUp_Key);
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
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

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
                    else if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        if (Action_button_L_VolDown != NULL)
                        {
                            Action_button_L_VolDown(0, Action_button_L_VolDown_Key);
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
                    //senquack - start is now remappable:
                    Game_JButton[event.jbutton.button] = 0;

                    if (Game_JButton[GP2X_BUTTON_R] && Game_JButton[GP2X_BUTTON_L])
                    {
                        SDL_Event pump_event;

                        pump_event.type = SDL_USEREVENT;
                        pump_event.user.code = EC_PROGRAM_QUIT;
                        pump_event.user.data1 = NULL;
                        pump_event.user.data2 = NULL;

                        SDL_PushEvent(&pump_event);
                    }
                    else
                    {
                        if (SMK_Playing)
                        {
                            EmulateKey(SDL_KEYUP, SDLK_ESCAPE);
                            break;
                        }

                        if (!Game_JButton[GP2X_BUTTON_R] && !Game_JButton[GP2X_BUTTON_L])
                        {
                            Action_button_Start(0, Action_button_Start_Key);
                        }
                    }
                    break;
                    //senquack - up/down/left/right are now mappable:
//                    case GP2X_BUTTON_UP:
//                    case GP2X_BUTTON_DOWN:
//                    case GP2X_BUTTON_LEFT:
//                    case GP2X_BUTTON_RIGHT:
                case GP2X_BUTTON_UP:
                    Game_JButton[event.jbutton.button] = 0;

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_DPAD)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // When cursor keys are controlled by ABXY buttons,
                        // the DPAD/stick buttons are remappable:
                        Action_button_Up(0, Action_button_Up_Key);
                    }

                    if (Game_Paused)
                    {
                        EmulateKey(SDL_KEYUP, SDLK_UP);
                    }

                    break;
                case GP2X_BUTTON_DOWN:
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_DPAD)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // When cursor keys are controlled by ABXY buttons,
                        // the DPAD/stick buttons are remappable:
                        Action_button_Down(0, Action_button_Down_Key);
                    }

                    if (Game_Paused)
                    {
                        EmulateKey(SDL_KEYUP, SDLK_DOWN);
                    }

                    break;
                case GP2X_BUTTON_LEFT:
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_DPAD)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // When cursor keys are controlled by ABXY buttons,
                        // the DPAD/stick buttons are remappable:
                        Action_button_Left(0, Action_button_Left_Key);
                    }

                    if (Game_Paused)
                    {
                        EmulateKey(SDL_KEYUP, SDLK_LEFT);
                    }

                    break;
                case GP2X_BUTTON_RIGHT:
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

                    if (Game_CursorButtons == GAME_CURSORBUTTONS_DPAD)
                    {
                        if (Game_JKeyboard && !Game_Paused)
                        {
                            ChangeCursorKeys();
                        }
                    }
                    else
                    {
                        // When cursor keys are controlled by ABXY buttons,
                        // the DPAD/stick buttons are remappable:
                        Action_button_Right(0, Action_button_Right_Key);
                    }

                    if (Game_Paused)
                    {
                        EmulateKey(SDL_KEYUP, SDLK_RIGHT);
                    }

                    break;
                case GP2X_BUTTON_UPLEFT:
                case GP2X_BUTTON_UPRIGHT:
                case GP2X_BUTTON_DOWNLEFT:
                case GP2X_BUTTON_DOWNRIGHT:
                    //senquack
                    Game_JButton[event.jbutton.button] = 0;

                    if (Game_JKeyboard && !Game_Paused)
                    {
                        ChangeCursorKeys();
                    }
                    break;
                case GP2X_BUTTON_R:
                    //senquack - Button R is now mappable and also is no longer the
                    //           de-facto control key, as that is also mappable now
                    Game_JButton[GP2X_BUTTON_R] = 0;
//                        if (!Game_Paused)
//                        {
//                            if (Game_JKeyboard)
//                            {
//                                ChangeCursorKeys();
//                            }
//                        }
                    Action_button_R(0, Action_button_R_Key);
                    break;
                case GP2X_BUTTON_L:
                    //senquack - Button L is now mappable (when using touchscreen)
                    Game_JButton[GP2X_BUTTON_L] = 0;

                    if (!Game_UsingTouchscreen)
                    {
                        if (!Game_Paused)
                        {
                            if (Game_JKeyboard)
                            {
                                ReleaseCursorKeys();
                                Game_JKeyboard = 0;
                            }
                            else
                            {
                                Game_JKeyboard = 1;
                                ChangeCursorKeys();
                            }
                        }
                    }
                    else
                    {
                        Action_button_L(0, Action_button_L_Key);
                    }
                    break;
                default:
                    return 0;
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

