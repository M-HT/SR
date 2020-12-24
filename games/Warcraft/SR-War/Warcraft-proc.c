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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>		/* needed for procedure Game_openFlags */
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
#else
    #include <SDL/SDL.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Warcraft-proc.h"
#include "Warcraft-timer.h"
#include "Game_thread.h"

#if defined(__MINGW32__)
    #include <sys/timeb.h>
#endif

int Game_errno(void)
{
    int err;

    err = errno;

    return (err >= 0 && err < 256)?(errno_rtable[err]):(err);
}

uint32_t Game_clock(void)
{
    uint32_t ret;

    if (Thread_Exit) Game_StopMain();

    ret = (SDL_GetTicks() - Game_BaseClockValue) / 10;
    if (ret == Game_LastClockValue)
    {
        SDL_Delay(1);
        return (SDL_GetTicks() - Game_BaseClockValue) / 10;
    }
    else
    {
        Game_LastClockValue = ret;
        return ret;
    }
}

int Game_cputs(const char *buf)
{
    int ret;

    if (buf != NULL)
    {
        if (*buf == 0) return 0;
    }

    ret = printf("%s", buf);

    return (ret)?0:EOF;
}

void Game_delay(uint32_t milliseconds)
{
    uint32_t end;

    if (milliseconds == 0) return;

    end = milliseconds + SDL_GetTicks();

    while (SDL_GetTicks() <= end)
    {
        Game_RunTimerDelay();
    }
}

void Game_dos_gettime(void *dtime)
{
#pragma pack(1)
    struct __attribute__ ((__packed__)) watcom_dostime_t {
        unsigned char hour; /* 0-23 */
        unsigned char minute; /* 0-59 */
        unsigned char second; /* 0-59 */
        unsigned char hsecond; /* 1/100 second; 0-99 */
    };
#pragma pack()

#define DTIME ( (struct watcom_dostime_t *) dtime )

    struct tm *lt;

#if defined(__MINGW32__)
    struct timeb tb;

    ftime(&tb);

    lt = localtime(&tb.time);

    DTIME->hour = lt->tm_hour;
    DTIME->minute = lt->tm_min;
    DTIME->second = lt->tm_sec;
    DTIME->hsecond = tb.millitm / 10;
#else
    struct timeval tv;

    gettimeofday(&tv, NULL);

    lt = localtime(&tv.tv_sec);

    DTIME->hour = lt->tm_hour;
    DTIME->minute = lt->tm_min;
    DTIME->second = lt->tm_sec;
    DTIME->hsecond = tv.tv_usec / 10000;
#endif
}

int Game_fmemcmp(void *s1, uint32_t s1_seg, const void *s2, uint32_t s2_seg, size_t n)
{
    return memcmp(s1, s2, n);
}

uint64_t Game_fmemcpy(void *dest, uint32_t dest_seg, const void *src, uint32_t src_seg, size_t n)
{
    uint32_t ret2;

    ret2 = (uint32_t) memcpy(dest, src, n);

    return ret2 + ( ((uint64_t) dest_seg) << 32 );
}

uint64_t Game_fmemset(void *s, uint32_t s_seg, int c, size_t n)
{
    uint32_t ret2;

    ret2 = (uint32_t) memset(s, c, n);

    return ret2 + ( ((uint64_t) s_seg) << 32 );
}

int Game_raise(int condition)
{
    int newcondition;

    if (Thread_Exit) Game_StopMain();

    newcondition = (condition >= 0 && condition < SIGNAL_NUM)?signal_table[condition]:0;

    if (newcondition == 0)
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Invalid signal number... (%i)\n", condition);
#endif
        Game_StopMain();
    }

    return raise(newcondition);
}

#define WATCOM_IOFBF  0x0100  /* full buffering */
#define WATCOM_IOLBF  0x0200  /* line buffering */
#define WATCOM_IONBF  0x0400  /* no buffering */

int Game_setvbuf(FILE *fp, char *buf, int mode, size_t size)
{
    int newmode;

    if (mode == WATCOM_IOFBF) newmode = _IOFBF;
    else if (mode == WATCOM_IOLBF) newmode = _IOLBF;
    else if (mode == WATCOM_IONBF) newmode = _IONBF;
    else return 1;

    return setvbuf(fp, buf, newmode, size);
}

#define WATCOM_SIG_IGN         ((sighandler_t) 1)
#define WATCOM_SIG_DFL         ((sighandler_t) 2)
#define WATCOM_SIG_ERR         ((sighandler_t) 3)

