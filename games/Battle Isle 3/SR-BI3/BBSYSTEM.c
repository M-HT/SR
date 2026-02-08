/**
 *
 *  Copyright (C) 2020-2026 Roman Pauer
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

#include "BBSYSTEM.h"
#include "BBDSA.h"
#include "BBAVI.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


extern char DSA_IconID[100];
extern char DSA_WindowName[100];


static unsigned int SYSTEM_OS = 0;
static int SYSTEM_initialized = 0;
static unsigned int SYSTEM_tickBase;


void CCALL SYSTEM_SetInitValues_c(int type, const char *value)
{
    if (type == 0)
    {
        strncpy(DSA_WindowName, value, 100);
        DSA_WindowName[99] = 0;
    }
    else if (type == 1)
    {
        strncpy(DSA_IconID, value, 100);
        DSA_IconID[99] = 0;
    }
}

void CCALL SYSTEM_SystemTask_c(void)
{
    MSG msg;

    if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    DSA_SystemTask_c();
    AVI_SystemTask_c();
}

int CCALL SYSTEM_Init_c(void)
{
    unsigned int platform;
    DWORD version;

    if (SYSTEM_initialized)
    {
        return 1;
    }

    SYSTEM_tickBase = GetTickCount();

    version = GetVersion();
    platform = (version >> 16) & 0x8000;
    if (platform == 0)
    {
        SYSTEM_OS = 1; // Windows NT
    }
    else if (platform == 0x8000)
    {
        if ((version & 0xff) < 4)
        {
            SYSTEM_OS = 2; // Win32s with Windows 3.1
        }
        else
        {
            SYSTEM_OS = 4; // Windows 95
        }
    }

    SYSTEM_initialized = 1;
    return 1;
}

void CCALL SYSTEM_Exit_c(void)
{
    if (SYSTEM_initialized)
    {
        SYSTEM_initialized = 0;
    }
}

unsigned int CCALL SYSTEM_GetTicks_c(void)
{
    return (GetTickCount() - SYSTEM_tickBase) / 17;
}

int CCALL SYSTEM_IsApplicationActive_c(void)
{
    return (IsIconic((HWND)DSAWIN_GetMainWindowHandle_c()) == 0)?1:0;
}

void CCALL SYSTEM_WaitTicks_c(unsigned int ticks)
{
    unsigned int start_ticks;
    int finish;
    UINT_PTR timerID;
    MSG msg;

    finish = 0;
    timerID = SetTimer((HWND)DSAWIN_GetMainWindowHandle_c(), 0xAA, (1000 * ticks) / 60, NULL);
    if (timerID != 0)
    {
        while (!finish)
        {
            if (GetMessageA(&msg, NULL, 0, 0))
            {
                if ((msg.hwnd == DSAWIN_GetMainWindowHandle_c()) &&
                    (msg.message == WM_TIMER) &&
                    (msg.wParam == timerID )
                   )
                {
                    finish = 1;
                }

                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }

            DSA_SystemTask_c();
        }

        KillTimer((HWND)DSAWIN_GetMainWindowHandle_c(), timerID);
    }
    else
    {
        // change: insert delays into active waiting
        //start_ticks = SYSTEM_GetTicks_c();
        //while (SYSTEM_GetTicks_c() < ticks + start_ticks)
        //{
        //    SYSTEM_SystemTask_c();
        //}
        start_ticks = GetTickCount();
        finish = (1000 * ticks) / 60;
        while (1)
        {
            ticks = GetTickCount();
            if (((int)(ticks - start_ticks)) >= finish) break;

            SYSTEM_SystemTask_c();

            if (GetTickCount() == ticks)
            {
                if (!PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE | PM_NOYIELD))
                {
                    Sleep(0);
                    if (GetTickCount() == ticks)
                    {
                        if (!PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE | PM_NOYIELD))
                        {
                            Sleep(1);
                        }
                    }
                }
            }
        }
    }
}

void CCALL SYSTEM_EnterCriticalSection_c(void)
{
}

void CCALL SYSTEM_LeaveCriticalSection_c(void)
{
}

int CCALL SYSTEM_InCriticalSection_c(void)
{
    return 0;
}

unsigned int CCALL SYSTEM_GetOS_c(void)
{
    return SYSTEM_OS;
}

