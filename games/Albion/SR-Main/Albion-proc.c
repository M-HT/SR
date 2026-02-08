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
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-proc.h"
#include "Albion-timer.h"
#include "Game_memory.h"
#include "Game_misc.h"
#include "Game_thread.h"


int32_t CCALL Game_errno(void)
{
    int err;

    err = errno;

    return (err >= 0 && err < 256)?(errno_rtable[err]):(err);
}

void Game_Set_errno_val(void)
{
    errno_val = Game_errno();
}

void Game_Set_errno_error(int errornum)
{
    errno_val = (errornum >= 0 && errornum < 256)?(errno_rtable[errornum]):(errornum);
}

char * CCALL Game_ctime2(const int32_t *timep, char *buf, int32_t max)
{
    time_t t2;

    t2 = *timep;
    if (0 == strftime(buf, max, "%a %b %d %H:%M:%S %Y\n", localtime(&t2)))
    {
        buf[0] = 0;
    }

    return buf;
}

static int32_t Game_fclose2(void *stream)
{
    FILE *fp;
    int ret;

    if ((sizeof(void *) > 4))
    {
        fp = *(FILE **)stream;
        x86_free(stream);
    }
    else fp = (FILE *)stream;

    ret = fclose(fp);
    Game_Set_errno_val();

    return ret;
}

int32_t CCALL Game_fclose(void *stream)
{
    Game_list_remove(&Game_FopenList, (uintptr_t)stream);

    return Game_fclose2(stream);
}

int32_t CCALL Game_fcloseall(void)
{
    Game_list_clear(&Game_FopenList, (void (*)(uintptr_t)) &Game_fclose2);
    return 0;
}

int32_t CCALL Game_fputs(const char *s, void *stream)
{
    return fputs(s, (sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream);
}

char * CCALL Game_fgets(char *s, int32_t size, void *stream)
{
    return fgets(s, size, (sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream);
}

int32_t CCALL Game_ftime(watcom_timeb *tp)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    struct timeb tb;

    ftime(&tb);

    tp->time = (int32_t)tb.time;
    tp->millitm = tb.millitm;
    tp->timezone = tb.timezone;
    tp->dstflag = tb.dstflag;
#else
    struct timeval tv;

    gettimeofday(&tv, NULL);

    tp->time = (int32_t)tv.tv_sec;
    tp->millitm = tv.tv_usec / 1000;
    tp->timezone = 0;
    tp->dstflag = 0;
#endif

    return 0;
}

uint32_t CCALL Game_dos_getvect(const int32_t intnum)
{
    return Game_InterruptTable[intnum & 0xff];
}

void CCALL Game_dos_setvect(const int32_t intnum, const uint32_t handler_low, const uint32_t handler_high)
{
#if defined(__DEBUG__)
    fprintf(stderr, "Setting interrupt vector: %i\n", intnum & 0xff);
#endif

    Game_InterruptTable[intnum & 0xff] = handler_low;
}

void CCALL Game_Sync(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "sync\n");
#endif

#if defined(__WIN32__)
#else
    sync();
#endif
}

static void Game_WaitVerticalRetraceTicks(const int32_t ticks);
void CCALL Game_WaitTimerTicks(const int32_t ticks)
{
    Game_WaitVerticalRetraceTicks(ticks);
}

static void Game_WaitVerticalRetraceTicks(const int32_t ticks)
{
    uint32_t VSyncTick;

    VSyncTick = Game_VSyncTick;

    if (Thread_Exit) Game_StopMain();

    if (ticks > 0)
    {
        while (1)
        {
            if (ticks > 0 &&
                ( (0xffffffff - VSyncTick >= (uint32_t) ticks && VSyncTick + (uint32_t) ticks > Game_VSyncTick) ||
                (0xffffffff - VSyncTick < (uint32_t) ticks && ( VSyncTick < Game_VSyncTick || VSyncTick + (uint32_t) ticks > Game_VSyncTick) ) ) )
            {
                Game_RunTimerDelay();
                if (Thread_Exit) Game_StopMain();
            }
            else
            {
                break;
            }
        };
    }

}

