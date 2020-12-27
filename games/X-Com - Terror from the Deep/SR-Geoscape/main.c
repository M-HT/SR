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

#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_thread.h>
    #include <SDL2/SDL_version.h>
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL/SDL.h>
    #include <SDL/SDL_thread.h>
    #include <SDL/SDL_version.h>
    #include <SDL/SDL_mixer.h>
#endif
#include "Game_defs.h"

#define DEFINE_VARIABLES
    #include "Game_vars.h"
#undef DEFINE_VARIABLES

#include "main.h"
#include "Geoscape-proc.h"
#include "Geoscape-proc-vfs.h"
#include "Geoscape-music.h"
#include "Geoscape-sound.h"
#include "Geoscape-timer.h"
#include "Geoscape-music-midiplugin.h"
#include "Geoscape-music-midiplugin2.h"
#include "Game_config.h"
#include "Game_thread.h"
#include "virtualfs.h"
#include "display.h"
#include "audio.h"
#include "input.h"

static void Display_RecalculateResolution(int w, int h)
{
    if (Display_FSType == 1)
    {
        if ((((double)w) / h) > (((double)Display_Width) / Display_Height))
        {
            Picture_Height = h;
            Picture_Width = (((double)h) * Display_Width) / Display_Height;
        }
        else
        {
            Picture_Width = w;
            Picture_Height = (((double)w) * Display_Height) / Display_Width;
        }
    }
    else
    {
        Picture_Width = w;
        Picture_Height = h;
    }

    Display_Width = w;
    Display_Height = h;

    Picture_Position_UL_X = (Display_Width - Picture_Width) / 2;
    Picture_Position_UL_Y = (Display_Height - Picture_Height) / 2;
    Picture_Position_BR_X = Picture_Position_UL_X + Picture_Width - 1;
    Picture_Position_BR_Y = Picture_Position_BR_Y + Picture_Height - 1;

    Game_VideoAspectX = (320 << 16) / Picture_Width;
    Game_VideoAspectY = (200 << 16) / Picture_Height;

    Game_VideoAspectXR = (Picture_Width << 16) / 320;
    Game_VideoAspectYR = (Picture_Height << 16) / 200;
}

