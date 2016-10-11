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

#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include "gmcat2mid.h"



#define GETU32FBE(buf) (			\
            (uint32_t) ( (buf)[0] ) << 24 | \
            (uint32_t) ( (buf)[1] ) << 16 | \
            (uint32_t) ( (buf)[2] ) <<  8 | \
            (uint32_t) ( (buf)[3] )       )

#define GETU16FBE(buf) (			\
            (uint32_t) ( (buf)[0] ) <<  8 | \
            (uint32_t) ( (buf)[1] )       )

#define READU32BE(res, buf) {		\
            res = GETU32FBE(buf);	\
            buf+=4;					}

#define GETU32FLE(buf) (			\
            (uint32_t) ( (buf)[0] )       | \
            (uint32_t) ( (buf)[1] ) <<  8 | \
            (uint32_t) ( (buf)[2] ) << 16 | \
            (uint32_t) ( (buf)[3] ) << 24 )

#define GETU16FLE(buf) (			\
            (uint32_t) ( (buf)[0] )       | \
            (uint32_t) ( (buf)[1] ) <<  8 )

#define READU32LE(res, buf) {		\
            res = GETU32FLE(buf);	\
            buf+=4;					}

#define READ_VARLEN(res, buf) {		\
            unsigned int ___data;						\
            res = 0;									\
            do {										\
                ___data = *(buf);						\
                buf++;									\
                res = (res << 7) | (___data & 0x7f);	\
            } while (___data & 0x80);					}

// Midi Status Bytes
#define MIDI_STATUS_NOTE_OFF		0x80
#define MIDI_STATUS_NOTE_ON			0x90
#define MIDI_STATUS_AFTERTOUCH		0xA0
#define MIDI_STATUS_CONTROLLER		0xB0
#define MIDI_STATUS_PROG_CHANGE		0xC0
#define MIDI_STATUS_PRESSURE		0xD0
#define MIDI_STATUS_PITCH_WHEEL		0xE0
#define MIDI_STATUS_SYSEX			0xF0


typedef struct _subsequence_
{
    uint8_t *data;
    uint32_t midi_size;
} subsequence;

typedef struct _seq_track_
{
    uint8_t *data;
    uint32_t midi_size, channel;
} seq_track;


static const int8_t instrument_velocity[128] = {
0x64, 0x64, 0x64, 0x64, 0x64, 0x5A, 0x64, 0x64, 0x64, 0x64, 0x64, 0x5A, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x55, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x5A, 0x5A, 0x6E, 0x50,
0x64, 0x64, 0x64, 0x5A, 0x46, 0x64, 0x64, 0x64, 0x46, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x5A, 0x64, 0x64, 0x64, 0x7F, 0x64, 0x64, 0x6E, 0x64, 0x64, 0x5A, 0x78, 0x64, 0x7F,
0x64, 0x64, 0x5A, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x5F, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x73, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64 };

