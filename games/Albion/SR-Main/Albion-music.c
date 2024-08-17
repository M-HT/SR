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
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-music.h"
#include "Albion-music-midiplugin.h"
#include "Albion-music-midiplugin2.h"
#include "Albion-music-xmiplayer.h"
#include "Albion-AIL.h"
#include "xmi2mid.h"

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
#elif defined(ARMV6) && defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
#include <arm_acle.h>
#endif


#define STATUS_STOPPED 0
#define STATUS_PLAYING 1
#define STATUS_PAUSED 2

#if !defined(RW_SEEK_SET)
    #define RW_SEEK_SET	0
#endif
#if !defined(RW_SEEK_CUR)
    #define RW_SEEK_CUR	1
#endif
#if !defined(RW_SEEK_END)
    #define RW_SEEK_END	2
#endif

static AIL_sequence* Game_Sequence[3] = { NULL, NULL, NULL };
static AIL_sequence* Game_ActiveSequence = NULL;    /* active sequence - sequence being played */

static struct _xmi_player *player = NULL;
static SDL_sem *sem1 = NULL;
static int last_volume1;

extern int32_t AIL_preference[];


static void set_volume(int32_t Svolume)
{
    uint32_t volume;

    volume = ( ( (uint32_t) Svolume ) * Game_MusicMasterVolume * 521 ) >> 16; /* (volume * Game_MusicMasterVolume * 128) / (127 * 127) */

    Mix_VolumeMusic(volume);
}

static void release_all(AIL_sequence *S)
{
    if (Game_ActiveSequence == S)
    {
        Mix_HaltMusic();
    }

    S->status = STATUS_STOPPED;

    if (S->midi_music != NULL)
    {
        Mix_FreeMusic(S->midi_music);
        S->midi_music = NULL;
    }

    if (S->midi_RW != NULL)
    {
        SDL_FreeRW(S->midi_RW);
        S->midi_RW = NULL;
    }

    if (S->midi != NULL)
    {
        free(S->midi);
        S->midi = NULL;
    }

}

static inline void LockSem(SDL_sem *sem)
{
    while (SDL_SemWait(sem));
}

static int ActivateSequence(AIL_sequence *S)
{
    if (Game_ActiveSequence != NULL)
    {
        if (Game_Sequence[0] != S)
        {
            if (Game_ActiveSequence->status != STATUS_STOPPED)
            {
                return 0;
            }
        }
    }

    Game_ActiveSequence = S;

    return 1;
}

