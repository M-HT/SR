/**
 *
 *  Copyright (C) 2019-2024 Roman Pauer
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

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#if defined(PANDORA) || defined(PYRA)
#define CONFIG_FILE "../Septerra.cfg"
#else
#define CONFIG_FILE "Septerra.cfg"
#endif

extern int32_t Patch_PreselectCharacters;

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
    int items, num_int;

    Game_Installation = 0;

    Intro_Play = 1;

    Display_Mode = 0;
    Display_VSync = 0;
    Display_Width = 0;
    Display_Height = 0;
    Display_Resizable = 0;
    Display_ScalingQuality = 1;
    Display_IntegerScaling = 0;
    Display_DelayAfterFlip = 0;

    Audio_SampleRate = 0;
    Audio_BufferSize = 0;
    Audio_ResamplingQuality = 0;

    Patch_PreselectCharacters = 1;

    Option_DefaultMovement = 0;
    Option_MovieResolution = 1;
    Option_MoviesPlay = 1;
    Option_PointSoundsPlay = 1;
    Option_SoundsPlay = 1;

    Cheat_ENEMIES = 0;
    Cheat_SIGHT = 0;
    Cheat_HIDETEXT = 0;
    Cheat_FPS = 0;

    Input_GameController = 0;

    Controller_Deadzone = 1000;

    Keys_SwitchWSAD = 0;
    Keys_SwitchArrowKeys = 0;

    CPU_SleepMode = 0;

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
        items = fscanf(f, "%8191[\n\r]", buf);

        // read line
        buf[0] = 0;
        items = fscanf(f, "%8191[^\n^\r]", buf);
        if (items <= 0) continue;

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

        if ( strncasecmp(str, "Game_", 5) == 0 ) // str begins with "Game_"
        {
            // game settings

            str += 5;

            if ( strcasecmp(str, "Installation") == 0 ) // str equals "Installation"
            {
                if ( strcasecmp(param, "installed") == 0 ) // param equals "installed"
                {
                    Game_Installation = 0;
                }
                else if ( strcasecmp(param, "portable") == 0 ) // param equals "portable"
                {
                    Game_Installation = 1;
                }
            }

        }
        else if ( strncasecmp(str, "Intro_", 6) == 0 ) // str begins with "Intro_"
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
            else if ( strcasecmp(str, "Scaling") == 0 ) // str equals "Scaling"
            {
                if ( strcasecmp(param, "basicnb") == 0 ) // param equals "basicnb"
                {
                    Display_ScalingQuality = 0;
                }
                else if ( strcasecmp(param, "basic") == 0 ) // param equals "basic"
                {
                    Display_ScalingQuality = 1;
                }
            }
            else if ( strcasecmp(str, "IntegerScaling") == 0 ) // str equals "IntegerScaling"
            {
                if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
                {
                    Display_IntegerScaling = 1;
                }
                else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
                {
                    Display_IntegerScaling = 0;
                }
            }
            else if ( strcasecmp(str, "DelayAfterFlip") == 0 ) // str equals "DelayAfterFlip"
            {
                num_int = 0;
                sscanf(param, "%i", &num_int);
                if ((num_int > 0) && (num_int <= 20))
                {
                    Display_DelayAfterFlip = num_int;
                }
            }

        }
        else if ( strncasecmp(str, "Audio_", 6) == 0 ) // str begins with "Audio_"
        {
            // audio settings

            str += 6;

            if ( strcasecmp(str, "SampleRate") == 0 ) // str equals "SampleRate"
            {
                num_int = 0;
                sscanf(param, "%i", &num_int);
                if (num_int > 0)
                {
                    Audio_SampleRate = num_int;
                }
            }
            else if ( strcasecmp(str, "BufferSize") == 0 ) // str equals "BufferSize"
            {
                num_int = 0;
                sscanf(param, "%i", &num_int);
                if (num_int > 0)
                {
                    Audio_BufferSize = num_int;
                }
            }
            else if ( strcasecmp(str, "ResamplingQuality") == 0 ) // str equals "ResamplingQuality"
            {
                num_int = 0;
                sscanf(param, "%i", &num_int);
                if (num_int >= 0)
                {
                    Audio_ResamplingQuality = num_int;
                }
            }
        }
        else if ( strncasecmp(str, "Patch_", 6) == 0 ) // str begins with "Patch_"
        {
            // patch settings

            str += 6;

            if ( strcasecmp(str, "PreselectCharacters") == 0 ) // str equals "PreselectCharacters"
            {
                if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
                {
                    Patch_PreselectCharacters = 1;
                }
                else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
                {
                    Patch_PreselectCharacters = 0;
                }
            }
        }
        else if ( strncasecmp(str, "Option_", 7) == 0 ) // str begins with "Option_"
        {
            // command line options

            str += 7;

            if ( strcasecmp(str, "DefaultMovement") == 0 ) // str equals "DefaultMovement"
            {
                if ( strcasecmp(param, "walk") == 0 ) // param equals "walk"
                {
                    Option_DefaultMovement = 0;
                }
                else if ( strcasecmp(param, "run") == 0 ) // param equals "run"
                {
                    Option_DefaultMovement = 1;
                }
            }
            else if ( strcasecmp(str, "MovieResolution") == 0 ) // str equals "MovieResolution"
            {
                if ( strcasecmp(param, "native") == 0 ) // param equals "native"
                {
                    Option_MovieResolution = 0;
                }
                else if ( strcasecmp(param, "doubled") == 0 ) // param equals "doubled"
                {
                    Option_MovieResolution = 1;
                }
            }
            else if ( strcasecmp(str, "MoviesPlay") == 0 ) // str equals "MoviesPlay"
            {
                if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
                {
                    Option_MoviesPlay = 1;
                }
                else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
                {
                    Option_MoviesPlay = 0;
                }
            }
            else if ( strcasecmp(str, "PointSoundsPlay") == 0 ) // str equals "PointSoundsPlay"
            {
                if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
                {
                    Option_PointSoundsPlay = 1;
                }
                else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
                {
                    Option_PointSoundsPlay = 0;
                }
            }
            else if ( strcasecmp(str, "SoundsPlay") == 0 ) // str equals "SoundsPlay"
            {
                if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
                {
                    Option_SoundsPlay = 1;
                }
                else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
                {
                    Option_SoundsPlay = 0;
                }
            }

        }
        else if ( strncasecmp(str, "Cheat_", 6) == 0 ) // str begins with "Cheat_"
        {
            // cheats settings

            str += 6;

            if ( strcasecmp(str, "ENEMIES") == 0 ) // str equals "ENEMIES"
            {
                if ( strcasecmp(param, "enabled") == 0 ) // param equals "enabled"
                {
                    Cheat_ENEMIES = 1;
                }
                else if ( strcasecmp(param, "disabled") == 0 ) // param equals "disabled"
                {
                    Cheat_ENEMIES = 0;
                }
            }
            else if ( strcasecmp(str, "SIGHT") == 0 ) // str equals "SIGHT"
            {
                if ( strcasecmp(param, "enabled") == 0 ) // param equals "enabled"
                {
                    Cheat_SIGHT = 1;
                }
                else if ( strcasecmp(param, "disabled") == 0 ) // param equals "disabled"
                {
                    Cheat_SIGHT = 0;
                }
            }
            else if ( strcasecmp(str, "HIDETEXT") == 0 ) // str equals "HIDETEXT"
            {
                if ( strcasecmp(param, "enabled") == 0 ) // param equals "enabled"
                {
                    Cheat_HIDETEXT = 1;
                }
                else if ( strcasecmp(param, "disabled") == 0 ) // param equals "disabled"
                {
                    Cheat_HIDETEXT = 0;
                }
            }
            else if ( strcasecmp(str, "FPS") == 0 ) // str equals "FPS"
            {
                if ( strcasecmp(param, "enabled") == 0 ) // param equals "enabled"
                {
                    Cheat_FPS = 1;
                }
                else if ( strcasecmp(param, "disabled") == 0 ) // param equals "disabled"
                {
                    Cheat_FPS = 0;
                }
            }

        }
#if !defined(PANDORA) && !defined(PYRA)
        else if ( strncasecmp(str, "Input_", 6) == 0 ) // str begins with "Input_"
        {
            // input settings

            str += 6;

            if ( strcasecmp(str, "GameController") == 0 ) // str equals "GameController"
            {
                if ( strcasecmp(param, "yes") == 0 ) // param equals "yes"
                {
                    Input_GameController = 1;
                }
                else if ( strcasecmp(param, "no") == 0 ) // param equals "no"
                {
                    Input_GameController = 0;
                }
            }
        }
        else if ( strncasecmp(str, "Controller_", 11) == 0 ) // str begins with "Controller_"
        {
            // controller settings

            str += 11;

            if ( strcasecmp(str, "Deadzone") == 0 ) // str equals "Deadzone"
            {
                num_int = 0;
                sscanf(param, "%i", &num_int);
                if (num_int >= 0)
                {
                    Controller_Deadzone = num_int;
                }
            }
        }
#endif
        else if ( strncasecmp(str, "Keys_", 5) == 0 ) // str begins with "Keys_"
        {
            // keys settings

            str += 5;

            if ( strcasecmp(str, "WSAD") == 0 ) // str equals "WSAD"
            {
                if ( strcasecmp(param, "WSAD") == 0 ) // param equals "WSAD"
                {
                    Keys_SwitchWSAD = 0;
                }
                else if ( strcasecmp(param, "ArrowKeys") == 0 ) // param equals "ArrowKeys"
                {
                    Keys_SwitchWSAD = 1;
                }
            }
            else if ( strcasecmp(str, "ArrowKeys") == 0 ) // str equals "ArrowKeys"
            {
                if ( strcasecmp(param, "WSAD") == 0 ) // param equals "WSAD"
                {
                    Keys_SwitchArrowKeys = 1;
                }
                else if ( strcasecmp(param, "ArrowKeys") == 0 ) // param equals "ArrowKeys"
                {
                    Keys_SwitchArrowKeys = 0;
                }
            }

        }
        else if ( strncasecmp(str, "CPU_", 4) == 0 ) // str begins with "CPU_"
        {
            // CPU settings

            str += 4;

            if ( strcasecmp(str, "SleepMode") == 0 ) // str equals "SleepMode"
            {
                if ( strcasecmp(param, "standard") == 0 ) // param equals "standard"
                {
                    CPU_SleepMode = 0;
                }
                else if ( strcasecmp(param, "reduced") == 0 ) // param equals "reduced"
                {
                    CPU_SleepMode = 1;
                }
                else if ( strcasecmp(param, "nosleep") == 0 ) // param equals "nosleep"
                {
                    CPU_SleepMode = 2;
                }
            }
        }
    }

    fclose(f);
}
