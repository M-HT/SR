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

#if defined(__WINE__) || !defined(OLDVIDEO)
#define USE_QUICKTIMELIB
#else
#undef USE_QUICKTIMELIB
#endif

#include "BBAVI.h"
#include "BBDSA.h"
#include "BBMEM.h"
#include "BBSYSTEM.h"
#include "BBDBG.h"
#include "BBFX.h"
#include "BBBLEV.h"
#if defined(USE_QUICKTIMELIB)
#include <lqt/lqt.h>
#include <lqt/colormodels.h>
#include <stdlib.h>
#endif
#include <windows.h>
#if !defined(USE_QUICKTIMELIB)
#include <digitalv.h>
#include <mciavi.h>
#endif


// exported variables
#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t w_avi_device_id;
#ifdef __cplusplus
}
#endif


extern HPALETTE DSA_hPalette;


typedef struct {
    HWND hWnd;
    char alias[30];
#if defined(USE_QUICKTIMELIB)
    uint16_t reserved;

    quicktime_t *qt;

    int play_video;
    int video_width;
    int video_height;
    int video_depth;
    int64_t video_duration;
    long video_frames;
    int video_time_scale;
    int colormodel;
    int read_compressed_video;
    int palette_colors;

    int play_audio;
    int audio_channels;
    long audio_sample_rate;
    int audio_format_size;
    int decode_raw_audio;
    int64_t audio_length;

    RGBQUAD video_palette[256];
#else
    MCIDEVICEID wDeviceID;
#endif
    DSA_Palette palette1;
    DSA_Palette palette2;
} AVI_Video;


static int AVI_initialized = 0;
static int AVI_wasApplicationActive = 0;
static int AVI_destortionLevel = 0;
static int AVI_numVideos = 0;
static const char AVI_windowClass[] = "BB_AVICALLBACK";
static AVI_Video *AVI_videos[5];
static int32_t AVI_desktopWidth;
static volatile int AVI_cancelPlayback;
static int AVI_stopPlayback;
static volatile int AVI_playbackFinished;
static int32_t AVI_desktopHeight;


#if defined(USE_QUICKTIMELIB)

static int compare_fourcc(const uint8_t *str1, const uint8_t *str2)
{
    if ((str1[0] == str2[0]) &&
        (str1[1] == str2[1]) &&
        (str1[2] == str2[2]) &&
        (str1[3] == str2[3])
       )
    {
        return 1;
    }
    return 0;
}

static uint32_t read_32le(const uint8_t *ptr)
{
    return ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
}

static void close_video(AVI_Video *video)
{
    if (video->qt != NULL)
    {
        quicktime_close(video->qt);
        video->qt = NULL;
    }
}

static int open_video(const char *path, AVI_Video *video)
{
#if defined(__WINE__)
    WCHAR pathW[MAX_PATH];
    char *pathUnix;
#endif

    static const uint8_t fourcc_rle8[4] = {1, 0, 0, 0};

#if defined(__WINE__)
    MultiByteToWideChar(CP_ACP, 0, path, -1, pathW, MAX_PATH);
    pathUnix = wine_get_unix_file_name(pathW);

    video->qt = lqt_open_read(pathUnix);
#else
    video->qt = lqt_open_read(path);
#endif
    if (video->qt == NULL) return 1;

    if (quicktime_video_tracks(video->qt) <= 0)
    {
        close_video(video);
        return 2;
    }

    video->play_video = 0;
    if (quicktime_supported_video(video->qt, 0))
    {
        video->play_video = 1;
        video->read_compressed_video = 0;
    }
    else if (compare_fourcc((const uint8_t *)quicktime_video_compressor(video->qt, 0), fourcc_rle8))
    {
        video->play_video = 1;
        video->read_compressed_video = 1;
    }

    video->video_time_scale = lqt_video_time_scale(video->qt, 0);
    video->video_duration = lqt_video_duration(video->qt, 0);
    video->video_frames = quicktime_video_length(video->qt, 0);

    if (video->play_video)
    {
        video->video_width = quicktime_video_width(video->qt, 0);
        video->video_height = quicktime_video_height(video->qt, 0);
        video->video_depth = quicktime_video_depth(video->qt, 0);

        if (video->read_compressed_video)
        {
            video->colormodel = BC_BGR8888;
        }
        else
        {
            int color_models[3];

            color_models[0] = BC_BGR888;
            color_models[1] = BC_BGR8888;
            color_models[2] = LQT_COLORMODEL_NONE;
            video->colormodel = lqt_get_best_colormodel(video->qt, 0, color_models);

            lqt_set_cmodel(video->qt, 0, video->colormodel);
        }
    }

    video->play_audio = 0;
    if (quicktime_audio_tracks(video->qt) > 0)
    {
        if (quicktime_supported_audio(video->qt, 0))
        {
            video->audio_channels = quicktime_track_channels(video->qt, 0);
            video->audio_sample_rate = quicktime_sample_rate(video->qt, 0);

            if (((video->audio_channels == 1) ||
                 (video->audio_channels == 2)
                ) &&
                (video->audio_sample_rate > 0) &&
                (video->audio_sample_rate <= 48000)
               )
            {
                video->play_audio = 1;
            }
        }
    }

    if (video->play_audio)
    {
        lqt_sample_format_t format;

        format = lqt_get_sample_format(video->qt, 0);
        if ((format == LQT_SAMPLE_UINT8) || (format == LQT_SAMPLE_INT16))
        {
            video->audio_format_size = (format == LQT_SAMPLE_UINT8)?1:2;
            video->decode_raw_audio = 1;
        }
        else
        {
            video->audio_format_size = 2;
            video->decode_raw_audio = 0;
        }

        video->audio_length = quicktime_audio_length(video->qt, 0);
    }

    return 0;
}

