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

#include "Game_defs.h"
#include "Game_vars.h"
#include "xmi2mid.h"
#include "Albion-music-xmiplayer.h"
#include <math.h>
#include <string.h>
#ifdef USE_SPEEXDSP_RESAMPLER
#include <speex/speex_resampler.h>
#endif

#if ( \
    defined(__aarch64__) || \
    defined(_M_ARM64) || \
    defined(_M_ARM64EC) \
)
    #define ARMV8 1
#else
    #undef ARMV8
#endif

#if (!defined(ARMV8)) && ( \
    (defined(__ARM_ARCH) && (__ARM_ARCH >= 6)) || \
    (defined(_M_ARM) && (_M_ARM >= 6)) || \
    (defined(__TARGET_ARCH_ARM) && (__TARGET_ARCH_ARM >= 6)) || \
    (defined(__TARGET_ARCH_THUMB) && (__TARGET_ARCH_THUMB >= 3)) \
)
    #define ARMV6 1
#else
    #undef ARMV6
#endif

#if defined(ARMV8)
#include <arm_neon.h>
#elif defined(ARMV6) && defined(__ARM_ACLE) && __ARM_FEATURE_SAT
#include <arm_acle.h>
#endif


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

typedef struct _midi_channel {
    const WAVE_ENTRY *instrument;
    int bank;
    int program;
    int channel_volume; // 0 - 127
    int pan;            // 0 - 127
    int note_volume;    // 0 - 127
    int pitch_bend;     // -8192 - 8191
    int32_t volume_left;
    int32_t volume_right;
    uint32_t time_left;
    const uint8_t *sample_data;
    uint32_t sample_length;
    uint32_t format;
    int32_t key;
    int32_t rate;
    uint64_t pos;
    uint64_t pos_inc;
#ifdef USE_SPEEXDSP_RESAMPLER
    SpeexResamplerState *resampler;
    float *converted_data;
#endif
} midi_channel;

typedef struct _xmi_player {
    int playing;
    unsigned int rate;
    unsigned int samples_per_tick;
    unsigned int samples_left;
    int volume;         // 0-128
    int loop_count;
    int pitch_bend_sensitivity;
    const WAVE_ENTRY *wave_synthetizer;
    const uint8_t *seq;
    uint32_t seq_len;
    uint32_t event_time;
    const uint8_t *current_event;
    int32_t *accum_data;
#ifdef USE_SPEEXDSP_RESAMPLER
    float *resampled_data;
#endif
    midi_channel channels[16];
} xmi_player;


#define READ_VARLEN(res, buf) {                 \
    unsigned int ___data;                       \
    res = 0;                                    \
    do {                                        \
        ___data = *(buf);                       \
        buf++;                                  \
        res = (res << 7) | (___data & 0x7f);    \
    } while (___data & 0x80);                   }


static INLINE int32_t S8toS16(uint8_t a)
{
    return (int16_t)((a << 8) | (a ^ 0x80));
}

static INLINE int32_t U8toS16(uint8_t a)
{
    return ((a << 8) | a) - 32768;
}

