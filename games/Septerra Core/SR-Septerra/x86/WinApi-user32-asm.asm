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
    %define AdjustWindowRectEx_c _AdjustWindowRectEx_c
    %define ClipCursor_c _ClipCursor_c
    %define CreateWindowExA_c _CreateWindowExA_c
    %define DefWindowProcA_c _DefWindowProcA_c
    %define DestroyWindow_c _DestroyWindow_c
    %define DispatchMessageA_c _DispatchMessageA_c
    %define GetAsyncKeyState_c _GetAsyncKeyState_c
    %define GetCursorPos_c _GetCursorPos_c
    %define GetKeyState_c _GetKeyState_c
    %define GetMessageA_c _GetMessageA_c
    %define GetMessagePos_c _GetMessagePos_c
    %define GetMessageTime_c _GetMessageTime_c
    %define GetSystemMetrics_c _GetSystemMetrics_c
    %define LoadCursorA_c _LoadCursorA_c
    %define LoadIconA_c _LoadIconA_c
    %define LoadImageA_c _LoadImageA_c
    %define MessageBoxA_c _MessageBoxA_c
    %define OffsetRect_c _OffsetRect_c
    %define PeekMessageA_c _PeekMessageA_c
    %define PostMessageA_c _PostMessageA_c
    %define PostQuitMessage_c _PostQuitMessage_c
    %define PtInRect_c _PtInRect_c
    %define RegisterClassA_c _RegisterClassA_c
    %define SetCursor_c _SetCursor_c
    %define SetCursorPos_c _SetCursorPos_c
    %define SetFocus_c _SetFocus_c
    %define ShowCursor_c _ShowCursor_c
    %define ShowWindow_c _ShowWindow_c
    %define TranslateMessage_c _TranslateMessage_c
    %define UpdateWindow_c _UpdateWindow_c
    %define ValidateRect_c _ValidateRect_c
    %define WaitMessage_c _WaitMessage_c
%endif

extern AdjustWindowRectEx_c
extern ClipCursor_c
extern CreateWindowExA_c
extern DefWindowProcA_c
extern DestroyWindow_c
extern DispatchMessageA_c
extern GetAsyncKeyState_c
extern GetCursorPos_c
extern GetKeyState_c
extern GetMessageA_c
extern GetMessagePos_c
extern GetMessageTime_c
extern GetSystemMetrics_c
extern LoadCursorA_c
extern LoadIconA_c
extern LoadImageA_c
extern MessageBoxA_c
extern OffsetRect_c
extern PeekMessageA_c
extern PostMessageA_c
extern PostQuitMessage_c
extern PtInRect_c
extern RegisterClassA_c
extern SetCursor_c
extern SetCursorPos_c
extern SetFocus_c
extern ShowCursor_c
extern ShowWindow_c
extern TranslateMessage_c
extern UpdateWindow_c
extern ValidateRect_c
extern WaitMessage_c

global RunWndProc_asm
global _RunWndProc_asm

global AdjustWindowRectEx_asm2c
global ClipCursor_asm2c
global CreateWindowExA_asm2c
global DefWindowProcA_asm2c
global DestroyWindow_asm2c
global DispatchMessageA_asm2c
global GetAsyncKeyState_asm2c
global GetCursorPos_asm2c
global GetKeyState_asm2c
global GetMessageA_asm2c
global GetMessagePos_asm2c
global GetMessageTime_asm2c
global GetSystemMetrics_asm2c
global LoadCursorA_asm2c
global LoadIconA_asm2c
global LoadImageA_asm2c
global MessageBoxA_asm2c
global OffsetRect_asm2c
global PeekMessageA_asm2c
global PostMessageA_asm2c
global PostQuitMessage_asm2c
global PtInRect_asm2c
global RegisterClassA_asm2c
global SetCursor_asm2c
global SetCursorPos_asm2c
global SetFocus_asm2c
global ShowCursor_asm2c
global ShowWindow_asm2c
global TranslateMessage_asm2c
global UpdateWindow_asm2c
global ValidateRect_asm2c
global WaitMessage_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
RunWndProc_asm:
_RunWndProc_asm:

; [esp + 5*4] = uint32_t (*WndProc)(void *, uint32_t, uint32_t, uint32_t)
; [esp + 4*4] = uint32_t lParam
; [esp + 3*4] = uint32_t wParam
; [esp + 2*4] = uint32_t uMsg
; [esp +   4] = void *hwnd
; [esp      ] = return address

        push dword [esp + 4*4]
        push dword [esp + 4*4]
        push dword [esp + 4*4]
        push dword [esp + 4*4]

        call dword [esp + 9*4]

        retn