static void write_track(uint32_t channel, uint8_t *midi, uint8_t *track, subsequence *subsequences, unsigned int time_division, uint32_t *program_number, int *midi_loop)
{
    unsigned int last_event, event_type, interval;
    uint32_t prog_num;

    last_event = 0xff;
    event_type = 0xf0;
    if (program_number != NULL)
    {
        prog_num = *program_number;
    }
    else
    {
        prog_num = 0xff;
    }

    while (1)
    {
        unsigned int interval_size;
        uint8_t *oldtrack;

        oldtrack = track;
        READ_VARLEN(interval, track);
        interval_size = track - oldtrack;

        if (track[0] >= 0x80)
        {
            last_event = track[0];
            event_type = track[0] & 0xf0;
            track++;
        }
        switch (event_type)
        {
            case MIDI_STATUS_NOTE_OFF:
            case MIDI_STATUS_NOTE_ON:
                while (interval_size != 0)
                {
                    *midi = *oldtrack;
                    midi++;
                    oldtrack++;
                    interval_size--;
                }

                if (track[1] == 0)
                {
                    midi[0] = MIDI_STATUS_NOTE_ON | channel;
                    midi[1] = track[0];
                    midi[2] = 0;
                }
                else if (channel == 9)
                {
                    midi[0] = MIDI_STATUS_NOTE_ON | channel;
                    midi[1] = track[0];
                    midi[2] = (( (int) ((int8_t) (track[1])) ) * 0x50) / 128;
                }
                else
                {
                    midi[0] = MIDI_STATUS_NOTE_ON | channel;
                    midi[1] = track[0];
                    midi[2] = (( (int) ((int8_t) (track[1])) ) * instrument_velocity[prog_num]) / 128;
                }

                midi+=3;
                track+=2;

                break;

            case MIDI_STATUS_CONTROLLER:
                if ( (track[0] == 0x7e) || (track[0] == 0 && track[1] == 0) )
                {
                    // don't write event if possible, otherwise write meaningless event
                    if (interval)
                    {
                        while (interval_size != 0)
                        {
                            *midi = *oldtrack;
                            midi++;
                            oldtrack++;
                            interval_size--;
                        }
                        midi[0] = 0xff; // meta event
                        midi[1] = 0x06; // marker
                        midi[2] = 0;    // event length

                        midi+=3;
                    }

                    track+=2;
                    break;
                }

                if (track[0] == 0)
                {
                    unsigned int newtempo;

                    while (interval_size != 0)
                    {
                        *midi = *oldtrack;
                        midi++;
                        oldtrack++;
                        interval_size--;
                    }
                    midi[0] = 0xff; // meta event
                    midi[1] = 0x51; // set tempo
                    midi[2] = 3;    // event length

                    newtempo = 60000000 / ( (120 * ( (((unsigned int) (track[1])) * 2) / 5 ) ) / time_division );

                    midi[3] = (newtempo >> 16) & 0xff;  // tempo
                    midi[4] = (newtempo >> 8) & 0xff;   // tempo
                    midi[5] = newtempo & 0xff;          // tempo

                    midi+=6;
                    track+=2;

                    break;
                }
                else if (track[0] == 0x5b)
                {
                    while (interval_size != 0)
                    {
                        *midi = *oldtrack;
                        midi++;
                        oldtrack++;
                        interval_size--;
                    }
                    midi[0] = event_type | channel;
                    midi[1] = track[0];
                    midi[2] = track[1];

                    midi+=3;
                    track+=2;

                    break;
                }

            case MIDI_STATUS_AFTERTOUCH:
            case MIDI_STATUS_PITCH_WHEEL:
                while (interval_size != 0)
                {
                    *midi = *oldtrack;
                    midi++;
                    oldtrack++;
                    interval_size--;
                }
                midi[0] = event_type | channel;
                midi[1] = track[0];
                midi[2] = track[1];

                midi+=3;
                track+=2;

                break;

            case MIDI_STATUS_PROG_CHANGE:
                if (track[0] == 0x7e)
                {
                    // don't write event if possible, otherwise write meaningless event
                    if (interval)
                    {
                        while (interval_size != 0)
                        {
                            *midi = *oldtrack;
                            midi++;
                            oldtrack++;
                            interval_size--;
                        }
                        midi[0] = 0xff; // meta event
                        midi[1] = 0x06; // marker
                        midi[2] = 0;    // event length

                        midi+=3;
                    }

                    if (midi_loop != NULL)
                    {
                        *midi_loop = 1;
                    }

                    track++;
                    break;
                }

                prog_num = track[0];
                if (program_number != NULL)
                {
                    *program_number = prog_num;
                }

                if (track[0] == 0x57 || track[0] == 0x3f)
                {
                    while (interval_size != 0)
                    {
                        *midi = *oldtrack;
                        midi++;
                        oldtrack++;
                        interval_size--;
                    }
                    midi[0] = event_type | channel;
                    midi[1] = 0x3e;

                    midi+=2;
                    track++;

                    break;
                }

            case MIDI_STATUS_PRESSURE:
                while (interval_size != 0)
                {
                    *midi = *oldtrack;
                    midi++;
                    oldtrack++;
                    interval_size--;
                }
                midi[0] = event_type | channel;
                midi[1] = track[0];

                midi+=2;
                track++;

                break;

            case MIDI_STATUS_SYSEX:
                switch (last_event)
                {
                    case 0xff:
                        while (interval_size != 0)
                        {
                            *midi = *oldtrack;
                            midi++;
                            oldtrack++;
                            interval_size--;
                        }
                        midi[0] = 0xff; // meta event
                        midi[1] = 0x2f; // end of track
                        midi[2] = 0;    // event length

                        return;
                    case 0xfe:
                        // don't write event if possible, otherwise write meaningless event
                        if (interval)
                        {
                            while (interval_size != 0)
                            {
                                *midi = *oldtrack;
                                midi++;
                                oldtrack++;
                                interval_size--;
                            }
                            midi[0] = 0xff; // meta event
                            midi[1] = 0x06; // marker
                            midi[2] = 0;    // event length

                            midi+=3;
                        }

                        write_track(channel, midi, subsequences[track[0]].data, subsequences, time_division, &prog_num, midi_loop);
                        midi += subsequences[track[0]].midi_size;
                        track++;
                        break;
                    case 0xfd:
                        // don't write event if possible, otherwise write meaningless event
                        if (interval)
                        {
                            while (interval_size != 0)
                            {
                                *midi = *oldtrack;
                                midi++;
                                oldtrack++;
                                interval_size--;
                            }
                            midi[0] = 0xff; // meta event
                            midi[1] = 0x06; // marker
                            midi[2] = 0;    // event length
                        }

                        return;
                }
            break;
        }
    }
}

