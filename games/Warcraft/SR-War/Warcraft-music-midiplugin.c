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
    #include <dlfcn.h>
#endif
#define USE_RWOPS
#include "Game_defs.h"
#undef USE_RWOPS
#include "Game_vars.h"
#include "Warcraft-music-midiplugin.h"
#include "xmi2mid.h"
#include "midi-plugins.h"

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
#elif defined(ARMV6) && defined(__ARM_ACLE) && (__ARM_FEATURE_SIMD32 || __ARM_FEATURE_SAT)
#include <arm_acle.h>
#endif


#define BUFFER_SIZE 28672

#define MP_STOPPED 0
#define MP_PLAYING 1
#define MP_PAUSED  2
#define MP_STARTED 3

typedef struct _MP_midi_ {
    Uint16 bytes_left[2], start_index, reserved;
    volatile Uint8 read_buffer, write_buffer, status;
    Uint8 end_of_midi;
    Sint32 volume, loop_count;
    void *midi;
    AIL_sequence *S;
    SDL_sem *sem;
    Uint8 buffer[2][BUFFER_SIZE];
} MP_midi;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    static HMODULE MP_handle;
#else
    static void *MP_handle;
#endif

static midi_plugin_functions MP_functions;

static void *temp_buf;
static int length_multiplier;

static MP_midi MP_sequence[3];

static SDL_Thread *MP_thread;
static volatile int thread_finish;
static int last_volume;


static inline void LockSem(SDL_sem *sem)
{
    while (SDL_SemWait(sem));
}

