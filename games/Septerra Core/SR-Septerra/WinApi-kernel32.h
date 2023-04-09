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

#if !defined(_WINAPI_KERNEL32_H_INCLUDED_)
#define _WINAPI_KERNEL32_H_INCLUDED_

#include "ptr32.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t Beep_c(uint32_t dwFreq, uint32_t dwDuration);
uint32_t CloseHandle_c(void *hObject);
uint32_t CreateDirectoryA_c(const char *lpPathName, void *lpSecurityAttributes);
void *CreateFileA_c(const char *lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode, void *lpSecurityAttributes, uint32_t dwCreationDistribution, uint32_t dwFlagsAndAttributes, void *hTemplateFile);
void *CreateMutexA_c(void *lpMutexAttributes, uint32_t bInitialOwner, const char *lpName);
uint32_t CreatePipe_c(PTR32(void) *hReadPipe, PTR32(void) *hWritePipe, void *lpPipeAttributes, uint32_t nSize);
void DeleteCriticalSection_c(void *lpCriticalSection);
uint32_t DeleteFileA_c(const char *lpFileName);
void EnterCriticalSection_c(void *lpCriticalSection);
void ExitProcess_c(uint32_t uExitCode);
uint32_t FileTimeToLocalFileTime_c(const void *lpFileTime, void *lpLocalFileTime);
uint32_t FileTimeToSystemTime_c(const void *lpFileTime, void *lpSystemTime);
uint32_t FindClose_c(void *hFindFile);
void *FindFirstFileA_c(const char *lpFileName, void *lpFindFileData);
uint32_t FindNextFileA_c(void *hFindFile, void *lpFindFileData);
void *GetCurrentProcess_c(void);
void *GetCurrentThread_c(void);
uint32_t GetFullPathNameA_c(const char *lpFileName, uint32_t nBufferLength, char *lpBuffer, PTR32(char) *lpFilePart);
uint32_t GetLastError_c(void);
uint32_t GetPrivateProfileStringA_c(const char *lpAppName, const char *lpKeyName, const char *lpDefault, char *lpReturnedString, uint32_t nSize, const char *lpFileName);
void InitializeCriticalSection_c(void *lpCriticalSection);
void LeaveCriticalSection_c(void *lpCriticalSection);
uint32_t QueryPerformanceCounter_c(void *lpPerformanceCount);
uint32_t QueryPerformanceFrequency_c(void *lpFrequency);
uint32_t ReadFile_c(void *hFile, void *lpBuffer, uint32_t nNumberOfBytesToRead, uint32_t *lpNumberOfBytesRead, void *lpOverlapped);
uint32_t SetErrorMode_c(uint32_t uMode);
uint32_t SetFilePointer_c(void *hFile, uint32_t lDistanceToMove, uint32_t *lpDistanceToMoveHigh, uint32_t dwMoveMethod);
uint32_t SetPriorityClass_c(void *hProcess, uint32_t fdwPriority);
uint32_t SetThreadPriority_c(void *hThread, int32_t nPriority);
void Sleep_c(uint32_t cMilliseconds);
uint32_t WriteFile_c(void *hFile, const void *lpBuffer, uint32_t nNumberOfBytesToWrite, uint32_t *lpNumberOfBytesWritten, void *lpOverlapped);

#ifdef __cplusplus
}
#endif

#endif

