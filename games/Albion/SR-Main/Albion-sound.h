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

#if !defined(_ALBION_SOUND_H_INCLUDED_)
#define _ALBION_SOUND_H_INCLUDED_

struct _AIL_sample;

#ifdef __cplusplus
extern "C" {
#endif

extern struct _AIL_sample *Game_AIL_allocate_sample_handle(void *dig);
extern void Game_AIL_release_sample_handle(struct _AIL_sample *S);
extern void Game_AIL_init_sample(struct _AIL_sample *S);
extern void Game_AIL_set_sample_address(struct _AIL_sample *S, void *start, uint32_t len);
extern void Game_AIL_start_sample(struct _AIL_sample *S);
extern void Game_AIL_end_sample(struct _AIL_sample *S);
extern void Game_AIL_set_sample_loop_count(struct _AIL_sample *S, int32_t loop_count);
extern void Game_AIL_set_sample_pan(struct _AIL_sample *S, int32_t pan);
extern void Game_AIL_set_sample_playback_rate(struct _AIL_sample *S, int32_t playback_rate);
extern void Game_AIL_set_sample_type(struct _AIL_sample *S, int32_t format, uint32_t flags);
extern void Game_AIL_set_sample_volume(struct _AIL_sample *S, int32_t volume);
extern uint32_t Game_AIL_sample_status(struct _AIL_sample *S);
extern void Game_AIL_set_digital_master_volume(void *dig, int32_t master_volume);

#ifdef __cplusplus
}
#endif

#endif /* _ALBION_SOUND_H_INCLUDED_ */