static void MidiPlugin_MusicPlayer(void *udata, Uint8 *stream, int len)
{
    int max_seq, len2;
    Uint8 *mp_buf;
    char active_seq[3];

#define SEQ_LEN(n) (MP_sequence[n].bytes_left[MP_sequence[n].read_buffer & 1])
#define SEQ_BUF(n) (&(MP_sequence[n].buffer[MP_sequence[n].read_buffer & 1][MP_sequence[n].start_index]))

    mp_buf = (udata != NULL)?((Uint8 *) udata):(stream);
    max_seq = 0;
    len2 = len * length_multiplier;
    active_seq[0] = active_seq[1] = active_seq[2] = 1;

    while (len2)
    {
        int curlen, index;

        curlen = len2;

        for (index = 0; index <= 2; index++)
        {
            if (MP_sequence[index].status == MP_STOPPED)
            {
                MP_sequence[index].start_index = 0;
                MP_sequence[index].read_buffer = MP_sequence[index].write_buffer;
                active_seq[index] = 0;
            }
            else if (active_seq[index])
            {
                if ((MP_sequence[index].read_buffer == MP_sequence[index].write_buffer) ||
                    (MP_sequence[index].status != MP_PLAYING)
                   )
                {
                    active_seq[index] = 0;
                }
            }
        }

        if ((active_seq[0] + active_seq[1] + active_seq[2]) > max_seq) max_seq = (active_seq[0] + active_seq[1] + active_seq[2]);

        switch (active_seq[0] + active_seq[1] + active_seq[2])
        {
            case 0:
                if (max_seq == 0)
                {
                    if ((Game_AudioFormat == AUDIO_S8) || (Game_AudioFormat == AUDIO_S16LSB))
                    {
                        memset(stream, 0, len);
                        return;
                    }
                }

                memset(mp_buf, 0, curlen);

                break;
            case 1:
                {
                    void *src_buf;

                    if (active_seq[0])
                    {
                        if (SEQ_LEN(0) < curlen) curlen = SEQ_LEN(0);
                        src_buf = SEQ_BUF(0);
                    }
                    else if (active_seq[1])
                    {
                        if (SEQ_LEN(1) < curlen) curlen = SEQ_LEN(1);
                        src_buf = SEQ_BUF(1);
                    }
                    else
                    {
                        if (SEQ_LEN(2) < curlen) curlen = SEQ_LEN(2);
                        src_buf = SEQ_BUF(2);
                    }

                    memcpy(mp_buf, src_buf, curlen);
                }
                break;
            case 2:
                {
                    Uint8 *src_buf1, *src_buf2;
                    int pos;

                    if (active_seq[0])
                    {
                        if (SEQ_LEN(0) < curlen) curlen = SEQ_LEN(0);
                        src_buf1 = (Uint8 *) SEQ_BUF(0);

                        if (active_seq[1])
                        {
                            if (SEQ_LEN(1) < curlen) curlen = SEQ_LEN(1);
                            src_buf2 = (Uint8 *) SEQ_BUF(1);
                        }
                        else
                        {
                            if (SEQ_LEN(2) < curlen) curlen = SEQ_LEN(2);
                            src_buf2 = (Uint8 *) SEQ_BUF(2);
                        }
                    }
                    else
                    {
                        if (SEQ_LEN(1) < curlen) curlen = SEQ_LEN(1);
                        if (SEQ_LEN(2) < curlen) curlen = SEQ_LEN(2);
                        src_buf1 = (Uint8 *) SEQ_BUF(1);
                        src_buf2 = (Uint8 *) SEQ_BUF(2);
                    }

                    for (pos = 0; pos < curlen; pos += 4)
                    {
                        #if defined(ARMV8)
                            int16x4_t srcval1, srcval2;
                            uint32x2_t dstval;

                            srcval1 = vreinterpret_s16_u32(vld1_dup_u32((Uint32 *) &(src_buf1[pos])));
                            srcval2 = vreinterpret_s16_u32(vld1_dup_u32((Uint32 *) &(src_buf2[pos])));
                            dstval = vreinterpret_u32_s16(vqadd_s16(srcval1, srcval2));
                            vst1_lane_u32((Uint32 *) &(mp_buf[pos]), dstval, 0);
                        #elif defined(ARMV6)
                            Uint32 srcval1, srcval2;
                            Uint32 dstval;

                            srcval1 = *((Uint32 *) &(src_buf1[pos]));
                            srcval2 = *((Uint32 *) &(src_buf2[pos]));

                        #if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                            dstval = __qadd16(srcval1, srcval2);
                        #else
                            asm ( "qadd16 %[dvalue], %[svalue1], %[svalue2]" : [dvalue] "=r" (dstval) : [svalue1] "r" (srcval1), [svalue2] "r" (srcval2) );
                        #endif

                            *((Uint32 *) &(mp_buf[pos])) = dstval;
                        #else
                            Sint32 val;

                            val = ((Sint32) *((Sint16 *) &(src_buf1[pos]))) + ((Sint32) *((Sint16 *) &(src_buf2[pos])));
                            if (val >= 32768) val = 32767;
                            else if (val < -32768) val = -32768;
                            *((Sint16 *) &(mp_buf[pos])) = val;

                            val = ((Sint32) *((Sint16 *) &(src_buf1[pos + 2]))) + ((Sint32) *((Sint16 *) &(src_buf2[pos + 2])));
                            if (val >= 32768) val = 32767;
                            else if (val < -32768) val = -32768;
                            *((Sint16 *) &(mp_buf[pos + 2])) = val;
                        #endif
                    }
                }

                break;
            case 3:
                {
                    Uint8 *src_buf1, *src_buf2, *src_buf3;
                    int pos;

                    src_buf1 = (Uint8 *) SEQ_BUF(0);
                    src_buf2 = (Uint8 *) SEQ_BUF(1);
                    src_buf3 = (Uint8 *) SEQ_BUF(2);

                    if (SEQ_LEN(0) < curlen) curlen = SEQ_LEN(0);
                    if (SEQ_LEN(1) < curlen) curlen = SEQ_LEN(1);
                    if (SEQ_LEN(2) < curlen) curlen = SEQ_LEN(2);
                    for (pos = 0; pos < curlen; pos += 4)
                    {
                        #if defined(ARMV8)
                            int16x4_t srcval1, srcval2, srcval3;
                            int32x4_t tmpval;
                            uint32x2_t dstval;

                            srcval1 = vreinterpret_s16_u32(vld1_dup_u32((Uint32 *) &(src_buf1[pos])));
                            srcval2 = vreinterpret_s16_u32(vld1_dup_u32((Uint32 *) &(src_buf2[pos])));
                            srcval3 = vreinterpret_s16_u32(vld1_dup_u32((Uint32 *) &(src_buf3[pos])));
                            tmpval = vaddl_s16(srcval1, srcval2);
                            tmpval = vaddw_s16(tmpval, srcval3);
                            dstval = vreinterpret_u32_s16(vqmovn_s32(tmpval));
                            vst1_lane_u32((Uint32 *) &(mp_buf[pos]), dstval, 0);
                        #elif defined(ARMV6)
                            Uint32 srcval1, srcval2, srcval3;
                            Sint32 val1, val2;

                            srcval1 = *((Uint32 *) &(src_buf1[pos]));
                            srcval2 = *((Uint32 *) &(src_buf2[pos]));
                            srcval3 = *((Uint32 *) &(src_buf3[pos]));

                            val1 = ((Sint32)(Sint16)srcval1) + ((Sint32)(Sint16)srcval2) + ((Sint32)(Sint16)srcval3);
                        #if defined(__ARM_ACLE) && __ARM_FEATURE_SAT
                            val1 = __ssat(val1, 16);
                        #else
                            asm ( "ssat %[result], #16, %[value]" : [result] "=r" (val1) : [value] "r" (val1) : "cc" );
                        #endif

                            val2 = ((Sint32)(Sint16)(srcval1 >> 16)) + ((Sint32)(Sint16)(srcval2 >> 16)) + ((Sint32)(Sint16)(srcval3 >> 16));
                        #if defined(__ARM_ACLE) && __ARM_FEATURE_SAT
                            val2 = __ssat(val2, 16);
                        #else
                            asm ( "ssat %[result], #16, %[value]" : [result] "=r" (val2) : [value] "r" (val2) : "cc" );
                        #endif

                            *((Uint32 *) &(mp_buf[pos])) = (val1 & 0xffff) | (val2 << 16);
                        #else
                            Sint32 val;

                            val = ((Sint32) *((Sint16 *) &(src_buf1[pos]))) + ((Sint32) *((Sint16 *) &(src_buf2[pos]))) + ((Sint32) *((Sint16 *) &(src_buf3[pos])));
                            if (val >= 32768) val = 32767;
                            else if (val < -32768) val = -32768;
                            *((Sint16 *) &(mp_buf[pos])) = val;

                            val = ((Sint32) *((Sint16 *) &(src_buf1[pos + 2]))) + ((Sint32) *((Sint16 *) &(src_buf2[pos + 2]))) + ((Sint32) *((Sint16 *) &(src_buf3[pos + 2])));
                            if (val >= 32768) val = 32767;
                            else if (val < -32768) val = -32768;
                            *((Sint16 *) &(mp_buf[pos + 2])) = val;
                        #endif
                    }
                }
                break;
        }

        for (index = 0; index <= 2; index++)
        {
            if (!active_seq[index]) continue;

            if (SEQ_LEN(index) == curlen)
            {
                MP_sequence[index].start_index = 0;
                MP_sequence[index].read_buffer = (MP_sequence[index].read_buffer + 1) & 3;
            }
            else
            {
                MP_sequence[index].start_index += curlen;
                SEQ_LEN(index) -= curlen;
            }
        }

        mp_buf += curlen;
        len2 -= curlen;
    }

    if (udata == NULL)
    {
        if (Game_AudioFormat == AUDIO_U16LSB)
        {
            for (; len != 0; len -= 4)
            {
                #if defined(ARMV8)
                    uint16x4_t srcval1, srcval2;
                    uint32x2_t dstval;

                    srcval1 = vreinterpret_u16_u32(vld1_dup_u32((Uint32 *) stream));
                    srcval2 = vdup_n_u16(0x8000);
                    dstval = vreinterpret_u32_u16(vadd_u16(srcval1, srcval2));
                    vst1_lane_u32((Uint32 *) stream, dstval, 0);
                #elif defined(ARMV6)
                    Uint32 srcval;

                    srcval = *((Uint32 *) stream);
                #if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                    srcval = __uadd16(srcval, 0x80008000);
                #else
                    asm ( "uadd16 %[dvalue], %[value1], %[value2]" : [dvalue] "=r" (srcval) : [value1] "r" (srcval), [value2] "r" (0x80008000) : "cc" );
                #endif
                    *((Uint32 *) stream) = srcval;
                #else
                    *((Uint16 *) stream) = (((Sint32) *((Sint16 *) stream)) + 32768);
                    *((Uint16 *) (stream + 2)) = (((Sint32) *((Sint16 *) (stream + 2))) + 32768);
                #endif
                stream += 4;
            }
        }
    }
    else
    {
        Sint16 *src_buf;

        src_buf = (Sint16 *)udata;

        if (Game_AudioChannels == 2)
        {
            switch (Game_AudioFormat)
            {
                case AUDIO_S8:
                    for (; len != 0; len -= 2)
                    {
                        #if defined(ARMV6) || defined(ARMV8)
                            Uint32 srcval;

                            srcval = *((Uint32 *) src_buf);
                            *((Uint16 *) &(stream[0])) = ((Uint8)(srcval >> 8)) | ( ((Uint32)(Uint8)(srcval >> 24)) << 8 );
                        #else
                            ((Sint8 *) stream)[0] = ((Sint32) src_buf[0]) >> 8;
                            ((Sint8 *) stream)[1] = ((Sint32) src_buf[1]) >> 8;
                        #endif
                        stream += 2;
                        src_buf += 2;
                    }
                    break;
                case AUDIO_U8:
                    for (; len != 0; len -= 2)
                    {
                        #if defined(ARMV6) || defined(ARMV8)
                            Uint32 srcval;

                            srcval = *((Uint32 *) src_buf);
                            *((Uint16 *) &(stream[0])) = ((Uint8)((srcval + 0x8000) >> 8)) | ( ((Uint32)(Uint8)((srcval + 0x80000000) >> 24)) << 8 );
                        #else
                            stream[0] = (((Sint32) src_buf[0]) + 32768) >> 8;
                            stream[1] = (((Sint32) src_buf[1]) + 32768) >> 8;
                        #endif
                        stream += 2;
                        src_buf += 2;
                    }
                    break;
            }
        }
        else
        {
            switch (Game_AudioFormat)
            {
                case AUDIO_S16LSB:
                    for (; len != 0; len -= 2)
                    {
                        #if defined(ARMV8)
                            int16x4_t srcval;
                            int32x2_t tmpval;

                            srcval = vreinterpret_s16_u32(vld1_dup_u32((Uint32 *) src_buf));
                            tmpval = vpaddl_s16(srcval);
                            srcval = vreinterpret_s16_s32(vshr_n_s32(tmpval, 1));
                            vst1_lane_s16((Sint16 *) stream, srcval, 0);
                        #elif defined(ARMV6)
                            Uint32 srcval;

                            srcval = *((Uint32 *) src_buf);
                        #if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
                            srcval = __shadd16(srcval, srcval >> 16);
                        #else
                            asm ( "shadd16 %[dvalue], %[value1], %[value2]" : [dvalue] "=r" (srcval) : [value1] "r" (srcval), [value2] "r" (srcval >> 16) );
                        #endif
                            *((Sint16 *) stream) = srcval;
                        #else
                            *((Sint16 *) stream) = (((Sint32) src_buf[0]) + ((Sint32) src_buf[1])) >> 1;
                        #endif
                        stream += 2;
                        src_buf += 2;
                    }
                    break;
                case AUDIO_U16LSB:
                    for (; len != 0; len -= 2)
                    {
                        #if defined(ARMV6) || defined(ARMV8)
                            Uint32 srcval;

                            srcval = *((Uint32 *) src_buf);
                            *((Uint16 *) stream) = (((Sint32)(Sint16)srcval) + ((Sint32)(Sint16)(srcval >> 16)) + 65536) >> 1;
                        #else
                            *((Uint16 *) stream) = (((Sint32) src_buf[0]) + ((Sint32) src_buf[1]) + 65536) >> 1;
                        #endif
                        stream += 2;
                        src_buf += 2;
                    }
                    break;
                case AUDIO_S8:
                    for (; len != 0; len --)
                    {
                        #if defined(ARMV6) || defined(ARMV8)
                            Uint32 srcval;

                            srcval = *((Uint32 *) src_buf);
                            *((Sint8 *) stream) = (((Sint32)(Sint16)srcval) + ((Sint32)(Sint16)(srcval >> 16))) >> 9;
                        #else
                            *((Sint8 *) stream) = (((Sint32) src_buf[0]) + ((Sint32) src_buf[1])) >> 9;
                        #endif
                        stream ++;
                        src_buf += 2;
                    }
                    break;
                case AUDIO_U8:
                    for (; len != 0; len --)
                    {
                        #if defined(ARMV6) || defined(ARMV8)
                            Uint32 srcval;

                            srcval = *((Uint32 *) src_buf);
                            *stream = (((int32_t)(int16_t)srcval) + ((int32_t)(int16_t)(srcval >> 16)) + 65536) >> 9;
                        #else
                            *stream = (((Sint32) src_buf[0]) + ((Sint32) src_buf[1]) + 65536) >> 9;
                        #endif
                        stream ++;
                        src_buf += 2;
                    }
                    break;
            }
        }
    }

#undef SEQ_BUF
#undef SEQ_LEN
}