static int read_palette(const char *path, AVI_Video *video)
{
    int index, is_video, num_colors;
    HANDLE hAvi;
    uint8_t buffer[12];
    DWORD dwBytesRead;
    unsigned int cur_riff_offset, max_riff_offset, chunk_size;
    unsigned int cur_hdrl_offset, max_hdrl_offset;
    unsigned int cur_strl_offset, max_strl_offset;
    BITMAPINFOHEADER bmih;

    static const uint8_t fourcc_riff[4] = {'R', 'I', 'F', 'F'};
    static const uint8_t fourcc_avi[4]  = {'A', 'V', 'I', ' '};
    static const uint8_t fourcc_list[4] = {'L', 'I', 'S', 'T'};
    static const uint8_t fourcc_hdrl[4] = {'h', 'd', 'r', 'l'};
    static const uint8_t fourcc_strl[4] = {'s', 't', 'r', 'l'};
    static const uint8_t fourcc_strh[4] = {'s', 't', 'r', 'h'};
    static const uint8_t fourcc_strf[4] = {'s', 't', 'r', 'f'};
    static const uint8_t fourcc_vids[4] = {'v', 'i', 'd', 's'};

    video->palette_colors = 0;
    for (index = 0; index < 256; index++)
    {
        video->video_palette[index].rgbBlue = 0;
        video->video_palette[index].rgbGreen = 0;
        video->video_palette[index].rgbRed = 0;
        video->video_palette[index].rgbReserved = 0;
    }

    if (!video->play_video) return 0;
    if ((video->video_depth <= 0) || (video->video_depth > 8)) return 0;

    hAvi = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
    if (hAvi == INVALID_HANDLE_VALUE) return (video->read_compressed_video)?1:0;

    if (!ReadFile(hAvi, buffer, 12, &dwBytesRead, NULL) || (dwBytesRead != 12)) goto read_error;
    if (!compare_fourcc(buffer, fourcc_riff) || !compare_fourcc(buffer + 8, fourcc_avi)) goto read_error;

    cur_riff_offset = 12;
    max_riff_offset = 8 + read_32le(buffer + 4);

    // loop over 1st level chunks
    while (cur_riff_offset < max_riff_offset)
    {
        if (INVALID_SET_FILE_POINTER == SetFilePointer(hAvi, cur_riff_offset, NULL, FILE_BEGIN)) goto read_error;
        if (!ReadFile(hAvi, buffer, 8, &dwBytesRead, NULL) || (dwBytesRead != 8)) goto read_error;

        if (!compare_fourcc(buffer, fourcc_list))
        {
            chunk_size = read_32le(buffer + 4);
            cur_riff_offset += 8 + chunk_size + (chunk_size & 1);
            continue;
        }

        if (!ReadFile(hAvi, buffer + 8, 4, &dwBytesRead, NULL) || (dwBytesRead != 4)) goto read_error;

        if (!compare_fourcc(buffer + 8, fourcc_hdrl))
        {
            chunk_size = read_32le(buffer + 4);
            cur_riff_offset += 8 + chunk_size + (chunk_size & 1);
            continue;
        }

        chunk_size = read_32le(buffer + 4);
        cur_hdrl_offset = cur_riff_offset + 12;
        max_hdrl_offset = cur_riff_offset + 8 + chunk_size;
        cur_riff_offset += 8 + chunk_size + (chunk_size & 1);

        // loop over 2nd level chunks
        while (cur_hdrl_offset < max_hdrl_offset)
        {
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hAvi, cur_hdrl_offset, NULL, FILE_BEGIN)) goto read_error;
            if (!ReadFile(hAvi, buffer, 8, &dwBytesRead, NULL) || (dwBytesRead != 8)) goto read_error;

            if (!compare_fourcc(buffer, fourcc_list))
            {
                chunk_size = read_32le(buffer + 4);
                cur_hdrl_offset += 8 + chunk_size + (chunk_size & 1);
                continue;
            }

            if (!ReadFile(hAvi, buffer + 8, 4, &dwBytesRead, NULL) || (dwBytesRead != 4)) goto read_error;

            if (!compare_fourcc(buffer + 8, fourcc_strl))
            {
                chunk_size = read_32le(buffer + 4);
                cur_hdrl_offset += 8 + chunk_size + (chunk_size & 1);
                continue;
            }

            chunk_size = read_32le(buffer + 4);
            cur_strl_offset = cur_hdrl_offset + 12;
            max_strl_offset = cur_hdrl_offset + 8 + chunk_size;
            cur_hdrl_offset += 8 + chunk_size + (chunk_size & 1);

            is_video = 0;

            // loop over 3rd level chunks
            while (cur_strl_offset < max_strl_offset)
            {
                if (INVALID_SET_FILE_POINTER == SetFilePointer(hAvi, cur_strl_offset, NULL, FILE_BEGIN)) goto read_error;
                if (!ReadFile(hAvi, buffer, 8, &dwBytesRead, NULL) || (dwBytesRead != 8)) goto read_error;

                if (compare_fourcc(buffer, fourcc_strh))
                {
                    if (!ReadFile(hAvi, buffer + 8, 4, &dwBytesRead, NULL) || (dwBytesRead != 4)) goto read_error;

                    if (!compare_fourcc(buffer + 8, fourcc_vids)) break;

                    is_video = 1;
                }
                else if (compare_fourcc(buffer, fourcc_strf) && is_video)
                {
                    if (!ReadFile(hAvi, &bmih, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL) || (dwBytesRead != sizeof(BITMAPINFOHEADER))) goto read_error;

                    if (bmih.biSize != sizeof(BITMAPINFOHEADER)) break;

                    if ((bmih.biCompression == BI_RGB) && (bmih.biBitCount <= 8))
                    {
                        num_colors = bmih.biClrUsed;
                        if (bmih.biClrUsed == 0)
                        {
                            bmih.biClrUsed = 1 << bmih.biBitCount;
                        }
                    }
                    else if ((bmih.biCompression == BI_RLE8) || (bmih.biCompression == BI_RLE4))
                    {
                        num_colors = bmih.biClrUsed;
                    }
                    else num_colors = 0;

                    if (num_colors)
                    {
                        if (!ReadFile(hAvi, video->video_palette, num_colors * sizeof(RGBQUAD), &dwBytesRead, NULL) || (dwBytesRead != num_colors * sizeof(RGBQUAD))) goto read_error;

                        video->palette_colors = num_colors;
                    }
                }

                chunk_size = read_32le(buffer + 4);
                cur_strl_offset += 8 + chunk_size + (chunk_size & 1);
                continue;
            }
        }
    }

read_error:
    CloseHandle(hAvi);

    return (video->read_compressed_video && !video->palette_colors)?2:0;
}

static void decode_rle8(uint8_t *src, int length, int width, int height, uint8_t **rows)
{
    uint8_t *row;
    int x, y, num, index;
    uint8_t b1, b2;

    // https://wiki.multimedia.cx/index.php/Microsoft_RLE

    x = y = 0;
    row = rows[0];
    while (1)
    {
        if (length < 2) break;

        b1 = src[0];
        b2 = src[1];
        src += 2;
        length -= 2;

        if (b1 != 0)
        {
            // Encoded Mode
            num = (x + b1 > width)?width - x:b1;
            for (index = 0; index < num; index++)
            {
                row[x + index] = b2;
            }

            x += num;
            continue;
        }

        if (b2 >= 3)
        {
            // Absolute Mode
            if (b2 > length) break;

            num = (x + b2 > width)?width - x:b2;
            for (index = 0; index < num; index++)
            {
                row[x + index] = src[index];
            }

            x += num;
            src += b2 + (b2 & 1);
            length -= b2 + (b2 & 1);
            continue;
        }

        if (b2 == 0)
        {
            // End of line
            x = 0;
            y++;
            if (y >= height) break;
            row = rows[y];
        }
        else if (b2 == 1)
        {
            // End of bitmap
            break;
        }
        else
        {
            // Delta
            if (length < 2) break;

            b1 = src[0];
            b2 = src[1];
            src += 2;
            length -= 2;

            x += b1;
            if (x > width) x = width;
            y += b2;
            if (y >= height) break;
            row = rows[y];
        }
    }
}

#endif


static LRESULT WINAPI AVI_VideoCallback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;

    uMsg = uMsg & 0xffff;

    switch (uMsg)
    {
#if !defined(USE_QUICKTIMELIB)
    case MM_MCINOTIFY:
        AVI_playbackFinished = 1;
        return 0;
#endif

    case WM_KEYUP:
        if (wParam != VK_ESCAPE)
        {
            return uMsg;
        }
        // @fallthrough@
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        AVI_cancelPlayback = 1;
        return 0;

    case WM_ERASEBKGND:
        return 0;

    case WM_PAINT:
        BeginPaint(hWnd, &paint);
        EndPaint(hWnd, &paint);
        return 0;

    default:
        return DefWindowProcA(hWnd, uMsg, wParam, lParam);
    }
}

