/**
 *
 *  Copyright (C) 2016-2020 Roman Pauer
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
    #include <windows.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "midi-plugins2.h"

static char *midi_device_name = NULL;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
static HMIDISTRM hStream = NULL;
static HANDLE midi_thread_handle = NULL;

static volatile int midi_quit = 0;
static volatile int midi_loaded = 0;
static int midi_playing = 0;
static volatile int midi_loop_count = 0;
static volatile uint8_t *midi_first_data = NULL;
static volatile uint8_t *midi_next_data = NULL;
static MIDIHDR midi_headers[4];
static volatile int midi_header_wait = 0;
static volatile int midi_header_send = 0;

static CRITICAL_SECTION midi_critical_section;

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

typedef struct {
	const uint8_t *ptr;
	unsigned int len, delta;
	uint8_t event_status;
	int eot;
} midi_track_info;

typedef struct {
	uint8_t *next;
	unsigned int len;
} midi_data_info;

#define MAXIMUM_DATA_LENGTH (65536 - sizeof(midi_data_info))

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

static void free_midi_data(uint8_t *data)
{
	while (data != NULL)
	{
		midi_data_info *info;
		uint8_t *next;

		info = (midi_data_info *) &(data[MAXIMUM_DATA_LENGTH]);
		next = info->next;
		free(data);
		data = next;
	};
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

static int preprocessmidi(const uint8_t *midi, unsigned int midilen, unsigned int *timediv, uint8_t **dataptr)
{
	unsigned int number_of_tracks, time_division, index, varlen;
	midi_track_info *tracks, *curtrack;
	uint8_t *first_data, *curdata;
	midi_data_info *curinfo;
	int retval, lasttracknum, eventextralen;
	MIDIEVENT event;

	retval = readmidi(midi, midilen, &number_of_tracks, &time_division, &tracks);
	if (retval) return retval;

	// prepare tracks
	for (index = 0; index < number_of_tracks; index++)
	{
		curtrack = &(tracks[index]);

		// read delta
		curtrack->delta = read_varlen(curtrack);
	}

	first_data = (uint8_t *) malloc(65536);
	if (first_data == NULL)
	{
		retval = 11;
		goto midi_error_1;
	}

	curdata = first_data;
	curinfo = (midi_data_info *) &(curdata[MAXIMUM_DATA_LENGTH]);
	curinfo->next = NULL;
	curinfo->len = 0;

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
		event.dwDeltaTime = curtrack->delta;
		event.dwStreamID = 0;

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
					event.dwEvent = MEVT_F_SHORT | (curtrack->event_status) | (((uint32_t)(curtrack->ptr[0])) << 8) | (((uint32_t)(curtrack->ptr[1])) << 16);
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
					event.dwEvent = MEVT_F_SHORT | (curtrack->event_status) | (((uint32_t)(curtrack->ptr[0])) << 8);
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
				if (curtrack->event_status == 0xff)
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
								event.dwEvent = (((uint32_t)MEVT_TEMPO) << 24) | (((uint32_t)(curtrack->ptr[2])) << 16) | (((uint32_t)(curtrack->ptr[3])) << 8) | ((uint32_t)(curtrack->ptr[4]));
								eventextralen = 0;
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
				else if ((curtrack->event_status == 0xf0) || (curtrack->event_status == 0xf7))
				{
					const uint8_t *startevent;

					startevent = curtrack->ptr;

					varlen = read_varlen(curtrack);
					if (varlen <= curtrack->len)
					{
						curtrack->ptr += varlen;
						curtrack->len -= varlen;

						eventextralen = 1 + curtrack->ptr - startevent;
						event.dwEvent = MEVT_F_LONG | (eventextralen & 0xffffff);
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
#define SHORT_EVENT_LENGTH (sizeof(event) - sizeof(event.dwParms))

			if ((curinfo->len + SHORT_EVENT_LENGTH + ((eventextralen + 3) & ~3)) > MAXIMUM_DATA_LENGTH)
			{
				if ((SHORT_EVENT_LENGTH + ((eventextralen + 3) & ~3)) > MAXIMUM_DATA_LENGTH)
				{
					retval = 12;
					goto midi_error_2;
				}

				curinfo->next = (uint8_t *) malloc(65536);
				if (curinfo->next == NULL)
				{
					retval = 13;
					goto midi_error_2;
				}

				curdata = curinfo->next;
				curinfo = (midi_data_info *) &(curdata[MAXIMUM_DATA_LENGTH]);
				curinfo->next = NULL;
				curinfo->len = 0;
			}

			memcpy(&(curdata[curinfo->len]), &event, SHORT_EVENT_LENGTH);
			curinfo->len += SHORT_EVENT_LENGTH;

			if (eventextralen > 0)
			{
				curdata[curinfo->len] = curtrack->event_status;
				memcpy(&(curdata[curinfo->len + 1]), 1 + curtrack->ptr - eventextralen, eventextralen - 1);
				curinfo->len += (eventextralen + 3) & ~3;
			}

#undef SHORT_EVENT_LENGTH
		}

		// read delta
		curtrack->delta = read_varlen(curtrack);
	};

	if ((curdata == first_data) && (curinfo->len == 0))
	{
		retval = 14;
		goto midi_error_2;
	}

	// return values
	*timediv = time_division;
	*dataptr = first_data;

	free(tracks);
	return 0;

midi_error_2:
	free_midi_data(first_data);
midi_error_1:
	free(tracks);
	return retval;
}


static DWORD WINAPI MidiThreadProc(LPVOID lpParameter)
{
	unsigned int num_waiting, send_next;
	midi_data_info *midi_info;

	while (1)
	{
		Sleep(10);
		if (midi_quit) return 0;
		if (!midi_loaded) continue;

		num_waiting = (midi_header_send + 4 - midi_header_wait) & 3;
		if (num_waiting >= 2) continue;


		EnterCriticalSection(&midi_critical_section);

		if (!midi_loaded)
		{
			LeaveCriticalSection(&midi_critical_section);
			continue;
		}

		num_waiting = (midi_header_send + 4 - midi_header_wait) & 3;
		while (num_waiting > 0)
		{
			if (!(midi_headers[midi_header_wait].dwFlags & MHDR_DONE))
			{
				break;
			}

			midiOutUnprepareHeader((HMIDIOUT)hStream, &(midi_headers[midi_header_wait]), sizeof(MIDIHDR));
			midi_header_wait = (midi_header_wait + 1) & 3;

			num_waiting = (midi_header_send + 4 - midi_header_wait) & 3;
		}

		while ((num_waiting < 2) && (midi_next_data != NULL))
		{
			midi_info = (midi_data_info *) &(midi_next_data[MAXIMUM_DATA_LENGTH]);

			midi_headers[midi_header_send].lpData = (LPSTR) midi_next_data;
			midi_headers[midi_header_send].dwBufferLength = midi_info->len;
			midi_headers[midi_header_send].dwBytesRecorded = midi_info->len;
			midi_headers[midi_header_send].dwFlags = 0;

			if (MMSYSERR_NOERROR != midiOutPrepareHeader((HMIDIOUT)hStream, &(midi_headers[midi_header_send]), sizeof(MIDIHDR)))
			{
				break;
			}

			if (MMSYSERR_NOERROR != midiStreamOut(hStream, &(midi_headers[midi_header_send]), sizeof(MIDIHDR)))
			{
				midiOutUnprepareHeader((HMIDIOUT)hStream, &(midi_headers[midi_header_send]), sizeof(MIDIHDR));
				break;
			}

			if (midi_info->next == NULL)
			{
				if (midi_loop_count == 0)
				{
					midi_next_data = NULL;
					break;
				}
				else if (midi_loop_count > 0)
				{
					midi_loop_count--;
				}
			}

			send_next = ((midi_info->next == NULL) && (midi_next_data != midi_first_data))?1:0;

			midi_header_send = (midi_header_send + 1) & 3;
			midi_next_data = (midi_info->next == NULL)?midi_first_data:midi_info->next;

			if (send_next)
			{
				midi_info = (midi_data_info *) &(midi_next_data[MAXIMUM_DATA_LENGTH]);

				midi_headers[midi_header_send].lpData = (LPSTR) midi_next_data;
				midi_headers[midi_header_send].dwBufferLength = midi_info->len;
				midi_headers[midi_header_send].dwBytesRecorded = midi_info->len;
				midi_headers[midi_header_send].dwFlags = 0;

				if (MMSYSERR_NOERROR != midiOutPrepareHeader((HMIDIOUT)hStream, &(midi_headers[midi_header_send]), sizeof(MIDIHDR)))
				{
					break;
				}

				if (MMSYSERR_NOERROR != midiStreamOut(hStream, &(midi_headers[midi_header_send]), sizeof(MIDIHDR)))
				{
					midiOutUnprepareHeader((HMIDIOUT)hStream, &(midi_headers[midi_header_send]), sizeof(MIDIHDR));
					break;
				}

				midi_header_send = (midi_header_send + 1) & 3;
				midi_next_data = (midi_info->next == NULL)?midi_first_data:midi_info->next;
			}

			num_waiting = (midi_header_send + 4 - midi_header_wait) & 3;
		}

		LeaveCriticalSection(&midi_critical_section);
	};
}


static void close_midi(void)
{
	if (!midi_loaded) return;

	EnterCriticalSection(&midi_critical_section);

	if (midi_playing)
	{
		if (MMSYSERR_NOERROR == midiStreamPause(hStream))
		{
			midi_playing = 0;
		}
	}

	midiStreamStop(hStream);

	while (midi_header_wait != midi_header_send)
	{
		midiOutUnprepareHeader((HMIDIOUT)hStream, &(midi_headers[midi_header_wait]), sizeof(MIDIHDR));
		midi_header_wait = (midi_header_wait + 1) & 3;
	};

	free_midi_data((uint8_t *) midi_first_data);
	midi_first_data = midi_next_data = NULL;
	midi_header_wait = midi_header_send = 0;
	midi_loaded = 0;

	LeaveCriticalSection(&midi_critical_section);
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
		uint8_t *dataptr;
		MIDIPROPTEMPO miditempo;
		MIDIPROPTIMEDIV miditimediv;

		if (preprocessmidi(midibuffer, size, &timediv, &dataptr))
		{
			return -4;
		}

		EnterCriticalSection(&midi_critical_section);

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

		midi_first_data = midi_next_data = dataptr;
		midi_header_wait = midi_header_send = 0;

		midi_loaded = 1;

		if (!midi_playing)
		{
			if (MMSYSERR_NOERROR == midiStreamRestart(hStream))
			{
				midi_playing = 1;
			}
		}

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
        EnterCriticalSection(&midi_critical_section);

		if (MMSYSERR_NOERROR == midiStreamPause(hStream))
		{
			midi_playing = 0;
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

	{
		DWORD volparam;
		int ok;

		volparam = 33818125;
		volparam = (volparam * volume) >> 16;
		volparam |= volparam << 16;

		EnterCriticalSection(&midi_critical_section);

		if (MMSYSERR_NOERROR == midiOutSetVolume((HMIDIOUT)hStream, volparam))
		{
			ok = 1;
		}
		else
		{
			ok = 0;
		}

		LeaveCriticalSection(&midi_critical_section);

		if (!ok) return -3;
	}

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
	if (hStream != NULL)
	{
		close_midi();
	}

	if (midi_thread_handle != NULL)
	{
		midi_quit = 1;
		WaitForSingleObject(midi_thread_handle, INFINITE);
		CloseHandle(midi_thread_handle);
		midi_thread_handle = NULL;
		midi_quit = 0;
	}

	if (hStream != NULL)
	{
		DeleteCriticalSection(&midi_critical_section);

		midiStreamClose(hStream);
		hStream = NULL;
	}
#endif

    if (midi_device_name != NULL)
    {
        free(midi_device_name);
        midi_device_name = NULL;
    }
}


int initialize_midi_plugin2(midi_plugin2_parameters const *parameters, midi_plugin2_functions *functions)
{
    char const *device_name;

    if (functions == NULL) return -3;

    device_name = NULL;
    if (parameters != NULL)
    {
        device_name = parameters->midi_device_name;
    }

    if ((device_name != NULL) && (*device_name != 0))
    {
        midi_device_name = strdup(device_name);
    }

    functions->play = &play;
    functions->pause = &pause;
    functions->resume = &resume;
    functions->halt = &halt;
    functions->set_volume = &set_volume;
    functions->set_loop_count = &set_loop_count;
    functions->shutdown_plugin = &shutdown_plugin;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
{
	int numDevices, devid;
	MIDIOUTCAPS midicaps;
	UINT uDeviceID;

	numDevices = midiOutGetNumDevs();
	if (numDevices == 0) return -4;

	if (midi_device_name == NULL)
	{
		uDeviceID = MIDI_MAPPER;
	}
	else
	{
		uDeviceID = numDevices;
		for (devid = -1; devid < numDevices; devid++)
		{
			if (MMSYSERR_NOERROR != midiOutGetDevCaps(devid, &midicaps, sizeof(midicaps))) continue;

			if (0 == strcmp(midi_device_name, midicaps.szPname))
			{
				uDeviceID = devid;
				break;
			}
		}

		if (uDeviceID == numDevices) return -5;
	}

	if (MMSYSERR_NOERROR != midiStreamOpen(&hStream, &uDeviceID, 1, 0, 0, CALLBACK_NULL))
	{
		if (midi_device_name != NULL) return -6;

		uDeviceID = 0;
		if (MMSYSERR_NOERROR != midiStreamOpen(&hStream, &uDeviceID, 1, 0, 0, CALLBACK_NULL))
		{
			return -6;
		}
	}

	midi_thread_handle = CreateThread(NULL, 4096, &MidiThreadProc, NULL, 0, NULL);
	if (midi_thread_handle == NULL)
	{
		midiStreamClose(hStream);
		hStream = NULL;

		return -7;
	}

    InitializeCriticalSection(&midi_critical_section);
}
#endif

    return 0;
}

