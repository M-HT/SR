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

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
#else
    #include <dlfcn.h>
#endif
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
    #define USE_RWOPS
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL/SDL.h>
    #define USE_RWOPS
    #include <SDL/SDL_mixer.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Xcom-music-midiplugin.h"
#include "gmcat2mid.h"
#include "midi-plugins.h"
#include "virtualfs.h"

#if ( \
    defined(__ARM_ARCH_6__) || \
    defined(__ARM_ARCH_6J__) || \
    defined(__ARM_ARCH_6K__) || \
    defined(__ARM_ARCH_6Z__) || \
    defined(__ARM_ARCH_6ZK__) || \
    defined(__ARM_ARCH_6T2__) || \
    defined(__ARM_ARCH_7__) || \
    defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7R__) || \
    defined(__ARM_ARCH_7M__) || \
    defined(__ARM_ARCH_7S__) || \
    (defined(_M_ARM) && (_M_ARM >= 6)) || \
    (defined(__TARGET_ARCH_ARM) && (__TARGET_ARCH_ARM >= 6)) || \
    (defined(__TARGET_ARCH_THUMB) && (__TARGET_ARCH_THUMB >= 3)) \
)
    #define ARMV6 1
#else
    #undef ARMV6
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
    Sint32 loop_count;
    void *midi;
    SDL_sem *sem;
    Uint32 reserved2, reserved3;
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

static MP_midi MP_sequence;

static SDL_Thread *MP_thread;
static volatile int thread_finish;


static inline void LockSem(SDL_sem *sem)
{
    while (SDL_SemWait(sem));
}

