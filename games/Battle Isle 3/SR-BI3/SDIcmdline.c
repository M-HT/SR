/**
 *
 *  Copyright (C) 2021 Roman Pauer
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

#include "SDIcmdline.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


int cmdline_ContainsOption(int option)
{
    const char *curptr;
    char option_upper;

    curptr = GetCommandLineA();
    if (curptr == NULL) return 0;

    option_upper = (char)option;
    if ((option_upper >= 'a') && (option_upper <= 'z'))
    {
        option_upper -= 0x20;
    }

    while (1)
    {
        while ((*curptr != 0) && (*curptr != '/') && (*curptr != '-'))
        {
            curptr++;
        }

        if (*curptr == 0)
        {
            return 0;
        }

        curptr++;

        if ((*curptr == option_upper) ||
            ((*curptr >= 'a') &&
             (*curptr <= 'z') &&
             ((*curptr - 0x20) == option_upper)
            )
           )
        {
            return 1;
        }
    };
}

void cmdline_ReadLanguageOption(uint32_t *language)
{
    const char *curptr;

    curptr = GetCommandLineA();
    if (curptr == NULL) return;

    while (*curptr != 0)
    {
        if ((*curptr == '/') || (*curptr == '-'))
        {
            curptr++;

            if ((*curptr >= '0') && (*curptr <= '9'))
            {
                *language = *curptr - '0';
                break;
            }
        }
        else
        {
            curptr++;
        }
    };
}

