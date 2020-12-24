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

static int Action_toggle_scaling(int pressed, int key, SDL_Event *event)
{
    if (pressed == 0)
    {
        Display_ChangeMode = 1;
    }
    return 1;
}



void Init_Input(void)
{
    Game_Joystick = 0;
}

void Init_Input2(void)
{
    // without this code, calling localtime in function Game_readdir (called by Game_dos_findfirst) in file Albion-proc-vfs.c crashes on Pandora
    {
        struct stat statbuf;

        statbuf.st_mtime = 0;
        localtime(&(statbuf.st_mtime));
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
    switch (_event->type)
    {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            _event->key.keysym.mod = (SDLMod) ((unsigned int) _event->key.keysym.mod & ~(KMOD_LCTRL));

            switch ((int) _event->key.keysym.sym)
            {
                case PANDORA_BUTTON_SELECT:
                    return Action_toggle_scaling ((_event->type == SDL_KEYDOWN)?1:0, 0, _event);
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
}

