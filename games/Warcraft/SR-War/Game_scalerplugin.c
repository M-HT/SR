/**
 *
 *  Copyright (C) 2021 Roman Pauer
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

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
#else
    #include <dlfcn.h>
#endif
#include <stddef.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
#else
    #include <SDL/SDL.h>
    #include <unistd.h>
#endif
#include "Game_vars.h"
#include "Game_scalerplugin.h"
#include "scaler-plugins.h"


#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    static HMODULE SP_handle;
#else
    static void *SP_handle;
#endif

static scaler_plugin_functions SP_functions;

static SDL_sem *sem_startcmd, *sem_finishcmd;
static SDL_Thread *scaler_thread;
static volatile int scaler_cmd, scaler_factor, scaler_src_width, scaler_src_height, scaler_y_first, scaler_y_last;
static volatile const void *scaler_src;
static volatile void *scaler_dst;

static int plugin_initialized = 0;


static int ScalerPlugin_Thread(void *data)
{
    while (1)
    {
        SDL_SemWait(sem_startcmd);
        if (scaler_cmd == 0) return 0;

        SP_functions.scale(scaler_factor, (const void *)scaler_src, (void *)scaler_dst, scaler_src_width, scaler_src_height, scaler_y_first, scaler_y_last);

        SDL_SemPost(sem_finishcmd);
    };
}

int ScalerPlugin_Startup(void)
{
    scaler_plugin_initialize SP_initialize;
    int cpu_count;

    if (plugin_initialized) return 0;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    if (Game_AdvancedScaler == 2) SP_handle = LoadLibrary(".\\scaler-hqx.dll");
    else if (Game_AdvancedScaler == 3) SP_handle = LoadLibrary(".\\scaler-xbrz.dll");
    else return 1;

    #define free_library FreeLibrary
    #define get_proc_address GetProcAddress
#else
    if (Game_AdvancedScaler == 2) SP_handle = dlopen("./scaler-hqx.so", RTLD_LAZY);
    else if (Game_AdvancedScaler == 3) SP_handle = dlopen("./scaler-xbrz.so", RTLD_LAZY);
    else return 1;

    #define free_library dlclose
    #define get_proc_address dlsym
#endif
    if (SP_handle == NULL) return 2;

    SP_initialize = (scaler_plugin_initialize) get_proc_address(SP_handle, SCALER_PLUGIN_INITIALIZE);

    if (SP_initialize == NULL)
    {
        free_library(SP_handle);
        return 3;
    }

#ifdef USE_SDL2
    cpu_count = SDL_GetCPUCount();
#elif (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    cpu_count = info.dwNumberOfProcessors;
#elif defined(_SC_NPROCESSORS_ONLN)
    cpu_count = (int)sysconf(_SC_NPROCESSORS_ONLN);
#else
    cpu_count = 1;
#endif

    if (SP_initialize(&SP_functions))
    {
        free_library(SP_handle);
        return 4;
    }

    sem_startcmd = NULL;
    sem_finishcmd = NULL;
    scaler_thread = NULL;
    if (cpu_count >= 4)
    {
        sem_startcmd = SDL_CreateSemaphore(0);
        if (sem_startcmd != NULL)
        {
            sem_finishcmd = SDL_CreateSemaphore(0);
            if (sem_finishcmd != NULL)
            {
                scaler_thread = SDL_CreateThread(
                    ScalerPlugin_Thread,
#ifdef USE_SDL2
                    "scaler",
#endif
                    NULL
                );
                if (scaler_thread == NULL)
                {
                    SDL_DestroySemaphore(sem_finishcmd);
                    sem_finishcmd = NULL;
                    SDL_DestroySemaphore(sem_finishcmd);
                    sem_startcmd = NULL;
                }
            }
            else
            {
                SDL_DestroySemaphore(sem_startcmd);
                sem_startcmd = NULL;
            }
        }
    }

    plugin_initialized = 1;

    return 0;

#undef get_proc_address
#undef free_library
}

void ScalerPlugin_Shutdown(void)
{
    if (!plugin_initialized) return;

    if (scaler_thread != NULL)
    {
        scaler_cmd = 0;
        SDL_SemPost(sem_startcmd);

        SDL_WaitThread(scaler_thread, NULL);
        scaler_thread = NULL;

        SDL_DestroySemaphore(sem_finishcmd);
        sem_finishcmd = NULL;

        SDL_DestroySemaphore(sem_finishcmd);
        sem_startcmd = NULL;
    }

    SP_functions.shutdown_plugin();
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    FreeLibrary(SP_handle);
#else
    dlclose(SP_handle);
#endif
    SP_handle = NULL;

    plugin_initialized = 0;
}

void ScalerPlugin_scale(int factor, const void *src, void *dst, int src_width, int src_height, int game)
{
    if (!plugin_initialized) return;

    if (game && (scaler_thread != NULL))
    {
        int y_middle;

        scaler_cmd = 1;
        scaler_factor = factor;
        scaler_src = src;
        scaler_dst = dst;
        scaler_src_width = src_width;
        scaler_src_height = src_height;
        y_middle = src_height / 2;
        scaler_y_first = y_middle;
        scaler_y_last = src_height;

        SDL_SemPost(sem_startcmd);

        SP_functions.scale(factor, src, dst, src_width, src_height, 0, y_middle);

        SDL_SemWait(sem_finishcmd);
    }
    else
    {
        SP_functions.scale(factor, src, dst, src_width, src_height, 0, src_height);
    }
}

int ScalerPlugin_get_maximum_scale_factor(void)
{
    if (!plugin_initialized) return 0;

    return SP_functions.get_maximum_scale_factor();
}

