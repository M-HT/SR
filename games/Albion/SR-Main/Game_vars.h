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

#if !defined(_GAME_VARS_H_INCLUDED_)
#define _GAME_VARS_H_INCLUDED_

#include <errno.h>
#include <stdio.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
#else
    #include <SDL/SDL.h>
    #ifdef ALLOW_OPENGL
        #include <SDL/SDL_opengl.h>
    #endif
#endif
#include "Game_defs.h"

#if defined(DEFINE_VARIABLES)
    #define EXTERNAL_VARIABLE
#else
    #define EXTERNAL_VARIABLE extern
#endif

EXTERNAL_VARIABLE uint32_t X86_InterruptFlag;		/* interrupt flag indicator */

EXTERNAL_VARIABLE uint8_t *Game_FrameBuffer;		/* video memory (all) */
EXTERNAL_VARIABLE PTR32(uint8_t) Game_ScreenWindow;	/* video bank (64KiB) */
EXTERNAL_VARIABLE uint32_t Game_InterruptTable[256];	/* interrupt table */
EXTERNAL_VARIABLE void *Game_MouseTable[8];			/* mouse functions table */
EXTERNAL_VARIABLE void *Game_AllocatedMemory[256];	/* dos allocated memory table */
EXTERNAL_VARIABLE pixel_format_orig Game_Palette_Or[256];	/* original palette (rgba) */
EXTERNAL_VARIABLE uint32_t Game_ScreenWindowNum;	/* screen number in video bank */
EXTERNAL_VARIABLE volatile uint32_t Game_DisplayStart;		/* offset in video memory where display starts */
EXTERNAL_VARIABLE uint32_t Game_ESP_Original_Value;	/* original value of ESP */
EXTERNAL_VARIABLE uint32_t Game_NextMemory;			/* next memory place to allocate in memory table */
EXTERNAL_VARIABLE SDL_Cursor *Game_OldCursor;		/* original cursor */
EXTERNAL_VARIABLE SDL_Cursor *Game_NoCursor;		/* invisible cursor */
EXTERNAL_VARIABLE SDL_Cursor *Game_MinCursor;		/* minimal cursor */
EXTERNAL_VARIABLE uint8_t Game_NoCursorData;		/* invisible cursor data */
EXTERNAL_VARIABLE uint8_t Game_MinCursorData[10];	/* minimal cursor data */
EXTERNAL_VARIABLE int Game_MouseCursor;				/* mouse cursor type in window
                                                       0: normal
                                                       1: minimal
                                                       2: none */
EXTERNAL_VARIABLE int Game_PlayIntro;				/* play intro on start ? */
EXTERNAL_VARIABLE int Game_SwitchWSAD;				/* switch WSAD keys for arrow keys ? */
EXTERNAL_VARIABLE int Game_SwitchArrowKeys;			/* switch arrow keys for WSAD keys ? */


// global display variables
EXTERNAL_VARIABLE uint32_t Game_VideoAspectX;		/* X aspect ratio */
EXTERNAL_VARIABLE uint32_t Game_VideoAspectY;		/* Y aspect ratio */
EXTERNAL_VARIABLE uint32_t Game_VideoAspectXR;		/* 1 / (X aspect ratio) */
EXTERNAL_VARIABLE uint32_t Game_VideoAspectYR;		/* 1 / (Y aspect ratio) */
EXTERNAL_VARIABLE uint32_t Display_Width;			/* physical display width */
EXTERNAL_VARIABLE uint32_t Display_Height;			/* physical display height */
EXTERNAL_VARIABLE uint32_t Display_Bitsperpixel;	/* display bits per pixel */
EXTERNAL_VARIABLE uint32_t Display_Fullscreen;		/* display - fulscreen ? */
EXTERNAL_VARIABLE uint32_t Display_FSType;			/* fulscreen type */
EXTERNAL_VARIABLE uint32_t Display_MouseLocked;		/* is mouse locked in the window ? */
EXTERNAL_VARIABLE volatile int32_t Display_ChangeMode;		/* flag to change display mode: 1 = forward, -1 = backward */
EXTERNAL_VARIABLE uint32_t Render_Width;			/* render width */
EXTERNAL_VARIABLE uint32_t Render_Height;			/* render height */
EXTERNAL_VARIABLE uint32_t Picture_Width;			/* picture width */
EXTERNAL_VARIABLE uint32_t Picture_Height;			/* picture height */
EXTERNAL_VARIABLE int32_t Picture_Position_UL_X;	/* picture position - upper left corner - x (relative to display) */
EXTERNAL_VARIABLE int32_t Picture_Position_UL_Y;	/* picture position - upper left corner - y (relative to display) */
EXTERNAL_VARIABLE int32_t Picture_Position_BR_X;	/* picture position - bottom right corner - x (relative to display) */
EXTERNAL_VARIABLE int32_t Picture_Position_BR_Y;	/* picture position - bottom right corner - y (relative to display) */
EXTERNAL_VARIABLE Game_Flip_Procedure Display_Flip_Procedure;	/* flip procedure */
EXTERNAL_VARIABLE int Font_Size_Shift;				/* font size = font size * 2 ^ Font_Size_Shift */

