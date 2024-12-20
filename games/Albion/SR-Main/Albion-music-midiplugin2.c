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
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-music-midiplugin2.h"
#include "Albion-music-xmiplayer.h"
#include "xmi2mid.h"
#include "midi-plugins.h"
#include "midi-plugins2.h"

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
    int miditype;
    void *midi;
    AIL_sequence *S;
    SDL_sem *sem;
    Uint8 buffer[2][BUFFER_SIZE];
} MP_midi;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    static HMODULE MP2_handle;
#else
    static void *MP2_handle;
#endif

static midi_plugin2_functions MP2_functions;

static struct _xmi_player *player = NULL;

static void *temp_buf;
static int length_multiplier;

static MP_midi MP_sequence[3];
static MP_midi* ActiveSequence = NULL;    /* active sequence - sequence being played */

static SDL_Thread *MP_thread;
static volatile int thread_finish;
static int last_volume1;


static inline void LockSem(SDL_sem *sem)
{
    while (SDL_SemWait(sem));
}

static int ActivateSequence(MP_midi *seq)
{
    if (ActiveSequence != NULL)
    {
        if (&(MP_sequence[0]) != seq)
        {
            if (ActiveSequence->status != MP_STOPPED)
            {
                return 0;
            }
        }
    }

    ActiveSequence = seq;

    return 1;
}

