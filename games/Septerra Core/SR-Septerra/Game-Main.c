/**
 *
 *  Copyright (C) 2019-2023 Roman Pauer
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

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unistd.h>
#endif

#include <SDL.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "Game-Config.h"

#if (SDL_MAJOR_VERSION == 1) && SDL_VERSION_ATLEAST(1, 2, 50)
#warning Compilation using sdl12-compat detected.
#warning The compiled program might not work properly.
#warning Compilation using SDL2 is recommended.
#endif

#ifdef _WIN32
#define WINAPI_NODEF_DEFINITIONS
#endif
#include "WinApi.h"
#include "ptr32.h"


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


extern uint32_t security_cookie_;
extern uint32_t bShowEnemyStatus;
extern uint32_t bShowEnemyLOS;
extern uint32_t bHideText;
extern uint32_t bShowFPS;

#ifdef _WIN32
extern void init_sleepmode(void);
#endif


static char command_line[16];


#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
extern int CALLBACK WinMain_(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow
);
#else
extern int WinMain_asm(
  void *hInstance,
  void *hPrevInstance,
  char *lpCmdLine,
  int   nCmdShow
);
#endif
#ifdef __cplusplus
}
#endif

static void apply_cheats(void)
{
    bShowEnemyStatus = Cheat_ENEMIES;
    bShowEnemyLOS = Cheat_SIGHT;
    bHideText = Cheat_HIDETEXT;
    bShowFPS = Cheat_FPS;
}

static void prepare_command_line(void)
{
    command_line[0] = 0;

    if (Option_MovieResolution == 0)
    {
        strcat(command_line, " /d");
    }

    if (!Option_MoviesPlay)
    {
        strcat(command_line, " /m");
    }

    if (!Option_PointSoundsPlay)
    {
        strcat(command_line, " /p");
    }

    if (!Option_SoundsPlay)
    {
        strcat(command_line, " /q");
    }

    if (Option_DefaultMovement != 0)
    {
        strcat(command_line, " /r");
    }
}

static void init_security_cookie(void)
{
    srand(time(NULL));

    security_cookie_ ^= rand();
    security_cookie_ ^= rand() << 15;
    security_cookie_ ^= rand() << 30;
}

#ifdef _WIN32

static void init_libquicktime(void)
{
    if (NULL == getenv("LIBQUICKTIME_PLUGIN_DIR"))
    {
        char envstr[MAX_PATH+24];
        strcpy(envstr, "LIBQUICKTIME_PLUGIN_DIR");
        envstr[23] = '=';
        envstr[24] = 0;

        if (NULL != getcwd(&(envstr[24]), MAX_PATH))
        {
            putenv(envstr);
        }
    }
}

#else

void Winapi_InitTicks(void);

#endif


int main(int argc, char *argv[])
{
    if (sizeof(PTR32(void)) != 4)
    {
        fprintf(stderr, "Error: The program wasn't compiled correctly for %i-bits\n", (int) (8 * sizeof(void*)));
        return 0;
    }
    else if (sizeof(void*) != 4)
    {
        if ((uintptr_t)argv > UINT32_MAX)
        {
            fprintf(stderr, "Error: The program must be run with the loader for %i-bits\n", (int) (8 * sizeof(void*)));
            return 0;
        }
    }

    if (SDL_Init(SDL_INIT_NOPARACHUTE))
    {
#ifdef _WIN32
        MessageBoxA(NULL, "Error: SDL_Init", "Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
#else
        eprintf("Error: SDL_Init");
#endif
        exit(1);
    }

    atexit(SDL_Quit);

#if (SDL_MAJOR_VERSION == 1)
    const SDL_version *link_version = SDL_Linked_Version();
    if (SDL_VERSIONNUM(link_version->major, link_version->minor, link_version->patch) >= SDL_VERSIONNUM(1,2,50))
    {
        fprintf(stderr, "Warning: sdl12-compat detected.\nWarning: The program might not work properly.\nWarning: Using SDL2 version is recommended.\n");
    }
#endif

#ifdef _WIN32
    init_libquicktime();
#endif

    ReadConfiguration();

#ifdef _WIN32
    init_sleepmode();
#endif

    apply_cheats();

    prepare_command_line();

    init_security_cookie();

#ifdef _WIN32
    return WinMain_((void *)1, NULL, command_line, 5); // 5 = SW_SHOW
#else
    Winapi_InitTicks();

    return WinMain_asm((void *)1, NULL, command_line, 5); // 5 = SW_SHOW
#endif
}