sighandler_t Game_signal(int signum, sighandler_t handler)
{
    int newsignum;
    sighandler_t newhandler, oldhandler;

    newsignum = (signum >= 0 && signum < SIGNAL_NUM)?signal_table[signum]:0;

    if (newsignum == 0)
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Invalid signal number... (%i)\n", signum);
#endif
        Game_StopMain();
    }

    if (handler == WATCOM_SIG_IGN) newhandler = SIG_IGN;
    else if (handler == WATCOM_SIG_DFL) newhandler = SIG_DFL;
    else if (handler == WATCOM_SIG_ERR) newhandler = SIG_ERR;
    else newhandler = handler;

    oldhandler = signal(newsignum, newhandler);

    if (oldhandler == SIG_IGN) return WATCOM_SIG_IGN;
    else if (oldhandler == SIG_DFL) return WATCOM_SIG_DFL;
    else if (oldhandler == SIG_ERR) return WATCOM_SIG_ERR;
    else return oldhandler;
}

void Game_FlipScreen(void)
{
    SDL_Event event;

    event.type = SDL_USEREVENT;
    event.user.code = EC_DISPLAY_FLIP_START;
    event.user.data1 = NULL;
    event.user.data2 = NULL;

    SDL_PushEvent(&event);
}

void Game_Sync(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "sync\n");
#endif

#if defined(__WIN32__)
#else
    sync();
#endif
}

#define WATCOM_O_RDONLY       0x0000  /* open for read only */
#define WATCOM_O_WRONLY       0x0001  /* open for write only */
#define WATCOM_O_RDWR         0x0002  /* open for read and write */
#define WATCOM_O_APPEND       0x0010  /* writes done at end of file */
#define WATCOM_O_CREAT        0x0020  /* create new file */
#define WATCOM_O_TRUNC        0x0040  /* truncate existing file */
#define WATCOM_O_NOINHERIT    0x0080  /* file is not inherited by child process */
#define WATCOM_O_TEXT         0x0100  /* text file */
#define WATCOM_O_BINARY       0x0200  /* binary file */
#define WATCOM_O_EXCL         0x0400  /* exclusive open */

int Game_openFlags(int flags)
{
    int newflags;

#define NO_FLAG_DEFINED

    newflags = 0;
#if defined(_O_RDONLY) || defined(O_RDONLY)
    #if !defined(O_RDONLY)
        #define O_RDONLY _O_RDONLY
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_RDONLY) newflags |= O_RDONLY;
#endif

#if defined(_O_WRONLY) || defined(O_WRONLY)
    #if !defined(O_WRONLY)
        #define O_WRONLY _O_WRONLY
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_WRONLY) newflags |= O_WRONLY;
#endif

#if defined(_O_RDWR) || defined(O_RDWR)
    #if !defined(O_RDWR)
        #define O_RDWR _O_RDWR
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_RDWR) newflags |= O_RDWR;
#endif

#if defined(_O_APPEND) || defined(O_APPEND)
    #if !defined(O_APPEND)
        #define O_APPEND _O_APPEND
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_APPEND) newflags |= O_APPEND;
#endif

#if defined(_O_CREAT) || defined(O_CREAT)
    #if !defined(O_CREAT)
        #define O_CREAT _O_CREAT
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_CREAT) newflags |= O_CREAT;
#endif

#if defined(_O_TRUNC) || defined(O_TRUNC)
    #if !defined(O_TRUNC)
        #define O_TRUNC _O_TRUNC
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_TRUNC) newflags |= O_TRUNC;
#endif

#if defined(_O_NOINHERIT) || defined(O_NOINHERIT)
    #if !defined(O_NOINHERIT)
        #define O_NOINHERIT _O_NOINHERIT
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_NOINHERIT) newflags |= O_NOINHERIT;
#endif

#if defined(_O_TEXT) || defined(O_TEXT)
    #if !defined(O_TEXT)
        #define O_TEXT _O_TEXT
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_TEXT) newflags |= O_TEXT;
#endif

#if defined(_O_BINARY) || defined(O_BINARY)
    #if !defined(O_BINARY)
        #define O_BINARY _O_BINARY
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_BINARY) newflags |= O_BINARY;
#endif

#if defined(_O_EXCL) || defined(O_EXCL)
    #if !defined(O_EXCL)
        #define O_EXCL _O_EXCL
    #endif

    #undef NO_FLAG_DEFINED

    if (flags & WATCOM_O_EXCL) newflags |= O_EXCL;
#endif

#if defined(NO_FLAG_DEFINED)
    #error fcntl.h not included
#endif

#undef NO_FLAG_DEFINED

    return newflags;
}