static void Game_Display_Create(void)
{
    SDL_LockMutex(Game_ScreenMutex);

#ifdef USE_SDL2
    if (Display_Fullscreen && Display_FSType)
    {
        Game_Window = SDL_CreateWindow("SDL Geoscape", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_INPUT_GRABBED);
    }
    else
    {
        Uint32 flags;

        if (Display_Fullscreen)
        {
            flags = SDL_WINDOW_FULLSCREEN | SDL_WINDOW_INPUT_GRABBED;
        }
        else
        {
            flags = (Display_MouseLocked)?SDL_WINDOW_INPUT_GRABBED:0;
        }

        Game_Window = SDL_CreateWindow("SDL Geoscape", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Display_Width, Display_Height, flags);
    }

    if (Game_Window != NULL)
    {
        Game_Renderer = SDL_CreateRenderer(Game_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (Game_Renderer != NULL)
        {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

            if (Display_Fullscreen && Display_FSType)
            {
                int w, h;
                SDL_Rect viewport;

                if (SDL_GetRendererOutputSize(Game_Renderer, &w, &h))
                {
                    SDL_GetWindowSize(Game_Window, &w, &h);
                }

                Display_RecalculateResolution(w, h);

                viewport.x = Picture_Position_UL_X;
                viewport.y = Picture_Position_UL_Y;
                viewport.w = Picture_Width;
                viewport.h = Picture_Height;
                SDL_RenderSetViewport(Game_Renderer, &viewport);
            }
        }
        else
        {
            SDL_DestroyWindow(Game_Window);
            Game_Window = NULL;
        }
    }

    if (Game_Window != NULL)
    {
        Game_Texture = SDL_CreateTexture(Game_Renderer, (Display_Bitsperpixel == 32)?SDL_PIXELFORMAT_ARGB8888:SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, Render_Width, Render_Height);
        if (Game_Texture == NULL)
        {
            SDL_DestroyRenderer(Game_Renderer);
            Game_Renderer = NULL;
            SDL_DestroyWindow(Game_Window);
            Game_Window = NULL;
        }
    }

    if (Game_Window != NULL)
    {
        int mousex, mousey;

        SDL_ShowCursor(SDL_DISABLE);

        if (Display_MouseLocked)
        {
            Game_OldCursor = SDL_GetCursor();
            SDL_SetCursor(Game_NoCursor);

            SDL_SetRelativeMouseMode(SDL_TRUE);

            SDL_GetWindowSize(Game_Window, &mousex, &mousey);
            SDL_WarpMouseInWindow(Game_Window, mousex / 2, mousey / 2);
        }
        else
        {
            if (Display_Fullscreen)
            {
                Game_OldCursor = SDL_GetCursor();
                SDL_SetCursor(Game_NoCursor);

                SDL_GetWindowSize(Game_Window, &mousex, &mousey);
                SDL_WarpMouseInWindow(Game_Window, mousex / 2, mousey / 2);
            }
            else
            {
                if (Game_MouseCursor == 2)
                {
                    Game_OldCursor = SDL_GetCursor();
                    SDL_SetCursor(Game_NoCursor);
                }
                else if (Game_MouseCursor == 1)
                {
                    Game_OldCursor = SDL_GetCursor();
                    SDL_SetCursor(Game_MinCursor);
                }

                SDL_GetMouseState(&mousex, &mousey);
                SDL_WarpMouseInWindow(Game_Window, mousex, mousey);
            }

            SDL_ShowCursor(SDL_ENABLE);
        }

        Game_DisplayActive = 1;
    }
#else
    {
        Uint32 flags;

    #ifdef ALLOW_OPENGL
        if (Game_UseOpenGL)
        {
            if (Display_Bitsperpixel == 32)
            {
                SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
                SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
                SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
            }
            else if (Display_Bitsperpixel == 16)
            {
                SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
                SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
                SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
            }
            SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

            flags = SDL_OPENGL;
            if (Display_Fullscreen)
            {
                flags |= SDL_FULLSCREEN | SDL_NOFRAME;
            }
        }
        else
    #endif
        if (Display_Fullscreen)
        {
            flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN | SDL_NOFRAME;
        }
        else
        {
            flags = SDL_SWSURFACE;
        }

        if (Display_Fullscreen && Display_FSType)
        {
            Game_Screen = SDL_SetVideoMode (0, 0, 0, flags);

            if (Game_Screen != NULL)
            {
                Display_RecalculateResolution(Game_Screen->w, Game_Screen->h);
            }
        }
        else
        {
            Game_Screen = SDL_SetVideoMode (Display_Width, Display_Height, Display_Bitsperpixel, flags);
        }
    }

    if (Game_Screen != NULL)
    {
        int mousex, mousey;

        SDL_ShowCursor(SDL_DISABLE);
        SDL_WM_SetCaption ("SDL Geoscape", NULL);

        Reposition_Display();

        if (Display_MouseLocked)
        {
            Game_OldCursor = SDL_GetCursor();
            SDL_SetCursor(Game_NoCursor);

            SDL_WM_GrabInput(SDL_GRAB_ON);

            SDL_WarpMouse(Game_Screen->w / 2, Game_Screen->h / 2);
        }
        else
        {
            if (Display_Fullscreen)
            {
                Game_OldCursor = SDL_GetCursor();
                SDL_SetCursor(Game_NoCursor);

                SDL_WarpMouse(Game_Screen->w / 2, Game_Screen->h / 2);
            }
            else
            {
                if (Game_MouseCursor == 2)
                {
                    Game_OldCursor = SDL_GetCursor();
                    SDL_SetCursor(Game_NoCursor);
                }
                else if (Game_MouseCursor == 1)
                {
                    Game_OldCursor = SDL_GetCursor();
                    SDL_SetCursor(Game_MinCursor);
                }

                SDL_GetMouseState(&mousex, &mousey);
                SDL_WarpMouse(mousex, mousey);
            }

            SDL_ShowCursor(SDL_ENABLE);
        }

    #ifdef ALLOW_OPENGL
        if (Game_UseOpenGL)
        {
            int index;

            glViewport(Picture_Position_UL_X, Picture_Position_UL_Y, Picture_Width, Picture_Height);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glGenTextures(3, &(Game_GLTexture[0]));

            for (index = 0; index < 3; index++)
            {
                glBindTexture(GL_TEXTURE_2D, Game_GLTexture[index]);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Render_Width, Render_Height, 0, GL_BGRA, (Display_Bitsperpixel == 32)?GL_UNSIGNED_INT_8_8_8_8_REV:GL_UNSIGNED_SHORT_5_6_5_REV, Game_TextureData);
            }

            Game_CurrentTexture = 0;
        }
    #endif

        Game_DisplayActive = 1;
    }
#endif

    SDL_UnlockMutex(Game_ScreenMutex);

    SDL_SemPost(Game_DisplaySem);
}

static void Game_Display_Destroy(int post)
{
    SDL_LockMutex(Game_ScreenMutex);

    Game_DisplayActive = 0;

    if (Game_OldCursor != NULL)
    {
        SDL_SetCursor(Game_OldCursor);
        Game_OldCursor = NULL;
    }

    // clear screen
#ifdef USE_SDL2
    SDL_SetRenderDrawColor(Game_Renderer, 0, 0, 0, 255);
    SDL_RenderClear(Game_Renderer);
    SDL_RenderPresent(Game_Renderer);
#else
#ifdef ALLOW_OPENGL
    if (Game_UseOpenGL)
    {
        // do nothing
    }
    else
#endif
    {
        SDL_Rect rect;

        rect.x = 0;
        rect.y = 0;
        rect.w = Game_Screen->w;
        rect.h = Game_Screen->h;
        SDL_FillRect(Game_Screen, &rect, 0);
        SDL_Flip(Game_Screen);
    }
#endif

    SDL_ShowCursor(SDL_DISABLE);

#ifdef USE_SDL2
    SDL_SetRelativeMouseMode(SDL_FALSE);

    SDL_DestroyTexture(Game_Texture);
    Game_Texture = NULL;
    SDL_DestroyRenderer(Game_Renderer);
    Game_Renderer = NULL;
    SDL_DestroyWindow(Game_Window);
    Game_Window = NULL;
#else
    SDL_WM_GrabInput(SDL_GRAB_OFF);

    //senquack - should not free screens allocated by setvideomode
//    SDL_FreeSurface(Game_Screen);
    Game_Screen = NULL;

#ifdef ALLOW_OPENGL
    if (Game_UseOpenGL)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(3, &(Game_GLTexture[0]));
    }
