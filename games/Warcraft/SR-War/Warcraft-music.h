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

#if !defined(_WARCRAFT_MUSIC_H_INCLUDED_)
#define _WARCRAFT_MUSIC_H_INCLUDED_

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

#ifdef __cplusplus
}
#endif

#endif /* _WARCRAFT_MUSIC_H_INCLUDED_ */
