/**
 *
 *  Copyright (C) 2019-2020 Roman Pauer
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
#else
#include <stdio.h>
#endif

#include <SDL.h>
#include <time.h>
#include <stdlib.h>
#include "Game-Config.h"

#ifdef _WIN32
#define WINAPI_NODEF_DEFINITIONS
#endif
#include "WinApi.h"


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


extern uint32_t security_cookie_;


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

void init_security_cookie(void)
{
    srand(time(NULL));

    security_cookie_ ^= rand();
    security_cookie_ ^= rand() << 15;
    security_cookie_ ^= rand() << 30;
}

#ifdef _WIN32

void init_libquicktime(void)
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

int CALLBACK WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow
)
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE))
    {
        MessageBoxA(NULL, "Error: SDL_Init", "Error", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
        ExitProcess(1);
    }

    atexit(SDL_Quit);

    init_libquicktime();

    ReadConfiguration();

    init_security_cookie();
    return WinMain_(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

#else

void Winapi_InitTicks(void);

int main(int argc, char *argv[])
{
    char cmdline;
    if (SDL_Init(SDL_INIT_NOPARACHUTE))
    {
        eprintf("Error: SDL_Init");
        exit(1);
    }

    atexit(SDL_Quit);

    ReadConfiguration();

    init_security_cookie();

    Winapi_InitTicks();

    cmdline = 0;
    return WinMain_asm((void *)1, NULL, &cmdline, 5); // 5 = SW_SHOW
}

#endif
