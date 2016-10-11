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
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <math.h>
#include "midi-plugins.h"
#include "wildmidi_lib.h"

#define DIG_F_16BITS_MASK        1
#define DIG_F_STEREO_MASK        2

#define DIG_PCM_SIGN             0x0001   // (obsolete)
#define DIG_PCM_ORDER            0x0002

typedef struct {                         // Wave library entry
    int32_t   bank;                      // XMIDI bank, MIDI patch for sample
    int32_t   patch;

    int32_t   root_key;                  // Root MIDI note # for sample (or -1)

    uint32_t  file_offset;               // Offset of wave data from start-of-file
    uint32_t  size;                      // Size of wave sample in bytes

    int32_t   format;                    // DIG_F format (8/16 bits, mono/stereo)
    uint32_t  flags;                     // DIG_PCM_SIGN / DIG_PCM_ORDER (stereo)
    int32_t   playback_rate;             // Playback rate in hertz
} WAVE_ENTRY;

#pragma pack(1)

typedef struct __attribute__ ((__packed__)) {
    char Magic1[12];
    char Magic2[10];
    char Description[60];
    uint8_t Number_of_instruments;
    uint8_t Voices;
    uint8_t Channels;
    uint16_t Waveforms;
    uint16_t Master_volume;
    uint32_t Data_size;
    uint8_t Reserved[36];
    uint16_t Instrument_ID;
    char Instrument_name[16];
    uint32_t Instrument_size;
    uint8_t Layers;
    uint8_t Reserved2[40];
    uint8_t Layer_duplicate;
    uint8_t Layer;
    uint32_t Layer_size;
    uint8_t Number_of_samples;
    uint8_t Reserved3[40];
} _patch_header;

typedef struct __attribute__ ((__packed__)) {
    char Wave_name[7];
    uint8_t Fractions;
    uint32_t Sample_data_size;
    uint32_t Loop_start;
    uint32_t Loop_end;
    uint16_t Sample_rate;
    uint32_t Low_frequency;
    uint32_t High_frequency;
    uint32_t Root_frequency;
    uint16_t Tune;
    uint8_t Panning;
    uint8_t Envelope_rates_on[3];
    uint8_t Envelope_rates_off[3];
    uint8_t Envelope_offsets_on[3];
    uint8_t Envelope_offsets_off[3];
    uint8_t Tremolo_sweep;
    uint8_t Tremolo_rate;
    uint8_t Tremolo_depth;
    uint8_t Vibrato_sweep;
    uint8_t Vibrato_rate;
    uint8_t Vibrato_depth;
    uint8_t Sampling_modes;
    uint16_t Scale_frequency;
    uint16_t Scale_factor;
    uint8_t Reserved[36];
} _sample_header;

#pragma pack()