static void MusicPostMix(void *udata, Uint8 *stream, int len)
{
    int count, volume, index;
    int16_t temp_buf[2*256];

    LockSem(sem1);

    if ((Game_Sequence[1] == NULL) || (Game_Sequence[1]->status != STATUS_PLAYING))
    {
        SDL_SemPost(sem1);
        return;
    }

    volume = ( Game_Sequence[1]->volume * Game_SoundMasterVolume * 521 ) >> 16; // (volume * Game_SoundMasterVolume * 128) / (127 * 127)

    if (volume != last_volume1)
    {
        last_volume1 = volume;
        xmi_player_set_volume(player, volume);
    }

    if (Game_AudioChannels == 2)
    {
        len >>= 1;
    }
    if ((Game_AudioFormat == AUDIO_S16LSB) || (Game_AudioFormat == AUDIO_U16LSB))
    {
        len >>= 1;
    }

    while (len)
    {
        count = len;
        if (count > 256) count = 256;
        len -= count;

        xmi_player_get_data(player, temp_buf, 2 * 256 * sizeof(int16_t));

        if (Game_AudioChannels == 2)
        {
            switch (Game_AudioFormat)
            {
                case AUDIO_S16LSB:
                    for (index = 0; index < count; index++)
                    {
                        #if defined(ARMV8)
                            int16x4_t srcval1, srcval2;
                            uint32x2_t dstval;

                            srcval1 = vreinterpret_s16_u32(vld1_dup_u32((uint32_t *)&(temp_buf[2 * index])));
                            srcval2 = vreinterpret_s16_u32(vld1_dup_u32((uint32_t *)stream));
                            dstval = vreinterpret_u32_s16(vqadd_s16(srcval1, srcval2));
                            vst1_lane_u32((uint32_t *)stream, dstval, 0);
                        #elif defined(ARMV6)
                            uint32_t srcval1, srcval2;
                            uint32_t dstval;

                            srcval1 = ((uint32_t *)temp_buf)[index];
                            srcval2 = *(uint32_t *)stream;

                        #if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                            dstval = __qadd16(srcval1, srcval2);
                        #else
                            asm ( "qadd16 %[dvalue], %[svalue1], %[svalue2]" : [dvalue] "=r" (dstval) : [svalue1] "r" (srcval1), [svalue2] "r" (srcval2) );
                        #endif

                            *(uint32_t *)stream = dstval;
                        #else
                            int32_t val;

                            val = temp_buf[2 * index] + ((int16_t *)stream)[0];
                            if (val >= 32768) val = 32767;
                            else if (val < -32768) val = -32768;
                            ((int16_t *)stream)[0] = val;

                            val = temp_buf[2 * index + 1] + ((int16_t *)stream)[1];
                            if (val >= 32768) val = 32767;
                            else if (val < -32768) val = -32768;
                            ((int16_t *)stream)[1] = val;
                        #endif
                        stream += 4;
                    }
                    break;
                case AUDIO_U16LSB:
                    for (index = 0; index < count; index++)
                    {
                        #if defined(ARMV8)
                            int16x4_t srcval1, srcval2, xorval;
                            uint32x2_t dstval;

                            srcval1 = vreinterpret_s16_u32(vld1_dup_u32((uint32_t *)&(temp_buf[2 * index])));
                            srcval2 = vreinterpret_s16_u32(vld1_dup_u32((uint32_t *)stream));
                            xorval = vdup_n_s16(0x8000);
                            srcval2 = veor_s16(srcval2, xorval);
                            dstval = vreinterpret_u32_s16(vqadd_s16(srcval1, srcval2));
                            dstval = veor_u32(dstval, vreinterpret_u32_s16(xorval));
                            vst1_lane_u32((uint32_t *)stream, dstval, 0);
                        #elif defined(ARMV6)
                            uint32_t srcval1, srcval2;
                            uint32_t dstval;

                            srcval1 = ((uint32_t *)temp_buf)[index];
                            srcval2 = *(uint32_t *)stream ^ 0x80008000UL;

                        #if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                            dstval = __qadd16(srcval1, srcval2);
                        #else
                            asm ( "qadd16 %[dvalue], %[svalue1], %[svalue2]" : [dvalue] "=r" (dstval) : [svalue1] "r" (srcval1), [svalue2] "r" (srcval2) );
                        #endif

                            *(uint32_t *)stream = dstval ^ 0x80008000UL;
                        #else
                            int32_t val;

                            val = temp_buf[2 * index] + ((uint16_t *)stream)[0];
                            if (val >= 65536) val = 65535;
                            else if (val < 0) val = 0;
                            ((uint16_t *)stream)[0] = val;

                            val = temp_buf[2 * index + 1] + ((uint16_t *)stream)[1];
                            if (val >= 65536) val = 65535;
                            else if (val < 0) val = 0;
                            ((uint16_t *)stream)[1] = val;
                        #endif
                        stream += 4;
                    }
                    break;
                case AUDIO_S8:
                    for (index = 0; index < count * 2; index++)
                    {
                        int32_t val;

                        val = (temp_buf[index] >> 8) + *(int8_t *)stream;
                        if (val >= 128) val = 127;
                        else if (val < -128) val = -128;
                        *(int8_t *)stream = val;

                        stream++;
                    }
                    break;
                case AUDIO_U8:
                    for (index = 0; index < count * 2; index++)
                    {
                        int32_t val;

                        val = (temp_buf[index] >> 8) + *(uint8_t *)stream;
                        if (val >= 256) val = 255;
                        else if (val < 0) val = 0;
                        *(uint8_t *)stream = val;

                        stream++;
                    }
                    break;
            }
        }
        else
        {
            switch (Game_AudioFormat)
            {
                case AUDIO_S16LSB:
                    for (index = 0; index < count; index++)
                    {
                        #if defined(ARMV6)
                            uint32_t srcval1, srcval2;
                            uint32_t tmpval, dstval;

                            srcval1 = ((uint32_t *)temp_buf)[index];
                            srcval2 = *(uint16_t *)stream;

                        #if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                            tmpval = __shsax(srcval1, srcval1);
                            dstval = __qadd16(tmpval, srcval2);
                        #else
                            asm ( "shsax %[dvalue], %[svalue1], %[svalue1]" : [dvalue] "=r" (tmpval) : [svalue1] "r" (srcval1) );
                            asm ( "qadd16 %[dvalue], %[svalue1], %[svalue2]" : [dvalue] "=r" (dstval) : [svalue1] "r" (tmpval), [svalue2] "r" (srcval2) );
                        #endif

                            *(uint16_t *)stream = dstval;
                        #else
                            int32_t val;

                            val = ((temp_buf[2 * index] + temp_buf[2 * index + 1]) >> 1) + *(int16_t *)stream;
                            if (val >= 32768) val = 32767;
                            else if (val < -32768) val = -32768;
                            *(int16_t *)stream = val;
                        #endif
                        stream += 2;
                    }
                    break;
                case AUDIO_U16LSB:
                    for (index = 0; index < count; index++)
                    {
                        #if defined(ARMV6)
                            uint32_t srcval1, srcval2;
                            uint32_t tmpval, dstval;

                            srcval1 = ((uint32_t *)temp_buf)[index];
                            srcval2 = *(uint16_t *)stream ^ 0x8000;

                        #if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                            tmpval = __shsax(srcval1, srcval1);
                            dstval = __qadd16(tmpval, srcval2);
                        #else
                            asm ( "shsax %[dvalue], %[svalue1], %[svalue1]" : [dvalue] "=r" (tmpval) : [svalue1] "r" (srcval1) );
                            asm ( "qadd16 %[dvalue], %[svalue1], %[svalue2]" : [dvalue] "=r" (dstval) : [svalue1] "r" (tmpval), [svalue2] "r" (srcval2) );
                        #endif

                            *(uint16_t *)stream = dstval ^ 0x8000;
                        #else
                            int32_t val;

                            val = ((temp_buf[2 * index] + temp_buf[2 * index + 1]) >> 1) + *(uint16_t *)stream;
                            if (val >= 65536) val = 65535;
                            else if (val < 0) val = 0;
                            *(uint16_t *)stream = val;
                        #endif
                        stream += 2;
                    }
                    break;
                case AUDIO_S8:
                    for (index = 0; index < count; index++)
                    {
                        int32_t val;

                        val = ((temp_buf[2 * index] + temp_buf[2 * index + 1]) >> 9) + *(int8_t *)stream;
                        if (val >= 128) val = 127;
                        else if (val < -128) val = -128;
                        *(int8_t *)stream = val;

                        stream++;
                    }
                    break;
                case AUDIO_U8:
                    for (index = 0; index < count; index++)
                    {
                        int32_t val;

                        val = ((temp_buf[2 * index] + temp_buf[2 * index + 1]) >> 9) + *(uint8_t *)stream;
                        if (val >= 256) val = 255;
                        else if (val < 0) val = 0;
                        *(uint8_t *)stream = val;

                        stream++;
                    }
                    break;
            }
        }


        if (!xmi_player_is_playing(player))
        {
            Game_Sequence[1]->status = STATUS_STOPPED;
            break;
        }
    }

    SDL_SemPost(sem1);
}


