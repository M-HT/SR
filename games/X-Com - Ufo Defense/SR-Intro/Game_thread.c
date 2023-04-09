/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
#else
    #include <SDL/SDL.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Game_scalerplugin.h"
#include "Game_thread.h"
#include "Intro-proc-events.h"
#include "main.h"
#include "display.h"

#if defined(__GNU_LIBRARY__) || defined(__GLIBC__)
    #ifndef __USE_GNU
        #define __USE_GNU
    #endif
    #include <pthread.h>
#endif

static void ChangeThreadPriority(void)
{
#if defined(SCHED_BATCH)
    struct sched_param param;

    param.sched_priority = 0;
    pthread_setschedparam(pthread_self(), SCHED_BATCH, &param);
#endif
}

int Game_Main(void)
{
#define MLEN 9

    const static char main_filename[MLEN+1] = "INTRO.EXE";
    char main_filename_local[MLEN+1];
    char *main_filename_local_ptr[3];

    memcpy(main_filename_local, main_filename, MLEN+1);
    if (main_arg1 != NULL) strcpy(main_arg1, main_argv[1]);

    main_filename_local_ptr[0] = main_filename_local;
    main_filename_local_ptr[1] = main_arg1;
    main_filename_local_ptr[2] = NULL;

    if (Thread_Exit)
    {
        return 1;
    }
    else
    {
        return Game_Main_Asm(1, main_filename_local_ptr);
    }

#undef MLEN
}

void Game_StopMain(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "Stopping main...\n");
#endif
    if (Game_Music)
    {
        Mix_HaltMusic();
    }
    if (Game_Sound)
    {
        Mix_HaltChannel(-1);
    }

    Game_TimerTick += 2;
    Game_StopMain_Asm();
}

void *Game_AllocateMemory(uint32_t size)
{
    void *mem;

    mem = malloc(size);

    if (mem != NULL)
    {
        if (Game_NextMemory != 256)
        {
            Game_AllocatedMemory[Game_NextMemory] = mem;
            Game_NextMemory++;
        }
        else
        {
            int i;

            for (i = 0; i < 256; i++)
            {
                if (Game_AllocatedMemory[i] == NULL)
                {
                    Game_AllocatedMemory[i] = mem;
                    break;
                }
            }
        }
    }

    return mem;
}

void Game_FreeMemory(void *mem)
{
    unsigned int i;

    for (i = 0; i < 256; i++)
    {
        if (Game_AllocatedMemory[i] == mem)
        {
            Game_AllocatedMemory[i] = NULL;
            if (i + 1 == Game_NextMemory) Game_NextMemory--;
            break;
        }
    }

    free(mem);
}

int Game_MainThread(void *data)
{

    Game_CleanState(Thread_Exit);

    ChangeThreadPriority();

    {
        int ret;

        ret = Game_Main();

#if defined(__DEBUG__)
    fprintf(stderr, "Exit code: %i\n", ret);
#endif

        Game_ExitCode = ret;
    }

    Thread_Exited = 1;

    {
        SDL_Event event;

        event.type = SDL_USEREVENT;
        event.user.code = EC_PROGRAM_QUIT;
        event.user.data1 = NULL;
        event.user.data2 = NULL;

        SDL_PushEvent(&event);
    }

    return 0;
}

int Game_FlipThread(void *data)
{
    SDL_Event event;
#if !defined(USE_SDL2)
    int clear_screen;
#endif

#undef FPS_WRITE

#ifdef FPS_WRITE
    uint32_t CurrentTicks, LastDisplayTicks, NumDisplay, LastTimer, CurrentTimer;

    LastDisplayTicks = SDL_GetTicks();
    NumDisplay = 0;
    LastTimer = Game_VSyncTick;
#endif

#if !defined(USE_SDL2)
    clear_screen = 0;
#endif

    while (1)
    {
        SDL_SemWait(Game_FlipSem);
        if (Thread_Exit) return 0;

#ifdef FPS_WRITE
        CurrentTicks = SDL_GetTicks();
        if (CurrentTicks - LastDisplayTicks >= 1000 )
        {
            CurrentTimer = Game_VSyncTick;

fprintf(stderr, "fps: %.3f    tps: %.3f\n", (float) NumDisplay * 1000 / (CurrentTicks - LastDisplayTicks), (float) (CurrentTimer - LastTimer) * 1000 / (CurrentTicks - LastDisplayTicks));

            NumDisplay = 1;
            LastDisplayTicks = CurrentTicks;
            LastTimer = CurrentTimer;
        }
        else
        {
            NumDisplay++;
        }
#endif

        SDL_LockMutex(Game_ScreenMutex);

        if (Game_DisplayActive)
        {
        #ifdef USE_SDL2
            Display_Flip_Procedure(Game_FrameBuffer, Game_TextureData);

            if (Scaler_ScaleTextureData)
            {
                ScalerPlugin_scale(Scaler_ScaleFactor, Game_TextureData, Game_ScaledTextureData, Render_Width, Render_Height, 1);
            }
        #else
        #ifdef ALLOW_OPENGL
            if (Game_UseOpenGL)
            {
                Display_Flip_Procedure(Game_FrameBuffer, Game_TextureData);

                if (Scaler_ScaleTextureData)
                {
                    ScalerPlugin_scale(Scaler_ScaleFactor, Game_TextureData, Game_ScaledTextureData, Render_Width, Render_Height, 1);
                }
            }
            else
        #endif
            {
                /* ??? */

                SDL_LockSurface(Game_Screen);

                if (Display_ChangeMode != 0)
                {
                    int mousex, mousey;

                    Game_GetGameMouse(&mousex, &mousey);

                    if (Change_Display_Mode(Display_ChangeMode))
                    {
                        clear_screen = 3;
                    }

                    Display_ChangeMode = 0;

                    Game_VideoAspectX = ((320-1) << 16) / (Picture_Width-1);
                    Game_VideoAspectY = ((200-1) << 16) / (Picture_Height-1);

                    Game_VideoAspectXR = ((Picture_Width-1) << 16) / (320-1);
                    Game_VideoAspectYR = ((Picture_Height-1) << 16) / (200-1);

                    Game_RepositionMouse(mousex, mousey);
                }

                if (clear_screen)
                {
                    SDL_Rect rect;

                    clear_screen--;

                    rect.x = 0;
                    rect.y = 0;
                    rect.w = Game_Screen->w;
                    rect.h = Game_Screen->h;
                    SDL_FillRect(Game_Screen, &rect, 0);
                }

                Display_Flip_Procedure(Game_FrameBuffer, Game_Screen->pixels);

                /* ??? */
                SDL_UnlockSurface(Game_Screen);
                SDL_Flip(Game_Screen);
            }
        #endif
        }

        SDL_UnlockMutex(Game_ScreenMutex);

        event.type = SDL_USEREVENT;
        event.user.code = EC_DISPLAY_FLIP_FINISH;
        event.user.data1 = NULL;
        event.user.data2 = NULL;

        SDL_PushEvent(&event);

        if (Thread_Exit) return 0;
    }
}
