/**
 *
 *  Copyright (C) 2016 Roman Pauer
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
#include <fcntl.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Geoscape-proc-vfs.h"
#include "Geoscape-proc.h"
#include "Game_thread.h"
#include "virtualfs.h"

static void Game_Set_errno_val(void)
{
    errno_val = Game_errno();
}


int Game_dopen(CALL_PARAMS1 const char *path, const char *mode)
{
    char temp_str[MAX_PATH];
    int ret;

#if defined(__DEBUG__)
    fprintf(stderr, "dopen: original name: %s\n", path);
#endif

    vfs_get_real_name(path, (char *) &temp_str, NULL);

#if defined(__DEBUG__)
    fprintf(stderr, "dopen: real name: %s\n", (char *) &temp_str);
#endif

    ret = open((char *) &temp_str, O_RDONLY);

    if (ret == -1)
    {
        return 0;
    }

    if (ret & (~0xffff))
    {
#if defined(__DEBUG__)
        fprintf(stderr, "Error: large file descriptor\n");
#endif
        Game_StopMain();
    }

    return ret;
}

FILE *Game_fopen(const char *filename, const char *mode)
{
    char temp_str[MAX_PATH];
    FILE *ret;
    file_entry *realdir;
    int vfs_err;

#if defined(__DEBUG__)
    fprintf(stderr, "fopen: original name: %s\n", filename);
#endif

    vfs_err = vfs_get_real_name(filename, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
    fprintf(stderr, "fopen: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

    ret = fopen((char *) &temp_str, mode);
    Game_Set_errno_val();

    if (vfs_err && ret != NULL)
    {
        vfs_add_file(realdir, (char *) &temp_str);
    }

    return ret;
}