int AVI_Init_c(void)
{
    WNDCLASSA wndClass;
#if !defined(USE_QUICKTIMELIB)
    MCI_OPEN_PARMS mciOpenParams;
    MCIERROR mcierr;
    char errorText[1024];
#endif
    int index;

    if (!AVI_initialized)
    {
        for (index = 0; index < 5; index++)
        {
            AVI_videos[index] = NULL;
        }

        wndClass.style = CS_NOCLOSE | CS_DBLCLKS;
        wndClass.lpfnWndProc = (WNDPROC)AVI_VideoCallback_c;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = DSAWIN_GetInstance_c();
        wndClass.hIcon = NULL;
        wndClass.hCursor = NULL;
        wndClass.hbrBackground = NULL;
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = AVI_windowClass;
        if (!RegisterClassA(&wndClass))
        {
            return 0;
        }

#if defined(USE_QUICKTIMELIB)
        lqt_registry_init();
#else
        mciOpenParams.dwCallback = 0;
        mciOpenParams.lpstrDeviceType = "avivideo";
        mcierr = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE, (uintptr_t)&mciOpenParams);
        if (mcierr)
        {
            mciGetErrorStringA(mcierr, errorText, 1024);
            MessageBoxA(DSAWIN_GetMainWindowHandle_c(), errorText, "MCI-Error", MB_OK);
            UnregisterClassA(AVI_windowClass, DSAWIN_GetInstance_c());
            return 0;
        }
        w_avi_device_id = mciOpenParams.wDeviceID;
#endif

        DSA_GetScreenExtends_c(&AVI_desktopWidth, &AVI_desktopHeight);
    }

    AVI_initialized++;
    return 1;
}

void AVI_Exit_c(void)
{
#if !defined(USE_QUICKTIMELIB)
    MCI_CLOSE_PARMS mciCloseParams;
#endif
    int index;

    AVI_initialized--;
    if (AVI_initialized > 0)
    {
        return;
    }

    for (index = 0; index < 5; index++)
    {
        // change: fixed error
        //AVI_videos[index] == NULL;
        AVI_videos[index] = NULL;
    }

#if defined(USE_QUICKTIMELIB)
    lqt_registry_destroy();
#else
    mciCloseParams.dwCallback = 0;
    mciSendCommandA(w_avi_device_id, MCI_CLOSE, MCI_WAIT, (uintptr_t)&mciCloseParams);
#endif

    UnregisterClassA(AVI_windowClass, DSAWIN_GetInstance_c());
}

void AVI_SetDestortionLevel_c(int destortionLevel)
{
    AVI_destortionLevel = destortionLevel;
}

void AVI_SystemTask_c(void)
{
    int isApplicationActive, index;

    isApplicationActive = SYSTEM_IsApplicationActive_c();

    // change: removed unneeded
    //if (isApplicationActive || !AVI_wasApplicationActive)
    {
        // change: removed unneeded
        //if (isApplicationActive || AVI_wasApplicationActive)
        {
            if (isApplicationActive && !AVI_wasApplicationActive)
            {
                for (index = 0; index < 5; index++)
                {
                    if (AVI_videos[index] != NULL)
                    {
                        ShowWindow(AVI_videos[index]->hWnd, SW_HIDE);
                    }
                }
            }
        }
    }

    AVI_wasApplicationActive = isApplicationActive;
}

