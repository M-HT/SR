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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>		/* needed for procedure Game_openFlags */
#include <ctype.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-proc.h"
#include "Albion-timer.h"
#include "Game_thread.h"

int Game_errno(void)
{
    int err;

    err = errno;

    return (err >= 0 && err < 256)?(errno_rtable[err]):(err);
}

off_t Game_filelength(int fd)
{
    off_t origpos, endpos;

    origpos = lseek(fd, 0, SEEK_CUR);
    if (origpos == (off_t)-1) return (off_t)-1;

    endpos = lseek(fd, 0, SEEK_END);
    lseek(fd, origpos, SEEK_SET);

    return endpos;
}

uint64_t Game_dos_getvect(const int32_t intnum)
{
    return (uint64_t) ((uint32_t) Game_InterruptTable[intnum & 0xff]);
}

#define WATCOM_BUFSIZ 0x1000

void Game_setbuf(FILE *fp, char *buf)
{
    setvbuf(fp, buf, buf ? _IOFBF : _IONBF, WATCOM_BUFSIZ);
}

void Game_dos_setvect(const int32_t intnum, const uint32_t handler_low, const uint32_t handler_high)
{
#if defined(__DEBUG__)
    fprintf(stderr, "Setting interrupt vector: %i\n", intnum & 0xff);
#endif

    Game_InterruptTable[intnum & 0xff] = (void *) handler_low;
}

off_t Game_tell(int handle)
{
    return lseek(handle, 0, SEEK_CUR);
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

void Game_WaitTimerTicks(const int32_t ticks)
{
    Game_WaitVerticalRetraceTicks(ticks);
}

void Game_WaitVerticalRetraceTicks(const int32_t ticks)
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

void Game_WaitAfterVerticalRetrace(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "WaitAfterVerticalRetrace\n");
#endif

    Game_WaitVerticalRetraceTicks(0);
}

void Game_WaitForVerticalRetrace(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "WaitForVerticalRetrace\n");
#endif

    Game_WaitVerticalRetraceTicks(1);
}

void Game_WaitAfter2ndVerticalRetrace(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "WaitAfter2ndVerticalRetrace\n");
#endif

    Game_WaitVerticalRetraceTicks(1);
}

void Game_WaitFor2ndVerticalRetrace(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "WaitFor2ndVerticalRetrace\n");
#endif

    Game_WaitVerticalRetraceTicks(2);
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

#define WATCOM_MAX_PATH   144 /* maximum length of full pathname */
#define WATCOM_MAX_DRIVE   3  /* maximum length of drive component */
#define WATCOM_MAX_DIR    130 /* maximum length of path component */
#define WATCOM_MAX_FNAME   9  /* maximum length of file name component */
#define WATCOM_MAX_EXT     5  /* maximum length of extension component */

void Game_splitpath(const char *path, char *drive, char *dir, char *fname, char *ext)
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
                min = (firstcolon - path < WATCOM_MAX_DRIVE - 1)?(firstcolon - path):(WATCOM_MAX_DRIVE - 1);

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
                min = (lastslash - firstcolon < WATCOM_MAX_DIR - 1)?(lastslash - firstcolon):(WATCOM_MAX_DIR - 1);

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
                min = (lastdot - lastslash < WATCOM_MAX_FNAME - 1)?(lastdot - lastslash):(WATCOM_MAX_FNAME - 1);

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
                min = (firstzero - lastdot < WATCOM_MAX_EXT - 1)?(firstzero - lastdot):(WATCOM_MAX_EXT - 1);

                strncpy(ext, lastdot, min);
                ext[min] = 0;
            }
        }

    }
}

