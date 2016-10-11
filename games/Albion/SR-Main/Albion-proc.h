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

#if !defined(_ALBION_PROC_H_INCLUDED_)
#define _ALBION_PROC_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

extern int Game_errno(void);
extern off_t Game_filelength(int fd);
extern uint64_t Game_dos_getvect(const int32_t intnum);
extern void Game_setbuf(FILE *fp, char *buf);
extern void Game_dos_setvect(const int32_t intnum, const uint32_t handler_low, const uint32_t handler_high);
extern off_t Game_tell(int handle);
extern void Game_Sync(void);
extern void Game_WaitTimerTicks(const int32_t ticks);
extern void Game_WaitVerticalRetraceTicks(const int32_t ticks);
extern void Game_WaitAfterVerticalRetrace(void);
extern void Game_WaitForVerticalRetrace(void);
extern void Game_WaitAfter2ndVerticalRetrace(void);
extern void Game_WaitFor2ndVerticalRetrace(void);
extern int Game_openFlags(int flags);
extern void Game_splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);

#ifdef __cplusplus
}
#endif

#endif /* _ALBION_PROC_H_INCLUDED_ */