#endif
#endif

    SDL_UnlockMutex(Game_ScreenMutex);

    if (post)
    {
        SDL_SemPost(Game_DisplaySem);
    }
}

void Game_CleanState(int imm)
{
    int i;
    SDL_Event event;

    for (i = 0; i < 256; i++)
    {
        if (Game_AllocatedMemory[i] != NULL)
        {
            free(Game_AllocatedMemory[i]);
            Game_AllocatedMemory[i] = NULL;
        }
    }

    Game_ScreenWindow = Game_FrameBuffer;
    Game_NextMemory = 0;

    Game_KQueueWrite = 0;
    Game_KQueueRead = 0;
    Game_KBufferWrite = 0;
    Game_KBufferRead = 0;
    Game_LastKeyStroke = 0;
    Game_MouseX = 0;
    Game_MouseY = 0;
    Game_MouseButtons = 0;
    Game_MousePressedButtons = 0;

    memset(&Game_InterruptTable, 0, sizeof(Game_InterruptTable));
    memset(&Game_MouseTable, 0, sizeof(Game_MouseTable));
    memset(&Game_Palette_Or, 0, sizeof(Game_Palette_Or));

#ifdef USE_SDL2
    if (Game_Window != NULL)
#else
    if (Game_Screen != NULL)
#endif
    {
        if (imm)
        {
            Game_Display_Destroy(0);
        }
        else
        {
            event.type = SDL_USEREVENT;
            event.user.code = EC_DISPLAY_DESTROY;
            event.user.data1 = NULL;
            event.user.data2 = NULL;

            SDL_PushEvent(&event);

            SDL_SemWait(Game_DisplaySem);
        }
    }
}

static void Game_Cleanup(void)
{
    if (Game_Sound || Game_Music)
    {
        if (Game_Music)
        {
            Mix_HaltMusic();
            if (Game_MidiSubsystem)
            {
                if (Game_MidiSubsystem <= 20)
                {
                    MidiPlugin_Shutdown();
                }
                else
                {
                    MidiPlugin2_Shutdown();
                }
            }
        }
        if (Game_Sound)
        {
            Mix_HaltChannel(-1);
        }
        Mix_CloseAudio();
    }

    Game_CleanState(1);

#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
    if (Game_TextureData != NULL)
    {
        free(Game_TextureData);
        Game_TextureData = NULL;
    }
#endif

    if (Game_MidiDevice != NULL)
    {
        free(Game_MidiDevice);
        Game_MidiDevice = NULL;
    }

    if (Game_MT32RomsPath != NULL)
    {
        free(Game_MT32RomsPath);
        Game_MT32RomsPath = NULL;
    }

    if (Game_SoundFontPath != NULL)
    {
        free(Game_SoundFontPath);
        Game_SoundFontPath = NULL;
    }

    if (Game_samples[0].start != NULL)
    {
        free(Game_samples[0].start);
        Game_samples[0].start = NULL;
    }

    if (Game_samples[1].start != NULL)
    {
        free(Game_samples[1].start);
        Game_samples[1].start = NULL;
    }

    if (Temp_Font_Data != NULL)
    {
        free(Temp_Font_Data);
        Temp_Font_Data = NULL;
    }

    if (UFO_Font != NULL)
    {
        free(UFO_Font);
        UFO_Font = NULL;
    }

    if (Game_ScreenMutex != NULL)
    {
        SDL_DestroyMutex(Game_ScreenMutex);
        Game_ScreenMutex = NULL;
    }

    if (Game_FlipSem != NULL)
    {
        SDL_DestroySemaphore(Game_FlipSem);
        Game_FlipSem = NULL;
    }

    if (Game_DisplaySem != NULL)
    {
        SDL_DestroySemaphore(Game_DisplaySem);
        Game_DisplaySem = NULL;
    }

    /*if (Game_ScreenWindow != NULL)
    {
        free(Game_ScreenWindow);
        Game_ScreenWindow = NULL;
    }*/

    if (Game_FrameMemory != NULL)
    {
        free(Game_FrameMemory);
        Game_FrameMemory = NULL;
    }

    if (Game_MinCursor != NULL)
    {
        SDL_FreeCursor(Game_MinCursor);
        Game_MinCursor = NULL;
    }

    if (Game_NoCursor != NULL)
    {
        SDL_FreeCursor(Game_NoCursor);
        Game_NoCursor = NULL;
    }

    SDL_Quit();
}

static void Game_BuildRTable(void)
{
    int i;
    unsigned int err;

    for (i = 0; i < 256; i++) errno_rtable[i] = i;

    for (i = ERRNO_NUM - 1; i >= 0; i--)
    {
        err = errno_table[i];

        if (err < 256)
        {
            errno_rtable[err] = i;
        }
    }
}

