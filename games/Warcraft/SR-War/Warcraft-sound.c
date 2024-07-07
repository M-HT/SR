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

#include <inttypes.h>
#include <malloc.h>
#include <string.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL/SDL.h>
    #include <SDL/SDL_mixer.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Warcraft-sound.h"


void Game_ChannelFinished(int channel)
{
    Game_channels[channel].status = 1;
    if (Game_channels[channel].sample_num != -1)
    {
        Game_samples[Game_channels[channel].sample_num].status = 2;
        Game_samples[Game_channels[channel].sample_num].channel_num = -1;
        Game_samples[Game_channels[channel].sample_num].EOS_status = 1;
        Game_channels[channel].sample_num = -1;
    }
    Game_channels[channel].status = 0;
}

static void HaltChannel(int channel)
{
    int sample_num;

    sample_num = Game_channels[channel].sample_num;
    Game_channels[channel].sample_num = -1;

    Mix_HaltChannel(channel);

    Game_channels[channel].sample_num = sample_num;
}

AIL_sample *Game_AIL_allocate_sample_handle(void *dig)
{
    AIL_sample *ret;
    int loop;

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_allocate_sample_handle: ");
#endif

    ret = NULL;
    for (loop = 0; loop < GAME_SAMPLES_MAXIMUM; loop++)
    {
        if (Game_samples[loop].status == 0)
        {
            ret = &(Game_samples[loop]);
            break;
        }
    }

#if defined(__DEBUG__)
    fprintf(stderr, "return: 0x%" PRIxPTR "\n", (uintptr_t) ret);
#endif

    memset(ret, 0, sizeof(AIL_sample));
    ret->status = 1;
    ret->sample_num = loop;

    return ret;
}

void Game_AIL_release_sample_handle(AIL_sample *S)
{
    Game_sample *sample;

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_release_sample_handle: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    S->EOS = NULL;
    if (S->status == 4 &&
        S->channel_num != -1
        )
    {
        int channel_num;

        channel_num = S->channel_num;
        S->status = 2;
        HaltChannel(channel_num);

        Game_channels[channel_num].status = 1;
        S->channel_num = -1;
        Game_channels[channel_num].sample_num = -1;
        Game_channels[channel_num].status = 0;
    }

    if (S->sample != NULL)
    {
        sample = S->sample;

        if (sample->self_ptr == NULL)
        {
            free(sample);
        }
        else
        {
            sample->num_ref--;
        }

        S->sample = NULL;
    }

    S->status = 0;

}

void Game_AIL_init_sample(AIL_sample *S)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_init_sample: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    S->channel_num = -1;
    S->_stereo = 0;
    S->_16bit = 0;
    S->_signed = 0;
    S->start = NULL;
    S->len = 0;
    S->playback_rate = 22050;
    S->volume = /*127*/ default_sample_volume;
    S->pan = 64;
    S->loop_count = 0;
    S->EOS = NULL;
    S->EOS_status = 0;
    S->status = 2;
}

void Game_AIL_set_sample_address(AIL_sample *S, void *start, uint32_t len)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_address: 0x%" PRIxPTR ", 0x%" PRIxPTR ", %i\n", (uintptr_t) S, (uintptr_t) start, len);
#endif

    S->start = start;
    S->len = len;
}

void Game_AIL_set_sample_type(AIL_sample *S, int32_t format, uint32_t flags)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_type: 0x%" PRIxPTR ", 0x%x, %i\n", (uintptr_t) S, format, flags);
#endif

    S->_stereo = (format & 2)?1:0;
    S->_16bit = (format & 1)?1:0;
    S->_signed = (flags & 1)?1:0;
}