static void update_channel_instrument(const WAVE_ENTRY *wave_synthetizer, midi_channel *channel)
{
    const WAVE_ENTRY *new_instrument;
    int entry;

    if (channel == NULL) return;

    new_instrument = NULL;
    if (wave_synthetizer != NULL)
    {
        for (entry = 0; entry < 512; entry++)
        {
            if (wave_synthetizer[entry].bank == -1) break;

            if ((wave_synthetizer[entry].bank == channel->bank) && (wave_synthetizer[entry].patch == channel->program))
            {
                new_instrument = &(wave_synthetizer[entry]);
                break;
            }
        }
    }

    if (new_instrument != channel->instrument)
    {
        channel->instrument = new_instrument;
        channel->note_volume = 0;

#ifdef USE_SPEEXDSP_RESAMPLER
        if (channel->resampler != NULL)
        {
            speex_resampler_destroy(channel->resampler);
            channel->resampler = NULL;
        }
        if (channel->converted_data != NULL)
        {
            free(channel->converted_data);
            channel->converted_data = NULL;
        }
#endif

        if (new_instrument != NULL)
        {
            channel->sample_data = new_instrument->file_offset + (const uint8_t *)wave_synthetizer;
            switch (new_instrument->format & 3)
            {
                case 0: // 8-bit, mono
                    channel->sample_length = new_instrument->size;
                    break;
                case 1: // 16-bit, mono
                case 2: // 8-bit, stereo
                    channel->sample_length = new_instrument->size >> 1;
                    break;
                case 3: // 16-bit, stereo
                    channel->sample_length = new_instrument->size >> 2;
                    break;
            }
            channel->format = (new_instrument->format & 3) | ((new_instrument->flags & 1) << 2);
            channel->key = new_instrument->root_key;
            channel->rate = new_instrument->playback_rate;

#ifdef USE_SPEEXDSP_RESAMPLER
            if (Game_ResamplingQuality > 0) do
            {
                int err, index, converted_length;

                channel->resampler = speex_resampler_init((new_instrument->format & DIG_F_STEREO_MASK)?2:1, new_instrument->playback_rate, new_instrument->playback_rate, SPEEX_RESAMPLER_QUALITY_DESKTOP, &err);
                if ((channel->resampler == NULL) || (err != RESAMPLER_ERR_SUCCESS))
                {
                    channel->resampler = NULL;
                    break;
                }

                converted_length = channel->sample_length * ((new_instrument->format & DIG_F_STEREO_MASK)?2:1);

                channel->converted_data = (float *)malloc(converted_length * sizeof(float));
                if (channel->converted_data == NULL)
                {
                    speex_resampler_destroy(channel->resampler);
                    channel->resampler = NULL;
                    break;
                }

                switch (channel->format)
                {
                    case 0: // 8-bit, mono, unsigned
                    case 2: // 8-bit, stereo, unsigned
                        for (index = 0; index < converted_length; index++)
                        {
                            channel->converted_data[index] = (float)U8toS16(((uint8_t *)channel->sample_data)[index]);
                        }
                        break;
                    case 1: // 16-bit, mono, unsigned
                    case 3: // 16-bit, stereo, unsigned
                        for (index = 0; index < converted_length; index++)
                        {
                            channel->converted_data[index] = (float)(((uint16_t *)channel->sample_data)[index] - 32768);
                        }
                        break;
                    case 4: // 8-bit, mono, signed
                    case 6: // 8-bit, stereo, signed
                        for (index = 0; index < converted_length; index++)
                        {
                            channel->converted_data[index] = (float)S8toS16(((uint8_t *)channel->sample_data)[index]);
                        }
                        break;
                    case 5: // 16-bit, mono, signed
                    case 7: // 16-bit, stereo, signed
                        for (index = 0; index < converted_length; index++)
                        {
                            channel->converted_data[index] = (float)(((int16_t *)channel->sample_data)[index]);
                        }
                        break;
                }
            } while (0);
#endif
        }
    }

}

static void update_channel_volume(midi_channel *channel)
{
    if (channel == NULL) return;

    channel->volume_left = (channel->channel_volume * channel->note_volume * 521 * (128 - channel->pan)) >> 16; // (channel->channel_volume * channel->note_volume * 128 * (128 - channel->pan)) / (127 * 127)
    channel->volume_right = (channel->channel_volume * channel->note_volume * 521 * channel->pan) >> 16; // (channel->channel_volume * channel->note_volume * 128 * channel->pan) / (127 * 127)
}

struct _xmi_player *xmi_player_create(unsigned int rate, const void *wave_synthetizer)
{
    xmi_player *player;
    int ch;

    player = (xmi_player *) calloc(1, sizeof(xmi_player));
    if (player == NULL) return NULL;

    player->rate = rate;
    player->volume = 0;
    player->samples_per_tick = rate / 120;
    player->wave_synthetizer = (const WAVE_ENTRY *) wave_synthetizer;

    player->accum_data = (int32_t *)malloc(player->samples_per_tick * 2 * sizeof(int32_t));
    if (player->accum_data == NULL)
    {
        free(player);
        return NULL;
    }

#ifdef USE_SPEEXDSP_RESAMPLER
    if (Game_ResamplingQuality > 0)
    {
        player->resampled_data = (float *)malloc(player->samples_per_tick * 2 * sizeof(float));
        if (player->resampled_data == NULL)
        {
            free(player->accum_data);
            free(player);
            return NULL;
        }
    }
#endif

