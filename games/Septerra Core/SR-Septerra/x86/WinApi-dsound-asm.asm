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
    %define DirectSoundCreate_c _DirectSoundCreate_c

    %define IDirectSound_QueryInterface_c _IDirectSound_QueryInterface_c
    %define IDirectSound_AddRef_c _IDirectSound_AddRef_c
    %define IDirectSound_Release_c _IDirectSound_Release_c
    %define IDirectSound_CreateSoundBuffer_c _IDirectSound_CreateSoundBuffer_c
    %define IDirectSound_GetCaps_c _IDirectSound_GetCaps_c
    %define IDirectSound_DuplicateSoundBuffer_c _IDirectSound_DuplicateSoundBuffer_c
    %define IDirectSound_SetCooperativeLevel_c _IDirectSound_SetCooperativeLevel_c
    %define IDirectSound_Compact_c _IDirectSound_Compact_c
    %define IDirectSound_GetSpeakerConfig_c _IDirectSound_GetSpeakerConfig_c
    %define IDirectSound_SetSpeakerConfig_c _IDirectSound_SetSpeakerConfig_c
    %define IDirectSound_Initialize_c _IDirectSound_Initialize_c

    %define IDirectSoundBuffer_QueryInterface_c _IDirectSoundBuffer_QueryInterface_c
    %define IDirectSoundBuffer_AddRef_c _IDirectSoundBuffer_AddRef_c
    %define IDirectSoundBuffer_Release_c _IDirectSoundBuffer_Release_c
    %define IDirectSoundBuffer_GetCaps_c _IDirectSoundBuffer_GetCaps_c
    %define IDirectSoundBuffer_GetCurrentPosition_c _IDirectSoundBuffer_GetCurrentPosition_c
    %define IDirectSoundBuffer_GetFormat_c _IDirectSoundBuffer_GetFormat_c
    %define IDirectSoundBuffer_GetVolume_c _IDirectSoundBuffer_GetVolume_c
    %define IDirectSoundBuffer_GetPan_c _IDirectSoundBuffer_GetPan_c
    %define IDirectSoundBuffer_GetFrequency_c _IDirectSoundBuffer_GetFrequency_c
    %define IDirectSoundBuffer_GetStatus_c _IDirectSoundBuffer_GetStatus_c
    %define IDirectSoundBuffer_Initialize_c _IDirectSoundBuffer_Initialize_c
    %define IDirectSoundBuffer_Lock_c _IDirectSoundBuffer_Lock_c
    %define IDirectSoundBuffer_Play_c _IDirectSoundBuffer_Play_c
    %define IDirectSoundBuffer_SetCurrentPosition_c _IDirectSoundBuffer_SetCurrentPosition_c
    %define IDirectSoundBuffer_SetFormat_c _IDirectSoundBuffer_SetFormat_c
    %define IDirectSoundBuffer_SetVolume_c _IDirectSoundBuffer_SetVolume_c
    %define IDirectSoundBuffer_SetPan_c _IDirectSoundBuffer_SetPan_c
    %define IDirectSoundBuffer_SetFrequency_c _IDirectSoundBuffer_SetFrequency_c
    %define IDirectSoundBuffer_Stop_c _IDirectSoundBuffer_Stop_c
    %define IDirectSoundBuffer_Unlock_c _IDirectSoundBuffer_Unlock_c
    %define IDirectSoundBuffer_Restore_c _IDirectSoundBuffer_Restore_c

    %define IDirectSoundVtbl_asm2c _IDirectSoundVtbl_asm2c
    %define IDirectSoundBufferVtbl_asm2c _IDirectSoundBufferVtbl_asm2c
%endif

extern DirectSoundCreate_c

extern IDirectSound_QueryInterface_c
extern IDirectSound_AddRef_c
extern IDirectSound_Release_c
extern IDirectSound_CreateSoundBuffer_c
extern IDirectSound_GetCaps_c
extern IDirectSound_DuplicateSoundBuffer_c
extern IDirectSound_SetCooperativeLevel_c
extern IDirectSound_Compact_c
extern IDirectSound_GetSpeakerConfig_c
extern IDirectSound_SetSpeakerConfig_c
extern IDirectSound_Initialize_c

