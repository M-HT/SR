/**
 *
 *  Copyright (C) 2020-2021 Roman Pauer
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

#if !defined(_BBDOS_H_INCLUDED_)
#define _BBDOS_H_INCLUDED_

#include <stdint.h>

#define DOS_OPEN_MODE_READ 2
#define DOS_OPEN_MODE_WRITE 4
#define DOS_OPEN_MODE_APPEND 8

#define DOS_SEEK_CUR 0
#define DOS_SEEK_SET 1
#define DOS_SEEK_END 2

#ifdef __cplusplus
extern "C" {
#endif

int DOS_Init_c(void);
void DOS_Exit_c(void);
int DOS_Open_c(const char *path, unsigned int mode);
int DOS_Close_c(int file_handle);
int DOS_Read_c(int file_handle, void *buffer, unsigned int length);
int DOS_Write_c(int file_handle, const void *buffer, unsigned int length);
int DOS_Seek_c(int file_handle, int origin, int offset);
void *DOS_ReadFile_c(const char *path, void *buffer);
int DOS_WriteFile_c(const char *path, const void *buffer, unsigned int length);
int DOS_GetFileLength_c(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* _BBDOS_H_INCLUDED_ */
