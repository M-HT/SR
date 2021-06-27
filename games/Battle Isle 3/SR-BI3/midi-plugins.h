/**
 *
 *  Copyright (C) 2016-2018 Roman Pauer
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

#if !defined(_MIDI_PLUGIN_H_INCLUDED_)
#define _MIDI_PLUGIN_H_INCLUDED_

typedef struct _midi_plugin_parameters_ {
    char const *timidity_cfg_path;
    char const *soundfont_path;
    char const *drivers_cat_path;
    char const *mt32_roms_path;
    int opl3_bank_number;
} midi_plugin_parameters;

typedef struct _midi_plugin_functions_ {
    int (*set_master_volume) (unsigned char master_volume); // master_volume = 0 - 127
    void * (*open_file) (char const *midifile);
    void * (*open_buffer) (void const *midibuffer, long int size);
    long int (*get_data) (void *handle, void *buffer, long int size);
    int (*rewind_midi) (void *handle);
    int (*close_midi) (void *handle);
    void (*shutdown_plugin) (void);
} midi_plugin_functions;

typedef int (*midi_plugin_initialize)(unsigned short int rate, midi_plugin_parameters const *parameters, midi_plugin_functions *functions);

#define MIDI_PLUGIN_INITIALIZE "initialize_midi_plugin"

#endif /* _MIDI_PLUGIN_H_INCLUDED_ */

