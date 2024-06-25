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

#include "emu_opl2.h"
#include "nuked_opl3/opl3.h"


extern void adlib_init(uint32_t samplerate);
extern void adlib_write(uintptr_t idx, uint8_t val);
extern void adlib_getsample(int16_t* sndptr, intptr_t numsamples);


static unsigned int rate;
static unsigned int adlib_address;
static int adlib_emulator;
static opl3_chip chip;

extern "C" {

void emu_opl2_init(unsigned int samplerate, int emulator)
{
    rate = samplerate;
    adlib_address = 0;
    adlib_emulator = emulator;
    if (emulator)
    {
        OPL3_Reset(&chip, samplerate);
    }
    else
    {
        adlib_init(samplerate);
    }
}

void emu_opl2_write_388(uint8_t value)
{
    adlib_address = value;
    //adlib_write_index(0x388, value);
}

void emu_opl2_write_389(uint8_t value)
{
    if (adlib_emulator)
    {
        OPL3_WriteRegBuffered(&chip, adlib_address, value);
    }
    else
    {
        adlib_write(adlib_address, value);
    }
}

uint8_t emu_opl2_read_388(void)
{
    return 0x06;
}

uint8_t emu_opl2_read_389(void)
{
    return 0xff;
}

void emu_opl2_getsamples(int16_t *samples, int numsamples)
{
    if (adlib_emulator)
    {
        if (rate == 49716)
        {
            if (numsamples > 0)
            {
                int16_t buf[4];

                for (numsamples--; numsamples != 0; numsamples--)
                {
                    OPL3_Generate4Ch(&chip, samples);
                    samples += 2;
                }

                OPL3_Generate4Ch(&chip, buf);
                samples[0] = buf[0];
                samples[1] = buf[1];
            }
        }
        else
        {
            OPL3_GenerateStream(&chip, samples, numsamples);
        }
    }
    else
    {
        adlib_getsample(samples, numsamples);
    }
}

}

