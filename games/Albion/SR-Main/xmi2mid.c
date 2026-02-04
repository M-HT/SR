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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include "xmi2mid.h"



#define GETU32FBE(buf) (                    \
            (uint32_t) ( (buf)[0] ) << 24 | \
            (uint32_t) ( (buf)[1] ) << 16 | \
            (uint32_t) ( (buf)[2] ) <<  8 | \
            (uint32_t) ( (buf)[3] )       )

#define GETU16FLE(buf) (                    \
            (uint32_t) ( (buf)[0] )       | \
            (uint32_t) ( (buf)[1] ) <<  8 )

#define READU32BE(res, buf) {       \
            res = GETU32FBE(buf);   \
            buf+=4;                 }

#define EVEN_LENGTH(len) ( (len+1)&(~1) )

#define READ_INTERVAL(res, buf) {               \
            res = 0;                            \
            while (((*(buf)) & 0x80) == 0) {    \
                res+=(*(buf));                  \
                buf++;                     }    }

#define READ_VARLEN(res, buf) {                         \
            unsigned int ___data;                       \
            res = 0;                                    \
            do {                                        \
                ___data = *(buf);                       \
                buf++;                                  \
                res = (res << 7) | (___data & 0x7f);    \
            } while (___data & 0x80);                   }

// "FORM"
#define BE_STR_FORM 0x464F524D
// "XDIR"
#define BE_STR_XDIR 0x58444952
// "INFO"
#define BE_STR_INFO 0x494E464F
// "CAT "
#define BE_STR_CAT  0x43415420
// "XMID"
#define BE_STR_XMID 0x584D4944
// "EVNT"
#define BE_STR_EVNT 0x45564E54

// Midi Status Bytes
#define MIDI_STATUS_NOTE_OFF    0x08
#define MIDI_STATUS_NOTE_ON     0x09
#define MIDI_STATUS_AFTERTOUCH  0x0A
#define MIDI_STATUS_CONTROLLER  0x0B
#define MIDI_STATUS_PROG_CHANGE 0x0C
#define MIDI_STATUS_PRESSURE    0x0D
#define MIDI_STATUS_PITCH_WHEEL 0x0E
#define MIDI_STATUS_SYSEX       0x0F


typedef struct _midi_event_
{
    uint32_t time;
    int32_t length;
    const uint8_t *data;
} midi_event;


const uint8_t *xmi_find_sequence(const uint8_t *xmi, int seq_num, uint32_t *seq_len)
{
    int num_sequences;
    uint32_t chunk_name, evnt_len;

    if (xmi == NULL) return NULL;

    READU32BE(chunk_name, xmi);

    num_sequences = 1;
    if (chunk_name == BE_STR_FORM)
    {
        const uint8_t *form_start;
        uint32_t form_len, info_len;

        READU32BE(form_len, xmi);
        if (form_len < 14) return NULL;

        form_start = xmi;
        READU32BE(chunk_name, xmi);

        if (chunk_name != BE_STR_XDIR) return NULL;

        READU32BE(chunk_name, xmi);

        if (chunk_name == BE_STR_INFO)
        {
            READU32BE(info_len, xmi);
            if (info_len == 2)
            {
                num_sequences = (int) GETU16FLE(xmi);
            }
        }

        xmi = form_start + EVEN_LENGTH(form_len);

        READU32BE(chunk_name, xmi);
    }

    if (seq_num >= num_sequences) return NULL;

    if (chunk_name != BE_STR_CAT) return NULL;
    xmi += 4; // skip cat lenght
    READU32BE(chunk_name, xmi);
    if (chunk_name != BE_STR_XMID) return NULL;

    while (seq_num > 0)
    {
        uint32_t form_len;

        READU32BE(chunk_name, xmi);
        if (chunk_name != BE_STR_FORM) return NULL;
        READU32BE(form_len, xmi);
        xmi += EVEN_LENGTH(form_len);
        seq_num--;
    }

    READU32BE(chunk_name, xmi);
    if (chunk_name != BE_STR_FORM) return NULL;

    xmi += 4; // skip form lenght
    READU32BE(chunk_name, xmi);
    if (chunk_name != BE_STR_XMID) return NULL;

    READU32BE(chunk_name, xmi);
    while (chunk_name != BE_STR_EVNT)
    {
        uint32_t chunk_len;

        READU32BE(chunk_len, xmi);
        xmi += EVEN_LENGTH(chunk_len);

        READU32BE(chunk_name, xmi);
    }

    READU32BE(evnt_len, xmi);

    if (seq_len != NULL) *seq_len = evnt_len;

    return xmi;
}