void CCALL Game_WaitAfterVerticalRetrace(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "WaitAfterVerticalRetrace\n");
#endif

    Game_WaitVerticalRetraceTicks(0);
}

void CCALL Game_WaitForVerticalRetrace(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "WaitForVerticalRetrace\n");
#endif

    Game_WaitVerticalRetraceTicks(1);
}

void CCALL Game_WaitAfter2ndVerticalRetrace(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "WaitAfter2ndVerticalRetrace\n");
#endif

    Game_WaitVerticalRetraceTicks(1);
}

void CCALL Game_WaitFor2ndVerticalRetrace(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "WaitFor2ndVerticalRetrace\n");
#endif

    Game_WaitVerticalRetraceTicks(2);
}

#define WATCOM_MAX_PATH   144 /* maximum length of full pathname */
#define WATCOM_MAX_DRIVE   3  /* maximum length of drive component */
#define WATCOM_MAX_DIR    130 /* maximum length of path component */
#define WATCOM_MAX_FNAME   9  /* maximum length of file name component */
#define WATCOM_MAX_EXT     5  /* maximum length of extension component */

void CCALL Game_splitpath(const char *path, char *drive, char *dir, char *fname, char *ext)
{
    char *firstcolon;
    char *lastslash;
    char *lastdot;
    char *firstzero;
    int min;

    if (path == NULL || *path == 0)
    {
        if (drive != NULL) *drive = 0;
        if (dir != NULL) *dir = 0;
        if (fname != NULL) *fname = 0;
        if (ext != NULL) *ext = 0;
    }
    else
    {
        lastslash = NULL;
        lastdot = NULL;
        firstcolon = (char *) path;
        firstzero = (char *) path;

        while (*firstzero != 0 && firstzero - path <= WATCOM_MAX_PATH)
        {
            if (*firstzero == ':' && firstcolon == path) firstcolon = firstzero + 1;
            if (*firstzero == '\\') lastslash = firstzero + 1;
            if (*firstzero == '.') lastdot = firstzero;

            firstzero++;
        }

        if (lastslash == NULL)
        {
            lastslash = firstcolon;
            if (lastdot == NULL)
            {
                lastdot = firstzero;
            }
        }
        else
        {
            if (lastdot == NULL)
            {
                lastdot = firstzero;
            }
            else
            {
                if (lastdot < lastslash)
                {
                    lastdot = firstzero;
                }
            }
        }

        if (drive != NULL)
        {
            if (firstcolon == path)
            {
                *drive = 0;
            }
            else
            {
                min = (firstcolon - path < WATCOM_MAX_DRIVE - 1)?(int)(firstcolon - path):(WATCOM_MAX_DRIVE - 1);

                strncpy(drive, path, min);
                drive[min] = 0;
            }
        }

        if (dir != NULL)
        {
            if (lastslash == firstcolon)
            {
                *dir = 0;
            }
            else
            {
                min = (lastslash - firstcolon < WATCOM_MAX_DIR - 1)?(int)(lastslash - firstcolon):(WATCOM_MAX_DIR - 1);

                strncpy(dir, firstcolon, min);
                dir[min] = 0;
            }
        }

        if (fname != NULL)
        {
            if (lastdot == lastslash)
            {
                *fname = 0;
            }
            else
            {
                min = (lastdot - lastslash < WATCOM_MAX_FNAME - 1)?(int)(lastdot - lastslash):(WATCOM_MAX_FNAME - 1);

                strncpy(fname, lastslash, min);
                fname[min] = 0;
            }
        }


        if (ext != NULL)
        {
            if (firstzero == lastdot)
            {
                *ext = 0;
            }
            else
            {
                min = (firstzero - lastdot < WATCOM_MAX_EXT - 1)?(int)(firstzero - lastdot):(WATCOM_MAX_EXT - 1);

                strncpy(ext, lastdot, min);
                ext[min] = 0;
            }
        }

    }
}