extern IDirectSoundBuffer_QueryInterface_c
extern IDirectSoundBuffer_AddRef_c
extern IDirectSoundBuffer_Release_c
extern IDirectSoundBuffer_GetCaps_c
extern IDirectSoundBuffer_GetCurrentPosition_c
extern IDirectSoundBuffer_GetFormat_c
extern IDirectSoundBuffer_GetVolume_c
extern IDirectSoundBuffer_GetPan_c
extern IDirectSoundBuffer_GetFrequency_c
extern IDirectSoundBuffer_GetStatus_c
extern IDirectSoundBuffer_Initialize_c
extern IDirectSoundBuffer_Lock_c
extern IDirectSoundBuffer_Play_c
extern IDirectSoundBuffer_SetCurrentPosition_c
extern IDirectSoundBuffer_SetFormat_c
extern IDirectSoundBuffer_SetVolume_c
extern IDirectSoundBuffer_SetPan_c
extern IDirectSoundBuffer_SetFrequency_c
extern IDirectSoundBuffer_Stop_c
extern IDirectSoundBuffer_Unlock_c
extern IDirectSoundBuffer_Restore_c

global DirectSoundCreate_asm2c

global IDirectSoundVtbl_asm2c
global IDirectSoundBufferVtbl_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
DirectSoundCreate_asm2c:

; [esp + 3*4] = LPUNKNOWN pUnkOuter
; [esp + 2*4] = LPDIRECTSOUND* ppDS
; [esp +   4] = LPGUID lpGuid
; [esp      ] = return address

        Call_Asm_Stack3 DirectSoundCreate_c

        retn 3*4

; end procedure DirectSoundCreate_asm2c


align 16
IDirectSound_QueryInterface_asm2c:

; [esp + 3*4] = LPVOID * ppvObj
; [esp + 2*4] = REFIID riid
; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectSound_QueryInterface_c

        retn 3*4

; end procedure IDirectSound_QueryInterface_asm2c


align 16
IDirectSound_AddRef_asm2c:

; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectSound_AddRef_c

        retn 4

; end procedure IDirectSound_AddRef_asm2c


align 16
IDirectSound_Release_asm2c:

; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectSound_Release_c

        retn 4

; end procedure IDirectSound_Release_asm2c


align 16
IDirectSound_CreateSoundBuffer_asm2c:

; [esp + 4*4] = LPUNKNOWN pUnkOuter
; [esp + 3*4] = LPDIRECTSOUNDBUFFER * ppDSBuffer
; [esp + 2*4] = LPCDSBUFFERDESC pcDSBufferDesc
; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack4 IDirectSound_CreateSoundBuffer_c

        retn 4*4

; end procedure IDirectSound_CreateSoundBuffer_asm2c


align 16
IDirectSound_GetCaps_asm2c:

; [esp + 2*4] = LPDSCAPS pDSCaps
; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSound_GetCaps_c

        retn 2*4

; end procedure IDirectSound_GetCaps_asm2c


align 16
IDirectSound_DuplicateSoundBuffer_asm2c:

; [esp + 3*4] = LPDIRECTSOUNDBUFFER * ppDSBufferDuplicate
; [esp + 2*4] = LPDIRECTSOUNDBUFFER pDSBufferOriginal
; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectSound_DuplicateSoundBuffer_c

        retn 3*4

; end procedure IDirectSound_DuplicateSoundBuffer_asm2c


align 16
IDirectSound_SetCooperativeLevel_asm2c:

; [esp + 3*4] = DWORD dwLevel
; [esp + 2*4] = HWND hwnd
; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectSound_SetCooperativeLevel_c

        retn 3*4

; end procedure IDirectSound_SetCooperativeLevel_asm2c


align 16
IDirectSound_Compact_asm2c:

; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectSound_Compact_c

        retn 4

; end procedure IDirectSound_Compact_asm2c


align 16
IDirectSound_GetSpeakerConfig_asm2c:

; [esp + 2*4] = LPDWORD pdwSpeakerConfig
; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSound_GetSpeakerConfig_c

        retn 2*4

; end procedure IDirectSound_GetSpeakerConfig_asm2c


align 16
IDirectSound_SetSpeakerConfig_asm2c:

; [esp + 2*4] = DWORD dwSpeakerConfig
; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSound_SetSpeakerConfig_c

        retn 2*4

; end procedure IDirectSound_SetSpeakerConfig_asm2c


align 16
IDirectSound_Initialize_asm2c:

