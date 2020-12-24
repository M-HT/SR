/**
 *
 *  Copyright (C) 2016-2020 Roman Pauer
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
#include "Warcraft-music-midiplugin2.h"
#include "xmi2mid.h"
#include "midi-plugins2.h"



#define MP_STOPPED 0
#define MP_PLAYING 1
#define MP_PAUSED  2
#define MP_STARTED 3

typedef struct _MP_midi_ {
    Uint8 status, reserved[3];
    Sint32 volume, loop_count;
    AIL_sequence *S;
} MP_midi;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    static HMODULE MP2_handle;
#else
    static void *MP2_handle;
#endif

static midi_plugin2_functions MP2_functions;

static MP_midi MP_sequence[3];
static MP_midi* ActiveSequence = NULL;    /* active sequence - sequence being played */


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


int MidiPlugin2_Startup(void)
{
    midi_plugin2_initialize MP2_initialize;
    midi_plugin2_parameters MP2_parameters;
    int index;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    if (Game_MidiSubsystem == 11) {
        MP2_handle = LoadLibrary(".\\midi2-windows.dll");
    } else if (Game_MidiSubsystem == 12) {
        MP2_handle = LoadLibrary(".\\midi2-alsa.dll");
    } else return 1;

    #define free_library FreeLibrary
    #define get_proc_address GetProcAddress
#else
    if (Game_MidiSubsystem == 11) {
        MP2_handle = dlopen("./midi2-windows.so", RTLD_LAZY);
    } else if (Game_MidiSubsystem == 12) {
        MP2_handle = dlopen("./midi2-alsa.so", RTLD_LAZY);
    } else return 1;

    #define free_library dlclose
    #define get_proc_address dlsym
#endif
    if (MP2_handle == NULL)
    {
        return 2;
    }

    MP2_initialize = (midi_plugin2_initialize) get_proc_address(MP2_handle, MIDI_PLUGIN2_INITIALIZE);

    if (MP2_initialize == NULL)
    {
        free_library(MP2_handle);
        return 3;
    }

    memset(&MP2_parameters, 0, sizeof(MP2_parameters));
    MP2_parameters.midi_device_name = Game_MidiDevice;

    if (MP2_initialize(&MP2_parameters, &MP2_functions))
    {
        free_library(MP2_handle);
        return 4;
    }

    // initialize buffers
    for (index = 0; index <= 2; index++)
    {
        memset(&(MP_sequence[index]), 0, sizeof(MP_midi));
    }

    return 0;

#undef get_proc_address
#undef free_library
}

void MidiPlugin2_Shutdown(void)
{
    MP2_functions.halt();

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
            MP_sequence[index].status = MP_STOPPED;

            MP_sequence[index].S = S;
            S->mp_sequence = &(MP_sequence[index]);

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

    mp_sequence->status = MP_STOPPED;
    if (ActiveSequence == mp_sequence)
    {
        MP2_functions.halt();
        ActiveSequence = NULL;
    }
    if (S->midi != NULL)
    {
        free(S->midi);
        S->midi = NULL;
    }

    mp_sequence->S = NULL;
    S->mp_sequence = NULL;

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

    mp_sequence->status = MP_STOPPED;
    if (ActiveSequence == mp_sequence)
    {
        MP2_functions.halt();
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

    if (mp_sequence->status == MP_PAUSED)
    {
        if (ActiveSequence == mp_sequence)
        {
            MidiPlugin2_AIL_resume_sequence(S);
            return;
        }
    }

    if (ActiveSequence != mp_sequence)
    {
        if ( !ActivateSequence(mp_sequence) )
        {
            mp_sequence->status = MP_PLAYING;
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

    if (mp_sequence->status == MP_PLAYING)
    {
        mp_sequence->status = MP_PAUSED;

        if (ActiveSequence == mp_sequence)
        {
            MP2_functions.pause();
        }
    }
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
            MidiPlugin2_AIL_start_sequence(S);
            return;
        }
        else
        {
            if ( ActivateSequence(mp_sequence) )
            {
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
                MidiPlugin2_AIL_start_sequence(S);
                return;
            }
        }
    }
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

    if (ActiveSequence == mp_sequence)
    {
        MP2_functions.halt();
    }

    mp_sequence->status = MP_STOPPED;
}

void MidiPlugin2_AIL_set_sequence_volume(AIL_sequence *S, int32_t volume, int32_t ms)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL) return;

    mp_sequence->volume = volume;

    if ((ActiveSequence == mp_sequence) &&
        (mp_sequence->status != MP_STOPPED))
    {
        MP2_functions.set_volume(( mp_sequence->volume * Game_MusicMasterVolume * 517 ) >> 16); /* (volume * Game_MusicMasterVolume) / (127) */
    }
}

void MidiPlugin2_AIL_set_sequence_loop_count(AIL_sequence *S, int32_t loop_count)
{
    MP_midi *mp_sequence;

    mp_sequence = (MP_midi *) S->mp_sequence;
    if (mp_sequence == NULL) return;

    mp_sequence->loop_count = loop_count;

    if ((ActiveSequence == mp_sequence) &&
        (mp_sequence->status != MP_STOPPED))
    {
        MP2_functions.set_loop_count(mp_sequence->loop_count - 1);
    }
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

