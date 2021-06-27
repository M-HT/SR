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
    %define ClientToScreen_c _ClientToScreen_c
    %define ClipCursor_c _ClipCursor_c
    %define CreateWindowExA_c _CreateWindowExA_c
    %define GetActiveWindow_c _GetActiveWindow_c
    %define GetCursorPos_c _GetCursorPos_c
    %define GetDC_c _GetDC_c
    %define GetWindowRect_c _GetWindowRect_c
    %define IsIconic_c _IsIconic_c
    %define IsWindowVisible_c _IsWindowVisible_c
    %define LoadCursorA_c _LoadCursorA_c
    %define LoadStringA_c _LoadStringA_c
    %define MessageBoxA_c _MessageBoxA_c
    %define MoveWindow_c _MoveWindow_c
    %define ReleaseCapture_c _ReleaseCapture_c
    %define ReleaseDC_c _ReleaseDC_c
    %define ScreenToClient_c _ScreenToClient_c
    %define SendMessageA_c _SendMessageA_c
    %define SetActiveWindow_c _SetActiveWindow_c
    %define SetCursor_c _SetCursor_c
    %define SetCursorPos_c _SetCursorPos_c
    %define SetRect_c _SetRect_c
    %define SetRectEmpty_c _SetRectEmpty_c
    %define SetWindowPos_c _SetWindowPos_c
    %define ShowCursor_c _ShowCursor_c
    %define ShowWindow_c _ShowWindow_c
    %define UnregisterClassA_c _UnregisterClassA_c
    %define wsprintfA2_c _wsprintfA2_c
%endif

extern ClientToScreen_c
extern ClipCursor_c
extern CreateWindowExA_c
extern GetActiveWindow_c
extern GetCursorPos_c
extern GetDC_c
extern GetWindowRect_c
extern IsIconic_c
extern IsWindowVisible_c
extern LoadCursorA_c
extern LoadStringA_c
extern MessageBoxA_c
extern MoveWindow_c
extern ReleaseCapture_c
extern ReleaseDC_c
extern ScreenToClient_c
extern SendMessageA_c
extern SetActiveWindow_c
extern SetCursor_c
extern SetCursorPos_c
extern SetRect_c
extern SetRectEmpty_c
extern SetWindowPos_c
extern ShowCursor_c
extern ShowWindow_c
extern UnregisterClassA_c
extern wsprintfA2_c

global ClientToScreen_asm2c
global ClipCursor_asm2c
global CreateWindowExA_asm2c
global GetActiveWindow_asm2c
global GetCursorPos_asm2c
global GetDC_asm2c
global GetWindowRect_asm2c
global IsIconic_asm2c
global IsWindowVisible_asm2c
global LoadCursorA_asm2c
global LoadStringA_asm2c
global MessageBoxA_asm2c
global MoveWindow_asm2c
global ReleaseCapture_asm2c
global ReleaseDC_asm2c
global ScreenToClient_asm2c
global SendMessageA_asm2c
global SetActiveWindow_asm2c
global SetCursor_asm2c
global SetCursorPos_asm2c
global SetRect_asm2c
global SetRectEmpty_asm2c
global SetWindowPos_asm2c
global ShowCursor_asm2c
global ShowWindow_asm2c
global UnregisterClassA_asm2c
global wsprintfA_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
ClientToScreen_asm2c:

; [esp + 2*4] = LPPOINT lpPoint
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack2 ClientToScreen_c

        retn 2*4

; end procedure ClientToScreen_asm2c


align 16
ClipCursor_asm2c:

; [esp +   4] = CONST RECT * lpRect
; [esp      ] = return address

        Call_Asm_Stack1 ClipCursor_c

        retn 1*4

; end procedure ClipCursor_asm2c


align 16
CreateWindowExA_asm2c:

; [esp + 12*4] = LPVOID lpParam
; [esp + 11*4] = HINSTANCE hInstance
; [esp + 10*4] = HMENU hMenu
; [esp + 9*4] = HWND hWndParent
; [esp + 8*4] = int nHeight
; [esp + 7*4] = int nWidth
; [esp + 6*4] = int Y
; [esp + 5*4] = int X
; [esp + 4*4] = DWORD dwStyle
; [esp + 3*4] = LPCSTR lpWindowName
; [esp + 2*4] = LPCSTR lpClassName
; [esp +   4] = DWORD dwExStyle
; [esp      ] = return address

        Call_Asm_Stack12 CreateWindowExA_c

        retn 12*4

; end procedure CreateWindowExA_asm2c


align 16
GetActiveWindow_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 GetActiveWindow_c

        retn

; end procedure GetActiveWindow_asm2c


align 16
GetCursorPos_asm2c:

; [esp +   4] = LPPOINT lpPoint
; [esp      ] = return address

        Call_Asm_Stack1 GetCursorPos_c

        retn 1*4

; end procedure GetCursorPos_asm2c


align 16
GetDC_asm2c:

; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack1 GetDC_c

        retn 1*4

; end procedure GetDC_asm2c


align 16
GetWindowRect_asm2c:

; [esp + 2*4] = LPRECT lpRect
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack2 GetWindowRect_c

        retn 2*4

; end procedure GetWindowRect_asm2c


