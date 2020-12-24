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
#include <string.h>

#if (SDL_MAJOR_VERSION > 1 || SDL_MAJOR_VERSION == 1 && (SDL_MINOR_VERSION > 2 || SDL_MINOR_VERSION == 2 && SDL_PATCHLEVEL >= 9 ) )
    #include <SDL/SDL_gp2x.h>
#endif


// definitions
//senquack
#define GAME_SCROLLBUTTONS_F100	  (0)
#define GAME_SCROLLBUTTONS_F200LH (1)
#define GAME_SCROLLBUTTONS_F200RH (2)

// GAME_JOYSTICK != JOYSTICK_NONE
#define CURSOR_KEY_UP            (0)
#define CURSOR_KEY_DOWN          (2)
#define CURSOR_KEY_LEFT          (1)
#define CURSOR_KEY_RIGHT         (3)
#define CTRL_KEY                 (4)
#define ALT_KEY                  (5)

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
// GAME_JOYSTICK != JOYSTICK_NONE
static volatile int Game_JButton[20];	/* joystick buttons */

// GAME_JOYSTICK = JOYSTICK_GP2X
//senquack
static void (*Action_button_Up)(int pressed, int key);	/* procedure for Up button */
static void (*Action_button_Down)(int pressed, int key);	/* procedure for Down button */
static void (*Action_button_Left)(int pressed, int key);	/* procedure for Left button */
static void (*Action_button_Right)(int pressed, int key);	/* procedure for Right button */
static void (*Action_button_UpLeft)(int pressed, int key);	/* procedure for UpLeft button */
static void (*Action_button_UpRight)(int pressed, int key);	/* procedure for UpRight button */
static void (*Action_button_DownLeft)(int pressed, int key);	/* procedure for DownLeft button */
static void (*Action_button_DownRight)(int pressed, int key);	/* procedure for DownRight button */

static void (*Action_button_A)(int pressed, int key);	/* procedure for A button */
static void (*Action_button_B)(int pressed, int key);	/* procedure for B button */
static void (*Action_button_X)(int pressed, int key);	/* procedure for X button */
static void (*Action_button_Y)(int pressed, int key);	/* procedure for Y button */
static void (*Action_button_Select)(int pressed, int key);	/* procedure for Select button */

//senquack - had to make Pause remappable
static void (*Action_button_Start)(int pressed, int key);	/* procedure for Select button */

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
static void (*Action_button_L_Up)(int pressed, int key);	/* procedure for L+Up button */
static void (*Action_button_L_Down)(int pressed, int key);	/* procedure for L+Down button */
static void (*Action_button_L_Left)(int pressed, int key);	/* procedure for L+Left button */
static void (*Action_button_L_Right)(int pressed, int key);	/* procedure for L+Right button */
static void (*Action_button_L_UpLeft)(int pressed, int key);	/* procedure for L+UpLeft button */
static void (*Action_button_L_UpRight)(int pressed, int key);	/* procedure for L+UpRight button */
static void (*Action_button_L_DownLeft)(int pressed, int key);	/* procedure for L+DownLeft button */
static void (*Action_button_L_DownRight)(int pressed, int key);	/* procedure for L+DownRight button */
static void (*Action_button_L_Select)(int pressed, int key);		/* procedure for L+Select button */
static void (*Action_button_L_VolUp)(int pressed, int key);		/* procedure for L+Volume+ button */
static void (*Action_button_L_VolDown)(int pressed, int key);	/* procedure for L+Volume- button */

//senquack - modified:
static int Action_button_A_Key, Action_button_B_Key, Action_button_X_Key,
           Action_button_Y_Key, Action_button_Select_Key, Action_button_Start_Key,
           Action_button_VolUp_Key, Action_button_VolDown_Key,
           Action_button_Up_Key, Action_button_Down_Key,
           Action_button_Left_Key, Action_button_Right_Key,
           Action_button_UpLeft_Key, Action_button_UpRight_Key,
           Action_button_DownLeft_Key, Action_button_DownRight_Key;

//senquack - modified:
static int Action_button_R_A_Key, Action_button_R_B_Key, Action_button_R_X_Key,
           Action_button_R_Y_Key, Action_button_R_Select_Key, Action_button_R_VolUp_Key,
           Action_button_R_VolDown_Key, Action_button_L_Up_Key, Action_button_L_Down_Key,
           Action_button_L_Left_Key, Action_button_L_Right_Key, Action_button_L_VolDown_Key,
           Action_button_L_VolUp_Key, Action_button_L_Select_Key,
           Action_button_L_UpLeft_Key, Action_button_L_UpRight_Key,
           Action_button_L_DownLeft_Key, Action_button_L_DownRight_Key;

//senquack
static int Game_UsingTouchscreen;			/* Playing on GP2X with touchscreen? */
static int Game_CurrentlyScrolling;			/* Are we currently scrolling in the game?
                                               Needed for proper input handling.	*/
static int Game_ScrollButtons;				// GAME_SCROLLBUTTONS_F100: L+Stick used to scroll
                                            // GAME_SCROLLBUTTONS_F200RH: Stick (DPAD) of F200 used to scroll
                                            // GAME_SCROLLBUTTONS_F200LH: A/B/X/Y used to scroll


// functions
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
        EmulateKey((pressed)?SDL_KEYDOWN:SDL_KEYUP, key);
    }
}

//senquack - new macros to rotate geoscape; do nothing in Tactical
static void Action_rotateup(int pressed, int key)
{
}
static void Action_rotatedown(int pressed, int key)
{
}
static void Action_rotateleft(int pressed, int key)
{
}
static void Action_rotateright(int pressed, int key)
{
}