static int Compare_Samples(AIL_sample *S, Game_sample *sample)
{
    unsigned int len;
    uint8_t *Sdata;

    if (sample == NULL) return -1;

    if (S->_stereo == sample->_stereo &&
        S->_16bit == sample->_16bit &&
        S->_signed == sample->_signed &&
        S->start == sample->start &&
        S->playback_rate == sample->playback_rate &&
        S->len == sample->len &&

        S->len >= 4
        )
    {
        len = S->len;
        Sdata = (uint8_t *) S->start;

        if ( sample->orig_data[0] == *( (uint32_t *) &(Sdata[((len - 4) / 8) & 0xfffffffc]) ) &&
             sample->orig_data[1] == *( (uint32_t *) &(Sdata[((len - 4) / 4) & 0xfffffffc]) ) &&
             sample->orig_data[2] == *( (uint32_t *) &(Sdata[((len - 4) / 2) & 0xfffffffc]) ) &&
             sample->orig_data[3] == *( (uint32_t *) &(Sdata[sample->orig_data_offset[0]]) ) &&
             sample->orig_data[4] == *( (uint32_t *) &(Sdata[sample->orig_data_offset[1]]) ) &&
             sample->orig_data[5] == *( (uint32_t *) &(Sdata[sample->orig_data_offset[2]]) )
            )
        {
            return 0;
        }
    }

    return 1;
}

#if !defined(USE_SDL2)
static int Is_MulPow2(uint32_t src_rate, uint32_t dst_rate)
{
    uint32_t higher, lower;
    int mult;

    higher = (src_rate <= dst_rate)?dst_rate:src_rate;
    lower = (src_rate <= dst_rate)?src_rate:dst_rate;
    mult = (src_rate <= dst_rate)?1:-1;

    while (higher >= lower)
    {
        if (higher == lower)
        {
            return mult;
        }
        else if (higher & 1)
        {
            break;
        }
        higher /= 2;
        mult*=2;
    }
    return 0;
}
#endif

// returns dst size in bytes
static uint32_t Get_Resampled_Size(int _stereo, int _16bit, uint32_t src_rate, uint32_t dst_rate, uint32_t src_size_bytes, uint32_t *dst_size_samples)
{
    if (!_stereo && !_16bit)
    {
        // byte samples
        *dst_size_samples = ( ((uint64_t) src_size_bytes) * dst_rate ) / src_rate;
        return *dst_size_samples;
    }
    else if (!_stereo || !_16bit)
    {
        // word samples
        *dst_size_samples = ( ((uint64_t) (src_size_bytes / 2)) * dst_rate ) / src_rate;
        return *dst_size_samples * 2;
    }
    else
    {
        // dword samples
        *dst_size_samples = ( ((uint64_t) (src_size_bytes / 4)) * dst_rate ) / src_rate;
        return *dst_size_samples * 4;
    }
}

static void Resample(int _stereo, int _16bit, uint32_t src_rate, uint32_t dst_rate, uint8_t *srcbuf, uint8_t *dstbuf, uint32_t dst_size)
{
    if (src_rate <= dst_rate)
    {
        uint32_t delta, curpos;

        curpos = 0;
        delta = (((uint64_t) src_rate) << 31) / dst_rate;

        if (!_stereo && !_16bit)
        {
            // byte samples
            uint8_t *src, *dst;

            src = srcbuf;
            dst = dstbuf;

            while (dst_size != 0)
            {
                *dst = *src;
                dst++;
                curpos+=delta;
                if (curpos & 0x80000000)
                {
                    curpos &= 0x7fffffff;
                    src++;
                }
                dst_size--;
            }
        }
        else if (!_stereo || !_16bit)
        {
            // word samples
            uint16_t *src, *dst;

            src = (uint16_t *) srcbuf;
            dst = (uint16_t *) dstbuf;

            while (dst_size != 0)
            {
                *dst = *src;
                dst++;
                curpos+=delta;
                if (curpos & 0x80000000)
                {
                    curpos &= 0x7fffffff;
                    src++;
                }
                dst_size--;
            }
        }
        else
        {
            // dword samples
            uint32_t *src, *dst;

            src = (uint32_t *) srcbuf;
            dst = (uint32_t *) dstbuf;

            while (dst_size != 0)
            {
                *dst = *src;
                dst++;
                curpos+=delta;
                if (curpos & 0x80000000)
                {
                    curpos &= 0x7fffffff;
                    src++;
                }
                dst_size--;
            }
        }
    }
    else
    {
        uint32_t delta_frac, delta_int, curpos;

        curpos = 0;
        delta_int  = ( (((uint64_t) src_rate) << 31) / dst_rate ) >> 31;
        delta_frac = ( (((uint64_t) src_rate) << 31) / dst_rate ) & 0x7fffffff;

        if (!_stereo && !_16bit)
        {
            // byte samples
            uint8_t *src, *dst;

            src = srcbuf;
            dst = dstbuf;

            while (dst_size != 0)
            {
                *dst = *src;
                dst++;
                src+=delta_int;
                curpos+=delta_frac;
                if (curpos & 0x80000000)
                {
                    curpos &= 0x7fffffff;
                    src++;
                }
                dst_size--;
            }
        }
        else if (!_stereo || !_16bit)
        {
            // word samples
            uint16_t *src, *dst;

            src = (uint16_t *) srcbuf;
            dst = (uint16_t *) dstbuf;

            while (dst_size != 0)
            {
                *dst = *src;
                dst++;
                src+=delta_int;
                curpos+=delta_frac;
                if (curpos & 0x80000000)
                {
                    curpos &= 0x7fffffff;
                    src++;
                }
                dst_size--;
            }
        }
        else
        {
            // dword samples
            uint32_t *src, *dst;

            src = (uint32_t *) srcbuf;
            dst = (uint32_t *) dstbuf;

            while (dst_size != 0)
            {
                *dst = *src;
                dst++;
                src+=delta_int;
                curpos+=delta_frac;
                if (curpos & 0x80000000)
                {
                    curpos &= 0x7fffffff;
                    src++;
                }
                dst_size--;
            }
        }
    }
}

