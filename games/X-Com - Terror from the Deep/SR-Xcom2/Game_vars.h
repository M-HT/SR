/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
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

EXTERNAL_VARIABLE int Game_SDLVersionNum;			/* linked SDL version */

EXTERNAL_VARIABLE int Game_Executable;				/* current executable */
EXTERNAL_VARIABLE char **main_argv;
EXTERNAL_VARIABLE uint8_t *Game_FrameMemory;		/* allocated video memory */
EXTERNAL_VARIABLE uint8_t *Game_FrameBuffer;		/* pointer to video memory (all) */
EXTERNAL_VARIABLE PTR32(uint8_t) Game_ScreenWindow;	/* video bank (64KiB) */
EXTERNAL_VARIABLE uint32_t Game_InterruptTable[256];	/* interrupt table */
EXTERNAL_VARIABLE void *Game_MouseTable[8];			/* mouse functions table */
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
EXTERNAL_VARIABLE int32_t Game_PlayIntro;			/* play intro on start ? */

EXTERNAL_VARIABLE void *Game_DopenList;				/* list of files opened using Game_dopen */
EXTERNAL_VARIABLE void *Game_FopenList;				/* list of files opened using Game_fopen */
EXTERNAL_VARIABLE void *Game_MallocList;			/* list of memory allocated using Game_malloc */
EXTERNAL_VARIABLE void *Game_AllocateMemoryList;	/* list of memory allocated using Game_AllocateMemory */

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

EXTERNAL_VARIABLE uint32_t Game_AdvancedScaling;	/* advanced scaling enabled ? */
EXTERNAL_VARIABLE uint32_t Game_ScalingQuality;		/* scaling quality: 0 = nearest neighbour, 1 = bilinear */
EXTERNAL_VARIABLE uint32_t Game_AdvancedScaler;		/* advanced scaler: 0 = none, 1 = nearest neighbour, 2 = hqx, 3 = xbrz */
EXTERNAL_VARIABLE int Game_ScaleFactor;				/* factor for advanced scaler: 0 = max */
EXTERNAL_VARIABLE int Game_ExtraScalerThreads;		/* number of extra threads for advanced scaler: -1 = auto */

#if SDL_VERSION_ATLEAST(2,0,0)
EXTERNAL_VARIABLE SDL_Window *Game_Window;
EXTERNAL_VARIABLE SDL_Renderer *Game_Renderer;
EXTERNAL_VARIABLE SDL_Texture *Game_Texture[3];
EXTERNAL_VARIABLE SDL_Texture *Game_ScaledTexture[3];
#else
EXTERNAL_VARIABLE SDL_Surface *Game_Screen;
#ifdef ALLOW_OPENGL
EXTERNAL_VARIABLE uint32_t Game_UseOpenGL;			/* use OpenGL for drawing ? */
EXTERNAL_VARIABLE GLuint Game_GLTexture[3];
EXTERNAL_VARIABLE GLuint Game_GLFramebuffer[3];
EXTERNAL_VARIABLE GLuint Game_GLScaledTexture[3];
#endif
#endif
#if defined(ALLOW_OPENGL) || SDL_VERSION_ATLEAST(2,0,0)
EXTERNAL_VARIABLE void *Game_TextureData;
EXTERNAL_VARIABLE void *Game_ScaledTextureData;
EXTERNAL_VARIABLE int Game_CurrentTexture;
EXTERNAL_VARIABLE int Scaler_ScaleFactor;
EXTERNAL_VARIABLE int Scaler_ScaleTextureData;
EXTERNAL_VARIABLE int Scaler_ScaleTexture;
EXTERNAL_VARIABLE int Scaler_ScaledTextureWidth;
EXTERNAL_VARIABLE int Scaler_ScaledTextureHeight;
#endif

EXTERNAL_VARIABLE int Game_Delay_Game;				/* time in ms to delay the game in timer tick */
EXTERNAL_VARIABLE int Game_Main_Loop_VSync_Ticks;	/* maximum number of vsync ticks to wait in game main loop */
EXTERNAL_VARIABLE int32_t Game_Skip_Scrolling_SlowDown;	/* skip scrolling slowdown once */

