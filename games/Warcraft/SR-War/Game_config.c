/**
 *
 *  Copyright (C) 2016-2020 Roman Pauer
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
#include <string.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Game_config.h"
#include "main.h"
#include "display.h"
#include "audio.h"
#include "input.h"

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

void Game_ReadConfig(void)
{
    // senquack - config files are now specified via command line
//    const static char config_filename[] = "Warcraft.cfg";

    FILE *f;
    char buf[8192];
    char *str, *param;
    int items, num_int;

    // senquack - config files are now specified via command line
//    f = fopen(config_filename, "rt");
    f = fopen(Game_ConfigFilename, "rt");
//    if (f == NULL) return;
    if (f == NULL)
    {
        fprintf(stderr, "Could not open config file: %s\n", Game_ConfigFilename);
        return;
    }
    else
    {
        fprintf(stderr, "Using config file: %s\n", Game_ConfigFilename);
    }

    while (!feof(f))
    {
        // skip empty lines
        items = fscanf(f, "%8192[\n\r]", buf);

        // read line
        buf[0] = 0;
        items = fscanf(f, "%8192[^\n^\r]", buf);
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

        if (Config_Display(str, param)) continue;
        if (Config_Audio(str, param)) continue;
        if (Config_Input(str, param)) continue;

        if ( strncasecmp(str, "Audio_", 6) == 0 ) // str begins with "Audio_"
        {
            // audio settings

            str += 6;

            if ( strcasecmp(str, "Channels") == 0 ) // str equals "Channels"
            {
                if ( strcasecmp(param, "stereo") == 0 ) // param equals "stereo"
                {
                    Game_AudioChannels = 2;
                }
                else if ( strcasecmp(param, "mono") == 0 ) // param equals "mono"
                {
                    Game_AudioChannels = 1;
                }
            }
            else if ( strcasecmp(str, "Resolution") == 0 ) // str equals "Resolution"
            {
                if ( strcasecmp(param, "16") == 0 ) // param equals "16"
                {
                    Game_AudioFormat = AUDIO_S16SYS;
                }
                else if ( strcasecmp(param, "8") == 0 ) // param equals "8"
                {
                    Game_AudioFormat = AUDIO_S8;
                }
            }
            else if ( strcasecmp(str, "Sample_Rate") == 0 ) // str equals "Sample_Rate"
            {
                num_int = 0;
                sscanf(param, "%i", &num_int);
                if (num_int > 0)
                {
                    Game_AudioRate = num_int;
                }
            }
            else if ( strcasecmp(str, "Buffer_Size") == 0 ) // str equals "Buffer_Size"
            {
                num_int = 0;
                sscanf(param, "%i", &num_int);
                if (num_int > 0)
                {
                    Game_AudioBufferSize = num_int;
                }
            }
            else if ( strcasecmp(str, "Interpolation") == 0 ) // str equals "Interpolation"
            {
                // audio interpolation

                if ( strcasecmp(param, "on") == 0 ) // param equals "on"
                {
                    Game_InterpolateAudio = 1;
                }
                else if ( strcasecmp(param, "off") == 0 ) // param equals "off"
                {
                    Game_InterpolateAudio = 0;
                }
            }
            else if ( strcasecmp(str, "Music_Volume") == 0 ) // str equals "Music_Volume"
            {
                // music volume relative to overall volume

                num_int = 0;
                sscanf(param, "%i", &num_int);
                if (num_int < 0)
                {
                    Game_AudioMusicVolume = 0;
                }
                else if (num_int > 127)
                {
                    Game_AudioMusicVolume = 127;
                }
                else
                {
                    Game_AudioMusicVolume = num_int;
                }
            }
            else if ( strcasecmp(str, "MIDI_Subsystem") == 0 ) // str equals "MIDI_Subsystem"
            {
                // MIDI subsystem

                if ( strcasecmp(param, "wildmidi") == 0 ) // param equals "wildmidi"
                {
                    Game_MidiSubsystem = 1;
                }
                else if ( strcasecmp(param, "bassmidi") == 0 ) // param equals "bassmidi"
                {
                    Game_MidiSubsystem = 2;
                }
                else if ( strcasecmp(param, "adlmidi") == 0 ) // param equals "adlmidi"
                {
                    Game_MidiSubsystem = 3;
                }
                else if ( strcasecmp(param, "nativewindows") == 0 ) // param equals "nativewindows"
                {
                    Game_MidiSubsystem = 11;
                }
                else if ( strcasecmp(param, "alsa") == 0 ) // param equals "alsa"
                {
                    Game_MidiSubsystem = 12;
                }
                else if ( strcasecmp(param, "sdl_mixer") == 0 ) // param equals "sdl_mixer"
                {
                    Game_MidiSubsystem = 0;
                }
            }
            else if ( strcasecmp(str, "SoundFont_Path") == 0 ) // str equals "SoundFont_Path"
            {
                // path to SoundFont file

                if (*param != 0)
                {
                    Game_SoundFontPath = strdup(param);
                }
            }
            else if ( strcasecmp(str, "MIDI_Device") == 0 ) // str equals "MIDI_Device"
            {
                // MIDI Device name

                if (*param != 0)
                {
                    Game_MidiDevice = strdup(param);
                }
            }

        }
        else if ( strncasecmp(str, "Sound", 5) == 0 ) // str begins with "Sound"
        {
            str += 5;

            if (str[0] == 0) // str equals "Sound"
            {
                // sound setting

                if ( strcasecmp(param, "on") == 0 ) // param equals "on"
                {
                    Game_Sound = 1;
                }
                else if ( strcasecmp(param, "off") == 0 ) // param equals "off"
                {
                    Game_Sound = 0;
                }
            }
        }
        else if ( strncasecmp(str, "Music", 5) == 0 ) // str begins with "Music"
        {
            str += 5;

            if (str[0] == 0) // str equals "Music"
            {
                // music setting

                if ( strcasecmp(param, "on") == 0 ) // param equals "on"
                {
                    Game_Music = 1;
                }
                else if ( strcasecmp(param, "off") == 0 ) // param equals "off"
                {
                    Game_Music = 0;
                }
            }
        }
        else if ( strncasecmp(str, "Display_", 8) == 0 ) // str begins with "Display_"
        {
            str += 8;

            if ( strcasecmp(str, "MouseCursor") == 0 ) // str equals "MouseCursor"
            {
                if ( strcasecmp(param, "normal") == 0 ) // param equals "normal"
                {
                    Game_MouseCursor = 0;
                }
                else if ( strcasecmp(param, "minimal") == 0 ) // param equals "minimal"
                {
                    Game_MouseCursor = 1;
                }
                else if ( strcasecmp(param, "none") == 0 ) // param equals "none"
                {
                    Game_MouseCursor = 2;
                }
            }
        }

    }

    fclose(f);
}