#if !defined(USE_SDL2)
static void Interpolated_Resample(int _stereo, int _16bit, int _signed, uint32_t src_rate, uint32_t dst_rate, uint8_t *srcbuf, uint8_t *dstbuf, uint32_t src_size, uint32_t dst_size)
{
    uint32_t src_delta, src_pos, dst_lastsize;
    uint8_t *srclastbuf;

#define RESAMPLE(CALC, src_shift, dst_delta, src_type, dst_type, temp_type, dst_shift, dst_sub) \
    { \
        for (; dst_size != 0; dst_size--) \
        { \
            CALC(src_type, dst_type, temp_type, dst_shift, dst_sub) \
            \
            src_pos += src_delta; \
            srcbuf += (src_pos >> 16) << (src_shift); \
            src_pos &= 0xffff; \
            dstbuf += (dst_delta); \
        } \
        src_pos = 0x10000; \
        srcbuf = srclastbuf; \
        for (; dst_lastsize != 0; dst_lastsize--) \
        { \
            CALC(src_type, dst_type, temp_type, dst_shift, dst_sub) \
            \
            dstbuf += (dst_delta); \
        } \
    }

#define CALC_MONO(src_type, dst_type, temp_type, dst_shift, dst_sub) \
    { \
        ((dst_type *)dstbuf)[0] = (( ( ((temp_type) (((src_type *) srcbuf)[0])) * ((temp_type) (0x10000 - src_pos)) ) + ( ((temp_type) (((src_type *) srcbuf)[1])) * ((temp_type) src_pos) ) ) >> (16 - (dst_shift))) - (dst_sub); \
    }

#define CALC_STEREO(src_type, dst_type, temp_type, dst_shift, dst_sub) \
    { \
        ((dst_type *)dstbuf)[0] = (( ( ((temp_type) (((src_type *) srcbuf)[0])) * ((temp_type) (0x10000 - src_pos)) ) + ( ((temp_type) (((src_type *) srcbuf)[2])) * ((temp_type) src_pos) ) ) >> (16 - (dst_shift))) - (dst_sub); \
        ((dst_type *)dstbuf)[1] = (( ( ((temp_type) (((src_type *) srcbuf)[1])) * ((temp_type) (0x10000 - src_pos)) ) + ( ((temp_type) (((src_type *) srcbuf)[3])) * ((temp_type) src_pos) ) ) >> (16 - (dst_shift))) - (dst_sub); \
    }

    dst_lastsize = (((((uint64_t)dst_rate) << 16) / src_rate) + 0xffff) >> 16;
    dst_size -= dst_lastsize;
    src_delta = (((uint64_t)src_rate) << 16) / dst_rate;
    if (src_rate > dst_rate)
    {
        src_pos = (((uint64_t)(src_rate - dst_rate)) << 16) / src_rate;
    }
    else
    {
        src_pos = 0;
    }

    if (!_16bit)
    {
        if (_stereo)
        {
            srclastbuf = srcbuf + src_size - 2 * 2;

            if (Game_AudioFormat == AUDIO_S8)
            {
                if (_signed)
                    RESAMPLE(CALC_STEREO, 1, 2, int8_t, int8_t, int32_t, 0, 0)
                else
                    RESAMPLE(CALC_STEREO, 1, 2, uint8_t, int8_t, uint32_t, 0, 0x80)
            }
            else if (Game_AudioFormat == AUDIO_U8)
            {
                if (_signed)
                    RESAMPLE(CALC_STEREO, 1, 2, int8_t, uint8_t, int32_t, 0, -0x80)
                else
                    RESAMPLE(CALC_STEREO, 1, 2, uint8_t, uint8_t, uint32_t, 0, 0)
            }
            else
            {
                if (_signed)
                    RESAMPLE(CALC_STEREO, 1, 4, int8_t, int16_t, int32_t, 8, 0)
                else
                    RESAMPLE(CALC_STEREO, 1, 4, uint8_t, int16_t, uint32_t, 8, 0x8000)
            }
        }
        else
        {
            srclastbuf = srcbuf + src_size - 1 * 2;

            if (Game_AudioFormat == AUDIO_S8)
            {
                if (_signed)
                    RESAMPLE(CALC_MONO, 0, 1, int8_t, int8_t, int32_t, 0, 0)
                else
                    RESAMPLE(CALC_MONO, 0, 1, uint8_t, int8_t, uint32_t, 0, 0x80)
            }
            else if (Game_AudioFormat == AUDIO_U8)
            {
                if (_signed)
                    RESAMPLE(CALC_MONO, 0, 1, int8_t, uint8_t, int32_t, 0, -0x80)
                else
                    RESAMPLE(CALC_MONO, 0, 1, uint8_t, uint8_t, uint32_t, 0, 0)
            }
            else
            {
                if (_signed)
                    RESAMPLE(CALC_MONO, 0, 2, int8_t, int16_t, int32_t, 8, 0)
                else
                    RESAMPLE(CALC_MONO, 0, 2, uint8_t, int16_t, uint32_t, 8, 0x8000)
            }
        }
    }
    else
    {
        if (_stereo)
        {
            srclastbuf = srcbuf + src_size - 4 * 2;

            if (_signed)
                RESAMPLE(CALC_STEREO, 2, 4, int16_t, int16_t, int32_t, 0, 0)
            else
                RESAMPLE(CALC_STEREO, 2, 4, uint16_t, int16_t, uint32_t, 0, 0x8000)
        }
        else
        {
            srclastbuf = srcbuf + src_size - 2 * 2;

            if (_signed)
                RESAMPLE(CALC_MONO, 1, 2, int16_t, int16_t, int32_t, 0, 0)
            else
                RESAMPLE(CALC_MONO, 1, 2, uint16_t, int16_t, uint32_t, 0, 0x8000)
        }
    }

#undef CALC_STEREO
#undef CALC_MONO
#undef RESAMPLE
}
#endif

