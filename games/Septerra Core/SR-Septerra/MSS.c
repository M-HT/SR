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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "MSS.h"

#include <mpg123.h>


#define ASI_NOERR                   0   // Success -- no error
#define ASI_NOT_ENABLED             1   // ASI not enabled
#define ASI_ALREADY_STARTED         2   // ASI already started
#define ASI_INVALID_PARAM           3   // Invalid parameters used
#define ASI_INTERNAL_ERR            4   // Internal error in ASI driver
#define ASI_OUT_OF_MEM              5   // Out of system RAM
#define ASI_ERR_NOT_IMPLEMENTED     6   // Feature not implemented
#define ASI_NOT_FOUND               7   // ASI supported device not found
#define ASI_NOT_INIT                8   // ASI not initialized
#define ASI_CLOSE_ERR               9   // ASI not closed correctly


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


static int MSS_initialized = 0;


uint32_t ASI_startup_c (void)
{
#ifdef DEBUG_MSS
    eprintf("ASI_startup - ");
#endif

    if (MSS_initialized)
    {
        MSS_initialized++;
#ifdef DEBUG_MSS
        eprintf("%i (already initialized)\n", ASI_ALREADY_STARTED);
#endif
        return ASI_ALREADY_STARTED;
    }

    if (mpg123_init() != MPG123_OK)
    {
#ifdef DEBUG_MSS
        eprintf("%i (mpg123 error)\n", ASI_INTERNAL_ERR);
#endif
        return ASI_INTERNAL_ERR;
    }

    MSS_initialized = 1;

#ifdef DEBUG_MSS
    eprintf("OK\n");
#endif
    return ASI_NOERR;
}


uint32_t ASI_shutdown_c (void)
{
#ifdef DEBUG_MSS
    eprintf("ASI_shutdown - ");
#endif

    if (!MSS_initialized)
    {
#ifdef DEBUG_MSS
        eprintf("%i (not initialized)\n", ASI_NOT_INIT);
#endif
        return ASI_NOT_INIT;
    }

    MSS_initialized--;

    if (!MSS_initialized)
    {
        mpg123_exit();
    }

#ifdef DEBUG_MSS
    eprintf("OK\n");
#endif
    return ASI_NOERR;
}


void *ASI_stream_open_mpg123 (void *user, ssize_t (*read_CB)(void *, void *, size_t), off_t (*lseek_CB)(void *, off_t,  int))
{
#ifdef DEBUG_MSS
    eprintf("ASI_stream_open_mpg123: 0x%x, 0x%x, 0x%x - ", (uintptr_t) user, (uintptr_t) read_CB, (uintptr_t) lseek_CB);
#endif

    if (!MSS_initialized)
    {
#ifdef DEBUG_MSS
        eprintf("%i (not initialized)\n", 0);
#endif
        return NULL;
    }

    mpg123_handle *stream;

    stream = mpg123_new(NULL, NULL);
    if (stream == NULL)
    {
#ifdef DEBUG_MSS
        eprintf("mpg123 error - new\n");
#endif
        return NULL;
    }

#ifdef DEBUG_MSS
    eprintf("0x%x - ", (uintptr_t) stream);
#endif

    if (mpg123_param(stream, MPG123_ADD_FLAGS, MPG123_SKIP_ID3V2 /*| MPG123_NO_PEEK_END*/, 0.0) != MPG123_OK)
    {
        mpg123_delete(stream);
#ifdef DEBUG_MSS
        eprintf("mpg123 error - param\n");
#endif
        return NULL;
    }

    if (mpg123_replace_reader_handle(stream, read_CB, lseek_CB, NULL) != MPG123_OK)
    {
        mpg123_delete(stream);
#ifdef DEBUG_MSS
        eprintf("mpg123 error - replace_reader_handle\n");
#endif
        return NULL;
    }

    if (mpg123_open_handle(stream, user) != MPG123_OK)
    {
        mpg123_delete(stream);
#ifdef DEBUG_MSS
        eprintf("mpg123 error - open_handle\n");
#endif
        return NULL;
    }

    // settings for Septerra Core
    mpg123_format_none(stream);
    mpg123_format(stream, 44100, MPG123_MONO, MPG123_ENC_SIGNED_16);

    lseek_CB(user, 0, SEEK_SET);

#ifdef DEBUG_MSS
    eprintf("OK\n");
#endif
    return stream;
}


uint32_t ASI_stream_close_c (void *stream)
{
#ifdef DEBUG_MSS
    eprintf("ASI_stream_close: 0x%x - ", (uintptr_t) stream);
#endif

    if (!MSS_initialized)
    {
#ifdef DEBUG_MSS
        eprintf("%i (not initialized)\n", ASI_NOT_INIT);
#endif
        return ASI_NOT_INIT;
    }

    if (stream != NULL)
    {
        mpg123_close((mpg123_handle *)stream);
        mpg123_delete((mpg123_handle *)stream);
    }

#ifdef DEBUG_MSS
    eprintf("OK\n");
#endif
    return ASI_NOERR;
}


int32_t ASI_stream_process_c (void *stream, void *buffer, int32_t request_size)
{
#ifdef DEBUG_MSS
    eprintf("ASI_stream_process: 0x%x, 0x%x, %i - ", (uintptr_t) stream, (uintptr_t) buffer, request_size);
#endif

    if (!MSS_initialized)
    {
#ifdef DEBUG_MSS
        eprintf("%i (not initialized)\n", 0);
#endif
        return 0;
    }

    if (stream == NULL)
    {
#ifdef DEBUG_MSS
        eprintf("0 (stream == NULL)\n");
#endif
        return 0;
    }

    int32_t original_size = request_size;
    unsigned char *buf = (unsigned char *)buffer;

    while (request_size > 0)
    {
        size_t done = 0;
        int res = mpg123_read((mpg123_handle *)stream, buf, request_size, &done);

        request_size -= done;
        buf += done;

        if (res == MPG123_NEW_FORMAT)
        {
            long rate;
            int channels, encoding;

            if (mpg123_getformat((mpg123_handle *)stream, &rate, &channels, &encoding) != MPG123_OK)
            {
                break;
            }
            else
            {
#ifdef DEBUG_MSS
                eprintf("(%i, %i, %i) - ", (int)rate, channels, encoding);
#endif
                continue;
            }
        }

        if (res != MPG123_OK)
        {
            break;
        }
    }

    if (request_size > 0)
    {
        memset(buf, 0, request_size);
    }

#ifdef DEBUG_MSS
    eprintf("%i\n", original_size - request_size);
#endif
    return original_size - request_size;
}