static unsigned int writeevents(uint8_t *buf, midi_event *events, int num_events)
{
    unsigned int size;
    uint32_t last_time, delta_time;

    size = 0;
    last_time = 0;

    while (num_events != 0)
    {
        int32_t evlen;

        evlen = events->length;
        if (evlen < 0) evlen = -evlen;
        size+=evlen;
        delta_time = events->time - last_time;
        last_time = events->time;
        if (buf != NULL)
        {
            int var_index, var_size;
            uint8_t var_buf[5];

            var_index = 4;
            var_buf[4] = delta_time & 0x7f;
            delta_time>>=7;
            var_size = 1;
            while (delta_time)
            {
                var_size++;
                var_index--;
                var_buf[var_index] = 0x80 | (delta_time & 0x7f);
                delta_time>>=7;
            } ;
            size+=var_size;

            while (var_size)
            {
                *buf = var_buf[var_index];
                buf++;
                var_index++;
                var_size--;
            } ;

            memcpy(buf, events->data, evlen);
            if (events->length < 0)
            {
                buf[0] &= 0x8F; // change note on to note off
                buf[2] = 0;
            }
            if ((buf[0] == 0xFF) && (buf[1] == 0x51))
            {
                buf[3] = 0x07; // change tempo to 500000
                buf[4] = 0xA1;
                buf[5] = 0x20;
            }
            buf+=evlen;
        }
        else
        {
            do
            {
                size++;
                delta_time>>=7;
            } while (delta_time) ;
        }
        num_events--;
        events++;
    };

    return size;
}