static void Game_ReadFontData(void)
{
    FILE *f;
    off_t fsize;
    size_t items;

    f = Game_fopen("GEODATA\\SMALLSET.DAT", "rb");
    if (f == NULL) return;

    fsize = Game_filelength2(f);

    if (fsize == -1)
    {
        fclose(f);
        return;
    }

    UFO_Font = (uint8_t *) malloc(fsize);

    if (UFO_Font == NULL)
    {
        fclose(f);
        return;
    }

    items = fread(UFO_Font, 1, fsize, f);

    fclose(f);

    if (items != fsize)
    {
        free(UFO_Font);
        UFO_Font = NULL;
        return;
    }

    if (Font_Size_Shift > 0)
    {
        Temp_Font_Data = (uint8_t *) malloc((8*9) << (2*Font_Size_Shift + 1));
    }
    else
    {
        Temp_Font_Data = NULL;
    }
}

static int Game_Initialize(void)
{
    if (Game_Directory[0] == 0)
    {
        vfs_init(1);
    }
    else
    {
        char cur_dir[MAX_PATH];

        if (NULL != getcwd(cur_dir, MAX_PATH))
        {
            if (0 != chdir(Game_Directory));
            vfs_init(0);
            if (0 != chdir(cur_dir));
        }
    }

    main_arg1 = NULL;
    main_arg2 = NULL;

    if (main_argv[1] != NULL)
    {
        main_arg1 = strdup(main_argv[1]);

        if (main_argv[2] != NULL)
        {
            main_arg2 = strdup(main_argv[2]);
        }
    }

#ifdef USE_SDL2
    Game_Window = NULL;
    Game_Renderer = NULL;
    Game_Texture = NULL;
#else
    Game_Screen = NULL;
#endif
    Game_FrameMemory = NULL;
    Game_FrameBuffer = NULL;
    Game_ScreenWindow = NULL;
    memset(&Game_AllocatedMemory, 0, sizeof(Game_AllocatedMemory));

    Game_TimerRunning = 0;
    Game_TimerTick = 0;
    Game_TimerRun = 0;
    Game_VSyncTick = 0;
    Thread_Exited = 0;
    Thread_Exit = 0;
    Game_Paused = 0;
    Game_SDLTicks = 0;
    Game_LastAudio = 0;
    Game_OldCursor = NULL;
    Game_NoCursor = NULL;
    Game_MinCursor = NULL;
    Game_NoCursorData = 0;
    Game_MinCursorData[0] = 0x50; // data
    Game_MinCursorData[1] = 0xA8;
    Game_MinCursorData[2] = 0x50;
    Game_MinCursorData[3] = 0xA8;
    Game_MinCursorData[4] = 0x50;
    Game_MinCursorData[5] = 0xD8; // mask
    Game_MinCursorData[6] = 0xF8;
    Game_MinCursorData[7] = 0x50;
    Game_MinCursorData[8] = 0xF8;
    Game_MinCursorData[9] = 0xD8;
    Game_MouseCursor = 0;

    Game_Sound = 1;
    Game_Music = 1;
    Game_AudioMasterVolume = MIX_MAX_VOLUME;

    memset(&Game_MusicSequence, 0, sizeof(Game_MusicSequence));
    Game_MusicSequence.volume = 128;

    Game_SoundStereo = 0;
    Game_Sound16bit = 0;
    Game_SoundSigned = 0;

    Game_AudioPending = 1;
    Game_AudioSemaphore = 1;

    memset(&Game_samples, 0, sizeof(Game_samples));

    Game_SoundCfg.SoundDriver = 3;
    Game_SoundCfg.SoundBasePort = 0xffff;
    Game_SoundCfg.SoundIRQ = 0xffff;
    Game_SoundCfg.SoundDMA = 0xffff;
    Game_SoundCfg.MusicDriver = 4;
    Game_SoundCfg.MusicBasePort = 0xffff;
    Game_SoundCfg.SoundChannels = 4;
    Game_SoundCfg.SoundSwapStereo = 0;
    Game_SoundCfg.Reserved = 0;

    Display_ChangeMode = 0;
    Font_Size_Shift = 0;

    Game_Delay_Game = 1;
    Game_Main_Loop_VSync_Ticks = 5;

    Game_VolumeDelta = 0;

    Game_SoundFontPath = NULL;
    Game_MT32RomsPath = NULL;
    Game_MidiDevice = NULL;

    if (Game_ConfigFilename[0] == 0)
    {
        strcpy(Game_ConfigFilename, "Ufo.cfg");				//default config file
    }

    UFO_Font = NULL;
    Temp_Font_Data = NULL;
    Game_DisplaySem = NULL;
    Game_FlipSem = NULL;
    Game_ExitCode = 0;
    Game_ScreenMutex = NULL;
    Game_DisplayActive = 0;

#if defined(ALLOW_OPENGL) && !defined(USE_SDL2)
    Game_UseOpenGL = 0;
    Game_CurrentTexture = 0;
#endif
#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
    Game_TextureData = NULL;
#endif

    Init_Display();
    Init_Audio();
    Init_Input();

    if ( SDL_Init (SDL_INIT_VIDEO //| SDL_INIT_TIMER
                    | ((Game_Joystick)?SDL_INIT_JOYSTICK:0)
                    ) != 0 )
    {
        fprintf (stderr, "Error: Couldn't initialize SDL: %s\n", SDL_GetError ());
        return -1;
    }


    Game_NoCursor = SDL_CreateCursor(&Game_NoCursorData, &Game_NoCursorData, 8, 1, 0, 0);

    if (Game_NoCursor == NULL)
    {
        fprintf (stderr, "Error: Couldn't create cursor: %s\n", SDL_GetError ());
        return -2;
    }

    Game_MinCursor = SDL_CreateCursor(&(Game_MinCursorData[0]), &(Game_MinCursorData[5]), 8, 5, 2, 2);

    if (Game_MinCursor == NULL)
    {
        fprintf (stderr, "Error: Couldn't create cursor: %s\n", SDL_GetError ());
        Game_Cleanup();
        return -2;
    }

    if (Game_Joystick)
    {
        SDL_JoystickOpen(0);
    }

    Game_FrameMemory = (uint8_t *) malloc(/*320*201*/ 65536*2);
    if (Game_FrameMemory == NULL)
    {
        fprintf(stderr, "Error: Not enough memory\n");
        Game_Cleanup();
        return -3;
    }

    Game_FrameBuffer = (uint8_t *) ((((uintptr_t) Game_FrameMemory) + 65535) & ~0xffff);
    memset(Game_FrameBuffer, 0, 65536);

/*	Game_ScreenWindow = (uint8_t *) malloc(65536);
    if (Game_ScreenWindow == NULL)
    {
        fprintf(stderr, "Error: Not enough memory\n");
        return -4;
    }*/

    Game_DisplaySem = SDL_CreateSemaphore(0);
    if (Game_DisplaySem == NULL)
    {
        fprintf(stderr, "Error: Unable to create semaphore\n");
        Game_Cleanup();
        return -5;
    }

    Game_FlipSem = SDL_CreateSemaphore(0);
    if (Game_FlipSem == NULL)
    {
        fprintf(stderr, "Error: Unable to create semaphore\n");
        Game_Cleanup();
        return -6;
    }

    Game_ScreenMutex = SDL_CreateMutex();
    if (Game_ScreenMutex == NULL)
    {
        fprintf(stderr, "Error: Unable to create mutes\n");
        Game_Cleanup();
        return -7;
    }

    Game_BuildRTable();

    Game_stdin = stdin;
    Game_stdout = stdout;
    Game_stderr = stderr;

    return 0;
}

