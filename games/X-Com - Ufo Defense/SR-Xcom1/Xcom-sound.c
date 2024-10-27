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

#include <malloc.h>
#include <string.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Xcom-sound.h"
#ifdef USE_SPEEXDSP_RESAMPLER
#include <speex/speex_resampler.h>
#endif


#define NOTPLAYING 0        // No sound is playing.
#define PLAYINGNOTPENDING 1 // Playing a sound, but no sound is pending.
#define PENDINGSOUND 2      // Playing, and a sound is pending.


#pragma pack(2)

typedef struct _DIGPAK_SNDSTRUC_ {
    PTR32(char) sound;          // address of audio data. (originally real mode ptr)
    uint16_t sndlen;            // Length of audio sample.
    PTR32(int16_t) IsPlaying;   // Address of play status flag.
    int16_t frequency;          // Playback frequency. recommended 11khz.
} DIGPAK_SNDSTRUC;

#pragma pack()


static int sound_initialized = 0;
static SDL_mutex *sound_mutex = NULL;
static int sound_read_index;

#ifdef USE_SPEEXDSP_RESAMPLER
static SpeexResamplerState *resampler = NULL;
#endif


void Game_ChannelFinished(int channel)
{
    if (Game_samples[0].active)
    {
        //if (Game_samples[0].IsPlaying != NULL) *(Game_samples[0].IsPlaying) = 0;
        Game_samples[0].active = 0;
    }
}

static void Game_PlayAudio(void)
{
    //Mix_HaltChannel(GAME_SOUND_CHANNEL);

    Game_AudioChunk.allocated = 0;
    Game_AudioChunk.volume = MIX_MAX_VOLUME;

    if (Game_samples[0].start == NULL)
    {
        Game_AudioChunk.abuf = (Uint8 *) Game_samples[0].sound;
        Game_AudioChunk.alen = Game_samples[0].len;
    }
    else
    {
        Game_AudioChunk.abuf = (Uint8 *) Game_samples[0].start;
        Game_AudioChunk.alen = Game_samples[0].len_cvt;
    }

    Mix_PlayChannelTimed(GAME_SOUND_CHANNEL, &Game_AudioChunk, 0, -1);

    //if (Game_samples[0].IsPlaying != NULL) *(Game_samples[0].IsPlaying) = 1;
}

