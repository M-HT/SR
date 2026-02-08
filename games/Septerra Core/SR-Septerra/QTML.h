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

#if !defined(_QTML_H_INCLUDED_)
#define _QTML_H_INCLUDED_

#include "ptr32.h"
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// note: parameters smaller than 32 bits are extended to 32 bits by callers in the original/thunk asm code
// note: return values smaller than 32 bits are not expected by callers in the original asm code to have been extended to 32 bits

int16_t CCALL InitializeQTML_c (int32_t flag);
void CCALL TerminateQTML_c (void);

void * CCALL CreatePortAssociation_c (void *theWnd, void *storage, int32_t flags);
void CCALL DestroyPortAssociation_c (void *cgp);

void CCALL SetGWorld_c (void *port, void *gdh);
uint8_t * CCALL c2pstr_c (char *aStr);
int16_t CCALL PtrToHand_c (const void *srcPtr, PTR32(void *)*dstHndl, int32_t size);

int16_t CCALL FSMakeFSSpec_c (int16_t vRefNum, int32_t dirID, char *fileName, void *spec);
int16_t CCALL QTSetDDPrimarySurface_c (void *lpNewDDSurface, uint32_t flags);
int32_t CCALL NativeEventToMacEvent_c (void *nativeEvent, void *macEvent);

int16_t CCALL EnterMovies_c (void);
void CCALL ExitMovies_c (void);
void CCALL StartMovie_c (void *theMovie);
void CCALL StopMovie_c (void *theMovie);
uint8_t CCALL IsMovieDone_c (void *theMovie);
void CCALL DisposeMovie_c (void *theMovie);
int16_t CCALL OpenMovieFile_c (const void *fileSpec, int16_t *resRefNum, int8_t permission);
int16_t CCALL CloseMovieFile_c (int16_t resRefNum);
int16_t CCALL NewMovieFromFile_c (PTR32(void)*theMovie, int16_t resRefNum, int16_t *resId, uint8_t *resName, int16_t newMovieFlags, uint8_t *dataRefWasChanged);
void CCALL GetMovieBox_c (void *theMovie, void *boxRect);
void * CCALL NewMovieController_c (void *theMovie, const void *movieRect, int32_t someFlags);
void CCALL DisposeMovieController_c (void *mc);
int16_t CCALL QTRegisterAccessKey_c (uint8_t *accessKeyType, int32_t flags, void *accessKey);
int16_t CCALL QTUnregisterAccessKey_c (uint8_t *accessKeyType, int32_t flags, void *accessKey);
void * CCALL MCIsPlayerEvent_c (void *mc, const void *e);
void * CCALL MCDoAction_c (void *mc, int16_t action, void *params);

#ifdef __cplusplus
}
#endif

#endif

