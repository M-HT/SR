/**
 *
 *  Copyright (C) 2016-2021 Roman Pauer
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
#include "Geoscape-music-midiplugin2.h"
#include "gmcat2mid.h"
#include "midi-plugins2.h"
#include "Geoscape-proc-vfs.h"


#define MP_STOPPED 0
#define MP_PLAYING 1
#define MP_PAUSED  2
#define MP_STARTED 3


typedef struct _MP_midi_ {
    Uint8 status, reserved[3];
    Sint32 loop_count;
} MP_midi;


static const unsigned char roland_canvas_initial_sysex[] = {
    0xF0, 0x41, 0x10, 0x42, 0x12, // sysex event header
    0x40, 0x01, 0x30, 0x02, 0x04, 0x00, 0x40, 0x40, 0x00, 0x00, // reverb settings
    0x09, // checkum
    0xF7, // end of sysex event
    0xFF  // end of sysex events list
};

static const unsigned char roland_canvas_reset_controller[] = {
    0xB0, // start of controller events list
    0x5B, 0x1E, // reverb send level
    0x5D, 0x00, // chorus send level
    0xFF  // end of controller events list
};

static const unsigned char roland_mt32_reset_controller[] = {
    0xB0, // start of controller events list
    0x07, 0x64, // volume 100
    0x0A, 0x40, // pan 64
    0xFF  // end of controller events list
};

#define SYSEX_HEADER_LENGTH 5
static const unsigned char roland_mt32_sysex_header[SYSEX_HEADER_LENGTH] = {
    0xF0, 0x41, 0x10, 0x16, 0x12
};

static unsigned char *roland_mt32_initial_sysex = NULL;

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

static int prepare_roland_mt32_sysex(void)
{
    FILE *f;
    uint32_t num_files, file_offset, file_len, lapc1_pat_len;
    uint8_t *lapc1_pat, *roland_mt32_sysex, *cur_sysex;
    unsigned int file_counter;
    uint8_t name_len;
    uint8_t name[256];

    // open file "drivers.cat"
    f = Game_fopen("C:\\SOUND\\DRIVERS.CAT", "rb");
    if (f == NULL)
    {
        return 2;
    }

    // find file "lapc1.pat" in "drivers.cat"
    if (1 != fread(&num_files, 4, 1, f))
    {
        fclose(f);
        return 3;
    }
    num_files >>= 3;

    lapc1_pat = NULL;
    for (file_counter = 0; file_counter < num_files; file_counter++)
    {
        if (0 != fseek(f, 8 * file_counter, SEEK_SET)) break;
        if (1 != fread(&file_offset, 4, 1, f)) break;
        if (1 != fread(&file_len, 4, 1, f)) break;

        if (file_len == 0) continue;

        if (0 != fseek(f, file_offset, SEEK_SET)) break;
        if (1 != fread(&name_len, 1, 1, f)) break;
        if (name_len == 0) continue;
        if (name_len != fread(name, 1, name_len, f)) break;
        name[name_len] = 0;

        if (strcasecmp((char *)name, "lapc1.pat") == 0)
        {
            lapc1_pat_len = file_len;
            lapc1_pat = malloc(file_len);
            if (lapc1_pat == NULL) break;
            if (file_len != fread(lapc1_pat, 1, file_len, f))
            {
                free(lapc1_pat);
                lapc1_pat = NULL;
            }

            break;
        }
    }

    fclose(f);

    if (lapc1_pat == NULL)
    {
        return 4;
    }

    // count number and lenth of sysex events
    file_counter = 0;
    file_offset = 0;
    do
    {
        while (file_offset < lapc1_pat_len && lapc1_pat[file_offset] != 0xff) file_offset++;
        file_counter++;
        file_offset++;

        if (file_offset >= lapc1_pat_len)
        {
            free(lapc1_pat);
            return 5;
        }
    } while (lapc1_pat[file_offset] != 0xff);

    roland_mt32_sysex = (uint8_t *) malloc(1 + file_offset + file_counter * (SYSEX_HEADER_LENGTH + 1));
    if (roland_mt32_sysex == NULL)
    {
        free(lapc1_pat);
        return 6;
    }

    // prepare sysex events
    cur_sysex = roland_mt32_sysex;
    file_offset = 0;
    do
    {
        int checksum;

        memcpy(cur_sysex, roland_mt32_sysex_header, SYSEX_HEADER_LENGTH);
        cur_sysex += SYSEX_HEADER_LENGTH;

        checksum = 0;
        while (lapc1_pat[file_offset] != 0xff)
        {
            checksum += lapc1_pat[file_offset];
            *cur_sysex = lapc1_pat[file_offset];
            cur_sysex++;
            file_offset++;
        }

        cur_sysex[0] = (-checksum) & 0x7f;
        cur_sysex[1] = 0xf7;
        cur_sysex += 2;
        file_offset++;
    } while (lapc1_pat[file_offset] != 0xff);

    *cur_sysex = 0xf7;

    free(lapc1_pat);

    roland_mt32_initial_sysex = roland_mt32_sysex;
    return 0;
}


int MidiPlugin2_Startup(void)
{
    midi_plugin2_initialize MP2_initialize;
    midi_plugin2_parameters MP2_parameters;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    if (Game_MidiSubsystem == 21 || Game_MidiSubsystem == 31) {
        MP2_handle = LoadLibrary(".\\midi2-windows.dll");
    } else if (Game_MidiSubsystem == 22 || Game_MidiSubsystem == 32) {
        MP2_handle = LoadLibrary(".\\midi2-alsa.dll");
    } else return 1;

    #define free_library FreeLibrary
    #define get_proc_address GetProcAddress
#else
    if (Game_MidiSubsystem == 21 || Game_MidiSubsystem == 31) {
        MP2_handle = dlopen("./midi2-windows.so", RTLD_LAZY);
    } else if (Game_MidiSubsystem == 22 || Game_MidiSubsystem == 32) {
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
    if (Game_MidiSubsystem <= 30)
    {
        MP2_parameters.initial_sysex_events = roland_canvas_initial_sysex;
        MP2_parameters.reset_controller_events = roland_canvas_reset_controller;
    }
    else
    {
        if (roland_mt32_initial_sysex == NULL)
        {
            if (0 != prepare_roland_mt32_sysex())
            {
                free_library(MP2_handle);
                return 4;
            }
        }
        MP2_parameters.initial_sysex_events = roland_mt32_initial_sysex;
        MP2_parameters.reset_controller_events = roland_mt32_reset_controller;
        MP2_parameters.midi_type = 1;
    }

    if (MP2_initialize(&MP2_parameters, &MP2_functions))
    {
        free_library(MP2_handle);
        return 5;
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
        else if (Game_MidiSubsystem <= 30)
        {
            Game_MusicSequence.midi = gmcat2mid(seq, &Game_MusicSequence.midi_size, &midi_loop);
        }
        else
        {
            Game_MusicSequence.midi = rolandcat2mid(seq, &Game_MusicSequence.midi_size, &midi_loop);
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

