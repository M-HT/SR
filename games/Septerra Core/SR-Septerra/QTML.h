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

#if !defined(_QTML_H_INCLUDED_)
#define _QTML_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int16_t InitializeQTML_c (int32_t flag);
void TerminateQTML_c (void);

void *CreatePortAssociation_c (void *theWnd, void *storage, int32_t flags);
void DestroyPortAssociation_c (void *cgp);

void SetGWorld_c (void *port, void *gdh);
unsigned char *c2pstr_c (char *aStr);
int16_t PtrToHand_c (const void *srcPtr, void ***dstHndl, int32_t size);

int16_t FSMakeFSSpec_c (int16_t vRefNum, int32_t dirID, char *fileName, void *spec);
int16_t QTSetDDPrimarySurface_c (void *lpNewDDSurface, uint32_t flags);
int32_t NativeEventToMacEvent_c (void *nativeEvent, void *macEvent);

int16_t EnterMovies_c (void);
void ExitMovies_c (void);
void StartMovie_c (void *theMovie);
void StopMovie_c (void *theMovie);
uint8_t IsMovieDone_c (void *theMovie);
void DisposeMovie_c (void *theMovie);
int16_t OpenMovieFile_c (const void *fileSpec, int16_t *resRefNum, int8_t permission);
int16_t CloseMovieFile_c (int16_t resRefNum);
int16_t NewMovieFromFile_c (void **theMovie, int16_t resRefNum, int16_t *resId, unsigned char *resName, int16_t newMovieFlags, uint8_t *dataRefWasChanged);
void GetMovieBox_c (void *theMovie, void *boxRect);
void *NewMovieController_c (void *theMovie, const void *movieRect, int32_t someFlags);
void DisposeMovieController_c (void *mc);
int16_t QTRegisterAccessKey_c (unsigned char *accessKeyType, int32_t flags, void *accessKey);
int16_t QTUnregisterAccessKey_c (unsigned char *accessKeyType, int32_t flags, void *accessKey);
void *MCIsPlayerEvent_c (void *mc, const void *e);
void *MCDoAction_c (void *mc, int16_t action, void *params);

#ifdef __cplusplus
}
#endif

#endif

