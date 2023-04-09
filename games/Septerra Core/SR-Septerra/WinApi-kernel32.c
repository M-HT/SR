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

#define _XOPEN_SOURCE 700
#define _SVID_SOURCE 1
#define _DEFAULT_SOURCE 1
#define _FILE_OFFSET_BITS 64
#include <inttypes.h>
#include "WinApi-kernel32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <errno.h>

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
#define WINAPI_NODEF_DEFINITIONS
#endif
#include "WinApi.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#define USE_OLD_GETTIME 0

#if (USE_OLD_GETTIME)
#include <sys/time.h>
#endif

#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include "CLIB.h"

#if (USE_OLD_GETTIME)
static struct timeval start_time;
#else
static struct timespec start_time;

#if defined(CLOCK_MONOTONIC_RAW)
static clockid_t monotonic_clock_id;

#define MONOTONIC_CLOCK_TYPE monotonic_clock_id
#else
#define MONOTONIC_CLOCK_TYPE CLOCK_MONOTONIC
#endif
#endif

#endif


#ifndef _WIN32
#define GENERIC_READ    0x80000000
#define GENERIC_WRITE   0x40000000

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5


#define HIGH_PRIORITY_CLASS 0x00000080

#define THREAD_PRIORITY_TIME_CRITICAL 15


#define MAX_PATH 260

#define FILE_ATTRIBUTE_READONLY             0x00000001
#define FILE_ATTRIBUTE_HIDDEN               0x00000002
#define FILE_ATTRIBUTE_SYSTEM               0x00000004
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020
#define FILE_ATTRIBUTE_DEVICE               0x00000040
#define FILE_ATTRIBUTE_NORMAL               0x00000080
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800
#define FILE_ATTRIBUTE_OFFLINE              0x00001000
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000
#define FILE_ATTRIBUTE_VALID_FLAGS          0x00007fb7
#define FILE_ATTRIBUTE_VALID_SET_FLAGS      0x000031a7

#define EPOCH_TIME INT64_C(116444736000000000)
#endif

#define eprintf(...) fprintf(stderr,__VA_ARGS__)

#define PSEUDO_HANDLE_CURRENT_PROCESS ((PTR32(void))(uint32_t)-1)
#define PSEUDO_HANDLE_CURRENT_THREAD ((PTR32(void))(uint32_t)-2)


#if !defined(_WIN32)
typedef struct {
    DIR *dirinfo;
    char *directory;
    char *filename;
} find_file_state;

typedef struct _filetime_ {
    uint32_t dwLowDateTime;
    uint32_t dwHighDateTime;
} filetime;

typedef struct _large_integer {
    uint32_t LowPart;
    int32_t HighPart;
} large_integer;

#pragma pack(2)
typedef struct __attribute__ ((__packed__)) _systemtime {
    uint16_t wYear;
    uint16_t wMonth;
    uint16_t wDayOfWeek;
    uint16_t wDay;
    uint16_t wHour;
    uint16_t wMinute;
    uint16_t wSecond;
    uint16_t wMilliseconds;
} systemtime;
#pragma pack()

typedef struct _win32_find_data_ {
    uint32_t dwFileAttributes;
    filetime ftCreationTime;
    filetime ftLastAccessTime;
    filetime ftLastWriteTime;
    uint32_t nFileSizeHigh;
    uint32_t nFileSizeLow;
    uint32_t dwReserved0;
    uint32_t dwReserved1;
    char cFileName[MAX_PATH];
    char cAlternateFileName[14];
} win32_find_data;

#endif


#if !defined(_WIN32)
void Winapi_InitTicks(void)
{
    // initialize start time

#if (USE_OLD_GETTIME)
    gettimeofday(&start_time, NULL);
#else
#if defined(CLOCK_MONOTONIC_RAW)
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &start_time))
    {
        monotonic_clock_id = CLOCK_MONOTONIC;
        clock_gettime(CLOCK_MONOTONIC, &start_time);
    }
    else
    {
        monotonic_clock_id = CLOCK_MONOTONIC_RAW;
    }
#else
    clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif
#endif
}

unsigned int Winapi_GetTicks(void)
{
#if (USE_OLD_GETTIME)
    struct timeval _tv;

    gettimeofday(&_tv, NULL);

    return ((_tv.tv_sec - start_time.tv_sec) * 1000) + ((_tv.tv_usec - start_time.tv_usec) / 1000);
#else
    struct timespec _tp;

    clock_gettime(MONOTONIC_CLOCK_TYPE, &_tp);

    return ((_tp.tv_sec - start_time.tv_sec) * 1000) + ((_tp.tv_nsec - start_time.tv_nsec) / 1000000);
#endif
}


static int file_pattern_match(const char *filename, const char *pattern)
{
    int i, asterisk;

    if ((pattern[0] == '*') && (pattern[1] == '.') && (pattern[2] == '*') && (pattern[3] == 0) )
    {
        return 1;
    }

new_segment:
    asterisk = 0;

    if (*pattern == '*')
    {
        asterisk = 1;
        do
        {
            pattern++;
        } while (*pattern == '*');
    }

test_match:

    for (i = 0; (pattern[i] != 0) && (pattern[i] != '*'); i++)
    {
        if (toupper(filename[i]) != pattern[i])
        {
            if (filename[i] == 0) return 0;
            if ( (pattern[i] == '?') && (filename[i] != '.') ) continue;
            if (!asterisk) return 0;
            filename++;
            goto test_match;
        }
    }

    if (pattern[i] == '*')
    {
        filename += i;
        pattern += i;
        goto new_segment;
    }
    if (filename[i] == 0) return 1;
    if ( (i != 0) && (pattern[i - 1] == '*') ) return 1;
    if (!asterisk) return 0;
    filename++;
    goto test_match;

    return 1;
}

/*

wildcard matching:
http://xoomer.alice.it/acantato/dev/wildcard/wildmatch.html

*/

