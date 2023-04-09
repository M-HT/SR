/**
 *
 *  Copyright (C) 2019-2023 Roman Pauer
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

#if !defined(_WINAPI_DSOUND_H_INCLUDED_)
#define _WINAPI_DSOUND_H_INCLUDED_

#include "ptr32.h"

struct IDirectSound_c;
struct IDirectSoundBuffer_c;

struct _dsbufferdesc;
struct twaveformatex;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t DirectSoundCreate_c(void *lpGuid, PTR32(struct IDirectSound_c) *ppDS, void *pUnkOuter);

uint32_t IDirectSound_QueryInterface_c(struct IDirectSound_c *lpThis, void * riid, PTR32(void)* ppvObj);
uint32_t IDirectSound_AddRef_c(struct IDirectSound_c *lpThis);
uint32_t IDirectSound_Release_c(struct IDirectSound_c *lpThis);
uint32_t IDirectSound_CreateSoundBuffer_c(struct IDirectSound_c *lpThis, const struct _dsbufferdesc * pcDSBufferDesc, PTR32(struct IDirectSoundBuffer_c)* ppDSBuffer, void * pUnkOuter);
uint32_t IDirectSound_GetCaps_c(struct IDirectSound_c *lpThis, void * pDSCaps);
uint32_t IDirectSound_DuplicateSoundBuffer_c(struct IDirectSound_c *lpThis, struct IDirectSoundBuffer_c * pDSBufferOriginal, PTR32(struct IDirectSoundBuffer_c)* ppDSBufferDuplicate);
uint32_t IDirectSound_SetCooperativeLevel_c(struct IDirectSound_c *lpThis, void * hwnd, uint32_t dwLevel);
uint32_t IDirectSound_Compact_c(struct IDirectSound_c *lpThis);
uint32_t IDirectSound_GetSpeakerConfig_c(struct IDirectSound_c *lpThis, uint32_t * pdwSpeakerConfig);
uint32_t IDirectSound_SetSpeakerConfig_c(struct IDirectSound_c *lpThis, uint32_t dwSpeakerConfig);
uint32_t IDirectSound_Initialize_c(struct IDirectSound_c *lpThis, const void * pcGuidDevice);

uint32_t IDirectSoundBuffer_QueryInterface_c(struct IDirectSoundBuffer_c *lpThis, void * riid, PTR32(void)* ppvObj);
uint32_t IDirectSoundBuffer_AddRef_c(struct IDirectSoundBuffer_c *lpThis);
uint32_t IDirectSoundBuffer_Release_c(struct IDirectSoundBuffer_c *lpThis);
uint32_t IDirectSoundBuffer_GetCaps_c(struct IDirectSoundBuffer_c *lpThis, void * pDSBufferCaps);
uint32_t IDirectSoundBuffer_GetCurrentPosition_c(struct IDirectSoundBuffer_c *lpThis, uint32_t * pdwCurrentPlayCursor, uint32_t * pdwCurrentWriteCursor);
uint32_t IDirectSoundBuffer_GetFormat_c(struct IDirectSoundBuffer_c *lpThis, void * pwfxFormat, uint32_t dwSizeAllocated, uint32_t * pdwSizeWritten);
uint32_t IDirectSoundBuffer_GetVolume_c(struct IDirectSoundBuffer_c *lpThis, int32_t * plVolume);
uint32_t IDirectSoundBuffer_GetPan_c(struct IDirectSoundBuffer_c *lpThis, int32_t * plPan);
uint32_t IDirectSoundBuffer_GetFrequency_c(struct IDirectSoundBuffer_c *lpThis, uint32_t * pdwFrequency);
uint32_t IDirectSoundBuffer_GetStatus_c(struct IDirectSoundBuffer_c *lpThis, uint32_t * pdwStatus);
uint32_t IDirectSoundBuffer_Initialize_c(struct IDirectSoundBuffer_c *lpThis, struct IDirectSound_c * pDirectSound, const void * pcDSBufferDesc);
uint32_t IDirectSoundBuffer_Lock_c(struct IDirectSoundBuffer_c *lpThis, uint32_t dwOffset, uint32_t dwBytes, PTR32(void)* ppvAudioPtr1, uint32_t * pdwAudioBytes1, PTR32(void)* ppvAudioPtr2, uint32_t * pdwAudioBytes2, uint32_t dwFlags);
uint32_t IDirectSoundBuffer_Play_c(struct IDirectSoundBuffer_c *lpThis, uint32_t dwReserved1, uint32_t dwReserved2, uint32_t dwFlags);
uint32_t IDirectSoundBuffer_SetCurrentPosition_c(struct IDirectSoundBuffer_c *lpThis, uint32_t dwNewPosition);
uint32_t IDirectSoundBuffer_SetFormat_c(struct IDirectSoundBuffer_c *lpThis, const struct twaveformatex * pcfxFormat);
uint32_t IDirectSoundBuffer_SetVolume_c(struct IDirectSoundBuffer_c *lpThis, int32_t lVolume);
uint32_t IDirectSoundBuffer_SetPan_c(struct IDirectSoundBuffer_c *lpThis, int32_t lPan);
uint32_t IDirectSoundBuffer_SetFrequency_c(struct IDirectSoundBuffer_c *lpThis, uint32_t dwFrequency);
uint32_t IDirectSoundBuffer_Stop_c(struct IDirectSoundBuffer_c *lpThis);
uint32_t IDirectSoundBuffer_Unlock_c(struct IDirectSoundBuffer_c *lpThis, void * pvAudioPtr1, uint32_t dwAudioBytes1, void * pvAudioPtr2, uint32_t dwAudioBytes2);
uint32_t IDirectSoundBuffer_Restore_c(struct IDirectSoundBuffer_c *lpThis);

#ifdef __cplusplus
}
#endif

#endif