; end procedure RunWndProc_asm


align 16
AdjustWindowRectEx_asm2c:

; [esp + 4*4] = DWORD  dwExStyle
; [esp + 3*4] = BOOL   bMenu
; [esp + 2*4] = DWORD  dwStyle
; [esp +   4] = LPRECT lpRect
; [esp      ] = return address

        Call_Asm_Stack4 AdjustWindowRectEx_c

        retn 4*4

; end procedure AdjustWindowRectEx_asm2c


align 16
ClipCursor_asm2c:

; [esp +   4] = const RECT *lpRect
; [esp      ] = return address

        Call_Asm_Stack1 ClipCursor_c

        retn 4

; end procedure ClipCursor_asm2c


align 16
CreateWindowExA_asm2c:

; [esp + 12*4] = LPVOID    lpParam
; [esp + 11*4] = HINSTANCE hInstance
; [esp + 10*4] = HMENU     hMenu
; [esp +  9*4] = HWND      hWndParent
; [esp +  8*4] = int       nHeight
; [esp +  7*4] = int       nWidth
; [esp +  6*4] = int       Y
; [esp +  5*4] = int       X
; [esp +  4*4] = DWORD     dwStyle
; [esp +  3*4] = LPCSTR    lpWindowName
; [esp +  2*4] = LPCSTR    lpClassName
; [esp +    4] = DWORD     dwExStyle
; [esp       ] = return address

        Call_Asm_Stack12 CreateWindowExA_c

        retn 12*4

; end procedure CreateWindowExA_asm2c


align 16
DefWindowProcA_asm2c:

; [esp + 4*4] = LPARAM lParam
; [esp + 3*4] = WPARAM wParam
; [esp + 2*4] = UINT   Msg
; [esp +   4] = HWND   hWnd
; [esp      ] = return address

        Call_Asm_Stack4 DefWindowProcA_c

        retn 4*4

; end procedure DefWindowProcA_asm2c


align 16
DestroyWindow_asm2c:

; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack1 DestroyWindow_c

        retn 4

; end procedure DestroyWindow_asm2c


align 16
DispatchMessageA_asm2c:

; [esp +   4] = const MSG *lpMsg
; [esp      ] = return address

        Call_Asm_Stack1 DispatchMessageA_c

        retn 4

; end procedure DispatchMessageA_asm2c


align 16
GetAsyncKeyState_asm2c:

; [esp +   4] = int vKey
; [esp      ] = return address

        Call_Asm_Stack1 GetAsyncKeyState_c

        retn 4

; end procedure GetAsyncKeyState_asm2c


align 16
GetCursorPos_asm2c:

; [esp +   4] = LPPOINT lpPoint
; [esp      ] = return address

        Call_Asm_Stack1 GetCursorPos_c

        retn 4

; end procedure GetCursorPos_asm2c


align 16
GetKeyState_asm2c:

; [esp +   4] = int nVirtKey
; [esp      ] = return address

        Call_Asm_Stack1 GetKeyState_c

        retn 4

; end procedure GetKeyState_asm2c


align 16
GetMessageA_asm2c:

; [esp + 4*4] = UINT  wMsgFilterMax
; [esp + 3*4] = UINT  wMsgFilterMin
; [esp + 2*4] = HWND  hWnd
; [esp +   4] = LPMSG lpMsg
; [esp      ] = return address

        Call_Asm_Stack4 GetMessageA_c

        retn 4*4

; end procedure GetMessageA_asm2c


align 16
GetMessagePos_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 GetMessagePos_c

        retn

; end procedure GetMessagePos_asm2c


align 16
GetMessageTime_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 GetMessageTime_c

        retn

; end procedure GetMessageTime_asm2c


align 16
GetSystemMetrics_asm2c:

; [esp +   4] = int nIndex
; [esp      ] = return address

        Call_Asm_Stack1 GetSystemMetrics_c

        retn 4

; end procedure GetSystemMetrics_asm2c


align 16
LoadCursorA_asm2c:

; [esp + 2*4] = LPCSTR    lpCursorName
; [esp +   4] = HINSTANCE hInstance
; [esp      ] = return address

        Call_Asm_Stack2 LoadCursorA_c

        retn 2*4

; end procedure LoadCursorA_asm2c


align 16
LoadIconA_asm2c:

; [esp + 2*4] = LPCSTR    lpIconName
; [esp +   4] = HINSTANCE hInstance
; [esp      ] = return address

        Call_Asm_Stack2 LoadIconA_c

        retn 2*4

