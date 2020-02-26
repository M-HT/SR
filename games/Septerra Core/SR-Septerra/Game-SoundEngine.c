/**
 *
 *  Copyright (C) 2019-2020 Roman Pauer
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
#include <unistd.h>
#include <string.h>
#include "Game-SoundEngine.h"
#include "MSS.h"
#include "Game-DataFiles.h"
#include "WinApi-kernel32.h"

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#endif

typedef struct _SE_struc_4 // final size
{
    struct _SE_struc_4 *Next;
    int32_t RecordHandle;
    uint32_t RecordSize;
    uint8_t *RecordData;
    uint32_t se4_f10;
    int32_t RecordOffset;
    uint32_t RecordKey;
    uint32_t BufferOffset;
    uint32_t BufferAvailable;
    uint32_t se4_f24;
    uint8_t *Buffer;
    int32_t RecordSize_16;
    uint32_t se4_f30;
    uint16_t se4_f34;
    uint16_t se4_f36;
    uint16_t se4_f38;
    uint8_t se4_f3A;
    uint8_t se4_f3B;
    uint8_t se4_f3C;
    uint8_t se4_f3D;
    uint8_t se4_f3E;
    uint8_t se4_f3F;
    uint8_t se4_f40;
    uint8_t se4_f41;
    uint8_t se4_f42;
    uint8_t se4_f43;
    uint32_t se4_f44;
    uint32_t se4_f48;
    uint32_t se4_f4C;
    uint32_t se4_f50;
} SE_struc_4;


typedef struct _SE_struc_9 // final size
{
    struct _SE_struc_9 *Next;
    struct _SE_struc_3 *data9_3;
    uint32_t se9_f08;
    struct _SE_struc_4 *data9_4;
    uint32_t se9_f10;
    uint8_t *Buffer;
    int32_t RecordOffset;
    void *Stream;
    uint32_t se9_f20;
    uint32_t BufferAvailable;
    uint32_t BufferOffset;
    uint32_t se9_f2C;
    int32_t se9_f30;
    uint32_t se9_f34;
    uint32_t se9_f38;
    uint32_t se9_f3C;
    uint32_t se9_f40;
    uint32_t se9_f44;
    uint32_t se9_f48;
    uint32_t se9_f4C;
    uint32_t se9_f50;
    uint32_t se9_f54;
    uint32_t se9_f58;
    uint32_t se9_f5C;
    uint32_t se9_f60;
    uint32_t se9_f64;
    uint32_t se9_f68;
    uint32_t se9_f6C;
    uint32_t se9_f70;
    uint8_t se9_f74;
    uint8_t se9_f75;
    uint8_t se9_f76;
    uint8_t se9_f77;
    uint32_t se9_f78;
    uint32_t se9_f7C;
    uint32_t se9_f80;
} SE_struc_9;


extern void *hWritePipe;


#ifdef _WIN32
// sub_4642A0
static void CALLBACK SE_fptc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    int32_t PipeValue = 3;
    uint32_t NumberOfBytesWritten;
    WriteFile_c(hWritePipe, &PipeValue, 4, &NumberOfBytesWritten, NULL);
}
#else
static pthread_t timer_thread_id;

static void *SE_timer_CB(void *arg)
{
    struct timespec timer_time;
    int32_t PipeValue = 3;
    uint32_t NumberOfBytesWritten;

    clock_gettime(CLOCK_MONOTONIC, &timer_time);

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    timer_time.tv_nsec += 20000000;
    if (timer_time.tv_nsec >= 1000000000)
    {
        timer_time.tv_nsec -= 1000000000;
        timer_time.tv_sec++;
    }

    while (1)
    {
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_testcancel();
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

        if (0 != clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timer_time, NULL)) continue;

        timer_time.tv_nsec += 20000000;
        if (timer_time.tv_nsec >= 1000000000)
        {
            timer_time.tv_nsec -= 1000000000;
            timer_time.tv_sec++;
        }

        WriteFile_c(hWritePipe, &PipeValue, 4, &NumberOfBytesWritten, NULL);
    };

    return NULL;
}
#endif

uint32_t SoundEngine_StartTimer(void)
{
#ifdef _WIN32
    return timeSetEvent(20, 4, &SE_fptc, (DWORD_PTR)0, TIME_PERIODIC);
#else
    pthread_attr_t attr;

    if (0 != pthread_attr_init(&attr)) return 0;

    if (0 != pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE))
    {
        pthread_attr_destroy(&attr);
        return 0;
    }

    if (0 != pthread_create(&timer_thread_id, &attr, &SE_timer_CB, NULL))
    {
        pthread_attr_destroy(&attr);
        return 0;
    }

    pthread_attr_destroy(&attr);
    return 1;
#endif
}

void SoundEngine_StopTimer(uint32_t uTimerID)
{
#ifdef _WIN32
    timeKillEvent(uTimerID);
#else
    pthread_cancel(timer_thread_id);
    pthread_join(timer_thread_id, NULL);
#endif
}

static ssize_t SE_read_CB_4(SE_struc_4 *struc4, uint8_t *buf, ssize_t count)
{
    int32_t bytes_returned;
    uint32_t bytes_to_read;
    uint32_t bytes_read;
    int32_t bytes_to_copy;

    bytes_returned = 0;

    if (count > 0)
    {
        do
        {
            if (struc4->BufferOffset < struc4->BufferAvailable)
            {
                bytes_to_copy = struc4->BufferAvailable - struc4->BufferOffset;
                if (bytes_to_copy >= count)
                {
                    bytes_to_copy = count;
                }

                memcpy(&(buf[bytes_returned]), &(struc4->Buffer[struc4->BufferOffset]), bytes_to_copy);

                count -= bytes_to_copy;
                bytes_returned += bytes_to_copy;
                struc4->BufferOffset += bytes_to_copy;
            }

            if (count <= 0) break;

            RecordSeek(struc4->RecordHandle, struc4->RecordOffset + 16, 0);

            bytes_to_read = 1024;
            if (struc4->RecordOffset + 1024 > struc4->RecordSize_16)
            {
                bytes_to_read = (struc4->RecordSize_16 - struc4->RecordOffset < 0) ? 0 : struc4->RecordSize_16 - struc4->RecordOffset;
            }

            bytes_read = RecordRead(struc4->RecordHandle, struc4->Buffer, bytes_to_read);

            struc4->BufferOffset = 0;
            struc4->RecordOffset += bytes_read;
            struc4->BufferAvailable = bytes_read;
        }
        while (bytes_read != 0);
    }

    return bytes_returned;
}

static off_t SE_lseek_CB_4(SE_struc_4 *struc4, off_t offset, int whence)
{
    uint32_t bytes_to_read;
    uint32_t bytes_read;

    switch (whence)
    {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            offset += struc4->RecordOffset;
            if (struc4->BufferOffset < struc4->BufferAvailable)
            {
                offset -= struc4->BufferAvailable - struc4->BufferOffset;
            }
            break;
        case SEEK_END:
            offset += struc4->RecordSize_16;
            break;
        default:
            return -1;
    }

    if (offset < 0) offset = 0;
    else if (offset > struc4->RecordSize_16) offset = struc4->RecordSize_16;

    if ((offset < struc4->RecordOffset - 1024) || (offset >= struc4->RecordOffset))
    {
        struc4->RecordOffset = offset;

        bytes_to_read = 1024;
        if (offset + 1024 > struc4->RecordSize_16)
        {
            bytes_to_read = (struc4->RecordSize_16 - offset < 0) ? 0 : struc4->RecordSize_16 - offset;
        }

        RecordSeek(struc4->RecordHandle, offset + 16, 0);
        bytes_read = RecordRead(struc4->RecordHandle, struc4->Buffer, bytes_to_read);
        struc4->BufferOffset = 0;
        struc4->RecordOffset += bytes_read;
        struc4->BufferAvailable = bytes_read;
    }
    else
    {
        struc4->BufferOffset = offset - (struc4->RecordOffset - 1024);
    }

    return offset;
}

// sub_468130
void SoundEngine_DecodeMP3Stream(struct _SE_struc_4 *struc4)
{
    uint32_t record_size;
    void *stream;

    record_size = struc4->RecordSize;

    struc4->Buffer = (uint8_t *) malloc(1024);
    struc4->RecordOffset = 0;
    struc4->BufferOffset = 0;
    struc4->BufferAvailable = 0;

    stream = ASI_stream_open_mpg123(struc4, (ssize_t (*)(void *, void *, size_t)) &SE_read_CB_4, (off_t (*)(void *, off_t,  int)) &SE_lseek_CB_4);
    ASI_stream_process_c(stream, struc4->RecordData, record_size);
    ASI_stream_close_c(stream);

    free(struc4->Buffer);
    struc4->Buffer = NULL;
}


static ssize_t SE_read_CB_9(SE_struc_9 *struc9, uint8_t *buf, ssize_t count)
{
    int32_t RecordHandle;
    int32_t RecordSize_16;
    uint32_t bytes_to_read;
    uint32_t bytes_read;
    int32_t bytes_to_copy;
    int32_t bytes_returned;

    bytes_returned = 0;
    RecordHandle = struc9->data9_4->RecordHandle;
    RecordSize_16 = struc9->data9_4->RecordSize_16;

    if ( count > 0 )
    {
        do
        {
            if (struc9->BufferOffset < struc9->BufferAvailable)
            {
                bytes_to_copy = struc9->BufferAvailable - struc9->BufferOffset;
                if (bytes_to_copy >= count)
                {
                    bytes_to_copy = count;
                }
                count -= bytes_to_copy;

                memcpy(&(buf[bytes_returned]), &(struc9->Buffer[struc9->BufferOffset]), bytes_to_copy);

                bytes_returned += bytes_to_copy;
                struc9->BufferOffset += bytes_to_copy;
            }

            if (count <= 0) break;

            bytes_to_read = 1024;
            if (struc9->RecordOffset + 1024 > RecordSize_16)
            {
                if (RecordSize_16 - struc9->RecordOffset <= 0) break;
                bytes_to_read = RecordSize_16 - struc9->RecordOffset;
            }

            RecordSeek(RecordHandle, struc9->RecordOffset + 16, 0);
            bytes_read = RecordRead(RecordHandle, struc9->Buffer, bytes_to_read);

            struc9->BufferOffset = 0;
            struc9->RecordOffset += bytes_read;
            struc9->BufferAvailable = bytes_read;
        }
        while (bytes_read != 0);
    }

    return bytes_returned;
}

static off_t SE_lseek_CB_9(SE_struc_9 *struc9, off_t offset, int whence)
{
    int32_t RecordHandle;
    int32_t RecordSize_16;
    uint32_t bytes_to_read;
    uint32_t bytes_read;

    RecordHandle = struc9->data9_4->RecordHandle;
    RecordSize_16 = struc9->data9_4->RecordSize_16;

    switch (whence)
    {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            offset += struc9->RecordOffset;
            if (struc9->BufferOffset < struc9->BufferAvailable)
            {
                offset -= struc9->BufferAvailable - struc9->BufferOffset;
            }
            break;
        case SEEK_END:
            offset += RecordSize_16;
            break;
        default:
            return -1;
    }

    if (offset < 0) offset = 0;
    else if (offset > RecordSize_16) offset = RecordSize_16;

    if ( offset < struc9->RecordOffset - 1024 || offset >= struc9->RecordOffset )
    {
        struc9->RecordOffset = offset;

        bytes_to_read = 1024;
        if ( offset + 1024 > RecordSize_16 )
        {
            bytes_to_read = RecordSize_16 - offset;
        }

        RecordSeek(RecordHandle, offset + 16, 0);
        bytes_read = RecordRead(RecordHandle, struc9->Buffer, bytes_to_read);
        struc9->BufferOffset = 0;
        struc9->RecordOffset += bytes_read;
        struc9->BufferAvailable = bytes_read;
    }
    else
    {
        struc9->BufferOffset = offset - (struc9->RecordOffset - 1024);
    }

    return offset;
}

// sub_468D10
void SoundEngine_OpenMP3Stream(struct _SE_struc_9 *struc9)
{
    if (struc9->Stream != NULL)
    {
        ASI_stream_close_c(struc9->Stream);
    }

    if (struc9->Buffer == NULL)
    {
        struc9->Buffer = (uint8_t *) malloc(1024);
    }

    struc9->RecordOffset = 0;
    struc9->Stream = ASI_stream_open_mpg123(struc9, (ssize_t (*)(void *, void *, size_t)) &SE_read_CB_9, (off_t (*)(void *, off_t,  int)) &SE_lseek_CB_9);
}

