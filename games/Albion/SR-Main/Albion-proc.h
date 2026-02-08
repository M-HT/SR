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

#if !defined(_ALBION_PROC_H_INCLUDED_)
#define _ALBION_PROC_H_INCLUDED_

#include "Game_defs.h"

#pragma pack(1)

typedef struct PACKED {
    int32_t time;
    uint16_t millitm;
    int16_t timezone;
    int16_t dstflag;
} watcom_timeb;

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t CCALL Game_errno(void);
extern void Game_Set_errno_val(void);
extern void Game_Set_errno_error(int32_t value);
extern char * CCALL Game_ctime2(const int32_t *timep, char *buf, int32_t max);
extern int32_t CCALL Game_fclose(void *stream);
extern int32_t CCALL Game_fcloseall(void);
extern int32_t CCALL Game_fputs(const char *s, void *stream);
extern char * CCALL Game_fgets(char *s, int32_t size, void *stream);
extern int32_t CCALL Game_ftime(watcom_timeb *tp);
extern uint32_t CCALL Game_dos_getvect(const int32_t intnum);
extern void CCALL Game_dos_setvect(const int32_t intnum, const uint32_t handler_low, const uint32_t handler_high);
extern void CCALL Game_Sync(void);
extern void CCALL Game_WaitTimerTicks(const int32_t ticks);
extern void CCALL Game_WaitAfterVerticalRetrace(void);
extern void CCALL Game_WaitForVerticalRetrace(void);
extern void CCALL Game_WaitAfter2ndVerticalRetrace(void);
extern void CCALL Game_WaitFor2ndVerticalRetrace(void);
extern void CCALL Game_splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);

#ifdef __cplusplus
}
#endif

#endif /* _ALBION_PROC_H_INCLUDED_ */