void Game_AIL_start_sample(AIL_sample *S)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_start_sample: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    /* allocate channel */
    {
        int channel_num, loop;
        AIL_sample_CB EOS;

        EOS = S->EOS;
        S->EOS = NULL;
        if (S->status == 4 &&
            S->channel_num != -1
            )
        {
            channel_num = S->channel_num;
            S->status = 2;
            HaltChannel(channel_num);
        }
        else
        {
            channel_num = -1;
            for (loop = 0; loop < Game_MixerChannels; loop++)
            {
                if (Game_channels[loop].status == 0)
                {
                    channel_num = loop;
                    break;
                }
            }
            if (channel_num == -1)
            {
                if (Game_MixerChannels < GAME_MIXER_CHANNELS_MAXIMUM)
                {
                    channel_num = Game_MixerChannels;
                    Game_MixerChannels++;
#if defined(__DEBUG__)
        fprintf(stderr, "allocating more channels\n");
        fflush(stderr);
#endif
                    Mix_AllocateChannels(Game_MixerChannels);
                }
            }
        }
        S->EOS_status = 0;
        S->EOS = EOS;

        if (channel_num != -1)
        {
            Game_channels[channel_num].sample_num = S->sample_num;
            Game_channels[channel_num].status = 1;
            S->channel_num = channel_num;
        }
        else
        {
#if defined(__DEBUG__)
        fprintf(stderr, "unable to allocate channel\n");
        fflush(stderr);
#endif
            S->EOS_status = 1;

            return;
        }
    }
