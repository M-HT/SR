/**
 *
 *  Copyright (C) 2019-2026 Roman Pauer
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

#if !defined(_MSS_H_INCLUDED_)
#define _MSS_H_INCLUDED_

#include <stdint.h>
#include <sys/types.h>
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t CCALL ASI_startup_c (void);
uint32_t CCALL ASI_shutdown_c (void);
uint32_t CCALL ASI_stream_close_c (void *stream);
int32_t CCALL ASI_stream_process_c (void *stream, void *buffer, int32_t request_size);

#ifdef __cplusplus
}
#endif

void *ASI_stream_open_mpg123 (void *user, ssize_t (*read_CB)(void *, void *, size_t), off_t (*lseek_CB)(void *, off_t,  int));

#endif

