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

#if defined(__APPLE__)
    #define PLUGIN_ENABLED
#else
    #undef PLUGIN_ENABLED
#endif

#ifdef PLUGIN_ENABLED
    #include <AvailabilityMacros.h>
    #include <CoreMIDI/CoreMIDI.h>
    #include <mach/mach_time.h>
    #include <pthread.h>
    #include <limits.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "midi-plugins2.h"

static int midi_type;

#ifdef PLUGIN_ENABLED
#if defined(MAC_OS_VERSION_11_0) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_11_0
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_VERSION_11_0
#define MIDI_API 0
#else
#define MIDI_API 1
#endif
#else
#define MIDI_API -1
#endif

enum {
    NOTIFY_REMOVE   = 1 << 0,
    NOTIFY_ADD      = 1 << 1,
    NOTIFY_CHANGEID = 1 << 2
};

typedef struct {
    uint32_t tick;
    uint32_t len;
    union {
        void *sysex;
#if (MIDI_API >= 0)
        struct {
            uint32_t *sysex;
            uint32_t data;
        } ump;
#endif
#if (MIDI_API <= 0)
        struct {
            uint8_t *sysex;
            uint8_t data[4];
        } old;
#endif
    };
    uint64_t time;
} midi_event_info;

typedef struct {
    unsigned int time_division, last_tick, tempo, tempo_tick, num_allocated, num_events;
    uint64_t tempo_time, last_time, next_sysex_time;
    midi_event_info *events;
} preprocess_state;


static MIDIClientRef midi_client = 0;
static MIDIPortRef midi_port;
static MIDIEndpointRef midi_endpoint;
static int32_t midi_unique_id;
static pthread_t midi_thread;

#if (MIDI_API == 0)
static int use_ump;
#endif
static struct mach_timebase_info tb_info;

static volatile unsigned int notification_events = 0;

static volatile midi_event_info *midi_events = NULL;
static volatile unsigned int midi_current_event;
static volatile uint64_t midi_base_time, midi_last_time, midi_pause_time;

static pthread_mutex_t midi_mutex, notification_mutex;
static uint8_t midi_buf[65536];


static void calculate_next_sysex_tick(preprocess_state *state);
static void calculate_event_time(preprocess_state *state);
static int add_midi_event(preprocess_state *state, uint8_t status, uint8_t byte1, uint8_t byte2, const uint8_t *data_ptr, unsigned int data_len);
static void free_midi_data(midi_event_info *data);
static int send_initial_sysex_events(unsigned char const *sysex_events);

#include "midi2-common.h"


static uint64_t nsec2abstime(uint64_t nsec)
{
    uint64_t quot, rem;

    quot = nsec / tb_info.numer;
    rem = nsec % tb_info.numer;

    return (quot * tb_info.denom) + ((rem * tb_info.denom) / tb_info.numer);
    //return (nsec * tb_info.denom) / tb_info.numer;
}

static void calculate_next_sysex_tick(preprocess_state *state)
{
    state->last_tick = ( ((state->next_sysex_time - state->tempo_time) * state->time_division + (state->tempo * (uint64_t) 1000 - 1)) / (state->tempo * (uint64_t) 1000) ) + state->tempo_tick;
}

static void calculate_event_time(preprocess_state *state)
{
    div_t divres;

    // calculate event time in nanoseconds
    divres = div(state->last_tick - state->tempo_tick, state->time_division);

    state->last_time = ( ((1000 * divres.rem) * (uint64_t)state->tempo) / state->time_division )
                     + ( (divres.quot * (uint64_t)state->tempo) * 1000 )
                     + state->tempo_time
                     ;

    //state->last_time = ( (((state->last_tick - state->tempo_tick) * (uint64_t) 1000) * state->tempo) / state->time_division ) + state->tempo_time;
}

#if (MIDI_API >= 0)
static unsigned int convert_sysex_to_ump(uint32_t *ump_data, uint8_t status, const uint8_t *sysex_data, unsigned int sysex_len)
{
    int sysex_hasstart, sysex_hasend;
    unsigned int ump_len;

    sysex_hasstart = (status == 0xf0);
    sysex_hasend = (sysex_len != 0 && sysex_data[sysex_len - 1] == 0xf7);
    if (sysex_hasend) sysex_len--;

    if (sysex_len <= 6)
    {
        // one 64-bit UMP
        ump_data[0] = (kMIDIMessageTypeSysEx << 28) | ((sysex_hasstart ? (sysex_hasend ? kMIDISysExStatusComplete : kMIDISysExStatusStart) : (sysex_hasend ? kMIDISysExStatusEnd : kMIDISysExStatusContinue)) << 20) | (sysex_len << 16) | ((sysex_len > 0) ? (sysex_data[0] << 8) : 0) | ((sysex_len > 1) ? sysex_data[1] : 0);
        ump_data[1] = ((sysex_len > 2) ? (sysex_data[2] << 24) : 0) | ((sysex_len > 3) ? (sysex_data[3] << 16) : 0) | ((sysex_len > 4) ? (sysex_data[4] << 8) : 0) | ((sysex_len > 5) ? sysex_data[5] : 0);
        ump_len = 2;
    }
    else
    {
        ump_data[0] = (kMIDIMessageTypeSysEx << 28) | ((sysex_hasstart ? kMIDISysExStatusStart : kMIDISysExStatusContinue) << 20) | (6 << 16) | (sysex_data[0] << 8) | sysex_data[1];
        ump_data[1] = (sysex_data[2] << 24) | (sysex_data[3] << 16) | (sysex_data[4] << 8) | sysex_data[5];
        ump_len = 2;
        sysex_len -= 6;
        sysex_data += 6;
        while (sysex_len > 6)
        {
            ump_data[ump_len] = (kMIDIMessageTypeSysEx << 28) | (kMIDISysExStatusContinue << 20) | (6 << 16) | (sysex_data[0] << 8) | sysex_data[1];
            ump_data[ump_len + 1] = (sysex_data[2] << 24) | (sysex_data[3] << 16) | (sysex_data[4] << 8) | sysex_data[5];
            ump_len += 2;
            sysex_len -= 6;
            sysex_data += 6;
        }
        ump_data[ump_len] = (kMIDIMessageTypeSysEx << 28) | ((sysex_hasend ? kMIDISysExStatusEnd : kMIDISysExStatusContinue) << 20) | (sysex_len << 16) | ((sysex_len > 0) ? (sysex_data[0] << 8) : 0) | ((sysex_len > 1) ? sysex_data[1] : 0);
        ump_data[ump_len + 1] = ((sysex_len > 2) ? (sysex_data[2] << 24) : 0) | ((sysex_len > 3) ? (sysex_data[3] << 16) : 0) | ((sysex_len > 4) ? (sysex_data[4] << 8) : 0) | ((sysex_len > 5) ? sysex_data[5] : 0);
        ump_len += 2;
    }

    return ump_len;
}
#endif

