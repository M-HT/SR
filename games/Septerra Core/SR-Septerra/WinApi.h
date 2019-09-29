/**
 *
 *  Copyright (C) 2019 Roman Pauer
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

#if !defined(_WINAPI_H_INCLUDED_)
#define _WINAPI_H_INCLUDED_

#ifdef WINAPI_DEFINE_VARIABLES
#define WINAPI_STORAGE
#else
#define WINAPI_STORAGE extern
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
// C11
WINAPI_STORAGE _Thread_local unsigned int Winapi_LastError;
#elif defined(__cplusplus) && __cplusplus >= 201103L
// C++11
WINAPI_STORAGE thread_local unsigned int Winapi_LastError;
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC) || defined(__IBMC__) || defined(__IBMCPP__) || defined(__ibmxl__) || defined(__GNUC__) || defined(__llvm__) || (defined(__INTEL_COMPILER) && defined(__linux__))
WINAPI_STORAGE __thread unsigned int Winapi_LastError;
#elif defined(_MSC_VER) || (defined(__INTEL_COMPILER) && defined(_WIN32)) || defined(__BORLANDC__) || defined(__DMC__)
WINAPI_STORAGE __declspec(thread) unsigned int Winapi_LastError;
#else
#error thread local variables are not supported
#endif

#define Winapi_GetLastError() (Winapi_LastError)
#define Winapi_SetLastError(n) Winapi_LastError = (n)


#ifndef WINAPI_NODEF_DEFINITIONS
#define ERROR_FILE_NOT_FOUND 2
#define ERROR_ACCESS_DENIED 5
#define ERROR_INVALID_HANDLE 6
#define ERROR_NOT_ENOUGH_MEMORY 8
#define ERROR_NO_MORE_FILES 18
#define ERROR_WRITE_FAULT 29
#define ERROR_READ_FAULT 30
#define ERROR_HANDLE_EOF 38
#define ERROR_NOT_SUPPORTED 50
#define ERROR_FILE_EXISTS 80
#define ERROR_INVALID_PARAMETER 87
#define ERROR_ALREADY_EXISTS 183


#define INVALID_HANDLE_VALUE (handle)(-1)
#endif


typedef enum _handle_types_ {
    HT_NONE,
    HT_FILE,
    HT_SEARCH,
    HT_PIPE
} handle_types;


typedef struct {
    handle_types handle_type;
    void *f;
} T_file_handle, *file_handle;

typedef struct {
    handle_types handle_type;
    void *d;
} T_search_handle, *search_handle;

typedef struct {
    handle_types handle_type;
    void *pp;
} T_pipe_handle, *pipe_handle;

typedef union {
    handle_types handle_type;
    T_file_handle fh;
    T_search_handle sh;
    T_pipe_handle pph;
} T_handle, *handle;


handle Winapi_AllocHandle(void);


#endif

