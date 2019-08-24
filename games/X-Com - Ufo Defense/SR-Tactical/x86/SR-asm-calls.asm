;;
;;  Copyright (C) 2016-2019 Roman Pauer
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

%include "misc.inc"

%ifidn __OUTPUT_FORMAT__, win32
    %define Game_errno _Game_errno
    %define Game_dlseek _Game_dlseek
    %define Game_dopen _Game_dopen
    %define Game_dread _Game_dread
    %define Game_dclose _Game_dclose
    %define Game_DigPlay _Game_DigPlay
    %define Game_AudioCapabilities _Game_AudioCapabilities
    %define Game_StopSound _Game_StopSound
    %define Game_PostAudioPending _Game_PostAudioPending
    %define Game_SetPlayMode _Game_SetPlayMode
    %define Game_PendingAddress _Game_PendingAddress
    %define Game_ReportSemaphoreAddress _Game_ReportSemaphoreAddress
    %define Game_SetBackFillMode _Game_SetBackFillMode
    %define Game_VerifyDMA _Game_VerifyDMA
    %define Game_SetDPMIMode _Game_SetDPMIMode
    %define Game_FillSoundCfg _Game_FillSoundCfg
    %define Game_RealPtr _Game_RealPtr

    %define vfprintf _vfprintf
    %define vprintf _vprintf
    %define Game_WaitVerticalRetraceTicks _Game_WaitVerticalRetraceTicks

    %define Game_checkch _Game_checkch
    %define Game_getch _Game_getch

    %define Game_ExitMain_Asm _Game_ExitMain_Asm
    %define fclose _fclose
    %define feof _feof
    %define fgetc _fgetc
    %define Game_filelength2 _Game_filelength2
    %define Game_free _Game_free
    %define ftell _ftell
    %define isatty _isatty
    %define Game_malloc _Game_malloc
    %define time _time

    %define Game_fopen _Game_fopen
    %define fputc _fputc
    %define fputs _fputs
    %define strcat _strcat

    %define Game_ReadSong _Game_ReadSong
    %define memset _memset
    %define strncmp _strncmp

    %define fread _fread
    %define fwrite _fwrite
%endif

extern Game_errno
extern Game_dlseek
extern Game_dopen
extern Game_dread
extern Game_dclose
extern Game_DigPlay
extern Game_AudioCapabilities
extern Game_StopSound
extern Game_PostAudioPending
extern Game_SetPlayMode
extern Game_PendingAddress
extern Game_ReportSemaphoreAddress
extern Game_SetBackFillMode
extern Game_VerifyDMA
extern Game_SetDPMIMode
extern Game_FillSoundCfg
extern Game_RealPtr



; stack params
extern vfprintf
extern vprintf
extern Game_WaitVerticalRetraceTicks
; 0 params
extern Game_checkch
extern Game_getch
; 1 param
extern Game_ExitMain_Asm
extern fclose
extern feof
extern fgetc
extern Game_filelength2
extern Game_free
extern ftell
extern isatty
extern Game_malloc
extern time
; 2 params
extern Game_fopen
extern fputc
extern fputs
extern strcat
; 3 params
extern Game_ReadSong
extern memset
extern strncmp
; 4 params
extern fread
extern fwrite
; 5 params

extern errno_val

; null procedures
global SR___CHK
; stack params
global SR_cprintf
global SR_fprintf
global SR_printf

global SR_dlseek
global SR_dopen
global SR_dread
global SR_dclose

global SR_DigPlay
global SR_AudioCapabilities
global SR_StopSound
global SR_PostAudioPending
global SR_SetPlayMode
global SR_PendingAddress
global SR_ReportSemaphoreAddress
global SR_SetBackFillMode
global SR_VerifyDMA
global SR_SetDPMIMode
global SR_FillSoundCfg
global SR_RealPtr
; 0 params
global SR_checkch
global SR_getch
; 1 param
global SR_exit
global SR_fclose
global SR_feof
global SR_fgetc
global SR_filelength2
global SR__nfree
global SR_ftell
global SR_isatty
global SR__nmalloc
global SR_time
global SR_WaitVerticalRetraceTicks2
; 2 params
global SR_fopen
global SR_fputc
global SR_fputs
global SR_strcat
; 3 params
global SR_ReadSong
global SR_memset
global SR_strncmp
; 4 params
global SR_fread
global SR_fwrite
; 5 params

