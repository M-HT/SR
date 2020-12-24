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

#include <malloc.h>
#include <string.h>
#ifdef USE_SDL2
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL/SDL_mixer.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Geoscape-music.h"
#include "Geoscape-music-midiplugin.h"
#include "Geoscape-music-midiplugin2.h"
#include "gmcat2mid.h"

void Game_SetMusicVolume(void)
{
    int new_volume;

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 20)
        {
            MidiPlugin_SetMusicVolume();
        }
        else
        {
            MidiPlugin2_SetMusicVolume();
        }
        return;
    }

//senquack - SOUND STUFF
//    Mix_VolumeMusic((Game_AudioMasterVolume * Game_MusicSequence.volume) >> 7);
    // Volume's relative loudness is now configurable:
    new_volume = (Game_AudioMasterVolume * Game_MusicSequence.volume) >> 7;
    new_volume = (new_volume * Game_AudioMusicVolume) >> 7;
    Mix_VolumeMusic(new_volume);
}

void Game_start_sequence(uint8_t *seq)
{
    int midi_loop;

    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 20)
        {
            MidiPlugin_start_sequence(seq);
        }
        else
        {
            MidiPlugin2_start_sequence(seq);
        }
        return;
    }

    Mix_HaltMusic();

    if (Game_MusicSequence.midi_music != NULL)
    {
        Mix_FreeMusic(Game_MusicSequence.midi_music);
        Game_MusicSequence.midi_music = NULL;
    }

    if (Game_MusicSequence.midi_RW != NULL)
    {
        SDL_FreeRW(Game_MusicSequence.midi_RW);
        Game_MusicSequence.midi_RW = NULL;
    }

    if (Game_MusicSequence.midi != NULL)
    {
        free(Game_MusicSequence.midi);
        Game_MusicSequence.midi = NULL;
    }

    if (strcmp((const char *)seq, "MIDI") == 0)
    {
        midi_loop = seq[5];
        Game_MusicSequence.midi = NULL;
        Game_MusicSequence.midi_RW = NULL;

        Game_MusicSequence.midi_music = Mix_LoadMUS((char *) &(seq[6]));
        if (Game_MusicSequence.midi_music == NULL) return;
    }
    else
    {
        Game_MusicSequence.midi = gmcat2mid(seq, &Game_MusicSequence.midi_size, &midi_loop);
        if (Game_MusicSequence.midi == NULL) return;

        Game_MusicSequence.midi_RW = SDL_RWFromMem(Game_MusicSequence.midi, Game_MusicSequence.midi_size);
        if (Game_MusicSequence.midi_RW == NULL) return;

        Game_MusicSequence.midi_music = Mix_LoadMUS_RW(
            Game_MusicSequence.midi_RW
#ifdef USE_SDL2
            , 0
#endif
        );
        if (Game_MusicSequence.midi_music == NULL) return;
    }

    Mix_PlayMusic(Game_MusicSequence.midi_music, (midi_loop)?-1:1);
}

void Game_stop_sequence(void)
{
    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 20)
        {
            MidiPlugin_stop_sequence();
        }
        else
        {
            MidiPlugin2_stop_sequence();
        }
        return;
    }

    Mix_HaltMusic();
}

void Game_set_sequence_volume(uint32_t volume)
{
    Game_MusicSequence.volume = volume;

    Game_SetMusicVolume();
}

uint32_t Game_sequence_playing(void)
{
    if (Game_MidiSubsystem)
    {
        if (Game_MidiSubsystem <= 20)
        {
            return MidiPlugin_sequence_playing();
        }
        else
        {
            return MidiPlugin2_sequence_playing();
        }
    }

    return ( Mix_PlayingMusic() )?1:0;
}
