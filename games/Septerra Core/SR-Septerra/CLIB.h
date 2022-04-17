/**
 *
 *  Copyright (C) 2019-2022 Roman Pauer
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

#if !defined(_CLIB_H_INCLUDED_)
#define _CLIB_H_INCLUDED_

#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memset_c(void *s, int32_t c, uint32_t n);
void *memcpy_c(void *dest, const void *src, uint32_t n);

int32_t _stricmp_c(const char *s1, const char *s2);
char *strncpy_c(char *dest, const char *src, uint32_t n);
int32_t strncmp_c(const char *s1, const char *s2, uint32_t n);
char *strncat_c(char *dest, const char *src, uint32_t n);
int32_t _strnicmp_c(const char *s1, const char *s2, uint32_t n);

void *malloc_c(uint32_t size);
void free_c(void *ptr);
void *calloc_c(uint32_t nmemb, uint32_t size);

int32_t atol_c(const char *nptr);
int32_t toupper_c(int32_t c);

uint32_t fread_c(void *ptr, uint32_t size, uint32_t nmemb, void *stream);
int32_t ftell_c(void *stream);
int32_t fseek_c(void *stream, int32_t offset, int32_t whence);
void *fopen_c(const char *path, const char *mode);
int32_t fclose_c(void *fp);

int32_t system_c(const char *command);
void exit_c(int32_t status);
void srand_c(uint32_t seed);
int32_t rand_c(void);

void __report_gsfailure_c(void);

int32_t _except_handler4_c(int32_t, void *TargetFrame, int32_t);
int32_t _except_handler3_c(int32_t, void *TargetFrame, int32_t);

uint32_t _beginthread_c(void(*start_address)(void *), uint32_t stack_size, void *arglist);

void sync_c(void);

#ifdef __cplusplus
}
#endif

int CLIB_FindFile(const char *src, char *dst);

#endif