AIL_sequence *Game_AIL_allocate_sequence_handle(void *mdi)
{
    AIL_sequence *ret;
    int index;

#define ORIG_SEQUENCE_SIZE 2092
#define SEQUENCE_SIZE ( (sizeof(AIL_sequence) > ORIG_SEQUENCE_SIZE)?(sizeof(AIL_sequence)):(ORIG_SEQUENCE_SIZE) )

    ret = (AIL_sequence *) malloc(SEQUENCE_SIZE);

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_allocate_sequence_handle: return: 0x%" PRIxPTR "\n", (uintptr_t) ret);
#endif

    if (ret != NULL)
    {
        memset(ret, 0, SEQUENCE_SIZE);

        if (Game_MidiSubsystem)
        {
            if (Game_MidiSubsystem <= 10)
            {
                MidiPlugin_AIL_allocate_sequence_handle2(mdi, ret);
            }
            else
            {
                MidiPlugin2_AIL_allocate_sequence_handle2(mdi, ret);
            }
        }
        else
        {
            for (index = 0; index <= 2; index++)
            {
                if (Game_Sequence[index] == NULL)
                {
                    Game_Sequence[index] = ret;
                    break;
                }
            }

            if (Game_Sequence[1] == ret)
            {
                if (sem1 == NULL)
                {
                    sem1 = SDL_CreateSemaphore(1);
                }
            }
        }
    }

    return ret;

#undef SEQUENCE_SIZE
#undef ORIG_SEQUENCE_SIZE
}