// global audio variables
EXTERNAL_VARIABLE uint32_t Game_Sound;				/* is sound enabled ? */
EXTERNAL_VARIABLE uint32_t Game_Music;				/* is music enabled ? */
EXTERNAL_VARIABLE uint32_t Game_AudioMasterVolume;	/* audio master volume */
EXTERNAL_VARIABLE unsigned int Game_AudioFormat;	/* audio format */
EXTERNAL_VARIABLE unsigned int Game_AudioChannels;	/* number of audio channels */
EXTERNAL_VARIABLE int Game_AudioRate;				/* audio rate (Hz) */
EXTERNAL_VARIABLE int Game_AudioBufferSize;			/* audio buffer size */
EXTERNAL_VARIABLE int Game_ResamplingQuality;		/* audio resampling quality
                                                       0: normal quality
                                                       1: higher quality */
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
                                                       3: ADLMIDI
                                                       10: adlib - dosbox opl emulator
                                                       11: MT32 - munt emulator
                                                       12: AWE32 - pcem emu8k emulator
                                                       21: native Windows
                                                       22: ALSA
                                                       31: MT32 - native Windows
                                                       32: MT32 - ALSA */
EXTERNAL_VARIABLE int Game_LoadMidiFiles;			// Load midi files from disk ?
EXTERNAL_VARIABLE char *Game_SoundFontPath;			/* Path to SoundFont file */
EXTERNAL_VARIABLE char *Game_MT32RomsPath;			/* Path to MT32 roms */
EXTERNAL_VARIABLE char *Game_AWE32RomPath;			/* Path to AWE32 rom */
EXTERNAL_VARIABLE char *Game_MidiDevice;			/* MIDI device name */
EXTERNAL_VARIABLE int Game_OPL3BankNumber;			/* OPL3 bank number (0-77) */
EXTERNAL_VARIABLE int Game_OPL3Emulator;			/* OPL3 emulator
                                                       0: fast - DOSBox
                                                       1: precise - Nuked OPL3 */
EXTERNAL_VARIABLE int Game_MT32DelaySysex;			/* Add delays when sending sysex messages on MT-32 ? (to prevent buffer overflow with Rev.0 MT-32) */
EXTERNAL_VARIABLE int Game_SongLength;				/* Length of last loaded song */


EXTERNAL_VARIABLE PTR32(FILE) Game_stdin;			/* stdin */
EXTERNAL_VARIABLE PTR32(FILE) Game_stdout;			/* stdout */
EXTERNAL_VARIABLE PTR32(FILE) Game_stderr;			/* stderr */

EXTERNAL_VARIABLE PTR32(uint8_t) Zero_Segment;		/* 64KiB of zeros */

#if (EXE_BUILD == EXE_COMBINED)
EXTERNAL_VARIABLE void *Geoscape_DataBackup;		/* copy of geoscape data segment */
EXTERNAL_VARIABLE void *Tactical_DataBackup;		/* copy of tactical data segment */
#endif

EXTERNAL_VARIABLE volatile uint32_t Game_TimerRunning;	/* is timer interrupt running ? */
EXTERNAL_VARIABLE volatile uint32_t Game_TimerTick;	/* Timer tick counter */
EXTERNAL_VARIABLE volatile uint32_t Game_TimerRun;	/* Timer "number of runs" counter */
EXTERNAL_VARIABLE volatile uint32_t Game_VSyncTick;	/* VSync tick counter */
EXTERNAL_VARIABLE volatile uint32_t Thread_Exited;	/* did main thread exit ? */
EXTERNAL_VARIABLE volatile uint32_t Thread_Exit;	/* should thread exit ? */
EXTERNAL_VARIABLE volatile uint32_t VK_Visible;		/* is virtual keyboard visible ? */
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



extern int Game_Main_Asm(int argc, PTR32(char) argv[], void *main_proc);
extern NORETURN void Game_StopMain_Asm(void);

extern void Game_RunTimer_Asm(void *timer_proc);

#ifdef __cplusplus
extern "C" {
#endif

#if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_GEOSCAPE))
extern int32_t geoscape_errno_val;
extern int32_t geoscape_main_();
extern void geoscape_update_timer();
extern uint8_t geoscape_data_begin, geoscape_data_end, geoscape_bss_begin, geoscape_bss_end;
#endif
#if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_TACTICAL))
extern int32_t tactical_errno_val;
extern int32_t tactical_main_();
extern void tactical_update_timer();
extern uint8_t tactical_data_begin, tactical_data_end, tactical_bss_begin, tactical_bss_end;
#endif

#ifdef __cplusplus
}
#endif

#endif /* _GAME_VARS_H_INCLUDED_ */