static void *A_create_wave_synthetizer(void *wave_lib)
{
    WAVE_ENTRY *wave;
    int num_patches, index, config_size;
    void **patches;
    char *config_file;
    char config_name[60];

    if (wave_lib == NULL) return NULL;

    wave = (WAVE_ENTRY *)wave_lib;
    for (num_patches = 0; num_patches < 512; num_patches++)
    {
        if (wave->bank == -1) break;
        wave++;
    }

    patches = (void **)malloc((num_patches + 1) * sizeof(void **));
    if (patches == NULL) return NULL;
    memset(patches, 0, (num_patches + 1) * sizeof(void *));

    wave = (WAVE_ENTRY *)wave_lib;
    config_size = 0;

    for (index = 0; index < num_patches; index++)
    {
        _patch_header *patch_header;
        _sample_header *sample_header;
        void *sample_ptr;
        int sample_size;

        sample_size = wave[index].size;
        if (wave[index].format & DIG_F_STEREO_MASK)
        {
            sample_size >>= 1;
        }
        patches[index] = malloc(sizeof(_patch_header) + sizeof(_sample_header) + sample_size);
        if (patches[index] == NULL) goto create_wave_synthetizer_error1;

        patch_header = (_patch_header *)patches[index];
        sample_header = (_sample_header *) (sizeof(_patch_header) + (uintptr_t)patch_header);
        sample_ptr = (void *) (sizeof(_sample_header) + (uintptr_t)sample_header);

        memset(patch_header, 0, sizeof(_patch_header) + sizeof(_sample_header));

        strcpy(patch_header->Magic1, "GF1PATCH110");
        strcpy(patch_header->Magic2, "ID#000002");

        sprintf(&(patch_header->Description[2]), "%i |%i|%i|patch-%i.pat\n", wave[index].patch, (int)patch_header, sizeof(_patch_header) + sizeof(_sample_header) + sample_size, wave[index].patch);
        patch_header->Description[1] = strlen(&(patch_header->Description[2]));
        config_size += patch_header->Description[1];

        patch_header->Number_of_instruments = 1;
        patch_header->Voices = 14;
        patch_header->Waveforms = 1;
        patch_header->Master_volume = 127;
        patch_header->Data_size = sample_size;
        patch_header->Instrument_size = sample_size + sizeof(_sample_header);
        patch_header->Layers = 1;
        patch_header->Layer_size = sample_size + sizeof(_sample_header);
        patch_header->Number_of_samples = 1;

        sample_header->Sample_data_size = sample_size;
        sample_header->Loop_start = 0;
        sample_header->Loop_end = sample_size;
        sample_header->Sample_rate = wave[index].playback_rate;
        sample_header->Low_frequency = 6875 * pow(2, ((-1) + 3) / 12.0);
        sample_header->High_frequency = 6875 * pow(2, ((128) + 3) / 12.0);
        sample_header->Root_frequency = 6875 * pow(2, (wave[index].root_key + 3) / 12.0);
        //sample_header->Root_frequency = 1000 * 440 * pow(2, (wave[index].root_key-69)/12.0);
        sample_header->Tune = 1;
        sample_header->Panning = 7;
        sample_header->Sampling_modes = ( (wave[index].format & DIG_F_16BITS_MASK)?1:0 ) | ( (wave[index].flags & DIG_PCM_SIGN)?0:2 );
        sample_header->Scale_frequency = 64; // ???
        sample_header->Scale_factor = 1024;

        if (wave[index].format & DIG_F_STEREO_MASK)
        {
            // convert to mono
            int counter;

            if (wave[index].format & DIG_F_16BITS_MASK)
            {
                if (wave[index].flags & DIG_PCM_SIGN)
                {
                    // 16 bit signed
                    int16_t *src, *dst;

                    src = (int16_t *)(wave[index].file_offset + (uintptr_t)wave_lib);
                    dst = (int16_t *)sample_ptr;

                    for (counter = sample_size >> 1; counter != 0; counter--)
                    {
                        *dst = (src[0] + src[1]) >> 1;
                        src += 2;
                        dst++;
                    }
                }
                else
                {
                    // 16 bit unsigned
                    uint16_t *src, *dst;

                    src = (uint16_t *)(wave[index].file_offset + (uintptr_t)wave_lib);
                    dst = (uint16_t *)sample_ptr;

                    for (counter = sample_size >> 1; counter != 0; counter--)
                    {
                        *dst = (src[0] + src[1]) >> 1;
                        src += 2;
                        dst++;
                    }
                }
            }
            else
            {
                if (wave[index].flags & DIG_PCM_SIGN)
                {
                    // 8 bit signed
                    int8_t *src, *dst;

                    src = (int8_t *)(wave[index].file_offset + (uintptr_t)wave_lib);
                    dst = (int8_t *)sample_ptr;

                    for (counter = sample_size; counter != 0; counter--)
                    {
                        *dst = (src[0] + src[1]) >> 1;
                        src += 2;
                        dst++;
                    }
                }
                else
                {
                    // 8 bit unsigned
                    uint8_t *src, *dst;

                    src = (uint8_t *)(wave[index].file_offset + (uintptr_t)wave_lib);
                    dst = (uint8_t *)sample_ptr;

                    for (counter = sample_size; counter != 0; counter--)
                    {
                        *dst = (src[0] + src[1]) >> 1;
                        src += 2;
                        dst++;
                    }
                }
            }
        }
        else
        {
            memcpy(sample_ptr, (void *)(wave[index].file_offset + (uintptr_t)wave_lib), sample_size);
        }
    }

    config_file = (char *)malloc(config_size);
    if (config_file == NULL) goto create_wave_synthetizer_error1;

    sprintf(config_name, "|%i|%i|timidity.cfg", (int)config_file, config_size);

    config_size = 0;
    for (index = 0; index < num_patches; index++)
    {
        _patch_header *patch_header;

        patch_header = (_patch_header *)patches[index];
        memcpy(&(config_file[config_size]), &(patch_header->Description[2]), patch_header->Description[1]);

        config_size += patch_header->Description[1];
    }

    if (WildMidi_LoadConfig(config_name)) goto create_wave_synthetizer_error2;

    free(config_file);

    return (void *)patches;

create_wave_synthetizer_error2:
    free(config_file);
create_wave_synthetizer_error1:
    for (index = 0; index < num_patches; index++)
    {
        if (patches[index] != NULL) free(patches[index]);
    }
    free(patches);

    return NULL;
}