void Game_AIL_release_sequence_handle(AIL_sequence *S)
{
    int index;

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_release_sequence_handle: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 10)
        {
            MidiPlugin_AIL_release_sequence_handle(S);
        }
        else
        {
            MidiPlugin2_AIL_release_sequence_handle(S);
        }
        return;
    }

    for (index = 0; index <= 2; index++)
    {
        if (Game_Sequence[index] == S)
        {
            if (index == 1)
            {
                LockSem(sem1);
                xmi_player_close(player);
                Mix_SetPostMix(NULL, NULL);
                Game_Sequence[index] = NULL;
                SDL_SemPost(sem1);
            }
            else
            {
                Game_Sequence[index] = NULL;
            }
            break;
        }
    }

    release_all(S);

    if (Game_ActiveSequence == S)
    {
        Game_ActiveSequence = NULL;
    }

    free(S);
}

int32_t Game_AIL_init_sequence(AIL_sequence *S, void *start, int32_t sequence_num)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_init_sequence: 0x%" PRIxPTR ", 0x%" PRIxPTR ", %i\n", (uintptr_t) S, (uintptr_t) start, sequence_num);
#endif

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 10)
        {
            return MidiPlugin_AIL_init_sequence(S, start, sequence_num);
        }
        else
        {
            return MidiPlugin2_AIL_init_sequence(S, start, sequence_num);
        }
    }

    if (Game_Sequence[1] == S)
    {
        LockSem(sem1);
        xmi_player_close(player);
    }

    release_all(S);

    S->volume = AIL_preference[MDI_DEFAULT_VOLUME];
    S->loop_count = 0;
    S->start = start;
    S->sequence_num = sequence_num;

    if (Game_Sequence[1] == S)
    {
        xmi_player_open(player, (uint8_t *) start, sequence_num);
        SDL_SemPost(sem1);
    }
    else
    {
        S->midi = xmi2mid((uint8_t *) start, sequence_num, &(S->midi_size));
    }

    return 1;
}

void Game_AIL_start_sequence(AIL_sequence *S)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_start_sequence: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 10)
        {
            MidiPlugin_AIL_start_sequence(S);
        }
        else
        {
            MidiPlugin2_AIL_start_sequence(S);
        }
        return;
    }

    if (S->status == STATUS_PAUSED)
    {
        if (Game_ActiveSequence == S)
        {
            Game_AIL_resume_sequence(S);
            return;
        }
    }

    if (Game_Sequence[1] == S)
    {
        S->status = STATUS_PLAYING;
        return;
    }
    else if (Game_ActiveSequence != S)
    {
        if ( !ActivateSequence(S) )
        {
            S->status = STATUS_PLAYING;
            return;
        }
    }

    Mix_HaltMusic();

    S->status = STATUS_STOPPED;

    if (S->midi_music != NULL)
    {
        Mix_FreeMusic(S->midi_music);
        S->midi_music = NULL;
    }

    if (S->midi == NULL) return;

    /* set volume */
    set_volume(S->volume);

    if (S->midi_RW == NULL)
    {
        S->midi_RW = SDL_RWFromMem(S->midi, S->midi_size);
        if (S->midi_RW == NULL) return;
    }
    else
    {
        SDL_RWseek(S->midi_RW, 0, RW_SEEK_SET);
    }

    if (S->midi_music == NULL)
    {
        S->midi_music = Mix_LoadMUS_RW(
            S->midi_RW
#if SDL_VERSION_ATLEAST(2,0,0)
            , 0
#endif
        );
        if (S->midi_music == NULL) return;
    }

    Mix_PlayMusic(S->midi_music, -1);

    S->status = STATUS_PLAYING;
}

