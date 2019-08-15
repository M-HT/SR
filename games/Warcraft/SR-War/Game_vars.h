/**
 *
 *  Copyright (C) 2016-2018 Roman Pauer
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
#include <signal.h>
#include <SDL/SDL.h>
#ifdef ALLOW_OPENGL
    #include <SDL/SDL_opengl.h>
#endif
#include "Game_defs.h"

#if defined(DEFINE_VARIABLES)
    #define EXTERNAL_VARIABLE
#else
    #define EXTERNAL_VARIABLE extern
#endif

EXTERNAL_VARIABLE uint32_t X86_InterruptFlag;		/* interrupt flag indicator */

EXTERNAL_VARIABLE SDL_Surface *Game_Screen;
EXTERNAL_VARIABLE uint8_t *Game_FrameBuffer;		/* video memory (all) */
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
#ifdef ALLOW_OPENGL
EXTERNAL_VARIABLE uint32_t Game_UseOpenGL;			/* use OpenGL for drawing ? */
EXTERNAL_VARIABLE void *Game_TextureData;
EXTERNAL_VARIABLE GLuint Game_GLTexture[3];
EXTERNAL_VARIABLE int Game_CurrentTexture;
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
EXTERNAL_VARIABLE int Game_InterpolateAudio;		/* interpolate audio when resampling ? */
EXTERNAL_VARIABLE AIL_sequence* Game_MainSequence;	/* main sequence - priority sequence */
EXTERNAL_VARIABLE AIL_sequence* Game_ActiveSequence;	/* active sequence - sequence being played */
EXTERNAL_VARIABLE int Game_VolumeDelta;				/*  0: Volume is stationary
                                                        1: Volume is increasing
                                                       -1: Volume is decreasing	*/
EXTERNAL_VARIABLE int Game_AudioMusicVolume;		// Music volume relative to overall volume (0-127)
EXTERNAL_VARIABLE int Game_MidiSubsystem;			/* MIDI subsystem
                                                       0: SDL_mixer
                                                       1: WildMidi
                                                       2: BASSMIDI
                                                       3: ADLMIDI
                                                       11: native Windows
                                                       12: ALSA */
EXTERNAL_VARIABLE char *Game_SoundFontPath;			/* Path to SoundFont file */
EXTERNAL_VARIABLE char *Game_MidiDevice;			/* MIDI device name */

// global input variables
EXTERNAL_VARIABLE uint32_t Game_Joystick;			/* is joystick used ? */


EXTERNAL_VARIABLE Game_sample *Game_SampleCache[GAME_SAMPLE_CACHE_SIZE];
EXTERNAL_VARIABLE Game_channel Game_channels[GAME_MIXER_CHANNELS_MAXIMUM];
EXTERNAL_VARIABLE AIL_sample Game_samples[GAME_SAMPLES_MAXIMUM];


EXTERNAL_VARIABLE uint32_t Game_BaseClockValue;		/* clock base value */
EXTERNAL_VARIABLE uint32_t Game_LastClockValue;		/* last value returned by clock */
EXTERNAL_VARIABLE FILE *Game_stdin;					/* stdin */
EXTERNAL_VARIABLE FILE *Game_stdout;				/* stdout */
EXTERNAL_VARIABLE FILE *Game_stderr;				/* stderr */

EXTERNAL_VARIABLE uint32_t Game_Registers[8];		/* used to save registers when replacing stack call based procedures */

EXTERNAL_VARIABLE uint8_t *Warcraft_Font;			/* warcraft font data */
EXTERNAL_VARIABLE uint8_t *Temp_Font_Data;			/* temporary font data */


EXTERNAL_VARIABLE volatile uint32_t Game_TimerRunning;	/* is timer interrupt running ? */
EXTERNAL_VARIABLE volatile uint32_t Game_TimerTick;	/* Timer tick counter */
EXTERNAL_VARIABLE volatile uint32_t Game_TimerRun;	/* Timer number of runs counter */
EXTERNAL_VARIABLE volatile uint32_t Game_VSyncTick;	/* VSync tick counter */
EXTERNAL_VARIABLE volatile uint32_t Thread_Exited;	/* did main thread exit ? */
EXTERNAL_VARIABLE volatile uint32_t Thread_Exit;	/* should thread exit ? */
EXTERNAL_VARIABLE volatile uint32_t Game_Paused;	/* is game paused? */

EXTERNAL_VARIABLE SDL_sem *Game_DisplaySem;
EXTERNAL_VARIABLE SDL_sem *Game_FlipSem;

EXTERNAL_VARIABLE SDL_mutex *Game_ScreenMutex;
EXTERNAL_VARIABLE volatile int Game_DisplayActive;

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

EXTERNAL_VARIABLE uint32_t errno_rtable[256];		/* reverse errno table */

#undef EXTERNAL_VARIABLE

#define ERRNO_NUM 41
#define SIGNAL_NUM 8

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


#if !defined(SIGBREAK)
    #define SIGBREAK SIGTSTP
#endif
int signal_table[SIGNAL_NUM] = {
    0, SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM, SIGBREAK
};
#else
/*extern const uint32_t errno_table[ERRNO_NUM];*/
extern const int signal_table[SIGNAL_NUM];
#endif


extern char **argv_val;


extern int Game_Main_Asm(int argc, char *argv[]);
extern void Game_StopMain_Asm(void) __attribute__ ((__noreturn__));

extern uint32_t Game_MouseMove(uint32_t state, uint32_t x, uint32_t y);
extern uint32_t Game_MouseButton(uint32_t state, uint32_t action, uint32_t x, uint32_t y);
extern void Game_RunTimer_Asm(void);
extern void Game_RunInt9_Asm(void);
extern void Game_RunAILcallback_Asm(AIL_sample_CB callback, AIL_sample *sample);

#ifdef __cplusplus
extern "C" {
#endif

extern int errno_val;
extern int32_t default_sample_volume;
extern int32_t default_sequence_volume;

#ifdef __cplusplus
}
#endif

#endif /* _GAME_VARS_H_INCLUDED_ */
