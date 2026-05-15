;;
;;  Copyright (C) 2019-2026 Roman Pauer
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

%include "x64inc.inc"
%include "asm-calls.inc"

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

%ifidn __OUTPUT_FORMAT__, elf64
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif
%ifidn __OUTPUT_FORMAT__, win64
section_prolog:
        SECTION_PROLOG
%endif

align 16
OpenGameDataFiles_asm2c:

; [esp + 3*4] = uint32_t (*_MessageProc)(const char *, uint32_t, uint32_t)
; [esp + 2*4] = const char *_SourcePath
; [esp +   4] = const char *ManifestFilePath
; [esp      ] = return address

        Call_Asm_Stack3 OpenGameDataFiles

; end procedure OpenGameDataFiles_asm2c


align 16
CloseGameDataFiles_asm2c:

; [esp] = return address

        Call_Asm_Stack0 CloseGameDataFiles

; end procedure CloseGameDataFiles_asm2c


align 16
RecordOpen_asm2c:

; [esp + 4] = uint32_t RecordKey
; [esp    ] = return address

        Call_Asm_Stack1 RecordOpen

; end procedure RecordOpen_asm2c


align 16
RecordTryOpen_asm2c:

; [esp + 4] = uint32_t RecordKey
; [esp    ] = return address

        Call_Asm_Stack1 RecordTryOpen

; end procedure RecordTryOpen_asm2c


align 16
RecordGetDataFilePathAndOffset_asm2c:

; [esp + 2*4] = uint32_t *Offset
; [esp +   4] = uint32_t RecordKey
; [esp      ] = return address

        Call_Asm_Stack2 RecordGetDataFilePathAndOffset

; end procedure RecordGetDataFilePathAndOffset_asm2c


align 16
RecordSeek_asm2c:

; [esp + 3*4] = int32_t Whence
; [esp + 2*4] = int32_t Offset
; [esp +   4] = int32_t RecordHandle
; [esp      ] = return address

        Call_Asm_Stack3 RecordSeek

; end procedure RecordSeek_asm2c


align 16
RecordRead_asm2c:

; [esp + 3*4] = uint32_t NumberOfBytes
; [esp + 2*4] = uint8_t *ReadBuffer
; [esp +   4] = int32_t RecordHandle
; [esp      ] = return address

        Call_Asm_Stack3 RecordRead

; end procedure RecordRead_asm2c


align 16
RecordClose_asm2c:

; [esp + 4] = int32_t RecordHandle
; [esp    ] = return address

        Call_Asm_Stack1 RecordClose

; end procedure RecordClose_asm2c


align 16
RecordGetSize_asm2c:

; [esp + 4] = int32_t RecordHandle
; [esp    ] = return address

        Call_Asm_Stack1 RecordGetSize

; end procedure RecordGetSize_asm2c


align 16
GetFirstLevelRecordKey_asm2c:

; [esp] = return address

        Call_Asm_Stack0 GetFirstLevelRecordKey

; end procedure GetFirstLevelRecordKey_asm2c


align 16
GetNextLevelRecordKey_asm2c:

; [esp] = return address

        Call_Asm_Stack0 GetNextLevelRecordKey

; end procedure GetNextLevelRecordKey_asm2c


%ifidn __OUTPUT_FORMAT__, win64
section_end:

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
%endif