void Game_AIL_stop_sequence(AIL_sequence *S)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_stop_sequence: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 10)
        {
            MidiPlugin_AIL_stop_sequence(S);
        }
        else
        {
            MidiPlugin2_AIL_stop_sequence(S);
        }
        return;
    }

    if (S->status == STATUS_PLAYING)
    {
        S->status = STATUS_PAUSED;

        if (Game_ActiveSequence == S)
        {
            Mix_PauseMusic();
        }
    }
}

void Game_AIL_resume_sequence(AIL_sequence *S)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_resume_sequence: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 10)
        {
            MidiPlugin_AIL_resume_sequence(S);
        }
        else
        {
            MidiPlugin2_AIL_resume_sequence(S);
        }
        return;
    }

    if (S->status == STATUS_PAUSED)
    {
        if (Game_ActiveSequence == S)
        {
            S->status = STATUS_PLAYING;
            Mix_ResumeMusic();
        }
        else
        {
            if ( (Game_Sequence[1] != S) && ActivateSequence(S) )
            {
                Game_AIL_start_sequence(S);
            }
            else
            {
                S->status = STATUS_PLAYING;
            }
        }
    }
    else if (S->status == STATUS_STOPPED)
    {
        if (Game_ActiveSequence == S)
        {
            Game_AIL_start_sequence(S);
        }
        else
        {
            if ( (Game_Sequence[1] != S) && ActivateSequence(S) )
            {
                Game_AIL_start_sequence(S);
            }
            else
            {
                S->status = STATUS_PLAYING;
            }
        }
    }
    else  if (S->status == STATUS_PLAYING)
    {
        if (Game_ActiveSequence != S)
        {
            if ( (Game_Sequence[1] != S) && ActivateSequence(S) )
            {
                Game_AIL_start_sequence(S);
            }
        }
    }
}

void Game_AIL_end_sequence(AIL_sequence *S)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_end_sequence: 0x%" PRIxPTR "\n", (uintptr_t) S);
#endif

    if (S == NULL) return;

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 10)
        {
            MidiPlugin_AIL_end_sequence(S);
        }
        else
        {
            MidiPlugin2_AIL_end_sequence(S);
        }
        return;
    }

    if (Game_ActiveSequence == S)
    {
        Mix_HaltMusic();
    }

    S->status = STATUS_STOPPED;
}

void Game_AIL_set_sequence_volume(AIL_sequence *S, int32_t volume, int32_t ms)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sequence_volume: 0x%" PRIxPTR ", %i, %i\n", (uintptr_t) S, volume, ms);
#endif

    S->volume = volume; /* 0-127 */

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 10)
        {
            MidiPlugin_AIL_set_sequence_volume(S, volume, ms);
        }
        else
        {
            MidiPlugin2_AIL_set_sequence_volume(S, volume, ms);
        }
        return;
    }

    if ((Game_ActiveSequence == S) &&
        (S->status != STATUS_STOPPED))
    {
        set_volume(volume);
    }
}

void Game_AIL_set_sequence_loop_count(AIL_sequence *S, int32_t loop_count)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_sequence_loop_count: 0x%" PRIxPTR ", %i\n", (uintptr_t) S, loop_count);
#endif

    S->loop_count = loop_count;

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 10)
        {
            MidiPlugin_AIL_set_sequence_loop_count(S, loop_count);
        }
        else
        {
            MidiPlugin2_AIL_set_sequence_loop_count(S, loop_count);
        }
        return;
    }

    if (Game_Sequence[1] == S)
    {
        LockSem(sem1);
        xmi_player_set_loop_count(player, loop_count - 1);
        SDL_SemPost(sem1);
    }
}

