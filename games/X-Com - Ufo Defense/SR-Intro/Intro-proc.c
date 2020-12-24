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
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
#else
    #include <SDL/SDL.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Intro-proc.h"
#include "Intro-timer.h"
#include "Game_thread.h"

int Game_errno(void)
{
    int err;

    err = errno;

    return (err >= 0 && err < 256)?(errno_rtable[err]):(err);
}

int Game_checkch(void)
{
    if (Game_KBufferWrite == Game_KBufferRead || SDL_GetTicks() - Game_LastKeyStroke < GAME_KEYBOARD_TYPE_RATE)
    {
        return 0; // kbd buffer empty
    }
    else
    {
        return -1; // kbd buffer not empty
    }
}

int Game_getch(void)
{
    int ret;

    while (Game_KBufferWrite == Game_KBufferRead || SDL_GetTicks() - Game_LastKeyStroke < GAME_KEYBOARD_TYPE_RATE)
    {
        Game_RunTimerDelay();
        if (Thread_Exit) Game_StopMain();
    };

    Game_LastKeyStroke = SDL_GetTicks();
    ret = Game_KBuffer[Game_KBufferRead] & 0xffff;

    Game_KBufferRead = (Game_KBufferRead + 1) & (GAME_KBUFFER_LENGTH - 1);

    return ret;
}

int Game_dlseek(int fd, int offset, int whence)
{
    return lseek(fd, offset, ( (whence == 0)?(SEEK_SET):( (whence == 1)?(SEEK_CUR):( (whence == 2)?(SEEK_END):(whence) ) ) ));
}

int Game_dread(void *buf, int count, int fd)
{
    ssize_t rcount;

    rcount = read(fd, buf, count);

    return (rcount < 0)?0:rcount;
}


void Game_dclose(int fd)
{
    close(fd);
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

