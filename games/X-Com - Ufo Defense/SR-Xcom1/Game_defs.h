/**
 *
 *  Copyright (C) 2016-2026 Roman Pauer
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

#if !defined(_GAME_DEFS_H_INCLUDED_)
#define _GAME_DEFS_H_INCLUDED_

#include <SDL.h>
#include <SDL_mixer.h>
#include <limits.h>
#include "ptr32.h"


#define EXE_COMBINED 0
#define EXE_GEOSCAPE 1
#define EXE_TACTICAL 2
#define EXE_INTRO 3

#if !defined(EXE_BUILD)
#define EXE_BUILD EXE_COMBINED
#elif (EXE_BUILD < 0 || EXE_BUILD > 3)
#error EXE_BUILD has unsupported value
#endif


// channel number where the sound samples are played
#define GAME_SOUND_CHANNEL (0)


/* Maximum available memory = 16MiB */
#define GAME_MAX_FREE_MEMORY (0x1000000)

/* must be a power of 2 */
#define GAME_KQUEUE_LENGTH (256)

/* must be a power of 2 */
#define GAME_KBUFFER_LENGTH (256)


#define GAME_KEYBOARD_TYPE_RATE (30)


#define GAME_MAX_SCALE_FACTOR (6)


#if !defined(MAX_PATH)
    #if defined(_MAX_PATH)
        #define MAX_PATH _MAX_PATH
    #elif defined(_POSIX_PATH_MAX)
        #define MAX_PATH _POSIX_PATH_MAX
    #elif defined(PATH_MAX)
        #define MAX_PATH PATH_MAX
    #else
        #define MAX_PATH 256
    #endif
#endif


#define EC_DISPLAY_CREATE		(0)
#define EC_DISPLAY_DESTROY		(1)
#define EC_DISPLAY_FLIP_START	(2)
#define EC_DISPLAY_FLIP_FINISH	(3)
#define EC_PROGRAM_QUIT			(4)
#define EC_MOUSE_MOVE			(5)
#define EC_MOUSE_SET 			(6)
#define EC_GET_MOUSE_POS		(7)
#define EC_INPUT_KEY 			(8)
//senquack:
#define EC_DELAY				(9)


#if defined(__GNUC__)
    #define INLINE __inline__
    #define NOINLINE __attribute__ ((__noinline__))
    #define NORETURN __attribute__ ((__noreturn__))
    #define PACKED __attribute__ ((__packed__))
    #if defined(__i386) || (defined(__x86_64) && defined(_WIN32))
        #define CCALL __attribute__ ((__cdecl__))
    #else
        #define CCALL
    #endif
#elif defined(_MSC_VER)
    #define INLINE __inline
    #define NOINLINE __declspec(noinline)
    #define NORETURN __declspec(noreturn)
    #define PACKED
    #define CCALL __cdecl
    #define chdir _chdir
    #define close _close
    #define fileno _fileno
    #define getcwd _getcwd
    #define lseek _lseek
    #define open _open
    #define read _read
    #define strcasecmp _stricmp
    #define strdup _strdup
    #define strncasecmp _strnicmp
    #define tzset _tzset
    #ifdef _WIN64
        typedef __int64 ssize_t;
    #else
        typedef int ssize_t;
    #endif
    #if _MSC_VER >= 1900
        #pragma comment(lib, "legacy_stdio_definitions.lib")
    #endif
#else
    #define INLINE inline
    #define NOINLINE
    #define NORETURN
    #define PACKED
    #define CCALL
#endif


#pragma pack(1)

typedef struct _Game_DPMIDWORDREGS_ {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t reserved;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint16_t flags;
  uint16_t es;
  uint16_t ds;
  uint16_t fs;
  uint16_t ip;
  uint16_t cs;
  uint16_t sp;
  uint16_t ss;
} Game_DPMIDWORDREGS;

