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

#if defined(__DEBUG__) && defined(DEBUG_CLIB)
#include <inttypes.h>
#endif
#include "CLIB-proc.h"
#include <stdio.h>
#include "printf_x86.h"


static void file_out(char character, void* arg)
{
#if defined(__DEBUG__) && defined(DEBUG_CLIB)
    if (arg != stderr)
    {
        fputc(character, stderr);
    }
#endif

    fputc(character, (FILE *)arg);
}

int32_t CLIB_vprintf(const char *format, uint32_t *ap)\
{
    int res;

#if defined(__DEBUG__) && defined(DEBUG_CLIB)
    fprintf(stderr, "vprintf: 0x%" PRIxPTR " (%s) - (", (uintptr_t) format, format);
#endif

    res = vfctprintf_x86(file_out, stdout, format, ap);

#if defined(__DEBUG__) && defined(DEBUG_CLIB)
    fprintf(stderr, ") %i\n", res);
#endif

    return res;
}

int32_t CLIB_vfprintf(void *stream, const char *format, uint32_t *ap)
{
    int res;

#if defined(__DEBUG__) && defined(DEBUG_CLIB)
    if (stream != stderr)
    {
        fprintf(stderr, "vfprintf: 0x%" PRIxPTR ", 0x%" PRIxPTR " (%s) - (", (uintptr_t) stream, (uintptr_t) format, format);
    }
#endif

    res = vfctprintf_x86(file_out, stream, format, ap);

#if defined(__DEBUG__) && defined(DEBUG_CLIB)
    if (stream != stderr)
    {
        fprintf(stderr, ") %i\n", res);
    }
#endif

    return res;
}

int32_t CLIB_vsprintf(char *str, const char *format, uint32_t *ap)
{
    int res;

#if defined(__DEBUG__) && defined(DEBUG_CLIB)
    fprintf(stderr, "vsprintf: 0x%" PRIxPTR ", 0x%" PRIxPTR " (%s) - ", (uintptr_t) str, (uintptr_t) format, format);
#endif

    res = vsprintf_x86(str, format, ap);

#if defined(__DEBUG__) && defined(DEBUG_CLIB)
    fprintf(stderr, "%i (%s)\n", res, str);
#endif

    return res;
}

int32_t CLIB_vsnprintf(char *str, uint32_t size, const char *format, uint32_t *ap)
{
    int res;

#if defined(__DEBUG__) && defined(DEBUG_CLIB)
    fprintf(stderr, "vsnprintf: 0x%" PRIxPTR ", %i, 0x%" PRIxPTR " (%s) - ", (uintptr_t) str, size, (uintptr_t) format, format);
#endif

    res = vsnprintf_x86(str, size, format, ap);

#if defined(__DEBUG__) && defined(DEBUG_CLIB)
    fprintf(stderr, "%i (%s)\n", res, str);
#endif

    return res;
}