static void Game_Initialize2(void)
{
    uint32_t Sound, Music;
    int frequency, channels;
    Uint16 format;

    if (Game_Sound || Game_Music)
    {
        Sound = Game_Sound;
        Music = Game_Music;
        Game_Sound = 0;
        Game_Music = 0;

        if ( SDL_InitSubSystem(SDL_INIT_AUDIO) == 0 )
        {
            frequency = Game_AudioRate;
            format = Game_AudioFormat;
            channels = Game_AudioChannels;

            if ( Mix_OpenAudio(frequency, format, channels, Game_AudioBufferSize) == 0)
            {
                if ( Mix_QuerySpec(&frequency, &format, &channels) )
                {
#if defined(__DEBUG__)
                    fprintf(stderr, "Audio rate: %i\n", frequency);
                    fprintf(stderr, "Audio format: 0x%x\n", format);
                    fprintf(stderr, "Audio channels: %i\n", channels);
#endif
                    Game_AudioRate = frequency;
                    Game_AudioFormat = format;
                    Game_AudioChannels = channels;
                }

                Mix_AllocateChannels(1);

                if (Music && Game_MidiSubsystem)
                {
                    if (Game_MidiSubsystem <= 20)
                    {
                        if (MidiPlugin_Startup())
                        {
                            Music = 0;
                        }
                    }
                    else
                    {
                        if (MidiPlugin2_Startup())
                        {
                            Music = 0;
                        }
                    }
                }

                Game_Sound = Sound;
                Game_Music = Music;

                if (!Sound && !Music) Game_AudioMasterVolume = 0;

                Mix_ChannelFinished(Game_ChannelFinished);
            }
            else
            {
                SDL_QuitSubSystem(SDL_INIT_AUDIO);
            }
        }
    }

    if (Game_Sound)
    {
        Game_AudioPending = 0;
        Game_AudioSemaphore = 0;

        Game_SoundCfg.SoundDriver = 0;          // soundblaster
        Game_SoundCfg.SoundBasePort = 0x0220;   // SB base port
        Game_SoundCfg.SoundIRQ = 7;             // SB IRQ
        Game_SoundCfg.SoundDMA = 1;             // SB DMA
        Game_SoundCfg.SoundChannels = 8;
        Game_SoundCfg.SoundSwapStereo = 0;

//senquack - SOUND STUFF
//        Sound samples relative volume now configurable
//        Mix_Volume(-1, Game_AudioMasterVolume);
        Mix_Volume(-1, (Game_AudioMasterVolume * Game_AudioSampleVolume) >> 7);
    }

    if (Game_Music)
    {
        if (Game_MidiSubsystem == 10)
        {
            Game_SoundCfg.MusicDriver = 0;          // adlib / soundblaster fm
            Game_SoundCfg.MusicBasePort = 0x0388;   // adlib base port
        }
        else if (Game_MidiSubsystem == 11)
        {
            Game_SoundCfg.MusicDriver = 1;          // roland lapc-1 / mt32
            Game_SoundCfg.MusicBasePort = 0x0330;   // MT32 base port
        }
        else
        {
            Game_SoundCfg.MusicDriver = 3;          // soundblaster awe32 / general midi
            Game_SoundCfg.MusicBasePort = 0x0330;   // GM base port
        }

//senquack - SOUND STUFF
//        Mix_VolumeMusic(Game_AudioMasterVolume);
        // Music loudness now configurable.
        // On GP2X, AudioMasterVolume does nothing, overall volume is controlled by /dev/mixer.
        Mix_VolumeMusic((Game_AudioMasterVolume * Game_AudioMusicVolume) >> 7);
    }

    Init_Display2();
    Init_Audio2();
    Init_Input2();

#if !defined(USE_SDL2)
    SDL_EnableUNICODE(1);
#endif

    Game_VideoAspectX = (320 << 16) / Picture_Width;
    Game_VideoAspectY = (200 << 16) / Picture_Height;

    Game_VideoAspectXR = (Picture_Width << 16) / 320;
    Game_VideoAspectYR = (Picture_Height << 16) / 200;

#if defined(ALLOW_OPENGL) || defined(USE_SDL2)
#if !defined(USE_SDL2)
    if (Game_UseOpenGL)
#endif
    {
        Game_TextureData = malloc(Render_Width * Render_Height * Display_Bitsperpixel / 8);
    }
#endif
}

