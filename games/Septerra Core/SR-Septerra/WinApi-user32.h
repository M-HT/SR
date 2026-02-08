/**
 *
 *  Copyright (C) 2019-2026 Roman Pauer
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
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// note: return values smaller than 32 bits are not expected by callers in the original asm code to have been extended to 32 bits

uint32_t CCALL AdjustWindowRectEx_c(void *lpRect, uint32_t dwStyle, uint32_t bMenu, uint32_t dwExStyle);
uint32_t CCALL ClipCursor_c(const void *lpRect);
void * CCALL CreateWindowExA_c(uint32_t dwExStyle, const char *lpClassName, const char *lpWindowName, uint32_t dwStyle, int32_t x, int32_t y, int32_t nWidth, int32_t nHeight, void *hWndParent, void *hMenu, void *hInstance, void *lpParam);
uint32_t CCALL DefWindowProcA_c(void *hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam);
uint32_t CCALL DestroyWindow_c(void *hWnd);
uint32_t CCALL DispatchMessageA_c(void *lpMsg);
int16_t CCALL GetAsyncKeyState_c(int32_t vKey);
uint32_t CCALL GetCursorPos_c(void *lpPoint);
int16_t CCALL GetKeyState_c(int32_t nVirtKey);
uint32_t CCALL GetMessageA_c(void *lpMsg, void *hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax);
uint32_t CCALL GetMessagePos_c(void);
uint32_t CCALL GetMessageTime_c(void);
int32_t CCALL GetSystemMetrics_c(int32_t nIndex);
void * CCALL LoadCursorA_c(void *hInstance, PTR32(const char) lpCursorName);
void * CCALL LoadIconA_c(void *hInstance, PTR32(const char) lpIconName);
void * CCALL LoadImageA_c(void *hinst, const char *lpszName, uint32_t uType, int32_t cxDesired, int32_t cyDesired, uint32_t fuLoad);
uint32_t CCALL MessageBoxA_c(void *hWnd, const char *lpText, const char *lpCaption, uint32_t uType);
uint32_t CCALL OffsetRect_c(void *lprc, int32_t dx, int32_t dy);
uint32_t CCALL PeekMessageA_c(void *lpMsg, void *hWnd, uint32_t wMsgFilterMin, uint32_t wMsgFilterMax, uint32_t wRemoveMsg);
uint32_t CCALL PostMessageA_c(void *hWnd, uint32_t Msg, uint32_t wParam, uint32_t lParam);
void CCALL PostQuitMessage_c(int32_t nExitCode);
uint32_t CCALL PtInRect_c(const void *lprc, int32_t px, int32_t py);
uint32_t CCALL RegisterClassA_c(void *lpWndClass);
void * CCALL SetCursor_c(PTR32(void) hCursor);
uint32_t CCALL SetCursorPos_c(int32_t X, int32_t Y);
void * CCALL SetFocus_c(void *hWnd);
int32_t CCALL ShowCursor_c(uint32_t bShow);
uint32_t CCALL ShowWindow_c(void *hWnd, int32_t nCmdShow);
uint32_t CCALL TranslateMessage_c(void *pMsg);
uint32_t CCALL UpdateWindow_c(void *hWnd);
uint32_t CCALL ValidateRect_c(void *hWnd, const void *lpRect);
uint32_t CCALL WaitMessage_c(void);

#ifdef __cplusplus
}
#endif

#endif

