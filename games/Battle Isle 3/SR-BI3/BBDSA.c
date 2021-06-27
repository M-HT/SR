/**
 *
 *  Copyright (C) 2020-2021 Roman Pauer
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

#include "BBDSA.h"
#include "BBDBG.h"
#include "BBSYSTEM.h"
#include "BBLBL.h"
#include "BBMEM.h"
#include "BBLL.h"
#include "BBBLEV.h"
#include "BBFX.h"
#include "SDIresource.h"
#include <string.h>
#include "WinApi-wing32.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


// exported variables
#ifdef __cplusplus
extern "C" {
#endif
extern DSA_Screen *p_last_touched_screen;
extern int32_t sl_screenwidth;
extern int32_t sl_screenheight;
extern DSA_Screen *p_mouse_gui;
extern DSA_Screen *h_mouse_win;
extern int32_t b_mouse_capture_on;
extern int32_t b_application_active;
#ifdef __cplusplus
}
#endif


#define BBDSA_UNKNOWN1 1
#define BBDSA_VISIBLE 2

#define BBDSA_EXTRA_SCREEN 0
#define BBDSA_EXTRA_BUTTON_STATE 4
#define BBDSA_EXTRA_RESIZING_MODE 8
#define BBDSA_EXTRA_PALETTE_REMAPPING 12
#define BBDSA_EXTRA_SIZE 16

#define IDC_ICON1 101


typedef struct {
    int used;
    DSA_Screen *screen;
} DSA_Screen2;

typedef union {
    LOGPALETTE pal;
    struct {
        WORD         palVersion2;
        WORD         palNumEntries2;
        PALETTEENTRY palPalEntry2[256];
    } pal2;
} myLOGPALETTE;


static int DSA_deactivate = 0;
static const INT DSA_elements[19] = { COLOR_ACTIVEBORDER, COLOR_ACTIVECAPTION, COLOR_APPWORKSPACE, COLOR_BACKGROUND, COLOR_3DDKSHADOW, COLOR_3DSHADOW, COLOR_BTNTEXT, COLOR_CAPTIONTEXT, COLOR_GRAYTEXT, COLOR_HIGHLIGHT, COLOR_HIGHLIGHTTEXT, COLOR_INACTIVEBORDER, COLOR_INACTIVECAPTION, COLOR_MENU, COLOR_MENUTEXT, COLOR_SCROLLBAR, COLOR_WINDOW, COLOR_WINDOWFRAME, COLOR_WINDOWTEXT };
static const COLORREF DSA_elementColors[19] = { RGB(0, 0, 0), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(0, 0, 0), RGB(255, 255, 255), RGB(255, 255, 255), RGB(0, 0, 0), RGB(0, 0, 0) };
static int DSA_isBitmapDirty = 0;
static const char DSA_mainClass[] = "BBMAINWINDOW";
static const char DSA_windowClasses[5][5] = { "0000", "0001", "XXXX", "0003", "XXXX" };
static const DWORD DSA_windowStyles[5] = { WS_POPUP, WS_POPUP | WS_CAPTION, 0xFFFFFFFF, WS_POPUP | WS_CAPTION, 0xFFFFFFFF };
static HINSTANCE DSA_hInstance = NULL;
static int DSA_initialized = 0;
static DSA_Screen *DSA_activeScreen = NULL;
static int DSA_isBGFixed = 0;
static int DSA_isBGBlack = 0;
static HDC DSA_BG_hDC = NULL;
static HGDIOBJ DSA_BG_hBitmap = NULL;
static uint8_t *DSA_BG_buffer = NULL;
static int DSA_isBGInRAM = 1;
static int DSA_BG_offsetX = -1;
static int DSA_BG_offsetY = -1;
static int DSA_wasVisible = 0;
unsigned int DSA_pixelMapList = 0;
static DSA_Palette DSA_palette = { 256, 1, { { 0 } } };
static int DSA_bitsPerPixel = 0;
static DSA_Screen *DSA_nextActiveScreen;
static COLORREF DSA_systemColors[19];
static LOGPALETTE *DSA_systemPalette;
static BYTE DSA_keyState[256];
char DSA_IconID[100];
char DSA_WindowName[100];
HPALETTE DSA_hPalette;
static PALETTEENTRY DSA_paletteEntries[256];
static int DSA_activated;
OPM_struct DSA_pixelMapBG;
static DSA_Screen2 DSA_screens[64];
RGBQUAD DSA_colorTable[256];


static int DSA_GetMyIndex_c(void *hWnd);
static void DSA_ActivateApp_c(int isActive);
static void DSA_SetColorTable_c(void);
static LRESULT WINAPI DSA_Mainwindow_Callback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT WINAPI DSA_0000_Callback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT WINAPI DSA_0003_Callback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


static HPALETTE DSA_CreatePalette_c(void)
{
    HPALETTE hPal;
    int index;
    myLOGPALETTE palette;

    palette.pal.palVersion = 0x300;
    palette.pal.palNumEntries = 256;
    memset(palette.pal.palPalEntry, 0, 256 * sizeof(PALETTEENTRY));

    for (index = 0; index < 256; index++)
    {
        palette.pal2.palPalEntry2[index].peRed = DSA_palette.palPalEntry[index].peRed;
        palette.pal2.palPalEntry2[index].peGreen = DSA_palette.palPalEntry[index].peGreen;
        palette.pal2.palPalEntry2[index].peBlue = DSA_palette.palPalEntry[index].peBlue;
        palette.pal2.palPalEntry2[index].peFlags = PC_NOCOLLAPSE;
    }

    palette.pal2.palPalEntry2[0].peFlags = 0;
    palette.pal2.palPalEntry2[255].peFlags = 0;

    hPal = CreatePalette(&(palette.pal));
    if (hPal == NULL)
    {
        DBG_Panic_c("C:\\DATA\\BBLIB\\SRC\\BASE\\bbdsa.c", 611);
        return 0;
    }

    return hPal;
}

static void DSA_MapPalette_c(LOGPALETTE *palette)
{
    HPALETTE hPrevPal;
    HDC hDC;
    HPALETTE hPal;
    int index;
    myLOGPALETTE pal2;

    pal2.pal.palVersion = 0x300;
    pal2.pal.palNumEntries = 256;
    memset(pal2.pal.palPalEntry, 0, 256 * sizeof(PALETTEENTRY));

    if (palette != NULL)
    {
        memmove(pal2.pal.palPalEntry, palette->palPalEntry, 256 * sizeof(PALETTEENTRY));
        for (index = 0; index < 256; index++)
        {
            pal2.pal2.palPalEntry2[index].peFlags = PC_NOCOLLAPSE;
        }
    }
    else
    {
        for (index = 0; index < 256; index++)
        {
            pal2.pal2.palPalEntry2[index].peRed = 0;
            pal2.pal2.palPalEntry2[index].peGreen = 0;
            pal2.pal2.palPalEntry2[index].peBlue = 0;
            pal2.pal2.palPalEntry2[index].peFlags = PC_NOCOLLAPSE;
        }
    }

    pal2.pal2.palPalEntry2[255].peRed = 255;
    pal2.pal2.palPalEntry2[255].peGreen = 255;
    pal2.pal2.palPalEntry2[255].peBlue = 255;
    pal2.pal2.palPalEntry2[0].peFlags = 0;
    pal2.pal2.palPalEntry2[255].peFlags = 0;

    hDC = GetDC(NULL);
    hPal = CreatePalette(&(pal2.pal));
    if (hPal != NULL)
    {
        UnrealizeObject(hPal);
        hPrevPal = SelectPalette(hDC, hPal, FALSE);
        RealizePalette(hDC);
        DeleteObject(SelectPalette(hDC, hPrevPal, FALSE));
    }

    ReleaseDC(NULL, hDC);
}

void DSA_ReuseStaticColors_c(unsigned int osVersions)
{
    HDC hDC;

    if (!(SYSTEM_GetOS_c() & osVersions))
    {
        return;
    }

    hDC = GetDC(NULL);
    if (DSA_bitsPerPixel == 8)
    {
        SetSystemPaletteUse(hDC, SYSPAL_NOSTATIC);
    }
    ReleaseDC(NULL, hDC);

    DSA_MapPalette_c(NULL);
    DSA_ActivatePal_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
}

void DSA_FreeStaticColors_c(unsigned int osVersions)
{
    HDC hDC;

    if (!(SYSTEM_GetOS_c() & osVersions))
    {
        return;
    }

    hDC = GetDC(NULL);
    if (DSA_bitsPerPixel == 8)
    {
        SetSystemPaletteUse(hDC, SYSPAL_STATIC);
    }
    ReleaseDC(0, hDC);

    DSA_MapPalette_c(NULL);
    DSA_ActivatePal_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
}

void DSAWIN_PrepareInit_c(void *hInstance)
{
    DSA_hInstance = (HINSTANCE)hInstance;
}

void *DSAWIN_GetMainWindowHandle_c(void)
{
    return (DSA_screens[0].used)?DSA_screens[0].screen->hWnd:NULL;
}

void *DSAWIN_GetInstance_c(void)
{
    return DSA_hInstance;
}

int DSA_Init_c(void)
{
    int index;
    HDC hDC;
    WNDCLASSA WndClass;

    if (DSA_initialized)
    {
        return 1;
    }

    memset(&WndClass, 0, sizeof(WNDCLASSA));
    hDC = GetDC(NULL);
    if (hDC == NULL)
    {
        DBG_Panic_c("C:\\DATA\\BBLIB\\SRC\\BASE\\bbdsa.c", 1185);
        return 0;
    }

    DSA_bitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);
    ReleaseDC(NULL, hDC);

    if (DSA_bitsPerPixel < 8)
    {
        MessageBoxA(NULL, "This program needs at least 256 colors! Please switch to an appropriate color resolution", "SETTINGS ERROR", MB_OK | MB_ICONERROR);
        return 0;
    }

    DSA_systemPalette = (LOGPALETTE *)MEM_malloc_c(sizeof(LOGPALETTE) + 255 * sizeof(PALETTEENTRY), NULL, NULL, 0, 0);
    if (DSA_systemPalette == NULL)
    {
        return 0;
    }

    DSA_systemPalette->palNumEntries = 256;
    DSA_systemPalette->palVersion = 0x300;
    hDC = GetDC(NULL);
    GetSystemPaletteEntries(hDC, 0, 256, DSA_systemPalette->palPalEntry);
    ReleaseDC(NULL, hDC);

    DSA_MapPalette_c(NULL);
    DSA_BG_hDC = NULL;
    DSA_BG_hBitmap = NULL;
    DSA_BG_buffer = NULL;
    DSA_pixelMapBG.flags = 0;
    DSA_activeScreen = NULL;

    sl_screenwidth = GetSystemMetrics(SM_CXSCREEN);
    sl_screenheight = GetSystemMetrics(SM_CYSCREEN);
    b_mouse_capture_on = 0;
    p_last_touched_screen = NULL;

    for (index = 1; index < 64; index++)
    {
        DSA_screens[index].used = 0;
    }

    DSA_screens[0].used = 1;
    DSA_screens[0].screen = (DSA_Screen *)MEM_malloc_c(sizeof(DSA_Screen), NULL, NULL, 0, 0);
    if (DSA_screens[0].screen == NULL)
    {
        MEM_free_c(DSA_systemPalette);
        return 0;
    }

    DSA_screens[0].screen->hDC = NULL;
    DSA_screens[0].screen->hBitmap = NULL;
    DSA_screens[0].screen->opm = NULL;
    DSA_screens[0].screen->unknown_08 = 0;
    DSA_screens[0].screen->unknown_00 = 0;

    WndClass.style = CS_BYTEALIGNWINDOW | CS_NOCLOSE | CS_DBLCLKS;
    WndClass.lpfnWndProc = (WNDPROC)DSA_Mainwindow_Callback_c;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = BBDSA_EXTRA_SIZE;
    WndClass.hInstance = DSA_hInstance;
    // change: don't load app icon from executable resources
    //WndClass.hIcon = LoadIconA(DSA_hInstance, (LPCSTR)MAKEINTRESOURCE(IDC_ICON1));
    WndClass.hIcon = (HICON)resource_LoadAppIcon();
    WndClass.hCursor = NULL;
    WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = DSA_mainClass;
    if (!RegisterClassA(&WndClass))
    {
        MEM_free_c(DSA_systemPalette);
        return 0;
    }

    WndClass.style = CS_BYTEALIGNWINDOW | CS_NOCLOSE | CS_DBLCLKS;
    WndClass.lpfnWndProc = (WNDPROC)DSA_0000_Callback_c;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = BBDSA_EXTRA_SIZE;
    WndClass.hInstance = DSA_hInstance;
    WndClass.hIcon = LoadIconA(NULL, (LPCSTR)IDI_APPLICATION);
    WndClass.hCursor = NULL;
    WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = DSA_windowClasses[0];
    if (!RegisterClassA(&WndClass))
    {
        UnregisterClassA(DSA_mainClass, DSA_hInstance);
        MEM_free_c(DSA_systemPalette);
        return 0;
    }

    WndClass.style = CS_BYTEALIGNWINDOW | CS_NOCLOSE | CS_DBLCLKS;
    WndClass.lpfnWndProc = (WNDPROC)DSA_0003_Callback_c;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = BBDSA_EXTRA_SIZE;
    WndClass.hInstance = DSA_hInstance;
    WndClass.hIcon = LoadIconA(NULL, (LPCSTR)IDI_APPLICATION);
    WndClass.hCursor = NULL;
    WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = DSA_windowClasses[1];
    if (!RegisterClassA(&WndClass))
    {
        UnregisterClassA(DSA_mainClass, DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[0], DSA_hInstance);
        MEM_free_c(DSA_systemPalette);
        return 0;
    }

    WndClass.style = CS_BYTEALIGNWINDOW | CS_NOCLOSE | CS_DBLCLKS;
    WndClass.lpfnWndProc = (WNDPROC)DSA_0003_Callback_c;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = BBDSA_EXTRA_SIZE;
    WndClass.hInstance = DSA_hInstance;
    WndClass.hIcon = LoadIconA(NULL, (LPCSTR)IDI_APPLICATION);
    WndClass.hCursor = NULL;
    WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = DSA_windowClasses[3];
    if (!RegisterClassA(&WndClass))
    {
        UnregisterClassA(DSA_mainClass, DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[0], DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[1], DSA_hInstance);
        MEM_free_c(DSA_systemPalette);
        return 0;
    }

    DSA_screens[0].screen->hWnd = CreateWindowExA(
        0,
        DSA_mainClass,
        DSA_WindowName,
        WS_POPUP,
        -GetSystemMetrics(SM_CXBORDER),
        -GetSystemMetrics(SM_CYBORDER),
        GetSystemMetrics(SM_CXSCREEN) + GetSystemMetrics(SM_CXBORDER),
        GetSystemMetrics(SM_CYSCREEN) + GetSystemMetrics(SM_CYBORDER),
        NULL,
        NULL,
        DSA_hInstance,
        0
    );
    if (DSA_screens[0].screen->hWnd == NULL)
    {
        UnregisterClassA(DSA_mainClass, DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[0], DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[1], DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[3], DSA_hInstance);
        MEM_free_c(DSA_systemPalette);
        return 0;
    }

    SetTimer(DSA_screens[0].screen->hWnd, 0, 100, NULL);
    ShowWindow(DSA_screens[0].screen->hWnd, SW_SHOW);

    DSA_hPalette = DSA_CreatePalette_c();
    if (DSA_hPalette == NULL)
    {
        SendMessageA(DSA_screens[0].screen->hWnd, WM_QUIT, 0, 0);
        UnregisterClassA(DSA_mainClass, DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[0], DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[1], DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[3], DSA_hInstance);
        MEM_free_c(DSA_systemPalette);
        return 0;
    }

    DSA_pixelMapList = LL_NewList_c();
    if (DSA_pixelMapList == 0)
    {
        SendMessageA(DSA_screens[0].screen->hWnd, WM_QUIT, 0, 0);
        UnregisterClassA(DSA_mainClass, DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[0], DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[1], DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[3], DSA_hInstance);
        DeleteObject(DSA_hPalette);
        MEM_free_c(DSA_systemPalette);
        return 0;
    }

    DSA_initialized = 1;
    return 1;
}

void DSA_Exit_c(void)
{
    int index;

    if (DSA_initialized)
    {
        KillTimer((HWND)DSA_screens[0].screen->hWnd, 0);

        for (index = 1; index < 64; index++)
        {
            if (DSA_screens[index].used)
            {
                DSA_CloseScreen_c(DSA_screens[index].screen);
            }
        }

        if (IsWindow((HWND)DSA_screens[0].screen->hWnd))
        {
            SendMessageA((HWND)DSA_screens[0].screen->hWnd, WM_CLOSE, 0, 0);
            DSA_screens[0].screen->hWnd = NULL;
        }

        if (DSA_BG_hDC != NULL)
        {
            OPM_Del_c(&DSA_pixelMapBG);
        }

        MEM_free_c(DSA_screens[0].screen);
        MEM_free_c(DSA_systemPalette);
        UnregisterClassA(DSA_mainClass, DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[0], DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[1], DSA_hInstance);
        UnregisterClassA(DSA_windowClasses[3], DSA_hInstance);
        DeleteObject(DSA_hPalette);
    }

    LL_DestroyList_c(DSA_pixelMapList);
    DSA_initialized = 0;
}

int DSA_OpenScreen_c(DSA_Screen *screen, OPM_struct *opm, int param3, const char *windowName, int x, int y, int type)
{
    int index, width, height;
    HWND hWnd;

    x = (int16_t)x;
    y = (int16_t)y;

    screen->unknown_00 = 0;

    for (index = 0; index < 64; index++)
    {
        if (!DSA_screens[index].used) break;
    }

    if (index >= 64)
    {
        return 0;
    }

    DSA_screens[index].screen = screen;

    if (x == -1)
    {
        x = (sl_screenwidth / 2) - (opm->width >> 1);
    }

    if (y == -1)
    {
        y = (sl_screenheight / 2) - (opm->height >> 1);
    }

    if (DSA_windowStyles[type & 3] & WS_CAPTION)
    {
        width = opm->width + 2 * GetSystemMetrics(SM_CXBORDER);
        height = opm->height + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER);
        hWnd = CreateWindowExA(
            0,
            DSA_windowClasses[type & 3],
            windowName,
            DSA_windowStyles[type & 3],
            x,
            y,
            width,
            height,
            DSA_screens[0].screen->hWnd,
            NULL,
            DSA_hInstance,
            NULL
        );
    }
    else
    {
        hWnd = CreateWindowExA(
            0,
            DSA_windowClasses[type & 3],
            windowName,
            DSA_windowStyles[type & 3],
            x,
            y,
            opm->width,
            opm->height,
            DSA_screens[0].screen->hWnd,
            NULL,
            DSA_hInstance,
            NULL
        );
    }

    screen->hWnd = hWnd;
    if (screen->hWnd == NULL)
    {
        return 0;
    }

    screen->hDC = opm->hDC;
    screen->hBitmap = opm->hBitmap;
    screen->opm = opm;
    screen->type = type;
    screen->size = opm->height * opm->width;
    screen->flags = 0;
    SetWindowLongA(screen->hWnd, BBDSA_EXTRA_SCREEN, (LONG)screen);
    DSA_screens[index].used = 1;
    DSA_activeScreen = screen;
    SendMessageA(screen->hWnd, WM_QUERYNEWPALETTE, 0, 0);

    return 1;
}

void DSA_CloseScreen_c(DSA_Screen *screen)
{
    int index;

    index = DSA_GetMyIndex_c(screen->hWnd);
    if (index == -1)
    {
        return;
    }

    if (!DSA_screens[index].used)
    {
        return;
    }

    if (IsWindow((HWND)screen->hWnd))
    {
        SendMessageA((HWND)screen->hWnd, WM_CLOSE, 0, 0);

        DSA_screens[index].screen->hWnd = NULL;

        if (p_last_touched_screen == screen)
        {
            p_last_touched_screen = NULL;
        }

        if (DSA_activeScreen == screen)
        {
            DSA_activeScreen = NULL;
        }

        if (screen == p_mouse_gui)
        {
            p_mouse_gui = NULL;
        }

        if (screen == h_mouse_win)
        {
            h_mouse_win = NULL;
        }

        //if (screen == h_mouse_win)
        //{
        //    h_mouse_win = NULL;
        //}

        if (screen == DSA_nextActiveScreen)
        {
            DSA_nextActiveScreen = NULL;
        }
    }

    screen->unknown_00 = 0;
    DSA_screens[index].used = 0;
}

void DSA_CopyMainOPMToScreen_c(DSA_Screen *screen, int onlyModified)
{
    OPM_struct *opm;

    opm = screen->opm;
    if (onlyModified & 0xffff)
    {
        if (!(opm->flags & BBOPM_MODIFIED))
        {
            return;
        }
    }

    opm->flags &= ~BBOPM_MODIFIED;
    if (!IsIconic((HWND)DSA_screens[0].screen->hWnd))
    {
        InvalidateRect((HWND)screen->hWnd, NULL, TRUE);
        UpdateWindow((HWND)screen->hWnd);
    }
}

void DSA_StretchOPMToScreen_c(DSA_Screen *screen, int xDst, int yDst, int widthDst, int heightDst, OPM_struct *opm, int xSrc, int ySrc, int widthSrc, int heightSrc)
{
    HDC hDC;
    HWND hWnd;
    HPALETTE hPrevPal;

    hWnd = (screen != NULL)?(HWND)screen->hWnd:NULL;
    hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        return;
    }

    hPrevPal = SelectPalette(hDC, DSA_hPalette, FALSE);
    if (hPrevPal != NULL)
    {
        WinGStretchBlt_c(hDC, (int16_t)xDst, (int16_t)yDst, (int16_t)widthDst, (int16_t)heightDst, (HDC)opm->hDC, (int16_t)xSrc, (int16_t)ySrc, (int16_t)widthSrc, (int16_t)heightSrc);
        SelectPalette(hDC, hPrevPal, FALSE);
    }
    ReleaseDC(hWnd, hDC);
}

void DSA_CopyOPMToScreenEx_c(DSA_Screen *screen, int xDst, int yDst, int width, int height, OPM_struct *opm, int xSrc, int ySrc)
{
    HDC hDC;
    HWND hWnd;
    HPALETTE hPrevPal;
    int screenWidth;
    int screenHeight;

    if (screen != NULL)
    {
        hWnd = (HWND)screen->hWnd;
        screenWidth = screen->opm->width;
        screenHeight = screen->opm->height;
    }
    else
    {
        hWnd = NULL;
        screenWidth = sl_screenwidth;
        screenHeight = sl_screenheight;
    }

    xDst = (int16_t)xDst;
    yDst = (int16_t)yDst;
    width = (int16_t)width;
    height = (int16_t)height;
    xSrc = (int16_t)xSrc;
    ySrc = (int16_t)ySrc;

    if (xDst + width > screenWidth)
    {
        width = screenWidth - xDst;
    }

    if (yDst + height > screenHeight)
    {
        height = screenHeight - yDst;
    }

    hDC = GetDC(hWnd);
    if (hDC == NULL)
    {
        return;
    }

    hPrevPal = SelectPalette(hDC, DSA_hPalette, FALSE);
    if (hPrevPal != NULL)
    {
        WinGBitBlt_c(hDC, xDst, yDst, width, height, (HDC)opm->hDC, xSrc, ySrc);
        SelectPalette(hDC, hPrevPal, FALSE);
    }
    ReleaseDC(hWnd, hDC);
}

static int DSA_GetMyIndex_c(void *hWnd)
{
    int index;

    for (index = 0; index < 64; index++)
    {
        if ((DSA_screens[index].used) && (DSA_screens[index].screen->hWnd == hWnd))
        {
            return index;
        }
    }

    return -1;
}

void DSA_TotalRepaint_c(void)
{
    int index;

    if (!DSA_initialized)
    {
        return;
    }

    for (index = 0; index < 64; index++)
    {
        if (DSA_screens[index].used)
        {
            InvalidateRect((HWND)DSA_screens[index].screen->hWnd, NULL, (index == 0)?TRUE:FALSE);
            UpdateWindow((HWND)DSA_screens[index].screen->hWnd);
        }
    }
}

void DSA_MoveScreen_c(DSA_Screen *screen, int changeX, int changeY)
{
    int index;
    RECT rect;

    GetWindowRect((HWND)screen->hWnd, &rect);
    SetWindowPos((HWND)screen->hWnd, HWND_TOP, changeX + rect.left, changeY + rect.top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);

    for (index = 0; index < 64; index++)
    {
        if (DSA_screens[index].used)
        {
            UpdateWindow((HWND)DSA_screens[index].screen->hWnd);
        }
    }
}

int DSA_ResizeScreen_c(DSA_Screen *screen, OPM_struct *opm, int redraw)
{
    int width, height, index;

    screen->size = opm->height * opm->width;
    screen->opm = opm;
    screen->hDC = opm->hDC;
    screen->hBitmap = opm->hBitmap;

    if (DSA_windowStyles[screen->type & 3] & WS_CAPTION)
    {
        width = opm->width + 2 * GetSystemMetrics(SM_CXBORDER);
        height = opm->height + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER);
    }
    else
    {
        width = opm->width;
        height = opm->height;
    }

    memset(opm->buffer, 0, opm->height * opm->width);
    SetWindowPos((HWND)screen->hWnd, HWND_TOP, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOZORDER | SWP_NOMOVE);
    WinGSetDIBColorTable_c((HDC)opm->hDC, 0, 256, DSA_colorTable);

    if (redraw & 0xff)
    {
        InvalidateRect((HWND)screen->hWnd, NULL, TRUE);
        UpdateWindow((HWND)screen->hWnd);
    }

    for (index = 0; index < 64; index++)
    {
        if (DSA_screens[index].used)
        {
            if (DSA_screens[index].screen != screen)
            {
                InvalidateRect((HWND)DSA_screens[index].screen->hWnd, NULL, TRUE);
                UpdateWindow((HWND)DSA_screens[index].screen->hWnd);
            }
        }
    }

    return 1;
}

int DSA_DrawSizingScreen_c(DSA_Screen *screen, int16_t *rect)
{
    int index;

    if (DSA_windowStyles[screen->type & 3] & WS_CAPTION)
    {
        rect[2] += 2 * GetSystemMetrics(SM_CXBORDER);
        rect[3] += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYBORDER);
    }

    SetWindowPos((HWND)screen->hWnd, HWND_TOP, 0, 0, rect[2], rect[3], SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
    InvalidateRect((HWND)screen->hWnd, NULL, TRUE);
    UpdateWindow((HWND)screen->hWnd);

    for (index = 0; index < 64; index++)
    {
        if (DSA_screens[index].used)
        {
            if (DSA_screens[index].screen != screen)
            {
                UpdateWindow((HWND)DSA_screens[index].screen->hWnd);
            }
        }
    }

    return 1;
}

void DSA_EnterResizingMode_c(DSA_Screen *screen)
{
    SendMessageA((HWND)screen->hWnd, WM_USER, 1, 0);
}

void DSA_LeaveResizingMode_c(DSA_Screen *screen)
{
    SendMessageA((HWND)screen->hWnd, WM_USER, 0, 0);
}

void DSA_GetDSAMetrics_c(DSA_Screen *screen, int32_t *x, int32_t *y, int32_t *width, int32_t *height, uint8_t *isVisible)
{
    RECT rect;

    if (screen != NULL)
    {
        GetWindowRect((HWND)screen->hWnd, &rect);
    }
    else
    {
        GetWindowRect((HWND)DSAWIN_GetMainWindowHandle_c(), &rect);
    }

    if (x != NULL)
    {
        *x = rect.left;
    }

    if (y != NULL)
    {
        *y = rect.top;
    }

    if (width != NULL)
    {
        *width = rect.right - rect.left;
    }

    if (height != NULL)
    {
        *height = rect.bottom - rect.top;
    }

    if (isVisible != NULL)
    {
        if (screen != NULL)
        {
            *isVisible = IsWindowVisible((HWND)screen->hWnd);
        }
        else
        {
            *isVisible = 1;
        }
    }
}

void DSA_SetDSAPos_c(DSA_Screen *screen, int x, int y, int repaint)
{
    RECT rect;

    if (x == -1)
    {
        x = (sl_screenwidth / 2) - (screen->opm->width >> 1);
    }
    else if (x == -2)
    {
        GetWindowRect((HWND)screen->hWnd, &rect);
        x = rect.left;
    }

    if (y == -1)
    {
        y = (sl_screenheight / 2) - (screen->opm->height >> 1);
    }
    else if (y == -2)
    {
        GetWindowRect((HWND)screen->hWnd, &rect);
        y = rect.top;
    }

    SetWindowPos((HWND)screen->hWnd, HWND_TOP, x, y, 0, 0, SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOZORDER | SWP_NOSIZE);

    if (repaint & 0xff)
    {
        DSA_TotalRepaint_c();
    }
}

static LRESULT DSA_MouseEvent_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DSA_Screen *screen, LONG buttonState)
{
    BLEV_Event event;

    uMsg = uMsg & 0xffff;

    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
        event.type = BBBLEV_TYPE_MOUSELEFTDOWN;
        buttonState |= BBBLEV_BUTTON_MOUSELEFT;
        break;

    case WM_LBUTTONUP:
        event.type = BBBLEV_TYPE_MOUSELEFTUP;
        buttonState &= ~BBBLEV_BUTTON_MOUSELEFT;
        break;

    case WM_LBUTTONDBLCLK:
        event.type = BBBLEV_TYPE_MOUSELEFTDOUBLECLICK;
        buttonState &= ~BBBLEV_BUTTON_MOUSELEFT;
        break;

    case WM_RBUTTONDOWN:
        event.type = BBBLEV_TYPE_MOUSERIGHTDOWN;
        buttonState |= BBBLEV_BUTTON_MOUSERIGHT;
        break;

    case WM_RBUTTONUP:
        event.type = BBBLEV_TYPE_MOUSERIGHTUP;
        buttonState &= ~BBBLEV_BUTTON_MOUSERIGHT;
        break;

    case WM_RBUTTONDBLCLK:
        event.type = BBBLEV_TYPE_MOUSERIGHTDOUBLECLICK;
        buttonState &= ~BBBLEV_BUTTON_MOUSERIGHT;
        break;

    default:
        break;
    }

    event.screen = screen;
    event.key = -1;
    event.x = lParam & 0xffff;
    event.y = lParam >> 16;
    event.buttonState = buttonState;
    SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, buttonState);
    BLEV_PutEvent_c(&event);

    return 0;
}

static LRESULT DSA_KeyEvent_c(HWND hWnd, UINT uMsg, UINT wParam, LPARAM lParam, DSA_Screen *screen, LONG buttonState)
{
    WORD chars[2];
    BLEV_Event event;

    uMsg = uMsg & 0xffff;

    event.type = (uMsg == WM_KEYUP)?BBBLEV_TYPE_KEYUP:BBBLEV_TYPE_KEYDOWN;

    switch (wParam)
    {
    case VK_SHIFT:
        if (uMsg == WM_KEYUP)
        {
            SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, buttonState & ~BBBLEV_BUTTON_SHIFT);
        }
        else
        {
            SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, buttonState | BBBLEV_BUTTON_SHIFT);
        }
        return 0;

    case VK_CONTROL:
        if (uMsg == WM_KEYUP)
        {
            SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, buttonState & ~BBBLEV_BUTTON_CONTROL);
        }
        else
        {
            SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, buttonState | BBBLEV_BUTTON_CONTROL);
        }
        return 0;

    case VK_MENU:
        if (uMsg == WM_KEYUP)
        {
            SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, buttonState & ~BBBLEV_BUTTON_ALT);
        }
        else
        {
            SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, buttonState | BBBLEV_BUTTON_ALT);
        }
        return 0;

    default:
        break;
    }

    switch (wParam)
    {
    case VK_TAB:
        event.key = BBBLEV_KEY_TAB;
        break;

    case VK_BACK:
        event.key = BBBLEV_KEY_BACKSPACE;
        break;

    case VK_RETURN:
        event.key = BBBLEV_KEY_ENTER;
        break;

    case VK_PRIOR:
        event.key = BBBLEV_KEY_PAGEUP;
        break;

    case VK_NEXT:
        event.key = BBBLEV_KEY_PAGEDOWN;
        break;

    case VK_ESCAPE:
        event.key = BBBLEV_KEY_ESCAPE;
        break;

    case VK_END:
        event.key = BBBLEV_KEY_END;
        break;

    case VK_HOME:
        event.key = BBBLEV_KEY_HOME;
        break;

    case VK_LEFT:
        event.key = BBBLEV_KEY_LEFT;
        break;

    case VK_UP:
        event.key = BBBLEV_KEY_UP;
        break;

    case VK_RIGHT:
        event.key = BBBLEV_KEY_RIGHT;
        break;

    case VK_DOWN:
        event.key = BBBLEV_KEY_DOWN;
        break;

    case VK_DELETE:
        event.key = BBBLEV_KEY_DELETE;
        break;

    case VK_INSERT:
        event.key = BBBLEV_KEY_INSERT;
        break;

    default:
        if ((wParam >= VK_F1) && (wParam <= VK_F12))
        {
            event.key = wParam + (BBBLEV_KEY_F1 - VK_F1);
        }
        else
        {
            GetKeyboardState(DSA_keyState);
            if (ToAscii(wParam, (lParam >> 16) & 0xff, DSA_keyState, chars, 0) == 1)
            {
                event.key = chars[0] & 0xff;
            }
            else
            {
                return DefWindowProcA(hWnd, uMsg, wParam, lParam);
            }
        }
        break;
    }

    event.buttonState = buttonState;
    event.screen = screen;
    event.x = -1;
    event.y = -1;
    BLEV_PutEvent_c(&event);

    return 0;
}

static LRESULT WINAPI DSA_Mainwindow_Callback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HPALETTE hPrevPal;
    HDC hDC;
    UINT mappedPaletteEntries;
    LONG buttonState;
    PAINTSTRUCT paint;

    uMsg = uMsg & 0xffff;

    buttonState = GetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE);

    switch (uMsg)
    {
    case WM_COMMAND:
        // wParam
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
        return DSA_MouseEvent_c(hWnd, uMsg, wParam, lParam, NULL, buttonState);

    case WM_KEYDOWN:
    case WM_KEYUP:
        return DSA_KeyEvent_c(hWnd, uMsg, wParam, lParam, NULL, buttonState);

    case WM_ACTIVATE:
        if ((wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE))
        {
            DSA_activeScreen = NULL;
        }

        if (((wParam & 0xffff) != WA_ACTIVE) || DSA_deactivate)
        {
            return 0;
        }

        DSA_deactivate = 1;
        DSA_ActivateApp_c(1);
        DSA_ActivatePal_c();
        break;

    case WM_ACTIVATEAPP:
#if defined(__WINE__)
        // change: workaround
        return 0;
#endif
        DSA_activated = wParam & 0xff;
        if (DSA_deactivate)
        {
            if (!DSA_activated)
            {
                DSA_ActivateApp_c(DSA_activated);
            }

            if (DSA_hPalette != NULL)
            {
                UnrealizeObject(DSA_hPalette);
                hDC = GetDC(hWnd);
                if (hDC != NULL)
                {
                    hPrevPal = SelectPalette(hDC, DSA_hPalette, FALSE);
                    RealizePalette(hDC);
                    SelectPalette(hDC, hPrevPal, FALSE);
                    ReleaseDC(hWnd, hDC);
                }
            }
        }
        break;

    case WM_PALETTECHANGED:
        if (((HWND)wParam == hWnd) || (!DSA_deactivate))
        {
            break;
        }
        // @fallthrough@
    case WM_QUERYNEWPALETTE:
        mappedPaletteEntries = 0;
        if (DSA_deactivate)
        {
            if ( DSA_hPalette )
            {
                hDC = GetDC(hWnd);
                if (hDC != NULL)
                {
                    hPrevPal = SelectPalette(hDC, DSA_hPalette, FALSE);
                    if (hPrevPal != NULL)
                    {
                        DSA_SetColorTable_c();
                        mappedPaletteEntries = RealizePalette(hDC);
                        SelectPalette(hDC, hPrevPal, FALSE);
                        ReleaseDC(hWnd, hDC);
                    }
                }
            }
            return mappedPaletteEntries;
        }
        break;

    case WM_CREATE:
        SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, 0);
        SetWindowLongA(hWnd, BBDSA_EXTRA_RESIZING_MODE, 0);
        SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, 0);
        // change: remove unused
        //DSA_hDesktopWnd = GetDesktopWindow();
        //dword_10016680 = (int)GetSystemMenu(hWnd, 0);
        break;

    case WM_MOVE:
    case WM_SIZE:
        break;

    case WM_ERASEBKGND:
        if ((DSA_BG_hDC != NULL) && (!IsIconic(hWnd)) && (!DSA_isBGBlack))
        {
            return 1;
        }
        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &paint);
        if ((!DSA_isBGFixed) && (DSA_BG_hDC != NULL) && (!IsIconic(hWnd)) && (!DSA_isBGBlack))
        {
            hPrevPal = SelectPalette(hDC, DSA_hPalette, FALSE);
            if (hPrevPal != NULL)
            {
                WinGBitBlt_c(
                    hDC,
                    paint.rcPaint.left,
                    paint.rcPaint.top,
                    paint.rcPaint.right - paint.rcPaint.left,
                    paint.rcPaint.bottom - paint.rcPaint.top,
                    DSA_BG_hDC,
                    paint.rcPaint.left,
                    paint.rcPaint.top
                );
                SelectPalette(hDC, hPrevPal, FALSE);
            }
        }
        EndPaint(hWnd, &paint);
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        PostQuitMessage(0);
        // change: remove unused
        //SYSTEM_QuitProgramFlag = 1;
        //byte_1001910C = 0;
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_MOUSEMOVE:
        // change: remove unused
        //BLEV_keyState = 0;
        //if (wParam & MK_CONTROL)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_CONTROL;
        //}
        //if (wParam & MK_LBUTTON)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_MOUSELEFT;
        //}
        //if (wParam & MK_RBUTTON)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_MOUSERIGHT;
        //}
        //if (wParam & MK_SHIFT)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_SHIFT;
        //}

        p_last_touched_screen = NULL;
        break;

    default:
        break;
    }

    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

static LRESULT WINAPI DSA_0000_Callback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BLEV_Event event;
    RECT rect;
    HDC hDC;
    LONG mappedPaletteEntries;
    HPALETTE hPrevPal;
    DSA_Screen *screen;
    LONG buttonState;
    LONG resizingMode;
    PAINTSTRUCT paint;

    uMsg = uMsg & 0xffff;

    screen = (DSA_Screen *)GetWindowLongA(hWnd, BBDSA_EXTRA_SCREEN);
    buttonState = GetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE);
    resizingMode = GetWindowLongA(hWnd, BBDSA_EXTRA_RESIZING_MODE);

    switch (uMsg)
    {
    case WM_SYSCOMMAND:
        // change: remove unused
        //if (!dword_100190AC)
        //{
        //    return 0;
        //}
        break;

    case WM_COMMAND:
        // wParam
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
        return DSA_MouseEvent_c(hWnd, uMsg, wParam, lParam, screen, buttonState);

    case WM_KEYDOWN:
    case WM_KEYUP:
        return DSA_KeyEvent_c(hWnd, uMsg, wParam, lParam, screen, buttonState);

    case WM_USER:
        SetWindowLongA(hWnd, BBDSA_EXTRA_RESIZING_MODE, wParam);
        return 0;

    case WM_ACTIVATE:
        switch (wParam)
        {
        case WA_INACTIVE:
            DSA_activeScreen = NULL;
            SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, 0);
            break;

        case WA_ACTIVE:
            DSA_activeScreen = screen;
            SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, 1);
            break;

        case WA_CLICKACTIVE:
            DSA_activeScreen = screen;
            SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, 0);
            break;
        }
        return 0;

    case WM_PALETTECHANGED:
        if (((HWND)wParam == hWnd) || IsIconic(DSA_screens[0].screen->hWnd))
        {
            return 0;
        }
        // @fallthrough@
    case WM_QUERYNEWPALETTE:
        mappedPaletteEntries = 0;
        hPrevPal = NULL;
        if (!IsIconic(DSA_screens[0].screen->hWnd))
        {
            mappedPaletteEntries = GetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING);
            if (mappedPaletteEntries <= 0)
            {
                if (DSA_hPalette != NULL)
                {
                    hDC = GetDC(hWnd);
                    if (hDC != NULL)
                    {
                        hPrevPal = SelectPalette(hDC, DSA_hPalette, FALSE);
                        if (hPrevPal != NULL)
                        {
                            mappedPaletteEntries = RealizePalette(hDC);
                            SelectPalette(hDC, hPrevPal, FALSE);
                            ReleaseDC(hWnd, hDC);
                            if (screen->hDC != NULL)
                            {
                                mappedPaletteEntries = WinGSetDIBColorTable_c(screen->hDC, 0, 256, DSA_colorTable);
                            }
                        }
                    }
                }
                return mappedPaletteEntries;
            }
        }
        SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, mappedPaletteEntries - 1);
        return 0;

    case WM_CREATE:
        SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, 0);
        SetWindowLongA(hWnd, BBDSA_EXTRA_RESIZING_MODE, 0);
        SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, 0);
        return 0;

    case WM_MOVE:
    case WM_SIZE:
        return 0;

    case WM_ERASEBKGND:
        if (!resizingMode)
        {
            return 1;
        }
        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &paint);
        if ((paint.rcPaint.right == paint.rcPaint.left) ||
            (paint.rcPaint.bottom == paint.rcPaint.top) ||
            resizingMode ||
            IsIconic(hWnd)
           )
        {
            if (resizingMode == 1)
            {
                GetClientRect(hWnd, &rect);
                SaveDC(hDC);

                SelectObject(hDC, GetStockObject(WHITE_PEN));

                MoveToEx(hDC, 0, rect.bottom - 1, 0);
                LineTo(hDC, 0, 0);
                LineTo(hDC, rect.right - 1, 0);

                MoveToEx(hDC, 1, rect.bottom - 2, 0);
                LineTo(hDC, 1, 1);
                LineTo(hDC, rect.right - 2, 1);

                MoveToEx(hDC, 0, rect.bottom - 1, 0);
                LineTo(hDC, rect.right - 1, rect.bottom - 1);
                LineTo(hDC, rect.right - 1, 0);

                MoveToEx(hDC, 1, rect.bottom - 2, 0);
                LineTo(hDC, rect.right - 2, rect.bottom - 2);
                LineTo(hDC, rect.right - 2, 1);

                RestoreDC(hDC, -1);
            }
        }
        else
        {
            hPrevPal = SelectPalette(hDC, DSA_hPalette, FALSE);
            if (hPrevPal != NULL)
            {
                WinGBitBlt_c(
                    hDC,
                    paint.rcPaint.left,
                    paint.rcPaint.top,
                    paint.rcPaint.right - paint.rcPaint.left,
                    paint.rcPaint.bottom - paint.rcPaint.top,
                    screen->hDC,
                    paint.rcPaint.left,
                    paint.rcPaint.top
                );
                SelectPalette(hDC, hPrevPal, FALSE);
            }
        }
        EndPaint(hWnd, &paint);
        return 0;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_MOUSEMOVE:
        // change: remove unused
        //BLEV_keyState = 0;
        //if (wParam & MK_CONTROL)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_CONTROL;
        //}
        //if (wParam & MK_LBUTTON)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_MOUSELEFT;
        //}
        //if (wParam & MK_RBUTTON)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_MOUSERIGHT;
        //}
        //if (wParam & MK_SHIFT)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_SHIFT;
        //}

        p_last_touched_screen = screen;
        if (screen->flags & BBDSA_UNKNOWN1)
        {
            event.type = BBBLEV_TYPE_MOUSEMOVE;
            event.screen = screen;
            event.buttonState = 0;
            event.key = 0;
            event.x = lParam & 0xffff;
            event.y = lParam >> 16;
            BLEV_PutEvent_c(&event);
        }
        return 0;

    default:
        break;
    }

    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

static LRESULT WINAPI DSA_0003_Callback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BLEV_Event event;
    RECT rect;
    HDC hDC;
    LONG mappedPaletteEntries;
    HPALETTE hPrevPal;
    DSA_Screen *screen;
    LONG buttonState;
    LONG resizingMode;
    PAINTSTRUCT paint;

    uMsg = uMsg & 0xffff;

    screen = (DSA_Screen *)GetWindowLongA(hWnd, BBDSA_EXTRA_SCREEN);
    buttonState = GetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE);
    resizingMode = GetWindowLongA(hWnd, BBDSA_EXTRA_RESIZING_MODE);

    switch (uMsg)
    {
    case WM_SYSCOMMAND:
        // change: remove unused
        //if (!dword_100190AC)
        //{
        //    return 0;
        //}
        break;

    case WM_COMMAND:
        // wParam
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
        return DSA_MouseEvent_c(hWnd, uMsg, wParam, lParam, screen, buttonState);

    case WM_KEYDOWN:
    case WM_KEYUP:
        return DSA_KeyEvent_c(hWnd, uMsg, wParam, lParam, screen, buttonState);

    case WM_USER:
        SetWindowLongA(hWnd, BBDSA_EXTRA_RESIZING_MODE, wParam);
        return 0;

    case WM_ACTIVATE:
        switch (wParam)
        {
        case WA_INACTIVE:
            DSA_activeScreen = NULL;
            SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, 0);
            break;

        case WA_ACTIVE:
            DSA_activeScreen = screen;
            SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, 1);
            break;

        case WA_CLICKACTIVE:
            DSA_activeScreen = screen;
            SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, 0);
            break;
        }
        return 0;

    case WM_PALETTECHANGED:
        if (((HWND)wParam == hWnd) || IsIconic(DSA_screens[0].screen->hWnd))
        {
            return 0;
        }
        // @fallthrough@
    case WM_QUERYNEWPALETTE:
        mappedPaletteEntries = 0;
        hPrevPal = NULL;
        if (!IsIconic(DSA_screens[0].screen->hWnd))
        {
            mappedPaletteEntries = GetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING);
            if (mappedPaletteEntries <= 0)
            {
                if (DSA_hPalette != NULL)
                {
                    hDC = GetDC(hWnd);
                    if (hDC != NULL)
                    {
                        hPrevPal = SelectPalette(hDC, DSA_hPalette, FALSE);
                        if (hPrevPal != NULL)
                        {
                            mappedPaletteEntries = RealizePalette(hDC);
                            SelectPalette(hDC, hPrevPal, FALSE);
                            ReleaseDC(hWnd, hDC);
                            if (screen->hDC != NULL)
                            {
                                mappedPaletteEntries = WinGSetDIBColorTable_c(screen->hDC, 0, 256, DSA_colorTable);
                            }
                        }
                    }
                }
                return mappedPaletteEntries;
            }
        }
        SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, mappedPaletteEntries - 1);
        return 0;

    case WM_CREATE:
        SetWindowLongA(hWnd, BBDSA_EXTRA_BUTTON_STATE, 0);
        SetWindowLongA(hWnd, BBDSA_EXTRA_RESIZING_MODE, 0);
        SetWindowLongA(hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, 0);
        return 0;

    case WM_MOVE:
    case WM_SIZE:
        return 0;

    case WM_ERASEBKGND:
        if (!resizingMode)
        {
            return 1;
        }
        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &paint);
        if ((paint.rcPaint.right == paint.rcPaint.left) ||
            (paint.rcPaint.bottom == paint.rcPaint.top) ||
            resizingMode ||
            IsIconic(hWnd)
           )
        {
            if (resizingMode == 1)
            {
                GetClientRect(hWnd, &rect);
                SaveDC(hDC);

                SelectObject(hDC, GetStockObject(WHITE_PEN));

                MoveToEx(hDC, 0, rect.bottom - 1, 0);
                LineTo(hDC, 0, 0);
                LineTo(hDC, rect.right - 1, 0);

                MoveToEx(hDC, 1, rect.bottom - 2, 0);
                LineTo(hDC, 1, 1);
                LineTo(hDC, rect.right - 2, 1);

                MoveToEx(hDC, 0, rect.bottom - 1, 0);
                LineTo(hDC, rect.right - 1, rect.bottom - 1);
                LineTo(hDC, rect.right - 1, 0);

                MoveToEx(hDC, 1, rect.bottom - 2, 0);
                LineTo(hDC, rect.right - 2, rect.bottom - 2);
                LineTo(hDC, rect.right - 2, 1);

                RestoreDC(hDC, -1);
            }
        }
        else
        {
            hPrevPal = SelectPalette(hDC, DSA_hPalette, FALSE);
            if (hPrevPal != NULL)
            {
                WinGBitBlt_c(
                    hDC,
                    paint.rcPaint.left,
                    paint.rcPaint.top,
                    paint.rcPaint.right - paint.rcPaint.left,
                    paint.rcPaint.bottom - paint.rcPaint.top,
                    screen->hDC,
                    paint.rcPaint.left,
                    paint.rcPaint.top
                );
                SelectPalette(hDC, hPrevPal, FALSE);
            }
        }
        EndPaint(hWnd, &paint);
        return 0;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_MOUSEMOVE:
        // change: remove unused
        //BLEV_keyState = 0;
        //if (wParam & MK_CONTROL)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_CONTROL;
        //}
        //if (wParam & MK_LBUTTON)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_MOUSELEFT;
        //}
        //if (wParam & MK_RBUTTON)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_MOUSERIGHT;
        //}
        //if (wParam & MK_SHIFT)
        //{
        //    BLEV_keyState |= BBBLEV_BUTTON_SHIFT;
        //}

        p_last_touched_screen = screen;
        if (screen->flags & BBDSA_UNKNOWN1)
        {
            event.type = BBBLEV_TYPE_MOUSEMOVE;
            event.screen = screen;
            event.buttonState = 0;
            event.key = 0;
            event.x = lParam & 0xffff;
            event.y = lParam >> 16;
            BLEV_PutEvent_c(&event);
        }
        return 0;

    default:
        break;
    }

    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

void DSA_GetScreenExtends_c(int32_t *width, int32_t *height)
{
    *width = GetSystemMetrics(SM_CXSCREEN);
    *height = GetSystemMetrics(SM_CYSCREEN);
}

void *DSA_GetActiveScreen_c(void)
{
    return DSA_activeScreen;
}

void DSA_SetActiveScreen_c(DSA_Screen *screen)
{
    LONG oldValue;

    if (screen == NULL)
    {
        screen = DSA_screens[0].screen;
    }

    oldValue = GetWindowLongA((HWND)screen->hWnd, BBDSA_EXTRA_PALETTE_REMAPPING);
    SetWindowLongA((HWND)screen->hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, oldValue + 1);

    if (IsIconic((HWND)DSA_screens[0].screen->hWnd))
    {
        DSA_nextActiveScreen = screen;
    }
    else
    {
        DSA_activeScreen = screen;
        SetActiveWindow((HWND)screen->hWnd);
        DSA_nextActiveScreen = NULL;
    }
}

void *DSA_GetLastTouchedScreen_c(void)
{
    return p_last_touched_screen;
}

void DSA_CopyPartOPMToScreen_c(DSA_Screen *screen, int x, int y, int width, int height)
{
    RECT rect;

    if (IsIconic((HWND)DSA_screens[0].screen->hWnd))
    {
        return;
    }

    rect.left = x;
    rect.top = y;
    rect.right = x + width;
    rect.bottom = y + height;

    InvalidateRect((HWND)screen->hWnd, &rect, FALSE);
    UpdateWindow((HWND)screen->hWnd);
}

int DSA_ScreenVisibility_c(DSA_Screen *screen, int show)
{
    BOOL res;

    res = FALSE;
    if (show & 0xff)
    {
        screen->flags |= BBDSA_VISIBLE;
        if (!IsIconic((HWND)DSA_screens[0].screen->hWnd))
        {
            res = ShowWindow((HWND)screen->hWnd, SW_SHOWNORMAL);
        }
    }
    else
    {
        screen->flags &= ~BBDSA_VISIBLE;
        res = ShowWindow((HWND)screen->hWnd, SW_HIDE);
    }

    return (res)?1:0;
}

int DSA_LoadBackground_c(const char *path)
{
    unsigned int bgWidth, bgHeight;
    void *lib;
    GFX_struct *gfx;

    lib = LBL_OpenLib_c(path, 1);
    if (lib == NULL)
    {
        return 0;
    }

    gfx = (GFX_struct *)LBL_ReadEntry_c(lib, NULL, 0, 1, NULL);
    if (gfx == NULL)
    {
        LBL_CloseLib_c(lib);
        // change: added return
        return 0;
    }

    LBL_CloseLib_c(lib);

    bgHeight = GetSystemMetrics(SM_CYBORDER) + sl_screenheight + 8;
    // change: fix bug
    //bgWidth = ((GetSystemMetrics(SM_CXBORDER) / 4) * 4) + sl_screenwidth + 4;
    bgWidth = ((GetSystemMetrics(SM_CXBORDER) / 4) * 4) + ((sl_screenwidth + 3) & ~3) + 4;
    if (!OPM_New_c(bgWidth, bgHeight, 1, &DSA_pixelMapBG, NULL))
    {
        MEM_free_c(gfx);
        return 0;
    }

    DSA_BG_hDC = (HDC)DSA_pixelMapBG.hDC;
    DSA_BG_hBitmap = (HGDIOBJ)DSA_pixelMapBG.hBitmap;
    DSA_BG_buffer = DSA_pixelMapBG.buffer;

    if (gfx->width <= sl_screenwidth)
    {
        memset(DSA_pixelMapBG.buffer, 0, DSA_pixelMapBG.height * DSA_pixelMapBG.width);
        OPM_CopyGFXOPM_c(&DSA_pixelMapBG, gfx, (sl_screenwidth / 2) - (gfx->width >> 1), sl_screenheight - gfx->height, 0);
        DSA_BG_offsetX = (sl_screenwidth / 2) - (gfx->width >> 1);
        DSA_BG_offsetY = sl_screenheight - gfx->height;
    }
    else
    {
        OPM_CopyGFXOPM_c(&DSA_pixelMapBG, gfx, -((gfx->width - sl_screenwidth) / 2), 1 - (gfx->height - sl_screenheight), 0);
        DSA_BG_offsetX = -((gfx->width - sl_screenwidth) / 2);
        DSA_BG_offsetY = 1 - (gfx->height - sl_screenheight);
    }

    MEM_free_c(gfx);

    DSA_screens[0].screen->hDC = DSA_BG_hDC;
    DSA_screens[0].screen->opm = &DSA_pixelMapBG;
    // change: fix error
    //DSA_screens[0].screen->hBitmap = DSA_pixelMapBG.buffer;
    DSA_screens[0].screen->hBitmap = DSA_pixelMapBG.hBitmap;
    DSA_screens[0].screen->size = DSA_pixelMapBG.size;

    InvalidateRect((HWND)DSA_screens[0].screen->hWnd, NULL, FALSE);
    UpdateWindow((HWND)DSA_screens[0].screen->hWnd);

    return 1;
}

void DSA_GetPalette_c(DSA_Palette *palette)
{
    memmove(palette, &DSA_palette, sizeof(DSA_Palette));
}

void DSA_FixBackground_c(int isFixed)
{
    isFixed = isFixed & 0xff;

    DSA_isBGFixed = isFixed;
    if (!isFixed)
    {
        InvalidateRect((HWND)DSA_screens[0].screen->hWnd, NULL, FALSE);
        SYSTEM_SystemTask_c();
    }
}

void DSA_SetCapture_c(DSA_Screen *screen)
{
    if (screen != NULL)
    {
        SetCapture((HWND)screen->hWnd);
    }
    else
    {
        ReleaseCapture();
    }
}

void DSA_SystemTask_c(void)
{
    int isVisible, index;

    isVisible = (IsIconic((HWND)DSA_screens[0].screen->hWnd) == 0)?1:0;

    if (!isVisible && DSA_wasVisible)
    {
        InvalidateRect(NULL, NULL, TRUE);
    }
    else
    {
        if (isVisible || DSA_wasVisible)
        {
            if (isVisible && !DSA_wasVisible)
            {
                DSA_ActivatePal_c();

                for (index = 1; index < 64; index++)
                {
                    if (DSA_screens[index].screen != NULL)
                    {
                        if (DSA_screens[index].screen->flags & BBDSA_VISIBLE)
                        {
                            ShowWindow((HWND)DSA_screens[index].screen->hWnd, SW_SHOWNORMAL);
                        }
                        else
                        {
                            ShowWindow((HWND)DSA_screens[index].screen->hWnd, SW_HIDE);
                        }
                    }
                }

                DSA_ActivatePal_c();

                if (DSA_nextActiveScreen != NULL)
                {
                    DSA_SetActiveScreen_c(DSA_nextActiveScreen);
                    DSA_nextActiveScreen = NULL;
                }

                DSA_TotalRepaint_c();
            }
            else
            {
                if ((DSA_BG_hDC != NULL) && DSA_isBGInRAM)
                {
                    OPM_AccessBitmap_c(&DSA_pixelMapBG);
                }
            }
        }
    }

    DSA_wasVisible = isVisible;
}

int DSA_SetBackgroundInRAM_c(int value)
{
    int oldvalue;

    oldvalue = DSA_isBGInRAM;
    DSA_isBGInRAM = value & 0xff;
    return oldvalue;
}

void DSA_SetPal_c(int unused, DSA_Palette *palette, unsigned int src_start_entry, unsigned int num_entries, unsigned int dst_start_entry)
{
    unsigned int src_index, dst_index, counter;

    src_index = src_start_entry & 0xffff;
    dst_index = dst_start_entry & 0xffff;
    num_entries = num_entries & 0xffff;

    for (counter = 0; counter < num_entries && src_index < palette->palNumEntries && dst_index < 256; counter++)
    {
        if (palette->palPalEntry[src_index].peRed == 0)
        {
            palette->palPalEntry[src_index].peRed = 1;
        }
        if (palette->palPalEntry[src_index].peRed == 255)
        {
            palette->palPalEntry[src_index].peRed = 254;
        }
        if (palette->palPalEntry[src_index].peGreen == 0)
        {
            palette->palPalEntry[src_index].peGreen = 1;
        }
        if (palette->palPalEntry[src_index].peGreen == 255)
        {
            palette->palPalEntry[src_index].peGreen = 254;
        }
        if (palette->palPalEntry[src_index].peBlue == 0)
        {
            palette->palPalEntry[src_index].peBlue = 1;
        }
        if (palette->palPalEntry[src_index].peBlue == 255)
        {
            palette->palPalEntry[src_index].peBlue = 254;
        }

        DSA_palette.palPalEntry[dst_index].peRed = palette->palPalEntry[src_index].peRed;
        DSA_palette.palPalEntry[dst_index].peGreen = palette->palPalEntry[src_index].peGreen;
        DSA_palette.palPalEntry[dst_index].peBlue = palette->palPalEntry[src_index].peBlue;

        DSA_paletteEntries[dst_index].peRed = palette->palPalEntry[src_index].peRed;
        DSA_paletteEntries[dst_index].peGreen = palette->palPalEntry[src_index].peGreen;
        DSA_paletteEntries[dst_index].peBlue = palette->palPalEntry[src_index].peBlue;

        DSA_colorTable[dst_index].rgbRed = palette->palPalEntry[src_index].peRed;
        DSA_colorTable[dst_index].rgbGreen = palette->palPalEntry[src_index].peGreen;
        DSA_colorTable[dst_index].rgbBlue = palette->palPalEntry[src_index].peBlue;

        src_index++;
        dst_index++;
    }

    DSA_palette.palPalEntry[255].peRed = 255;
    DSA_palette.palPalEntry[255].peGreen = 255;
    DSA_palette.palPalEntry[255].peBlue = 255;
    DSA_palette.palPalEntry[0].peRed = 0;
    DSA_palette.palPalEntry[0].peGreen = 0;
    DSA_palette.palPalEntry[0].peBlue = 0;

    DSA_paletteEntries[255].peRed = 255;
    DSA_paletteEntries[255].peGreen = 255;
    DSA_paletteEntries[255].peBlue = 255;
    DSA_paletteEntries[0].peRed = 0;
    DSA_paletteEntries[0].peGreen = 0;
    DSA_paletteEntries[0].peBlue = 0;

    DSA_colorTable[255].rgbRed = 255;
    DSA_colorTable[255].rgbGreen = 255;
    DSA_colorTable[255].rgbBlue = 255;
    DSA_colorTable[0].rgbRed = 0;
    DSA_colorTable[0].rgbGreen = 0;
    DSA_colorTable[0].rgbBlue = 0;
}

void DSA_ActivatePal_c(void)
{
    int index;

    if (DSA_hPalette == NULL)
    {
        return;
    }

    for (index = 0; index < 256; index++)
    {
        DSA_paletteEntries[index].peFlags = PC_NOCOLLAPSE;
        DSA_colorTable[index].rgbReserved = 0;
    }

    DSA_paletteEntries[0].peFlags = 0;
    DSA_paletteEntries[255].peFlags = 0;

    SetPaletteEntries(DSA_hPalette, 0, 256, DSA_paletteEntries);

    if (SYSTEM_IsApplicationActive_c())
    {
        for (index = 0; index < 64; index++)
        {
            if (DSA_screens[index].used)
            {
                SendMessageA((HWND)DSA_screens[index].screen->hWnd, WM_QUERYNEWPALETTE, 0, 0);
            }
        }
    }
}

static void DSA_ActivateApp_c(int isActive)
{
    int index;
    HDC hDC;

    hDC = GetDC(NULL);

    if (isActive && (GetSystemPaletteUse(hDC) == SYSPAL_STATIC))
    {
        for (index = 0; index < 19; index++)
        {
            DSA_systemColors[index] = GetSysColor(DSA_elements[index]);
        }

        ClipCursor(NULL);

        if (DSA_bitsPerPixel == 8)
        {
            SetSystemPaletteUse(hDC, SYSPAL_NOSTATIC);
            SetSysColors(19, DSA_elements, DSA_elementColors);
        }

        DSA_MapPalette_c(NULL);
        FX_ReserveDevices_c(1);
        b_application_active = 1;
    }
    else if (!isActive)
    {
        DSA_MapPalette_c(DSA_systemPalette);

        if (DSA_bitsPerPixel == 8)
        {
            SetSystemPaletteUse(hDC, SYSPAL_STATIC);
            SetSysColors(19, DSA_elements, DSA_systemColors);
        }

        ClipCursor(NULL);
        FX_ReserveDevices_c(0);
        ShowWindow(DSAWIN_GetMainWindowHandle_c(), SW_MINIMIZE);
        SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
        DSA_deactivate = 0;
        b_application_active = 0;
        InvalidateRect(NULL, NULL, TRUE);
    }

    ReleaseDC(NULL, hDC);
}

static void DSA_SetColorTable_c(void)
{
    OPM_struct *opm;

    LL_Reset_c(DSA_pixelMapList);
    while (1)
    {
        opm = (OPM_struct *)LL_GetData_c(DSA_pixelMapList);
        if (opm == NULL)
        {
            break;
        }

        if (opm->hDC != NULL)
        {
            WinGSetDIBColorTable_c(opm->hDC, 0, 256, DSA_colorTable);
        }

        LL_GotoNext_c(DSA_pixelMapList);
    }
}

void DSA_PreventPaletteRemapping_c(int valueAdd)
{
    int index;
    LONG oldValue;

    if (valueAdd < 0)
    {
        return;
    }

    for (index = 0; index < 64; index++)
    {
        if (DSA_screens[index].used)
        {
            oldValue = GetWindowLongA((HWND)DSA_screens[index].screen->hWnd, BBDSA_EXTRA_PALETTE_REMAPPING);
            SetWindowLongA((HWND)DSA_screens[index].screen->hWnd, BBDSA_EXTRA_PALETTE_REMAPPING, oldValue + valueAdd);
        }
    }
}

int DSA_GetBackgroundOffset_c(int32_t *offsetX, int32_t *offsetY)
{
    if (DSA_BG_hDC == NULL)
    {
        return 0;
    }

    *offsetX = DSA_BG_offsetX;
    *offsetY = DSA_BG_offsetY;
    return 1;
}

void DSA_SetBackground2Black_c(int isBlack)
{
    DSA_isBGBlack = isBlack & 0xff;
    InvalidateRect((HWND)DSA_screens[0].screen->hWnd, NULL, TRUE);
    UpdateWindow((HWND)DSA_screens[0].screen->hWnd);
}

int DSA_MarkBitmapAsDirty_c(int param1, int param2)
{
    if (param2)
    {
        return DSA_isBitmapDirty;
    }
    else
    {
        DSA_isBitmapDirty = 1;
        return 0xAD03;
    }
}