static void MidiPlugin2_MusicPlayer(void *udata, Uint8 *stream, int len)
{
    int max_seq, len2;
    Uint8 *mp_buf;
    int active_seq1;

#define SEQ_LEN(n) (MP_sequence[n].bytes_left[MP_sequence[n].read_buffer & 1])
#define SEQ_BUF(n) (&(MP_sequence[n].buffer[MP_sequence[n].read_buffer & 1][MP_sequence[n].start_index]))

    mp_buf = (udata != NULL)?((Uint8 *) udata):(stream);
    max_seq = 0;
    len2 = len * length_multiplier;
    active_seq1 = 1;

    while (len2)
    {
        int curlen;

        curlen = len2;

        if (MP_sequence[1].status == MP_STOPPED)
        {
            MP_sequence[1].start_index = 0;
            MP_sequence[1].read_buffer = MP_sequence[1].write_buffer;
            active_seq1 = 0;
        }
        else if (active_seq1)
        {
            if ((MP_sequence[1].read_buffer == MP_sequence[1].write_buffer) ||
                (MP_sequence[1].status != MP_PLAYING)
               )
            {
                active_seq1 = 0;
            }
        }

        if (active_seq1 > max_seq) max_seq = active_seq1;

        switch (active_seq1)
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
                if (SEQ_LEN(1) < curlen) curlen = SEQ_LEN(1);

                memcpy(mp_buf, SEQ_BUF(1), curlen);

                break;
        }

        if (active_seq1)
        {
            if (SEQ_LEN(1) == curlen)
            {
                MP_sequence[1].start_index = 0;
                MP_sequence[1].read_buffer = (MP_sequence[1].read_buffer + 1) & 3;
            }
            else
            {
                MP_sequence[1].start_index += curlen;
                SEQ_LEN(1) -= curlen;
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
                    *((uint32_t *) stream) = srcval;
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

static int MidiPlugin2_ProcessData(void *data)
{
    while (!thread_finish)
    {
        if (SDL_SemWait(MP_sequence[1].sem) == 0)
        {
            if (MP_sequence[1].S != NULL)
            {
                if (MP_sequence[1].status == MP_PLAYING)
                {
                    if (MP_sequence[1].end_of_midi)
                    {
                        if (MP_sequence[1].read_buffer == MP_sequence[1].write_buffer)
                        {
                            MP_sequence[1].status = MP_STOPPED;
                        }
                    }
                    else
                    {
                        if ((((MP_sequence[1].read_buffer + 1) & 3) == MP_sequence[1].write_buffer) ||
                            (MP_sequence[1].read_buffer == MP_sequence[1].write_buffer)
                           )
                        {
                            int volume, write_buffer;

                            write_buffer = MP_sequence[1].write_buffer & 1;

                            volume = ( MP_sequence[1].volume * Game_SoundMasterVolume * 521 ) >> 16; /* (volume * Game_SoundMasterVolume * 128) / (127 * 127) */

                            if (volume != last_volume1)
                            {
                                last_volume1 = volume;
                                xmi_player_set_volume(player, volume);
                            }

                            xmi_player_get_data(player, MP_sequence[1].buffer[write_buffer], BUFFER_SIZE);
                            MP_sequence[1].bytes_left[write_buffer] = BUFFER_SIZE;

                            if (!xmi_player_is_playing(player))
                            {
                                MP_sequence[1].end_of_midi = 1;
                            }

                            if (MP_sequence[1].read_buffer == MP_sequence[1].write_buffer)
                            {
                                MP_sequence[1].start_index = 0;
                            }
                            MP_sequence[1].write_buffer = (MP_sequence[1].write_buffer + 1) & 3;
                        }
                    }
                }
                else if (MP_sequence[1].status == MP_STARTED)
                {
                    MP_sequence[1].end_of_midi = 0;

                    if ((((MP_sequence[1].read_buffer + 1) & 3) == MP_sequence[1].write_buffer) ||
                        (MP_sequence[1].read_buffer == MP_sequence[1].write_buffer)
                       )
                    {
                        int volume, write_buffer;

                        write_buffer = MP_sequence[1].write_buffer & 1;

                        volume = ( MP_sequence[1].volume * Game_SoundMasterVolume * 521 ) >> 16; // (volume * Game_SoundMasterVolume * 128) / (127 * 127)

                        if (volume != last_volume1)
                        {
                            last_volume1 = volume;
                            xmi_player_set_volume(player, volume);
                        }

                        xmi_player_get_data(player, MP_sequence[1].buffer[write_buffer], BUFFER_SIZE);
                        MP_sequence[1].bytes_left[write_buffer] = BUFFER_SIZE;

                        if (!xmi_player_is_playing(player))
                        {
                            MP_sequence[1].end_of_midi = 1;
                        }

                        if (MP_sequence[1].read_buffer == MP_sequence[1].write_buffer)
                        {
                            MP_sequence[1].start_index = 0;
                        }
                        MP_sequence[1].write_buffer = (MP_sequence[1].write_buffer + 1) & 3;
                    }

                    MP_sequence[1].status = MP_PLAYING;
                }
            }

            SDL_SemPost(MP_sequence[1].sem);
        }

        SDL_Delay(10);
    }

    return 0;
}


int MidiPlugin2_Startup(void)
{
    const char *plugin_name;
    midi_plugin2_initialize MP2_initialize;
    midi_plugin2_parameters MP2_parameters;
    int index;

    last_volume1 = -1;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define free_library FreeLibrary
    #define get_proc_address GetProcAddress

    if (Game_MidiSubsystem == 11 || Game_MidiSubsystem == 21) plugin_name = ".\\midi2-windows.dll";
    else if (Game_MidiSubsystem == 12 || Game_MidiSubsystem == 22) plugin_name = ".\\midi2-alsa.dll";
    else
    {
        fprintf(stderr, "%s: error: %s\n", "midi2", "unknown plugin");
        return 1;
    }

    fprintf(stderr, "%s: loading dynamic library: %s\n", "midi2", plugin_name);
    MP2_handle = LoadLibraryA(plugin_name);

    if (MP2_handle == NULL)
    {
        fprintf(stderr, "%s: load error: 0x%x\n", "midi2", GetLastError());
        return 2;
    }
#else
    #define free_library dlclose
    #define get_proc_address dlsym

    if (Game_MidiSubsystem == 11 || Game_MidiSubsystem == 21) plugin_name = "./midi2-windows.so";
    else if (Game_MidiSubsystem == 12 || Game_MidiSubsystem == 22) plugin_name = "./midi2-alsa.so";
    else
    {
        fprintf(stderr, "%s: error: %s\n", "midi2", "unknown plugin");
        return 1;
    }

    fprintf(stderr, "%s: loading shared object: %s\n", "midi2", plugin_name);
    MP2_handle = dlopen(plugin_name, RTLD_LAZY);

    if (MP2_handle == NULL)
    {
        fprintf(stderr, "%s: load error: %s\n", "midi2", dlerror());
        return 2;
    }
#endif

    MP2_initialize = (midi_plugin2_initialize) get_proc_address(MP2_handle, MIDI_PLUGIN2_INITIALIZE);

    if (MP2_initialize == NULL)
    {
        fprintf(stderr, "%s: error: %s\n", "midi2", "initialization function not available in plugin");
        free_library(MP2_handle);
        return 3;
    }

    memset(&MP2_parameters, 0, sizeof(MP2_parameters));
    MP2_parameters.midi_device_name = Game_MidiDevice;
    if (Game_MidiSubsystem > 20)
    {
        MP2_parameters.midi_type = 2;
    }
    MP2_parameters.mt32_delay = Game_MT32DelaySysex;
    MP2_parameters.mt32_display_text = "  A  L  B  I  O  N";

    if (MP2_initialize(&MP2_parameters, &MP2_functions))
    {
        fprintf(stderr, "%s: error: %s\n", "midi2", "failed to initialize plugin");
        free_library(MP2_handle);
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
        MidiPlugin2_ProcessData,
#if SDL_VERSION_ATLEAST(2,0,0)
        "midi2",
#endif
        NULL
    );
    if (MP_thread == NULL)
    {
        fprintf(stderr, "%s: error: %s\n", "midi2", "failed to create thread");
        if (temp_buf != NULL)
        {
            free(temp_buf);
            temp_buf = NULL;
        }
        SDL_DestroySemaphore(MP_sequence[2].sem);
        SDL_DestroySemaphore(MP_sequence[1].sem);
        SDL_DestroySemaphore(MP_sequence[0].sem);
        MP2_functions.shutdown_plugin();
        free_library(MP2_handle);
        return 5;
    }

    // set mixer
    Mix_HookMusic(&MidiPlugin2_MusicPlayer, temp_buf);

    fprintf(stderr, "%s: OK\n", "midi2");

    return 0;

#undef get_proc_address
#undef free_library
}

void MidiPlugin2_Restore(void)
{
    if (MP_thread != NULL)
    {
        // set mixer
        Mix_HookMusic(&MidiPlugin2_MusicPlayer, temp_buf);
    }
}

void MidiPlugin2_Shutdown(void)
{
    int index;

    MP2_functions.halt();

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
        if (MP_sequence[index].miditype)
        {
            xmi_player_close(player);
        }
        SDL_DestroySemaphore(MP_sequence[index].sem);
    }

    // deallocate buffers
    if (temp_buf != NULL)
    {
        free(temp_buf);
        temp_buf = NULL;
    }

    MP2_functions.shutdown_plugin();
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    FreeLibrary(MP2_handle);
#else
    dlclose(MP2_handle);
#endif
    MP2_handle = NULL;
}


void MidiPlugin2_AIL_allocate_sequence_handle2(void *mdi, AIL_sequence *S)
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
            MP_sequence[index].miditype = (index == 1)?1:0;
            MP_sequence[index].midi = NULL;

            MP_sequence[index].S = S;
            S->mp_sequence = &(MP_sequence[index]);

            SDL_SemPost(MP_sequence[index].sem);

            return;
        }
    }
}

