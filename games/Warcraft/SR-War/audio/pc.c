/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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
    Game_AudioMusicVolume = 80;
    Game_AudioFormat = AUDIO_S16SYS;
    Game_AudioChannels = 2;
    Game_AudioRate = 22050;
    Game_AudioBufferSize = 2048;
    Game_InterpolateAudio = 1;
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    Game_MidiSubsystem = 3;
#else
    Game_MidiSubsystem = 3;
#endif
    Game_OPL3Emulator = 1;
}

void Init_Audio2(void)
{
}

int Config_Audio(char *str, char *param)
{
    return 0;
}

void Cleanup_Audio(void)
{
}

void Change_HW_Audio_Volume(int amount)
{
}
