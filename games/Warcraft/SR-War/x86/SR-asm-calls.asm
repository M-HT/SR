;;
;;  Copyright (C) 2016-2026 Roman Pauer
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

    %define CLIB_vfprintf _CLIB_vfprintf
    %define CLIB_vprintf _CLIB_vprintf
    %define CLIB_vsprintf _CLIB_vsprintf

    %define Game_AIL_active_sample_count _Game_AIL_active_sample_count
    %define Game_AIL_allocate_sample_handle _Game_AIL_allocate_sample_handle
    %define Game_AIL_end_sample _Game_AIL_end_sample
    %define Game_AIL_init_sample _Game_AIL_init_sample
    %define Game_AIL_register_EOS_callback _Game_AIL_register_EOS_callback
    %define Game_AIL_release_sample_handle _Game_AIL_release_sample_handle
    %define Game_AIL_sample_status _Game_AIL_sample_status
    %define Game_AIL_set_digital_master_volume _Game_AIL_set_digital_master_volume
    %define Game_AIL_set_sample_address _Game_AIL_set_sample_address
    %define Game_AIL_set_sample_loop_count _Game_AIL_set_sample_loop_count
    %define Game_AIL_set_sample_pan _Game_AIL_set_sample_pan
    %define Game_AIL_set_sample_playback_rate _Game_AIL_set_sample_playback_rate
    %define Game_AIL_set_sample_type _Game_AIL_set_sample_type
    %define Game_AIL_set_sample_volume _Game_AIL_set_sample_volume
    %define Game_AIL_start_sample _Game_AIL_start_sample

    %define Game_AIL_allocate_sequence_handle _Game_AIL_allocate_sequence_handle
    %define Game_AIL_end_sequence _Game_AIL_end_sequence
    %define Game_AIL_init_sequence _Game_AIL_init_sequence
    %define Game_AIL_release_sequence_handle _Game_AIL_release_sequence_handle
    %define Game_AIL_resume_sequence _Game_AIL_resume_sequence
    %define Game_AIL_sequence_status _Game_AIL_sequence_status
    %define Game_AIL_set_sequence_loop_count _Game_AIL_set_sequence_loop_count
    %define Game_AIL_set_sequence_volume _Game_AIL_set_sequence_volume
    %define Game_AIL_start_sequence _Game_AIL_start_sequence
    %define Game_AIL_stop_sequence _Game_AIL_stop_sequence

    %define Game_clock _Game_clock

    %define Game_cputs _Game_cputs
    %define Game_delay _Game_delay
    %define Game_dos_gettime _Game_dos_gettime
    %define Game_ExitMain_Asm _Game_ExitMain_Asm
    %define Game_fclose _Game_fclose
    %define Game_fgetc _Game_fgetc
    %define Game_free _Game_free
    %define Game_ftell _Game_ftell
    %define Game_inp _Game_inp
    %define Game_malloc _Game_malloc
    %define puts _puts
    %define strlen _strlen
    %define Game_time _Game_time
    %define Game_unlink _Game_unlink

    %define Game_fopen _Game_fopen
    %define Game_fputc _Game_fputc
    %define Game_fputs _Game_fputs
    %define Game_outp _Game_outp
    %define Game_signal _Game_signal
    %define strcat _strcat
    %define strcmp _strcmp
    %define strcpy _strcpy
    %define strrchr _strrchr

    %define Game_fgets _Game_fgets
    %define Game_fseek _Game_fseek
    %define memcmp _memcmp
    %define memcpy _memcpy
    %define memmove _memmove
    %define memset _memset
    %define strncmp _strncmp
    %define strncasecmp _strncasecmp
    %define strtoul _strtoul

    %define Game_fmemset _Game_fmemset
    %define Game_fread _Game_fread
    %define Game_fwrite _Game_fwrite
    %define Game_int386x _Game_int386x
    %define Game_setvbuf _Game_setvbuf

    %define Game_fmemcmp _Game_fmemcmp
    %define Game_fmemcpy _Game_fmemcpy
%endif

extern Game_errno


; stack params
extern CLIB_vfprintf
extern CLIB_vprintf
extern CLIB_vsprintf

extern Game_AIL_active_sample_count
extern Game_AIL_allocate_sample_handle
extern Game_AIL_end_sample
extern Game_AIL_init_sample
extern Game_AIL_register_EOS_callback
extern Game_AIL_release_sample_handle
extern Game_AIL_sample_status
extern Game_AIL_set_digital_master_volume
extern Game_AIL_set_sample_address
extern Game_AIL_set_sample_loop_count
extern Game_AIL_set_sample_pan
extern Game_AIL_set_sample_playback_rate
extern Game_AIL_set_sample_type
extern Game_AIL_set_sample_volume
extern Game_AIL_start_sample