void MidiPlugin2_AIL_release_sequence_handle(AIL_sequence *S)
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
    if (mp_sequence->miditype)
    {
        xmi_player_close(player);
    }
    else
    {
        if (ActiveSequence == mp_sequence)
        {
            MP2_functions.halt();
            ActiveSequence = NULL;
        }
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

int32_t MidiPlugin2_AIL_init_sequence(AIL_sequence *S, void *start, int32_t sequence_num)
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
    if (mp_sequence->miditype)
    {
        xmi_player_close(player);
    }
    else
    {
        if (ActiveSequence == mp_sequence)
        {
            MP2_functions.halt();
        }
    }
    if (S->midi != NULL)
    {
        free(S->midi);
        S->midi = NULL;
    }

    mp_sequence->volume = 0;
    mp_sequence->loop_count = 0;
    S->start = start;
    S->sequence_num = sequence_num;

    if (mp_sequence->miditype)
    {
        xmi_player_open(player, (uint8_t *) start, sequence_num);
    }
    else
    {
        S->midi = xmi2mid((uint8_t *) start, sequence_num, &(S->midi_size));
    }

    SDL_SemPost(mp_sequence->sem);

    return 1;
}

void MidiPlugin2_AIL_start_sequence(AIL_sequence *S)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        S->status = MP_PLAYING;
        return;
    }

    LockSem(mp_sequence->sem);

    if (mp_sequence->miditype)
    {
        mp_sequence->status = MP_STARTED;
    }
    else
    {
        if (mp_sequence->status == MP_PAUSED)
        {
            if (ActiveSequence == mp_sequence)
            {
                SDL_SemPost(mp_sequence->sem);
                MidiPlugin2_AIL_resume_sequence(S);
                return;
            }
        }

        if (ActiveSequence != mp_sequence)
        {
            if ( !ActivateSequence(mp_sequence) )
            {
                mp_sequence->status = MP_PLAYING;
                SDL_SemPost(mp_sequence->sem);
                return;
            }
        }

        MP2_functions.halt();
        mp_sequence->status = MP_STOPPED;

        if (S->midi != NULL)
        {
            /* set volume */
            MP2_functions.set_volume(( mp_sequence->volume * Game_MusicMasterVolume * 517 ) >> 16); /* (volume * Game_MusicMasterVolume) / (127) */

            MP2_functions.play(S->midi, S->midi_size, mp_sequence->loop_count - 1);

            mp_sequence->status = MP_PLAYING;
        }
    }

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin2_AIL_stop_sequence(AIL_sequence *S)
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

    if (mp_sequence->miditype)
    {
        if (mp_sequence->status == MP_PLAYING)
        {
            mp_sequence->status = MP_PAUSED;
        }
        else if (mp_sequence->status == MP_STARTED)
        {
            mp_sequence->status = MP_STOPPED;
        }
    }
    else
    {
        if (mp_sequence->status == MP_PLAYING)
        {
            mp_sequence->status = MP_PAUSED;

            if (ActiveSequence == mp_sequence)
            {
                MP2_functions.pause();
            }
        }
    }

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin2_AIL_resume_sequence(AIL_sequence *S)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        S->status = MP_PLAYING;
        return;
    }

    LockSem(mp_sequence->sem);

    if (mp_sequence->miditype)
    {
        if (mp_sequence->status == MP_STOPPED)
        {
            mp_sequence->status = MP_STARTED;
        }
        else if (mp_sequence->status == MP_PAUSED)
        {
            mp_sequence->status = MP_PLAYING;
        }
    }
    else
    {
        if (mp_sequence->status == MP_PAUSED)
        {
            if (ActiveSequence == mp_sequence)
            {
                mp_sequence->status = MP_PLAYING;
                MP2_functions.resume();
            }
            else
            {
                if ( ActivateSequence(mp_sequence) )
                {
                    SDL_SemPost(mp_sequence->sem);
                    MidiPlugin2_AIL_start_sequence(S);
                    return;
                }
                else
                {
                    mp_sequence->status = MP_PLAYING;
                }
            }
        }
        else if (mp_sequence->status == MP_STOPPED)
        {
            if (ActiveSequence == mp_sequence)
            {
                SDL_SemPost(mp_sequence->sem);
                MidiPlugin2_AIL_start_sequence(S);
                return;
            }
            else
            {
                if ( ActivateSequence(mp_sequence) )
                {
                    SDL_SemPost(mp_sequence->sem);
                    MidiPlugin2_AIL_start_sequence(S);
                    return;
                }
                else
                {
                    mp_sequence->status = MP_PLAYING;
                }
            }
        }
        else  if (mp_sequence->status == MP_PLAYING)
        {
            if (ActiveSequence != mp_sequence)
            {
                if ( ActivateSequence(mp_sequence) )
                {
                    SDL_SemPost(mp_sequence->sem);
                    MidiPlugin2_AIL_start_sequence(S);
                    return;
                }
            }
        }
    }

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin2_AIL_end_sequence(AIL_sequence *S)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL)
    {
        S->status = MP_STOPPED;
        return;
    }

    LockSem(mp_sequence->sem);

    if (mp_sequence->miditype)
    {
        mp_sequence->status = MP_STOPPED;
    }
    else
    {
        if (ActiveSequence == mp_sequence)
        {
            MP2_functions.halt();
        }

        mp_sequence->status = MP_STOPPED;
    }

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin2_AIL_set_sequence_volume(AIL_sequence *S, int32_t volume, int32_t ms)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL) return;

    LockSem(mp_sequence->sem);

    mp_sequence->volume = volume;

    if (!mp_sequence->miditype)
    {
        if ((ActiveSequence == mp_sequence) &&
            (mp_sequence->status != MP_STOPPED))
        {
            MP2_functions.set_volume(( mp_sequence->volume * Game_MusicMasterVolume * 517 ) >> 16); /* (volume * Game_MusicMasterVolume) / (127) */
        }
    }

    SDL_SemPost(mp_sequence->sem);
}

