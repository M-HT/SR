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

#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #include <alsa/asoundlib.h>
    #include <pthread.h>
    #include <limits.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "midi-plugins2.h"

static unsigned char *reset_controller_events = NULL;

#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
typedef struct {
    const uint8_t *ptr;
    unsigned int len, delta;
    uint8_t event_status;
    int eot;
} midi_track_info;

typedef struct {
    uint32_t tick;
    uint8_t type, channel, data1, data2;
    union {
        int32_t pitch;
        int32_t tempo;
        uint32_t len;
    };
    uint8_t *sysex;
    uint64_t time;
} midi_event_info;


static snd_seq_t *midi_seq = NULL;
static char *dst_address = NULL;
static int src_client_id, src_port_id, dst_client_id, dst_port_id;
static int midi_queue;
static pthread_t midi_thread;

static volatile int midi_quit = 0;
static volatile int midi_loaded = 0;
static volatile int midi_playing = 0;
static volatile int midi_current_volume = 128;
static volatile int midi_new_volume = 128;
static volatile int midi_loop_count = 0;
static volatile int midi_eof = 0;

static volatile midi_event_info *midi_events = NULL;
static volatile unsigned int midi_current_event;
static volatile unsigned int midi_base_tick;
static volatile unsigned int midi_last_tick;
static volatile uint64_t midi_base_time;

static int channel_volume[MIDI_CHANNELS];
static int channel_notes[MIDI_CHANNELS][128];

static pthread_mutex_t midi_mutex;

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

static const uint8_t alsa_event_types[8] = {
    SND_SEQ_EVENT_NOTEOFF,
    SND_SEQ_EVENT_NOTEON,
    SND_SEQ_EVENT_KEYPRESS,
    SND_SEQ_EVENT_CONTROLLER,
    SND_SEQ_EVENT_PGMCHANGE,
    SND_SEQ_EVENT_CHANPRESS,
    SND_SEQ_EVENT_PITCHBEND,
    SND_SEQ_EVENT_NONE
};


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

