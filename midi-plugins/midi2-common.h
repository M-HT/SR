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

typedef struct {
    const uint8_t *ptr;
    unsigned int len, delta;
    uint8_t event_status;
    int eot;
} midi_track_info;

static unsigned char *initial_sysex_events = NULL;
static unsigned char *reset_controller_events = NULL;
static int mt32_delay;

static volatile int midi_quit = 0;
static volatile int midi_loaded = 0;
static volatile int midi_playing = 0;
static volatile int midi_current_volume = 127;
static volatile int midi_new_volume = 127;
static volatile int midi_loop_count = 0;
static volatile int midi_eof = 0;

#if !defined(MIDI_CHANNELS)
#define MIDI_CHANNELS 16
#endif

static int channel_volume[MIDI_CHANNELS];
static int channel_notes[MIDI_CHANNELS][128];

#define GETU32FBE(buf) (                    \
            (uint32_t) ( (buf)[0] ) << 24 | \
            (uint32_t) ( (buf)[1] ) << 16 | \
            (uint32_t) ( (buf)[2] ) <<  8 | \
            (uint32_t) ( (buf)[3] )       )

#define GETU16FBE(buf) (                    \
            (uint32_t) ( (buf)[0] ) <<  8 | \
            (uint32_t) ( (buf)[1] )       )

// Midi Status Bytes
#define MIDI_STATUS_NOTE_OFF    0x08
#define MIDI_STATUS_NOTE_ON     0x09
#define MIDI_STATUS_AFTERTOUCH  0x0A
#define MIDI_STATUS_CONTROLLER  0x0B
#define MIDI_STATUS_PROG_CHANGE 0x0C
#define MIDI_STATUS_PRESSURE    0x0D
#define MIDI_STATUS_PITCH_WHEEL 0x0E
#define MIDI_STATUS_SYSEX       0x0F


static unsigned int read_varlen(midi_track_info *track);

#include "midi2-gm_on_mt32.h"

static unsigned int read_varlen(midi_track_info *track)
{
    unsigned int varlen, ___data;

    varlen = 0;
    if (track->len != 0)
    {
        do {
            ___data = *(track->ptr);
            track->ptr++;
            track->len--;
            varlen = (varlen << 7) | (___data & 0x7f);
        } while ((___data & 0x80) && (track->len != 0));
    }

    track->eot = (track->len == 0)?1:0;

    return (track->eot)?0:varlen;
}

static int readmidi(const uint8_t *midi, unsigned int midilen, unsigned int *number_of_tracks_ptr, unsigned int *time_division_ptr, midi_track_info **tracks_ptr)
{
    unsigned int format_type, number_of_tracks, time_division, index;
    midi_track_info *tracks;
    const uint8_t *cur_position;
    int retval;

    if (midilen < 14)
    {
        // not enough place for midi header
        return 1;
    }

    if (GETU32FBE(midi) != 0x4D546864)
    {
        // "MThd"
        return 2;
    }

    if (GETU32FBE(midi + 4) != 6)
    {
        // wrong midi header size
        return 3;
    }

    format_type = GETU16FBE(midi + 8);
    number_of_tracks = GETU16FBE(midi + 10);
    time_division = GETU16FBE(midi + 12);

    if ((format_type != 0) && (format_type != 1))
    {
        // unsupported midi format
        return 4;
    }

    if ((number_of_tracks == 0) || ((format_type == 0) && (number_of_tracks != 1)))
    {
        // wrong number of tracks
        return 5;
    }

    if (time_division & 0x8000)
    {
        if (((time_division & 0x7f00) == 0) || ((time_division & 0xff) == 0))
        {
            // wrong time division
            return 6;
        }
    }
    else
    {
        if (time_division == 0)
        {
            // wrong time division
            return 6;
        }
    }

    tracks = (midi_track_info *) malloc(number_of_tracks * sizeof(midi_track_info));
    if (tracks == NULL)
    {
        return 7;
    }

    // find tracks
    cur_position = midi + 14;
    for (index = 0; index < number_of_tracks; index++)
    {
        unsigned int track_len;

        if ((cur_position - midi) + 8 > midilen)
        {
            // not enough place for track header
            retval = 8;
            goto midi_error_1;
        }

        if (GETU32FBE(cur_position) != 0x4D54726B)
        {
            // "MTrk"
            retval = 9;
            goto midi_error_1;
        }

        track_len = GETU32FBE(cur_position + 4);

        if ((cur_position - midi) + track_len > midilen)
        {
            // not enough place for track
            retval = 10;
            goto midi_error_1;
        }

        tracks[index].len = track_len;
        tracks[index].ptr = cur_position + 8;
        tracks[index].event_status = 0;
        tracks[index].eot = (track_len == 0)?1:0;

        cur_position = cur_position + 8 + track_len;
    }

    *number_of_tracks_ptr = number_of_tracks;
    *time_division_ptr = time_division;
    *tracks_ptr = tracks;

    return 0;

midi_error_1:
    free(tracks);
    return retval;
}

