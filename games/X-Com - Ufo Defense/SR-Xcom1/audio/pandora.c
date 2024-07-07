/**
 *
 *  Copyright (C) 2016-2024 Roman Pauer
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

#include "../Game_defs.h"
#include "../Game_vars.h"

void Init_Audio(void)
{
    Game_AudioFormat = AUDIO_S16SYS;
    Game_AudioChannels = 2;
    Game_AudioRate = 22050;
    Game_AudioBufferSize = 1024;
    Game_ResamplingQuality = 0;

    Game_AudioMusicVolume = 128;    // Music's relative loudness x/128
    Game_AudioSampleVolume = 128;   // Sound sample relative loudness x/128
    Game_MidiSubsystem = 1;
}

void Init_Audio2(void)
{
}

int Config_Audio(char *str, char *param)
{
    int num_int;

//senquack - on GP2X, this doesn't really do much, I have left it out of the .cfg files
//					so GP2X users don't get confused.
    if ( strcasecmp(str, "Audio_Volume") == 0)	// str equals "Audio_Volume"
    {
        num_int = 0;
        sscanf(param, "%i", &num_int);
        if (num_int < 0) Game_AudioMasterVolume = 0;
        else if (num_int > MIX_MAX_VOLUME) Game_AudioMasterVolume = MIX_MAX_VOLUME;
        else Game_AudioMasterVolume = num_int;

        return 1;
    }

    return 0;
}

void Cleanup_Audio(void)
{
}

void Change_HW_Audio_Volume(int amount)
{
    SDL_Event event;
    int newvolume;

    newvolume = Game_AudioMasterVolume + amount;

    if (newvolume < 0)
    {
        Game_AudioMasterVolume = 0;
    }
    else if (newvolume > 128)
    {
        Game_AudioMasterVolume = 128;
    }
    else
    {
        Game_AudioMasterVolume = newvolume;
    }

    event.type = SDL_USEREVENT;
    event.user.code = EC_SET_VOLUME;
    event.user.data1 = NULL;
    event.user.data2 = NULL;

    SDL_PushEvent(&event);
}