static void Conv_find(win32_find_data *buffer, struct stat *filestat, const char *filename)
{
    int64_t dt_result;

    // file attributes
    buffer->dwFileAttributes = (S_ISDIR(filestat->st_mode))?FILE_ATTRIBUTE_DIRECTORY:FILE_ATTRIBUTE_NORMAL;

    // file's creation time and date
    dt_result = ((int64_t) filestat->st_ctime) * 10000000;
    dt_result += (filestat->st_ctim.tv_nsec / 100) & ~1; // set least significant bit to zero to indicate system time
    dt_result += EPOCH_TIME;

    buffer->ftCreationTime.dwLowDateTime = (uint32_t) dt_result;
    buffer->ftCreationTime.dwHighDateTime = (uint32_t) (dt_result >> 32);

    // file's last access time and date
    dt_result = ((int64_t) filestat->st_atime) * 10000000;
    dt_result += (filestat->st_atim.tv_nsec / 100) & ~1; // set least significant bit to zero to indicate system time
    dt_result += EPOCH_TIME;

    buffer->ftLastAccessTime.dwLowDateTime = (uint32_t) dt_result;
    buffer->ftLastAccessTime.dwHighDateTime = (uint32_t) (dt_result >> 32);

    // file's modification time and date
    dt_result = ((int64_t) filestat->st_mtime) * 10000000;
    dt_result += (filestat->st_mtim.tv_nsec / 100) & ~1; // set least significant bit to zero to indicate system time
    dt_result += EPOCH_TIME;

    buffer->ftLastWriteTime.dwLowDateTime = (uint32_t) dt_result;
    buffer->ftLastWriteTime.dwHighDateTime = (uint32_t) (dt_result >> 32);

    // file size
    buffer->nFileSizeHigh = (uint32_t) (filestat->st_size >> 32);
    buffer->nFileSizeLow = (uint32_t) filestat->st_size;


    buffer->dwReserved0 = 0;
    buffer->dwReserved1 = 0;

    // file name
    strncpy(buffer->cFileName, filename, 260);
    buffer->cFileName[259] = 0;

    buffer->cAlternateFileName[0] = 0;
}

#endif

uint32_t Beep_c(uint32_t dwFreq, uint32_t dwDuration)
{
#ifdef DEBUG_KERNEL32
    eprintf("Beep: %i, %i\n", dwFreq, dwDuration);
#endif

#ifdef _WIN32
    return Beep(dwFreq, dwDuration);
#else
    struct timespec _tp, rem;
    int ret;

    _tp.tv_sec = dwDuration / 1000;
    _tp.tv_nsec = (dwDuration % 1000) * 1000000;

    ret = nanosleep(&_tp, &rem);

    while (1)
    {
        if (ret == 0) break;
        else if (errno != EINTR) break;
        else if ((rem.tv_sec == 0) && (rem.tv_nsec < 500000)) break;

        _tp.tv_sec = rem.tv_sec;
        _tp.tv_nsec = rem.tv_nsec;

        ret = nanosleep(&_tp, &rem);
    }

    return 1;
#endif
}

