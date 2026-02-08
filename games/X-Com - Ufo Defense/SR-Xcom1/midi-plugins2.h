/**
 *
 *  Copyright (C) 2016-2026 Roman Pauer
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

#if defined(__GNUC__) && (defined(__i386) || (defined(__x86_64) && defined(_WIN32)))
    #define MIDI_PLUGIN2_API __attribute__ ((__cdecl__))
#elif defined(_MSC_VER)
    #define MIDI_PLUGIN2_API __cdecl
#else
    #define MIDI_PLUGIN2_API
#endif

typedef struct _midi_plugin2_parameters_ {
    char const *midi_device_name;
    unsigned char const *initial_sysex_events;
    unsigned char const *reset_controller_events;
    int midi_type; // 0 = GM, 1 = MT-32, 2 = GM data on MT-32 device
    int mt32_delay;
    char const *mt32_display_text;
} midi_plugin2_parameters;

typedef struct _midi_plugin2_functions_ {
    int (MIDI_PLUGIN2_API *play) (void const *midibuffer, long int size, int loop_count);
    int (MIDI_PLUGIN2_API *pause) (void);
    int (MIDI_PLUGIN2_API *resume) (void);
    int (MIDI_PLUGIN2_API *halt) (void);
    int (MIDI_PLUGIN2_API *set_volume) (unsigned char volume); // volume = 0 - 127
    int (MIDI_PLUGIN2_API *set_loop_count) (int loop_count); // -1 = unlimited
    void (MIDI_PLUGIN2_API *shutdown_plugin) (void);
} midi_plugin2_functions;

typedef int (MIDI_PLUGIN2_API *midi_plugin2_initialize)(midi_plugin2_parameters const *parameters, midi_plugin2_functions *functions);

#define MIDI_PLUGIN2_INITIALIZE "initialize_midi_plugin2"

#endif /* _MIDI_PLUGINS2_H_INCLUDED_ */

