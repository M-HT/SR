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


static void (*Action_button_A)(int pressed, int key);	/* procedure for A button */
static void (*Action_button_B)(int pressed, int key);	/* procedure for B button */
static void (*Action_button_X)(int pressed, int key);	/* procedure for X button */
static void (*Action_button_Y)(int pressed, int key);	/* procedure for Y button */
static void (*Action_button_Select)(int pressed, int key);	/* procedure for Select button */
static void (*Action_button_Start)(int pressed, int key);	/* procedure for Start button */
static void (*Action_button_VolUp)(int pressed, int key);	/* procedure for Volume+ button */
static void (*Action_button_VolDown)(int pressed, int key);	/* procedure for Volume- button */

static void (*Action_button_R_A)(int pressed, int key);	/* procedure for R+A button */
static void (*Action_button_R_B)(int pressed, int key);	/* procedure for R+B button */
static void (*Action_button_R_X)(int pressed, int key);	/* procedure for R+X button */
static void (*Action_button_R_Y)(int pressed, int key);	/* procedure for R+Y button */
static void (*Action_button_R_Select)(int pressed, int key);	/* procedure for R+Select button */
static void (*Action_button_R_Start)(int pressed, int key);	/* procedure for R+Start button */
static void (*Action_button_R_VolUp)(int pressed, int key);		/* procedure for R+Volume+ button */
static void (*Action_button_R_VolDown)(int pressed, int key);	/* procedure for R+Volume- button */

static int Action_button_A_Key, Action_button_B_Key, Action_button_X_Key,
           Action_button_Y_Key, Action_button_Select_Key, Action_button_Start_Key,
           Action_button_VolUp_Key, Action_button_VolDown_Key;

static int Action_button_R_A_Key, Action_button_R_B_Key, Action_button_R_X_Key,
           Action_button_R_Y_Key, Action_button_R_Select_Key, Action_button_R_Start_Key,
           Action_button_R_VolUp_Key, Action_button_R_VolDown_Key;


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


// GAME_JOYSTICK != JOYSTICK_NONE

// GAME_JOYSTICK = JOYSTICK_GP2X
#if !defined(SDLK_ENTER)
    #define SDLK_ENTER SDLK_RETURN
#endif
static void Action_key(int pressed, int key)
{
    EmulateKey((pressed)?SDL_KEYDOWN:SDL_KEYUP, key);
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

// GAME_JOYSTICK != JOYSTICK_NONE

// GAME_JOYSTICK = JOYSTICK_GP2X
    Action_button_A = &Action_key;
    Action_button_B = &Action_key;
    Action_button_X = &Action_key;
    Action_button_Y = &Action_key;
    Action_button_Select = &Action_key;
    Action_button_Start = &Action_key;
    Action_button_VolUp = &Action_volume_increase;
    Action_button_VolDown = &Action_volume_decrease;

    Action_button_A_Key = SDLK_a;
    Action_button_B_Key = SDLK_b;
    Action_button_X_Key = SDLK_x;
    Action_button_Y_Key = SDLK_y;
    Action_button_Select_Key = SDLK_RETURN;
    Action_button_Start_Key = SDLK_ESCAPE;
    Action_button_VolUp_Key = 0;
    Action_button_VolDown_Key = 0;

    Action_button_R_A = NULL;
    Action_button_R_B = NULL;
    Action_button_R_X = NULL;
    Action_button_R_Y = NULL;
    Action_button_R_Select = NULL;
    Action_button_R_Start = NULL;
    Action_button_R_VolUp = NULL;
    Action_button_R_VolDown = NULL;

    Action_button_R_A_Key = 0;
    Action_button_R_B_Key = 0;
    Action_button_R_X_Key = 0;
    Action_button_R_Y_Key = 0;
    Action_button_R_Select_Key = 0;
    Action_button_R_Start_Key = 0;
    Action_button_R_VolUp_Key = 0;
    Action_button_R_VolDown_Key = 0;
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
                case GP2X_BUTTON_START:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_Start != NULL)
                        {
                            Action_button_R_Start(1, Action_button_R_Start_Key);
                        }
                        else
                        {
                            Action_button_Start(1, Action_button_Start_Key);
                        }
                    }
                    else
                    {
                        Action_button_Start(1, Action_button_Start_Key);
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
                    Game_JButton[event.jbutton.button] = 1;
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
                case GP2X_BUTTON_START:
                    if ( Game_JButton[GP2X_BUTTON_R] )
                    {
                        if (Action_button_R_Start != NULL)
                        {
                            Action_button_R_Start(0, Action_button_R_Start_Key);
                        }
                        else
                        {
                            Action_button_Start(0, Action_button_Start_Key);
                        }
                    }
                    else
                    {
                        Action_button_Start(0, Action_button_Start_Key);
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
                case GP2X_BUTTON_UP:
                case GP2X_BUTTON_DOWN:
                case GP2X_BUTTON_LEFT:
                case GP2X_BUTTON_RIGHT:
                case GP2X_BUTTON_UPLEFT:
                case GP2X_BUTTON_UPRIGHT:
                case GP2X_BUTTON_DOWNLEFT:
                case GP2X_BUTTON_DOWNRIGHT:
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
}

