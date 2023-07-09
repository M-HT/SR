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
#include <unistd.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
#else
    #include <SDL/SDL.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Game_scalerplugin.h"
#include "Game_misc.h"
#include "Game_thread.h"
#include "Xcom-proc.h"
#include "Xcom-proc-events.h"
#include "Xcom-music.h"
#include "Xcom-sound.h"
#include "main.h"
#include "display.h"

#if defined(__GNU_LIBRARY__) || defined(__GLIBC__)
    #ifndef __USE_GNU
        #define __USE_GNU 1
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

static __attribute__ ((noinline)) void Game_CleanAfterMain(void)
{
    Game_Executable = 0;

    // clear mouse and interrupt table
    memset(&Game_InterruptTable, 0, sizeof(Game_InterruptTable));
    memset(&Game_MouseTable, 0, sizeof(Game_MouseTable));

    if (Game_Music)
    {
        // stop playing music
        Game_stop_sequence();
    }

    if (Game_Sound)
    {
        // stop playing sound
        Game_StopSound();
        Game_AudioPending = 0;
    }

    // close opened files
    Game_list_clear(&Game_FopenList, (void (*)(uintptr_t)) &fclose);
    Game_list_clear(&Game_DopenList, (void (*)(uintptr_t)) &close);

    // sync data to disk
    fflush(Game_stdout);
    fflush(Game_stderr);
    Game_Sync();

    // free allocated memory
    Game_list_clear(&Game_MallocList, (void (*)(uintptr_t)) &free);
    Game_list_clear(&Game_AllocateMemoryList, (void (*)(uintptr_t)) &free);
}

#if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_GEOSCAPE))
static __attribute__ ((noinline)) int Game_Main_Geoscape(const char *arg1)
{
    const static char main_filename[] = "GEOSCAPE.EXE";
    const char *main_argv[3];

    main_argv[0] = main_filename;
    main_argv[1] = arg1;
    main_argv[2] = NULL;

#if (EXE_BUILD == EXE_COMBINED)
    memcpy(&geoscape_data_begin, Geoscape_DataBackup, &geoscape_data_end - &geoscape_data_begin);
#endif

    memset(&geoscape_bss_begin, 0, &geoscape_bss_end - &geoscape_bss_begin);

    if (Thread_Exit)
    {
        return 1;
    }
    else
    {
        Game_Executable = EXE_GEOSCAPE;
        return Game_Main_Asm((arg1 != NULL)?2:1, main_argv, (void *)geoscape_main_);
    }
}
#endif

#if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_TACTICAL))
static __attribute__ ((noinline)) int Game_Main_Tactical(const char *arg1)
{
    const static char main_filename[] = "TACTICAL.EXE";
    const char *main_argv[3];

    main_argv[0] = main_filename;
    main_argv[1] = arg1;
    main_argv[2] = NULL;

#if (EXE_BUILD == EXE_COMBINED)
    memcpy(&tactical_data_begin, Tactical_DataBackup, &tactical_data_end - &tactical_data_begin);
#endif

    memset(&tactical_bss_begin, 0, &tactical_bss_end - &tactical_bss_begin);

    if (Thread_Exit)
    {
        return 1;
    }
    else
    {
        Game_Executable = EXE_TACTICAL;
        return Game_Main_Asm((arg1 != NULL)?2:1, main_argv, (void *)tactical_main_);
    }
}
#endif

#if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_INTRO))
static __attribute__ ((noinline)) int Game_Main_Intro(void)
{
    const static char main_filename[] = "INTRO.EXE";
    const char *main_argv[2];

    main_argv[0] = main_filename;
    main_argv[1] = NULL;

    memset(&intro_bss_begin, 0, &intro_bss_end - &intro_bss_begin);

    if (Thread_Exit)
    {
        return 1;
    }
    else
    {
        Game_Executable = EXE_INTRO;
        return Game_Main_Asm(1, main_argv, (void *)intro_main_);
    }
}
#endif

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

int Game_MainThread(void *data)
{

    Game_CleanState(Thread_Exit);

    ChangeThreadPriority();

    {
        int ret;

#if (EXE_BUILD == EXE_GEOSCAPE)
        ret = Game_Main_Geoscape(main_argv[1]);
        Game_CleanAfterMain();
#elif (EXE_BUILD == EXE_TACTICAL)
        ret = Game_Main_Tactical(main_argv[1]);
        Game_CleanAfterMain();
#elif (EXE_BUILD == EXE_INTRO)
        ret = Game_Main_Intro();
        Game_CleanAfterMain();
#else
        if (Game_PlayIntro)
        {
            ret = Game_Main_Intro();
            Game_CleanAfterMain();
        }
        else ret = 0;

        if (!ret)
        {
            if (Game_MidiRemapGM2MT32)
            {
                Game_MidiRemapGM2MT32 = 0;
                Game_SoundCfg.MusicDriver = 1;          // roland lapc-1 / mt32
                Game_SoundCfg.MusicBasePort = 0x0330;   // MT32 base port
            }

            ret = Game_Main_Geoscape("0");
            Game_CleanAfterMain();

            while (ret >= 2)
            {
                ret = Game_Main_Tactical("1");
                Game_CleanAfterMain();
                if (!ret) break;
                ret = Game_Main_Geoscape("1");
                Game_CleanAfterMain();
            }
        }
#endif

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
