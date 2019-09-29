;;
;;  Copyright (C) 2019 Roman Pauer
;;
;;  Permission is hereby granted, free of charge, to any person obtaining a copy of
;;  this software and associated documentation files (the "Software"), to deal in
;;  the Software without restriction, including without limitation the rights to
;;  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
;;  of the Software, and to permit persons to whom the Software is furnished to do
;;  so, subject to the following conditions:
;;
;;  The above copyright notice and this permission notice shall be included in all
;;  copies or substantial portions of the Software.
;;
;;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;;  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;;  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;;  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;;  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;;  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;;  SOFTWARE.
;;

%include "asm-calls.inc"

%ifidn __OUTPUT_FORMAT__, win32
    %define Beep_c _Beep_c
    %define CloseHandle_c _CloseHandle_c
    %define CreateDirectoryA_c _CreateDirectoryA_c
    %define CreateFileA_c _CreateFileA_c
    %define CreateMutexA_c _CreateMutexA_c
    %define CreatePipe_c _CreatePipe_c
    %define DeleteCriticalSection_c _DeleteCriticalSection_c
    %define DeleteFileA_c _DeleteFileA_c
    %define EnterCriticalSection_c _EnterCriticalSection_c
    %define ExitProcess_c _ExitProcess_c
    %define FileTimeToLocalFileTime_c _FileTimeToLocalFileTime_c
    %define FileTimeToSystemTime_c _FileTimeToSystemTime_c
    %define FindClose_c _FindClose_c
    %define FindFirstFileA_c _FindFirstFileA_c
    %define FindNextFileA_c _FindNextFileA_c
    %define GetCurrentProcess_c _GetCurrentProcess_c
    %define GetCurrentThread_c _GetCurrentThread_c
    %define GetFullPathNameA_c _GetFullPathNameA_c
    %define GetLastError_c _GetLastError_c
    %define GetPrivateProfileStringA_c _GetPrivateProfileStringA_c
    %define InitializeCriticalSection_c _InitializeCriticalSection_c
    %define LeaveCriticalSection_c _LeaveCriticalSection_c
    %define QueryPerformanceCounter_c _QueryPerformanceCounter_c
    %define QueryPerformanceFrequency_c _QueryPerformanceFrequency_c
    %define ReadFile_c _ReadFile_c
    %define SetErrorMode_c _SetErrorMode_c
    %define SetFilePointer_c _SetFilePointer_c
    %define SetPriorityClass_c _SetPriorityClass_c
    %define SetThreadPriority_c _SetThreadPriority_c
    %define Sleep_c _Sleep_c
    %define WriteFile_c _WriteFile_c
%endif

extern Beep_c
extern CloseHandle_c
extern CreateDirectoryA_c
extern CreateFileA_c
extern CreateMutexA_c
extern CreatePipe_c
extern DeleteCriticalSection_c
extern DeleteFileA_c
extern EnterCriticalSection_c
extern ExitProcess_c
extern FileTimeToLocalFileTime_c
extern FileTimeToSystemTime_c
extern FindClose_c
extern FindFirstFileA_c
extern FindNextFileA_c
extern GetCurrentProcess_c
extern GetCurrentThread_c
extern GetFullPathNameA_c
extern GetLastError_c
extern GetPrivateProfileStringA_c
extern InitializeCriticalSection_c
extern LeaveCriticalSection_c
extern QueryPerformanceCounter_c
extern QueryPerformanceFrequency_c
extern ReadFile_c
extern SetErrorMode_c
extern SetFilePointer_c
extern SetPriorityClass_c
extern SetThreadPriority_c
extern Sleep_c
extern WriteFile_c

global Beep_asm2c
global CloseHandle_asm2c
global CreateDirectoryA_asm2c
global CreateFileA_asm2c
global CreateMutexA_asm2c
global CreatePipe_asm2c
global DeleteCriticalSection_asm2c
global DeleteFileA_asm2c
global EnterCriticalSection_asm2c
global ExitProcess_asm2c
global FileTimeToLocalFileTime_asm2c
global FileTimeToSystemTime_asm2c
global FindClose_asm2c
global FindFirstFileA_asm2c
global FindNextFileA_asm2c
global GetCurrentProcess_asm2c
global GetCurrentThread_asm2c
global GetFullPathNameA_asm2c
global GetLastError_asm2c
global GetPrivateProfileStringA_asm2c
global InitializeCriticalSection_asm2c
global LeaveCriticalSection_asm2c
global QueryPerformanceCounter_asm2c
global QueryPerformanceFrequency_asm2c
global ReadFile_asm2c
global SetErrorMode_asm2c
global SetFilePointer_asm2c
global SetPriorityClass_asm2c
global SetThreadPriority_asm2c
global Sleep_asm2c
global WriteFile_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
Beep_asm2c:

; [esp + 2*4] = DWORD dwDuration
; [esp +   4] = DWORD dwFreq
; [esp      ] = return address

        Call_Asm_Stack2 Beep_c

        retn 2*4

