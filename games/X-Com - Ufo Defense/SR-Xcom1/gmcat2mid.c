/**
 *
 *  Copyright (C) 2016-2021 Roman Pauer
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
    uint32_t midi_size_melodic, midi_size_rhythm;
} subsequence;

typedef struct _seq_track_
{
    uint8_t *data;
    uint32_t midi_size, channel;
} seq_track;


enum _midi_device_ {
    DEVICE_GM,
    DEVICE_MT32
};


static const int8_t instrument_velocity[128] = {
0x64, 0x64, 0x64, 0x64, 0x64, 0x5A, 0x64, 0x64, 0x64, 0x64, 0x64, 0x5A, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x55, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x5A, 0x5A, 0x6E, 0x50,
0x64, 0x64, 0x64, 0x5A, 0x46, 0x64, 0x64, 0x64, 0x46, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x5A, 0x64, 0x64, 0x64, 0x7F, 0x64, 0x64, 0x6E, 0x64, 0x64, 0x5A, 0x78, 0x64, 0x7F,
0x64, 0x64, 0x5A, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x5F, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x73, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64 };


static const int8_t midi_gm2mt32[128] = {
    // Piano
    5, // 1 Acoustic Grand Piano -> Elec Piano 3
    -1, // 2 Bright Acoustic Piano
    2, // 3 Electric Grand Piano -> Acou Piano 3
    -1, // 4 Honky-tonk Piano
    3, // 5 Electric Piano 1 -> Elec Piano 1
    -1, // 6 Electric Piano 2
    -1, // 7 Harpsichord
    21, // 8 Clavinet -> Clavi 3
    // Chromatic Percussion
    -1, // 9 Celesta
    101, // 10 Glockenspiel -> Glock
    -1, // 11 Music Box
    -1, // 12 Vibraphone
    -1, // 13 Marimba
    -1, // 14 Xylophone
    -1, // 15 Tubular Bells
    -1, // 16 Dulcimer
    // Organ
    -1, // 17 Drawbar Organ
    -1, // 18 Percussive Organ
    -1, // 19 Rock Organ
    -1, // 20 Church Organ
    -1, // 21 Reed Organ
    -1, // 22 Accordion
    -1, // 23 Harmonica
    -1, // 24 Tango Accordion
    // Guitar
    -1, // 25 Acoustic Guitar (nylon)
    -1, // 26 Acoustic Guitar (steel)
    -1, // 27 Electric Guitar (jazz)
    59, // 28 Electric Guitar (clean) -> Guitar 1
    -1, // 29 Electric Guitar (muted)
    62, // 30 Electric Guitar (overdriven) -> Elec Guitar 2
    48, // 31 Electric Guitar (distortion) -> Str Sect 1
    -1, // 32 Electric Guitar (harmonics)
    // Bass
    -1, // 33 Acoustic Bass
    -1, // 34 Electric Bass (finger)
    65, // 35 Electric Bass (picked) -> Acou Bass 2
    -1, // 36 Fretless Bass
    68, // 37 Slap Bass 1 -> Slap Bass 1
    69, // 38 Slap Bass 2 -> Slap Bass 2
    -1, // 39 Synth Bass 1
    -1, // 40 Synth Bass 2
    // Strings
    -1, // 41 Violin
    -1, // 42 Viola
    -1, // 43 Cello
    -1, // 44 Contrabass
    -1, // 45 Tremolo Strings
    -1, // 46 Pizzicato Strings
    -1, // 47 Orchestral Harp
    112, // 48 Timpani -> Timpani
    // Ensemble
    52, // 49 String Ensemble 1 -> Violin 1
    -1, // 50 String Ensemble 2
    49, // 51 Synth Strings 1 -> Str Sect 2
    -1, // 52 Synth Strings 2
    34, // 53 Choir Aahs -> Chorale
    -1, // 54 Voice Oohs
    -1, // 55 Synth Voice or Solo Vox
    -1, // 56 Orchestra Hit
    // Brass
    -1, // 57 Trumpet
    -1, // 58 Trombone
    -1, // 59 Tuba
    -1, // 60 Muted Trumpet
    -1, // 61 French Horn
    -1, // 62 Brass Section
    42, // 63 Synth Brass 1 -> Oboe 2001
    43, // 64 Synth Brass 2 -> Echo Pan
    // Reed
    -1, // 65 Soprano Sax
    -1, // 66 Alto Sax
    -1, // 67 Tenor Sax
    -1, // 68 Baritone Sax
    -1, // 69 Oboe
    -1, // 70 English Horn
    -1, // 71 Bassoon
    -1, // 72 Clarinet
    // Pipe
    -1, // 73 Piccolo
    -1, // 74 Flute
    -1, // 75 Recorder
    -1, // 76 Pan Flute
    -1, // 77 Blown bottle
    -1, // 78 Shakuhachi
    -1, // 79 Whistle
    -1, // 80 Ocarina
    // Synth Lead
    -1, // 81 Lead 1 (square)
    -1, // 82 Lead 2 (sawtooth)
    -1, // 83 Lead 3 (calliope)
    -1, // 84 Lead 4 (chiff)
    -1, // 85 Lead 5 (charang)
    -1, // 86 Lead 6 (space voice)
    -1, // 87 Lead 7 (fifths)
    -1, // 88 Lead 8 (bass and lead)
    // Synth Pad
    -1, // 89 Pad 1 (new age or fantasia)
    33, // 90 Pad 2 (warm) -> Harmo Pan
    -1, // 91 Pad 3 (polysynth or poly)
    -1, // 92 Pad 4 (choir)
    -1, // 93 Pad 5 (bowed glass or bowed)
    -1, // 94 Pad 6 (metallic)
    -1, // 95 Pad 7 (halo)
    -1, // 96 Pad 8 (sweep)
    // Synth Effects
    -1, // 97 FX 1 (rain)
    -1, // 98 FX 2 (soundtrack)
    -1, // 99 FX 3 (crystal)
    -1, // 100 FX 4 (atmosphere)
    -1, // 101 FX 5 (brightness)
    -1, // 102 FX 6 (goblins)
    -1, // 103 FX 7 (echoes or echo drops)
    -1, // 104 FX 8 (sci-fi or star theme)
    // Ethnic
    -1, // 105 Sitar
    -1, // 106 Banjo
    -1, // 107 Shamisen
    -1, // 108 Koto
    -1, // 109 Kalimba
    -1, // 110 Bag pipe
    -1, // 111 Fiddle
    -1, // 112 Shanai
    // Percussive
    -1, // 113 Tinkle Bell
    -1, // 114 Agogô
    -1, // 115 Steel Drums
    -1, // 116 Woodblock
    -1, // 117 Taiko Drum
    -1, // 118 Melodic Tom or 808 Toms
    -1, // 119 Synth Drum
    -1, // 120 Reverse Cymbal
    // Sound Effects
    -1, // 121 Guitar Fret Noise
    -1, // 122 Breath Noise
    -1, // 123 Seashore
    -1, // 124 Bird Tweet
    -1, // 125 Telephone Ring
    -1, // 126 Helicopter
    -1, // 127 Applause
    -1, // 128 Gunshot
};


static void write_track(uint32_t channel, uint8_t *midi, uint8_t *track, subsequence *subsequences, unsigned int time_division, uint32_t *program_number, int *midi_loop, enum _midi_device_ device)
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

        if (track[0] >= 0x80) // not running status
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
                    midi[2] = 0; // note off
                }
                else if (device == DEVICE_MT32)
                {
                    midi[0] = MIDI_STATUS_NOTE_ON | channel;
                    midi[1] = track[0];
                    midi[2] = track[1];
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
                if (device == DEVICE_MT32 && track[0] == 7 && channel == 9) // Main Volume on rhythm channel
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
                    midi[2] = track[1] - 3;

                    midi+=3;
                    track+=2;

                    break;
                }

                if ( (device == DEVICE_GM && track[0] == 0x7e) || (track[0] == 0 && track[1] == 0) )
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

                if (track[0] == 0) // change tempo
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
                else if (device == DEVICE_GM && track[0] == 0x5b) // Effects 1 Depth (formerly External Effects Depth)
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
                    midi[2] = 30;

                    midi+=3;
                    track+=2;

                    break;
                }

                // fallthrough
            case MIDI_STATUS_AFTERTOUCH: // ignored by the driver
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
                if (track[0] == 0x7e) // enable looping
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

                if (device == DEVICE_MT32 && channel == 9)
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

                    track++;
                    break;
                }

                prog_num = track[0];
                if (program_number != NULL)
                {
                    *program_number = prog_num;
                }

                if (device == DEVICE_GM && (track[0] == 0x57 || track[0] == 0x3f)) // Lead 8 (bass and lead) / Synth Brass 2
                {
                    while (interval_size != 0)
                    {
                        *midi = *oldtrack;
                        midi++;
                        oldtrack++;
                        interval_size--;
                    }
                    midi[0] = event_type | channel;
                    midi[1] = 0x3e; // Synth Brass 1

                    midi+=2;
                    track++;

                    break;
                }

                // fallthrough
            case MIDI_STATUS_PRESSURE: // ignored by the driver
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
                    case 0xff: // end of track
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
                    case 0xfe: // play subsequence
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

                        write_track(channel, midi, subsequences[track[0]].data, subsequences, time_division, &prog_num, midi_loop, device);
                        midi += (channel == 9) ? subsequences[track[0]].midi_size_rhythm : subsequences[track[0]].midi_size_melodic;
                        track++;
                        break;
                    case 0xfd: // return from subsequence
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

static unsigned int get_midi_size(uint8_t *gmcat, subsequence *subsequences, enum _midi_device_ device, int rhythm_channel)
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

        if (gmcat[0] >= 0x80) // not running status
        {
            last_event = gmcat[0];
            gmcat++;
        }
        switch (last_event & 0xf0)
        {
            case MIDI_STATUS_CONTROLLER:
                if ( (device == DEVICE_GM && gmcat[0] == 0x7e) || (gmcat[0] == 0 && gmcat[1] == 0) )
                {
                    if (interval)
                    {
                        midi_size += 3 + interval_size;
                    }

                    gmcat+=2;
                    break;
                }

                if (gmcat[0] == 0) // change tempo
                {
                    midi_size += 6 + interval_size;
                    gmcat+=2;
                    break;
                }

                // fallthrough
            case MIDI_STATUS_NOTE_OFF:
            case MIDI_STATUS_NOTE_ON:
            case MIDI_STATUS_AFTERTOUCH: // ignored by the driver
            case MIDI_STATUS_PITCH_WHEEL:
                midi_size += 3 + interval_size;
                gmcat+=2;
                break;

            case MIDI_STATUS_PROG_CHANGE:
                if ((gmcat[0] == 0x7e) || // enable looping
                    (device == DEVICE_MT32 && rhythm_channel)
                   )
                {
                    if (interval)
                    {
                        midi_size += 3 + interval_size;
                    }

                    gmcat++;
                    break;
                }

                // fallthrough
            case MIDI_STATUS_PRESSURE: // ignored by the driver
                midi_size += 2 + interval_size;
                gmcat++;
                break;

            case MIDI_STATUS_SYSEX:
                switch (last_event)
                {
                    case 0xff: // end of track
                        return midi_size + interval_size + 3;
                    case 0xfe: // play subsequence
                        if (interval)
                        {
                            midi_size += 3 + interval_size;
                        }

                        if (rhythm_channel)
                        {
                            if (subsequences[gmcat[0]].midi_size_rhythm == 0)
                            {
                                // count subsequence midi size (rhythm)
                                subsequences[gmcat[0]].midi_size_rhythm = get_midi_size(subsequences[gmcat[0]].data, subsequences, device, 1);
                            }
                            midi_size += subsequences[gmcat[0]].midi_size_rhythm;
                        }
                        else
                        {
                            if (subsequences[gmcat[0]].midi_size_melodic == 0)
                            {
                                // count subsequence midi size (melodic)
                                subsequences[gmcat[0]].midi_size_melodic = get_midi_size(subsequences[gmcat[0]].data, subsequences, device, 0);
                            }
                            midi_size += subsequences[gmcat[0]].midi_size_melodic;
                        }
                        gmcat++;
                        break;
                    case 0xfd: // return from subsequence
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

static uint8_t *midicat2mid(enum _midi_device_ device, uint8_t *gmcat, unsigned int *res_midi_size, int *midi_loop)
{
    uint8_t *midi, *midi_track;
    unsigned int time_division, num_subsequences, num_tracks, midi_size, loop;

    subsequence subsequences[256];
    seq_track tracks[16];

    time_division = ((unsigned int) (gmcat[0])) / 5;
    num_subsequences = gmcat[1];
    gmcat+=2;

    // read subsequences
    for (loop = 0; loop < num_subsequences; loop++)
    {
        register unsigned int subsequence_len;

        subsequence_len = GETU16FLE(gmcat);
        subsequences[loop].data = gmcat+4;
        subsequences[loop].midi_size_melodic = 0;
        subsequences[loop].midi_size_rhythm = 0;
        gmcat+=subsequence_len;
    }

    num_tracks = gmcat[0];
    gmcat++;

    // read tracks
    for (loop = 0; loop < num_tracks; loop++)
    {
        register unsigned int track_len;

        tracks[loop].channel = gmcat[0];
        gmcat++;

        track_len = GETU16FLE(gmcat);
        tracks[loop].data = gmcat+4;
        gmcat+=track_len;
    }

    midi_size = 14;
    // count final midi size
    for (loop = 0; loop < num_tracks; loop++)
    {
        tracks[loop].midi_size = get_midi_size(tracks[loop].data, &(subsequences[0]), device, (tracks[loop].channel == 9)?1:0);
        midi_size += 8 + tracks[loop].midi_size;
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

        write_track(tracks[loop].channel, &(midi_track[8]), tracks[loop].data, &(subsequences[0]), time_division, NULL, midi_loop, device);

        midi_track += 8 + tracks[loop].midi_size;
    }

    return midi;
}

uint8_t *gmcat2mid(uint8_t *gmcat, unsigned int *res_midi_size, int *midi_loop)
{
    return midicat2mid(DEVICE_GM, gmcat, res_midi_size, midi_loop);
}

uint8_t *rolandcat2mid(uint8_t *rolandcat, unsigned int *res_midi_size, int *midi_loop)
{
    return midicat2mid(DEVICE_MT32, rolandcat, res_midi_size, midi_loop);
}


static void remap_track(uint8_t *track, subsequence *subsequences)
{
    unsigned int last_event, event_type, interval;

    last_event = 0xff;
    event_type = 0xf0;

    while (1)
    {
        READ_VARLEN(interval, track);

        if (track[0] >= 0x80) // not running status
        {
            last_event = track[0];
            event_type = track[0] & 0xf0;
            track++;
        }
        switch (event_type)
        {
            case MIDI_STATUS_NOTE_OFF:
            case MIDI_STATUS_NOTE_ON:
            case MIDI_STATUS_CONTROLLER:
            case MIDI_STATUS_AFTERTOUCH: // ignored by the driver
            case MIDI_STATUS_PITCH_WHEEL:
                track+=2;
                break;

            case MIDI_STATUS_PROG_CHANGE:
                if (track[0] == 0x7e) // enable looping
                {
                    track++;
                    break;
                }

                if (midi_gm2mt32[track[0]] < 0)
                {
                    // unmapped instrument
                }
                else
                {
                    track[0] = midi_gm2mt32[track[0]];
                }

                // fallthrough
            case MIDI_STATUS_PRESSURE: // ignored by the driver
                track++;
                break;

            case MIDI_STATUS_SYSEX:
                switch (last_event)
                {
                    case 0xff: // end of track
                        return;
                    case 0xfe: // play subsequence
                        if (subsequences[track[0]].midi_size_melodic == 0)
                        {
                            subsequences[track[0]].midi_size_melodic = 1;
                            remap_track(subsequences[track[0]].data, subsequences);
                        }

                        track++;
                        break;
                    case 0xfd: // return from subsequence
                        return;
                }
            break;
        }
    }
}

void gmcat2rolandcat(uint8_t *gmcat)
{
    unsigned int num_subsequences, num_tracks, loop;

    subsequence subsequences[256];
    seq_track tracks[16];

    num_subsequences = gmcat[1];
    gmcat+=2;

    // read subsequences
    for (loop = 0; loop < num_subsequences; loop++)
    {
        register unsigned int subsequence_len;

        subsequence_len = GETU16FLE(gmcat);
        subsequences[loop].data = gmcat+4;
        subsequences[loop].midi_size_melodic = 0;
        gmcat+=subsequence_len;
    }

    num_tracks = gmcat[0];
    gmcat++;

    // read tracks
    for (loop = 0; loop < num_tracks; loop++)
    {
        register unsigned int track_len;

        tracks[loop].channel = gmcat[0];
        gmcat++;

        track_len = GETU16FLE(gmcat);
        tracks[loop].data = gmcat+4;
        gmcat+=track_len;
    }

    // remap midi tracks
    for (loop = 0; loop < num_tracks; loop++)
    {
        if (tracks[loop].channel != 9)
        {
            remap_track(tracks[loop].data, &(subsequences[0]));
        }
    }
}

// http://jedi.ks.uiuc.edu/~johns/links/music/midifile.html
// http://www.sonicspot.com/guide/midifiles.html
