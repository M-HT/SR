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

#if !defined(_WINAPI_USER32_H_INCLUDED_)
#define _WINAPI_USER32_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t ClientToScreen_c(void * hWnd, void * lpPoint);
uint32_t ClipCursor_c(const void * lpRect);
void * CreateWindowExA_c(uint32_t dwExStyle, const char * lpClassName, const char * lpWindowName, uint32_t dwStyle, int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, void * hWndParent, void * hMenu, void * hInstance, void * lpParam);
void * GetActiveWindow_c(void);
uint32_t GetCursorPos_c(void * lpPoint);
void * GetDC_c(void * hWnd);
uint32_t GetWindowRect_c(void * hWnd, void * lpRect);
uint32_t IsIconic_c(void * hWnd);
uint32_t IsWindowVisible_c(void * hWnd);
void * LoadCursorA_c(void * hInstance, const char * lpCursorName);
int32_t LoadStringA_c(void * hInstance, uint32_t uID, char * lpBuffer, int32_t cchBufferMax);
int32_t MessageBoxA_c(void * hWnd, const char * lpText, const char * lpCaption, uint32_t uType);
uint32_t MoveWindow_c(void * hWnd, int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, uint32_t bRepaint);
uint32_t ReleaseCapture_c(void);
int32_t ReleaseDC_c(void * hWnd, void * hDC);
uint32_t ScreenToClient_c(void * hWnd, void * lpPoint);
int32_t SendMessageA_c(void * hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam);
void * SetActiveWindow_c(void * hWnd);
void * SetCursor_c(void * hCursor);
uint32_t SetCursorPos_c(int32_t X, int32_t Y);
uint32_t SetRect_c(void * lprc, int32_t xLeft, int32_t yTop, int32_t xRight, int32_t yBottom);
uint32_t SetRectEmpty_c(void * lprc);
uint32_t SetWindowPos_c(void * hWnd, void * hWndInsertAfter, int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags);
int32_t ShowCursor_c(uint32_t bShow);
uint32_t ShowWindow_c(void * hWnd, int32_t nCmdShow);
uint32_t UnregisterClassA_c(const char * lpClassName, void * hInstance);

#ifdef __cplusplus
}
#endif

#endif