void MidiPlugin2_AIL_set_sequence_loop_count(AIL_sequence *S, int32_t loop_count)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL) return;

    LockSem(mp_sequence->sem);

    mp_sequence->loop_count = loop_count;

    if (mp_sequence->miditype)
    {
        xmi_player_set_loop_count(player, mp_sequence->loop_count - 1);
    }
    else
    {
        if ((ActiveSequence == mp_sequence) &&
            (mp_sequence->status != MP_STOPPED))
        {
            MP2_functions.set_loop_count(mp_sequence->loop_count - 1);
        }
    }

    SDL_SemPost(mp_sequence->sem);
}

#define SEQ_FREE          0x0001    // Sequence is available for allocation

#define SEQ_DONE          0x0002    // Sequence has finished playing, or has
                                    // never been started

#define SEQ_PLAYING       0x0004    // Sequence is playing

#define SEQ_STOPPED       0x0008    // Sequence has been stopped

#define SEQ_PLAYINGBUTRELEASED 0x0010 // Sequence is playing, but MIDI handle
                                      // has been temporarily released

uint32_t MidiPlugin2_AIL_sequence_status(AIL_sequence *S)
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

    return ret;
}

void *MidiPlugin2_AIL_create_wave_synthesizer2(void *dig, void *mdi, void *wave_lib, int32_t polyphony)
{
    LockSem(MP_sequence[1].sem);
    last_volume1 = -1;
    if (player == NULL)
    {
        MP_sequence[1].status = MP_STOPPED;
        MP_sequence[1].loop_count = 1;
        MP_sequence[1].end_of_midi = 1;

        player = xmi_player_create(Game_AudioRate, wave_lib);
    }
    SDL_SemPost(MP_sequence[1].sem);

    return player;
}

void MidiPlugin2_AIL_destroy_wave_synthesizer2(void *W)
{
    LockSem(MP_sequence[1].sem);
    MP_sequence[1].status = MP_STOPPED;
    MP_sequence[1].loop_count = 1;
    MP_sequence[1].end_of_midi = 1;

    if (player != NULL)
    {
        xmi_player_destroy(player);
        player = NULL;
    }

    SDL_SemPost(MP_sequence[1].sem);
}

