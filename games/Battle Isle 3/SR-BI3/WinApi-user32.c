/**
 *
 *  Copyright (C) 2021-2026 Roman Pauer
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

#include "WinApi-user32.h"
#include "SDIresource.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "printf_x86.h"

uint32_t CCALL ClientToScreen_c(void * hWnd, void * lpPoint)
{
    return ClientToScreen((HWND)hWnd, (LPPOINT)lpPoint);
}

uint32_t CCALL ClipCursor_c(const void * lpRect)
{
    return ClipCursor((CONST RECT *)lpRect);
}

void * CCALL CreateWindowExA_c(uint32_t dwExStyle, const char * lpClassName, const char * lpWindowName, uint32_t dwStyle, int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, void * hWndParent, void * hMenu, void * hInstance, void * lpParam)
{
    return CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, (HWND)hWndParent, (HMENU)hMenu, (HINSTANCE)hInstance, lpParam);
}

void * CCALL GetActiveWindow_c(void)
{
    return GetActiveWindow();
}

uint32_t CCALL GetCursorPos_c(void * lpPoint)
{
    return GetCursorPos((LPPOINT)lpPoint);
}

void * CCALL GetDC_c(void * hWnd)
{
    return GetDC((HWND)hWnd);
}

uint32_t CCALL GetWindowRect_c(void * hWnd, void * lpRect)
{
    return GetWindowRect((HWND)hWnd, (LPRECT)lpRect);
}

uint32_t CCALL IsIconic_c(void * hWnd)
{
    return IsIconic((HWND)hWnd);
}

uint32_t CCALL IsWindowVisible_c(void * hWnd)
{
    return IsWindowVisible((HWND)hWnd);
}

void * CCALL LoadCursorA_c(void * hInstance, const char * lpCursorName)
{
    return LoadCursorA((HINSTANCE)hInstance, lpCursorName);
}

int32_t CCALL LoadStringA_c(void * hInstance, uint32_t uID, char * lpBuffer, int32_t cchBufferMax)
{
    // change: don't load string from executable resources
    //return LoadStringA((HINSTANCE)hInstance, uID, lpBuffer, cchBufferMax);
    return resource_LoadStringA(uID, lpBuffer, cchBufferMax);
}

int32_t CCALL MessageBoxA_c(void * hWnd, const char * lpText, const char * lpCaption, uint32_t uType)
{
    return MessageBoxA((HWND)hWnd, lpText, lpCaption, uType);
}

uint32_t CCALL MoveWindow_c(void * hWnd, int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, uint32_t bRepaint)
{
    return MoveWindow((HWND)hWnd, X, Y, nWidth, nHeight, bRepaint);
}

uint32_t CCALL ReleaseCapture_c(void)
{
    return ReleaseCapture();
}

int32_t CCALL ReleaseDC_c(void * hWnd, void * hDC)
{
    return ReleaseDC((HWND)hWnd, (HDC)hDC);
}

uint32_t CCALL ScreenToClient_c(void * hWnd, void * lpPoint)
{
    return ScreenToClient((HWND)hWnd, (LPPOINT)lpPoint);
}

int32_t CCALL SendMessageA_c(void * hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam)
{
    return SendMessageA((HWND)hWnd, Msg, wParam, lParam);
}

void * CCALL SetActiveWindow_c(void * hWnd)
{
    return SetActiveWindow((HWND)hWnd);
}

void * CCALL SetCursor_c(void * hCursor)
{
    return SetCursor((HCURSOR)hCursor);
}

uint32_t CCALL SetCursorPos_c(int32_t X, int32_t Y)
{
    return SetCursorPos(X, Y);
}

uint32_t CCALL SetRect_c(void * lprc, int32_t xLeft, int32_t yTop, int32_t xRight, int32_t yBottom)
{
    return SetRect((LPRECT)lprc, xLeft, yTop, xRight, yBottom);
}

uint32_t CCALL SetRectEmpty_c(void * lprc)
{
    return SetRectEmpty((LPRECT)lprc);
}

uint32_t CCALL SetWindowPos_c(void * hWnd, void * hWndInsertAfter, int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags)
{
    return SetWindowPos((HWND)hWnd, (HWND)hWndInsertAfter, X, Y, cx, cy, uFlags);
}

int32_t CCALL ShowCursor_c(uint32_t bShow)
{
    return ShowCursor(bShow);
}

uint32_t CCALL ShowWindow_c(void * hWnd, int32_t nCmdShow)
{
    return ShowWindow((HWND)hWnd, nCmdShow);
}

uint32_t CCALL UnregisterClassA_c(const char * lpClassName, void * hInstance)
{
    return UnregisterClassA(lpClassName, (HINSTANCE)hInstance);
}

int32_t CCALL wsprintfA2_c(char * param1, const char * param2, uint32_t *ap)
{
    return vsprintf_x86(param1, param2, ap);
}

