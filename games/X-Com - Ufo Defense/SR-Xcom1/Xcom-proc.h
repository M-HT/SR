/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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

#ifdef __cplusplus
extern "C" {
#endif

extern void Game_Set_errno_val(void);
extern void Game_Set_errno_val_num(int32_t value);
extern int Game_checkch(void);
extern int Game_getch(void);
extern off_t Game_filelength2(FILE *f);
extern void *Game_malloc(uint32_t size);
extern void Game_free(void *ptr);
extern void *Game_AllocateMemory(uint32_t size);
extern void Game_FreeMemory(void *mem);
extern int Game_dlseek(int fd, int offset, int whence);
extern int Game_dread(void *buf, int count, int fd);
extern void Game_dclose(int fd);
extern int Game_fclose(FILE *fp);
extern int Game_fcloseall(void);
extern void Game_SlowDownMainLoop(void);
extern void Game_SlowDownScrolling(void);
extern void Game_Sync(void);
extern void Game_WaitVerticalRetraceTicks(const int32_t ticks);

#ifdef __cplusplus
}
#endif

extern void Game_FlipScreen(void);

#endif /* _XCOM_PROC_H_INCLUDED_ */