%include "SR-asm-calls.inc"

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
Game_Set_errno_Asm:

        push eax
        push ecx
        push edx

    ; remember original esp value
        mov eax, esp
    ; reserve 4 bytes on stack
        sub esp, byte 4
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp], eax

    ; stack is aligned to 16 bytes

        call Game_errno

        mov [errno_val], eax

    ; restore original esp value from stack
        mov esp, [esp]

        pop edx
        pop ecx
        pop eax

        retn

; end procedure Game_Set_errno_Asm

; null procedures
align 16
SR___CHK:

; [esp] = return address

        retn 4

; end procedure SR___CHK

; stack params
align 16
SR_fprintf:

; [esp + 3*4] = ...
; [esp + 2*4] = const char *format
; [esp +   4] = FILE *fp
; [esp      ] = return address

        Game_Call_Asm_VariableStack2 vfprintf,-1

; end procedure SR_fprintf

align 16
SR_cprintf:
SR_printf:

; [esp + 2*4] = ...
; [esp +   4] = const char *format
; [esp      ] = return address

        Game_Call_Asm_VariableStack1 vprintf,-1

; end procedure SR_printf

align 16
SR_dlseek:

; [esp + 3*4] = int whence
; [esp + 2*4] = int offset
; [esp +   4] = int fd
; [esp      ] = return address

        Game_Call_Asm_Stack3 Game_dlseek,-1

; end procedure SR_dlseek

align 16
SR_dopen:

; [esp + 2*4] = const char *mode
; [esp +   4] = const char *path
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_dopen,-1

; end procedure SR_dopen

align 16
SR_dread:

; [esp + 3*4] = int fd
; [esp + 2*4] = int count
; [esp +   4] = void *buf
; [esp      ] = return address

        Game_Call_Asm_Stack3 Game_dread,-1

; end procedure SR_dread

align 16
SR_dclose:

; [esp +   4] = int fd
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_dclose,-1

; end procedure SR_dclose

align 16
SR_DigPlay:

; [esp +   4] = SNDSTRUC *sndplay
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_DigPlay,-1

; end procedure SR_DigPlay

align 16
SR_AudioCapabilities:

        Game_Call_Asm_Reg0 Game_AudioCapabilities,-1

; end procedure SR_AudioCapabilities

align 16
SR_StopSound:

        Game_Call_Asm_Reg0 Game_StopSound,-1

; end procedure SR_StopSound

align 16
SR_PostAudioPending:

; [esp +   4] = SNDSTRUC *sndplay
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_PostAudioPending,-1

; end procedure SR_PostAudioPending

align 16
SR_SetPlayMode:

; [esp +   4] = short mode
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_SetPlayMode,-1

; end procedure SR_SetPlayMode

align 16
SR_PendingAddress:

        Game_Call_Asm_Reg0 Game_PendingAddress,-1

; end procedure SR_PendingAddress

align 16
SR_ReportSemaphoreAddress:

        Game_Call_Asm_Reg0 Game_ReportSemaphoreAddress,-1

; end procedure SR_ReportSemaphoreAddress

align 16
SR_SetBackFillMode:

; [esp +   4] = short mode
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_SetBackFillMode,-1

; end procedure SR_SetBackFillMode

align 16
SR_VerifyDMA:

; [esp +   8] = short length
; [esp +   4] = char *data
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_VerifyDMA,-1

; end procedure SR_VerifyDMA

align 16
SR_SetDPMIMode:

; [esp +   4] = short mode
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_SetDPMIMode,-1

; end procedure SR_SetDPMIMode

align 16
SR_FillSoundCfg:

; [esp +   8] = int count
; [esp +   4] = void *buf
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_FillSoundCfg,-1

; end procedure SR_FillSoundCfg

align 16
SR_RealPtr:

; [esp +   4] = char *real
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_RealPtr,-1

; end procedure SR_RealPtr


; 0 params
align 16
SR_checkch:

        Game_Call_Asm_Reg0 Game_checkch,-1

; end procedure SR_checkch

align 16
SR_getch:

        Game_Call_Asm_Reg0 Game_getch,-1

; end procedure SR_getch


; 1 param
align 16
SR_exit:

; eax = int status

        jmp Game_ExitMain_Asm

