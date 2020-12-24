/**
 *
 *  Copyright (C) 2016-2020 Roman Pauer
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
    #include <dlfcn.h>
#endif
#include <stdio.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Intro-music-midiplugin2.h"
#include "gmcat2mid.h"
#include "midi-plugins2.h"
#include "virtualfs.h"



#define MP_STOPPED 0
#define MP_PLAYING 1
#define MP_PAUSED  2
#define MP_STARTED 3

typedef struct _MP_midi_ {
    Uint8 status, reserved[3];
    Sint32 loop_count;
} MP_midi;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    static HMODULE MP2_handle;
#else
    static void *MP2_handle;
#endif

static midi_plugin2_functions MP2_functions;

static MP_midi MP_sequence;


static uint8_t *load_file(char const *midifile, unsigned int *res_midi_size)
{
    if (midifile == NULL) return NULL;

    FILE *f;
    size_t filelen;
    uint8_t *midibuffer;

    f = fopen(midifile, "rb");
    if (f == NULL) return NULL;

    if (fseek(f, 0, SEEK_END)) goto load_file_error;
    filelen = ftell(f);
    if ((filelen == -1) || (filelen == 0)) goto load_file_error;
    if (fseek(f, 0, SEEK_SET)) goto load_file_error;

    midibuffer = (uint8_t *) malloc(filelen);
    if (midibuffer == NULL) goto load_file_error;

    if (fread(midibuffer, 1, filelen, f) != filelen) goto load_file_error2;
    fclose(f);

    if (res_midi_size != NULL)
    {
        *res_midi_size = filelen;
    }

    return midibuffer;

load_file_error2:
    free(midibuffer);
load_file_error:
    fclose(f);
    return NULL;
}


int MidiPlugin2_Startup(void)
{
    midi_plugin2_initialize MP2_initialize;
    midi_plugin2_parameters MP2_parameters;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    if (Game_MidiSubsystem == 21) {
        MP2_handle = LoadLibrary(".\\midi2-windows.dll");
    } else if (Game_MidiSubsystem == 22) {
        MP2_handle = LoadLibrary(".\\midi2-alsa.dll");
    } else return 1;

    #define free_library FreeLibrary
    #define get_proc_address GetProcAddress
#else
    if (Game_MidiSubsystem == 21) {
        MP2_handle = dlopen("./midi2-windows.so", RTLD_LAZY);
    } else if (Game_MidiSubsystem == 22) {
        MP2_handle = dlopen("./midi2-alsa.so", RTLD_LAZY);
    } else return 1;

    #define free_library dlclose
    #define get_proc_address dlsym
#endif
    if (MP2_handle == NULL) return 2;

    MP2_initialize = (midi_plugin2_initialize) get_proc_address(MP2_handle, MIDI_PLUGIN2_INITIALIZE);

    if (MP2_initialize == NULL)
    {
        free_library(MP2_handle);
        return 3;
    }

    memset(&MP2_parameters, 0, sizeof(MP2_parameters));
    MP2_parameters.midi_device_name = Game_MidiDevice;

    if (MP2_initialize(&MP2_parameters, &MP2_functions))
    {
        free_library(MP2_handle);
        return 4;
    }

    // initialize buffers
    memset(&(MP_sequence), 0, sizeof(MP_midi));

    MidiPlugin2_SetMusicVolume();

    return 0;

#undef get_proc_address
#undef free_library
}

void MidiPlugin2_Shutdown(void)
{
    MP2_functions.halt();

    MP2_functions.shutdown_plugin();
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    FreeLibrary(MP2_handle);
#else
    dlclose(MP2_handle);
#endif
    MP2_handle = NULL;
}


void MidiPlugin2_SetMusicVolume(void)
{
    int new_volume;

    if (Game_Music)
    {
        new_volume = (Game_AudioMasterVolume * Game_MusicSequence.volume * Game_AudioMusicVolume * 127) >> 21;

        MP2_functions.set_volume(new_volume);
    }
}

void MidiPlugin2_start_sequence(uint8_t *seq)
{
    int midi_loop;

    if (Game_Music)
    {
        MP_sequence.status = MP_STOPPED;
        if (Game_MusicSequence.midi != NULL)
        {
            free(Game_MusicSequence.midi);
            Game_MusicSequence.midi = NULL;
        }

        if (strcmp((const char *)seq, "MIDI") == 0)
        {
            midi_loop = seq[5];
            Game_MusicSequence.midi = load_file((char *) &(seq[6]), &Game_MusicSequence.midi_size);
        }
        else
        {
            Game_MusicSequence.midi = gmcat2mid(seq, &Game_MusicSequence.midi_size, &midi_loop);
        }

        if (Game_MusicSequence.midi == NULL) return;

        MP_sequence.loop_count = (midi_loop)?-1:0;

        MP_sequence.status = MP_STARTED;

        MP2_functions.play(Game_MusicSequence.midi, Game_MusicSequence.midi_size, MP_sequence.loop_count);
    } else {
        MP_sequence.status = MP_STARTED;
    }
}

void MidiPlugin2_stop_sequence(void)
{
    if (Game_Music)
    {
        MP_sequence.status = MP_STOPPED;

        MP2_functions.halt();
    }
    else
    {
        MP_sequence.status = MP_STOPPED;
    }
}

uint32_t MidiPlugin2_sequence_playing(void)
{
    uint32_t ret;

    switch (MP_sequence.status)
    {
        case MP_PLAYING:
        case MP_STARTED:
            ret = 1;
            break;
        default:
            ret = 0;
            break;
    }

    return ret;
}

