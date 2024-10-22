/**
 *
 *  Copyright (C) 2016-2024 Roman Pauer
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

#if !defined(_ALBION_PROC_H_INCLUDED_)
#define _ALBION_PROC_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t Game_errno(void);
extern void Game_Set_errno_val(void);
extern void Game_Set_errno_error(int32_t value);
extern int32_t Game_fclose(FILE *fp);
extern int32_t Game_fcloseall(void);
extern int32_t Game_filelength(int32_t fd);
extern uint32_t Game_dos_getvect(const int32_t intnum);
extern int32_t Game_lseek(int32_t fd, int32_t offset, int32_t whence);
extern void Game_setbuf(FILE *fp, char *buf);
extern void Game_dos_setvect(const int32_t intnum, const uint32_t handler_low, const uint32_t handler_high);
extern int32_t Game_tell(int32_t handle);
extern int32_t Game_time(int32_t *tloc);
extern void Game_Sync(void);
extern void Game_WaitTimerTicks(const int32_t ticks);
extern void Game_WaitVerticalRetraceTicks(const int32_t ticks);
extern void Game_WaitAfterVerticalRetrace(void);
extern void Game_WaitForVerticalRetrace(void);
extern void Game_WaitAfter2ndVerticalRetrace(void);
extern void Game_WaitFor2ndVerticalRetrace(void);
extern int32_t Game_openFlags(int32_t flags);
extern void Game_splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);

#ifdef __cplusplus
}
#endif

#endif /* _ALBION_PROC_H_INCLUDED_ */
