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

#if !defined(_BBLBL_H_INCLUDED_)
#define _BBLBL_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int LBL_Init_c(void);
void LBL_Exit_c(void);
void *LBL_OpenLib_c(const char *path, int param2);
void LBL_CloseLib_c(void *lib);
void *LBL_ReadEntry_c(void *lib, void *entry_data, unsigned int entry_number, int close_file, void *entry_metadata);
int LBL_GetEntrySize_c(void *lib, unsigned int entry_number);
void LBL_CloseFile_c(void *lib);
int LBL_GetNOFEntries_c(void *lib);

#ifdef __cplusplus
}
#endif

#endif /* _BBLBL_H_INCLUDED_ */
