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

#include "BBFX.h"
#include "BBDOS.h"
#include "BBMEM.h"
#include <windows.h>


#define GETLE32(_buf,_index) ( ((uint32_t)_buf[_index]) | (((uint32_t)_buf[(_index) + 1]) << 8) | (((uint32_t)_buf[(_index) + 2]) << 16) | (((uint32_t)_buf[(_index) + 3]) << 24) )
#define GETLE16(_buf,_index) ( ((uint32_t)_buf[_index]) | (((uint32_t)_buf[(_index) + 1]) << 8) )


typedef struct _FX_struct {
    uint8_t *total_ptr;
    int total_size;
    int priority;
    int ID;
    int remaining_loops;
    uint8_t *remaining_ptr;
    int remaining_size;
} FX_struct;


static int FX_initialized = 0;
static int FX_reserved = 0;
static int FX_counter = 0;
static int FX_waveOpened = 0;
static HWAVEOUT FX_hWaveOut = NULL;
static UINT_PTR FX_timerID = 0;
static WAVEHDR FX_waveHeaders[2] = {{0, 0, 0, 0, 0, 0, NULL, 0}, {0, 0, 0, 0, 0, 0, NULL, 0}};
static unsigned int FX_volume = 65535;
static int FX_inTimer = 0;
static int FX_outputEnabled = 1;
static DWORD FX_originalVolume = 0;
static int FX_deviceReserved = 0;
static const WAVEFORMATEX FX_waveFormat = {WAVE_FORMAT_PCM, 1, 11025, 11025, 1, 8, 0};
static int FX_sampleID[4];
static uint8_t FX_waveBuffers[2][2756];
static FX_struct FX_playingSamples[4];
static void *FX_libraries[128];


static void FX_StopPlaying_c(void)
{
    int index;

    if (!FX_deviceReserved)
    {
        return;
    }

    waveOutReset(FX_hWaveOut);

    if (FX_timerID != 0)
    {
        KillTimer(0, FX_timerID);
        FX_timerID = 0;
    }

    for (index = 0; index < 4; index++)
    {
        FX_playingSamples[index].total_size = 0;
        FX_playingSamples[index].remaining_size = 0;
    }
}

int CCALL FX_Init_c(void)
{
    int index;

    if (!FX_initialized)
    {
        for (index = 0; index < 128; index++)
        {
            FX_libraries[index] = NULL;
        }

        for (index = 0; index < 2756; index++)
        {
            FX_waveBuffers[0][index] = FX_waveBuffers[1][index] = 0x80;
        }

        FX_deviceReserved = (FX_ReserveDevices_c(1))?1:0;
    }

    FX_initialized++;
    return 1;
}

void CCALL FX_Exit_c(void)
{
    int index;

    FX_initialized--;
    if (FX_initialized > 0)
    {
        return;
    }

    FX_ReserveDevices_c(0);

    for (index = 0; index < 128; index++)
    {
        if (FX_libraries[index] != NULL)
        {
            MEM_free_c(FX_libraries[index]);
            FX_libraries[index] = NULL;
        }
    }

    FX_deviceReserved = 0;
}