static int MidiPlugin_ProcessData(void *data)
{
    while (!thread_finish)
    {
        int need_lock, index;
        char todo[3];

        for (need_lock = 0; need_lock <= 1; need_lock++)
        {
            for (index = 0; index <= 2; index++)
            {
                if (need_lock)
                {
                    if (!todo[index]) continue;

                    todo[index] = (SDL_SemWait(MP_sequence[index].sem))?1:0;
                }
                else
                {
                    todo[index] = (SDL_SemTryWait(MP_sequence[index].sem))?1:0;
                }

                if (todo[index]) continue;

                if ((MP_sequence[index].S != NULL) && (MP_sequence[index].midi != NULL))
                {
                    if (MP_sequence[index].status == MP_PLAYING)
                    {
                        if (MP_sequence[index].end_of_midi)
                        {
                            if (MP_sequence[index].read_buffer == MP_sequence[index].write_buffer)
                            {
                                MP_sequence[index].status = MP_STOPPED;
                            }
                        }
                        else
                        {
                            if ((((MP_sequence[index].read_buffer + 1) & 3) == MP_sequence[index].write_buffer) ||
                                (MP_sequence[index].read_buffer == MP_sequence[index].write_buffer)
                               )
                            {
                                int volume, write_buffer;

                                write_buffer = MP_sequence[index].write_buffer & 1;

                                if (index == 0)
                                {
                                    volume = ( MP_sequence[index].volume * Game_MusicMasterVolume * 517 ) >> 16; /* (volume * Game_MusicMasterVolume) / (127) */
                                }
                                else
                                {
                                    volume = ( MP_sequence[index].volume * Game_SoundMasterVolume * 517 ) >> 16; /* (volume * Game_SoundMasterVolume) / (127) */
                                }

                                if (volume != last_volume)
                                {
                                    last_volume = volume;
                                    MP_functions.set_master_volume(volume);
                                }

                                MP_sequence[index].bytes_left[write_buffer] = MP_functions.get_data(MP_sequence[index].midi, (char *) MP_sequence[index].buffer[write_buffer], BUFFER_SIZE);
                                if (MP_sequence[index].bytes_left[write_buffer] != BUFFER_SIZE)
                                {
                                    MP_functions.rewind_midi(MP_sequence[index].midi);

                                    if (MP_sequence[index].loop_count != 0)
                                    {
                                        MP_sequence[index].loop_count--;
                                        if (MP_sequence[index].loop_count == 0)
                                        {
                                            MP_sequence[index].loop_count = 1;
                                            MP_sequence[index].end_of_midi = 1;
                                        }
                                    }

                                    if (!MP_sequence[index].end_of_midi)
                                    {
                                        int size;

                                        size = MP_functions.get_data(MP_sequence[index].midi, (char *) &(MP_sequence[index].buffer[write_buffer][MP_sequence[index].bytes_left[write_buffer]]), BUFFER_SIZE - MP_sequence[index].bytes_left[write_buffer]);
                                        MP_sequence[index].bytes_left[write_buffer] += size;
                                    }
                                }

                                if (MP_sequence[index].read_buffer == MP_sequence[index].write_buffer)
                                {
                                    MP_sequence[index].start_index = 0;
                                }
                                MP_sequence[index].write_buffer = (MP_sequence[index].write_buffer + 1) & 3;
                            }
                        }
                    }
                    else if (MP_sequence[index].status == MP_STARTED)
                    {
                        MP_sequence[index].end_of_midi = 0;

                        if ((((MP_sequence[index].read_buffer + 1) & 3) == MP_sequence[index].write_buffer) ||
                            (MP_sequence[index].read_buffer == MP_sequence[index].write_buffer)
                           )
                        {
                            int volume, write_buffer;

                            write_buffer = MP_sequence[index].write_buffer & 1;

                            if (index == 0)
                            {
                                volume = ( MP_sequence[index].volume * Game_MusicMasterVolume * 517 ) >> 16; // (volume * Game_MusicMasterVolume) / (127)
                            }
                            else
                            {
                                volume = ( MP_sequence[index].volume * Game_SoundMasterVolume * 517 ) >> 16; // (volume * Game_SoundMasterVolume) / (127)
                            }

                            if (volume != last_volume)
                            {
                                last_volume = volume;
                                MP_functions.set_master_volume(volume);
                            }

                            MP_sequence[index].bytes_left[write_buffer] = MP_functions.get_data(MP_sequence[index].midi, (char *) MP_sequence[index].buffer[write_buffer], BUFFER_SIZE);

                            if (MP_sequence[index].read_buffer == MP_sequence[index].write_buffer)
                            {
                                MP_sequence[index].start_index = 0;
                            }
                            MP_sequence[index].write_buffer = (MP_sequence[index].write_buffer + 1) & 3;
                        }

                        MP_sequence[index].status = MP_PLAYING;
                    }
                }

                SDL_SemPost(MP_sequence[index].sem);
            }
        }

        SDL_Delay(10);
    }

    return 0;
}


