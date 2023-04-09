/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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
#ifdef USE_SDL2
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL/SDL_mixer.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Intro-sound.h"

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

    Mix_PlayChannel(GAME_SOUND_CHANNEL, &Game_AudioChunk, 0);

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

static void Resample(int _stereo, int _16bit, uint32_t src_rate, uint32_t dst_rate, uint8_t *srcbuf, uint8_t *dstbuf, uint32_t src_size)
{
    if (src_rate <= dst_rate)
    {
        uint32_t delta, curpos, dst_size;

        curpos = 0;
        delta = (((uint64_t) src_rate) << 31) / dst_rate;

        if (!_stereo && !_16bit)
        {
            // byte samples
            uint8_t *src, *dst;

            dst_size = ( ((uint64_t) src_size) * dst_rate ) / src_rate;

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

            dst_size = ( ((uint64_t) (src_size / 2)) * dst_rate ) / src_rate;

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

            dst_size = ( ((uint64_t) (src_size / 4)) * dst_rate ) / src_rate;

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
        uint32_t delta_frac, delta_int, curpos, dst_size;

        curpos = 0;
        delta_int  = ( (((uint64_t) src_rate) << 31) / dst_rate ) >> 31;
        delta_frac = ( (((uint64_t) src_rate) << 31) / dst_rate ) & 0x7fffffff;

        if (!_stereo && !_16bit)
        {
            // byte samples
            uint8_t *src, *dst;

            dst_size = ( ((uint64_t) src_size) * dst_rate ) / src_rate;

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

            dst_size = ( ((uint64_t) (src_size/2)) * dst_rate ) / src_rate;

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

            dst_size = ( ((uint64_t) (src_size/4)) * dst_rate ) / src_rate;

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

    dst_lastsize = (((dst_rate << 16) / src_rate) + 0xffff) >> 16;
    dst_size -= dst_lastsize;
    src_delta = (src_rate << 16) / dst_rate;
    if (src_rate > dst_rate)
    {
        src_pos = ((src_rate - dst_rate) << 16) / src_rate;
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

static void Game_InsertSample(int pending, DIGPAK_SNDSTRUC *sndplay)
{
    Game_sample *sample;
    int same_audio_params;
    unsigned int audio_format, audio_channels;
    SDL_AudioCVT cvt;

    if (sndplay->frequency == 0) return;

    sample = &(Game_samples[(pending)?1:0]);

    if (sample->start != NULL)
    {
        free(sample->start);
        sample->start = NULL;
    }

    sample->active = 1;
    sample->_stereo = Game_SoundStereo;
    sample->_16bit = Game_Sound16bit;
    sample->_signed = Game_SoundSigned;
    sample->len = sndplay->sndlen;
    sample->playback_rate = sndplay->frequency;
    sample->sound = sndplay->sound;
    sample->IsPlaying = sndplay->IsPlaying;

    //if (sample->IsPlaying != NULL) *(sample->IsPlaying) = 0;

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
        if (sample->playback_rate == Game_AudioRate)
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

        if (Game_InterpolateAudio)
        {
#if !defined(USE_SDL2)
            if (Game_AudioRate != sample->playback_rate)
            {
                // interpolated resampling
                uint32_t newlen_bytes, newlen_samples, form_mult;

                resample_type = 2;

                newlen_bytes = Get_Resampled_Size(sample->_stereo, sample->_16bit, sample->playback_rate, Game_AudioRate, sample->len, &newlen_samples);

                if ((!(sample->_16bit)) && ((Game_AudioFormat == AUDIO_S8) || (Game_AudioFormat == AUDIO_U8)))
                {
                    audio_format = Game_AudioFormat;
                }
                else
                {
                    audio_format = AUDIO_S16LSB;
                }

                if ((!(sample->_16bit)) && (Game_AudioFormat != AUDIO_S8) && (Game_AudioFormat != AUDIO_U8))
                {
                    form_mult = 2;
                }
                else
                {
                    form_mult = 1;
                }

                SDL_BuildAudioCVT(&cvt, audio_format, audio_channels, Game_AudioRate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                sample->start = malloc(newlen_bytes * cvt.len_mult * form_mult);

                cvt.buf = (Uint8 *) sample->start;
                cvt.len = newlen_bytes * form_mult;
                Interpolated_Resample(sample->_stereo, sample->_16bit, sample->_signed, sample->playback_rate, Game_AudioRate, (uint8_t *) sample->sound, cvt.buf, sample->len, newlen_samples);
            }
#endif
        }
        else
        {
            // SDL1 supports resampling only when higher frequency equals lower frequency multiplied by power of 2,
            // so I'm using my resampling when it's not the case
            // or when the target frequency is lower than the source frequency to use less memory
#if !defined(USE_SDL2)
            if (Game_AudioRate < sample->playback_rate ||
                (Is_MulPow2(Game_AudioRate, sample->playback_rate) == 0)
               )
#endif
            {
                // near-neighbour resampling
                uint32_t newlen_bytes, newlen_samples;

                resample_type = 1;

                newlen_bytes = Get_Resampled_Size(sample->_stereo, sample->_16bit, sample->playback_rate, Game_AudioRate, sample->len, &newlen_samples);

                SDL_BuildAudioCVT(&cvt, audio_format, audio_channels, Game_AudioRate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

                sample->start = malloc(newlen_bytes * cvt.len_mult);

                cvt.buf = (Uint8 *) sample->start;
                cvt.len = newlen_bytes;
                Resample(sample->_stereo, sample->_16bit, sample->playback_rate, Game_AudioRate, (uint8_t *) sample->sound, cvt.buf, newlen_samples);
            }
        }

        if (resample_type == 0)
        {
            // sdl resampling

            SDL_BuildAudioCVT(&cvt, audio_format, audio_channels, sample->playback_rate, Game_AudioFormat, Game_AudioChannels, Game_AudioRate);

            sample->start = malloc(sample->len * cvt.len_mult);

            cvt.buf = (Uint8 *) sample->start;
            cvt.len = sample->len;
            memcpy(cvt.buf, sample->sound, cvt.len);
        }

        SDL_ConvertAudio(&cvt);

        sample->len_cvt = cvt.len_cvt;
    }

    if (pending)
    {
        Game_AudioPending = 1;
    }
    else
    {
        Game_PlayAudio();
    }
}


int16_t Game_DigPlay(DIGPAK_SNDSTRUC *sndplay)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: Playing sound:\n\tsample length: %i\n\tfrequency: %i\n", sndplay->sndlen, sndplay->frequency);
#endif
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
    if (Mix_Playing(GAME_SOUND_CHANNEL))
    {
        Game_samples[1].active = 0;
        Mix_HaltChannel(GAME_SOUND_CHANNEL);
    }
}

int16_t Game_PostAudioPending(DIGPAK_SNDSTRUC *sndplay)
{
#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: posting audio pending:\n\tsample length: %i\n\tfrequency: %i\n", sndplay->sndlen, sndplay->frequency);
#endif
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

int16_t Game_SetPlayMode(int32_t playmode)
{
    playmode = (int16_t)playmode;

#if defined(__DEBUG__)
    fprintf(stderr, "DIGPAK: Set Play mode: %i - ", playmode);
    if (playmode == 0) fprintf(stderr, "8-bit PCM\n");
    else if (playmode == 1) fprintf(stderr, "8-bit stereo PCM\n");
    else if (playmode == 2) fprintf(stderr, "16-bit PCM\n");
    else if (playmode == 3) fprintf(stderr, "16-bit stereo PCM\n");
    else fprintf(stderr, "unknown\n");
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

int16_t Game_SetBackFillMode(int32_t mode)
{
#if defined(__DEBUG__)
    mode = (int16_t)mode;
    fprintf(stderr, "DIGPAK: DMA BackFill mode: %i - %s\n", mode, (mode)?"off":"on");
#endif
    return 0; // command ignored
}

int16_t Game_VerifyDMA(char *data, int32_t length)
{
#if defined(__DEBUG__)
    length = (int16_t)length;
    fprintf(stderr, "DIGPAK: verifying dma block: %i\n", length);
#endif
    return 1;
}

void Game_SetDPMIMode(int32_t mode)
{
#if defined(__DEBUG__)
    mode = (int16_t)mode;
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
