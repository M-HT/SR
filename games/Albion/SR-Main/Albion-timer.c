/**
 *
 *  Copyright (C) 2016-2022 Roman Pauer
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

#include "Game_defs.h"
#include "Game_vars.h"
#include "Game_thread.h"
#include "Albion-timer.h"
#include "Albion-proc-events.h"
#include "audio.h"
#include "input.h"

void Game_RunTimer(void)
{
    if (!Game_TimerRunning)
    {
        if (X86_InterruptFlag)
        {
            while (Game_TimerRun != Game_TimerTick)
            {
                if (Thread_Exit)
                {
                    Game_StopMain();
                }

                Game_TimerRun++;

                if (/*X86_InterruptFlag &&*/ Game_InterruptTable[8] != NULL)
                {
                    Game_TimerRunning = 1;
                    Game_RunTimer_Asm();
                    Game_TimerRunning = 0;
                }

                if (Game_InterruptTable[9] != NULL)
                {
                    int ret;
                    SDL_Event event;

                    Game_TimerRunning = 1;
                    ret = Game_ProcessMEvents();
                    if (!ret) Game_ProcessKEvents();

                    event.type = SDL_USEREVENT;
                    event.user.code = EC_DISPLAY_FLIP_START;
                    event.user.data1 = NULL;
                    event.user.data2 = NULL;

                    SDL_PushEvent(&event);

                    Game_TimerRunning = 0;
                }
            }
        }
        else
        {
            if (Thread_Exit)
            {
                Game_StopMain();
            }
        }
    }
    else
    {
        if (Thread_Exit)
        {
            Game_StopMain();
        }
    }
}

void Game_RunTimerDelay(void)
{
    if (Game_TimerRun == Game_TimerTick)
    {
        SDL_Delay(1);
    }
    else
    {
        Game_RunTimer();
    }
}

int Game_TimerThread(void *data)
{
    uint32_t LastTimerTick, TickCounter;

    LastTimerTick = SDL_GetTicks();
    TickCounter = 0;

    while (1)
    {
        SDL_Delay(1);

        {
            register uint32_t Ticks, Diff;

            Ticks = SDL_GetTicks();
            Diff = (TickCounter)?17:16;

            if (Ticks - LastTimerTick >= Diff)
            {
                LastTimerTick+=Diff;

                if (TickCounter)
                {
                    TickCounter--;
                }
                else
                {
                    TickCounter=2;
                }
            }
            else
            {
                continue;
            }
        }

        if (Thread_Exit && Thread_Exited)
        {
            return 0;
        }

        if (Game_Paused) continue;

        if (Game_VolumeDelta == 1)
        {
//senquack - SOUND STUFF
            Change_HW_Audio_Volume(1);
        }
        else if (Game_VolumeDelta == -1)
        {
//senquack - SOUND STUFF
            Change_HW_Audio_Volume(-1);
        }

        if (SMK_Playing) continue;

        Game_TimerTick++;
        Game_VSyncTick++;

        if (Thread_Exit) continue;

        Handle_Timer_Input_Event();
    }

    return 0;
}
