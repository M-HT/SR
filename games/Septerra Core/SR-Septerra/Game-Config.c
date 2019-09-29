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

#define GAME_CONFIG_DEFINE_VARIABLES
#include "Game-Config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CLIB.h"

#if (defined(__WIN32__) || (__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef PANDORA
#define CONFIG_FILE "../Septerra.cfg"
#else
#define CONFIG_FILE "Septerra.cfg"
#endif

static char *trim_string(char *buf)
{
    int len;

    while (*buf == ' ') buf++;

    len = strlen(buf);

    while (len != 0 && buf[len - 1] == ' ')
    {
        len--;
        buf[len] = 0;
    }

    return buf;
}

void ReadConfiguration(void)
{
    FILE *f;
    char buf[8192];
    char *str, *param;
    int num_int;

    Intro_Play = 1;

    Display_Mode = 0;
    Display_VSync = 0;
    Display_Width = 0;
    Display_Height = 0;
    Display_Resizable = 0;

#ifdef _WIN32
    f = fopen(CONFIG_FILE, "rt");
#else
    if (!CLIB_FindFile(CONFIG_FILE, buf)) return;

    f = fopen(buf, "rt");
#endif
    if (f == NULL) return;


    while (!feof(f))
    {
        // skip empty lines
        fscanf(f, "%8192[\n\r]", buf);

        // read line
        buf[0] = 0;
        fscanf(f, "%8192[^\n^\r]", buf);

        // trim line
        str = trim_string(buf);

        if (str[0] == 0) continue;
        if (str[0] == '#') continue;

        // find parameter (after '=')
        param = strchr(str, '=');

        if (param == NULL) continue;

        // split string into two strings
        *param = 0;
        param++;

        // trim them
        str = trim_string(str);
        param = trim_string(param);

        if ( strncasecmp(str, "Intro_", 6) == 0 ) // str begins with "Intro_"
        {
            // intro settings

            str += 6;

            if ( strcasecmp(str, "Play") == 0 ) // str equals "Play"
            {
                if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
                {
                    Intro_Play = 1;
                }
                else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
                {
                    Intro_Play = 0;
                }
            }

        }
        else if ( strncasecmp(str, "Display_", 8) == 0 ) // str begins with "Display_"
        {
            // display settings

            str += 8;

            if ( strcasecmp(str, "Mode") == 0 ) // str equals "Mode"
            {
                if ( strcasecmp(param, "window") == 0 ) // param equals "window"
                {
                    Display_Mode = 0;
                }
                else if ( strcasecmp(param, "desktop") == 0 ) // param equals "desktop"
                {
                    Display_Mode = 1;
                }
                else if ( strcasecmp(param, "fullscreen") == 0 ) // param equals "fullscreen"
                {
                    Display_Mode = 2;
                }
            }
            else if ( strcasecmp(str, "VSync") == 0 ) // str equals "VSync"
            {
                if ( strcasecmp(param, "on") == 0 ) // param equals "on"
                {
                    Display_VSync = 1;
                }
                else if ( strcasecmp(param, "off") == 0 ) // param equals "off"
                {
                    Display_VSync = 0;
                }
            }
            else if ( strcasecmp(str, "Width") == 0 ) // str equals "Width"
            {
                num_int = 0;
                sscanf(param, "%i", &num_int);
                if (num_int > 0)
                {
                    Display_Width = (num_int > 640)?num_int:640;
                }
            }
            else if ( strcasecmp(str, "Height") == 0 ) // str equals "Height"
            {
                num_int = 0;
                sscanf(param, "%i", &num_int);
                if (num_int > 0)
                {
                    Display_Height = (num_int > 480)?num_int:480;
                }
            }
            else if ( strcasecmp(str, "Resizable") == 0 ) // str equals "Resizable"
            {
                if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
                {
                    Display_Resizable = 1;
                }
                else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
                {
                    Display_Resizable = 0;
                }
            }

        }

    }

    fclose(f);
}
