/**
 *
 *  Copyright (C) 2020-2024 Roman Pauer
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

#include "SDImidi.h"
#include "midi-plugins.h"
#include "midi-plugins2.h"
#include "Game-Config.h"
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#if defined(__WINE__)
    #undef MIDI_WINDOWS
#else
    #define MIDI_WINDOWS 1
#endif

#if !defined(MIDI_WINDOWS)
    #include <dlfcn.h>
#endif


#define MS_STOPPED 0
#define MS_OPENED  1
#define MS_PLAYING 2


#define BUFFER_SIZE 28672

#define MP_STOPPED 0
#define MP_PLAYING 1
#define MP_PAUSED  2
#define MP_STARTED 3


typedef struct _MP_midi_ {
    uint16_t bytes_left[2], start_index, reserved;
    volatile uint8_t read_buffer, write_buffer, status;
    uint8_t end_of_midi;
    int32_t loop_count;
    void *midi;
    CRITICAL_SECTION critsec;
    WAVEHDR header[2];
    uint8_t buffer[2][BUFFER_SIZE];
} MP_midi;


#if defined(MIDI_WINDOWS)
    static HMODULE MP_handle = NULL;
    static HMODULE MP2_handle = NULL;
#else
    static void *MP_handle = NULL;
    static void *MP2_handle = NULL;
#endif

static midi_plugin_functions MP_functions;
static midi_plugin2_functions MP2_functions;

static void *midi_buffer;
static int midi_status, midi_size;

static MP_midi MP_sequence;

static HWAVEOUT hWaveOut;

static HANDLE MP_thread;
static volatile int thread_finish;


static DWORD WINAPI MP_ProcessData(LPVOID lpParameter)
{
    while (!thread_finish)
    {
        if (TryEnterCriticalSection(&(MP_sequence.critsec)))
        {
            if (MP_sequence.midi != NULL)
            {
                if (MP_sequence.status == MP_PLAYING)
                {
                    if (MP_sequence.header[MP_sequence.read_buffer & 1].dwFlags & WHDR_DONE)
                    {
                        MP_sequence.header[MP_sequence.read_buffer & 1].dwFlags &= ~WHDR_DONE;

                        MP_sequence.start_index = 0;
                        MP_sequence.read_buffer = (MP_sequence.read_buffer + 1) & 3;
                    }

                    if (MP_sequence.end_of_midi)
                    {
                        if (MP_sequence.read_buffer == MP_sequence.write_buffer)
                        {
                            MP_sequence.status = MP_STOPPED;
                        }
                    }
                    else
                    {
                        if ((((MP_sequence.read_buffer + 1) & 3) == MP_sequence.write_buffer) ||
                            (MP_sequence.read_buffer == MP_sequence.write_buffer)
                           )
                        {
                            int write_buffer;

                            write_buffer = MP_sequence.write_buffer & 1;

                            MP_sequence.bytes_left[write_buffer] = MP_functions.get_data(MP_sequence.midi, (char *) MP_sequence.buffer[write_buffer], BUFFER_SIZE);
                            if (MP_sequence.bytes_left[write_buffer] != BUFFER_SIZE)
                            {
                                MP_functions.rewind_midi(MP_sequence.midi);

                                if (MP_sequence.loop_count != 0)
                                {
                                    MP_sequence.loop_count--;
                                    if (MP_sequence.loop_count == 0)
                                    {
                                        MP_sequence.loop_count = 1;
                                        MP_sequence.end_of_midi = 1;
                                    }
                                }

                                if (!MP_sequence.end_of_midi)
                                {
                                    int size;

                                    size = MP_functions.get_data(MP_sequence.midi, (char *) &(MP_sequence.buffer[write_buffer][MP_sequence.bytes_left[write_buffer]]), BUFFER_SIZE - MP_sequence.bytes_left[write_buffer]);
                                    MP_sequence.bytes_left[write_buffer] += size;
                                }
                                else
                                {
                                    memset(&(MP_sequence.buffer[write_buffer][MP_sequence.bytes_left[write_buffer]]), 0, BUFFER_SIZE - MP_sequence.bytes_left[write_buffer]);
                                }
                            }

                            if (MP_sequence.read_buffer == MP_sequence.write_buffer)
                            {
                                MP_sequence.start_index = 0;
                            }

                            waveOutWrite(hWaveOut, &(MP_sequence.header[write_buffer]), sizeof(WAVEHDR));

                            MP_sequence.write_buffer = (MP_sequence.write_buffer + 1) & 3;
                        }
                    }
                }
                else if (MP_sequence.status == MP_STARTED)
                {
                    MP_sequence.end_of_midi = 0;

                    if ((((MP_sequence.read_buffer + 1) & 3) == MP_sequence.write_buffer) ||
                        (MP_sequence.read_buffer == MP_sequence.write_buffer)
                       )
                    {
                        int write_buffer;

                        write_buffer = MP_sequence.write_buffer & 1;

                        MP_sequence.bytes_left[write_buffer] = MP_functions.get_data(MP_sequence.midi, (char *) MP_sequence.buffer[write_buffer], BUFFER_SIZE);

                        if (MP_sequence.read_buffer == MP_sequence.write_buffer)
                        {
                            MP_sequence.start_index = 0;
                        }

                        waveOutWrite(hWaveOut, &(MP_sequence.header[write_buffer]), sizeof(WAVEHDR));

                        MP_sequence.write_buffer = (MP_sequence.write_buffer + 1) & 3;
                    }

                    MP_sequence.status = MP_PLAYING;
                }
            }

            LeaveCriticalSection(&(MP_sequence.critsec));
        }

        Sleep(10);
    }

    return 0;
}

static int MP_Startup(void)
{
    const char *plugin_name;
    midi_plugin_initialize MP_initialize;
    midi_plugin_parameters MP_parameters;
    WAVEFORMATEX waveFormat;
    MMRESULT res;
    unsigned int rate;

    if (MP_handle != NULL) return 0;

#if defined(MIDI_WINDOWS)
    #define free_library FreeLibrary
    #define get_proc_address GetProcAddress

    if (Audio_MidiSubsystem == 1) plugin_name = ".\\midi-wildmidi.dll";
    else if (Audio_MidiSubsystem == 2) plugin_name = ".\\midi-bassmidi.dll";
    else if (Audio_MidiSubsystem == 3) plugin_name = ".\\midi-adlmidi.dll";
    else
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "unknown plugin");
        return 1;
    }

    fprintf(stderr, "%s: loading dynamic library: %s\n", "midi", plugin_name);
    MP_handle = LoadLibraryA(plugin_name);

    if (MP_handle == NULL)
    {
        fprintf(stderr, "%s: load error: 0x%x\n", "midi", (unsigned int)GetLastError());
        return 2;
    }
#else
    #define free_library dlclose
    #define get_proc_address dlsym

    if (Audio_MidiSubsystem == 1) plugin_name = "./midi-wildmidi.so";
    else if (Audio_MidiSubsystem == 2) plugin_name = "./midi-bassmidi.so";
    else if (Audio_MidiSubsystem == 3) plugin_name = "./midi-adlmidi.so";
    else
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "unknown plugin");
        return 1;
    }

    fprintf(stderr, "%s: loading shared object: %s\n", "midi", plugin_name);
    MP_handle = dlopen(plugin_name, RTLD_LAZY);

    if (MP_handle == NULL)
    {
        fprintf(stderr, "%s: load error: %s\n", "midi", dlerror());
        return 2;
    }
#endif

    MP_initialize = (midi_plugin_initialize) get_proc_address(MP_handle, MIDI_PLUGIN_INITIALIZE);

    if (MP_initialize == NULL)
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "initialization function not available in plugin");
        free_library(MP_handle);
        MP_handle = NULL;
        return 3;
    }

    rate = 44100;
    if (Audio_MidiResamplingQuality > 0)
    {
        WAVEOUTCAPS caps;

        res = waveOutGetDevCaps(WAVE_MAPPER, &caps, sizeof(WAVEOUTCAPS));
        if (MMSYSERR_NOERROR == res)
        {
            if (caps.dwFormats & WAVE_FORMAT_96S16)
            {
                rate = 96000;
            }
        }
    }

    memset(&MP_parameters, 0, sizeof(MP_parameters));
    MP_parameters.soundfont_path = Audio_SoundFontPath;
    MP_parameters.opl3_bank_number = Audio_OPL3BankNumber;
    MP_parameters.opl3_emulator = Audio_OPL3Emulator;
    MP_parameters.resampling_quality = Audio_MidiResamplingQuality;
    MP_parameters.sampling_rate = rate;

    if (MP_initialize(0, &MP_parameters, &MP_functions))
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "failed to initialize plugin");
        free_library(MP_handle);
        MP_handle = NULL;
        return 4;
    }

    // initialize buffers
    memset(&(MP_sequence), 0, (char *) &(MP_sequence.buffer) - (char *) &(MP_sequence));
    InitializeCriticalSection(&(MP_sequence.critsec));

    // initialize waveout
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 2;
    waveFormat.nSamplesPerSec = rate;
    waveFormat.nAvgBytesPerSec = 4 * rate;
    waveFormat.nBlockAlign = 4;
    waveFormat.wBitsPerSample = 16;
    waveFormat.cbSize = 0;

    res = waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, WAVE_ALLOWSYNC);
    if (MMSYSERR_NOERROR != res)
    {
        fprintf(stderr, "%s: %s error: 0x%x\n", "midi", "waveOutOpen", res);
        DeleteCriticalSection(&(MP_sequence.critsec));
        MP_functions.shutdown_plugin();
        free_library(MP_handle);
        MP_handle = NULL;
        return 5;
    }

    MP_sequence.header[0].lpData = (LPSTR) &(MP_sequence.buffer[0]);
    MP_sequence.header[0].dwBufferLength = BUFFER_SIZE;
    MP_sequence.header[1].lpData = (LPSTR) &(MP_sequence.buffer[1]);
    MP_sequence.header[1].dwBufferLength = BUFFER_SIZE;

    res = waveOutPrepareHeader(hWaveOut, &(MP_sequence.header[0]), sizeof(WAVEHDR));
    if (MMSYSERR_NOERROR != res)
    {
        fprintf(stderr, "%s: %s error: 0x%x\n", "midi", "waveOutPrepareHeader", res);
        waveOutClose(hWaveOut);
        DeleteCriticalSection(&(MP_sequence.critsec));
        MP_functions.shutdown_plugin();
        free_library(MP_handle);
        MP_handle = NULL;
        return 5;
    }
    res = waveOutPrepareHeader(hWaveOut, &(MP_sequence.header[1]), sizeof(WAVEHDR));
    if (MMSYSERR_NOERROR != res)
    {
        fprintf(stderr, "%s: %s error: 0x%x\n", "midi", "waveOutPrepareHeader", res);
        waveOutUnprepareHeader(hWaveOut, &(MP_sequence.header[0]), sizeof(WAVEHDR));
        waveOutClose(hWaveOut);
        DeleteCriticalSection(&(MP_sequence.critsec));
        MP_functions.shutdown_plugin();
        free_library(MP_handle);
        MP_handle = NULL;
        return 5;
    }

    MP_functions.set_master_volume(Audio_MidiVolume);

    // start thread
    thread_finish = 0;

    MP_thread = CreateThread(NULL, 0, MP_ProcessData, NULL, 0, NULL);
    if (MP_thread == NULL)
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "failed to create thread");
        DeleteCriticalSection(&(MP_sequence.critsec));
        MP_functions.shutdown_plugin();
        free_library(MP_handle);
        MP_handle = NULL;
        return 6;
    }

    midi_status = MS_STOPPED;

    fprintf(stderr, "%s: OK\n", "midi");

    return 0;

#undef get_proc_address
#undef free_library
}

static void MP_Shutdown(void)
{
    if (MP_handle == NULL) return;

    // stop thread
    thread_finish = 1;
    WaitForSingleObjectEx(MP_thread, INFINITE, FALSE);
    CloseHandle(MP_thread);

    // close waveout
    waveOutReset(hWaveOut);
    waveOutUnprepareHeader(hWaveOut, &(MP_sequence.header[1]), sizeof(WAVEHDR));
    waveOutUnprepareHeader(hWaveOut, &(MP_sequence.header[0]), sizeof(WAVEHDR));
    waveOutClose(hWaveOut);

    // close midi files
    if (MP_sequence.midi != NULL)
    {
        MP_functions.close_midi(MP_sequence.midi);
        MP_sequence.midi = NULL;
    }
    DeleteCriticalSection(&(MP_sequence.critsec));

    MP_functions.shutdown_plugin();
#if defined(MIDI_WINDOWS)
    FreeLibrary(MP_handle);
#else
    dlclose(MP_handle);
#endif
    MP_handle = NULL;
}


static int MP2_Startup(void)
{
    const char *plugin_name;
    midi_plugin2_initialize MP2_initialize;
    midi_plugin2_parameters MP2_parameters;

    if (MP2_handle != NULL) return 0;

#if defined(MIDI_WINDOWS)
    #define free_library FreeLibrary
    #define get_proc_address GetProcAddress

    if (Audio_MidiSubsystem == 11 || Audio_MidiSubsystem == 21) plugin_name = ".\\midi2-windows.dll";
    else if (Audio_MidiSubsystem == 12 || Audio_MidiSubsystem == 22) plugin_name = ".\\midi2-alsa.dll";
    else
    {
        fprintf(stderr, "%s: error: %s\n", "midi2", "unknown plugin");
        return 1;
    }

    fprintf(stderr, "%s: loading dynamic library: %s\n", "midi2", plugin_name);
    MP2_handle = LoadLibraryA(plugin_name);

    if (MP2_handle == NULL)
    {
        fprintf(stderr, "%s: load error: 0x%x\n", "midi2", (unsigned int)GetLastError());
        return 2;
    }
#else
    #define free_library dlclose
    #define get_proc_address dlsym

    if (Audio_MidiSubsystem == 11 || Audio_MidiSubsystem == 21) plugin_name = "./midi2-windows.so";
    else if (Audio_MidiSubsystem == 12 || Audio_MidiSubsystem == 22) plugin_name = "./midi2-alsa.so";
    else
    {
        fprintf(stderr, "%s: error: %s\n", "midi2", "unknown plugin");
        return 1;
    }

    fprintf(stderr, "%s: loading shared object: %s\n", "midi2", plugin_name);
    MP2_handle = dlopen(plugin_name, RTLD_LAZY);

    if (MP2_handle == NULL)
    {
        fprintf(stderr, "%s: load error: %s\n", "midi2", dlerror());
        return 2;
    }
#endif

    MP2_initialize = (midi_plugin2_initialize) get_proc_address(MP2_handle, MIDI_PLUGIN2_INITIALIZE);

    if (MP2_initialize == NULL)
    {
        fprintf(stderr, "%s: error: %s\n", "midi2", "initialization function not available in plugin");
        free_library(MP2_handle);
        MP2_handle = NULL;
        return 3;
    }

    memset(&MP2_parameters, 0, sizeof(MP2_parameters));
    MP2_parameters.midi_device_name = Audio_MidiDevice;
    if (Audio_MidiSubsystem > 20)
    {
        MP2_parameters.midi_type = 2;
    }
    MP2_parameters.mt32_delay = Audio_MT32DelaySysex;
    MP2_parameters.mt32_display_text = "  Battle Isle 2220";

    if (MP2_initialize(&MP2_parameters, &MP2_functions))
    {
        fprintf(stderr, "%s: error: %s\n", "midi2", "failed to initialize plugin");
        free_library(MP2_handle);
        MP2_handle = NULL;
        return 4;
    }

    midi_buffer = NULL;
    midi_status = MS_STOPPED;

    MP2_functions.set_volume(Audio_MidiVolume);

    fprintf(stderr, "%s: OK\n", "midi2");

    return 0;

#undef get_proc_address
#undef free_library
}

static void MP2_Shutdown(void)
{
    if (MP2_handle == NULL) return;

    MP2_functions.halt();

    MP2_functions.shutdown_plugin();
#if defined(MIDI_WINDOWS)
    FreeLibrary(MP2_handle);
#else
    dlclose(MP2_handle);
#endif
    MP2_handle = NULL;
}


int midi_PluginStartup(void) __attribute__((noinline));
int midi_PluginStartup(void)
{
    if (Audio_MidiSubsystem == 0) return 0;

    if (Audio_MidiSubsystem <= 10)
    {
        return MP_Startup();
    }
    else
    {
        return MP2_Startup();
    }
}

void midi_PluginShutdown(void)
{
    if (Audio_MidiSubsystem == 0) return;

    if (Audio_MidiSubsystem <= 10)
    {
        MP_Shutdown();
    }
    else
    {
        MP2_Shutdown();
    }
}


static void unload_midi(void)
{
    if (midi_buffer != NULL)
    {
        HeapFree(GetProcessHeap(), 0, midi_buffer);

        midi_buffer = NULL;
    }
}

static int load_midi(const char *filename)
{
    HANDLE hFile;
    DWORD dwFileSize, dwFileSizeHigh, dwBytesRead;
    BOOL bResult;

    hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return 0;

    dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
    if ((dwFileSize == INVALID_FILE_SIZE) || (dwFileSizeHigh != 0))
    {
        CloseHandle(hFile);
        return 0;
    }

    midi_buffer = HeapAlloc(GetProcessHeap(), 0, dwFileSize);
    if (midi_buffer == NULL)
    {
        CloseHandle(hFile);
        return 0;
    }

    bResult = ReadFile(hFile, midi_buffer, dwFileSize, &dwBytesRead, NULL);
    if ((!bResult) || (dwBytesRead != dwFileSize))
    {
        unload_midi();
        CloseHandle(hFile);
        return 0;
    }

    CloseHandle(hFile);

    midi_size = dwFileSize;

    return 1;
}


int midi_OpenSDIMusic(const char *filename)
{
    // change:
    if (Audio_MidiSubsystem == 0)
    {
        char strCommand[1024];
        char strReturnString[128];

        sprintf(strCommand, "open sequencer!%s alias sdimusic", filename);
        return mciSendStringA(strCommand, strReturnString, 128, NULL);
    }

    if (Audio_MidiSubsystem <= 10)
    {
        if (MP_handle == NULL)
        {
            midi_status = MS_OPENED;
            return 0;
        }

        EnterCriticalSection(&(MP_sequence.critsec));

        waveOutReset(hWaveOut);
        MP_sequence.status = MP_STOPPED;
        MP_sequence.read_buffer = 0;
        MP_sequence.write_buffer = 0;
        MP_sequence.end_of_midi = 0;

        if (MP_sequence.midi != NULL)
        {
            MP_functions.close_midi(MP_sequence.midi);
            MP_sequence.midi = NULL;
        }

        unload_midi();

        midi_status = MS_STOPPED;

        if (!load_midi(filename))
        {
            LeaveCriticalSection(&(MP_sequence.critsec));
            return 2;
        }

        MP_sequence.loop_count = 0;
        MP_sequence.midi = MP_functions.open_buffer(midi_buffer, midi_size);
        LeaveCriticalSection(&(MP_sequence.critsec));

        if (MP_sequence.midi == NULL) return 2;
    }
    else
    {
        if (MP2_handle == NULL)
        {
            midi_status = MS_OPENED;
            return 0;
        }

        MP2_functions.halt();
        unload_midi();

        midi_status = MS_STOPPED;

        if (!load_midi(filename)) return 2;
    }

    midi_status = MS_OPENED;
    return 0;
}

unsigned int midi_GetSDIMusicID(void)
{
    // change:
    if (Audio_MidiSubsystem == 0)
    {
        return mciGetDeviceIDA("sdimusic");
    }

    if (Audio_MidiSubsystem <= 10)
    {
        if (MP_handle == NULL) return 2;
    }
    else
    {
        if (MP2_handle == NULL) return 2;
    }

    return (midi_status != MS_STOPPED)?1:0;
}

int midi_PlaySDIMusic(void)
{
    // change:
    if (Audio_MidiSubsystem == 0)
    {
        char strReturnString[128];

        return mciSendStringA("play sdimusic from 0", strReturnString, 128, NULL);
    }

    if (Audio_MidiSubsystem <= 10)
    {
        if (MP_handle == NULL)
        {
            midi_status = MS_PLAYING;
            return 0;
        }

        if (midi_status == MS_STOPPED) return 3;

        if (midi_status == MS_OPENED)
        {
            EnterCriticalSection(&(MP_sequence.critsec));

            MP_sequence.status = MP_STARTED;

            LeaveCriticalSection(&(MP_sequence.critsec));

            midi_status = MS_PLAYING;
        }
    }
    else
    {
        if (MP2_handle == NULL)
        {
            midi_status = MS_PLAYING;
            return 0;
        }

        if (midi_status == MS_STOPPED) return 3;

        if (midi_status == MS_OPENED)
        {
            if (MP2_functions.play(midi_buffer, midi_size, -1)) return 4;

            midi_status = MS_PLAYING;
        }
    }

    return 0;
}

int midi_CloseSDIMusic(void)
{
    // change:
    if (Audio_MidiSubsystem == 0)
    {
        char strReturnString[128];

        return mciSendStringA("close sdimusic", strReturnString, 128, NULL);
    }

    if (Audio_MidiSubsystem <= 10)
    {
        if (MP_handle == NULL)
        {
            midi_status = MS_STOPPED;
            return 0;
        }

        EnterCriticalSection(&(MP_sequence.critsec));

        waveOutReset(hWaveOut);
        MP_sequence.status = MP_STOPPED;
        MP_sequence.read_buffer = 0;
        MP_sequence.write_buffer = 0;

        if (MP_sequence.midi != NULL)
        {
            MP_functions.close_midi(MP_sequence.midi);
            MP_sequence.midi = NULL;
        }

        LeaveCriticalSection(&(MP_sequence.critsec));

        unload_midi();
    }
    else
    {
        if (MP2_handle == NULL)
        {
            midi_status = MS_STOPPED;
            return 0;
        }

        MP2_functions.halt();
        unload_midi();
    }

    midi_status = MS_STOPPED;
    return 0;
}

int midi_IsPlaying(unsigned int musicID)
{
    // change:
    if (Audio_MidiSubsystem == 0)
    {
        MCI_STATUS_PARMS mciStatusParms;

        mciStatusParms.dwCallback = 0;
        mciStatusParms.dwItem = MCI_STATUS_MODE;

        if (0 != mciSendCommandA(musicID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatusParms))
        {
            return 0;
        }

        return ((mciStatusParms.dwReturn == MCI_MODE_PLAY) || (mciStatusParms.dwReturn == MCI_MODE_SEEK))?1:0;
    }

    if (Audio_MidiSubsystem <= 10)
    {
        if (MP_handle == NULL)
        {
            return (midi_status == MS_PLAYING)?1:0;
        }
    }
    else
    {
        if (MP2_handle == NULL)
        {
            return (midi_status == MS_PLAYING)?1:0;
        }
    }
    if (musicID == 0) return 0;

    return (midi_status == MS_PLAYING)?1:0;
}


int midi_OpenTestMusic(void)
{
    // change:
    if (Audio_MidiSubsystem == 0)
    {
        char strReturnString[1024];

        return mciSendStringA("open sequencer!TST.MID alias testmusic", strReturnString, 1024, NULL);
    }

    return midi_OpenSDIMusic("TST.MID");
}

int midi_PlayTestMusic(void)
{
    // change:
    if (Audio_MidiSubsystem == 0)
    {
        char strReturnString[1024];

        return mciSendStringA("play testmusic from 0", strReturnString, 1024, NULL);
    }

    return midi_PlaySDIMusic();
}

int midi_CloseTestMusic(void)
{
    // change:
    if (Audio_MidiSubsystem == 0)
    {
        char strReturnString[128];

        return mciSendStringA("close testmusic", strReturnString, 128, NULL);
    }

    return midi_CloseSDIMusic();
}


int midi_GetErrorString(int error, char *text, unsigned int length)
{
    // change:
    if (Audio_MidiSubsystem == 0)
    {
        return mciGetErrorStringA(error, text, length);
    }

    const char *errorText;

    if (text == NULL) return 0;

    switch (error)
    {
    case 1:
        errorText = "MIDI plugin is not loaded";
        break;
    case 2:
        errorText = "MIDI file could not be loaded";
        break;
    case 3:
        errorText = "MIDI file is not loaded";
        break;
    case 4:
        errorText = "MIDI file could not start playing";
        break;
    default:
        errorText = NULL;
        break;
    }

    if (errorText == NULL) return 0;

    if (NULL == lstrcpynA(text, errorText, length))
    {
        *text = 0;
        return 0;
    }

    return 1;
}

