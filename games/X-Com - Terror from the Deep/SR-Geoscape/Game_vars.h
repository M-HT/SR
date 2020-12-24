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

EXTERNAL_VARIABLE char **main_argv;
EXTERNAL_VARIABLE char *main_arg1;
EXTERNAL_VARIABLE char *main_arg2;
#ifdef USE_SDL2
EXTERNAL_VARIABLE SDL_Window *Game_Window;
EXTERNAL_VARIABLE SDL_Renderer *Game_Renderer;
EXTERNAL_VARIABLE SDL_Texture *Game_Texture;
EXTERNAL_VARIABLE void *Game_TextureData;
#else
EXTERNAL_VARIABLE SDL_Surface *Game_Screen;
#endif
EXTERNAL_VARIABLE uint8_t *Game_FrameMemory;		/* allocated video memory */
EXTERNAL_VARIABLE uint8_t *Game_FrameBuffer;		/* pointer to video memory (all) */
EXTERNAL_VARIABLE uint8_t *Game_ScreenWindow;		/* video bank (64KiB) */
EXTERNAL_VARIABLE void *Game_InterruptTable[256];	/* interrupt table */
EXTERNAL_VARIABLE void *Game_MouseTable[8];			/* mouse functions table */
EXTERNAL_VARIABLE void *Game_AllocatedMemory[256];	/* dos allocated memory table */
EXTERNAL_VARIABLE pixel_format_orig Game_Palette_Or[256];	/* original palette (rgba) */
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

// global display variables
EXTERNAL_VARIABLE uint32_t Game_VideoAspectX;		/* X aspect ratio */
EXTERNAL_VARIABLE uint32_t Game_VideoAspectY;		/* Y aspect ratio */
EXTERNAL_VARIABLE uint32_t Game_VideoAspectXR;		/* 1 / (X aspect ratio) */
EXTERNAL_VARIABLE uint32_t Game_VideoAspectYR;		/* 1 / (Y aspect ratio) */
EXTERNAL_VARIABLE uint32_t Display_Width;			/* physical display width */
EXTERNAL_VARIABLE uint32_t Display_Height;			/* physical display height */
EXTERNAL_VARIABLE uint32_t Display_Bitsperpixel;	/* display bits per pixel */
EXTERNAL_VARIABLE uint32_t Display_Fullscreen;		/* display - fulscreen ? */
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
#if defined(ALLOW_OPENGL) && !defined(USE_SDL2)
EXTERNAL_VARIABLE uint32_t Game_UseOpenGL;			/* use OpenGL for drawing ? */
EXTERNAL_VARIABLE void *Game_TextureData;
EXTERNAL_VARIABLE GLuint Game_GLTexture[3];
EXTERNAL_VARIABLE int Game_CurrentTexture;
#endif

EXTERNAL_VARIABLE int Game_Delay_Game;				/* time in ms to delay the game in timer tick */
EXTERNAL_VARIABLE int Game_Main_Loop_VSync_Ticks;	/* maximum number of vsync ticks to wait in game main loop */

// global audio variables
EXTERNAL_VARIABLE uint32_t Game_Sound;				/* is sound enabled ? */
EXTERNAL_VARIABLE uint32_t Game_Music;				/* is music enabled ? */
EXTERNAL_VARIABLE uint32_t Game_AudioMasterVolume;	/* audio master volume */
EXTERNAL_VARIABLE unsigned int Game_AudioFormat;	/* audio format */
EXTERNAL_VARIABLE unsigned int Game_AudioChannels;	/* number of audio channels */
EXTERNAL_VARIABLE int Game_AudioRate;				/* audio rate (Hz) */
EXTERNAL_VARIABLE int Game_AudioBufferSize;			/* audio buffer size */
EXTERNAL_VARIABLE int Game_InterpolateAudio;		/* interpolate audio when resampling ? */
EXTERNAL_VARIABLE Game_sequence Game_MusicSequence; /* sequence played */
EXTERNAL_VARIABLE int Game_SoundStereo;
EXTERNAL_VARIABLE int Game_Sound16bit;
EXTERNAL_VARIABLE int Game_SoundSigned;
EXTERNAL_VARIABLE int16_t Game_AudioPending;
EXTERNAL_VARIABLE int16_t Game_AudioSemaphore;
EXTERNAL_VARIABLE Game_sample Game_samples[2];      /* active and pending game sample */
EXTERNAL_VARIABLE Mix_Chunk Game_AudioChunk;        /* playing audio chunk */
EXTERNAL_VARIABLE Game_SoundConfig Game_SoundCfg;   /* content of file sound.cfg */
//senquack - SOUND STUFF
EXTERNAL_VARIABLE int Game_VolumeDelta;					/*  0: Volume is stationary
                                                            1: Volume is increasing
                                                           -1: Volume is decreasing	*/
