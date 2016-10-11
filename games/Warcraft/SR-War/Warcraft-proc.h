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

#if !defined(_WARCRAFT_PROC_H_INCLUDED_)
#define _WARCRAFT_PROC_H_INCLUDED_

typedef void (*sighandler_t)(int);

#ifdef __cplusplus
extern "C" {
#endif

extern int Game_errno(void);
extern uint32_t Game_clock(void);
extern int Game_cputs(const char *buf);
extern void Game_delay(uint32_t milliseconds);
extern void Game_dos_gettime(void *dtime);
extern int Game_fmemcmp(void *s1, uint32_t s1_seg, const void *s2, uint32_t s2_seg, size_t n);
extern uint64_t Game_fmemcpy(void *dest, uint32_t dest_seg, const void *src, uint32_t src_seg, size_t n);
extern uint64_t Game_fmemset(void *s, uint32_t s_seg, int c, size_t n);
extern int Game_raise(int condition);
extern int Game_setvbuf(FILE *fp, char *buf, int mode, size_t size);
extern sighandler_t Game_signal(int signum, sighandler_t handler);
extern void Game_FlipScreen(void);
extern void Game_Sync(void);
extern int Game_openFlags(int flags);

#ifdef __cplusplus
}
#endif

#endif /* _WARCRAFT_PROC_H_INCLUDED_ */
