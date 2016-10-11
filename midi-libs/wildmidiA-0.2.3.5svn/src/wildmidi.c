  /*
	wildmidi.c

 	Midi Player using the WildMidi Midi Processing Library

    Copyright (C) Chris Ison 2001-2011

    This file is part of WildMIDI.

    WildMIDI is free software: you can redistribute and/or modify the player
    under the terms of the GNU General Public License and you can redistribute
    and/or modify the library under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation, either version 3 of
    the licenses, or(at your option) any later version.

    WildMIDI is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License and
    the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU General Public License and the
    GNU Lesser General Public License along with WildMIDI.  If not,  see
    <http://www.gnu.org/licenses/>.

    Email: wildcode@users.sourceforge.net
*/

#include "config.h"

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#if (defined _WIN32) || (defined __CYGWIN__)
# include <conio.h>
# include <windows.h>
# include <mmsystem.h>
#else
# ifdef HAVE_ALSA
#  include <alsa/asoundlib.h>
# else
#  ifdef HAVE_SYS_SOUNDCARD_H
#   include <sys/soundcard.h>
#  elif defined HAVE_LINUX_SOUNDCARD_H
#   include <linux/soundcard.h>
#  elif HAVE_MACHINE_SOUNDCARD_H
#   include <machine/soundcard.h>
#  endif
# endif
#endif

#include "wildmidi_lib.h"

#ifndef FNONBLOCK
#define FNONBLOCK _FNONBLOCK
#endif


struct _midi_test {
	unsigned char *data;
	unsigned long int size;
};

// scale test from 0 to 127
/* test a
 * offset 18-21 (0x12-0x15) - track size
 * offset 25 (0x1A) = bank number
 * offset 28 (0x1D) = patch number
 */
