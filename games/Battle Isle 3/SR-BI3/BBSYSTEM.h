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

#if !defined(_BBSYSTEM_H_INCLUDED_)
#define _BBSYSTEM_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void SYSTEM_SetInitValues_c(int type, const char *value);
void SYSTEM_SystemTask_c(void);
int SYSTEM_Init_c(void);
void SYSTEM_Exit_c(void);
unsigned int SYSTEM_GetTicks_c(void);
int SYSTEM_IsApplicationActive_c(void);
void SYSTEM_WaitTicks_c(unsigned int ticks);
void SYSTEM_EnterCriticalSection_c(void);
void SYSTEM_LeaveCriticalSection_c(void);
int SYSTEM_InCriticalSection_c(void);
unsigned int SYSTEM_GetOS_c(void);

#ifdef __cplusplus
}
#endif

#endif /* _BBSYSTEM_H_INCLUDED_ */