; end procedure LoadIconA_asm2c


align 16
LoadImageA_asm2c:

; [esp + 6*4] = UINT      fuLoad
; [esp + 5*4] = int       cy
; [esp + 4*4] = int       cx
; [esp + 3*4] = UINT      type
; [esp + 2*4] = LPCSTR    name
; [esp +   4] = HINSTANCE hInst
; [esp      ] = return address

        Call_Asm_Stack6 LoadImageA_c

        retn 6*4

; end procedure LoadImageA_asm2c


align 16
MessageBoxA_asm2c:

; [esp + 4*4] = UINT   uType
; [esp + 3*4] = LPCSTR lpCaption
; [esp + 2*4] = LPCSTR lpText
; [esp +   4] = HWND   hWnd
; [esp      ] = return address

        Call_Asm_Stack4 MessageBoxA_c

        retn 4*4

; end procedure MessageBoxA_asm2c


align 16
OffsetRect_asm2c:

; [esp + 3*4] = int    dy
; [esp + 2*4] = int    dx
; [esp +   4] = LPRECT lprc
; [esp      ] = return address

        Call_Asm_Stack3 OffsetRect_c

        retn 3*4

; end procedure OffsetRect_asm2c


align 16
PeekMessageA_asm2c:

; [esp + 5*4] = UINT  wRemoveMsg
; [esp + 4*4] = UINT  wMsgFilterMax
; [esp + 3*4] = UINT  wMsgFilterMin
; [esp + 2*4] = HWND  hWnd
; [esp +   4] = LPMSG lpMsg
; [esp      ] = return address

        Call_Asm_Stack5 PeekMessageA_c

        retn 5*4

; end procedure PeekMessageA_asm2c


align 16
PostMessageA_asm2c:

; [esp + 4*4] = LPARAM lParam
; [esp + 3*4] = WPARAM wParam
; [esp + 2*4] = UINT   Msg
; [esp +   4] = HWND   hWnd
; [esp      ] = return address

        Call_Asm_Stack4 PostMessageA_c

        retn 4*4

; end procedure PostMessageA_asm2c


align 16
PostQuitMessage_asm2c:

; [esp +   4] = int nExitCode
; [esp      ] = return address

        Call_Asm_Stack1 PostQuitMessage_c

        retn 4

; end procedure PostQuitMessage_asm2c


align 16
PtInRect_asm2c:

; [esp + 3*4] = POINT      pt (.y)
; [esp + 2*4] = POINT      pt (.x)
; [esp +   4] = const RECT *lprc
; [esp      ] = return address

        Call_Asm_Stack3 PtInRect_c

        retn 3*4

; end procedure PtInRect_asm2c


align 16
RegisterClassA_asm2c:

; [esp +   4] = const WNDCLASSA *lpWndClass
; [esp      ] = return address

        Call_Asm_Stack1 RegisterClassA_c

        retn 4

; end procedure RegisterClassA_asm2c


align 16
SetCursor_asm2c:

; [esp +   4] = HCURSOR hCursor
; [esp      ] = return address

        Call_Asm_Stack1 SetCursor_c

        retn 4

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
SetFocus_asm2c:

; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack1 SetFocus_c

        retn 4

; end procedure SetFocus_asm2c


align 16
ShowCursor_asm2c:

; [esp +   4] = BOOL bShow
; [esp      ] = return address

        Call_Asm_Stack1 ShowCursor_c

        retn 4

; end procedure ShowCursor_asm2c


align 16
ShowWindow_asm2c:

; [esp + 2*4] = int  nCmdShow
; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack2 ShowWindow_c

        retn 2*4

; end procedure ShowWindow_asm2c


align 16
TranslateMessage_asm2c:

; [esp +   4] = const MSG *lpMsg
; [esp      ] = return address

        Call_Asm_Stack1 TranslateMessage_c

        retn 4

; end procedure TranslateMessage_asm2c


align 16
UpdateWindow_asm2c:

; [esp +   4] = HWND hWnd
; [esp      ] = return address

        Call_Asm_Stack1 UpdateWindow_c

        retn 4

; end procedure UpdateWindow_asm2c


align 16
ValidateRect_asm2c:

; [esp + 2*4] = const RECT *lpRect
; [esp +   4] = HWND       hWnd
; [esp      ] = return address

        Call_Asm_Stack2 ValidateRect_c

        retn 2*4

; end procedure ValidateRect_asm2c


align 16
WaitMessage_asm2c:

; [esp      ] = return address

        Call_Asm_Stack0 WaitMessage_c

        retn

; end procedure WaitMessage_asm2c