unsigned char midi_test_c_scale[] = {
	0x4d, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, // 0x00
	0x00, 0x00, 0x00, 0x01, 0x00, 0x06, 0x4d, 0x54, // 0x08
	0x72, 0x6b, 0x00, 0x00, 0x02, 0x63, 0x00, 0xb0, // 0x10
	0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x90, 0x00,	// 0x18  C
	0x64, 0x08, 0x80, 0x00, 0x00, 0x08, 0x90, 0x02,	// 0x20  D
	0x64, 0x08, 0x80, 0x02, 0x00, 0x08, 0x90, 0x04,	// 0x28  E
	0x64, 0x08, 0x80, 0x04, 0x00, 0x08, 0x90, 0x05,	// 0x30  F
	0x64, 0x08, 0x80, 0x05, 0x00, 0x08, 0x90, 0x07,	// 0x38  G
	0x64, 0x08, 0x80, 0x07, 0x00, 0x08, 0x90, 0x09,	// 0x40  A
	0x64, 0x08, 0x80, 0x09, 0x00, 0x08, 0x90, 0x0b,	// 0x48  B
	0x64, 0x08, 0x80, 0x0b, 0x00, 0x08, 0x90, 0x0c,	// 0x50  C
	0x64, 0x08, 0x80, 0x0c, 0x00, 0x08, 0x90, 0x0e,	// 0x58  D
	0x64, 0x08, 0x80, 0x0e, 0x00, 0x08, 0x90, 0x10,	// 0x60  E
	0x64, 0x08, 0x80, 0x10, 0x00, 0x08, 0x90, 0x11,	// 0x68  F
	0x64, 0x08, 0x80, 0x11, 0x00, 0x08, 0x90, 0x13,	// 0x70  G
	0x64, 0x08, 0x80, 0x13, 0x00, 0x08, 0x90, 0x15,	// 0x78  A
	0x64, 0x08, 0x80, 0x15, 0x00, 0x08, 0x90, 0x17,	// 0x80  B
	0x64, 0x08, 0x80, 0x17, 0x00, 0x08, 0x90, 0x18,	// 0x88  C
	0x64, 0x08, 0x80, 0x18, 0x00, 0x08, 0x90, 0x1a,	// 0x90  D
	0x64, 0x08, 0x80, 0x1a, 0x00, 0x08, 0x90, 0x1c,	// 0x98  E
	0x64, 0x08, 0x80, 0x1c, 0x00, 0x08, 0x90, 0x1d,	// 0xA0  F
	0x64, 0x08, 0x80, 0x1d, 0x00, 0x08, 0x90, 0x1f,	// 0xA8  G
	0x64, 0x08, 0x80, 0x1f, 0x00, 0x08, 0x90, 0x21,	// 0xB0  A
	0x64, 0x08, 0x80, 0x21, 0x00, 0x08, 0x90, 0x23,	// 0xB8  B
	0x64, 0x08, 0x80, 0x23, 0x00, 0x08, 0x90, 0x24,	// 0xC0  C
	0x64, 0x08, 0x80, 0x24, 0x00, 0x08, 0x90, 0x26,	// 0xC8  D
	0x64, 0x08, 0x80, 0x26, 0x00, 0x08, 0x90, 0x28,	// 0xD0  E
	0x64, 0x08, 0x80, 0x28, 0x00, 0x08, 0x90, 0x29,	// 0xD8  F
	0x64, 0x08, 0x80, 0x29, 0x00, 0x08, 0x90, 0x2b,	// 0xE0  G
	0x64, 0x08, 0x80, 0x2b, 0x00, 0x08, 0x90, 0x2d,	// 0xE8  A
	0x64, 0x08, 0x80, 0x2d, 0x00, 0x08, 0x90, 0x2f,	// 0xF0  B
	0x64, 0x08, 0x80, 0x2f, 0x00, 0x08, 0x90, 0x30,	// 0xF8  C
	0x64, 0x08, 0x80, 0x30, 0x00, 0x08, 0x90, 0x32,	// 0x100  D
	0x64, 0x08, 0x80, 0x32, 0x00, 0x08, 0x90, 0x34,	// 0x108  E
	0x64, 0x08, 0x80, 0x34, 0x00, 0x08, 0x90, 0x35,	// 0x110  F
	0x64, 0x08, 0x80, 0x35, 0x00, 0x08, 0x90, 0x37,	// 0x118  G
	0x64, 0x08, 0x80, 0x37, 0x00, 0x08, 0x90, 0x39,	// 0x120  A
	0x64, 0x08, 0x80, 0x39, 0x00, 0x08, 0x90, 0x3b,	// 0X128  B
	0x64, 0x08, 0x80, 0x3b, 0x00, 0x08, 0x90, 0x3c,	// 0x130  C
	0x64, 0x08, 0x80, 0x3c, 0x00, 0x08, 0x90, 0x3e,	// 0x138  D
	0x64, 0x08, 0x80, 0x3e, 0x00, 0x08, 0x90, 0x40,	// 0X140  E
	0x64, 0x08, 0x80, 0x40, 0x00, 0x08, 0x90, 0x41,	// 0x148  F
	0x64, 0x08, 0x80, 0x41, 0x00, 0x08, 0x90, 0x43,	// 0x150  G
	0x64, 0x08, 0x80, 0x43, 0x00, 0x08, 0x90, 0x45,	// 0x158  A
	0x64, 0x08, 0x80, 0x45, 0x00, 0x08, 0x90, 0x47,	// 0x160  B
	0x64, 0x08, 0x80, 0x47, 0x00, 0x08, 0x90, 0x48,	// 0x168  C
	0x64, 0x08, 0x80, 0x48, 0x00, 0x08, 0x90, 0x4a,	// 0x170  D
	0x64, 0x08, 0x80, 0x4a, 0x00, 0x08, 0x90, 0x4c,	// 0x178  E
	0x64, 0x08, 0x80, 0x4c, 0x00, 0x08, 0x90, 0x4d,	// 0x180  F
	0x64, 0x08, 0x80, 0x4d, 0x00, 0x08, 0x90, 0x4f,	// 0x188  G
	0x64, 0x08, 0x80, 0x4f, 0x00, 0x08, 0x90, 0x51,	// 0x190  A
	0x64, 0x08, 0x80, 0x51, 0x00, 0x08, 0x90, 0x53,	// 0x198  B
	0x64, 0x08, 0x80, 0x53, 0x00, 0x08, 0x90, 0x54,	// 0x1A0  C
	0x64, 0x08, 0x80, 0x54, 0x00, 0x08, 0x90, 0x56,	// 0x1A8  D
	0x64, 0x08, 0x80, 0x56, 0x00, 0x08, 0x90, 0x58,	// 0x1B0  E
	0x64, 0x08, 0x80, 0x58, 0x00, 0x08, 0x90, 0x59,	// 0x1B8  F
	0x64, 0x08, 0x80, 0x59, 0x00, 0x08, 0x90, 0x5b,	// 0x1C0  G
	0x64, 0x08, 0x80, 0x5b, 0x00, 0x08, 0x90, 0x5d,	// 0x1C8  A
	0x64, 0x08, 0x80, 0x5d, 0x00, 0x08, 0x90, 0x5f,	// 0x1D0  B
	0x64, 0x08, 0x80, 0x5f, 0x00, 0x08, 0x90, 0x60,	// 0x1D8  C
	0x64, 0x08, 0x80, 0x60, 0x00, 0x08, 0x90, 0x62,	// 0x1E0  D
	0x64, 0x08, 0x80, 0x62, 0x00, 0x08, 0x90, 0x64,	// 0x1E8  E
	0x64, 0x08, 0x80, 0x64, 0x00, 0x08, 0x90, 0x65,	// 0x1F0  F
	0x64, 0x08, 0x80, 0x65, 0x00, 0x08, 0x90, 0x67,	// 0x1F8  G
	0x64, 0x08, 0x80, 0x67, 0x00, 0x08, 0x90, 0x69,	// 0x200  A
	0x64, 0x08, 0x80, 0x69, 0x00, 0x08, 0x90, 0x6b,	// 0x208  B
	0x64, 0x08, 0x80, 0x6b, 0x00, 0x08, 0x90, 0x6c,	// 0x210  C
	0x64, 0x08, 0x80, 0x6c, 0x00, 0x08, 0x90, 0x6e,	// 0x218  D
	0x64, 0x08, 0x80, 0x6e, 0x00, 0x08, 0x90, 0x70,	// 0x220  E
	0x64, 0x08, 0x80, 0x70, 0x00, 0x08, 0x90, 0x71,	// 0x228  F
	0x64, 0x08, 0x80, 0x71, 0x00, 0x08, 0x90, 0x73,	// 0x230  G
	0x64, 0x08, 0x80, 0x73, 0x00, 0x08, 0x90, 0x75,	// 0x238  A
	0x64, 0x08, 0x80, 0x75, 0x00, 0x08, 0x90, 0x77,	// 0x240  B
	0x64, 0x08, 0x80, 0x77, 0x00, 0x08, 0x90, 0x78,	// 0x248  C
	0x64, 0x08, 0x80, 0x78, 0x00, 0x08, 0x90, 0x7a,	// 0x250  D
	0x64, 0x08, 0x80, 0x7a, 0x00, 0x08, 0x90, 0x7c,	// 0x258  E
	0x64, 0x08, 0x80, 0x7c, 0x00, 0x08, 0x90, 0x7d,	// 0x260  F
	0x64, 0x08, 0x80, 0x7d, 0x00, 0x08, 0x90, 0x7f,	// 0x268  G
	0x64, 0x08, 0x80, 0x7f, 0x00, 0x08, 0xff, 0x2f, // 0x270
	0x00 // 0x278
};