int MidiPlugin_Startup(void)
{
    const char *plugin_name;
    midi_plugin_initialize MP_initialize;
    midi_plugin_parameters MP_parameters;
    int index;

    last_volume = -1;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define free_library FreeLibrary
    #define get_proc_address GetProcAddress

    if (Game_MidiSubsystem == 1) plugin_name = ".\\midi-wildmidi.dll";
    else if (Game_MidiSubsystem == 2) plugin_name = ".\\midi-bassmidi.dll";
    else if (Game_MidiSubsystem == 3) plugin_name = ".\\midi-adlmidi.dll";
    else
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "unknown plugin");
        return 1;
    }

    fprintf(stderr, "%s: loading dynamic library: %s\n", "midi", plugin_name);
    MP_handle = LoadLibraryA(plugin_name);

    if (MP_handle == NULL)
    {
        fprintf(stderr, "%s: load error: 0x%x\n", "midi", GetLastError());
        return 2;
    }
#else
    #define free_library dlclose
    #define get_proc_address dlsym

    if (Game_MidiSubsystem == 1) plugin_name = "./midi-wildmidi.so";
    else if (Game_MidiSubsystem == 2) plugin_name = "./midi-bassmidi.so";
    else if (Game_MidiSubsystem == 3) plugin_name = "./midi-adlmidi.so";
    else
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "unknown plugin");
        return 1;
    }

    fprintf(stderr, "%s: loading shared object: %s\n", "midi", plugin_name);
    MP_handle = dlopen(plugin_name, RTLD_LAZY);

    if (MP_handle == NULL)
    {
        fprintf(stderr, "%s: load error: %s\n", "midi", dlerror());
        return 2;
    }