int16_t Game_ProcessAudio(void)
{
    void *start;

    if (!(Game_samples[0].active))
    {
        if (Game_samples[1].active)
        {
            start = Game_samples[0].start;

            //senquack - SOUND STUFF
            //optimizing
//            memcpy(&(Game_samples[0]), &(Game_samples[1]), sizeof(Game_sample));
//            memset(&(Game_samples[1]), 0, sizeof(Game_sample));
            Game_samples[0] = Game_samples[1];	// For all I know, this actually calls memcpy,
                                                            // but we have an arm-optimized we'll use anyway
            Game_samples[1].active = 0;	// Pretty sure we only need to zero these.
            Game_samples[1].start = start;

            Game_PlayAudio();

            Game_AudioPending = 0;

            return PLAYINGNOTPENDING;
        }
        else
        {
            return NOTPLAYING;
        }
    }
    else
    {
        return (Game_samples[1].active) ? PENDINGSOUND : PLAYINGNOTPENDING;
    }
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

static int16_t Game_InsertSample(int pending, DIGPAK_SNDSTRUC *sndplay)
{
    Game_sample sample;
    int same_audio_params;
    unsigned int audio_format, audio_channels;
    SDL_AudioCVT cvt;

    if (sndplay->frequency == 0) return 2;

    sample.active = 1;
    sample._stereo = Game_SoundStereo;
    sample._16bit = Game_Sound16bit;
    sample._signed = Game_SoundSigned;
    sample.start = NULL;
    sample.len = sndplay->sndlen;
    sample.len_cvt = 0;
    sample.playback_rate = sndplay->frequency;
    sample.sound = sndplay->sound;
    sample.IsPlaying = sndplay->IsPlaying;

    //if (sample.IsPlaying != NULL) *(sample.IsPlaying) = 0;

    if (Game_Sound16bit)
    {
        audio_format = (Game_SoundSigned)?AUDIO_S16LSB:AUDIO_U16LSB;
    }
    else
    {
        audio_format = (Game_SoundSigned)?AUDIO_S8:AUDIO_U8;
    }

    audio_channels = (Game_SoundStereo)?2:1;

    same_audio_params = 0;
    if (audio_channels == Game_AudioChannels)
    {
        if (sample.playback_rate == Game_AudioRate)
        {
            if (audio_format == Game_AudioFormat)
            {
                same_audio_params = 1;
            }
        }
    }

    if (!same_audio_params)
    {
        int resample_type;

        resample_type = 0;

        if (Game_AudioRate != sample.playback_rate)
        {
            uint32_t newlen_bytes, newlen_samples, form_mult;

#ifdef USE_SPEEXDSP_RESAMPLER
            if (resampler != NULL)
            {
                spx_uint32_t in_len, out_len;
                int16_t *converted_data;

                resample_type = 3;

                newlen_bytes = Get_Resampled_Size(sample._stereo, sample._16bit, sample.playback_rate, Game_AudioRate, sample.len, &newlen_samples);

                form_mult = (sample._16bit) ? 1 : 2;

                SDL_BuildAudioCVT(&cvt, AUDIO_S16LSB, audio_channels, Game_AudioRate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                sample.start = malloc(newlen_bytes * cvt.len_mult * form_mult);

                cvt.buf = (Uint8 *) sample.start;
                cvt.len = newlen_bytes * form_mult;

                if (sample._16bit && sample._signed)
                {
                    converted_data = NULL;
                }
                else
                {
                    converted_data = (int16_t *)malloc(sample.len * (sample._16bit ? 1 : 2));

                    out_len = sample.len;
                    if (sample._16bit)
                    {
                        out_len >>= 1;
                        for (in_len = 0; in_len < out_len; in_len++)
                        {
                            converted_data[in_len] = ((uint16_t *)sample.sound)[in_len] - 32768;
                        }
                    }
                    else if (sample._signed)
                    {
                        for (in_len = 0; in_len < out_len; in_len++)
                        {
                            uint8_t value;
                            value = ((uint8_t *)sample.sound)[in_len];
                            converted_data[in_len] = (value << 8) | (value ^ 0x80);
                        }
                    }
                    else
                    {
                        for (in_len = 0; in_len < out_len; in_len++)
                        {
                            uint8_t value;
                            value = ((uint8_t *)sample.sound)[in_len];
                            converted_data[in_len] = ((value << 8) | value) - 32768;
                        }
                    }
                }

                speex_resampler_set_rate(resampler, sample.playback_rate, Game_AudioRate);

                in_len = sample.len;
                if (sample._stereo) in_len >>= 1;
                if (sample._16bit) in_len >>= 1;
                out_len = newlen_samples;

                if (sample._stereo)
                {
                    speex_resampler_process_interleaved_int(resampler, (converted_data != NULL) ? converted_data : (int16_t *)sample.sound, &in_len, (int16_t *)cvt.buf, &out_len);
                }
                else
                {
                    speex_resampler_process_int(resampler, 0, (converted_data != NULL) ? converted_data : (int16_t *)sample.sound, &in_len, (int16_t *)cvt.buf, &out_len);
                }

                if (converted_data != NULL)
                {
                    free(converted_data);
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

                newlen_bytes = Get_Resampled_Size(sample._stereo, sample._16bit, sample.playback_rate, Game_AudioRate, sample.len, &newlen_samples);

                audio_format = ((!(sample._16bit)) && ((Game_AudioFormat == AUDIO_S8) || (Game_AudioFormat == AUDIO_U8))) ? Game_AudioFormat : AUDIO_S16LSB;

                form_mult = ((!(sample._16bit)) && (Game_AudioFormat != AUDIO_S8) && (Game_AudioFormat != AUDIO_U8)) ? 2 : 1;

                SDL_BuildAudioCVT(&cvt, audio_format, audio_channels, Game_AudioRate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                sample.start = malloc(newlen_bytes * cvt.len_mult * form_mult);

                cvt.buf = (Uint8 *) sample.start;
                cvt.len = newlen_bytes * form_mult;
                Interpolated_Resample(sample._stereo, sample._16bit, sample._signed, sample.playback_rate, Game_AudioRate, (uint8_t *) sample.sound, cvt.buf, sample.len, newlen_samples);
            }
        }

        if (resample_type == 0)
        {
            // sdl resampling

            SDL_BuildAudioCVT(&cvt, audio_format, audio_channels, sample.playback_rate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

            sample.start = malloc(sample.len * cvt.len_mult);

            cvt.buf = (Uint8 *) sample.start;
            cvt.len = sample.len;
            memcpy(cvt.buf, sample.sound, cvt.len);
        }

        SDL_ConvertAudio(&cvt);

        sample.len_cvt = cvt.len_cvt;
    }

    if (sound_initialized)
    {
        SDL_LockMutex(sound_mutex);
    }

    if (pending < 0)
    {
        pending = (Game_samples[0].active) ? 1 : 0;
    }
    else if (pending)
    {
        pending = 1;
    }

    if (Game_samples[pending].start != NULL)
    {
        free(Game_samples[pending].start);
    }

    Game_samples[pending] = sample;

    if (pending)
    {
        Game_AudioPending = 1;
    }
    else if (!sound_initialized)
    {
        Game_PlayAudio();
    }

    if (sound_initialized)
    {
        SDL_UnlockMutex(sound_mutex);
    }

    return pending;
}


int16_t Game_DigPlay(struct _DIGPAK_SNDSTRUC_ *sndplay)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: Playing sound:\n\tsample length: %i\n\tfrequency: %i\n", sndplay->sndlen, sndplay->frequency);
#endif
    if (sound_initialized)
    {
        SDL_LockMutex(sound_mutex);

        if (!Game_samples[0].active)
        {
            SDL_UnlockMutex(sound_mutex);

            Game_InsertSample(0, sndplay);

            return 0; // ???
        }
        else
        {
            SDL_UnlockMutex(sound_mutex);

            return 2; // ???
        }
    }
    else
    {
        if (NOTPLAYING == Game_ProcessAudio())
        {
            Game_InsertSample(0, sndplay);

            return 0; // ???
        }
        else
        {
            return 2; // ???
        }
    }
}

int16_t Game_AudioCapabilities(void)
{
/* Bit flags to denote audio driver capabilities. */
/* returned by the AudioCapabilities call.				*/
#define PLAYBACK 1    // Bit zero true if can play audio in the background.
#define MASSAGE  2    // Bit one is true if data is massaged.
#define FIXEDFREQ 4   // Bit two is true if driver plays at fixed frequency.
#define USESTIMER 8   // Bit three is true, if driver uses timer.
#define SHARESTIMER 16 // Bit 4, timer can be shared
#define STEREOPAN 64	// Bit 6, supports stereo panning.
#define STEREOPLAY 128 // Bit 7, supports 8 bit PCM stereo playback.
#define AUDIORECORD 256 // Bit 8, supports audio recording!
#define DMABACKFILL 512  // Bit 9, support DMA backfilling.
#define PCM16 1024			// Bit 10, supports 16 bit digital audio.
#define PCM16STEREO 2048 // Bit 11, driver support 16 bit digital sound
// All digpak drivers which support 16 bit digital sound ASSUME 16 bit
// SIGNED data, whereas all 8 bit data is assumed UNSIGNED.  This correlates
// exactly to the hardware specifcations for most all DMA driven PC sound
// cards, including SB16, Gravis UltraSound and ProAudio Spectrum 16 sound cards.

/*
DMABACKFILL // is tested
STEREOPLAY // is tested
*/

#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: Reporting capabilities\n");
#endif

    return PLAYBACK | STEREOPAN | STEREOPLAY | PCM16 | PCM16STEREO;
}

void Game_StopSound(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: stopping sound\n");
#endif
    if (sound_initialized)
    {
        SDL_LockMutex(sound_mutex);

        Game_samples[1].active = 0;
        Game_samples[0].active = 0;
        Game_AudioPending = 0;

        SDL_UnlockMutex(sound_mutex);
    }
    else
    {
        if (Mix_Playing(GAME_SOUND_CHANNEL))
        {
            Game_samples[1].active = 0;
            Mix_HaltChannel(GAME_SOUND_CHANNEL);
        }
    }

#ifdef USE_SPEEXDSP_RESAMPLER
    if (resampler != NULL)
    {
        speex_resampler_destroy(resampler);
        resampler = NULL;
    }
#endif
}

int16_t Game_PostAudioPending(struct _DIGPAK_SNDSTRUC_ *sndplay)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: posting audio pending:\n\tsample length: %i\n\tfrequency: %i\n", sndplay->sndlen, sndplay->frequency);
#endif
    if (sound_initialized)
    {
        SDL_LockMutex(sound_mutex);

        if (!Game_samples[1].active)
        {
            SDL_UnlockMutex(sound_mutex);

            return Game_InsertSample(-1, sndplay);
        }
        else
        {
            SDL_UnlockMutex(sound_mutex);

            return 2; // Already a sound effect pending, this one not posted
        }
    }
    else
    {
        switch (Game_ProcessAudio())
        {
            case NOTPLAYING:
                Game_InsertSample(0, sndplay);

                return 0; // Sound was started playing
            case PLAYINGNOTPENDING:
                Game_InsertSample(1, sndplay);

                return 1; // Sound was posted as pending to play
            case PENDINGSOUND:
            default:
                return 2; // Already a sound effect pending, this one not posted
        }
    }
}

int16_t Game_SetPlayMode(int16_t playmode)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: Set Play mode: %i - ", playmode);
    if (playmode == 0) fprintf(stderr, "8-bit PCM\n");
    else if (playmode == 1) fprintf(stderr, "8-bit stereo PCM\n");
    else if (playmode == 2) fprintf(stderr, "16-bit PCM\n");
    else if (playmode == 3) fprintf(stderr, "16-bit stereo PCM\n");
    else fprintf(stderr, "unknown\n");
#endif

#ifdef USE_SPEEXDSP_RESAMPLER
    if (resampler != NULL)
    {
        speex_resampler_destroy(resampler);
        resampler = NULL;
    }
#endif

    switch (playmode)
    {
        case 0:
            Game_SoundStereo = 0;
            Game_Sound16bit = 0;
            Game_SoundSigned = 0;
            break;
        case 1:
            Game_SoundStereo = 1;
            Game_Sound16bit = 0;
            Game_SoundSigned = 0;
            break;
        case 2:
            Game_SoundStereo = 0;
            Game_Sound16bit = 1;
            Game_SoundSigned = 1;
            break;
        case 3:
            Game_SoundStereo = 1;
            Game_Sound16bit = 1;
            Game_SoundSigned = 1;
            break;
        default:
            return 0; // mode not supported by this driver
    }

#ifdef USE_SPEEXDSP_RESAMPLER
    if (Game_ResamplingQuality > 0)
    {
        int err;

        resampler = speex_resampler_init(Game_SoundStereo ? 2 : 1, Game_AudioRate, Game_AudioRate, SPEEX_RESAMPLER_QUALITY_DESKTOP, &err);
        if ((resampler == NULL) || (err != RESAMPLER_ERR_SUCCESS))
        {
            resampler = NULL;
        }
        else
        {
            speex_resampler_skip_zeros(resampler);
        }
    }
#endif

    return 1; // mode set
}

int16_t *Game_PendingAddress(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: reporting pending address\n");
#endif
    return &Game_AudioPending;
}

int16_t *Game_ReportSemaphoreAddress(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: reporting semaphore address\n");
#endif
    return &Game_AudioSemaphore;
}

int16_t Game_SetBackFillMode(int16_t mode)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: DMA BackFill mode: %i - %s\n", mode, (mode)?"off":"on");
#endif
    return 0; // command ignored
}

int16_t Game_VerifyDMA(char *data, int16_t length)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: verifying dma block: %i\n", length);
#endif
    return 1;
}