void *AVI_OpenVideo_c(const char *path, const uint8_t *param2)
{
#if !defined(USE_QUICKTIMELIB)
    MCIERROR mcierr;
    MCI_SET_PARMS mciSetParams;
    MCI_STATUS_PARMS mciStatusParams;
    MCI_OPEN_PARMS mciOpenParams;
    MCI_CLOSE_PARMS mciCloseParams;
    HPALETTE hAviPalette;
#endif
    int index2;
    int video_index;
    int palette_index;
    AVI_Video *video;
    unsigned int os;
    int index;
    char tempText[100];
    PALETTEENTRY dsaPalEntries[256];
    PALETTEENTRY aviPalEntries[256];

    if (!SYSTEM_IsApplicationActive_c())
    {
        return (void *)(intptr_t)-1;
    }

    os = SYSTEM_GetOS_c();

    for (video_index = 0; video_index < 5; video_index++)
    {
        if (AVI_videos[video_index] == NULL)
        {
            break;
        }
    }

    if (video_index == 5)
    {
        DBG_Panic_c("C:\\DATA\\BBLIB\\SRC\\BASE\\AVI.c", 1082);
        return NULL;
    }

    video = (AVI_Video *)MEM_malloc_c(sizeof(AVI_Video), "C:\\DATA\\BBLIB\\SRC\\BASE\\AVI.c", "AVI-STRUCTURE", 1089, 1);
    if (video == NULL)
    {
        DBG_Panic_c("C:\\DATA\\BBLIB\\SRC\\BASE\\AVI.c", 1092);
        return NULL;
    }

    video->hWnd = CreateWindowExA(
        0,
        AVI_windowClass,
        AVI_windowClass,
        WS_POPUP,
        0,
        0,
        320,
        240,
        DSAWIN_GetMainWindowHandle_c(),
        NULL,
        DSAWIN_GetInstance_c(),
        0
    );
    if (video->hWnd == NULL)
    {
        UnregisterClassA(AVI_windowClass, DSAWIN_GetInstance_c());
        return NULL;
    }

    strcpy(video->alias, "bbvideo");
    wsprintfA(tempText, "%d", SYSTEM_GetTicks_c());
    strcat(video->alias, tempText);
    if (AVI_destortionLevel >= 1)
    {
        DSA_FreeStaticColors_c(0xFFFFFFFF);
    }

#if defined(USE_QUICKTIMELIB)
    if (open_video(path, video))
    {
        MessageBoxA(DSAWIN_GetMainWindowHandle_c(), "Error opening video file", "MCI-Error OPEN", MB_OK);
        SendMessageA(video->hWnd, WM_CLOSE, 0, 0);
        if (AVI_destortionLevel >= 1)
        {
            DSA_ReuseStaticColors_c(0xFFFFFFFF);
        }
        return NULL;
    }

    if (read_palette(path, video))
    {
        close_video(video);
        SendMessageA(video->hWnd, WM_CLOSE, 0, 0);
        if (AVI_destortionLevel >= 1)
        {
            DSA_ReuseStaticColors_c(0xFFFFFFFF);
        }
        return NULL;
    }

    for (index = 0; index < 256; index++)
    {
        aviPalEntries[index].peRed = video->video_palette[index].rgbRed;
        aviPalEntries[index].peGreen = video->video_palette[index].rgbGreen;
        aviPalEntries[index].peBlue = video->video_palette[index].rgbBlue;
        aviPalEntries[index].peFlags = 0;
    }
#else
    // open video
    mciOpenParams.dwCallback = (uintptr_t)video->hWnd;
    mciOpenParams.lpstrElementName = path;
    mciOpenParams.lpstrAlias = video->alias;
    mcierr = mciSendCommandA(w_avi_device_id, MCI_OPEN, MCI_OPEN_ALIAS | MCI_OPEN_ELEMENT, (uintptr_t)&mciOpenParams);
    if (mcierr)
    {
        mciGetErrorStringA(mcierr, tempText, 100);
        MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error OPEN", MB_OK);
        SendMessageA(video->hWnd, WM_CLOSE, 0, 0);
        if (AVI_destortionLevel >= 1)
        {
            DSA_ReuseStaticColors_c(0xFFFFFFFF);
        }
        return NULL;
    }

    video->wDeviceID = mciOpenParams.wDeviceID;

    // get video palette handle
    mciStatusParams.dwCallback = (uintptr_t)video->hWnd;
    mciStatusParams.dwItem = MCI_DGV_STATUS_HPAL;
    mcierr = mciSendCommandA(video->wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (uintptr_t)&mciStatusParams);
    if (mcierr)
    {
        mciGetErrorStringA(mcierr, tempText, 100);
        mciCloseParams.dwCallback = (uintptr_t)video->hWnd;
        mciSendCommandA(video->wDeviceID, MCI_CLOSE, MCI_WAIT, (uintptr_t)&mciCloseParams);
        SendMessageA(video->hWnd, WM_CLOSE, 0, 0);
        if (AVI_destortionLevel >= 1)
        {
            DSA_ReuseStaticColors_c(0xFFFFFFFF);
        }
        return NULL;
    }

    hAviPalette = (HPALETTE)mciStatusParams.dwReturn;

    GetPaletteEntries(hAviPalette, 0, 256, aviPalEntries);
#endif

    GetPaletteEntries(DSA_hPalette, 0, 256, dsaPalEntries);

    for (index = 0; index < 256; index++)
    {
        if (aviPalEntries[index].peRed > 254)
        {
            aviPalEntries[index].peRed = 254;
        }
        if (aviPalEntries[index].peRed == 0)
        {
            aviPalEntries[index].peRed = 1;
        }
        if (aviPalEntries[index].peGreen > 254)
        {
            aviPalEntries[index].peGreen = 254;
        }
        if (aviPalEntries[index].peGreen == 0)
        {
            aviPalEntries[index].peGreen = 1;
        }
        if (aviPalEntries[index].peBlue > 254)
        {
            aviPalEntries[index].peBlue = 254;
        }
        if (aviPalEntries[index].peBlue == 0)
        {
            aviPalEntries[index].peBlue = 1;
        }
    }

    for (index = 0; index < 256; index++)
    {
        if (param2[index] == 1)
        {
            video->palette2.palPalEntry[index].peRed = dsaPalEntries[index].peRed;
            video->palette2.palPalEntry[index].peGreen = dsaPalEntries[index].peGreen;
            video->palette2.palPalEntry[index].peBlue = dsaPalEntries[index].peBlue;
        }
        else
        {
            video->palette2.palPalEntry[index].peRed = aviPalEntries[index].peRed;
            video->palette2.palPalEntry[index].peGreen = aviPalEntries[index].peGreen;
            video->palette2.palPalEntry[index].peBlue = aviPalEntries[index].peBlue;
        }
        video->palette1.palPalEntry[index].peRed = dsaPalEntries[index].peRed;
        video->palette1.palPalEntry[index].peGreen = dsaPalEntries[index].peGreen;
        video->palette1.palPalEntry[index].peBlue = dsaPalEntries[index].peBlue;
    }

    palette_index = 10;
    for (index2 = 0; index2 < 10; index2++)
    {
        if (param2[index2] == 1)
        {
            while (( palette_index < 246) && (param2[palette_index] == 1))
            {
                palette_index++;
            }

            if (palette_index < 246)
            {
                if (os == 4)
                {
                    if (dsaPalEntries[index2].peRed >= 254)
                    {
                        video->palette2.palPalEntry[palette_index].peRed = dsaPalEntries[index2].peRed - 1;
                    }
                    else
                    {
                        video->palette2.palPalEntry[palette_index].peRed = dsaPalEntries[index2].peRed + 1;
                    }
                }
                else
                {
                    video->palette2.palPalEntry[palette_index].peRed = dsaPalEntries[index2].peRed;
                }
                video->palette2.palPalEntry[palette_index].peGreen = dsaPalEntries[index2].peGreen;
                video->palette2.palPalEntry[palette_index].peBlue = dsaPalEntries[index2].peBlue;
                palette_index++;
            }
        }
    }

    palette_index = 244;
    for (index2 = 255; index2 > 244; index2--)
    {
        if (param2[index2] == 1)
        {
            while ((palette_index > 10) && (param2[palette_index] == 1))
            {
                palette_index--;
            }

            if (palette_index > 10)
            {
                if (os == 4)
                {
                    if (dsaPalEntries[index2].peRed >= 254)
                    {
                        video->palette2.palPalEntry[palette_index].peRed = dsaPalEntries[index2].peRed - 1;
                    }
                    else
                    {
                        video->palette2.palPalEntry[palette_index].peRed = dsaPalEntries[index2].peRed + 1;
                    }
                }
                else
                {
                    video->palette2.palPalEntry[palette_index].peRed = dsaPalEntries[index2].peRed;
                }
                video->palette2.palPalEntry[palette_index].peGreen = dsaPalEntries[index2].peGreen;
                video->palette2.palPalEntry[palette_index--].peBlue = dsaPalEntries[index2].peBlue;
            }
        }
    }

    video->palette1.palNumEntries = 256;
    video->palette1.palVersion = 1;
    video->palette2.palNumEntries = 256;
    video->palette2.palVersion = 1;

#if !defined(USE_QUICKTIMELIB)
    // use frames for timing
    mciSetParams.dwCallback = 0;
    mciSetParams.dwTimeFormat = MCI_FORMAT_FRAMES;
    mcierr = mciSendCommandA(video->wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (uintptr_t)&mciSetParams);
    if (mcierr)
    {
        mciGetErrorStringA(mcierr, tempText, 100);
        MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error SETTIMEBASE", MB_OK);
        mciCloseParams.dwCallback = 0;
        mciSendCommandA(video->wDeviceID, MCI_CLOSE, MCI_WAIT, (uintptr_t)&mciCloseParams);
        SendMessageA(video->hWnd, WM_CLOSE, 0, 0);
        if (AVI_destortionLevel >= 1)
        {
            DSA_ReuseStaticColors_c(0xFFFFFFFF);
        }
        return NULL;
    }

    // seek to keyframes (don't seek exactly)
    mciSetParams.dwCallback = 0;
    mcierr = mciSendCommandA(video->wDeviceID, MCI_SET, MCI_DGV_SET_SEEK_EXACTLY | MCI_SET_OFF, (uintptr_t)&mciSetParams);
    if (mcierr)
    {
        mciGetErrorStringA(mcierr, tempText, 100);
        MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error SETTIMEBASE", MB_OK);
        mciCloseParams.dwCallback = 0;
        mciSendCommandA(video->wDeviceID, MCI_CLOSE, MCI_WAIT, (uintptr_t)&mciCloseParams);
        SendMessageA(video->hWnd, WM_CLOSE, 0, 0);
        if (AVI_destortionLevel >= 1)
        {
            DSA_ReuseStaticColors_c(0xFFFFFFFF);
        }
        return NULL;
    }
#endif

    AVI_videos[video_index] = video;
    AVI_numVideos++;
    return video;
}

