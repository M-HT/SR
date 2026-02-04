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

#define _FILE_OFFSET_BITS 64
#define _TIME_BITS 64
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
#include <io.h>
#include <sys/timeb.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Warcraft-proc.h"
#include "Warcraft-timer.h"
#include "Game_memory.h"
#include "Game_thread.h"



#define SIGNAL_NUM 8

#if !defined(SIGBREAK)
    #define SIGBREAK SIGTSTP
#endif
static int signal_table[SIGNAL_NUM] = {
    0, SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM, SIGBREAK
};


int32_t Game_errno(void)
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

int32_t Game_cputs(const char *buf)
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

int32_t Game_fclose(void *stream)
{
    FILE *fp;

    if ((sizeof(void *) > 4))
    {
        fp = *(FILE **)stream;
        x86_free(stream);
    }
    else fp = (FILE *)stream;

    return fclose(fp);
}

int32_t Game_fgetc(void *stream)
{
    return fgetc((sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream);
}

int32_t Game_ftell(void *stream)
{
    return ftell((sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream);
}

int32_t Game_fputc(int32_t c, void *stream)
{
    return fputc(c, (sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream);
}

int32_t Game_fputs(const char *s, void *stream)
{
    return fputs(s, (sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream);
}

char *Game_fgets(char *s, int32_t size, void *stream)
{
    return fgets(s, size, (sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream);
}

int32_t Game_fseek(void *stream, int32_t offset, int32_t whence)
{
    return fseek((sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream, offset, whence);
}

uint32_t Game_fread(void *ptr, uint32_t size, uint32_t nmemb, void *stream)
{
    return (uint32_t)fread(ptr, size, nmemb, (sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream);
}

uint32_t Game_fwrite(const void *ptr, uint32_t size, uint32_t nmemb, void *stream)
{
    return (uint32_t)fwrite(ptr, size, nmemb, (sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream);
}

void Game_dos_gettime(void *dtime)
{
#pragma pack(1)
    struct PACKED watcom_dostime_t {
        uint8_t hour; /* 0-23 */
        uint8_t minute; /* 0-59 */
        uint8_t second; /* 0-59 */
        uint8_t hsecond; /* 1/100 second; 0-99 */
    };
#pragma pack()

#define DTIME ( (struct watcom_dostime_t *) dtime )

    struct tm *lt;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
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

int32_t Game_fmemcmp(void *s1, uint32_t s1_seg, const void *s2, uint32_t s2_seg, uint32_t n)
{
    return memcmp(s1, s2, n);
}

void *Game_fmemcpy(void *dest, uint32_t dest_seg, const void *src, uint32_t src_seg, uint32_t n)
{
    return memcpy(dest, src, n);
}

void *Game_fmemset(void *s, uint32_t s_seg, int32_t c, uint32_t n)
{
    return memset(s, c, n);
}

void *Game_malloc(uint32_t size)
{
    return x86_malloc(size);
}

void Game_free(void *ptr)
{
    x86_free(ptr);
}

#define WATCOM_IOFBF  0x0100  /* full buffering */
#define WATCOM_IOLBF  0x0200  /* line buffering */
#define WATCOM_IONBF  0x0400  /* no buffering */

int32_t Game_setvbuf(void *fp, char *buf, int32_t mode, uint32_t size)
{
    int newmode;

    if (mode == WATCOM_IOFBF) newmode = _IOFBF;
    else if (mode == WATCOM_IOLBF) newmode = _IOLBF;
    else if (mode == WATCOM_IONBF) newmode = _IONBF;
    else return 1;

    return setvbuf((sizeof(void *) > 4) ? *(FILE **)fp : (FILE *)fp, buf, newmode, size);
}

#define WATCOM_SIG_IGN 1
#define WATCOM_SIG_DFL 2
#define WATCOM_SIG_ERR 3

uint32_t Game_signal(int32_t signum, uint32_t handler)
{
    int newsignum;
    void (*newhandler)(int), (*oldhandler)(int);

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
    else
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Unsupported signal handler... (%i)\n", handler);
#endif
        Game_StopMain();
    }

    oldhandler = signal(newsignum, newhandler);

    if (oldhandler == SIG_IGN) return WATCOM_SIG_IGN;
    else if (oldhandler == SIG_DFL) return WATCOM_SIG_DFL;
    else if (oldhandler == SIG_ERR) return WATCOM_SIG_ERR;
    else return 0; // oldhandler
}

uint32_t Game_strtoul(const char *nptr, PTR32(char) *endptr, int32_t base)
{
    char *endptr2;
    unsigned long ret;

    ret = strtoul(nptr, &endptr2, base);
    if (endptr != NULL) *endptr = endptr2;
    return ret;
}

int32_t Game_time(int32_t *tloc)
{
    time_t t;

    t = time(NULL);

    if (tloc != NULL) *tloc = (int32_t)t;

    return (int32_t)t;
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

