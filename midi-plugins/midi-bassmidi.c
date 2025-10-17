/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
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

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
#else
    #define _FILE_OFFSET_BITS 64
    #include <unistd.h>
    #include <sys/types.h>
    #include <dirent.h>
#endif

#include <string.h>
#include <malloc.h>
#include <stdint.h>
#include "midi-plugins.h"
#include "bass.h"
#include "bassmidi.h"

#ifdef _MSC_VER
    #define EXPORT __declspec(dllexport)
    #define strdup _strdup
#elif defined __GNUC__
    #define EXPORT __attribute__ ((visibility ("default")))
#else
    #define EXPORT
#endif

static HSOUNDFONT soundfont_handle = 0;
static int resampling_quality = 0;


static int file_exists(char const *filename)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    DWORD dwAttrib;
#endif

    if (filename == NULL) return 0;
    if (*filename == 0) return 0;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    dwAttrib = GetFileAttributesA(filename);
    if ((dwAttrib == INVALID_FILE_ATTRIBUTES) || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
#else
    if (access(filename, F_OK))
#endif
    {
        return 0;
    }
    else
    {
        return 1;
    }
}




static int set_master_volume(unsigned char master_volume) // master_volume = 0 - 127
{
    if (soundfont_handle == 0) return -2;

    if (master_volume > 127) master_volume = 127;

    if (!BASS_MIDI_FontSetVolume(soundfont_handle, ((float)master_volume) / 127.0f))
    {
        return -1;
    }

    return 0;
}

static void *open_file(char const *midifile)
{
    HSTREAM stream;

    if (midifile == NULL) return NULL;

    stream = BASS_MIDI_StreamCreateFile(0, midifile, 0, 0, BASS_STREAM_DECODE, 0);
    if (stream != 0)
    {
        if (resampling_quality > 0)
        {
            BASS_ChannelSetAttribute(stream, BASS_ATTRIB_MIDI_SRC, 1);
        }
    }

    return (void *)(uintptr_t) stream;
}

static void *open_buffer(void const *midibuffer, long int size)
{
    HSTREAM stream;

    if (midibuffer == NULL) return NULL;
    if (size <= 0) return NULL;

    stream = BASS_MIDI_StreamCreateFile(1, midibuffer, 0, size, BASS_STREAM_DECODE, 0);
    if (stream != 0)
    {
        if (resampling_quality > 0)
        {
            BASS_ChannelSetAttribute(stream, BASS_ATTRIB_MIDI_SRC, 1);
        }
    }

    return (void *)(uintptr_t) stream;
}

static long int get_data(void *handle, void *buffer, long int size)
{
    if (handle == NULL) return -2;
    if (buffer == NULL) return -3;
    if (size < 0) return -4;
    if (size < 4) return 0;

    return BASS_ChannelGetData((HSTREAM)(uintptr_t)handle, buffer, size);
}

static int rewind_midi(void *handle)
{
    QWORD curpos;

    if (handle == NULL) return -2;

    curpos = BASS_ChannelGetPosition((HSTREAM)(uintptr_t)handle, BASS_POS_BYTE);
    if (curpos == 0) return 0; // this helps to work around some bug

    if (!BASS_ChannelSetPosition((HSTREAM)(uintptr_t)handle, 0, BASS_POS_BYTE | BASS_MUSIC_POSRESETEX))
    {
        return -1;
    }

    return 0;
}

static int close_midi(void *handle)
{
    if (handle == NULL) return -2;

    if (!BASS_StreamFree((HSTREAM)(uintptr_t)handle))
    {
        return -1;
    }

    return 0;
}

static void shutdown_plugin(void)
{
    if (soundfont_handle)
    {
        BASS_MIDI_FontFree(soundfont_handle);
        soundfont_handle = 0;
    }
    BASS_Free();
}


EXPORT
int initialize_midi_plugin(unsigned short int rate, midi_plugin_parameters const *parameters, midi_plugin_functions *functions)
{
    char const *soundfont_sf2;
    char *soundfont_name;
    BASS_MIDI_FONT fonts[1];
    unsigned int sampling_rate;

    soundfont_sf2 = NULL;
    soundfont_name = NULL;
    resampling_quality = 0;
    sampling_rate = rate;
    if (parameters != NULL)
    {
        soundfont_sf2 = parameters->soundfont_path;
        resampling_quality = parameters->resampling_quality;
        if (sampling_rate == 0)
        {
            sampling_rate = parameters->sampling_rate;
        }
    }

    if ((sampling_rate < 11000) || (sampling_rate > 384000)) return -2;
    if (functions == NULL) return -3;

    if (sampling_rate == 384000)
    {
        // bassmidi crashes at 384000, so I'm using a lower frequency - the difference is negligible
        sampling_rate = 383999;
    }

    if (soundfont_sf2 != NULL)
    {
        if (!file_exists(parameters->soundfont_path))
        {
            soundfont_sf2 = NULL;
        }
    }
    else
    {
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
        WIN32_FIND_DATA finddata;
        HANDLE findhandle;
        findhandle = FindFirstFile("./*.sf2", &finddata);
        if (findhandle != INVALID_HANDLE_VALUE)
        {
            if ((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                soundfont_name = strdup(&(finddata.cFileName[0]));
                soundfont_sf2 = soundfont_name;
            }
            else
            {
                while (FindNextFile(findhandle, &finddata))
                {
                    if ((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                    {
                        soundfont_name = strdup(&(finddata.cFileName[0]));
                        if (soundfont_name != NULL)
                        {
                            soundfont_sf2 = soundfont_name;
                            break;
                        }
                    }
                }
            }

            FindClose(findhandle);
        }
#else
        DIR *curdir;
        curdir = opendir(".");
        if (curdir != NULL)
        {
            struct dirent *curentry;
            while((curentry = readdir(curdir)) != NULL)
            {
                int len;
                len = strlen(&(curentry->d_name[0]));
                if (len < 5) continue;
                if (strcasecmp(&(curentry->d_name[len - 4]), ".sf2")) continue;

                if (file_exists(&(curentry->d_name[0])))
                {
                    soundfont_name = strdup(&(curentry->d_name[0]));
                    if (soundfont_name != NULL)
                    {
                        soundfont_sf2 = soundfont_name;
                        break;
                    }
                }
            }
            closedir(curdir);
        }
#endif
    }

    if (soundfont_sf2 == NULL) return -4;

    functions->set_master_volume = &set_master_volume;
    functions->open_file = &open_file;
    functions->open_buffer = &open_buffer;
    functions->get_data = &get_data;
    functions->rewind_midi = &rewind_midi;
    functions->close_midi = &close_midi;
    functions->shutdown_plugin = &shutdown_plugin;

    if (!BASS_Init(0, sampling_rate, 0, 0, NULL))
    {
        if (soundfont_name != NULL) free(soundfont_name);
        return -1;
    }

    BASS_SetVolume(1.0f);

    soundfont_handle = BASS_MIDI_FontInit(soundfont_sf2, 0);
    if (soundfont_handle == 0)
    {
        if (soundfont_name != NULL) free(soundfont_name);
        BASS_Free();
        return -1;
    }

    if (soundfont_name != NULL) free(soundfont_name);

    fonts[0].font = soundfont_handle;
    fonts[0].preset = -1;   // all presets
    fonts[0].bank = 0;      // default banks
    if(!BASS_MIDI_StreamSetFonts(0, fonts, 1))
    {
        BASS_MIDI_FontFree(soundfont_handle);
        soundfont_handle = 0;
        BASS_Free();
        return -1;
    }

    return 0;
}