extern Game_AIL_allocate_sequence_handle
extern Game_AIL_end_sequence
extern Game_AIL_init_sequence
extern Game_AIL_release_sequence_handle
extern Game_AIL_resume_sequence
extern Game_AIL_sequence_status
extern Game_AIL_set_sequence_loop_count
extern Game_AIL_set_sequence_volume
extern Game_AIL_start_sequence
extern Game_AIL_stop_sequence
; 0 params
extern Game_clock
; 1 param
extern Game_cputs
extern Game_delay
extern Game_dos_gettime
extern Game_ExitMain_Asm
extern Game_fclose
extern Game_fgetc
extern Game_free
extern Game_ftell
extern Game_inp
extern Game_malloc
extern puts
extern strlen
extern Game_time
extern Game_unlink
; 2 params
extern Game_fopen
extern Game_fputc
extern Game_fputs
extern Game_outp
extern Game_signal
extern strcat
extern strcmp
extern strcpy
extern strrchr
; 3 params
extern Game_fgets
extern Game_fseek
extern memcmp
extern memcpy
extern memmove
extern memset
extern strncmp
extern strncasecmp
extern strtoul
; 4 params
extern Game_fmemset
extern Game_fread
extern Game_fwrite
extern Game_int386x
extern Game_setvbuf
; 5 params
extern Game_fmemcmp
extern Game_fmemcpy

extern errno_val

; null procedures
global SR___GETDS
global SR__harderr
; stack params
global SR_fprintf
global SR_printf
global SR_sprintf

global SR_AIL_active_sample_count
global SR_AIL_allocate_sample_handle
global SR_AIL_end_sample
global SR_AIL_init_sample
global SR_AIL_register_EOS_callback
global SR_AIL_release_sample_handle
global SR_AIL_sample_status
global SR_AIL_set_digital_master_volume
global SR_AIL_set_sample_address
global SR_AIL_set_sample_loop_count
global SR_AIL_set_sample_pan
global SR_AIL_set_sample_playback_rate
global SR_AIL_set_sample_type
global SR_AIL_set_sample_volume
global SR_AIL_start_sample

global SR_AIL_allocate_sequence_handle
global SR_AIL_end_sequence
global SR_AIL_init_sequence
global SR_AIL_release_sequence_handle
global SR_AIL_resume_sequence
global SR_AIL_sequence_status
global SR_AIL_set_sequence_loop_count
global SR_AIL_set_sequence_volume
global SR_AIL_start_sequence
global SR_AIL_stop_sequence
; 0 params
global SR___clock
global SR_j___clock
; 1 param
global SR_cputs
global SR___delay
global SR_j___delay
global SR__dos_gettime
global SR_exit
global SR_fclose
global SR_fgetc
global SR__nfree
global SR_ftell
global SR_inp
global SR__nmalloc
global SR_puts
global SR_strlen
global SR_time
global SR_unlink
global SR_j_unlink
; 2 params
global SR_fopen
global SR_fputc
global SR_fputs
global SR_outp
global SR_signal
global SR_strcat
global SR_strcmp
global SR_strcpy
global SR_strrchr
global SR_vprintf
; 3 params
global SR_fgets
global SR_fseek
global SR_memcmp
global SR_memcpy
global SR_memmove
global SR_memset
global SR_strncmp
global SR_strnicmp
global SR_strtoul
; 4 params
global SR__fmemset
global SR_fread
global SR_fwrite
global SR_int386x
global SR_setvbuf
; 5 params
global SR__fmemcmp
global SR__fmemcpy

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
SR___GETDS:
SR__harderr:
null_procedure:

        retn

; end procedure null_procedure

; stack params
align 16
SR_fprintf:

; [esp + 3*4] = ...
; [esp + 2*4] = const char *format
; [esp +   4] = FILE *fp
; [esp      ] = return address

        Game_Call_Asm_VariableStack2 CLIB_vfprintf,-1

; end procedure SR_fprintf

align 16
SR_printf:

; [esp + 2*4] = ...
; [esp +   4] = const char *format
; [esp      ] = return address

        Game_Call_Asm_VariableStack1 CLIB_vprintf,-1

; end procedure SR_printf

align 16
SR_sprintf:

; [esp + 3*4] = ...
; [esp + 2*4] = const char *format
; [esp +   4] = char *buf
; [esp      ] = return address

        Game_Call_Asm_VariableStack2 CLIB_vsprintf,-1

; end procedure SR_sprintf

align 16
SR_AIL_active_sample_count:

; [esp +   4] = void *dig
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_active_sample_count,-1

; end procedure SR_AIL_active_sample_count

align 16
SR_AIL_allocate_sample_handle:

; [esp +   4] = void *dig
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_allocate_sample_handle,-1

; end procedure SR_AIL_allocate_sample_handle

align 16
SR_AIL_end_sample:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_end_sample,-1

; end procedure SR_AIL_end_sample

align 16
SR_AIL_init_sample:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_init_sample,-1

; end procedure SR_AIL_init_sample

align 16
SR_AIL_register_EOS_callback:

; [esp + 2*4] = AIL_sample_CB EOS
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_AIL_register_EOS_callback,-1

; end procedure SR_AIL_register_EOS_callback

align 16
SR_AIL_release_sample_handle:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_release_sample_handle,-1

; end procedure SR_AIL_release_sample_handle

align 16
SR_AIL_sample_status:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_sample_status,-1

; end procedure SR_AIL_sample_status

align 16
SR_AIL_set_digital_master_volume:

; [esp + 2*4] = int32_t master_volume
; [esp +   4] = void *dig
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_AIL_set_digital_master_volume,-1

; end procedure SR_AIL_set_digital_master_volume

align 16
SR_AIL_set_sample_address:

; [esp + 3*4] = uint32_t len
; [esp + 2*4] = void *start
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack3 Game_AIL_set_sample_address,-1

; end procedure SR_AIL_set_sample_address

align 16
SR_AIL_set_sample_loop_count:

; [esp + 2*4] = int32_t loop_count
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_AIL_set_sample_loop_count,-1

; end procedure SR_AIL_set_sample_loop_count

align 16
SR_AIL_set_sample_pan:

; [esp + 2*4] = int32_t pan
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_AIL_set_sample_pan,-1

; end procedure SR_AIL_set_sample_pan

align 16
SR_AIL_set_sample_playback_rate:

; [esp + 2*4] = int32_t playback_rate
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_AIL_set_sample_playback_rate,-1

; end procedure SR_AIL_set_sample_playback_rate

align 16
SR_AIL_set_sample_type:

; [esp + 3*4] = uint32_t flags
; [esp + 2*4] = int32_t format
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack3 Game_AIL_set_sample_type,-1

; end procedure SR_AIL_set_sample_type

align 16
SR_AIL_set_sample_volume:

; [esp + 2*4] = int32_t volume
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_AIL_set_sample_volume,-1

; end procedure SR_AIL_set_sample_volume

align 16
SR_AIL_start_sample:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_start_sample,-1

; end procedure SR_AIL_start_sample


align 16
SR_AIL_allocate_sequence_handle:

; [esp +   4] = void *mdi
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_allocate_sequence_handle,-1

; end procedure SR_AIL_allocate_sequence_handle

align 16
SR_AIL_end_sequence:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_end_sequence,-1

; end procedure SR_AIL_end_sequence

align 16
SR_AIL_init_sequence:

; [esp + 3*4] = int32_t sequence_num
; [esp + 2*4] = void *start
; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack3 Game_AIL_init_sequence,-1

; end procedure SR_AIL_init_sequence

align 16
SR_AIL_release_sequence_handle:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_release_sequence_handle,-1

; end procedure SR_AIL_release_sequence_handle

align 16
SR_AIL_resume_sequence:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_resume_sequence,-1

; end procedure SR_AIL_resume_sequence

align 16
SR_AIL_sequence_status:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_sequence_status,-1

; end procedure SR_AIL_sequence_status

align 16
SR_AIL_set_sequence_loop_count:

; [esp + 2*4] = int32_t loop_count
; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack2 Game_AIL_set_sequence_loop_count,-1

; end procedure SR_AIL_set_sequence_loop_count

align 16
SR_AIL_set_sequence_volume:

; [esp + 3*4] = int32_t ms
; [esp + 2*4] = int32_t volume
; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack3 Game_AIL_set_sequence_volume,-1

; end procedure SR_AIL_set_sequence_volume

align 16
SR_AIL_start_sequence:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_start_sequence,-1

; end procedure SR_AIL_start_sequence

align 16
SR_AIL_stop_sequence:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack1 Game_AIL_stop_sequence,-1

