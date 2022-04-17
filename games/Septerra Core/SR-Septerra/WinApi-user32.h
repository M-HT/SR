/**
 *
 *  Copyright (C) 2019-2022 Roman Pauer
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

#include "ptr32.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t AdjustWindowRectEx_c(void *lpRect, uint32_t dwStyle, uint32_t bMenu, uint32_t dwExStyle);
uint32_t ClipCursor_c(const void *lpRect);
void *CreateWindowExA_c(uint32_t dwExStyle, const char *lpClassName, const char *lpWindowName, uint32_t dwStyle, int32_t x, int32_t y, int32_t nWidth, int32_t nHeight, void *hWndParent, void *hMenu, void *hInstance, void *lpParam);
uint32_t DefWindowProcA_c(void *hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam);
uint32_t DestroyWindow_c(void *hWnd);
uint32_t DispatchMessageA_c(void *lpMsg);
int16_t GetAsyncKeyState_c(int32_t vKey);
uint32_t GetCursorPos_c(void *lpPoint);
int16_t GetKeyState_c(int32_t nVirtKey);
uint32_t GetMessageA_c(void *lpMsg, void *hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax);
uint32_t GetMessagePos_c(void);
uint32_t GetMessageTime_c(void);
int32_t GetSystemMetrics_c(int32_t nIndex);
void *LoadCursorA_c(void *hInstance, const char *lpCursorName);
void *LoadIconA_c(void *hInstance, const char *lpIconName);
void *LoadImageA_c(void *hinst, const char *lpszName, uint32_t uType, int32_t cxDesired, int32_t cyDesired, uint32_t fuLoad);
uint32_t MessageBoxA_c(void *hWnd, const char *lpText, const char *lpCaption, uint32_t uType);
uint32_t OffsetRect_c(void *lprc, int32_t dx, int32_t dy);
uint32_t PeekMessageA_c(void *lpMsg, void *hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax, uint32_t wRemoveMsg);
uint32_t PostMessageA_c(void *hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam);
void PostQuitMessage_c(int32_t nExitCode);
uint32_t PtInRect_c(const void *lprc, int32_t px, int32_t py);
uint32_t RegisterClassA_c(void *lpWndClass);
void *SetCursor_c(void *hCursor);
uint32_t SetCursorPos_c(int32_t X, int32_t Y);
void *SetFocus_c(void *hWnd);
int32_t ShowCursor_c(uint32_t bShow);
uint32_t ShowWindow_c(void *hWnd, int32_t nCmdShow);
uint32_t TranslateMessage_c(void *pMsg);
uint32_t UpdateWindow_c(void *hWnd);
uint32_t ValidateRect_c(void *hWnd, const void *lpRect);
uint32_t WaitMessage_c(void);

#ifdef __cplusplus
}
#endif

#endif

