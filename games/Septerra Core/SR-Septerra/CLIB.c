/**
 *
 *  Copyright (C) 2019-2026 Roman Pauer
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
#ifdef DEBUG_CLIB
#include <inttypes.h>
#endif
#include <stdlib.h>
#include "CLIB.h"
#include "Game-Memory.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include "printf_x86.h"
#include "ptr32.h"

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
#include <process.h>
#else
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#endif


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif
extern void CCALL run_thread_asm(void *arglist, void(*start_address)(void *));
#ifdef __cplusplus
}
#endif


void * CCALL memset_c(void *s, int32_t c, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("memset: 0x%" PRIxPTR ", 0x%x, %i\n", (uintptr_t) s, c, n);
#endif

    return memset(s, c, n);
}

void * CCALL memcpy_c(void *dest, const void *src, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("memcpy: 0x%" PRIxPTR ", 0x%" PRIxPTR ", %i\n", (uintptr_t) dest, (uintptr_t) src, n);
#endif

    // either IDA misidentified memmove as memcpy
    // or Septerra Core uses memcpy on overlapping regions
    //return memcpy(dest, src, n);
    return memmove(dest, src, n);
}


int32_t CCALL _stricmp_c(const char *s1, const char *s2)
{
#ifdef DEBUG_CLIB
    eprintf("_stricmp: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s) - %i\n", (uintptr_t) s1, s1, (uintptr_t) s2, s2, strcasecmp(s1, s2));
#endif

    return strcasecmp(s1, s2);
}

char * CCALL strncpy_c(char *dest, const char *src, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("strncpy: 0x%" PRIxPTR ", 0x%" PRIxPTR " (%s), %i\n", (uintptr_t) dest, (uintptr_t) src, src, n);
#endif

    return strncpy(dest, src, n);
}

int32_t CCALL strncmp_c(const char *s1, const char *s2, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("strncmp: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s), %i - %i\n", (uintptr_t) s1, s1, (uintptr_t) s2, s2, n, strncmp(s1, s2, n));
#endif

    return strncmp(s1, s2, n);
}

char * CCALL strncat_c(char *dest, const char *src, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("strncat: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s), %i\n", (uintptr_t) dest, dest, (uintptr_t) src, src, n);
#endif

    return strncat(dest, src, n);
}

void * CCALL malloc_c(uint32_t size)
{
#ifdef DEBUG_CLIB
    eprintf("malloc: %i\n", size);
#endif

    return x86_malloc(size);
}

void CCALL free_c(void *ptr)
{
#ifdef DEBUG_CLIB
    eprintf("free: 0x%" PRIxPTR "\n", (uintptr_t) ptr);
#endif

    x86_free(ptr);
}

void * CCALL calloc_c(uint32_t nmemb, uint32_t size)
{
#ifdef DEBUG_CLIB
    eprintf("calloc: %i, %i\n", nmemb, size);
#endif

    return x86_calloc(nmemb, size);
}


int32_t CCALL atol_c(const char *nptr)
{
#ifdef DEBUG_CLIB
    eprintf("atol: 0x%" PRIxPTR " (%s) - %i\n", (uintptr_t) nptr, nptr, (int)atol(nptr));
#endif

    return atol(nptr);
}

int32_t CCALL toupper_c(int32_t c)
{
#ifdef DEBUG_CLIB
    eprintf("toupper: %i (%c) - %i\n", c, c, toupper(c));
#endif

    return toupper(c);
}


int32_t CCALL sprintf2_c(char *str, const char *format, uint32_t *ap)
{
    int res;

#ifdef DEBUG_CLIB
    eprintf("sprintf: 0x%" PRIxPTR ", 0x%" PRIxPTR " (%s) - ", (uintptr_t) str, (uintptr_t) format, format);
#endif

    res = vsprintf_x86(str, format, ap);

#ifdef DEBUG_CLIB
    eprintf("%i (%s)\n", res, str);
#endif

    return res;
}

int32_t CCALL sscanf2_c(const char *str, const char *format, uint32_t *ap)
{
#define MAX_VALUES 2
    int res, num, index;
    uintptr_t values[MAX_VALUES];
    void *ptrvals[MAX_VALUES];

#ifdef DEBUG_CLIB
    eprintf("sscanf: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s) - ", (uintptr_t) str, str, (uintptr_t) format, format);
#endif

    num = 0;
    if (format != NULL)
    {
        for (index = 0; format[index] != 0; index++)
        {
            if (format[index] == '%')
            {
                num++;
                if (num > MAX_VALUES) break;

                if (format[index + 1] == 's')
                {
                    ptrvals[num - 1] = (void *)((PTR32(void) *)ap)[num - 1];
                }
                else if (format[index + 1] == 'd')
                {
                    ptrvals[num - 1] = &(values[num - 1]);
                }
                else
                {
                    eprintf("sscanf: unsupported format: %s\n", format);
                    exit(1);
                }
            }
        }
    }

    switch (num)
    {
        case 1:
            res = sscanf(str, format, ptrvals[0]);
            break;
        case 2:
            res = sscanf(str, format, ptrvals[0], ptrvals[1]);
            break;
        default:
            eprintf("sscanf: unsupported format: %s\n", format);
            exit(1);
    }

    for (index = 0; index < res; index++)
    {
        if (ptrvals[index] == &(values[index]))
        {
            *((uint32_t *)((PTR32(uint32_t) *)ap)[index]) = (uint32_t)values[index];
        }
    }

#ifdef DEBUG_CLIB
    eprintf("%i\n", res);
#endif

    return res;
#undef MAX_VALUES
}


int32_t CCALL system_c(const char *command)
{
#ifdef DEBUG_CLIB
    eprintf("system: 0x%" PRIxPTR " (%s)\n", (uintptr_t) command, command);
#endif

    return 0;
}

void CCALL srand_c(uint32_t seed)
{
#ifdef DEBUG_CLIB
    eprintf("srand: 0x%x\n", seed);
#endif

    srand(seed);
}

int32_t CCALL rand_c(void)
{
#ifdef DEBUG_CLIB
    eprintf("rand\n");
#endif

    // Microsoft implementation returns values in range 0-0x7fff
    return rand() & 0x7fff;
}


void CCALL __report_gsfailure_c(void)
{
    fprintf(stderr, "security cookie check failed\n");
    exit(0x409);
}

int32_t CCALL _except_handler4_c(int32_t _1, void *TargetFrame, int32_t _3)
{
    fprintf(stderr, "exception handler: %i\n", 4);
    exit(0);
}

typedef struct {
    void(*start_address)(void *);
    void *arglist;
} run_thread_args;

#ifdef _WIN32
static void run_thread(void *arg)
{
    void(*start_address)(void *) = ((run_thread_args *)arg)->start_address;
    void *arglist = ((run_thread_args *)arg)->arglist;
    free(arg);

    run_thread_asm(arglist, start_address);
}
#else
static void *run_thread(void *arg)
{
    void(*start_address)(void *) = ((run_thread_args *)arg)->start_address;
    void *arglist = ((run_thread_args *)arg)->arglist;
    free(arg);

    run_thread_asm(arglist, start_address);
    return NULL;
}
#endif

uint32_t CCALL _beginthread_c(void(*start_address)(void *), uint32_t stack_size, void *arglist)
{
    run_thread_args *thread_args;

#ifdef DEBUG_CLIB
    eprintf("_beginthread: 0x%" PRIxPTR ", %i, 0x%" PRIxPTR "\n", (uintptr_t) start_address, stack_size, (uintptr_t) arglist);
#endif

    thread_args = (run_thread_args *) malloc(sizeof(run_thread_args));
    if (thread_args == NULL)
    {
        return 0;
    }

    thread_args->start_address = start_address;
    thread_args->arglist = arglist;

#ifdef _WIN32
    if ((intptr_t)-1 == (intptr_t)_beginthread(run_thread, stack_size, thread_args))
    {
        free(thread_args);
        return 0;
    }

    // function should return handle to thread, but Septerra Core only checks whether return value is 0
    return 1;
#else
    pthread_attr_t attr;
    pthread_t thread;

    if (0 != pthread_attr_init(&attr))
    {
        free(thread_args);
        return 0;
    }

    if (0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
    {
        pthread_attr_destroy(&attr);
        free(thread_args);
        return 0;
    }

    if (stack_size != 0)
    {
        if (0 != pthread_attr_setstacksize(&attr, stack_size))
        {
            pthread_attr_destroy(&attr);
            free(thread_args);
            return 0;
        }
    }

    if (0 != pthread_create(&thread, &attr, &run_thread, thread_args))
    {
        pthread_attr_destroy(&attr);
        free(thread_args);
        return 0;
    }

    pthread_attr_destroy(&attr);
    // function should return handle to thread, but Septerra Core only checks whether return value is 0
    return 1;
#endif
}

void CCALL sync_c(void)
{
#ifdef DEBUG_CLIB
    eprintf("sync\n");
#endif

#if !defined(_WIN32)
    sync();
#endif
}


#if !defined(_WIN32)
int CLIB_FindFile(const char *src, char *dst)
{
    const char *filename_start;
    DIR *dir;
    struct dirent *entry;
    const char *slash, *backslash;
    char *replace, *dirstart;
    int dirlen, found;

    filename_start = src;
    if (src[0] == '.')
    {
        if (src[1] == 0)
        {
            dst[0] = '.';
            dst[1] = 0;
            return 1;
        }

        if ((src[1] == '\\') || (src[1] == '/'))
        {
            if (src[2] == 0)
            {
                dst[0] = '.';
                dst[1] = 0;
                return 1;
            }

            filename_start += 2;
        }
    }

    backslash = strchr(filename_start, '\\');
    slash = strchr(filename_start, '/');

    if ((backslash != NULL) || (slash != NULL))
    {
        if (slash == NULL) slash = backslash;
        else if ((backslash != NULL) && (backslash < slash)) slash = backslash;
        dirlen = slash - filename_start;
        memcpy(dst, filename_start, dirlen);
        dst[dirlen] = 0;

        if (0 != access(dst, F_OK))
        {
            found = 0;

            dir = opendir("./");

            if (dir != NULL)
            while (1)
            {
                entry = readdir(dir);
                if (entry == NULL) break;

                if (0 == strcasecmp(dst, entry->d_name))
                {
                    strcpy(dst, entry->d_name);
                    found = 1;
                    break;
                }
            }

            if (dir != NULL) closedir(dir);

            if (!found)
            {
                strcpy(dst, filename_start);
                for (replace = dst; *replace != 0; replace++)
                {
                    if (*replace == '\\') *replace = '/';
                }

                return 0;
            }
        }

        dst[dirlen] = '/';
        dst[dirlen + 1] = 0;
        dirstart = dst;
        dst += dirlen + 1;
        filename_start += dirlen + 1;

        if ((strchr(filename_start, '\\') != NULL) || (strchr(filename_start, '/') != NULL))
        {
            // Septerra Core doesn't use paths more than 1 deep
            strcpy(dst, filename_start);
            for (replace = dst; *replace != 0; replace++)
            {
                if (*replace == '\\') *replace = '/';
            }

            if (0 == access(dirstart, F_OK))
            {
                return 1;
            }

            eprintf("CLIB_FindFile: unimplemented path finding: %s\n", src);
            exit(1);
        }
        else
        {
            strcpy(dst, filename_start);
            if (0 == access(dirstart, F_OK))
            {
                return 1;
            }
            else
            {
                *dst = 0;
            }
        }
    }
    else
    {
        if (0 == access(filename_start, F_OK))
        {
            strcpy(dst, filename_start);
            return 1;
        }

        dst[0] = '.';
        dst[1] = '/';
        dst[2] = 0;
        dirstart = dst;
    }

    dir = opendir(dirstart);

    if (dir != NULL)
    while (1)
    {
        entry = readdir(dir);
        if (entry == NULL) break;

        if (0 == strcasecmp(filename_start, entry->d_name))
        {
            strcpy(dst, entry->d_name);
            closedir(dir);
            return 1;
        }
    }

    if (dir != NULL) closedir(dir);

    strcpy(dst, filename_start);
    return 0;
}
#endif