static void MidiPlugin_MusicPlayer(void *udata, Uint8 *stream, int len)
{
    int max_seq, len2;
    Uint8 *mp_buf;
    int active_seq;

#define SEQ_LEN (MP_sequence.bytes_left[MP_sequence.read_buffer & 1])
#define SEQ_BUF (&(MP_sequence.buffer[MP_sequence.read_buffer & 1][MP_sequence.start_index]))

    mp_buf = (udata != NULL)?((Uint8 *) udata):(stream);
    max_seq = 0;
    len2 = len * length_multiplier;
    active_seq = 1;

    while (len2)
    {
        int curlen;

        curlen = len2;

        if (MP_sequence.status == MP_STOPPED)
        {
            MP_sequence.start_index = 0;
            MP_sequence.read_buffer = MP_sequence.write_buffer;
            active_seq = 0;
        }
        else if (active_seq)
        {
            if ((MP_sequence.read_buffer == MP_sequence.write_buffer) ||
                (MP_sequence.status != MP_PLAYING)
               )
            {
                active_seq = 0;
            }
        }

        if (active_seq > max_seq) max_seq = active_seq;

        switch (active_seq)
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
                if (SEQ_LEN < curlen) curlen = SEQ_LEN;

                memcpy(mp_buf, SEQ_BUF, curlen);

                break;
        }

        if (active_seq)
        {
            if (SEQ_LEN == curlen)
            {
                MP_sequence.start_index = 0;
                MP_sequence.read_buffer = (MP_sequence.read_buffer + 1) & 3;
            }
            else
            {
                MP_sequence.start_index += curlen;
                SEQ_LEN -= curlen;
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
                #if defined(ARMV6)
                    register Uint32 srcval;

                    srcval = *((Uint32 *) stream);
                    asm("uadd16 %[value1], %[value1], %[value2]" : [value1] "+r" (srcval) : [value2] "r" (0x80008000));
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
                        #if defined(ARMV6)
                            register Uint32 srcval;

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
                        #if defined(ARMV6)
                            register Uint32 srcval;

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
                        #if defined(ARMV6)
                            register Uint32 srcval;

                            srcval = *((Uint32 *) src_buf);
                            asm("shadd16 %[value1], %[value1], %[value2]" : [value1] "+r" (srcval) : [value2] "r" (srcval >> 16));
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
                        #if defined(ARMV6)
                            register Uint32 srcval;

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
                        #if defined(ARMV6)
                            register Uint32 srcval;

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
                        #if defined(ARMV6)
                            register Uint32 srcval;

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
        if (SDL_SemWait(MP_sequence.sem) == 0)
        {
            if (MP_sequence.midi != NULL)
            {
                if (MP_sequence.status == MP_PLAYING)
                {
                    if (MP_sequence.end_of_midi)
                    {
                        if (MP_sequence.read_buffer == MP_sequence.write_buffer)
                        {
                            MP_sequence.status = MP_STOPPED;
                        }
                    }
                    else
                    {
                        if ((((MP_sequence.read_buffer + 1) & 3) == MP_sequence.write_buffer) ||
                            (MP_sequence.read_buffer == MP_sequence.write_buffer)
                           )
                        {
                            int write_buffer;

                            write_buffer = MP_sequence.write_buffer & 1;

                            MP_sequence.bytes_left[write_buffer] = MP_functions.get_data(MP_sequence.midi, (char *) MP_sequence.buffer[write_buffer], BUFFER_SIZE);
                            if (MP_sequence.bytes_left[write_buffer] != BUFFER_SIZE)
                            {
                                MP_functions.rewind_midi(MP_sequence.midi);

                                if (MP_sequence.loop_count != 0)
                                {
                                    MP_sequence.loop_count--;
                                    if (MP_sequence.loop_count == 0)
                                    {
                                        MP_sequence.loop_count = 1;
                                        MP_sequence.end_of_midi = 1;
                                    }
                                }

                                if (!MP_sequence.end_of_midi)
                                {
                                    int size;

                                    size = MP_functions.get_data(MP_sequence.midi, (char *) &(MP_sequence.buffer[write_buffer][MP_sequence.bytes_left[write_buffer]]), BUFFER_SIZE - MP_sequence.bytes_left[write_buffer]);
                                    MP_sequence.bytes_left[write_buffer] += size;
                                }
                            }

                            if (MP_sequence.read_buffer == MP_sequence.write_buffer)
                            {
                                MP_sequence.start_index = 0;
                            }
                            MP_sequence.write_buffer = (MP_sequence.write_buffer + 1) & 3;
                        }
                    }
                }
                else if (MP_sequence.status == MP_STARTED)
                {
                    MP_sequence.end_of_midi = 0;

                    if ((((MP_sequence.read_buffer + 1) & 3) == MP_sequence.write_buffer) ||
                        (MP_sequence.read_buffer == MP_sequence.write_buffer)
                       )
                    {
                        int write_buffer;

                        write_buffer = MP_sequence.write_buffer & 1;

                        MP_sequence.bytes_left[write_buffer] = MP_functions.get_data(MP_sequence.midi, (char *) MP_sequence.buffer[write_buffer], BUFFER_SIZE);

                        if (MP_sequence.read_buffer == MP_sequence.write_buffer)
                        {
                            MP_sequence.start_index = 0;
                        }
                        MP_sequence.write_buffer = (MP_sequence.write_buffer + 1) & 3;
                    }

                    MP_sequence.status = MP_PLAYING;
                }
            }

            SDL_SemPost(MP_sequence.sem);
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
    char temp_str[MAX_PATH];

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define free_library FreeLibrary
    #define get_proc_address GetProcAddress

    if (Game_MidiSubsystem == 1) plugin_name = ".\\midi-wildmidi.dll";
    else if (Game_MidiSubsystem == 2) plugin_name = ".\\midi-bassmidi.dll";
    else if (Game_MidiSubsystem == 3) plugin_name = ".\\midi-adlmidi.dll";
    else if (Game_MidiSubsystem == 10) plugin_name = ".\\adlib-dosbox_opl.dll";
    else if (Game_MidiSubsystem == 11) plugin_name = ".\\mt32-munt.dll";
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
    else if (Game_MidiSubsystem == 10) plugin_name = "./adlib-dosbox_opl.so";
    else if (Game_MidiSubsystem == 11) plugin_name = "./mt32-munt.so";
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
    MP_parameters.mt32_roms_path = Game_MT32RomsPath;
    MP_parameters.opl3_bank_number = Game_OPL3BankNumber;
    MP_parameters.opl3_emulator = Game_OPL3Emulator;

    vfs_get_real_name("C:\\SOUND\\DRIVERS.CAT", (char *) &temp_str, NULL);
    MP_parameters.drivers_cat_path = (char *) &temp_str;

    if (MP_initialize(Game_AudioRate, &MP_parameters, &MP_functions))
    {
        fprintf(stderr, "%s: error: %s\n", "midi", "failed to initialize plugin");
        free_library(MP_handle);
        return 4;
    }

    // initialize buffers
    memset(&(MP_sequence), 0, (char *) &(MP_sequence.buffer) - (char *) &(MP_sequence));
    MP_sequence.sem = SDL_CreateSemaphore(1);

    MidiPlugin_SetMusicVolume();

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
#ifdef USE_SDL2
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
        SDL_DestroySemaphore(MP_sequence.sem);
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
    // unset mixer
    Mix_PauseMusic();
    Mix_HookMusic(NULL, NULL);

    // stop thread
    thread_finish = 1;
    SDL_SemPost(MP_sequence.sem);
    SDL_WaitThread(MP_thread, NULL);

    // close midi files
    if (MP_sequence.midi != NULL)
    {
        MP_functions.close_midi(MP_sequence.midi);
        MP_sequence.midi = NULL;
    }
    SDL_DestroySemaphore(MP_sequence.sem);

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


void MidiPlugin_SetMusicVolume(void)
{
    int new_volume;

    if (Game_Music)
    {
        new_volume = (Game_AudioMasterVolume * Game_MusicSequence.volume * Game_AudioMusicVolume * 127) >> 21;

        LockSem(MP_sequence.sem);

        MP_functions.set_master_volume(new_volume);

        SDL_SemPost(MP_sequence.sem);
    }
}

void MidiPlugin_start_sequence(uint8_t *seq)
{
    int midi_loop;

    if (Game_Music)
    {
        LockSem(MP_sequence.sem);

        MP_sequence.status = MP_STOPPED;
        MP_sequence.end_of_midi = 0;
        if (MP_sequence.midi != NULL)
        {
            MP_functions.close_midi(MP_sequence.midi);
            MP_sequence.midi = NULL;
        }
        if (Game_MusicSequence.midi != NULL)
        {
            free(Game_MusicSequence.midi);
            Game_MusicSequence.midi = NULL;
        }

        if (Game_MidiSubsystem >= 10) {
            Game_MusicSequence.midi = NULL;

            MP_sequence.loop_count = 1;

            MP_sequence.midi = MP_functions.open_buffer(seq, 33000);
        }
        else if (strcmp((const char *)seq, "MIDI") == 0)
        {
            midi_loop = seq[5];
            Game_MusicSequence.midi = NULL;

            MP_sequence.loop_count = (midi_loop)?0:1;

            MP_sequence.midi = MP_functions.open_file((char *) &(seq[6]));
        }
        else
        {
            Game_MusicSequence.midi = gmcat2mid(seq, &Game_MusicSequence.midi_size, &midi_loop);
            if (Game_MusicSequence.midi == NULL) return;

            MP_sequence.loop_count = (midi_loop)?0:1;

            MP_sequence.midi = MP_functions.open_buffer(Game_MusicSequence.midi, Game_MusicSequence.midi_size);
        }

        MP_sequence.status = MP_STARTED;

        SDL_SemPost(MP_sequence.sem);
    } else {
        MP_sequence.status = MP_STARTED;
    }
}

void MidiPlugin_stop_sequence(void)
{
    if (Game_Music)
    {
        LockSem(MP_sequence.sem);

        MP_sequence.status = MP_STOPPED;

        if (MP_sequence.midi != NULL)
        {
            MP_functions.rewind_midi(MP_sequence.midi);
        }

        SDL_SemPost(MP_sequence.sem);
    }
    else
    {
        MP_sequence.status = MP_STOPPED;
    }
}

uint32_t MidiPlugin_sequence_playing(void)
{
    uint32_t ret;

    if (Game_Music)
    {
        LockSem(MP_sequence.sem);
    }

    switch (MP_sequence.status)
    {
        case MP_PLAYING:
        case MP_STARTED:
            ret = 1;
            break;
        default:
            ret = 0;
            break;
    }

    if (Game_Music)
    {
        SDL_SemPost(MP_sequence.sem);
    }

    return ret;
}