void AVI_CloseVideo_c(void *video)
{
#if !defined(USE_QUICKTIMELIB)
    MCI_CLOSE_PARMS mciCloseParams;
#endif
    AVI_Video *video2;
    int index;

    if (video == NULL)
    {
        return;
    }

    video2 = (AVI_Video *)video;
    for (index = 0; index < 5; index++)
    {
        if (AVI_videos[index] == video2)
        {
            AVI_videos[index] = NULL;
            break;
        }
    }

#if defined(USE_QUICKTIMELIB)
    close_video(video2);
#else
    // close video
    mciCloseParams.dwCallback = (uintptr_t)video2->hWnd;
    mciSendCommandA(video2->wDeviceID, MCI_CLOSE, MCI_WAIT, (uintptr_t)&mciCloseParams);
#endif

    SendMessageA(video2->hWnd, WM_CLOSE, 0, 0);
    DSA_ReuseStaticColors_c(1);
    MEM_free_c(video);
    AVI_numVideos--;
}

int AVI_PlayVideo_c(void *video, int x, int y, int param4, int param5, int volume, unsigned int flags)
{
#if defined(USE_QUICKTIMELIB)
    struct {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD bmiColors[256];
    } bmi;
    HDC hAviDC;
    HBITMAP hAviFrame;
    HGDIOBJ hAviSelected;
    void *pAviData;
    uint8_t** rows;
    DSA_Palette *video_palette;
    uint8_t *buffer;
    int rowspan, index, diff_time, blit_mode, old_blt_mode, buffer_alloc, buffer_length;
    unsigned int last_frame, next_frame, first_frame;
    int64_t next_frame_time, frame_num;
    int audio_buffer_size, audio_temp_buffer_samples, audio_eof, current_header, samples_available;
    uint8_t *audio_buffer;
    int16_t *audio_temp_buffer[2];
    int64_t last_position;
    HWAVEOUT hWaveOut;
    WAVEFORMATEX waveFormat;
    WAVEHDR waveHeader[16];
#else
    MCI_GENERIC_PARMS mciStopParams;
    MCI_PLAY_PARMS mciPlayParams;
    MCI_DGV_WINDOW_PARMS mciWindowParams;
    unsigned int mciPlayFlags;
    MCIERROR mcierr;
    MCI_DGV_PUT_PARMS mciPutParams;
    MCI_STATUS_PARMS mciStatusParams;
    MCI_DGV_WHERE_PARMS mciWhereParams;
    char tempText[1024];
#endif
    BLEV_Event event;
    int returnValue;
    int width;
    int height;
    HDC hDC;
    HPALETTE hPrevPal;
    AVI_Video *video2;
    int volume2;

    returnValue = 0;
    if (!SYSTEM_IsApplicationActive_c())
    {
        return 0;
    }

    if (video == NULL)
    {
        return 0;
    }

    video2 = (AVI_Video *)video;
    FX_ReserveDevices_c(0);
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();

    if ((SYSTEM_GetOS_c() == 1) && (flags & 2))
    {
        flags = (flags & 0xFFFFFFFD) | 4;
    }

    if (!(flags & 0x40000000))
    {
        DSA_SetPal_c(0, &(video2->palette2), 0, 256, 0);
        DSA_ActivatePal_c();
    }

    hDC = GetDC(video2->hWnd);
    hPrevPal = SelectPalette(hDC, (HPALETTE)DSA_hPalette, FALSE);
    if (hPrevPal == NULL)
    {
        DSA_ReuseStaticColors_c(1);
        FX_ReserveDevices_c(1);
        return 0;
    }
    RealizePalette(hDC);
    SelectPalette(hDC, hPrevPal, FALSE);
    ReleaseDC(video2->hWnd, hDC);

#if defined(USE_QUICKTIMELIB)
    hAviDC = NULL;
    hAviFrame = NULL;
    rows = NULL;
    buffer = NULL;
    buffer_alloc = 0;
    if (video2->play_video)
    {
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = video2->video_width;
        bmi.bmiHeader.biHeight = (video2->read_compressed_video)?video2->video_height:-video2->video_height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = (video2->read_compressed_video)?8:((video2->colormodel == BC_BGR888)?24:32);
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = 0;
        bmi.bmiHeader.biXPelsPerMeter = 0;
        bmi.bmiHeader.biYPelsPerMeter = 0;
        bmi.bmiHeader.biClrUsed = (video2->read_compressed_video)?video2->palette_colors:256;
        bmi.bmiHeader.biClrImportant = 0;

        if (video2->read_compressed_video)
        {
            for (index = 0; index < 256; index++)
            {
                bmi.bmiColors[index] = video2->video_palette[index];
            }
        }
        else
        {
            video_palette = (flags & 0x40000000)?(&(video2->palette1)):(&(video2->palette2));
            for (index = 0; index < 256; index++)
            {
                bmi.bmiColors[index].rgbBlue = video_palette->palPalEntry[index].peBlue;
                bmi.bmiColors[index].rgbGreen = video_palette->palPalEntry[index].peGreen;
                bmi.bmiColors[index].rgbRed = video_palette->palPalEntry[index].peRed;
                bmi.bmiColors[index].rgbReserved = 0;
            }
        }

        pAviData = NULL;
        rows = (uint8_t **) HeapAlloc(GetProcessHeap(), 0, video2->video_height * sizeof(uint8_t*));
        if (rows != NULL)
        {
            hAviDC = CreateCompatibleDC(NULL);
            if (hAviDC != NULL)
            {
                hAviFrame = CreateDIBSection(hAviDC, (const BITMAPINFO *)&bmi, DIB_RGB_COLORS, &pAviData, NULL, 0);
                if (hAviFrame != NULL)
                {
                    hAviSelected = SelectObject(hAviDC, hAviFrame);
                    if (hAviSelected == NULL)
                    {
                        DeleteObject(hAviFrame);
                        DeleteDC(hAviDC);
                        pAviData = NULL;
                    }
                }
                else
                {
                    DeleteDC(hAviDC);
                    pAviData = NULL;
                }
            }
        }

        if (pAviData == NULL)
        {
            if (rows != NULL)
            {
                HeapFree(GetProcessHeap(), 0, rows);
            }
            MessageBoxA(DSAWIN_GetMainWindowHandle_c(), "Error creating bitmap", "MCI-Error SETVIDEO", MB_OK);
            FX_ReserveDevices_c(1);
            DSA_ReuseStaticColors_c(1);
            DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
            DSA_ActivatePal_c();
            DSA_TotalRepaint_c();
            return 0;
        }

        rowspan = ((video2->video_width * bmi.bmiHeader.biBitCount + 31) & ~31) >> 3;

        rows[0] = (uint8_t *)pAviData;
        for (index = 1; index < video2->video_height; index++)
        {
            rows[index] = rows[0] + index * rowspan;
        }

        if (!video2->read_compressed_video)
        {
            lqt_set_row_span(video2->qt, 0, rowspan);
        }
    }

    audio_temp_buffer_samples = 0;
    audio_buffer = NULL;
    audio_temp_buffer[0] = NULL;
    hWaveOut = NULL;
    if (video2->play_audio)
    {
        audio_temp_buffer_samples = (video2->audio_sample_rate < 8000)?1024:(1024 * (video2->audio_sample_rate / 11025));
        audio_buffer_size = 16 * audio_temp_buffer_samples * video2->audio_format_size * video2->audio_channels;

        audio_buffer = (uint8_t *) HeapAlloc(GetProcessHeap(), 0, audio_buffer_size);
        if (audio_buffer != NULL)
        {
            if ((video2->audio_channels > 1) && !video2->decode_raw_audio)
            {
                audio_temp_buffer[0] = (int16_t *) HeapAlloc(GetProcessHeap(), 0, video2->audio_channels * audio_temp_buffer_samples * sizeof(int16_t));
                if (audio_temp_buffer[0] == NULL)
                {
                    HeapFree(GetProcessHeap(), 0, audio_buffer);
                    audio_buffer = NULL;

                }
                else
                {
                    audio_temp_buffer[1] = audio_temp_buffer[0] + audio_temp_buffer_samples;
                }
            }
        }

        if (audio_buffer != NULL)
        {
            // initialize waveout
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = video2->audio_channels;
            waveFormat.nSamplesPerSec = video2->audio_sample_rate;
            waveFormat.nAvgBytesPerSec = video2->audio_channels * video2->audio_format_size * video2->audio_sample_rate;
            waveFormat.nBlockAlign = video2->audio_channels * video2->audio_format_size;
            waveFormat.wBitsPerSample = video2->audio_format_size * 8;
            waveFormat.cbSize = 0;

            if (MMSYSERR_NOERROR != waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, WAVE_ALLOWSYNC))
            {
                hWaveOut = NULL;
            }
            else
            {
                for (index = 0; index < 16; index++)
                {
                    waveHeader[index].dwBufferLength = audio_temp_buffer_samples * video2->audio_format_size * video2->audio_channels;
                    waveHeader[index].lpData = (LPSTR)(audio_buffer + index * waveHeader[index].dwBufferLength);
                    waveHeader[index].dwFlags = 0;

                    if (MMSYSERR_NOERROR != waveOutPrepareHeader(hWaveOut, &(waveHeader[index]), sizeof(WAVEHDR)))
                    {
                        for (index--; index >= 0; index--)
                        {
                            waveOutUnprepareHeader(hWaveOut, &(waveHeader[index]), sizeof(WAVEHDR));
                        }

                        waveOutClose(hWaveOut);
                        hWaveOut = NULL;
                        break;
                    }

                    waveHeader[index].dwFlags |= WHDR_DONE;
                }
            }
        }
    }
