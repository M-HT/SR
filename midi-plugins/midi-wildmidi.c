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

#include <stdlib.h>
#include "midi-plugins.h"
#include "wildmidi_lib.h"


typedef struct {
    void *handle;
    uint64_t pos;
    uint32_t step;
    int num_samples;
    int16_t samples[2*1000];
} resampling_handle;


static unsigned int sampling_rate = 0;


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
    if (master_volume > 127) master_volume = 127;

    return WildMidi_MasterVolume(master_volume);
}

static void *open_file(char const *midifile)
{
    resampling_handle *handle;

    if (midifile == NULL) return NULL;

    if (sampling_rate > 65535)
    {
        handle = (resampling_handle *) malloc(sizeof(resampling_handle));
        if (handle == NULL) return NULL;

        handle->handle = WildMidi_Open(midifile);
        if (handle->handle == NULL)
        {
            free(handle);
            return NULL;
        }

        handle->pos = 0;
        handle->step = (uint32_t) ((((uint64_t)65535) << 32) / sampling_rate);
        handle->num_samples = 0;

        return handle;
    }
    else
    {
        return WildMidi_Open(midifile);
    }
}

static void *open_buffer(void const *midibuffer, long int size)
{
    resampling_handle *handle;

    if (midibuffer == NULL) return NULL;
    if (size <= 0) return NULL;

    if (sampling_rate > 65535)
    {
        handle = (resampling_handle *) malloc(sizeof(resampling_handle));
        if (handle == NULL) return NULL;

        handle->handle = WildMidi_OpenBuffer((unsigned char *)midibuffer, size);
        if (handle->handle == NULL)
        {
            free(handle);
            return NULL;
        }

        handle->pos = 0;
        handle->step = (uint32_t) ((((uint64_t)65535) << 32) / sampling_rate);
        handle->num_samples = 0;

        return handle;
    }
    else
    {
        return WildMidi_OpenBuffer((unsigned char *)midibuffer, size);
    }
}

static long int get_data(void *handle, void *buffer, long int size)
{
    if (handle == NULL) return -2;
    if (buffer == NULL) return -3;
    if (size < 0) return -4;
    if (size < 4) return 0;

    if (sampling_rate > 65535)
    {
        resampling_handle *rhandle;
        long int num_to_write, num_to_read, num_read;
        unsigned int index;

        rhandle = (resampling_handle *)handle;

        num_to_write = size >> 2;
        while (num_to_write > 0)
        {
            num_to_read = ((((num_to_write - 1) * (uint64_t) rhandle->step) + rhandle->pos) >> 32) + 2;
            if (num_to_read > 1000) num_to_read = 1000;
            num_read = WildMidi_GetOutput(rhandle->handle, (int8_t *) &(rhandle->samples[2 * rhandle->num_samples]), (num_to_read - rhandle->num_samples) << 2) >> 2;
            if ((num_read > 0) && (num_read < num_to_read - rhandle->num_samples))
            {
                rhandle->samples[2 * (rhandle->num_samples + num_read)] = rhandle->samples[2 * (rhandle->num_samples + num_read - 1)];
                rhandle->samples[2 * (rhandle->num_samples + num_read) + 1] = rhandle->samples[2 * (rhandle->num_samples + num_read - 1) + 1];
                num_read++;
            }
            rhandle->num_samples += num_read;

            if (rhandle->num_samples < 2) break;

#define POS ((uint32_t)(rhandle->pos >> 32))
#define FRAC ((int32_t)(((uint32_t)rhandle->pos) >> 16))

            while ((num_to_write > 0) && (POS + 1 < rhandle->num_samples))
            {
                ((int16_t *)buffer)[0] = (rhandle->samples[2 * POS] * (0x10000 - FRAC) + rhandle->samples[2 * (POS + 1)] * (FRAC)) >> 16;
                ((int16_t *)buffer)[1] = (rhandle->samples[2 * POS + 1] * (0x10000 - FRAC) + rhandle->samples[2 * (POS + 1) + 1] * (FRAC)) >> 16;
                buffer = 4 + (uint8_t *)buffer;

                rhandle->pos += rhandle->step;
                num_to_write--;
            }

            for (index = 0; POS + index < rhandle->num_samples; index++)
            {
                rhandle->samples[2 * index] = rhandle->samples[2 * (POS + index)];
                rhandle->samples[2 * index + 1] = rhandle->samples[2 * (POS + index) + 1];
            }

            rhandle->num_samples -= POS;
            rhandle->pos = (uint32_t)rhandle->pos;

#undef FRAC
#undef POS
        }

        return ((size >> 2) - num_to_write) << 2;
    }
    else
    {
        return WildMidi_GetOutput(handle, buffer, size);
    }
}