static int preprocessmidi(const uint8_t *midi, unsigned int midilen, unsigned int *timediv, midi_event_info **dataptr)
{
    unsigned int number_of_tracks, index, varlen;
    midi_track_info *tracks, *curtrack;
    preprocess_state state;
    int retval, lasttracknum;

    memset(&state, 0, sizeof(state));

    retval = readmidi(midi, midilen, &number_of_tracks, &state.time_division, &tracks);
    if (retval) return retval;

    if (midi_type == 2)
    {
        mt32_init_vars_and_install_timbres(number_of_tracks, tracks);
    }

    // prepare tracks
    for (index = 0; index < number_of_tracks; index++)
    {
        curtrack = &(tracks[index]);

        // read delta
        curtrack->delta = read_varlen(curtrack);
    }

    state.tempo = 500000; // 500000 MPQN = 120 BPM
    state.num_allocated = midilen / 4;

    state.events = (midi_event_info *) malloc(sizeof(midi_event_info) * state.num_allocated);
    if (state.events == NULL)
    {
        retval = 11;
        goto midi_error_1;
    }

    memset(&(state.events[0]), 0, sizeof(midi_event_info));

    lasttracknum = -1;
    while (1)
    {
        curtrack = NULL;

        if ((lasttracknum >= 0) && (!tracks[lasttracknum].eot) && (tracks[lasttracknum].delta == 0))
        {
            curtrack = &(tracks[lasttracknum]);
        }
        else
        {
            unsigned int mindelta;
            mindelta = UINT_MAX;
            for (index = 0; index < number_of_tracks; index++)
            {
                if ((!tracks[index].eot) && (tracks[index].delta < mindelta))
                {
                    mindelta = tracks[index].delta;
                    curtrack = &(tracks[index]);
                    lasttracknum = index;
                }
            }
        }

        if (curtrack == NULL)
        {
            if (mt32_delay && (state.events[state.num_events].time < state.next_sysex_time))
            {
                // add extra event
                calculate_next_sysex_tick(&state);
                calculate_event_time(&state);
                add_midi_event(&state, 0xf4, 0, 0, NULL, 0); // unused/invalid midi event type
            }

            break;
        }

        // update deltas
        if (curtrack->delta != 0)
        for (index = 0; index < number_of_tracks; index++)
        {
            if ((!tracks[index].eot) && (index != lasttracknum))
            {
                tracks[index].delta -= curtrack->delta;
            }
        }

        // read and process data
        state.last_tick += curtrack->delta;

        calculate_event_time(&state);

        if (*curtrack->ptr & 0x80)
        {
            curtrack->event_status = *curtrack->ptr;
            curtrack->ptr += 1;
            curtrack->len -= 1;
        }

        switch (curtrack->event_status >> 4)
        {
            case MIDI_STATUS_NOTE_OFF:
            case MIDI_STATUS_NOTE_ON:
            case MIDI_STATUS_AFTERTOUCH:
            case MIDI_STATUS_CONTROLLER:
            case MIDI_STATUS_PITCH_WHEEL:
                if (curtrack->len >= 2)
                {
                    if (midi_type == 2)
                    {
                        retval = mt32_add_short_event(&state, curtrack->event_status, curtrack->ptr[0], curtrack->ptr[1]);
                    }
                    else
                    {
                        retval = add_midi_event(&state, curtrack->event_status, curtrack->ptr[0], curtrack->ptr[1], NULL, 0);
                    }
                    curtrack->ptr += 2;
                    curtrack->len -= 2;
                }
                else
                {
                    curtrack->len = 0;
                    curtrack->eot = 1;
                }
                break;

            case MIDI_STATUS_PROG_CHANGE:
            case MIDI_STATUS_PRESSURE:
                if (curtrack->len >= 1)
                {
                    if (midi_type == 2)
                    {
                        retval = mt32_add_short_event(&state, curtrack->event_status, curtrack->ptr[0], 0);
                    }
                    else
                    {
                        retval = add_midi_event(&state, curtrack->event_status, curtrack->ptr[0], 0, NULL, 0);
                    }
                    curtrack->ptr += 1;
                    curtrack->len -= 1;
                }
                else
                {
                    curtrack->len = 0;
                    curtrack->eot = 1;
                }
                break;

            case MIDI_STATUS_SYSEX:
                if (curtrack->event_status == 0xff) // meta events
                {
                    if (curtrack->len >= 2)
                    {
                        if (curtrack->ptr[0] == 0x2f) // end of track
                        {
                            curtrack->len = 0;
                            curtrack->eot = 1;
                        }
                        else
                        {
                            if ((curtrack->ptr[0] == 0x51) && (curtrack->ptr[1] == 3) && (curtrack->len >= 5)) // set tempo
                            {
                                retval = add_midi_event(&state, curtrack->event_status, curtrack->ptr[0], 0, curtrack->ptr + 2, curtrack->ptr[1]);
                            }

                            // read length and skip event
                            curtrack->ptr += 1;
                            curtrack->len -= 1;
                            varlen = read_varlen(curtrack);
                            if (varlen <= curtrack->len)
                            {
                                curtrack->ptr += varlen;
                                curtrack->len -= varlen;
                            }
                            else
                            {
                                curtrack->len = 0;
                                curtrack->eot = 1;
                            }
                        }
                    }
                    else
                    {
                        curtrack->len = 0;
                        curtrack->eot = 1;
                    }
                }
                else if ((curtrack->event_status == 0xf0) || (curtrack->event_status == 0xf7)) // sysex
                {
                    varlen = read_varlen(curtrack);
                    if (varlen <= curtrack->len)
                    {
                        retval = add_midi_event(&state, curtrack->event_status, 0, 0, curtrack->ptr, varlen);
                        curtrack->ptr += varlen;
                        curtrack->len -= varlen;
                    }
                    else
                    {
                        curtrack->len = 0;
                        curtrack->eot = 1;
                    }
                }
                else
                {
                    curtrack->len = 0;
                    curtrack->eot = 1;
                }
                break;

            default:
                curtrack->len = 0;
                curtrack->eot = 1;
                break;
        }

        if (retval) goto midi_error_2;

        // read delta
        curtrack->delta = read_varlen(curtrack);
    };

    if (state.num_events == 0)
    {
        retval = 14;
        goto midi_error_2;
    }

    // return values
    *timediv = state.time_division;
    *dataptr = state.events;

    free(tracks);
    return 0;

midi_error_2:
    free_midi_data(state.events);
midi_error_1:
    free(tracks);
    *timediv = 0;
    *dataptr = NULL;
    return retval;
}