int CCALL FX_ReserveDevices_c(int reserve)
{
    WAVEOUTCAPS caps;
    int index;

    if ((!FX_deviceReserved) && (FX_initialized > 0))
    {
        return 0;
    }

    if (reserve & 0xff)
    {
        if (FX_reserved)
        {
            return 1;
        }


        if (MMSYSERR_NOERROR != waveOutOpen(&FX_hWaveOut, WAVE_MAPPER, &FX_waveFormat, 0, 0, WAVE_ALLOWSYNC))
        {
            FX_waveOpened = 0;
            FX_hWaveOut = NULL;
            return 0;
        }

        FX_waveOpened = 1;
        FX_reserved = 1;

        waveOutGetDevCapsA(WAVE_MAPPER, &caps, sizeof(WAVEOUTCAPS));
        waveOutGetVolume(FX_hWaveOut, &FX_originalVolume);
        waveOutSetVolume(FX_hWaveOut, (FX_volume & 0xffff) | ((FX_volume & 0xffff) << 16));

        for (index = 0; index < 2; index++)
        {
            FX_waveHeaders[index].lpData = (LPSTR) &(FX_waveBuffers[index]);
            FX_waveHeaders[index].dwBufferLength = 2756;
            FX_waveHeaders[index].dwFlags = 0;
            FX_waveHeaders[index].dwLoops = 0;

            if (MMSYSERR_NOERROR != waveOutPrepareHeader(FX_hWaveOut, &(FX_waveHeaders[index]), sizeof(WAVEHDR)))
            {
                for (index--; index >= 0; index--)
                {
                    waveOutUnprepareHeader(FX_hWaveOut, &(FX_waveHeaders[index]), sizeof(WAVEHDR));
                }

                waveOutClose(FX_hWaveOut);
                FX_waveOpened = 0;
                FX_hWaveOut = NULL;
                return 0;
            }
        }

        for (index = 0; index < 4; index++)
        {
            FX_playingSamples[index].total_ptr = NULL;
            FX_playingSamples[index].total_size = 0;
            FX_playingSamples[index].priority = 0;
        }

        return 1;
    }
    else
    {
        if (FX_hWaveOut == NULL)
        {
            return 1;
        }

        if (!FX_reserved)
        {
            return 1;
        }

        FX_StopPlaying_c();

        for (index = 0; index < 2; index++)
        {
            waveOutUnprepareHeader(FX_hWaveOut, &(FX_waveHeaders[index]), sizeof(WAVEHDR));
        }

        waveOutSetVolume(FX_hWaveOut, (FX_originalVolume & 0xffff) | ((FX_originalVolume & 0xffff) << 16));
        waveOutClose(FX_hWaveOut);
        FX_hWaveOut = NULL;
        FX_waveOpened = 0;
        FX_reserved = 0;

        return 1;
    }
}

int CCALL FX_ReadLib_c(const char *path)
{
    int index;

    for (index = 0; index < 128; index++)
    {
        if (FX_libraries[index] == NULL) break;
    }

    if (index >= 128)
    {
        return -1;
    }

    FX_libraries[index] = DOS_ReadFile_c(path, NULL);
    if (FX_libraries[index] == NULL)
    {
        return -1;
    }

    return index;
}

void CCALL FX_FreeLib_c(int lib_handle)
{
    if ((lib_handle >= 0) && (lib_handle < 128))
    {
        if (FX_libraries[lib_handle] != NULL)
        {
            MEM_free_c(FX_libraries[lib_handle]);
            FX_libraries[lib_handle] = NULL;
        }
    }
}

void CCALL FX_StopAllSamples_c(void)
{
    if (FX_waveOpened)
    {
        FX_StopPlaying_c();
    }
}

static VOID CALLBACK FX_TimerFunc(HWND a1, UINT a2, UINT_PTR a3, DWORD a4)
{
    uint8_t *buffer;
    unsigned int accum;
    uint8_t *samples_ptr[4];
    int header;
    int num_samples;
    int index;
    int samples_size[4];
    int buf_index;

    header = -1;
    num_samples = 0;

    if (FX_inTimer)
    {
        return;
    }

    FX_inTimer = 1;

    if (FX_waveHeaders[0].dwFlags & WHDR_DONE)
    {
        header = 0;
    }
    else if (FX_waveHeaders[1].dwFlags & WHDR_DONE)
    {
        header = 1;
    }

    if (header != -1)
    {
        for (index = 0; index < 4; index++)
        {
            if (FX_playingSamples[index].remaining_size <= 0)
            {
                samples_ptr[index] = NULL;
                FX_sampleID[index] = -1;
            }
            else
            {
                samples_ptr[index] = FX_playingSamples[index].remaining_ptr;
                samples_size[index] = FX_playingSamples[index].remaining_size;
                if (samples_size[index] >= 2756)
                {
                    samples_size[index] = 2756;
                }
                FX_playingSamples[index].remaining_ptr += samples_size[index];
                FX_playingSamples[index].remaining_size -= samples_size[index];
                FX_sampleID[index] = FX_playingSamples[index].ID;

                if (FX_playingSamples[index].remaining_size == 0)
                {
                    if (FX_playingSamples[index].remaining_loops <= 0)
                    {
                        FX_playingSamples[index].total_size = 0;
                    }
                    else
                    {
                        FX_playingSamples[index].remaining_ptr = FX_playingSamples[index].total_ptr;
                        FX_playingSamples[index].remaining_size = FX_playingSamples[index].total_size;
                        FX_playingSamples[index].remaining_loops--;
                    }
                }
                num_samples++;
            }
        }

        FX_waveHeaders[header].dwFlags &= ~WHDR_DONE;
        buffer = (uint8_t *)FX_waveHeaders[header].lpData;
        for (buf_index = 0; buf_index < 2756; buf_index++)
        {
            accum = 0;
            for (index = 0; index < 4; index++)
            {
                if (samples_ptr[index] != NULL)
                {
                    accum += samples_ptr[index][0];
                    samples_ptr[index]++;
                    samples_size[index]--;
                    if (samples_size[index] == 0)
                    {
                        samples_ptr[index] = NULL;
                    }
                }
                else
                {
                    accum += 0x80;
                }
            }

            buffer[buf_index] = accum >> 2;
        }

        waveOutWrite(FX_hWaveOut, &(FX_waveHeaders[header]), sizeof(WAVEHDR));
    }

    FX_inTimer = 0;
}