; end procedure Beep_asm2c


align 16
CloseHandle_asm2c:

; [esp +   4] = HANDLE hObject
; [esp      ] = return address

        Call_Asm_Stack1 CloseHandle_c

        retn 4

; end procedure CloseHandle_asm2c


align 16
CreateDirectoryA_asm2c:

; [esp + 2*4] = LPSECURITY_ATTRIBUTES lpSecurityAttributes
; [esp +   4] = LPCSTR                lpPathName
; [esp      ] = return address

        Call_Asm_Stack2 CreateDirectoryA_c

        retn 2*4

; end procedure CreateDirectoryA_asm2c


align 16
CreateFileA_asm2c:

; [esp + 7*4] = HANDLE                hTemplateFile
; [esp + 6*4] = DWORD                 dwFlagsAndAttributes
; [esp + 5*4] = DWORD                 dwCreationDisposition
; [esp + 4*4] = LPSECURITY_ATTRIBUTES lpSecurityAttributes
; [esp + 3*4] = DWORD                 dwShareMode
; [esp + 2*4] = DWORD                 dwDesiredAccess
; [esp +   4] = LPCSTR                lpFileName
; [esp      ] = return address

        Call_Asm_Stack7 CreateFileA_c

        retn 7*4

; end procedure CreateFileA_asm2c


align 16
CreateMutexA_asm2c:

; [esp + 3*4] = LPCSTR                lpName
; [esp + 2*4] = BOOL                  bInitialOwner
; [esp +   4] = LPSECURITY_ATTRIBUTES lpMutexAttributes
; [esp      ] = return address

        Call_Asm_Stack3 CreateMutexA_c

        retn 3*4

; end procedure CreateMutexA_asm2c


align 16
CreatePipe_asm2c:

; [esp + 4*4] = LPCSTR lpString
; [esp + 3*4] = int    y
; [esp + 2*4] = int    x
; [esp +   4] = HDC    hdc
; [esp      ] = return address

        Call_Asm_Stack4 CreatePipe_c

        retn 4*4

; end procedure CreatePipe_asm2c


align 16
DeleteCriticalSection_asm2c:

; [esp +   4] = LPCRITICAL_SECTION lpCriticalSection
; [esp      ] = return address

        Call_Asm_Stack1 DeleteCriticalSection_c

        retn 4

; end procedure DeleteCriticalSection_asm2c


align 16
DeleteFileA_asm2c:

; [esp +   4] = LPCSTR lpFileName
; [esp      ] = return address

        Call_Asm_Stack1 DeleteFileA_c

        retn 4

; end procedure DeleteFileA_asm2c


align 16
EnterCriticalSection_asm2c:

; [esp +   4] = HDC   hdc
; [esp      ] = return address

        Call_Asm_Stack1 EnterCriticalSection_c

        retn 4

; end procedure EnterCriticalSection_asm2c


align 16
ExitProcess_asm2c:

; [esp +   4] = UINT uExitCode
; [esp      ] = return address

        Call_Asm_Stack1 ExitProcess_c

        retn 4

; end procedure ExitProcess_asm2c


align 16
FileTimeToLocalFileTime_asm2c:

; [esp + 2*4] = LPFILETIME     lpLocalFileTime
; [esp +   4] = const FILETIME *lpFileTime
; [esp      ] = return address

        Call_Asm_Stack2 FileTimeToLocalFileTime_c

        retn 2*4

; end procedure FileTimeToLocalFileTime_asm2c


align 16
FileTimeToSystemTime_asm2c:

; [esp + 2*4] = LPSYSTEMTIME   lpSystemTime
; [esp +   4] = const FILETIME *lpFileTime
; [esp      ] = return address

        Call_Asm_Stack2 FileTimeToSystemTime_c

        retn 2*4

; end procedure FileTimeToSystemTime_asm2c


align 16
FindClose_asm2c:

; [esp +   4] = HANDLE hFindFile
; [esp      ] = return address

        Call_Asm_Stack1 FindClose_c

        retn 4

; end procedure FindClose_asm2c


align 16
FindFirstFileA_asm2c:

; [esp + 2*4] = LPWIN32_FIND_DATAA lpFindFileData
; [esp +   4] = LPCSTR             lpFileName
; [esp      ] = return address

        Call_Asm_Stack2 FindFirstFileA_c

        retn 2*4

; end procedure FindFirstFileA_asm2c


align 16
FindNextFileA_asm2c:

; [esp + 2*4] = LPWIN32_FIND_DATAA lpFindFileData
; [esp +   4] = HANDLE             hFindFile
; [esp      ] = return address

        Call_Asm_Stack2 FindNextFileA_c

        retn 2*4

; end procedure FindNextFileA_asm2c


align 16
GetCurrentProcess_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 GetCurrentProcess_c

        retn

