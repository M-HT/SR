/**
 *
 *  Copyright (C) 2019-2021 Roman Pauer
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

int32_t memcmp_c(const void *s1, const void *s2, uint32_t n);
void *memcpy_c(void *dest, const void *src, uint32_t n);
void *memset_c(void *s, int32_t c, uint32_t n);

char *strcat_c(char *dest, const char *src);
int32_t strcmp_c(const char *s1, const char *s2);
char *strcpy_c(char *dest, const char *src);
uint32_t strlen_c(const char *s);
char *strncpy_c(char *dest, const char *src, uint32_t n);
int32_t _strnicmp_c(const char *s1, const char *s2, uint32_t n);
char *strstr_c(const char *haystack, const char *needle);

void ms_srand_c(uint32_t seed);
int32_t ms_rand_c(void);
int32_t wc_rand_c(void);

int32_t atoi_c(const char *nptr);
int32_t atol_c(const char *nptr);
char *_ltoa_c(int32_t value, char *buffer, int32_t radix);

int32_t isalnum_c(int32_t c);

int32_t _except_handler3_c(void *exception_record, void *registration, void *context, void *dispatcher);

void sync_c(void);

#ifdef __cplusplus
}
#endif

#endif

