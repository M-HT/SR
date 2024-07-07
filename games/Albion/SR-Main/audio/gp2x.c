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

//senquack - SOUND STUFF
// For opening GP2X's /dev/mixer
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/soundcard.h>
#include <fcntl.h>


static int InitialVolume;      // Initial GP2X volume (before SDL clobbers it).
                               //  Also saved so we can set it back to this
                               //  on exit because we are polite.

//senquack - SOUND STUFF

// Set new GP2X mixer level, 0-100
static void Set_GP2X_Volume(int newvol)
{
    int soundDev;
    int vol;

#if defined(__DEBUG__)
    fprintf(stderr, "Setting GP2X mixer to: %d\n", newvol);
#endif

    if ((newvol >= 0) && (newvol <= 100))
    {
        soundDev = open("/dev/mixer", O_RDWR);
        if (soundDev != -1)
        {
            vol = ((newvol << 8) | newvol);
            ioctl(soundDev, SOUND_MIXER_WRITE_PCM, &vol);
            close(soundDev);
        }
    }
}

//senquack - new, called at startup so we can
// 	set it back to what it was when we exit.
// Returns 0-100, current mixer volume, -1 on error.
static int Get_GP2X_Volume(void)
{
    int soundDev;
    int vol;

    vol = -1;
    soundDev = open("/dev/mixer", O_RDONLY);
    if (soundDev != -1)
    {
        ioctl(soundDev, SOUND_MIXER_READ_PCM, &vol);
        close(soundDev);
        if (vol != -1)
        {
            //just return one channel , not both channels, they're hopefully the same anyways
            return (vol & 0xFF);
        }
    }

    return vol;
}


void Init_Audio(void)
{
    Game_AudioFormat = AUDIO_S16SYS;
    Game_AudioChannels = 1;
    Game_AudioRate = 22050;
    Game_AudioBufferSize = 256;
    Game_ResamplingQuality = 0;
    Game_SwapSoundChannels = 1;
    Game_MidiSubsystem = 0;

    // Important to get the GP2X's current mixer level, as SDL clobbers it. We will
    //		set it back to this after initializing the video & audio.
    InitialVolume = Get_GP2X_Volume();
}

void Init_Audio2(void)
{
    //senquack - SOUND STUFF
    // We read the current mixer level at first startup, before all video/audio initialization.
    //		Then, after all that we set it back to that first reading because SDL clobbers it.
    Set_GP2X_Volume(InitialVolume);

}

int Config_Audio(char *str, char *param)
{
    return 0;
}

void Cleanup_Audio(void)
{
    Set_GP2X_Volume(InitialVolume);
}

//Will change the volume level up or down, if amount is positive or negative.
//Will do volume level clamping.
void Change_HW_Audio_Volume(int amount)
{
    int current_volume;

    current_volume = Get_GP2X_Volume();

    if (current_volume == -1)
    {
        current_volume = 67;
    }

    //senquack - limit amount volume can be changed when we're down low already..
    //		So headphone users can set a good volume
    if ((amount > +1) && current_volume <= 12)
    {
        amount = +1;
    }
    else if ((amount < -1) && current_volume <= 12)
    {
        amount = -1;
    }

    current_volume += amount;

    if (current_volume > 100)
    {
        current_volume = 100;
    }
    else if (current_volume < 0)
    {
        current_volume = 0;
    }
    Set_GP2X_Volume(current_volume);
}

