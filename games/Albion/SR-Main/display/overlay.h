/**
 *
 *  Copyright (C) 2016-2022 Roman Pauer
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

static int Get_DrawOverlay(uint8_t *src, Game_OverlayInfo *OverlayInfo)
{
    int DrawOverlay;

    DrawOverlay = 0;
    if ((OverlayInfo->Enhanced3DEngineUsed >= 0) && (OverlayInfo->ViewportWidth != 0))
    {
        uint8_t *ptr;
        uint32_t counter;
        int DrawOverlay1, DrawOverlay2;

        if (OverlayInfo->Enhanced3DEngineUsed) DrawOverlay = 4;

        DrawOverlay1 = 1;
        ptr = &(src[(OverlayInfo->ViewportY + OverlayInfo->ViewportHeight - 2) * 360 + OverlayInfo->ViewportX + 1]);
        for (counter = 0; counter < 8; counter++)
        {
            if (*ptr != counter + 19)
            {
                DrawOverlay1 = 0;
                break;
            }
            ptr++;
        }

        DrawOverlay2 = 1;
        ptr = &(src[(OverlayInfo->ViewportY + OverlayInfo->ViewportHeight - 2) * 360 + OverlayInfo->ViewportX + OverlayInfo->ViewportWidth - 10]);
        for (counter = 0; counter < 8; counter++)
        {
            if (*ptr != counter + 31)
            {
                DrawOverlay2 = 0;
                break;
            }
            ptr++;
        }

        if (DrawOverlay1) DrawOverlay |= 1;
        if (DrawOverlay2) DrawOverlay |= 2;
    }

    return DrawOverlay;
}