; end procedure GetCurrentProcess_asm2c


align 16
GetCurrentThread_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 GetCurrentThread_c

        retn

; end procedure GetCurrentThread_asm2c


align 16
GetFullPathNameA_asm2c:

; [esp + 4*4] = LPSTR  *lpFilePart
; [esp + 3*4] = LPSTR  lpBuffer
; [esp + 2*4] = DWORD  nBufferLength
; [esp +   4] = LPCSTR lpFileName
; [esp      ] = return address

        Call_Asm_Stack4 GetFullPathNameA_c

        retn 4*4

; end procedure GetFullPathNameA_asm2c


align 16
GetLastError_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 GetLastError_c

        retn

; end procedure GetLastError_asm2c


align 16
GetPrivateProfileStringA_asm2c:

; [esp + 6*4] = HDC   hdcSrc
; [esp + 5*4] = int   cy
; [esp + 4*4] = int   cx
; [esp + 3*4] = int   y
; [esp + 2*4] = int   x
; [esp +   4] = HDC   hdc
; [esp      ] = return address

        Call_Asm_Stack6 GetPrivateProfileStringA_c

        retn 6*4

; end procedure GetPrivateProfileStringA_asm2c


align 16
InitializeCriticalSection_asm2c:

; [esp +   4] = LPCRITICAL_SECTION lpCriticalSection
; [esp      ] = return address

        Call_Asm_Stack1 InitializeCriticalSection_c

        retn 4

; end procedure InitializeCriticalSection_asm2c


align 16
LeaveCriticalSection_asm2c:

; [esp +   4] = LPCRITICAL_SECTION lpCriticalSection
; [esp      ] = return address

        Call_Asm_Stack1 LeaveCriticalSection_c

        retn 4

; end procedure LeaveCriticalSection_asm2c


align 16
QueryPerformanceCounter_asm2c:

; [esp +   4] = LARGE_INTEGER *lpPerformanceCount
; [esp      ] = return address

        Call_Asm_Stack1 QueryPerformanceCounter_c

        retn 4

; end procedure QueryPerformanceCounter_asm2c


align 16
QueryPerformanceFrequency_asm2c:

; [esp +   4] = LARGE_INTEGER *lpFrequency
; [esp      ] = return address

        Call_Asm_Stack1 QueryPerformanceFrequency_c

        retn 4

; end procedure QueryPerformanceFrequency_asm2c


align 16
ReadFile_asm2c:

; [esp + 5*4] = LPOVERLAPPED lpOverlapped
; [esp + 4*4] = LPDWORD      lpNumberOfBytesRead
; [esp + 3*4] = DWORD        nNumberOfBytesToRead
; [esp + 2*4] = LPVOID       lpBuffer
; [esp +   4] = HANDLE       hFile
; [esp      ] = return address

        Call_Asm_Stack5 ReadFile_c

        retn 5*4

; end procedure ReadFile_asm2c


align 16
SetErrorMode_asm2c:

; [esp +   4] = UINT uMode
; [esp      ] = return address

        Call_Asm_Stack1 SetErrorMode_c

        retn 4

; end procedure SetErrorMode_asm2c


align 16
SetFilePointer_asm2c:

; [esp + 4*4] = DWORD  dwMoveMethod
; [esp + 3*4] = PLONG  lpDistanceToMoveHigh
; [esp + 2*4] = LONG   lDistanceToMove
; [esp +   4] = HANDLE hFile
; [esp      ] = return address

        Call_Asm_Stack4 SetFilePointer_c

        retn 4*4

; end procedure SetFilePointer_asm2c


align 16
SetPriorityClass_asm2c:

; [esp + 2*4] = DWORD  dwPriorityClass
; [esp +   4] = HANDLE hProcess
; [esp      ] = return address

        Call_Asm_Stack2 SetPriorityClass_c

        retn 2*4

; end procedure SetPriorityClass_asm2c


align 16
SetThreadPriority_asm2c:

; [esp + 2*4] = int    nPriority
; [esp +   4] = HANDLE hThread
; [esp      ] = return address

        Call_Asm_Stack2 SetThreadPriority_c

        retn 2*4

; end procedure SetThreadPriority_asm2c


align 16
Sleep_asm2c:

; [esp +   4] = DWORD dwMilliseconds
; [esp      ] = return address

        Call_Asm_Stack1 Sleep_c

        retn 4

; end procedure Sleep_asm2c


align 16
WriteFile_asm2c:

; [esp + 5*4] = LPOVERLAPPED lpOverlapped
; [esp + 4*4] = LPDWORD      lpNumberOfBytesWritten
; [esp + 3*4] = DWORD        nNumberOfBytesToWrite
; [esp + 2*4] = LPCVOID      lpBuffer
; [esp +   4] = HANDLE       hFile
; [esp      ] = return address

        Call_Asm_Stack5 WriteFile_c

        retn 5*4

; end procedure WriteFile_asm2c