; end procedure SR_AIL_stop_sequence


; 0 params
align 16
SR___clock:
SR_j___clock:

        Game_Call_Asm_Reg0 Game_clock,-1

; end procedure SR___clock


; 1 param
align 16
SR_cputs:

; eax = int handle

        Game_Call_Asm_Reg1 Game_cputs,'get_errno_val'

; end procedure SR_cputs

align 16
SR___delay:
SR_j___delay:

; eax = unsigned milliseconds

    Game_Call_Asm_Reg1 Game_delay,-1

; end procedure SR___delay

align 16
SR__dos_gettime:

; eax = struct dostime_t *time

    Game_Call_Asm_Reg1 Game_dos_gettime,-1

; end procedure SR__dos_gettime

align 16
SR_exit:

; eax = FILE *fp

        jmp Game_ExitMain_Asm

; end procedure SR_exit

align 16
SR_fclose:

; eax = FILE *fp

        Game_Call_Asm_Reg1 Game_fclose,'get_errno_val'

; end procedure SR_fclose

align 16
SR_fgetc:

; eax = FILE *fp

        Game_Call_Asm_Reg1 Game_fgetc,'get_errno_val'

; end procedure SR_fgetc

align 16
SR__nfree:

; eax = void __near *ptr

        Game_Call_Asm_Reg1 Game_free,-1

; end procedure SR__nfree

align 16
SR_ftell:

; eax = FILE *fp

        Game_Call_Asm_Reg1 Game_ftell,'get_errno_val'

; end procedure SR_ftell

align 16
SR_inp:

; eax = int port

        Game_Call_Asm_Reg1 Game_inp,-1

; end procedure SR_inp

align 16
SR__nmalloc:

; eax = size_t size

        Game_Call_Asm_Reg1 Game_malloc,-1

; end procedure SR__nmalloc

align 16
SR_puts:

; eax = const char *buf

        Game_Call_Asm_Reg1 puts,'get_errno_val'

; end procedure SR_puts

align 16
SR_strlen:

; eax = const char *s

        Game_Call_Asm_Reg1 strlen,-1

; end procedure SR_strlen

align 16
SR_time:

; eax = time_t *tloc

        Game_Call_Asm_Reg1 Game_time,-1

; end procedure SR_time

align 16
SR_unlink:
SR_j_unlink:

; eax = const char *path

; errno_val is set inside Game_unlink

        Game_Call_Asm_Reg1 Game_unlink,-1

; end procedure SR_unlink


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

    Game_Call_Asm_Reg2 Game_fputc,'get_errno_val'

; end procedure SR_fputc

align 16
SR_fputs:

; eax = const char *buf
; edx = FILE *fp

    Game_Call_Asm_Reg2 Game_fputs,'get_errno_val'

; end procedure SR_fputs

align 16
SR_outp:

; eax = int port
; edx = int value

        Game_Call_Asm_Reg2 Game_outp,-1

; end procedure SR_outp

align 16
SR_signal:

; eax = int sig
; edx = void (*func)(int))

        Game_Call_Asm_Reg2 Game_signal,-1

; end procedure SR_signal

align 16
SR_strcat:

; eax = char *dst
; edx = const char *src

        Game_Call_Asm_Reg2 strcat,-1

; end procedure SR_strcat

align 16
SR_strcmp:

; eax = const char *s1
; edx = const char *s2

        Game_Call_Asm_Reg2 strcmp,-1

; end procedure SR_strcmp

align 16
SR_strcpy:

; eax = char *dst
; edx = const char *src

        Game_Call_Asm_Reg2 strcpy,-1

; end procedure SR_strcpy

align 16
SR_strrchr:

; eax = const char *s
; edx = int c

        Game_Call_Asm_Reg2 strrchr,-1

; end procedure SR_strrchr

align 16
SR_vprintf:

; eax = const char *format
; edx = va_list arg

        mov edx, [edx]
        Game_Call_Asm_Reg2 CLIB_vprintf,-1

; end procedure SR_vprintf

; 3 params
align 16
SR_fgets:

; eax = char *buf
; edx = int n
; ebx = FILE *fp

        Game_Call_Asm_Reg3 Game_fgets,'get_errno_val'

; end procedure SR_fgets

align 16
SR_fseek:

; eax = FILE *fp
; edx = long int offset
; ebx = int where

        Game_Call_Asm_Reg3 Game_fseek,'get_errno_val'

; end procedure SR_fseek

align 16
SR_memcmp:

