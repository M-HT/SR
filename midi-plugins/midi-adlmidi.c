/**
 *
 *  Copyright (C) 2016-2018 Roman Pauer
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

#include <stdlib.h>
#include "midi-plugins.h"
#include "adlmidi.h"

static struct ADL_MIDIPlayer *adl_handle = NULL;
static unsigned char adl_volume = 127;



static int set_master_volume(unsigned char master_volume) // master_volume = 0 - 127
{
    if (master_volume > 127) master_volume = 127;

    adl_volume = master_volume;

    return 0;
}

static void *open_file(char const *midifile)
{
    if (midifile == NULL) return NULL;
    if (adl_handle == NULL) return NULL;

    if (adl_openFile(adl_handle, midifile)) return NULL;

    return (void *) 1;
}

static void *open_buffer(void const *midibuffer, long int size)
{
    if (midibuffer == NULL) return NULL;
    if (size <= 0) return NULL;
    if (adl_handle == NULL) return NULL;

    if (adl_openData(adl_handle, midibuffer, size)) return NULL;

    return (void *) 1;
}

static long int get_data(void *handle, void *buffer, long int size)
{
    int num_samples;

    if (handle == NULL) return -2;
    if (buffer == NULL) return -3;
    if (size < 0) return -4;
    if (size == 0) return 0;
    if (adl_handle == NULL) return -5;

    num_samples = adl_play(adl_handle, size / 2, (short *) buffer);

    if ((num_samples != 0) && (adl_volume != 127))
    {
        int index, vol;
        short *buf;

        buf = (short *) buffer;
        vol = ((adl_volume * 65536) + 63) / 127;

        for (index = 0; index < num_samples; index++)
        {
            //buf[index] = (buf[index] * adl_volume) / 127;
            buf[index] = (buf[index] * vol) >> 16;
        }
    }

    return 2 * num_samples;
}

static int rewind_midi(void *handle)
{
    if (handle == NULL) return -2;
    if (adl_handle == NULL) return -3;

    adl_positionRewind(adl_handle);

    return 0;
}

static int close_midi(void *handle)
{
    if (handle == NULL) return -2;
    if (adl_handle == NULL) return -3;

    adl_reset(adl_handle);

    return 0;
}

static void shutdown_plugin(void)
{
    if (adl_handle != NULL)
    {
        adl_close(adl_handle);
        adl_handle = NULL;
    }
}

int initialize_midi_plugin(unsigned short int rate, midi_plugin_parameters const *parameters, midi_plugin_functions *functions)
{
    int bank_number;

    if ((rate < 11000) || (rate > 65000)) return -2;
    if (functions == NULL) return -3;

    functions->set_master_volume = &set_master_volume;
    functions->open_file = &open_file;
    functions->open_buffer = &open_buffer;
    functions->get_data = &get_data;
    functions->rewind_midi = &rewind_midi;
    functions->close_midi = &close_midi;
    functions->shutdown_plugin = &shutdown_plugin;

    adl_handle = adl_init(rate);
    if (adl_handle == NULL) return -1;


    // set number of emulated chips
    if (adl_setNumChips(adl_handle, 1))
    {
        adl_close(adl_handle);
        adl_handle = NULL;
        return -1;
    }

    bank_number = 0;
    if (parameters != NULL)
    {
        bank_number = parameters->opl3_bank_number;
    }

    if (adl_setBank(adl_handle, bank_number))
    {
        adl_close(adl_handle);
        adl_handle = NULL;
        return -1;
    }

    // reset after bank load
    adl_reset(adl_handle);

    // disable looping
    adl_setLoopEnabled(adl_handle, 0);

    return 0;
}