; end procedure SR_exit

align 16
SR_fclose:

; eax = FILE *fp

        Game_Call_Asm_Reg1 fclose,'get_errno_val'

; end procedure SR_fclose

align 16
SR_feof:

; eax = FILE *fp

        Game_Call_Asm_Reg1 feof,-1

; end procedure SR_feof

align 16
SR_fgetc:

; eax = FILE *fp

        Game_Call_Asm_Reg1 fgetc,'get_errno_val'

; end procedure SR_fgetc

align 16
SR_filelength2:

; eax = FILE *fp

        Game_Call_Asm_Reg1 Game_filelength2,-1

; end procedure SR_filelength2

align 16
SR__nfree:

; eax = void __near *ptr

        Game_Call_Asm_Reg1 Game_free,-1

; end procedure SR__nfree

align 16
SR_ftell:

; eax = FILE *fp

        Game_Call_Asm_Reg1 ftell,'get_errno_val'

; end procedure SR_ftell

align 16
SR_isatty:

; eax = int handle

        Game_Call_Asm_Reg1 isatty,'get_errno_val'

; end procedure SR_isatty

align 16
SR__nmalloc:

; eax = size_t size

        Game_Call_Asm_Reg1 Game_malloc,-1

; end procedure SR__nmalloc

align 16
SR_time:

; eax = time_t *tloc

        Game_Call_Asm_Reg1 time,-1

; end procedure SR_time

align 16
SR_WaitVerticalRetraceTicks2:

; eax = int ticks

        Game_Call_Asm_Reg1 Game_WaitVerticalRetraceTicks,-1

; end procedure SR_WaitVerticalRetraceTicks2

; 2 params
align 16
SR_fopen:

; eax = const char *filename
; edx = const char *mode

; errno_val is set inside Game_fopen

        Game_Call_Asm_Reg2 Game_fopen,-1

; end procedure SR_fopen

align 16
SR_fputc:

; eax = int c
; edx = FILE *fp

    Game_Call_Asm_Reg2 fputc,'get_errno_val'

; end procedure SR_fputc

align 16
SR_fputs:

;	return value:
;		<0 => -1
;		watcom expects -1 (on error)
;		fputs returns negative value

; eax = const char *buf
; edx = FILE *fp

;	Game_Call_Asm_Reg2 fputs,'get_errno_val'
        push ecx

    ; remember original esp value
        mov ecx, esp
    ; reserve 12 bytes on stack
        sub esp, byte 12
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
    ; save original esp value on stack
        mov [esp + 2*4], ecx

    ; put function arguments to stack
        mov [esp + 1*4], edx
        mov [esp], eax
    ; stack is aligned to 16 bytes

        call fputs

    ; restore original esp value from stack
        mov esp, [esp + 2*4]

        cmp eax, 0
        jge SR_fputs_1
        mov eax, -1
    SR_fputs_1:

        call Game_Set_errno_Asm

        pop ecx

        retn

; end procedure SR_fputs

align 16
align 16
SR_strcat:

; eax = char *dst
; edx = const char *src

        Game_Call_Asm_Reg2 strcat,-1

; end procedure SR_strcat


; 3 params
align 16
SR_ReadSong:

; eax = const char *catalog_name
; edx = int index
; ebx = uint8_t *buf

        Game_Call_Asm_Reg3 Game_ReadSong,-1

; end procedure SR_ReadSong

align 16
SR_memset:

; eax = void *dst
; edx = int c
; ebx = size_t length

        Game_Call_Asm_Reg3 memset,-1

; end procedure SR_memset

align 16
SR_strncmp:

; eax = const char *s1
; edx = const char *s2
; ebx = size_t n

        Game_Call_Asm_Reg3 strncmp,-1

; end procedure SR_strncmp


; 4 params
align 16
SR_fread:

; eax = void *buf
; edx = size_t elsize
; ebx = size_t nelem
; ecx = FILE *fp

        Game_Call_Asm_Reg4 fread,'get_errno_val'

; end procedure SR_fread

align 16
SR_fwrite:

; eax = void *buf
; edx = size_t elsize
; ebx = size_t nelem
; ecx = FILE *fp

        Game_Call_Asm_Reg4 fwrite,'get_errno_val'

; end procedure SR_fwrite

; 5 params