void Game_SetDPMIMode(int16_t mode)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: Setting DPMI mode: %i - %s\n", mode, (mode)?"32 bit addressing":"16 bit addressing");
#endif
}

int32_t Game_FillSoundCfg(void *buf, int32_t count)
{
    memcpy(buf, &Game_SoundCfg, count);
    return count;
}

uint32_t Game_RealPtr(uint32_t ptr)
{
    return ptr;
}

static void sound_player(void *udata, Uint8 *stream, int len)
{
    Uint8 *abuf;
    void *start;
    int alen, len2;

    Game_AudioSemaphore = 1;

    SDL_LockMutex(sound_mutex);

    if (Game_samples[0].active)
    {
        // play from active sample

        if (Game_samples[0].start == NULL)
        {
            abuf = (Uint8 *) Game_samples[0].sound;
            alen = Game_samples[0].len;
        }
        else
        {
            abuf = (Uint8 *) Game_samples[0].start;
            alen = Game_samples[0].len_cvt;
        }

        len2 = alen - sound_read_index;
        if (len2 > len) len2 = len;

#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_MixAudioFormat(stream, &(abuf[sound_read_index]), Game_AudioFormat, len2, (Game_AudioSampleVolume * Game_AudioMasterVolume) >> 7);
#else
        SDL_MixAudio(stream, &(abuf[sound_read_index]), len2, (Game_AudioSampleVolume * Game_AudioMasterVolume) >> 7);
#endif
        stream += len2;
        len -= len2;

        sound_read_index += len2;
        if (sound_read_index >= alen)
        {
            // active sample finished playing

            Game_samples[0].active = 0;
            sound_read_index = 0;

            if (Game_samples[1].active)
            {
                // switch pending sample to active sample

                start = Game_samples[0].start;
                Game_samples[0] = Game_samples[1];
                Game_samples[1].active = 0;
                Game_samples[1].start = start;

                Game_AudioPending = 0;

                if (len)
                {
                    // play from previously pending sample

                    if (Game_samples[0].start == NULL)
                    {
                        abuf = (Uint8 *) Game_samples[0].sound;
                        alen = Game_samples[0].len;
                    }
                    else
                    {
                        abuf = (Uint8 *) Game_samples[0].start;
                        alen = Game_samples[0].len_cvt;
                    }

                    len2 = (alen <= len) ? alen : len;

#if SDL_VERSION_ATLEAST(2,0,0)
                    SDL_MixAudioFormat(stream, abuf, Game_AudioFormat, len2, (Game_AudioSampleVolume * Game_AudioMasterVolume) >> 7);
#else
                    SDL_MixAudio(stream, abuf, len2, (Game_AudioSampleVolume * Game_AudioMasterVolume) >> 7);
#endif
                    stream += len2;
                    len -= len2;

                    sound_read_index = len2;
                    if (sound_read_index >= alen)
                    {
                        // sample finished playing

                        Game_samples[0].active = 0;
                        sound_read_index = 0;
                    }
                }
            }
        }
    }

    SDL_UnlockMutex(sound_mutex);

    Game_AudioSemaphore = 0;
}

void Game_InitializeSound(void)
{
    sound_mutex = SDL_CreateMutex();
    if (sound_mutex == NULL) return;

    sound_read_index = 0;
    sound_initialized = 1;
    Mix_SetPostMix(&sound_player, NULL);
}

void Game_DeinitializeSound(void)
{
    if (!sound_initialized) return;

    SDL_LockMutex(sound_mutex);

    Game_samples[1].active = 0;
    Game_samples[0].active = 0;
    Game_AudioPending = 0;

    sound_initialized = 0;

    SDL_UnlockMutex(sound_mutex);

    Mix_SetPostMix(NULL, NULL);

    SDL_DestroyMutex(sound_mutex);
    sound_mutex = NULL;
}

