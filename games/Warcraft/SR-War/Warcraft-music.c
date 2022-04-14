/**
 *
 *  Copyright (C) 2016-2022 Roman Pauer
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
    #define USE_RWOPS
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL/SDL.h>
    #define USE_RWOPS
    #include <SDL/SDL_mixer.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Warcraft-music.h"
#include "Warcraft-music-midiplugin.h"
#include "Warcraft-music-midiplugin2.h"
#include "xmi2mid.h"

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

static int ActivateSequence(AIL_sequence *S)
{
    if (Game_ActiveSequence != NULL)
    {
        if (Game_MainSequence != S)
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


AIL_sequence *Game_AIL_allocate_sequence_handle(void *mdi)
{
    AIL_sequence *ret;

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
            if (Game_MainSequence == NULL)
            {
                Game_MainSequence = ret;
            }
        }
    }

    return ret;

#undef SEQUENCE_SIZE
#undef ORIG_SEQUENCE_SIZE
}

void Game_AIL_release_sequence_handle(AIL_sequence *S)
{
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

    if (Game_MainSequence == S)
    {
        Game_MainSequence = NULL;
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

    release_all(S);

    S->volume = /*63*/ default_sequence_volume;
    S->loop_count = 0;
    S->start = start;
    S->sequence_num = sequence_num;

    S->midi = xmi2mid((uint8_t *) start, sequence_num, &(S->midi_size));

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

    if (Game_ActiveSequence != S)
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
#ifdef USE_SDL2
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
            if ( ActivateSequence(S) )
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
            if ( ActivateSequence(S) )
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
            if ( ActivateSequence(S) )
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
            if ( Mix_PlayingMusic() )
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

#define WAVE_SYNTH_SIZE 1948

    //ret = malloc(WAVE_SYNTH_SIZE);

    static char wave_synth[WAVE_SYNTH_SIZE];

    ret = &(wave_synth[0]);

#if defined(__DEBUG__)
    fprintf(stderr, "AIL_create_wave_synthesizer: return: 0x%" PRIxPTR "\n", (uintptr_t) ret);
#endif

    if (ret != NULL)
    {
        memset(ret, 0, WAVE_SYNTH_SIZE);
    }

    return ret;

#undef WAVE_SYNTH_SIZE
}

void Game_AIL_destroy_wave_synthesizer(void *W)
{
    if (W != NULL)
    {
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