#endif

    MP_initialize = (midi_plugin_initialize) get_proc_address(MP_handle, MIDI_PLUGIN_INITIALIZE);

    if (MP_initialize == NULL)
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "initialization function not available in plugin");
        free_library(MP_handle);
        return 3;
    }

    memset(&MP_parameters, 0, sizeof(MP_parameters));
    MP_parameters.soundfont_path = Game_SoundFontPath;
    MP_parameters.opl3_bank_number = 43; // 43 = AIL (Warcraft)
    MP_parameters.opl3_emulator = Game_OPL3Emulator;
    MP_parameters.resampling_quality = Game_ResamplingQuality;
    MP_parameters.sampling_rate = Game_AudioRate;

    if (MP_initialize(0, &MP_parameters, &MP_functions))
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "failed to initialize plugin");
        free_library(MP_handle);
        return 4;
    }

    // initialize buffers
    for (index = 0; index <= 2; index++)
    {
        memset(&(MP_sequence[index]), 0, (char *) &(MP_sequence[index].buffer) - (char *) &(MP_sequence[index]));
        MP_sequence[index].sem = SDL_CreateSemaphore(1);
    }

    if ((Game_AudioChannels == 2) &&
        ((Game_AudioFormat == AUDIO_U16LSB) ||
         (Game_AudioFormat == AUDIO_S16LSB)
        )
       )
    {
        temp_buf = NULL;
        length_multiplier = 1;
    }
    else
    {
        if (Game_AudioChannels == 2)
        {
            if ((Game_AudioFormat == AUDIO_U8) ||
                (Game_AudioFormat == AUDIO_S8)
               )
            {
                length_multiplier = 2;
                temp_buf = malloc(Game_AudioBufferSize * 2);
            }
            else
            {
                length_multiplier = 1;
                temp_buf = malloc(Game_AudioBufferSize);
            }
        }
        else
        {
            if ((Game_AudioFormat == AUDIO_U8) ||
                (Game_AudioFormat == AUDIO_S8)
               )
            {
                length_multiplier = 4;
                temp_buf = malloc(Game_AudioBufferSize * 4);
            }
            else
            {
                length_multiplier = 2;
                temp_buf = malloc(Game_AudioBufferSize * 2);
            }
        }
    }

    // start thread
    thread_finish = 0;

    MP_thread = SDL_CreateThread(
        MidiPlugin_ProcessData,
#if SDL_VERSION_ATLEAST(2,0,0)
        "midi",
#endif
        NULL
    );
    if (MP_thread == NULL)
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "failed to create thread");
        if (temp_buf != NULL)
        {
            free(temp_buf);
            temp_buf = NULL;
        }
        SDL_DestroySemaphore(MP_sequence[2].sem);
        SDL_DestroySemaphore(MP_sequence[1].sem);
        SDL_DestroySemaphore(MP_sequence[0].sem);
        MP_functions.shutdown_plugin();
        free_library(MP_handle);
        return 5;
    }

    // set mixer
    Mix_HookMusic(&MidiPlugin_MusicPlayer, temp_buf);

    fprintf(stderr, "%s: OK\n", "midi");

    return 0;