; [esp + 2*4] = LPCGUID pcGuidDevice
; [esp +   4] = struct IDirectSound *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSound_Initialize_c

        retn 2*4

; end procedure IDirectSound_Initialize_asm2c


align 16
IDirectSoundBuffer_QueryInterface_asm2c:

; [esp + 3*4] = LPVOID * ppvObj
; [esp + 2*4] = REFIID riid
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectSoundBuffer_QueryInterface_c

        retn 3*4

; end procedure IDirectSoundBuffer_QueryInterface_asm2c


align 16
IDirectSoundBuffer_AddRef_asm2c:

; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectSoundBuffer_AddRef_c

        retn 4

; end procedure IDirectSoundBuffer_AddRef_asm2c


align 16
IDirectSoundBuffer_Release_asm2c:

; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectSoundBuffer_Release_c

        retn 4

; end procedure IDirectSoundBuffer_Release_asm2c


align 16
IDirectSoundBuffer_GetCaps_asm2c:

; [esp + 2*4] = LPDSBCAPS pDSBufferCaps
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_GetCaps_c

        retn 2*4

; end procedure IDirectSoundBuffer_GetCaps_asm2c


align 16
IDirectSoundBuffer_GetCurrentPosition_asm2c:

; [esp + 3*4] = LPDWORD pdwCurrentWriteCursor
; [esp + 2*4] = LPDWORD pdwCurrentPlayCursor
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectSoundBuffer_GetCurrentPosition_c

        retn 3*4

; end procedure IDirectSoundBuffer_GetCurrentPosition_asm2c


align 16
IDirectSoundBuffer_GetFormat_asm2c:

; [esp + 4*4] = LPDWORD pdwSizeWritten
; [esp + 3*4] = DWORD dwSizeAllocated
; [esp + 2*4] = LPWAVEFORMATEX pwfxFormat
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack4 IDirectSoundBuffer_GetFormat_c

        retn 4*4

; end procedure IDirectSoundBuffer_GetFormat_asm2c


align 16
IDirectSoundBuffer_GetVolume_asm2c:

; [esp + 2*4] = LPLONG plVolume
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_GetVolume_c

        retn 2*4

; end procedure IDirectSoundBuffer_GetVolume_asm2c


align 16
IDirectSoundBuffer_GetPan_asm2c:

; [esp + 2*4] = LPLONG plPan
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_GetPan_c

        retn 2*4

; end procedure IDirectSoundBuffer_GetPan_asm2c


align 16
IDirectSoundBuffer_GetFrequency_asm2c:

; [esp + 2*4] = LPDWORD pdwFrequency
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_GetFrequency_c

        retn 2*4

; end procedure IDirectSoundBuffer_GetFrequency_asm2c


align 16
IDirectSoundBuffer_GetStatus_asm2c:

; [esp + 2*4] = LPDWORD pdwStatus
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_GetStatus_c

        retn 2*4

; end procedure IDirectSoundBuffer_GetStatus_asm2c


align 16
IDirectSoundBuffer_Initialize_asm2c:

; [esp + 3*4] = LPCDSBUFFERDESC pcDSBufferDesc
; [esp + 2*4] = LPDIRECTSOUND pDirectSound
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack3 IDirectSoundBuffer_Initialize_c

        retn 3*4

; end procedure IDirectSoundBuffer_Initialize_asm2c


align 16
IDirectSoundBuffer_Lock_asm2c:

; [esp + 8*4] = DWORD dwFlags
; [esp + 7*4] = LPDWORD pdwAudioBytes2
; [esp + 6*4] = LPVOID * ppvAudioPtr2
; [esp + 5*4] = LPDWORD pdwAudioBytes1
; [esp + 4*4] = LPVOID * ppvAudioPtr1
; [esp + 3*4] = DWORD dwBytes
; [esp + 2*4] = DWORD dwOffset
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack8 IDirectSoundBuffer_Lock_c

        retn 8*4

; end procedure IDirectSoundBuffer_Lock_asm2c


align 16
IDirectSoundBuffer_Play_asm2c:

; [esp + 4*4] = DWORD dwFlags
; [esp + 3*4] = DWORD dwPriority
; [esp + 2*4] = DWORD dwReserved1
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack4 IDirectSoundBuffer_Play_c

        retn 4*4

; end procedure IDirectSoundBuffer_Play_asm2c


align 16
IDirectSoundBuffer_SetCurrentPosition_asm2c:

