/**
 *
 *  Copyright (C) 2016 Roman Pauer
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

#include <stdio.h>
#include <stdarg.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Intro-proc-stack.h"

int Game_printf(CALL_PARAMS1 const char *format, ...)
{
	va_list ap;
	int ret;

	va_start(ap, format);

	ret = vprintf(format, ap);

	va_end(ap);

	return ret;
}

int Game_printf_ptr(uint32_t *params)
{
    int num;
    const char *str;

    num = 0;
    str = (const char *) params[0];
    if (str != NULL)
    {
        while (*str != 0)
        {
            if (*str == '%') num++;
            str++;
        }
    }

    switch (num)
    {
        case 0:
            return printf((const char *) params[0]);
        case 1:
            return printf((const char *) params[0], params[1]);
        case 2:
            return printf((const char *) params[0], params[1], params[2]);
        case 3:
            return printf((const char *) params[0], params[1], params[2], params[3]);
        case 4:
            return printf((const char *) params[0], params[1], params[2], params[3], params[4]);
        case 5:
            return printf((const char *) params[0], params[1], params[2], params[3], params[4], params[5]);
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            return printf((const char *) params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10]);
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            return printf((const char *) params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11], params[12], params[13], params[14], params[15]);
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            return printf((const char *) params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8], params[9], params[10], params[11], params[12], params[13], params[14], params[15], params[16], params[17], params[18], params[19], params[20]);
        default:
            // at most 20 parameters supported
            return -1;
    }
}