EXTERNAL_VARIABLE uint32_t Game_AdvancedScaling;	/* advanced scaling enabled ? */
EXTERNAL_VARIABLE uint32_t Game_ScalingQuality;		/* scaling quality: 0 = nearest neighbour, 1 = bilinear */
EXTERNAL_VARIABLE uint32_t Game_AdvancedScaler;		/* advanced scaler: 0 = none, 1 = nearest neighbour, 2 = hqx, 3 = xbrz */
EXTERNAL_VARIABLE int Game_ScaleFactor;				/* factor for advanced scaler: 0 = max */
EXTERNAL_VARIABLE int Game_ExtraScalerThreads;		/* number of extra threads for advanced scaler: -1 = auto */
EXTERNAL_VARIABLE Game_Advanced_Flip_Procedure Display_Advanced_Flip_Procedure;	/* advanced flip procedure */

#if SDL_VERSION_ATLEAST(2,0,0)
EXTERNAL_VARIABLE SDL_Window *Game_Window;
EXTERNAL_VARIABLE SDL_Renderer *Game_Renderer;
EXTERNAL_VARIABLE SDL_Texture *Game_Texture[3];
EXTERNAL_VARIABLE SDL_Texture *Game_Texture2[3];
EXTERNAL_VARIABLE SDL_Texture *Game_ScaledTexture[3];
#else
EXTERNAL_VARIABLE SDL_Surface *Game_Screen;
#ifdef ALLOW_OPENGL
EXTERNAL_VARIABLE uint32_t Game_UseOpenGL;			/* use OpenGL for drawing ? */
EXTERNAL_VARIABLE GLuint Game_GLTexture[3];
EXTERNAL_VARIABLE GLuint Game_GLTexture2[3];
EXTERNAL_VARIABLE GLuint Game_GLFramebuffer[3];
EXTERNAL_VARIABLE GLuint Game_GLScaledTexture[3];
#endif
#endif
#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
EXTERNAL_VARIABLE void *Game_TextureData;
EXTERNAL_VARIABLE void *Game_TextureData2;
EXTERNAL_VARIABLE void *Game_ScaledTextureData;
EXTERNAL_VARIABLE int Game_CurrentTexture;
EXTERNAL_VARIABLE int Game_UseTextureData2;
EXTERNAL_VARIABLE int Scaler_ScaleFactor;
EXTERNAL_VARIABLE int Scaler_ScaleTextureData;
EXTERNAL_VARIABLE int Scaler_ScaleTexture;
#endif

// global audio variables
EXTERNAL_VARIABLE uint32_t Game_Sound;				/* is sound enabled ? */
EXTERNAL_VARIABLE uint32_t Game_Music;				/* is music enabled ? */
EXTERNAL_VARIABLE uint32_t Game_SoundMasterVolume;	/* sound master volume */
EXTERNAL_VARIABLE uint32_t Game_MusicMasterVolume;	/* Music master volume */
EXTERNAL_VARIABLE int Game_MixerChannels;			/* number of used mixer channels */
EXTERNAL_VARIABLE unsigned int Game_AudioFormat;	/* audio format */
EXTERNAL_VARIABLE unsigned int Game_AudioChannels;	/* number of audio channels */
EXTERNAL_VARIABLE int Game_AudioRate;				/* audio rate (Hz) */
EXTERNAL_VARIABLE int Game_AudioBufferSize;			/* audio buffer size */
EXTERNAL_VARIABLE int Game_ResamplingQuality;		/* audio resampling quality
                                                       0: normal quality
                                                       1: higher quality */