; eax = const void *s1
; edx = const void *s2
; ebx = size_t length

        Game_Call_Asm_Reg3 memcmp,-1

; end procedure SR_memcmp

align 16
SR_memcpy:

; eax = void *dst
; edx = const void *src
; ebx = size_t length

        Game_Call_Asm_Reg3 memcpy,-1

; end procedure SR_memcpy

align 16
SR_memmove:

; eax = void *dst
; edx = const void *src
; ebx = size_t length

        Game_Call_Asm_Reg3 memmove,-1

; end procedure SR_memmove

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

align 16
SR_strnicmp:

; eax = const char *s1
; edx = const char *s2
; ebx = size_t len

        Game_Call_Asm_Reg3 strncasecmp,-1

; end procedure SR_strnicmp

align 16
SR_strtoul:

; eax = const char *ptr
; edx = char **endptr
; ebx = int base

        Game_Call_Asm_Reg3 strtoul,'get_errno_val'

; end procedure SR_strtoul


; 4 params
align 16
SR__fmemset:

; eax = void *dst
;  dx = segment of dst
; ebx = int c
; ecx = size_t length

        ;Game_Call_Asm_Reg4 Game_fmemset,-1

    ; save original ecx value on stack
        push ecx
    ; remember original esp value
        lea ecx, [esp + 4]
    ; reserve 16 bytes on stack
        sub esp, byte 16
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; put function arguments to stack
        mov [esp], eax
        mov [esp + 1*4], edx
        mov [esp + 2*4], ebx

    ; read original ecx value from stack
        mov eax, [ecx - 4]

    ; put function argument to stack
        mov [esp + 3*4], eax

    ; save original esp value on stack
        mov [esp + 4*4], ecx

    ; stack is aligned to 16 bytes

        call Game_fmemset

    ; restore original edx value from stack
        mov edx, [esp + 1*4]

    ; restore original esp value from stack
        mov esp, [esp + 4*4]

        retn

; end procedure SR__fmemset

align 16
SR_fread:

; eax = void *buf
; edx = size_t elsize
; ebx = size_t nelem
; ecx = FILE *fp

        Game_Call_Asm_Reg4 Game_fread,'get_errno_val'

; end procedure SR_fread

align 16
SR_fwrite:

; eax = const void *buf
; edx = size_t elsize
; ebx = size_t nelem
; ecx = FILE *fp

        Game_Call_Asm_Reg4 Game_fwrite,'get_errno_val'

; end procedure SR_fwrite

align 16
SR_int386x:

; eax = int inter_no
; edx = const union REGS *in_regs
; ebx = union REGS *out_regs
; ecx = struct SREGS *seg regs

        Game_Call_Asm_Reg4 Game_int386x,-1

; end procedure SR_int386x

align 16
SR_setvbuf:

; eax = FILE *fp
; edx = char *buf
; ebx = int mode
; ecx = size_t size

        Game_Call_Asm_Reg4 Game_setvbuf,'get_errno_val'

; end procedure SR_setvbuf

; 5 params
align 16
SR__fmemcmp:

; eax = const void *s1
;  dx = segment of s1
; ebx = const void *s2
;  cx = segment of s2
; [esp + 4] = size_t length
;
; [esp    ] = return address

        Game_Call_Asm_Reg5 Game_fmemcmp,-1

; end procedure SR__fmemcmp

align 16
SR__fmemcpy:

; eax = void *dst
;  dx = segment of dst
; ebx = const void *src
;  cx = segment of src
; [esp + 4] = size_t length
;
; [esp    ] = return address

        ;Game_Call_Asm_Reg5 Game_fmemcpy,-1

    ; save original ecx value on stack
        push ecx
    ; remember original esp value
        lea ecx, [esp + 4]
    ; reserve 20 bytes on stack
        sub esp, byte 20
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h

    ; put function arguments to stack
        mov [esp], eax
        mov [esp + 1*4], edx
        mov [esp + 2*4], ebx

    ; read original ecx value from stack
        mov eax, [ecx - 4]

    ; read original function argument from stack
        mov edx, [ecx + 4]

    ; put function arguments to stack
        mov [esp + 3*4], eax
        mov [esp + 4*4], edx

    ; save original esp value on stack
        mov [esp + 5*4], ecx

    ; stack is aligned to 16 bytes

        call Game_fmemcpy

    ; restore original edx value from stack
        mov edx, [esp + 1*4]

    ; restore original esp value from stack
        mov esp, [esp + 5*4]

        retn 4

; end procedure SR__fmemcpy

