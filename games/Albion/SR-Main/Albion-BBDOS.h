/**
 *
 *  Copyright (C) 2018-2026 Roman Pauer
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

#if !defined(_ALBION_BBDOS_H_INCLUDED_)
#define _ALBION_BBDOS_H_INCLUDED_

#include <stdint.h>
#include "Game_defs.h"

#define DOS_OPEN_MODE_READ 2
#define DOS_OPEN_MODE_CREATE 4
#define DOS_OPEN_MODE_RDWR 8
#define DOS_OPEN_MODE_APPEND 16

#define DOS_SEEK_CUR 0
#define DOS_SEEK_SET 1
#define DOS_SEEK_END 2

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t CCALL DOS_Init(void);
extern void CCALL DOS_Exit(void);
extern int32_t CCALL DOS_Open(const char *path, uint32_t mode);
extern int32_t CCALL DOS_Close(int32_t file_handle);
extern int32_t CCALL DOS_Read(int32_t file_handle, void *buffer, uint32_t length);
extern int32_t CCALL DOS_Write(int32_t file_handle, const void *buffer, uint32_t length);
extern int32_t CCALL DOS_Seek(int32_t file_handle, int32_t origin, int32_t offset);
extern int32_t CCALL DOS_GetFileLength(const char *path);
extern int32_t CCALL DOS_exists(const char *path);
extern int32_t CCALL DOS_setcurrentdir(const char *path);
extern int32_t CCALL DOS_GetSeekPosition(int32_t file_handle);


#ifdef __cplusplus
}
#endif

#endif /* _ALBION_BBDOS_H_INCLUDED_ */