#else
    wsprintfA(tempText, "setvideo %s palette handle to %d", video2->alias, DSA_hPalette);
    mcierr = mciSendStringA(tempText, NULL, 0, NULL);
    if (mcierr)
    {
        mciGetErrorStringA(mcierr, tempText, 1024);
        MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error SETVIDEO", MB_OK);
        FX_ReserveDevices_c(1);
        DSA_ReuseStaticColors_c(1);
        DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
        DSA_ActivatePal_c();
        DSA_TotalRepaint_c();
        return 0;
    }
#endif

    if (volume > 0)
    {
#if defined(USE_QUICKTIMELIB)
        if (hWaveOut != NULL)
        {
            volume2 = volume;
            if (volume2 >= 65536)
            {
                volume2 = 65535;
            }
            if (MMSYSERR_NOERROR != waveOutSetVolume(hWaveOut, volume2 | ((uint32_t)volume2 << 16)))
            {
                MessageBoxA(DSAWIN_GetMainWindowHandle_c(), "Error setting volume", "MCI-Error SETVIDEO", MB_OK);
            }
        }
#else
        volume2 = volume / 64;
        if (volume2 >= 1000)
        {
            volume2 = 999;
        }
        wsprintfA(tempText, "setaudio %s volume to %d", video2->alias, volume2);
        mcierr = mciSendStringA(tempText, NULL, 0, NULL);
        if (mcierr)
        {
            mciGetErrorStringA(mcierr, tempText, 1024);
            MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error SETVIDEO", MB_OK);
        }
#endif
    }

#if defined(USE_QUICKTIMELIB)
    width = video2->video_width;
    height = video2->video_height;
#else
    mciWhereParams.dwCallback = (uintptr_t)video2->hWnd;
    SetRectEmpty(&(mciWhereParams.rc));
    mcierr = mciSendCommandA(video2->wDeviceID, MCI_WHERE, MCI_OVLY_WHERE_DESTINATION, (uintptr_t)&mciWhereParams);
    if (mcierr)
    {
        mciGetErrorStringA(mcierr, tempText, 1024);
        MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error WHERE", MB_OK);
        ShowWindow(video2->hWnd, SW_HIDE);
        FX_ReserveDevices_c(1);
        DSA_ReuseStaticColors_c(1);
        DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
        DSA_ActivatePal_c();
        DSA_TotalRepaint_c();
        return 0;
    }

    width = mciWhereParams.rc.right - mciWhereParams.rc.left;
    height = mciWhereParams.rc.bottom - mciWhereParams.rc.top;
#endif

    if ((width > 320) && (flags & 2))
    {
        flags = (flags & 0xFFFFFFFD) | 4;
    }

    switch (flags & 7)
    {
    case 1:
        if (x < 0)
        {
            x = ((unsigned int)(AVI_desktopWidth - width)) >> 1;
        }
        if (y < 0)
        {
            y = ((unsigned int)(AVI_desktopHeight - height)) >> 1;
        }

        MoveWindow(
            video2->hWnd,
            x,
            y,
            width,
            height,
            FALSE
        );

#if defined(USE_QUICKTIMELIB)
        blit_mode = 0;
#else
        mciWindowParams.dwCallback = (uintptr_t)video2->hWnd;
        mciWindowParams.hWnd = video2->hWnd;
        mcierr = mciSendCommandA(video2->wDeviceID, MCI_WINDOW, MCI_DGV_WINDOW_HWND | MCI_WAIT, (uintptr_t)&mciWindowParams);
        if (mcierr)
        {
            mciGetErrorStringA(mcierr, tempText, 1024);
            MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error WINDOW", MB_OK);
            ShowWindow(video2->hWnd, SW_HIDE);
            FX_ReserveDevices_c(1);
            DSA_ReuseStaticColors_c(1);
            DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
            MoveWindow(video2->hWnd, AVI_desktopWidth, AVI_desktopHeight, 100, 100, FALSE);
            DSA_ActivatePal_c();
            DSA_TotalRepaint_c();
            return 0;
        }

        mciPlayFlags = 0;
#endif

        break;

    case 2:
        MoveWindow(video2->hWnd, 0, 0, 100, 100, FALSE);
#if defined(USE_QUICKTIMELIB)
        blit_mode = 2;
#else
        mciPlayFlags = MCI_MCIAVI_PLAY_FULLSCREEN;
#endif
        break;

    case 4:
        width = (AVI_desktopWidth <= 1024)?AVI_desktopWidth:1024;
        height = (AVI_desktopHeight <= 768)?AVI_desktopHeight:768;

        MoveWindow(
            video2->hWnd,
            ((unsigned int)(AVI_desktopWidth - width)) >> 1,
            AVI_desktopHeight - height,
            width,
            height,
            FALSE
        );

#if defined(USE_QUICKTIMELIB)
        blit_mode = 1;
#else
        mciPutParams.dwCallback = (uintptr_t)video2->hWnd;
        mciPutParams.rc.left = 0;
        mciPutParams.rc.top = 0;
        mciPutParams.rc.right = width;
        mciPutParams.rc.bottom = height;
        mcierr = mciSendCommandA(video2->wDeviceID, MCI_PUT, MCI_DGV_PUT_DESTINATION | MCI_DGV_RECT | MCI_WAIT, (uintptr_t)&mciPutParams);
        if (mcierr)
        {
            mciGetErrorStringA(mcierr, tempText, 1024);
            MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error PUT Destination", MB_OK);
            ShowWindow(video2->hWnd, SW_HIDE);
            FX_ReserveDevices_c(1);
            DSA_ReuseStaticColors_c(1);
            DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
            DSA_ActivatePal_c();
            DSA_TotalRepaint_c();
            MoveWindow(video2->hWnd, AVI_desktopWidth, AVI_desktopHeight, 100, 100, FALSE);
            return 0;
        }

        mciPutParams.dwCallback = (uintptr_t)video2->hWnd;
        mciPutParams.rc = mciWhereParams.rc;
        mcierr = mciSendCommandA(video2->wDeviceID, MCI_PUT, MCI_DGV_PUT_SOURCE | MCI_DGV_RECT | MCI_WAIT, (uintptr_t)&mciPutParams);
        if (mcierr)
        {
            mciGetErrorStringA(mcierr, tempText, 1024);
            MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error PUT SOURCE", MB_OK);
            ShowWindow(video2->hWnd, SW_HIDE);
            FX_ReserveDevices_c(1);
            DSA_ReuseStaticColors_c(1);
            DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
            DSA_ActivatePal_c();
            DSA_TotalRepaint_c();
            MoveWindow(video2->hWnd, AVI_desktopWidth, AVI_desktopHeight, 100, 100, FALSE);
            return 0;
        }

        mciWindowParams.dwCallback = (uintptr_t)video2->hWnd;
        mciWindowParams.hWnd = video2->hWnd;
        mcierr = mciSendCommandA(video2->wDeviceID, MCI_WINDOW, MCI_DGV_WINDOW_HWND | MCI_WAIT, (uintptr_t)&mciWindowParams);
        if (mcierr)
        {
            mciGetErrorStringA(mcierr, tempText, 1024);
            MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error WINDOW", MB_OK);
            ShowWindow(video2->hWnd, SW_HIDE);
            FX_ReserveDevices_c(1);
            DSA_ReuseStaticColors_c(1);
            DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
            DSA_ActivatePal_c();
            DSA_TotalRepaint_c();
            MoveWindow(video2->hWnd, AVI_desktopWidth, AVI_desktopHeight, 100, 100, FALSE);
            return 0;
        }

        mciPlayFlags = 0;
#endif

        break;

    default:
#if defined(USE_QUICKTIMELIB)
        blit_mode = 0;
#else
        mciPlayFlags = 0;
#endif
        break;
    }

