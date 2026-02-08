/**
 *
 *  Copyright (C) 2021-2026 Roman Pauer
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

#include "SDIvideo.h"
#include "BBDSA.h"
#if defined(USE_QUICKTIMELIB)
#include "BBSYSTEM.h"
#include <lqt/lqt.h>
#include <lqt/colormodels.h>
#include <stdlib.h>
#endif
#include <windows.h>


typedef struct {
    int32_t field00;
    int32_t field04;
    int32_t field08;
    int32_t field0c;
} video_struct;

typedef struct {
    HWND hWnd;
    uint32_t reserved[9];
#if defined(USE_QUICKTIMELIB)
    quicktime_t *qt;

    long frame_number;
    long video_frames;
    int video_width;
    int video_height;
    int64_t video_duration;
    int video_time_scale;
    int colormodel;

    int play_audio;
    int audio_channels;
    long audio_sample_rate;
    int audio_format_size;
    int decode_raw_audio;
    int64_t audio_length;

    int32_t screen_width;
    int32_t screen_height;

    int blit_mode;
    int stretch_width;
    int stretch_height;
    HDC hAviDC;
    HBITMAP hAviFrame;
    uint8_t** rows;

    unsigned int last_frame;
    unsigned int next_frame;
    unsigned int first_frame;

    uint8_t *audio_buffer;
    int16_t *audio_temp_buffer[2];
    HWAVEOUT hWaveOut;
    WAVEHDR waveHeader[16];

    int audio_temp_buffer_samples;
    int audio_eof;
    int current_header;
#endif
} AVI_struct;


static const char PRE_windowClass[] = "PREFIXCALLBACK";
static const char POST_windowClass[] = "POSTFIXCALLBACK";
static const char SS_windowClass[] = "SSCALLBACK";


extern AVI_struct video_PRE_avi;
extern MCIDEVICEID video_PRE_mciId;
extern int32_t video_PRE_dword_4CF328;
extern int32_t video_PRE_dword_4CF32C;
extern video_struct video_PRE_stru_4CF330[33];
extern volatile int32_t video_PRE_dword_4CF540;
extern volatile uint8_t video_PRE_byte_4CF544;
extern volatile uint8_t video_PRE_byte_4CF548;
extern volatile int32_t video_PRE_dword_4CF54C;
extern uint8_t video_PRE_byte_4CF550;

extern AVI_struct video_POST_avi;
extern MCIDEVICEID video_POST_mciId;
extern video_struct video_POST_stru_4CFA40[3];
extern volatile int32_t video_POST_dword_4CFA70;
extern volatile uint8_t video_POST_byte_4CFA74;
extern volatile uint8_t video_POST_byte_4CFA78;
extern volatile int32_t video_POST_dword_4CFA7C;
extern uint8_t video_POST_byte_4CFA80;


#if defined(USE_QUICKTIMELIB)

static int close_video(AVI_struct *video)
{
    if (video->qt != NULL)
    {
        quicktime_close(video->qt);
        video->qt = NULL;
    }

    return 0;
}

static int open_video(const char *path, AVI_struct *video)
{
#if defined(__WINE__)
    WCHAR pathW[MAX_PATH];
    char *pathUnix;
#endif
    int color_models[3];
    lqt_sample_format_t format;

    video->hAviFrame = NULL;
    video->rows = NULL;
    video->frame_number = -1;

    video->audio_buffer = NULL;
    video->audio_temp_buffer[0] = NULL;
    video->hWaveOut = NULL;

#if defined(__WINE__)
    MultiByteToWideChar(CP_ACP, 0, path, -1, pathW, MAX_PATH);
    pathUnix = wine_get_unix_file_name(pathW);

    video->qt = lqt_open_read(pathUnix);
#else
    video->qt = lqt_open_read(path);
#endif
    if (video->qt == NULL) return 0;

    if ((quicktime_video_tracks(video->qt) <= 0) || !quicktime_supported_video(video->qt, 0))
    {
        close_video(video);
        return 0;
    }

    video->video_frames = quicktime_video_length(video->qt, 0);

    video->video_time_scale = lqt_video_time_scale(video->qt, 0);
    video->video_duration = lqt_video_duration(video->qt, 0);

    video->video_width = quicktime_video_width(video->qt, 0);
    video->video_height = quicktime_video_height(video->qt, 0);

    color_models[0] = BC_BGR888;
    color_models[1] = BC_BGR8888;
    color_models[2] = LQT_COLORMODEL_NONE;
    video->colormodel = lqt_get_best_colormodel(video->qt, 0, color_models);

    lqt_set_cmodel(video->qt, 0, video->colormodel);


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

    return 1;
}

static int play_video_prepare(uint32_t zoomed, AVI_struct *video)
{
    int x, y, width, height;
    RECT rect;
    struct {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD bmiColors[256];
    } bmi;
    HGDIOBJ hAviSelected;
    void *pAviData;
    int rowspan, index;
    int audio_buffer_size;
    WAVEFORMATEX waveFormat;

    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();

    DSA_GetScreenExtends_c(&(video->screen_width), &(video->screen_height));


    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = video->video_width;
    bmi.bmiHeader.biHeight = -video->video_height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = (video->colormodel == BC_BGR888)?24:32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;

    pAviData = NULL;
    video->rows = (uint8_t **) HeapAlloc(GetProcessHeap(), 0, video->video_height * sizeof(uint8_t*));
    if (video->rows == NULL)
    {
        return 0;
    }

    video->hAviDC = CreateCompatibleDC(NULL);
    if (video->hAviDC != NULL)
    {
        video->hAviFrame = CreateDIBSection(video->hAviDC, (const BITMAPINFO *)&bmi, DIB_RGB_COLORS, &pAviData, NULL, 0);
        if (video->hAviFrame != NULL)
        {
            hAviSelected = SelectObject(video->hAviDC, video->hAviFrame);
            if (hAviSelected == NULL)
            {
                DeleteObject(video->hAviFrame);
                DeleteDC(video->hAviDC);
                pAviData = NULL;
            }
        }
        else
        {
            DeleteDC(video->hAviDC);
            pAviData = NULL;
        }
    }

    if (pAviData == NULL)
    {
        HeapFree(GetProcessHeap(), 0, video->rows);
        video->rows = NULL;
        return 0;
    }

    rowspan = ((video->video_width * bmi.bmiHeader.biBitCount + 31) & ~31) >> 3;

    video->rows[0] = (uint8_t *)pAviData;
    for (index = 1; index < video->video_height; index++)
    {
        video->rows[index] = video->rows[0] + index * rowspan;
    }

    lqt_set_row_span(video->qt, 0, rowspan);


    if (video->play_audio)
    {
        video->audio_temp_buffer_samples = (video->audio_sample_rate < 8000)?1024:(1024 * (video->audio_sample_rate / 11025));
        audio_buffer_size = 16 * video->audio_temp_buffer_samples * video->audio_format_size * video->audio_channels;

        video->audio_buffer = (uint8_t *) HeapAlloc(GetProcessHeap(), 0, audio_buffer_size);
        if (video->audio_buffer != NULL)
        {
            if ((video->audio_channels > 1) && !video->decode_raw_audio)
            {
                video->audio_temp_buffer[0] = (int16_t *) HeapAlloc(GetProcessHeap(), 0, video->audio_channels * video->audio_temp_buffer_samples * sizeof(int16_t));
                if (video->audio_temp_buffer[0] == NULL)
                {
                    HeapFree(GetProcessHeap(), 0, video->audio_buffer);
                    video->audio_buffer = NULL;

                }
                else
                {
                    video->audio_temp_buffer[1] = video->audio_temp_buffer[0] + video->audio_temp_buffer_samples;
                }
            }
        }

        if (video->audio_buffer != NULL)
        {
            // initialize waveout
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = video->audio_channels;
            waveFormat.nSamplesPerSec = video->audio_sample_rate;
            waveFormat.nAvgBytesPerSec = video->audio_channels * video->audio_format_size * video->audio_sample_rate;
            waveFormat.nBlockAlign = video->audio_channels * video->audio_format_size;
            waveFormat.wBitsPerSample = video->audio_format_size * 8;
            waveFormat.cbSize = 0;

            if (MMSYSERR_NOERROR != waveOutOpen(&(video->hWaveOut), WAVE_MAPPER, &waveFormat, 0, 0, WAVE_ALLOWSYNC))
            {
                video->hWaveOut = NULL;
            }
            else
            {
                for (index = 0; index < 16; index++)
                {
                    video->waveHeader[index].dwBufferLength = video->audio_temp_buffer_samples * video->audio_format_size * video->audio_channels;
                    video->waveHeader[index].lpData = (LPSTR)(video->audio_buffer + index * video->waveHeader[index].dwBufferLength);
                    video->waveHeader[index].dwFlags = 0;

                    if (MMSYSERR_NOERROR != waveOutPrepareHeader(video->hWaveOut, &(video->waveHeader[index]), sizeof(WAVEHDR)))
                    {
                        for (index--; index >= 0; index--)
                        {
                            waveOutUnprepareHeader(video->hWaveOut, &(video->waveHeader[index]), sizeof(WAVEHDR));
                        }

                        waveOutClose(video->hWaveOut);
                        video->hWaveOut = NULL;
                        break;
                    }

                    video->waveHeader[index].dwFlags |= WHDR_DONE;
                }
            }
        }
    }


    if (zoomed & 0xff)
    {
        width = (video->screen_width <= 1024)?video->screen_width:1024;
        height = (video->screen_height <= 768)?video->screen_height:768;

        x = (video->screen_width - width) >> 1;
        y = video->screen_height - height;

        video->blit_mode = 1;
        video->stretch_width = width;
        video->stretch_height = height;
    }
    else
    {
        width = video->video_width;
        height = video->video_height;

        x = (video->screen_width - width) >> 1;
        y = (video->screen_height - height) >> 1;

        video->blit_mode = 0;
    }

    MoveWindow(video->hWnd, x, y, width, height, FALSE);
    ShowWindow(video->hWnd, TRUE);

    SetRect(
        &rect,
        (video->screen_width >> 1) - 158,
        (video->screen_height >> 1) - 98,
        (video->screen_width >> 1) + 158,
        (video->screen_height >> 1) + 98
    );

    if (SYSTEM_GetOS_c() != 2)
    {
        SetCursorPos(video->screen_width >> 1, video->screen_height >> 1);
        ShowCursor(FALSE);
    }

    ClipCursor(&rect);

    return 1;
}

static int play_video_start(int start_frame, AVI_struct *video)
{
    int64_t start_time;
    unsigned int start_ticks, start_samples;

    if (start_frame == 0)
    {
        start_ticks = 0;

        lqt_seek_video(video->qt, 0, 0);

        if (video->hWaveOut != NULL)
        {
            quicktime_set_audio_position(video->qt, 0, 0);
        }
    }
    else if (start_frame < video->video_frames)
    {
        start_time = lqt_get_frame_time(video->qt, 0, start_frame);
        start_ticks = (int) ((((double)start_time) / video->video_time_scale) * 1000);

        lqt_seek_video(video->qt, 0, start_time);

        if (video->hWaveOut != NULL)
        {
            start_samples = (int) ((((double)start_time) / video->video_time_scale) * video->audio_sample_rate);
            quicktime_set_audio_position(video->qt, start_samples, 0);
        }
    }
    else
    {
        return 0;
    }

    video->frame_number = start_frame;

    // decode first frame
    if (lqt_decode_video(video->qt, video->rows, 0))
    {
        return 0;
    }

    video->audio_eof = (video->hWaveOut != NULL)?0:1;
    video->current_header = 0;

    video->next_frame = GetTickCount();
    video->first_frame = video->next_frame - start_ticks;
    video->last_frame = video->first_frame + (int)((((double)video->video_duration) / video->video_time_scale) * 1000);

    return 1;
}

static int play_video_process(AVI_struct *video)
{
    int diff_time, old_blt_mode;
    HDC hDC;
    int64_t next_frame_time, last_position;
    int samples_available, index;
    int16_t *output_i;

    diff_time = GetTickCount() - video->last_frame;
    if (diff_time >= 0)
    {
        return 0;
    }

    if (!video->audio_eof)
    {
        if (video->waveHeader[video->current_header].dwFlags & WHDR_DONE)
        {
            samples_available = video->audio_temp_buffer_samples;

            last_position = lqt_last_audio_position(video->qt, 0);

            if (samples_available >= video->audio_length - last_position)
            {
                samples_available = (int)(video->audio_length - last_position);
                video->audio_eof = 1;
            }

            if (video->decode_raw_audio)
            {
                samples_available = lqt_decode_audio_raw(video->qt, video->waveHeader[video->current_header].lpData, samples_available, 0);
            }
            else if (video->audio_channels == 1)
            {
                output_i = (int16_t *) video->waveHeader[video->current_header].lpData;
                if (lqt_decode_audio_track(video->qt, &output_i, NULL, samples_available, 0))
                {
                    samples_available = 0;
                }
            }
            else
            {
                if (lqt_decode_audio_track(video->qt, video->audio_temp_buffer, NULL, samples_available, 0))
                {
                    samples_available = 0;
                }
                else
                {
                    for (index = 0; index < samples_available; index++)
                    {
                        ((int16_t *) video->waveHeader[video->current_header].lpData)[2 * index] = video->audio_temp_buffer[0][index];
                        ((int16_t *) video->waveHeader[video->current_header].lpData)[2 * index + 1] = video->audio_temp_buffer[1][index];
                    }
                }
            }

            if (samples_available == 0)
            {
                video->audio_eof = 1;
            }
            else
            {
                video->waveHeader[video->current_header].dwBufferLength = samples_available * video->audio_format_size * video->audio_channels;
                video->waveHeader[video->current_header].dwFlags &= ~WHDR_DONE;
                waveOutWrite(video->hWaveOut, &(video->waveHeader[video->current_header]), sizeof(WAVEHDR));
                video->current_header = (video->current_header + 1) & 15;
            }
        }
    }

    diff_time = GetTickCount() - video->next_frame;
    if (diff_time >= 0)
    {
        hDC = GetDC(video->hWnd);
        if (video->blit_mode)
        {
            old_blt_mode = SetStretchBltMode(hDC, COLORONCOLOR);
            StretchBlt(hDC, 0, 0, video->stretch_width, video->stretch_height,
                       video->hAviDC, 0, 0, video->video_width, video->video_height,
                       SRCCOPY);
            SetStretchBltMode(hDC, old_blt_mode);
        }
        else
        {
            BitBlt(hDC, 0, 0, video->video_width, video->video_height, video->hAviDC, 0, 0, SRCCOPY);
        }
        ReleaseDC(video->hWnd, hDC);

        video->frame_number++;

        if (video->frame_number < video->video_frames)
        {
            next_frame_time = lqt_frame_time(video->qt, 0);
            video->next_frame = video->first_frame + (int)((((double)next_frame_time) / video->video_time_scale) * 1000);

            if (lqt_decode_video(video->qt, video->rows, 0))
            {
                return 0;
            }
        }
        else
        {
            video->frame_number = video->video_frames;
            video->next_frame = video->last_frame;
        }
    }

    return 1;
}

static void play_video_stop(AVI_struct *video)
{
    if (video->hWaveOut != NULL)
    {
        waveOutReset(video->hWaveOut);
    }
}

static void play_video_unprepare(AVI_struct *video)
{
    int index;

    ShowCursor(TRUE);
    ClipCursor(FALSE);

    ShowWindow(video->hWnd, FALSE);
    MoveWindow(video->hWnd, video->screen_width, video->screen_height, 100, 100, FALSE);

    if (video->hWaveOut != NULL)
    {
        waveOutReset(video->hWaveOut);
        for (index = 15; index >= 0; index--)
        {
            waveOutUnprepareHeader(video->hWaveOut, &(video->waveHeader[index]), sizeof(WAVEHDR));
        }
        waveOutClose(video->hWaveOut);
        video->hWaveOut = NULL;
    }
    if (video->audio_temp_buffer[0] != NULL)
    {
        HeapFree(GetProcessHeap(), 0, video->audio_temp_buffer[0]);
        video->audio_temp_buffer[0] = NULL;
    }
    if (video->audio_buffer != NULL)
    {
        HeapFree(GetProcessHeap(), 0, video->audio_buffer);
        video->audio_buffer = NULL;
    }

    if (video->hAviFrame != NULL)
    {
        SelectObject(video->hAviDC, video->hAviFrame);
        DeleteObject(video->hAviFrame);
        video->hAviFrame = NULL;
        DeleteDC(video->hAviDC);
        video->hAviDC = NULL;
        HeapFree(GetProcessHeap(), 0, video->rows);
        video->rows = NULL;
    }

    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
    SYSTEM_SystemTask_c();
}

#endif


static LRESULT WINAPI PRE_VideoCallback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int index, frame_number;
#if !defined(USE_QUICKTIMELIB)
    MCI_STATUS_PARMS mciStatusParams;
#endif
    PAINTSTRUCT paint;

    uMsg = uMsg & 0xffff;

    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return 0;

    case WM_PAINT:
        BeginPaint(hWnd, &paint);
        EndPaint(hWnd, &paint);
        return 0;

    case WM_CHAR:
#if defined(USE_QUICKTIMELIB)
        frame_number = video_PRE_avi.frame_number;
#else
        mciStatusParams.dwCallback = 0;
        mciStatusParams.dwItem = MCI_STATUS_POSITION;
        mciStatusParams.dwTrack = 1;

        if (0 == mciSendCommandA(video_PRE_mciId, MCI_STATUS, MCI_STATUS_ITEM, (uintptr_t)&mciStatusParams))
        {
            frame_number = (int32_t)mciStatusParams.dwReturn;
        }
        else frame_number = -1;
#endif

        if (frame_number >= 0)
        {
            for (index = 0; index < video_PRE_dword_4CF32C; index++)
            {
                if ((video_PRE_stru_4CF330[11 * video_PRE_dword_4CF328 + index].field00 <= frame_number) &&
                    (video_PRE_stru_4CF330[11 * video_PRE_dword_4CF328 + index].field04 > frame_number)
                   )
                {
                    if (wParam == VK_SPACE)
                    {
                        video_PRE_dword_4CF540 = video_PRE_stru_4CF330[11 * video_PRE_dword_4CF328 + index].field0c;
                        return 0;
                    }

                    if (wParam == VK_ESCAPE)
                    {
                        video_PRE_dword_4CF540 = video_PRE_stru_4CF330[11 * video_PRE_dword_4CF328 + index].field08;
                        if (index > 0)
                        {
                            video_PRE_byte_4CF548 = 1;
                            video_PRE_dword_4CF54C = index;
                        }
                        return 0;
                  }
                }
            }
        }

        return DefWindowProcA(hWnd, uMsg, wParam, lParam);

#if !defined(USE_QUICKTIMELIB)
    case MM_MCINOTIFY:
        if (!video_PRE_byte_4CF544)
        {
            video_PRE_dword_4CF540 = -1;
        }
        video_PRE_byte_4CF544 = 0;
        return 0;
#endif

    default:
        return DefWindowProcA(hWnd, uMsg, wParam, lParam);
    }
}

uint32_t CCALL video_RegisterClass_PRE_Video(void)
{
    WNDCLASSA wndClass;

    wndClass.style = CS_NOCLOSE | CS_DBLCLKS;
    wndClass.lpfnWndProc = (WNDPROC)PRE_VideoCallback_c;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = DSAWIN_GetInstance_c();
    wndClass.hIcon = NULL;
    wndClass.hCursor = NULL;
    wndClass.hbrBackground = NULL;
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = PRE_windowClass;

    return RegisterClassA(&wndClass);
}

int CCALL video_Open_PRE_Video(const char *path)
{
#if defined(USE_QUICKTIMELIB)
    return open_video(path, &video_PRE_avi);
#else
    return -1;
#endif
}

int CCALL video_Close_PRE_Video(void)
{
#if defined(USE_QUICKTIMELIB)
    return close_video(&video_PRE_avi);
#else
    return -1;
#endif
}

int CCALL video_Play_PRE_Video(uint32_t zoomed)
{
#if defined(USE_QUICKTIMELIB)
    int start_frame;

    if (!play_video_prepare(zoomed, &video_PRE_avi))
    {
        return 0;
    }

    start_frame = 0;
    video_PRE_dword_4CF540 = -2;

    while (video_PRE_dword_4CF540 != -1)
    {
        if (!play_video_start(start_frame, &video_PRE_avi))
        {
            break;
        }

        while (video_PRE_dword_4CF540 == -2)
        {
            if (!play_video_process(&video_PRE_avi))
            {
                if (!video_PRE_byte_4CF544)
                {
                    video_PRE_dword_4CF540 = -1;
                }
                video_PRE_byte_4CF544 = 0;
                break;
            }

            SYSTEM_SystemTask_c();

            if (video_PRE_byte_4CF548)
            {
                if (video_PRE_stru_4CF330[11 * video_PRE_dword_4CF328 + video_PRE_dword_4CF54C].field04 <= video_PRE_avi.frame_number)
                {
                    video_PRE_dword_4CF540 = -1;
                }
            }

            if (!SYSTEM_IsApplicationActive_c() && video_PRE_byte_4CF550)
            {
                // change: don't pause playback
                //video_PRE_byte_4CF550 = 0;
                //play_video_pause(&video_PRE_avi);
            }

            if (SYSTEM_IsApplicationActive_c() && !video_PRE_byte_4CF550 )
            {
                // change: don't resume playback
                //video_PRE_byte_4CF550 = 1;
                //play_video_resume(&video_PRE_avi);
            }
        }

        if ((video_PRE_dword_4CF540 == -1) || video_PRE_byte_4CF548)
        {
            break;
        }

        video_PRE_byte_4CF544 = 1;
        play_video_stop(&video_PRE_avi);

        start_frame = video_PRE_stru_4CF330[11 * video_PRE_dword_4CF328 + video_PRE_dword_4CF540].field00;
        video_PRE_dword_4CF540 = -2;
    }

    play_video_unprepare(&video_PRE_avi);

    return 0;
#else
    return -1;
#endif
}


static LRESULT WINAPI POST_VideoCallback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int index, frame_number;
#if !defined(USE_QUICKTIMELIB)
    MCI_STATUS_PARMS mciStatusParams;
#endif
    PAINTSTRUCT paint;

    uMsg = uMsg & 0xffff;

    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return 0;

    case WM_PAINT:
        BeginPaint(hWnd, &paint);
        EndPaint(hWnd, &paint);
        return 0;

    case WM_CHAR:
#if defined(USE_QUICKTIMELIB)
        frame_number = video_POST_avi.frame_number;
#else
        mciStatusParams.dwCallback = 0;
        mciStatusParams.dwItem = MCI_STATUS_POSITION;
        mciStatusParams.dwTrack = 1;

        if (0 == mciSendCommandA(video_POST_mciId, MCI_STATUS, MCI_STATUS_ITEM, (uintptr_t)&mciStatusParams))
        {
            frame_number = (int32_t)mciStatusParams.dwReturn;
        }
        else frame_number = -1;
#endif

        if (frame_number >= 0)
        {
            for (index = 0; index < 3; index++)
            {
                if ((video_POST_stru_4CFA40[index].field00 <= frame_number) &&
                    (video_POST_stru_4CFA40[index].field04 > frame_number)
                   )
                {
                    if (wParam == VK_SPACE)
                    {
                        video_POST_dword_4CFA70 = video_POST_stru_4CFA40[index].field0c;
                        return 0;
                    }

                    if (wParam == VK_ESCAPE)
                    {
                        video_POST_dword_4CFA70 = video_POST_stru_4CFA40[index].field08;
                        if (index > 0)
                        {
                            video_POST_byte_4CFA78 = 1;
                            video_POST_dword_4CFA7C = index;
                        }
                        return 0;
                    }
                }
            }
        }

        return DefWindowProcA(hWnd, uMsg, wParam, lParam);

#if !defined(USE_QUICKTIMELIB)
    case MM_MCINOTIFY:
        if (!video_POST_byte_4CFA74)
        {
            video_POST_dword_4CFA70 = -1;
        }
        video_POST_byte_4CFA74 = 0;
        return 0;
#endif

    default:
        return DefWindowProcA(hWnd, uMsg, wParam, lParam);
    }
}

uint32_t CCALL video_RegisterClass_POST_Video(void)
{
    WNDCLASSA wndClass;

    wndClass.style = CS_NOCLOSE | CS_DBLCLKS;
    wndClass.lpfnWndProc = (WNDPROC)POST_VideoCallback_c;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = DSAWIN_GetInstance_c();
    wndClass.hIcon = NULL;
    wndClass.hCursor = NULL;
    wndClass.hbrBackground = NULL;
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = POST_windowClass;

    return RegisterClassA(&wndClass);
}

int CCALL video_Open_POST_Video(const char *path)
{
#if defined(USE_QUICKTIMELIB)
    return open_video(path, &video_POST_avi);
#else
    return -1;
#endif
}

int CCALL video_Close_POST_Video(void)
{
#if defined(USE_QUICKTIMELIB)
    return close_video(&video_POST_avi);
#else
    return -1;
#endif
}

int CCALL video_Play_POST_Video(uint32_t zoomed)
{
#if defined(USE_QUICKTIMELIB)
    int start_frame;

    if (!play_video_prepare(zoomed, &video_POST_avi))
    {
        return 0;
    }

    start_frame = 0;
    video_POST_dword_4CFA70 = -2;

    while (video_POST_dword_4CFA70 != -1)
    {
        if (!play_video_start(start_frame, &video_POST_avi))
        {
            break;
        }

        while (video_POST_dword_4CFA70 == -2)
        {
            if (!play_video_process(&video_POST_avi))
            {
                if (!video_POST_byte_4CFA74)
                {
                    video_POST_dword_4CFA70 = -1;
                }
                video_POST_byte_4CFA74 = 0;
                break;
            }

            SYSTEM_SystemTask_c();

            if (video_POST_byte_4CFA78)
            {
                if (video_POST_stru_4CFA40[video_POST_dword_4CFA7C].field04 <= video_POST_avi.frame_number)
                {
                    video_POST_dword_4CFA70 = -1;
                }
            }

            if (!SYSTEM_IsApplicationActive_c() && video_POST_byte_4CFA80)
            {
                // change: don't pause playback
                //video_POST_byte_4CFA80 = 0;
                //play_video_pause(&video_POST_avi);
            }

            if (SYSTEM_IsApplicationActive_c() && !video_POST_byte_4CFA80)
            {
                // change: don't resume playback
                //video_POST_byte_4CFA80 = 1;
                //play_video_resume(&video_POST_avi);
            }
        }

        if ((video_POST_dword_4CFA70 == -1) || video_POST_byte_4CFA78)
        {
            break;
        }

        video_POST_byte_4CFA74 = 1;
        play_video_stop(&video_POST_avi);

        start_frame = video_POST_stru_4CFA40[video_POST_dword_4CFA70].field00;
        video_POST_dword_4CFA70 = -2;
    }

    play_video_unprepare(&video_POST_avi);

    return 0;
#else
    return -1;
#endif
}


static LRESULT WINAPI SS_VideoCallback_c(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;

    uMsg = uMsg & 0xffff;

    switch (uMsg)
    {
    case WM_PAINT:
        BeginPaint(hWnd, &paint);
        EndPaint(hWnd, &paint);
        return 0;

    case WM_ERASEBKGND:
        return 0;

    case MM_MCINOTIFY:
        return 0;

    default:
        return DefWindowProcA(hWnd, uMsg, wParam, lParam);
    }
}

uint32_t CCALL video_RegisterClass_SS_Video(void)
{
    WNDCLASSA wndClass;

    wndClass.style = CS_NOCLOSE | CS_DBLCLKS;
    wndClass.lpfnWndProc = (WNDPROC)SS_VideoCallback_c;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = DSAWIN_GetInstance_c();
    wndClass.hIcon = NULL;
    wndClass.hCursor = NULL;
    wndClass.hbrBackground = NULL;
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = SS_windowClass;

    return RegisterClassA(&wndClass);
}

