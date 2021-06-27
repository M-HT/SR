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

#include "WinApi-winmm.h"
#include <windows.h>

uint32_t mciGetErrorStringA_c(uint32_t mcierr, char * pszText, uint32_t cchText)
{
    return mciGetErrorStringA(mcierr, pszText, cchText);
}

uint32_t mciSendCommandA_c(uint32_t mciId, uint32_t uMsg, uint32_t * dwParam1, uint32_t * dwParam2)
{
#if defined(__WINE__)
    // when using wine, this is called only by the detection, so it's ok to return 0 (= OK)
    return 0;
#else
    return mciSendCommandA(mciId, uMsg, (DWORD_PTR)dwParam1, (DWORD_PTR)dwParam2);
#endif
}