align 16
IsIconic_asm2c:

; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack1 IsIconic_c

        retn 1*4

; end procedure IsIconic_asm2c


align 16
IsWindowVisible_asm2c:

; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack1 IsWindowVisible_c

        retn 1*4

; end procedure IsWindowVisible_asm2c


align 16
LoadCursorA_asm2c:

; [esp + 2*4] = LPCSTR lpCursorName
; [esp +   4] = HINSTANCE hInstance
; [esp      ] = return address

        Call_Asm_Stack2 LoadCursorA_c

        retn 2*4

; end procedure LoadCursorA_asm2c


align 16
LoadStringA_asm2c:

; [esp + 4*4] = int cchBufferMax
; [esp + 3*4] = LPSTR lpBuffer
; [esp + 2*4] = UINT uID
; [esp +   4] = HINSTANCE hInstance
; [esp      ] = return address

        Call_Asm_Stack4 LoadStringA_c

        retn 4*4

; end procedure LoadStringA_asm2c


align 16
MessageBoxA_asm2c:

; [esp + 4*4] = UINT uType
; [esp + 3*4] = LPCSTR lpCaption
; [esp + 2*4] = LPCSTR lpText
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack4 MessageBoxA_c

        retn 4*4

; end procedure MessageBoxA_asm2c


align 16
MoveWindow_asm2c:

; [esp + 6*4] = WINBOOL bRepaint
; [esp + 5*4] = int nHeight
; [esp + 4*4] = int nWidth
; [esp + 3*4] = int Y
; [esp + 2*4] = int X
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack6 MoveWindow_c

        retn 6*4

; end procedure MoveWindow_asm2c


align 16
ReleaseCapture_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 ReleaseCapture_c

        retn

; end procedure ReleaseCapture_asm2c


align 16
ReleaseDC_asm2c:

; [esp + 2*4] = HDC hDC
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack2 ReleaseDC_c

        retn 2*4

; end procedure ReleaseDC_asm2c


align 16
ScreenToClient_asm2c:

; [esp + 2*4] = LPPOINT lpPoint
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack2 ScreenToClient_c

        retn 2*4

; end procedure ScreenToClient_asm2c


align 16
SendMessageA_asm2c:

; [esp + 4*4] = LPARAM lParam
; [esp + 3*4] = WPARAM wParam
; [esp + 2*4] = UINT Msg
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack4 SendMessageA_c

        retn 4*4

; end procedure SendMessageA_asm2c


align 16
SetActiveWindow_asm2c:

; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack1 SetActiveWindow_c

        retn 1*4

; end procedure SetActiveWindow_asm2c


align 16
SetCursor_asm2c:

; [esp +   4] = HCURSOR hCursor
; [esp      ] = return address

        Call_Asm_Stack1 SetCursor_c

        retn 1*4

; end procedure SetCursor_asm2c


align 16
SetCursorPos_asm2c:

; [esp + 2*4] = int Y
; [esp +   4] = int X
; [esp      ] = return address

        Call_Asm_Stack2 SetCursorPos_c

        retn 2*4

; end procedure SetCursorPos_asm2c


align 16
SetRect_asm2c:

; [esp + 5*4] = int yBottom
; [esp + 4*4] = int xRight
; [esp + 3*4] = int yTop
; [esp + 2*4] = int xLeft
; [esp +   4] = LPRECT lprc
; [esp      ] = return address

        Call_Asm_Stack5 SetRect_c

        retn 5*4

; end procedure SetRect_asm2c


align 16
SetRectEmpty_asm2c:

; [esp +   4] = LPRECT lprc
; [esp      ] = return address

        Call_Asm_Stack1 SetRectEmpty_c

        retn 1*4

; end procedure SetRectEmpty_asm2c


align 16
SetWindowPos_asm2c:

; [esp + 7*4] = UINT uFlags
; [esp + 6*4] = int cy
; [esp + 5*4] = int cx
; [esp + 4*4] = int Y
; [esp + 3*4] = int X
; [esp + 2*4] = HWND hWndInsertAfter
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack7 SetWindowPos_c

        retn 7*4

; end procedure SetWindowPos_asm2c


align 16
ShowCursor_asm2c:

; [esp +   4] = WINBOOL bShow
; [esp      ] = return address

        Call_Asm_Stack1 ShowCursor_c

        retn 1*4

; end procedure ShowCursor_asm2c


align 16
ShowWindow_asm2c:

; [esp + 2*4] = int nCmdShow
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack2 ShowWindow_c

        retn 2*4

; end procedure ShowWindow_asm2c


align 16
UnregisterClassA_asm2c:

; [esp + 2*4] = HINSTANCE hInstance
; [esp +   4] = LPCSTR lpClassName
; [esp      ] = return address

        Call_Asm_Stack2 UnregisterClassA_c

        retn 2*4

; end procedure UnregisterClassA_asm2c


align 16
wsprintfA_asm2c:

; [esp + 3*4] = ...
; [esp + 2*4] = LPCSTR param2
; [esp +   4] = LPSTR param1
; [esp      ] = return address

        Call_Asm_VariableStack2 wsprintfA2_c

        retn

; end procedure wsprintfA_asm2c


