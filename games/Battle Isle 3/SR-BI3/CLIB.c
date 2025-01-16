/**
 *
 *  Copyright (C) 2019-2024 Roman Pauer
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

#include <stdlib.h>
#include <unistd.h>
#include "CLIB.h"

#include <string.h>
#include <stdio.h>

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


static int32_t ms_randseed;


int32_t memcmp_c(const void *s1, const void *s2, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("memcmp: 0x%x, 0x%x, %i\n", (uintptr_t) s1, (uintptr_t) s2, n);
#endif

    return memcmp(s1, s2, n);
}

void *memcpy_c(void *dest, const void *src, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("memcpy: 0x%x, 0x%x, %i\n", (uintptr_t) dest, (uintptr_t) src, n);
#endif

    return memcpy(dest, src, n);
}

void *memset_c(void *s, int32_t c, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("memset: 0x%x, 0x%x, %i\n", (uintptr_t) s, c, n);
#endif

    return memset(s, c, n);
}


char *strcat_c(char *dest, const char *src)
{
#ifdef DEBUG_CLIB
    eprintf("strcat: 0x%x (%s), 0x%x (%s)\n", (uintptr_t) dest, dest, (uintptr_t) src, src);
#endif

    return strcat(dest, src);
}

int32_t strcmp_c(const char *s1, const char *s2)
{
#ifdef DEBUG_CLIB
    eprintf("strcmp: 0x%x (%s), 0x%x (%s)\n", (uintptr_t) s1, s1, (uintptr_t) s2, s2);
#endif

    return strcmp(s1, s2);
}

char *strcpy_c(char *dest, const char *src)
{
#ifdef DEBUG_CLIB
    eprintf("strcpy: 0x%x (%s), 0x%x (%s)\n", (uintptr_t) dest, dest, (uintptr_t) src, src);
#endif

    return strcpy(dest, src);
}

uint32_t strlen_c(const char *s)
{
#ifdef DEBUG_CLIB
    eprintf("strlen: 0x%x (%s)\n", (uintptr_t) s, s);
#endif

    return strlen(s);
}

char *strncpy_c(char *dest, const char *src, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("strncpy: 0x%x, 0x%x (%s), %i\n", (uintptr_t) dest, (uintptr_t) src, src, n);
#endif

    return strncpy(dest, src, n);
}

int32_t _strnicmp_c(const char *s1, const char *s2, uint32_t n)
{
#ifdef DEBUG_CLIB
    eprintf("_strnicmp: 0x%x (%s), 0x%x (%s), %i\n", (uintptr_t) s1, s1, (uintptr_t) s2, s2, n);
#endif

    return strncasecmp(s1, s2, n);
}

char *strstr_c(const char *haystack, const char *needle)
{
#ifdef DEBUG_CLIB
    eprintf("strstr: 0x%x (%s), 0x%x (%s)\n", (uintptr_t) haystack, haystack, (uintptr_t) needle, needle);
#endif

    return strstr(haystack, needle);
}


void ms_srand_c(uint32_t seed)
{
#ifdef DEBUG_CLIB
    eprintf("srand (MS): 0x%x\n", seed);
#endif

    ms_randseed = seed;
}

int32_t ms_rand_c(void)
{
    int32_t newvalue;

#ifdef DEBUG_CLIB
    eprintf("rand (MS)\n");
#endif

    newvalue = (ms_randseed * 0x343FD) + 0x269EC3;
    ms_randseed = newvalue;
    return (newvalue & 0x7FFF0000) >> 16;
}

int32_t wc_rand_c(void)
{
#ifdef DEBUG_CLIB
    eprintf("rand (Watcom)\n");
#endif

    // Watcom implementation returns values in range 0-0x7fff
    return rand() & 0x7fff;
}


int32_t atoi_c(const char *nptr)
{
#ifdef DEBUG_CLIB
    eprintf("atoi: 0x%x (%s)\n", (uintptr_t) nptr, nptr);
#endif

    return atoi(nptr);
}

int32_t atol_c(const char *nptr)
{
#ifdef DEBUG_CLIB
    eprintf("atol: 0x%x (%s)\n", (uintptr_t) nptr, nptr);
#endif

    return atol(nptr);
}

static void xtoa(uint32_t value, char *buffer, int32_t radix, int negative)
{
    char ch;
    char *pos1, *pos2;
    unsigned int rem;

    if (negative)
    {
        *buffer = '-';
        buffer++;
        value = -(int32_t)value;
    }

    pos1 = buffer;
    do
    {
        rem = value % radix;
        value /= radix;

        if (rem <= 9)
        {
            *buffer++ = rem + '0';
        }
        else
        {
            *buffer++ = rem + 'a' - 10;
        }
    }
    while (value != 0);
    *buffer = 0;

    pos2 = buffer - 1;
    do
    {
        ch = *pos2;
        *pos2 = *pos1;
        *pos1 = ch;

        pos2--;
        pos1++;
    }
    while (pos1 < pos2);
}

char *_ltoa_c(int32_t value, char *buffer, int32_t radix)
{
#ifdef DEBUG_CLIB
    eprintf("_ltoa: %i, 0x%x, %i\n", value, (uintptr_t) buffer, radix);
#endif

    if ((radix == 10) && (value < 0))
    {
        xtoa(value, buffer, 10, 1);
    }
    else
    {
        xtoa(value, buffer, radix, 0);
    }

    return buffer;
}


int32_t isalnum_c(int32_t c)
{
#ifdef DEBUG_CLIB
    eprintf("isalnum: %i\n", c);
#endif

    return isalnum(c);
}


int32_t _except_handler3_c(void *exception_record, void *registration, void *context, void *dispatcher)
{
#if defined(_WIN32)
    fprintf(stderr, "exception handler 3: %i\n", (int) ((PEXCEPTION_RECORD)exception_record)->ExceptionCode);
#else
    fprintf(stderr, "exception handler 3: %i\n", *((uint32_t *)exception_record));
#endif
#if defined(__WINE__)
    TerminateProcess(GetCurrentProcess(), 0);
    return 0;
#else
    exit(0);
#endif
}


void sync_c(void)
{
#ifdef DEBUG_CLIB
    eprintf("sync\n");
#endif

#if defined(__WINE__) || !defined(_WIN32)
    sync();
#endif
}

