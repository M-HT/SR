/**
 *
 *  Copyright (C) 2016-2026 Roman Pauer
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

#if defined(__DEBUG__)
#include <inttypes.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-sound.h"
#include "Albion-AIL.h"
#include "Game_memory.h"
#ifdef USE_SDL2
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL/SDL_mixer.h>
#endif
#ifdef USE_SPEEXDSP_RESAMPLER
#include <speex/speex_resampler.h>
#endif


typedef struct _AIL_sample
{
    int channel;
    Game_sample *sample;
    int _stereo, _16bit, _signed;
    void *start;
    uint32_t len;
    int32_t playback_rate;  /* Hz */
    int32_t volume;         /* 0-127 */
    int32_t pan;            /* (0=L ... 127=R) */
    int32_t loop_count;
    Mix_Chunk chunk;
} AIL_sample;


extern int32_t AIL_preference[];

struct _AIL_sample *Game_AIL_allocate_sample_handle(void *dig)
{
    AIL_sample *ret;

#define ORIG_SAMPLE_SIZE 644
#define SAMPLE_SIZE ( (sizeof(AIL_sample) > ORIG_SAMPLE_SIZE)?(sizeof(AIL_sample)):(ORIG_SAMPLE_SIZE) )

    ret = (AIL_sample *) x86_malloc(SAMPLE_SIZE);

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_allocate_sample_handle: return: 0x%" PRIxPTR "\n", (uintptr_t) ret);
#endif

    if (ret != NULL)
    {
        memset(ret, 0, SAMPLE_SIZE);

        ret->sample = NULL;
        ret->channel = Game_MixerChannels;

        Game_MixerChannels++;
        if (Game_MixerChannels > GAME_MIXER_CHANNELS_INITIAL)
        {
            Mix_AllocateChannels(Game_MixerChannels);
        }

        Game_AIL_init_sample(ret);
    }

    return ret;

#undef SAMPLE_SIZE
#undef ORIG_SAMPLE_SIZE
}

void Game_AIL_release_sample_handle(struct _AIL_sample *S)
{
    Game_sample *sample;

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_release_sample_handle: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    Mix_HaltChannel(S->channel);

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

    x86_free(S);
}

void Game_AIL_init_sample(struct _AIL_sample *S)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_init_sample: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    S->_stereo = 0;
    S->_16bit = 0;
    S->_signed = 0;
    S->start = NULL;
    S->len = 0;
    S->playback_rate = 22050;
    S->volume = AIL_preference[DIG_DEFAULT_VOLUME];
    S->pan = 64;
    S->loop_count = 0;

}

void Game_AIL_set_sample_address(struct _AIL_sample *S, void *start, uint32_t len)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_address: 0x%" PRIxPTR ", 0x%" PRIxPTR ", %i\n", (uintptr_t) S, (uintptr_t) start, len);
#endif

    S->start = start;
    S->len = len;
}

void Game_AIL_set_sample_type(struct _AIL_sample *S, int32_t format, uint32_t flags)
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

        if ( sample->orig_data[0] == *( (uint32_t *) &(Sdata[((len - 4) / 8) & 0xfffffffcUL]) ) &&
             sample->orig_data[1] == *( (uint32_t *) &(Sdata[(((len - 4) * 2) / 8) & 0xfffffffcUL]) ) &&
             sample->orig_data[2] == *( (uint32_t *) &(Sdata[(((len - 4) * 3) / 8) & 0xfffffffcUL]) ) &&
             sample->orig_data[3] == *( (uint32_t *) &(Sdata[(((len - 4) * 4) / 8) & 0xfffffffcUL]) ) &&
             sample->orig_data[4] == *( (uint32_t *) &(Sdata[(((len - 4) * 5) / 8) & 0xfffffffcUL]) ) &&
             sample->orig_data[5] == *( (uint32_t *) &(Sdata[(((len - 4) * 6) / 8) & 0xfffffffcUL]) ) &&
             sample->orig_data[6] == *( (uint32_t *) &(Sdata[(((len - 4) * 7) / 8) & 0xfffffffcUL]) )
            )
        {
            return 0;
        }
    }

    return 1;
}

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

