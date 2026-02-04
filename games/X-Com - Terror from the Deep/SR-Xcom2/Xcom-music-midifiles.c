/**
 *
 *  Copyright (C) 2016-2026 Roman Pauer
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

#include <string.h>
#include <fcntl.h>
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
#include <io.h>
#else
#include <unistd.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "virtualfs.h"
#include "Xcom-proc.h"
#include "Xcom-proc-vfs.h"

const static struct {
    const char *filename;
    int midiloop;
} MidiFiles[20] = {
    {"GMNEWINT.MID", 1},
    {"GMSIG.MID", 0},
    {"GMWAITLO.MID", 1},
    {"GMGeo1.mid", 1},
    {"GMGeo2.mid", 1},
    {"GMGeo3.mid", 1},
    {"GMGeo4.mid", 1},
    {"GMGeo5.mid", 1},
    {"GMGeo6.mid", 1},
    {"GMInter1.mid", 1},
    {"GMINTER2.MID", 1},
    {"GMINTER3.MID", 1},
    {"GMISPOSH.MID", 1},
    {"GMBIGMAR.MID", 1},
    {"GMDEFBAS.MID", 1},
    {"GMATTBAS.MID", 1},
    {"GMDEBGOO.MID", 1},
    {"GMDEBPOR.MID", 1},
    {"GMTACWET.MID", 1},
    {"GMTACDRY.MID", 1}
};


#ifdef __cplusplus
extern "C" {
#endif

extern int Game_ReadSong(const char *catalog_name, int index, uint8_t *buf);

#ifdef __cplusplus
}
#endif


static int Game_FindMidiFile(int index, uint8_t *buf)
{
    char namebuf[24];
    char temp_str[MAX_PATH];
    int fd;

    if ((index < 0) || (index > 19)) return 0;

    strcpy((char *) &namebuf, "C:\\SOUND\\");
    strcat((char *) &namebuf, MidiFiles[index].filename);

#if defined(__DEBUG__)
    fprintf(stderr, "FindMidiFile: original name: %s\n", (char *) &namebuf);
#endif

    vfs_get_real_name((char *) &namebuf, (char *) &temp_str, NULL);

#if defined(__DEBUG__)
    fprintf(stderr, "FindMidiFile: real name: %s\n", (char *) &temp_str);
#endif

    fd = open((char *) &temp_str, O_RDONLY);

    if (fd == -1)
    {
        return 0;
    }

    close(fd);

    strcpy((char *) buf, "MIDI");
    buf[5] = MidiFiles[index].midiloop;
    strcpy((char *) &(buf[6]), (char *) &temp_str);

    return 1;
}

int Game_ReadSong(const char *catalog_name, int index, uint8_t *buf)
{
    void *stream;
    FILE *f;
    uint32_t num_files, file_offset, file_size, next_offset;
    uint8_t name_length;

    Game_SongLength = 0;

    if (index < 0) return 0;

    if (Game_LoadMidiFiles)
    {
        if (Game_FindMidiFile(index, buf))
        {
            return 1;
        }
    }

    stream = Game_fopen(catalog_name, "rb");
    if (stream == NULL) return 0;
    f = (sizeof(void *) > 4) ? *(FILE **)stream : (FILE *)stream;
    if (fread(&num_files, 4, 1, f) != 1) goto read_song_error;

    num_files >>= 3;
    if (index >= (int32_t)num_files) goto read_song_error;

    if (fseek(f, 8*index, SEEK_SET)) goto read_song_error;
    if (fread(&file_offset, 4, 1, f) != 1) goto read_song_error;
    if (fread(&file_size, 4, 1, f) != 1) goto read_song_error;

    if (file_size == 0) goto read_song_error;
    if (file_size > 20000) goto read_song_error;

    if (index < (int32_t)num_files - 1)
    {
        if (fread(&next_offset, 4, 1, f) != 1) goto read_song_error;
    }
    else
    {
        int catalog_size;

        if (fseek(f, 0, SEEK_END)) goto read_song_error;
        catalog_size = ftell(f);
        if (catalog_size < 0) goto read_song_error;
        next_offset = catalog_size;
    }

    if (fseek(f, file_offset, SEEK_SET)) goto read_song_error;

    if (next_offset - file_offset != file_size)
    {
        if (fread(&name_length, 1, 1, f) != 1) goto read_song_error;
        if (name_length)
        {
            if (fseek(f, name_length, SEEK_CUR)) goto read_song_error;
        }
    }

    if (fread(buf, 1, file_size, f) != file_size) goto read_song_error;

    Game_SongLength = file_size;

    Game_fclose(stream);
    return 1;

read_song_error:
    Game_fclose(stream);
    return 0;
}

