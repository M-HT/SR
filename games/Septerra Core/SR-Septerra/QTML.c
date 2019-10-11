/**
 *
 *  Copyright (C) 2019 Roman Pauer
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

// https://wiki.multimedia.cx/index.php?title=Sorenson_Video_1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <lqt/lqt.h>
#include <lqt/colormodels.h>
#include "QTML.h"
#include "Game-Config.h"

#if (defined(__WIN32__) || (__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifndef _WIN32
#include <pthread.h>
#include "CLIB.h"
#endif


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


#pragma pack(1)
typedef struct __attribute__ ((__packed__)) Rect {
    int16_t                         top;
    int16_t                         left;
    int16_t                         bottom;
    int16_t                         right;
} Rect;

typedef struct __attribute__ ((__packed__)) EventRecord {
    uint16_t                        what;
    uint32_t                        message;
    uint32_t                        when;
    uint32_t                        where;
    uint16_t                        modifiers;
} EventRecord;
#pragma pack()

typedef struct FSSpec {
    int16_t                         vRefNum;
    int32_t                         parID;
    char                            name[256];
} FSSpec;

typedef struct NativeEvent {
    uint32_t hWnd;
    uint32_t Msg;
    uint32_t wParam;
    uint32_t lParam;
    uint32_t time;
    uint32_t x;
    uint32_t y;
} NativeEvent;

typedef struct MovieRecord {
    quicktime_t *qt;
    uint8_t** frame;
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_Renderer *Renderer;
    SDL_Texture *Video;
#else
    SDL_Surface *Screen;
    SDL_Surface *Video;
    SDL_Surface *Video2;
    SDL_Overlay *Overlay;
#endif
    int video_width;
    int video_height;
    int colormodel;
#if SDL_VERSION_ATLEAST(2,0,0)
    unsigned int format;
#else
    SDL_Rect srcrect;
#endif
    SDL_Rect dstrect;
    int rowspan_y;
    int rowspan_uv;
    volatile int is_playing;
    volatile int stop_playback;
    volatile int updated;
    volatile int flipped;
#ifndef _WIN32
    pthread_t thread_id;
#endif

    // audio
    int play_audio;
    int audio_channels;
    long audio_sample_rate;
    int decode_raw_audio;
#if SDL_VERSION_ATLEAST(2,0,0)
    SDL_AudioFormat audio_format;
#else
    int audio_format;
#endif
    int audio_format_size;
    int audio_eof;
    uint8_t silence;
    int64_t audio_length;
    uint8_t *audio_buffer;
#if SDL_VERSION_ATLEAST(2,0,0)
    #define MAX_CHANNELS 6
#else
    #define MAX_CHANNELS 2
#endif
    int16_t *audio_temp_buffer[MAX_CHANNELS];
    int audio_buffer_size;
    //int audio_temp_buffer_size;
    int audio_temp_buffer_samples;
    volatile int audio_read_offset;
    volatile int audio_write_offset;
} MovieRecord, *Movie;


static quicktime_t *qthandle[4];


static void *lpDDSurface;


#if SDL_VERSION_ATLEAST(2,0,0)
#ifdef __cplusplus
extern "C" {
#endif
extern SDL_Renderer *GetSurfaceRenderer(void *);
#ifdef __cplusplus
}
#endif
#endif


#if SDL_VERSION_ATLEAST(2,0,0)
static void CopyFrameToTexture(Movie movie)
{
    uint8_t *dst, *src;
    int dstpitch, srcpitch, copylength, height;

    if (0 == SDL_LockTexture(movie->Video, NULL, (void **)&dst, &dstpitch))
    {
        src = (uint8_t *) movie->frame[0];
        srcpitch = movie->rowspan_y;

        if (srcpitch == dstpitch)
        {
            memcpy(dst, src, movie->video_height * srcpitch);
        }
        else
        {
            copylength = (srcpitch < dstpitch)?srcpitch:dstpitch;

            for (height = movie->video_height; height != 0; height--)
            {
                memcpy(dst, src, copylength);

                dst += dstpitch;
                src += srcpitch;
            }
        }

        SDL_UnlockTexture(movie->Video);
    }
}
#else
static void CopyYUV(uint8_t **frame, SDL_Overlay *Overlay)
{
    int plane;

    if (0 == SDL_LockYUVOverlay(Overlay))
    {
        for (plane = 0; plane < Overlay->planes; plane++)
        {
            memcpy(Overlay->pixels[plane], frame[plane], Overlay->h * Overlay->pitches[plane]);
        }

        SDL_UnlockYUVOverlay(Overlay);
    }
}

static void ScaleFrame(SDL_Surface *Video, SDL_Surface *Video2)
{
    int height, width, bpp;
    uint8_t *srcline, *dstline;

    SDL_LockSurface(Video2);

    srcline = (uint8_t *) Video->pixels;
    dstline = (uint8_t *) Video2->pixels;
    bpp = Video->format->BitsPerPixel;
    for (height = Video->h; height != 0; height--)
    {
        if (bpp == 32)
        {
            uint32_t *src, *dst, *dst2;
            src = (uint32_t *)srcline;
            dst = (uint32_t *)dstline;
            dst2 = (uint32_t *)(dstline + Video2->pitch);
            for (width = Video->w; width != 0; width--)
            {
                dst[0] = dst[1] = dst2[0] = dst2[1] = src[0];
                src++;
                dst += 2;
                dst2 += 2;
            }
        }
        else if (bpp == 16)
        {
            uint16_t *src, *dst, *dst2;
            src = (uint16_t *)srcline;
            dst = (uint16_t *)dstline;
            dst2 = (uint16_t *)(dstline + Video2->pitch);
            for (width = Video->w; width != 0; width--)
            {
                dst[0] = dst[1] = dst2[0] = dst2[1] = src[0];
                src++;
                dst += 2;
                dst2 += 2;
            }
        }

        srcline += Video->pitch;
        dstline += 2 * Video2->pitch;
    }

    SDL_UnlockSurface(Video2);
}
#endif

static int decode_more_audio(Movie movie)
{
    int read_offset, write_offset, max_size, bytes_available, samples_available, decoded_samples, channel;
    int16_t *output[1];
    int64_t last_position;

    if (!movie->play_audio) return 0;
    if (movie->audio_eof) return 0;

    read_offset = movie->audio_read_offset;
    write_offset = movie->audio_write_offset;

    max_size = movie->audio_temp_buffer_samples * movie->audio_format_size * movie->audio_channels;

    if (write_offset < read_offset)
    {
        bytes_available = read_offset - write_offset;
        if (bytes_available <= max_size) return 0;
        bytes_available = max_size;
    }
    else
    {
        bytes_available = movie->audio_buffer_size - write_offset;
        if (bytes_available <= max_size)
        {
            if (read_offset == 0) return 0;
        }
        else
        {
            bytes_available = max_size;
        }
    }

    if (bytes_available == max_size)
    {
        samples_available = movie->audio_temp_buffer_samples;
    }
    else
    {
        samples_available = (bytes_available / movie->audio_format_size) / movie->audio_channels;
    }

    last_position = lqt_last_audio_position(movie->qt, 0);

    if (samples_available >= movie->audio_length - last_position)
    {
        samples_available = movie->audio_length - last_position;
        movie->audio_eof = 1;
    }

    if (movie->decode_raw_audio)
    {
        decoded_samples = lqt_decode_audio_raw(movie->qt, &(movie->audio_buffer[write_offset]), samples_available, 0);
        if (decoded_samples == 0)
        {
            movie->audio_eof = 1;
        }
        else
        {
            write_offset += decoded_samples * movie->audio_format_size * movie->audio_channels;
            if (write_offset >= movie->audio_buffer_size) write_offset -= movie->audio_buffer_size;
            movie->audio_write_offset = write_offset;
        }
    }
    else if (movie->audio_channels == 1)
    {
        output[0] = (int16_t *) &(movie->audio_buffer[write_offset]);
        if (lqt_decode_audio_track(movie->qt, output, NULL, samples_available, 0))
        {
            movie->audio_eof = 1;
        }
        else
        {
            write_offset += samples_available * movie->audio_format_size * movie->audio_channels;
            if (write_offset >= movie->audio_buffer_size) write_offset -= movie->audio_buffer_size;
            movie->audio_write_offset = write_offset;
        }
    }
    else
    {
        if (lqt_decode_audio_track(movie->qt, movie->audio_temp_buffer, NULL, samples_available, 0))
        {
            movie->audio_eof = 1;
        }
        else
        {
            output[0] = (int16_t *) &(movie->audio_buffer[write_offset]);
            if (movie->audio_channels == 2)
            {
                for (decoded_samples = 0; decoded_samples < samples_available; decoded_samples++)
                {
                    output[0][0] = movie->audio_temp_buffer[0][decoded_samples];
                    output[0][1] = movie->audio_temp_buffer[1][decoded_samples];
                    output[0]+=2;
                }
            }
            else
            {
                for (decoded_samples = 0; decoded_samples < samples_available; decoded_samples++)
                {
                    for (channel = 0; channel < movie->audio_channels; channel++)
                    {
                        output[0][0] = movie->audio_temp_buffer[channel][decoded_samples];
                        output[0]++;
                    }
                }
            }

            write_offset += samples_available * movie->audio_format_size * movie->audio_channels;
            if (write_offset >= movie->audio_buffer_size) write_offset -= movie->audio_buffer_size;
            movie->audio_write_offset = write_offset;
        }
    }

    return 1;
}

#ifdef _WIN32
static void movie_thread(void *arg)
#else
static void *movie_thread(void *arg)
#endif
{
    Movie movie;
    int video_time_scale, phase, playing, diff_time;
    int64_t video_duration, next_frame_time;
    unsigned int next_frame, first_frame;
#if !SDL_VERSION_ATLEAST(2,0,0)
    SDL_Rect dstrect;
#endif
    SDL_Event event;

    movie = (Movie)arg;

    video_time_scale = lqt_video_time_scale(movie->qt, 0);
    video_duration = lqt_video_duration(movie->qt, 0);

    phase = 4;
    playing = 1;

    // prepare event
    event.type = SDL_USEREVENT;
    event.user.data1 = NULL;
    event.user.data2 = NULL;

    // decode first frame
    if (lqt_decode_video(movie->qt, movie->frame, 0))
    {
        playing = 0;
    }
    else
    {
        decode_more_audio(movie);
        decode_more_audio(movie);

#if SDL_VERSION_ATLEAST(2,0,0)
        movie->updated = 0;
        movie->flipped = 0;

        // push event to render texture
        event.user.code = 2;
        SDL_PushEvent(&event);
#else
        if (movie->Overlay != NULL)
        {
            SDL_FillRect(movie->Screen, NULL, 0);
            SDL_Flip(movie->Screen);

            CopyYUV(movie->frame, movie->Overlay);
        }
        else
        {
            dstrect = movie->dstrect;
            if (movie->Video2 != NULL)
            {
                ScaleFrame(movie->Video, movie->Video2);
                SDL_BlitSurface(movie->Video2, &(movie->srcrect), movie->Screen, &dstrect);
            }
            else
            {
                SDL_BlitSurface(movie->Video, &(movie->srcrect), movie->Screen, &dstrect);
            }
        }
#endif

        if (movie->play_audio)
        {
            SDL_PauseAudio(0);
        }
    }


    first_frame = SDL_GetTicks();
    next_frame = 0;

    while (playing)
    {
        switch (phase)
        {
        case 0: // wait for update
            if (movie->stop_playback)
            {
                playing = 0;
                break;
            }

#if SDL_VERSION_ATLEAST(2,0,0)
            if (!movie->updated)
            {
                if (!decode_more_audio(movie))
                {
                    SDL_Delay(0);
                }
            }
            else
            {
                movie->updated = 0;
                phase++;
            }
#else
            phase++;
#endif
            break;
        case 1: // decode
            decode_more_audio(movie);

            next_frame_time = lqt_frame_time(movie->qt, 0);

            if (next_frame_time > video_duration)
            {
                playing = 0;
                break;
            }

            next_frame = first_frame + (int)((((double)next_frame_time) / video_time_scale) * 1000);

            if (lqt_decode_video(movie->qt, movie->frame, 0))
            {
                playing = 0;
            }
            else
            {
                phase++;

#if !SDL_VERSION_ATLEAST(2,0,0)
                if (movie->Overlay != NULL)
                {
                    CopyYUV(movie->frame, movie->Overlay);
                }
                else
                {
                    dstrect = movie->dstrect;

                    if ((dstrect.x != 0) || (dstrect.y != 0) || (movie->srcrect.w < movie->Screen->w) || (movie->srcrect.h < movie->Screen->h))
                    {
                        SDL_FillRect(movie->Screen, NULL, 0);
                    }

                    if (movie->Video2 != NULL)
                    {
                        ScaleFrame(movie->Video, movie->Video2);
                        SDL_BlitSurface(movie->Video2, &(movie->srcrect), movie->Screen, &dstrect);
                    }
                    else
                    {
                        SDL_BlitSurface(movie->Video, &(movie->srcrect), movie->Screen, &dstrect);
                    }
                }
#endif
            }
            break;
        case 2: // wait for flip
            if (movie->stop_playback)
            {
                playing = 0;
                break;
            }

#if SDL_VERSION_ATLEAST(2,0,0)
            if (!movie->flipped)
            {
                if (!decode_more_audio(movie))
                {
                    SDL_Delay(0);
                }
            }
            else
            {
                movie->flipped = 0;
                phase++;

                // push event to render texture
                event.user.code = 2;
                SDL_PushEvent(&event);
            }
#else
            phase++;
#endif
            break;
        case 3: // wait for next frame time
            if (movie->stop_playback)
            {
                playing = 0;
                break;
            }

            diff_time = SDL_GetTicks() - next_frame;
            if (diff_time < 0)
            {
                if (!decode_more_audio(movie))
                {
                    SDL_Delay(1);
                }
            }
            else
            {
                phase++;
            }
            break;
        case 4: // display
#if SDL_VERSION_ATLEAST(2,0,0)
            // push event to flip buffers
            event.user.code = 3;
            SDL_PushEvent(&event);
#else
            if (movie->Overlay != NULL)
            {
                SDL_DisplayYUVOverlay(movie->Overlay, &(movie->dstrect));
            }
            else
            {
                SDL_Flip(movie->Screen);
            }
#endif
            phase = 0;
            break;
        }

    };

    movie->is_playing = 0;

    if (!movie->stop_playback)
    {
        // push event to trigger end of movie check in main thread
        event.user.code = 1;
        SDL_PushEvent(&event);
    }

#ifdef _WIN32
    _endthread();
#else
    return NULL;
#endif
}

// callback procedure for filling audio data
static void fill_audio(void *udata, Uint8 *stream, int len)
{
    Movie movie;
    int read_offset, write_offset, bytes_available1, bytes_available2;

    movie = (Movie)udata;
    read_offset = movie->audio_read_offset;
    write_offset = movie->audio_write_offset;

    if (read_offset == write_offset)
    {
        bytes_available1 = 0;
        bytes_available2 = 0;
    }
    else if (read_offset < write_offset)
    {
        bytes_available1 = write_offset - read_offset;
        if (bytes_available1 > len) bytes_available1 = len;
        bytes_available2 = 0;
    }
    else
    {
        bytes_available1 = movie->audio_buffer_size - read_offset;
        if (bytes_available1 >= len)
        {
            bytes_available1 = len;
            bytes_available2 = 0;
        }
        else
        {
            bytes_available2 = write_offset;
            if (bytes_available2 > len - bytes_available1) bytes_available2 = len - bytes_available1;
        }
    }

    if (bytes_available1)
    {
        SDL_memcpy(stream, &(movie->audio_buffer[read_offset]), bytes_available1);
        stream += bytes_available1;
        len -= bytes_available1;
        read_offset += bytes_available1;
        if (read_offset >= movie->audio_buffer_size) read_offset -= movie->audio_buffer_size;
        movie->audio_read_offset = read_offset;

        if (bytes_available2)
        {
            SDL_memcpy(stream, &(movie->audio_buffer[read_offset]), bytes_available2);
            stream += bytes_available2;
            len -= bytes_available2;
            read_offset += bytes_available2;
            if (read_offset >= movie->audio_buffer_size) read_offset -= movie->audio_buffer_size;
            movie->audio_read_offset = read_offset;
        }
    }

    if (len)
    {
        // silence
        SDL_memset(stream, movie->silence, len);
    }
}


//EXTERN_API( OSErr )
//InitializeQTML                  (long                   flag);
int16_t InitializeQTML_c (int32_t flag)
{
#ifdef DEBUG_QTML
    eprintf("InitializeQTML: 0x%x - ", flag);
#endif

    qthandle[0] = qthandle[1] = qthandle[2] = qthandle[3] = NULL;

#ifdef DEBUG_QTML
    eprintf("OK\n");
#endif
    return 0;
}

//EXTERN_API( void )
//TerminateQTML                   (void);
void TerminateQTML_c (void)
{
#ifdef DEBUG_QTML
    eprintf("TerminateQTML\n");
#endif

}


//EXTERN_API( GrafPtr )
//CreatePortAssociation           (void *                 theWnd,
//                                 Ptr                    storage,
//                                 long                   flags);
void *CreatePortAssociation_c (void *theWnd, void *storage, int32_t flags)
{
#ifdef DEBUG_QTML
    eprintf("CreatePortAssociation: 0x%x, 0x%x, 0x%x - ", (uintptr_t)theWnd, (uintptr_t)storage, flags);
#endif

#ifdef DEBUG_QTML
    eprintf("ok: 0x%x\n", 4);
#endif
    return (void *)4;
}

//EXTERN_API( void )
//DestroyPortAssociation          (CGrafPtr               cgp);
void DestroyPortAssociation_c (void *cgp)
{
#ifdef DEBUG_QTML
    eprintf("DestroyPortAssociation: 0x%x\n", (uintptr_t)cgp);
#endif

}


//EXTERN_API( void )
//SetGWorld                       (CGrafPtr               port,
//                                 GDHandle               gdh)                                FOURWORDINLINE(0x203C, 0x0008, 0x0006, 0xAB1D);
void SetGWorld_c (void *port, void *gdh)
{
#ifdef DEBUG_QTML
    eprintf("SetGWorld: 0x%x, 0x%x\n", (uintptr_t)port, (uintptr_t)gdh);
#endif

}

//EXTERN_API_C( StringPtr )
//c2pstr                          (char *                 aStr);
unsigned char *c2pstr_c (char *aStr)
{
#ifdef DEBUG_QTML
    eprintf("c2pstr: 0x%x (%s)- ", (uintptr_t)aStr, aStr);
#endif

#ifdef DEBUG_QTML
    eprintf("ok\n");
#endif
    return (unsigned char *)aStr;
}


//EXTERN_API( OSErr )
//PtrToHand                       (const void *           srcPtr,
//                                 Handle *               dstHndl,
//                                 long                   size);
int16_t PtrToHand_c (const void *srcPtr, void ***dstHndl, int32_t size)
{
#ifdef DEBUG_QTML
    char *str;
    str = (char *) malloc(size + 1);
    memcpy(str, srcPtr, size);
    str[size] = 0;

    eprintf("PtrToHand: 0x%x (%s), 0x%x, %i - ", (uintptr_t)srcPtr, str, (uintptr_t)dstHndl, size);

    free(str);
#endif

#ifdef DEBUG_QTML
    eprintf("ok: 0x%x\n", 5);
#endif
    *dstHndl = (void **)5;
    return 0;
}


//EXTERN_API( OSErr )
//FSMakeFSSpec                    (short                  vRefNum,
//                                 long                   dirID,
//                                 ConstStr255Param       fileName,
//                                 FSSpec *               spec)                               TWOWORDINLINE(0x7001, 0xAA52);
int16_t FSMakeFSSpec_c (int16_t vRefNum, int32_t dirID, char *fileName, void *spec)
{
#ifdef DEBUG_QTML
    eprintf("FSMakeFSSpec: %i, %i, 0x%x (%s), 0x%x - ", vRefNum, dirID, (uintptr_t)fileName, fileName, (uintptr_t)spec);
#endif

    ((FSSpec *)spec)->vRefNum = vRefNum;
    ((FSSpec *)spec)->parID = dirID;
    strncpy(((FSSpec *)spec)->name, fileName, 256);
    ((FSSpec *)spec)->name[255] = 0;

#ifdef DEBUG_QTML
    eprintf("ok\n");
#endif
    return 0;
}

//EXTERN_API( OSErr )
//QTSetDDPrimarySurface           (void *                 lpNewDDSurface,
//                                 unsigned long          flags);
int16_t QTSetDDPrimarySurface_c (void *lpNewDDSurface, uint32_t flags)
{
#ifdef DEBUG_QTML
    eprintf("QTSetDDPrimarySurface: 0x%x, 0x%x - ", (uintptr_t)lpNewDDSurface, flags);
#endif

    lpDDSurface = lpNewDDSurface;

#ifdef DEBUG_QTML
    eprintf("ok\n");
#endif
    return 0;
}

//EXTERN_API( long )
//NativeEventToMacEvent           (void *                 nativeEvent,
//                                 EventRecord *          macEvent);
int32_t NativeEventToMacEvent_c (void *nativeEvent, void *macEvent)
{
#ifdef DEBUG_QTML
    eprintf("NativeEventToMacEvent: 0x%x, 0x%x - ", (uintptr_t)nativeEvent, (uintptr_t)macEvent);
#endif

    ((EventRecord *)macEvent)->what = ((NativeEvent *)nativeEvent)->Msg;
    ((EventRecord *)macEvent)->message = ((NativeEvent *)nativeEvent)->lParam;
    ((EventRecord *)macEvent)->when = ((NativeEvent *)nativeEvent)->time;
    ((EventRecord *)macEvent)->where = (((NativeEvent *)nativeEvent)->x & 0xffff) | (((((NativeEvent *)nativeEvent)->y & 0xffff)) << 16);
    ((EventRecord *)macEvent)->modifiers = ((NativeEvent *)nativeEvent)->wParam;

#ifdef DEBUG_QTML
    eprintf("ok\n");
#endif
    return 0;
}


//EXTERN_API( OSErr )
//EnterMovies                     (void)                                                      TWOWORDINLINE(0x7001, 0xAAAA);
int16_t EnterMovies_c (void)
{
#ifdef DEBUG_QTML
    eprintf("EnterMovies - ");
#endif

#ifdef DEBUG_QTML
    eprintf("OK\n");
#endif
    return 0;
}

//EXTERN_API( void )
//ExitMovies                      (void)                                                      TWOWORDINLINE(0x7002, 0xAAAA);
void ExitMovies_c (void)
{
#ifdef DEBUG_QTML
    eprintf("ExitMovies\n");
#endif

}

//EXTERN_API( void )
//StartMovie                      (Movie                  theMovie)                           TWOWORDINLINE(0x700B, 0xAAAA);
void StartMovie_c (void *theMovie)
{
    Movie movie;
#ifndef _WIN32
    pthread_attr_t attr;
#endif

#ifdef DEBUG_QTML
    eprintf("StartMovie: 0x%x\n", (uintptr_t)theMovie);
#endif

    if (theMovie == NULL) return;

    movie = (Movie)theMovie;

    movie->is_playing = 0;
    movie->stop_playback = 0;


#ifdef _WIN32
    movie->is_playing = 1;
    if ((intptr_t)-1 == _beginthread(&movie_thread, 0, movie))
    {
        movie->is_playing = 0;
    }
#else
    if (0 != pthread_attr_init(&attr)) return;

    if (0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
    {
        pthread_attr_destroy(&attr);
        return;
    }

    movie->is_playing = 1;
    if (0 != pthread_create(&movie->thread_id, &attr, &movie_thread, movie))
    {
        movie->is_playing = 0;
    }

    pthread_attr_destroy(&attr);
#endif
}

//EXTERN_API( void )
//StopMovie                       (Movie                  theMovie)                           TWOWORDINLINE(0x700C, 0xAAAA);
void StopMovie_c (void *theMovie)
{
    Movie movie;

#ifdef DEBUG_QTML
    eprintf("StopMovie: 0x%x\n", (uintptr_t)theMovie);
#endif

    if (theMovie == NULL) return;

    movie = (Movie)theMovie;
    if (movie->play_audio)
    {
        SDL_LockAudio();
        SDL_PauseAudio(1);
        SDL_UnlockAudio();
    }

    if (movie->is_playing)
    {
        movie->stop_playback = 1;
        do
        {
            SDL_Delay(1);
        } while (movie->is_playing);
    }
}


//EXTERN_API( Boolean )
//IsMovieDone                     (Movie                  theMovie)                           THREEWORDINLINE(0x303C, 0x00DD, 0xAAAA);
uint8_t IsMovieDone_c (void *theMovie)
{
#ifdef DEBUG_QTML
    eprintf("IsMovieDone: 0x%x - ", (uintptr_t)theMovie);
#endif

    if (theMovie == NULL) return 1;

    if (((Movie)theMovie)->is_playing)
    {
#ifdef DEBUG_QTML
        eprintf("no\n");
#endif
        return 0;
    }
    else
    {
#ifdef DEBUG_QTML
        eprintf("yes\n");
#endif
        return 1;
    }
}

//EXTERN_API( void )
//DisposeMovie                    (Movie                  theMovie)                           TWOWORDINLINE(0x7023, 0xAAAA);
void DisposeMovie_c (void *theMovie)
{
#ifdef DEBUG_QTML
    eprintf("DisposeMovie: 0x%x\n", (uintptr_t)theMovie);
#endif

    if (theMovie == NULL) return;

    quicktime_close(((Movie)theMovie)->qt);
    free(theMovie);
}

//EXTERN_API( OSErr )
//OpenMovieFile                   (const FSSpec *         fileSpec,
//                                 short *                resRefNum,
//                                 SInt8                  permission)                         THREEWORDINLINE(0x303C, 0x0192, 0xAAAA);
int16_t OpenMovieFile_c (const void *fileSpec, int16_t *resRefNum, int8_t permission)
{
    int RefNum;
#ifndef _WIN32
    char buf[8192];
#endif

#ifdef DEBUG_QTML
    eprintf("OpenMovieFile: 0x%x (%s), 0x%x, 0x%x - ", (uintptr_t)fileSpec, ((FSSpec *)fileSpec)->name, (uintptr_t)resRefNum, permission);
#endif

    for (RefNum = 0; RefNum <= 3; RefNum++)
    {
        if (qthandle[RefNum] == NULL) break;
    }
    if (RefNum > 3)
    {
#ifdef DEBUG_QTML
        eprintf("error\n");
#endif
        return 1;
    }

    if (!Intro_Play)
    {
        const char *slash, *backslash, *name;

        name = ((FSSpec *)fileSpec)->name;

        backslash = strrchr(name, '\\');
        if (backslash != NULL)
        {
            name = backslash + 1;
        }

        slash = strrchr(name, '/');
        if (slash != NULL)
        {
            name = slash + 1;
        }

        if (0 == strcasecmp(name, "monolith.db")) return 1;
        if (0 == strcasecmp(name, "topware.db")) return 1;
        if (0 == strcasecmp(name, "valkyrie.db")) return 1;
        if (0 == strcasecmp(name, "m1.db")) return 1;
    }

#ifndef _WIN32
    if (!CLIB_FindFile(((FSSpec *)fileSpec)->name, buf))
    {
#ifdef DEBUG_QTML
        eprintf("error\n");
#endif
        return 1;
    }
#endif


#ifdef _WIN32
    qthandle[RefNum] = lqt_open_read(((FSSpec *)fileSpec)->name);
#else
    qthandle[RefNum] = lqt_open_read(buf);
#endif

    if (qthandle[RefNum] == NULL)
    {
#ifdef DEBUG_QTML
        eprintf("error\n");
#endif
        return 1;
    }

    if ((quicktime_video_tracks(qthandle[RefNum]) < 1) || !quicktime_supported_video(qthandle[RefNum], 0))
    {
        quicktime_close(qthandle[RefNum]);
        qthandle[RefNum] = NULL;
#ifdef DEBUG_QTML
        eprintf("error\n");
#endif
        return 1;
    }

    *resRefNum = RefNum + 1;

#ifdef DEBUG_QTML
    eprintf("ok: %i\n", RefNum + 1);
#endif

    return 0;
}

//EXTERN_API( OSErr )
//CloseMovieFile                  (short                  resRefNum)                          THREEWORDINLINE(0x303C, 0x00D5, 0xAAAA);
int16_t CloseMovieFile_c (int16_t resRefNum)
{
#ifdef DEBUG_QTML
    eprintf("CloseMovieFile: %i - ", resRefNum);
#endif

    if ((resRefNum < 1) || (resRefNum > 4) || (qthandle[resRefNum - 1] == NULL))
    {
#ifdef DEBUG_QTML
        eprintf("error\n");
#endif
        return 1;
    }

    if (qthandle[resRefNum - 1] != (void *)(intptr_t)-1)
    {
        quicktime_close(qthandle[resRefNum - 1]);
    }

    qthandle[resRefNum - 1] = NULL;

#ifdef DEBUG_QTML
    eprintf("OK\n");
#endif
    return 0;
}

//EXTERN_API( OSErr )
//NewMovieFromFile                (Movie *                theMovie,
//                                 short                  resRefNum,
//                                 short *                resId,
//                                 StringPtr              resName,
//                                 short                  newMovieFlags,
//                                 Boolean *              dataRefWasChanged)                  THREEWORDINLINE(0x303C, 0x00F0, 0xAAAA);
int16_t NewMovieFromFile_c (void **theMovie, int16_t resRefNum, int16_t *resId, unsigned char *resName, int16_t newMovieFlags, uint8_t *dataRefWasChanged)
{
    Movie movie;
    int color_models[5], model_index, use_yuv;

#ifdef DEBUG_QTML
    eprintf("NewMovieFromFile: 0x%x, %i, 0x%x, 0x%x, 0x%x, 0x%x - ", (uintptr_t)theMovie, resRefNum, (uintptr_t)resId, (uintptr_t)resName, newMovieFlags, (uintptr_t)dataRefWasChanged);
#endif

    if ((resRefNum < 1) || (resRefNum > 4) || (qthandle[resRefNum - 1] == NULL))
    {
#ifdef DEBUG_QTML
        eprintf("error\n");
#endif
        return 1;
    }

    movie = (Movie) malloc(sizeof(MovieRecord));
    if (movie == NULL)
    {
#ifdef DEBUG_QTML
        eprintf("error\n");
#endif
        return 1;
    }

    movie->qt = qthandle[resRefNum - 1];

    movie->frame = NULL;

#if SDL_VERSION_ATLEAST(2,0,0)
    movie->Renderer = NULL;
    movie->Video = NULL;
#else
    movie->Screen = SDL_GetVideoSurface();
    movie->Video = NULL;
    movie->Video2 = NULL;
    movie->Overlay = NULL;
#endif


    movie->video_width = quicktime_video_width(movie->qt, 0);
    movie->video_height = quicktime_video_height(movie->qt, 0);

    model_index = 0;
    use_yuv = 1;

#if defined(PANDORA) && !SDL_VERSION_ATLEAST(2,0,0)
    {
        char namebuf[8];
        if (NULL != SDL_VideoDriverName(namebuf, 8))
        {
            if (0 == strcmp(namebuf, "omapdss"))
            {
                // on Pandora, when using omapdss, the yuv overlays don't work
                use_yuv = 0;
            }
        }
    }
#endif

    if (use_yuv)
    {
        color_models[model_index++] = BC_YUV422; // SDL_YUY2_OVERLAY / SDL_PIXELFORMAT_YUY2 - packed mode
        color_models[model_index++] = BC_YUV420P; // SDL_IYUV_OVERLAY / SDL_PIXELFORMAT_IYUV - planar mode
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    color_models[model_index++] = BC_RGB565;
#else
    if (movie->Screen->format->BitsPerPixel == 16)
    {
        color_models[model_index++] = (movie->Screen->format->Rshift)?BC_RGB565:BC_BGR565;
    }
#endif

    color_models[model_index++] = BC_BGR8888;
    color_models[model_index++] = LQT_COLORMODEL_NONE;

    movie->colormodel = lqt_get_best_colormodel(movie->qt, 0, color_models);

#if SDL_VERSION_ATLEAST(2,0,0)
    switch(movie->colormodel)
    {
    case BC_YUV422:
        movie->format = SDL_PIXELFORMAT_YUY2;
        break;
    case BC_YUV420P:
        movie->format = SDL_PIXELFORMAT_IYUV;
        break;
    case BC_RGB565:
        movie->format = SDL_PIXELFORMAT_RGB565;
        break;
    case BC_BGR8888:
        movie->format = SDL_PIXELFORMAT_BGR888;
        break;
    default:
        movie->format = SDL_PIXELFORMAT_UNKNOWN;
        break;
    }
#endif

    qthandle[resRefNum - 1] = (quicktime_t *)(intptr_t)-1;

    *theMovie = movie;

#ifdef DEBUG_QTML
    eprintf("ok: 0x%x\n", (uintptr_t)movie);
#endif

    return 0;
}

//EXTERN_API( void )
//GetMovieBox                     (Movie                  theMovie,
//                                 Rect *                 boxRect)                            THREEWORDINLINE(0x303C, 0x00F9, 0xAAAA);
void GetMovieBox_c (void *theMovie, void *boxRect)
{
#ifdef DEBUG_QTML
    eprintf("GetMovieBox: 0x%x, 0x%x\n", (uintptr_t)theMovie, (uintptr_t)boxRect);
#endif

    if ((theMovie == NULL) || (boxRect == NULL)) return;

    ((Rect *)boxRect)->top = 0;
    ((Rect *)boxRect)->left = 0;
    ((Rect *)boxRect)->bottom = ((Movie)theMovie)->video_height;
    ((Rect *)boxRect)->right = ((Movie)theMovie)->video_width;
}


static void check_movie_audio(Movie movie)
{
    movie->play_audio = 0;
    movie->audio_buffer = NULL;
    movie->audio_temp_buffer[0] = NULL;
    movie->audio_read_offset = 0;
    movie->audio_write_offset = 0;

    if (quicktime_audio_tracks(movie->qt) <= 0) return;
    if (!quicktime_supported_audio(movie->qt, 0)) return;

    movie->audio_channels = quicktime_track_channels(movie->qt, 0);
    movie->audio_sample_rate = quicktime_sample_rate(movie->qt, 0);

    if (movie->audio_sample_rate > 48000) return;
    if ((movie->audio_channels != 1) &&
        (movie->audio_channels != 2)
#if SDL_VERSION_ATLEAST(2,0,0)
        &&
        (movie->audio_channels != 4) &&
        (movie->audio_channels != 6)
#endif
       ) return;

    switch (lqt_get_sample_format(movie->qt, 0))
    {
        case LQT_SAMPLE_INT8:
            movie->decode_raw_audio = 1;
            movie->audio_format = AUDIO_S8;
            movie->audio_format_size = 1;
            break;
        case LQT_SAMPLE_UINT8:
            movie->decode_raw_audio = 1;
            movie->audio_format = AUDIO_U8;
            movie->audio_format_size = 1;
            break;
        case LQT_SAMPLE_INT16:
            movie->decode_raw_audio = 1;
            movie->audio_format = AUDIO_S16SYS;
            movie->audio_format_size = 2;
            break;
#if SDL_VERSION_ATLEAST(2,0,0)
        case LQT_SAMPLE_INT32:
            movie->decode_raw_audio = 1;
            movie->audio_format = AUDIO_S32SYS;
            movie->audio_format_size = 4;
            break;
        case LQT_SAMPLE_FLOAT:
            movie->decode_raw_audio = 1;
            movie->audio_format = AUDIO_F32SYS;
            movie->audio_format_size = sizeof(float);
            break;
#else
        case LQT_SAMPLE_INT32:
        case LQT_SAMPLE_FLOAT:
#endif
        case LQT_SAMPLE_DOUBLE:
            movie->decode_raw_audio = 0;
            movie->audio_format = AUDIO_S16SYS;
            movie->audio_format_size = 2;
            break;

        case LQT_SAMPLE_UNDEFINED:
        default:
            return;
    }

    if (SDL_WasInit(SDL_INIT_AUDIO)) return;

    movie->audio_temp_buffer_samples = (movie->audio_sample_rate < 8000)?1024:(1024 * (movie->audio_sample_rate / 11025));
    movie->audio_buffer_size = 16 * movie->audio_temp_buffer_samples * movie->audio_format_size * movie->audio_channels;

    movie->audio_eof = 0;

    movie->audio_length = quicktime_audio_length(movie->qt, 0);

    movie->play_audio = 1;
}


//EXTERN_API( ComponentInstance )
//NewMovieController              (Movie                  theMovie,
//                                 const Rect *           movieRect,
//                                 long                   someFlags)                          THREEWORDINLINE(0x303C, 0x018A, 0xAAAA);
void *NewMovieController_c (void *theMovie, const void *movieRect, int32_t someFlags)
{
    Movie movie;
    const Rect *rect;

#ifdef DEBUG_QTML
    eprintf("NewMovieController: 0x%x, 0x%x, 0x%x - ", (uintptr_t)theMovie, (uintptr_t)movieRect, someFlags);
#endif

    if (theMovie == NULL) return NULL;

    movie = (Movie)theMovie;
    rect = (const Rect *)movieRect;

    movie->rowspan_y = 0;
    movie->rowspan_uv = 0;

    if (rect != NULL)
    {
        movie->dstrect.x = rect->left;
        movie->dstrect.y = rect->top;
        movie->dstrect.w = rect->right - rect->left;
        movie->dstrect.h = rect->bottom - rect->top;
    }
    else
    {
        movie->dstrect.x = 0;
        movie->dstrect.y = 0;
        movie->dstrect.w = movie->video_width;
        movie->dstrect.h = movie->video_height;
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    movie->frame = lqt_rows_alloc(movie->video_width, movie->video_height, movie->colormodel, &(movie->rowspan_y), &(movie->rowspan_uv));

    if (movie->frame == NULL)
    {
#ifdef DEBUG_QTML
        eprintf("error\n");
#endif
        return NULL;
    }

    movie->Renderer = GetSurfaceRenderer(lpDDSurface);

    movie->Video = SDL_CreateTexture(movie->Renderer, movie->format, SDL_TEXTUREACCESS_STREAMING, movie->video_width, movie->video_height);
    if (movie->Video == NULL)
    {
        lqt_rows_free(movie->frame);
        movie->frame = NULL;
#ifdef DEBUG_QTML
        eprintf("error\n");
#endif
        return NULL;
    }


#else
    movie->srcrect.x = 0;
    movie->srcrect.y = 0;
    movie->srcrect.w = movie->dstrect.w;
    movie->srcrect.h = movie->dstrect.h;

    if ((movie->colormodel == BC_YUV422) || (movie->colormodel == BC_YUV420P))
    {
        movie->Overlay = SDL_CreateYUVOverlay(movie->video_width, movie->video_height, (movie->colormodel == BC_YUV422)?SDL_YUY2_OVERLAY:SDL_IYUV_OVERLAY, movie->Screen);
        if (movie->Overlay == NULL)
        {
#ifdef DEBUG_QTML
            eprintf("error\n");
#endif
            return NULL;
        }

        movie->rowspan_y = movie->Overlay->pitches[0];
        if (movie->Overlay->planes > 1)
        {
            movie->rowspan_uv = movie->Overlay->pitches[1];
        }

        movie->frame = lqt_rows_alloc(movie->video_width, movie->video_height, movie->colormodel, &(movie->rowspan_y), &(movie->rowspan_uv));

        if (movie->frame == NULL)
        {
            SDL_FreeYUVOverlay(movie->Overlay);
            movie->Overlay = NULL;
#ifdef DEBUG_QTML
            eprintf("error\n");
#endif
            return NULL;
        }

    }
    else
    {
        movie->frame = lqt_rows_alloc(movie->video_width, movie->video_height, movie->colormodel, &(movie->rowspan_y), &(movie->rowspan_uv));

        if (movie->frame == NULL)
        {
#ifdef DEBUG_QTML
            eprintf("error\n");
#endif
            return NULL;
        }

        if (movie->colormodel == BC_RGB565)
        {
            movie->Video = SDL_CreateRGBSurfaceFrom(movie->frame[0], movie->video_width, movie->video_height, 16, movie->rowspan_y, 0xf800, 0x07e0, 0x001f, 0);
        }
        else if (movie->colormodel == BC_BGR565)
        {
            movie->Video = SDL_CreateRGBSurfaceFrom(movie->frame[0], movie->video_width, movie->video_height, 16, movie->rowspan_y, 0x001f, 0x07e0, 0xf800, 0);
        }
        else
        {
            movie->Video = SDL_CreateRGBSurfaceFrom(movie->frame[0], movie->video_width, movie->video_height, 32, movie->rowspan_y, 0xff0000, 0xff00, 0xff, 0);
        }

        if (movie->Video == NULL)
        {
            lqt_rows_free(movie->frame);
            movie->frame = NULL;
#ifdef DEBUG_QTML
            eprintf("error\n");
#endif
            return NULL;
        }

        if (movie->dstrect.w > movie->video_width)
        {
            movie->Video2 = SDL_CreateRGBSurface(
                (movie->Video->format->BitsPerPixel == 16)?movie->Screen->flags & SDL_HWSURFACE:0,
                movie->Video->w * 2,
                movie->Video->h * 2,
                movie->Video->format->BitsPerPixel,
                movie->Video->format->Rmask,
                movie->Video->format->Gmask,
                movie->Video->format->Bmask,
                movie->Video->format->Amask
            );
        }

    }

    lqt_set_cmodel(movie->qt, 0, movie->colormodel);
#endif

    check_movie_audio(movie);

    if (movie->play_audio)
    {
        // allocate buffers
        movie->audio_buffer = (uint8_t *)malloc(movie->audio_buffer_size);
        if (movie->audio_buffer == NULL)
        {
            movie->play_audio = 0;
#ifdef DEBUG_QTML
            eprintf("Error: Audio buffer allocation\n");
#endif
        }
        else if ((movie->audio_channels > 1) && !movie->decode_raw_audio)
        {
            movie->audio_temp_buffer[0] = (int16_t *)malloc(movie->audio_channels * movie->audio_temp_buffer_samples * sizeof(int16_t));
            if (movie->audio_temp_buffer[0] == NULL)
            {
                movie->play_audio = 0;
                free(movie->audio_buffer);
                movie->audio_buffer = NULL;
#ifdef DEBUG_QTML
                eprintf("Error: Audio buffer allocation\n");
#endif
            }
            else
            {
                int channel;
                for (channel = 1; channel <= movie->audio_channels; channel++)
                {
                    movie->audio_temp_buffer[channel] = movie->audio_temp_buffer[channel - 1] + movie->audio_temp_buffer_samples;
                }
            }
        }
    }

    if (movie->play_audio)
    {
        // init/open SDL audio
        if (0 != SDL_InitSubSystem(SDL_INIT_AUDIO))
        {
            movie->play_audio = 0;
            free(movie->audio_buffer);
            movie->audio_buffer = NULL;
            if (movie->audio_temp_buffer[0] != NULL)
            {
                free(movie->audio_temp_buffer[0]);
                movie->audio_temp_buffer[0] = NULL;
            }
#ifdef DEBUG_QTML
            eprintf("Error: SDL_InitSubSystem: Audio\n");
#endif
        }
        else
        {
            SDL_AudioSpec desired;

            desired.freq = movie->audio_sample_rate;
            desired.format = movie->audio_format;
            desired.channels = movie->audio_channels;
            desired.samples = movie->audio_temp_buffer_samples;
            desired.callback = &fill_audio;
            desired.userdata = movie;

            if (0 != SDL_OpenAudio(&desired, NULL))
            {
                movie->play_audio = 0;
                SDL_QuitSubSystem(SDL_INIT_AUDIO);
                free(movie->audio_buffer);
                movie->audio_buffer = NULL;
                if (movie->audio_temp_buffer[0] != NULL)
                {
                    free(movie->audio_temp_buffer[0]);
                    movie->audio_temp_buffer[0] = NULL;
                }
#ifdef DEBUG_QTML
                eprintf("Error: SDL_OpenAudio\n");
#endif
            }
            else
            {
                movie->silence = desired.silence;
            }
        }
    }

#ifdef DEBUG_QTML
    eprintf("ok: 0x%x\n", (uintptr_t)movie);
#endif
    return movie;
}

//EXTERN_API( void )
//DisposeMovieController          (ComponentInstance      mc)                                 THREEWORDINLINE(0x303C, 0x018B, 0xAAAA);
void DisposeMovieController_c (void *mc)
{
    Movie movie;

#ifdef DEBUG_QTML
    eprintf("DisposeMovieController: 0x%x\n", (uintptr_t)mc);
#endif

    if (mc == NULL) return;

    movie = (Movie)mc;

    if (movie->play_audio)
    {
        SDL_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);

        if (movie->audio_temp_buffer[0] != NULL)
        {
            free(movie->audio_temp_buffer[0]);
            movie->audio_temp_buffer[0] = NULL;
        }
        if (movie->audio_buffer != NULL)
        {
            free(movie->audio_buffer);
            movie->audio_buffer = NULL;
        }
    }

#if SDL_VERSION_ATLEAST(2,0,0)
    if (movie->Video != NULL) SDL_DestroyTexture(movie->Video);
#else
    if (movie->Overlay != NULL) SDL_FreeYUVOverlay(movie->Overlay);
    if (movie->Video2 != NULL) SDL_FreeSurface(movie->Video2);
    if (movie->Video != NULL) SDL_FreeSurface(movie->Video);
#endif
    lqt_rows_free(movie->frame);
}

//EXTERN_API( OSErr )
//QTRegisterAccessKey             (Str255                 accessKeyType,
//                                 long                   flags,
//                                 Handle                 accessKey)                          THREEWORDINLINE(0x303C, 0x02B4, 0xAAAA);
int16_t QTRegisterAccessKey_c (unsigned char *accessKeyType, int32_t flags, void *accessKey)
{
#ifdef DEBUG_QTML
    eprintf("QTRegisterAccessKey: 0x%x (%s), 0x%x, 0x%x - ", (uintptr_t)accessKeyType, accessKeyType, flags, (uintptr_t)accessKey);
#endif

#ifdef DEBUG_QTML
    eprintf("ok\n");
#endif
    return 0;
}

//EXTERN_API( OSErr )
//QTUnregisterAccessKey           (Str255                 accessKeyType,
//                                 long                   flags,
//                                 Handle                 accessKey)                          THREEWORDINLINE(0x303C, 0x02B5, 0xAAAA);
int16_t QTUnregisterAccessKey_c (unsigned char *accessKeyType, int32_t flags, void *accessKey)
{
#ifdef DEBUG_QTML
    eprintf("QTUnregisterAccessKey: 0x%x (%s), 0x%x, 0x%x - ", (uintptr_t)accessKeyType, accessKeyType, flags, (uintptr_t)accessKey);
#endif

#ifdef DEBUG_QTML
    eprintf("ok\n");
#endif
    return 0;
}


//EXTERN_API( ComponentResult )
//MCIsPlayerEvent                 (MovieController        mc,
//                                 const EventRecord *    e)                                  FIVEWORDINLINE(0x2F3C, 0x0004, 0x0007, 0x7000, 0xA82A);
void *MCIsPlayerEvent_c (void *mc, const void *e)
{
#ifdef DEBUG_QTML
    eprintf("MCIsPlayerEvent: 0x%x, 0x%x - ", (uintptr_t)mc, (uintptr_t)e);
#endif

    if ((mc != NULL) && (e != NULL))
    {
        const EventRecord *event;
        Movie movie;

        event = (const EventRecord *)e;
        movie = (Movie)mc;

#if SDL_VERSION_ATLEAST(2,0,0)
        if ((event->what == 0x8000)) // WM_APP
        {
            if (movie->rowspan_uv > 0)
            {
                SDL_UpdateYUVTexture(movie->Video, NULL, (const Uint8*) movie->frame[0], movie->rowspan_y, (const Uint8*) movie->frame[1], movie->rowspan_uv, (const Uint8*) movie->frame[2], movie->rowspan_uv);
            }
            else
            {
                CopyFrameToTexture(movie);
                //SDL_UpdateTexture(movie->Video, NULL, movie->frame[0], movie->rowspan_y);
            }

            SDL_RenderCopy(movie->Renderer, movie->Video, NULL, &(movie->dstrect));

            movie->updated = 1;
        }
        else if ((event->what == 0x8001)) // WM_APP+1
        {
            SDL_RenderPresent(movie->Renderer);

            // clear next frame
            SDL_SetRenderDrawColor(movie->Renderer, 0, 0, 0, 255);
            SDL_RenderClear(movie->Renderer);

            movie->flipped = 1;
        }
        else
#endif
        if ((event->what == 0x0012)) // WM_QUIT
        {
            if (movie->play_audio)
            {
                SDL_LockAudio();
                SDL_PauseAudio(1);
                SDL_UnlockAudio();
            }

            movie->stop_playback = 1;
            while (movie->is_playing)
            {
                SDL_Delay(1);
            };
        }
    }

#ifdef DEBUG_QTML
    eprintf("ok\n");
#endif
    return NULL;
}

//EXTERN_API( ComponentResult )
//MCDoAction                      (MovieController        mc,
//                                 short                  action,
//                                 void *                 params)                             FIVEWORDINLINE(0x2F3C, 0x0006, 0x0009, 0x7000, 0xA82A);
void *MCDoAction_c (void *mc, int16_t action, void *params)
{
#ifdef DEBUG_QTML
    eprintf("MCDoAction: 0x%x, %i, 0x%x - ", (uintptr_t)mc, action, (uintptr_t)params);
#endif

#ifdef DEBUG_QTML
    eprintf("ok\n");
#endif
    return NULL;
}

