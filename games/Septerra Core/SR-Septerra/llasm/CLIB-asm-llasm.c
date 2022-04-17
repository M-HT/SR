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

#include <inttypes.h>
#include "CLIB-asm-llasm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "printf_x86.h"


#define eprintf(...) fprintf(stderr,__VA_ARGS__)


int32_t sprintf2_c(char *str, const char *format, uint32_t *ap)
{
    int res;

#ifdef DEBUG_CLIB
    eprintf("sprintf: 0x%" PRIxPTR ", 0x%" PRIxPTR " (%s) - ", (uintptr_t) str, (uintptr_t) format, format);
#endif

    res = vsprintf_x86(str, format, ap);

#ifdef DEBUG_CLIB
    eprintf("%i (%s)\n", res, str);
#endif

    return res;
}

int32_t sscanf2_c(const char *str, const char *format, uint32_t *ap)
{
    int res, num, index;
    uintptr_t values[2];

#ifdef DEBUG_CLIB
    eprintf("sscanf: 0x%" PRIxPTR " (%s), 0x%" PRIxPTR " (%s) - ", (uintptr_t) str, str, (uintptr_t) format, format);
#endif

    num = 0;
    if (format != NULL)
    {
        for (index = 0; format[index] != 0; index++)
        {
            if (format[index] == '%')
            {
                num++;

                if (format[index + 1] != 's' && format[index + 1] != 'd')
                {
                    eprintf("sscanf: unsupported format: %s\n", format);
                    exit(1);
                }
            }
        }
    }

    switch (num)
    {
        case 1:
            res = sscanf(str, format, &(values[0]));
            break;
        case 2:
            res = sscanf(str, format, &(values[0]), &(values[1]));
            break;
        default:
            eprintf("sscanf: unsupported format: %s\n", format);
            exit(1);
    }

    for (index = 0; index < res; index++)
    {
        ap[index] = (uint32_t)values[index];
    }

#ifdef DEBUG_CLIB
    eprintf("%i\n", res);
#endif

    return res;
}


void _time642_c(uint32_t *t64)
{
    time_t t;
    uint64_t i;

#ifdef DEBUG_CLIB
    eprintf("_time64\n");
#endif

    t = time(NULL);
    i = t;

    t64[0] = (uint32_t)i;
    t64[1] = (uint32_t)(i >> 32);
}