#define SEQ_FREE          0x0001    // Sequence is available for allocation

#define SEQ_DONE          0x0002    // Sequence has finished playing, or has
                                    // never been started

#define SEQ_PLAYING       0x0004    // Sequence is playing

#define SEQ_STOPPED       0x0008    // Sequence has been stopped

#define SEQ_PLAYINGBUTRELEASED 0x0010 // Sequence is playing, but MIDI handle
                                      // has been temporarily released

uint32_t Game_AIL_sequence_status(AIL_sequence *S)
{
    if (S == NULL) return SEQ_FREE;

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 10)
        {
            return MidiPlugin_AIL_sequence_status(S);
        }
        else
        {
            return MidiPlugin2_AIL_sequence_status(S);
        }
    }

    switch (S->status)
    {
        case STATUS_STOPPED:
            return SEQ_DONE;
        case STATUS_PLAYING:
            if ( (Game_ActiveSequence != S) || Mix_PlayingMusic() )
            {
                return SEQ_PLAYING;
            }
            else
            {
                return SEQ_DONE;
            }
        case STATUS_PAUSED:
            return SEQ_STOPPED;
        default:
            return SEQ_FREE;
    }
}

void *Game_AIL_create_wave_synthesizer(void *dig, void *mdi, void *wave_lib, int32_t polyphony)
{
    void *ret;

#define WAVE_SYNTH_SIZE 1692

    //ret = malloc(WAVE_SYNTH_SIZE);

    static char wave_synth[WAVE_SYNTH_SIZE];

    ret = &(wave_synth[0]);

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_create_wave_synthesizer: return: 0x%" PRIxPTR "\n", (uintptr_t) ret);
#endif

    if (ret != NULL)
    {
        memset(ret, 0, WAVE_SYNTH_SIZE);

        if (Game_MidiSubsystem)
        {
            if (Game_MidiSubsystem <= 10)
            {
                *(void**)ret = MidiPlugin_AIL_create_wave_synthesizer2(dig, mdi, wave_lib, polyphony);
            }
            else
            {
                *(void**)ret = MidiPlugin2_AIL_create_wave_synthesizer2(dig, mdi, wave_lib, polyphony);
            }
        }
        else
        {
            LockSem(sem1);

            last_volume1 = -1;
            if (player == NULL)
            {
                player = xmi_player_create(Game_AudioRate, wave_lib);
                Mix_SetPostMix(MusicPostMix, NULL);
            }

            SDL_SemPost(sem1);
        }
    }

    return ret;

#undef WAVE_SYNTH_SIZE
}

void Game_AIL_destroy_wave_synthesizer(void *W)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_destroy_wave_synthesizer: 0x%" PRIxPTR "\n", (uintptr_t) W);
#endif

    if (W != NULL)
    {
        if (Game_MidiSubsystem)
        {
            if (Game_MidiSubsystem <= 10)
            {
                MidiPlugin_AIL_destroy_wave_synthesizer2(*(void**)W);
            }
            else
            {
                MidiPlugin2_AIL_destroy_wave_synthesizer2(*(void**)W);
            }
        }
        else
        {
            LockSem(sem1);

            if (player != NULL)
            {
                xmi_player_destroy(player);
                player = NULL;
                Mix_SetPostMix(NULL, NULL);
            }

            SDL_SemPost(sem1);
        }

        //free(W);
    }
}


/*
AIL_allocate_sequence_handle
AIL_create_wave_synthesizer
AIL_destroy_wave_synthesizer
AIL_end_sequence
AIL_init_sequence
AIL_release_sequence_handle
AIL_resume_sequence
AIL_sequence_status
AIL_set_sequence_loop_count
AIL_set_sequence_volume
AIL_start_sequence
AIL_stop_sequence

*/
