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

#include "WinApi-kernel32.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


extern const char *SDI_INI_path;
extern const char *SDI_install_path;
extern const char *SDI_CD_path;


uint32_t CloseHandle_c(void * hObject)
{
    return CloseHandle((HANDLE)hObject);
}

void * CreateFileA_c(const char * lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode, void * lpSecurityAttributes, uint32_t dwCreationDisposition, uint32_t dwFlagsAndAttributes, void * hTemplateFile)
{
    return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, (LPSECURITY_ATTRIBUTES)lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, (HANDLE)hTemplateFile);
}

uint32_t FindClose_c(void * hFindFile)
{
    return FindClose((HANDLE)hFindFile);
}

void * FindFirstFileA_c(const char * lpFileName, void * lpFindFileData)
{
    return FindFirstFileA(lpFileName, (LPWIN32_FIND_DATAA)lpFindFileData);
}

uint32_t FindNextFileA_c(void * hFindFile, void * lpFindFileData)
{
    return FindNextFileA((HANDLE)hFindFile, (LPWIN32_FIND_DATAA)lpFindFileData);
}

uint32_t GetCurrentDirectoryA_c(uint32_t nBufferLength, char * lpBuffer)
{
    return GetCurrentDirectoryA(nBufferLength, lpBuffer);
}

uint32_t GetPrivateProfileIntA_c(const char * lpAppName, const char * lpKeyName, int32_t nDefault, const char * lpFileName)
{
    // change: use full path to SDI.INI
    //return GetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, lpFileName);
    return GetPrivateProfileIntA(lpAppName, lpKeyName, nDefault, SDI_INI_path);
}

uint32_t GetPrivateProfileStringA_c(const char * lpAppName, const char * lpKeyName, const char * lpDefault, char * lpReturnedString, uint32_t nSize, const char * lpFileName)
{
    // change: don't get paths from SDI.INI
    if (0 == lstrcmpiA(lpAppName, "FILES"))
    {
        const char *path;
        int len;

        if ((0 == lstrcmpiA(lpKeyName, "SDIPATH")) || (0 == lstrcmpiA(lpKeyName, "PATH2")))
        {
            path = SDI_install_path;
        }
        else if (0 == lstrcmpiA(lpKeyName, "PATH3"))
        {
            path = SDI_CD_path;
        }
        else
        {
            if (lpReturnedString != NULL)
            {
                *lpReturnedString = 0;
            }
            return 0;
        }

        if (lpReturnedString == NULL)
        {
            return 0;
        }

        len = lstrlenA(path);
        if (len < nSize)
        {
            lstrcpyA(lpReturnedString, path);
            return len;
        }
        else
        {
            lstrcpynA(lpReturnedString, path, nSize - 1);
            return nSize - 1;
        }
    }

    // change: use full path to SDI.INI
    //return GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
    return GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, SDI_INI_path);
}

uint32_t GetTickCount_c(void)
{
    return GetTickCount();
}

void GlobalMemoryStatus_c(void * lpBuffer)
{
    // change: return saturated values
    //return GlobalMemoryStatus((LPMEMORYSTATUS)lpBuffer);
    MEMORYSTATUSEX status;

    ((LPMEMORYSTATUS)lpBuffer)->dwLength = sizeof(MEMORYSTATUS);
    status.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&status))
    {
        ((LPMEMORYSTATUS)lpBuffer)->dwMemoryLoad = status.dwMemoryLoad;
        ((LPMEMORYSTATUS)lpBuffer)->dwTotalPhys     = (status.ullTotalPhys     <= MAXLONG   ) ? status.ullTotalPhys     : MAXLONG;
        ((LPMEMORYSTATUS)lpBuffer)->dwAvailPhys     = (status.ullAvailPhys     <= 1000000000) ? status.ullAvailPhys     : 1000000000;
        ((LPMEMORYSTATUS)lpBuffer)->dwTotalPageFile = (status.ullTotalPageFile <= MAXLONG   ) ? status.ullTotalPageFile : MAXLONG;
        ((LPMEMORYSTATUS)lpBuffer)->dwAvailPageFile = (status.ullAvailPageFile <= 1000000000) ? status.ullAvailPageFile : 1000000000;
        ((LPMEMORYSTATUS)lpBuffer)->dwTotalVirtual  = (status.ullTotalVirtual  <= MAXLONG   ) ? status.ullTotalVirtual  : MAXLONG;
        ((LPMEMORYSTATUS)lpBuffer)->dwAvailVirtual  = (status.ullAvailVirtual  <= 1000000000) ? status.ullAvailVirtual  : 1000000000;
    }
    else
    {
        ((LPMEMORYSTATUS)lpBuffer)->dwMemoryLoad = 100;
        ((LPMEMORYSTATUS)lpBuffer)->dwTotalPhys     = MAXLONG;
        ((LPMEMORYSTATUS)lpBuffer)->dwAvailPhys     = 0;
        ((LPMEMORYSTATUS)lpBuffer)->dwTotalPageFile = MAXLONG;
        ((LPMEMORYSTATUS)lpBuffer)->dwAvailPageFile = 0;
        ((LPMEMORYSTATUS)lpBuffer)->dwTotalVirtual  = MAXLONG;
        ((LPMEMORYSTATUS)lpBuffer)->dwAvailVirtual  = 0;
    }
}

uint32_t ReadFile_c(void * hFile, void * lpBuffer, uint32_t nNumberOfBytesToRead, uint32_t * lpNumberOfBytesRead, void * lpOverlapped)
{
    return ReadFile((HANDLE)hFile, lpBuffer, nNumberOfBytesToRead, (LPDWORD)lpNumberOfBytesRead, (LPOVERLAPPED)lpOverlapped);
}

uint32_t SetCurrentDirectoryA_c(const char * lpPathName)
{
    return SetCurrentDirectoryA(lpPathName);
}

uint32_t SetFilePointer_c(void * hFile, int32_t lDistanceToMove, int32_t * lpDistanceToMoveHigh, uint32_t dwMoveMethod)
{
    return SetFilePointer((HANDLE)hFile, lDistanceToMove, (PLONG)lpDistanceToMoveHigh, dwMoveMethod);
}

uint32_t WriteFile_c(void * hFile, const void * lpBuffer, uint32_t nNumberOfBytesToWrite, uint32_t * lpNumberOfBytesWritten, void * lpOverlapped)
{
    return WriteFile((HANDLE)hFile, lpBuffer, nNumberOfBytesToWrite, (LPDWORD)lpNumberOfBytesWritten, (LPOVERLAPPED)lpOverlapped);
}

uint32_t WritePrivateProfileStringA_c(const char * lpAppName, const char * lpKeyName, const char * lpString, const char * lpFileName)
{
    // change: use full path to SDI.INI
    //return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
    return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, SDI_INI_path);
}

