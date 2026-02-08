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

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #ifndef WINVER
    #define WINVER 0x0602
    #endif
    #include <windows.h>
    #include <cfgmgr32.h>
    #include <mmddk.h>
    #include <limits.h>
    #include <stddef.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "midi-plugins2.h"

#ifdef _MSC_VER
    #define EXPORT __declspec(dllexport)
    #define strdup _strdup
#elif defined __GNUC__
    #define EXPORT __attribute__ ((visibility ("default")))
#else
    #define EXPORT
#endif

static int midi_type;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
// Multimedia Class Scheduler Service Functions
typedef HANDLE(WINAPI *AvSetMmThreadCharacteristicsWFunc) (LPCWSTR TaskName, LPDWORD TaskIndex);
typedef BOOL(WINAPI *AvRevertMmThreadCharacteristicsFunc) (HANDLE AvrtHandle);

// Configuration Manager Notification Functions
typedef CONFIGRET(WINAPI *CM_Register_Notification_Func) (PCM_NOTIFY_FILTER pFilter, PVOID pContext, PCM_NOTIFY_CALLBACK pCallback, PHCMNOTIFICATION pNotifyContext);
typedef CONFIGRET(WINAPI *CM_Unregister_Notification_Func) (HCMNOTIFICATION NotifyContext);

enum {
	NOTIFY_UNREGISTER = 1 << 0,
	NOTIFY_REREGISTER = 1 << 1,
	NOTIFY_NEWDEVICE  = 1 << 2
};

typedef struct {
	uint32_t tick;
	uint32_t event;
	uint8_t *sysex;
	uint32_t time;
} midi_event_info;

typedef struct {
	unsigned int time_division, last_tick, tempo, tempo_tick, num_allocated, num_events;
	uint64_t tempo_time, last_time;
	uint32_t next_sysex_time, event_time;
	midi_event_info *events;
} preprocess_state;

typedef struct _midi_queue_info_ {
	MIDIHDR header;
	struct _midi_queue_info_ *next;
	uint8_t buffer[65532];
} midi_queue_info;


static HMIDISTRM hStream = NULL;
static HANDLE midi_thread_handle = NULL;
static char *midi_stream_name = NULL;
static LPWSTR lpDeviceInterfaceName = NULL;

static HMODULE hAvrt = NULL;
static AvSetMmThreadCharacteristicsWFunc dyn_AvSetMmThreadCharacteristicsW;
static AvRevertMmThreadCharacteristicsFunc dyn_AvRevertMmThreadCharacteristics;

static HMODULE hCfgmgr32 = NULL;
static CM_Register_Notification_Func dyn_CM_Register_Notification;
static CM_Unregister_Notification_Func dyn_CM_Unregister_Notification;

static HCMNOTIFICATION hNotificationHandle = NULL;
static HCMNOTIFICATION hNotificationInstance = NULL;
static volatile unsigned int notification_events = 0;

static volatile midi_queue_info *midi_queue_wait = NULL;
static volatile midi_queue_info *midi_queue_send = NULL;

static volatile midi_event_info *midi_events = NULL;
static volatile unsigned int midi_current_event;
static volatile uint32_t midi_base_time;

static CRITICAL_SECTION midi_critical_section, notification_critical_section;


static void calculate_next_sysex_tick(preprocess_state *state);
static void calculate_event_time(preprocess_state *state);
static int add_midi_event(preprocess_state *state, uint8_t status, uint8_t byte1, uint8_t byte2, const uint8_t *data_ptr, unsigned int data_len);
static void free_midi_data(midi_event_info *data);
static int send_initial_sysex_events(unsigned char const *sysex_events);

#include "midi2-common.h"


static void calculate_next_sysex_tick(preprocess_state *state)
{
	state->last_tick = (unsigned int)( ((state->next_sysex_time * (uint64_t)1000000 - state->tempo_time) * state->time_division + (state->tempo * (uint64_t) 1000 - 1)) / (state->tempo * (uint64_t) 1000) ) + state->tempo_tick;
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

	// calculate event time in milliseconds
	state->event_time = (uint32_t)(state->last_time / 1000000);
}

