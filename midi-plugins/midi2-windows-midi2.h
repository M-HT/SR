/**
 *
 *  Copyright (C) 2026 Roman Pauer
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

#include <roapi.h>
#include <winstring.h>


// Windows Runtime Functions
typedef HRESULT(WINAPI *CoCreateInstanceFunc)(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv);
typedef HRESULT(WINAPI *RoInitializeFunc)(RO_INIT_TYPE initType);
typedef HRESULT(WINAPI *RoGetActivationFactoryFunc)(HSTRING activatableClassId, REFIID iid, void **factory);
typedef void(WINAPI *RoUninitializeFunc)();
typedef HRESULT(WINAPI *WindowsCreateStringReferenceFunc)(PCWSTR sourceString, UINT32 length, HSTRING_HEADER *hstringHeader, HSTRING *string);
typedef HRESULT(WINAPI *WindowsCompareStringOrdinalFunc)(HSTRING string1, HSTRING string2, INT32 *result);
typedef HRESULT(WINAPI *WindowsDeleteStringFunc)(HSTRING string);

enum {
    NOTIFY_REMOVE   = 1 << 0,
    NOTIFY_ADD      = 1 << 1,
    NOTIFY_CHANGEID = 1 << 2
};

#include "midi2-windows-midi2-winrt.h"


static IMidiClientInitializer *midi2_initializer = NULL;
static IMidiClockStatics *midi2_clock = NULL;
static IMidiSession *midi2_session = NULL;
static IMidiEndpointConnection *midi2_endpoint_connection = NULL;
static IMidiEndpointDeviceWatcher *midi2_watcher = NULL;
static int64_t midi2_notify_added_token, midi2_notify_removed_token, midi2_notify_updated_token;
static HSTRING midi2_endpoint_id;
static unsigned int midi2_group, midi2_max_words;

static HMODULE hCombase = NULL;
static CoCreateInstanceFunc dyn_CoCreateInstance;
static RoInitializeFunc dyn_RoInitialize;
static RoGetActivationFactoryFunc dyn_RoGetActivationFactory;
static RoUninitializeFunc dyn_RoUninitialize;
static WindowsCreateStringReferenceFunc dyn_WindowsCreateStringReference;
static WindowsCompareStringOrdinalFunc dyn_WindowsCompareStringOrdinal;
static WindowsDeleteStringFunc dyn_WindowsDeleteString;

static volatile uint64_t midi2_base_timestamp, midi2_last_timestamp, midi2_pause_timestamp;

#define MIDI2_BUFFER_SIZE 16384
static uint32_t midi2_buf[MIDI2_BUFFER_SIZE];


static unsigned int midi2_convert_sysex_to_ump(uint32_t *ump_data, uint8_t status, const uint8_t *sysex_data, unsigned int sysex_len)
{
    int sysex_hasstart, sysex_hasend;
    unsigned int ump_len;

    sysex_hasstart = (status == 0xf0);
    sysex_hasend = (sysex_len != 0 && sysex_data[sysex_len - 1] == 0xf7);
    if (sysex_hasend) sysex_len--;

    if (sysex_len <= 6)
    {
        // one 64-bit UMP
        ump_data[0] = (MidiMessageType_DataMessage64 << 28) | (midi2_group << 24) | ((sysex_hasstart ? (sysex_hasend ? MidiSystemExclusive8Status_CompleteMessageInSingleMessagePacket : MidiSystemExclusive8Status_StartMessagePacket) : (sysex_hasend ? MidiSystemExclusive8Status_EndMessagePacket : MidiSystemExclusive8Status_ContinueMessagePacket)) << 20) | (sysex_len << 16) | ((sysex_len > 0) ? (sysex_data[0] << 8) : 0) | ((sysex_len > 1) ? sysex_data[1] : 0);
        ump_data[1] = ((sysex_len > 2) ? (sysex_data[2] << 24) : 0) | ((sysex_len > 3) ? (sysex_data[3] << 16) : 0) | ((sysex_len > 4) ? (sysex_data[4] << 8) : 0) | ((sysex_len > 5) ? sysex_data[5] : 0);
        ump_len = 2;
    }
    else
    {
        ump_data[0] = (MidiMessageType_DataMessage64 << 28) | (midi2_group << 24) | ((sysex_hasstart ? MidiSystemExclusive8Status_StartMessagePacket : MidiSystemExclusive8Status_ContinueMessagePacket) << 20) | (6 << 16) | (sysex_data[0] << 8) | sysex_data[1];
        ump_data[1] = (sysex_data[2] << 24) | (sysex_data[3] << 16) | (sysex_data[4] << 8) | sysex_data[5];
        ump_len = 2;
        sysex_len -= 6;
        sysex_data += 6;
        while (sysex_len > 6)
        {
            ump_data[ump_len] = (MidiMessageType_DataMessage64 << 28) | (midi2_group << 24) | (MidiSystemExclusive8Status_ContinueMessagePacket << 20) | (6 << 16) | (sysex_data[0] << 8) | sysex_data[1];
            ump_data[ump_len + 1] = (sysex_data[2] << 24) | (sysex_data[3] << 16) | (sysex_data[4] << 8) | sysex_data[5];
            ump_len += 2;
            sysex_len -= 6;
            sysex_data += 6;
        }
        ump_data[ump_len] = (MidiMessageType_DataMessage64 << 28) | (midi2_group << 24) | ((sysex_hasend ? MidiSystemExclusive8Status_EndMessagePacket : MidiSystemExclusive8Status_ContinueMessagePacket) << 20) | (sysex_len << 16) | ((sysex_len > 0) ? (sysex_data[0] << 8) : 0) | ((sysex_len > 1) ? sysex_data[1] : 0);
        ump_data[ump_len + 1] = ((sysex_len > 2) ? (sysex_data[2] << 24) : 0) | ((sysex_len > 3) ? (sysex_data[3] << 16) : 0) | ((sysex_len > 4) ? (sysex_data[4] << 8) : 0) | ((sysex_len > 5) ? sysex_data[5] : 0);
        ump_len += 2;
    }

    return ump_len;
}

// todo:
static int midi2_add_midi_event(preprocess_state *state, uint8_t status, uint8_t byte1, uint8_t byte2, const uint8_t *data_ptr, unsigned int data_len)
{
    midi_event_info event;

    event.tick = state->last_tick;
    event.sysex = NULL;
    event.time = state->event_time;

    if ((status >> 4) != MIDI_STATUS_SYSEX)
    {
        event.data_or_len = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (status << 16) | (byte1 << 8) | (byte2);
    }
    else
    {
        if (status == 0xff) // meta events
        {
            if ((byte1 == 0x51) && (data_len == 3)) // set tempo
            {
                // time_division is assumed to be positive (ticks per beat / PPQN - Pulses (i.e. clocks) Per Quarter Note)

                event.data_or_len = 0;

                state->tempo = (((uint32_t)(data_ptr[0])) << 16) | (((uint32_t)(data_ptr[1])) << 8) | ((uint32_t)(data_ptr[2]));
                state->tempo_tick = event.tick;
                state->tempo_time = event.time;
            }
            else return 0;
        }
        else if ((status == 0xf0) || (status == 0xf7)) // sysex
        {
            if (data_len == 0 && status == 0xf7) return 0;

            event.data_or_len = 2 * ((data_len + ((data_len != 0 && data_ptr[data_len - 1] == 0xf7) ? 4 : 5)) / 6);
            if (event.data_or_len == 0) event.data_or_len = 2;
            else if (event.data_or_len > MIDI2_BUFFER_SIZE) return 0;

            event.sysex = malloc(event.data_or_len * sizeof(uint32_t));
            if (event.sysex == NULL) return 12;

            midi2_convert_sysex_to_ump((uint32_t *)event.sysex, status, data_ptr, data_len);

            if (mt32_delay)
            {
                if (event.time < state->next_sysex_time)
                {
                    // calculate new event tick and time
                    calculate_next_sysex_tick(state);
                    calculate_event_time(state);

                    event.tick = state->last_tick;
                    event.time = state->event_time;
                }

                state->next_sysex_time = event.time + (40 + 10 + ((event.data_or_len * 10000 + 31249) / 31250));
            }
        }
        else if ((status == 0xf4) && mt32_delay) // extra event
        {
            event.data_or_len = 0;
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
    state->events[0].data_or_len = state->num_events;

    return 0;
}

static void midi2_drain_midi_buffer(uint64_t timestamp, unsigned int word_count)
{
    unsigned int index, count;
    uint32_t results;

    if (midi2_endpoint_connection == NULL) return;

    index = 0;
    while (word_count != 0)
    {
        count = (word_count <= midi2_max_words) ? word_count : midi2_max_words;
        IMidiEndpointConnection_SendMultipleMessagesWordArray(midi2_endpoint_connection, timestamp, index, count, MIDI2_BUFFER_SIZE, midi2_buf, &results);
        index += count;
        word_count -= count;
    }
}

static void midi2_disconnect_and_release_connection(IMidiEndpointConnection *connection)
{
    HRESULT hr;
    IMidiEndpointConnectionSource *source;
    GUID connection_id;

    hr = IMidiEndpointConnection_QueryInterface(connection, REF(IID_IMidiEndpointConnectionSource), (void **)&source);
    if (SUCCEEDED(hr))
    {
        hr = IMidiEndpointConnectionSource_get_ConnectionId(source, &connection_id);
        if (SUCCEEDED(hr))
        {
            IMidiSession_DisconnectEndpointConnection(midi2_session, &connection_id);
        }

        IMidiEndpointConnectionSource_Release(source);
    }

    IMidiEndpointConnection_Release(connection);
}


static int MIDI_PLUGIN2_API midi2_pause(void);
static int MIDI_PLUGIN2_API midi2_resume(void);
static int midi2_send_initial_sysex_events(unsigned char const *sysex_events);
static void midi2_reset_playing(void);

static DWORD WINAPI midi2_MidiThreadProc(LPVOID lpParameter)
{
    midi_event_info *events;
    unsigned int current_event, num_events, insert_events, current_notification_events;
    uint64_t base_timestamp;
    int do_sleep, endpoint_exists, chan, more_events;
    int64_t time_diff, base_diff;
    HANDLE AvrtHandle;
    HRESULT hr;
    DWORD TaskIndex, BaseTicks;
    unsigned int word_count, uninitialize;
    uint64_t timestamp_now, new_timestamp;
    double time_ms;

    BaseTicks = GetTickCount();

    AvrtHandle = NULL;
    TaskIndex = 0;
    if (hAvrt != NULL)
    {
        AvrtHandle = dyn_AvSetMmThreadCharacteristicsW(L"Pro Audio", &TaskIndex);
    }
    hr = dyn_RoInitialize(RO_INIT_MULTITHREADED);
    uninitialize = (hr == S_OK || hr == S_FALSE);

    do_sleep = 1;
    endpoint_exists = 1;

    for (;;)
    {
        if (do_sleep)
        {
            do_sleep = 0;
            Sleep(10);
        }

        if (midi_quit)
        {
            if (uninitialize) dyn_RoUninitialize();
            if (AvrtHandle != NULL)
            {
                dyn_AvRevertMmThreadCharacteristics(AvrtHandle);
            }
            return 0;
        }

        if (notification_events != 0)
        {
            EnterCriticalSection(&notification_critical_section);
            current_notification_events = notification_events;
            notification_events = 0;
            LeaveCriticalSection(&notification_critical_section);

            if ((current_notification_events & NOTIFY_CHANGEID) && endpoint_exists)
            {
                IMidiEndpointConnectionSource *source;
                HSTRING endpoint_id;

                endpoint_id = NULL;
                hr = IMidiEndpointConnection_QueryInterface(midi2_endpoint_connection, REF(IID_IMidiEndpointConnectionSource), (void **)&source);
                if (SUCCEEDED(hr))
                {
                    hr = IMidiEndpointConnectionSource_get_ConnectedEndpointDeviceId(source, &endpoint_id);
                    if (FAILED(hr)) endpoint_id = NULL;

                    IMidiEndpointConnectionSource_Release(source);
                }

                if (endpoint_id != NULL)
                {
                    EnterCriticalSection(&notification_critical_section);
                    if (midi2_endpoint_id != NULL)
                    {
                        dyn_WindowsDeleteString(midi2_endpoint_id);
                    }
                    midi2_endpoint_id = endpoint_id;
                    LeaveCriticalSection(&notification_critical_section);
                }
            }

            if ((current_notification_events & NOTIFY_REMOVE) && endpoint_exists)
            {
                endpoint_exists = 0;

                EnterCriticalSection(&midi_critical_section);
                midi2_disconnect_and_release_connection(midi2_endpoint_connection);
                midi2_endpoint_connection = NULL;

                if (midi_loaded)
                {
                    if (midi_loaded && (midi_loop_count == 0))
                    {
                        midi_eof = 1;
                    }
                }
                LeaveCriticalSection(&midi_critical_section);
            }

            if ((current_notification_events & NOTIFY_ADD) && !endpoint_exists)
            {
                IMidiEndpointConnection *connection;
                bool opened;

                hr = IMidiSession_CreateEndpointConnection(midi2_session, midi2_endpoint_id, &connection);
                if (SUCCEEDED(hr))
                {
                    hr = IMidiEndpointConnection_Open(connection, &opened);
                    if (FAILED(hr) || !opened)
                    {
                        midi2_disconnect_and_release_connection(connection);
                        connection = NULL;
                    }
                }
                else connection = NULL;

                if (connection != NULL)
                {
                    int was_playing;

                    endpoint_exists = 1;

                    EnterCriticalSection(&midi_critical_section);

                    was_playing = (midi_loaded && midi_playing);
                    if (was_playing)
                    {
                        midi2_pause();
                    }

                    midi2_endpoint_connection = connection;

                    if (midi_type == 2)
                    {
                        mt32_initialize_gm();
                    }

                    if (initial_sysex_events != NULL)
                    {
                        midi2_send_initial_sysex_events(initial_sysex_events);
                    }

                    midi2_reset_playing();

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
                            hr = IMidiClockStatics_OffsetTimestampByMilliseconds(midi2_clock, midi2_base_timestamp, events[current_event].time, &new_timestamp);
                            if (SUCCEEDED(hr))
                            {
                                midi2_base_timestamp = new_timestamp;
                            }
                        }
                    }

                    if (was_playing)
                    {
                        midi2_resume();
                    }

                    LeaveCriticalSection(&midi_critical_section);
                }
            }
        }

        if ((!midi_loaded) || (!midi_playing))
        {
            do_sleep = 1;
            continue;
        }


        EnterCriticalSection(&midi_critical_section);

        if ((!midi_loaded) || (!midi_playing) || (midi_eof))
        {
            LeaveCriticalSection(&midi_critical_section);
            do_sleep = 1;
            continue;
        }

        if ((!endpoint_exists) && (midi_loop_count == 0))
        {
            midi_eof = 1;
            LeaveCriticalSection(&midi_critical_section);
            do_sleep = 1;
            continue;
        }

        events = (midi_event_info *) midi_events;
        current_event = midi_current_event;
        base_timestamp = midi2_base_timestamp;

        num_events = events[0].data_or_len;

        if (current_event > num_events)
        {
            if (midi_loop_count == 0)
            {
                midi_eof = 1;
                LeaveCriticalSection(&midi_critical_section);
                do_sleep = 1;
                continue;
            }
            else if (midi_loop_count > 0)
            {
                midi_loop_count--;
            }

            // looping
            hr = IMidiClockStatics_OffsetTimestampByMilliseconds(midi2_clock, base_timestamp, events[num_events].time, &new_timestamp);
            if (SUCCEEDED(hr))
            {
                base_timestamp = new_timestamp;
            }

            current_event = 1;

            midi_current_event = current_event;
            midi2_base_timestamp = base_timestamp;
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
                if (GetTickCount() - BaseTicks >= 5000)
                {
                    state_mt32_display = 2;
                    insert_events |= 2;
                }
            }
        }

        if (insert_events)
        {
            word_count = 0;

            if (insert_events & 1)
            {
                for (chan = 0; chan < MIDI_CHANNELS; chan++)
                {
                    midi2_buf[word_count] = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (7 << 8) | ((midi_current_volume * channel_volume[chan]) / 127);
                    word_count++;
                }
            }
            if (insert_events & 2)
            {
                word_count += midi2_convert_sysex_to_ump(midi2_buf + word_count, sysex_mt32_reset_display[0], sysex_mt32_reset_display + 1, sizeof(sysex_mt32_reset_display) - 2);
            }

            midi2_drain_midi_buffer(midi2_last_timestamp, word_count);
        }

        time_diff = 100;
        hr = IMidiClockStatics_get_Now(midi2_clock, &timestamp_now);
        if (SUCCEEDED(hr))
        {
            base_diff = timestamp_now - base_timestamp;

            hr = IMidiClockStatics_ConvertTimestampTicksToMilliseconds(midi2_clock, (base_diff >= 0) ? base_diff : -base_diff, &time_ms);
            if (SUCCEEDED(hr))
            {
                base_diff = (base_diff >= 0) ? (int64_t)time_ms : -(int64_t)time_ms;
                time_diff = events[current_event].time - base_diff;
            }
        }

        if (time_diff >= 100)
        {
            LeaveCriticalSection(&midi_critical_section);
            do_sleep = 1;
            continue;
        }

        more_events = 1;

        do
        {
            word_count = 0;

            for (;;)
            {
                if (events[current_event].data_or_len == 0)
                {
                    // do nothing
                }
                else if (events[current_event].sysex != NULL)
                {
                    if (word_count + events[current_event].data_or_len > MIDI2_BUFFER_SIZE)
                    {
                        // sysex doesn't fit -> drain buffer
                        break;
                    }
                    else
                    {
                        memcpy(midi2_buf + word_count, events[current_event].sysex, events[current_event].data_or_len * sizeof(uint32_t));
                        word_count += events[current_event].data_or_len;
                    }
                }
                else
                {
                    if (word_count >= MIDI2_BUFFER_SIZE)
                    {
                        // event doesn't fit -> drain buffer
                        break;
                    }
                    else
                    {
                        midi2_buf[word_count] = events[current_event].data_or_len;
                        word_count++;

                        switch ((events[current_event].data_or_len >> 20) & 0x0f)
                        {
                            case MIDI_STATUS_NOTE_ON:
                                if ((events[current_event].data_or_len & 0xff) != 0)
                                {
                                    // note on
                                    channel_notes[(events[current_event].data_or_len >> 16) & 0x0f][(events[current_event].data_or_len >> 8) & 0xff]++;
                                    break;
                                }
                                // fallthrough
                            case MIDI_STATUS_NOTE_OFF:
                                // note off
                                if (channel_notes[(events[current_event].data_or_len >> 16) & 0x0f][(events[current_event].data_or_len >> 8) & 0xff] > 0)
                                {
                                    channel_notes[(events[current_event].data_or_len >> 16) & 0x0f][(events[current_event].data_or_len >> 8) & 0xff]--;
                                }
                                break;
                            case MIDI_STATUS_CONTROLLER:
                                if (((events[current_event].data_or_len >> 8) & 0xff) == 7)
                                {
                                    // volume
                                    channel_volume[(events[current_event].data_or_len >> 16) & 0x0f] = events[current_event].data_or_len & 0xff;
                                    midi2_buf[word_count - 1] = (midi2_buf[word_count - 1] & 0xffffff00) | ((midi_current_volume * (events[current_event].data_or_len & 0xff)) / 127);
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }

                current_event++;
                if (current_event > num_events)
                {
                    more_events = 0;
                    break;
                }
                else if (events[current_event].time != events[current_event - 1].time)
                {
                    // next event has different time -> send data in buffer
                    time_diff = events[current_event].time - base_diff;
                    if (time_diff >= 100) more_events = 0;
                    break;
                }
            }


            if (word_count != 0)
            {
                hr = IMidiClockStatics_OffsetTimestampByMilliseconds(midi2_clock, base_timestamp, events[current_event - 1].time, &new_timestamp);
                midi2_drain_midi_buffer((SUCCEEDED(hr)) ? new_timestamp : 0, word_count);
            }
        } while (more_events);

        midi_current_event = current_event;
        hr = IMidiClockStatics_OffsetTimestampByMilliseconds(midi2_clock, base_timestamp, events[current_event - 1].time, &new_timestamp);
        if (SUCCEEDED(hr))
        {
            midi2_last_timestamp = new_timestamp;
        }

        LeaveCriticalSection(&midi_critical_section);
    };
}

// todo:
static int midi2_send_initial_sysex_events(unsigned char const *sysex_events)
{
    int events_len, len, delay_len, min_delay;
    uint64_t next_timestamp;
    unsigned int word_count;
    HRESULT hr;
    uint64_t timestamp_now, new_timestamp;

    if (midi2_session == NULL) return 0;

    events_len = 0;

    EnterCriticalSection(&midi_critical_section);

    next_timestamp = 0;

    if (mt32_delay)
    {
        hr = IMidiClockStatics_get_Now(midi2_clock, &timestamp_now);
        if (SUCCEEDED(hr))
        {
            hr = IMidiClockStatics_OffsetTimestampByMilliseconds(midi2_clock, timestamp_now, 40 + 10 + 10, &new_timestamp);
            if (SUCCEEDED(hr))
            {
                next_timestamp = new_timestamp;
            }
        }
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

        word_count = midi2_convert_sysex_to_ump(midi2_buf, sysex_events[0], sysex_events + 1, len - 1);

        midi2_drain_midi_buffer(next_timestamp, word_count);

        sysex_events += len + delay_len;
        events_len += len + delay_len;

        if (mt32_delay)
        {
            if (min_delay < 40) min_delay = 40;
            hr = IMidiClockStatics_OffsetTimestampByMilliseconds(midi2_clock, next_timestamp, min_delay + 10 + ((len * 10000 + 31249) / 31250), &new_timestamp);
            if (SUCCEEDED(hr))
            {
                next_timestamp = new_timestamp;
            }
        }
    };

    events_len++;

    if (mt32_delay)
    {
        do
        {
            Sleep(100);
            hr = IMidiClockStatics_get_Now(midi2_clock, &timestamp_now);
            if (FAILED(hr)) break;
        } while ((int64_t)(timestamp_now - next_timestamp) > 0);
    }

    LeaveCriticalSection(&midi_critical_section);

    return events_len;
}

static void midi2_reset_playing(void)
{
    int chan;
    unsigned int word_count;
    int64_t time_diff;
    unsigned char *controller_events;
    HRESULT hr;
    uint64_t timestamp_now;
    double time_ms;

    if (midi2_session == NULL) return;

    EnterCriticalSection(&midi_critical_section);

    word_count = 0;

    for (chan = 0; chan < MIDI_CHANNELS; chan++)
    {
        // MT-32 doesn't support All sounds off, so Omni off is used instead
        midi2_buf[word_count] = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | ((midi_type ? 0x7c : 0x78) << 8) | (0); // Omni off / All sounds off (abrupt stop of sound on channel)
        word_count++;

        midi2_buf[word_count] = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (0x79 << 8) | (0); // All controllers off (this message clears all the controller values for this channel, back to their default values)
        word_count++;

        midi2_buf[word_count] = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (0x7b << 8) | (0); // All notes off (this message stops all the notes that are currently playing)
        word_count++;

        // All controllers off doesn't set volume and pan to default values
        // Volume is set at start of playing, so only pan is set to default value
        midi2_buf[word_count] = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (0x0a << 8) | (64); // Pan
        word_count++;

        if (reset_controller_events != NULL)
        {
            controller_events = reset_controller_events;
            while (*controller_events != 0xff)
            {
                midi2_buf[word_count] = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (MIDI_STATUS_CONTROLLER << 20) | (chan << 16) | (controller_events[0] << 8) | (controller_events[1]);
                word_count++;

                controller_events += 2;
            }
        }
    }

    midi2_buf[word_count] = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (MIDI_STATUS_PROG_CHANGE << 20) | (9 << 16) | (0 << 8);
    word_count++;

    for (chan = 0; chan < MIDI_CHANNELS; chan++)
    {
        midi2_buf[word_count] = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (MIDI_STATUS_PITCH_WHEEL << 20) | (chan << 16) | (0x00 << 8) | (0x40);
        word_count++;
    }

    midi2_drain_midi_buffer(midi_loaded ? midi2_last_timestamp : 0, word_count);

    if (midi_loaded)
    {
        hr = IMidiClockStatics_get_Now(midi2_clock, &timestamp_now);
        if (SUCCEEDED(hr))
        {
            time_diff = timestamp_now - midi2_last_timestamp;

            if (time_diff > 0)
            {
                hr = IMidiClockStatics_ConvertTimestampTicksToMilliseconds(midi2_clock, time_diff, &time_ms);
                if (SUCCEEDED(hr))
                {
                    Sleep((DWORD)time_ms);
                }
            }
        }
    }

    LeaveCriticalSection(&midi_critical_section);
}

static void midi2_close_midi(void)
{
    if (!midi_loaded) return;

    midi2_reset_playing();

    free_midi_data((midi_event_info *)midi_events);
    midi_loaded = 0;
}


static void midi2_load_windows_dlls(void)
{
    hCombase = LoadLibraryExW(L"combase.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hCombase != NULL)
    {
        dyn_CoCreateInstance = (CoCreateInstanceFunc)GetProcAddress(hCombase, "CoCreateInstance");
        dyn_RoInitialize = (RoInitializeFunc)GetProcAddress(hCombase, "RoInitialize");
        dyn_RoGetActivationFactory = (RoGetActivationFactoryFunc)GetProcAddress(hCombase, "RoGetActivationFactory");
        dyn_RoUninitialize = (RoUninitializeFunc)GetProcAddress(hCombase, "RoUninitialize");
        dyn_WindowsCreateStringReference = (WindowsCreateStringReferenceFunc)GetProcAddress(hCombase, "WindowsCreateStringReference");
        dyn_WindowsCompareStringOrdinal = (WindowsCompareStringOrdinalFunc)GetProcAddress(hCombase, "WindowsCompareStringOrdinal");
        dyn_WindowsDeleteString = (WindowsDeleteStringFunc)GetProcAddress(hCombase, "WindowsDeleteString");
        if ((dyn_CoCreateInstance == NULL) ||
            (dyn_RoInitialize == NULL) ||
            (dyn_RoGetActivationFactory == NULL) ||
            (dyn_RoUninitialize == NULL) ||
            (dyn_WindowsCreateStringReference == NULL) ||
            (dyn_WindowsCompareStringOrdinal == NULL) ||
            (dyn_WindowsDeleteString == NULL)
           )
        {
            FreeLibrary(hCombase);
            hCombase = NULL;
        }
    }
}

static int midi2_initialize_midi_sdk_and_service(void)
{
    HRESULT hr;
    IMidiClientInitializer *initializer;

    initializer = NULL;
    hr = dyn_CoCreateInstance(REF(IID_MidiClientInitializerUuid), NULL, CLSCTX_INPROC_SERVER | CLSCTX_FROM_DEFAULT_CONTEXT, REF(IID_IMidiClientInitializer), (void **)&initializer);
    if (FAILED(hr) || initializer == NULL) return -1;

    hr = IMidiClientInitializer_EnsureServiceAvailable(initializer);
    if (FAILED(hr))
    {
        IMidiClientInitializer_Release(initializer);
        return -2;
    }

    midi2_initializer = initializer;
    return 0;
}

static int midi2_initialize_clock(void)
{
    HRESULT hr;
    HSTRING class_id;
    IMidiClockStatics *clock, *agile; // agile only implements IAgileObject/IUnknown interface
    HSTRING_HEADER hstring_header;

    hr = dyn_WindowsCreateStringReference(name_MidiClock, (UINT32)wcslen(name_MidiClock), &hstring_header, &class_id);
    if (FAILED(hr)) return -1;

    // get clock (interface to static functions)
    hr = dyn_RoGetActivationFactory(class_id, REF(IID_IMidiClockStatics), (void **)&clock);
    if (FAILED(hr)) return -2;

    // verify that clock implements IAgileObject interface
    hr = IMidiClockStatics_QueryInterface(clock, REF(IID_IAgileObject), (void **)&agile);
    if (FAILED(hr))
    {
        IMidiClockStatics_Release(clock);
        return -3;
    }
    IMidiClockStatics_Release(agile);

    midi2_clock = clock;
    return 0;
}

static int midi2_create_session(const wchar_t *name)
{
    HRESULT hr;
    HSTRING class_id, session_name;
    IMidiSessionStatics *factory, *agile; // agile only implements IAgileObject/IUnknown interface
    IMidiSession *session;
    HSTRING_HEADER hstring_header;

    hr = dyn_WindowsCreateStringReference(name_MidiSession, (UINT32)wcslen(name_MidiSession), &hstring_header, &class_id);
    if (FAILED(hr)) return -1;

    // get session factory (interface to static functions)
    hr = dyn_RoGetActivationFactory(class_id, REF(IID_IMidiSessionStatics), (void **)&factory);
    if (FAILED(hr)) return -2;

    // verify that factory implements IAgileObject interface
    hr = IMidiSessionStatics_QueryInterface(factory, REF(IID_IAgileObject), (void **)&agile);
    if (FAILED(hr))
    {
        IMidiSessionStatics_Release(factory);
        return -3;
    }
    IMidiSessionStatics_Release(agile);

    // at this point class_id is no longer used, so hstring_header can be reused
    hr = dyn_WindowsCreateStringReference(name, (UINT32)wcslen(name), &hstring_header, &session_name);
    if (FAILED(hr))
    {
        IMidiSessionStatics_Release(factory);
        return -4;
    }

    // create session
    hr = IMidiSessionStatics_Create(factory, session_name, &session);
    IMidiSessionStatics_Release(factory);
    if (FAILED(hr)) return -5;

    midi2_session = session;
    return 0;
}


static int midi2_connect_endpoint(IMidiEndpointDeviceInformation *endpoint)
{
    HRESULT hr;
    HSTRING endpoint_id;
    IMidiEndpointConnection *connection;
    bool opened;
    uint32_t max_words;

    hr = IMidiEndpointDeviceInformation_get_EndpointDeviceId(endpoint, &endpoint_id);
    if (FAILED(hr)) return -1;

    hr = IMidiSession_CreateEndpointConnection(midi2_session, endpoint_id, &connection);
    if (FAILED(hr))
    {
        dyn_WindowsDeleteString(endpoint_id);
        return -2;
    }

    hr = IMidiEndpointConnection_Open(connection, &opened);
    if (FAILED(hr) || !opened)
    {
        midi2_disconnect_and_release_connection(connection);
        dyn_WindowsDeleteString(endpoint_id);
        return -3;
    }

    hr = IMidiEndpointConnection_GetSupportedMaxMidiWordsPerTransmission(connection, &max_words);
    max_words &= ~1;
    if (FAILED(hr) || max_words == 0)
    {
        midi2_disconnect_and_release_connection(connection);
        dyn_WindowsDeleteString(endpoint_id);
        return -4;
    }

    if (max_words > MIDI2_BUFFER_SIZE) max_words = MIDI2_BUFFER_SIZE;
    midi2_max_words = max_words;
    midi2_endpoint_connection = connection;
    midi2_endpoint_id = endpoint_id;
    return 0;
}

static int midi2_find_endpoint(const char *device_name_utf8)
{
    HRESULT hr;
    INT32 cr;
    wchar_t *device_name_utf16;
    HSTRING class_id, device_name, endpoint_name, endpoint_id, block_name;
    IMidiEndpointDeviceInformationStatics *factory, *agile; // agile only implements IAgileObject/IUnknown interface
    IVectorView *endpoints, *function_blocks, *group_terminal_blocks;
    IMidiEndpointDeviceInformation *endpoint;
    IMidiFunctionBlock *function_block;
    IMidiGroupTerminalBlock *group_terminal_block;
    IMidiGroup *block_group;
    uint32_t num_endpoints, endpoint_index, num_blocks, block_index;
    int same_name, first_group;
    enum MidiFunctionBlockDirection function_block_direction;
    enum MidiGroupTerminalBlockDirection group_terminal_block_direction;
    uint8_t group_index;
    struct MidiEndpointTransportSuppliedInfo transport_info;
    HSTRING_HEADER hstring_header;

    hr = dyn_WindowsCreateStringReference(name_MidiEndpointDeviceInformation, (UINT32)wcslen(name_MidiEndpointDeviceInformation), &hstring_header, &class_id);
    if (FAILED(hr)) return -1;

    // get factory (interface to static functions)
    hr = dyn_RoGetActivationFactory(class_id, REF(IID_IMidiEndpointDeviceInformationStatics), (void **)&factory);
    if (FAILED(hr)) return -2;

    // verify that factory implements IAgileObject interface
    hr = IMidiEndpointDeviceInformationStatics_QueryInterface(factory, REF(IID_IAgileObject), (void **)&agile);
    if (FAILED(hr))
    {
        IMidiEndpointDeviceInformationStatics_Release(factory);
        return -3;
    }
    IMidiEndpointDeviceInformationStatics_Release(agile);

    // find endpoints
    hr = IMidiEndpointDeviceInformationStatics_FindAll3(factory, MidiEndpointDeviceInformationSortOrder_None, MidiEndpointDeviceInformationFilters_AllStandardEndpoints, &endpoints);
    IMidiEndpointDeviceWatcherStatics_Release(factory);
    if (FAILED(hr)) return -4;

    hr = IVectorView_get_Size(endpoints, &num_endpoints);
    if (FAILED(hr) || num_endpoints == 0)
    {
        IVectorView_Release(endpoints);
        return -5;
    }

    // prepare name for searching
    device_name_utf16 = NULL;
    device_name = NULL;
    if (device_name_utf8 != NULL && *device_name_utf8 != 0)
    {
        int len;

        len = MultiByteToWideChar(CP_UTF8, 0, device_name_utf8, -1, NULL, 0);
        if (len != 0)
        {
            device_name_utf16 = (wchar_t *)malloc(len * sizeof(wchar_t));
            if (device_name_utf16 != NULL)
            {
                len = MultiByteToWideChar(CP_UTF8, 0, device_name_utf8, -1, device_name_utf16, len);
                if (len != 0)
                {
                    // at this point class_id is no longer used, so hstring_header can be reused
                    hr = dyn_WindowsCreateStringReference(device_name_utf16, len - 1, &hstring_header, &device_name);
                    if (FAILED(hr)) device_name = NULL;
                }

                if (device_name == NULL)
                {
                    free(device_name_utf16);
                    device_name_utf16 = NULL;
                }
            }
        }

        if (device_name_utf16 == NULL)
        {
            IVectorView_Release(endpoints);
            return -6;
        }
    }

    first_group = -1;
    for (endpoint_index = 0; endpoint_index < num_endpoints; endpoint_index++)
    {
        hr = IVectorView_GetAt(endpoints, endpoint_index, (void **)&endpoint);
        if (FAILED(hr)) continue;

        if (device_name != NULL)
        {
            // searching for endpoint by name
            same_name = 0;

            hr = IMidiEndpointDeviceInformation_get_Name(endpoint, &endpoint_name);
            if (SUCCEEDED(hr))
            {
                hr = dyn_WindowsCompareStringOrdinal(endpoint_name, device_name, &cr);
                if (SUCCEEDED(hr) && cr == 0) same_name = 1;

                dyn_WindowsDeleteString(endpoint_name);
            }

            if (!same_name)
            {
                hr = IMidiEndpointDeviceInformation_get_EndpointDeviceId(endpoint, &endpoint_id);
                if (SUCCEEDED(hr))
                {
                    hr = dyn_WindowsCompareStringOrdinal(endpoint_id, device_name, &cr);
                    if (SUCCEEDED(hr) && cr == 0) same_name = 1;

                    dyn_WindowsDeleteString(endpoint_id);
                }
            }

            if (!same_name)
            {
                IMidiEndpointDeviceInformation_Release(endpoint);
                continue;
            }
        }
        else
        {
            // looking for first suitable endpoint

            transport_info.Name = NULL;
            transport_info.Description = NULL;
            transport_info.SerialNumber = NULL;
            transport_info.ManufacturerName = NULL;
            transport_info.TransportCode = NULL;

            hr = IMidiEndpointDeviceInformation_GetTransportSuppliedInfo(endpoint, &transport_info);
            if (SUCCEEDED(hr))
            {
                if (transport_info.Name != NULL) dyn_WindowsDeleteString(transport_info.Name);
                if (transport_info.Description != NULL) dyn_WindowsDeleteString(transport_info.Description);
                if (transport_info.SerialNumber != NULL) dyn_WindowsDeleteString(transport_info.SerialNumber);
                if (transport_info.ManufacturerName != NULL) dyn_WindowsDeleteString(transport_info.ManufacturerName);
                if (transport_info.TransportCode != NULL) dyn_WindowsDeleteString(transport_info.TransportCode);
            }

            // ignore endpoints with LOOP transport
            if (FAILED(hr) || memcmp(&transport_info.TransportId, &IID_Midi2LoopbackMidiTransport, sizeof(GUID)) == 0)
            {
                IMidiEndpointDeviceInformation_Release(endpoint);
                continue;
            }
        }

        // check for function block (or group terminal block) with correct direction and get number of first group in block
        num_blocks = 0;
        hr = IMidiEndpointDeviceInformation_GetDeclaredFunctionBlocks(endpoint, &function_blocks);
        if (SUCCEEDED(hr))
        {
            hr = IVectorView_get_Size(function_blocks, &num_blocks);
            if (FAILED(hr) || num_blocks == 0)
            {
                num_blocks = 0;
                IVectorView_Release(function_blocks);
            }
        }

        if (num_blocks != 0)
        {
            for (block_index = 0; block_index < num_blocks; block_index++)
            {
                hr = IVectorView_GetAt(function_blocks, block_index, (void **)&function_block);
                if (FAILED(hr)) continue;

                hr = IMidiFunctionBlock_get_Direction(function_block, &function_block_direction);
                if (SUCCEEDED(hr) && (function_block_direction == MidiFunctionBlockDirection_BlockInput || function_block_direction == MidiFunctionBlockDirection_Bidirectional))
                {
                    hr = IMidiFunctionBlock_get_FirstGroup(function_block, &block_group);
                    if (SUCCEEDED(hr))
                    {
                        hr = IMidiGroup_get_Index(block_group, &group_index);
                        if (SUCCEEDED(hr))
                        {
                            first_group = group_index;
                        }

                        IMidiGroup_Release(block_group);
                    }
                }

                IMidiFunctionBlock_Release(function_block);

                if (first_group >= 0) break;
            }

            IVectorView_Release(function_blocks);
        }
        else
        {
            num_blocks = 0;
            hr = IMidiEndpointDeviceInformation_GetGroupTerminalBlocks(endpoint, &group_terminal_blocks);
            if (SUCCEEDED(hr))
            {
                hr = IVectorView_get_Size(group_terminal_blocks, &num_blocks);
                if (FAILED(hr) || num_blocks == 0)
                {
                    num_blocks = 0;
                    IVectorView_Release(group_terminal_blocks);
                }
            }

            if (num_blocks != 0)
            {
                for (block_index = 0; block_index < num_blocks; block_index++)
                {
                    hr = IVectorView_GetAt(group_terminal_blocks, block_index, (void **)&group_terminal_block);
                    if (FAILED(hr)) continue;

                    hr = IMidiGroupTerminalBlock_get_Direction(group_terminal_block, &group_terminal_block_direction);
                    if (SUCCEEDED(hr) && (group_terminal_block_direction == MidiGroupTerminalBlockDirection_BlockInput || group_terminal_block_direction == MidiGroupTerminalBlockDirection_Bidirectional))
                    {
                        hr = IMidiGroupTerminalBlock_get_FirstGroup(group_terminal_block, &block_group);
                        if (SUCCEEDED(hr))
                        {
                            hr = IMidiGroup_get_Index(block_group, &group_index);
                            if (SUCCEEDED(hr))
                            {
                                first_group = group_index;
                            }

                            IMidiGroup_Release(block_group);
                        }
                    }

                    IMidiGroupTerminalBlock_Release(group_terminal_block);

                    if (first_group >= 0) break;
                }

                IVectorView_Release(group_terminal_blocks);
            }
        }

        if (first_group >= 0)
        {
            if (midi2_connect_endpoint(endpoint) < 0) first_group = -1;
        }

        IMidiEndpointDeviceInformation_Release(endpoint);

        if (first_group < 0) continue;

        midi2_group = first_group;
        break;
    }

    // if searching by name and not finding endpoint, then search for names of function blocks (or group terminal blocks)
    if (first_group < 0 && device_name != NULL)
    {
        for (endpoint_index = 0; endpoint_index < num_endpoints; endpoint_index++)
        {
            hr = IVectorView_GetAt(endpoints, endpoint_index, (void **)&endpoint);
            if (FAILED(hr)) continue;

            num_blocks = 0;
            hr = IMidiEndpointDeviceInformation_GetDeclaredFunctionBlocks(endpoint, &function_blocks);
            if (SUCCEEDED(hr))
            {
                hr = IVectorView_get_Size(function_blocks, &num_blocks);
                if (FAILED(hr) || num_blocks == 0)
                {
                    num_blocks = 0;
                    IVectorView_Release(function_blocks);
                }
            }

            if (num_blocks != 0)
            {
                for (block_index = 0; block_index < num_blocks; block_index++)
                {
                    hr = IVectorView_GetAt(function_blocks, block_index, (void **)&function_block);
                    if (FAILED(hr)) continue;

                    hr = IMidiFunctionBlock_get_Direction(function_block, &function_block_direction);
                    if (SUCCEEDED(hr) && (function_block_direction == MidiFunctionBlockDirection_BlockInput || function_block_direction == MidiFunctionBlockDirection_Bidirectional))
                    {
                        hr = IMidiFunctionBlock_get_Name(function_block, &block_name);
                        if (SUCCEEDED(hr))
                        {
                            hr = dyn_WindowsCompareStringOrdinal(block_name, device_name, &cr);
                            dyn_WindowsDeleteString(block_name);
                            if (SUCCEEDED(hr) && cr == 0)
                            {
                                hr = IMidiFunctionBlock_get_FirstGroup(function_block, &block_group);
                                if (SUCCEEDED(hr))
                                {
                                    hr = IMidiGroup_get_Index(block_group, &group_index);
                                    if (SUCCEEDED(hr))
                                    {
                                        first_group = group_index;
                                    }

                                    IMidiGroup_Release(block_group);
                                }
                            }
                        }
                    }

                    IMidiFunctionBlock_Release(function_block);

                    if (first_group >= 0) break;
                }
            }
            else
            {
                num_blocks = 0;
                hr = IMidiEndpointDeviceInformation_GetGroupTerminalBlocks(endpoint, &group_terminal_blocks);
                if (SUCCEEDED(hr))
                {
                    hr = IVectorView_get_Size(group_terminal_blocks, &num_blocks);
                    if (FAILED(hr) || num_blocks == 0)
                    {
                        num_blocks = 0;
                        IVectorView_Release(group_terminal_blocks);
                    }
                }

                if (num_blocks != 0)
                {
                    for (block_index = 0; block_index < num_blocks; block_index++)
                    {
                        hr = IVectorView_GetAt(group_terminal_blocks, block_index, (void **)&group_terminal_block);
                        if (FAILED(hr)) continue;

                        hr = IMidiGroupTerminalBlock_get_Direction(group_terminal_block, &group_terminal_block_direction);
                        if (SUCCEEDED(hr) && (group_terminal_block_direction == MidiGroupTerminalBlockDirection_BlockInput || group_terminal_block_direction == MidiGroupTerminalBlockDirection_Bidirectional))
                        {
                            hr = IMidiGroupTerminalBlock_get_Name(group_terminal_block, &block_name);
                            if (SUCCEEDED(hr))
                            {
                                hr = dyn_WindowsCompareStringOrdinal(block_name, device_name, &cr);
                                dyn_WindowsDeleteString(block_name);
                                if (SUCCEEDED(hr) && cr == 0)
                                {
                                    hr = IMidiGroupTerminalBlock_get_FirstGroup(group_terminal_block, &block_group);
                                    if (SUCCEEDED(hr))
                                    {
                                        hr = IMidiGroup_get_Index(block_group, &group_index);
                                        if (SUCCEEDED(hr))
                                        {
                                            first_group = group_index;
                                        }

                                        IMidiGroup_Release(block_group);
                                    }
                                }
                            }
                        }

                        IMidiGroupTerminalBlock_Release(group_terminal_block);

                        if (first_group >= 0) break;
                    }

                    IVectorView_Release(group_terminal_blocks);
                }
            }

            if (first_group >= 0)
            {
                if (midi2_connect_endpoint(endpoint) < 0) first_group = -1;
            }

            IMidiEndpointDeviceInformation_Release(endpoint);

            if (first_group < 0) continue;

            midi2_group = first_group;
            break;
        }
    }

    if (device_name_utf16 != NULL) free(device_name_utf16);

    IVectorView_Release(endpoints);

    return (first_group < 0) ? -7 : 0;
}

static void midi2_notify_added(IMidiEndpointDeviceWatcher *sender, IMidiEndpointDeviceInformationAddedEventArgs *args)
{
    HRESULT hr;
    IMidiEndpointDeviceInformation *added_device;
    HSTRING endpoint_id;
    INT32 cr;

    hr = IMidiEndpointDeviceInformationAddedEventArgs_get_AddedDevice(args, &added_device);
    if (FAILED(hr)) return;

    hr = IMidiEndpointDeviceInformation_get_EndpointDeviceId(added_device, &endpoint_id);
    IMidiEndpointDeviceInformation_Release(added_device);
    if (FAILED(hr)) return;

    EnterCriticalSection(&notification_critical_section);
    hr = dyn_WindowsCompareStringOrdinal(endpoint_id, midi2_endpoint_id, &cr);
    if (SUCCEEDED(hr) && cr == 0)
    {
        notification_events |= NOTIFY_ADD;
    }
    LeaveCriticalSection(&notification_critical_section);

    dyn_WindowsDeleteString(endpoint_id);
};

static void midi2_notify_removed(IMidiEndpointDeviceWatcher *sender, IMidiEndpointDeviceInformationRemovedEventArgs *args)
{
    HRESULT hr;
    HSTRING endpoint_id;
    INT32 cr;

    hr = IMidiEndpointDeviceInformationRemovedEventArgs_get_EndpointDeviceId(args, &endpoint_id);
    if (FAILED(hr)) return;

    EnterCriticalSection(&notification_critical_section);
    hr = dyn_WindowsCompareStringOrdinal(endpoint_id, midi2_endpoint_id, &cr);
    if (SUCCEEDED(hr) && cr == 0)
    {
        notification_events |= NOTIFY_REMOVE;
    }
    LeaveCriticalSection(&notification_critical_section);

    dyn_WindowsDeleteString(endpoint_id);
};

static void midi2_notify_updated(IMidiEndpointDeviceWatcher *sender, IMidiEndpointDeviceInformationUpdatedEventArgs *args)
{
    HRESULT hr;
    HSTRING endpoint_id;
    INT32 cr;
    bool updated;

    hr = IMidiEndpointDeviceInformationUpdatedEventArgs_get_IsDeviceIdentityUpdated(args, &updated);
    if (FAILED(hr) || !updated) return;

    hr = IMidiEndpointDeviceInformationUpdatedEventArgs_get_EndpointDeviceId(args, &endpoint_id);
    if (FAILED(hr)) return;

    EnterCriticalSection(&notification_critical_section);
    hr = dyn_WindowsCompareStringOrdinal(endpoint_id, midi2_endpoint_id, &cr);
    if (SUCCEEDED(hr) && cr == 0)
    {
        notification_events |= NOTIFY_ADD;
    }
    else
    {
        notification_events |= NOTIFY_CHANGEID;
    }
    LeaveCriticalSection(&notification_critical_section);

    dyn_WindowsDeleteString(endpoint_id);
};

// methods for ITypedEventHandler
static HRESULT STDMETHODCALLTYPE midi2_notify_event_QueryInterface(ITypedEventHandler *This, REFIID riid, void **ppvObject)
{
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE midi2_notify_event_AddRef(ITypedEventHandler *This)
{
    return 2;
}

static ULONG STDMETHODCALLTYPE midi2_notify_event_Release(ITypedEventHandler *This)
{
    return 1;
}

static HRESULT STDMETHODCALLTYPE midi2_notify_event_Invoke(void **This, void *sender, void *args)
{
    ((void (*)(void *, void *))(This[1]))(sender, args);
    return S_OK;
}

static int midi2_start_watcher(void)
{
    HRESULT hr;
    HSTRING class_id;
    IMidiEndpointDeviceWatcherStatics *factory, *agile; // agile only implements IAgileObject/IUnknown interface
    IMidiEndpointDeviceWatcher *watcher;
    HSTRING_HEADER hstring_header;

    // vtable for ITypedEventHandler
    static const void *notify_event_vtable[4] = {
        (void *)&midi2_notify_event_QueryInterface,
        (void *)&midi2_notify_event_AddRef,
        (void *)&midi2_notify_event_Release,
        (void *)&midi2_notify_event_Invoke
    };

    // 3 objects with ITypedEventHandler interface
    static const void *notify_event_added[2] = { (void *)&notify_event_vtable, (void *)&midi2_notify_added };
    static const void *notify_event_removed[2] = { (void *)&notify_event_vtable, (void *)&midi2_notify_removed };
    static const void *notify_event_updated[2] = { (void *)&notify_event_vtable, (void *)&midi2_notify_updated };


    hr = dyn_WindowsCreateStringReference(name_MidiEndpointDeviceWatcher, (UINT32)wcslen(name_MidiEndpointDeviceWatcher), &hstring_header, &class_id);
    if (FAILED(hr)) return -1;

    // get watcher factory (interface to static functions)
    hr = dyn_RoGetActivationFactory(class_id, REF(IID_IMidiEndpointDeviceWatcherStatics), (void **)&factory);
    if (FAILED(hr)) return -2;

    // verify that factory implements IAgileObject interface
    hr = IMidiEndpointDeviceWatcherStatics_QueryInterface(factory, REF(IID_IAgileObject), (void **)&agile);
    if (FAILED(hr))
    {
        IMidiEndpointDeviceWatcherStatics_Release(factory);
        return -3;
    }
    IMidiEndpointDeviceWatcherStatics_Release(agile);

    // create watcher
    hr = IMidiEndpointDeviceWatcherStatics_Create(factory, &watcher);
    IMidiEndpointDeviceWatcherStatics_Release(factory);
    if (FAILED(hr)) return -4;

    // add event handlers to watcher
    hr = IMidiEndpointDeviceWatcher_add_Added(watcher, (ITypedEventHandler *)&notify_event_added, &midi2_notify_added_token);
    if (FAILED(hr))
    {
        IMidiEndpointDeviceWatcher_Release(watcher);
        return -5;
    }

    hr = IMidiEndpointDeviceWatcher_add_Removed(watcher, (ITypedEventHandler *)&notify_event_removed, &midi2_notify_removed_token);
    if (FAILED(hr))
    {
        IMidiEndpointDeviceWatcher_remove_Added(watcher, midi2_notify_added_token);
        IMidiEndpointDeviceWatcher_Release(watcher);
        return -6;
    }

    hr = IMidiEndpointDeviceWatcher_add_Updated(watcher, (ITypedEventHandler *)&notify_event_updated, &midi2_notify_updated_token);
    if (FAILED(hr))
    {
        IMidiEndpointDeviceWatcher_remove_Added(watcher, midi2_notify_added_token);
        IMidiEndpointDeviceWatcher_remove_Removed(watcher, midi2_notify_removed_token);
        IMidiEndpointDeviceWatcher_Release(watcher);
        return -7;
    }

    // start the watcher
    hr = IMidiEndpointDeviceWatcher_Start(watcher);
    if (FAILED(hr))
    {
        IMidiEndpointDeviceWatcher_remove_Added(watcher, midi2_notify_added_token);
        IMidiEndpointDeviceWatcher_remove_Removed(watcher, midi2_notify_removed_token);
        IMidiEndpointDeviceWatcher_remove_Updated(watcher, midi2_notify_updated_token);
        IMidiEndpointDeviceWatcher_Release(watcher);
        return -8;
    }

    midi2_watcher = watcher;
    return 0;
}


static int MIDI_PLUGIN2_API midi2_play(void const *midibuffer, long int size, int loop_count)
{
    unsigned int timediv;
    midi_event_info *dataptr;
    int chan;
    HRESULT hr;
    uint64_t timestamp_now;

    if (midibuffer == NULL) return -1;
    if (size <= 0) return -2;

    if (loop_count < -1) loop_count = -1;

    if (midi2_session == NULL) return -3;

    midi2_close_midi();

    midi_loop_count = loop_count;

    if (preprocessmidi((const uint8_t *)midibuffer, size, &timediv, &dataptr))
    {
        return -4;
    }

    EnterCriticalSection(&midi_critical_section);

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

    hr = IMidiClockStatics_get_Now(midi2_clock, &timestamp_now);
    if (SUCCEEDED(hr))
    {
        midi2_base_timestamp = timestamp_now;
    }
    midi_playing = 1;

    LeaveCriticalSection(&midi_critical_section);

    return 0;
}

static int MIDI_PLUGIN2_API midi2_pause(void)
{
    int chan, note, num;
    unsigned int word_count;
    HRESULT hr;
    uint64_t timestamp_now;

    if (midi2_session == NULL) return -1;
    if (!midi_loaded) return -2;

    if (midi_playing)
    {
        EnterCriticalSection(&midi_critical_section);

        word_count = 0;

        if (state_mt32_display == 2 && !mt32_delay)
        {
            state_mt32_display = 4;

            word_count = midi2_convert_sysex_to_ump(midi2_buf, sysex_mt32_display[0], sysex_mt32_display + 1, sizeof(sysex_mt32_display) - 2);
        }

        // stop playing notes on all channels
        for (chan = 0; chan < MIDI_CHANNELS; chan++)
        {
            for (note = 0; note < 128; note++)
            {
                if (channel_notes[chan][note] > 0)
                for (num = channel_notes[chan][note]; num != 0; num--)
                {
                    if (word_count == MIDI2_BUFFER_SIZE)
                    {
                        midi2_drain_midi_buffer(midi2_last_timestamp, word_count);
                        word_count = 0;
                    }

                    midi2_buf[word_count] = (MidiMessageType_Midi1ChannelVoice32 << 28) | (midi2_group << 24) | (MIDI_STATUS_NOTE_OFF << 20) | (chan << 16) | (note << 8) | (0);
                    word_count++;
                }
            }
        }

        midi2_drain_midi_buffer(midi2_last_timestamp, word_count);

        hr = IMidiClockStatics_get_Now(midi2_clock, &timestamp_now);
        if (SUCCEEDED(hr))
        {
            midi2_pause_timestamp = timestamp_now;
        }
        midi_playing = 0;

        LeaveCriticalSection(&midi_critical_section);
    }

    return 0;
}

static int MIDI_PLUGIN2_API midi2_resume(void)
{
    unsigned int word_count;
    HRESULT hr;
    uint64_t timestamp_now;

    if (midi2_session == NULL) return -1;
    if (!midi_loaded) return -2;

    if (!midi_playing)
    {
        EnterCriticalSection(&midi_critical_section);

        if (state_mt32_display == 4)
        {
            state_mt32_display = 2;

            word_count = midi2_convert_sysex_to_ump(midi2_buf, sysex_mt32_reset_display[0], sysex_mt32_reset_display + 1, sizeof(sysex_mt32_reset_display) - 2);

            midi2_drain_midi_buffer(midi2_last_timestamp, word_count);
        }

        hr = IMidiClockStatics_get_Now(midi2_clock, &timestamp_now);
        if (SUCCEEDED(hr))
        {
            midi2_base_timestamp += timestamp_now - midi2_pause_timestamp;
        }
        midi_playing = 1;

        LeaveCriticalSection(&midi_critical_section);
    }

    return 0;
}

static int MIDI_PLUGIN2_API midi2_halt(void)
{
    if (midi2_session == NULL) return -1;

    midi2_close_midi();

    return 0;
}

static int MIDI_PLUGIN2_API midi2_set_volume(unsigned char volume) // volume = 0 - 127
{
    if (volume > 127) volume = 127;

    if (midi2_session == NULL) return -1;

    midi_new_volume = volume;

    return 0;
}

static int MIDI_PLUGIN2_API midi2_set_loop_count(int loop_count) // -1 = unlimited
{
    if (loop_count < -1) loop_count = -1;

    if (midi2_session == NULL) return -1;
    if (!midi_loaded) return -2;

    midi_loop_count = loop_count;

    return 0;
}

static void midi2_shutdown_plugin_local(void)
{
    if (midi2_watcher != NULL)
    {
        IMidiEndpointDeviceWatcher_Stop(midi2_watcher);

        IMidiEndpointDeviceWatcher_remove_Added(midi2_watcher, midi2_notify_added_token);
        IMidiEndpointDeviceWatcher_remove_Removed(midi2_watcher, midi2_notify_removed_token);
        IMidiEndpointDeviceWatcher_remove_Updated(midi2_watcher, midi2_notify_updated_token);

        IMidiEndpointDeviceWatcher_Release(midi2_watcher);
        midi2_watcher = NULL;
    }

    if (midi2_session != NULL)
    {
        if (midi_loaded)
        {
            midi2_close_midi();
        }
        else
        {
            midi2_reset_playing();
        }

        if (midi_thread_handle != NULL)
        {
            midi_quit = 1;
            WaitForSingleObject(midi_thread_handle, INFINITE);
            CloseHandle(midi_thread_handle);
            midi_thread_handle = NULL;
            midi_quit = 0;
        }

        if (midi_type == 2)
        {
            mt32_shutdown_gm();
        }
        else if (midi_type)
        {
            if (state_mt32_display >= 4)
            {
                state_mt32_display = 2;
                midi2_send_initial_sysex_events(sysex_mt32_reset_display);
            }
        }

        if (midi2_endpoint_id != NULL)
        {
            dyn_WindowsDeleteString(midi2_endpoint_id);
            midi2_endpoint_id = NULL;
        }
        if (midi2_endpoint_connection != NULL)
        {
            midi2_disconnect_and_release_connection(midi2_endpoint_connection);
            midi2_endpoint_connection = NULL;
        }
        IMidiSession_Release(midi2_session);
        midi2_session = NULL;
    }

    if (midi2_clock != NULL)
    {
        IMidiClockStatics_Release(midi2_clock);
        midi2_clock = NULL;
    }

    if (midi2_initializer != NULL)
    {
        IMidiClientInitializer_Release(midi2_initializer);
        midi2_initializer = NULL;
    }

    dyn_RoUninitialize();

    notification_events = 0;

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

    FreeLibrary(hCombase);
}

static void MIDI_PLUGIN2_API midi2_shutdown_plugin(void)
{
    midi2_shutdown_plugin_local();
    shutdown_plugin_common();
}


static int midi2_initialize_midi_plugin2(midi_plugin2_parameters const *parameters, midi_plugin2_functions *functions)
{
    char const *device_name;
    unsigned char const *sysex_events, *controller_events;
    int events_len;
    HRESULT hr;

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

    dyn_add_midi_event = midi2_add_midi_event;
    dyn_send_initial_sysex_events = midi2_send_initial_sysex_events;

    midi2_load_windows_dlls();
    if (hCombase == NULL) return -4;

    hr = dyn_RoInitialize(RO_INIT_MULTITHREADED);
    if (hr != S_OK && hr != S_FALSE)
    {
        FreeLibrary(hCombase);
        return -5;
    }

    if (midi2_initialize_midi_sdk_and_service() < 0)
    {
        midi2_shutdown_plugin_local();
        return -6;
    }

    if (midi2_initialize_clock() < 0)
    {
        midi2_shutdown_plugin_local();
        return -7;
    }

    if (midi2_create_session(L"midi2-windows") < 0)
    {
        midi2_shutdown_plugin_local();
        return -8;
    }

    if (midi2_find_endpoint(device_name) < 0)
    {
        IMidiSession_Release(midi2_session);
        midi2_session = NULL;
        midi2_shutdown_plugin_local();
        return -9;
    }

    midi2_start_watcher();

    midi2_last_timestamp = 0;

    if (midi_type == 2)
    {
        mt32_initialize_gm();
    }
    else if (midi_type)
    {
        if (state_mt32_display == 1)
        {
            state_mt32_display = (mt32_delay) ? 4 : 5;
            midi2_send_initial_sysex_events(sysex_mt32_display);
        }
    }

    if (sysex_events != NULL && *sysex_events == 0xf0)
    {
        events_len = midi2_send_initial_sysex_events(sysex_events);

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

    midi2_reset_playing();

    midi_thread_handle = CreateThread(NULL, 65536, &midi2_MidiThreadProc, NULL, 0, NULL);
    if (midi_thread_handle == NULL)
    {
        if (midi_type == 2)
        {
            mt32_shutdown_gm();
        }
        midi2_disconnect_and_release_connection(midi2_endpoint_connection);
        midi2_endpoint_connection = NULL;
        IMidiSession_Release(midi2_session);
        midi2_session = NULL;
        midi2_shutdown_plugin_local();
        return -10;
    }

    functions->play = &midi2_play;
    functions->pause = &midi2_pause;
    functions->resume = &midi2_resume;
    functions->halt = &midi2_halt;
    functions->set_volume = &midi2_set_volume;
    functions->set_loop_count = &midi2_set_loop_count;
    functions->shutdown_plugin = &midi2_shutdown_plugin;

    return 0;
}

