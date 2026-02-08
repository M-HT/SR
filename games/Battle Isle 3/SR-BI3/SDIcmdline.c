/**
 *
 *  Copyright (C) 2021-2026 Roman Pauer
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
#include <windows.h>


int CCALL cmdline_ContainsOption(int option)
{
    LPCWSTR cmdline, curptr;
    LPWSTR *argv;
    int argc, index;
    WCHAR option_upper;

    cmdline = GetCommandLineW();
    if (cmdline == NULL) return 0;

    argv = CommandLineToArgvW(cmdline, &argc);
    if (argv == NULL) return 0;

    option_upper = (WCHAR)option;
    if ((option_upper >= L'a') && (option_upper <= L'z'))
    {
        option_upper -= 0x20;
    }

    for (index = 1; index < argc; index++)
    {
        curptr = argv[index];

        while (1)
        {
            while ((*curptr != 0) && (*curptr != L'/') && (*curptr != L'-'))
            {
                curptr++;
            }

            if (*curptr == 0)
            {
                break;
            }

            curptr++;

            if ((*curptr == option_upper) ||
                ((*curptr >= L'a') &&
                 (*curptr <= L'z') &&
                 ((*curptr - 0x20) == option_upper)
                )
               )
            {
                LocalFree(argv);
                return 1;
            }
        };
    }

    LocalFree(argv);
    return 0;
}

void CCALL cmdline_ReadLanguageOption(uint32_t *language)
{
    LPCWSTR cmdline, curptr;
    LPWSTR *argv;
    int argc, index;

    cmdline = GetCommandLineW();
    if (cmdline == NULL) return;

    argv = CommandLineToArgvW(cmdline, &argc);
    if (argv == NULL) return;

    for (index = 1; index < argc; index++)
    {
        curptr = argv[index];

        while (*curptr != 0)
        {
            if ((*curptr == L'/') || (*curptr == L'-'))
            {
                curptr++;

                if ((*curptr >= L'0') && (*curptr <= L'9'))
                {
                    *language = *curptr - L'0';

                    LocalFree(argv);
                    return;
                }
            }
            else
            {
                curptr++;
            }
        };
    }

    LocalFree(argv);
}

