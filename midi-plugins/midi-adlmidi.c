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

#include <stdlib.h>
#include <string.h>
#include "midi-plugins.h"
#include "adlmidi.h"
#ifdef USE_SPEEXDSP_RESAMPLER
#include <speex/speex_resampler.h>
#include <stdint.h>
#endif


#define ADLMIDI_VERSION_ATLEAST(major,minor,patchlevel) defined(ADLMIDI_VERSION_MAJOR) && (ADLMIDI_VERSION_MAJOR > (major) || (ADLMIDI_VERSION_MAJOR == (major) && (ADLMIDI_VERSION_MINOR > (minor) || (ADLMIDI_VERSION_MINOR == (minor) && ADLMIDI_VERSION_PATCHLEVEL >= (patchlevel)))))


static struct ADL_MIDIPlayer *adl_handle = NULL;
static unsigned char adl_volume = 127;
#ifdef USE_SPEEXDSP_RESAMPLER
static SpeexResamplerState *resampler = NULL;
static uint64_t resample_step;
static int resample_num_samples;
static int16_t resample_samples[2*1024];
#endif



static void send_master_volume_sysex(void)
{
    ADL_UInt8 sysex[8];

    sysex[0] = 0xf0;
    sysex[1] = 0x7f;
    sysex[2] = 0x7f;
    sysex[3] = 0x04;
    sysex[4] = 0x01;
    sysex[5] = 0x00;
    sysex[6] = adl_volume;
    sysex[7] = 0xf7;

    adl_rt_systemExclusive(adl_handle, sysex, 8);
}

static int set_master_volume(unsigned char master_volume) // master_volume = 0 - 127
{
    if (master_volume > 127) master_volume = 127;

    adl_volume = master_volume;

    send_master_volume_sysex();

    return 0;
}

static void *open_file(char const *midifile)
{
    if (midifile == NULL) return NULL;
    if (adl_handle == NULL) return NULL;

    if (adl_openFile(adl_handle, midifile)) return NULL;

    send_master_volume_sysex();

#ifdef USE_SPEEXDSP_RESAMPLER
    resample_num_samples = 0;
#endif

    return (void *) 1;
}

static void *open_buffer(void const *midibuffer, long int size)
{
    if (midibuffer == NULL) return NULL;
    if (size <= 0) return NULL;
    if (adl_handle == NULL) return NULL;

    if (adl_openData(adl_handle, midibuffer, size)) return NULL;

    send_master_volume_sysex();

#ifdef USE_SPEEXDSP_RESAMPLER
    resample_num_samples = 0;
#endif

    return (void *) 1;
}

static long int get_data(void *handle, void *buffer, long int size)
{
    if (handle == NULL) return -2;
    if (buffer == NULL) return -3;
    if (size < 0) return -4;
    if (size < 4) return 0;
    if (adl_handle == NULL) return -5;

#ifdef USE_SPEEXDSP_RESAMPLER
    if (resampler != NULL)
    {
        long int num_to_write, num_to_read, num_read;
        unsigned int index;

        num_to_write = size >> 2;

        while (num_to_write > 0)
        {
            spx_uint32_t in_len, out_len;
            int err;

            num_to_read = (((num_to_write + 1) * resample_step) >> 32) + 8;
            if (num_to_read & 7) num_to_read += 8 - (num_to_read & 7);
            if (num_to_read > 1024) num_to_read = 1024;
            num_read = adl_play(adl_handle, (num_to_read - resample_num_samples) << 1, &(resample_samples[2 * resample_num_samples])) >> 1;
            resample_num_samples += num_read;

            if (resample_num_samples == 0) break;

            in_len = resample_num_samples;
            out_len = num_to_write;

            err = speex_resampler_process_interleaved_int(resampler, resample_samples, &in_len, (spx_int16_t *)buffer, &out_len);
            if ((err != RESAMPLER_ERR_SUCCESS) || (out_len == 0)) break;

            buffer = (void *)((out_len << 2) + (uintptr_t)buffer);
            num_to_write -= out_len;

            for (index = 0; in_len + index < resample_num_samples; index++)
            {
                resample_samples[2 * index] = resample_samples[2 * (in_len + index)];
                resample_samples[2 * index + 1] = resample_samples[2 * (in_len + index) + 1];
            }

            resample_num_samples -= in_len;
        }

        return ((size >> 2) - num_to_write) << 2;
    }
#endif

    return adl_play(adl_handle, size >> 1, (short *) buffer) << 1;
}