static int rewind_midi(void *handle)
{
    unsigned long int sample_pos;

    if (handle == NULL) return -2;

    sample_pos = 0;

    if (sampling_rate > 65535)
    {
        ((resampling_handle *)handle)->pos = 0;
        ((resampling_handle *)handle)->num_samples = 0;

        return WildMidi_FastSeek(((resampling_handle *)handle)->handle, &sample_pos);
    }
    else
    {
        return WildMidi_FastSeek(handle, &sample_pos);
    }
}

static int close_midi(void *handle)
{
    if (handle == NULL) return -2;

    if (sampling_rate > 65535)
    {
        void *orig_handle;

        orig_handle = ((resampling_handle *)handle)->handle;
        free(handle);

        return WildMidi_Close(orig_handle);
    }
    else
    {
        return WildMidi_Close(handle);
    }
}

static void shutdown_plugin(void)
{
    WildMidi_Shutdown();
}


__attribute__ ((visibility ("default")))
int initialize_midi_plugin(unsigned short int rate, midi_plugin_parameters const *parameters, midi_plugin_functions *functions)
{
    char const *timidity_cfg;
    int resampling_quality;

    timidity_cfg = NULL;
    resampling_quality = 0;
    sampling_rate = rate;
    if (parameters != NULL)
    {
        timidity_cfg = parameters->timidity_cfg_path;
        resampling_quality = parameters->resampling_quality;
        if (sampling_rate == 0)
        {
            sampling_rate = parameters->sampling_rate;
        }
    }

    if ((sampling_rate < 11000) || (sampling_rate > 0x7fffffff)) return -2;
    if (functions == NULL) return -3;

    if (sampling_rate < 11025) sampling_rate = 11025;

    if (timidity_cfg != NULL)
    {
        timidity_cfg = check_file(timidity_cfg);
    }
    else
    {
        timidity_cfg = check_file(getenv("TIMIDITY_CFG"));

        if (timidity_cfg == NULL) timidity_cfg = check_file("timidity.cfg");

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
        if (timidity_cfg == NULL) timidity_cfg = check_file("C:\\TIMIDITY\\timidity.cfg");
#else
        if (timidity_cfg == NULL) timidity_cfg = check_file("/etc/timidity/timidity.cfg");
        if (timidity_cfg == NULL) timidity_cfg = check_file("/etc/timidity.cfg");
        if (timidity_cfg == NULL) timidity_cfg = check_file("/usr/share/timidity/timidity.cfg");
        if (timidity_cfg == NULL) timidity_cfg = check_file("/usr/local/lib/timidity/timidity.cfg");
#endif
    }

    if (timidity_cfg == NULL) return -4;

    functions->set_master_volume = &set_master_volume;
    functions->open_file = &open_file;
    functions->open_buffer = &open_buffer;
    functions->get_data = &get_data;
    functions->rewind_midi = &rewind_midi;
    functions->close_midi = &close_midi;
    functions->shutdown_plugin = &shutdown_plugin;

    return WildMidi_Init(timidity_cfg, (sampling_rate > 65535) ? 65535 : sampling_rate, (resampling_quality > 0) ? WM_MO_ENHANCED_RESAMPLING : 0);
}