//senquack - new macros to moving up/down levels in battlescape, zoom in/out in geoscape
static void Action_levelup(int pressed, int key)
{
    static int32_t orig_x, orig_y;			// Stores mouse coordinates while macro occurs

    if (!Game_Paused)
    {
        if (pressed)
        {
            orig_x = Game_MouseX;			// Store away mouse coordinates before macro
            orig_y = Game_MouseY;

            // Simulate movement to battlescape "Level Up" icon
            if ((Display_Height == 240))
            {
                Game_MouseX = 94;
                Game_MouseY = 183;
            }
            else
            {
                Game_MouseX = 94;
                Game_MouseY = 152;
            }

            SDL_Event pump_event;

            //		IMPORTANT:  This delay prevents accidental registering of mouse clicks
            //			when cursor moves back to original position.  Game needs time to
            //			pickup changes.
            EmulateDelay(70);

            pump_event.type = SDL_MOUSEBUTTONDOWN;
            pump_event.button.button = SDL_BUTTON_LEFT;
            pump_event.button.state = SDL_PRESSED;
            pump_event.button.x = Game_MouseX;
            pump_event.button.y = Game_MouseY;

            SDL_PushEvent(&pump_event);

            //		IMPORTANT:  This delay prevents accidental registering of mouse clicks
            //			when cursor moves back to original position.  Game needs time to
            //			pickup changes.
            EmulateDelay(70);

            pump_event.type = SDL_MOUSEBUTTONUP;
            pump_event.button.button = SDL_BUTTON_LEFT;
            pump_event.button.state = SDL_RELEASED;
            pump_event.button.x = Game_MouseX;
            pump_event.button.y = Game_MouseY;

            SDL_PushEvent(&pump_event);
        }
        else
        {
            Game_MouseX = orig_x;
            Game_MouseY = orig_y;

            SDL_WarpMouse(Game_MouseX, Game_MouseY);
        }
    }
}

static void Action_leveldown(int pressed, int key)
{
    static int32_t orig_x, orig_y;			// Stores mouse coordinates while macro occurs

    if (!Game_Paused)
    {
        if (pressed)
        {
            orig_x = Game_MouseX;			// Store away mouse coordinates before macro
            orig_y = Game_MouseY;

            // Simulate movement to battlescape "Level Down" icon
            if ((Display_Height == 240))
            {
                Game_MouseX = 94;
                Game_MouseY = 201;
            }
            else
            {
                Game_MouseX = 94;
                Game_MouseY = 168;
            }

            SDL_Event pump_event;

            //		IMPORTANT:  This delay prevents accidental registering of mouse clicks
            //			when cursor moves back to original position.  Game needs time to
            //			pickup changes.
            EmulateDelay(70);

            pump_event.type = SDL_MOUSEBUTTONDOWN;
            pump_event.button.button = SDL_BUTTON_LEFT;
            pump_event.button.state = SDL_PRESSED;
            pump_event.button.x = Game_MouseX;
            pump_event.button.y = Game_MouseY;

            SDL_PushEvent(&pump_event);

            //		IMPORTANT:  This delay prevents accidental registering of mouse clicks
            //			when cursor moves back to original position.  Game needs time to
            //			pickup changes.
            EmulateDelay(70);

            pump_event.type = SDL_MOUSEBUTTONUP;
            pump_event.button.button = SDL_BUTTON_LEFT;
            pump_event.button.state = SDL_RELEASED;
            pump_event.button.x = Game_MouseX;
            pump_event.button.y = Game_MouseY;

            SDL_PushEvent(&pump_event);
        }
        else
        {
            Game_MouseX = orig_x;
            Game_MouseY = orig_y;

            SDL_WarpMouse(Game_MouseX, Game_MouseY);
        }
    }
}

static void Action_selectnextsoldier(int pressed, int key)
{
    static int32_t orig_x, orig_y;			// Stores mouse coordinates while macro occurs

    if (!Game_Paused)
    {
        if (pressed)
        {
            orig_x = Game_MouseX;			// Store away mouse coordinates before macro
            orig_y = Game_MouseY;

            // Simulate movement to battlescape "Select Next Soldier" icon
            if ((Display_Height == 240))
            {
                Game_MouseX = 190;
                Game_MouseY = 183;
            }
            else
            {
                Game_MouseX = 190;
                Game_MouseY = 152;
            }

            SDL_Event pump_event;

            //		IMPORTANT:  This delay prevents accidental registering of mouse clicks
            //			when cursor moves back to original position.  Game needs time to
            //			pickup changes.
            EmulateDelay(70);

            pump_event.type = SDL_MOUSEBUTTONDOWN;
            pump_event.button.button = SDL_BUTTON_LEFT;
            pump_event.button.state = SDL_PRESSED;
            pump_event.button.x = Game_MouseX;
            pump_event.button.y = Game_MouseY;

            SDL_PushEvent(&pump_event);

            //		IMPORTANT:  This delay prevents accidental registering of mouse clicks
            //			when cursor moves back to original position.  Game needs time to
            //			pickup changes.
            EmulateDelay(70);

            pump_event.type = SDL_MOUSEBUTTONUP;
            pump_event.button.button = SDL_BUTTON_LEFT;
            pump_event.button.state = SDL_RELEASED;
            pump_event.button.x = Game_MouseX;
            pump_event.button.y = Game_MouseY;

            SDL_PushEvent(&pump_event);
        }
        else
        {
            Game_MouseX = orig_x;
            Game_MouseY = orig_y;

            SDL_WarpMouse(Game_MouseX, Game_MouseY);
        }
    }
}