void Game_AIL_start_sample(struct _AIL_sample *S)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_start_sample: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    Mix_HaltChannel(S->channel);

    /* set panning */
    {
        int32_t leftpan, rightpan;
        uint32_t left, right;

        leftpan = S->pan;
        rightpan = 127 - S->pan;

        left = (leftpan >= 63)?255:( (leftpan * 265265) >> 16 ); /* (leftpan * 255) / 63 */
        right = (rightpan >= 63)?255:( (rightpan * 265265) >> 16 ); /* (rightpan * 255) / 63 */

        Mix_SetPanning(S->channel, 255, 255);
        if (Game_SwapSoundChannels)
        {
            Mix_SetPanning(S->channel, right, left);
        }
        else
        {
            Mix_SetPanning(S->channel, left, right);
        }
    }

    /* set volume */
    {
        uint32_t volume;

        volume = ( ( (uint32_t) S->volume ) * Game_SoundMasterVolume * 521 ) >> 16; /* (volume * Game_SoundMasterVolume * 128) / (127 * 127) */

        Mix_Volume(S->channel, volume);
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

                channels = (S->_stereo) ? 2 : 1;

                if (S->_16bit)
                {
                    format = (S->_signed) ? AUDIO_S16LSB : AUDIO_U16LSB;
                }
                else
                {
                    format = (S->_signed) ? AUDIO_S8 : AUDIO_U8;
                }

                resample_type = 0;
                if (Game_AudioRate != S->playback_rate)
                {
                    uint32_t newlen_bytes, newlen_samples, form_mult;

#ifdef USE_SPEEXDSP_RESAMPLER
                    if (Game_ResamplingQuality > 0)
                    {
                        SpeexResamplerState *resampler;
                        int err;
                        spx_uint32_t in_len, out_len;
                        int16_t *converted_data;

                        resample_type = 3;

                        newlen_bytes = Get_Resampled_Size(S->_stereo, S->_16bit, S->playback_rate, Game_AudioRate, S->len, &newlen_samples);

                        form_mult = (S->_16bit) ? 1 : 2;

                        SDL_BuildAudioCVT(&cvt, AUDIO_S16LSB, channels, Game_AudioRate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                        sample = (Game_sample *) malloc(offsetof(Game_sample, data) + newlen_bytes * cvt.len_mult * form_mult);

                        sample->start = (uint8_t *) S->start;
                        sample->len = S->len;

                        cvt.buf = (Uint8 *) &(sample->data);
                        cvt.len = newlen_bytes * form_mult;

                        resampler = speex_resampler_init(channels, S->playback_rate, Game_AudioRate, SPEEX_RESAMPLER_QUALITY_DESKTOP, &err);
                        if ((resampler != NULL) && (err == RESAMPLER_ERR_SUCCESS))
                        {
                            if (S->_16bit && S->_signed)
                            {
                                converted_data = NULL;
                            }
                            else
                            {
                                converted_data = (int16_t *)malloc(S->len * (S->_16bit ? 1 : 2));
                                if (converted_data != NULL)
                                {
                                    out_len = S->len;
                                    if (S->_16bit)
                                    {
                                        out_len >>= 1;
                                        for (in_len = 0; in_len < out_len; in_len++)
                                        {
                                            converted_data[in_len] = ((uint16_t *)sample->start)[in_len] - 32768;
                                        }
                                    }
                                    else if (S->_signed)
                                    {
                                        for (in_len = 0; in_len < out_len; in_len++)
                                        {
                                            uint8_t value;
                                            value = ((uint8_t *)sample->start)[in_len];
                                            converted_data[in_len] = (value << 8) | (value ^ 0x80);
                                        }
                                    }
                                    else
                                    {
                                        for (in_len = 0; in_len < out_len; in_len++)
                                        {
                                            uint8_t value;
                                            value = ((uint8_t *)sample->start)[in_len];
                                            converted_data[in_len] = ((value << 8) | value) - 32768;
                                        }
                                    }
                                }
                                else err = RESAMPLER_ERR_ALLOC_FAILED;
                            }

                            if (err == RESAMPLER_ERR_SUCCESS)
                            {
                                speex_resampler_skip_zeros(resampler);

                                in_len = S->len;
                                if (S->_stereo) in_len >>= 1;
                                if (S->_16bit) in_len >>= 1;
                                out_len = newlen_samples;

                                if (S->_stereo)
                                {
                                    err = speex_resampler_process_interleaved_int(resampler, (converted_data != NULL) ? converted_data : (int16_t *)sample->start, &in_len, (int16_t *)cvt.buf, &out_len);
                                }
                                else
                                {
                                    err = speex_resampler_process_int(resampler, 0, (converted_data != NULL) ? converted_data : (int16_t *)sample->start, &in_len, (int16_t *)cvt.buf, &out_len);
                                }
                            }

                            if (converted_data != NULL)
                            {
                                free(converted_data);
                            }

                            speex_resampler_destroy(resampler);
                        }
                        else err = RESAMPLER_ERR_ALLOC_FAILED;

                        if (err != RESAMPLER_ERR_SUCCESS)
                        {
                            free(sample);
                            resample_type = 0;
                        }
                    }
                    else
#endif
#if SDL_VERSION_ATLEAST(2,0,0)
                    if (Game_ResamplingQuality <= 0)
#endif
                    {
                        // interpolated resampling
                        resample_type = 2;

                        newlen_bytes = Get_Resampled_Size(S->_stereo, S->_16bit, S->playback_rate, Game_AudioRate, S->len, &newlen_samples);

                        format = ((!(S->_16bit)) && ((Game_AudioFormat == AUDIO_S8) || (Game_AudioFormat == AUDIO_U8))) ? Game_AudioFormat : AUDIO_S16LSB;

                        form_mult = ((!(S->_16bit)) && (Game_AudioFormat != AUDIO_S8) && (Game_AudioFormat != AUDIO_U8)) ? 2 : 1;

                        SDL_BuildAudioCVT(&cvt, format, channels, Game_AudioRate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                        sample = (Game_sample *) malloc(offsetof(Game_sample, data) + newlen_bytes * cvt.len_mult * form_mult);

                        sample->start = (uint8_t *) S->start;
                        sample->len = S->len;

                        cvt.buf = (Uint8 *) &(sample->data);
                        cvt.len = newlen_bytes * form_mult;
                        Interpolated_Resample(S->_stereo, S->_16bit, S->_signed, S->playback_rate, Game_AudioRate, sample->start, cvt.buf, S->len, newlen_samples);
                    }
                }

                if (resample_type == 0)
                {
                    // sdl resampling

                    SDL_BuildAudioCVT(&cvt, format, channels, S->playback_rate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                    sample = (Game_sample *) malloc(offsetof(Game_sample, data) + S->len * cvt.len_mult);

                    sample->start = (uint8_t *) S->start;
                    sample->len = S->len;

                    cvt.buf = (Uint8 *) &(sample->data);
                    cvt.len = S->len;
                    memcpy(cvt.buf, S->start, cvt.len);
                }

                if (sample->len >= 4)
                {
                    sample->orig_data[0] = *( (uint32_t *) &(sample->start[((sample->len - 4) / 8) & 0xfffffffcUL]) );
                    sample->orig_data[1] = *( (uint32_t *) &(sample->start[(((sample->len - 4) * 2) / 8) & 0xfffffffcUL]) );
                    sample->orig_data[2] = *( (uint32_t *) &(sample->start[(((sample->len - 4) * 3) / 8) & 0xfffffffcUL]) );
                    sample->orig_data[3] = *( (uint32_t *) &(sample->start[(((sample->len - 4) * 4) / 8) & 0xfffffffcUL]) );
                    sample->orig_data[4] = *( (uint32_t *) &(sample->start[(((sample->len - 4) * 5) / 8) & 0xfffffffcUL]) );
                    sample->orig_data[5] = *( (uint32_t *) &(sample->start[(((sample->len - 4) * 6) / 8) & 0xfffffffcUL]) );
                    sample->orig_data[6] = *( (uint32_t *) &(sample->start[(((sample->len - 4) * 7) / 8) & 0xfffffffcUL]) );
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
        S->chunk.volume = 128;

        S->chunk.abuf = (Uint8 *) &(sample->data);
        S->chunk.alen = sample->len_cvt;

        Mix_PlayChannelTimed(S->channel, &S->chunk, S->loop_count, -1);
    }

}

void Game_AIL_end_sample(struct _AIL_sample *S)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_end_sample: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    Mix_HaltChannel(S->channel);
}

void Game_AIL_set_sample_playback_rate(struct _AIL_sample *S, int32_t playback_rate)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_playback_rate: 0x%" PRIxPTR ", %i\n", (uintptr_t) S, playback_rate);
#endif

    S->playback_rate = playback_rate;
}

void Game_AIL_set_sample_volume(struct _AIL_sample *S, int32_t volume)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_volume: 0x%" PRIxPTR ", %i\n", (uintptr_t) S, volume);
#endif

    S->volume = volume; /* 0-127 */
}

void Game_AIL_set_sample_pan(struct _AIL_sample *S, int32_t pan)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_pan: 0x%" PRIxPTR ", %i\n", (uintptr_t) S, pan);
#endif

    S->pan = pan; /* 0-127 */
}

void Game_AIL_set_sample_loop_count(struct _AIL_sample *S, int32_t loop_count)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sample_loop_count: 0x%" PRIxPTR ", %i\n", (uintptr_t) S, loop_count);
#endif

    S->loop_count = loop_count;
}

uint32_t Game_AIL_sample_status(struct _AIL_sample *S)
{
    uint32_t ret;

    ret = ( Mix_Playing(S->channel) )?4:2;

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


/*
AIL_allocate_sample_handle
AIL_end_sample
AIL_init_sample
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