static unsigned int get_midi_size(uint8_t *gmcat, subsequence *subsequences)
{
    unsigned int last_event, interval, midi_size;

    last_event = 0xff;
    midi_size = 0;

    while (1)
    {
        unsigned int interval_size;
        uint8_t *oldgmcat;

        oldgmcat = gmcat;
        READ_VARLEN(interval, gmcat);
        interval_size = gmcat - oldgmcat;

        if (gmcat[0] >= 0x80)
        {
            last_event = gmcat[0];
            gmcat++;
        }
        switch (last_event & 0xf0)
        {
            case MIDI_STATUS_CONTROLLER:
                if ( (gmcat[0] == 0x7e) || (gmcat[0] == 0 && gmcat[1] == 0) )
                {
                    if (interval)
                    {
                        midi_size += 3 + interval_size;
                    }

                    gmcat+=2;
                    break;
                }

                if (gmcat[0] == 0)
                {
                    midi_size += 6 + interval_size;
                    gmcat+=2;
                    break;
                }

            case MIDI_STATUS_NOTE_OFF:
            case MIDI_STATUS_NOTE_ON:
            case MIDI_STATUS_AFTERTOUCH:
            case MIDI_STATUS_PITCH_WHEEL:
                midi_size += 3 + interval_size;
                gmcat+=2;
                break;

            case MIDI_STATUS_PROG_CHANGE:
                if (gmcat[0] == 0x7e)
                {
                    if (interval)
                    {
                        midi_size += 3 + interval_size;
                    }

                    gmcat++;
                    break;
                }

            case MIDI_STATUS_PRESSURE:
                midi_size += 2 + interval_size;
                gmcat++;
                break;

            case MIDI_STATUS_SYSEX:
                switch (last_event)
                {
                    case 0xff:
                        return midi_size + interval_size + 3;
                    case 0xfe:
                        if (interval)
                        {
                            midi_size += 3 + interval_size;
                        }

                        midi_size += subsequences[gmcat[0]].midi_size;
                        gmcat++;
                        break;
                    case 0xfd:
                        if (interval)
                        {
                            midi_size += 3 + interval_size;
                        }

                        return midi_size;
                }
            break;
        }
    }
}

uint8_t *gmcat2mid(uint8_t *gmcat, unsigned int *res_midi_size, int *midi_loop)
{
    uint8_t *midi, *midi_track;
    unsigned int time_division, num_subsequences, num_tracks, midi_size;

    subsequence subsequences[256];
    seq_track tracks[16];

    time_division = ((unsigned int) (gmcat[0])) / 5;
    num_subsequences = gmcat[1];
    gmcat+=2;

    // read subsequences
    {
        register unsigned int loop;
        for (loop = 0; loop < num_subsequences; loop++)
        {
            register unsigned int subsequence_len;

            subsequence_len = GETU16FLE(gmcat);
            subsequences[loop].data = gmcat+4;
            gmcat+=subsequence_len;
        }
    }

    num_tracks = gmcat[0];
    gmcat++;

    // read tracks
    {
        register unsigned int loop;
        for (loop = 0; loop < num_tracks; loop++)
        {
            register unsigned int track_len;

            tracks[loop].channel = gmcat[0];
            gmcat++;

            track_len = GETU16FLE(gmcat);
            tracks[loop].data = gmcat+4;
            gmcat+=track_len;
        }
    }

    // count subsequences midi size
    {
        unsigned int loop;
        for (loop = 0; loop < num_subsequences; loop++)
        {
            subsequences[loop].midi_size = get_midi_size(subsequences[loop].data, &(subsequences[0]));
        }
    }

    midi_size = 14;
    // count final midi size
    {
        unsigned int loop;
        for (loop = 0; loop < num_tracks; loop++)
        {
            tracks[loop].midi_size = get_midi_size(tracks[loop].data, &(subsequences[0]));
            midi_size += 8 + tracks[loop].midi_size;
        }
    }

    if (res_midi_size != NULL)
    {
        *res_midi_size = midi_size;
    }

    if (midi_loop != NULL)
    {
        *midi_loop = 0;
    }

    midi = (uint8_t *) malloc(midi_size);

    if (midi == NULL)
    {
        return NULL;
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
    midi[9] = (num_tracks == 1)?0:1;
    // number of tracks
    midi[10] = 0;
    midi[11] = num_tracks;
    // time division
    midi[12] = (time_division >> 8) & 0xff;
    midi[13] = time_division & 0xff;

    midi_track = &(midi[14]);

    // write midi tracks
    {
        unsigned int loop;
        for (loop = 0; loop < num_tracks; loop++)
        {
            // chunk
            midi_track[0] = 'M';
            midi_track[1] = 'T';
            midi_track[2] = 'r';
            midi_track[3] = 'k';
            // chunk length
            midi_track[4] = (tracks[loop].midi_size >> 24) & 0xff;
            midi_track[5] = (tracks[loop].midi_size >> 16) & 0xff;
            midi_track[6] = (tracks[loop].midi_size >> 8) & 0xff;
            midi_track[7] = tracks[loop].midi_size & 0xff;

            write_track(tracks[loop].channel, &(midi_track[8]), tracks[loop].data, &(subsequences[0]), time_division, NULL, midi_loop);

            midi_track += 8 + tracks[loop].midi_size;
        }
    }


    return midi;

// http://jedi.ks.uiuc.edu/~johns/links/music/midifile.html
// http://www.sonicspot.com/guide/midifiles.html
}