struct _midi_test midi_test[]= {
	{midi_test_c_scale, 663},
	{NULL, 0}
};

int midi_test_max = 1;

/*
 ==============================
 Audio Output Functions
 ------------------------------
 ==============================
*/

unsigned int rate = 32072;
char  *pcmname = NULL;

int (*send_output) (char * output_data, int output_size);
void (*close_output) ( void );
int audio_fd;

static void
shutdown_output ( void ) {
	printf("Shutting Down Sound System\r\n");
	if (audio_fd != -1)
		close(audio_fd);
}

/*
 Wav Output Functions
*/

static char wav_file[1024] = "\0";
unsigned long int wav_size;

static int write_wav_output (char * output_data, int output_size);
static void close_wav_output ( void );

static int
open_wav_output ( void ) {

	unsigned char wav_hdr[] = { 0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00,
				0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
				0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
				0x44, 0xAC, 0x00, 0x00, 0x10, 0xB1, 0x02, 0x00,
				0x04, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61,
				0x00, 0x00, 0x00, 0x00 };

	if (wav_file[0] == '\0')
		return -1;
#ifdef _WIN32
	if ((audio_fd = open(wav_file, (O_RDWR | O_CREAT | O_TRUNC | O_BINARY))) < 0) {
#else
	if ((audio_fd = open(wav_file, (O_RDWR | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))) < 0) {
#endif
		return -1;
	} else {
		unsigned long int bytes_per_sec;

		wav_hdr[24] = (rate) & 0xFF;
		wav_hdr[25] = (rate >> 8) & 0xFF;

		bytes_per_sec = rate * 4;
		wav_hdr[28] = (bytes_per_sec) & 0xFF;
		wav_hdr[29] = (bytes_per_sec >> 8) & 0xFF;
		wav_hdr[30] = (bytes_per_sec >> 16) & 0xFF;
		wav_hdr[31] = (bytes_per_sec >> 24) & 0xFF;
	}

	if (write(audio_fd, &wav_hdr, 44) < 0) {
		printf("ERROR: Writing Header %s\r\n", strerror(errno));
		shutdown_output();
		return -1;
	}

	wav_size = 0;
	send_output = write_wav_output;
	close_output = close_wav_output;
	return 0;
}

static int
write_wav_output (char * output_data, int output_size) {
	if (write(audio_fd, output_data, output_size) < 0) {
		printf("ERROR: Writing Wav %s\r\n", strerror(errno));
		shutdown_output();
		return -1;
	}

	wav_size += output_size;
	return 0;
}

static void
close_wav_output ( void ) {
	char wav_count[4];
	if (audio_fd == -1)
		return;

	wav_count[0] = (wav_size) & 0xFF;
	wav_count[1] = (wav_size >> 8) & 0xFF;
	wav_count[2] = (wav_size >> 16) & 0xFF;
	wav_count[3] = (wav_size >> 24) & 0xFF;
	lseek(audio_fd,40,SEEK_SET);
	if (write(audio_fd,&wav_count,4) < 0) {
        printf("ERROR: Writing Wav %s\r\n", strerror(errno));
		shutdown_output();
	}


	wav_size += 36;
	wav_count[0] = (wav_size) & 0xFF;
	wav_count[1] = (wav_size >> 8) & 0xFF;
	wav_count[2] = (wav_size >> 16) & 0xFF;
	wav_count[3] = (wav_size >> 24) & 0xFF;
	lseek(audio_fd,4,SEEK_SET);
    if (write(audio_fd,&wav_count,4) < 0) {
        printf("ERROR: Writing Wav %s\r\n", strerror(errno));
		shutdown_output();
	}

	shutdown_output();
}


#if (defined _WIN32) || (defined __CYGWIN__)

HWAVEOUT hWaveOut;
WAVEHDR header;
unsigned long int mm_buffer_count;
static CRITICAL_SECTION waveCriticalSection;

static int write_mm_output (char * output_data, int output_size);
static void close_mm_output ( void );

WAVEHDR *mm_blocks;
#define MM_BLOCK_SIZE 16384
#define MM_BLOCK_COUNT 3

unsigned long int mm_free_blocks = MM_BLOCK_COUNT;
unsigned long int mm_current_block = 0;

static void CALLBACK mmOutProc( HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 ) {
	int* freeBlockCounter = (int*)dwInstance;
	HWAVEOUT tmp_hWaveOut = hWaveOut;
	DWORD tmp_dwParam1 = dwParam1;
	DWORD tmp_dwParam2 = dwParam2;

    tmp_hWaveOut = hWaveOut;
    tmp_dwParam1 = dwParam2;
    tmp_dwParam2 = dwParam1;

	if(uMsg != WOM_DONE)
		return;
	EnterCriticalSection(&waveCriticalSection);
	 (*freeBlockCounter)++;
	LeaveCriticalSection(&waveCriticalSection);
}


static int
open_mm_output ( void ) {
	WAVEFORMATEX wfx;
	char *mm_buffer;
	int i;

	InitializeCriticalSection(&waveCriticalSection);

	if((mm_buffer = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, ((MM_BLOCK_SIZE + sizeof(WAVEHDR)) * MM_BLOCK_COUNT))) == NULL)  {
		printf("Memory allocation error\r\n");
		return -1;
	}

	mm_blocks = (WAVEHDR*)mm_buffer;
	mm_buffer += sizeof(WAVEHDR) * MM_BLOCK_COUNT;

	for(i = 0; i < MM_BLOCK_COUNT; i++) {
		mm_blocks[i].dwBufferLength = MM_BLOCK_SIZE;
		mm_blocks[i].lpData = mm_buffer;
		mm_buffer += MM_BLOCK_SIZE;
	}

	wfx.nSamplesPerSec = rate;
	wfx.wBitsPerSample = 16;
	wfx.nChannels = 2;
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nBlockAlign = (wfx.wBitsPerSample >> 3) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

	if(waveOutOpen( &hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)mmOutProc, (DWORD_PTR)&mm_free_blocks, CALLBACK_FUNCTION ) != MMSYSERR_NOERROR) {
		printf("unable to open WAVE_MAPPER device\r\n");
		return -1;
	 }


	send_output = write_mm_output;
	close_output = close_mm_output;
	return 0;
}

