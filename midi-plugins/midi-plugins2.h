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

#if !defined(_MIDI_PLUGINS2_H_INCLUDED_)
#define _MIDI_PLUGINS2_H_INCLUDED_

typedef struct _midi_plugin2_parameters_ {
    char const *midi_device_name;
    unsigned char const *initial_sysex_events;
    unsigned char const *reset_controller_events;
    int midi_type; // 0 = GM, 1 = MT-32, 2 = GM data on MT-32 device
    int mt32_delay;
} midi_plugin2_parameters;

typedef struct _midi_plugin2_functions_ {
    int (*play) (void const *midibuffer, long int size, int loop_count);
    int (*pause) (void);
    int (*resume) (void);
    int (*halt) (void);
    int (*set_volume) (unsigned char volume); // volume = 0 - 127
    int (*set_loop_count) (int loop_count); // -1 = unlimited
    void (*shutdown_plugin) (void);
} midi_plugin2_functions;

typedef int (*midi_plugin2_initialize)(midi_plugin2_parameters const *parameters, midi_plugin2_functions *functions);

#define MIDI_PLUGIN2_INITIALIZE "initialize_midi_plugin2"

#endif /* _MIDI_PLUGINS2_H_INCLUDED_ */

