/**
 *
 *  Copyright (C) 2023-2025 Roman Pauer
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

#include "Game-Display.h"
#include "Game-Config.h"
#include "platform.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>


static int restore_resolution;
static char primary_device_name[32];
static DWORD original_width, original_height, original_bpp, original_frequency, original_flags;


NOINLINE void SetResolution(void);
void SetResolution(void)
{
    DISPLAY_DEVICEA display_device;
    DEVMODEA devmode;
    DWORD index;
    DWORD matched_bpp, matched_frequency, matched_flags;

    restore_resolution = 0;

    if ((Display_Width <= 0) || (Display_Height <= 0))
    {
        // resolution change not requested
        return;
    }

    display_device.cb = sizeof(DISPLAY_DEVICEA);
    for (index = 0; ; index++)
    {
        if (!EnumDisplayDevicesA(NULL, index, &display_device, 0))
        {
            // primary display adapter not found - shouldn't happen
            return;
        }

        if (display_device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
        {
            // primary display adapter found
            memcpy(primary_device_name, display_device.DeviceName, 32);
            break;
        }
    }

    devmode.dmSize = sizeof(DEVMODEA);
    devmode.dmDriverExtra = 0;
    if (!EnumDisplaySettingsA(primary_device_name, ENUM_CURRENT_SETTINGS, &devmode))
    {
        // error getting current display settings
        return;
    }

    if ((Display_Width == devmode.dmPelsWidth) &&
        (Display_Height == devmode.dmPelsHeight) &&
        ((Display_BitsPerPixel == 0) || (Display_BitsPerPixel == devmode.dmBitsPerPel)) &&
        ((Display_Frequency == 0) || (Display_Frequency == devmode.dmDisplayFrequency))
       )
    {
        // display already at requested resolution
        Display_BitsPerPixel = devmode.dmBitsPerPel;
        Display_Frequency = devmode.dmDisplayFrequency;
        fprintf(stderr, "%s: %s: %ix%i, %i bit, %i Hz\n", "display", "resolution already set", Display_Width, Display_Height, Display_BitsPerPixel, Display_Frequency);
        return;
    }

    original_width = devmode.dmPelsWidth;
    original_height = devmode.dmPelsHeight;
    original_bpp = devmode.dmBitsPerPel;
    original_frequency = devmode.dmDisplayFrequency;
    original_flags = devmode.dmDisplayFlags;

    matched_bpp = 0;
    matched_frequency = 0;
    matched_flags = 0;
    for (index = 0; ; index++)
    {
        if (!EnumDisplaySettingsA(primary_device_name, index, &devmode)) break;

        if (Display_Width != devmode.dmPelsWidth) continue;
        if (Display_Height != devmode.dmPelsHeight) continue;
        if ((Display_BitsPerPixel != 0) && (Display_BitsPerPixel != devmode.dmBitsPerPel)) continue;
        if ((Display_Frequency != 0) && (Display_Frequency != devmode.dmDisplayFrequency)) continue;

        if (matched_bpp != 0)
        do {
            if ((Display_BitsPerPixel == 0) && (matched_bpp != devmode.dmBitsPerPel))
            {
                if (((matched_bpp > original_bpp) && (devmode.dmBitsPerPel < matched_bpp)) ||
                    ((matched_bpp < original_bpp) && (devmode.dmBitsPerPel > matched_bpp) && (devmode.dmBitsPerPel <= original_bpp))
                   )
                {
                    // better match
                    matched_bpp = 0;
                }

                break;
            }

            if ((Display_Frequency == 0) && (matched_frequency != devmode.dmDisplayFrequency))
            {
                if (((matched_frequency > original_frequency) && (devmode.dmDisplayFrequency < matched_frequency)) ||
                    ((matched_frequency < original_frequency) && (devmode.dmDisplayFrequency > matched_frequency) && (devmode.dmDisplayFrequency <= original_frequency))
                   )
                {
                    // better match
                    matched_bpp = 0;
                }

                break;
            }

            if (matched_flags != devmode.dmDisplayFlags)
            {
                if (devmode.dmDisplayFlags == original_flags)
                {
                    // better match
                    matched_bpp = 0;
                }

                break;
            }
        } while (0);

        if (matched_bpp == 0)
        {
            matched_bpp = devmode.dmBitsPerPel;
            matched_frequency = devmode.dmDisplayFrequency;
            matched_flags = devmode.dmDisplayFlags;

            if ((matched_bpp == original_bpp) && (matched_frequency == original_frequency) && (matched_flags == original_flags))
            {
                // best match
                break;
            }
        }
    }

    if (matched_bpp == 0)
    {
        // requested resolution not supported
        fprintf(stderr, "%s: %s: %ix%i\n", "display", "resolution not supported", Display_Width, Display_Height);
        return;
    }

    Display_BitsPerPixel = matched_bpp;
    Display_Frequency = matched_frequency;

    devmode.dmPelsWidth = Display_Width;
    devmode.dmPelsHeight = Display_Height;
    devmode.dmBitsPerPel = matched_bpp;
    devmode.dmDisplayFrequency = matched_frequency;
    devmode.dmDisplayFlags = matched_flags;
    devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;

    if (ChangeDisplaySettingsExA(primary_device_name, &devmode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
    {
        // resolution change not succesful
        fprintf(stderr, "%s: %s: %ix%i, %i bit, %i Hz\n", "display", "error setting resolution", Display_Width, Display_Height, Display_BitsPerPixel, Display_Frequency);
        return;
    }

    // resolution change succesful
    restore_resolution = 1;
    fprintf(stderr, "%s: %s: %ix%i, %i bit, %i Hz\n", "display", "error setting resolution", Display_Width, Display_Height, Display_BitsPerPixel, Display_Frequency);
}

NOINLINE void RestoreResolution(void);
void RestoreResolution(void)
{
    DEVMODEA devmode;

    if (!restore_resolution) return;

    devmode.dmSize = sizeof(DEVMODEA);
    devmode.dmDriverExtra = 0;
    devmode.dmPelsWidth = original_width;
    devmode.dmPelsHeight = original_height;
    devmode.dmBitsPerPel = original_bpp;
    devmode.dmDisplayFrequency = original_frequency;
    devmode.dmDisplayFlags = original_flags;
    devmode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY | DM_DISPLAYFLAGS;

    if (ChangeDisplaySettingsExA(primary_device_name, &devmode, NULL, CDS_FULLSCREEN, NULL) == DISP_CHANGE_SUCCESSFUL)
    {
        // resolution change succesful
        restore_resolution = 0;
    }
}

