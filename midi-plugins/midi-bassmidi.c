/**
 *
 *  Copyright (C) 2016 Roman Pauer
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

#if (defined(_WIN32) || defined(__WIN32__) || (__WINDOWS__))
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <dirent.h>
#endif

#include <string.h>
#include <malloc.h>
#include "midi-plugins.h"
#include "bass.h"
#include "bassmidi.h"

static HSOUNDFONT soundfont_handle = 0;


static int file_exists(char const *filename)
{
    if (filename == NULL) return 0;
    if (*filename == 0) return 0;

#if (defined(_WIN32) || defined(__WIN32__) || (__WINDOWS__))
    DWORD dwAttrib;

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
    if (midifile == NULL) return NULL;

    return (void *) BASS_MIDI_StreamCreateFile(0, midifile, 0, 0, BASS_STREAM_DECODE, 0);
}

static void *open_buffer(void const *midibuffer, long int size)
{
    if (midibuffer == NULL) return NULL;
    if (size <= 0) return NULL;

    return (void *) BASS_MIDI_StreamCreateFile(1, midibuffer, 0, size, BASS_STREAM_DECODE, 0);
}

static long int get_data(void *handle, void *buffer, long int size)
{
    if (handle == NULL) return -2;
    if (buffer == NULL) return -3;
    if (size < 0) return -4;
    if (size == 0) return 0;

    return BASS_ChannelGetData((HSTREAM)handle, buffer, size);
}

static int rewind_midi(void *handle)
{
    QWORD curpos;

    if (handle == NULL) return -2;

    curpos = BASS_ChannelGetPosition((HSTREAM)handle, BASS_POS_BYTE);
    if (curpos == 0) return 0; // this helps to work around some bug

    if (!BASS_ChannelSetPosition((HSTREAM)handle, 0, BASS_POS_BYTE | BASS_MUSIC_POSRESETEX))
    {
        return -1;
    }

    return 0;
}

static int close_midi(void *handle)
{
    if (handle == NULL) return -2;

    if (!BASS_StreamFree((HSTREAM)handle))
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


int initialize_midi_plugin(unsigned short int rate, midi_plugin_parameters const *parameters, midi_plugin_functions *functions)
{
    char const *soundfont_sf2;
    char *soundfont_name;
    BASS_MIDI_FONT fonts[1];

    if ((rate < 11000) || (rate > 48000)) return -2;
    if (functions == NULL) return -3;

    soundfont_sf2 = NULL;
    soundfont_name = NULL;
    if (parameters != NULL)
    {
        soundfont_sf2 = parameters->soundfont_path;
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
#if (defined(_WIN32) || defined(__WIN32__) || (__WINDOWS__))
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

    if (!BASS_Init(0, rate, 0, 0, NULL))
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

