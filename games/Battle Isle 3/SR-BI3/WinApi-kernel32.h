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

#if !defined(_WINAPI_KERNEL32_H_INCLUDED_)
#define _WINAPI_KERNEL32_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t CloseHandle_c(void * hObject);
void * CreateFileA_c(const char * lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode, void * lpSecurityAttributes, uint32_t dwCreationDisposition, uint32_t dwFlagsAndAttributes, void * hTemplateFile);
uint32_t FindClose_c(void * hFindFile);
void * FindFirstFileA_c(const char * lpFileName, void * lpFindFileData);
uint32_t FindNextFileA_c(void * hFindFile, void * lpFindFileData);
uint32_t GetCurrentDirectoryA_c(uint32_t nBufferLength, char * lpBuffer);
uint32_t GetPrivateProfileIntA_c(const char * lpAppName, const char * lpKeyName, int32_t nDefault, const char * lpFileName);
uint32_t GetPrivateProfileStringA_c(const char * lpAppName, const char * lpKeyName, const char * lpDefault, char * lpReturnedString, uint32_t nSize, const char * lpFileName);
uint32_t GetTickCount_c(void);
void GlobalMemoryStatus_c(void * lpBuffer);
uint32_t ReadFile_c(void * hFile, void * lpBuffer, uint32_t nNumberOfBytesToRead, uint32_t * lpNumberOfBytesRead, void * lpOverlapped);
uint32_t SetCurrentDirectoryA_c(const char * lpPathName);
uint32_t SetFilePointer_c(void * hFile, int32_t lDistanceToMove, int32_t * lpDistanceToMoveHigh, uint32_t dwMoveMethod);
uint32_t WriteFile_c(void * hFile, const void * lpBuffer, uint32_t nNumberOfBytesToWrite, uint32_t * lpNumberOfBytesWritten, void * lpOverlapped);
uint32_t WritePrivateProfileStringA_c(const char * lpAppName, const char * lpKeyName, const char * lpString, const char * lpFileName);

#ifdef __cplusplus
}
#endif

#endif

