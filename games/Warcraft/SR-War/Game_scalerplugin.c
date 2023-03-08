/**
 *
 *  Copyright (C) 2021-2023 Roman Pauer
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


#define MAX_EXTRA_THREADS 7


typedef struct {
    SDL_sem *startcmd, *finishcmd;
    SDL_Thread *thread;
    volatile int cmd, factor, src_width, src_height, y_first, y_last;
    volatile const void *src;
    volatile void *dst;
} thread_data_t;


#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    static HMODULE SP_handle;
#else
    static void *SP_handle;
#endif

static scaler_plugin_functions SP_functions;

static thread_data_t extra_threads[MAX_EXTRA_THREADS];
static int num_extra_threads;

static int plugin_initialized = 0;


static int ScalerPlugin_Thread(thread_data_t *data)
{
    while (1)
    {
        SDL_SemWait(data->startcmd);
        if (data->cmd == 0) return 0;

        SP_functions.scale(data->factor, (const void *)data->src, (void *)data->dst, data->src_width, data->src_height, data->y_first, data->y_last);

        SDL_SemPost(data->finishcmd);
    };
}

int ScalerPlugin_Startup(void)
{
    scaler_plugin_initialize SP_initialize;
    int cpu_count, index, extra_num;

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

    if (SP_initialize(&SP_functions))
    {
        free_library(SP_handle);
        return 4;
    }

    for (index = 0; index < MAX_EXTRA_THREADS; index++)
    {
        extra_threads[index].startcmd = NULL;
        extra_threads[index].finishcmd = NULL;
        extra_threads[index].thread = NULL;
    }

    if (Game_ExtraScalerThreads >= 0)
    {
        extra_num = Game_ExtraScalerThreads;
        if (extra_num > MAX_EXTRA_THREADS) extra_num = MAX_EXTRA_THREADS;
    }
    else
    {
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

        if (cpu_count < 3) extra_num = 0;
        else if (cpu_count < 6) extra_num = 1;
        else if (cpu_count < 10) extra_num = 2;
        else if (cpu_count < 14) extra_num = 3;
        else if (cpu_count < 20) extra_num = 4;
        else if (cpu_count < 24) extra_num = 5;
        else if (cpu_count < 28) extra_num = 6;
        else extra_num = 7;
    }

    for (index = 0; index < extra_num; index++)
    {
        extra_threads[index].startcmd = SDL_CreateSemaphore(0);
        if (extra_threads[index].startcmd == NULL)
        {
            extra_num = index;
            break;
        }

        extra_threads[index].finishcmd = SDL_CreateSemaphore(0);
        if (extra_threads[index].finishcmd == NULL)
        {
            SDL_DestroySemaphore(extra_threads[index].startcmd);
            extra_threads[index].startcmd = NULL;
            extra_num = index;
            break;
        }

        extra_threads[index].thread = SDL_CreateThread(
            (int (*)(void *))ScalerPlugin_Thread,
#ifdef USE_SDL2
            "scaler",
#endif
            &extra_threads[index]
        );
        if (extra_threads[index].thread == NULL)
        {
            SDL_DestroySemaphore(extra_threads[index].finishcmd);
            extra_threads[index].finishcmd = NULL;
            SDL_DestroySemaphore(extra_threads[index].finishcmd);
            extra_threads[index].startcmd = NULL;
            extra_num = index;
            break;
        }
    }

    num_extra_threads = extra_num;

    plugin_initialized = 1;

    return 0;

#undef get_proc_address
#undef free_library
}

void ScalerPlugin_Shutdown(void)
{
    int index;

    if (!plugin_initialized) return;

    for (index = 0; index < MAX_EXTRA_THREADS; index++)
    {
        if (extra_threads[index].thread != NULL)
        {
            extra_threads[index].cmd = 0;
            SDL_SemPost(extra_threads[index].startcmd);

            SDL_WaitThread(extra_threads[index].thread, NULL);
            extra_threads[index].thread = NULL;

            SDL_DestroySemaphore(extra_threads[index].finishcmd);
            extra_threads[index].finishcmd = NULL;

            SDL_DestroySemaphore(extra_threads[index].startcmd);
            extra_threads[index].startcmd = NULL;
        }
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

    if (game && num_extra_threads)
    {
        int y_diff, y_next, index;

        y_diff = (src_height + num_extra_threads) / (num_extra_threads + 1);
        y_next = 0;

        for (index = 0; index < num_extra_threads; index++)
        {
            extra_threads[index].cmd = 1;
            extra_threads[index].factor = factor;
            extra_threads[index].src = src;
            extra_threads[index].dst = dst;
            extra_threads[index].src_width = src_width;
            extra_threads[index].src_height = src_height;
            extra_threads[index].y_first = y_next;
            y_next += y_diff;
            extra_threads[index].y_last = y_next;

            SDL_SemPost(extra_threads[index].startcmd);
        }

        SP_functions.scale(factor, src, dst, src_width, src_height, y_next, src_height);

        for (index = 0; index < num_extra_threads; index++)
        {
            SDL_SemWait(extra_threads[index].finishcmd);
        }
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

