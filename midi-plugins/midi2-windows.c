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

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #ifndef WINVER
    #define WINVER 0x0602
    #endif
    #include <windows.h>
    #include <cfgmgr32.h>
    #include <mmddk.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "midi-plugins2.h"

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
	const uint8_t *ptr;
	unsigned int len, delta;
	uint8_t event_status;
	int eot;
} midi_track_info;

typedef struct {
	uint32_t tick;
	uint32_t event;
	uint8_t *sysex;
	uint32_t time;
} midi_event_info;

typedef struct _midi_queue_info_ {
	MIDIHDR header;
	struct _midi_queue_info_ *next;
	uint8_t buffer[65532];
} midi_queue_info;


static HMIDISTRM hStream = NULL;
static HANDLE midi_thread_handle = NULL;
static char *midi_stream_name = NULL;
static LPWSTR lpDeviceInterfaceName = NULL;
static unsigned char *initial_sysex_events = NULL;
static unsigned char *reset_controller_events = NULL;

static HMODULE hAvrt = NULL;
static AvSetMmThreadCharacteristicsWFunc dyn_AvSetMmThreadCharacteristicsW;
static AvRevertMmThreadCharacteristicsFunc dyn_AvRevertMmThreadCharacteristics;

static HMODULE hCfgmgr32 = NULL;
static CM_Register_Notification_Func dyn_CM_Register_Notification;
static CM_Unregister_Notification_Func dyn_CM_Unregister_Notification;

static HCMNOTIFICATION hNotificationHandle = NULL;
static HCMNOTIFICATION hNotificationInstance = NULL;
static volatile unsigned int notification_events = 0;

static volatile int midi_quit = 0;
static volatile int midi_loaded = 0;
static volatile int midi_playing = 0;
static volatile int midi_current_volume = 127;
static volatile int midi_new_volume = 127;
static volatile int midi_loop_count = 0;
static volatile int midi_eof = 0;

static volatile midi_queue_info *midi_queue_wait = NULL;
static volatile midi_queue_info *midi_queue_send = NULL;

static volatile midi_event_info *midi_events = NULL;
static volatile unsigned int midi_current_event;
static volatile uint32_t midi_base_time;

static int channel_volume[16];
static int channel_notes[16][128];

static CRITICAL_SECTION midi_critical_section, notification_critical_section;

#define GETU32FBE(buf) (			\
            (uint32_t) ( (buf)[0] ) << 24 | \
            (uint32_t) ( (buf)[1] ) << 16 | \
            (uint32_t) ( (buf)[2] ) <<  8 | \
            (uint32_t) ( (buf)[3] )       )

#define GETU16FBE(buf) (			\
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
	unsigned int number_of_tracks, time_division, index, varlen;
	midi_track_info *tracks, *curtrack;
	unsigned int num_allocated, num_events, last_tick;
	midi_event_info *events;
	int retval, lasttracknum, eventextralen;
	midi_event_info event;
	unsigned int tempo, tempo_tick;
	uint64_t tempo_time, last_time;

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
	events[0].event = 0;
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

			divres = div(last_tick - tempo_tick, time_division);

			last_time = ( ((1000 * divres.rem) * (uint64_t)tempo) / time_division )
			          + ( (divres.quot * (uint64_t)tempo) * 1000 )
			          + tempo_time
			          ;

			//last_time = ( (((last_tick - tempo_tick) * (uint64_t) 1000) * tempo) / time_division ) + tempo_time;
		}

		// calculate event time in milliseconds
		event.time = last_time / 1000000;

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
					event.event = MEVT_F_SHORT | (curtrack->event_status) | (((uint32_t)(curtrack->ptr[0])) << 8) | (((uint32_t)(curtrack->ptr[1])) << 16);
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
					event.event = MEVT_F_SHORT | (curtrack->event_status) | (((uint32_t)(curtrack->ptr[0])) << 8);
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
								event.event = (((uint32_t)MEVT_TEMPO) << 24) | (((uint32_t)(curtrack->ptr[2])) << 16) | (((uint32_t)(curtrack->ptr[3])) << 8) | ((uint32_t)(curtrack->ptr[4]));
								eventextralen = 0;

								// time_division is assumed to be positive (ticks per beat / PPQN - Pulses (i.e. clocks) Per Quarter Note)

								tempo = MEVT_EVENTPARM(event.event);
								tempo_tick = last_tick;
								tempo_time = last_time;
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
						event.event = varlen + ((curtrack->event_status == 0xf0)?1:0);
						if (event.event)
						{
							if ((event.event <= 0xffffff) && (offsetof(MIDIEVENT, dwParms) + ((event.event + 3) & ~3) <= sizeof(midi_queue_send->buffer)))
							{
								event.sysex = (uint8_t *) malloc(event.event);
								if (event.sysex == NULL)
								{
									retval = 12;
									goto midi_error_2;
								}

								event.event |= MEVT_F_LONG;

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
			events[0].event = num_events;
			num_events++;
		}

		// read delta
		curtrack->delta = read_varlen(curtrack);
	};

	if (events[0].event == 0)
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


static int send_initial_sysex_events(unsigned char const *sysex_events);
static void reset_playing(void);
static void get_device_interface_name(UINT uDeviceID);
static void register_notifications(void);
static midi_queue_info *allocate_queue_info(void);