static int
write_mm_output (char * output_data, int output_size) {
	WAVEHDR* current;
	int free_size = 0;
	int data_read = 0;
	current = &mm_blocks[mm_current_block];

	while (output_size) {
		if(current->dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(hWaveOut, current, sizeof(WAVEHDR));
		free_size = MM_BLOCK_SIZE - current->dwUser;
		if (free_size > output_size)
			free_size = output_size;

		memcpy(current->lpData + current->dwUser, &output_data[data_read], free_size);
		current->dwUser += free_size;
		output_size -= free_size;
		data_read += free_size;

		if (current->dwUser < MM_BLOCK_SIZE) {
			return 0;
		}

		current->dwBufferLength = MM_BLOCK_SIZE;
		waveOutPrepareHeader(hWaveOut, current, sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, current, sizeof(WAVEHDR));
		EnterCriticalSection(&waveCriticalSection);
		mm_free_blocks--;
		LeaveCriticalSection(&waveCriticalSection);
		while(!mm_free_blocks)
			Sleep(10);
		mm_current_block++;
		mm_current_block %= MM_BLOCK_COUNT;
		current = &mm_blocks[mm_current_block];
		current->dwUser = 0;
	}
	return 0;
}

static void
close_mm_output ( void ) {
	WAVEHDR* current;
	int i, j;
	current = &mm_blocks[mm_current_block];
	i = MM_BLOCK_SIZE - current->dwUser;

	for (j = i; i; i--) write_mm_output (0, 0);

	waveOutClose (hWaveOut);
	HeapFree(GetProcessHeap(), 0, mm_blocks);
}

#else
#ifdef HAVE_ALSA

void *buffer;
int bps;
int alsa_first_time = 1;
static snd_pcm_t  *pcm;

static int write_alsa_output (char * output_data, int output_size);
static void close_alsa_output ( void );

static int
open_alsa_output(void) {
	snd_pcm_hw_params_t     *hw;
	snd_pcm_sw_params_t     *sw;
	int err;
	unsigned int alsa_buffer_time;
	unsigned int alsa_period_time;

	if (!pcmname) {
	    pcmname = malloc (8);
        strcpy(pcmname,"default\0");
	}

	if ((err = snd_pcm_open (&pcm, pcmname, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		printf("Error: audio open error: %s\r\n", snd_strerror (-err));
		return -1;
	}

	snd_pcm_hw_params_alloca (&hw);

	if ((err = snd_pcm_hw_params_any(pcm, hw)) < 0) {
		printf("ERROR: No configuration available for playback: %s\r\n", snd_strerror(-err));

		return -1;
	}

	if ((err = snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		printf("Cannot set mmap'ed mode: %s.\r\n", snd_strerror(-err));
		return -1;
	}

	if (snd_pcm_hw_params_set_format (pcm, hw, SND_PCM_FORMAT_S16_LE) < 0) {
		printf("ALSA does not support 16bit signed audio for your soundcard\r\n");
		close_alsa_output();
		return -1;
	}

	if (snd_pcm_hw_params_set_channels (pcm, hw, 2) < 0) {
		printf("ALSA does not support stereo for your soundcard\r\n");
		close_alsa_output();
		return -1;
	}

	if (snd_pcm_hw_params_set_rate_near(pcm, hw, &rate, 0) < 0) {
		printf("ALSA does not support %iHz for your soundcard\r\n",rate);
		close_alsa_output();
		return -1;
	}

	alsa_buffer_time = 500000;
	alsa_period_time = 50000;

	if ((err = snd_pcm_hw_params_set_buffer_time_near(pcm, hw, &alsa_buffer_time, 0)) < 0)
	{
		printf("Set buffer time failed: %s.\r\n", snd_strerror(-err));
		return -1;
	}

	if ((err = snd_pcm_hw_params_set_period_time_near(pcm, hw, &alsa_period_time, 0)) < 0)
	{
		printf("Set period time failed: %s.\r\n", snd_strerror(-err));
		return -1;
	}

	if (snd_pcm_hw_params(pcm, hw) < 0)
	{
		printf("Unable to install hw params\r\n");
		return -1;
	}

	snd_pcm_sw_params_alloca(&sw);
	snd_pcm_sw_params_current(pcm, sw);
	if (snd_pcm_sw_params(pcm, sw) < 0)
	{
		printf("Unable to install sw params\r\n");
		return -1;
	}

	send_output = write_alsa_output;
	close_output = close_alsa_output;
	if (pcmname != NULL) {
	    free (pcmname);
	}
	return 0;
}

static int
write_alsa_output (char * output_data, int output_size) {
	int err;
	snd_pcm_uframes_t frames;

	while (output_size > 0) {
		frames = snd_pcm_bytes_to_frames(pcm, output_size);
        if ((err = snd_pcm_writei(pcm, output_data, frames)) < 0) {
			if (snd_pcm_state(pcm) == SND_PCM_STATE_XRUN) {
				if ((err = snd_pcm_prepare(pcm)) < 0)
					printf("snd_pcm_prepare() failed.\r\n");
				alsa_first_time = 1;
				continue;
			}
			return err;
		}

		output_size -= snd_pcm_frames_to_bytes(pcm, err);
		output_data += snd_pcm_frames_to_bytes(pcm, err);
		if (alsa_first_time) {
			alsa_first_time = 0;
			snd_pcm_start(pcm);
		}
	}
	return 0;
}

static void
close_alsa_output ( void ) {
	snd_pcm_close (pcm);
}

#else
/*
 OSS Output Functions
 --------------------
 uses mmap'd audio
*/

char *buffer = NULL;
unsigned long int max_buffer;
unsigned long int buffer_delay;
unsigned long int counter;
struct audio_buf_info info;

static int write_oss_output (char * output_data, int output_size);
static void close_oss_output ( void );

static int
open_oss_output( void ) {
	int caps, rc, tmp;
	unsigned long int omode = O_RDWR;
	unsigned long int mmmode = PROT_WRITE | PROT_READ;
	unsigned long int mmflags = MAP_FILE | MAP_SHARED;
	unsigned long int sz = sysconf (_SC_PAGESIZE);

   	if (!pcmname) {
	    pcmname = malloc (8);
        strcpy(pcmname,"default\0");
	}

	if ((audio_fd = open(pcmname, omode)) < 0) {
		printf("ERROR: Unable to open /dev/dsp (%s)\r\n",strerror(errno));
		return -1;
	}
	if (ioctl (audio_fd, SNDCTL_DSP_RESET, 0) < 0) {
		printf("ERROR: Unable to reset /dev/dsp\r\n");
		shutdown_output();
		return -1;
	}
	if (ioctl (audio_fd, SNDCTL_DSP_GETCAPS, &caps) == -1) {
		printf("ERROR: Driver Too Old\r\n");
		shutdown_output();
		return -1;
	}
	if (!(caps & DSP_CAP_TRIGGER) || !(caps & DSP_CAP_MMAP)) {
		printf ("Sound device can't do memory-mapped I/O.\r\n");
		shutdown_output();
		return -1;
	}
	if (ioctl (audio_fd, SNDCTL_DSP_GETOSPACE, &info) == -1) {
		printf ("Um, can't do GETOSPACE?\r\n");
		shutdown_output();
		return -1;
	}
	max_buffer = (info.fragstotal * info.fragsize + sz - 1) & ~(sz - 1);

	rc = AFMT_S16_LE;
	if (ioctl (audio_fd, SNDCTL_DSP_SETFMT, &rc) < 0 ) {
		printf("Can't set 16bit\r\n");
		shutdown_output();;
		return -1;
	}

	tmp = 2;
	if (ioctl (audio_fd, SNDCTL_DSP_CHANNELS, &tmp) < 0) {
		printf("Can't set stereo\r\n");
		shutdown_output();
		return -1;
	}

	if (ioctl (audio_fd, SNDCTL_DSP_SPEED, &rate) < 0) {
		printf("ERROR: /dev/dsp does not support %iHz output\r\n",rate);
		shutdown_output();
		return -1;
	}

	buffer = (char *) mmap(NULL, max_buffer, mmmode, mmflags, audio_fd, 0);
	if (buffer == MAP_FAILED) {
		printf("couldn't mmap %s\r\n",strerror(errno));
		shutdown_output();
		return -1;
	}

	tmp = 0;
	if (ioctl (audio_fd, SNDCTL_DSP_SETTRIGGER, &tmp) < 0) {
		printf("Couldn't toggle\r\n");
		munmap (buffer, info.fragstotal * info.fragsize);
		shutdown_output();
		return -1;
	}

	tmp = PCM_ENABLE_OUTPUT;
	if (ioctl (audio_fd, SNDCTL_DSP_SETTRIGGER, &tmp) < 0) {
		printf("Couldn't toggle\r\n");
		munmap (buffer, info.fragstotal * info.fragsize);
		shutdown_output();
		return -1;
	}
	buffer_delay = 1000000 / (rate / 4);
	send_output = write_oss_output;
	close_output = close_oss_output;
	return 0;
}

static int
write_oss_output(char * output_data, int output_size) {
	struct count_info count;
	int data_read = 0;
	int free_size = 0;
	while (output_size != 0) {
		while (1) {
			if (ioctl (audio_fd, SNDCTL_DSP_GETOPTR, &count) == -1) {
				printf("Dead Sound\r\n");
				munmap (buffer, info.fragstotal * info.fragsize);
				shutdown_output();
				return -1;
			}
			if ((count.ptr < (long int)counter) || (count.ptr >= ((long int)counter+4))) {
				break;
			}
			usleep(500);
		}
		if (count.ptr < (long int)counter) {
			free_size = max_buffer - counter;
		} else {
			free_size = count.ptr - counter;
		}
		if (free_size > output_size)
			free_size = output_size;

		memcpy(&buffer[counter], &output_data[data_read], free_size);
		data_read += free_size;
		counter += free_size;
		if (counter >= max_buffer)
			counter = 0;
		output_size -= free_size;
	}
	return 0;
}

static void
close_oss_output(void) {
	shutdown_output();
	if (buffer != NULL)
		munmap (buffer, info.fragstotal * info.fragsize);
	audio_fd = -1;
}

#endif // HAVE_ALSA
#endif



static struct option const long_options[] = {
	{"version",0,0,'v'},
	{"help",0,0,'h'},
	{"rate",1,0,'r'},
	{"master_volume",1,0,'m'},
	{"config_file",1,0,'c'},
	{"wavout",1,0,'o'},
	{"log_vol",0,0,'l'},
	{"reverb",0,0,'b'},
	{"test_midi",0,0,'t'},
	{"test_bank",1,0,'k'},
	{"test_patch",1,0,'p'},
	{"enhanced_resample",0,0,'e'},
	{"auddev",1,0,'d'},
	{"wholetempo",0,0,'w'},
	{"roundtempo",0,0,'n'},
	{NULL,0,NULL,0}
};

static void
do_help (void) {
	printf("  -v    --version        Display version\r\n");
	printf("  -h    --help           This help.\r\n");
#ifndef _WIN32
	printf("  -d D  --device=D       Use device D for audio output instead\r\n");
	printf("                         of the default\r\n");
#endif
    printf("MIDI Options\r\n");
    printf("  -w    --wholetempo       round down tempo to whole number\r\n");
    printf("  -n    --roundtempo       round tempo to nearest whole number\r\n");
	printf("Software Wavetable Options\r\n");
	printf("  -o W  --wavout=W       Saves the output to W in wav format\r\n");
	printf("                         at 44100Hz 16 bit stereo\r\n");
	printf("  -l    --log_vol        Use log volume adjustments\r\n");
	printf("  -r N  --rate=N         output at N samples per second\r\n");
	printf("  -c P  --config_file=P  P is the path and filename to your timidity.cfg\r\n");
	printf("                         Defaults to /etc/timidity.cfg\n\r\n");
	printf(" -m V  --master_volume=V Sets the master volumes, default is 100\r\n");
	printf("                         range is 0-127 with 127 being the loudest\r\n");
    printf(" -b    --reverb          Enable final output reverb engine\r\n");

}

static void
do_version (void) {
	printf("\nWildMidi %s Open Source Midi Sequencer\r\n",PACKAGE_VERSION);
	printf("Copyright (C) Chris Ison 2001-2011 wildcode@users.sourceforge.net\n\r\n");
	printf("WildMidi comes with ABSOLUTELY NO WARRANTY\r\n");
	printf("This is free software, and you are welcome to redistribute it\r\n");
	printf("under the terms and conditions of the GNU General Public License version 3.\r\n");
	printf("For more information see COPYING\n\r\n");
	printf("Report bugs to %s\r\n",PACKAGE_BUGREPORT);
	printf("WildMIDI homepage at %s\n\r\n",PACKAGE_URL);
}

static void
do_syntax (void) {
	printf("wildmidi [options] filename.mid\n\r\n");
}

int
main (int argc, char **argv) {
	struct _WM_Info * wm_info = NULL;
	int i;
	int option_index = 0;
	unsigned long int mixer_options = 0;
	static char *config_file = NULL;
	void *midi_ptr =  NULL;
	unsigned char master_volume = 100;
	int output_result = 0;
	char * output_buffer = NULL;
	unsigned long int perc_play = 0;
	unsigned long int pro_mins = 0;
	unsigned long int pro_secs = 0;
	unsigned long int apr_mins = 0;
	unsigned long int apr_secs = 0;
	unsigned char modes[4] = {0};
	unsigned long int count_diff;
	unsigned char ch;
	unsigned char test_midi = 0;
	unsigned char test_count = 0;
	unsigned char *test_data = NULL;
	unsigned char test_bank = 0;
	unsigned char test_patch = 0;
	static char spinner[] ="|/-\\";
	static int spinpoint = 0;
	unsigned long int seek_to_sample = 0;
    int inpause = 0;

#ifndef _WIN32
	int my_tty;
	struct termios _tty;
	tcflag_t _res_oflg = _tty.c_oflag;
	tcflag_t _res_lflg = _tty.c_lflag;

#define raw() (_tty.c_lflag &= ~(ICANON | ICRNL | ISIG), \
		_tty.c_oflag &= ~ONLCR, tcsetattr(my_tty, TCSANOW, &_tty))
#define savetty() ((void) tcgetattr(my_tty, &_tty), \
		_res_oflg = _tty.c_oflag, _res_lflg = _tty.c_lflag)
#define resetty() (_tty.c_oflag = _res_oflg, _tty.c_lflag = _res_lflg,\
		(void) tcsetattr(my_tty, TCSADRAIN, &_tty))
#endif

	do_version();
	while (1) {
		i = getopt_long (argc, argv, "vho:lr:c:m:btk:p:ed:wn", long_options, &option_index);
		if (i == -1)
			break;
		switch (i) {
			case 'v': // Version
				return 0;
			case 'h': // help
				do_syntax();
				do_help();
				return 0;
			case 'r': // SoundCard Rate
				rate = atoi(optarg);
				break;
			case 'b': // Reverb
				mixer_options ^= WM_MO_REVERB;
				break;
			case 'm': // Master Volume
				master_volume = (unsigned char)atoi(optarg);
				break;
			case 'o': // Wav Output
				strcpy(wav_file,optarg);
				break;
			case 'c': // Config File
				config_file = malloc (strlen(optarg)+1);
				strcpy(config_file,optarg);
				break;
			case 'd': // Output device
				pcmname = malloc (strlen(optarg)+1);
				strcpy(pcmname,optarg);
				break;
			case 'e': // Enhanced Resampling
				mixer_options |= WM_MO_ENHANCED_RESAMPLING;
				break;
			case 'l': // log volume
				mixer_options |= WM_MO_LOG_VOLUME;
				break;
			case 't': // play test midis
				test_midi = 1;
				break;
			case 'k': // set test bank
				test_bank = (unsigned char)atoi(optarg);
				break;
			case 'p': // set test patch
				test_patch = (unsigned char)atoi(optarg);
				break;
            case 'w': // whole number tempo
                mixer_options |= WM_MO_WHOLETEMPO;
                break;
            case 'n': // whole number tempo
                mixer_options |= WM_MO_ROUNDTEMPO;
                break;
			default:
				printf ("Unknown Option -%o ??\r\n", i);
				return 0;
		}
	}

	if (!config_file) {
		config_file = malloc(sizeof(WILDMIDI_CFG)+1);
		strncpy (config_file, WILDMIDI_CFG, sizeof(WILDMIDI_CFG));
		config_file[sizeof(WILDMIDI_CFG)] = '\0';
	}
	if ((optind < argc) || (test_midi)) {
		printf("Initializing Sound System\r\n");

		if (wav_file[0] != '\0') {
			if (open_wav_output() == -1) {
				return 0;
			}
		} else {
#if (defined _WIN32) || (defined __CYGWIN__)
			if (open_mm_output() == -1) {
#else
#ifdef HAVE_ALSA
			if (open_alsa_output() == -1) {
#else
			if (open_oss_output() == -1) {
#endif
#endif
				return 0;
			}
		}
		printf("Initializing %s\n\r\n", WildMidi_GetString(WM_GS_VERSION));
		printf(" +  Volume up        e  Better resampling    n  Next Midi\r\n");
		printf(" -  Volume down      l  Log volume           q  Quit\r\n");
        printf(" ,  1sec Seek Back   r  Reverb               .  1sec Seek Forward\r\n");
        printf("                     p  Pause On/Off\n\r\n");

		if (WildMidi_Init (config_file, rate, mixer_options) == -1) {
			return 0;
		}
		WildMidi_MasterVolume(master_volume);

		output_buffer = malloc(16384);

		if (output_buffer == NULL) {
			printf("Not enough ram, exiting\r\n");
			WildMidi_Shutdown();
			return 0;
		}
#ifndef _WIN32
		my_tty = fileno(stdin);
		if (isatty(my_tty)) {
			savetty();
			raw();
			fcntl(0, F_SETFL, FNONBLOCK);
		}
#endif

		while ((optind < argc) || (test_midi)) {
			if (!test_midi) {
                char * real_file = strrchr(argv[optind], '/');
                if (real_file == NULL) {
                    real_file = strrchr(argv[optind], '\\');
                }

                printf ("Playing ");
                if (real_file != NULL) {
                    printf("%s \r\n", (real_file+1));
                } else {
                    printf("%s \r\n", argv[optind]);
                }

                midi_ptr = WildMidi_Open (argv[optind]);
                if (midi_ptr == NULL) {
                    optind++;
                    continue;
                }
                wm_info = WildMidi_GetInfo(midi_ptr);

                optind++;
			} else {
				if (test_count == midi_test_max) {
					break;
				}
				test_data = malloc(midi_test[test_count].size);
				memcpy(test_data, midi_test[test_count].data, midi_test[test_count].size);
				test_data[25] = test_bank;
				test_data[28] = test_patch;
				midi_ptr = WildMidi_OpenBuffer(test_data, 633);
				wm_info = WildMidi_GetInfo(midi_ptr);
				test_count++;
				printf ("Playing test midi no. %i\r\n", test_count);
			}

            apr_mins = wm_info->approx_total_samples / (rate * 60);
            apr_secs = (wm_info->approx_total_samples % (rate * 60)) / rate;

            if (midi_ptr == NULL) {
                fprintf(stderr,"Skipping %s\r\n",argv[optind]);
                optind++;
                continue;
            }
			mixer_options = wm_info->mixer_options;
			fprintf(stderr, "\r");

			while (1) {
                count_diff = wm_info->approx_total_samples - wm_info->current_sample;

				if (count_diff == 0)
					break;

				ch = 0;
#ifdef _WIN32
				if (_kbhit()) {
					ch = _getch();
					putch(ch);
				}
#else
				if (read(my_tty, &ch, 1) != 1)
					ch = 0;
#endif
				if (ch) {
					switch (ch) {
						case 'l':
							WildMidi_SetOption(midi_ptr, WM_MO_LOG_VOLUME, ((mixer_options & WM_MO_LOG_VOLUME) ^ WM_MO_LOG_VOLUME));
							mixer_options ^= WM_MO_LOG_VOLUME;
							break;
						case 'r':
							WildMidi_SetOption(midi_ptr, WM_MO_REVERB, ((mixer_options & WM_MO_REVERB) ^ WM_MO_REVERB));
							mixer_options ^= WM_MO_REVERB;
							break;
						case 'e':
							WildMidi_SetOption(midi_ptr, WM_MO_ENHANCED_RESAMPLING, ((mixer_options & WM_MO_ENHANCED_RESAMPLING) ^ WM_MO_ENHANCED_RESAMPLING));
							mixer_options ^= WM_MO_ENHANCED_RESAMPLING;
							break;
						case 'n':
							goto NEXTMIDI;
						case 'p':
                            if (inpause) {
                                inpause = 0;
                                fprintf(stderr, "       \r");
                            } else {
                                inpause = 1;
                                fprintf(stderr, "Paused \r");
                                continue;
                            }
                            break;
						case 'q':
							printf("\r\n");
							WildMidi_Close(midi_ptr);
							WildMidi_Shutdown();
							printf("Shutting down Sound System\r\n");
							close_output();
#ifndef _WIN32
							if (isatty(my_tty))
								resetty();
#endif
							printf("\r\n");
							exit (0);
						case '-':
							if (master_volume > 0) {
								master_volume--;
								WildMidi_MasterVolume(master_volume);
							}
							break;
						case '+':
							if (master_volume < 127) {
								master_volume++;
								WildMidi_MasterVolume(master_volume);
							}
							break;
                        // seeking examples
                        case ',': // fast seek backwards
                            if (wm_info->current_sample < rate) {
                                seek_to_sample = 0;
                            } else {
                                seek_to_sample = wm_info->current_sample - rate;
                            }
                            WildMidi_FastSeek(midi_ptr, &seek_to_sample);
                            break;
                        case '.': // fast seek forwards
                            if ((wm_info->approx_total_samples - wm_info->current_sample) < rate) {
                                seek_to_sample = wm_info->approx_total_samples;
                            } else {
                                seek_to_sample = wm_info->current_sample + rate;
                            }
                            WildMidi_FastSeek(midi_ptr, &seek_to_sample);
                            break;
                        default:
                            break;
					}
				}

				if (inpause) {
                    wm_info = WildMidi_GetInfo(midi_ptr);
                    perc_play =  (wm_info->current_sample * 100) / wm_info->approx_total_samples;
                    pro_mins = wm_info->current_sample / (rate * 60);
                    pro_secs = (wm_info->current_sample % (rate * 60)) / rate;
                    {
                        int mode_count = 0;
                        if (mixer_options & WM_MO_LOG_VOLUME) {
                            modes[mode_count++] = 'l';
                        }
                        if (mixer_options & WM_MO_REVERB) {
                            modes[mode_count++] = 'r';
                        }
                        if (mixer_options & WM_MO_ENHANCED_RESAMPLING) {
                            modes[mode_count++] = 'e';
                        }
                        if (mode_count !=3) {
                            do {
                                modes[mode_count++] = ' ';
                            } while (mode_count != 3);
                        }
                        modes[3] = '\0';
                    }
                    fprintf(stderr, "        [Approx %2lum %2lus Total] [%s] [%3i] [%2lum %2lus Processed] [%2lu%%] 0  \r",
                        apr_mins, apr_secs, modes, master_volume,
                        pro_mins, pro_secs, perc_play);

#ifdef _WIN32
                    Sleep(5);
#else
                    usleep(5000);
#endif
                    continue;
				}

				if (count_diff < 4096) {
					output_result = WildMidi_GetOutput (midi_ptr, output_buffer, (count_diff * 4));
				} else {
					output_result = WildMidi_GetOutput (midi_ptr, output_buffer, 4096);
				}

                if (output_result <= 0)
                    break;

				wm_info = WildMidi_GetInfo(midi_ptr);
                perc_play =  (wm_info->current_sample * 100) / wm_info->approx_total_samples;
                pro_mins = wm_info->current_sample / (rate * 60);
                pro_secs = (wm_info->current_sample % (rate * 60)) / rate;
				{
					int mode_count = 0;
					if (mixer_options & WM_MO_LOG_VOLUME) {
						modes[mode_count++] = 'l';
					}
					if (mixer_options & WM_MO_REVERB) {
						modes[mode_count++] = 'r';
					}
					if (mixer_options & WM_MO_ENHANCED_RESAMPLING) {
						modes[mode_count++] = 'e';
					}
					if (mode_count !=3) {
						do {
							modes[mode_count++] = ' ';
						} while (mode_count != 3);
					}
					modes[3] = '\0';
				}
				fprintf(stderr, "        [Approx %2lum %2lus Total] [%s] [%3i] [%2lum %2lus Processed] [%2lu%%] %c  \r",
					apr_mins, apr_secs, modes, master_volume,
					pro_mins, pro_secs, perc_play, spinner[spinpoint++%4]);

				if (output_result > 0)
                    send_output (output_buffer, output_result);
			}
NEXTMIDI:
			fprintf(stderr, "\r\n");
			if (WildMidi_Close(midi_ptr) == -1) {
				printf ("oops\r\n");
			}
			memset(output_buffer, 0, 16384);
			send_output (output_buffer, 16384);
		}
		memset(output_buffer, 0, 16384);
		send_output (output_buffer, 16384);
		send_output (output_buffer, 16384);
#ifdef _WIN32
		Sleep(5);
#else
		usleep(5000);
#endif
		if (WildMidi_Shutdown() == -1)

			printf("oops\r\n");
		printf("Shutting down Sound System\r\n");
		close_output();
#ifndef _WIN32
		if (isatty(my_tty))
			resetty();
#endif
	} else {
		printf("ERROR: No midi file given\r\n");
		do_syntax();
		return 0;
	}

	if (output_buffer != NULL)
		free(output_buffer);
	printf("\r");
	return 0;
}