static int add_midi_event(preprocess_state *state, uint8_t status, uint8_t byte1, uint8_t byte2, const uint8_t *data_ptr, unsigned int data_len)
{
    midi_event_info event;

    event.tick = state->last_tick;
    event.sysex = NULL;
    event.time = state->last_time;

    if ((status >> 4) != MIDI_STATUS_SYSEX)
    {
#if (MIDI_API >= 0)
#if (MIDI_API == 0)
        if (use_ump)
#endif
        {
            event.len = 1;
            event.ump.data = (kMIDIMessageTypeChannelVoice1 << 28) | (status << 16) | (byte1 << 8) | (byte2);
        }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
        else
#endif
        {
            event.len = ((status >> 4) == MIDI_STATUS_PROG_CHANGE || (status >> 4) == MIDI_STATUS_PRESSURE) ? 2 : 3;
            event.old.data[0] = status;
            event.old.data[1] = byte1;
            event.old.data[2] = byte2;
            event.old.data[3] = 0;
        }
#endif
    }
    else
    {
        if (status == 0xff) // meta events
        {
            if ((byte1 == 0x51) && (data_len == 3)) // set tempo
            {
                // time_division is assumed to be positive (ticks per beat / PPQN - Pulses (i.e. clocks) Per Quarter Note)

                event.len = 0;

                state->tempo = (((uint32_t)(data_ptr[0])) << 16) | (((uint32_t)(data_ptr[1])) << 8) | ((uint32_t)(data_ptr[2]));
                state->tempo_tick = event.tick;
                state->tempo_time = event.time;
            }
            else return 0;
        }
        else if ((status == 0xf0) || (status == 0xf7)) // sysex
        {
#if (MIDI_API >= 0)
#if (MIDI_API == 0)
            if (use_ump)
#endif
            {
                if (data_len == 0 && status == 0xf7) return 0;

                event.len = 2 * ((data_len + ((data_len != 0 && data_ptr[data_len - 1] == 0xf7) ? 4 : 5)) / 6);
                if (event.len == 0) event.len = 2;
                else if (event.len > (sizeof(midi_buf) - (offsetof(MIDIEventList, packet) + offsetof(MIDIEventPacket, words))) >> 2) return 0;

                event.ump.sysex = (uint32_t *) malloc(event.len * sizeof(uint32_t));
                if (event.ump.sysex == NULL) return 12;

                convert_sysex_to_ump(event.ump.sysex, status, data_ptr, data_len);
            }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
            else
#endif
            {
                event.len = data_len + ((status == 0xf0)?1:0);
                if (event.len == 0) return 0;
                else if (event.len > sizeof(midi_buf) - (offsetof(MIDIPacketList, packet) + offsetof(MIDIPacket, data))) return 0;

                event.old.sysex = (uint8_t *) malloc(event.len);
                if (event.old.sysex == NULL) return 12;

                if (status == 0xf0)
                {
                    event.old.sysex[0] = 0xf0;
                    memcpy(event.old.sysex + 1, data_ptr, data_len);
                }
                else
                {
                    memcpy(event.old.sysex, data_ptr, data_len);
                }
            }
#endif

            if (mt32_delay)
            {
                if (event.time < state->next_sysex_time)
                {
                    // calculate new event tick and time
                    calculate_next_sysex_tick(state);
                    calculate_event_time(state);

                    event.tick = state->last_tick;
                    event.time = state->last_time;
                }

                state->next_sysex_time = event.time + (40 + 10 + ((event.len * 10000 + 31249) / 31250)) * 1000000;
            }
        }
        else if ((status == 0xf4) && mt32_delay) // extra event
        {
            event.len = 0;
        }
        else return 0;
    }

    if ((state->num_events + 1) >= state->num_allocated)
    {
        midi_event_info *new_events;

        new_events = (midi_event_info *) realloc(state->events, sizeof(midi_event_info) * state->num_allocated * 2);
        if (new_events == NULL) return 13;

        state->num_allocated = state->num_allocated * 2;
        state->events = new_events;
    }

    state->num_events++;
    state->events[state->num_events] = event;
    state->events[0].len = state->num_events;

    return 0;
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


static int MIDI_PLUGIN2_API pause_0(void);
static int MIDI_PLUGIN2_API resume(void);
static int send_initial_sysex_events(unsigned char const *sysex_events);
static void reset_playing(void);

static void *midi_thread_proc(void *arg)
{
    midi_event_info *events;
    unsigned int current_event, num_events, insert_events, current_notification_events;
    uint64_t base_time;
    int do_sleep, endpoint_exists, chan, more_events;
    int64_t time_diff, base_diff;
    struct timespec _tp, base_tp;
    struct {
        union {
#if (MIDI_API >= 0)
            struct {
                MIDIEventList *list;
                MIDIEventPacket *packet, *nextpacket;
            } ump;
#endif
#if (MIDI_API <= 0)
            struct {
                MIDIPacketList *list;
                MIDIPacket *packet, *nextpacket;
            } old;
#endif
        };
        int max_data, state;
    } pkt;

    clock_gettime(CLOCK_MONOTONIC, &base_tp);

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
    if (use_ump)
#endif
    {
        pkt.ump.list = (MIDIEventList *)midi_buf;
    }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
    else
#endif
    {
        pkt.old.list = (MIDIPacketList *)midi_buf;
    }
#endif

    do_sleep = 1;
    endpoint_exists = 1;

    for (;;)
    {
        if (do_sleep)
        {
            do_sleep = 0;

            _tp.tv_sec = 0;
            _tp.tv_nsec = 10000000; // 10ms

            nanosleep(&_tp, NULL);
        }

        if (midi_quit) return NULL;

        if (notification_events != 0)
        {
            pthread_mutex_lock(&notification_mutex);
            current_notification_events = notification_events;
            notification_events = 0;
            pthread_mutex_unlock(&notification_mutex);

            if ((current_notification_events & NOTIFY_CHANGEID) && endpoint_exists)
            {
                int32_t unique_id;
                if (MIDIObjectGetIntegerProperty(midi_endpoint, kMIDIPropertyUniqueID, &unique_id) == noErr)
                {
                    midi_unique_id = unique_id;
                }
            }

            if ((current_notification_events & NOTIFY_REMOVE) && endpoint_exists)
            {
                endpoint_exists = 0;
                midi_endpoint = 0;

                if (midi_loaded)
                {
                    pthread_mutex_lock(&midi_mutex);
                    if (midi_loaded && (midi_loop_count == 0))
                    {
                        midi_eof = 1;
                    }
                    pthread_mutex_unlock(&midi_mutex);
                }
            }

            if ((current_notification_events & NOTIFY_ADD) && !endpoint_exists)
            {
                MIDIObjectRef object;
                MIDIObjectType object_type;

                if (MIDIObjectFindByUniqueID(midi_unique_id, &object, &object_type) == noErr)
                {
                    if (object_type == kMIDIObjectType_Destination)
                    {
                        endpoint_exists = 1;
                    }
                }

                if (endpoint_exists)
                {
                    int was_playing;

                    pthread_mutex_lock(&midi_mutex);

                    was_playing = (midi_loaded && midi_playing);
                    if (was_playing)
                    {
                        pause_0();
                    }

                    midi_endpoint = (MIDIEndpointRef)object;

                    if (midi_type == 2)
                    {
                        mt32_initialize_gm();
                    }

                    if (initial_sysex_events != NULL)
                    {
                        send_initial_sysex_events(initial_sysex_events);
                    }

                    reset_playing();

                    if (midi_loaded && !midi_eof)
                    {
                        if (midi_type == 2)
                        {
                            mt32_reinstall_timbres();
                        }

                        for (chan = 0; chan < MIDI_CHANNELS; chan++)
                        {
                            channel_volume[chan] = 100;
                        }

                        // set current volume to a different value than new volume in order to set the channel volume at start of playing
                        midi_current_volume = midi_new_volume ^ 1;

                        memset(channel_notes, 0, 128*MIDI_CHANNELS*sizeof(int));

                        if (midi_current_event > 1)
                        {
                            // play song from the beginning
                            current_event = midi_current_event;
                            events = (midi_event_info *) midi_events;
                            midi_current_event = 1;
                            midi_base_time += events[current_event].time;
                        }
                    }

                    if (was_playing)
                    {
                        resume();
                    }

                    pthread_mutex_unlock(&midi_mutex);
                }
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

        if ((!endpoint_exists) && (midi_loop_count == 0))
        {
            midi_eof = 1;
            pthread_mutex_unlock(&midi_mutex);
            do_sleep = 1;
            continue;
        }

        events = (midi_event_info *) midi_events;
        current_event = midi_current_event;
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
            base_time += events[num_events].time;

            current_event = 1;

            midi_current_event = current_event;
            midi_base_time = base_time;
        }

        insert_events = 0;
        if (endpoint_exists)
        {
            if (midi_new_volume != midi_current_volume)
            {
                midi_current_volume = midi_new_volume;
                insert_events |= 1;
            }
            if (state_mt32_display == 5)
            {
                clock_gettime(CLOCK_MONOTONIC, &_tp);
                _tp.tv_sec -= base_tp.tv_sec;
                if (_tp.tv_nsec < base_tp.tv_nsec) _tp.tv_sec--;
                if (_tp.tv_sec >= 5)
                {
                    state_mt32_display = 2;
                    insert_events |= 2;
                }
            }
        }

        if (insert_events)
        {
#if (MIDI_API >= 0)
#if (MIDI_API == 0)
            if (use_ump)
#endif
            {
                pkt.ump.list->protocol = kMIDIProtocol_1_0;
                pkt.ump.list->numPackets = 0;
                pkt.ump.packet = pkt.ump.list->packet;

                if (insert_events & 1)
                {
                    pkt.ump.packet->timeStamp = nsec2abstime(midi_last_time);
                    pkt.ump.packet->wordCount = 0;

                    for (chan = 0; chan < MIDI_CHANNELS; chan++)
                    {
                        pkt.ump.packet->words[pkt.ump.packet->wordCount] = (kMIDIMessageTypeChannelVoice1 << 28) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (7 << 8) | ((midi_current_volume * channel_volume[chan]) / 127);
                        pkt.ump.packet->wordCount++;
                    }

                    pkt.ump.list->numPackets++;
                    pkt.ump.packet = MIDIEventPacketNext(pkt.ump.packet);
                }
                if (insert_events & 2)
                {
                    pkt.ump.packet->timeStamp = nsec2abstime(midi_last_time);

                    pkt.ump.packet->wordCount = convert_sysex_to_ump(pkt.ump.packet->words, sysex_mt32_reset_display[0], sysex_mt32_reset_display + 1, sizeof(sysex_mt32_reset_display) - 2);

                    pkt.ump.list->numPackets++;
                    pkt.ump.packet = MIDIEventPacketNext(pkt.ump.packet);
                }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
                MIDISendEventList(midi_port, midi_endpoint, pkt.ump.list);
#pragma clang diagnostic pop
            }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
            else
#endif
            {
                pkt.old.list->numPackets = 0;
                pkt.old.packet = pkt.old.list->packet;

                if (insert_events & 1)
                {
                    pkt.old.packet->timeStamp = nsec2abstime(midi_last_time);
                    pkt.old.packet->length = 0;

                    for (chan = 0; chan < MIDI_CHANNELS; chan++)
                    {
                        pkt.old.packet->data[pkt.old.packet->length + 0] = (MIDI_STATUS_CONTROLLER << 4) | chan;
                        pkt.old.packet->data[pkt.old.packet->length + 1] = 7;
                        pkt.old.packet->data[pkt.old.packet->length + 2] = (midi_current_volume * channel_volume[chan]) / 127;
                        pkt.old.packet->length += 3;
                    }

                    pkt.old.list->numPackets++;
                    pkt.old.packet = MIDIPacketNext(pkt.old.packet);
                }
                if (insert_events & 2)
                {
                    pkt.old.packet->timeStamp = nsec2abstime(midi_last_time);
                    pkt.old.packet->length = sizeof(sysex_mt32_reset_display) - 1;

                    memcpy(pkt.old.packet->data, sysex_mt32_reset_display, sizeof(sysex_mt32_reset_display) - 1);

                    pkt.old.list->numPackets++;
                    pkt.old.packet = MIDIPacketNext(pkt.old.packet);
                }

                MIDISend(midi_port, midi_endpoint, pkt.old.list);
            }
#endif
        }

        base_diff = clock_gettime_nsec_np(CLOCK_UPTIME_RAW) - base_time;

        time_diff = events[current_event].time - base_diff;

        if (time_diff >= 100000000) // 100ms
        {
            pthread_mutex_unlock(&midi_mutex);
            do_sleep = 1;
            continue;
        }

        more_events = 1;
#if (MIDI_API >= 0)
#if (MIDI_API == 0)
        if (use_ump)
#endif
        {
            pkt.ump.list->protocol = kMIDIProtocol_1_0;

            do
            {
                pkt.ump.list->numPackets = 0;
                pkt.ump.packet = pkt.ump.list->packet;
                pkt.max_data = ((int)(sizeof(midi_buf) - ((uintptr_t)pkt.ump.packet->words - (uintptr_t)pkt.ump.list))) >> 2;
                pkt.ump.packet->timeStamp = nsec2abstime(base_time + events[current_event].time);
                pkt.ump.packet->wordCount = 0;
                pkt.state = 0;

                for (;;)
                {
                    if (events[current_event].len == 0)
                    {
                        // do nothing
                    }
                    else if (events[current_event].ump.sysex != NULL)
                    {
                        if (pkt.ump.packet->wordCount != 0)
                        {
                            // packet already contains events (sysex must be the only event in packet) -> start new packet (with current event)
                            pkt.state = -1;
                        }
                        else if (events[current_event].len > pkt.max_data)
                        {
                            // sysex doesn't fit into list (list already contains packets) -> start new list (with current event)
                            break;
                        }
                        else
                        {
                            // sysex must be the only event in packet -> start new packet (with next event)
                            pkt.state = 1;
                            pkt.ump.packet->wordCount = events[current_event].len;
                            memcpy(pkt.ump.packet->words, events[current_event].ump.sysex, events[current_event].len * sizeof(uint32_t));
                        }
                    }
                    else
                    {
                        if (pkt.ump.packet->wordCount >= pkt.max_data)
                        {
                            // event doesn't fit into list -> start new list (with current event)
                            break;
                        }
                        else
                        {
                            pkt.ump.packet->words[pkt.ump.packet->wordCount] = events[current_event].ump.data;
                            pkt.ump.packet->wordCount++;

                            switch((events[current_event].ump.data >> 20) & 0x0f)
                            {
                                case MIDI_STATUS_NOTE_ON:
                                    if ((events[current_event].ump.data & 0xff) != 0)
                                    {
                                        // note on
                                        channel_notes[(events[current_event].ump.data >> 16) & 0x0f][(events[current_event].ump.data >> 8) & 0xff]++;
                                        break;
                                    }
                                    // fallthrough
                                case MIDI_STATUS_NOTE_OFF:
                                    // note off
                                    if (channel_notes[(events[current_event].ump.data >> 16) & 0x0f][(events[current_event].ump.data >> 8) & 0xff] > 0)
                                    {
                                        channel_notes[(events[current_event].ump.data >> 16) & 0x0f][(events[current_event].ump.data >> 8) & 0xff]--;
                                    }
                                    break;
                                case MIDI_STATUS_CONTROLLER:
                                    if (((events[current_event].ump.data >> 8) & 0xff) == 7)
                                    {
                                        // volume
                                        channel_volume[(events[current_event].ump.data >> 16) & 0x0f] = events[current_event].ump.data & 0xff;
                                        pkt.ump.packet->words[pkt.ump.packet->wordCount - 1] = (pkt.ump.packet->words[pkt.ump.packet->wordCount - 1] & 0xffffff00) | ((midi_current_volume * (events[current_event].ump.data & 0xff)) / 127);
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }

                    if (pkt.state >= 0)
                    {
                        current_event++;
                        if (current_event > num_events)
                        {
                            more_events = 0;
                            break;
                        }
                        else if (events[current_event].time != events[current_event - 1].time)
                        {
                            // next packet has different time -> start new packet
                            pkt.state = 1;
                            time_diff = events[current_event].time - base_diff;
                            if (time_diff >= 100000000) // 100ms
                            {
                                more_events = 0;
                                break;
                            }
                        }
                    }

                    if (pkt.state != 0)
                    {
                        pkt.ump.nextpacket = MIDIEventPacketNext(pkt.ump.packet);
                        pkt.max_data = ((int)(sizeof(midi_buf) - ((uintptr_t)pkt.ump.nextpacket->words - (uintptr_t)pkt.ump.list))) >> 2;
                        if (pkt.max_data <= 0) break;
                        pkt.ump.list->numPackets++;
                        pkt.ump.packet = pkt.ump.nextpacket;
                        pkt.ump.packet->timeStamp = nsec2abstime(base_time + events[current_event].time);
                        pkt.ump.packet->wordCount = 0;
                        pkt.state = 0;
                    }
                }

                if (pkt.ump.packet->wordCount != 0)
                {
                    pkt.ump.list->numPackets++;
                }

                if (pkt.ump.list->numPackets != 0 && endpoint_exists)
                {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
                    MIDISendEventList(midi_port, midi_endpoint, pkt.ump.list);
#pragma clang diagnostic pop
                }
            } while (more_events);
        }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
        else
#endif
        {
            do
            {
                pkt.old.list->numPackets = 0;
                pkt.old.packet = pkt.old.list->packet;
                pkt.max_data = (int)(sizeof(midi_buf) - ((uintptr_t)pkt.old.packet->data - (uintptr_t)pkt.old.list));
                pkt.old.packet->timeStamp = nsec2abstime(base_time + events[current_event].time);
                pkt.old.packet->length = 0;
                pkt.state = 0;

                for (;;)
                {
                    if (events[current_event].len == 0)
                    {
                        // do nothing
                    }
                    else if (events[current_event].old.sysex != NULL)
                    {
                        if (pkt.old.packet->length != 0)
                        {
                            // packet already contains events (sysex must be the only event in packet) -> start new packet (with current event)
                            pkt.state = -1;
                        }
                        else if (events[current_event].len > pkt.max_data)
                        {
                            // sysex doesn't fit into list (list already contains packets) -> start new list (with current event)
                            break;
                        }
                        else
                        {
                            // sysex must be the only event in packet -> start new packet (with next event)
                            pkt.state = 1;
                            pkt.old.packet->length = events[current_event].len;
                            memcpy(pkt.old.packet->data, events[current_event].old.sysex, events[current_event].len);
                        }
                    }
                    else
                    {
                        if (pkt.old.packet->length + events[current_event].len > pkt.max_data)
                        {
                            // event doesn't fit into list -> start new list (with current event)
                            break;
                        }
                        else
                        {
                            memcpy(pkt.old.packet->data + pkt.old.packet->length, events[current_event].old.data, events[current_event].len);
                            pkt.old.packet->length += events[current_event].len;

                            switch(events[current_event].old.data[0] >> 4)
                            {
                                case MIDI_STATUS_NOTE_ON:
                                    if (events[current_event].old.data[2] != 0)
                                    {
                                        // note on
                                        channel_notes[events[current_event].old.data[0] & 0x0f][events[current_event].old.data[1]]++;
                                        break;
                                    }
                                    // fallthrough
                                case MIDI_STATUS_NOTE_OFF:
                                    // note off
                                    if (channel_notes[events[current_event].old.data[0] & 0x0f][events[current_event].old.data[1]] > 0)
                                    {
                                        channel_notes[events[current_event].old.data[0] & 0x0f][events[current_event].old.data[1]]--;
                                    }
                                    break;
                                case MIDI_STATUS_CONTROLLER:
                                    if (events[current_event].old.data[1] == 7)
                                    {
                                        // volume
                                        channel_volume[events[current_event].old.data[0] & 0x0f] = events[current_event].old.data[2];
                                        pkt.old.packet->data[pkt.old.packet->length - 1] = (midi_current_volume * events[current_event].old.data[2]) / 127;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }

                    if (pkt.state >= 0)
                    {
                        current_event++;
                        if (current_event > num_events)
                        {
                            more_events = 0;
                            break;
                        }
                        else if (events[current_event].time != events[current_event - 1].time)
                        {
                            // next packet has different time -> start new packet
                            pkt.state = 1;
                            time_diff = events[current_event].time - base_diff;
                            if (time_diff >= 100000000) // 100ms
                            {
                                more_events = 0;
                                break;
                            }
                        }
                    }

                    if (pkt.state != 0)
                    {
                        pkt.old.nextpacket = MIDIPacketNext(pkt.old.packet);
                        pkt.max_data = (int)(sizeof(midi_buf) - ((uintptr_t)pkt.old.nextpacket->data - (uintptr_t)pkt.old.list));
                        if (pkt.max_data <= 0) break;
                        pkt.old.list->numPackets++;
                        pkt.old.packet = pkt.old.nextpacket;
                        pkt.old.packet->timeStamp = nsec2abstime(base_time + events[current_event].time);
                        pkt.old.packet->length = 0;
                        pkt.state = 0;
                    }
                }

                if (pkt.old.packet->length != 0)
                {
                    pkt.old.list->numPackets++;
                }

                if (pkt.old.list->numPackets != 0 && endpoint_exists)
                {
                    MIDISend(midi_port, midi_endpoint, pkt.old.list);
                }
            } while (more_events);
        }
#endif

        midi_current_event = current_event;
        midi_last_time = base_time + events[current_event - 1].time;

        pthread_mutex_unlock(&midi_mutex);
    };
}

static int send_initial_sysex_events(unsigned char const *sysex_events)
{
    int events_len, len, delay_len, min_delay;
    uint64_t next_time;
    struct timespec _tp;
    struct {
        union {
#if (MIDI_API >= 0)
            struct {
                MIDIEventList *list;
                MIDIEventPacket *packet;
            } ump;
#endif
#if (MIDI_API <= 0)
            struct {
                MIDIPacketList *list;
                MIDIPacket *packet;
            } old;
#endif
        };
    } pkt;

    if (midi_client == 0) return 0;

    events_len = 0;

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
    if (use_ump)
#endif
    {
        pkt.ump.list = (MIDIEventList *)midi_buf;
        pkt.ump.packet = pkt.ump.list->packet;
    }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
    else
#endif
    {
        pkt.old.list = (MIDIPacketList *)midi_buf;
        pkt.old.packet = pkt.old.list->packet;
    }
#endif

    pthread_mutex_lock(&midi_mutex);

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
    if (use_ump)
#endif
    {
        pkt.ump.list->protocol = kMIDIProtocol_1_0;
        pkt.ump.list->numPackets = 1;
        pkt.ump.packet->timeStamp = 0;
    }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
    else
#endif
    {
        pkt.old.list->numPackets = 1;
        pkt.old.packet->timeStamp = 0;
    }
#endif

    if (mt32_delay)
    {
        next_time = clock_gettime_nsec_np(CLOCK_UPTIME_RAW) + (40 + 10 + 10) * 1000000;
    }

    while (*sysex_events == 0xf0)
    {
        len = 2;
        while (sysex_events[len - 1] != 0xf7) len++;

        delay_len = 0;
        min_delay = 0;
        if (!(sysex_events[len] & 0x80))
        {
            delay_len++;
            min_delay = sysex_events[len];
            if (!(sysex_events[len + 1] & 0x80))
            {
                delay_len++;
                min_delay |= (sysex_events[len + 1] & 7) << 7;
            }
        }

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
        if (use_ump)
#endif
        {
            if (mt32_delay)
            {
                pkt.ump.packet->timeStamp = nsec2abstime(next_time);
            }

            pkt.ump.packet->wordCount = convert_sysex_to_ump(pkt.ump.packet->words, sysex_events[0], sysex_events + 1, len - 1);

            if (midi_endpoint != 0)
            {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
                MIDISendEventList(midi_port, midi_endpoint, pkt.ump.list);
#pragma clang diagnostic pop
            }
        }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
        else
#endif
        {
            if (mt32_delay)
            {
                pkt.old.packet->timeStamp = nsec2abstime(next_time);
            }

            pkt.old.packet->length = len;

            memcpy(pkt.old.packet->data, sysex_events, len);

            if (midi_endpoint != 0)
            {
                MIDISend(midi_port, midi_endpoint, pkt.old.list);
            }
        }
#endif

        sysex_events += len + delay_len;
        events_len += len + delay_len;

        if (mt32_delay)
        {
            if (min_delay < 40) min_delay = 40;
            next_time += (min_delay + 10 + ((len * 10000 + 31249) / 31250)) * 1000000;
        }
    };

    events_len++;

    if (mt32_delay)
    {
        do
        {
            _tp.tv_sec = 0;
            _tp.tv_nsec = 100000000; // 100ms

            nanosleep(&_tp, NULL);
        } while ((int64_t)(clock_gettime_nsec_np(CLOCK_UPTIME_RAW) - next_time) > 0);
    }

    pthread_mutex_unlock(&midi_mutex);

    return events_len;
}

static void reset_playing(void)
{
    int chan;
    int64_t time_diff;
    unsigned char *controller_events;
    struct timespec _tp;
    struct {
        union {
#if (MIDI_API >= 0)
            struct {
                MIDIEventList *list;
                MIDIEventPacket *packet;
            } ump;
#endif
#if (MIDI_API <= 0)
            struct {
                MIDIPacketList *list;
                MIDIPacket *packet;
            } old;
#endif
        };
    } pkt;

    if (midi_client == 0) return;

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
    if (use_ump)
#endif
    {
        pkt.ump.list = (MIDIEventList *)midi_buf;
        pkt.ump.packet = pkt.ump.list->packet;
    }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
    else
#endif
    {
        pkt.old.list = (MIDIPacketList *)midi_buf;
        pkt.old.packet = pkt.old.list->packet;
    }
#endif

    pthread_mutex_lock(&midi_mutex);

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
    if (use_ump)
#endif
    {
        pkt.ump.list->protocol = kMIDIProtocol_1_0;
        pkt.ump.list->numPackets = 1;
        pkt.ump.packet->wordCount = 0;
        pkt.ump.packet->timeStamp = midi_loaded ? nsec2abstime(midi_last_time) : 0;

        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            // MT-32 doesn't support All sounds off, so Omni off is used instead
            pkt.ump.packet->words[pkt.ump.packet->wordCount] = (kMIDIMessageTypeChannelVoice1 << 28) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | ((midi_type ? 0x7c : 0x78) << 8) | (0); // Omni off / All sounds off (abrupt stop of sound on channel)
            pkt.ump.packet->wordCount++;

            pkt.ump.packet->words[pkt.ump.packet->wordCount] = (kMIDIMessageTypeChannelVoice1 << 28) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (0x79 << 8) | (0); // All controllers off (this message clears all the controller values for this channel, back to their default values)
            pkt.ump.packet->wordCount++;

            pkt.ump.packet->words[pkt.ump.packet->wordCount] = (kMIDIMessageTypeChannelVoice1 << 28) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (0x7b << 8) | (0); // All notes off (this message stops all the notes that are currently playing)
            pkt.ump.packet->wordCount++;

            // All controllers off doesn't set volume and pan to default values
            // Volume is set at start of playing, so only pan is set to default value
            pkt.ump.packet->words[pkt.ump.packet->wordCount] = (kMIDIMessageTypeChannelVoice1 << 28) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (0x0a << 8) | (64); // Pan
            pkt.ump.packet->wordCount++;

            if (reset_controller_events != NULL)
            {
                controller_events = reset_controller_events;
                while (*controller_events != 0xff)
                {
                    pkt.ump.packet->words[pkt.ump.packet->wordCount] = (kMIDIMessageTypeChannelVoice1 << 28) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (controller_events[0] << 8) | (controller_events[1]);
                    pkt.ump.packet->wordCount++;

                    controller_events += 2;
                }
            }
        }

        pkt.ump.packet->words[pkt.ump.packet->wordCount] = (kMIDIMessageTypeChannelVoice1 << 28) | (MIDI_STATUS_PROG_CHANGE << 20) | (9 << 16) | (0 << 8);
        pkt.ump.packet->wordCount++;

        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            pkt.ump.packet->words[pkt.ump.packet->wordCount] = (kMIDIMessageTypeChannelVoice1 << 28) | (MIDI_STATUS_PITCH_WHEEL << 20) | (chan << 16) | (0x00 << 8) | (0x40);
            pkt.ump.packet->wordCount++;
        }

        if (midi_endpoint != 0)
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
            MIDISendEventList(midi_port, midi_endpoint, pkt.ump.list);
#pragma clang diagnostic pop
        }
    }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
    else
#endif
    {
        pkt.old.list->numPackets = 1;
        pkt.old.packet->length = 0;
        pkt.old.packet->timeStamp = midi_loaded ? nsec2abstime(midi_last_time) : 0;

        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            // MT-32 doesn't support All sounds off, so Omni off is used instead
            pkt.old.packet->data[pkt.old.packet->length + 0] = (MIDI_STATUS_CONTROLLER << 4) | chan;
            pkt.old.packet->data[pkt.old.packet->length + 1] = midi_type ? 0x7c : 0x78; // Omni off / All sounds off (abrupt stop of sound on channel)
            pkt.old.packet->data[pkt.old.packet->length + 2] = 0;
            pkt.old.packet->length += 3;

            pkt.old.packet->data[pkt.old.packet->length + 0] = (MIDI_STATUS_CONTROLLER << 4) | chan;
            pkt.old.packet->data[pkt.old.packet->length + 1] = 0x79; // All controllers off (this message clears all the controller values for this channel, back to their default values)
            pkt.old.packet->data[pkt.old.packet->length + 2] = 0;
            pkt.old.packet->length += 3;

            pkt.old.packet->data[pkt.old.packet->length + 0] = (MIDI_STATUS_CONTROLLER << 4) | chan;
            pkt.old.packet->data[pkt.old.packet->length + 1] = 0x7b; // All notes off (this message stops all the notes that are currently playing)
            pkt.old.packet->data[pkt.old.packet->length + 2] = 0;
            pkt.old.packet->length += 3;

            // All controllers off doesn't set volume and pan to default values
            // Volume is set at start of playing, so only pan is set to default value
            pkt.old.packet->data[pkt.old.packet->length + 0] = (MIDI_STATUS_CONTROLLER << 4) | chan;
            pkt.old.packet->data[pkt.old.packet->length + 1] = 0x0a; // Pan
            pkt.old.packet->data[pkt.old.packet->length + 2] = 64;
            pkt.old.packet->length += 3;

            if (reset_controller_events != NULL)
            {
                controller_events = reset_controller_events;
                while (*controller_events != 0xff)
                {
                    pkt.old.packet->data[pkt.old.packet->length + 0] = (MIDI_STATUS_CONTROLLER << 4) | chan;
                    pkt.old.packet->data[pkt.old.packet->length + 1] = controller_events[0];
                    pkt.old.packet->data[pkt.old.packet->length + 2] = controller_events[1];
                    pkt.old.packet->length += 3;

                    controller_events += 2;
                }
            }
        }

        pkt.old.packet->data[pkt.old.packet->length + 0] = (MIDI_STATUS_PROG_CHANGE << 4) | 9;
        pkt.old.packet->data[pkt.old.packet->length + 1] = 0;
        pkt.old.packet->length += 2;

        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            pkt.old.packet->data[pkt.old.packet->length + 0] = (MIDI_STATUS_PITCH_WHEEL << 4) | chan;
            pkt.old.packet->data[pkt.old.packet->length + 1] = 0x00;
            pkt.old.packet->data[pkt.old.packet->length + 2] = 0x40;
            pkt.old.packet->length += 3;
        }

        if (midi_endpoint != 0)
        {
            MIDISend(midi_port, midi_endpoint, pkt.old.list);
        }
    }
#endif

    if (midi_loaded)
    {
        time_diff = clock_gettime_nsec_np(CLOCK_UPTIME_RAW) - midi_last_time;

        if (time_diff > 0)
        {
            _tp.tv_sec = 0;
            _tp.tv_nsec = time_diff;

            nanosleep(&_tp, NULL);
        }
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


static void notify_procedure(const MIDINotification *message, void * __nullable ref_con)
{
    const MIDIObjectAddRemoveNotification *addremove;
    const MIDIObjectPropertyChangeNotification *propchange;
    int32_t unique_id;

    switch(message->messageID)
    {
        case kMIDIMsgObjectAdded:
            addremove = (const MIDIObjectAddRemoveNotification *)message;
            if (addremove->childType == kMIDIObjectType_Destination)
            {
                if (MIDIObjectGetIntegerProperty(addremove->child, kMIDIPropertyUniqueID, &unique_id) == noErr)
                {
                    if (unique_id == midi_unique_id)
                    {
                        pthread_mutex_lock(&notification_mutex);
                        notification_events |= NOTIFY_ADD;
                        pthread_mutex_unlock(&notification_mutex);
                    }
                }
            }
            break;
        case kMIDIMsgObjectRemoved:
            addremove = (const MIDIObjectAddRemoveNotification *)message;
            if (addremove->childType == kMIDIObjectType_Destination)
            {
                if (addremove->child == midi_endpoint)
                {
                    pthread_mutex_lock(&notification_mutex);
                    notification_events |= NOTIFY_REMOVE;
                    pthread_mutex_unlock(&notification_mutex);
                }
            }
            break;
        case kMIDIMsgPropertyChanged:
            propchange = (const MIDIObjectPropertyChangeNotification *)message;
            if (propchange->objectType == kMIDIObjectType_Destination)
            {
                if (CFStringCompare(propchange->propertyName, kMIDIPropertyUniqueID, 0) == kCFCompareEqualTo)
                {
                    if (propchange->object == midi_endpoint)
                    {
                        pthread_mutex_lock(&notification_mutex);
                        notification_events |= NOTIFY_CHANGEID | NOTIFY_ADD;
                        pthread_mutex_unlock(&notification_mutex);
                    }
                    else
                    {
                        if (MIDIObjectGetIntegerProperty(propchange->object, kMIDIPropertyUniqueID, &unique_id) == noErr)
                        {
                            if (unique_id == midi_unique_id)
                            {
                                pthread_mutex_lock(&notification_mutex);
                                notification_events |= NOTIFY_ADD;
                                pthread_mutex_unlock(&notification_mutex);
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
}

static int find_endpoint(const char *device_name)
{
    ItemCount num_destinations, index;
    CFStringRef cf_device_name, cf_name;
    MIDIEndpointRef endpoint;
    MIDIEntityRef entity;
    MIDIDeviceRef device;
    MIDIObjectRef object;
    MIDIObjectType object_type;
    int found;
    int32_t unique_id;
    long device_id;
    char *endptr;

    num_destinations = MIDIGetNumberOfDestinations();
    if (num_destinations == 0) return -1;

    found = 0;
    if (device_name != NULL && *device_name != 0)
    {
        cf_device_name = CFStringCreateWithCString(kCFAllocatorDefault, device_name, kCFStringEncodingUTF8);
        if (cf_device_name == NULL) return -2;

        for (index = 0; index < num_destinations; index++)
        {
            endpoint = MIDIGetDestination(index);
            if (endpoint == 0) continue;

            if (MIDIObjectGetStringProperty(endpoint, kMIDIPropertyName, &cf_name) == noErr)
            {
                if (CFStringCompare(cf_device_name, cf_name, 0) == kCFCompareEqualTo) found = 1;
                CFRelease(cf_name);
                if (found) break;
            }

            if (MIDIObjectGetStringProperty(endpoint, kMIDIPropertyDisplayName, &cf_name) == noErr)
            {
                if (CFStringCompare(cf_device_name, cf_name, 0) == kCFCompareEqualTo) found = 1;
                CFRelease(cf_name);
                if (found) break;
            }

            if (MIDIEndpointGetEntity(endpoint, &entity) != noErr) continue;
            if (MIDIEntityGetDevice(entity, &device) != noErr) continue;

            if (MIDIObjectGetStringProperty(device, kMIDIPropertyName, &cf_name) == noErr)
            {
                if (CFStringCompare(cf_device_name, cf_name, 0) == kCFCompareEqualTo) found = 1;
                CFRelease(cf_name);
                if (found) break;
            }
        }

        CFRelease(cf_device_name);

        if (!found)
        {
            errno = 0;
            device_id = strtol(device_name, &endptr, 10);
            if ((errno == 0) && (*endptr == 0) && (device_id >= INT32_MIN) && (device_id <= INT32_MAX))
            {
                if (MIDIObjectFindByUniqueID(device_id, &object, &object_type) == noErr)
                {
                    if (object_type == kMIDIObjectType_Destination)
                    {
                        midi_endpoint = (MIDIEndpointRef)object;
                        midi_unique_id = device_id;
                        return 0;
                    }
                }
            }
        }
    }
    else
    {
        endpoint = MIDIGetDestination(0);
        if (endpoint != 0) found = 1;
    }

    if (found)
    {
        midi_endpoint = endpoint;

        if (MIDIObjectGetIntegerProperty(endpoint, kMIDIPropertyUniqueID, &unique_id) == noErr)
        {
            midi_unique_id = unique_id;
        }

        return 0;
    }

    return -3;
}

#endif


static int MIDI_PLUGIN2_API play(void const *midibuffer, long int size, int loop_count)
{
    if (midibuffer == NULL) return -1;
    if (size <= 0) return -2;

    if (loop_count < -1) loop_count = -1;

#ifdef PLUGIN_ENABLED
    if (midi_client == 0) return -3;

    close_midi();

    midi_loop_count = loop_count;

    {
        unsigned int timediv;
        midi_event_info *dataptr;
        int chan;

        if (preprocessmidi(midibuffer, size, &timediv, &dataptr))
        {
            return -4;
        }

        pthread_mutex_lock(&midi_mutex);

        memset(channel_notes, 0, 128*MIDI_CHANNELS*sizeof(int));

        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            channel_volume[chan] = 100;
        }

        // set current volume to a different value than new volume in order to set the channel volume at start of playing
        midi_current_volume = midi_new_volume ^ 1;

        midi_events = dataptr;
        midi_current_event = 1;

        midi_loaded = 1;
        midi_eof = 0;

        midi_playing = 1;
        midi_base_time = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);

        pthread_mutex_unlock(&midi_mutex);
    }
#endif

    return 0;
}

static int MIDI_PLUGIN2_API pause_0(void)
{
#ifdef PLUGIN_ENABLED
    int chan, note, num;
    struct {
        union {
#if (MIDI_API >= 0)
            struct {
                MIDIEventList *list;
                MIDIEventPacket *packet;
            } ump;
#endif
#if (MIDI_API <= 0)
            struct {
                MIDIPacketList *list;
                MIDIPacket *packet;
            } old;
#endif
        };
        int max_data;
    } pkt;

    if (midi_client == 0) return -1;
    if (!midi_loaded) return -2;

    if (midi_playing)
    {
#if (MIDI_API >= 0)
#if (MIDI_API == 0)
        if (use_ump)
#endif
        {
            pkt.ump.list = (MIDIEventList *)midi_buf;
            pkt.ump.packet = pkt.ump.list->packet;
        }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
        else
#endif
        {
            pkt.old.list = (MIDIPacketList *)midi_buf;
            pkt.old.packet = pkt.old.list->packet;
        }
#endif

        pthread_mutex_lock(&midi_mutex);

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
        if (use_ump)
#endif
        {
            pkt.ump.list->protocol = kMIDIProtocol_1_0;
            pkt.ump.list->numPackets = 1;
        }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
        else
#endif
        {
            pkt.old.list->numPackets = 1;
        }
#endif

        if (state_mt32_display == 2 && !mt32_delay)
        {
            state_mt32_display = 4;

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
            if (use_ump)
#endif
            {
                pkt.ump.packet->timeStamp = nsec2abstime(midi_last_time);

                pkt.ump.packet->wordCount = convert_sysex_to_ump(pkt.ump.packet->words, sysex_mt32_display[0], sysex_mt32_display + 1, sizeof(sysex_mt32_display) - 2);

                pkt.ump.list->numPackets++;
                pkt.ump.packet = MIDIEventPacketNext(pkt.ump.packet);
            }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
            else
#endif
            {
                pkt.old.packet->timeStamp = nsec2abstime(midi_last_time);
                pkt.old.packet->length = sizeof(sysex_mt32_display) - 1;

                memcpy(pkt.old.packet->data, sysex_mt32_display, sizeof(sysex_mt32_display) - 1);

                pkt.old.list->numPackets++;
                pkt.old.packet = MIDIPacketNext(pkt.old.packet);
            }
#endif
        }

        // stop playing notes on all channels
#if (MIDI_API >= 0)
#if (MIDI_API == 0)
        if (use_ump)
#endif
        {
            pkt.ump.packet->timeStamp = nsec2abstime(midi_last_time);
            pkt.ump.packet->wordCount = 0;
            pkt.max_data = ((int)(sizeof(midi_buf) - ((uintptr_t)pkt.ump.packet->words - (uintptr_t)pkt.ump.list))) >> 2;

            for (chan = 0; chan < MIDI_CHANNELS; chan++)
            {
                for (note = 0; note < 128; note++)
                {
                    if (channel_notes[chan][note] > 0)
                    for (num = channel_notes[chan][note]; num != 0; num--)
                    {
                        if (pkt.ump.packet->wordCount >= pkt.max_data) break;

                        pkt.ump.packet->words[pkt.ump.packet->wordCount] = (kMIDIMessageTypeChannelVoice1 << 28) | (MIDI_STATUS_NOTE_OFF << 20) | (chan << 16) | (note << 8) | (0);
                        pkt.ump.packet->wordCount++;
                    }
                }
            }

            if (midi_endpoint != 0)
            {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
                MIDISendEventList(midi_port, midi_endpoint, pkt.ump.list);
#pragma clang diagnostic pop
            }
        }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
        else
#endif
        {
            pkt.old.packet->timeStamp = nsec2abstime(midi_last_time);
            pkt.old.packet->length = 0;
            pkt.max_data = (int)(sizeof(midi_buf) - ((uintptr_t)pkt.old.packet->data - (uintptr_t)pkt.old.list));

            for (chan = 0; chan < MIDI_CHANNELS; chan++)
            {
                for (note = 0; note < 128; note++)
                {
                    if (channel_notes[chan][note] > 0)
                    for (num = channel_notes[chan][note]; num != 0; num--)
                    {
                        if (pkt.old.packet->length + 3 > pkt.max_data) break;

                        pkt.old.packet->data[pkt.old.packet->length + 0] = (MIDI_STATUS_NOTE_OFF << 4) | chan;
                        pkt.old.packet->data[pkt.old.packet->length + 1] = note;
                        pkt.old.packet->data[pkt.old.packet->length + 2] = 0;
                        pkt.old.packet->length += 3;
                    }
                }
            }

            if (midi_endpoint != 0)
            {
                MIDISend(midi_port, midi_endpoint, pkt.old.list);
            }
        }
#endif

        midi_playing = 0;
        midi_pause_time = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);

        pthread_mutex_unlock(&midi_mutex);
    }

#endif

    return 0;
}

static int MIDI_PLUGIN2_API resume(void)
{
#ifdef PLUGIN_ENABLED
    struct {
        union {
#if (MIDI_API >= 0)
            struct {
                MIDIEventList *list;
                MIDIEventPacket *packet;
            } ump;
#endif
#if (MIDI_API <= 0)
            struct {
                MIDIPacketList *list;
                MIDIPacket *packet;
            } old;
#endif
        };
    } pkt;

    if (midi_client == 0) return -1;
    if (!midi_loaded) return -2;

    if (!midi_playing)
    {
        pthread_mutex_lock(&midi_mutex);

        if (state_mt32_display == 4)
        {
            state_mt32_display = 2;

#if (MIDI_API >= 0)
#if (MIDI_API == 0)
            if (use_ump)
#endif
            {
                pkt.ump.list = (MIDIEventList *)midi_buf;
                pkt.ump.list->protocol = kMIDIProtocol_1_0;
                pkt.ump.list->numPackets = 1;
                pkt.ump.packet = pkt.ump.list->packet;

                pkt.ump.packet->timeStamp = nsec2abstime(midi_last_time);

                pkt.ump.packet->wordCount = convert_sysex_to_ump(pkt.ump.packet->words, sysex_mt32_reset_display[0], sysex_mt32_reset_display + 1, sizeof(sysex_mt32_reset_display) - 2);

                if (midi_endpoint != 0)
                {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability-new"
                    MIDISendEventList(midi_port, midi_endpoint, pkt.ump.list);
#pragma clang diagnostic pop
                }
            }
#endif
#if (MIDI_API <= 0)
#if (MIDI_API == 0)
            else
#endif
            {
                pkt.old.list = (MIDIPacketList *)midi_buf;
                pkt.old.list->numPackets = 1;
                pkt.old.packet = pkt.old.list->packet;

                pkt.old.packet->timeStamp = nsec2abstime(midi_last_time);
                pkt.old.packet->length = sizeof(sysex_mt32_reset_display) - 1;

                memcpy(pkt.old.packet->data, sysex_mt32_reset_display, sizeof(sysex_mt32_reset_display) - 1);

                if (midi_endpoint != 0)
                {
                    MIDISend(midi_port, midi_endpoint, pkt.old.list);
                }
            }
#endif
        }

        midi_playing = 1;
        midi_base_time += clock_gettime_nsec_np(CLOCK_UPTIME_RAW) - midi_pause_time;

        pthread_mutex_unlock(&midi_mutex);
    }

#endif

    return 0;
}

static int MIDI_PLUGIN2_API halt(void)
{
#ifdef PLUGIN_ENABLED
    if (midi_client == 0) return -1;

    close_midi();

#endif

    return 0;
}

static int MIDI_PLUGIN2_API set_volume(unsigned char volume) // volume = 0 - 127
{
    if (volume > 127) volume = 127;

#ifdef PLUGIN_ENABLED
    if (midi_client == 0) return -1;

    midi_new_volume = volume;

#endif

    return 0;
}

static int MIDI_PLUGIN2_API set_loop_count(int loop_count) // -1 = unlimited
{
    if (loop_count < -1) loop_count = -1;

#ifdef PLUGIN_ENABLED
    if (midi_client == 0) return -1;
    if (!midi_loaded) return -2;

    midi_loop_count = loop_count;

#endif

    return 0;
}

static void MIDI_PLUGIN2_API shutdown_plugin(void)
{
#ifdef PLUGIN_ENABLED
    if (midi_client != 0)
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

        if (midi_type == 2)
        {
            mt32_shutdown_gm();
        }
        else if (midi_type)
        {
            if (state_mt32_display >= 4)
            {
                state_mt32_display = 2;
                send_initial_sysex_events(sysex_mt32_reset_display);
            }
        }

        MIDIPortDispose(midi_port);
        MIDIClientDispose(midi_client);
        midi_client = 0;
        midi_unique_id = 0;
    }

    notification_events = 0;

    pthread_mutex_destroy(&notification_mutex);
    pthread_mutex_destroy(&midi_mutex);

    if (reset_controller_events != NULL)
    {
        free(reset_controller_events);
        reset_controller_events = NULL;
    }

    if (initial_sysex_events != NULL)
    {
        free(initial_sysex_events);
        initial_sysex_events = NULL;
    }
#endif
}


__attribute__ ((visibility ("default")))
int MIDI_PLUGIN2_API initialize_midi_plugin2(midi_plugin2_parameters const *parameters, midi_plugin2_functions *functions)
{
    if (functions == NULL) return -3;

    midi_type = (parameters != NULL) ? parameters->midi_type : 0;

    functions->play = &play;
    functions->pause = &pause_0;
    functions->resume = &resume;
    functions->halt = &halt;
    functions->set_volume = &set_volume;
    functions->set_loop_count = &set_loop_count;
    functions->shutdown_plugin = &shutdown_plugin;

#ifdef PLUGIN_ENABLED
{
    char const *device_name;
    unsigned char const *sysex_events, *controller_events;
    int events_len;
    pthread_attr_t thread_attr;
    pthread_mutexattr_t mutex_attr;

    device_name = NULL;
    sysex_events = NULL;
    controller_events = NULL;
    mt32_delay = 0;
    if (parameters != NULL)
    {
        device_name = parameters->midi_device_name;
        if (midi_type != 2) sysex_events = parameters->initial_sysex_events;
        if (midi_type != 2) controller_events = parameters->reset_controller_events;
        if (midi_type) mt32_delay = parameters->mt32_delay;
        prepare_mt32_display_sysex(parameters->mt32_display_text);
    }

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);

    if (pthread_mutex_init(&midi_mutex, &mutex_attr))
    {
        pthread_mutexattr_destroy(&mutex_attr);
        return -7;
    }

    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_NORMAL);

    if (pthread_mutex_init(&notification_mutex, &mutex_attr))
    {
        pthread_mutex_destroy(&midi_mutex);
        pthread_mutexattr_destroy(&mutex_attr);
        return -7;
    }

    pthread_mutexattr_destroy(&mutex_attr);

    if (noErr != MIDIClientCreate(CFSTR("midi2-coremidi"), notify_procedure, NULL, &midi_client))
    {
        midi_client = 0;
        pthread_mutex_destroy(&notification_mutex);
        pthread_mutex_destroy(&midi_mutex);
        return -4;
    }

    if (find_endpoint(device_name) < 0)
    {
        MIDIClientDispose(midi_client);
        midi_client = 0;
        pthread_mutex_destroy(&notification_mutex);
        pthread_mutex_destroy(&midi_mutex);
        return -5;
    }

    if (noErr != MIDIOutputPortCreate(midi_client, CFSTR("midi2-coremidi"), &midi_port))
    {
        MIDIClientDispose(midi_client);
        midi_client = 0;
        pthread_mutex_destroy(&notification_mutex);
        pthread_mutex_destroy(&midi_mutex);
        return -6;
    }

#if (MIDI_API == 0)
    use_ump = 0;
    if (__builtin_available(macOS 11.0, *)) use_ump = 1;
#endif
    mach_timebase_info(&tb_info);

    midi_last_time = 0;

    if (midi_type == 2)
    {
        mt32_initialize_gm();
    }
    else if (midi_type)
    {
        if (state_mt32_display == 1)
        {
            state_mt32_display = (mt32_delay) ? 4 : 5;
            send_initial_sysex_events(sysex_mt32_display);
        }
    }

    if (sysex_events != NULL && *sysex_events == 0xf0)
    {
        events_len = send_initial_sysex_events(sysex_events);

        if (events_len > 1)
        {
            initial_sysex_events = (unsigned char *)malloc(events_len);
            if (initial_sysex_events != NULL)
            {
                memcpy(initial_sysex_events, sysex_events, events_len);
            }
        }
    }

    if (controller_events != NULL && *controller_events == 0xb0)
    {
        events_len = 1;
        while (controller_events[events_len] != 0xff) events_len++;

        if (events_len > 1)
        {
            reset_controller_events = (unsigned char *)malloc(events_len);
            if (reset_controller_events != NULL)
            {
                memcpy(reset_controller_events, controller_events + 1, events_len);
            }
        }
    }

    reset_playing();

    pthread_attr_init(&thread_attr);

    //pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    if (pthread_create(&midi_thread, &thread_attr, &midi_thread_proc, NULL) != 0)
    {
        pthread_attr_destroy(&thread_attr);
        if (midi_type == 2)
        {
            mt32_shutdown_gm();
        }
        MIDIPortDispose(midi_port);
        MIDIClientDispose(midi_client);
        midi_client = 0;
        shutdown_plugin();
        return -7;
    }

    pthread_attr_destroy(&thread_attr);
}
#endif

    return 0;
}

