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

#if !defined(_XCOM_PROC_H_INCLUDED_)
#define _XCOM_PROC_H_INCLUDED_

#include <sys/types.h>
#include "Game_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void CCALL Game_Set_errno_val(void);
extern void CCALL Game_Set_errno_val_num(int32_t value);
extern int32_t CCALL Game_checkch(void);
extern int32_t CCALL Game_getch(void);
extern int32_t CCALL Game_filelength2(void *stream);
extern void * CCALL Game_malloc(uint32_t size);
extern void CCALL Game_free(void *ptr);
extern void *Game_AllocateMemory(uint32_t size);
extern void Game_FreeMemory(void *mem);
extern int32_t CCALL Game_time(int32_t *tloc);
extern int32_t CCALL Game_dlseek(int32_t fd, int32_t offset, int32_t whence);
extern int32_t CCALL Game_dread(void *buf, int32_t count, int32_t fd);
extern void CCALL Game_dclose(int32_t fd);
extern int32_t CCALL Game_fclose(void *stream);
extern int32_t CCALL Game_fcloseall(void);
extern int32_t CCALL Game_feof(void *stream);
extern int32_t CCALL Game_fflush(void *stream);
extern int32_t CCALL Game_fgetc(void *stream);
extern int32_t CCALL Game_fputc(int32_t c, void *stream);
extern int32_t CCALL Game_fputs(const char *s, void *stream);
extern uint32_t CCALL Game_fread(void *ptr, uint32_t size, uint32_t nmemb, void *stream);
extern uint32_t CCALL Game_fwrite(const void *ptr, uint32_t size, uint32_t nmemb, void *stream);
extern void CCALL Game_SlowDownMainLoop(void);
extern void CCALL Game_SlowDownScrolling(void);
extern void CCALL Game_Sync(void);
extern void CCALL Game_WaitVerticalRetraceTicks(const int32_t ticks);

#ifdef __cplusplus
}
#endif

extern void Game_FlipScreen(void);

#endif /* _XCOM_PROC_H_INCLUDED_ */
