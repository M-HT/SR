;;
;;  Copyright (C) 2019 Roman Pauer
;;
;;  Permission is hereby granted, free of charge, to any person obtaining a copy of
;;  this software and associated documentation files (the "Software"), to deal in
;;  the Software without restriction, including without limitation the rights to
;;  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
;;  of the Software, and to permit persons to whom the Software is furnished to do
;;  so, subject to the following conditions:
;;
;;  The above copyright notice and this permission notice shall be included in all
;;  copies or substantial portions of the Software.
;;
;;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;;  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;;  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;;  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;;  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;;  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;;  SOFTWARE.
;;

%include "asm-calls.inc"

%ifidn __OUTPUT_FORMAT__, win32
    %define InitializeQTML_c _InitializeQTML_c
    %define TerminateQTML_c _TerminateQTML_c

    %define CreatePortAssociation_c _CreatePortAssociation_c
    %define DestroyPortAssociation_c _DestroyPortAssociation_c

    %define SetGWorld_c _SetGWorld_c
    %define c2pstr_c _c2pstr_c
    %define PtrToHand_c _PtrToHand_c

    %define FSMakeFSSpec_c _FSMakeFSSpec_c
    %define QTSetDDPrimarySurface_c _QTSetDDPrimarySurface_c
    %define NativeEventToMacEvent_c _NativeEventToMacEvent_c

    %define EnterMovies_c _EnterMovies_c
    %define ExitMovies_c _ExitMovies_c
    %define StartMovie_c _StartMovie_c
    %define StopMovie_c _StopMovie_c
    %define IsMovieDone_c _IsMovieDone_c
    %define DisposeMovie_c _DisposeMovie_c
    %define OpenMovieFile_c _OpenMovieFile_c
    %define CloseMovieFile_c _CloseMovieFile_c
    %define NewMovieFromFile_c _NewMovieFromFile_c
    %define GetMovieBox_c _GetMovieBox_c
    %define NewMovieController_c _NewMovieController_c
    %define DisposeMovieController_c _DisposeMovieController_c
    %define QTRegisterAccessKey_c _QTRegisterAccessKey_c
    %define QTUnregisterAccessKey_c _QTUnregisterAccessKey_c
    %define MCIsPlayerEvent_c _MCIsPlayerEvent_c
    %define MCDoAction_c _MCDoAction_c
%endif

extern InitializeQTML_c
extern TerminateQTML_c

extern CreatePortAssociation_c
extern DestroyPortAssociation_c

extern SetGWorld_c
extern c2pstr_c
extern PtrToHand_c

extern FSMakeFSSpec_c
extern QTSetDDPrimarySurface_c
extern NativeEventToMacEvent_c

extern EnterMovies_c
extern ExitMovies_c
extern StartMovie_c
extern StopMovie_c
extern IsMovieDone_c
extern DisposeMovie_c
extern OpenMovieFile_c
extern CloseMovieFile_c
extern NewMovieFromFile_c
extern GetMovieBox_c
extern NewMovieController_c
extern DisposeMovieController_c
extern QTRegisterAccessKey_c
extern QTUnregisterAccessKey_c
extern MCIsPlayerEvent_c
extern MCDoAction_c


global InitializeQTML_asm2c
global TerminateQTML_asm2c

global CreatePortAssociation_asm2c
global DestroyPortAssociation_asm2c

global SetGWorld_asm2c
global c2pstr_asm2c
global PtrToHand_asm2c

global FSMakeFSSpec_asm2c
global QTSetDDPrimarySurface_asm2c
global NativeEventToMacEvent_asm2c

global EnterMovies_asm2c
global ExitMovies_asm2c
global StartMovie_asm2c
global StopMovie_asm2c
global IsMovieDone_asm2c
global DisposeMovie_asm2c
global OpenMovieFile_asm2c
global CloseMovieFile_asm2c
global NewMovieFromFile_asm2c
global GetMovieBox_asm2c
global NewMovieController_asm2c
global DisposeMovieController_asm2c
global QTRegisterAccessKey_asm2c
global QTUnregisterAccessKey_asm2c
global MCIsPlayerEvent_asm2c
global MCDoAction_asm2c


%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
InitializeQTML_asm2c:

; [esp + 4] = int32_t flag
; [esp    ] = return address

        Call_Asm_Stack1 InitializeQTML_c

        retn

; end procedure InitializeQTML_asm2c


align 16
TerminateQTML_asm2c:

; [esp] = return address

        Call_Asm_Stack0 TerminateQTML_c

        retn

; end procedure TerminateQTML_asm2c


align 16
CreatePortAssociation_asm2c:

; [esp + 3*4] = int32_t flags
; [esp + 2*4] = void *storage
; [esp +   4] = void *theWnd
; [esp      ] = return address

        Call_Asm_Stack3 CreatePortAssociation_c

        retn

; end procedure CreatePortAssociation_asm2c


align 16
DestroyPortAssociation_asm2c:

; [esp + 4] = void *cgp
; [esp    ] = return address

        Call_Asm_Stack1 DestroyPortAssociation_c

        retn

; end procedure DestroyPortAssociation_asm2c


align 16
SetGWorld_asm2c:

; [esp + 2*4] = void *gdh
; [esp +   4] = void *port
; [esp      ] = return address

        Call_Asm_Stack2 SetGWorld_c

        retn

; end procedure SetGWorld_asm2c


align 16
c2pstr_asm2c:

; [esp + 4] = char *aStr
; [esp    ] = return address

        Call_Asm_Stack1 c2pstr_c

        retn

; end procedure c2pstr_asm2c


align 16
PtrToHand_asm2c:

; [esp + 3*4] = int32_t size
; [esp + 2*4] = void ***dstHndl
; [esp +   4] = const void *srcPtr
; [esp      ] = return address

        Call_Asm_Stack3 PtrToHand_c

        retn

; end procedure PtrToHand_asm2c


align 16
FSMakeFSSpec_asm2c:

; [esp + 4*4] = void *spec
; [esp + 3*4] = char *fileName
; [esp + 2*4] = int32_t dirID
; [esp +   4] = int16_t vRefNum
; [esp      ] = return address

        Call_Asm_Stack4 FSMakeFSSpec_c

        retn

; end procedure FSMakeFSSpec_asm2c


align 16
QTSetDDPrimarySurface_asm2c:

; [esp + 2*4] = uint32_t flags
; [esp +   4] = void *lpNewDDSurface
; [esp      ] = return address

        Call_Asm_Stack2 QTSetDDPrimarySurface_c

        retn

; end procedure QTSetDDPrimarySurface_asm2c


align 16
NativeEventToMacEvent_asm2c:

; [esp + 2*4] = void *macEvent
; [esp +   4] = void *nativeEvent
; [esp      ] = return address

        Call_Asm_Stack2 NativeEventToMacEvent_c

        retn

; end procedure NativeEventToMacEvent_asm2c


align 16
EnterMovies_asm2c:

; [esp] = return address

        Call_Asm_Stack0 EnterMovies_c

        retn

; end procedure EnterMovies_asm2c


align 16
ExitMovies_asm2c:

; [esp] = return address

        Call_Asm_Stack0 ExitMovies_c

        retn

; end procedure ExitMovies_asm2c


align 16
StartMovie_asm2c:

; [esp + 4] = void *theMovie
; [esp    ] = return address

        Call_Asm_Stack1 StartMovie_c

        retn

; end procedure StartMovie_asm2c


align 16
StopMovie_asm2c:

; [esp + 4] = void *theMovie
; [esp    ] = return address

        Call_Asm_Stack1 StopMovie_c

        retn

; end procedure StopMovie_asm2c


align 16
IsMovieDone_asm2c:

; [esp + 4] = void *theMovie
; [esp    ] = return address

        Call_Asm_Stack1 IsMovieDone_c

        retn

; end procedure IsMovieDone_asm2c


align 16
DisposeMovie_asm2c:

; [esp + 4] = void *theMovie
; [esp    ] = return address

        Call_Asm_Stack1 DisposeMovie_c

        retn

; end procedure DisposeMovie_asm2c


align 16
OpenMovieFile_asm2c:

; [esp + 3*4] = int8_t permission
; [esp + 2*4] = int16_t *resRefNum
; [esp +   4] = const void *fileSpec
; [esp      ] = return address

        Call_Asm_Stack3 OpenMovieFile_c

        retn

; end procedure OpenMovieFile_asm2c


align 16
CloseMovieFile_asm2c:

; [esp + 4] = int16_t resRefNum
; [esp    ] = return address

        Call_Asm_Stack1 CloseMovieFile_c

        retn

; end procedure CloseMovieFile_asm2c


align 16
NewMovieFromFile_asm2c:

; [esp + 6*4] = uint8_t *dataRefWasChanged
; [esp + 5*4] = int16_t newMovieFlags
; [esp + 4*4] = unsigned char *resName
; [esp + 3*4] = int16_t *resId
; [esp + 2*4] = int16_t resRefNum
; [esp +   4] = void **theMovie
; [esp      ] = return address

        Call_Asm_Stack6 NewMovieFromFile_c

        retn

; end procedure NewMovieFromFile_asm2c


align 16
GetMovieBox_asm2c:

; [esp + 2*4] = void *boxRect
; [esp +   4] = void *theMovie
; [esp      ] = return address

        Call_Asm_Stack2 GetMovieBox_c

        retn

; end procedure GetMovieBox_asm2c


align 16
NewMovieController_asm2c:

; [esp + 3*4] = int32_t someFlags
; [esp + 2*4] = const void *movieRect
; [esp +   4] = void *theMovie
; [esp      ] = return address

        Call_Asm_Stack3 NewMovieController_c

        retn

; end procedure NewMovieController_asm2c


align 16
DisposeMovieController_asm2c:

; [esp + 4] = void *mc
; [esp    ] = return address

        Call_Asm_Stack1 DisposeMovieController_c

        retn

; end procedure DisposeMovieController_asm2c


align 16
QTRegisterAccessKey_asm2c:

; [esp + 3*4] = void *accessKey
; [esp + 2*4] = int32_t flags
; [esp +   4] = unsigned char *accessKeyType
; [esp      ] = return address

        Call_Asm_Stack3 QTRegisterAccessKey_c

        retn

; end procedure QTRegisterAccessKey_asm2c


align 16
QTUnregisterAccessKey_asm2c:

; [esp + 3*4] = void *accessKey
; [esp + 2*4] = int32_t flags
; [esp +   4] = unsigned char *accessKeyType
; [esp      ] = return address

        Call_Asm_Stack3 QTUnregisterAccessKey_c

        retn

; end procedure QTUnregisterAccessKey_asm2c


align 16
MCIsPlayerEvent_asm2c:

; [esp + 2*4] = const void *e
; [esp +   4] = void *mc
; [esp      ] = return address

        Call_Asm_Stack2 MCIsPlayerEvent_c

        retn

; end procedure MCIsPlayerEvent_asm2c


align 16
MCDoAction_asm2c:

; [esp + 3*4] = void *params
; [esp + 2*4] = int16_t action
; [esp +   4] = void *mc
; [esp      ] = return address

        Call_Asm_Stack3 MCDoAction_c

        retn

; end procedure MCDoAction_asm2c