static int rewind_midi(void *handle)
{
    if (handle == NULL) return -2;
    if (adl_handle == NULL) return -3;

    adl_positionRewind(adl_handle);

#ifdef USE_SPEEXDSP_RESAMPLER
    resample_num_samples = 0;
#endif

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
#ifdef USE_SPEEXDSP_RESAMPLER
    if (resampler != NULL)
    {
        speex_resampler_destroy(resampler);
        resampler = NULL;
    }
#endif
}

__attribute__ ((visibility ("default")))
int initialize_midi_plugin(unsigned short int rate, midi_plugin_parameters const *parameters, midi_plugin_functions *functions)
{
    int bank_number, emulator;
    int resampling_quality;
    unsigned int sampling_rate;

    bank_number = 0;
    emulator = 0;
    resampling_quality = 0;
    sampling_rate = rate;
    if (parameters != NULL)
    {
        bank_number = parameters->opl3_bank_number;
        emulator = parameters->opl3_emulator;
        resampling_quality = parameters->resampling_quality;
        if (sampling_rate == 0)
        {
            sampling_rate = parameters->sampling_rate;
        }
    }

    if ((sampling_rate < 11000) || (sampling_rate > 0x1fffff)) return -2;
    if (functions == NULL) return -3;

    functions->set_master_volume = &set_master_volume;
    functions->open_file = &open_file;
    functions->open_buffer = &open_buffer;
    functions->get_data = &get_data;
    functions->rewind_midi = &rewind_midi;
    functions->close_midi = &close_midi;
    functions->shutdown_plugin = &shutdown_plugin;

    if (resampling_quality > 0)
    {
        if (strstr(adl_linkedLibraryVersion(), "(HQ)") != NULL)
        {
            // libADLMIDI is compiled with high quality resampler
            resampling_quality = 0;
        }
    }

#ifdef USE_SPEEXDSP_RESAMPLER
    if ((resampling_quality > 0) && (sampling_rate != 49716))
    {
        int err;

        resampler = speex_resampler_init(2, 49716, sampling_rate, SPEEX_RESAMPLER_QUALITY_DESKTOP, &err);
        if ((resampler == NULL) || (err != RESAMPLER_ERR_SUCCESS))
        {
            resampler = NULL;
        }
        else
        {
            resample_step = ((((uint64_t)49716) << 32) / sampling_rate);
        }
    }
    else resampler = NULL;

    if (resampler != NULL) adl_handle = adl_init(49716);
    else
#endif
    adl_handle = adl_init(sampling_rate);
    if (adl_handle == NULL)
    {
        shutdown_plugin();
        return -1;
    }


    // set number of emulated chips
    if (adl_setNumChips(adl_handle, 1))
    {
        shutdown_plugin();
        return -1;
    }

    adl_setVolumeRangeModel(adl_handle, ADLMIDI_VolumeModel_Generic);

#if ADLMIDI_VERSION_ATLEAST(1,3,2)
    if (emulator)
    {
        adl_switchEmulator(adl_handle, ADLMIDI_EMU_NUKED);
    }
    else
    {
        adl_switchEmulator(adl_handle, ADLMIDI_EMU_DOSBOX);
    }
#endif

    if (adl_setBank(adl_handle, bank_number))
    {
        shutdown_plugin();
        return -1;
    }

    // reset after bank load
    adl_reset(adl_handle);

    // disable looping
    adl_setLoopEnabled(adl_handle, 0);

    return 0;
}

