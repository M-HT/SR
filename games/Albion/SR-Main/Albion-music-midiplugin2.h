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

#if !defined(_ALBION_MUSIC_MIDIPLUGIN2_H_INCLUDED_)
#define _ALBION_MUSIC_MIDIPLUGIN2_H_INCLUDED_

#include "Albion-music.h"

int MidiPlugin2_Startup(void);
void MidiPlugin2_Restore(void);
void MidiPlugin2_Shutdown(void);

void MidiPlugin2_AIL_allocate_sequence_handle2(void *mdi, AIL_sequence *S);
void MidiPlugin2_AIL_release_sequence_handle(AIL_sequence *S);
int32_t MidiPlugin2_AIL_init_sequence(AIL_sequence *S, void *start, int32_t sequence_num);
void MidiPlugin2_AIL_start_sequence(AIL_sequence *S);
void MidiPlugin2_AIL_stop_sequence(AIL_sequence *S);
void MidiPlugin2_AIL_resume_sequence(AIL_sequence *S);
void MidiPlugin2_AIL_end_sequence(AIL_sequence *S);
void MidiPlugin2_AIL_set_sequence_volume(AIL_sequence *S, int32_t volume, int32_t ms);
void MidiPlugin2_AIL_set_sequence_loop_count(AIL_sequence *S, int32_t loop_count);
uint32_t MidiPlugin2_AIL_sequence_status(AIL_sequence *S);
void *MidiPlugin2_AIL_create_wave_synthesizer2(void *dig, void *mdi, void *wave_lib, int32_t polyphony);
void MidiPlugin2_AIL_destroy_wave_synthesizer2(void *W);

#endif /* _ALBION_MUSIC_MIDIPLUGIN2_H_INCLUDED_ */