uint32_t CloseHandle_c(void *hObject)
{
#ifdef DEBUG_KERNEL32
    eprintf("CloseHandle: 0x%" PRIxPTR "\n", (uintptr_t) hObject);
#endif

    if (hObject == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    // pseudo handles
    if ((hObject == PSEUDO_HANDLE_CURRENT_PROCESS) || (hObject == PSEUDO_HANDLE_CURRENT_THREAD))
    {
        return 1;
    }

    switch (((handle)hObject)->handle_type)
    {
        case HT_FILE:
#define hFile ((file_handle)hObject)

            if (hFile->f != NULL)
            {
                fclose((FILE *)hFile->f);
            }

            free(hFile);

            return 1;
#undef hFile
        case HT_PIPE:
#define hPipe ((pipe_handle)hObject)

            if (hPipe->pp != NULL)
            {
#ifdef _WIN32
                CloseHandle((HANDLE)hPipe->pp);
#else
                close((intptr_t)hPipe->pp);
#endif
            }

            free(hPipe);

            return 1;
#undef hPipe
        default:
            break;
    }

    eprintf("Unimplemented: %s\n", "CloseHandle");
    exit(1);
//    return CloseHandle((HANDLE)hObject);
}

uint32_t CreateDirectoryA_c(const char *lpPathName, void *lpSecurityAttributes)
{
#ifdef DEBUG_KERNEL32
    eprintf("CreateDirectoryA: %s, 0x%" PRIxPTR "\n", lpPathName, (uintptr_t)lpSecurityAttributes);
#endif

    if (lpPathName == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

#ifdef _WIN32
    if (0 !=
#if defined(__MINGW32__)
        mkdir(lpPathName)
#else
        mkdir(lpPathName, 0777)
#endif
    )
#else
    char buf[8192];

    if (CLIB_FindFile(lpPathName, buf))
    {
        Winapi_SetLastError(ERROR_FILE_EXISTS);
        return 0;
    }

    if (0 != mkdir(buf, 0777))
#endif
    {
        if (errno == EEXIST)
        {
            Winapi_SetLastError(ERROR_FILE_EXISTS);
        }
        else
        {
            Winapi_SetLastError(ERROR_ACCESS_DENIED);
        }
        return 0;
    }

    return 1;
}

void *CreateFileA_c(const char *lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode, void *lpSecurityAttributes, uint32_t dwCreationDistribution, uint32_t dwFlagsAndAttributes, void *hTemplateFile)
{
    int file_exists;
    char mode[4];
    int SeekToStart;
    handle ret;
#ifndef _WIN32
    char buf[8192];
#endif

#ifdef DEBUG_KERNEL32
    eprintf("CreateFileA: %s, 0x%x, %i, %i, 0x%x - ", lpFileName, dwDesiredAccess, dwShareMode, dwCreationDistribution, dwFlagsAndAttributes);
#endif

    if (lpFileName == NULL)
    {
#ifdef DEBUG_KERNEL32
        eprintf("error\n");
#endif
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return INVALID_HANDLE_VALUE;
    }

    if (dwDesiredAccess == 0 || hTemplateFile != 0)
    {
#ifdef DEBUG_KERNEL32
        eprintf("error\n");
#endif
        Winapi_SetLastError(ERROR_NOT_SUPPORTED);
        return INVALID_HANDLE_VALUE;
    }

#ifdef _WIN32
    file_exists = (INVALID_FILE_ATTRIBUTES != GetFileAttributes(lpFileName));
#else
    file_exists = CLIB_FindFile(lpFileName, buf);
#endif

    SeekToStart = 0;
    mode[0] = mode[1] = mode[2] = mode[3] = 0;

    switch (dwCreationDistribution)
    {
        case CREATE_NEW:
            if (file_exists)
            {
#ifdef DEBUG_KERNEL32
                eprintf("error\n");
#endif
                Winapi_SetLastError(ERROR_FILE_EXISTS);
                return INVALID_HANDLE_VALUE;
            }

            mode[0] = 'w';
            mode[1] = 'b';
            if (dwDesiredAccess & GENERIC_READ)
            {
                mode[2] = '+';
            }
            break;
        case CREATE_ALWAYS:
            mode[0] = 'w';
            mode[1] = 'b';
            if (dwDesiredAccess & GENERIC_READ)
            {
                mode[2] = '+';
            }
            break;
        case OPEN_EXISTING:
            if (!file_exists)
            {
#ifdef DEBUG_KERNEL32
                eprintf("error\n");
#endif
                Winapi_SetLastError(ERROR_FILE_NOT_FOUND);
                return INVALID_HANDLE_VALUE;
            }

            if (dwDesiredAccess & GENERIC_READ)
            {
                mode[0] = 'r';
                mode[1] = 'b';
                if (dwDesiredAccess & GENERIC_WRITE)
                {
                    mode[2] = '+';
                }
            }
            else
            {
                mode[0] = 'a';
                mode[1] = 'b';
                SeekToStart = 1;
            }
            break;
        case OPEN_ALWAYS:
            if (file_exists)
            {
                if (dwDesiredAccess & GENERIC_READ)
                {
                    mode[0] = 'r';
                    mode[1] = 'b';
                    if (dwDesiredAccess & GENERIC_WRITE)
                    {
                        mode[2] = '+';
                    }
                }
                else
                {
                    mode[0] = 'a';
                    mode[1] = 'b';
                    SeekToStart = 1;
                }
            }
            else
            {
                mode[0] = 'w';
                mode[1] = 'b';
                if (dwDesiredAccess & GENERIC_READ)
                {
                    mode[2] = '+';
                }
            }
            break;
        case TRUNCATE_EXISTING:
            if (dwDesiredAccess & GENERIC_WRITE)
            {
                if (!file_exists)
                {
#ifdef DEBUG_KERNEL32
                    eprintf("error\n");
#endif
                    Winapi_SetLastError(ERROR_FILE_NOT_FOUND);
                    return INVALID_HANDLE_VALUE;
                }

                mode[0] = 'w';
                mode[1] = 'b';
                if (dwDesiredAccess & GENERIC_READ)
                {
                    mode[2] = '+';
                }
            }
            break;
    }

    if (mode[0] == 0)
    {
#ifdef DEBUG_KERNEL32
        eprintf("error\n");
#endif
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return INVALID_HANDLE_VALUE;
    }

    ret = Winapi_AllocHandle();
    if (ret == NULL)
    {
#ifdef DEBUG_KERNEL32
        eprintf("error\n");
#endif
        return NULL;
    }

    ret->handle_type = HT_FILE;

#ifdef _WIN32
    ret->fh.f = fopen(lpFileName, mode);
#else
    ret->fh.f = fopen(buf, mode);
#endif
    if (ret->fh.f == NULL)
    {
        free(ret);
#ifdef DEBUG_KERNEL32
        eprintf("error\n");
#endif
        Winapi_SetLastError(ERROR_ACCESS_DENIED);
        return INVALID_HANDLE_VALUE;
    }

    if (SeekToStart)
    {
        fseek((FILE *)ret->fh.f, 0, SEEK_SET);
    }

#ifdef DEBUG_KERNEL32
    eprintf("0x%" PRIxPTR "\n", (uintptr_t)ret);
#endif
    return ret;
}

void *CreateMutexA_c(void *lpMutexAttributes, uint32_t bInitialOwner, const char *lpName)
{
    // Septerra Core doesn't use the mutex - it only checks last error for ERROR_ALREADY_EXISTS, to prevent two instances of the application to run
    Winapi_SetLastError(ERROR_ACCESS_DENIED);
    return NULL;
}

uint32_t CreatePipe_c(PTR32(void) *hReadPipe, PTR32(void) *hWritePipe, void *lpPipeAttributes, uint32_t nSize)
{
    handle hread, hwrite;
    int ret;

#ifdef DEBUG_KERNEL32
    eprintf("CreatePipe: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR ", %i - ", (uintptr_t)hReadPipe, (uintptr_t)hWritePipe, (uintptr_t)lpPipeAttributes, nSize);
#endif

    if ((hReadPipe == NULL) || (hWritePipe == NULL))
    {
#ifdef DEBUG_KERNEL32
        eprintf("error\n");
#endif
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if ((lpPipeAttributes == NULL) && (nSize == 0))
    {
        hread = Winapi_AllocHandle();
        if (hread == NULL)
        {
#ifdef DEBUG_KERNEL32
        eprintf("error\n");
#endif
            Winapi_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return 0;
        }

        hread->handle_type = HT_PIPE;

        hwrite = Winapi_AllocHandle();
        if (hwrite == NULL)
        {
            free(hread);
#ifdef DEBUG_KERNEL32
        eprintf("error\n");
#endif
            Winapi_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return 0;
        }

        hwrite->handle_type = HT_PIPE;

#ifdef _WIN32
        ret = CreatePipe((PHANDLE)&(hread->pph.pp), (PHANDLE)&(hwrite->pph.pp), (LPSECURITY_ATTRIBUTES)lpPipeAttributes, nSize);
#else
        int pipefd[2];

        if (pipe(pipefd))
        {
            ret = 0;
        }
        else
        {
            hread->pph.pp = (void *)(intptr_t)pipefd[0];
            hwrite->pph.pp = (void *)(intptr_t)pipefd[1];
            ret = 1;
        }
#endif

        if (ret == 0)
        {
            free(hwrite);
            free(hread);
#ifdef DEBUG_KERNEL32
            eprintf("error\n");
#endif
            Winapi_SetLastError(ERROR_ACCESS_DENIED);
            return 0;
        }

        *hReadPipe = hread;
        *hWritePipe = hwrite;

#ifdef DEBUG_KERNEL32
        eprintf("OK: 0x%" PRIxPTR ", 0x%" PRIxPTR "\n", (uintptr_t)hread, (uintptr_t)hwrite);
#endif

        return 1;
    }

    eprintf("Unsupported method: %s\n", "CreatePipe");
    exit(1);
}

void DeleteCriticalSection_c(void *lpCriticalSection)
{
#ifdef DEBUG_KERNEL32
    eprintf("DeleteCriticalSection: 0x%" PRIxPTR "\n", (uintptr_t)lpCriticalSection);
#endif

#ifdef _WIN32
    DeleteCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
#else
    if (*(pthread_mutex_t **)lpCriticalSection != NULL)
    {
        pthread_mutex_destroy(*(pthread_mutex_t **)lpCriticalSection);
        *(pthread_mutex_t **)lpCriticalSection = NULL;
    }
#endif
}

uint32_t DeleteFileA_c(const char *lpFileName)
{
    eprintf("Unimplemented: %s\n", "DeleteFileA");
    exit(1);
//    return DeleteFileA(lpFileName);
}

void EnterCriticalSection_c(void *lpCriticalSection)
{
#ifdef DEBUG_KERNEL32
    eprintf("EnterCriticalSection: 0x%" PRIxPTR "\n", (uintptr_t)lpCriticalSection);
#endif

#ifdef _WIN32
    EnterCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
#else
    pthread_mutex_lock(*(pthread_mutex_t **)lpCriticalSection);
#endif
}

void ExitProcess_c(uint32_t uExitCode)
{
    exit(uExitCode);
}

uint32_t FileTimeToLocalFileTime_c(const void *lpFileTime, void *lpLocalFileTime)
{
#ifdef DEBUG_KERNEL32
    eprintf("FileTimeToLocalFileTime\n");
#endif

    if (lpFileTime == NULL || lpLocalFileTime == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

#ifdef _WIN32
    return FileTimeToLocalFileTime((const FILETIME *)lpFileTime, (LPFILETIME)lpLocalFileTime);
#else
    ((filetime *)lpLocalFileTime)->dwLowDateTime = ((const filetime *)lpFileTime)->dwLowDateTime | 1;  // set least significant bit to 1 to indicate local time
    ((filetime *)lpLocalFileTime)->dwHighDateTime = ((const filetime *)lpFileTime)->dwHighDateTime;

    return 1;
#endif
}

uint32_t FileTimeToSystemTime_c(const void *lpFileTime, void *lpSystemTime)
{
#ifdef DEBUG_KERNEL32
    eprintf("FileTimeToSystemTime\n");
#endif

    if (lpFileTime == NULL || lpSystemTime == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

#ifdef _WIN32
    return FileTimeToSystemTime((const FILETIME *)lpFileTime, (LPSYSTEMTIME)lpSystemTime);
#else
    int64_t dt_time;
    struct tm timem, *ptimem;
    time_t timet;

    dt_time = ( ( ( (uint32_t) (((const filetime *)lpFileTime)->dwLowDateTime) ) | ( ( (uint64_t) ( (uint32_t) (((const filetime *)lpFileTime)->dwHighDateTime) ) ) << 32 ) ) - EPOCH_TIME );
    timet = dt_time / 10000000;

    if (dt_time & 1)
    {
        // least significant bit indicates local time
        ptimem = localtime_r(&timet, &timem);
    }
    else
    {
        // least significant bit indicates system time
        ptimem = gmtime_r(&timet, &timem);
    }

    if (ptimem == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    ((systemtime *)lpSystemTime)->wYear = timem.tm_year + 1900;
    ((systemtime *)lpSystemTime)->wMonth = timem.tm_mon + 1;
    ((systemtime *)lpSystemTime)->wDayOfWeek = timem.tm_wday;
    ((systemtime *)lpSystemTime)->wDay = timem.tm_mday;
    ((systemtime *)lpSystemTime)->wHour = timem.tm_hour;
    ((systemtime *)lpSystemTime)->wMinute = timem.tm_min;
    ((systemtime *)lpSystemTime)->wSecond = timem.tm_sec;
    ((systemtime *)lpSystemTime)->wMilliseconds = (dt_time / 10000) % 1000;

    return 1;
#endif
}

uint32_t FindClose_c(void *hFindFile)
{
#ifdef DEBUG_KERNEL32
    eprintf("FindClose: 0x%" PRIxPTR "\n", (uintptr_t) hFindFile);
#endif

    if (hFindFile == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (((handle)hFindFile)->handle_type != HT_SEARCH)
    {
        Winapi_SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }

    if (((handle)hFindFile)->sh.d != NULL)
    {
#ifdef _WIN32
        FindClose((HANDLE)((handle)hFindFile)->sh.d);
#else
        find_file_state *state;

        state = (find_file_state *) ((handle)hFindFile)->sh.d;

        if (state->dirinfo != NULL)
        {
            closedir(state->dirinfo);
            state->dirinfo = NULL;
        }
        if (state->directory != NULL)
        {
            free(state->directory);
            state->directory = NULL;
        }
        if (state->filename != NULL)
        {
            free(state->filename);
            state->filename = NULL;
        }

        free(state);
#endif
        ((handle)hFindFile)->sh.d = NULL;
    }

    free(hFindFile);

    return 1;
}

void *FindFirstFileA_c(const char *lpFileName, void *lpFindFileData)
{
    handle ret;

#ifdef DEBUG_KERNEL32
    eprintf("FindFirstFileA: %s, 0x%" PRIxPTR "\n", lpFileName, (uintptr_t) lpFindFileData);
#endif

    if ((lpFileName == NULL) || (lpFindFileData == NULL))
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return INVALID_HANDLE_VALUE;
    }

#ifdef _WIN32
    ret = Winapi_AllocHandle();
    if (ret == NULL)
    {
        return INVALID_HANDLE_VALUE;
    }

    ret->handle_type = HT_SEARCH;
    ret->sh.d = FindFirstFileA(lpFileName, (LPWIN32_FIND_DATAA)lpFindFileData);

    if (ret->sh.d == INVALID_HANDLE_VALUE)
    {
        free(ret);
        return INVALID_HANDLE_VALUE;
    }

    return ret;
#else
    int readdirtype;
    struct stat filestat;
    DIR *dirinfo;
    struct dirent *direntry;
    find_file_state *state;
    const char *pattern, *slash;
    char *replace;
    char orig_directory[MAX_PATH];
    char buf[8192];

    // check for wildcards
    if (strchr(lpFileName, '*') != NULL)
    {
        readdirtype = 1;
    }
    else if (strchr(lpFileName, '?') != NULL)
    {
        readdirtype = 1;
    }
    else
    {
        readdirtype = 0;
    }

    if (readdirtype == 0)
    {
        // no wildcards

        if (!CLIB_FindFile(lpFileName, buf))
        {
            Winapi_SetLastError(ERROR_FILE_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }

        if (0 != stat(buf, &filestat))
        {
            Winapi_SetLastError(ERROR_ACCESS_DENIED);
            return INVALID_HANDLE_VALUE;
        }

        if (S_ISDIR(filestat.st_mode))
        {
            // directory
            dirinfo = opendir(buf);
            if (dirinfo == NULL)
            {
                Winapi_SetLastError(ERROR_ACCESS_DENIED);
                return INVALID_HANDLE_VALUE;
            }

            direntry = readdir(dirinfo);
            if (direntry == NULL)
            {
                closedir(dirinfo);
                Winapi_SetLastError(ERROR_FILE_NOT_FOUND);
                return INVALID_HANDLE_VALUE;
            }
        }
        else
        {
            // regular file / other
            dirinfo = NULL;
            direntry = NULL;
        }
    }
    else
    {
        // wildcards

        // find last slash or backslash in lpFileName
        slash = strrchr(lpFileName, '/');
        pattern = strrchr(lpFileName, '\\');

        if (pattern == NULL)
        {
            pattern = slash;
        }
        else
        {
            if (slash > pattern)
            {
                pattern = slash;
            }
        }

        // copy lpFileName without last entry (delimited by '\' or '/') to orig_directory
        if ((pattern == NULL) || (pattern == lpFileName))
        {
            orig_directory[1] = '.';
            orig_directory[0] = 0;

            pattern = lpFileName;
        }
        else
        {
            strncpy(orig_directory, lpFileName, MAX_PATH - 1);
            orig_directory[MAX_PATH - 1] = 0;

            if (pattern - lpFileName < MAX_PATH)
            {
                orig_directory[pattern - lpFileName] = 0;
            }

            pattern++;
        }

        if (!CLIB_FindFile(orig_directory, buf))
        {
            Winapi_SetLastError(ERROR_FILE_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }

        dirinfo = opendir(buf);
        if (dirinfo == NULL)
        {
            Winapi_SetLastError(ERROR_ACCESS_DENIED);
            return INVALID_HANDLE_VALUE;
        }

        // copy pattern to orig_directory
        strncpy(orig_directory, pattern, MAX_PATH - 1);
        orig_directory[MAX_PATH - 1] = 0;

        // convert pattern to uppercase
        for (replace = orig_directory; *replace != 0; replace++)
        {
            *replace = toupper(*replace);
        }

        direntry = readdir(dirinfo);

        // find first name matching pattern
        while (direntry != NULL && !file_pattern_match(direntry->d_name, orig_directory))
        {
            direntry = readdir(dirinfo);
        }

        if (direntry == NULL)
        {
            closedir(dirinfo);
            Winapi_SetLastError(ERROR_FILE_NOT_FOUND);
            return INVALID_HANDLE_VALUE;
        }
    }


    ret = Winapi_AllocHandle();
    if (ret == NULL)
    {
        if (dirinfo != NULL) closedir(dirinfo);
        Winapi_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return INVALID_HANDLE_VALUE;
    }

    state = (find_file_state *) malloc(sizeof(find_file_state));
    if (state == NULL)
    {
        free(ret);
        if (dirinfo != NULL) closedir(dirinfo);
        Winapi_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return INVALID_HANDLE_VALUE;
    }

    ret->handle_type = HT_SEARCH;
    ret->sh.d = state;

    state->dirinfo = dirinfo;
    state->directory = NULL;
    state->filename = NULL;

    if (dirinfo != NULL)
    {
        state->directory = strdup(buf);

        if (state->directory == NULL)
        {
            free(state);
            free(ret);
            if (dirinfo != NULL) closedir(dirinfo);
            Winapi_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return INVALID_HANDLE_VALUE;
        }

        if (readdirtype)
        {
            state->filename = strdup(orig_directory);

            if (state->filename == NULL)
            {
                free(state->directory);
                free(state);
                free(ret);
                if (dirinfo != NULL) closedir(dirinfo);
                Winapi_SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                return INVALID_HANDLE_VALUE;
            }
        }

        snprintf(buf, 8192, "%s/%s", state->directory, direntry->d_name);

        if (0 != stat(buf, &filestat))
        {
            if (state->filename != NULL) free(state->filename);
            free(state->directory);
            free(state);
            free(ret);
            if (dirinfo != NULL) closedir(dirinfo);
            Winapi_SetLastError(ERROR_ACCESS_DENIED);
            return INVALID_HANDLE_VALUE;
        }
    }

    // fill find structure
    Conv_find((win32_find_data *)lpFindFileData, &filestat, (direntry != NULL)?direntry->d_name:buf);

#ifdef DEBUG_KERNEL32
    eprintf("found file: %s\n", (direntry != NULL)?direntry->d_name:buf);
#endif

    return ret;
#endif
}

uint32_t FindNextFileA_c(void *hFindFile, void *lpFindFileData)
{
#ifdef DEBUG_KERNEL32
    eprintf("FindNextFileA: 0x%" PRIxPTR ", 0x%" PRIxPTR "\n", (uintptr_t) hFindFile, (uintptr_t) lpFindFileData);
#endif

    if (hFindFile == NULL || lpFindFileData == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (((handle)hFindFile)->handle_type != HT_SEARCH)
    {
        Winapi_SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }

#ifdef _WIN32
    return FindNextFileA((HANDLE)((handle)hFindFile)->sh.d, (LPWIN32_FIND_DATAA)lpFindFileData);
#else
    struct dirent *direntry;
    find_file_state *state;
    struct stat filestat;
    char buf[8192];

    state = (find_file_state *) ((handle)hFindFile)->sh.d;

    if (state->dirinfo == NULL)
    {
        direntry = NULL;
    }
    else if (state->filename == NULL)
    {
        direntry = readdir(state->dirinfo);
    }
    else
    {
        direntry = readdir(state->dirinfo);

        // find first name matching pattern
        while (direntry != NULL && !file_pattern_match(direntry->d_name, state->filename))
        {
            direntry = readdir(state->dirinfo);
        }
    }

    if (direntry == NULL)
    {
        if (state->dirinfo != NULL)
        {
            closedir(state->dirinfo);
            state->dirinfo = NULL;
        }
        if (state->directory != NULL)
        {
            free(state->directory);
            state->directory = NULL;
        }
        if (state->filename != NULL)
        {
            free(state->filename);
            state->filename = NULL;
        }

        Winapi_SetLastError(ERROR_NO_MORE_FILES);
        return 0;
    }

    snprintf(buf, 8192, "%s/%s", state->directory, direntry->d_name);

    stat(buf, &filestat);

    // fill find structure
    Conv_find((win32_find_data *)lpFindFileData, &filestat, direntry->d_name);

#ifdef DEBUG_KERNEL32
    eprintf("found file: %s\n", direntry->d_name);
#endif

    return 1;
#endif
}

void *GetCurrentProcess_c(void)
{
#ifdef DEBUG_KERNEL32
    eprintf("GetCurrentProcess\n");
#endif

    // this is a pseudo handle to the current process
    return PSEUDO_HANDLE_CURRENT_PROCESS;
}

void *GetCurrentThread_c(void)
{
#ifdef DEBUG_KERNEL32
    eprintf("GetCurrentThread\n");
#endif

    // this is a pseudo handle to the current thread
    return PSEUDO_HANDLE_CURRENT_THREAD;
}

uint32_t GetFullPathNameA_c(const char *lpFileName, uint32_t nBufferLength, char *lpBuffer, PTR32(char) *lpFilePart)
{
    size_t len;
    char *p1, *p2;

#ifdef DEBUG_KERNEL32
    eprintf("GetFullPathNameA: %s, %i, 0x%" PRIxPTR ", 0x%" PRIxPTR "\n", lpFileName, nBufferLength, (uintptr_t)lpBuffer, (uintptr_t)lpFilePart);
#endif

    if (lpFileName == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    len = strlen(lpFileName);

    if (len + 1 > nBufferLength)
    {
        return len + 1;
    }

    if (lpBuffer == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    // Septerra Core only uses the function to get file path for movie playing, so it doesn't need to be full path
    strcpy(lpBuffer, lpFileName);

    if (lpFilePart != NULL)
    {
        p1 = strrchr(lpBuffer, '\\');
        p2 = strrchr(lpBuffer, '/');

        if (p1 == NULL)
        {
            p1 = p2;
        }
        else if (p2 != NULL)
        {
            if ((uintptr_t)p2 > (uintptr_t)p1)
            {
                p1 = p2;
            }
        }

        if (p1 == NULL)
        {
            p1 = lpBuffer;
        }

        *lpFilePart = p1;
    }

    return len;
}

uint32_t GetLastError_c(void)
{
    return Winapi_GetLastError();
}

uint32_t GetPrivateProfileStringA_c(const char *lpAppName, const char *lpKeyName, const char *lpDefault, char *lpReturnedString, uint32_t nSize, const char *lpFileName)
{
    FILE *file;
    unsigned int remaining_size, correct_appname;
    size_t default_length, length;
    int items;
    char buf[8192];

#ifdef DEBUG_KERNEL32
    eprintf("GetPrivateProfileStringA: %s, %s, %s, 0x%" PRIxPTR ", %i, %s\n", lpAppName, lpKeyName, lpDefault, (uintptr_t)lpReturnedString, nSize, lpFileName);
#endif

    if ((lpReturnedString == NULL) || (nSize == 0))
    {
        return 0;
    }

    if (nSize == 1)
    {
        lpReturnedString[0] = 0;
        return 0;
    }

    if ((nSize == 2) && ((lpAppName == NULL) || (lpKeyName == NULL)))
    {
        lpReturnedString[0] = 0;
        lpReturnedString[1] = 0;
        return 0;
    }

    file = NULL;
    if (lpFileName != NULL)
    {
#ifdef _WIN32
        file = fopen(lpFileName, "rb");
#else
        if (CLIB_FindFile(lpFileName, buf))
        {
            file = fopen(buf, "rb");
        }
#endif
    }

    if (file == NULL)
    {
#ifdef DEBUG_KERNEL32
        eprintf("default KeyValue (%i): %s\n", 1, lpDefault);
#endif

        if ((lpAppName == NULL) || (lpKeyName == NULL) || (lpDefault == NULL))
        {
            lpReturnedString[0] = 0;
            lpReturnedString[1] = 0;
            return 0;
        }

        default_length = strlen(lpDefault);

        if (default_length < nSize)
        {
            strcpy(lpReturnedString, lpDefault);
            return default_length;
        }
        else
        {
            strncpy(lpReturnedString, lpDefault, nSize - 1);
            lpReturnedString[nSize - 1] = 0;
            return nSize - 1;
        }
    }

    correct_appname = 0;
    remaining_size = nSize;

    while (!feof(file))
    {
        items = fscanf(file, "%8191[\r\n]", buf);
        buf[0] = 0;
        items = fscanf(file, "%8191[^\r\n]", buf);
        if (items <= 0) continue;
        length = strlen(buf);

        if (length == 0) continue;

        if (buf[0] == '[')
        {
            // section name

            char *endstr;

            endstr = strchr((char *) &buf, ']');
            if (endstr != NULL)
            {
                *endstr = 0;
                length = endstr - (char *) &(buf[1]);
            }
            else
            {
                length--;
            }

            if (lpAppName == NULL)
            {
                if (length < remaining_size)
                {
                    strcpy(lpReturnedString, (char *) &(buf[1]));
                    lpReturnedString += length + 1;
                    remaining_size -= length + 1;
                }
                else
                {
                    strncpy(lpReturnedString, (char *) &(buf[1]), remaining_size);
                    lpReturnedString[remaining_size - 1] = 0;
                    lpReturnedString[remaining_size - 2] = 0;

                    fclose(file);

                    return nSize - 2;
                }
            }
            else
            {
                if (strcasecmp(lpAppName, (char *) &(buf[1])) == 0)
                {
                    correct_appname = 1;
                }
                else
                {
                    correct_appname = 0;
                }
            }
        }
        else
        {
            char *keyvalue, *endstr;

            if ((lpAppName != NULL) && correct_appname)
            {
                keyvalue = strchr((char *) &buf, '=');
                if (keyvalue != NULL)
                {
                    *keyvalue = 0;
                    endstr = keyvalue - 1;
                    keyvalue++;
                }
                else
                {
                    endstr = (char *) &(buf[length - 1]);
                    keyvalue = (char *) &(buf[length]);
                }

                while ((endstr >= (char *) &buf) && (*endstr == ' '))
                {
                    *endstr = 0;
                    endstr--;
                }

                if (buf[0] == 0) continue;

                if (lpKeyName == NULL)
                {
                    length = (endstr + 1) - (char *) &buf;

                    if (length < remaining_size)
                    {
                        strcpy(lpReturnedString, (char *) &buf);
                        lpReturnedString += length + 1;
                        remaining_size -= length + 1;
                    }
                    else
                    {
                        strncpy(lpReturnedString, (char *) &buf, remaining_size);
                        lpReturnedString[remaining_size - 1] = 0;
                        lpReturnedString[remaining_size - 2] = 0;

                        fclose(file);

                        return nSize - 2;
                    }
                }
                else
                {
                    if (strcasecmp(lpKeyName, (char *) &buf) != 0) continue;

                    fclose(file);

                    while (*keyvalue == ' ') keyvalue++;

                    endstr = (char *) &(buf[length - 1]);

                    while ((endstr >= keyvalue) && (*endstr == ' '))
                    {
                        *endstr = 0;
                        endstr--;
                    }

                    length = (endstr + 1) - keyvalue;

                    if ((keyvalue[0] == '\"') ||(keyvalue[0] == '\''))
                    {
                        if ((keyvalue[length - 1] == keyvalue[0]) && (length != 1))
                        {
                            keyvalue[length - 1] = 0;
                            length--;
                        }
                        keyvalue++;
                        length--;
                    }

#ifdef DEBUG_KERNEL32
                    eprintf("KeyValue: %s\n", keyvalue);
#endif

                    if (length < nSize)
                    {
                        strcpy(lpReturnedString, keyvalue);
                        return length;
                    }
                    else
                    {
                        strncpy(lpReturnedString, keyvalue, nSize - 1);
                        lpReturnedString[nSize - 1] = 0;
                        return nSize - 1;
                    }
                }
            }
        }
    }

    fclose(file);

    if ((lpAppName == NULL) || (lpKeyName == NULL))
    {
        if (remaining_size == nSize)
        {
            lpReturnedString[0] = 0;
            lpReturnedString[1] = 0;
            return 0;
        }
        if (remaining_size == 0)
        {
            lpReturnedString[-1] = 0;
            lpReturnedString[-2] = 0;
            return nSize - 2;
        }

        lpReturnedString[0] = 0;
        return nSize - remaining_size;
    }

#ifdef DEBUG_KERNEL32
        eprintf("default KeyValue (%i): %s\n", 2, lpDefault);
#endif

    if (lpDefault == NULL)
    {
        lpReturnedString[0] = 0;
        return 0;
    }
    else
    {
        default_length = strlen(lpDefault);

        if (default_length < nSize)
        {
            strcpy(lpReturnedString, lpDefault);
            return default_length;
        }
        else
        {
            strncpy(lpReturnedString, lpDefault, nSize - 1);
            lpReturnedString[nSize - 1] = 0;
            return nSize - 1;
        }
    }
}

void InitializeCriticalSection_c(void *lpCriticalSection)
{
#ifdef DEBUG_KERNEL32
    eprintf("InitializeCriticalSection: 0x%" PRIxPTR "\n", (uintptr_t)lpCriticalSection);
#endif

#ifdef _WIN32
    InitializeCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
#else
    pthread_mutexattr_t attr;
    pthread_mutex_t *mutex;

    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if (mutex == NULL) goto InitializeCriticalSection_error;

    if (0 != pthread_mutexattr_init(&attr)) goto InitializeCriticalSection_error;
    if (0 != pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) goto InitializeCriticalSection_error;
    if (0 != pthread_mutex_init(mutex, &attr)) goto InitializeCriticalSection_error;
    pthread_mutexattr_destroy(&attr);

    *(pthread_mutex_t **)lpCriticalSection = mutex;

    return;

InitializeCriticalSection_error:
    eprintf("InitializeCriticalSection: mutex not created\n");
    exit(6);
#endif
}

void LeaveCriticalSection_c(void *lpCriticalSection)
{
#ifdef DEBUG_KERNEL32
    eprintf("LeaveCriticalSection: 0x%" PRIxPTR "\n", (uintptr_t)lpCriticalSection);
#endif

#ifdef _WIN32
    LeaveCriticalSection((LPCRITICAL_SECTION)lpCriticalSection);
#else
    pthread_mutex_unlock(*(pthread_mutex_t **)lpCriticalSection);
#endif
}

uint32_t QueryPerformanceCounter_c(void *lpPerformanceCount)
{
#ifdef _WIN32
    return QueryPerformanceCounter((LARGE_INTEGER *)lpPerformanceCount);
#else
    ((large_integer *)lpPerformanceCount)->LowPart = Winapi_GetTicks();
    ((large_integer *)lpPerformanceCount)->HighPart = 0;
    return 1;
#endif
}

uint32_t QueryPerformanceFrequency_c(void *lpFrequency)
{
#ifdef _WIN32
    return QueryPerformanceFrequency((LARGE_INTEGER *)lpFrequency);
#else
    // 1000 is enough for Septerra Core
    ((large_integer *)lpFrequency)->LowPart = 1000;
    ((large_integer *)lpFrequency)->HighPart = 0;

    return 1;
#endif
}

uint32_t ReadFile_c(void *hFile, void *lpBuffer, uint32_t nNumberOfBytesToRead, uint32_t *lpNumberOfBytesRead, void *lpOverlapped)
{
#ifdef DEBUG_KERNEL32
    eprintf("ReadFile: 0x%" PRIxPTR ", %i\n", (uintptr_t) hFile, nNumberOfBytesToRead);
#endif

    if (lpNumberOfBytesRead != NULL)
    {
        *lpNumberOfBytesRead = 0;
    }

    if (hFile == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (lpOverlapped != NULL)
    {
        Winapi_SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }

    if (lpBuffer == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (((handle)hFile)->handle_type == HT_PIPE)
    {
        if (((handle)hFile)->pph.pp == NULL)
        {
            Winapi_SetLastError(ERROR_INVALID_HANDLE);
            return 0;
        }

#ifdef _WIN32
        return ReadFile((HANDLE)((handle)hFile)->pph.pp, lpBuffer, nNumberOfBytesToRead, (LPDWORD)lpNumberOfBytesRead, (LPOVERLAPPED)lpOverlapped);
#else
        ssize_t ret;

        ret = read((intptr_t)((handle)hFile)->pph.pp, lpBuffer, nNumberOfBytesToRead);

        if (ret == -1)
        {
            return 0;
        }

        if (lpNumberOfBytesRead != NULL)
        {
            *lpNumberOfBytesRead = ret;
        }

        return 1;
#endif
    }

    if (((handle)hFile)->handle_type == HT_FILE)
    {
        FILE *f;
        ssize_t bytesreadtotal;
        uint32_t bytes_to_read;
        int iseof;

        f = (FILE *)((handle)hFile)->fh.f;
        if (f == NULL)
        {
            Winapi_SetLastError(ERROR_INVALID_HANDLE);
            return 0;
        }

        bytesreadtotal = 0;
        iseof = 0;
        bytes_to_read = nNumberOfBytesToRead;
        while (bytes_to_read)
        {
            size_t bytesread;

            bytesread = fread(lpBuffer, 1, bytes_to_read, f);
            if (bytesread == 0)
            {
                bytes_to_read = 0;

                if (feof(f))
                {
                    iseof = 1;
                }
                clearerr(f);
            }
            else
            {
                if (bytesread != bytes_to_read)
                {
                    clearerr(f);
                }
                bytesreadtotal += bytesread;
                bytes_to_read -= bytesread;
                lpBuffer = (void *) (((uintptr_t) lpBuffer) + bytesread);
            }
        }

        if ((bytesreadtotal != 0) || (nNumberOfBytesToRead == 0))
        {
            if (lpNumberOfBytesRead != NULL)
            {
                *lpNumberOfBytesRead = bytesreadtotal;
            }

            return 1;
        }
        else
        {
            if (iseof)
            {
                Winapi_SetLastError(ERROR_HANDLE_EOF);
                return 1;
            }
            else
            {
                Winapi_SetLastError(ERROR_READ_FAULT);
                return 0;
            }
        }
    }

    eprintf("Unsupported method: %s\n", "ReadFile");
    exit(1);
}

uint32_t SetErrorMode_c(uint32_t uMode)
{
#ifdef DEBUG_KERNEL32
    eprintf("SetErrorMode: 0x%x\n", uMode);
#endif

    return 0;
}

uint32_t SetFilePointer_c(void *hFile, uint32_t lDistanceToMove, uint32_t *lpDistanceToMoveHigh, uint32_t dwMoveMethod)
{
    eprintf("Unimplemented: %s\n", "SetFilePointer");
    exit(1);
//    return SetFilePointer((HANDLE)hFile, lDistanceToMove, (PLONG)lpDistanceToMoveHigh, dwMoveMethod);
}

uint32_t SetPriorityClass_c(void *hProcess, uint32_t fdwPriority)
{
#ifdef DEBUG_KERNEL32
    eprintf("SetPriorityClass: 0x%" PRIxPTR ", %i\n", (uintptr_t) hProcess, fdwPriority);
#endif

    if (hProcess == PSEUDO_HANDLE_CURRENT_PROCESS)
    {
        // pseudo handle
        if (fdwPriority == HIGH_PRIORITY_CLASS)
        {
            // function is failing
            return 0;
        }
    }

    eprintf("Unsupported method: %s\n", "SetPriorityClass");
    exit(1);
}

uint32_t SetThreadPriority_c(void *hThread, int32_t nPriority)
{
#ifdef DEBUG_KERNEL32
    eprintf("SetThreadPriority: 0x%" PRIxPTR ", %i\n", (uintptr_t) hThread, nPriority);
#endif

    if (hThread == PSEUDO_HANDLE_CURRENT_THREAD)
    {
        // pseudo handle

#ifdef _WIN32
        return SetThreadPriority(GetCurrentThread(), nPriority);
#else
        if (nPriority == THREAD_PRIORITY_TIME_CRITICAL)
        {
            // function is failing
            return 0;
        }
#endif
    }

    eprintf("Unsupported method: %s\n", "SetThreadPriority");
    exit(1);
}

void Sleep_c(uint32_t cMilliseconds)
{
#ifdef DEBUG_KERNEL32
    eprintf("Sleep: %i\n", cMilliseconds);
#endif

#ifdef _WIN32
    Sleep(cMilliseconds);
#else
    struct timespec _tp, rem;
    int ret;

    _tp.tv_sec = cMilliseconds / 1000;
    _tp.tv_nsec = (cMilliseconds % 1000) * 1000000;

    ret = nanosleep(&_tp, &rem);

    while (1)
    {
        if (ret == 0) break;
        else if (errno != EINTR) break;
        else if ((rem.tv_sec == 0) && (rem.tv_nsec < 500000)) break;

        _tp.tv_sec = rem.tv_sec;
        _tp.tv_nsec = rem.tv_nsec;

        ret = nanosleep(&_tp, &rem);
    }
#endif
}

uint32_t WriteFile_c(void *hFile, const void *lpBuffer, uint32_t nNumberOfBytesToWrite, uint32_t *lpNumberOfBytesWritten, void *lpOverlapped)
{
#ifdef DEBUG_KERNEL32
    eprintf("WriteFile: 0x%" PRIxPTR ", %i\n", (uintptr_t) hFile, nNumberOfBytesToWrite);
#endif

    if (lpNumberOfBytesWritten != NULL)
    {
        *lpNumberOfBytesWritten = 0;
    }

    if (hFile == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (lpOverlapped != NULL)
    {
        Winapi_SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }

    if (lpBuffer == NULL)
    {
        Winapi_SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (((handle)hFile)->handle_type == HT_PIPE)
    {
        if (((handle)hFile)->pph.pp == NULL)
        {
            Winapi_SetLastError(ERROR_INVALID_HANDLE);
            return 0;
        }

#ifdef _WIN32
        return WriteFile((HANDLE)((handle)hFile)->pph.pp, lpBuffer, nNumberOfBytesToWrite, (LPDWORD)lpNumberOfBytesWritten, (LPOVERLAPPED)lpOverlapped);
#else
        ssize_t ret;

        ret = write((intptr_t)((handle)hFile)->pph.pp, lpBuffer, nNumberOfBytesToWrite);

        if (ret == -1)
        {
            return 0;
        }

        if (lpNumberOfBytesWritten != NULL)
        {
            *lpNumberOfBytesWritten = ret;
        }

        return 1;
#endif
    }

    if (((handle)hFile)->handle_type == HT_FILE)
    {
        FILE *f;
        ssize_t byteswrittentotal;
        uint32_t bytes_to_write;

        f = (FILE *)((handle)hFile)->fh.f;
        if (f == NULL)
        {
            Winapi_SetLastError(ERROR_INVALID_HANDLE);
            return 0;
        }

        byteswrittentotal = 0;
        bytes_to_write = nNumberOfBytesToWrite;

        while (bytes_to_write)
        {
            size_t byteswritten;

            byteswritten = fwrite(lpBuffer, 1, bytes_to_write, f);
            if (byteswritten == 0)
            {
                bytes_to_write = 0;
                clearerr(f);
            }
            else
            {
                if (byteswritten != bytes_to_write)
                {
                    clearerr(f);
                }
                byteswrittentotal += byteswritten;
                bytes_to_write -= byteswritten;
                lpBuffer = (void *) (((uintptr_t) lpBuffer) + byteswritten);
            }
        }

        if ((byteswrittentotal != 0) || (nNumberOfBytesToWrite == 0))
        {
            if (lpNumberOfBytesWritten != NULL)
            {
                *lpNumberOfBytesWritten = byteswrittentotal;
            }

            return 1;
        }
        else
        {
            Winapi_SetLastError(ERROR_WRITE_FAULT);
            return 0;
        }
    }

    eprintf("Unsupported method: %s\n", "WriteFile");
    exit(1);
}