int CCALL FX_PlaySample_c(int lib_handle, int sample_number, int priority, int volume, int times_play)
{
    uint8_t *library, *entries;
    unsigned int num_samples, size, sample_offset;
    int index, free_sample, min_priority;

    free_sample = -1;
    min_priority = 100;

    if (!FX_deviceReserved)
    {
        return 0;
    }

    if (!FX_outputEnabled)
    {
        return 1;
    }

    if (!FX_waveOpened)
    {
        return -1;
    }

    if ((lib_handle < 0) || (sample_number < 0))
    {
        return -1;
    }

    if (lib_handle >= 128)
    {
        return -1;
    }

    library = (uint8_t *)(FX_libraries[lib_handle]);
    if (library == NULL)
    {
        return -1;
    }

    num_samples = GETLE16(library, 8);
    // change: fix condition
    //if (sample_number > num_samples)
    if ((unsigned int)sample_number >= num_samples)
    {
        return -1;
    }

    if (priority > 99) priority = 99;
    if (priority < 0) priority = 0;
    if (volume > 65535) volume = 65535;
    if (volume < 0) volume = 0;

    entries = &(library[GETLE32(library, 0)]);
    sample_offset = GETLE32(entries, 12 * sample_number + 8);
    if (sample_number == (num_samples - 1))
    {
        size = GETLE32(library, 0) - sample_offset;
    }
    else
    {
        size = GETLE32(entries, 12 * (sample_number + 1) + 8) - sample_offset;
    }

    if (size <= 200)
    {
        return -1;
    }

    for (index = 0; index < 4; index++)
    {
        if (FX_playingSamples[index].total_size == 0)
        {
            free_sample = index;
            break;
        }

        if ((priority >= FX_playingSamples[index].priority) && (min_priority >= FX_playingSamples[index].priority))
        {
            free_sample = index;
            min_priority = FX_playingSamples[index].priority;
        }
    }

    if (free_sample == -1)
    {
        return -1;
    }

    *((volatile int *) &FX_playingSamples[free_sample].remaining_size) = 0;
    *((volatile int *) &FX_playingSamples[free_sample].total_size) = 0;

    FX_playingSamples[free_sample].total_ptr = &(library[sample_offset + 50]);
    FX_playingSamples[free_sample].remaining_ptr = &(library[sample_offset + 50]);
    FX_playingSamples[free_sample].priority = priority;
    FX_playingSamples[free_sample].remaining_loops = times_play - 1;
    FX_playingSamples[free_sample].ID = FX_counter;
    FX_counter++;
    FX_playingSamples[free_sample].total_size = size - 200;
    FX_playingSamples[free_sample].remaining_size = size - 200;

    if (FX_timerID == 0)
    {
        for (index = 0; index < 2; index++)
        {
            FX_waveHeaders[index].dwFlags |= WHDR_DONE;
        }

        FX_timerID = SetTimer(0, 0, 10, FX_TimerFunc);
        if (FX_timerID == 0)
        {
            FX_playingSamples[free_sample].priority = 0;
            return -1;
        }
    }

    return FX_playingSamples[free_sample].ID;
}

void CCALL FX_SetVolume_c(unsigned int volume)
{
    if (!FX_deviceReserved)
    {
        return;
    }

    waveOutSetVolume(FX_hWaveOut, (volume & 0xffff) | ((volume & 0xffff) << 16));
    FX_volume = volume;
    FX_outputEnabled = (volume != 0)?1:0;
}

int CCALL FM_IsError_c(void)
{
    return 0;
}

