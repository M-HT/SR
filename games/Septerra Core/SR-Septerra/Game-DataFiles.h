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

#if !defined(_GAME_DATAFILES_H_INCLUDED_)
#define _GAME_DATAFILES_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void OpenGameDataFiles(const char *ManifestFilePath, const char *_SourcePath, uint32_t (*_MessageProc)(const char *, uint32_t, uint32_t));
void CloseGameDataFiles(void);
int32_t RecordOpen(uint32_t RecordKey);
int32_t RecordTryOpen(uint32_t RecordKey);
const char *RecordGetDataFilePathAndOffset(uint32_t RecordKey, uint32_t *Offset);
void RecordSeek(int32_t RecordHandle, int32_t Offset, int32_t Whence);
uint32_t RecordRead(int32_t RecordHandle, uint8_t *ReadBuffer, uint32_t NumberOfBytes);
void RecordClose(int32_t RecordHandle);
uint32_t RecordGetSize(int32_t RecordHandle);
uint32_t GetFirstLevelRecordKey(void);
uint32_t GetNextLevelRecordKey(void);

#ifdef __cplusplus
}
#endif

#endif

