/**
 *
 *  Copyright (C) 2016-2021 Roman Pauer
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

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <mt32emu/mt32emu.h>
#include "emu_mt32.h"

#define ACK 0xfe
#define IO_READY 0x00
#define OUT_READY 0x80

static uint8_t return_data[2];
static uint8_t midimode, lastmidimode;
static uint8_t message_data[4];
static int message_offset, sysex_offset;
static uint8_t sysex_data[1024];

static MT32Emu::Synth *synth;

static const char *romnames[] = {
    "CM32L_CONTROL.ROM",
    "CM32L_PCM.ROM",
    "MT32_CONTROL.ROM",
    "MT32_PCM.ROM",
};


static class MT32ReportHandler : public MT32Emu::ReportHandler {
protected:
    virtual void onErrorControlROM() { }

    virtual void onErrorPCMROM() { }

    virtual void showLCDMessage(const char *message) { }

    virtual void printDebug(const char *fmt, va_list list) { }
} reportHandler;


static void strlower(char *str)
{
    while (*str != 0)
    {
        *str = tolower(*str);
        str++;
    }
}

static char const *check_file(char const *filename)
{
    if (filename == NULL) return NULL;
    if (*filename == 0) return NULL;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    DWORD dwAttrib;

    dwAttrib = GetFileAttributesA(filename);
    if ((dwAttrib == INVALID_FILE_ATTRIBUTES) || (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
#else
    if (access(filename, F_OK))
#endif
    {
        return NULL;
    }
    else
    {
        return filename;
    }
}

extern "C" {

int emu_mt32_init(unsigned int samplerate, char const *mt32_roms)
{
    synth = NULL;

    if (MT32Emu::SAMPLE_RATE != samplerate)
    {
        return 1;
    }

    if (mt32_roms == NULL)
    {
        return 2;
    }

    int pathlen = strlen(mt32_roms);
    if ((pathlen >= 4078) || (pathlen <= 1))
    {
        return 3;
    }

    char pathname[4096];

    strcpy(pathname, mt32_roms);
    if (pathname[pathlen - 1] != '/' && pathname[pathlen - 1] != '\\')
    {
        pathname[pathlen] = '/';
        pathlen++;
    }

    char *fileptr = &(pathname[pathlen]);

    int control_rom = -1;
    int control_rom_lower = 0;
    strcpy(fileptr, romnames[0]);
    if (check_file(pathname))
    {
        control_rom = 0;
    }
    else
    {
        strlower(fileptr);
        if (check_file(pathname))
        {
            control_rom = 0;
            control_rom_lower = 1;
        }
    }

    int pcm_rom = -1;
    int pcm_rom_lower = 0;
    if (control_rom >= 0)
    {
        strcpy(fileptr, romnames[1]);
        if (check_file(pathname))
        {
            pcm_rom = 1;
        }
        else
        {
            strlower(fileptr);
            if (check_file(pathname))
            {
                pcm_rom = 1;
                pcm_rom_lower = 1;
            }
            else
            {
                control_rom = -1;
            }
        }
    }

    if (control_rom < 0)
    {
        strcpy(fileptr, romnames[2]);
        if (check_file(pathname))
        {
            control_rom = 2;
            control_rom_lower = 0;
        }
        else
        {
            strlower(fileptr);
            if (check_file(pathname))
            {
                control_rom = 2;
                control_rom_lower = 1;
            }
        }

        if (control_rom >= 0)
        {
            strcpy(fileptr, romnames[3]);
            if (check_file(pathname))
            {
                pcm_rom = 3;
                pcm_rom_lower = 0;
            }
            else
            {
                strlower(fileptr);
                if (check_file(pathname))
                {
                    pcm_rom = 3;
                    pcm_rom_lower = 1;
                }
                else
                {
                    control_rom = -1;
                }
            }
        }
    }

    if (control_rom < 0)
    {
        return 4;
    }

    return_data[0] = 0;
    return_data[1] = OUT_READY;
    lastmidimode = midimode = 0;
    message_data[2] = 0;
    message_data[3] = 0;
    message_offset = 1;
    sysex_offset = 1;

    // init
    MT32Emu::FileStream controlROMFile;
    MT32Emu::FileStream pcmROMFile;

    strcpy(fileptr, romnames[control_rom]);
    if (control_rom_lower)
    {
        strlower(fileptr);
    }
    if (!controlROMFile.open(pathname))
    {
        return 5;
    }

    strcpy(fileptr, romnames[pcm_rom]);
    if (pcm_rom_lower)
    {
        strlower(fileptr);
    }
    if (!pcmROMFile.open(pathname))
    {
        return 6;
    }

    const MT32Emu::ROMImage *controlROMImage = MT32Emu::ROMImage::makeROMImage(&controlROMFile);
    const MT32Emu::ROMImage *pcmROMImage = MT32Emu::ROMImage::makeROMImage(&pcmROMFile);
    synth = new MT32Emu::Synth(&reportHandler);
    if (!synth->open(*controlROMImage, *pcmROMImage))
    {
        return 7;
    }
    MT32Emu::ROMImage::freeROMImage(controlROMImage);
    MT32Emu::ROMImage::freeROMImage(pcmROMImage);

    return 0;
}

void emu_mt32_shutdown(void)
{
    if (synth != NULL)
    {
        synth->close();
        delete synth;
        synth = NULL;
    }
}

void emu_mt32_write_330(uint8_t value)
{
restart_switch:
    switch (midimode)
    {
        case 0:
            switch (value >> 4)
            {
                case 0x08: // Note Off Event
                case 0x09: // Note On Event
                case 0x0a: // Note Aftertouch Event
                case 0x0b: // Controller Event
                case 0x0e: // Pitch Bend Event
                    message_data[0] = value;
                    lastmidimode = midimode = 2;
                    return;
                case 0x0c: // Program Change Event
                case 0x0d: // Channel Aftertouch Event
                    message_data[0] = value;
                    lastmidimode = midimode = 1;
                    return;

                case 0x0f: // SysEx Event
                    sysex_data[0] = value;
                    midimode = 4;
                    return;
                default:
                    midimode = lastmidimode;
                    goto restart_switch;
            }
        case 1:
            message_data[message_offset] = value;

            // send message
            synth->playMsg(message_data[0] | (((uint32_t)(message_data[1])) << 8) | (((uint32_t)(message_data[2])) << 16) | (((uint32_t)(message_data[3])) << 24));

            message_offset = 1;
            midimode = 0;
            return;
        case 2:
            message_data[message_offset++] = value;
            midimode--;
            return;
        default:
            sysex_data[sysex_offset++] = value;
            if (value == 0xf7)
            {
                // send sysex
                synth->playSysex(sysex_data, sysex_offset);

                sysex_offset = 1;
                midimode = 0;
            }
            return;
    }
}

void emu_mt32_write_331(uint8_t value)
{
    switch (value)
    {
        case 0x3f: // enter UART mode
            return_data[0] = ACK;
            return_data[1] = IO_READY;
            break;
        case 0xff: // reset
            return_data[0] = ACK;
            return_data[1] = IO_READY;
            break;
        default:
            return_data[0] = 0;
            return_data[1] = OUT_READY;
            break;
    }
}

uint8_t emu_mt32_read_330(void)
{
    uint8_t retval = return_data[0];
    return_data[0] = 0;
    return_data[1] = OUT_READY;
    return retval;
}

uint8_t emu_mt32_read_331(void)
{
    return return_data[1];
}

void emu_mt32_getsamples(int16_t *samples, int numsamples)
{
    synth->render(samples, numsamples);
}

}

