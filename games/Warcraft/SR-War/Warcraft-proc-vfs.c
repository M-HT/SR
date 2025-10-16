/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
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

#define _FILE_OFFSET_BITS 64
#define _TIME_BITS 64
#include <stdio.h>
#include <fcntl.h>
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
#include <io.h>
#else
#include <unistd.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Warcraft-proc-vfs.h"
#include "Warcraft-proc.h"
#include "virtualfs.h"

static void Game_Set_errno_val(void)
{
    errno_val = Game_errno();
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
        vfs_add_file(realdir, (char *) &temp_str, 0);
    }

    return ret;
}

int32_t Game_open(const char *pathname, int32_t flags, uint32_t mode)
{
    char temp_str[MAX_PATH];
    int ret;
    file_entry *realdir;
    int vfs_err;

#if defined(__DEBUG__)
    fprintf(stderr, "open: original name: %s\n", pathname);
#endif

    vfs_err = vfs_get_real_name(pathname, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
    fprintf(stderr, "open: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

    ret = open((char *) &temp_str, flags, mode);
    Game_Set_errno_val();

    if (vfs_err && ret != -1)
    {
        vfs_add_file(realdir, (char *) &temp_str, 0);
    }

    return ret;
}

int32_t Game_unlink(const char *pathname)
{
    char temp_str[MAX_PATH];
    int ret;
    file_entry *realdir;
    int vfs_err;

#if defined(__DEBUG__)
    fprintf(stderr, "unlink: original name: %s\n", pathname);
#endif

    vfs_err = vfs_get_real_name(pathname, (char *) &temp_str, &realdir);

#if defined(__DEBUG__)
    fprintf(stderr, "unlink: real name: %s (%i)\n", (char *) &temp_str, vfs_err);
#endif

    ret = unlink((char *) &temp_str);
    Game_Set_errno_val();

    if ( (vfs_err == 0) && (ret == 0) )
    {
        vfs_delete_entry(realdir);
    }

    return ret;
}