; [esp + 2*4] = DWORD dwNewPosition
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_SetCurrentPosition_c

        retn 2*4

; end procedure IDirectSoundBuffer_SetCurrentPosition_asm2c


align 16
IDirectSoundBuffer_SetFormat_asm2c:

; [esp + 2*4] = LPCWAVEFORMATEX pcfxFormat
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_SetFormat_c

        retn 2*4

; end procedure IDirectSoundBuffer_SetFormat_asm2c


align 16
IDirectSoundBuffer_SetVolume_asm2c:

; [esp + 2*4] = LONG lVolume
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_SetVolume_c

        retn 2*4

; end procedure IDirectSoundBuffer_SetVolume_asm2c


align 16
IDirectSoundBuffer_SetPan_asm2c:

; [esp + 2*4] = LONG lPan
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_SetPan_c

        retn 2*4

; end procedure IDirectSoundBuffer_SetPan_asm2c


align 16
IDirectSoundBuffer_SetFrequency_asm2c:

; [esp + 2*4] = DWORD dwFrequency
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack2 IDirectSoundBuffer_SetFrequency_c

        retn 2*4

; end procedure IDirectSoundBuffer_SetFrequency_asm2c


align 16
IDirectSoundBuffer_Stop_asm2c:

; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectSoundBuffer_Stop_c

        retn 4

; end procedure IDirectSoundBuffer_Stop_asm2c


align 16
IDirectSoundBuffer_Unlock_asm2c:

; [esp + 5*4] = DWORD dwAudioBytes2
; [esp + 4*4] = LPVOID pvAudioPtr2
; [esp + 3*4] = DWORD dwAudioBytes1
; [esp + 2*4] = LPVOID pvAudioPtr1
; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack5 IDirectSoundBuffer_Unlock_c

        retn 5*4

; end procedure IDirectSoundBuffer_Unlock_asm2c


align 16
IDirectSoundBuffer_Restore_asm2c:

; [esp +   4] = struct IDirectSoundBuffer *lpThis
; [esp      ] = return address

        Call_Asm_Stack1 IDirectSoundBuffer_Restore_c

        retn 4

; end procedure IDirectSoundBuffer_Restore_asm2c



%ifidn __OUTPUT_FORMAT__, elf32
section .rdata progbits alloc noexec nowrite align=8
%else
section .rdata rdata align=8
%endif

IDirectSoundVtbl_asm2c:
dd IDirectSound_QueryInterface_asm2c
dd IDirectSound_AddRef_asm2c
dd IDirectSound_Release_asm2c
dd IDirectSound_CreateSoundBuffer_asm2c
dd IDirectSound_GetCaps_asm2c
dd IDirectSound_DuplicateSoundBuffer_asm2c
dd IDirectSound_SetCooperativeLevel_asm2c
dd IDirectSound_Compact_asm2c
dd IDirectSound_GetSpeakerConfig_asm2c
dd IDirectSound_SetSpeakerConfig_asm2c
dd IDirectSound_Initialize_asm2c

IDirectSoundBufferVtbl_asm2c:
dd IDirectSoundBuffer_QueryInterface_asm2c
dd IDirectSoundBuffer_AddRef_asm2c
dd IDirectSoundBuffer_Release_asm2c
dd IDirectSoundBuffer_GetCaps_asm2c
dd IDirectSoundBuffer_GetCurrentPosition_asm2c
dd IDirectSoundBuffer_GetFormat_asm2c
dd IDirectSoundBuffer_GetVolume_asm2c
dd IDirectSoundBuffer_GetPan_asm2c
dd IDirectSoundBuffer_GetFrequency_asm2c
dd IDirectSoundBuffer_GetStatus_asm2c
dd IDirectSoundBuffer_Initialize_asm2c
dd IDirectSoundBuffer_Lock_asm2c
dd IDirectSoundBuffer_Play_asm2c
dd IDirectSoundBuffer_SetCurrentPosition_asm2c
dd IDirectSoundBuffer_SetFormat_asm2c
dd IDirectSoundBuffer_SetVolume_asm2c
dd IDirectSoundBuffer_SetPan_asm2c
dd IDirectSoundBuffer_SetFrequency_asm2c
dd IDirectSoundBuffer_Stop_asm2c
dd IDirectSoundBuffer_Unlock_asm2c
dd IDirectSoundBuffer_Restore_asm2c

