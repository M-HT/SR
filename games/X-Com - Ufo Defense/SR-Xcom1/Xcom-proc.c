/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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
#include "Xcom-proc.h"
#include "Xcom-timer.h"
#include "Game_misc.h"
#include "Game_thread.h"


void Game_Set_errno_val(void)
{
    int err;

    err = errno;

    Game_Set_errno_val_num((err >= 0 && err < 256)?(errno_rtable[err]):(err));
}

void Game_Set_errno_val_num(int32_t value)
{
    switch (Game_Executable)
    {
    #if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_GEOSCAPE))
        case EXE_GEOSCAPE:
            geoscape_errno_val = value;
            break;
    #endif
    #if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_TACTICAL))
        case EXE_TACTICAL:
            tactical_errno_val = value;
            break;
    #endif
    #if ((EXE_BUILD == EXE_COMBINED) || (EXE_BUILD == EXE_INTRO))
        case EXE_INTRO:
            intro_errno_val = value;
            break;
    #endif
        default:
            break;
    }
}

int32_t Game_checkch(void)
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

int32_t Game_getch(void)
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

off_t Game_filelength2(FILE *f)
{
    off_t origpos, endpos;
    int fd;

    fd = fileno(f);
    origpos = lseek(fd, 0, SEEK_CUR);
    if (origpos == (off_t)-1) return (off_t)-1;

    endpos = lseek(fd, 0, SEEK_END);
    lseek(fd, origpos, SEEK_SET);

    return endpos;
}

void *Game_malloc(uint32_t size)
{
    uint8_t *ptr, *out;

    // round the size up to the nearest multiply of 4
    // to prevent buffer overflows with instructions reading word or dword
    size = (size + 3) & ~3UL;

    // X-Com has a few buffer underflows
    // That's why I allocate 4kB more memory before the resulting pointer and 4kB after (for overflows)
    ptr = (uint8_t *) malloc(size + 8192);
    if (ptr == NULL) return NULL;

    out = ptr + 4096;

    if (!Game_list_insert(&Game_MallocList, (uintptr_t)out))
    {
        free(ptr);
        return NULL;
    }

    memset(ptr, 0, size + 8192);

    return out;
}

void Game_free(void *ptr)
{
    if (ptr == NULL) return;

    Game_list_remove(&Game_MallocList, (uintptr_t)ptr);
    free(((uint8_t *) ptr) - 4096);
}

void *Game_AllocateMemory(uint32_t size)
{
    void *mem;

    mem = malloc(size);
    if (mem == NULL) return NULL;

    if (!Game_list_insert(&Game_AllocateMemoryList, (uintptr_t)mem))
    {
        free(mem);
        return NULL;
    }

    return mem;
}

void Game_FreeMemory(void *mem)
{
    if (mem == NULL) return;

    Game_list_remove(&Game_AllocateMemoryList, (uintptr_t)mem);
    free(mem);
}

int32_t Game_dlseek(int32_t fd, int32_t offset, int32_t whence)
{
    return lseek(fd, offset, ( (whence == 0)?(SEEK_SET):( (whence == 1)?(SEEK_CUR):( (whence == 2)?(SEEK_END):(whence) ) ) ));
}

int32_t Game_dread(void *buf, int32_t count, int32_t fd)
{
    ssize_t rcount;

    rcount = read(fd, buf, count);

    return (rcount < 0)?0:rcount;
}


void Game_dclose(int32_t fd)
{
    Game_list_remove(&Game_DopenList, fd);
    close(fd);
}

int32_t Game_fclose(FILE *fp)
{
    int ret;

    Game_list_remove(&Game_FopenList, (uintptr_t)fp);
    ret = fclose(fp);
    Game_Set_errno_val();

    return ret;
}

int32_t Game_fcloseall(void)
{
    Game_list_clear(&Game_FopenList, (void (*)(uintptr_t)) &fclose);
    return 0;
}

void Game_SlowDownMainLoop(void)
{
    static uint32_t lasttick = 0;

    uint32_t VSyncTick;
    int32_t ticks;

    ticks = Game_Main_Loop_VSync_Ticks;

    if (ticks <= 0) return;

    VSyncTick = Game_VSyncTick;

    if (VSyncTick - lasttick >= (uint32_t) ticks)
    {
        lasttick = VSyncTick;
    }
    else
    {
        Game_WaitVerticalRetraceTicks(ticks - (VSyncTick - lasttick));
        lasttick += ticks;
    }
}

void Game_SlowDownScrolling(void)
{
    static uint32_t lasttick = 0;

    if (Game_Skip_Scrolling_SlowDown)
    {
        Game_Skip_Scrolling_SlowDown = 0;
    }
    else
    {
        if (Game_VSyncTick == lasttick)
        {
            Game_WaitVerticalRetraceTicks(1);
        }
        lasttick = Game_VSyncTick;
    }
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

void Game_WaitVerticalRetraceTicks(const int32_t ticks)
{
    uint32_t VSyncTick;

    VSyncTick = Game_VSyncTick;

#if defined(__DEBUG__)
    fprintf(stderr, "Waiting retrace ticks: %i\n", ticks);
#endif

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

void Game_FlipScreen(void)
{
    SDL_Event event;

    event.type = SDL_USEREVENT;
    event.user.code = EC_DISPLAY_FLIP_START;
    event.user.data1 = NULL;
    event.user.data2 = NULL;

    SDL_PushEvent(&event);
}