EXTERNAL_VARIABLE int Game_SwapSoundChannels;		/* swap left and right sound channel ? */
EXTERNAL_VARIABLE int Game_VolumeDelta;             /*  0: Volume is stationary
                                                        1: Volume is increasing
                                                       -1: Volume is decreasing	*/
EXTERNAL_VARIABLE int Game_MidiSubsystem;			/* MIDI subsystem
                                                       0: SDL_mixer
                                                       1: WildMidi
                                                       2: BASSMIDI
                                                       3: ADLMIDI
                                                       11: native Windows
                                                       12: ALSA */
EXTERNAL_VARIABLE char *Game_SoundFontPath;			/* Path to SoundFont file */
EXTERNAL_VARIABLE char *Game_MidiDevice;			/* MIDI device name */
EXTERNAL_VARIABLE int Game_OPL3Emulator;			/* OPL3 emulator
                                                       0: fast - DOSBox
                                                       1: precise - Nuked OPL3 */

// global input variables
EXTERNAL_VARIABLE uint32_t Game_Joystick;			/* is joystick used ? */
//senquack - now we keep track of what mouse buttons are currently pressed so
//				we can support two ways of using the touchscreen.  The first way
//				lets users touch/tap the screen and that is LMB, and if they
//				hold the buttom assigned to RMB while touch/tapping the screen,
//				that is RMB.  The second way lets users only move the cursor
//				when touching the touchscreen, and only then the buttons for
//				LMB/RMB are pressed are mouse clicks registered.
EXTERNAL_VARIABLE uint32_t Game_RMBActive;	/* 1 if button for RMB is held  */
EXTERNAL_VARIABLE uint32_t Game_TouchscreenButtonEvents;	/* New option, see comments above */

// 3d engine variables
EXTERNAL_VARIABLE volatile uint32_t Game_UseEnhanced3DEngine;
EXTERNAL_VARIABLE volatile uint32_t Game_UseEnhanced3DEngineNewValue;
EXTERNAL_VARIABLE uint8_t *Game_ScreenViewpartOverlay[2];
EXTERNAL_VARIABLE uint8_t *Game_ScreenViewpartOriginal[2];
EXTERNAL_VARIABLE Game_OverlayInfo Game_OverlayDraw;
EXTERNAL_VARIABLE Game_OverlayInfo Game_OverlayDisplay;

EXTERNAL_VARIABLE int Game_ScreenshotFormat;    /* screenshot format
                                                    0: Original (LBM)
                                                    1: LBM
                                                    2: LBM with lines in file padded to 16 bytes
                                                    3: TGA
                                                    4: BMP
                                                    5: PNG */
EXTERNAL_VARIABLE int Game_ScreenshotEnhancedResolution;
EXTERNAL_VARIABLE uint32_t Game_ScreenshotEnabled;
EXTERNAL_VARIABLE uint32_t Game_ScreenshotAutomaticFilename;

EXTERNAL_VARIABLE Game_sample *Game_SampleCache[GAME_SAMPLE_CACHE_SIZE];


EXTERNAL_VARIABLE PTR32(FILE) Game_stdin;					/* stdin */
EXTERNAL_VARIABLE PTR32(FILE) Game_stdout;				/* stdout */
EXTERNAL_VARIABLE PTR32(FILE) Game_stderr;				/* stderr */

EXTERNAL_VARIABLE char Albion_CDPath[256];			/* path to albion cd */
EXTERNAL_VARIABLE uint8_t *Albion_Font;				/* albion font data */
EXTERNAL_VARIABLE uint8_t *Temp_Font_Data;			/* temporary font data */
EXTERNAL_VARIABLE Albion_Lang Albion_Font_Lang;		/* albion font language */


