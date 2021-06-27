;;
;;  Copyright (C) 2021 Roman Pauer
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
    %define CloseHandle_c _CloseHandle_c
    %define CreateFileA_c _CreateFileA_c
    %define FindClose_c _FindClose_c
    %define FindFirstFileA_c _FindFirstFileA_c
    %define FindNextFileA_c _FindNextFileA_c
    %define GetCurrentDirectoryA_c _GetCurrentDirectoryA_c
    %define GetPrivateProfileIntA_c _GetPrivateProfileIntA_c
    %define GetPrivateProfileStringA_c _GetPrivateProfileStringA_c
    %define GetTickCount_c _GetTickCount_c
    %define GlobalMemoryStatus_c _GlobalMemoryStatus_c
    %define ReadFile_c _ReadFile_c
    %define SetCurrentDirectoryA_c _SetCurrentDirectoryA_c
    %define SetFilePointer_c _SetFilePointer_c
    %define WriteFile_c _WriteFile_c
    %define WritePrivateProfileStringA_c _WritePrivateProfileStringA_c
%endif

extern CloseHandle_c
extern CreateFileA_c
extern FindClose_c
extern FindFirstFileA_c
extern FindNextFileA_c
extern GetCurrentDirectoryA_c
extern GetPrivateProfileIntA_c
extern GetPrivateProfileStringA_c
extern GetTickCount_c
extern GlobalMemoryStatus_c
extern ReadFile_c
extern SetCurrentDirectoryA_c
extern SetFilePointer_c
extern WriteFile_c
extern WritePrivateProfileStringA_c

global CloseHandle_asm2c
global CreateFileA_asm2c
global FindClose_asm2c
global FindFirstFileA_asm2c
global FindNextFileA_asm2c
global GetCurrentDirectoryA_asm2c
global GetPrivateProfileIntA_asm2c
global GetPrivateProfileStringA_asm2c
global GetTickCount_asm2c
global GlobalMemoryStatus_asm2c
global ReadFile_asm2c
global SetCurrentDirectoryA_asm2c
global SetFilePointer_asm2c
global WriteFile_asm2c
global WritePrivateProfileStringA_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
CloseHandle_asm2c:

; [esp +   4] = HANDLE hObject
; [esp      ] = return address

        Call_Asm_Stack1 CloseHandle_c

        retn 1*4

; end procedure CloseHandle_asm2c


align 16
CreateFileA_asm2c:

; [esp + 7*4] = HANDLE hTemplateFile
; [esp + 6*4] = DWORD dwFlagsAndAttributes
; [esp + 5*4] = DWORD dwCreationDisposition
; [esp + 4*4] = LPSECURITY_ATTRIBUTES lpSecurityAttributes
; [esp + 3*4] = DWORD dwShareMode
; [esp + 2*4] = DWORD dwDesiredAccess
; [esp +   4] = LPCSTR lpFileName
; [esp      ] = return address

        Call_Asm_Stack7 CreateFileA_c

        retn 7*4

; end procedure CreateFileA_asm2c


align 16
FindClose_asm2c:

; [esp +   4] = HANDLE hFindFile
; [esp      ] = return address

        Call_Asm_Stack1 FindClose_c

        retn 1*4

; end procedure FindClose_asm2c


align 16
FindFirstFileA_asm2c:

; [esp + 2*4] = LPWIN32_FIND_DATAA lpFindFileData
; [esp +   4] = LPCSTR lpFileName
; [esp      ] = return address

        Call_Asm_Stack2 FindFirstFileA_c

        retn 2*4

; end procedure FindFirstFileA_asm2c


align 16
FindNextFileA_asm2c:

; [esp + 2*4] = LPWIN32_FIND_DATAA lpFindFileData
; [esp +   4] = HANDLE hFindFile
; [esp      ] = return address

        Call_Asm_Stack2 FindNextFileA_c

        retn 2*4

; end procedure FindNextFileA_asm2c


