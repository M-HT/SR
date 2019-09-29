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
    %define OpenGameDataFiles _OpenGameDataFiles
    %define CloseGameDataFiles _CloseGameDataFiles
    %define RecordOpen _RecordOpen
    %define RecordTryOpen _RecordTryOpen
    %define RecordGetDataFilePathAndOffset _RecordGetDataFilePathAndOffset
    %define RecordSeek _RecordSeek
    %define RecordRead _RecordRead
    %define RecordClose _RecordClose
    %define RecordGetSize _RecordGetSize
    %define GetFirstLevelRecordKey _GetFirstLevelRecordKey
    %define GetNextLevelRecordKey _GetNextLevelRecordKey
%endif

extern OpenGameDataFiles
extern CloseGameDataFiles
extern RecordOpen
extern RecordTryOpen
extern RecordGetDataFilePathAndOffset
extern RecordSeek
extern RecordRead
extern RecordClose
extern RecordGetSize
extern GetFirstLevelRecordKey
extern GetNextLevelRecordKey

extern GetRecordName_

global GetRecordName_asm
global _GetRecordName_asm
global MessageProc_asm
global _MessageProc_asm

global OpenGameDataFiles_asm2c
global CloseGameDataFiles_asm2c
global RecordOpen_asm2c
global RecordTryOpen_asm2c
global RecordGetDataFilePathAndOffset_asm2c
global RecordSeek_asm2c
global RecordRead_asm2c
global RecordClose_asm2c
global RecordGetSize_asm2c
global GetFirstLevelRecordKey_asm2c
global GetNextLevelRecordKey_asm2c

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
GetRecordName_asm:
_GetRecordName_asm:

; [esp + 4] = uint32_t RecordKey
; [esp    ] = return address

        jmp GetRecordName_

; end procedure GetRecordName_asm

align 16
MessageProc_asm:
_MessageProc_asm:

; [esp + 4*4] = uint32_t (*MessageProc)(const char *, uint32_t, uint32_t)
; [esp + 3*4] = uint32_t MessageCode
; [esp + 2*4] = uint32_t MessageType
; [esp +   4] = const char *MessageText
; [esp      ] = return address

        jmp dword [esp + 4*4]

; end procedure MessageProc_asm


align 16
OpenGameDataFiles_asm2c:

; [esp + 3*4] = uint32_t (*_MessageProc)(const char *, uint32_t, uint32_t)
; [esp + 2*4] = const char *_SourcePath
; [esp +   4] = const char *ManifestFilePath
; [esp      ] = return address

        Call_Asm_Stack3 OpenGameDataFiles

        retn

; end procedure OpenGameDataFiles_asm2c


align 16
CloseGameDataFiles_asm2c:

; [esp] = return address

        Call_Asm_Stack0 CloseGameDataFiles

        retn

; end procedure CloseGameDataFiles_asm2c


align 16
RecordOpen_asm2c:

; [esp + 4] = uint32_t RecordKey
; [esp    ] = return address

        Call_Asm_Stack1 RecordOpen

        retn

; end procedure RecordOpen_asm2c


align 16
RecordTryOpen_asm2c:

; [esp + 4] = uint32_t RecordKey
; [esp    ] = return address

        Call_Asm_Stack1 RecordTryOpen

        retn

; end procedure RecordTryOpen_asm2c


align 16
RecordGetDataFilePathAndOffset_asm2c:

; [esp + 2*4] = uint32_t *Offset
; [esp +   4] = uint32_t RecordKey
; [esp      ] = return address

        Call_Asm_Stack2 RecordGetDataFilePathAndOffset

        retn

; end procedure RecordGetDataFilePathAndOffset_asm2c


align 16
RecordSeek_asm2c:

; [esp + 3*4] = int32_t Whence
; [esp + 2*4] = int32_t Offset
; [esp +   4] = int32_t RecordHandle
; [esp      ] = return address

        Call_Asm_Stack3 RecordSeek

        retn

; end procedure RecordSeek_asm2c


align 16
RecordRead_asm2c:

; [esp + 3*4] = uint32_t NumberOfBytes
; [esp + 2*4] = uint8_t *ReadBuffer
; [esp +   4] = int32_t RecordHandle
; [esp      ] = return address

        Call_Asm_Stack3 RecordRead

        retn

; end procedure RecordRead_asm2c


align 16
RecordClose_asm2c:

; [esp + 4] = int32_t RecordHandle
; [esp    ] = return address

        Call_Asm_Stack1 RecordClose

        retn

; end procedure RecordClose_asm2c


align 16
RecordGetSize_asm2c:

; [esp + 4] = int32_t RecordHandle
; [esp    ] = return address

        Call_Asm_Stack1 RecordGetSize

        retn

; end procedure RecordGetSize_asm2c


align 16
GetFirstLevelRecordKey_asm2c:

; [esp] = return address

        Call_Asm_Stack0 GetFirstLevelRecordKey

        retn

; end procedure GetFirstLevelRecordKey_asm2c


align 16
GetNextLevelRecordKey_asm2c:

; [esp] = return address

        Call_Asm_Stack0 GetNextLevelRecordKey

        retn

; end procedure GetNextLevelRecordKey_asm2c

