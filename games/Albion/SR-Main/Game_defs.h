/**
 *
 *  Copyright (C) 2016 Roman Pauer
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

#include <limits.h>
#include <inttypes.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL/SDL.h>
    #include <SDL/SDL_mixer.h>
#endif


#if defined(__arm__) || defined(_ARM)
    #define CALL_PARAMS1 uint32_t tmpreg1 __attribute__ ((__unused__)), uint32_t tmpreg2 __attribute__ ((__unused__)), uint32_t tmpreg3 __attribute__ ((__unused__)), uint32_t tmpreg4 __attribute__ ((__unused__)),
#else
    #define CALL_PARAMS1
#endif


/* Maximum available memory = 16MiB */
#define GAME_MAX_FREE_MEMORY (0x1000000)

/* must be a power of 2 */
#define GAME_MQUEUE_LENGTH (256)

/* must be a power of 2 */
#define GAME_KQUEUE_LENGTH (256)

/* must be a power of 2 */
#define GAME_KBUFFER_LENGTH (256)


#define GAME_KEYBOARD_TYPE_RATE (30)


#if !defined(MAX_PATH)
    #if defined(_POSIX_PATH_MAX)
        #define MAX_PATH _POSIX_PATH_MAX
    #else
        #define MAX_PATH 256
    #endif
#endif


#define GAME_MIXER_CHANNELS_INITIAL (16)

#define GAME_SAMPLE_CACHE_SIZE (32)

#define EC_DISPLAY_CREATE		(0)
#define EC_DISPLAY_DESTROY		(1)
#define EC_DISPLAY_FLIP_START	(2)
#define EC_DISPLAY_FLIP_FINISH	(3)
#define EC_PROGRAM_QUIT			(4)
#define EC_MOUSE_MOVE			(5)
#define EC_MOUSE_SET 			(6)
#define EC_INPUT_KEY 			(7)


typedef enum {
    AL_UNKNOWN = 0,
    AL_ENG_FRE,
    AL_CZE,
    AL_GER
} Albion_Lang;

#pragma pack(1)

typedef struct __attribute__ ((__packed__)) _Game_DPMIDWORDREGS_ {
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

typedef struct __attribute__ ((__packed__)) _Game_DPMIWORDREGS_ {
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

typedef struct __attribute__ ((__packed__)) _Game_DPMIBYTEREGS_ {
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

typedef union __attribute__ ((__packed__)) _Game_DPMIREGS_ {
  Game_DPMIDWORDREGS d;
  Game_DPMIWORDREGS w;
  Game_DPMIBYTEREGS h;
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
} Game_register;

typedef struct _Game_SREGS_ {
  uint16_t es;
  uint16_t cs;
  uint16_t ss;
  uint16_t ds;
  uint16_t fs;
  uint16_t gs;
} Game_SREGS;

typedef struct _Game_DWORDREGS_ {
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t esi;
  uint32_t edi;
  uint32_t cflag;
} Game_DWORDREGS;

typedef struct _Game_WORDREGS_ {
  uint16_t ax, _upper_ax;
  uint16_t bx, _upper_bx;
  uint16_t cx, _upper_cx;
  uint16_t dx, _upper_dx;
  uint16_t si, _upper_si;
  uint16_t di, _upper_di;
  uint32_t cflag;
} Game_WORDREGS;

typedef struct _Game_BYTEREGS_ {
  uint8_t al;
  uint8_t ah;
  uint16_t _upper_ax;
  uint8_t bl;
  uint8_t bh;
  uint16_t _upper_bx;
  uint8_t cl;
  uint8_t ch;
  uint16_t _upper_cx;
  uint8_t dl;
  uint8_t dh;
  uint16_t _upper_dx;
  uint16_t si, _upper_si;
  uint16_t di, _upper_di;
  uint32_t cflag;
} Game_BYTEREGS;

typedef union _Game_REGS_ {
  Game_DWORDREGS d;
  Game_WORDREGS w;
  Game_BYTEREGS h;
} Game_REGS;


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

typedef struct _Game_OverlayInfo
{
    int32_t Enhanced3DEngineUsed;
    uint8_t *ScreenViewpartOverlay;
    uint8_t *ScreenViewpartOriginal;
    uint32_t ViewportX;
    uint32_t ViewportY;
    uint32_t ViewportWidth;
    uint32_t ViewportHeight;
    uint32_t OverlayX;
    uint32_t OverlayY;
    uint32_t OverlayWidth;
    uint32_t OverlayHeight;
} Game_OverlayInfo;


#pragma pack()


typedef struct _Game_sample {
    struct _Game_sample **self_ptr;
    int num_ref;
    int _stereo, _16bit, _signed;
    uint8_t *start;
    uint32_t len, len_cvt, time;
    int32_t playback_rate;	/* Hz */
    uint32_t orig_data[6];
    uint32_t orig_data_offset[3];
    uint8_t data[];
} Game_sample;

typedef struct _AIL_sample
{
    int channel;
    Game_sample *sample;
    int _stereo, _16bit, _signed;
    void *start;
    uint32_t len;
    int32_t playback_rate;	/* Hz */
    int32_t volume;			/* 0-127 */
    int32_t pan;			/* (0=L ... 127=R) */
    int32_t loop_count;
    Mix_Chunk chunk;
} AIL_sample;

typedef struct _AIL_sequence
{
    int status;
    void *start;
    int32_t sequence_num;
    int32_t volume;			/* 0-127 */
    int32_t loop_count;
    uint8_t *midi;
    uint32_t midi_size;
    SDL_RWops *midi_RW;
    Mix_Music *midi_music;
    void *mp_sequence;
} AIL_sequence;


#endif /* _GAME_DEFS_H_INCLUDED_ */
