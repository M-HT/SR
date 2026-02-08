/**
 *
 *  Copyright (C) 2019-2026 Roman Pauer
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
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t CCALL memcmp_c(const void *s1, const void *s2, uint32_t n);
void * CCALL memcpy_c(void *dest, const void *src, uint32_t n);
void * CCALL memset_c(void *s, int32_t c, uint32_t n);

char * CCALL strcat_c(char *dest, const char *src);
int32_t CCALL strcmp_c(const char *s1, const char *s2);
char * CCALL strcpy_c(char *dest, const char *src);
uint32_t CCALL strlen_c(const char *s);
char * CCALL strncpy_c(char *dest, const char *src, uint32_t n);
int32_t CCALL _strnicmp_c(const char *s1, const char *s2, uint32_t n);
char * CCALL strstr_c(const char *haystack, const char *needle);

int32_t CCALL printf2_c(const char *format, uint32_t *ap);
int32_t CCALL sprintf2_c(char *str, const char *format, uint32_t *ap);

void CCALL ms_srand_c(uint32_t seed);
int32_t CCALL ms_rand_c(void);
int32_t CCALL wc_rand_c(void);

int32_t CCALL atoi_c(const char *nptr);
int32_t CCALL atol_c(const char *nptr);
char * CCALL _ltoa_c(int32_t value, char *buffer, int32_t radix);

int32_t CCALL isalnum_c(int32_t c);

int32_t CCALL _except_handler3_c(void *exception_record, void *registration, void *context, void *dispatcher);

void CCALL sync_c(void);

#ifdef __cplusplus
}
#endif

#endif