static int add_midi_event(preprocess_state *state, uint8_t status, uint8_t byte1, uint8_t byte2, const uint8_t *data_ptr, unsigned int data_len)
{
	midi_event_info event;

	event.tick = state->last_tick;
	event.sysex = NULL;
	event.time = state->event_time;

	if ((status >> 4) != MIDI_STATUS_SYSEX)
	{
		event.event = MEVT_F_SHORT | (status) | (((uint32_t)byte1) << 8) | (((uint32_t)byte2) << 16);
	}
	else
	{
		if (status == 0xff) // meta events
		{
			if ((byte1 == 0x51) && (data_len == 3)) // set tempo
			{
				// time_division is assumed to be positive (ticks per beat / PPQN - Pulses (i.e. clocks) Per Quarter Note)

				event.event = (((uint32_t)MEVT_TEMPO) << 24) | (((uint32_t)(data_ptr[0])) << 16) | (((uint32_t)(data_ptr[1])) << 8) | ((uint32_t)(data_ptr[2]));

				state->tempo = MEVT_EVENTPARM(event.event);
				state->tempo_tick = state->last_tick;
				state->tempo_time = state->last_time;
			}
			else return 0;
		}
		else if ((status == 0xf0) || (status == 0xf7)) // sysex
		{
			event.event = data_len + ((status == 0xf0)?1:0);
			if (event.event == 0) return 0;
			if ((event.event > 0xffffff) || (offsetof(MIDIEVENT, dwParms) + ((event.event + 3) & ~3) > sizeof(midi_queue_send->buffer))) return 0;

			event.sysex = (uint8_t *) malloc(event.event);
			if (event.sysex == NULL) return 12;

			event.event |= MEVT_F_LONG;

			if (status == 0xf0)
			{
				event.sysex[0] = 0xf0;
				memcpy(event.sysex + 1, data_ptr, data_len);
			}
			else
			{
				memcpy(event.sysex, data_ptr, data_len);
			}

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

				state->next_sysex_time = event.time + (40 + 10 + ((MEVT_EVENTPARM(event.event) * 10000 + 31249) / 31250));
			}
		}
		else if ((status == 0xf4) && mt32_delay) // extra event
		{
			event.event = MEVT_NOP << 24;
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
	state->events[0].event = state->num_events;

	return 0;
}

static void free_midi_data(midi_event_info *data)
{
	unsigned int index;

	if (data != NULL)
	{
		for (index = data[0].event; index != 0; index--)
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


static int send_initial_sysex_events(unsigned char const *sysex_events);
static void reset_playing(void);
static void get_device_interface_name(UINT uDeviceID);
static void register_notifications(void);
static midi_queue_info *allocate_queue_info(void);

static DWORD WINAPI MidiThreadProc(LPVOID lpParameter)
{
	midi_event_info *events;
	unsigned int current_event, num_events, current_notification_events, insert_events;
	uint32_t base_time, stream_time;
	int do_sleep, midi_stream_exists, chan;
	midi_queue_info *current_queue_wait, *current_queue_send, *qinfo;
	HANDLE AvrtHandle;
	DWORD TaskIndex, BaseTicks;
	MMTIME mmtime;
	MIDIEVENT *event;

	BaseTicks = GetTickCount();

	AvrtHandle = NULL;
	TaskIndex = 0;
	if (hAvrt != NULL)
	{
		AvrtHandle = dyn_AvSetMmThreadCharacteristicsW(L"Pro Audio", &TaskIndex);
	}

	do_sleep = 1;
	midi_stream_exists = 1;

	while (1)
	{
		if (do_sleep)
		{
			do_sleep = 0;
			Sleep(10);
		}

		if (midi_quit)
		{
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

			if ((current_notification_events & NOTIFY_UNREGISTER) && midi_stream_exists)
			{
				midi_stream_exists = 0;
				if (hNotificationHandle != NULL)
				{
					dyn_CM_Unregister_Notification(hNotificationHandle);
					hNotificationHandle = NULL;
				}
				if (midi_loaded)
				{
					EnterCriticalSection(&midi_critical_section);
					if (midi_loaded && (midi_loop_count == 0))
					{
						midi_eof = 1;
					}
					LeaveCriticalSection(&midi_critical_section);
				}
			}
			else if ((current_notification_events & NOTIFY_REREGISTER) && midi_stream_exists)
			{
				if (hNotificationHandle != NULL)
				{
					dyn_CM_Unregister_Notification(hNotificationHandle);
					hNotificationHandle = NULL;
				}
				register_notifications();
			}

			if ((current_notification_events & NOTIFY_NEWDEVICE) && !midi_stream_exists)
			{
				int numDevices, devid;
				MIDIOUTCAPS midicaps;
				UINT uDeviceID;
				MIDIPROPTEMPO miditempo;
				MIDIPROPTIMEDIV miditimediv;

				numDevices = midiOutGetNumDevs();

				uDeviceID = numDevices;
				for (devid = 0; devid < numDevices; devid++)
				{
					if (MMSYSERR_NOERROR != midiOutGetDevCaps(devid, &midicaps, sizeof(midicaps))) continue;

					if (0 == strcmp(midi_stream_name, midicaps.szPname))
					{
						uDeviceID = devid;
						break;
					}
				}

				if (uDeviceID != numDevices)
				{
					int was_playing;

					midi_stream_exists = 1;
					EnterCriticalSection(&midi_critical_section);

					was_playing = (midi_loaded && midi_playing);

					current_queue_wait = (midi_queue_info *) midi_queue_wait;
					current_queue_send = (midi_queue_info *) midi_queue_send;
					while (current_queue_wait != current_queue_send)
					{
						if (current_queue_wait->header.dwFlags & MHDR_DONE)
						{
							current_queue_wait->header.dwFlags &= ~MHDR_DONE;
							current_queue_wait = current_queue_wait->next;
						}
						else
						{
							Sleep(1);
						}
					}
					midi_queue_wait = current_queue_wait;

					if (midi_loaded)
					{
						miditimediv.cbStruct = sizeof(miditimediv);
						midiStreamProperty(hStream, (LPBYTE)&miditimediv, MIDIPROP_GET | MIDIPROP_TIMEDIV);

						miditempo.cbStruct = sizeof(miditimediv);
						midiStreamProperty(hStream, (LPBYTE)&miditempo, MIDIPROP_GET | MIDIPROP_TEMPO);
					}

					midiStreamClose(hStream);
					hStream = NULL;
					midi_playing = 0;

					if (lpDeviceInterfaceName != NULL)
					{
						free(lpDeviceInterfaceName);
						lpDeviceInterfaceName = NULL;
					}

					if (MMSYSERR_NOERROR == midiStreamOpen(&hStream, &uDeviceID, 1, 0, 0, CALLBACK_NULL))
					{
						get_device_interface_name(uDeviceID);
						register_notifications();

						if (midi_type == 2)
						{
							mt32_initialize_gm();
						}

						if (initial_sysex_events != NULL && *initial_sysex_events == 0xf0)
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

							midiStreamProperty(hStream, (LPBYTE)&miditimediv, MIDIPROP_SET | MIDIPROP_TIMEDIV);

							midiStreamProperty(hStream, (LPBYTE)&miditempo, MIDIPROP_SET | MIDIPROP_TEMPO);

							for (chan = 0; chan < MIDI_CHANNELS; chan++)
							{
								channel_volume[chan] = 100;
							}

							// set current volume to a different value than new volume in order to set the channel volume at start of playing
							midi_current_volume = midi_new_volume ^ 1;

							memset(channel_notes, 0, 128*MIDI_CHANNELS*sizeof(int));

							midi_current_event = 1;
						}

						if (was_playing)
						{
							if (MMSYSERR_NOERROR == midiStreamRestart(hStream))
							{
								midi_playing = 1;
							}
						}

						mmtime.wType = TIME_MS;
						if (MMSYSERR_NOERROR != midiStreamPosition(hStream, &mmtime, sizeof(MMTIME)))
						{
							if (midi_playing)
							{
								if (MMSYSERR_NOERROR == midiStreamPause(hStream))
								{
									midi_playing = 0;
								}
							}
							if (midi_loaded)
							{
								free_midi_data((midi_event_info *)midi_events);
								midi_loaded = 0;
							}
						}

						midi_base_time = mmtime.u.ms;
					}
					else
					{
						hStream = NULL;
						if (midi_loaded)
						{
							free_midi_data((midi_event_info *)midi_events);
							midi_loaded = 0;
						}
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

		events = (midi_event_info *) midi_events;
		current_event = midi_current_event;
		base_time = midi_base_time;

		num_events = events[0].event;

		current_queue_wait = (midi_queue_info *) midi_queue_wait;
		if (current_queue_wait->header.dwFlags & MHDR_DONE)
		{
			do
			{
				current_queue_wait->header.dwFlags &= ~MHDR_DONE;
				current_queue_wait = current_queue_wait->next;
			} while (current_queue_wait->header.dwFlags & MHDR_DONE);
			midi_queue_wait = current_queue_wait;
		}

		if (current_event > num_events)
		{
			if (current_queue_wait != midi_queue_send)
			{
				LeaveCriticalSection(&midi_critical_section);
				do_sleep = 1;
				continue;
			}

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
			base_time += events[num_events].time;

			current_event = 1;

			midi_current_event = current_event;
			midi_base_time = base_time;
		}

		insert_events = 0;
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

		if (insert_events)
		{
			current_queue_send = (midi_queue_info *) midi_queue_send;
			if (current_queue_send->next == current_queue_wait)
			{
				qinfo = allocate_queue_info();
				if (qinfo == NULL)
				{
					LeaveCriticalSection(&midi_critical_section);
					do_sleep = 1;
					continue;
				}

				qinfo->next = current_queue_wait;
				current_queue_send->next = qinfo;
			}

			current_queue_send->header.dwBytesRecorded = 0;

			if (insert_events & 1)
			{
				for (chan = 0; chan < MIDI_CHANNELS; chan++)
				{
					event = (MIDIEVENT *) &(current_queue_send->buffer[current_queue_send->header.dwBytesRecorded]);

					event->dwDeltaTime = 0;
					event->dwStreamID = 0;
					event->dwEvent = MEVT_F_SHORT | 0xb0 | chan | (7 << 8) | (((midi_current_volume * channel_volume[chan]) / 127) << 16);

					current_queue_send->header.dwBytesRecorded += offsetof(MIDIEVENT, dwParms);
				}
			}
			if (insert_events & 2)
			{
				event = (MIDIEVENT *) &(current_queue_send->buffer[current_queue_send->header.dwBytesRecorded]);

				event->dwDeltaTime = 0;
				event->dwStreamID = 0;
				event->dwEvent = MEVT_F_LONG | 11;

				memcpy(event->dwParms, sysex_mt32_reset_display, 11);
				current_queue_send->header.dwBytesRecorded += offsetof(MIDIEVENT, dwParms) + ((11 + 3) & ~3);
			}

			if (MMSYSERR_NOERROR == midiStreamOut(hStream, &(current_queue_send->header), sizeof(MIDIHDR)))
			{
				midi_queue_send = current_queue_send->next;
			}
		}

		mmtime.wType = TIME_MS;
		if (MMSYSERR_NOERROR != midiStreamPosition(hStream, &mmtime, sizeof(MMTIME)))
		{
			LeaveCriticalSection(&midi_critical_section);
			do_sleep = 1;
			continue;
		}

		stream_time = mmtime.u.ms - base_time;

		if ((current_event > num_events) || (events[current_event].time > stream_time + 90))
		{
			LeaveCriticalSection(&midi_critical_section);
			do_sleep = 1;
			continue;
		}

		current_queue_send = (midi_queue_info *) midi_queue_send;
		if (current_queue_send->next == current_queue_wait)
		{
			qinfo = allocate_queue_info();
			if (qinfo == NULL)
			{
				LeaveCriticalSection(&midi_critical_section);
				do_sleep = 1;
				continue;
			}

			qinfo->next = current_queue_wait;
			current_queue_send->next = qinfo;
		}

		current_queue_send->header.dwBytesRecorded = 0;

		while ((current_event <= num_events) && (events[current_event].time <= stream_time + 190))
		{
			if (current_queue_send->header.dwBytesRecorded + offsetof(MIDIEVENT, dwParms) > sizeof(current_queue_send->buffer))
			{
				break;
			}

			event = (MIDIEVENT *) &(current_queue_send->buffer[current_queue_send->header.dwBytesRecorded]);
			event->dwDeltaTime = events[current_event].tick - events[current_event - 1].tick;
			event->dwStreamID = 0;
			event->dwEvent = events[current_event].event;

			if (events[current_event].sysex != NULL)
			{
				if (current_queue_send->header.dwBytesRecorded + offsetof(MIDIEVENT, dwParms) + ((MEVT_EVENTPARM(event->dwEvent) + 3) & ~3) > sizeof(current_queue_send->buffer))
				{
					break;
				}

				memcpy(event->dwParms, events[current_event].sysex, MEVT_EVENTPARM(event->dwEvent));
				current_queue_send->header.dwBytesRecorded += offsetof(MIDIEVENT, dwParms) + ((MEVT_EVENTPARM(event->dwEvent) + 3) & ~3);
			}
			else
			{
				current_queue_send->header.dwBytesRecorded += offsetof(MIDIEVENT, dwParms);

				switch ((event->dwEvent >> 4) & 0x0f)
				{
					case MIDI_STATUS_NOTE_ON:
						if (((event->dwEvent >> 16) & 0xff) != 0)
						{
							// note on
							channel_notes[event->dwEvent & 0x0f][(event->dwEvent >> 8) & 0x7f]++;
							break;
						}
						// fallthrough
					case MIDI_STATUS_NOTE_OFF:
						// note off
						if ((channel_notes[event->dwEvent & 0x0f][(event->dwEvent >> 8) & 0x7f]) > 0)
						{
							channel_notes[event->dwEvent & 0x0f][(event->dwEvent >> 8) & 0x7f]--;
						}
						break;
					case MIDI_STATUS_CONTROLLER:
						if (((event->dwEvent >> 8) & 0xff) == 7)
						{
							// volume
							channel_volume[event->dwEvent & 0x0f] = (event->dwEvent >> 16) & 0x7f;
							event->dwEvent = (event->dwEvent & 0xff00ffff) | (((midi_current_volume * channel_volume[event->dwEvent & 0x0f]) / 127) << 16);
						}
						break;
					default:
						break;
				}
			}

			current_event++;
		}

		midi_current_event = current_event;

		if (current_queue_send->header.dwBytesRecorded)
		{
			if (MMSYSERR_NOERROR == midiStreamOut(hStream, &(current_queue_send->header), sizeof(MIDIHDR)))
			{
				midi_queue_send = current_queue_send->next;
			}
		}

		LeaveCriticalSection(&midi_critical_section);
	}
}


static int send_initial_sysex_events(unsigned char const *sysex_events)
{
	int events_len, len, delay_len, min_delay;
	uint32_t next_sysex_delta;
	midi_queue_info *current_queue_wait, *current_queue_send;
	MIDIEVENT *event;
	MMTIME mmtime;

	if (hStream == NULL) return 0;

	events_len = 0;

	EnterCriticalSection(&midi_critical_section);

	if (!midi_playing)
	{
		if (MMSYSERR_NOERROR == midiStreamRestart(hStream))
		{
			midi_playing = 1;
		}

		mmtime.wType = TIME_MS;
		if (MMSYSERR_NOERROR != midiStreamPosition(hStream, &mmtime, sizeof(MMTIME)))
		{
			if (MMSYSERR_NOERROR == midiStreamPause(hStream))
			{
				midi_playing = 0;
			}
			LeaveCriticalSection(&midi_critical_section);
			return 0;
		}

		midi_base_time = mmtime.u.ms;
	}

	if (midi_playing)
	{
		current_queue_wait = (midi_queue_info *) midi_queue_wait;
		current_queue_send = (midi_queue_info *) midi_queue_send;
		while (current_queue_wait != current_queue_send)
		{
			if (current_queue_wait->header.dwFlags & MHDR_DONE)
			{
				current_queue_wait->header.dwFlags &= ~MHDR_DONE;
				current_queue_wait = current_queue_wait->next;
			}
			else
			{
				Sleep(1);
			}
		}

		current_queue_send->header.dwBytesRecorded = 0;
		next_sysex_delta = 0;

		if (mt32_delay)
		{
			MIDIPROPTIMEDIV miditimediv;

			miditimediv.cbStruct = sizeof(miditimediv);
			if (MMSYSERR_NOERROR != midiStreamProperty(hStream, (LPBYTE)&miditimediv, MIDIPROP_GET | MIDIPROP_TIMEDIV))
			{
				if (MMSYSERR_NOERROR == midiStreamPause(hStream))
				{
					midi_playing = 0;
				}
				LeaveCriticalSection(&midi_critical_section);
				return 0;
			}

			mmtime.wType = TIME_MS;
			if (MMSYSERR_NOERROR != midiStreamPosition(hStream, &mmtime, sizeof(MMTIME)))
			{
				if (MMSYSERR_NOERROR == midiStreamPause(hStream))
				{
					midi_playing = 0;
				}
				LeaveCriticalSection(&midi_critical_section);
				return 0;
			}

			next_sysex_delta = (mmtime.u.ms - midi_base_time) + 40 + 10 + 10;

			event = (MIDIEVENT *) &(current_queue_send->buffer[current_queue_send->header.dwBytesRecorded]);
			event->dwDeltaTime = 0;
			event->dwStreamID = 0;
			event->dwEvent = (MEVT_TEMPO << 24) | (miditimediv.dwTimeDiv * 1000);

			current_queue_send->header.dwBytesRecorded += offsetof(MIDIEVENT, dwParms);
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

			if ((len > 0xffffff) || (offsetof(MIDIEVENT, dwParms) + ((len + 3) & ~3) > sizeof(current_queue_send->buffer)))
			{
				// too long sysex - skip it
				sysex_events += len + delay_len;
				events_len += len + delay_len;
				continue;
			}

			if (current_queue_send->header.dwBytesRecorded + offsetof(MIDIEVENT, dwParms) + ((len + 3) & ~3) > sizeof(current_queue_send->buffer))
			{
				if (MMSYSERR_NOERROR == midiStreamOut(hStream, &(current_queue_send->header), sizeof(MIDIHDR)))
				{
					current_queue_send = current_queue_send->next;
				}

				current_queue_send->header.dwBytesRecorded = 0;

				while (current_queue_wait != current_queue_send)
				{
					if (current_queue_wait->header.dwFlags & MHDR_DONE)
					{
						current_queue_wait->header.dwFlags &= ~MHDR_DONE;
						current_queue_wait = current_queue_wait->next;
					}
					else
					{
						Sleep(1);
					}
				}
			}

			event = (MIDIEVENT *) &(current_queue_send->buffer[current_queue_send->header.dwBytesRecorded]);
			event->dwDeltaTime = next_sysex_delta;
			event->dwStreamID = 0;
			event->dwEvent = MEVT_F_LONG | len;

			memcpy(event->dwParms, sysex_events, len);
			current_queue_send->header.dwBytesRecorded += offsetof(MIDIEVENT, dwParms) + ((len + 3) & ~3);

			sysex_events += len + delay_len;
			events_len += len + delay_len;

			if (mt32_delay)
			{
				if (min_delay < 40) min_delay = 40;
				next_sysex_delta = min_delay + 10 + ((len * 10000 + 31249) / 31250);
			}
		};

		events_len++;

		if (mt32_delay)
		{
			if (current_queue_send->header.dwBytesRecorded + offsetof(MIDIEVENT, dwParms) > sizeof(current_queue_send->buffer))
			{
				if (MMSYSERR_NOERROR == midiStreamOut(hStream, &(current_queue_send->header), sizeof(MIDIHDR)))
				{
					current_queue_send = current_queue_send->next;
				}

				current_queue_send->header.dwBytesRecorded = 0;

				while (current_queue_wait != current_queue_send)
				{
					if (current_queue_wait->header.dwFlags & MHDR_DONE)
					{
						current_queue_wait->header.dwFlags &= ~MHDR_DONE;
						current_queue_wait = current_queue_wait->next;
					}
					else
					{
						Sleep(1);
					}
				}
			}

			event = (MIDIEVENT *) &(current_queue_send->buffer[current_queue_send->header.dwBytesRecorded]);
			event->dwDeltaTime = next_sysex_delta;
			event->dwStreamID = 0;
			event->dwEvent = MEVT_NOP << 24;

			current_queue_send->header.dwBytesRecorded += offsetof(MIDIEVENT, dwParms);
		}

		if (current_queue_send->header.dwBytesRecorded)
		{
			if (MMSYSERR_NOERROR == midiStreamOut(hStream, &(current_queue_send->header), sizeof(MIDIHDR)))
			{
				current_queue_send = current_queue_send->next;
			}
		}

		while (current_queue_wait != current_queue_send)
		{
			if (current_queue_wait->header.dwFlags & MHDR_DONE)
			{
				current_queue_wait->header.dwFlags &= ~MHDR_DONE;
				current_queue_wait = current_queue_wait->next;
			}
			else
			{
				Sleep(1);
			}
		}

		midi_queue_wait = current_queue_wait;
		midi_queue_send = current_queue_send;

		if (MMSYSERR_NOERROR == midiStreamPause(hStream))
		{
			midi_playing = 0;
		}
	}

	LeaveCriticalSection(&midi_critical_section);

	return events_len;
}


static void reset_playing(void)
{
	int chan;

	if (hStream == NULL) return;

	for (chan = 0xb0; chan <= 0xbf; chan++)
	{
		// MT-32 doesn't support All sounds off, so Omni off is used instead
		midiOutShortMsg((HMIDIOUT)hStream, chan | ((midi_type ? 0x7c : 0x78) << 8) | (0x00 << 16)); // Omni off / All sounds off (abrupt stop of sound on channel)
		midiOutShortMsg((HMIDIOUT)hStream, chan | (0x79 << 8) | (0x00 << 16)); // All controllers off (this message clears all the controller values for this channel, back to their default values)
		midiOutShortMsg((HMIDIOUT)hStream, chan | (0x7b << 8) | (0x00 << 16)); // All notes off (this message stops all the notes that are currently playing)
		// All controllers off doesn't set volume and pan to default values
		// Volume is set at start of playing, so only pan is set to default value
		midiOutShortMsg((HMIDIOUT)hStream, chan | (0x0a << 8) | (0x40 << 16)); // Pan

		if (reset_controller_events != NULL)
		{
			unsigned char *controller_events;

			controller_events = reset_controller_events;
			while (*controller_events != 0xff)
			{
				midiOutShortMsg((HMIDIOUT)hStream, controller_events[0] | (controller_events[1] << 8));
				controller_events += 2;
			}
		}
	}

	midiOutShortMsg((HMIDIOUT)hStream, 0xc9 | (0x00 << 8));

	for (chan = 0xe0; chan <= 0xef; chan++)
	{
		midiOutShortMsg((HMIDIOUT)hStream, chan | (0x00 << 8) | (0x40 << 16));
	}
}


static void close_midi(void)
{
	if (!midi_loaded) return;

	EnterCriticalSection(&midi_critical_section);

	if (midi_playing)
	{
		midi_queue_info *current_queue_wait, *current_queue_send;

		current_queue_wait = (midi_queue_info *) midi_queue_wait;
		current_queue_send = (midi_queue_info *) midi_queue_send;
		while (current_queue_wait != current_queue_send)
		{
			if (current_queue_wait->header.dwFlags & MHDR_DONE)
			{
				current_queue_wait->header.dwFlags &= ~MHDR_DONE;
				current_queue_wait = current_queue_wait->next;
			}
			else
			{
				Sleep(1);
			}
		}
		midi_queue_wait = current_queue_wait;

		if (MMSYSERR_NOERROR == midiStreamPause(hStream))
		{
			midi_playing = 0;
		}
	}

	free_midi_data((midi_event_info *)midi_events);
	midi_loaded = 0;

	LeaveCriticalSection(&midi_critical_section);

	reset_playing();
}


static void load_windows_dlls(void)
{
	hAvrt = LoadLibraryW(L"avrt.dll");
	if (hAvrt != NULL)
	{
		dyn_AvSetMmThreadCharacteristicsW = (AvSetMmThreadCharacteristicsWFunc)GetProcAddress(hAvrt, "AvSetMmThreadCharacteristicsW");
		dyn_AvRevertMmThreadCharacteristics = (AvRevertMmThreadCharacteristicsFunc)GetProcAddress(hAvrt, "AvRevertMmThreadCharacteristics");
		if ((dyn_AvSetMmThreadCharacteristicsW == NULL) || (dyn_AvRevertMmThreadCharacteristics == NULL))
		{
			FreeLibrary(hAvrt);
			hAvrt = NULL;
		}
	}

	hCfgmgr32 = LoadLibraryW(L"cfgmgr32.dll");
	if (hCfgmgr32 != NULL)
	{
		dyn_CM_Register_Notification = (CM_Register_Notification_Func)GetProcAddress(hCfgmgr32, "CM_Register_Notification");
		dyn_CM_Unregister_Notification = (CM_Unregister_Notification_Func)GetProcAddress(hCfgmgr32, "CM_Unregister_Notification");
		if ((dyn_CM_Register_Notification == NULL) || (dyn_CM_Unregister_Notification == NULL))
		{
			FreeLibrary(hCfgmgr32);
			hCfgmgr32 = NULL;
		}
	}
}


static void get_device_interface_name(UINT uDeviceID)
{
	ULONG uSize;

	if ((uDeviceID < 0) || (midi_stream_name == NULL))
	{
		return;
	}

	if (MMSYSERR_NOERROR != midiOutMessage((HMIDIOUT)(UINT_PTR)uDeviceID, DRV_QUERYDEVICEINTERFACESIZE, (DWORD_PTR)&uSize, 0))
	{
		return;
	}

	if ((uSize <= 2) || (uSize & 1))
	{
		return;
	}

	lpDeviceInterfaceName = (LPWSTR)malloc(uSize);
	if (lpDeviceInterfaceName == NULL)
	{
		return;
	}

	if (MMSYSERR_NOERROR != midiOutMessage((HMIDIOUT)(UINT_PTR)uDeviceID, DRV_QUERYDEVICEINTERFACE, (DWORD_PTR)lpDeviceInterfaceName, uSize))
	{
		free(lpDeviceInterfaceName);
		lpDeviceInterfaceName = NULL;
		return;
	}
}

static DWORD WINAPI notify_callback(HCMNOTIFICATION hNotify, PVOID Context, CM_NOTIFY_ACTION Action, PCM_NOTIFY_EVENT_DATA EventData, DWORD EventDataSize)
{
	switch(Action)
	{
		// CM_NOTIFY_FILTER_TYPE_DEVICEHANDLE actions

		case CM_NOTIFY_ACTION_DEVICEQUERYREMOVE:
		case CM_NOTIFY_ACTION_DEVICECUSTOMEVENT:
			if (hNotificationHandle == NULL) hNotificationHandle = hNotify;
			break;
		case CM_NOTIFY_ACTION_DEVICEQUERYREMOVEFAILED:
			if (hNotificationHandle == NULL) hNotificationHandle = hNotify;
			EnterCriticalSection(&notification_critical_section);
			notification_events |= NOTIFY_REREGISTER;
			LeaveCriticalSection(&notification_critical_section);
			break;
		case CM_NOTIFY_ACTION_DEVICEREMOVEPENDING:
		case CM_NOTIFY_ACTION_DEVICEREMOVECOMPLETE:
			if (hNotificationHandle == NULL) hNotificationHandle = hNotify;
			EnterCriticalSection(&notification_critical_section);
			notification_events |= NOTIFY_UNREGISTER;
			LeaveCriticalSection(&notification_critical_section);
			break;

		// CM_NOTIFY_FILTER_TYPE_DEVICEINSTANCE actions

		case CM_NOTIFY_ACTION_DEVICEINSTANCEENUMERATED:
		case CM_NOTIFY_ACTION_DEVICEINSTANCEREMOVED:
			break;
		case CM_NOTIFY_ACTION_DEVICEINSTANCESTARTED:
			if (0 == memcmp(EventData->u.DeviceInstance.InstanceId, L"SWD\\MMDEVAPI\\", 2 * 13))
			{
				EnterCriticalSection(&notification_critical_section);
				notification_events |= NOTIFY_NEWDEVICE;
				LeaveCriticalSection(&notification_critical_section);
			}
			break;

		default:
			break;
	}

	return ERROR_SUCCESS;
}

static void register_notifications(void)
{
	HANDLE hDeviceInterface;
	CM_NOTIFY_FILTER cmFilter;

	if ((hCfgmgr32 == NULL) || (lpDeviceInterfaceName == NULL))
	{
		return;
	}

	hDeviceInterface = CreateFileW(lpDeviceInterfaceName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDeviceInterface == INVALID_HANDLE_VALUE)
	{
		return;
	}

	cmFilter.cbSize = sizeof(cmFilter);
	cmFilter.Flags = 0;
	cmFilter.FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEHANDLE;
	cmFilter.Reserved = 0;
	cmFilter.u.DeviceHandle.hTarget = hDeviceInterface;
	if (CR_SUCCESS != dyn_CM_Register_Notification(&cmFilter, NULL, &notify_callback, &hNotificationHandle))
	{
		hNotificationHandle = NULL;
		CloseHandle(hDeviceInterface);
		return;
	}

	CloseHandle(hDeviceInterface);

	if (hNotificationInstance == NULL)
	{
		cmFilter.FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEINSTANCE;
		cmFilter.Flags = CM_NOTIFY_FILTER_FLAG_ALL_DEVICE_INSTANCES;
		cmFilter.u.DeviceInstance.InstanceId[0] = 0;

		if (CR_SUCCESS != dyn_CM_Register_Notification(&cmFilter, NULL, &notify_callback, &hNotificationInstance))
		{
			hNotificationInstance = NULL;
			dyn_CM_Unregister_Notification(hNotificationHandle);
			hNotificationHandle = NULL;
			return;
		}
	}
}

static midi_queue_info *allocate_queue_info(void)
{
	midi_queue_info *qinfo;

	qinfo = (midi_queue_info *)malloc(sizeof(midi_queue_info));
	if (qinfo == NULL)
	{
		return NULL;
	}

	qinfo->header.lpData = (LPSTR)qinfo->buffer;
	qinfo->header.dwBufferLength = sizeof(qinfo->buffer);
	qinfo->header.dwBytesRecorded = 0;
	qinfo->header.dwFlags = 0;

	if (MMSYSERR_NOERROR != midiOutPrepareHeader((HMIDIOUT)hStream, &qinfo->header, sizeof(MIDIHDR)))
	{
		free(qinfo);
		return NULL;
	}

	return qinfo;
}

static int initialize_queue(void)
{
	midi_queue_info *qinfo;

	qinfo = allocate_queue_info();
	if (qinfo == NULL)
	{
		return 0;
	}

	qinfo->next = qinfo;
	midi_queue_wait = midi_queue_send = qinfo;

	qinfo = allocate_queue_info();
	if (qinfo == NULL)
	{
		return 0;
	}

	qinfo->next = (midi_queue_info *) midi_queue_wait;
	midi_queue_wait->next = qinfo;

	qinfo = allocate_queue_info();
	if (qinfo == NULL)
	{
		return 0;
	}

	qinfo->next = (midi_queue_info *) midi_queue_wait;
	midi_queue_wait->next->next = qinfo;

	return 1;
}

static void deinitialize_queue(void)
{
	midi_queue_info *first, *qinfo, *next;

	first = (midi_queue_info *) midi_queue_wait;
	if (first == NULL) return;

	qinfo = first->next;
	while (qinfo != first)
	{
		midiOutUnprepareHeader((HMIDIOUT)hStream, &qinfo->header, sizeof(MIDIHDR));
		next = qinfo->next;
		free(qinfo);
		qinfo = next;
	}

	midiOutUnprepareHeader((HMIDIOUT)hStream, &first->header, sizeof(MIDIHDR));
	free(first);

	midi_queue_wait = midi_queue_send = NULL;
}

#endif


static int MIDI_PLUGIN2_API play(void const *midibuffer, long int size, int loop_count)
{
    if (midibuffer == NULL) return -1;
    if (size <= 0) return -2;

    if (loop_count < -1) loop_count = -1;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -3;

	close_midi();

	midi_loop_count = loop_count;

	{
		unsigned int timediv;
		midi_event_info *dataptr;
		MIDIPROPTEMPO miditempo;
		MIDIPROPTIMEDIV miditimediv;
		MMTIME mmtime;
		int chan;

		if (preprocessmidi(midibuffer, size, &timediv, &dataptr))
		{
			return -4;
		}

		EnterCriticalSection(&midi_critical_section);

		for (chan = 0; chan < MIDI_CHANNELS; chan++)
		{
			channel_volume[chan] = 100;
		}

		// set current volume to a different value than new volume in order to set the channel volume at start of playing
		midi_current_volume = midi_new_volume ^ 1;

		miditimediv.cbStruct = sizeof(miditimediv);
		miditimediv.dwTimeDiv = timediv;
		if (MMSYSERR_NOERROR != midiStreamProperty(hStream, (LPBYTE)&miditimediv, MIDIPROP_SET | MIDIPROP_TIMEDIV))
		{
			free_midi_data(dataptr);
			LeaveCriticalSection(&midi_critical_section);
			return -5;
		}

		miditempo.cbStruct = sizeof(miditimediv);
		miditempo.dwTempo = 500000; // 120 BPM
		if (MMSYSERR_NOERROR != midiStreamProperty(hStream, (LPBYTE)&miditempo, MIDIPROP_SET | MIDIPROP_TEMPO))
		{
			free_midi_data(dataptr);
			LeaveCriticalSection(&midi_critical_section);
			return -6;
		}

		if (!midi_playing)
		{
			if (MMSYSERR_NOERROR == midiStreamRestart(hStream))
			{
				midi_playing = 1;
			}
		}

		if (!midi_playing)
		{
			free_midi_data(dataptr);
			LeaveCriticalSection(&midi_critical_section);
			return -7;
		}

		mmtime.wType = TIME_MS;
		if (MMSYSERR_NOERROR != midiStreamPosition(hStream, &mmtime, sizeof(MMTIME)))
		{
			if (MMSYSERR_NOERROR == midiStreamPause(hStream))
			{
				midi_playing = 0;
			}
			free_midi_data(dataptr);
			LeaveCriticalSection(&midi_critical_section);
			return -8;
		}

		midi_base_time = mmtime.u.ms;

		midi_events = dataptr;
		midi_current_event = 1;

		midi_loaded = 1;
		midi_eof = 0;

		memset(channel_notes, 0, 128*MIDI_CHANNELS*sizeof(int));

		LeaveCriticalSection(&midi_critical_section);
	}
#endif

    return 0;
}

static int MIDI_PLUGIN2_API pause(void)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -1;
	if (!midi_loaded) return -2;

	if (midi_playing)
	{
		midi_queue_info *current_queue_wait, *current_queue_send;
		int chan, note, num;

		EnterCriticalSection(&midi_critical_section);

		current_queue_wait = (midi_queue_info *) midi_queue_wait;
		current_queue_send = (midi_queue_info *) midi_queue_send;
		while (current_queue_wait != current_queue_send)
		{
			if (current_queue_wait->header.dwFlags & MHDR_DONE)
			{
				current_queue_wait->header.dwFlags &= ~MHDR_DONE;
				current_queue_wait = current_queue_wait->next;
			}
			else
			{
				Sleep(1);
			}
		}
		midi_queue_wait = current_queue_wait;

		if (state_mt32_display == 2 && !mt32_delay)
		{
			state_mt32_display = 4;
			send_initial_sysex_events(sysex_mt32_display);
		}
		else
		{
			if (MMSYSERR_NOERROR == midiStreamPause(hStream))
			{
				midi_playing = 0;
			}
		}

		if (!midi_playing)
		{
			// stop playing notes on all channels
			for (chan = 0; chan < MIDI_CHANNELS; chan++)
			{
				for (note = 0; note < 128; note++)
				{
					if (channel_notes[chan][note] > 0)
					for (num = channel_notes[chan][note]; num != 0; num--)
					{
						midiOutShortMsg((HMIDIOUT)hStream, 0x80 | chan | (note << 8) | (0x00 << 16)); // Note off
					}
				}
			}
		}

		LeaveCriticalSection(&midi_critical_section);

		if (midi_playing) return -3;
	}

#endif

    return 0;
}

static int MIDI_PLUGIN2_API resume(void)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -1;
	if (!midi_loaded) return -2;

	if (!midi_playing)
	{
		EnterCriticalSection(&midi_critical_section);

		if (state_mt32_display == 4)
		{
			state_mt32_display = 2;
			send_initial_sysex_events(sysex_mt32_reset_display);
		}

		if (MMSYSERR_NOERROR == midiStreamRestart(hStream))
		{
			midi_playing = 1;
		}

		LeaveCriticalSection(&midi_critical_section);

		if (!midi_playing) return -3;
	}

#endif

    return 0;
}

static int MIDI_PLUGIN2_API halt(void)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -1;

	close_midi();

#endif

    return 0;
}

static int MIDI_PLUGIN2_API set_volume(unsigned char volume) // volume = 0 - 127
{
    if (volume > 127) volume = 127;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -1;

	midi_new_volume = volume;

#endif

    return 0;
}

static int MIDI_PLUGIN2_API set_loop_count(int loop_count) // -1 = unlimited
{
    if (loop_count < -1) loop_count = -1;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -1;
	if (!midi_loaded) return -2;

	midi_loop_count = loop_count;

#endif

    return 0;
}

static void MIDI_PLUGIN2_API shutdown_plugin(void)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hNotificationInstance != NULL)
	{
		dyn_CM_Unregister_Notification(hNotificationInstance);
		hNotificationInstance = NULL;
	}
	if (hNotificationHandle != NULL)
	{
		dyn_CM_Unregister_Notification(hNotificationHandle);
		hNotificationHandle = NULL;
	}
	notification_events = 0;

	if (hStream != NULL)
	{
		if (midi_loaded)
		{
			close_midi();
		}
		else
		{
			reset_playing();
		}
	}

	if (midi_thread_handle != NULL)
	{
		midi_quit = 1;
		WaitForSingleObject(midi_thread_handle, INFINITE);
		CloseHandle(midi_thread_handle);
		midi_thread_handle = NULL;
		midi_quit = 0;
	}

	if (hAvrt != NULL)
	{
		FreeLibrary(hAvrt);
		hAvrt = NULL;
	}

	if (hCfgmgr32 != NULL)
	{
		FreeLibrary(hCfgmgr32);
		hCfgmgr32 = NULL;
	}

	if (hStream != NULL)
	{
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
		deinitialize_queue();
		midiStreamClose(hStream);
		hStream = NULL;
	}

	if (lpDeviceInterfaceName != NULL)
	{
		free(lpDeviceInterfaceName);
		lpDeviceInterfaceName = NULL;
	}

	if (midi_stream_name != NULL)
	{
		free(midi_stream_name);
		midi_stream_name = NULL;
	}

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

	DeleteCriticalSection(&notification_critical_section);
	DeleteCriticalSection(&midi_critical_section);
#endif
}


EXPORT
int MIDI_PLUGIN2_API initialize_midi_plugin2(midi_plugin2_parameters const *parameters, midi_plugin2_functions *functions)
{
    if (functions == NULL) return -3;

    midi_type = (parameters != NULL) ? parameters->midi_type : 0;

    functions->play = &play;
    functions->pause = &pause;
    functions->resume = &resume;
    functions->halt = &halt;
    functions->set_volume = &set_volume;
    functions->set_loop_count = &set_loop_count;
    functions->shutdown_plugin = &shutdown_plugin;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
{
	char const *device_name;
	unsigned char const *sysex_events, *controller_events;
	int events_len;
	int numDevices, devid;
	MIDIOUTCAPS midicaps;
	UINT uDeviceID;

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

	InitializeCriticalSection(&midi_critical_section);
	InitializeCriticalSection(&notification_critical_section);

	load_windows_dlls();

	numDevices = midiOutGetNumDevs();
	if (numDevices == 0)
	{
		shutdown_plugin();
		return -4;
	}

	if (device_name == NULL || *device_name == 0)
	{
		uDeviceID = MIDI_MAPPER;
	}
	else
	{
		uDeviceID = numDevices;
		for (devid = -1; devid < numDevices; devid++)
		{
			if (MMSYSERR_NOERROR != midiOutGetDevCaps(devid, &midicaps, sizeof(midicaps))) continue;

			if (0 == strcmp(device_name, midicaps.szPname))
			{
				uDeviceID = devid;
				midi_stream_name = strdup(midicaps.szPname);
				break;
			}
		}

		if (uDeviceID == numDevices)
		{
			long device_num;
			char *endptr;

			errno = 0;
			device_num = strtol(device_name, &endptr, 10);
			if ((errno == 0) && (*endptr == 0) && (device_num >= 0) && (device_num < numDevices))
			{
				if (MMSYSERR_NOERROR == midiOutGetDevCaps(device_num, &midicaps, sizeof(midicaps)))
				{
					uDeviceID = device_num;
					midi_stream_name = strdup(midicaps.szPname);
				}
			}
		}

		if (uDeviceID == numDevices)
		{
			shutdown_plugin();
			return -5;
		}
	}

	if (MMSYSERR_NOERROR != midiStreamOpen(&hStream, &uDeviceID, 1, 0, 0, CALLBACK_NULL))
	{
		if (device_name != NULL && *device_name != 0)
		{
			hStream = NULL;
			shutdown_plugin();
			return -6;
		}

		uDeviceID = 0;
		if (MMSYSERR_NOERROR != midiStreamOpen(&hStream, &uDeviceID, 1, 0, 0, CALLBACK_NULL))
		{
			hStream = NULL;
			shutdown_plugin();
			return -6;
		}
	}

	get_device_interface_name(uDeviceID);
	register_notifications();

	if (!initialize_queue())
	{
		deinitialize_queue();
		midiStreamClose(hStream);
		hStream = NULL;
		shutdown_plugin();
		return -7;
	}

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

	midi_thread_handle = CreateThread(NULL, 65536, &MidiThreadProc, NULL, 0, NULL);
	if (midi_thread_handle == NULL)
	{
		if (midi_type == 2)
		{
			mt32_shutdown_gm();
		}
		deinitialize_queue();
		midiStreamClose(hStream);
		hStream = NULL;
		shutdown_plugin();
		return -8;
	}
}
#endif

    return 0;
}