#undef get_proc_address
#undef free_library
}

void MidiPlugin_Shutdown(void)
{
    int index;

    // unset mixer
    Mix_PauseMusic();
    Mix_HookMusic(NULL, NULL);

    // stop thread
    thread_finish = 1;
    SDL_SemPost(MP_sequence[0].sem);
    SDL_SemPost(MP_sequence[1].sem);
    SDL_SemPost(MP_sequence[2].sem);
    SDL_WaitThread(MP_thread, NULL);

    // close midi files
    for (index = 0; index <= 2; index++)
    {
        if (MP_sequence[index].midi != NULL)
        {
            MP_functions.close_midi(MP_sequence[index].midi);
            MP_sequence[index].midi = NULL;
        }
        SDL_DestroySemaphore(MP_sequence[index].sem);
    }

    // deallocate buffers
    if (temp_buf != NULL)
    {
        free(temp_buf);
        temp_buf = NULL;
    }

    MP_functions.shutdown_plugin();
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    FreeLibrary(MP_handle);
#else
    dlclose(MP_handle);
#endif
    MP_handle = NULL;
}


void MidiPlugin_AIL_allocate_sequence_handle2(void *mdi, AIL_sequence *S)
{
    int index;

    if (!Game_Music) return;

    for (index = 0; index <= 2; index++)
    {
        if (MP_sequence[index].S == NULL)
        {
            LockSem(MP_sequence[index].sem);

            MP_sequence[index].status = MP_STOPPED;
            MP_sequence[index].end_of_midi = 0;
            MP_sequence[index].start_index = 0;
            MP_sequence[index].midi = NULL;

            MP_sequence[index].S = S;
            S->mp_sequence = &(MP_sequence[index]);

            SDL_SemPost(MP_sequence[index].sem);

            return;
        }
    }
}

