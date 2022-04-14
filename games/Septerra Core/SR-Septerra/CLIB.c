/**
 *
 *  Copyright (C) 2019-2022 Roman Pauer
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
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include "CLIB.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <dirent.h>

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#if !defined(_WIN32)
#include <pthread.h>
#endif


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif
extern void run_thread_asm(void *arglist, void(*start_address)(void *));
#ifdef __cplusplus
}
#endif


void *memset_c(void *s, int32_t c, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("memset: 0x%" PRIxPTR ", 0x%x, %i\n", (uintptr_t) s, c, n);
#endif

    return memset(s, c, n);
}

void *memcpy_c(void *dest, const void *src, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("memcpy: 0x%" PRIxPTR ", 0x%" PRIxPTR ", %i\n", (uintptr_t) dest, (uintptr_t) src, n);
#endif

    // either IDA misidentified memmove as memcpy
    // or Septerra Core uses memcpy on overlapping regions
    //return memcpy(dest, src, n);
    return memmove(dest, src, n);
}


int32_t _stricmp_c(const char *s1, const char *s2)
{
#ifdef DEBUG_CLIB
    eprintf("_stricmp: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s) - %i\n", (uintptr_t) s1, s1, (uintptr_t) s2, s2, strcasecmp(s1, s2));
#endif

    return strcasecmp(s1, s2);
}

char *strncpy_c(char *dest, const char *src, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("strncpy: 0x%" PRIxPTR ", 0x%" PRIxPTR " (%s), %i\n", (uintptr_t) dest, (uintptr_t) src, src, n);
#endif

    return strncpy(dest, src, n);
}

int32_t strncmp_c(const char *s1, const char *s2, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("strncmp: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s), %i - %i\n", (uintptr_t) s1, s1, (uintptr_t) s2, s2, n, strncmp(s1, s2, n));
#endif

    return strncmp(s1, s2, n);
}

char *strncat_c(char *dest, const char *src, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("strncat: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s), %i\n", (uintptr_t) dest, dest, (uintptr_t) src, src, n);
#endif

    return strncat(dest, src, n);
}

int32_t _strnicmp_c(const char *s1, const char *s2, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("_strnicmp: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s), %i - %i\n", (uintptr_t) s1, s1, (uintptr_t) s2, s2, n, strncasecmp(s1, s2, n));
#endif

    return strncasecmp(s1, s2, n);
}


void *malloc_c(uint32_t size)
{
#ifdef DEBUG_CLIB
    eprintf("malloc: %i\n", size);
#endif

    return malloc(size);
}

void free_c(void *ptr)
{
#ifdef DEBUG_CLIB
    eprintf("free: 0x%" PRIxPTR "\n", (uintptr_t) ptr);
#endif

    free(ptr);
}

void *calloc_c(uint32_t nmemb, uint32_t size)
{
#ifdef DEBUG_CLIB
    eprintf("calloc: %i, %i\n", nmemb, size);
#endif

    return calloc(nmemb, size);
}


int32_t atol_c(const char *nptr)
{
#ifdef DEBUG_CLIB
    eprintf("atol: 0x%" PRIxPTR " (%s) - %i\n", (uintptr_t) nptr, nptr, (int)atol(nptr));
#endif

    return atol(nptr);
}

int32_t toupper_c(int32_t c)
{
#ifdef DEBUG_CLIB
    eprintf("toupper: %i (%c) - %i\n", c, c, toupper(c));
#endif

    return toupper(c);
}


int32_t sscanf2_c(const char *str, const char *format, va_list ap)
{
    int res;

#ifdef DEBUG_CLIB
    eprintf("sscanf: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s) - ", (uintptr_t) str, str, (uintptr_t) format, format);
#endif

    res = vsscanf(str, format, ap);

#ifdef DEBUG_CLIB
    eprintf("%i\n", res);
#endif

    return res;
}


uint32_t fread_c(void *ptr, uint32_t size, uint32_t nmemb, void *stream)
{
#ifdef DEBUG_CLIB
    eprintf("fread: 0x%" PRIxPTR ", %i, %i, 0x%" PRIxPTR "\n", (uintptr_t) ptr, size, nmemb, (uintptr_t) stream);
#endif

    return fread(ptr, size, nmemb, (FILE *) stream);
}

int32_t ftell_c(void *stream)
{
#ifdef DEBUG_CLIB
    eprintf("ftell: 0x%" PRIxPTR " - %i\n", (uintptr_t) stream, (int)ftell((FILE *) stream));
#endif

    return ftell((FILE *) stream);
}

int32_t fseek_c(void *stream, int32_t offset, int32_t whence)
{
#ifdef DEBUG_CLIB
    eprintf("fseek: 0x%" PRIxPTR ", %i, %i\n", (uintptr_t) stream, offset, whence);
#endif

    return fseek((FILE *) stream, offset, whence);
}

void *fopen_c(const char *path, const char *mode)
{
#ifdef DEBUG_CLIB
    eprintf("fopen: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s)\n", (uintptr_t) path, path, (uintptr_t) mode, mode);
#endif

#ifdef _WIN32
    return fopen(path, mode);
#else
    char buf[8192];

    CLIB_FindFile(path, buf);
    return fopen(buf, mode);
#endif
}

int32_t fclose_c(void *fp)
{
#ifdef DEBUG_CLIB
    eprintf("fclose: 0x%" PRIxPTR "\n", (uintptr_t) fp);
#endif

    return fclose((FILE *) fp);
}


int32_t system_c(const char *command)
{
#ifdef DEBUG_CLIB
    eprintf("system: 0x%" PRIxPTR " (%s)\n", (uintptr_t) command, command);
#endif

    return 0;
}

void exit_c(int32_t status)
{
#ifdef DEBUG_CLIB
    eprintf("exit: %i\n", status);
#endif

    exit(status);
}

void srand_c(uint32_t seed)
{
#ifdef DEBUG_CLIB
    eprintf("srand: 0x%x\n", seed);
#endif

    srand(seed);
}

int32_t rand_c(void)
{
#ifdef DEBUG_CLIB
    eprintf("rand\n");
#endif

    // Microsoft implementation returns values in range 0-0x7fff
    return rand() & 0x7fff;
}


void __report_gsfailure_c(void)
{
    fprintf(stderr, "security cookie check failed\n");
    exit(0x409);
}

int32_t _except_handler4_c(int32_t _1, void *TargetFrame, int32_t _3)
{
    fprintf(stderr, "exception handler: %i\n", 4);
    exit(0);
}

int32_t _except_handler3_c(int32_t _1, void *TargetFrame, int32_t _3)
{
    fprintf(stderr, "exception handler: %i\n", 3);
    exit(0);
}

#if !defined(_WIN32)
typedef struct {
    void(*start_address)(void *);
    void *arglist;
} run_thread_args;

static void *run_thread(void *arg)
{
    void(*start_address)(void *) = ((run_thread_args *)arg)->start_address;
    void *arglist = ((run_thread_args *)arg)->arglist;
    free(arg);

    run_thread_asm(arglist, start_address);
    return NULL;
}
#endif

uint32_t _beginthread_c(void(*start_address)(void *), uint32_t stack_size, void *arglist)
{
#ifdef DEBUG_CLIB
    eprintf("_beginthread: 0x%" PRIxPTR ", %i, 0x%" PRIxPTR "\n", (uintptr_t) start_address, stack_size, (uintptr_t) arglist);
#endif

#ifdef _WIN32
    return _beginthread(start_address, stack_size, arglist);
#else
    pthread_attr_t attr;
    pthread_t thread;
    run_thread_args *thread_args;

    thread_args = (run_thread_args *) malloc(sizeof(run_thread_args));
    if (thread_args == NULL)
    {
        return 0;
    }

    thread_args->start_address = start_address;
    thread_args->arglist = arglist;

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

void sync_c(void)
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

