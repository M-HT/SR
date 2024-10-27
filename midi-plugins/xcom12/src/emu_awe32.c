/**
 *
 *  Copyright (C) 2024 Roman Pauer
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
#include "emu_awe32.h"
#include "pcem_emu8k/common.h"
#include "pcem_emu8k/sound_emu8k.h"


int emu8k_new_pos;

static emu8k_t synth;
static FILE *romhandle;

// called from emu8k_init
FILE *emu8k_romfopen(char *fn, char *mode)
{
    FILE *f;

    f = romhandle;
    romhandle = NULL;

    return f;
}

int emu_awe32_init(unsigned int samplerate, char const *awe32_rom)
{
    if (samplerate != 44100)
    {
        return 1;
    }

    if (awe32_rom == NULL)
    {
        return 2;
    }

    romhandle = fopen(awe32_rom, "rb");
    if (romhandle == NULL)
    {
        return 3;
    }

    emu8k_new_pos = 0;
    memset(&synth, 0, sizeof(synth));
    emu8k_init(&synth, 0, 0); // X-COM doesn't use any additional RAM

    return 0;
}

void emu_awe32_shutdown(void)
{
    emu8k_close(&synth);
}

void emu_awe32_write16(uint16_t port, uint16_t value)
{
    emu8k_outw(port, value, &synth);
}

uint16_t emu_awe32_read16(uint16_t port)
{
    uint16_t value;

    value = emu8k_inw(port, &synth);

    return value;
}

void emu_awe32_getsamples(int16_t *samples, int numsamples)
{
    int index;

    synth.pos = 0;
    emu8k_new_pos = numsamples;
    emu8k_update(&synth);

    for (index = 0; index < numsamples; index++)
    {
        samples[2 * index] = synth.buffer[2 * index];
        samples[2 * index + 1] = synth.buffer[2 * index + 1];
    }
}