static void free_midi_data(midi_event_info *data)
{
    unsigned int index;

    if (data != NULL)
    {
        for (index = data[0].len; index != 0; index--)
        {
            if (data[index].sysex != NULL)
            {
                free(data[index].sysex);
                data[index].sysex = NULL;
            }
        }

        free(data);
    }
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

    if (time_division == 0)
    {
        // wrong time division
        return 6;
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
    unsigned int number_of_tracks, time_division, index, varlen;
    midi_track_info *tracks, *curtrack;
    unsigned int num_allocated, num_events, last_tick;
    midi_event_info *events;
    int retval, lasttracknum, eventextralen;
    midi_event_info event;
    unsigned int tempo, tempo_tick;
    uint64_t tempo_time;

    retval = readmidi(midi, midilen, &number_of_tracks, &time_division, &tracks);
    if (retval) return retval;

    // prepare tracks
    for (index = 0; index < number_of_tracks; index++)
    {
        curtrack = &(tracks[index]);

        // read delta
        curtrack->delta = read_varlen(curtrack);
    }

    num_allocated = midilen / 4;
    num_events = 1;

    events = (midi_event_info *) malloc(sizeof(midi_event_info) * num_allocated);
    if (events == NULL)
    {
        retval = 11;
        goto midi_error_1;
    }

    events[0].tick = 0;
    events[0].type = SND_SEQ_EVENT_NONE;
    events[0].len = 0;
    events[0].sysex = NULL;
    events[0].time = 0;

    lasttracknum = -1;
    last_tick = 0;
    tempo = 500000; // 500000 MPQN = 120 BPM
    tempo_tick = 0;
    tempo_time = 0;
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

        if (curtrack == NULL) break;

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
        event.tick = last_tick + curtrack->delta;
        last_tick = event.tick;
        event.sysex = NULL;

        // calculate event time in nanoseconds
        {
            div_t divres;

            divres = div(event.tick - tempo_tick, time_division);

            event.time = ( ((1000 * divres.rem) * (uint64_t)tempo) / time_division )
                       + ( (divres.quot * (uint64_t)tempo) * 1000 )
                       + tempo_time
                       ;

            //event.time = ( (((event.tick - tempo_tick) * (uint64_t) 1000) * tempo) / time_division ) + tempo_time;
        }

        eventextralen = -1;

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
                    event.type = alsa_event_types[(curtrack->event_status >> 4) - 8];
                    event.channel = curtrack->event_status & 0x0f;
                    event.data1 = curtrack->ptr[0];
                    event.data2 = curtrack->ptr[1];
                    if ((curtrack->event_status >> 4) == MIDI_STATUS_PITCH_WHEEL)
                    {
                        event.pitch = ( ((int32_t)curtrack->ptr[0]) | (((int32_t)curtrack->ptr[1]) << 7) ) - 0x2000; // midi uses values 0 - 16383; alsa uses values -8192 - 8191
                    }
                    curtrack->ptr += 2;
                    curtrack->len -= 2;
                    eventextralen = 0;
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
                    event.type = alsa_event_types[(curtrack->event_status >> 4) - 8];
                    event.channel = curtrack->event_status & 0x0f;
                    event.data1 = curtrack->ptr[0];
                    curtrack->ptr += 1;
                    curtrack->len -= 1;
                    eventextralen = 0;
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
                                // time_division is assumed to be positive (ticks per beat / PPQN - Pulses (i.e. clocks) Per Quarter Note)

                                event.type = SND_SEQ_EVENT_TEMPO;
                                event.channel = curtrack->ptr[2];
                                event.data1 = curtrack->ptr[3];
                                event.data2 = curtrack->ptr[4];
                                event.tempo = (((uint32_t)(curtrack->ptr[2])) << 16) | (((uint32_t)(curtrack->ptr[3])) << 8) | ((uint32_t)(curtrack->ptr[4]));
                                eventextralen = 0;

                                tempo = event.tempo;
                                tempo_tick = event.tick;
                                tempo_time = event.time;
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
                    const uint8_t *startevent;

                    startevent = curtrack->ptr;

                    varlen = read_varlen(curtrack);
                    if (varlen <= curtrack->len)
                    {
                        event.type = SND_SEQ_EVENT_SYSEX;
                        event.len = varlen + ((curtrack->event_status == 0xf0)?1:0);
                        if (event.len)
                        {
                            event.sysex = (uint8_t *) malloc(event.len);
                            if (event.sysex == NULL)
                            {
                                retval = 12;
                                goto midi_error_2;
                            }

                            if ((curtrack->event_status == 0xf0))
                            {
                                event.sysex[0] = 0xf0;
                                memcpy(event.sysex + 1, curtrack->ptr, varlen);
                            }
                            else
                            {
                                memcpy(event.sysex, curtrack->ptr, varlen);
                            }

                            curtrack->ptr += varlen;
                            curtrack->len -= varlen;

                            eventextralen = 1 + curtrack->ptr - startevent;
                        }
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

        if (eventextralen >= 0)
        {
            if (num_events >= num_allocated)
            {
                midi_event_info *new_events;

                new_events = (midi_event_info *) realloc(events, sizeof(midi_event_info) * num_allocated * 2);
                if (new_events == NULL)
                {
                    retval = 13;
                    goto midi_error_2;
                }

                num_allocated = num_allocated * 2;
                events = new_events;
            }

            events[num_events] = event;
            events[0].len = num_events;
            num_events++;
        }

        // read delta
        curtrack->delta = read_varlen(curtrack);
    };

    if (events[0].len == 0)
    {
        retval = 14;
        goto midi_error_2;
    }

    // return values
    *timediv = time_division;
    *dataptr = events;

    free(tracks);
    return 0;

midi_error_2:
    free_midi_data(events);
midi_error_1:
    free(tracks);
    return retval;
}


static void *midi_thread_proc(void *arg)
{
    midi_event_info *events;
    unsigned int current_event, base_tick, num_events;
    uint64_t base_time;
    int do_sleep, num_input_events, dst_port_exists;
    snd_seq_queue_status_t *queue_status;
    const snd_seq_real_time_t *real_time;
    int64_t time_diff, base_diff;
    snd_seq_event_t event;
    snd_seq_event_t *input_event;

    snd_seq_queue_status_alloca(&queue_status);

    snd_seq_ev_clear(&event);
    event.queue = midi_queue;
    event.source.port = src_port_id;
    event.flags = SND_SEQ_TIME_STAMP_TICK | SND_SEQ_TIME_MODE_ABS;


    do_sleep = 1;
    dst_port_exists = 1;

    while (1)
    {
        if (do_sleep)
        {
            do_sleep = 0;

            struct timespec _tp;

            _tp.tv_sec = 0;
            _tp.tv_nsec = 10000000; // 10ms

            nanosleep(&_tp, NULL);
        }

        if (midi_quit) return NULL;

        // check announcement events
        num_input_events = snd_seq_event_input_pending(midi_seq, 1);
        for (; num_input_events; num_input_events--)
        {
            if (snd_seq_event_input(midi_seq, &input_event) < 0) break;

            switch(input_event->type)
            {
                case SND_SEQ_EVENT_PORT_START:
                    if (!dst_port_exists)
                    {
                        snd_seq_addr_t addr;
                        if (snd_seq_parse_address(midi_seq, &addr, dst_address) == 0)
                        {
                            if ((input_event->data.addr.client == addr.client) && (input_event->data.addr.port == addr.port))
                            {
                                // resubscribe to started port

                                pthread_mutex_lock(&midi_mutex);

                                dst_client_id = addr.client;
                                dst_port_id = addr.port;
                                dst_port_exists = 1;

                                snd_seq_connect_to(midi_seq, src_port_id, dst_client_id, dst_port_id);

                                pthread_mutex_unlock(&midi_mutex);
                            }
                        }
                    }
                    break;
                case SND_SEQ_EVENT_PORT_EXIT:
                    if (dst_port_exists)
                    {
                        if ((input_event->data.addr.client == dst_client_id) && (input_event->data.addr.port == dst_port_id))
                        {
                            dst_port_exists = 0;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        if ((!midi_loaded) || (!midi_playing))
        {
            do_sleep = 1;
            continue;
        }


        pthread_mutex_lock(&midi_mutex);

        if ((!midi_loaded) || (!midi_playing) || (midi_eof))
        {
            pthread_mutex_unlock(&midi_mutex);
            do_sleep = 1;
            continue;
        }

        events = (midi_event_info *) midi_events;
        current_event = midi_current_event;
        base_tick = midi_base_tick;
        base_time = midi_base_time;

        num_events = events[0].len;

        if (current_event > num_events)
        {
            if (midi_loop_count == 0)
            {
                midi_eof = 1;
                pthread_mutex_unlock(&midi_mutex);
                do_sleep = 1;
                continue;
            }
            else if (midi_loop_count > 0)
            {
                midi_loop_count--;
            }

            // looping
            base_tick += events[num_events].tick;
            base_time += events[num_events].time;

            current_event = 1;

            midi_current_event = current_event;
            midi_base_tick = base_tick;
            midi_base_time = base_time;
        }

        if ((midi_new_volume != midi_current_volume) && (events[current_event].tick != 0))
        {
            int chan;

            midi_current_volume = midi_new_volume;

            snd_seq_ev_set_fixed(&event);
            event.type = SND_SEQ_EVENT_CONTROLLER;
            event.time.tick = base_tick + events[current_event - 1].tick;
            event.dest.client = dst_client_id;
            event.dest.port = dst_port_id;
            event.data.control.param = MIDI_CTL_MSB_MAIN_VOLUME;

            for (chan = 0; chan < MIDI_CHANNELS; chan++)
            {
                event.data.control.channel = chan;
                event.data.control.value = (midi_current_volume * channel_volume[chan]) / 127;
                snd_seq_event_output(midi_seq, &event);
            }

            snd_seq_drain_output(midi_seq);
        }

        if (0 > snd_seq_get_queue_status(midi_seq, midi_queue, queue_status))
        {
            pthread_mutex_unlock(&midi_mutex);
            do_sleep = 1;
            continue;
        }

        real_time = snd_seq_queue_status_get_real_time(queue_status);

        base_diff = ((real_time->tv_sec * (uint64_t)1000000000) + real_time->tv_nsec) - base_time;

        time_diff = events[current_event].time - base_diff;

        if (time_diff >= 100000000) // 100ms
        {
            pthread_mutex_unlock(&midi_mutex);
            do_sleep = 1;
            continue;
        }

        do
        {
            // add events to queue
            event.type = events[current_event].type;
            event.time.tick = base_tick + events[current_event].tick;
            event.dest.client = dst_client_id;
            event.dest.port = dst_port_id;

            switch (event.type)
            {
                case SND_SEQ_EVENT_NOTEON:
                    snd_seq_ev_set_fixed(&event);
                    event.data.note.channel = events[current_event].channel;
                    event.data.note.note = events[current_event].data1;
                    event.data.note.velocity = events[current_event].data2;

                    if (event.data.note.velocity != 0)
                    {
                        // note on
                        channel_notes[event.data.note.channel][event.data.note.note]++;
                    }
                    else
                    {
                        // note off
                        if (channel_notes[event.data.note.channel][event.data.note.note] > 0)
                        {
                            channel_notes[event.data.note.channel][event.data.note.note]--;
                        }
                    }
                    break;
                case SND_SEQ_EVENT_NOTEOFF:
                    if (channel_notes[events[current_event].channel][events[current_event].data1] > 0)
                    {
                        channel_notes[events[current_event].channel][events[current_event].data1]--;
                    }
                    // fallthrough
                case SND_SEQ_EVENT_KEYPRESS:
                    snd_seq_ev_set_fixed(&event);
                    event.data.note.channel = events[current_event].channel;
                    event.data.note.note = events[current_event].data1;
                    event.data.note.velocity = events[current_event].data2;
                    break;
                case SND_SEQ_EVENT_CONTROLLER:
                    snd_seq_ev_set_fixed(&event);
                    event.data.control.channel = events[current_event].channel;
                    event.data.control.param = events[current_event].data1;
                    event.data.control.value = events[current_event].data2;
                    if (event.data.control.param == MIDI_CTL_MSB_MAIN_VOLUME)
                    {
                        channel_volume[event.data.control.channel] = event.data.control.value;
                        event.data.control.value = (event.data.control.value * midi_current_volume) / 127;
                    }
                    break;
                case SND_SEQ_EVENT_PGMCHANGE:
                case SND_SEQ_EVENT_CHANPRESS:
                    snd_seq_ev_set_fixed(&event);
                    event.data.control.channel = events[current_event].channel;
                    event.data.control.value = events[current_event].data1;
                    break;
                case SND_SEQ_EVENT_PITCHBEND:
                    snd_seq_ev_set_fixed(&event);
                    event.data.control.channel = events[current_event].channel;
                    event.data.control.value = events[current_event].pitch;
                    break;
                case SND_SEQ_EVENT_SYSEX:
                    snd_seq_ev_set_variable(&event, events[current_event].len, events[current_event].sysex);
                    break;
                case SND_SEQ_EVENT_TEMPO:
                    snd_seq_ev_set_fixed(&event);
                    event.dest.client = SND_SEQ_CLIENT_SYSTEM;
                    event.dest.port = SND_SEQ_PORT_SYSTEM_TIMER;
                    event.data.queue.queue = midi_queue;
                    event.data.queue.param.value = events[current_event].tempo;
                    break;
            }

            snd_seq_event_output(midi_seq, &event);

            current_event++;
            if (current_event > num_events) break;
            time_diff = events[current_event].time - base_diff;
        } while (time_diff < 100000000); // 100ms

        snd_seq_drain_output(midi_seq);

        midi_current_event = current_event;
        midi_last_tick = event.time.tick;

        pthread_mutex_unlock(&midi_mutex);
    };
}

static void send_initial_sysex_events(unsigned char const *sysex_events)
{
    snd_seq_event_t event;

    if (midi_seq == NULL) return;

    snd_seq_ev_clear(&event);
    event.queue = midi_queue;
    event.source.port = src_port_id;
    event.flags = SND_SEQ_TIME_STAMP_TICK | SND_SEQ_TIME_MODE_ABS;

    pthread_mutex_lock(&midi_mutex);

    event.time.tick = midi_last_tick;

    if (!midi_playing)
    {
        snd_seq_ev_set_fixed(&event);
        event.type = SND_SEQ_EVENT_CONTINUE;
        event.dest.client = SND_SEQ_CLIENT_SYSTEM;
        event.dest.port = SND_SEQ_PORT_SYSTEM_TIMER;
        event.data.queue.queue = midi_queue;

        if (0 <= snd_seq_event_output(midi_seq, &event))
        {
            if (0 <= snd_seq_drain_output(midi_seq))
            {
                midi_playing = 1;
            }
        }
    }

    if (midi_playing)
    {
        snd_seq_sync_output_queue(midi_seq);

        event.type = SND_SEQ_EVENT_SYSEX;
        event.dest.client = dst_client_id;
        event.dest.port = dst_port_id;

        while (*sysex_events == 0xf0)
        {
            int len;

            len = 2;
            while (sysex_events[len - 1] != 0xf7) len++;

            snd_seq_ev_set_variable(&event, len, (void *)sysex_events);
            sysex_events += len;
            snd_seq_event_output(midi_seq, &event);
        };

        snd_seq_ev_set_fixed(&event);
        event.type = SND_SEQ_EVENT_STOP;
        event.dest.client = SND_SEQ_CLIENT_SYSTEM;
        event.dest.port = SND_SEQ_PORT_SYSTEM_TIMER;
        event.data.queue.queue = midi_queue;

        snd_seq_event_output(midi_seq, &event);

        snd_seq_drain_output(midi_seq);

        snd_seq_sync_output_queue(midi_seq);

        midi_playing = 0;
    }

    pthread_mutex_unlock(&midi_mutex);
}

static void reset_playing(void)
{
    snd_seq_event_t event;
    int chan;

    if (midi_seq == NULL) return;

    snd_seq_ev_clear(&event);
    event.queue = midi_queue;
    event.source.port = src_port_id;
    event.flags = SND_SEQ_TIME_STAMP_TICK | SND_SEQ_TIME_MODE_ABS;

    pthread_mutex_lock(&midi_mutex);

    event.time.tick = midi_last_tick;

    if (!midi_playing)
    {
        snd_seq_ev_set_fixed(&event);
        event.type = SND_SEQ_EVENT_CONTINUE;
        event.dest.client = SND_SEQ_CLIENT_SYSTEM;
        event.dest.port = SND_SEQ_PORT_SYSTEM_TIMER;
        event.data.queue.queue = midi_queue;

        if (0 <= snd_seq_event_output(midi_seq, &event))
        {
            if (0 <= snd_seq_drain_output(midi_seq))
            {
                midi_playing = 1;
            }
        }
    }

    if (midi_playing)
    {
        snd_seq_sync_output_queue(midi_seq);

        event.type = SND_SEQ_EVENT_CONTROLLER;
        event.dest.client = dst_client_id;
        event.dest.port = dst_port_id;

        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            snd_seq_ev_set_fixed(&event);
            event.data.control.channel = chan;
            event.data.control.param = MIDI_CTL_ALL_SOUNDS_OFF; // All sounds off (abrupt stop of sound on channel)
            event.data.control.value = 0;

            snd_seq_event_output(midi_seq, &event);

            snd_seq_ev_set_fixed(&event);
            event.data.control.channel = chan;
            event.data.control.param = MIDI_CTL_RESET_CONTROLLERS; // All controllers off (this message clears all the controller values for this channel, back to their default values)
            event.data.control.value = 0;

            snd_seq_event_output(midi_seq, &event);

            snd_seq_ev_set_fixed(&event);
            event.data.control.channel = chan;
            event.data.control.param = MIDI_CTL_ALL_NOTES_OFF; // All notes off (this message stops all the notes that are currently playing)
            event.data.control.value = 0;

            snd_seq_event_output(midi_seq, &event);

            if (reset_controller_events != NULL)
            {
                unsigned char *controller_events;

                controller_events = reset_controller_events;
                while (*controller_events != 0xff)
                {
                    snd_seq_ev_set_fixed(&event);
                    event.data.control.channel = chan;
                    event.data.control.param = controller_events[0];
                    event.data.control.value = controller_events[1];
                    controller_events += 2;

                    snd_seq_event_output(midi_seq, &event);
                }
            }
        }

        snd_seq_ev_set_fixed(&event);
        event.type = SND_SEQ_EVENT_PGMCHANGE;
        event.data.control.channel = MIDI_GM_DRUM_CHANNEL;
        event.data.control.value = 0;

        snd_seq_event_output(midi_seq, &event);

        event.type = SND_SEQ_EVENT_PITCHBEND;

        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            snd_seq_ev_set_fixed(&event);
            event.data.control.channel = chan;
            event.data.control.value = 0; // midi uses values 0 - 16383; alsa uses values -8192 - 8191

            snd_seq_event_output(midi_seq, &event);
        }

        snd_seq_ev_set_fixed(&event);
        event.type = SND_SEQ_EVENT_STOP;
        event.dest.client = SND_SEQ_CLIENT_SYSTEM;
        event.dest.port = SND_SEQ_PORT_SYSTEM_TIMER;
        event.data.queue.queue = midi_queue;

        snd_seq_event_output(midi_seq, &event);

        snd_seq_drain_output(midi_seq);

        snd_seq_sync_output_queue(midi_seq);

        midi_playing = 0;
    }

    pthread_mutex_unlock(&midi_mutex);
}

static void close_midi(void)
{
    if (!midi_loaded) return;

    reset_playing();

    free_midi_data((midi_event_info *)midi_events);
    midi_loaded = 0;
}


static void error_handler(const char *file, int line, const char *function, int err, const char *fmt, ...)
{
    // do nothing
}

static int create_src_port(void)
{
    snd_seq_port_info_t *pinfo;

    snd_seq_port_info_alloca(&pinfo);

    snd_seq_port_info_set_name(pinfo, "midi2-alsa");

    snd_seq_port_info_set_capability(pinfo, SND_SEQ_PORT_CAP_WRITE);
    snd_seq_port_info_set_type(pinfo, SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_APPLICATION);

    if (snd_seq_create_port(midi_seq, pinfo) < 0) return -1;

    src_port_id = snd_seq_port_info_get_port(pinfo);

    return 0;
}

static int find_dst_port(const char *midi_address, int midi_type)
{
    snd_seq_client_info_t *cinfo;
    snd_seq_port_info_t *pinfo;
    int client_id[3], port_id[3];
    unsigned int ptype;
    char address[32];

    if (dst_address != NULL)
    {
        free(dst_address);
        dst_address = NULL;
    }

    if (midi_address != NULL && *midi_address != 0)
    {
        snd_seq_addr_t addr;
        if (snd_seq_parse_address(midi_seq, &addr, midi_address) < 0) return -1;

        dst_client_id = addr.client;
        dst_port_id = addr.port;

        snd_seq_port_info_alloca(&pinfo);

        if (snd_seq_get_any_port_info(midi_seq, dst_client_id, dst_port_id, pinfo) < 0) return -2;

        if ((snd_seq_port_info_get_capability(pinfo) & (SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE | SND_SEQ_PORT_CAP_NO_EXPORT)) != (SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE))
        {
            return -3;
        }
        else
        {
            dst_address = strdup(midi_address);
            return 0;
        }
    }
    else
    {
        client_id[0] = -1;
        client_id[1] = -1;
        client_id[2] = -1;

        snd_seq_client_info_alloca(&cinfo);
        snd_seq_port_info_alloca(&pinfo);

        snd_seq_client_info_set_client(cinfo, -1);
        while (snd_seq_query_next_client(midi_seq, cinfo) >= 0)
        {
            snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
            snd_seq_port_info_set_port(pinfo, -1);

            while (snd_seq_query_next_port(midi_seq, pinfo) >= 0)
            {
                if ( (snd_seq_port_info_get_capability(pinfo) & (SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE | SND_SEQ_PORT_CAP_NO_EXPORT)) == (SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE) )
                {
                    ptype = snd_seq_port_info_get_type(pinfo);
                    if (ptype & ( (midi_type)?SND_SEQ_PORT_TYPE_MIDI_MT32:(SND_SEQ_PORT_TYPE_MIDI_GENERIC | SND_SEQ_PORT_TYPE_MIDI_GM) ))
                    {
                        if (snd_seq_port_info_get_midi_channels(pinfo))
                        {
                            dst_client_id = snd_seq_client_info_get_client(cinfo);
                            dst_port_id = snd_seq_port_info_get_port(pinfo);
                            sprintf(address, "%i:%i", dst_client_id, dst_port_id);
                            dst_address = strdup(address);
                            return 0;
                        }
                        else if (client_id[0] == -1)
                        {
                            client_id[0] = snd_seq_client_info_get_client(cinfo);
                            port_id[0] = snd_seq_port_info_get_port(pinfo);
                        }
                    }
                    else if (midi_type && (ptype & SND_SEQ_PORT_TYPE_MIDI_GENERIC))
                    {
                        if (snd_seq_port_info_get_midi_channels(pinfo))
                        {
                            if (client_id[1] == -1)
                            {
                                client_id[1] = snd_seq_client_info_get_client(cinfo);
                                port_id[1] = snd_seq_port_info_get_port(pinfo);
                            }
                        }
                        else if (client_id[2] == -1)
                        {
                            client_id[2] = snd_seq_client_info_get_client(cinfo);
                            port_id[2] = snd_seq_port_info_get_port(pinfo);
                        }
                    }
                }
            }
        }

        for (ptype = 0; ptype <= 2; ptype++)
        {
            if (client_id[ptype] != -1)
            {
                dst_client_id = client_id[ptype];
                dst_port_id = port_id[ptype];
                sprintf(address, "%i:%i", dst_client_id, dst_port_id);
                dst_address = strdup(address);
                return 0;
            }
        }

        return -4;
    }
}

static int subscribe_announcements(int subscribe)
{
    snd_seq_addr_t sender, dest;
    snd_seq_port_subscribe_t *subs;

    sender.client = SND_SEQ_CLIENT_SYSTEM;
    sender.port = SND_SEQ_PORT_SYSTEM_ANNOUNCE;
    dest.client = src_client_id;
    dest.port = src_port_id;

    snd_seq_port_subscribe_alloca(&subs);
    snd_seq_port_subscribe_set_sender(subs, &sender);
    snd_seq_port_subscribe_set_dest(subs, &dest);

    if (subscribe)
    {
        if (snd_seq_subscribe_port(midi_seq, subs) < 0) return -1;
    }
    else
    {
        if (snd_seq_unsubscribe_port(midi_seq, subs) < 0) return -1;
    }

    return 0;
}

#endif


static int play(void const *midibuffer, long int size, int loop_count)
{
    if (midibuffer == NULL) return -1;
    if (size <= 0) return -2;

    if (loop_count < -1) loop_count = -1;

#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    if (midi_seq == NULL) return -3;

    close_midi();

    midi_loop_count = loop_count;

    {
        unsigned int timediv;
        midi_event_info *dataptr;
        snd_seq_queue_tempo_t *queue_tempo;
        int chan;

        if (preprocessmidi(midibuffer, size, &timediv, &dataptr))
        {
            return -4;
        }

        snd_seq_queue_tempo_alloca(&queue_tempo);

        pthread_mutex_lock(&midi_mutex);

        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            channel_volume[chan] = 127;
        }

        midi_current_volume = 128;

        snd_seq_queue_tempo_set_tempo(queue_tempo, 500000); // 120 BPM
        snd_seq_queue_tempo_set_ppq(queue_tempo, timediv);

        if (0 > snd_seq_set_queue_tempo(midi_seq, midi_queue, queue_tempo))
        {
            free_midi_data(dataptr);
            pthread_mutex_unlock(&midi_mutex);
            return -5;
        }

        if (!midi_playing)
        {
            if (0 <= snd_seq_start_queue(midi_seq, midi_queue, NULL))
            {
                if (0 <= snd_seq_drain_output(midi_seq))
                {
                    midi_playing = 1;
                    midi_base_tick = 0;
                    midi_base_time = 0;
                    midi_last_tick = 0;

                    snd_seq_sync_output_queue(midi_seq);
                }
            }
        }

        if (!midi_playing)
        {
            free_midi_data(dataptr);
            pthread_mutex_unlock(&midi_mutex);
            return -7;
        }

        midi_events = dataptr;
        midi_current_event = 1;

        midi_loaded = 1;
        midi_eof = 0;

        memset(channel_notes, 0, 128*MIDI_CHANNELS*sizeof(int));

        pthread_mutex_unlock(&midi_mutex);
    }
#endif

    return 0;
}

static int pause_0(void)
{
#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    snd_seq_event_t event;
    int chan, note, num;

    if (midi_seq == NULL) return -1;
    if (!midi_loaded) return -2;

    if (midi_playing)
    {
        snd_seq_ev_clear(&event);
        event.queue = midi_queue;
        event.source.port = src_port_id;
        event.flags = SND_SEQ_TIME_STAMP_TICK | SND_SEQ_TIME_MODE_ABS;

        event.type = SND_SEQ_EVENT_NOTEOFF;
        event.dest.client = dst_client_id;
        event.dest.port = dst_port_id;

        pthread_mutex_lock(&midi_mutex);

        event.time.tick = midi_last_tick;

        // stop playing notes on all channels
        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            for (note = 0; note < 128; note++)
            {
                if (channel_notes[chan][note] > 0)
                for (num = channel_notes[chan][note]; num != 0; num--)
                {
                    snd_seq_ev_set_fixed(&event);
                    event.data.note.channel = chan;
                    event.data.note.note = note;
                    event.data.note.velocity = 0;

                    if (0 > snd_seq_event_output(midi_seq, &event))
                    {
                        pthread_mutex_unlock(&midi_mutex);
                        return -3;
                    }
                }
            }
        }

        snd_seq_ev_set_fixed(&event);
        event.type = SND_SEQ_EVENT_STOP;
        event.dest.client = SND_SEQ_CLIENT_SYSTEM;
        event.dest.port = SND_SEQ_PORT_SYSTEM_TIMER;
        event.data.queue.queue = midi_queue;
        if (0 > snd_seq_event_output(midi_seq, &event))
        {
            pthread_mutex_unlock(&midi_mutex);
            return -3;
        }

        if (0 <= snd_seq_drain_output(midi_seq))
        {
            midi_playing = 0;

            snd_seq_sync_output_queue(midi_seq);
        }

        pthread_mutex_unlock(&midi_mutex);

        if (midi_playing) return -3;
    }

#endif

    return 0;
}

static int resume(void)
{
#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    snd_seq_event_t event;

    if (midi_seq == NULL) return -1;
    if (!midi_loaded) return -2;

    if (!midi_playing)
    {
        snd_seq_ev_clear(&event);
        event.queue = midi_queue;
        event.source.port = src_port_id;
        event.flags = SND_SEQ_TIME_STAMP_TICK | SND_SEQ_TIME_MODE_ABS;

        snd_seq_ev_set_fixed(&event);
        event.type = SND_SEQ_EVENT_CONTINUE;
        event.dest.client = SND_SEQ_CLIENT_SYSTEM;
        event.dest.port = SND_SEQ_PORT_SYSTEM_TIMER;
        event.data.queue.queue = midi_queue;

        pthread_mutex_lock(&midi_mutex);

        event.time.tick = midi_last_tick;

        if (0 > snd_seq_event_output(midi_seq, &event))
        {
            pthread_mutex_unlock(&midi_mutex);
            return -3;
        }

        if (0 <= snd_seq_drain_output(midi_seq))
        {
            midi_playing = 1;

            snd_seq_sync_output_queue(midi_seq);
        }

        pthread_mutex_unlock(&midi_mutex);

        if (!midi_playing) return -3;
    }

#endif

    return 0;
}

static int halt(void)
{
#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    if (midi_seq == NULL) return -1;

    close_midi();

#endif

    return 0;
}

static int set_volume(unsigned char volume) // volume = 0 - 127
{
    if (volume > 127) volume = 127;

#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    if (midi_seq == NULL) return -1;

    midi_new_volume = volume;

#endif

    return 0;
}

static int set_loop_count(int loop_count) // -1 = unlimited
{
    if (loop_count < -1) loop_count = -1;

#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    if (midi_seq == NULL) return -1;
    if (!midi_loaded) return -2;

    midi_loop_count = loop_count;

#endif

    return 0;
}

static void shutdown_plugin(void)
{
#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    if (midi_seq != NULL)
    {
        if (midi_loaded)
        {
            close_midi();
        }
        else
        {
            reset_playing();
        }

        midi_quit = 1;
        pthread_join(midi_thread, NULL);
        midi_quit = 0;

        pthread_mutex_destroy(&midi_mutex);

        snd_seq_sync_output_queue(midi_seq);

        snd_seq_disconnect_to(midi_seq, src_port_id, dst_client_id, dst_port_id);
        snd_seq_free_queue(midi_seq, midi_queue);
        snd_seq_delete_port(midi_seq, src_port_id);
        snd_seq_close(midi_seq);

        midi_seq = NULL;
    }

    if (dst_address != NULL)
    {
        free(dst_address);
        dst_address = NULL;
    }
#endif

    if (reset_controller_events != NULL)
    {
        free(reset_controller_events);
        reset_controller_events = NULL;
    }
}


__attribute__ ((visibility ("default")))
int initialize_midi_plugin2(midi_plugin2_parameters const *parameters, midi_plugin2_functions *functions)
{
    char const *address;
    unsigned char const *sysex_events, *controller_events;
    int midi_type;

    if (functions == NULL) return -3;

    address = NULL;
    sysex_events = NULL;
    controller_events = NULL;
    midi_type = 0;
    if (parameters != NULL)
    {
        address = parameters->midi_device_name;
        sysex_events = parameters->initial_sysex_events;
        controller_events = parameters->reset_controller_events;
        midi_type = parameters->midi_type;
    }

    if (controller_events != NULL && *controller_events == 0xb0)
    {
        int len;

        len = 1;
        while (controller_events[len] != 0xff) len++;

        if (len > 1)
        {
            reset_controller_events = (unsigned char *)malloc(len);
            if (reset_controller_events != NULL)
            {
                memcpy(reset_controller_events, controller_events + 1, len);
            }
        }
    }

    functions->play = &play;
    functions->pause = &pause_0;
    functions->resume = &resume;
    functions->halt = &halt;
    functions->set_volume = &set_volume;
    functions->set_loop_count = &set_loop_count;
    functions->shutdown_plugin = &shutdown_plugin;

    memset(channel_notes, 0, 128*MIDI_CHANNELS*sizeof(int));

#if !(defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
{
    pthread_attr_t attr;

    if (snd_seq_open(&midi_seq, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
    {
        midi_seq = NULL;
        return -4;
    }

    snd_lib_error_set_handler(error_handler);

    if (snd_seq_set_client_name(midi_seq, "midi2-alsa") < 0)
    {
        snd_seq_close(midi_seq);
        midi_seq = NULL;
        return -4;
    }

    src_client_id = snd_seq_client_id(midi_seq);
    if (src_client_id < 0)
    {
        snd_seq_close(midi_seq);
        midi_seq = NULL;
        return -4;
    }

    if (find_dst_port(address, midi_type) < 0)
    {
        snd_seq_close(midi_seq);
        midi_seq = NULL;
        return -5;
    }

    if (create_src_port() < 0)
    {
        snd_seq_close(midi_seq);
        midi_seq = NULL;
        return -4;
    }

    midi_queue = snd_seq_alloc_named_queue(midi_seq, "midi2-alsa");
    if (midi_queue < 0)
    {
        snd_seq_delete_port(midi_seq, src_port_id);
        snd_seq_close(midi_seq);
        midi_seq = NULL;
        return -4;
    }

    if (subscribe_announcements(1) < 0)
    {
        snd_seq_free_queue(midi_seq, midi_queue);
        snd_seq_delete_port(midi_seq, src_port_id);
        snd_seq_close(midi_seq);
        midi_seq = NULL;
        return -6;
    }

    if (snd_seq_connect_to(midi_seq, src_port_id, dst_client_id, dst_port_id) < 0)
    {
        subscribe_announcements(0);
        snd_seq_free_queue(midi_seq, midi_queue);
        snd_seq_delete_port(midi_seq, src_port_id);
        snd_seq_close(midi_seq);
        midi_seq = NULL;
        return -6;
    }

    if (pthread_mutex_init(&midi_mutex, NULL))
    {
        subscribe_announcements(0);
        snd_seq_disconnect_to(midi_seq, src_port_id, dst_client_id, dst_port_id);
        snd_seq_free_queue(midi_seq, midi_queue);
        snd_seq_delete_port(midi_seq, src_port_id);
        snd_seq_close(midi_seq);
        midi_seq = NULL;
        return -7;
    }

    midi_last_tick = 0;

    if (sysex_events != NULL && *sysex_events == 0xf0)
    {
        send_initial_sysex_events(sysex_events);
    }

    reset_playing();

    pthread_attr_init(&attr);

    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    if (pthread_create(&midi_thread, &attr, &midi_thread_proc, NULL) != 0)
    {
        pthread_attr_destroy(&attr);
        pthread_mutex_destroy(&midi_mutex);
        subscribe_announcements(0);
        snd_seq_disconnect_to(midi_seq, src_port_id, dst_client_id, dst_port_id);
        snd_seq_free_queue(midi_seq, midi_queue);
        snd_seq_delete_port(midi_seq, src_port_id);
        snd_seq_close(midi_seq);
        midi_seq = NULL;
        return -7;
    }

    pthread_attr_destroy(&attr);
}
#endif

    return 0;
}