static void Action_deselectcurrentsoldier(int pressed, int key)
{
    static int32_t orig_x, orig_y;			// Stores mouse coordinates while macro occurs

    if (!Game_Paused)
    {
        if (pressed)
        {
            orig_x = Game_MouseX;			// Store away mouse coordinates before macro
            orig_y = Game_MouseY;

            // Simulate movement to battlescape "Deselect Current Soldier" icon
            if ((Display_Height == 240))
            {
                Game_MouseX = 190;
                Game_MouseY = 201;
            }
            else
            {
                Game_MouseX = 190;
                Game_MouseY = 168;
            }

            SDL_Event pump_event;

            //		IMPORTANT:  This delay prevents accidental registering of mouse clicks
            //			when cursor moves back to original position.  Game needs time to
            //			pickup changes.
            EmulateDelay(70);

            pump_event.type = SDL_MOUSEBUTTONDOWN;
            pump_event.button.button = SDL_BUTTON_LEFT;
            pump_event.button.state = SDL_PRESSED;
            pump_event.button.x = Game_MouseX;
            pump_event.button.y = Game_MouseY;

            SDL_PushEvent(&pump_event);

            //		IMPORTANT:  This delay prevents accidental registering of mouse clicks
            //			when cursor moves back to original position.  Game needs time to
            //			pickup changes.
            EmulateDelay(70);

            pump_event.type = SDL_MOUSEBUTTONUP;
            pump_event.button.button = SDL_BUTTON_LEFT;
            pump_event.button.state = SDL_RELEASED;
            pump_event.button.x = Game_MouseX;
            pump_event.button.y = Game_MouseY;

            SDL_PushEvent(&pump_event);
        }
        else
        {
            Game_MouseX = orig_x;
            Game_MouseY = orig_y;

            SDL_WarpMouse(Game_MouseX, Game_MouseY);
        }
    }
}

//senquack - Pause is now a remappable action
static void Action_pause(int pressed, int key)
{
    if ( !(Game_JButton[GP2X_BUTTON_R] && Game_JButton[GP2X_BUTTON_L]) && pressed == 1)
    {
        //Make sure all scrolling is stopped
        Game_CurrentlyScrolling = 0;
#if defined(__DEBUG__)
        fprintf(stderr, "XCOM: Pausing game..\n");
#endif
        EmulateKey(SDL_KEYDOWN, SDLK_PAUSE);
    }
    else if ( !(Game_JButton[GP2X_BUTTON_R] && Game_JButton[GP2X_BUTTON_L]) && pressed == 0)
    {
#if defined(__DEBUG__)
        fprintf(stderr, "XCOM: Un-pausing game..\n");
#endif
        EmulateKey(SDL_KEYUP, SDLK_PAUSE);
    }
}

//senquack - volume increase/decrease actions
static void Action_volume_increase(int pressed, int key)
{
    if (pressed)
    {
#if defined(__DEBUG__)
        fprintf(stderr, "XCOM: Begin increasing GP2X volume..\n");
#endif
        Game_VolumeDelta = 1;			// Signifies code in game loop should increase volume
    }
    else
    {
#if defined(__DEBUG__)
        fprintf(stderr, "XCOM: Cease increasing GP2X volume..\n");
#endif
        Game_VolumeDelta = 0;
    }
}

static void Action_volume_decrease(int pressed, int key)
{
    if (pressed)
    {
#if defined(__DEBUG__)
        fprintf(stderr, "XCOM: Begin decreasing GP2X volume..\n");
#endif
        Game_VolumeDelta = -1;			// Signifies code in game loop should decrease volume
    }
    else
    {
#if defined(__DEBUG__)
        fprintf(stderr, "XCOM: Cease decreasing GP2X volume..\n");
#endif
        Game_VolumeDelta = 0;
    }
}


void Init_Input(void)
{
    Game_Joystick = 1;

//senquack
    Game_UsingTouchscreen = 0;
    Game_CurrentlyScrolling = 0;
    Game_ScrollButtons = GAME_SCROLLBUTTONS_F100;


// GAME_JOYSTICK != JOYSTICK_NONE

// GAME_JOYSTICK = JOYSTICK_GP2X
    //senquack - not remappable when not using the touchscreen:
    Action_button_Up = &Action_none;
    Action_button_Down = &Action_none;
    Action_button_Left = &Action_none;
    Action_button_Right = &Action_none;
    Action_button_UpLeft = &Action_none;
    Action_button_UpRight = &Action_none;
    Action_button_DownLeft = &Action_none;
    Action_button_DownRight = &Action_none;

    Action_button_A = &Action_key;
    Action_button_B = &Action_left_mouse_button;
    Action_button_X = &Action_right_mouse_button;
    Action_button_Y = &Action_key;
    Action_button_Select = &Action_none;
    //senquack
    Action_button_Start = &Action_pause;

    Action_button_VolUp = &Action_none;
    Action_button_VolDown = &Action_none;

    Action_button_A_Key = SDLK_BACKSPACE;
    Action_button_B_Key = 0;
    Action_button_X_Key = 0;
    Action_button_Y_Key = SDLK_y;
    Action_button_Select_Key = 0;
    Action_button_VolUp_Key = 0;
    Action_button_VolDown_Key = 0;
    //senquack - added these:
    Action_button_Start_Key = 0;
    Action_button_Up_Key = 0;
    Action_button_Down_Key = 0;
    Action_button_Left_Key = 0;
    Action_button_Right_Key = 0;
    Action_button_UpLeft_Key = 0;
    Action_button_UpRight_Key = 0;
    Action_button_DownLeft_Key = 0;
    Action_button_DownRight_Key = 0;

    //senquack
    Action_button_R_A = &Action_none;
    Action_button_R_B = &Action_key;
    Action_button_R_X = &Action_none;
    Action_button_R_Y = &Action_none;
    Action_button_R_Select = &Action_key;
    Action_button_R_VolUp = &Action_none;
    Action_button_R_VolDown = &Action_none;

    Action_button_R_A_Key = 0;
    Action_button_R_B_Key = SDLK_RETURN;
    Action_button_R_X_Key = 0;
    Action_button_R_Y_Key = 0;
    Action_button_R_Select_Key = SDLK_F15;
    Action_button_R_VolUp_Key = 0;
    Action_button_R_VolDown_Key = 0;

    //senquack:
    Action_button_L_Left = &Action_none;
    Action_button_L_Right = &Action_none;
    Action_button_L_Down = &Action_none;
    Action_button_L_Up = &Action_none;
    Action_button_L_UpLeft = &Action_none;
    Action_button_L_UpRight = &Action_none;
    Action_button_L_DownLeft = &Action_none;
    Action_button_L_DownRight = &Action_none;
    Action_button_L_Select = &Action_none;
    Action_button_L_VolUp = &Action_none;
    Action_button_L_VolDown = &Action_none;

    //senquack:
    Action_button_L_Left_Key = 0;
    Action_button_L_Right_Key = 0;
    Action_button_L_Down_Key = 0;
    Action_button_L_Up_Key = 0;
    Action_button_L_UpLeft_Key = 0;
    Action_button_L_UpRight_Key = 0;
    Action_button_L_DownLeft_Key = 0;
    Action_button_L_DownRight_Key = 0;
    Action_button_L_Select_Key = 0;
    Action_button_L_VolUp_Key = 0;
    Action_button_L_VolDown_Key = 0;
}