static void Game_Event_Loop(void)
{
    SDL_Thread *MainThread;
    SDL_Thread *FlipThread;
    SDL_Thread *TimerThread;
    SDL_Event event;
    uint32_t AppMouseFocus;
    uint32_t AppInputFocus;
    uint32_t AppActive;
    int FlipActive, CreateAfterFlip, DestroyAfterFlip;


    TimerThread = SDL_CreateThread(
        Game_TimerThread,
#ifdef USE_SDL2
        "timer",
#endif
        NULL
    );

    if (TimerThread == NULL)
    {
        fprintf(stderr, "Error: Unable to start timer thread\n");
        return;
    }

    FlipThread = SDL_CreateThread(
        Game_FlipThread,
#ifdef USE_SDL2
        "flip",
#endif
        NULL
    );

    if (FlipThread == NULL)
    {
        fprintf(stderr, "Error: Unable to start flip thread\n");

        Thread_Exited = 1;
        Thread_Exit = 1;

        SDL_WaitThread(TimerThread, NULL);

        return;
    }

    MainThread = SDL_CreateThread(
        Game_MainThread,
#ifdef USE_SDL2
        "main",
#endif
        NULL
    );

    if (MainThread == NULL)
    {
        fprintf(stderr, "Error: Unable to start main thread\n");

        Thread_Exited = 1;
        Thread_Exit = 1;

        SDL_SemPost(Game_FlipSem);

        SDL_WaitThread(FlipThread, NULL);
        SDL_WaitThread(TimerThread, NULL);

        return;
    }

#ifdef USE_SDL2
    AppMouseFocus = 1;
    AppInputFocus = 1;
    AppActive = 1;
#else
    {
        uint32_t AppState;

        AppState = SDL_GetAppState();

        AppMouseFocus = AppState & SDL_APPMOUSEFOCUS;
        AppInputFocus = AppState & SDL_APPINPUTFOCUS;
        AppActive = AppState & SDL_APPACTIVE;
    }
#endif

    FlipActive = 0;
    CreateAfterFlip = 0;
    DestroyAfterFlip = 0;

    while (!Thread_Exited && SDL_WaitEvent(&event))
    {
        if (Handle_Input_Event(&event)) continue;

        switch(event.type)
        {
        #ifdef USE_SDL2
            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_ENTER:
                        AppMouseFocus = 1;
                        break;
                    case SDL_WINDOWEVENT_LEAVE:
                        AppMouseFocus = 0;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        AppInputFocus = 1;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        AppInputFocus = 0;
                        break;
                    case SDL_WINDOWEVENT_MINIMIZED:
                        AppActive = 0;
                        break;
                    case SDL_WINDOWEVENT_MAXIMIZED:
                    case SDL_WINDOWEVENT_RESTORED:
                        AppActive = 1;
                        break;
                    //case SDL_WINDOWEVENT_CLOSE:
                    // todo: ?
                }

                break;
                // case SDL_WINDOWEVENT:
        #else
            case SDL_ACTIVEEVENT:
                if (event.active.state & SDL_APPMOUSEFOCUS)
                {
                    AppMouseFocus = event.active.gain;
                }
                if (event.active.state & SDL_APPINPUTFOCUS)
                {
                    AppInputFocus = event.active.gain;
                }
                if (event.active.state & SDL_APPACTIVE)
                {
                    AppActive = event.active.gain;
                }

                break;
                // case SDL_ACTIVEEVENT:
        #endif

            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (
                #ifdef USE_SDL2
                    Game_Window != NULL
                #else
                    Game_Screen != NULL
                #endif
                    && AppActive && AppInputFocus)
                {
                    if ( ( (Game_KQueueWrite + 1) & (GAME_KQUEUE_LENGTH - 1) ) == Game_KQueueRead )
                    {
#if defined(__DEBUG__)
                        printf("keyboard event queue overflow\n");
#endif
                    }
                    else
                    {
                        Game_EventKQueue[Game_KQueueWrite] = event;

                        Game_KQueueWrite = (Game_KQueueWrite + 1) & (GAME_KQUEUE_LENGTH - 1);
                    }
                }

                break;
                // case SDL_KEYDOWN, SDL_KEYUP:
            case SDL_MOUSEMOTION:
                if (
                #ifdef USE_SDL2
                    Game_Window != NULL
                #else
                    Game_Screen != NULL
                #endif
                    && AppActive && AppInputFocus && AppMouseFocus)
                {
                    Game_MouseX = event.motion.x;
                    Game_MouseY = event.motion.y;

                    /*Game_MouseButtons = ( (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)  )?1:0 ) |
                                        ( (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT) )?2:0 ) |
                                        ( (event.motion.state & SDL_BUTTON(SDL_BUTTON_MIDDLE))?4:0 );
                    Game_MousePressedButtons |= Game_MouseButtons;*/
                }
                break;
                // case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                if (
                #ifdef USE_SDL2
                    Game_Window != NULL
                #else
                    Game_Screen != NULL
                #endif
                    && AppActive && AppInputFocus && AppMouseFocus)
                {
                    if (event.button.button == SDL_BUTTON_LEFT ||
                        event.button.button == SDL_BUTTON_RIGHT ||
                        event.button.button == SDL_BUTTON_MIDDLE)
                    {
                        unsigned int buttonval;

                        /*Game_MouseX = event.button.x;
                        Game_MouseY = event.button.y;*/

                        switch (event.button.button)
                        {
                            case SDL_BUTTON_LEFT:
                                buttonval = 1;
                                break;
                            case SDL_BUTTON_RIGHT:
                                buttonval = 2;
                                break;
                            case SDL_BUTTON_MIDDLE:
                                buttonval = 4;
                                break;
                            default:
                                buttonval = 0;
                                break;
                        }

                        if (event.button.state == SDL_PRESSED)
                        {
                            Game_MouseButtons |= buttonval;
                            Game_MousePressedButtons |= buttonval;
                        }
                        else
                        {
                            Game_MouseButtons &= ~buttonval;
                        }
                    }
                #ifdef USE_SDL2
                    // handled below
                #else
                    else if (event.button.button == SDL_BUTTON_WHEELUP)
                    {
                        Display_ChangeMode = 1;
                    }
                    else if (event.button.button == SDL_BUTTON_WHEELDOWN)
                    {
                        Display_ChangeMode = -1;
                    }
                #endif
                }

                break;
                // case SDL_MOUSEBUTTONUP, SDL_MOUSEBUTTONDOWN:
        #ifdef USE_SDL2
            case SDL_MOUSEWHEEL:
                if (event.wheel.y > 0)
                {
                    Display_ChangeMode = 1;
                }
                else if (event.wheel.y < 0)
                {
                    Display_ChangeMode = -1;
                }
                break;
                // case SDL_MOUSEWHEEL:
        #endif
            case SDL_QUIT:
                /* todo: question */

                if (Thread_Exit) exit(1);

                Thread_Exit = 1;

                SDL_SemPost(Game_FlipSem);

                SDL_WaitThread(FlipThread, NULL);

                SDL_WaitThread(MainThread, NULL);

                SDL_WaitThread(TimerThread, NULL);

                break;
                // case SDL_QUIT:
            case SDL_USEREVENT:
                switch (event.user.code)
                {
                    case EC_DISPLAY_CREATE:
                        if (FlipActive)
                        {
                            CreateAfterFlip = 1;
                        }
                        else
                        {
                            Game_Display_Create();
                        }

                        break;
                        // case EC_DISPLAY_CREATE:

                    case EC_DISPLAY_DESTROY:
                        if (FlipActive)
                        {
                            DestroyAfterFlip = 1;
                        }
                        else
                        {
                            Game_Display_Destroy(1);
                        }

                        break;
                        // case EC_DISPLAY_DESTROY:

                    case EC_DISPLAY_FLIP_START:
                        if (!FlipActive &&
                        #ifdef USE_SDL2
                            Game_Window != NULL
                        #else
                            Game_Screen != NULL
                        #endif
                            )
                        {
                            FlipActive = 1;

                            /* ??? */
/*								SDL_LockSurface(Game_Screen);*/

                            SDL_SemPost(Game_FlipSem);
                        }

                        break;
                        // case EC_DISPLAY_FLIP_START:

                    case EC_DISPLAY_FLIP_FINISH:
                        if (FlipActive)
                        {
                        #if defined(USE_SDL2)
                            if (Game_DisplayActive)
                            {
                                SDL_UpdateTexture(Game_Texture, NULL, Game_TextureData, Render_Width * Display_Bitsperpixel / 8);
                                SDL_RenderCopy(Game_Renderer, Game_Texture, NULL, NULL);
                                SDL_RenderPresent(Game_Renderer);
                            }
                        #elif defined(ALLOW_OPENGL)
                            if (Game_UseOpenGL && Game_DisplayActive)
                            {
                                glBindTexture(GL_TEXTURE_2D, Game_GLTexture[Game_CurrentTexture]);

                                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Render_Width, Render_Height, GL_BGRA, (Display_Bitsperpixel == 32)?GL_UNSIGNED_INT_8_8_8_8_REV:GL_UNSIGNED_SHORT_5_6_5_REV, Game_TextureData);

                                glEnable(GL_TEXTURE_2D);

                                static const GLfloat QuadVertices[2*4] = {
                                    -1.0f,  1.0f,
                                     1.0f,  1.0f,
                                     1.0f, -1.0f,
                                    -1.0f, -1.0f,
                                };
                                static const GLfloat QuadTexCoords[2*4] = {
                                    0.0f, 0.0f,
                                    1.0f, 0.0f,
                                    1.0f, 1.0f,
                                    0.0f, 1.0f
                                };

                                glEnableClientState(GL_VERTEX_ARRAY);
                                glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                                glVertexPointer(2, GL_FLOAT, 0, &(QuadVertices[0]));
                                glTexCoordPointer(2, GL_FLOAT, 0, &(QuadTexCoords[0]));
                                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                                glDisableClientState(GL_VERTEX_ARRAY);

                                glDisable(GL_TEXTURE_2D);

                                SDL_GL_SwapBuffers();

                                Game_CurrentTexture++;
                                if (Game_CurrentTexture > 2)
                                {
                                    Game_CurrentTexture = 0;
                                }
                            }
                        #endif

                            /* ??? */
/*								SDL_UnlockSurface(Game_Screen);
                            SDL_Flip(Game_Screen);*/

                            FlipActive = 0;

                            if (DestroyAfterFlip)
                            {
                                Game_Display_Destroy(1);
                                DestroyAfterFlip = 0;
                            }

                            if (CreateAfterFlip)
                            {
                                Game_Display_Create();
                                CreateAfterFlip = 0;
                            }
                        }

                        break;
                        // case EC_DISPLAY_FLIP_FINISH:
                    case EC_PROGRAM_QUIT:
                        if (Thread_Exit) exit(1);

                        Thread_Exit = 1;

                        SDL_SemPost(Game_FlipSem);

                        SDL_WaitThread(FlipThread, NULL);

                        SDL_WaitThread(MainThread, NULL);

                        SDL_WaitThread(TimerThread, NULL);

                        break;
                        // case EC_PROGRAM_QUIT:
                    case EC_MOUSE_MOVE:
                        {
                        #ifdef USE_SDL2
                            SDL_WarpMouseInWindow(Game_Window, Game_MouseX + (int) event.user.data1, Game_MouseY + (int) event.user.data2);
                        #else
                            SDL_WarpMouse(Game_MouseX + (int) event.user.data1, Game_MouseY + (int) event.user.data2);
                        #endif
                        }
                        break;
                    case EC_MOUSE_SET:
                        {
                        #ifdef USE_SDL2
                            SDL_WarpMouseInWindow(Game_Window, (int) event.user.data1, (int) event.user.data2);
                        #else
                            SDL_WarpMouse((int) event.user.data1, (int) event.user.data2);
                        #endif
                        }
                        break;
                    case EC_GET_MOUSE_POS:
                        ((int32_t *) event.user.data1)[0] = Game_MouseX;
                        ((int32_t *) event.user.data1)[1] = Game_MouseY;
                        break;
                    //senquack - need this to allow macros to behave in battlescape
                    case EC_DELAY:
                        SDL_Delay((int) event.user.data1);
                        break;
                        // case EC_DELAY:

                    case EC_SET_VOLUME:
//senquack - SOUND STUFF
                        if (Game_Sound)
                        {
                            // Sound sample relative volume now configurable:
                            Mix_Volume(-1, (Game_AudioSampleVolume * Game_AudioMasterVolume) >> 7);
                        }

                        if (Game_Music)
                        {
                            Game_SetMusicVolume();
                        }
                        break;
                }

                break;
                // case SDL_USEREVENT:
            default:
                Handle_Input_Event2(&event);
                break;
        } // switch(event.type)
    }

}

