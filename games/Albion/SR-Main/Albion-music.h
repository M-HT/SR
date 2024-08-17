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

#if !defined(_ALBION_MUSIC_H_INCLUDED_)
#define _ALBION_MUSIC_H_INCLUDED_

#define USE_RWOPS
#ifdef USE_SDL2
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL/SDL_mixer.h>
#endif
#undef USE_RWOPS

typedef struct _AIL_sequence
{
    int status;
    void *start;
    int32_t sequence_num;
    int32_t volume;			/* 0-127 */
    int32_t loop_count;
    uint8_t *midi;
    uint32_t midi_size;
    SDL_RWops *midi_RW;
    Mix_Music *midi_music;
    void *mp_sequence;
} AIL_sequence;

#ifdef __cplusplus
extern "C" {
#endif

extern AIL_sequence *Game_AIL_allocate_sequence_handle(void *mdi);
extern void Game_AIL_release_sequence_handle(AIL_sequence *S);
extern int32_t Game_AIL_init_sequence(AIL_sequence *S, void *start, int32_t sequence_num);
extern void Game_AIL_start_sequence(AIL_sequence *S);
extern void Game_AIL_stop_sequence(AIL_sequence *S);
extern void Game_AIL_resume_sequence(AIL_sequence *S);
extern void Game_AIL_end_sequence(AIL_sequence *S);
extern void Game_AIL_set_sequence_volume(AIL_sequence *S, int32_t volume, int32_t ms);
extern void Game_AIL_set_sequence_loop_count(AIL_sequence *S, int32_t loop_count);
extern uint32_t Game_AIL_sequence_status(AIL_sequence *S);
extern void *Game_AIL_create_wave_synthesizer(void *dig, void *mdi, void *wave_lib, int32_t polyphony);
extern void Game_AIL_destroy_wave_synthesizer(void *W);

#ifdef __cplusplus
}
#endif

#endif /* _ALBION_MUSIC_H_INCLUDED_ */