void Init_Input2(void)
{
// senquack
#if defined(SDL_GP2X__H)

    if (SDL_GP2X_MouseType() == 2)			/* On a unit with touchscreen? */
    {
        // New SDL option allows us to block SDL from generating mouse button events
        // when touchscreen is pressed.  When using touchscreen, we want button events
        // controlled only by GP2X buttons.  If user has USB mouse connected this will not
        // be called.
        SDL_GP2X_TouchpadMouseButtonEvents(0);	/* If we're running on an F200 with touchscreen,
                                                            always block mouse button events from it. */

        if (Game_UsingTouchscreen)
        {
            // On touchscreen units, when the user hasn't explicity disabled the touchscreen in
            // UFO.cfg, force full-screen stretched mode.  Simplifies things.
            //Game_DisplayStretched = 1;
        }
        else
        {
            // If an F200 user has explicity disabled the touchscreen, disable mouse motion
            // events being registered by stylus movement.  New SDL option.
            SDL_GP2X_TouchpadMouseMotionEvents(0);
        }
    }
    else
    {
        Game_UsingTouchscreen = 0; 	/* When not on F200s, always disable this */
    }

#endif
}

int Config_Input(char *str, char *param)
{
//senquack
    if ( strcasecmp(str, "Scrollbuttons") == 0)	// str equals "Scrollbuttons"
    {
        if ( strcasecmp(param, "F100") == 0)	// param equals "F100"
        {
            // Use L+Stick to scroll/rotate
            Game_ScrollButtons = GAME_SCROLLBUTTONS_F100;
        }
        else if ( strcasecmp(param, "F200_rh") == 0) // param equals "F200_rh"
        {
            // Use DPAD of F200 to scroll/rotate
            Game_ScrollButtons = GAME_SCROLLBUTTONS_F200RH;
        }
        else if ( strcasecmp(param, "F200_lh") == 0) // param equals "F200_lh"
        {
            // Use A/B/X/Y buttons of F200 to scroll/rotate
            Game_ScrollButtons = GAME_SCROLLBUTTONS_F200LH;
        }
    }
    else if ( strcasecmp(str, "Touchscreen") == 0)	// str equals "Touchscreen"
    {
        if ( strcasecmp(param, "on") == 0)	// param equals "on"
        {
            // Use GP2X touchscreen if any is available
            Game_UsingTouchscreen = 1;
        }
        else if ( strcasecmp(param, "off") == 0) // param equals "off"
        {
            // Don't use GP2X touchscreen if any is available
//				Game_UsingTouchscreen = 0;	/* Default, Don't assign here. Just for clarification.
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

                if ( strcasecmp(param, "backspace") == 0 ) // param equals "backspace"
                {
                    // backspace key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_BACKSPACE;
                }
                else if ( strcasecmp(param, "enter") == 0 ) // param equals "enter"
                {
                    // enter key

                    Action_button = &Action_key;
                    Action_button_Key = SDLK_RETURN;
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
            else if ( strcasecmp(param, "rotateup") == 0 ) // param equals "rotateup"
            {
                // rotate Up macro

                Action_button = &Action_rotateup;
            }
            else if ( strcasecmp(param, "rotatedown") == 0 ) // param equals "rotatedown"
            {
                // rotate Down macro

                Action_button = &Action_rotatedown;
            }
            else if ( strcasecmp(param, "rotateleft") == 0 ) // param equals "rotateleft"
            {
                // rotate Left macro

                Action_button = &Action_rotateleft;
            }
            else if ( strcasecmp(param, "rotateright") == 0 ) // param equals "rotateright"
            {
                // rotate Right macro

                Action_button = &Action_rotateright;
            }
            //senquack
            else if ( strcasecmp(param, "levelup") == 0 ) // param equals "levelup"
            {
                // Battlescape View Level Up

                Action_button = &Action_levelup;
            }
            else if ( strcasecmp(param, "leveldown") == 0 ) // param equals "leveldown"
            {
                // Battlescape View Level Down

                Action_button = &Action_leveldown;
            }
            else if ( strcasecmp(param, "selectnextsoldier") == 0 ) // param equals "selectnextsoldier"
            {
                // Battlescape Select Next Soldier

                Action_button = &Action_selectnextsoldier;
            }
            else if ( strcasecmp(param, "deselectcurrentsoldier") == 0 ) // param equals "deselectcurrentsoldier"
            {
                // Battlescape Deselect Current Soldier

                Action_button = &Action_deselectcurrentsoldier;
            }
            //senquack
            else if ( strcasecmp(param, "pause") == 0 ) // param equals "pause"
            {
                // Pause game
                Action_button = &Action_pause;
            }
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
            //senquack
            else if ( strcasecmp(str, "Start") == 0 ) // str equals "Start"
            {
                Action_button_Start = (Action_button == NULL)?Action_none:Action_button;
                Action_button_Start_Key = Action_button_Key;
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
                Action_button_R_A = (Action_button == NULL)?Action_none:Action_button;
                Action_button_R_A_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+B") == 0 ) // str equals "R+B"
            {
                Action_button_R_B = (Action_button == NULL)?Action_none:Action_button;
                Action_button_R_B_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+X") == 0 ) // str equals "R+X"
            {
                Action_button_R_X = (Action_button == NULL)?Action_none:Action_button;
                Action_button_R_X_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+Y") == 0 ) // str equals "R+Y"
            {
                Action_button_R_Y = (Action_button == NULL)?Action_none:Action_button;
                Action_button_R_Y_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+Select") == 0 ) // str equals "R+Select"
            {
                Action_button_R_Select = (Action_button == NULL)?Action_none:Action_button;
                Action_button_R_Select_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+Vol+") == 0 ) // str equals "R+Vol+"
            {
                Action_button_R_VolUp = (Action_button == NULL)?Action_none:Action_button;
                Action_button_R_VolUp_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "R+Vol-") == 0 ) // str equals "R+Vol-"
            {
                Action_button_R_VolDown = (Action_button == NULL)?Action_none:Action_button;
                Action_button_R_VolDown_Key = Action_button_Key;
            }
            //senquack:
            else if ( strcasecmp(str, "L+Up") == 0 ) // str equals "L+Up"
            {
                Action_button_L_Up = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_Up_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+Down") == 0 ) // str equals "L+Down"
            {
                Action_button_L_Down = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_Down_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+Left") == 0 ) // str equals "L+Left"
            {
                Action_button_L_Left = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_Left_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+Right") == 0 ) // str equals "L+Right"
            {
                Action_button_L_Right = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_Right_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+UpLeft") == 0 ) // str equals "L+UpLeft"
            {
                Action_button_L_UpLeft = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_UpLeft_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+UpRight") == 0 ) // str equals "L+UpRight"
            {
                Action_button_L_UpRight = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_UpRight_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+DownLeft") == 0 ) // str equals "L+DownLeft"
            {
                Action_button_L_DownLeft = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_DownLeft_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+DownRight") == 0 ) // str equals "L+DownRight"
            {
                Action_button_L_DownRight = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_DownRight_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+Select") == 0 ) // str equals "L+Select"
            {
                Action_button_L_Select = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_Select_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+Vol+") == 0 ) // str equals "L+Vol+"
            {
                Action_button_L_VolUp = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_VolUp_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "L+Vol-") == 0 ) // str equals "L+Vol-"
            {
                Action_button_L_VolDown = (Action_button == NULL)?Action_none:Action_button;
                Action_button_L_VolDown_Key = Action_button_Key;
            }
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
            else if ( strcasecmp(str, "UpLeft") == 0 ) // str equals "UpLeft"
            {
                Action_button_UpLeft = (Action_button == NULL)?Action_none:Action_button;
                Action_button_UpLeft_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "UpRight") == 0 ) // str equals "UpRight"
            {
                Action_button_UpRight = (Action_button == NULL)?Action_none:Action_button;
                Action_button_UpRight_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "DownLeft") == 0 ) // str equals "DownLeft"
            {
                Action_button_DownLeft = (Action_button == NULL)?Action_none:Action_button;
                Action_button_DownLeft_Key = Action_button_Key;
            }
            else if ( strcasecmp(str, "DownRight") == 0 ) // str equals "DownRight"
            {
                Action_button_DownRight = (Action_button == NULL)?Action_none:Action_button;
                Action_button_DownRight_Key = Action_button_Key;
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
                //senquack - altered a lot of these:
                case GP2X_BUTTON_A:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        Action_button_R_A(1, Action_button_R_A_Key);
                    }
                    else
                    {
                        Action_button_A(1, Action_button_A_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_B:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        Action_button_R_B(1, Action_button_R_B_Key);
                    }
                    else
                    {
                        Action_button_B(1, Action_button_B_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_X:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        Action_button_R_X(1, Action_button_R_X_Key);
                    }
                    else
                    {
                        Action_button_X(1, Action_button_X_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_Y:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        Action_button_R_Y(1, Action_button_R_Y_Key);
                    }
                    else
                    {
                        Action_button_Y(1, Action_button_Y_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_SELECT:
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        if (Action_button_L_Select != NULL)
                        {
                            Action_button_L_Select(1, Action_button_L_Select_Key);
                        }
                        else
                        {
                            Action_button_Select(1, Action_button_Select_Key);
                        }
                    }
                    else if ( Game_JButton[GP2X_BUTTON_R] )
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
                    //senquack
                case GP2X_BUTTON_START:
                    Action_button_Start(1, Action_button_Start_Key);

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
                case GP2X_BUTTON_UP:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        Action_button_L_Up(1, Action_button_L_Up_Key);
                    }
                    else
                    {
                        Action_button_Up(1, Action_button_Up_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_DOWN:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        Action_button_L_Down(1, Action_button_L_Down_Key);
                    }
                    else
                    {
                        Action_button_Down(1, Action_button_Down_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_LEFT:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        Action_button_L_Left(1, Action_button_L_Left_Key);
                    }
                    else
                    {
                        Action_button_Left(1, Action_button_Left_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_RIGHT:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        Action_button_L_Right(1, Action_button_L_Right_Key);
                    }
                    else
                    {
                        Action_button_Right(1, Action_button_Right_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_UPLEFT:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        Action_button_L_UpLeft(1, Action_button_L_UpLeft_Key);
                    }
                    else
                    {
                        Action_button_UpLeft(1, Action_button_UpLeft_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_UPRIGHT:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        Action_button_L_UpRight(1, Action_button_L_UpRight_Key);
                    }
                    else
                    {
                        Action_button_UpRight(1, Action_button_UpRight_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_DOWNLEFT:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        Action_button_L_DownLeft(1, Action_button_L_DownLeft_Key);
                    }
                    else
                    {
                        Action_button_DownLeft(1, Action_button_DownLeft_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_DOWNRIGHT:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        Action_button_L_DownRight(1, Action_button_L_DownRight_Key);
                    }
                    else
                    {
                        Action_button_DownRight(1, Action_button_DownRight_Key);
                    }

                    if (!Game_Paused)
                    {
                        Game_JButton[event.jbutton.button] = 1;
                    }
                    break;
                case GP2X_BUTTON_R:
                    Game_JButton[GP2X_BUTTON_R] = 1;
                    break;
                case GP2X_BUTTON_L:
                    Game_JButton[GP2X_BUTTON_L] = 1;
                    break;
            }
            break;
            // case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            switch (event.jbutton.button)
            {
                case GP2X_BUTTON_A:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        Action_button_R_A(0, Action_button_R_A_Key);
                    }
                    else
                    {
                        Action_button_A(0, Action_button_A_Key);
                    }

                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_B:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        Action_button_R_B(0, Action_button_R_B_Key);
                    }
                    else
                    {
                        Action_button_B(0, Action_button_B_Key);
                    }

                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_X:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        Action_button_R_X(0, Action_button_R_X_Key);
                    }
                    else
                    {
                        Action_button_X(0, Action_button_X_Key);
                    }

                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_Y:
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        Action_button_R_Y(0, Action_button_R_Y_Key);
                    }
                    else
                    {
                        Action_button_Y(0, Action_button_Y_Key);
                    }

                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_SELECT:
                    if ( Game_JButton[GP2X_BUTTON_L] )
                    {
                        if (Action_button_L_Select != NULL)
                        {
                            Action_button_L_Select(0, Action_button_L_Select_Key);
                        }
                        else
                        {
                            Action_button_Select(0, Action_button_Select_Key);
                        }
                    }
                    else if ( Game_JButton[GP2X_BUTTON_R] )
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
                    if ( Game_JButton[GP2X_BUTTON_L] )
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
                    else if ( Game_JButton[GP2X_BUTTON_R] )
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
                    //senquack
                    if ( Game_JButton[GP2X_BUTTON_L] )
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
                    else if ( Game_JButton[GP2X_BUTTON_R] )
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
//senquack
                    else
                    {
                        Action_button_Start(0, Action_button_Start_Key);
                    }
                    // senquack - pausing is now remappable:
//                    else if (!Game_JButton[GP2X_BUTTON_R] && !Game_JButton[GP2X_BUTTON_L])
//                    {
//                        EmulateKey(SDL_KEYUP, SDLK_PAUSE);
//                    }
                    break;
                case GP2X_BUTTON_UP:
                    //senquack
                    if (Game_Paused)
                    {
                        EmulateKey(SDL_KEYUP, SDLK_UP);
                    }
                    else
                    {
                        if ( Game_JButton[GP2X_BUTTON_L] )
                        {
                            Action_button_L_Up(0, Action_button_L_Up_Key);
                        }
                        else
                        {
                            Action_button_Up(0, Action_button_Up_Key);
                        }
                    }
                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_DOWN:
                    //senquack
                    if (Game_Paused)
                    {
                        EmulateKey(SDL_KEYUP, SDLK_DOWN);
                    }
                    else
                    {
                        if ( Game_JButton[GP2X_BUTTON_L] )
                        {
                            Action_button_L_Down(0, Action_button_L_Down_Key);
                        }
                        else
                        {
                            Action_button_Down(0, Action_button_Down_Key);
                        }
                    }
                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_LEFT:
                    //senquack
                    if (Game_Paused)
                    {
                        EmulateKey(SDL_KEYUP, SDLK_LEFT);
                    }
                    else
                    {
                        if ( Game_JButton[GP2X_BUTTON_L] )
                        {
                            Action_button_L_Left(0, Action_button_L_Left_Key);
                        }
                        else
                        {
                            Action_button_Left(0, Action_button_Left_Key);
                        }
                    }
                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_RIGHT:
                    //senquack
                    if (Game_Paused)
                    {
                        EmulateKey(SDL_KEYUP, SDLK_RIGHT);
                    }
                    else
                    {
                        if ( Game_JButton[GP2X_BUTTON_L] )
                        {
                            Action_button_L_Right(0, Action_button_L_Right_Key);
                        }
                        else
                        {
                            Action_button_Right(0, Action_button_Right_Key);
                        }
                    }
                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_UPLEFT:
                    if ( !Game_Paused )
                    {
                        if ( Game_JButton[GP2X_BUTTON_L] )
                        {
                            Action_button_L_UpLeft(0, Action_button_L_UpLeft_Key);
                        }
                        else
                        {
                            Action_button_UpLeft(0, Action_button_UpLeft_Key);
                        }
                    }
                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_UPRIGHT:
                    if ( !Game_Paused )
                    {
                        if ( Game_JButton[GP2X_BUTTON_L] )
                        {
                            Action_button_L_UpRight(0, Action_button_L_UpRight_Key);
                        }
                        else
                        {
                            Action_button_UpRight(0, Action_button_UpRight_Key);
                        }
                    }
                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_DOWNLEFT:
                    if ( !Game_Paused )
                    {
                        if ( Game_JButton[GP2X_BUTTON_L] )
                        {
                            Action_button_L_DownLeft(0, Action_button_L_DownLeft_Key);
                        }
                        else
                        {
                            Action_button_DownLeft(0, Action_button_DownLeft_Key);
                        }
                    }
                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_DOWNRIGHT:
                    if ( !Game_Paused )
                    {
                        if ( Game_JButton[GP2X_BUTTON_L] )
                        {
                            Action_button_L_DownRight(0, Action_button_L_DownRight_Key);
                        }
                        else
                        {
                            Action_button_DownRight(0, Action_button_DownRight_Key);
                        }
                    }
                    Game_JButton[event.jbutton.button] = 0;
                    break;
                case GP2X_BUTTON_R:
                    Game_JButton[GP2X_BUTTON_R] = 0;
                    break;
                case GP2X_BUTTON_L:
                    Game_JButton[GP2X_BUTTON_L] = 0;
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
    //senquack
    int new_mousex, new_mousey;
    static int orig_mousex, orig_mousey;
    SDL_Event event;

    new_mousex = Game_MouseX;
    new_mousey = Game_MouseY;

    //senquack - BEGIN HANDLING BATTLEFIELD SCROLLING:
    if ( (Game_ScrollButtons == GAME_SCROLLBUTTONS_F100) )
    {
        if ( Game_JButton[GP2X_BUTTON_L] &&
              (Game_JButton[GP2X_BUTTON_UP]       || Game_JButton[GP2X_BUTTON_DOWN]    ||
               Game_JButton[GP2X_BUTTON_LEFT]     || Game_JButton[GP2X_BUTTON_RIGHT]   ||
               Game_JButton[GP2X_BUTTON_UPLEFT]   || Game_JButton[GP2X_BUTTON_UPRIGHT] ||
               Game_JButton[GP2X_BUTTON_DOWNLEFT] || Game_JButton[GP2X_BUTTON_DOWNRIGHT] ) )
        {

            // At least one scroll button is pressed
            if ( !Game_CurrentlyScrolling )
            {
                 // We weren't scrolling before this moment, store away cursor position

                if	((Game_MouseX != 0) && (Game_MouseX != 319) &&
                     (Game_MouseY != 0) && (Game_MouseY != (Display_Height == 240) ? 239 : 199))
                {
                    orig_mousex = Game_MouseX;
                    orig_mousey = Game_MouseY;
                }
                else
                {
                    // Cursor is already at edge of screen, store away position in center of
                    // 	screen so when we finish scrolling and return cursor it ceases
                    // 	any and all scrolling
                    orig_mousex = 160;
                    orig_mousey = (Display_Height == 240) ? 120: 100;
                }

                Game_CurrentlyScrolling = 1;
            }

            if ( Game_JButton[GP2X_BUTTON_UP] )
            {
                new_mousex = 160;
                new_mousey = 0;
            }
            else if ( Game_JButton[GP2X_BUTTON_DOWN] )
            {
                new_mousex = 160;
                new_mousey = (Display_Height == 240) ? 239: 199;
            }

            if ( Game_JButton[GP2X_BUTTON_LEFT] )
            {
                if ( Game_JButton[GP2X_BUTTON_UP] )
                {
                    // DPAD-modded F100 being used, "UpLeft" pressed
                    new_mousex = 0;
                    new_mousey = 0;
                }
                else if ( Game_JButton[GP2X_BUTTON_DOWN] )
                {
                    // DPAD-modded F100 being used, "DownLeft" pressed
                    new_mousex = 0;
                    new_mousey = (Display_Height == 240) ? 239: 199;
                }
                else
                {
                    // Only left is pressed
                    new_mousex = 0;
                    new_mousey = (Display_Height == 240) ? 120: 100;
                }
            }
            else if ( Game_JButton[GP2X_BUTTON_RIGHT] )
            {
                if ( Game_JButton[GP2X_BUTTON_UP] )
                {
                    // DPAD-modded F100 being used, "UpRight" pressed
                    new_mousex = 319;
                    new_mousey = 0;
                }
                else if ( Game_JButton[GP2X_BUTTON_DOWN] )
                {
                    // DPAD-modded F100 being used, "DownRight" pressed
                    new_mousex = 319;
                    new_mousey = (Display_Height == 240) ? 239: 199;
                }
                else
                {
                    new_mousex = 319;
                    new_mousey = (Display_Height == 240) ? 120: 100;
                }
            }

            if ( Game_JButton[GP2X_BUTTON_UPLEFT] )
            {
                new_mousex = 0;
                new_mousey = 0;
            }
            else if ( Game_JButton[GP2X_BUTTON_UPRIGHT] )
            {
                new_mousex = 319;
                new_mousey = 0;
            }
            else if ( Game_JButton[GP2X_BUTTON_DOWNLEFT] )
            {
                new_mousex = 0;
                new_mousey = (Display_Height == 240) ? 239: 199;
            }
            else if ( Game_JButton[GP2X_BUTTON_DOWNRIGHT] )
            {
                new_mousex = 319;
                new_mousey = (Display_Height == 240) ? 239: 199;
            }

            SDL_WarpMouse(new_mousex, new_mousey);
            Game_MouseX = new_mousex;
            Game_MouseY = new_mousey;

        }
        else
        {
            // No buttons used for scrolling are pressed. See if we were previously
            // 	scrolling and if so, make game aware we aren't anymore and move
            // 	cursor back to stored position.
            if ( Game_CurrentlyScrolling )
            {
                Game_CurrentlyScrolling = 0;
                SDL_WarpMouse(orig_mousex, orig_mousey);
                Game_MouseX = orig_mousex;
                Game_MouseY = orig_mousey;
            }
        }

    } //if ( Game_ScrollButtons == GAME_SCROLLBUTTONS_F100)

    if ( (Game_ScrollButtons == GAME_SCROLLBUTTONS_F200RH) )
    {
        if	 ( !Game_JButton[GP2X_BUTTON_L] &&
                (Game_JButton[GP2X_BUTTON_UP]       || Game_JButton[GP2X_BUTTON_DOWN]    ||
                 Game_JButton[GP2X_BUTTON_LEFT]     || Game_JButton[GP2X_BUTTON_RIGHT]   ||
                 Game_JButton[GP2X_BUTTON_UPLEFT]   || Game_JButton[GP2X_BUTTON_UPRIGHT] ||
                 Game_JButton[GP2X_BUTTON_DOWNLEFT] || Game_JButton[GP2X_BUTTON_DOWNRIGHT] ) )
        {
            // At least one scroll button is pressed
            if ( !Game_CurrentlyScrolling )
            {
                 // We weren't scrolling before this moment, store away cursor position

                if	((Game_MouseX != 0) && (Game_MouseX != 319) &&
                     (Game_MouseY != 0) && (Game_MouseY != (Display_Height == 240) ? 239 : 199))
                {
                    orig_mousex = Game_MouseX;
                    orig_mousey = Game_MouseY;
                }
                else
                {
                    // Cursor is already at edge of screen, store away position in center of
                    // 	screen so when we finish scrolling and return cursor it ceases
                    // 	any and all scrolling
                    orig_mousex = 160;
                    orig_mousey = (Display_Height == 240) ? 120: 100;
                }

                Game_CurrentlyScrolling = 1;
            }

            if ( Game_JButton[GP2X_BUTTON_UP] )
            {
                new_mousex = 160;
                new_mousey = 0;
            }
            else if ( Game_JButton[GP2X_BUTTON_DOWN] )
            {
                new_mousex = 160;
                new_mousey = (Display_Height == 240) ? 239: 199;
            }
            else if ( Game_JButton[GP2X_BUTTON_LEFT] )
            {
                new_mousex = 0;
                new_mousey = (Display_Height == 240) ? 120: 100;
            }
            else if ( Game_JButton[GP2X_BUTTON_RIGHT] )
            {
                new_mousex = 319;
                new_mousey = (Display_Height == 240) ? 120: 100;
            }
            else if ( Game_JButton[GP2X_BUTTON_UPLEFT] )
            {
                new_mousex = 0;
                new_mousey = 0;
            }
            else if ( Game_JButton[GP2X_BUTTON_UPRIGHT] )
            {
                new_mousex = 319;
                new_mousey = 0;
            }
            else if ( Game_JButton[GP2X_BUTTON_DOWNLEFT] )
            {
                new_mousex = 0;
                new_mousey = (Display_Height == 240) ? 239: 199;
            }
            else if ( Game_JButton[GP2X_BUTTON_DOWNRIGHT] )
            {
                new_mousex = 319;
                new_mousey = (Display_Height == 240) ? 239: 199;
            }

            SDL_WarpMouse(new_mousex, new_mousey);
            Game_MouseX = new_mousex;
            Game_MouseY = new_mousey;

        }
        else
        {
            // No buttons used for scrolling are pressed. See if we were previously
            // 	scrolling and if so, make game aware we aren't anymore and move
            // 	cursor back to stored position.
            if ( Game_CurrentlyScrolling )
            {
                Game_CurrentlyScrolling = 0;
                SDL_WarpMouse(orig_mousex, orig_mousey);
                Game_MouseX = orig_mousex;
                Game_MouseY = orig_mousey;
            }
        }
    } //if ( Game_ScrollButtons == GAME_SCROLLBUTTONS_F200RH)

    if ( (Game_ScrollButtons == GAME_SCROLLBUTTONS_F200LH) )
    {
        if  ( !Game_JButton[GP2X_BUTTON_R] &&
               (Game_JButton[GP2X_BUTTON_A] || Game_JButton[GP2X_BUTTON_B] ||
                Game_JButton[GP2X_BUTTON_X] || Game_JButton[GP2X_BUTTON_Y] ) )
        {
            // At least one scroll button is pressed
            if ( !Game_CurrentlyScrolling )
            {
                 // We weren't scrolling before this moment, store away cursor position

                if	((Game_MouseX != 0) && (Game_MouseX != 319) &&
                     (Game_MouseY != 0) && (Game_MouseY != (Display_Height == 240) ? 239 : 199))
                {
                    orig_mousex = Game_MouseX;
                    orig_mousey = Game_MouseY;
                }
                else
                {
                    // Cursor is already at edge of screen, store away position in center of
                    // 	screen so when we finish scrolling and return cursor it ceases
                    // 	any and all scrolling
                    orig_mousex = 160;
                    orig_mousey = (Display_Height == 240) ? 120: 100;
                }

                Game_CurrentlyScrolling = 1;
            }

            if ( Game_JButton[GP2X_BUTTON_Y] )
            {
                new_mousex = 160;
                new_mousey = 0;
            }
            else if ( Game_JButton[GP2X_BUTTON_X] )
            {
                new_mousex = 160;
                new_mousey = (Display_Height == 240) ? 239: 199;
            }

            if ( Game_JButton[GP2X_BUTTON_A] )
            {
                if ( Game_JButton[GP2X_BUTTON_Y] )
                {
                    // "UpLeft" pressed
                    new_mousex = 0;
                    new_mousey = 0;
                }
                else if ( Game_JButton[GP2X_BUTTON_X] )
                {
                    // "DownLeft" pressed
                    new_mousex = 0;
                    new_mousey = (Display_Height == 240) ? 239: 199;
                }
                else
                {
                    // Only "left" is pressed
                    new_mousex = 0;
                    new_mousey = (Display_Height == 240) ? 120: 100;
                }
            }
            else if ( Game_JButton[GP2X_BUTTON_B] )
            {
                if ( Game_JButton[GP2X_BUTTON_Y] )
                {
                    // "UpRight" pressed
                    new_mousex = 319;
                    new_mousey = 0;
                }
                else if ( Game_JButton[GP2X_BUTTON_X] )
                {
                    // "DownRight" pressed
                    new_mousex = 319;
                    new_mousey = (Display_Height == 240) ? 239: 199;
                }
                else
                {
                    // Only "right" is pressed
                    new_mousex = 319;
                    new_mousey = (Display_Height == 240) ? 120: 100;
                }
            }

            SDL_WarpMouse(new_mousex, new_mousey);
            Game_MouseX = new_mousex;
            Game_MouseY = new_mousey;

        }
        else
        {
            // No buttons used for scrolling are pressed. See if we were previously
            // 	scrolling and if so, make game aware we aren't anymore and move
            // 	cursor back to stored position.
            if ( Game_CurrentlyScrolling )
            {
                Game_CurrentlyScrolling = 0;
                SDL_WarpMouse(orig_mousex, orig_mousey);
                Game_MouseX = orig_mousex;
                Game_MouseY = orig_mousey;
            }
        }

    } //if ( Game_ScrollButtons == GAME_SCROLLBUTTONS_F200LH)

    //senquack - If not using touchscreen and also not using scroll hotkey combo, emulate
    //				mouse.
    if ( !Game_UsingTouchscreen && !Game_CurrentlyScrolling && !Game_JButton[GP2X_BUTTON_L] )
    {
        int deltax, deltay;

        deltax = 0;
        deltay = 0;

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

        if (!Game_JButton[GP2X_BUTTON_R] &&
            (Game_TimerTick & 3) == 0
            )
        {
            deltax = 0;
            deltay = 0;
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
}