//senquack - New ability to set relative loudness of music and sound, music
//					is too soft on GP2X otherwise.
EXTERNAL_VARIABLE int Game_AudioSampleVolume;	// Sample volume as x/128 % of overall volume
EXTERNAL_VARIABLE int Game_AudioMusicVolume;		// Music volume as x/128 % of overall volume
EXTERNAL_VARIABLE int Game_MidiSubsystem;			/* MIDI subsystem
                                                       0: SDL_mixer
                                                       1: WildMidi
                                                       2: BASSMIDI
                                                       10: adlib - dosbox opl emulator
                                                       11: MT32 - munt emulator
                                                       21: native Windows
                                                       22: ALSA */
EXTERNAL_VARIABLE char *Game_SoundFontPath;			/* Path to SoundFont file */
EXTERNAL_VARIABLE char *Game_MT32RomsPath;			/* Path to MT32 roms */
EXTERNAL_VARIABLE char *Game_MidiDevice;			/* MIDI device name */

// global input variables
EXTERNAL_VARIABLE uint32_t Game_Joystick;			/* is joystick used ? */


EXTERNAL_VARIABLE FILE *Game_stdin;					/* stdin */
EXTERNAL_VARIABLE FILE *Game_stdout;				/* stdout */
EXTERNAL_VARIABLE FILE *Game_stderr;				/* stderr */

EXTERNAL_VARIABLE uint8_t *UFO_Font;				/* ufo font data */
EXTERNAL_VARIABLE uint8_t *Temp_Font_Data;			/* temporary font data */

EXTERNAL_VARIABLE volatile uint32_t Game_TimerRunning;	/* is timer interrupt running ? */
EXTERNAL_VARIABLE volatile uint32_t Game_TimerTick;	/* Timer tick counter */
EXTERNAL_VARIABLE volatile uint32_t Game_TimerRun;	/* Timer "number of runs" counter */
EXTERNAL_VARIABLE volatile uint32_t Game_VSyncTick;	/* VSync tick counter */
EXTERNAL_VARIABLE volatile uint32_t Thread_Exited;	/* did main thread exit ? */
EXTERNAL_VARIABLE volatile uint32_t Thread_Exit;	/* should thread exit ? */
EXTERNAL_VARIABLE volatile uint32_t Game_Paused;	/* is game paused? */
EXTERNAL_VARIABLE volatile uint32_t Game_SDLTicks;	/* value of SDL_GetTicks - set in timer thread */
EXTERNAL_VARIABLE volatile uint32_t Game_LastAudio;	/* time of last call to Game_ProcessAudio */

EXTERNAL_VARIABLE SDL_sem *Game_DisplaySem;
EXTERNAL_VARIABLE SDL_sem *Game_FlipSem;

EXTERNAL_VARIABLE SDL_mutex *Game_ScreenMutex;
EXTERNAL_VARIABLE volatile int Game_DisplayActive;

EXTERNAL_VARIABLE SDL_Event Game_EventKQueue[GAME_KQUEUE_LENGTH];	/* game keyboard event queue */
EXTERNAL_VARIABLE uint32_t Game_KBuffer[GAME_KBUFFER_LENGTH];		/* game keyboard buffer */
EXTERNAL_VARIABLE volatile uint32_t Game_KQueueWrite;				/* index of first empty event */
EXTERNAL_VARIABLE volatile uint32_t Game_KQueueRead;				/* index of first non-empty event */
EXTERNAL_VARIABLE volatile uint32_t Game_KBufferWrite;				/* index of first empty keystroke */
EXTERNAL_VARIABLE volatile uint32_t Game_KBufferRead;				/* index of first non-empty keystroke */
EXTERNAL_VARIABLE uint32_t Game_LastKeyStroke;						/* time of last keystroke */
EXTERNAL_VARIABLE int32_t Game_MouseX;								/* mouse x coordinate */
EXTERNAL_VARIABLE int32_t Game_MouseY;								/* mouse y coordinate */
EXTERNAL_VARIABLE uint32_t Game_MouseButtons;						/* mouse buttons current state */
EXTERNAL_VARIABLE uint32_t Game_MousePressedButtons;				/* mouse buttons - pressed */

EXTERNAL_VARIABLE int Game_ExitCode;				/* exit code */

EXTERNAL_VARIABLE char Game_ConfigFilename[80];			/* Config file being used */

EXTERNAL_VARIABLE char Game_Directory[80];			/* Directory where the game is installed */


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



extern int Game_Main_Asm(int argc, char *argv[]);
extern void Game_StopMain_Asm(void) __attribute__ ((__noreturn__));

extern void Game_RunTimer_Asm(void);

#ifdef __cplusplus
extern "C" {
#endif

extern int errno_val;

#ifdef __cplusplus
}
#endif

#endif /* _GAME_VARS_H_INCLUDED_ */
