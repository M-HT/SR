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

#if !defined(_ALBION_PROC_VFS_H_INCLUDED_)
#define _ALBION_PROC_VFS_H_INCLUDED_

#include <stdio.h>
#include <sys/types.h>

#pragma pack(1)

typedef struct PACKED {
    uint16_t twosecs : 5;    // seconds / 2
    uint16_t minutes : 6;    // minutes (0,59)
    uint16_t hours   : 5;    // hours (0,23)
} watcom_ftime_t;

typedef struct PACKED {
    uint16_t day     : 5;    // day (1,31)
    uint16_t month   : 4;    // month (1,12)
    uint16_t year    : 7;    // 0 is 1980
} watcom_fdate_t;

struct watcom_dirent {
    union {
        struct {
            PTR32(void) realdir;
            PTR32(char) pattern;
            uint8_t isfirst;
            uint8_t readdirtype;
        } v;
        uint8_t        d_dta[10];       /* disk transfer area */
    } u;
    uint16_t           d_ctime;         /* file's creation time */
    uint16_t           d_cdate;         /* file's creation date */
    uint16_t           d_atime;         /* file's last access time */
    uint16_t           d_adate;         /* file's last access date */
    uint16_t           d_lfnax;         /* DOS LFN search handle */
    uint8_t            d_lfnsup;        /* DOS LFN support status */
    uint8_t            d_attr;          /* file's attribute */
    union PACKED {
        uint16_t           d_time;          /* file's time */
        watcom_ftime_t     vt;
    } ut;
    union PACKED {
        uint16_t           d_date;          /* file's date */
        watcom_fdate_t     vd;
    } ud;
    uint32_t           d_size;          /* file's size */
    char               d_name[13];      /* file's name */
    uint16_t           d_ino;           /* serial number (not used) */
    uint8_t            d_first;         /* flag for 1st time */
    PTR32(char)        d_openpath;      /* path specified to opendir */
    PTR32(void)        v_cur_res;       /* current result */
};

struct watcom_find_t {
    uint16_t cr_time;			/* time of file creation		*/
    uint16_t cr_date;			/* date of file creation		*/
    uint16_t ar_time;			/* time of last file access		*/
    uint16_t ar_date;			/* date of last file access		*/
    uint16_t lfnax;				/* DOS LFN search handle		*/
    uint8_t  lfnsup;			/* DOS LFN support status		*/
    union {
        uint8_t reserved[10];	/* reserved for use by DOS		*/
        struct {
            uint8_t find_attrib;
            PTR32(struct watcom_dirent) dirp;
            uint32_t signature1;
            uint8_t signature2;
        } v;
    } u;
    uint8_t attrib;				/* attribute byte for file		*/
    uint16_t wr_time;			/* time of last write to file	*/
    uint16_t wr_date;			/* date of last write to file	*/
    uint32_t size;				/* length of file in bytes		*/
    char name[13];				/* null-terminated filename		*/
};

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t Game_access(const char *path, int32_t mode);
extern char *Game_getcwd(char *buf, int32_t size);
extern void *Game_fopen(const char *filename, const char *mode);
extern int32_t Game_mkdir(const char *pathname);
extern int32_t Game_unlink(const char *pathname);
extern int32_t Game_rename(const char *oldpath, const char *newpath);
extern struct watcom_dirent *Game_opendir(const char *dirname);
extern struct watcom_dirent *Game_readdir(struct watcom_dirent *dirp);
extern int32_t Game_closedir(struct watcom_dirent *dirp);
extern uint32_t Game_dos_findfirst(const char *path, const uint32_t attributes, struct watcom_find_t *buffer);
extern uint32_t Game_dos_findnext(struct watcom_find_t *buffer);
extern uint32_t Game_dos_findclose(struct watcom_find_t *buffer);

#ifdef __cplusplus
}
#endif

#endif /* _ALBION_PROC_VFS_H_INCLUDED_ */