#if defined(USE_QUICKTIMELIB)
    if (video2->play_video && !video2->read_compressed_video)
    {
        lqt_seek_video(video2->qt, 0, 0);
    }
    if (hWaveOut != NULL)
    {
        quicktime_set_audio_position(video2->qt, 0, 0);
    }
#else
    mciStatusParams.dwCallback = 0;
    mciStatusParams.dwItem = MCI_STATUS_POSITION;
    mciStatusParams.dwTrack = 1;
    mcierr = mciSendCommandA(video2->wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (uintptr_t)&mciStatusParams);
    if (mcierr)
    {
        mciStatusParams.dwReturn = -1;
    }

    mciPlayParams.dwCallback = (uintptr_t)video2->hWnd;
#endif

    AVI_playbackFinished = 0;
    AVI_stopPlayback = 0;
    AVI_cancelPlayback = 0;
    ShowWindow(video2->hWnd, SW_SHOWNORMAL);

#if defined(USE_QUICKTIMELIB)
    // decode first frame
    if (video2->play_video)
    {
        frame_num = 0;

        if (video2->read_compressed_video)
        {
            buffer_length = lqt_read_video_frame(video2->qt, &buffer, &buffer_alloc, frame_num, &next_frame_time, 0);
            if (0 == buffer_length)
            {
                AVI_playbackFinished = 1;
            }
            else
            {
                decode_rle8(buffer, buffer_length, video2->video_width, video2->video_height, rows);
            }
        }
        else
        {
            if (lqt_decode_video(video2->qt, rows, 0))
            {
                AVI_playbackFinished = 1;
            }
        }
    }
    audio_eof = ((hWaveOut != NULL) && !AVI_playbackFinished)?0:1;
    current_header = 0;

    first_frame = GetTickCount();
    next_frame = first_frame;
    last_frame = first_frame + (int)((((double)video2->video_duration) / video2->video_time_scale) * 1000);

    while (!AVI_playbackFinished)
    {
        diff_time = GetTickCount() - last_frame;
        if (diff_time >= 0)
        {
            AVI_playbackFinished = 1;
            break;
        }

        if (!audio_eof)
        {
            if (waveHeader[current_header].dwFlags & WHDR_DONE)
            {
                samples_available = audio_temp_buffer_samples;

                last_position = lqt_last_audio_position(video2->qt, 0);

                if (samples_available >= video2->audio_length - last_position)
                {
                    samples_available = video2->audio_length - last_position;
                    audio_eof = 1;
                }

                if (video2->decode_raw_audio)
                {
                    samples_available = lqt_decode_audio_raw(video2->qt, waveHeader[current_header].lpData, samples_available, 0);
                }
                else if (video2->audio_channels == 1)
                {
                    audio_temp_buffer[1] = (int16_t *) waveHeader[current_header].lpData;
                    if (lqt_decode_audio_track(video2->qt, &(audio_temp_buffer[1]), NULL, samples_available, 0))
                    {
                        samples_available = 0;
                    }
                }
                else
                {
                    if (lqt_decode_audio_track(video2->qt, audio_temp_buffer, NULL, samples_available, 0))
                    {
                        samples_available = 0;
                    }
                    else
                    {
                        for (index = 0; index < samples_available; index++)
                        {
                            ((int16_t *) waveHeader[current_header].lpData)[2 * index] = audio_temp_buffer[0][index];
                            ((int16_t *) waveHeader[current_header].lpData)[2 * index + 1] = audio_temp_buffer[1][index];
                        }
                    }
                }

                if (samples_available == 0)
                {
                    audio_eof = 1;
                }
                else
                {
                    waveHeader[current_header].dwBufferLength = samples_available * video2->audio_format_size * video2->audio_channels;
                    waveHeader[current_header].dwFlags &= ~WHDR_DONE;
                    waveOutWrite(hWaveOut, &(waveHeader[current_header]), sizeof(WAVEHDR));
                    current_header = (current_header + 1) & 15;
                }
            }
        }

        if (video2->play_video)
        {
            diff_time = GetTickCount() - next_frame;
            if (diff_time >= 0)
            {
                hDC = GetDC((blit_mode == 2)?NULL:video2->hWnd);
                if (blit_mode)
                {
                    old_blt_mode = SetStretchBltMode(hDC, COLORONCOLOR);
                    StretchBlt(hDC, 0, 0, (blit_mode == 2)?AVI_desktopWidth:width, (blit_mode == 2)?AVI_desktopHeight:height,
                               hAviDC, 0, 0, video2->video_width, video2->video_height,
                               SRCCOPY);
                    SetStretchBltMode(hDC, old_blt_mode);
                }
                else
                {
                    BitBlt(hDC, 0, 0, video2->video_width, video2->video_height, hAviDC, 0, 0, SRCCOPY);
                }
                ReleaseDC(video2->hWnd, hDC);

                frame_num++;

                if (frame_num < video2->video_frames)
                {
                    if (video2->read_compressed_video)
                    {
                        buffer_length = lqt_read_video_frame(video2->qt, &buffer, &buffer_alloc, frame_num, &next_frame_time, 0);
                        if (0 == buffer_length)
                        {
                            AVI_playbackFinished = 1;
                        }
                        else
                        {
                            next_frame = first_frame + (int)((((double)next_frame_time) / video2->video_time_scale) * 1000);
                            decode_rle8(buffer, buffer_length, video2->video_width, video2->video_height, rows);
                        }
                    }
                    else
                    {
                        next_frame_time = lqt_frame_time(video2->qt, 0);
                        next_frame = first_frame + (int)((((double)next_frame_time) / video2->video_time_scale) * 1000);

                        if (lqt_decode_video(video2->qt, rows, 0))
                        {
                            AVI_playbackFinished = 1;
                        }
                    }
                }
                else
                {
                    frame_num = video2->video_frames;
                    next_frame = last_frame;
                }
            }
        }

        if (flags & 2)
        {
            Sleep(1);
        }
        else
        {
            SYSTEM_SystemTask_c();

            if (BLEV_GetEvent_c(&event))
            {
                switch (event.type)
                {
                case BBBLEV_TYPE_KEYUP:
                    if (event.key != BBBLEV_KEY_ESCAPE)
                    {
                        break;
                    }
                    // @fallthrough@
                case BBBLEV_TYPE_MOUSELEFTUP:
                case BBBLEV_TYPE_MOUSERIGHTUP:
                    AVI_playbackFinished = 1;
                    returnValue = 2;
                    break;
                default:
                    break;
                }
            }

            if (AVI_stopPlayback)
            {
                AVI_playbackFinished = 1;
                returnValue = 1;
            }

            if (AVI_cancelPlayback)
            {
                AVI_playbackFinished = 1;
                returnValue = 2;
            }

            if ((!SYSTEM_IsApplicationActive_c()) || AVI_playbackFinished)
            {
                ShowWindow(video2->hWnd, SW_HIDE);
                MoveWindow(video2->hWnd, AVI_desktopWidth, AVI_desktopHeight, 100, 100, FALSE);
                SYSTEM_SystemTask_c();
                SYSTEM_SystemTask_c();
                SYSTEM_SystemTask_c();
                SYSTEM_SystemTask_c();
                SYSTEM_SystemTask_c();
                AVI_playbackFinished = 1;
                if (!SYSTEM_IsApplicationActive_c())
                {
                    returnValue = 2;
                }
            }
        }
    }

    if (hWaveOut != NULL)
    {
        waveOutReset(hWaveOut);
        for (index = 15; index >= 0; index--)
        {
            waveOutUnprepareHeader(hWaveOut, &(waveHeader[index]), sizeof(WAVEHDR));
        }
        waveOutClose(hWaveOut);
    }
    if (audio_temp_buffer[0] != NULL)
    {
        HeapFree(GetProcessHeap(), 0, audio_temp_buffer[0]);
    }
    if (audio_buffer != NULL)
    {
        HeapFree(GetProcessHeap(), 0, audio_buffer);
    }

    if (buffer != NULL)
    {
        free(buffer);
    }
    if (hAviFrame != NULL)
    {
        SelectObject(hAviDC, hAviFrame);
        DeleteObject(hAviFrame);
        DeleteDC(hAviDC);
        HeapFree(GetProcessHeap(), 0, rows);
    }