    for (ch = 0; ch < 16; ch++)
    {
        update_channel_instrument(player->wave_synthetizer, &(player->channels[ch]));
    }

    return player;
}

void xmi_player_destroy(struct _xmi_player *player)
{
    if (player == NULL) return;

    xmi_player_close(player);

    if (player->accum_data != NULL)
    {
        free(player->accum_data);
    }
#ifdef USE_SPEEXDSP_RESAMPLER
    if (player->resampled_data != NULL)
    {
        free(player->resampled_data);
    }
#endif

    free(player);
}

int xmi_player_open(struct _xmi_player *player, const uint8_t *xmi, int seq_num)
{
    int ch;

    if (player == NULL) return 0;

    xmi_player_close(player);

    player->seq = xmi_find_sequence(xmi, seq_num, &player->seq_len);
    if (player->seq == NULL) return 0;

    player->playing = 1;
    player->samples_left = 0;
    player->loop_count = -1;
    player->pitch_bend_sensitivity = 2;
    player->event_time = 0;
    player->current_event = player->seq;

    for (ch = 0; ch < 16; ch++)
    {
        player->channels[ch].channel_volume = 100;
        player->channels[ch].pan = 64;
        player->channels[ch].note_volume = 0;
        player->channels[ch].pitch_bend = 0;
    }

    return 1;
}

void xmi_player_close(struct _xmi_player *player)
{
    int ch;

    if (player == NULL) return;

    player->playing = 0;
    player->seq = NULL;

    for (ch = 0; ch < 16; ch++)
    {
        player->channels[ch].instrument = NULL;
#ifdef USE_SPEEXDSP_RESAMPLER
        if (player->channels[ch].resampler != NULL)
        {
            speex_resampler_destroy(player->channels[ch].resampler);
            player->channels[ch].resampler = NULL;
        }
        if (player->channels[ch].converted_data != NULL)
        {
            free(player->channels[ch].converted_data);
            player->channels[ch].converted_data = NULL;
        }
#endif
    }
}

void xmi_player_set_volume(struct _xmi_player *player, int volume)
{
    if (player == NULL) return;

    player->volume = volume;
}

void xmi_player_set_loop_count(struct _xmi_player *player, int loop_count)
{
    if (player == NULL) return;

    player->loop_count = loop_count;
}

static INLINE int32_t clamp(int32_t value) {
#if defined(ARMV8)
    return vqmovns_s32(value);
#elif defined(ARMV6) && defined(__ARM_ACLE) && __ARM_FEATURE_SAT
    return __ssat(value, 16);
#elif defined(ARMV6) && defined(__GNUC__)
    asm ( "ssat %[result], #16, %[value]" : [result] "=r" (value) : [value] "r" (value) : "cc" );
    return value;
#else
    return value < -32768 ? -32768 : ((value >= 32768 ? 32767 : value));
#endif
}