#if defined(__DEBUG__)
        fprintf(stderr, "using channel: %i\n", S->channel_num);
        fflush(stderr);
#endif

    /* set panning */
    {
        int32_t leftpan, rightpan;
        uint32_t left, right;

        leftpan = S->pan;
        rightpan = 127 - S->pan;

        left = (leftpan >= 63)?255:( (leftpan * 265265) >> 16 ); /* (leftpan * 255) / 63 */
        right = (rightpan >= 63)?255:( (rightpan * 265265) >> 16 ); /* (rightpan * 255) / 63 */

        Mix_SetPanning(S->channel_num, 255, 255);
        Mix_SetPanning(S->channel_num, left, right);
    }

    /* set volume */
    {
        uint32_t volume;

        volume = ( ( (uint32_t) S->volume ) * Game_SoundMasterVolume * 521 ) >> 16; /* (volume * Game_SoundMasterVolume * 128) / (127 * 127) */

        Mix_Volume(S->channel_num, volume);
    }

    /* play sample */
    {
        SDL_AudioCVT cvt;
        unsigned int channels, format;
        int loop, minindex, resample_type;
        uint32_t minvalue;
        Game_sample *sample;

        sample = S->sample;
        if (sample != NULL)
        {
            if ( Compare_Samples(S, sample) )
            {
                if (sample->self_ptr == NULL)
                {
                    free(sample);
                }
                else
                {
                    sample->num_ref--;
                }

                sample = NULL;
            }

        }

        if (sample == NULL)
        {
            /* played sample is NOT the same as the sample played before */

            for (loop = 0; loop < GAME_SAMPLE_CACHE_SIZE; loop++)
            {
                if ( Compare_Samples(S, Game_SampleCache[loop]) == 0 )
                {
                    sample = Game_SampleCache[loop];
                }
            }

            if (sample == NULL)
            {
                /* played sample was NOT found in sample cache */
#if defined(__DEBUG__)
                fprintf(stderr, "sample was NOT found in sample cache\n");
#endif

                memset(&cvt, 0, sizeof(SDL_AudioCVT));

                channels = (S->_stereo)?2:1;

                if (S->_16bit)
                {
                    if (S->_signed)
                    {
                        format = AUDIO_S16LSB;
                    }
                    else
                    {
                        format = AUDIO_U16LSB;
                    }
                }
                else
                {
                    if (S->_signed)
                    {
                        format = AUDIO_S8;
                    }
                    else
                    {
                        format = AUDIO_U8;
                    }
                }

                resample_type = 0;
                if (Game_InterpolateAudio)
                {
#if !defined(USE_SDL2)
                    if (Game_AudioRate != S->playback_rate)
                    {
                        // interpolated resampling
                        uint32_t newlen_bytes, newlen_samples, form_mult;

                        resample_type = 2;

                        newlen_bytes = Get_Resampled_Size(S->_stereo, S->_16bit, S->playback_rate, Game_AudioRate, S->len, &newlen_samples);

                        if ((!(S->_16bit)) && ((Game_AudioFormat == AUDIO_S8) || (Game_AudioFormat == AUDIO_U8)))
                        {
                            format = Game_AudioFormat;
                        }
                        else
                        {
                            format = AUDIO_S16LSB;
                        }

                        if ((!(S->_16bit)) && (Game_AudioFormat != AUDIO_S8) && (Game_AudioFormat != AUDIO_U8))
                        {
                            form_mult = 2;
                        }
                        else
                        {
                            form_mult = 1;
                        }

                        SDL_BuildAudioCVT(&cvt, format, channels, Game_AudioRate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                        sample = (Game_sample *) malloc(sizeof(Game_sample) + newlen_bytes * cvt.len_mult * form_mult);

                        sample->start = (uint8_t *) S->start;
                        sample->len = S->len;

                        cvt.buf = (Uint8 *) &(sample->data);
                        cvt.len = newlen_bytes * form_mult;
                        Interpolated_Resample(S->_stereo, S->_16bit, S->_signed, S->playback_rate, Game_AudioRate, sample->start, cvt.buf, S->len, newlen_samples);

                        if (sample->len >= 4)
                        {
                            sample->orig_data[0] = *( (uint32_t *) &(sample->start[((sample->len - 4) / 8) & 0xfffffffc]) );
                            sample->orig_data[1] = *( (uint32_t *) &(sample->start[((sample->len - 4) / 4) & 0xfffffffc]) );
                            sample->orig_data[2] = *( (uint32_t *) &(sample->start[((sample->len - 4) / 2) & 0xfffffffc]) );
                            sample->orig_data_offset[0] = (rand() % (sample->len - 4)) & 0xfffffffc;
                            sample->orig_data[3] = *( (uint32_t *) &(sample->start[sample->orig_data_offset[0]]) );
                            sample->orig_data_offset[1] = (rand() % (sample->len - 4)) & 0xfffffffc;
                            sample->orig_data[4] = *( (uint32_t *) &(sample->start[sample->orig_data_offset[1]]) );
                            sample->orig_data_offset[2] = (rand() % (sample->len - 4)) & 0xfffffffc;
                            sample->orig_data[5] = *( (uint32_t *) &(sample->start[sample->orig_data_offset[2]]) );
                        }
                    }
#endif
                }
                else
                {
                    // SDL1 supports resampling only when higher frequency equals lower frequency multiplied by power of 2,
                    // so I'm using my resampling when it's not the case
                    // or when the target frequency is lower than the source frequency to use less memory
#if !defined(USE_SDL2)
                    if (Game_AudioRate < S->playback_rate ||
                        (Is_MulPow2(Game_AudioRate, S->playback_rate) == 0)
                       )
#endif
                    {
                        // near-neighbour resampling
                        uint32_t newlen_bytes, newlen_samples;

                        resample_type = 1;

                        newlen_bytes = Get_Resampled_Size(S->_stereo, S->_16bit, S->playback_rate, Game_AudioRate, S->len, &newlen_samples);

                        SDL_BuildAudioCVT(&cvt, format, channels, Game_AudioRate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                        sample = (Game_sample *) malloc(sizeof(Game_sample) + newlen_bytes * cvt.len_mult);

                        sample->start = (uint8_t *) S->start;
                        sample->len = S->len;

                        cvt.buf = (Uint8 *) &(sample->data);
                        cvt.len = newlen_bytes;
                        Resample(S->_stereo, S->_16bit, S->playback_rate, Game_AudioRate, sample->start, cvt.buf, newlen_samples);

                        if (sample->len >= 4)
                        {
                            sample->orig_data[0] = *( (uint32_t *) &(sample->start[((sample->len - 4) / 8) & 0xfffffffc]) );
                            sample->orig_data[1] = *( (uint32_t *) &(sample->start[((sample->len - 4) / 4) & 0xfffffffc]) );
                            sample->orig_data[2] = *( (uint32_t *) &(sample->start[((sample->len - 4) / 2) & 0xfffffffc]) );
                            sample->orig_data_offset[0] = (rand() % (sample->len - 4)) & 0xfffffffc;
                            sample->orig_data[3] = *( (uint32_t *) &(sample->start[sample->orig_data_offset[0]]) );
                            sample->orig_data_offset[1] = (rand() % (sample->len - 4)) & 0xfffffffc;
                            sample->orig_data[4] = *( (uint32_t *) &(sample->start[sample->orig_data_offset[1]]) );
                            sample->orig_data_offset[2] = (rand() % (sample->len - 4)) & 0xfffffffc;
                            sample->orig_data[5] = *( (uint32_t *) &(sample->start[sample->orig_data_offset[2]]) );
                        }
                    }
                }

                if (resample_type == 0)
                {
                    // sdl resampling

                    SDL_BuildAudioCVT(&cvt, format, channels, S->playback_rate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                    sample = (Game_sample *) malloc(sizeof(Game_sample) + S->len * cvt.len_mult);

                    sample->start = (uint8_t *) S->start;
                    sample->len = S->len;

                    cvt.buf = (Uint8 *) &(sample->data);
                    cvt.len = S->len;
                    memcpy(cvt.buf, S->start, cvt.len);

                    if (cvt.len >= 4)
                    {
                        sample->orig_data[0] = *( (uint32_t *) &(cvt.buf[((cvt.len - 4) / 8) & 0xfffffffc]) );
                        sample->orig_data[1] = *( (uint32_t *) &(cvt.buf[((cvt.len - 4) / 4) & 0xfffffffc]) );
                        sample->orig_data[2] = *( (uint32_t *) &(cvt.buf[((cvt.len - 4) / 2) & 0xfffffffc]) );
                        sample->orig_data_offset[0] = (rand() % (cvt.len - 4)) & 0xfffffffc;
                        sample->orig_data[3] = *( (uint32_t *) &(cvt.buf[sample->orig_data_offset[0]]) );
                        sample->orig_data_offset[1] = (rand() % (cvt.len - 4)) & 0xfffffffc;
                        sample->orig_data[4] = *( (uint32_t *) &(cvt.buf[sample->orig_data_offset[1]]) );
                        sample->orig_data_offset[2] = (rand() % (cvt.len - 4)) & 0xfffffffc;
                        sample->orig_data[5] = *( (uint32_t *) &(cvt.buf[sample->orig_data_offset[2]]) );
                    }
                }

                SDL_ConvertAudio(&cvt);

                sample->self_ptr = NULL;
                sample->num_ref = 0;

                sample->_stereo = S->_stereo;
                sample->_16bit = S->_16bit;
                sample->_signed = S->_signed;
                sample->playback_rate = S->playback_rate;

                sample->len_cvt = cvt.len_cvt;

                S->sample = sample;

                /* store sample into cache */
                for (loop = 0; loop < GAME_SAMPLE_CACHE_SIZE; loop++)
                {
                    if ( Game_SampleCache[loop] == NULL )
                    {
                        Game_SampleCache[loop] = sample;
                        sample->self_ptr = &(Game_SampleCache[loop]);
                        sample->num_ref = 1;

                        break;
                    }
                }

                if (sample->self_ptr == NULL)
                {
                    minindex = -1;
                    minvalue = 0xffffffff;

                    for (loop = 0; loop < GAME_SAMPLE_CACHE_SIZE; loop++)
                    {
                        if ( Game_SampleCache[loop]->num_ref == 0 )
                        {
                            if (Game_SampleCache[loop]->time < minvalue)
                            {
                                minvalue = Game_SampleCache[loop]->time;
                                minindex = loop;
                            }
                        }
                    }

                    if (minindex >= 0)
                    {
                        free(Game_SampleCache[minindex]);
                        Game_SampleCache[minindex] = sample;
                        sample->self_ptr = &(Game_SampleCache[minindex]);
                        sample->num_ref = 1;
                    }
                }
            }
            else
            {
                /* played sample was found in sample cache */
#if defined(__DEBUG__)
                fprintf(stderr, "sample was found in sample cache\n");
#endif

                S->sample = sample;
                sample->num_ref++;
            }
        }
#if defined(__DEBUG__)
        else
        {
            fprintf(stderr, "sample is the same as the sample played before\n");
        }
#endif

        sample->time = SDL_GetTicks();

        S->chunk.allocated = 0;
        S->chunk.volume = MIX_MAX_VOLUME;

        S->chunk.abuf = (Uint8 *) &(sample->data);
        S->chunk.alen = sample->len_cvt;

        S->status = 4;
        Mix_PlayChannelTimed(S->channel_num, &S->chunk, S->loop_count, -1);
    }

}

void Game_AIL_end_sample(AIL_sample *S)
{
    AIL_sample_CB EOS;

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_end_sample: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    EOS = S->EOS;
    S->EOS = NULL;
    if (S->status == 4 &&
        S->channel_num != -1
        )
    {
        int channel_num;

        channel_num = S->channel_num;
        S->status = 2;
        HaltChannel(channel_num);

        Game_channels[channel_num].status = 1;
        S->channel_num = -1;
        Game_channels[channel_num].sample_num = -1;
        Game_channels[channel_num].status = 0;

        if (S->EOS_status == 0) S->EOS_status = 1;
    }
    S->status = 2;
    S->EOS = EOS;

}

void Game_AIL_set_sample_playback_rate(AIL_sample *S, int32_t playback_rate)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_playback_rate: 0x%" PRIxPTR ", %i\n", (uintptr_t) S, playback_rate);
#endif

    S->playback_rate = playback_rate;
}

void Game_AIL_set_sample_volume(AIL_sample *S, int32_t volume)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_volume: 0x%" PRIxPTR ", %i\n", (uintptr_t) S, volume);
#endif

    S->volume = volume; /* 0-127 */
}