uint8_t *xmi2mid(const uint8_t *xmi, int seq_num, unsigned int *midi_size)
{
    int max_events, max_noteoffs, num_events, num_noteoffs, end, noteoff_index;
    unsigned int status, events_size;
    uint32_t seq_len, last_time, interval;
    const uint8_t *end_chunk, *event_data;
    uint8_t *midi;
    midi_event *events, *note_offs, *cur_event;


    xmi = xmi_find_sequence(xmi, seq_num, &seq_len);
    if (xmi == NULL) return NULL;

    end_chunk = xmi + seq_len;
    max_events = ((seq_len + 1) / 2) + 1;   // shortest event length is 2
    max_noteoffs = ((seq_len + 2) / 3) + 1; // note on event length is 3

    events = (midi_event *) malloc( (max_events + 2*max_noteoffs) * sizeof(midi_event));
    if (events == NULL) return NULL;

    cur_event = events;
    note_offs = events + max_events + max_noteoffs;

    num_events = 0;
    num_noteoffs = 0;
    last_time = 0;
    end = 0;

    // read events
    while (xmi < end_chunk && !end)
    {
        READ_INTERVAL(interval, xmi);

        last_time+=interval;

        // add note offs to list of events
        if ((num_noteoffs != 0) && (note_offs[0].time <= last_time))
        {
            noteoff_index = 1;
            while ((noteoff_index < num_noteoffs) && (note_offs[noteoff_index].time <= last_time))
            {
                noteoff_index++;
            }

            memcpy(cur_event, note_offs, noteoff_index * sizeof(midi_event));
            cur_event += noteoff_index;
            num_events += noteoff_index;
            note_offs += noteoff_index;
            num_noteoffs -= noteoff_index;
        }

        event_data = xmi;
        status = *xmi;
        xmi++;

        switch (status >> 4)
        {
            case MIDI_STATUS_NOTE_ON:
                cur_event->time = last_time;
                cur_event->length = 3;
                cur_event->data = event_data;
                cur_event++;
                num_events++;
                xmi+=2;

                READ_VARLEN(interval, xmi);

                if (interval != 0)
                {
                    // insert note off to sorted list of note offs

                    uint32_t insert_time;

                    noteoff_index = 0;
                    insert_time = last_time + interval;
                    while ((noteoff_index < num_noteoffs) && (note_offs[noteoff_index].time <= insert_time))
                    {
                        noteoff_index++;
                    }
                    if (noteoff_index < num_noteoffs)
                    {
                        memmove(&(note_offs[noteoff_index + 1]), &(note_offs[noteoff_index]), (num_noteoffs - noteoff_index) * sizeof(midi_event));
                    }

                    note_offs[noteoff_index].time = insert_time;
                    note_offs[noteoff_index].length = -3;
                    note_offs[noteoff_index].data = event_data;
                    num_noteoffs++;
                }
                else
                {
                    // add note off to list of events
                    cur_event->time = last_time;
                    cur_event->length = -3;
                    cur_event->data = event_data;
                    cur_event++;
                    num_events++;
                }

                break;

            //case MIDI_STATUS_NOTE_OFF:
            case MIDI_STATUS_AFTERTOUCH:
            case MIDI_STATUS_CONTROLLER:
            case MIDI_STATUS_PITCH_WHEEL:
                cur_event->time = last_time;
                cur_event->length = 3;
                cur_event->data = event_data;
                cur_event++;
                num_events++;
                xmi+=2;

                break;

            case MIDI_STATUS_PROG_CHANGE:
            case MIDI_STATUS_PRESSURE:
                cur_event->time = last_time;
                cur_event->length = 2;
                cur_event->data = event_data;
                cur_event++;
                num_events++;
                xmi++;

                break;

            case MIDI_STATUS_SYSEX:
                if (status == 0xff)
                {
                    unsigned int data;

                    data = *xmi;
                    xmi++;

                    if (data == 0x2f) // End Of Track
                    {
                        end = 1;
                    }
                    else if (data == 0x51) // Set Tempo
                    {
                        if ((last_time != 0) && (interval == 0))
                        {
                            READ_VARLEN(interval, xmi);
                            xmi+=interval;
                            break;
                        }
                    }
                }

                READ_VARLEN(interval, xmi);
                xmi+=interval;

                cur_event->time = last_time;
                cur_event->length = (int32_t)(xmi - event_data);
                cur_event->data = event_data;
                cur_event++;
                num_events++;

                break;

            default:
                break;

        }
    } ;

    if (num_noteoffs != 0)
    {
        memcpy(cur_event, note_offs, num_noteoffs * sizeof(midi_event));
        cur_event += num_noteoffs;
        num_events += num_noteoffs;
        note_offs += num_noteoffs;
        num_noteoffs = 0;
    }


    cur_event->time = 0xffffffff;
    cur_event->length = 0;
    cur_event->data = NULL;

    // get midi events size
    events_size = writeevents(NULL, events, num_events);

    midi = (uint8_t *) malloc(14 + 8 + events_size);

    if (midi == NULL)
    {
        free(events);
        return NULL;
    }

    if (midi_size)
    {
        *midi_size = 14 + 8 + events_size;
    }


    // header
    midi[0] = 'M';
    midi[1] = 'T';
    midi[2] = 'h';
    midi[3] = 'd';
    // header size
    midi[4] = 0;
    midi[5] = 0;
    midi[6] = 0;
    midi[7] = 6;
    // format type
    midi[8] = 0;
    midi[9] = 0;
    // number of tracks
    midi[10] = 0;
    midi[11] = 1;
    // time division
    midi[12] = 0;
    midi[13] = 60;

    // chunk
    midi[14] = 'M';
    midi[15] = 'T';
    midi[16] = 'r';
    midi[17] = 'k';
    // chunk length
    midi[18] = (events_size >> 24) & 0xff;
    midi[19] = (events_size >> 16) & 0xff;
    midi[20] = (events_size >> 8) & 0xff;
    midi[21] = events_size & 0xff;

    // write midi events
    writeevents(&(midi[14 + 8]), events, num_events);

    free(events);

    return midi;

// http://jedi.ks.uiuc.edu/~johns/links/music/midifile.html
// http://www.sonicspot.com/guide/midifiles.html
// http://www.borg.com/~jglatt/tech/midifile.htm
}