void xmi_player_get_data(struct _xmi_player *player, void *buffer, uint32_t size)
{
    uint32_t count, event_time, pos1, pos2;
    int ch, notes_playing;
    const uint8_t *event_data;
    unsigned int status, var_len, index;
    midi_channel *channel;
    int32_t left, right, frac;

    if ((buffer == NULL) || (size == 0)) return;

    memset(buffer, 0, size);

    if ((player == NULL) || (!player->playing)) return;

    size >>= 2;

    left = right = 0; // silence warnings

    while (size != 0)
    {
        if (player->samples_left == 0)
        {
            notes_playing = 0;
            for (ch = 0; ch < 16; ch++)
            {
                if (player->channels[ch].note_volume != 0)
                {
                    player->channels[ch].time_left--;
                    if (player->channels[ch].time_left == 0)
                    {
                        player->channels[ch].note_volume = 0;
                    }
                    else
                    {
                        notes_playing = 1;
                    }
                }
            }

            if ((player->current_event == NULL) && (!notes_playing))
            {
                player->playing = 0;
                break;
            }

            while (player->current_event != NULL)
            {
                event_data = player->current_event;
                event_time = 0;
                while ((*event_data & 0x80) == 0)
                {
                    event_time += *event_data;
                    event_data++;
                }

                if (event_time != player->event_time) break;

                player->event_time = 0;

                status = *event_data;
                event_data++;

                switch (status >> 4)
                {
                    case 0x08: // note off
                        // shouldn't exist in xmi
                        player->channels[status & 0x0f].note_volume = 0;
                        player->current_event = event_data + 2;
                        break;
                    case 0x09: // note on
                        channel = &(player->channels[status & 0x0f]);
                        if (channel->instrument != NULL)
                        {
                            channel->pos = 0;

                            if ((channel->key == event_data[0]) && (channel->pitch_bend == 0))
                            {
                                channel->pos_inc = (((int64_t)channel->rate) << 32) / player->rate;
#ifdef USE_SPEEXDSP_RESAMPLER
                                if (channel->resampler != NULL)
                                {
                                    speex_resampler_set_rate(channel->resampler, channel->rate, player->rate);
                                }
#endif
                            }
                            else
                            {
                                static const double twelfth_root_two = 1.05946309434;
                                double key_diff = (event_data[0] - channel->key) + (channel->pitch_bend * player->pitch_bend_sensitivity / 8192.0);
                                double new_rate = pow(twelfth_root_two, key_diff) * channel->rate;
                                channel->pos_inc = ((int64_t)(new_rate * 4294967296.0)) / player->rate;
#ifdef USE_SPEEXDSP_RESAMPLER
                                if (channel->resampler != NULL)
                                {
                                    speex_resampler_set_rate(channel->resampler, (int32_t)new_rate, player->rate);
                                }
#endif
                            }
#ifdef USE_SPEEXDSP_RESAMPLER
                            if (channel->resampler != NULL)
                            {
                                speex_resampler_reset_mem(channel->resampler);
                                speex_resampler_skip_zeros(channel->resampler);
                            }
#endif

                            channel->note_volume = event_data[1];
                            update_channel_volume(channel);
                        }
                        event_data += 2;
                        READ_VARLEN(var_len, event_data)
                        channel->time_left = var_len;
                        if (var_len == 0)
                        {
                            channel->note_volume = 0;
                        }
                        player->current_event = event_data;
                        break;
                    case 0x0a: // aftertouch
                        // ignore event
                        fprintf(stderr, "unhandled midi event: %.02x %.02x %.02x\n", status, event_data[0], event_data[1]);
                        player->current_event = event_data + 2;
                        break;
                    case 0x0b: // controller
                        switch (event_data[0])
                        {
                            case 0x01: // modulation wheel
                                if (event_data[1] != 0)
                                {
                                    // ignore non-default value
                                    fprintf(stderr, "unhandled midi event: %.02x %.02x %.02x\n", status, event_data[0], event_data[1]);
                                }
                                break;
                            case 0x07: // volume
                                channel = &(player->channels[status & 0x0f]);
                                channel->channel_volume = event_data[1];
                                update_channel_volume(channel);
                                break;
                            case 0x0a: // pan
                                channel = &(player->channels[status & 0x0f]);
                                channel->pan = event_data[1];
                                update_channel_volume(channel);
                                break;
                            case 0x40: // sustain (damper) pedal
                                if (event_data[1] != 0)
                                {
                                    // ignore non-default value
                                    fprintf(stderr, "unhandled midi event: %.02x %.02x %.02x\n", status, event_data[0], event_data[1]);
                                }
                                break;
                            case 0x72: // XMIDI patch bank select
                                channel = &(player->channels[status & 0x0f]);
                                channel->bank = event_data[1];
                                update_channel_instrument(player->wave_synthetizer, channel);
                                break;
                            default:
                                // ignore other controllers
                                fprintf(stderr, "unhandled midi event: %.02x %.02x %.02x\n", status, event_data[0], event_data[1]);
                                break;
                        }
                        player->current_event = event_data + 2;
                        break;
                    case 0x0c: // program change
                        channel = &(player->channels[status & 0x0f]);
                        channel->program = event_data[0];
                        player->current_event = event_data + 1;
                        update_channel_instrument(player->wave_synthetizer, channel);
                        break;
                    case 0x0d: // pressure
                        // ignore event
                        fprintf(stderr, "unhandled midi event: %.02x %.02x %.02x\n", status, event_data[0], 0);
                        player->current_event = event_data + 1;
                        break;
                    case 0x0e: // pitch bend
                        player->channels[status & 0x0f].pitch_bend = ((event_data[0] & 0x7f) | ((event_data[1] & 0x7f) << 7)) - 0x2000;
                        player->current_event = event_data + 2;
                        break;
                    case 0x0f: // sysex / meta events
                        if (status == 0xff) // meta events
                        {
                            if (event_data[0] == 0x2f) // end of track
                            {
                                player->current_event = NULL;
                                break;
                            }

                            // ignore other meta events
                            event_data++;
                            READ_VARLEN(var_len, event_data)
                            player->current_event = event_data + var_len;
                            break;
                        }

                        READ_VARLEN(var_len, event_data)
                        player->current_event = event_data + var_len;
                        break;
                    default:
                        player->current_event = NULL;
                        break;
                }

                if ((player->current_event != NULL) && (player->current_event >= player->seq + player->seq_len))
                {
                    player->current_event = NULL;
                }
            }

            if ((player->current_event == NULL) && (player->loop_count != 0))
            {
                player->current_event = player->seq;
                player->event_time = 0;
                if (player->loop_count > 0) player->loop_count--;
            }
            else
            {
                player->event_time++;
            }

            player->samples_left = player->samples_per_tick;
        }

        count = player->samples_left;
        if (count > size) count = size;

        player->samples_left -= count;
        size -= count;


        memset(player->accum_data, 0, count * 2 * sizeof(int32_t));

        for (ch = 0; ch < 16; ch++)
        {
            channel = &(player->channels[ch]);
            if (channel->note_volume == 0) continue;

#ifdef USE_SPEEXDSP_RESAMPLER
            if (channel->resampler != NULL)
            {
                int err;
                spx_uint32_t in_len, out_len;

                in_len = (spx_uint32_t)(channel->sample_length - channel->pos);
                out_len = count;

                if (channel->format & DIG_F_STEREO_MASK)
                {
                    // stereo
                    err = speex_resampler_process_interleaved_float(channel->resampler, &(channel->converted_data[2 * channel->pos]), &in_len, player->resampled_data, &out_len);
                    if (err == RESAMPLER_ERR_SUCCESS)
                    {
                        channel->pos += in_len;
                        if (out_len < count)
                        {
                            channel->note_volume = 0;
                        }

                        for (index = 0; index < out_len; index++)
                        {
                            player->accum_data[2 * index] += (((int32_t)player->resampled_data[2 * index]) * channel->volume_left) >> (7 + 6);
                            player->accum_data[2 * index + 1] += (((int32_t)player->resampled_data[2 * index + 1]) * channel->volume_right) >> (7 + 6);
                        }
                    }
                }
                else
                {
                    // mono
                    err = speex_resampler_process_float(channel->resampler, 0, &(channel->converted_data[channel->pos]), &in_len, player->resampled_data, &out_len);
                    if (err == RESAMPLER_ERR_SUCCESS)
                    {
                        channel->pos += in_len;
                        if (out_len < count)
                        {
                            channel->note_volume = 0;
                        }

                        for (index = 0; index < out_len; index++)
                        {
                            player->accum_data[2 * index] += (((int32_t)player->resampled_data[index]) * channel->volume_left) >> (7 + 6);
                            player->accum_data[2 * index + 1] += (((int32_t)player->resampled_data[index]) * channel->volume_right) >> (7 + 6);
                        }
                    }
                }
            }
            else
#endif
            for (index = 0; index < count; index++)
            {
                pos1 = channel->pos >> 32;
                frac = ((uint32_t)channel->pos) >> 16;
                pos2 = pos1 + 1;
                if (pos2 >= channel->sample_length) pos2 = pos1;

                switch (channel->format)
                {
                    case 0: // 8-bit, mono, unsigned
                        left = right = (
                            U8toS16(((uint8_t *)channel->sample_data)[pos1]) * (0x10000 - frac) +
                            U8toS16(((uint8_t *)channel->sample_data)[pos2]) * frac
                            ) >> 16;
                        break;
                    case 1: // 16-bit, mono, unsigned
                        left = right = (
                            (((uint16_t *)channel->sample_data)[pos1] - 32768) * (0x10000 - frac) +
                            (((uint16_t *)channel->sample_data)[pos2] - 32768) * frac
                            ) >> 16;
                        break;
                    case 2: // 8-bit, stereo, unsigned
                        left = (
                            U8toS16(((uint8_t *)channel->sample_data)[2 * pos1]) * (0x10000 - frac) +
                            U8toS16(((uint8_t *)channel->sample_data)[2 * pos2]) * frac
                            ) >> 16;
                        right = (
                            U8toS16(((uint8_t *)channel->sample_data)[2 * pos1 + 1]) * (0x10000 - frac) +
                            U8toS16(((uint8_t *)channel->sample_data)[2 * pos2 + 1]) * frac
                            ) >> 16;
                        break;
                    case 3: // 16-bit, stereo, unsigned
                        left = (
                            (((uint16_t *)channel->sample_data)[2 * pos1] - 32768) * (0x10000 - frac) +
                            (((uint16_t *)channel->sample_data)[2 * pos2] - 32768) * frac
                            ) >> 16;
                        right = (
                            (((uint16_t *)channel->sample_data)[2 * pos1 + 1] - 32768) * (0x10000 - frac) +
                            (((uint16_t *)channel->sample_data)[2 * pos2 + 1] - 32768) * frac
                            ) >> 16;
                        break;
                    case 4: // 8-bit, mono, signed
                        left = right = (
                            S8toS16(((uint8_t *)channel->sample_data)[pos1]) * (0x10000 - frac) +
                            S8toS16(((uint8_t *)channel->sample_data)[pos2]) * frac
                            ) >> 16;
                        break;
                    case 5: // 16-bit, mono, signed
                        left = right = (
                            ((int16_t *)channel->sample_data)[pos1] * (0x10000 - frac) +
                            ((int16_t *)channel->sample_data)[pos2] * frac
                            ) >> 16;
                        break;
                    case 6: // 8-bit, stereo, signed
                        left = (
                            S8toS16(((uint8_t *)channel->sample_data)[2 * pos1]) * (0x10000 - frac) +
                            S8toS16(((uint8_t *)channel->sample_data)[2 * pos2]) * frac
                            ) >> 16;
                        right = (
                            S8toS16(((uint8_t *)channel->sample_data)[2 * pos1 + 1]) * (0x10000 - frac) +
                            S8toS16(((uint8_t *)channel->sample_data)[2 * pos2 + 1]) * frac
                            ) >> 16;
                        break;
                    case 7: // 16-bit, stereo, signed
                        left = (
                            ((int16_t *)channel->sample_data)[2 * pos1] * (0x10000 - frac) +
                            ((int16_t *)channel->sample_data)[2 * pos2] * frac
                            ) >> 16;
                        right = (
                            ((int16_t *)channel->sample_data)[2 * pos1 + 1] * (0x10000 - frac) +
                            ((int16_t *)channel->sample_data)[2 * pos2 + 1] * frac
                            ) >> 16;
                        break;
                }

                player->accum_data[2 * index] += (left * channel->volume_left) >> (7 + 6);
                player->accum_data[2 * index + 1] += (right * channel->volume_right) >> (7 + 6);

                channel->pos += channel->pos_inc;
                if ((channel->pos >> 32) >= channel->sample_length)
                {
                    channel->note_volume = 0;
                    break;
                }
            }
        }

        for (index = 0; index < count; index++)
        {
            ((int16_t *)buffer)[2 * index] = clamp((player->accum_data[2 * index] * player->volume) >> 7);
            ((int16_t *)buffer)[2 * index + 1] = clamp((player->accum_data[2 * index + 1] * player->volume) >> 7);
        }

        buffer = &(((int16_t *)buffer)[2 * count]);
    }
}

int xmi_player_is_playing(struct _xmi_player *player)
{
    if (player == NULL) return 0;

    return player->playing;
}

