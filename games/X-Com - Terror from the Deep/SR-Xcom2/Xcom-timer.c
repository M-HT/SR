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

#include "Game_defs.h"
#include "Game_vars.h"
#include "Game_thread.h"
#include "Xcom-timer.h"
#include "Xcom-proc.h"
#include "Xcom-proc-events.h"
#include "Xcom-sound.h"
#include "audio.h"
#include "input.h"


void Game_RunTimer(void)
{
    int DelayGame;

    Game_LastAudio = Game_SDLTicks;
    if (Game_Sound)
    {
        Game_ProcessAudio();
    }

    if (!Game_TimerRunning)
    {
        if (X86_InterruptFlag)
        {
            DelayGame = 0;
            while (Game_TimerRun != Game_TimerTick)
            {
                DelayGame = 1;

                if (Thread_Exit)
                {
                    Game_StopMain();
                }

                Game_TimerRun++;

                if (/*X86_InterruptFlag &&*/ Game_InterruptTable[8] != 0)
                {
                    Game_TimerRunning = 1;
                    switch (Game_Executable)
                    {
                    #if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_GEOSCAPE))
                        case EXE_GEOSCAPE:
                            Game_RunTimer_Asm((void *)geoscape_update_timer);
                            break;
                    #endif
                    #if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_TACTICAL))
                        case EXE_TACTICAL:
                            Game_RunTimer_Asm((void *)tactical_update_timer);
                            break;
                    #endif
                        default:
                            break;
                    }
                    Game_TimerRunning = 0;
                }

                /*if (Game_InterruptTable[9] != 0)*/
                {
                    Game_ProcessKEvents();
                }
            }
            if (Game_Delay_Game >= 0 && DelayGame)
            {
                SDL_Delay(Game_Delay_Game);
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
        Game_ProcessAudio();
    }
    else
    {
        Game_RunTimer();
    }
}

int Game_TimerThread(void *data)
{
    uint32_t LastTimerTick, TimerTickCounter, LastVSyncTick, VSyncTickCounter;
    int SkipDelay, TimerAction, VSyncAction;

    LastVSyncTick = LastTimerTick = SDL_GetTicks();
    VSyncTickCounter = TimerTickCounter = 0;
    VSyncAction = TimerAction = SkipDelay = 0;

    while (1)
    {
        if (SkipDelay)
        {
            SkipDelay = 0;
        }
        else
        {
            SDL_Delay(1);
        }

        {
            uint32_t Ticks, Diff;

            Ticks = SDL_GetTicks();
            Game_SDLTicks = Ticks;
            Diff = (TimerTickCounter)?14:15;

            if (Ticks - LastTimerTick >= Diff)
            {
                LastTimerTick+=Diff;

                if (TimerTickCounter)
                {
                    TimerTickCounter--;
                }
                else
                {
                    TimerTickCounter=2;
                }

                TimerAction = 1;
            }

            Diff = (VSyncTickCounter)?17:16;

            if (Ticks - LastVSyncTick >= Diff)
            {
                LastVSyncTick+=Diff;

                if (VSyncTickCounter)
                {
                    VSyncTickCounter--;
                }
                else
                {
                    VSyncTickCounter=2;
                }

                VSyncAction = 1;
            }
        }

        if (Thread_Exit && Thread_Exited)
        {
            return 0;
        }

        if (TimerAction)
        {
            TimerAction = 0;

            Game_TimerTick++;

            if ( !Thread_Exit )
            {
                Handle_Timer_Input_Event();
            }

            SkipDelay = 1;
        }

        if (VSyncAction)
        {
            VSyncAction = 0;

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

            Game_VSyncTick++;

            Game_FlipScreen();

            SkipDelay = 1;
        }
    }

    return 0;
}

