/**
 *
 *  Copyright (C) 2021-2025 Roman Pauer
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

#if defined(__WINE__) || !defined(OLDVIDEO)
#define USE_QUICKTIMELIB
#else
#undef USE_QUICKTIMELIB
#endif

#include "SDI.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#if defined(USE_QUICKTIMELIB) && !defined(__WINE__)
#include <stdlib.h>
#endif
#include "platform.h"


static const char * const SDI_INI_name = "SDI.INI";
const char *SDI_INI_path;
const char *SDI_install_path;
const char *SDI_CD_path;

static struct {
    DWORD Last;
    int Delay;
} TicksArray[13];


void SDI_CheckTicksDelay(int index)
{
    DWORD Ticks;

    Ticks = GetTickCount();
    if (Ticks == TicksArray[index].Last)
    {
        if (TicksArray[index].Delay == 0)
        {
            TicksArray[index].Delay = 1;
            Sleep(0);
        }
        else
        {
            TicksArray[index].Last = 0;
            Sleep(1);
        }
    }
    else
    {
        TicksArray[index].Last = Ticks;
        TicksArray[index].Delay = 0;
    }
}


static int get_ansi_path(LPCWSTR lpPath, const char **result)
{
    DWORD dwAnsiLength, dwShortLength;
    HANDLE hHeap;
    char *lpAnsiBuffer;
    LPWSTR lpShortBuffer;
    BOOL bUsedDefaultChar;

    lpShortBuffer = NULL;

    // get length of ansi string
    dwAnsiLength = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, lpPath, -1, NULL, 0, NULL, NULL);
    hHeap = GetProcessHeap();
    if (dwAnsiLength < MAX_PATH)
    {
        // allocate buffer for ansi string
        lpAnsiBuffer = (char *) HeapAlloc(hHeap, 0, dwAnsiLength);
        if (lpAnsiBuffer == NULL) goto error;

        // convert unicode path to ansi string
        dwAnsiLength = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, lpPath, -1, lpAnsiBuffer, dwAnsiLength, NULL, &bUsedDefaultChar);
        if (dwAnsiLength == 0) goto error;

        if (!bUsedDefaultChar && dwAnsiLength < MAX_PATH)
        {
            // success, if ansi string is short enough and all characters could be represented
            *result = lpAnsiBuffer;
            return 1;
        }

        HeapFree(hHeap, 0, lpAnsiBuffer);
    }

    lpAnsiBuffer = NULL;

    // get length of short path
    dwShortLength = GetShortPathNameW(lpPath, NULL, 0);
    if (dwShortLength == 0) goto error;

    // allocate buffer for short path
    lpShortBuffer = (LPWSTR) HeapAlloc(hHeap, 0, dwShortLength * sizeof(WCHAR));
    if (lpShortBuffer == NULL) goto error;

    // get short path from unicode path
    dwShortLength = GetShortPathNameW(lpPath, lpShortBuffer, dwShortLength);
    if (dwShortLength == 0) goto error;

    // get length of ansi string
    dwAnsiLength = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, lpShortBuffer, -1, NULL, 0, NULL, NULL);
    if (dwAnsiLength >= MAX_PATH) goto error;

    // allocate buffer for ansi string
    lpAnsiBuffer = (char *) HeapAlloc(hHeap, 0, dwAnsiLength);
    if (lpAnsiBuffer == NULL) goto error;

    // convert short path to ansi string
    dwAnsiLength = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, lpShortBuffer, -1, lpAnsiBuffer, dwAnsiLength, NULL, &bUsedDefaultChar);
    if (dwAnsiLength == 0) goto error;

    HeapFree(hHeap, 0, lpShortBuffer);
    lpShortBuffer = NULL;

    if (!bUsedDefaultChar && dwAnsiLength < MAX_PATH)
    {
        // success, if ansi string is short enough and all characters could be represented
        *result = lpAnsiBuffer;
        return 1;
    }

error:
    if (lpShortBuffer != NULL)
    {
        HeapFree(hHeap, 0, lpShortBuffer);
    }
    if (lpAnsiBuffer != NULL)
    {
        HeapFree(hHeap, 0, lpAnsiBuffer);
    }
    *result = NULL;
    return 0;
}

static int get_environment_path(LPCWSTR lpName, const char **result)
{
    DWORD dwLength;
    LPWSTR lpBuffer, lpPath;
    HANDLE hHeap;
#if defined(__WINE__)
    DWORD dwUnixLength;
    char *lpUnixBUffer;
#endif

    // get length of environment variable
    dwLength = GetEnvironmentVariableW(lpName, NULL, 0);
    if (dwLength <= 1)
    {
        // environment variable doesn't exist
        *result = NULL;
        return 1;
    }

    // allocate buffer for environment variable
    hHeap = GetProcessHeap();
    lpBuffer = (LPWSTR) HeapAlloc(hHeap, 0, (dwLength + 1 + 1) * sizeof(WCHAR)); // include space for adding backslash
    if (lpBuffer == NULL) goto error;

    // get environment variable
    dwLength = GetEnvironmentVariableW(lpName, lpBuffer, dwLength);
    if (dwLength == 0) goto error;

    // strip trailing slashes/backslashes
    while ((dwLength != 0) && ((lpBuffer[dwLength - 1] == L'\\') || (lpBuffer[dwLength - 1] == L'/')))
    {
        lpBuffer[dwLength - 1] = 0;
        dwLength--;
    }

#if !defined(__WINE__)
    if ((dwLength == 2) && (lpBuffer[1] == L':'))
    {
        lpBuffer[2] = L'\\';
        lpBuffer[3] = 0;
        dwLength++;
    }
#endif

    if (dwLength == 0)
    {
        HeapFree(hHeap, 0, lpBuffer);
        *result = NULL;
        return 1;
    }

#if defined(__WINE__)
    // get length of unix path
    dwUnixLength = WideCharToMultiByte(CP_UNIXCP, 0, lpBuffer, -1, NULL, 0, NULL, NULL);

    // allocate buffer for unix path
    lpUnixBUffer = (char *) HeapAlloc(hHeap, 0, dwUnixLength);
    if (lpUnixBUffer == NULL) goto error;

    // convert environment variable to unix path
    dwUnixLength = WideCharToMultiByte(CP_UNIXCP, 0, lpBuffer, -1, lpUnixBUffer, dwUnixLength, NULL, NULL);
    if (dwUnixLength == 0)
    {
        HeapFree(hHeap, 0, lpUnixBUffer);
        goto error;
    }

    HeapFree(hHeap, 0, lpBuffer);
    lpBuffer = NULL;

    // get windows path from unix path
    lpPath = wine_get_dos_file_name(lpUnixBUffer);
    HeapFree(hHeap, 0, lpUnixBUffer);

    if (lpPath == NULL) goto error;
#else
    lpPath = lpBuffer;
#endif

    // get ansi path from unicode path
    if (!get_ansi_path(lpPath, result)) goto error;

    if (lpBuffer != NULL)
    {
        HeapFree(hHeap, 0, lpBuffer);
    }
    return 1;

error:
    if (lpBuffer != NULL)
    {
        HeapFree(hHeap, 0, lpBuffer);
    }
    *result = NULL;
    return 0;
}

static int get_executable_path(LPCWSTR lpFileName, int include_filename, const char **result)
{
    WCHAR executable[MAX_PATH];
    DWORD dwAttrib, dwLength;
    HANDLE hHeap;
    LPWSTR lpBuffer, lpFilePart;

    // get path to executable
    dwLength = GetModuleFileNameW(NULL, executable, MAX_PATH);
    if ((dwLength == 0) || (dwLength >= MAX_PATH)) return 0;

    // get length of full path
    dwLength = GetFullPathNameW(executable, 0, NULL, NULL);
    if (dwLength == 0) return 0;

    // allocate buffer for full path
    hHeap = GetProcessHeap();
    lpBuffer = (LPWSTR) HeapAlloc(hHeap, 0, (dwLength + ((lpFileName != NULL)?lstrlenW(lpFileName):0)) * sizeof(WCHAR));
    if (lpBuffer == NULL) goto error;

    // locate the filename in path
    dwLength = GetFullPathNameW(executable, dwLength, lpBuffer, &lpFilePart);
    if (dwLength == 0) goto error;

    if (lpFileName != NULL)
    {
        // replace filename in path with input filename
        lstrcpyW(lpFilePart, lpFileName);

        // check if input filename exists in executable's directory
        dwAttrib = GetFileAttributesW(lpBuffer);
        if ((dwAttrib == INVALID_FILE_ATTRIBUTES) || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) goto error;
    }

    // if it does, then use the full path to it
    if (!include_filename)
    {
        lpFilePart[-1] = 0;

        if ((lpBuffer[1] == L':') && (lpBuffer[2] == 0))
        {
            lpBuffer[2] = L'\\';
            lpBuffer[3] = 0;
        }
    }

    // get ansi path from unicode path
    if (!get_ansi_path(lpBuffer, result)) goto error;

    HeapFree(hHeap, 0, lpBuffer);
    return 1;

error:
    if (lpBuffer != NULL)
    {
        HeapFree(hHeap, 0, lpBuffer);
    }
    *result = NULL;
    return 0;
}

static int get_full_path(LPCWSTR lpFileName, const char **result)
{
    DWORD dwLength;
    HANDLE hHeap;
    LPWSTR lpBuffer;

    // get length of full path
    dwLength = GetFullPathNameW(lpFileName, 0, NULL, NULL);
    if (dwLength == 0) return 0;

    // allocate buffer for full path
    hHeap = GetProcessHeap();
    lpBuffer = (LPWSTR) HeapAlloc(hHeap, 0, dwLength * sizeof(WCHAR));
    if (lpBuffer == NULL) goto error;

    // get full path
    dwLength = GetFullPathNameW(lpFileName, dwLength, lpBuffer, NULL);
    if (dwLength == 0) goto error;

    // get ansi path from unicode path
    if (!get_ansi_path(lpBuffer, result)) goto error;

    HeapFree(hHeap, 0, lpBuffer);
    return 1;

error:
    if (lpBuffer != NULL)
    {
        HeapFree(hHeap, 0, lpBuffer);
    }
    *result = NULL;
    return 0;
}

static int get_current_path(const char **result)
{
    DWORD dwLength;
    HANDLE hHeap;
    LPWSTR lpBuffer;

    // get length of current path
    dwLength = GetCurrentDirectoryW(0, NULL);
    if (dwLength == 0) return 0;

    // allocate buffer for current path
    hHeap = GetProcessHeap();
    lpBuffer = (LPWSTR) HeapAlloc(hHeap, 0, dwLength * sizeof(WCHAR));
    if (lpBuffer == NULL) goto error;

    // get current path
    dwLength = GetCurrentDirectoryW(dwLength, lpBuffer);
    if (dwLength == 0) goto error;

    // get ansi path from unicode path
    if (!get_ansi_path(lpBuffer, result)) goto error;

    HeapFree(hHeap, 0, lpBuffer);
    return 1;

error:
    if (lpBuffer != NULL)
    {
        HeapFree(hHeap, 0, lpBuffer);
    }
    *result = NULL;
    return 0;
}

static int duplicate_ansi_string(char *lpPath, DWORD dwLength, const char **result)
{
    char *lpAnsiBuffer;

    if ((dwLength == 0) || (dwLength >= MAX_PATH)) return 0;

    lpAnsiBuffer = (char *) HeapAlloc(GetProcessHeap(), 0, dwLength + 1);
    if (lpAnsiBuffer == NULL) return 0;

    lstrcpyA(lpAnsiBuffer, lpPath);
    *result = lpAnsiBuffer;
    return 1;
}

static int set_current_path(void)
{
    WCHAR executable[MAX_PATH];
    DWORD dwLength;
    HANDLE hHeap;
    LPWSTR lpBuffer, lpFilePart;

    // get path to executable
    dwLength = GetModuleFileNameW(NULL, executable, MAX_PATH);
    if ((dwLength == 0) || (dwLength >= MAX_PATH)) return 0;

    // get length of full path
    dwLength = GetFullPathNameW(executable, 0, NULL, NULL);
    if (dwLength == 0) return 0;

    // allocate buffer for full path
    hHeap = GetProcessHeap();
    lpBuffer = (LPWSTR) HeapAlloc(hHeap, 0, dwLength * sizeof(WCHAR));
    if (lpBuffer == NULL) goto error;

    // locate the filename in path
    dwLength = GetFullPathNameW(executable, dwLength, lpBuffer, &lpFilePart);
    if (dwLength == 0) goto error;

    // remove filename in path
    lpFilePart[0] = 0;

    // set current directory
    if (!SetCurrentDirectoryW(lpBuffer)) goto error;

    HeapFree(hHeap, 0, lpBuffer);
    return 1;

error:
    if (lpBuffer != NULL)
    {
        HeapFree(hHeap, 0, lpBuffer);
    }
    return 0;
}

NOINLINE int SDI_LocatePaths(void);
int SDI_LocatePaths(void)
{
    DWORD dwAttrib, dwLength;
#if defined(USE_QUICKTIMELIB) && !defined(__WINE__)
    char buffer[MAX_PATH + 24];
#else
    char buffer[MAX_PATH + 8];
#endif


    // if environment variable SDI_INSTALL_PATH is set then it contains the path to the installed game
    if (!get_environment_path(L"SDI_INSTALL_PATH", &SDI_install_path)) return 0;


    SDI_INI_path = NULL;
    if (SDI_install_path != NULL)
    {
        lstrcpyA(buffer, SDI_install_path);
        if (!((buffer[1] == ':') && (buffer[2] == '\\') && (buffer[3] == 0)))
        {
            lstrcatA(buffer, "\\");
        }
        lstrcatA(buffer, SDI_INI_name);
        dwLength = lstrlenA(buffer);

        if (dwLength <= MAX_PATH)
        {
            // check if SDI.INI exists in the game install directory
            dwAttrib = GetFileAttributesA(buffer);
            if ((dwAttrib != INVALID_FILE_ATTRIBUTES) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
            {
                // if it does, then use the full path to it
                if (!duplicate_ansi_string(buffer, dwLength, &SDI_INI_path)) return 0;
            }
        }
    }

    if (SDI_INI_path == NULL)
    {
        // check if SDI.INI exists in executable's directory
        // if it does, then use the full path to it
        if (!get_executable_path(L"SDI.INI", 1, &SDI_INI_path)) return 0;
    }

    if (SDI_INI_path == NULL)
    {
        // check if SDI.INI exists in current directory
        dwAttrib = GetFileAttributesA(SDI_INI_name);
        if ((dwAttrib != INVALID_FILE_ATTRIBUTES) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
        {
            // if it does, then get the full path to it
            if (!get_full_path(L"SDI.INI", &SDI_INI_path)) return 0;
        }
    }

    if (SDI_INI_path == NULL)
    {
        // if SDI.INI wasn't found, then use SDI.INI in windows directory
        SDI_INI_path = SDI_INI_name;
    }


    if (SDI_install_path == NULL)
    {
        // check if TST.LIB exists in executable's directory
        // if it does, then use executable's directory as game install directory
        if (!get_executable_path(L"TST.LIB", 0, &SDI_install_path)) return 0;
    }

    if (SDI_install_path == NULL)
    {
        // use current directory as game install directory
        if (!get_current_path(&SDI_install_path)) return 0;
    }


    // if environment variable SDI_CD_PATH is set then it contains the path to the game CD
    if (!get_environment_path(L"SDI_CD_PATH", &SDI_CD_path)) return 0;

#if !defined(__WINE__)
    if (SDI_CD_path == NULL)
    {
        dwLength = GetPrivateProfileStringA("FILES", "PATH3", NULL, buffer, MAX_PATH + 1, SDI_INI_name);

        while ((dwLength != 0) && ((buffer[dwLength - 1] == '\\') || (buffer[dwLength - 1] == '/')))
        {
            buffer[dwLength - 1] = 0;
            dwLength--;
        }

        if ((dwLength != 0) && (dwLength < (MAX_PATH - 7)))
        {
            // add trailing backslash
            buffer[dwLength] = '\\';
            dwLength++;

            lstrcatA(buffer, "CD2.ID");

            // check if CD2.ID exists in path written in SDI.INI in windows directory
            dwAttrib = GetFileAttributesA(buffer);
            if ((dwAttrib != INVALID_FILE_ATTRIBUTES) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
            {
                // if it does, then use the path as game CD path
                if ((dwLength == 3) && (buffer[1] == ':'))
                {
                    buffer[dwLength] = 0;
                }
                else
                {
                    buffer[dwLength - 1] = 0;
                    dwLength--;
                }

                if (!duplicate_ansi_string(buffer, dwLength, &SDI_CD_path)) return 0;
            }
        }
    }
#endif

    if (SDI_CD_path == NULL)
    {
        // use DATA firectory in game install path as game CD path
        lstrcpyA(buffer, SDI_install_path);
        if (!((buffer[1] == ':') && (buffer[2] == '\\') && (buffer[3] == 0)))
        {
            lstrcatA(buffer, "\\");
        }
        lstrcatA(buffer, "DATA");

        if (!duplicate_ansi_string(buffer, lstrlenA(buffer), &SDI_CD_path)) return 0;
    }

#if defined(USE_QUICKTIMELIB) && !defined(__WINE__)
    if (NULL == getenv("LIBQUICKTIME_PLUGIN_DIR"))
    {
        const char *plugin_path;
        if (get_executable_path(NULL, 0, &plugin_path))
        {
            lstrcpyA(buffer, "LIBQUICKTIME_PLUGIN_DIR");
            buffer[23] = '=';
            buffer[24] = 0;
            lstrcatA(buffer, plugin_path);
            HeapFree(GetProcessHeap(), 0, (LPVOID)plugin_path);

            putenv(buffer);
        }
    }
#endif

    set_current_path();

    return 1;
}

