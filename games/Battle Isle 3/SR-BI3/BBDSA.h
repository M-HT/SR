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

#if !defined(_BBDSA_H_INCLUDED_)
#define _BBDSA_H_INCLUDED_

#include <stdint.h>
#include "BBOPM.h"


#pragma pack(1)

typedef struct __attribute__ ((__packed__)) _DSA_Screen {
    uint32_t unknown_00;
    OPM_struct *opm;
    uint32_t unknown_08;
    uint32_t type;
    void *hWnd;
    uint32_t flags;
    void *hDC;
    void *hBitmap;
    uint32_t size;
} DSA_Screen;

typedef struct __attribute__ ((__packed__)) _DSA_Palette {
    uint16_t palNumEntries;
    uint16_t palVersion;
    struct {
        uint8_t peRed;
        uint8_t peGreen;
        uint8_t peBlue;
        uint8_t peFlags;
    } palPalEntry[256];
} DSA_Palette;

#pragma pack()


#ifdef __cplusplus
extern "C" {
#endif

void DSA_ReuseStaticColors_c(unsigned int osVersions);
void DSA_FreeStaticColors_c(unsigned int osVersions);
void DSAWIN_PrepareInit_c(void *hInstance);
void *DSAWIN_GetMainWindowHandle_c(void);
void *DSAWIN_GetInstance_c(void);
int DSA_Init_c(void);
void DSA_Exit_c(void);
int DSA_OpenScreen_c(DSA_Screen *screen, OPM_struct *opm, int param3, const char *windowName, int x, int y, int type);
void DSA_CloseScreen_c(DSA_Screen *screen);
void DSA_CopyMainOPMToScreen_c(DSA_Screen *screen, int onlyModified);
void DSA_StretchOPMToScreen_c(DSA_Screen *screen, int xDst, int yDst, int widthDst, int heightDst, OPM_struct *opm, int xSrc, int ySrc, int widthSrc, int heightSrc);
void DSA_CopyOPMToScreenEx_c(DSA_Screen *screen, int xDst, int yDst, int width, int height, OPM_struct *opm, int xSrc, int ySrc);
void DSA_TotalRepaint_c(void);
void DSA_MoveScreen_c(DSA_Screen *screen, int changeX, int changeY);
int DSA_ResizeScreen_c(DSA_Screen *screen, OPM_struct *opm, int redraw);
int DSA_DrawSizingScreen_c(DSA_Screen *screen, int16_t *rect);
void DSA_EnterResizingMode_c(DSA_Screen *screen);
void DSA_LeaveResizingMode_c(DSA_Screen *screen);
void DSA_GetDSAMetrics_c(DSA_Screen *screen, int32_t *x, int32_t *y, int32_t *width, int32_t *height, uint8_t *isVisible);
void DSA_SetDSAPos_c(DSA_Screen *screen, int x, int y, int repaint);
void DSA_GetScreenExtends_c(int32_t *width, int32_t *height);
void *DSA_GetActiveScreen_c(void);
void DSA_SetActiveScreen_c(DSA_Screen *screen);
void *DSA_GetLastTouchedScreen_c(void);
void DSA_CopyPartOPMToScreen_c(DSA_Screen *screen, int x, int y, int width, int height);
int DSA_ScreenVisibility_c(DSA_Screen *screen, int show);
int DSA_LoadBackground_c(const char *path);
void DSA_GetPalette_c(DSA_Palette *palette);
void DSA_FixBackground_c(int isFixed);
void DSA_SetCapture_c(DSA_Screen *screen);
void DSA_SystemTask_c(void);
int DSA_SetBackgroundInRAM_c(int value);
void DSA_SetPal_c(int unused, DSA_Palette *palette, unsigned int src_start_entry, unsigned int num_entries, unsigned int dst_start_entry);
void DSA_ActivatePal_c(void);
void DSA_PreventPaletteRemapping_c(int valueAdd);
int DSA_GetBackgroundOffset_c(int32_t *offsetX, int32_t *offsetY);
void DSA_SetBackground2Black_c(int isBlack);
int DSA_MarkBitmapAsDirty_c(int param1, int param2);

#ifdef __cplusplus
}
#endif

#endif /* _BBDSA_H_INCLUDED_ */