static int A_destroy_wave_synthetizer(void *p)
{
    void **patches;

    if (p == NULL) return -1;

    if (WildMidi_UnloadConfig()) return -2;

    for (patches = (void **)p; *patches != NULL; patches++)
    {
        free(*patches);
    }

    free(p);
    return 0;
}

static int set_master_volume(unsigned char master_volume) // master_volume = 0 - 127
{
    if (master_volume > 127) master_volume = 127;

    return WildMidi_MasterVolume(master_volume);
}

static void *open_file(char const *midifile)
{
    if (midifile == NULL) return NULL;

    return WildMidi_Open(midifile);
}

static void *open_buffer(void const *midibuffer, long int size)
{
    if (midibuffer == NULL) return NULL;
    if (size <= 0) return NULL;

    return WildMidi_OpenBuffer((unsigned char *)midibuffer, size);
}

static long int get_data(void *handle, void *buffer, long int size)
{
    if (handle == NULL) return -2;
    if (buffer == NULL) return -3;
    if (size < 0) return -4;
    if (size == 0) return 0;

    return WildMidi_GetOutput(handle, buffer, size);
}

static int rewind_midi(void *handle)
{
    unsigned long int sample_pos;

    if (handle == NULL) return -2;

    sample_pos = 0;
    return WildMidi_FastSeek(handle, &sample_pos);
}

static int close_midi(void *handle)
{
    if (handle == NULL) return -2;

    return WildMidi_Close(handle);
}

static void shutdown_plugin(void)
{
    WildMidi_Shutdown();
}


int initialize_midi_plugin(unsigned short int rate, midi_plugin_parameters const *parameters, midi_plugin_functions *functions)
{
    if ((rate < 11000) || (rate > 65000)) return -2;
    if (functions == NULL) return -3;

    functions->set_master_volume = &set_master_volume;
    functions->open_file = &open_file;
    functions->open_buffer = &open_buffer;
    functions->get_data = &get_data;
    functions->rewind_midi = &rewind_midi;
    functions->close_midi = &close_midi;
    functions->shutdown_plugin = &shutdown_plugin;
    functions->A_create_wave_synthetizer = &A_create_wave_synthetizer;
    functions->A_destroy_wave_synthetizer = &A_destroy_wave_synthetizer;

    return WildMidi_Init(NULL, rate, WM_MO_NOCONFIG);
}

