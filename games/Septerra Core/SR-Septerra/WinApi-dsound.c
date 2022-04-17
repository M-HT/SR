/**
 *
 *  Copyright (C) 2019-2022 Roman Pauer
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

#include "WinApi-dsound.h"
#include <inttypes.h>
#include <SDL.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Game-Config.h"


#define make_hresult(s,f,c) ((uint32_t)(((unsigned long)(s)<<31)|((unsigned long)(f)<<16)|((unsigned long)(c))))

#define E_NOTIMPL ((uint32_t)0x80004001L)
#define E_NOINTERFACE ((uint32_t)0x80004002L)
#define E_POINTER ((uint32_t)0x80004003L)
#define E_OUTOFMEMORY ((uint32_t)0x8007000EL)
#define E_INVALIDARG ((uint32_t)0x80070057L)

#define DS_OK 0

#define _FACDS  0x878
#define MAKE_DSHRESULT(code)  make_hresult(1, _FACDS, code)

#define DSERR_OUTOFMEMORY               E_OUTOFMEMORY
#define DSERR_UNSUPPORTED               E_NOTIMPL
#define DSERR_INVALIDPARAM              E_INVALIDARG
#define DSERR_BADFORMAT                 MAKE_DSHRESULT(100)
#define DSERR_NODRIVER                  MAKE_DSHRESULT(120)

#define DSSCL_NORMAL                0x00000001
#define DSSCL_PRIORITY              0x00000002
#define DSSCL_EXCLUSIVE             0x00000003
#define DSSCL_WRITEPRIMARY          0x00000004

#define DSBCAPS_PRIMARYBUFFER       0x00000001
#define DSBCAPS_STATIC              0x00000002
#define DSBCAPS_LOCHARDWARE         0x00000004
#define DSBCAPS_LOCSOFTWARE         0x00000008
#define DSBCAPS_CTRL3D              0x00000010
#define DSBCAPS_CTRLFREQUENCY       0x00000020
#define DSBCAPS_CTRLPAN             0x00000040
#define DSBCAPS_CTRLVOLUME          0x00000080
#define DSBCAPS_CTRLPOSITIONNOTIFY  0x00000100
#define DSBCAPS_CTRLFX              0x00000200
#define DSBCAPS_STICKYFOCUS         0x00004000
#define DSBCAPS_GLOBALFOCUS         0x00008000
#define DSBCAPS_GETCURRENTPOSITION2 0x00010000
#define DSBCAPS_MUTE3DATMAXDISTANCE 0x00020000
#define DSBCAPS_LOCDEFER            0x00040000

#define DSCAPS_PRIMARYMONO          0x00000001
#define DSCAPS_PRIMARYSTEREO        0x00000002
#define DSCAPS_PRIMARY8BIT          0x00000004
#define DSCAPS_PRIMARY16BIT         0x00000008
#define DSCAPS_CONTINUOUSRATE       0x00000010
#define DSCAPS_EMULDRIVER           0x00000020
#define DSCAPS_CERTIFIED            0x00000040
#define DSCAPS_SECONDARYMONO        0x00000100
#define DSCAPS_SECONDARYSTEREO      0x00000200
#define DSCAPS_SECONDARY8BIT        0x00000400
#define DSCAPS_SECONDARY16BIT       0x00000800

#define WAVE_FORMAT_PCM 1
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

#define DSBPLAY_LOOPING             0x00000001

#define DSBLOCK_FROMWRITECURSOR     0x00000001
#define DSBLOCK_ENTIREBUFFER        0x00000002


struct IDirectSound_c {
    PTR32(void *) lpVtbl;
    uint32_t RefCount;
    struct IDirectSoundBuffer_c *PrimaryBuffer;
};

struct IDirectSoundBuffer_c {
    PTR32(void *) lpVtbl;
    uint32_t RefCount;
    struct IDirectSound_c *DirectSound;
    uint8_t primary, status, looping, sample_size_shift;
    int freq;
    uint16_t format;
    uint8_t channels;
    uint8_t silence;
    int32_t attenuation, pan;
    unsigned int left_volume, right_volume;
    union {
        struct {
            struct IDirectSoundBuffer_c *first;
            unsigned int num_channels;
            int16_t *conv_data;
            int32_t *accum_data;
#if SDL_VERSION_ATLEAST(2,0,0)
            SDL_AudioDeviceID device_id;
#endif
        };
        struct {
            struct IDirectSoundBuffer_c *next;
            struct IDirectSoundBuffer_c *prev;
            uint8_t *data;
            uint32_t size, read_offset, write_offset;
            int16_t last_conv_sample[2];
        };
    };
};


#pragma pack(2)
typedef struct __attribute__ ((__packed__)) twaveformatex {
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
    uint16_t cbSize;
} waveformatex,*pwaveforamtex,*lpwaveformatex;

#pragma pack()

typedef struct _dsbufferdesc {
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwBufferBytes;
    uint32_t dwReserved;
    PTR32(lpwaveformatex) lpwfxFormat;
} dsbufferdesc;


extern uint32_t IDirectSoundVtbl_asm2c;
extern uint32_t IDirectSoundBufferVtbl_asm2c;


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


static void StopPrimaryBuffer(struct IDirectSoundBuffer_c *lpThis);


static int same_format(struct IDirectSoundBuffer_c *buf1, struct IDirectSoundBuffer_c *buf2)
{
    if ((buf1->freq == buf2->freq) &&
        (buf1->format == buf2->format) &&
        (buf1->channels == buf2->channels)
       )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int calc_remaining(struct IDirectSoundBuffer_c *buf, int len, int *rem2)
{
    unsigned int read_offset, write_offset;
    int remaining1, remaining2;

    read_offset = buf->read_offset;
    write_offset = buf->write_offset;
    if (read_offset < write_offset)
    {
        remaining1 = write_offset - read_offset;
        if (remaining1 > len) remaining1 = len;
        remaining2 = 0;
    }
    else
    {
        remaining1 = buf->size - read_offset;
        if (remaining1 >= len)
        {
            remaining1 = len;
            remaining2 = 0;
        }
        else
        {
            if (buf->looping)
            {
                remaining2 = write_offset;
                if (remaining2 > len - remaining1) remaining2 = len - remaining1;
            }
            else
            {
                remaining2 = 0;
            }
        }
    }

    *rem2 = remaining2;
    return remaining1;
}

// only call from callback or with locked audio
static void remove_from_list(struct IDirectSoundBuffer_c *PrimaryBuffer, struct IDirectSoundBuffer_c *buf, int stop)
{
    if (PrimaryBuffer->first == buf)
    {
        PrimaryBuffer->first = (PrimaryBuffer->num_channels == 1)?NULL:buf->next;
    }
    PrimaryBuffer->num_channels--;

    buf->prev->next = buf->next;
    buf->next->prev = buf->prev;

    buf->next = NULL;
    buf->prev = NULL;

    if (stop)
    {
        buf->status = SDL_AUDIO_STOPPED;
        buf->read_offset = 0;
        buf->write_offset = 0;
        buf->last_conv_sample[0] = 0;
        buf->last_conv_sample[1] = 0;
    }
    else
    {
        buf->status = SDL_AUDIO_PAUSED;
    }
}

#include "WinApi-dsound-conv.h"

// callback procedure for filling audio data
static void fill_audio(void *udata, Uint8 *stream, int len)
{
    struct IDirectSoundBuffer_c *PrimaryBuffer, *buffer1, *current, *next;
    int num_playing, remaining1, remaining2, num_samples, remaining_samples;
    int32_t *cur_dst;

    PrimaryBuffer = (struct IDirectSoundBuffer_c *)udata;

    buffer1 = NULL;
    num_playing = 0;

    if (PrimaryBuffer->first != NULL)
    {
        current = PrimaryBuffer->first;
        do {
            // read_offset == write_offset -> full buffer
            //if (current->read_offset != current->write_offset)
            {
                num_playing++;
                if (num_playing == 1)
                {
                    buffer1 = current;
                }
            }
            current = current->next;
        } while (current != PrimaryBuffer->first);
    }


    if (num_playing == 0)
    {
        // silence
        SDL_memset(stream, PrimaryBuffer->silence, len);
        return;
    }

    if (num_playing == 1)
    {
        if (same_format(PrimaryBuffer, buffer1) &&
            (PrimaryBuffer->attenuation == 0) &&
            (buffer1->attenuation == 0) &&
            (PrimaryBuffer->pan == 0) &&
            (buffer1->pan == 0)
           )
        {
            remaining1 = calc_remaining(buffer1, len, &remaining2);

            SDL_memcpy(stream, &(buffer1->data[buffer1->read_offset]), remaining1);
            stream = (Uint8 *)(remaining1 + (uintptr_t)stream);
            buffer1->read_offset += remaining1;
            if (buffer1->read_offset == buffer1->size)
            {
                if (buffer1->looping)
                {
                    buffer1->read_offset = 0;
                }
                else
                {
                    remove_from_list(PrimaryBuffer, buffer1, 1);
                }
            }
            len -= remaining1;
            if (len == 0) return;

            if (remaining2 != 0)
            {
                SDL_memcpy(stream, &(buffer1->data[buffer1->read_offset]), remaining2);
                stream = (Uint8 *)(remaining2 + (uintptr_t)stream);
                buffer1->read_offset += remaining2;
                len -= remaining2;
                if (len == 0) return;
            }

            // silence
            SDL_memset(stream, PrimaryBuffer->silence, len);
            return;
        }

        // do full processing
    }


    num_samples = len >> PrimaryBuffer->sample_size_shift;
    // clear accumulator data
    SDL_memset(PrimaryBuffer->accum_data, 0, num_samples * 2 * sizeof(int32_t));

    current = PrimaryBuffer->first;
    do {
        next = current->next;
        //remaining1 = calc_remaining(current, num_samples << current->sample_size_shift, &remaining2);
        remaining1 = calc_remaining(current, current->size, &remaining2);

        cur_dst = PrimaryBuffer->accum_data;
        remaining_samples = num_samples;

        while ((remaining_samples != 0) && (remaining1 != 0))
        {
            int16_t *cur_src;
            int cur_num, freq_diff_shift;
            int left_volume, right_volume;

            if (current->freq == PrimaryBuffer->freq)
            {
                if (remaining1 >= (remaining_samples << current->sample_size_shift))
                {
                    remaining1 = (remaining_samples << current->sample_size_shift);
                    remaining2 = 0;
                }

                cur_num = remaining1 >> current->sample_size_shift;

                if ((current->channels == 2) && (current->format == AUDIO_S16SYS))
                {
                    cur_src = (int16_t *) &(current->data[current->read_offset]);
                }
                else
                {
                    cur_src = PrimaryBuffer->conv_data;

                    switch (current->format)
                    {
                    case AUDIO_U8:
                        if (current->channels == 1)
                            conv_u8_mono((uint8_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        else
                            conv_u8_stereo((uint8_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        break;
                    case AUDIO_S8:
                        if (current->channels == 1)
                            conv_s8_mono((int8_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        else
                            conv_s8_stereo((int8_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        break;
                    case AUDIO_U16LSB:
                        #if AUDIO_U16LSB == AUDIO_U16SYS
                            if (current->channels == 1)
                                conv_u16_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                            else
                                conv_u16_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #else
                            if (current->channels == 1)
                                conv_u16swap_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                            else
                                conv_u16swap_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #endif
                        break;
                    case AUDIO_S16LSB:
                        #if AUDIO_S16LSB == AUDIO_S16SYS
                            if (current->channels == 1)
                                conv_s16_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                            else
                                conv_s16_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #else
                            if (current->channels == 1)
                                conv_s16swap_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                            else
                                conv_s16swap_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #endif
                        break;
                    case AUDIO_U16MSB:
                        #if AUDIO_U16MSB == AUDIO_U16SYS
                            if (current->channels == 1)
                                conv_u16_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                            else
                                conv_u16_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #else
                            if (current->channels == 1)
                                conv_u16swap_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                            else
                                conv_u16swap_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #endif
                        break;
                    case AUDIO_S16MSB:
                        #if AUDIO_S16MSB == AUDIO_S16SYS
                            if (current->channels == 1)
                                conv_s16_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                            else
                                conv_s16_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #else
                            if (current->channels == 1)
                                conv_s16swap_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                            else
                                conv_s16swap_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #endif
                        break;
                    }

                }
            }
            else if ((current->freq == 2 * PrimaryBuffer->freq) && (current->channels == 1))
            {
                // use mono buffer with double frequency as stereo channel at normal frequency

                if (remaining1 >= (remaining_samples << (current->sample_size_shift + 1)))
                {
                    remaining1 = (remaining_samples << (current->sample_size_shift + 1));
                    remaining2 = 0;
                }

                cur_num = remaining1 >> (current->sample_size_shift + 1);

                if (current->format == AUDIO_S16SYS)
                {
                    cur_src = (int16_t *) &(current->data[current->read_offset]);
                }
                else
                {
                    cur_src = PrimaryBuffer->conv_data;

                    switch (current->format)
                    {
                    case AUDIO_U8:
                        conv_u8_stereo((uint8_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        break;
                    case AUDIO_S8:
                        conv_s8_stereo((int8_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        break;
                    case AUDIO_U16LSB:
                        #if AUDIO_U16LSB == AUDIO_U16SYS
                            conv_u16_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #else
                            conv_u16swap_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #endif
                        break;
                    case AUDIO_S16LSB:
                        #if AUDIO_S16LSB == AUDIO_S16SYS
                            conv_s16_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #else
                            conv_s16swap_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #endif
                        break;
                    case AUDIO_U16MSB:
                        #if AUDIO_U16MSB == AUDIO_U16SYS
                            conv_u16_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #else
                            conv_u16swap_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #endif
                        break;
                    case AUDIO_S16MSB:
                        #if AUDIO_S16MSB == AUDIO_S16SYS
                            conv_s16_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #else
                            conv_s16swap_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num);
                        #endif
                        break;
                    }

                }
            }
            else if (current->freq > PrimaryBuffer->freq)
            {
                freq_diff_shift = 1;
                while (current->freq > (PrimaryBuffer->freq << freq_diff_shift))
                {
                    freq_diff_shift++;
                }


                if (remaining1 >= (remaining_samples << (current->sample_size_shift + freq_diff_shift)))
                {
                    remaining1 = remaining_samples << (current->sample_size_shift + freq_diff_shift);
                    remaining2 = 0;
                }

                cur_num = remaining1 >> (current->sample_size_shift + freq_diff_shift);

                cur_src = PrimaryBuffer->conv_data;

                switch (current->format)
                {
                case AUDIO_U8:
                    if (current->channels == 1)
                        downrate_u8_mono((uint8_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    else
                        downrate_u8_stereo((uint8_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    break;
                case AUDIO_S8:
                    if (current->channels == 1)
                        downrate_s8_mono((int8_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    else
                        downrate_s8_stereo((int8_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    break;
                case AUDIO_U16LSB:
                    #if AUDIO_U16LSB == AUDIO_U16SYS
                        if (current->channels == 1)
                            downrate_u16_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                        else
                            downrate_u16_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    #else
                        if (current->channels == 1)
                            downrate_u16swap_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                        else
                            downrate_u16swap_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    #endif
                    break;
                case AUDIO_S16LSB:
                    #if AUDIO_S16LSB == AUDIO_S16SYS
                        if (current->channels == 1)
                            downrate_s16_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                        else
                            downrate_s16_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    #else
                        if (current->channels == 1)
                            downrate_s16swap_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                        else
                            downrate_s16swap_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    #endif
                    break;
                case AUDIO_U16MSB:
                    #if AUDIO_U16MSB == AUDIO_U16SYS
                        if (current->channels == 1)
                            downrate_u16_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                        else
                            downrate_u16_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    #else
                        if (current->channels == 1)
                            downrate_u16swap_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                        else
                            downrate_u16swap_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    #endif
                    break;
                case AUDIO_S16MSB:
                    #if AUDIO_S16MSB == AUDIO_S16SYS
                        if (current->channels == 1)
                            downrate_s16_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                        else
                            downrate_s16_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    #else
                        if (current->channels == 1)
                            downrate_s16swap_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                        else
                            downrate_s16swap_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift);
                    #endif
                    break;
                }

            }
            else
            {
                freq_diff_shift = 1;
                while ((current->freq << freq_diff_shift) < PrimaryBuffer->freq)
                {
                    freq_diff_shift++;
                }


                if (remaining1 >= ((remaining_samples << current->sample_size_shift) >> freq_diff_shift))
                {
                    remaining1 = (remaining_samples << current->sample_size_shift) >> freq_diff_shift;
                    remaining2 = 0;
                }

                cur_num = (remaining1 >> current->sample_size_shift) << freq_diff_shift;

                cur_src = PrimaryBuffer->conv_data;

                switch (current->format)
                {
                case AUDIO_U8:
                    if (current->channels == 1)
                        uprate_u8_mono((uint8_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    else
                        uprate_u8_stereo((uint8_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    break;
                case AUDIO_S8:
                    if (current->channels == 1)
                        uprate_s8_mono((int8_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    else
                        uprate_s8_stereo((int8_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    break;
                case AUDIO_U16LSB:
                    #if AUDIO_U16LSB == AUDIO_U16SYS
                        if (current->channels == 1)
                            uprate_u16_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                        else
                            uprate_u16_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    #else
                        if (current->channels == 1)
                            uprate_u16swap_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                        else
                            uprate_u16swap_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    #endif
                    break;
                case AUDIO_S16LSB:
                    #if AUDIO_S16LSB == AUDIO_S16SYS
                        if (current->channels == 1)
                            uprate_s16_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                        else
                            uprate_s16_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    #else
                        if (current->channels == 1)
                            uprate_s16swap_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                        else
                            uprate_s16swap_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    #endif
                    break;
                case AUDIO_U16MSB:
                    #if AUDIO_U16MSB == AUDIO_U16SYS
                        if (current->channels == 1)
                            uprate_u16_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                        else
                            uprate_u16_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    #else
                        if (current->channels == 1)
                            uprate_u16swap_mono((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                        else
                            uprate_u16swap_stereo((uint16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    #endif
                    break;
                case AUDIO_S16MSB:
                    #if AUDIO_S16MSB == AUDIO_S16SYS
                        if (current->channels == 1)
                            uprate_s16_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                        else
                            uprate_s16_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    #else
                        if (current->channels == 1)
                            uprate_s16swap_mono((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                        else
                            uprate_s16swap_stereo((int16_t *) &(current->data[current->read_offset]), cur_src, cur_num, freq_diff_shift, &(current->last_conv_sample[0]));
                    #endif
                    break;
                }

            }

            current->read_offset += remaining1;
            if (current->read_offset == current->size)
            {
                if (current->looping)
                {
                    current->read_offset = 0;
                }
                else
                {
                    remaining2 = 0;
                    remove_from_list(PrimaryBuffer, current, 1);
                }
            }

            // add converted data to accumulator data
            left_volume = (PrimaryBuffer->left_volume == 0x10000)?current->left_volume:((PrimaryBuffer->left_volume * current->left_volume) >> 16);
            right_volume = (PrimaryBuffer->right_volume == 0x10000)?current->right_volume:((PrimaryBuffer->right_volume * current->right_volume) >> 16);

            remaining_samples -= cur_num;

            if ((left_volume == 0x10000) && (right_volume == 0x10000))
            {
                for (; cur_num != 0; cur_num--)
                {
                    cur_dst[0] += cur_src[0];
                    cur_dst[1] += cur_src[1];
                    cur_dst += 2;
                    cur_src += 2;
                }
            }
            else
            {
                for (; cur_num != 0; cur_num--)
                {
                    cur_dst[0] += (left_volume * (int)cur_src[0]) >> 16;
                    cur_dst[1] += (right_volume * (int)cur_src[1]) >> 16;
                    cur_dst += 2;
                    cur_src += 2;
                }
            }

            remaining1 = remaining2;
            remaining2 = 0;
        }

        current = next;
    } while (current != PrimaryBuffer->first);


    switch (PrimaryBuffer->format)
    {
    case AUDIO_U8:
        if (PrimaryBuffer->channels == 1)
            accum_2_u8_mono(PrimaryBuffer->accum_data, (uint8_t *)stream, num_samples);
        else
            accum_2_u8_stereo(PrimaryBuffer->accum_data, (uint8_t *)stream, num_samples);
        break;
    case AUDIO_S8:
        if (PrimaryBuffer->channels == 1)
            accum_2_s8_mono(PrimaryBuffer->accum_data, (int8_t *)stream, num_samples);
        else
            accum_2_s8_stereo(PrimaryBuffer->accum_data, (int8_t *)stream, num_samples);
        break;
    case AUDIO_U16LSB:
        #if AUDIO_U16LSB == AUDIO_U16SYS
            if (PrimaryBuffer->channels == 1)
                accum_2_u16_mono(PrimaryBuffer->accum_data, (uint16_t *)stream, num_samples);
            else
                accum_2_u16_stereo(PrimaryBuffer->accum_data, (uint16_t *)stream, num_samples);
        #else
            if (PrimaryBuffer->channels == 1)
                accum_2_u16swap_mono(PrimaryBuffer->accum_data, (uint16_t *)stream, num_samples);
            else
                accum_2_u16swap_stereo(PrimaryBuffer->accum_data, (uint16_t *)stream, num_samples);
        #endif
        break;
    case AUDIO_S16LSB:
        #if AUDIO_S16LSB == AUDIO_S16SYS
            if (PrimaryBuffer->channels == 1)
                accum_2_s16_mono(PrimaryBuffer->accum_data, (int16_t *)stream, num_samples);
            else
                accum_2_s16_stereo(PrimaryBuffer->accum_data, (int16_t *)stream, num_samples);
        #else
            if (PrimaryBuffer->channels == 1)
                accum_2_s16swap_mono(PrimaryBuffer->accum_data, (int16_t *)stream, num_samples);
            else
                accum_2_s16swap_stereo(PrimaryBuffer->accum_data, (int16_t *)stream, num_samples);
        #endif
        break;
    case AUDIO_U16MSB:
        #if AUDIO_U16MSB == AUDIO_U16SYS
            if (PrimaryBuffer->channels == 1)
                accum_2_u16_mono(PrimaryBuffer->accum_data, (uint16_t *)stream, num_samples);
            else
                accum_2_u16_stereo(PrimaryBuffer->accum_data, (uint16_t *)stream, num_samples);
        #else
            if (PrimaryBuffer->channels == 1)
                accum_2_u16swap_mono(PrimaryBuffer->accum_data, (uint16_t *)stream, num_samples);
            else
                accum_2_u16swap_stereo(PrimaryBuffer->accum_data, (uint16_t *)stream, num_samples);
        #endif
        break;
    case AUDIO_S16MSB:
        #if AUDIO_S16MSB == AUDIO_S16SYS
            if (PrimaryBuffer->channels == 1)
                accum_2_s16_mono(PrimaryBuffer->accum_data, (int16_t *)stream, num_samples);
            else
                accum_2_s16_stereo(PrimaryBuffer->accum_data, (int16_t *)stream, num_samples);
        #else
            if (PrimaryBuffer->channels == 1)
                accum_2_s16swap_mono(PrimaryBuffer->accum_data, (int16_t *)stream, num_samples);
            else
                accum_2_s16swap_stereo(PrimaryBuffer->accum_data, (int16_t *)stream, num_samples);
        #endif
        break;
    }
}


uint32_t DirectSoundCreate_c(void *lpGuid, PTR32(struct IDirectSound_c *) *ppDS, void *pUnkOuter)
{
    struct IDirectSound_c *lpDS_c;

#ifdef DEBUG_DSOUND
    eprintf("DirectSoundCreate: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t)lpGuid, (uintptr_t)ppDS, (uintptr_t)pUnkOuter);
#endif

    if ((lpGuid != NULL) || (pUnkOuter != NULL))
    {
        return DSERR_INVALIDPARAM;
    }

    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
        eprintf("Error: SDL_InitSubSystem: Audio\n");
        exit(1);
    }

    lpDS_c = (struct IDirectSound_c *)malloc(sizeof(struct IDirectSound_c));

    if (lpDS_c == NULL)
    {
        return DSERR_OUTOFMEMORY;
    }

    lpDS_c->lpVtbl = FROMPTR(&IDirectSoundVtbl_asm2c);
    lpDS_c->RefCount = 1;
    lpDS_c->PrimaryBuffer = NULL;

    *ppDS = FROMPTR(lpDS_c);

#ifdef DEBUG_DSOUND
    eprintf("OK: 0x%" PRIxPTR "\n", (uintptr_t)lpDS_c);
#endif

    return DS_OK;
}


uint32_t IDirectSound_QueryInterface_c(struct IDirectSound_c *lpThis, void * riid, PTR32(void *)* ppvObj)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSound_QueryInterface: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR "\n", (uintptr_t)lpThis, (uintptr_t)riid, (uintptr_t)ppvObj);
#endif

    if (ppvObj == NULL) return E_POINTER;
    return E_NOINTERFACE;
}

uint32_t IDirectSound_AddRef_c(struct IDirectSound_c *lpThis)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSound_AddRef: 0x%" PRIxPTR " - ", (uintptr_t)lpThis);
#endif

    if (lpThis != NULL)
    {
        lpThis->RefCount++;
#ifdef DEBUG_DSOUND
        eprintf("%i\n", lpThis->RefCount);
#endif
        return lpThis->RefCount;
    }

#ifdef DEBUG_DSOUND
    eprintf("%i\n", 0);
#endif

    return 0;
}

uint32_t IDirectSound_Release_c(struct IDirectSound_c *lpThis)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSound_Release: 0x%" PRIxPTR " - ", (uintptr_t)lpThis);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DSOUND
        eprintf("%i\n", 0);
#endif
        return 0;
    }

    lpThis->RefCount--;
#ifdef DEBUG_DSOUND
    eprintf("%i\n", lpThis->RefCount);
#endif
    if (lpThis->RefCount == 0)
    {
        if (lpThis->PrimaryBuffer != NULL)
        {
            StopPrimaryBuffer(lpThis->PrimaryBuffer);
            lpThis->PrimaryBuffer = NULL;
        }
        free(lpThis);
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return 0;
    }
    return lpThis->RefCount;
}

uint32_t IDirectSound_CreateSoundBuffer_c(struct IDirectSound_c *lpThis, const struct _dsbufferdesc * pcDSBufferDesc, PTR32(struct IDirectSoundBuffer_c *)* ppDSBuffer, void * pUnkOuter)
{
    struct IDirectSoundBuffer_c *lpDSB_c;
    SDL_AudioSpec desired, obtained;

#ifdef DEBUG_DSOUND
    eprintf("IDirectSound_CreateSoundBuffer: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t)lpThis, (uintptr_t)pcDSBufferDesc, (uintptr_t)ppDSBuffer, (uintptr_t)pUnkOuter);
#endif

    if ((lpThis == NULL) || (pcDSBufferDesc == NULL) || (ppDSBuffer == NULL) || (pUnkOuter != NULL))
    {
        return DSERR_INVALIDPARAM;
    }

#ifdef DEBUG_DSOUND
    eprintf("%i, 0x%x, %i, 0x%x - ", pcDSBufferDesc->dwSize, pcDSBufferDesc->dwFlags, pcDSBufferDesc->dwBufferBytes, pcDSBufferDesc->lpwfxFormat);
#endif

    if ((pcDSBufferDesc->dwSize == 20) &&
        (pcDSBufferDesc->dwFlags == DSBCAPS_PRIMARYBUFFER) &&
        (pcDSBufferDesc->dwBufferBytes == 0) &&
        (pcDSBufferDesc->lpwfxFormat == 0)
       )
    {
        if (lpThis->PrimaryBuffer != NULL)
        {
#ifdef DEBUG_DSOUND
            eprintf("error\n");
#endif
            return DSERR_INVALIDPARAM;
        }

        lpDSB_c = (struct IDirectSoundBuffer_c *) malloc(sizeof(struct IDirectSoundBuffer_c));

        if (lpDSB_c == NULL)
        {
#ifdef DEBUG_DSOUND
            eprintf("error\n");
#endif
            return DSERR_OUTOFMEMORY;
        }

        lpDSB_c->lpVtbl = FROMPTR(&IDirectSoundBufferVtbl_asm2c);
        lpDSB_c->RefCount = 1;
        lpDSB_c->DirectSound = lpThis;

        lpDSB_c->primary = 1;
        lpDSB_c->status = SDL_AUDIO_STOPPED;
        lpDSB_c->looping = 1;
        lpDSB_c->sample_size_shift = 0;

        lpDSB_c->first = NULL;
        lpDSB_c->num_channels = 0;

        desired.freq = 44100;
        desired.format = AUDIO_S16LSB;
        desired.channels = 2;
        desired.samples = (Audio_BufferSize)?Audio_BufferSize:1024;
        desired.callback = &fill_audio;
        desired.userdata = lpDSB_c;

#if SDL_VERSION_ATLEAST(2,0,0)
        lpDSB_c->device_id = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, SDL_AUDIO_ALLOW_ANY_CHANGE);
        if (lpDSB_c->device_id == 0)
#else
        if (0 != SDL_OpenAudio(&desired, &obtained))
#endif
        {
            free(lpDSB_c);

#ifdef DEBUG_DSOUND
            eprintf("SDL error\n");
#endif
            return DSERR_OUTOFMEMORY;
        }

        if ((obtained.channels > 2) || ((obtained.size / (obtained.samples * obtained.channels)) > 2))
        {
#if SDL_VERSION_ATLEAST(2,0,0)
            SDL_CloseAudioDevice(lpDSB_c->device_id);

            lpDSB_c->device_id = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE
                #ifdef SDL_AUDIO_ALLOW_SAMPLES_CHANGE
                    | SDL_AUDIO_ALLOW_SAMPLES_CHANGE
                #endif
            );
            if (lpDSB_c->device_id == 0)
#else
            SDL_CloseAudio();
            if (0 == SDL_OpenAudio(&desired, NULL))
            {
                obtained = desired;
            }
            else
#endif
            {
                free(lpDSB_c);

#ifdef DEBUG_DSOUND
                eprintf("SDL error\n");
#endif
                return DSERR_OUTOFMEMORY;
            }
        }

        if (obtained.channels == 2)
        {
            lpDSB_c->sample_size_shift++;
        }
        if ((obtained.format != AUDIO_U8) && (obtained.format != AUDIO_S8))
        {
            lpDSB_c->sample_size_shift++;
        }

        lpDSB_c->freq = obtained.freq;
        lpDSB_c->format = obtained.format;
        lpDSB_c->channels = obtained.channels;
        lpDSB_c->silence = obtained.silence;
        lpDSB_c->attenuation = 0;
        lpDSB_c->pan = 0;
        lpDSB_c->left_volume = 0x10000;
        lpDSB_c->right_volume = 0x10000;

        lpDSB_c->conv_data = (int16_t *) malloc(obtained.samples * 2 * sizeof(int16_t));
        lpDSB_c->accum_data = (int32_t *) malloc(obtained.samples * 2 * sizeof(int32_t));
        if ((lpDSB_c->conv_data == NULL) || (lpDSB_c->accum_data == NULL))
        {
            if (lpDSB_c->conv_data != NULL) free(lpDSB_c->conv_data);
            if (lpDSB_c->accum_data != NULL) free(lpDSB_c->accum_data);
#if SDL_VERSION_ATLEAST(2,0,0)
            SDL_CloseAudioDevice(lpDSB_c->device_id);
            lpDSB_c->device_id = 0;
#else
            SDL_CloseAudio();
#endif

            free(lpDSB_c);

#ifdef DEBUG_DSOUND
            eprintf("error\n");
#endif
            return DSERR_OUTOFMEMORY;
        }

        lpThis->PrimaryBuffer = lpDSB_c;

        *ppDSBuffer = FROMPTR(lpDSB_c);

#ifdef DEBUG_DSOUND
        eprintf("OK: 0x%" PRIxPTR "\n", (uintptr_t)lpDSB_c);
#endif
        return DS_OK;
    }


    if ((pcDSBufferDesc->dwSize == 20) &&
        (pcDSBufferDesc->dwFlags == (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_GETCURRENTPOSITION2)) &&
        (pcDSBufferDesc->dwBufferBytes != 0) &&
        (pcDSBufferDesc->lpwfxFormat != 0)
       )
    {
        lpwaveformatex lpwfxFormat;

        lpwfxFormat = TOPTR_T(waveformatex, pcDSBufferDesc->lpwfxFormat);

#ifdef DEBUG_DSOUND
        eprintf("%i, %i, %i, %i, %i - ", pcDSBufferDesc->dwBufferBytes, lpwfxFormat->wFormatTag, lpwfxFormat->wBitsPerSample, lpwfxFormat->nChannels, lpwfxFormat->nSamplesPerSec);
#endif

        if ((lpwfxFormat->wFormatTag != WAVE_FORMAT_PCM) ||
            ((lpwfxFormat->wBitsPerSample != 8) && (lpwfxFormat->wBitsPerSample != 16)) ||
            ((lpwfxFormat->nChannels != 1) && (lpwfxFormat->nChannels != 2)) ||
            ((lpwfxFormat->nSamplesPerSec != 11025) && (lpwfxFormat->nSamplesPerSec != 22050) && (lpwfxFormat->nSamplesPerSec != 44100))
           )
        {
#ifdef DEBUG_DSOUND
            eprintf("error\n");
#endif
            return DSERR_INVALIDPARAM;
        }


        lpDSB_c = (struct IDirectSoundBuffer_c *) malloc(sizeof(struct IDirectSoundBuffer_c));

        if (lpDSB_c == NULL)
        {
#ifdef DEBUG_DSOUND
            eprintf("error\n");
#endif
            return DSERR_OUTOFMEMORY;
        }

        lpDSB_c->lpVtbl = FROMPTR(&IDirectSoundBufferVtbl_asm2c);
        lpDSB_c->RefCount = 1;
        lpDSB_c->DirectSound = lpThis;

        lpDSB_c->primary = 0;
        lpDSB_c->status = SDL_AUDIO_STOPPED;
        lpDSB_c->looping = 0;
        lpDSB_c->sample_size_shift = 0;
        if (lpwfxFormat->wBitsPerSample == 16)
        {
            lpDSB_c->sample_size_shift++;
        }
        if (lpwfxFormat->nChannels == 2)
        {
            lpDSB_c->sample_size_shift++;
        }

        lpDSB_c->freq = lpwfxFormat->nSamplesPerSec;
        lpDSB_c->format = (lpwfxFormat->wBitsPerSample == 16)?AUDIO_S16LSB:AUDIO_U8;
        lpDSB_c->channels = lpwfxFormat->nChannels;
        lpDSB_c->silence = (lpwfxFormat->wBitsPerSample == 16)?0:0x80;
        lpDSB_c->attenuation = 0;
        lpDSB_c->pan = 0;
        lpDSB_c->left_volume = 0x10000;
        lpDSB_c->right_volume = 0x10000;

        lpDSB_c->next = NULL;
        lpDSB_c->prev = NULL;

        lpDSB_c->data = (uint8_t *)malloc(pcDSBufferDesc->dwBufferBytes + 4); // allocate 4 more bytes to allow reading past the end of buffer
        if (lpDSB_c->data == NULL)
        {
            free(lpDSB_c);
#ifdef DEBUG_DSOUND
            eprintf("error\n");
#endif
            return DSERR_OUTOFMEMORY;
        }

        lpDSB_c->size = pcDSBufferDesc->dwBufferBytes;
        lpDSB_c->read_offset = 0;
        lpDSB_c->write_offset = 0;
        lpDSB_c->last_conv_sample[0] = 0;
        lpDSB_c->last_conv_sample[1] = 0;

        *ppDSBuffer = FROMPTR(lpDSB_c);

#ifdef DEBUG_DSOUND
        eprintf("OK: 0x%" PRIxPTR "\n", (uintptr_t)lpDSB_c);
#endif
        return DS_OK;
    }


    eprintf("Unsupported method: %s\n", "IDirectSound_CreateSoundBuffer");
    exit(1);
}

uint32_t IDirectSound_GetCaps_c(struct IDirectSound_c *lpThis, void * pDSCaps)
{
    eprintf("Unsupported method: %s\n", "IDirectSound_GetCaps");
    exit(1);
}

uint32_t IDirectSound_DuplicateSoundBuffer_c(struct IDirectSound_c *lpThis, struct IDirectSoundBuffer_c * pDSBufferOriginal, PTR32(struct IDirectSoundBuffer_c *)* ppDSBufferDuplicate)
{
    eprintf("Unsupported method: %s\n", "IDirectSound_DuplicateSoundBuffer");
    exit(1);
}

uint32_t IDirectSound_SetCooperativeLevel_c(struct IDirectSound_c *lpThis, void * hwnd, uint32_t dwLevel)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSound_SetCooperativeLevel: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%x - ", (uintptr_t)lpThis, (uintptr_t)hwnd, dwLevel);
#endif

    if ((lpThis == NULL) || (hwnd == NULL))
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if ((dwLevel != DSSCL_EXCLUSIVE) && (dwLevel != DSSCL_PRIORITY))
    {
        eprintf("Unsupported method: %s\n", "IDirectSound_SetCooperativeLevel");
        exit(1);
    }

#ifdef DEBUG_DSOUND
    eprintf("OK\n");
#endif
    return DS_OK;
}

uint32_t IDirectSound_Compact_c(struct IDirectSound_c *lpThis)
{
    eprintf("Unsupported method: %s\n", "IDirectSound_Compact");
    exit(1);
}

uint32_t IDirectSound_GetSpeakerConfig_c(struct IDirectSound_c *lpThis, uint32_t * pdwSpeakerConfig)
{
    eprintf("Unsupported method: %s\n", "IDirectSound_GetSpeakerConfig");
    exit(1);
}

uint32_t IDirectSound_SetSpeakerConfig_c(struct IDirectSound_c *lpThis, uint32_t dwSpeakerConfig)
{
    eprintf("Unsupported method: %s\n", "IDirectSound_SetSpeakerConfig");
    exit(1);
}

uint32_t IDirectSound_Initialize_c(struct IDirectSound_c *lpThis, const void * pcGuidDevice)
{
    eprintf("Unsupported method: %s\n", "IDirectSound_Initialize");
    exit(1);
}


uint32_t IDirectSoundBuffer_QueryInterface_c(struct IDirectSoundBuffer_c *lpThis, void * riid, PTR32(void *)* ppvObj)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_QueryInterface: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR "\n", (uintptr_t)lpThis, (uintptr_t)riid, (uintptr_t)ppvObj);
#endif

    if (ppvObj == NULL) return E_POINTER;
    return E_NOINTERFACE;
}

uint32_t IDirectSoundBuffer_AddRef_c(struct IDirectSoundBuffer_c *lpThis)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_AddRef: 0x%" PRIxPTR " - ", (uintptr_t)lpThis);
#endif

    if (lpThis != NULL)
    {
        lpThis->RefCount++;
#ifdef DEBUG_DSOUND
        eprintf("%i\n", lpThis->RefCount);
#endif
        return lpThis->RefCount;
    }

#ifdef DEBUG_DSOUND
    eprintf("%i\n", 0);
#endif

    return 0;
}

static void StopPrimaryBuffer(struct IDirectSoundBuffer_c *lpThis)
{
    struct IDirectSoundBuffer_c *current, *next;

    if (lpThis->status == SDL_AUDIO_PLAYING)
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_LockAudioDevice(lpThis->device_id);
        SDL_PauseAudioDevice(lpThis->device_id, 1);
        SDL_UnlockAudioDevice(lpThis->device_id);
#else
        SDL_LockAudio();
        SDL_PauseAudio(1);
        SDL_UnlockAudio();
#endif
        lpThis->status = SDL_AUDIO_STOPPED;
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_CloseAudioDevice(lpThis->device_id);
    lpThis->device_id = 0;
#else
    SDL_CloseAudio();
#endif

    current = lpThis->first;
    while (current != NULL)
    {
        next = current->next;

        current->next = NULL;
        current->prev = NULL;
        current->status = SDL_AUDIO_PAUSED;

        current = next;
    };

    lpThis->first = NULL;
    lpThis->num_channels = 0;
}

uint32_t IDirectSoundBuffer_Release_c(struct IDirectSoundBuffer_c *lpThis)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_Release: 0x%" PRIxPTR " - ", (uintptr_t)lpThis);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DSOUND
        eprintf("%i\n", 0);
#endif
        return 0;
    }

    lpThis->RefCount--;
#ifdef DEBUG_DSOUND
    eprintf("%i\n", lpThis->RefCount);
#endif
    if (lpThis->RefCount == 0)
    {
        if (lpThis->primary)
        {
            StopPrimaryBuffer(lpThis);

            if (lpThis->conv_data != NULL)
            {
                free(lpThis->conv_data);
                lpThis->conv_data = NULL;
            }
            if (lpThis->accum_data != NULL)
            {
                free(lpThis->accum_data);
                lpThis->accum_data = NULL;
            }
        }
        else
        {
            if (lpThis->status == SDL_AUDIO_PLAYING)
            {
                struct IDirectSoundBuffer_c *PrimaryBuffer;

                PrimaryBuffer = lpThis->DirectSound->PrimaryBuffer;

#if SDL_VERSION_ATLEAST(2,0,0)
                SDL_LockAudioDevice(PrimaryBuffer->device_id);
#else
                SDL_LockAudio();
#endif

                remove_from_list(PrimaryBuffer, lpThis, 1);

#if SDL_VERSION_ATLEAST(2,0,0)
                SDL_UnlockAudioDevice(PrimaryBuffer->device_id);
#else
                SDL_UnlockAudio();
#endif
            }

            if (lpThis->data != NULL)
            {
                free(lpThis->data);
                lpThis->data = NULL;
            }
        }
        free(lpThis);
        return 0;
    }
    return lpThis->RefCount;
}

uint32_t IDirectSoundBuffer_GetCaps_c(struct IDirectSoundBuffer_c *lpThis, void * pDSBufferCaps)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_GetCaps");
    exit(1);
}

uint32_t IDirectSoundBuffer_GetCurrentPosition_c(struct IDirectSoundBuffer_c *lpThis, uint32_t * pdwCurrentPlayCursor, uint32_t * pdwCurrentWriteCursor)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_GetCurrentPosition: 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t) lpThis, (uintptr_t) pdwCurrentPlayCursor, (uintptr_t) pdwCurrentWriteCursor);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (lpThis->primary)
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (pdwCurrentPlayCursor != NULL)
    {
        *pdwCurrentPlayCursor = lpThis->read_offset;
    }

    if (pdwCurrentWriteCursor != NULL)
    {
        *pdwCurrentWriteCursor = lpThis->write_offset;
    }

#ifdef DEBUG_DSOUND
    eprintf("OK - %i, %i\n", lpThis->read_offset, lpThis->write_offset);
#endif
    return DS_OK;
}

uint32_t IDirectSoundBuffer_GetFormat_c(struct IDirectSoundBuffer_c *lpThis, void * pwfxFormat, uint32_t dwSizeAllocated, uint32_t * pdwSizeWritten)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_GetFormat");
    exit(1);
}

uint32_t IDirectSoundBuffer_GetVolume_c(struct IDirectSoundBuffer_c *lpThis, int32_t * plVolume)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_GetVolume");
    exit(1);
}

uint32_t IDirectSoundBuffer_GetPan_c(struct IDirectSoundBuffer_c *lpThis, int32_t * plPan)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_GetPan");
    exit(1);
}

uint32_t IDirectSoundBuffer_GetFrequency_c(struct IDirectSoundBuffer_c *lpThis, uint32_t * pdwFrequency)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_GetFrequency");
    exit(1);
}

uint32_t IDirectSoundBuffer_GetStatus_c(struct IDirectSoundBuffer_c *lpThis, uint32_t * pdwStatus)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_GetStatus");
    exit(1);
}

uint32_t IDirectSoundBuffer_Initialize_c(struct IDirectSoundBuffer_c *lpThis, struct IDirectSound_c * pDirectSound, const void * pcDSBufferDesc)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_Initialize");
    exit(1);
}

uint32_t IDirectSoundBuffer_Lock_c(struct IDirectSoundBuffer_c *lpThis, uint32_t dwOffset, uint32_t dwBytes, PTR32(void *)* ppvAudioPtr1, uint32_t * pdwAudioBytes1, PTR32(void *)* ppvAudioPtr2, uint32_t * pdwAudioBytes2, uint32_t dwFlags)
{
    uint32_t locked_size;
#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_Lock: 0x%" PRIxPTR ", %i, %i, 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%" PRIxPTR ", 0x%x - ", (uintptr_t) lpThis, dwOffset, dwBytes, (uintptr_t) ppvAudioPtr1, (uintptr_t) pdwAudioBytes1, (uintptr_t) ppvAudioPtr2, (uintptr_t) pdwAudioBytes2, dwFlags);
#endif

    if ((lpThis == NULL) || (ppvAudioPtr1 == NULL) || (pdwAudioBytes1 == NULL))
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (dwFlags & DSBLOCK_FROMWRITECURSOR)
    {
        dwOffset = lpThis->write_offset;
    }
    if (dwFlags & DSBLOCK_ENTIREBUFFER)
    {
        dwBytes = lpThis->size;
    }

    if (lpThis->primary || (dwOffset >= lpThis->size) || (dwBytes > lpThis->size))
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    *ppvAudioPtr1 = FROMPTR(&(lpThis->data[dwOffset]));

    locked_size = lpThis->size - dwOffset;

    if (locked_size >= dwBytes)
    {
        *pdwAudioBytes1 = dwBytes;

        if (ppvAudioPtr2 != NULL)
        {
            *ppvAudioPtr2 = 0;
        }
        if (pdwAudioBytes2 != NULL)
        {
            *pdwAudioBytes2 = 0;
        }
    }
    else
    {
        *pdwAudioBytes1 = locked_size;
        if (ppvAudioPtr2 != NULL)
        {
            *ppvAudioPtr2 = FROMPTR(&(lpThis->data[0]));
            if (pdwAudioBytes2 != NULL)
            {
                *pdwAudioBytes2 = dwBytes - locked_size;
            }
        }
        else
        {
            if (pdwAudioBytes2 != NULL)
            {
                *pdwAudioBytes2 = 0;
            }
        }
    }

#ifdef DEBUG_DSOUND
        eprintf("OK\n");
#endif
        return DS_OK;
}

uint32_t IDirectSoundBuffer_Play_c(struct IDirectSoundBuffer_c *lpThis, uint32_t dwReserved1, uint32_t dwReserved2, uint32_t dwFlags)
{
    struct IDirectSoundBuffer_c *PrimaryBuffer;

#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_Play: 0x%" PRIxPTR ", 0x%x, 0x%x, 0x%x - ", (uintptr_t) lpThis, dwReserved1, dwReserved2, dwFlags);
#endif

    if ((lpThis == NULL) || (dwReserved1 != 0) || (dwReserved2 != 0))
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (lpThis->primary)
    {
        if (dwFlags != DSBPLAY_LOOPING)
        {
#ifdef DEBUG_DSOUND
            eprintf("error\n");
#endif
            return DSERR_INVALIDPARAM;
        }

        lpThis->status = SDL_AUDIO_PLAYING;
        lpThis->looping = 1;
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_PauseAudioDevice(lpThis->device_id, 0);
#else
        SDL_PauseAudio(0);
#endif

#ifdef DEBUG_DSOUND
        eprintf("OK\n");
#endif
        return DS_OK;
    }

    PrimaryBuffer = lpThis->DirectSound->PrimaryBuffer;

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_LockAudioDevice(PrimaryBuffer->device_id);
#else
    SDL_LockAudio();
#endif

    lpThis->looping = (dwFlags & DSBPLAY_LOOPING)?1:0;
    if (lpThis->status != SDL_AUDIO_PLAYING)
    {
        struct IDirectSoundBuffer_c *first, *last;

        lpThis->status = SDL_AUDIO_PLAYING;

        if (PrimaryBuffer->num_channels == 0)
        {
            PrimaryBuffer->num_channels = 1;
            PrimaryBuffer->first = lpThis;
            lpThis->next = lpThis;
            lpThis->prev = lpThis;
        }
        else
        {
            first = PrimaryBuffer->first;
            last = first->prev;
            last->next = lpThis;
            lpThis->prev = last;
            lpThis->next = first;
            first->prev = lpThis;
            PrimaryBuffer->num_channels++;
        }
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_UnlockAudioDevice(PrimaryBuffer->device_id);
#else
    SDL_UnlockAudio();
#endif

#ifdef DEBUG_DSOUND
    eprintf("OK\n");
#endif
    return DS_OK;
}

uint32_t IDirectSoundBuffer_SetCurrentPosition_c(struct IDirectSoundBuffer_c *lpThis, uint32_t dwNewPosition)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_SetCurrentPosition");
    exit(1);
}

uint32_t IDirectSoundBuffer_SetFormat_c(struct IDirectSoundBuffer_c *lpThis, const struct twaveformatex * pcfxFormat)
{
    SDL_AudioSpec desired, obtained;

#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_SetFormat: 0x%" PRIxPTR ", 0x%" PRIxPTR " - ", (uintptr_t) lpThis, (uintptr_t) pcfxFormat);
#endif

    if ((lpThis == NULL) || (pcfxFormat == NULL))
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (!lpThis->primary)
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

#ifdef DEBUG_DSOUND
    eprintf("%i, %i, %i, %i - ", pcfxFormat->wFormatTag, pcfxFormat->nChannels, pcfxFormat->nSamplesPerSec, pcfxFormat->wBitsPerSample);
#endif

    if ((pcfxFormat->wFormatTag != WAVE_FORMAT_PCM) ||
        ((pcfxFormat->nChannels != 1) && (pcfxFormat->nChannels != 2)) ||
        ((pcfxFormat->nSamplesPerSec != 11025) && (pcfxFormat->nSamplesPerSec != 22050) && (pcfxFormat->nSamplesPerSec != 44100)) ||
        ((pcfxFormat->wBitsPerSample != 8) && (pcfxFormat->wBitsPerSample != 16))
       )
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    desired.freq = pcfxFormat->nSamplesPerSec;
    desired.format = (pcfxFormat->wBitsPerSample == 16)?AUDIO_S16LSB:AUDIO_U8;
    desired.channels = pcfxFormat->nChannels;

    if ((desired.freq == lpThis->freq) &&
        (desired.format == lpThis->format) &&
        (desired.channels == lpThis->channels))
    {
#ifdef DEBUG_DSOUND
        eprintf("OK\n");
#endif
        return DS_OK;
    }

    desired.samples = (Audio_BufferSize)?Audio_BufferSize:(256 * (pcfxFormat->nSamplesPerSec / 11025));
    desired.callback = &fill_audio;
    desired.userdata = lpThis;

    if (lpThis->status == SDL_AUDIO_PLAYING)
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_LockAudioDevice(lpThis->device_id);
        SDL_PauseAudioDevice(lpThis->device_id, 1);
        SDL_UnlockAudioDevice(lpThis->device_id);
#else
        SDL_LockAudio();
        SDL_PauseAudio(1);
        SDL_UnlockAudio();
#endif
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_CloseAudioDevice(lpThis->device_id);
    lpThis->device_id = 0;
#else
    SDL_CloseAudio();
#endif

    if (lpThis->conv_data != NULL)
    {
        free(lpThis->conv_data);
        lpThis->conv_data = NULL;
    }
    if (lpThis->accum_data != NULL)
    {
        free(lpThis->accum_data);
        lpThis->accum_data = NULL;
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    lpThis->device_id = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (lpThis->device_id == 0)
#else
    if (0 != SDL_OpenAudio(&desired, &obtained))
#endif
    {
        lpThis->status = SDL_AUDIO_STOPPED;
#ifdef DEBUG_DSOUND
        eprintf("SDL error\n");
#endif
        return DSERR_BADFORMAT;
    }

    if ((obtained.channels > 2) || ((obtained.size / (obtained.samples * obtained.channels)) > 2))
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_CloseAudioDevice(lpThis->device_id);

        lpThis->device_id = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE
            #ifdef SDL_AUDIO_ALLOW_SAMPLES_CHANGE
                | SDL_AUDIO_ALLOW_SAMPLES_CHANGE
            #endif
        );
        if (lpThis->device_id == 0)
#else
        SDL_CloseAudio();
        if (0 == SDL_OpenAudio(&desired, NULL))
        {
            obtained = desired;
        }
        else
#endif
        {
            lpThis->status = SDL_AUDIO_STOPPED;
#ifdef DEBUG_DSOUND
            eprintf("SDL error\n");
#endif
            return DSERR_BADFORMAT;
        }
    }

    lpThis->sample_size_shift = 0;
    if (obtained.channels == 2)
    {
        lpThis->sample_size_shift++;
    }
    if ((obtained.format != AUDIO_U8) && (obtained.format != AUDIO_S8))
    {
        lpThis->sample_size_shift++;
    }

    lpThis->freq = obtained.freq;
    lpThis->format = obtained.format;
    lpThis->channels = obtained.channels;
    lpThis->silence = obtained.silence;

    lpThis->conv_data = (int16_t *) malloc(obtained.samples * 2 * sizeof(int16_t));
    lpThis->accum_data = (int32_t *) malloc(obtained.samples * 2 * sizeof(int32_t));
    if ((lpThis->conv_data == NULL) || (lpThis->accum_data == NULL))
    {
        if (lpThis->conv_data != NULL)
        {
            free(lpThis->conv_data);
            lpThis->conv_data = NULL;
        }
        if (lpThis->accum_data != NULL)
        {
            free(lpThis->accum_data);
            lpThis->accum_data = NULL;
        }
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_CloseAudioDevice(lpThis->device_id);
        lpThis->device_id = 0;
#else
        SDL_CloseAudio();
#endif

#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_OUTOFMEMORY;
    }

    if (lpThis->status == SDL_AUDIO_PLAYING)
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_PauseAudioDevice(lpThis->device_id, 0);
#else
        SDL_PauseAudio(0);
#endif
    }

#ifdef DEBUG_DSOUND
    eprintf("OK\n");
#endif
    return DS_OK;
}

static void RecalculateVolume(struct IDirectSoundBuffer_c *lpThis)
{
    int left_attenuation, right_attenuation, left_volume, right_volume;

    left_attenuation = right_attenuation = lpThis->attenuation;
    if (lpThis->pan > 0)
    {
        left_attenuation -= lpThis->pan;
    }
    else if (lpThis->pan < 0)
    {
        right_attenuation += lpThis->pan;
    }

    left_volume = (int) (powf(2.0f, left_attenuation / 600.0f) * 65536.0f);
    right_volume = (int) (powf(2.0f, right_attenuation / 600.0f) * 65536.0f);

    lpThis->left_volume = left_volume;
    lpThis->right_volume = right_volume;
}

uint32_t IDirectSoundBuffer_SetVolume_c(struct IDirectSoundBuffer_c *lpThis, int32_t lVolume)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_SetVolume: 0x%" PRIxPTR ", %i - ", (uintptr_t) lpThis, lVolume);
#endif

    if ((lpThis == NULL) || (lVolume > 0) || (lVolume < -10000))
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (lpThis->attenuation != lVolume)
    {
        lpThis->attenuation = lVolume;
        RecalculateVolume(lpThis);
    }

#ifdef DEBUG_DSOUND
    eprintf("OK\n");
#endif
    return DS_OK;
}

uint32_t IDirectSoundBuffer_SetPan_c(struct IDirectSoundBuffer_c *lpThis, int32_t lPan)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_SetPan: 0x%" PRIxPTR ", %i - ", (uintptr_t) lpThis, lPan);
#endif

    if ((lpThis == NULL) || (lPan > 10000) || (lPan < -10000))
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (lpThis->pan != lPan)
    {
        lpThis->pan = lPan;
        RecalculateVolume(lpThis);
    }

#ifdef DEBUG_DSOUND
    eprintf("OK\n");
#endif
    return DS_OK;
}

uint32_t IDirectSoundBuffer_SetFrequency_c(struct IDirectSoundBuffer_c *lpThis, uint32_t dwFrequency)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_SetFrequency");
    exit(1);
}

uint32_t IDirectSoundBuffer_Stop_c(struct IDirectSoundBuffer_c *lpThis)
{
    struct IDirectSoundBuffer_c *PrimaryBuffer;

#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_Stop: 0x%" PRIxPTR " - ", (uintptr_t) lpThis);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (lpThis->primary)
    {
#if SDL_VERSION_ATLEAST(2,0,0)
        SDL_LockAudioDevice(lpThis->device_id);
        SDL_PauseAudioDevice(lpThis->device_id, 1);
        SDL_UnlockAudioDevice(lpThis->device_id);
#else
        SDL_LockAudio();
        SDL_PauseAudio(1);
        SDL_UnlockAudio();
#endif
        lpThis->status = SDL_AUDIO_PAUSED;

#ifdef DEBUG_DSOUND
        eprintf("OK\n");
#endif
        return DS_OK;
    }

    PrimaryBuffer = lpThis->DirectSound->PrimaryBuffer;

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_LockAudioDevice(PrimaryBuffer->device_id);
#else
    SDL_LockAudio();
#endif

    if (lpThis->status == SDL_AUDIO_PLAYING)
    {
        remove_from_list(PrimaryBuffer, lpThis, 0);
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_UnlockAudioDevice(PrimaryBuffer->device_id);
#else
    SDL_UnlockAudio();
#endif

#ifdef DEBUG_DSOUND
    eprintf("OK\n");
#endif
    return DS_OK;
}

uint32_t IDirectSoundBuffer_Unlock_c(struct IDirectSoundBuffer_c *lpThis, void * pvAudioPtr1, uint32_t dwAudioBytes1, void * pvAudioPtr2, uint32_t dwAudioBytes2)
{
#ifdef DEBUG_DSOUND
    eprintf("IDirectSoundBuffer_Unlock: 0x%" PRIxPTR ", 0x%" PRIxPTR ", %i, 0x%" PRIxPTR ", %i - ", (uintptr_t) lpThis, (uintptr_t) pvAudioPtr1, dwAudioBytes1, (uintptr_t) pvAudioPtr2, dwAudioBytes2);
#endif

    if (lpThis == NULL)
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (lpThis->primary || (dwAudioBytes1 > lpThis->size) || (dwAudioBytes2 > lpThis->size))
    {
#ifdef DEBUG_DSOUND
        eprintf("error\n");
#endif
        return DSERR_INVALIDPARAM;
    }

    if (pvAudioPtr1 == &(lpThis->data[lpThis->write_offset]))
    {
        lpThis->write_offset += dwAudioBytes1;
        if (lpThis->write_offset >= lpThis->size)
        {
            lpThis->write_offset -= lpThis->size;
        }
    }
#ifdef DEBUG_DSOUND
    else if (pvAudioPtr1 != NULL)
    {
        eprintf("wrong offset 1: %i - ", lpThis->write_offset);
    }
#endif

    if (pvAudioPtr2 == &(lpThis->data[lpThis->write_offset]))
    {
        lpThis->write_offset += dwAudioBytes2;
        if (lpThis->write_offset >= lpThis->size)
        {
            lpThis->write_offset -= lpThis->size;
        }
    }
#ifdef DEBUG_DSOUND
    else if (pvAudioPtr2 != NULL)
    {
        eprintf("wrong offset 2: %i - ", lpThis->write_offset);
    }
#endif

#ifdef DEBUG_DSOUND
    eprintf("OK\n");
#endif
    return DS_OK;
}

uint32_t IDirectSoundBuffer_Restore_c(struct IDirectSoundBuffer_c *lpThis)
{
    eprintf("Unsupported method: %s\n", "IDirectSoundBuffer_Restore");
    exit(1);
}