align 16
GetCurrentDirectoryA_asm2c:

; [esp + 2*4] = LPSTR lpBuffer
; [esp +   4] = DWORD nBufferLength
; [esp      ] = return address

        Call_Asm_Stack2 GetCurrentDirectoryA_c

        retn 2*4

; end procedure GetCurrentDirectoryA_asm2c


align 16
GetPrivateProfileIntA_asm2c:

; [esp + 4*4] = LPCSTR lpFileName
; [esp + 3*4] = INT nDefault
; [esp + 2*4] = LPCSTR lpKeyName
; [esp +   4] = LPCSTR lpAppName
; [esp      ] = return address

        Call_Asm_Stack4 GetPrivateProfileIntA_c

        retn 4*4

; end procedure GetPrivateProfileIntA_asm2c


align 16
GetPrivateProfileStringA_asm2c:

; [esp + 6*4] = LPCSTR lpFileName
; [esp + 5*4] = DWORD nSize
; [esp + 4*4] = LPSTR lpReturnedString
; [esp + 3*4] = LPCSTR lpDefault
; [esp + 2*4] = LPCSTR lpKeyName
; [esp +   4] = LPCSTR lpAppName
; [esp      ] = return address

        Call_Asm_Stack6 GetPrivateProfileStringA_c

        retn 6*4

; end procedure GetPrivateProfileStringA_asm2c


align 16
GetTickCount_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 GetTickCount_c

        retn

; end procedure GetTickCount_asm2c


align 16
GlobalMemoryStatus_asm2c:

; [esp +   4] = LPMEMORYSTATUS lpBuffer
; [esp      ] = return address

        Call_Asm_Stack1 GlobalMemoryStatus_c

        retn 1*4

; end procedure GlobalMemoryStatus_asm2c


align 16
ReadFile_asm2c:

; [esp + 5*4] = LPOVERLAPPED lpOverlapped
; [esp + 4*4] = LPDWORD lpNumberOfBytesRead
; [esp + 3*4] = DWORD nNumberOfBytesToRead
; [esp + 2*4] = LPVOID lpBuffer
; [esp +   4] = HANDLE hFile
; [esp      ] = return address

        Call_Asm_Stack5 ReadFile_c

        retn 5*4

; end procedure ReadFile_asm2c


align 16
SetCurrentDirectoryA_asm2c:

; [esp +   4] = LPCSTR lpPathName
; [esp      ] = return address

        Call_Asm_Stack1 SetCurrentDirectoryA_c

        retn 1*4

; end procedure SetCurrentDirectoryA_asm2c


align 16
SetFilePointer_asm2c:

; [esp + 4*4] = DWORD dwMoveMethod
; [esp + 3*4] = PLONG lpDistanceToMoveHigh
; [esp + 2*4] = LONG lDistanceToMove
; [esp +   4] = HANDLE hFile
; [esp      ] = return address

        Call_Asm_Stack4 SetFilePointer_c

        retn 4*4

; end procedure SetFilePointer_asm2c


align 16
WriteFile_asm2c:

; [esp + 5*4] = LPOVERLAPPED lpOverlapped
; [esp + 4*4] = LPDWORD lpNumberOfBytesWritten
; [esp + 3*4] = DWORD nNumberOfBytesToWrite
; [esp + 2*4] = LPCVOID lpBuffer
; [esp +   4] = HANDLE hFile
; [esp      ] = return address

        Call_Asm_Stack5 WriteFile_c

        retn 5*4

; end procedure WriteFile_asm2c


align 16
WritePrivateProfileStringA_asm2c:

; [esp + 4*4] = LPCSTR lpFileName
; [esp + 3*4] = LPCSTR lpString
; [esp + 2*4] = LPCSTR lpKeyName
; [esp +   4] = LPCSTR lpAppName
; [esp      ] = return address

        Call_Asm_Stack4 WritePrivateProfileStringA_c

        retn 4*4

; end procedure WritePrivateProfileStringA_asm2c


