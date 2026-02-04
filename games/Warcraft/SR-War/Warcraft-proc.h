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

#if !defined(_WARCRAFT_PROC_H_INCLUDED_)
#define _WARCRAFT_PROC_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t Game_errno(void);
extern uint32_t Game_clock(void);
extern int32_t Game_cputs(const char *buf);
extern void Game_delay(uint32_t milliseconds);
extern int32_t Game_fclose(void *stream);
extern int32_t Game_fgetc(void *stream);
extern int32_t Game_ftell(void *stream);
extern int32_t Game_fputc(int32_t c, void *stream);
extern int32_t Game_fputs(const char *s, void *stream);
extern char *Game_fgets(char *s, int32_t size, void *stream);
extern int32_t Game_fseek(void *stream, int32_t offset, int32_t whence);
extern uint32_t Game_fread(void *ptr, uint32_t size, uint32_t nmemb, void *stream);
extern uint32_t Game_fwrite(const void *ptr, uint32_t size, uint32_t nmemb, void *stream);
extern void Game_dos_gettime(void *dtime);
extern int32_t Game_fmemcmp(void *s1, uint32_t s1_seg, const void *s2, uint32_t s2_seg, uint32_t n);
extern void *Game_fmemcpy(void *dest, uint32_t dest_seg, const void *src, uint32_t src_seg, uint32_t n);
extern void *Game_fmemset(void *s, uint32_t s_seg, int32_t c, uint32_t n);
extern void *Game_malloc(uint32_t size);
extern void Game_free(void *ptr);
extern int32_t Game_setvbuf(void *fp, char *buf, int32_t mode, uint32_t size);
extern uint32_t Game_signal(int32_t signum, uint32_t handler);
extern uint32_t Game_strtoul(const char *nptr, PTR32(char) *endptr, int32_t base);
extern int32_t Game_time(int32_t *tloc);
extern void Game_FlipScreen(void);
extern void Game_Sync(void);

#ifdef __cplusplus
}
#endif

#endif /* _WARCRAFT_PROC_H_INCLUDED_ */