EXTERNAL_VARIABLE volatile uint32_t Game_TimerRunning;	/* is timer interrupt running ? */
EXTERNAL_VARIABLE volatile uint32_t Game_TimerTick;	/* Timer tick counter */
EXTERNAL_VARIABLE volatile uint32_t Game_TimerRun;	/* Timer number of runs counter */
EXTERNAL_VARIABLE volatile uint32_t Game_VSyncTick;	/* VSync tick counter */
EXTERNAL_VARIABLE volatile uint32_t Thread_Exited;	/* did main thread exit ? */
EXTERNAL_VARIABLE volatile uint32_t Thread_Exit;	/* should thread exit ? */
EXTERNAL_VARIABLE volatile uint32_t Game_Paused;	/* is game paused? */
EXTERNAL_VARIABLE volatile uint32_t SMK_Playing;	/* is smk video playing? */

EXTERNAL_VARIABLE SDL_sem *Game_DisplaySem;
EXTERNAL_VARIABLE SDL_sem *Game_FlipSem;

EXTERNAL_VARIABLE SDL_Event Game_EventMQueue[GAME_MQUEUE_LENGTH];	/* game mouse event queue */
EXTERNAL_VARIABLE SDL_Event Game_EventKQueue[GAME_KQUEUE_LENGTH];	/* game keyboard event queue */
EXTERNAL_VARIABLE uint32_t Game_KBuffer[GAME_KBUFFER_LENGTH];		/* game keyboard buffer */
EXTERNAL_VARIABLE volatile uint32_t Game_MQueueWrite;				/* index of first empty event */
EXTERNAL_VARIABLE volatile uint32_t Game_MQueueRead;				/* index of first non-empty event */
EXTERNAL_VARIABLE volatile uint32_t Game_KQueueWrite;				/* index of first empty event */
EXTERNAL_VARIABLE volatile uint32_t Game_KQueueRead;				/* index of first non-empty event */
EXTERNAL_VARIABLE volatile uint32_t Game_KBufferWrite;				/* index of first empty keystroke */
EXTERNAL_VARIABLE volatile uint32_t Game_KBufferRead;				/* index of first non-empty keystroke */
EXTERNAL_VARIABLE uint32_t Game_LastKeyStroke;						/* time of last keystroke */

EXTERNAL_VARIABLE int Game_ExitCode;				/* exit code */

//senquack - multiple config files now supported:
EXTERNAL_VARIABLE char Game_ConfigFilename[80];			/* Config file being used */

EXTERNAL_VARIABLE char Game_Directory[80];			/* Directory where the game is installed */

EXTERNAL_VARIABLE uint32_t self_mod_width;			/* variable to store self modyfing screen width */

EXTERNAL_VARIABLE uint32_t errno_rtable[256];		/* reverse errno table */

#undef EXTERNAL_VARIABLE

#define ERRNO_NUM 41

#if defined(DEFINE_VARIABLES)

#if !defined(EZERO)
    #define EZERO 0
#endif

#ifdef __WIN32__
    #if !defined(ENOTBLK)
        #define ENOTBLK 26
    #endif

    #if !defined(ETXTBSY)
        #define ETXTBSY 15
    #endif
#endif

const static uint32_t errno_table[ERRNO_NUM] = {
    EZERO, ENOENT, E2BIG, ENOEXEC, EBADF, ENOMEM, EACCES, EEXIST, EXDEV, EINVAL, ENFILE, EMFILE, ENOSPC, EDOM, ERANGE, EDEADLK,
    EINTR, ECHILD, EAGAIN, EBUSY, EFBIG, EIO, EISDIR, ENOTDIR, EMLINK, ENOTBLK, ENOTTY, ENXIO, EPERM, EPIPE, EROFS, ESPIPE,
    ESRCH, ETXTBSY, EFAULT, ENAMETOOLONG, ENODEV, ENOLCK, ENOSYS, ENOTEMPTY, EILSEQ
};
#else
extern const uint32_t errno_table[ERRNO_NUM];
#endif



extern uint32_t mouse_pos[2];
extern uint32_t screen_window_ptr[4];
extern uint8_t keyboard_keys[0x80];


extern int Game_Main_Asm(int argc, PTR32(char) argv[]);
extern void Game_StopMain_Asm(void) __attribute__ ((__noreturn__));


extern uint32_t Game_MouseMove(uint32_t state, uint32_t x, uint32_t y);
extern uint32_t Game_MouseButton(uint32_t state, uint32_t action);
extern void Game_RunTimer_Asm(void);

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t errno_val;

#ifdef __cplusplus
}
#endif

#endif /* _GAME_VARS_H_INCLUDED_ */