void MidiPlugin_AIL_release_sequence_handle(AIL_sequence *S)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        S->status = MP_STOPPED;
        free(S);
        return;
    }

    LockSem(mp_sequence->sem);

    mp_sequence->status = MP_STOPPED;
    mp_sequence->end_of_midi = 0;
    if (mp_sequence->midi != NULL)
    {
        MP_functions.close_midi(mp_sequence->midi);
        mp_sequence->midi = NULL;
    }
    if (S->midi != NULL)
    {
        free(S->midi);
        S->midi = NULL;
    }

    mp_sequence->S = NULL;
    S->mp_sequence = NULL;

    SDL_SemPost(mp_sequence->sem);

    free(S);
}

int32_t MidiPlugin_AIL_init_sequence(AIL_sequence *S, void *start, int32_t sequence_num)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        S->status = MP_STOPPED;
        return 1;
    }

    LockSem(mp_sequence->sem);

    mp_sequence->status = MP_STOPPED;
    mp_sequence->end_of_midi = 0;
    if (mp_sequence->midi != NULL)
    {
        MP_functions.close_midi(mp_sequence->midi);
        mp_sequence->midi = NULL;
    }
    if (S->midi != NULL)
    {
        free(S->midi);
        S->midi = NULL;
    }

    mp_sequence->volume = /*63*/ default_sequence_volume;
    mp_sequence->loop_count = 0;
    S->start = start;
    S->sequence_num = sequence_num;

    S->midi = xmi2mid((uint8_t *) start, sequence_num, &(S->midi_size));

    mp_sequence->midi = MP_functions.open_buffer(S->midi, S->midi_size);

    SDL_SemPost(mp_sequence->sem);

    return 1;
}