int main (int argc, char *argv[])
{
    main_argv = argv;

    Game_ConfigFilename[0] = 0;
    Game_Directory[0] = 0;

    // read parameters
    if (argc > 1)
    {
        int numpar;
        char **param;

        numpar = argc;
        param = argv;

        while (numpar)
        {
            //senquack - new config option for specifying a config file to use
            if ( strncmp(*param, "--config-file", 13) == 0)
            {
                (*param) += 14;	// skip space or equals
                if ((strlen(*param)) > 0 && (strlen(*param) < 80))
                {
                    strcpy(Game_ConfigFilename, *param);
                }
            }
            else if ( strncmp(*param, "--game-dir", 10) == 0)
            {
                (*param) += 11;	// skip space or equals
                if ((strlen(*param)) > 0 && (strlen(*param) < 80))
                {
                    strcpy(Game_Directory, *param);
                }
            }

            numpar--;
            param++;
        }
    }

#if defined(__DEBUG__)
    fprintf(stderr, "Initializing...\n");
#endif
    // initialization
    {
        int return_value;

        return_value = Game_Initialize();
        if (return_value) return return_value;
    }

    Game_ReadConfig();
    Game_ReadFontData();

    Game_Initialize2();

#if defined(__DEBUG__)
    fprintf(stderr, "Starting game event loop...\n");
#endif
    Game_Event_Loop();

#if defined(__DEBUG__)
    fprintf(stderr, "Cleaning up...\n");
#endif

    Game_Cleanup();

    Cleanup_Input();
    Cleanup_Audio();
    Cleanup_Display();

    return Game_ExitCode;
}