void Game_AIL_set_sample_pan(AIL_sample *S, int32_t pan)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_pan: 0x%" PRIxPTR ", %i\n", (uintptr_t) S, pan);
#endif

    S->pan = pan; /* 0-127 */
}

void Game_AIL_set_sample_loop_count(AIL_sample *S, int32_t loop_count)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_loop_count: 0x%" PRIxPTR ", %i\n", (uintptr_t) S, loop_count);
#endif

    S->loop_count = loop_count;
}

uint32_t Game_AIL_sample_status(AIL_sample *S)
{
    uint32_t ret;

    ret = (S->status == 0)?1:( (S->status == 1)?2:( S->status ) );

/*
#define SMP_FREE          0x0001    // Sample is available for allocation

#define SMP_DONE          0x0002    // Sample has finished playing, or has
                                    // never been started

#define SMP_PLAYING       0x0004    // Sample is playing

#define SMP_STOPPED       0x0008    // Sample has been stopped

#define SMP_PLAYINGBUTRELEASED 0x0010 // Sample is playing, but digital handle
                                      // has been temporarily released

*/

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_sample_status: 0x%" PRIxPTR "  return: %i\n", (uintptr_t) S, ret);
#endif

    return ret;
}

void Game_AIL_set_digital_master_volume(void *dig, int32_t master_volume)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_digital_master_volume: %i\n", master_volume);
#endif

    Game_SoundMasterVolume = (uint32_t) master_volume; /* 0-127 */
    if (Game_SoundMasterVolume > 127) Game_SoundMasterVolume = 127;
}

AIL_sample_CB Game_AIL_register_EOS_callback(AIL_sample *S, AIL_sample_CB EOS)
{
    AIL_sample_CB ret;

    ret = S->EOS;

    S->EOS_status = 0;
    S->EOS = EOS;

    return ret;
}

int32_t Game_AIL_active_sample_count(void *dig)
{
    int ret, loop;

    ret = 0;

    for (loop = 0; loop < GAME_SAMPLES_MAXIMUM; loop++)
    {
        if (Game_samples[loop].status == 4) ret++;
    }

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_active_sample_count: return: %i\n", ret);
#endif

    return ret;
}


/*
AIL_active_sample_count
AIL_allocate_sample_handle
AIL_end_sample
AIL_init_sample
AIL_register_EOS_callback
AIL_release_sample_handle
AIL_sample_status
AIL_set_digital_master_volume
AIL_set_sample_address
AIL_set_sample_loop_count
AIL_set_sample_pan
AIL_set_sample_playback_rate
AIL_set_sample_type
AIL_set_sample_volume
AIL_start_sample
*/