void MidiPlugin_AIL_start_sequence(AIL_sequence *S)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        S->status = MP_PLAYING;
        return;
    }

    LockSem(mp_sequence->sem);

    mp_sequence->status = MP_STARTED;

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin_AIL_stop_sequence(AIL_sequence *S)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        if (S->status == MP_PLAYING)
        {
            S->status = MP_PAUSED;
        }
        return;
    }

    LockSem(mp_sequence->sem);

    if (mp_sequence->status == MP_PLAYING)
    {
        mp_sequence->status = MP_PAUSED;
    }
    else if (mp_sequence->status == MP_STARTED)
    {
        mp_sequence->status = MP_STOPPED;
    }

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin_AIL_resume_sequence(AIL_sequence *S)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        S->status = MP_PLAYING;
        return;
    }

    LockSem(mp_sequence->sem);

    if (mp_sequence->status == MP_STOPPED)
    {
        mp_sequence->status = MP_STARTED;
    }
    else if (mp_sequence->status == MP_PAUSED)
    {
        mp_sequence->status = MP_PLAYING;
    }

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin_AIL_end_sequence(AIL_sequence *S)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        S->status = MP_STOPPED;
        return;
    }

    LockSem(mp_sequence->sem);

    mp_sequence->status = MP_STOPPED;

    MP_functions.rewind_midi(mp_sequence->midi);

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin_AIL_set_sequence_volume(AIL_sequence *S, int32_t volume, int32_t ms)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL) return;

    LockSem(mp_sequence->sem);

    mp_sequence->volume = volume;

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin_AIL_set_sequence_loop_count(AIL_sequence *S, int32_t loop_count)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL) return;

    LockSem(mp_sequence->sem);

    mp_sequence->loop_count = loop_count;

    SDL_SemPost(mp_sequence->sem);
}

#define SEQ_FREE          0x0001    // Sequence is available for allocation

#define SEQ_DONE          0x0002    // Sequence has finished playing, or has
                                    // never been started

#define SEQ_PLAYING       0x0004    // Sequence is playing

#define SEQ_STOPPED       0x0008    // Sequence has been stopped

#define SEQ_PLAYINGBUTRELEASED 0x0010 // Sequence is playing, but MIDI handle
                                      // has been temporarily released

uint32_t MidiPlugin_AIL_sequence_status(AIL_sequence *S)
{
    MP_midi *mp_sequence;
    uint32_t ret;

    ret = 0;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        switch (S->status)
        {
            case MP_STOPPED:
                ret = SEQ_DONE;
                break;
            case MP_PLAYING:
            case MP_STARTED:
                ret = SEQ_PLAYING;
                break;
            case MP_PAUSED:
                ret = SEQ_STOPPED;
                break;
        }
        return ret;
    }

    LockSem(mp_sequence->sem);

    switch (mp_sequence->status)
    {
        case MP_STOPPED:
            ret = SEQ_DONE;
            break;
        case MP_PLAYING:
        case MP_STARTED:
            ret = SEQ_PLAYING;
            break;
        case MP_PAUSED:
            ret = SEQ_STOPPED;
            break;
    }

    SDL_SemPost(mp_sequence->sem);

    return ret;
}