typedef struct _Game_DPMIWORDREGS_ {
  uint16_t di, _upper_di;
  uint16_t si, _upper_si;
  uint16_t bp, _upper_bp;
  uint32_t reserved;
  uint16_t bx, _upper_bx;
  uint16_t dx, _upper_dx;
  uint16_t cx, _upper_cx;
  uint16_t ax, _upper_ax;
  uint16_t flags;
  uint16_t es;
  uint16_t ds;
  uint16_t fs;
  uint16_t ip;
  uint16_t cs;
  uint16_t sp;
  uint16_t ss;
} Game_DPMIWORDREGS;

typedef struct _Game_DPMIBYTEREGS_ {
  uint16_t di, _upper_di;
  uint16_t si, _upper_si;
  uint16_t bp, _upper_bp;
  uint32_t reserved;
  uint8_t bl;
  uint8_t bh;
  uint16_t _upper_bx;
  uint8_t dl;
  uint8_t dh;
  uint16_t _upper_dx;
  uint8_t cl;
  uint8_t ch;
  uint16_t _upper_cx;
  uint8_t al;
  uint8_t ah;
  uint16_t _upper_ax;
  uint16_t flags;
  uint16_t es;
  uint16_t ds;
  uint16_t fs;
  uint16_t ip;
  uint16_t cs;
  uint16_t sp;
  uint16_t ss;
} Game_DPMIBYTEREGS;

typedef struct _Game_DPMIPTR32REGS_ {
  PTR32_ALIGN(void, 1) edi;
  PTR32_ALIGN(void, 1) esi;
  PTR32_ALIGN(void, 1) ebp;
  PTR32_ALIGN(void, 1) reserved;
  PTR32_ALIGN(void, 1) ebx;
  PTR32_ALIGN(void, 1) edx;
  PTR32_ALIGN(void, 1) ecx;
  PTR32_ALIGN(void, 1) eax;
} Game_DPMIPTR32REGS;

typedef union _Game_DPMIREGS_ {
  Game_DPMIDWORDREGS d;
  Game_DPMIWORDREGS w;
  Game_DPMIBYTEREGS h;
  Game_DPMIPTR32REGS p;
} Game_DPMIREGS;

#pragma pack()


#pragma pack(4)

typedef union _Game_register_ {
    uint32_t e;
    struct {
        uint16_t x, x2;
    } w;
    struct {
        uint8_t l, h, l2, h2;
    } b;
    PTR32(void) p;
} Game_register;


typedef void (*Game_Flip_Procedure)(void *src, void *dst);

typedef union _pixel_format_orig_ {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } s;
    uint8_t c[4];
    uint32_t pix;
} pixel_format_orig;

typedef struct _Game_sample {
    int active, _stereo, _16bit, _signed;
    void *start;
    uint32_t len, len_cvt;
    int32_t playback_rate;	// Hz
    char *sound;            // address of audio data.
    int16_t *IsPlaying;     // Address of play status flag.
} Game_sample;

typedef struct _Game_sequence {
    uint32_t volume;			/* 0-128 */
    uint8_t *midi;
    uint32_t midi_size;
    SDL_RWops *midi_RW;
    Mix_Music *midi_music;
} Game_sequence;

#pragma pack()


#pragma pack(2)

typedef struct PACKED _Game_SoundConfig_
{
    uint16_t SoundDriver;
        // 0 - soundblaster mono 8-bit / soundblaster stereo / soundblaster 16 or awe32 / pro audio spectrum / pro audio spectrum 16
        // 1 -
        // 2 - gravis ultrasound
        // 3 - no sound
    uint16_t SoundBasePort;
    uint16_t SoundIRQ;
    uint16_t SoundDMA;
    uint16_t MusicDriver;
        // 0 - adlib / soundblaster fm
        // 1 - roland lapc-1 / mt32
        // 2 - gravis ultrasound
        // 3 - soundblaster awe32 / general midi
        // 4 - no music
    uint16_t MusicBasePort;
    uint16_t SoundChannels;
    uint16_t SoundSwapStereo;
    uint16_t Reserved;
} Game_SoundConfig;

#pragma pack()

#endif /* _GAME_DEFS_H_INCLUDED_ */
