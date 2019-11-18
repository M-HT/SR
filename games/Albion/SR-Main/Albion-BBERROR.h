/**
 *
 *  Copyright (C) 2018-2019 Roman Pauer
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

#if !defined(_ALBION_BBERROR_H_INCLUDED_)
#define _ALBION_BBERROR_H_INCLUDED_

#include <stdint.h>

typedef void (*ERROR_OutputFuncPtr)(const char *error_string);
typedef void (*ERROR_PrintErrorPtr)(char *buffer, const uint8_t *data);

#ifdef __cplusplus
extern "C" {
#endif

extern void ERROR_Init(ERROR_OutputFuncPtr output_func_ptr);
extern void ERROR_ClearStack(void);
extern int  ERROR_PushError(ERROR_PrintErrorPtr error_print_error_ptr, const char *error_prefix, int error_data_len, const uint8_t *error_data);
// todo: remove
extern int  ERROR_PushErrorDOS(ERROR_PrintErrorPtr error_print_error_ptr, const char *error_prefix, int error_data_len, const uint8_t *error_data);
extern void ERROR_PopError(void);
extern int  ERROR_IsStackEmpty(void);
extern void ERROR_PrintAllErrors(unsigned int flags);


#ifdef __cplusplus
}
#endif

#endif /* _ALBION_BBERROR_H_INCLUDED_ */