#else
    if (flags & 2)
    {
        mcierr = mciSendCommandA(video2->wDeviceID, MCI_PLAY, mciPlayFlags | MCI_WAIT, (uintptr_t)&mciPlayParams);
        if (mcierr)
        {
            mciGetErrorStringA(mcierr, tempText, 1024);
            MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error PLAY", MB_OK);
            ShowWindow(video2->hWnd, SW_HIDE);
            FX_ReserveDevices_c(1);
            DSA_ReuseStaticColors_c(1);
            DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
            DSA_ActivatePal_c();
            DSA_TotalRepaint_c();
            MoveWindow(video2->hWnd, AVI_desktopWidth, AVI_desktopHeight, 100, 100, FALSE);
            return 0;
        }
    }
    else
    {
        mcierr = mciSendCommandA(video2->wDeviceID, MCI_PLAY, mciPlayFlags | MCI_NOTIFY, (uintptr_t)&mciPlayParams);
        if (mcierr)
        {
            mciGetErrorStringA(mcierr, tempText, 1024);
            MessageBoxA(DSAWIN_GetMainWindowHandle_c(), tempText, "MCI-Error PLAY", MB_OK);
            ShowWindow(video2->hWnd, SW_HIDE);
            FX_ReserveDevices_c(1);
            DSA_ReuseStaticColors_c(1);
            DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
            DSA_ActivatePal_c();
            DSA_TotalRepaint_c();
            MoveWindow(video2->hWnd, AVI_desktopWidth, AVI_desktopHeight, 100, 100, FALSE);
            return 0;
        }

        while ( 1 )
        {
            if (AVI_playbackFinished)
            {
                break;
            }

            SYSTEM_SystemTask_c();

            if (BLEV_GetEvent_c(&event))
            {
                switch (event.type)
                {
                case BBBLEV_TYPE_KEYUP:
                    if (event.key != BBBLEV_KEY_ESCAPE)
                    {
                        break;
                    }
                    // @fallthrough@
                case BBBLEV_TYPE_MOUSELEFTUP:
                case BBBLEV_TYPE_MOUSERIGHTUP:
                    AVI_playbackFinished = 1;
                    returnValue = 2;
                    break;
                default:
                    break;
                }
            }

            if (AVI_stopPlayback)
            {
                AVI_playbackFinished = 1;
                returnValue = 1;
            }

            if (AVI_cancelPlayback)
            {
                AVI_playbackFinished = 1;
                returnValue = 2;
            }

            if ((!SYSTEM_IsApplicationActive_c()) || AVI_playbackFinished)
            {
                mciStopParams.dwCallback = 0;
                ShowWindow(video2->hWnd, SW_HIDE);
                MoveWindow(video2->hWnd, AVI_desktopWidth, AVI_desktopHeight, 100, 100, FALSE);
                SYSTEM_SystemTask_c();
                SYSTEM_SystemTask_c();
                SYSTEM_SystemTask_c();
                SYSTEM_SystemTask_c();
                SYSTEM_SystemTask_c();
                mcierr = mciSendCommandA(video2->wDeviceID, MCI_STOP, MCI_WAIT, (uintptr_t)&mciStopParams);
                AVI_playbackFinished = 1;
                if (!SYSTEM_IsApplicationActive_c())
                {
                    returnValue = 2;
                }
            }
        }
    }
#endif

    ShowWindow(video2->hWnd, SW_HIDE);
    if ((AVI_numVideos == 1) && (AVI_destortionLevel >= 1))
    {
        DSA_ReuseStaticColors_c(0xFFFFFFFF);
    }
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    if ((!(flags & 0x80000000)) || (returnValue == 2))
    {
        DSA_SetPal_c(0, &(video2->palette1), 0, 256, 0);
        DSA_ActivatePal_c();
        DSA_TotalRepaint_c();
    }
    FX_ReserveDevices_c(1);
    return returnValue;
}

