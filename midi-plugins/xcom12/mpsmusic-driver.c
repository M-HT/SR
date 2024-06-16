/**
 *
 *  Copyright (C) 2016-2024 Roman Pauer
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
    #include <unistd.h>
#endif

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "midi-plugins.h"
#include "emu_x86.h"


static char const *check_file(char const *filename)
{
    if (filename == NULL) return NULL;
    if (*filename == 0) return NULL;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    DWORD dwAttrib;

    dwAttrib = GetFileAttributesA(filename);
    if ((dwAttrib == INVALID_FILE_ATTRIBUTES) || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
#else
    if (access(filename, F_OK))
#endif
    {
        return NULL;
    }
    else
    {
        return filename;
    }
}




static int set_master_volume(unsigned char master_volume) // master_volume = 0 - 127
{
    if (!emu_x86_setvolume((master_volume * 128) / 127))
    {
        return -1;
    }

    return 0;
}

static void *open_file(char const *midifile)
{
    if (midifile == NULL) return NULL;

    FILE *f;
    size_t filelen;
    void *midibuffer;

    f = fopen(midifile, "rb");
    if (f == NULL) return NULL;

    if (fseek(f, 0, SEEK_END)) goto open_file_error;
    filelen = ftell(f);
    if ((filelen == -1) || (filelen == 0)) goto open_file_error;
    if (fseek(f, 0, SEEK_SET)) goto open_file_error;

    midibuffer = malloc(filelen);
    if (midibuffer == NULL) goto open_file_error;

    if (fread(midibuffer, 1, filelen, f) != filelen) goto open_file_error2;
    fclose(f);

    if (!emu_x86_playsequence(midibuffer, filelen))
    {
        free(midibuffer);
        return NULL;
    }

    free(midibuffer);
    return (void *) 1;

open_file_error2:
    free(midibuffer);
open_file_error:
    fclose(f);
    return NULL;
}

static void *open_buffer(void const *midibuffer, long int size)
{
    if (midibuffer == NULL) return NULL;
    if (size <= 0) return NULL;

    if (!emu_x86_playsequence(midibuffer, size))
    {
        return NULL;
    }

    return (void *) 1;
}

static long int get_data(void *handle, void *buffer, long int size)
{
    if (handle == NULL) return -2;
    if (buffer == NULL) return -3;
    if (size < 0) return -4;
    if (size < 4) return 0;

    return emu_x86_getdata(buffer, size);
}

static int rewind_midi(void *handle)
{
    if (handle == NULL) return -2;

    if (!emu_x86_rewindsequence())
    {
        return -1;
    }

    return 0;
}

static int close_midi(void *handle)
{
    if (handle == NULL) return -2;

    if (!emu_x86_stopsequence())
    {
        return -1;
    }

    return 0;
}

static void shutdown_plugin(void)
{
    emu_x86_shutdown();
}


__attribute__ ((visibility ("default")))
int initialize_midi_plugin(unsigned short int rate, midi_plugin_parameters const *parameters, midi_plugin_functions *functions)
{
    char const *drivers_cat;
    char const *mt32_roms;
    int opl3_emulator;
    unsigned int sampling_rate;

    drivers_cat = NULL;
    mt32_roms = NULL;
    opl3_emulator = 0;
    sampling_rate = rate;
    if (parameters != NULL)
    {
        drivers_cat = check_file(parameters->drivers_cat_path);
        mt32_roms = parameters->mt32_roms_path;
        opl3_emulator = parameters->opl3_emulator;
        if (sampling_rate == 0)
        {
            sampling_rate = parameters->sampling_rate;
        }
    }

    if ((sampling_rate < 11000) || (sampling_rate > 0x1fffff)) return -2;
    if (functions == NULL) return -3;
    if (drivers_cat == NULL) return -4;

    functions->set_master_volume = &set_master_volume;
    functions->open_file = &open_file;
    functions->open_buffer = &open_buffer;
    functions->get_data = &get_data;
    functions->rewind_midi = &rewind_midi;
    functions->close_midi = &close_midi;
    functions->shutdown_plugin = &shutdown_plugin;

    if (!emu_x86_initialize(sampling_rate, drivers_cat, mt32_roms, opl3_emulator))
    {
        return -1;
    }

    return 0;
}

