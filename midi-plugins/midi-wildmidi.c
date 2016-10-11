/**
 *
 *  Copyright (C) 2016 Roman Pauer
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

#if (defined(_WIN32) || defined(__WIN32__) || (__WINDOWS__))
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include <stdlib.h>
#include "midi-plugins.h"
#include "wildmidi_lib.h"


static char const *check_file(char const *filename)
{
    if (filename == NULL) return NULL;
    if (*filename == 0) return NULL;

#if (defined(_WIN32) || defined(__WIN32__) || (__WINDOWS__))
    DWORD dwAttrib;

    dwAttrib = GetFileAttributesA(filename);
    if ((dwAttrib == INVALID_FILE_ATTRIBUTES) || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
#else
    if (access(filename, F_OK))
#endif
    {
        return NULL;
    }
    else
    {
        return filename;
    }
}




static int set_master_volume(unsigned char master_volume) // master_volume = 0 - 127
{
    if (master_volume > 127) master_volume = 127;

    return WildMidi_MasterVolume(master_volume);
}

static void *open_file(char const *midifile)
{
    if (midifile == NULL) return NULL;

    return WildMidi_Open(midifile);
}

static void *open_buffer(void const *midibuffer, long int size)
{
    if (midibuffer == NULL) return NULL;
    if (size <= 0) return NULL;

    return WildMidi_OpenBuffer((unsigned char *)midibuffer, size);
}

static long int get_data(void *handle, void *buffer, long int size)
{
    if (handle == NULL) return -2;
    if (buffer == NULL) return -3;
    if (size < 0) return -4;
    if (size == 0) return 0;

    return WildMidi_GetOutput(handle, buffer, size);
}

static int rewind_midi(void *handle)
{
    unsigned long int sample_pos;

    if (handle == NULL) return -2;

    sample_pos = 0;
    return WildMidi_FastSeek(handle, &sample_pos);
}

static int close_midi(void *handle)
{
    if (handle == NULL) return -2;

    return WildMidi_Close(handle);
}

static void shutdown_plugin(void)
{
    WildMidi_Shutdown();
}


int initialize_midi_plugin(unsigned short int rate, midi_plugin_parameters const *parameters, midi_plugin_functions *functions)
{
    char const *timidity_cfg;

    if ((rate < 11000) || (rate > 65000)) return -2;
    if (functions == NULL) return -3;

    timidity_cfg = NULL;
    if (parameters != NULL)
    {
        timidity_cfg = parameters->timidity_cfg_path;
    }

    if (timidity_cfg != NULL)
    {
        timidity_cfg = check_file(timidity_cfg);
    }
    else
    {
        timidity_cfg = check_file(getenv("TIMIDITY_CFG"));

        if (timidity_cfg == NULL) timidity_cfg = check_file("timidity.cfg");

#if (defined(_WIN32) || defined(__WIN32__) || (__WINDOWS__))
        if (timidity_cfg == NULL) timidity_cfg = check_file("C:\\TIMIDITY\\timidity.cfg");
#else
        if (timidity_cfg == NULL) timidity_cfg = check_file("/etc/timidity/timidity.cfg");
        if (timidity_cfg == NULL) timidity_cfg = check_file("/etc/timidity.cfg");
        if (timidity_cfg == NULL) timidity_cfg = check_file("/usr/share/timidity/timidity.cfg");
        if (timidity_cfg == NULL) timidity_cfg = check_file("/usr/local/lib/timidity/timidity.cfg");
#endif
    }

    if (timidity_cfg == NULL) return -4;

    functions->set_master_volume = &set_master_volume;
    functions->open_file = &open_file;
    functions->open_buffer = &open_buffer;
    functions->get_data = &get_data;
    functions->rewind_midi = &rewind_midi;
    functions->close_midi = &close_midi;
    functions->shutdown_plugin = &shutdown_plugin;

    return WildMidi_Init(timidity_cfg, rate, 0);
}