static DWORD WINAPI MidiThreadProc(LPVOID lpParameter)
{
	midi_event_info *events;
	unsigned int current_event, num_events, current_notification_events;
	uint32_t base_time, stream_time;
	int do_sleep, midi_stream_exists, chan;
	midi_queue_info *current_queue_wait, *current_queue_send, *qinfo;
	HANDLE AvrtHandle;
	DWORD TaskIndex;
	MMTIME mmtime;
	MIDIEVENT *event;

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

						if (initial_sysex_events != NULL && *initial_sysex_events == 0xf0)
						{
							send_initial_sysex_events(initial_sysex_events);
						}

						reset_playing();

						if (midi_loaded && !midi_eof)
						{
							midiStreamProperty(hStream, (LPBYTE)&miditimediv, MIDIPROP_SET | MIDIPROP_TIMEDIV);

							midiStreamProperty(hStream, (LPBYTE)&miditempo, MIDIPROP_SET | MIDIPROP_TEMPO);

							for (chan = 0; chan < 16; chan++)
							{
								channel_volume[chan] = 100;
							}

							// set current volume to a different value than new volume in order to set the channel volume at start of playing
							midi_current_volume = midi_new_volume ^ 1;

							memset(channel_notes, 0, 128*16*sizeof(int));

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

		if ((!midi_loaded) || (!midi_playing) || (midi_eof))
		{
			LeaveCriticalSection(&midi_critical_section);
			do_sleep = 1;
			continue;
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

		if (midi_new_volume != midi_current_volume)
		{
			midi_current_volume = midi_new_volume;

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

			for (chan = 0; chan < 16; chan++)
			{
				event = (MIDIEVENT *) &(current_queue_send->buffer[current_queue_send->header.dwBytesRecorded]);

				event->dwDeltaTime = 0;
				event->dwStreamID = 0;
				event->dwEvent = MEVT_F_SHORT | 0xb0 | chan | (7 << 8) | (((midi_current_volume * channel_volume[chan]) / 127) << 16);

				current_queue_send->header.dwBytesRecorded += offsetof(MIDIEVENT, dwParms);
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
	int events_len;

	if (hStream == NULL) return 0;

	events_len = 0;

	while (*sysex_events == 0xf0)
	{
		int len;
		MIDIHDR midihdr;

		len = 2;
		while (sysex_events[len - 1] != 0xf7) len++;

		midihdr.lpData = (LPSTR)sysex_events;
		midihdr.dwBufferLength = len;
		midihdr.dwBytesRecorded = len;
		midihdr.dwFlags = 0;

		sysex_events += len;
		events_len += len;

		midiOutPrepareHeader((HMIDIOUT)hStream, &midihdr, sizeof(midihdr));
		midiOutLongMsg((HMIDIOUT)hStream, &midihdr, sizeof(midihdr));
		midiOutUnprepareHeader((HMIDIOUT)hStream, &midihdr, sizeof(midihdr));
	};

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

	if (MMSYSERR_NOERROR != midiOutMessage((HMIDIOUT)uDeviceID, DRV_QUERYDEVICEINTERFACESIZE, (DWORD_PTR)&uSize, 0))
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

	if (MMSYSERR_NOERROR != midiOutMessage((HMIDIOUT)uDeviceID, DRV_QUERYDEVICEINTERFACE, (DWORD_PTR)lpDeviceInterfaceName, uSize))
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


static int play(void const *midibuffer, long int size, int loop_count)
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

		for (chan = 0; chan < 16; chan++)
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

		memset(channel_notes, 0, 128*16*sizeof(int));

		LeaveCriticalSection(&midi_critical_section);
	}
#endif

    return 0;
}

static int pause(void)
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

		if (MMSYSERR_NOERROR == midiStreamPause(hStream))
		{
			midi_playing = 0;

			// stop playing notes on all channels
			for (chan = 0; chan < 16; chan++)
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

static int resume(void)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -1;
	if (!midi_loaded) return -2;

	if (!midi_playing)
	{
		EnterCriticalSection(&midi_critical_section);

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

static int halt(void)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -1;

	close_midi();

#endif

    return 0;
}

static int set_volume(unsigned char volume) // volume = 0 - 127
{
    if (volume > 127) volume = 127;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -1;

	midi_new_volume = volume;

#endif

    return 0;
}

static int set_loop_count(int loop_count) // -1 = unlimited
{
    if (loop_count < -1) loop_count = -1;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	if (hStream == NULL) return -1;
	if (!midi_loaded) return -2;

	midi_loop_count = loop_count;

#endif

    return 0;
}

static void shutdown_plugin(void)
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


__attribute__ ((visibility ("default")))
int initialize_midi_plugin2(midi_plugin2_parameters const *parameters, midi_plugin2_functions *functions)
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
	if (parameters != NULL)
	{
		device_name = parameters->midi_device_name;
		sysex_events = parameters->initial_sysex_events;
		controller_events = parameters->reset_controller_events;
	}

	memset(channel_notes, 0, 128*16*sizeof(int));

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

	midi_thread_handle = CreateThread(NULL, 4096, &MidiThreadProc, NULL, 0, NULL);
	if (midi_thread_handle == NULL)
	{
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

