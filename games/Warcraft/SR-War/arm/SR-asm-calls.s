@@
@@  Copyright (C) 2016-2019 Roman Pauer
@@
@@  Permission is hereby granted, free of charge, to any person obtaining a copy of
@@  this software and associated documentation files (the "Software"), to deal in
@@  the Software without restriction, including without limitation the rights to
@@  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
@@  of the Software, and to permit persons to whom the Software is furnished to do
@@  so, subject to the following conditions:
@@
@@  The above copyright notice and this permission notice shall be included in all
@@  copies or substantial portions of the Software.
@@
@@  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
@@  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
@@  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
@@  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
@@  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
@@  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
@@  SOFTWARE.
@@

.include "arm.inc"

.extern Game_errno
.extern Game_Registers


@ stack params
.extern Game_fprintf
.extern Game_printf
.extern Game_sprintf
.extern Game_open
.extern Game_openFlags

.extern Game_AIL_active_sample_count
.extern Game_AIL_allocate_sample_handle
.extern Game_AIL_end_sample
.extern Game_AIL_init_sample
.extern Game_AIL_register_EOS_callback
.extern Game_AIL_release_sample_handle
.extern Game_AIL_sample_status
.extern Game_AIL_set_digital_master_volume
.extern Game_AIL_set_sample_address
.extern Game_AIL_set_sample_loop_count
.extern Game_AIL_set_sample_pan
.extern Game_AIL_set_sample_playback_rate
.extern Game_AIL_set_sample_type
.extern Game_AIL_set_sample_volume
.extern Game_AIL_start_sample

.extern Game_AIL_allocate_sequence_handle
.extern Game_AIL_end_sequence
.extern Game_AIL_init_sequence
.extern Game_AIL_release_sequence_handle
.extern Game_AIL_resume_sequence
.extern Game_AIL_sequence_status
.extern Game_AIL_set_sequence_loop_count
.extern Game_AIL_set_sequence_volume
.extern Game_AIL_start_sequence
.extern Game_AIL_stop_sequence
@ 0 params
.extern Game_clock
@ 1 param
.extern close
.extern Game_cputs
.extern Game_delay
.extern Game_dos_gettime
.extern Game_ExitMain_Asm
.extern fclose
.extern fgetc
.extern free
.extern ftell
.extern getenv
.extern Game_inp
.extern malloc
.extern puts
.extern Game_raise
.extern strlen
.extern time
.extern Game_unlink
@ 2 params
.extern Game_fopen
.extern fputc
.extern fputs
.extern Game_outp
.extern Game_signal
.extern strcat
.extern strcmp
.extern strcpy
.extern strrchr
.extern vprintf
@ 3 params
.extern fgets
.extern fseek
.extern lseek
.extern memcmp
.extern memcpy
.extern memmove
.extern memset
.extern read
.extern strncmp
.extern strncpy
.extern strncasecmp
.extern strtoul
@ 4 params
.extern Game_fmemset
.extern fread
.extern fwrite
.extern Game_int386x
.extern Game_setvbuf
@ 5 params
.extern Game_fmemcmp
.extern Game_fmemcpy

.extern errno_val

@ null procedures
.global SR___CHK
.global SR___GETDS
.global SR__harderr
@ stack params
.global SR_fprintf
.global SR_printf
.global SR_sprintf
.global SR_open

.global SR_AIL_active_sample_count
.global SR_AIL_allocate_sample_handle
.global SR_AIL_end_sample
.global SR_AIL_init_sample
.global SR_AIL_register_EOS_callback
.global SR_AIL_release_sample_handle
.global SR_AIL_sample_status
.global SR_AIL_set_digital_master_volume
.global SR_AIL_set_sample_address
.global SR_AIL_set_sample_loop_count
.global SR_AIL_set_sample_pan
.global SR_AIL_set_sample_playback_rate
.global SR_AIL_set_sample_type
.global SR_AIL_set_sample_volume
.global SR_AIL_start_sample

.global SR_AIL_allocate_sequence_handle
.global SR_AIL_end_sequence
.global SR_AIL_init_sequence
.global SR_AIL_release_sequence_handle
.global SR_AIL_resume_sequence
.global SR_AIL_sequence_status
.global SR_AIL_set_sequence_loop_count
.global SR_AIL_set_sequence_volume
.global SR_AIL_start_sequence
.global SR_AIL_stop_sequence
@ 0 params
.global SR___clock
.global SR_j___clock
@ 1 param
.global SR___close
.global SR_j___close
.global SR_cputs
.global SR___delay
.global SR_j___delay
.global SR__dos_gettime
.global SR_exit
.global SR_fclose
.global SR_fgetc
.global SR__nfree
.global SR_ftell
.global SR_getenv
.global SR_inp
.global SR__nmalloc
.global SR_puts
.global SR_raise
.global SR_strlen
.global SR_time
.global SR_unlink
.global SR_j_unlink
@ 2 params
.global SR_fopen
.global SR_fputc
.global SR_fputs
.global SR_outp
.global SR_signal
.global SR_strcat
.global SR_strcmp
.global SR_strcpy
.global SR_strrchr
.global SR_vprintf
@ 3 params
.global SR_fgets
.global SR_fseek
.global SR_lseek
.global SR_memcmp
.global SR_memcpy
.global SR_memmove
.global SR_memset
.global SR_read
.global SR_strncmp
.global SR_strncpy
.global SR_strnicmp
.global SR_strtoul
@ 4 params
.global SR__fmemset
.global SR_fread
.global SR_fwrite
.global SR_int386x
.global SR_setvbuf
@ 5 params
.global SR__fmemcmp
.global SR__fmemcpy

.include "SR-asm-calls.inc"

.section .note.GNU-stack,"",%progbits
.section .text

@ null procedures
SR___CHK:

@ [esp] = return address

        @retn 4
        ldmfd esp!, {tmp1}
        add esp, esp, #4
        bx tmp1

@ end procedure SR___CHK

SR___GETDS:
SR__harderr:
null_procedure:

        @retn
        ldmfd esp!, {eip}

@ end procedure null_procedure

@ stack params
SR_fprintf:

@ [esp + 3*4] = ...
@ [esp + 2*4] = const char *format
@ [esp +   4] = FILE *fp
@ [esp      ] = return address

        @Game_Call_Asm_Stack fprintf,-1000
        Game_Call_Asm_Stack0 Game_fprintf,-1000

@ end procedure SR_fprintf

SR_printf:

@ [esp + 2*4] = ...
@ [esp +   4] = const char *format
@ [esp      ] = return address

        @Game_Call_Asm_Stack printf,-1000
        Game_Call_Asm_Stack0 Game_printf,-1000

@ end procedure SR_printf

SR_sprintf:

@ [esp + 3*4] = ...
@ [esp + 2*4] = const char *format
@ [esp +   4] = char *buf
@ [esp      ] = return address

        @Game_Call_Asm_Stack sprintf,-1000
        Game_Call_Asm_Stack0 Game_sprintf,-1000

@ end procedure SR_sprintf

SR_open:

@ [esp + 3*4] = ...
@ [esp + 2*4] = int access
@ [esp +   4] = const char *path
@ [esp      ] = return address


@ errno_val is set inside Game_open

        stmfd esp!, {eflags}

        @push dword [esp + 2*4]				; access (flags)
        ldr tmp1, [esp, #(3*4)]

        @call Game_openFlags
        bl Game_openFlags

        @mov [esp + 3*4], eax				; access (flags)
        mov tmp2, tmp1

        @add esp, byte 4

        ldr tmp1, [esp, #(2*4)]

        @call Game_open
        ldr tmp3, [esp, #(4*4)]
        bl Game_open
        mov eax, tmp1

        @jmp [Game_ReturnAddress]
        ldmfd esp!, {eflags, eip}

@ end procedure SR_open

SR_AIL_active_sample_count:

@ [esp +   4] = void *dig
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_active_sample_count,-1

@ end procedure SR_AIL_active_sample_count

SR_AIL_allocate_sample_handle:

@ [esp +   4] = void *dig
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_allocate_sample_handle,-1

@ end procedure SR_AIL_allocate_sample_handle

SR_AIL_end_sample:

@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_end_sample,-1

@ end procedure SR_AIL_end_sample

SR_AIL_init_sample:

@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_init_sample,-1

@ end procedure SR_AIL_init_sample

SR_AIL_register_EOS_callback:

@ [esp + 2*4] = AIL_sample_CB EOS
@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_register_EOS_callback,-1

@ end procedure SR_AIL_register_EOS_callback

SR_AIL_release_sample_handle:

@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_release_sample_handle,-1

@ end procedure SR_AIL_release_sample_handle

SR_AIL_sample_status:

@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_sample_status,-1

@ end procedure SR_AIL_sample_status

SR_AIL_set_digital_master_volume:

@ [esp + 2*4] = int32_t master_volume
@ [esp +   4] = void *dig
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_digital_master_volume,-1

@ end procedure SR_AIL_set_digital_master_volume

SR_AIL_set_sample_address:

@ [esp + 3*4] = uint32_t len
@ [esp + 2*4] = void *start
@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_address,-1

@ end procedure SR_AIL_set_sample_address

SR_AIL_set_sample_loop_count:

@ [esp + 2*4] = int32_t loop_count
@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_loop_count,-1

@ end procedure SR_AIL_set_sample_loop_count

SR_AIL_set_sample_pan:

@ [esp + 2*4] = int32_t pan
@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_pan,-1

@ end procedure SR_AIL_set_sample_pan

SR_AIL_set_sample_playback_rate:

@ [esp + 2*4] = int32_t playback_rate
@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_playback_rate,-1

@ end procedure SR_AIL_set_sample_playback_rate

SR_AIL_set_sample_type:

@ [esp + 3*4] = uint32_t flags
@ [esp + 2*4] = int32_t format
@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_type,-1

@ end procedure SR_AIL_set_sample_type

SR_AIL_set_sample_volume:

@ [esp + 2*4] = int32_t volume
@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_volume,-1

@ end procedure SR_AIL_set_sample_volume

SR_AIL_start_sample:

@ [esp +   4] = AIL_sample *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_start_sample,-1

@ end procedure SR_AIL_start_sample


SR_AIL_allocate_sequence_handle:

@ [esp +   4] = void *mdi
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_allocate_sequence_handle,-1

@ end procedure SR_AIL_allocate_sequence_handle

SR_AIL_end_sequence:

@ [esp +   4] = AIL_sequence *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_end_sequence,-1

@ end procedure SR_AIL_end_sequence

SR_AIL_init_sequence:

@ [esp + 3*4] = int32_t sequence_num
@ [esp + 2*4] = void *start
@ [esp +   4] = AIL_sequence *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_init_sequence,-1

@ end procedure SR_AIL_init_sequence

SR_AIL_release_sequence_handle:

@ [esp +   4] = AIL_sequence *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_release_sequence_handle,-1

@ end procedure SR_AIL_release_sequence_handle

SR_AIL_resume_sequence:

@ [esp +   4] = AIL_sequence *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_resume_sequence,-1

@ end procedure SR_AIL_resume_sequence

SR_AIL_sequence_status:

@ [esp +   4] = AIL_sequence *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_sequence_status,-1

@ end procedure SR_AIL_sequence_status

SR_AIL_set_sequence_loop_count:

@ [esp + 2*4] = int32_t loop_count
@ [esp +   4] = AIL_sequence *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sequence_loop_count,-1

@ end procedure SR_AIL_set_sequence_loop_count

SR_AIL_set_sequence_volume:

@ [esp + 3*4] = int32_t ms
@ [esp + 2*4] = int32_t volume
@ [esp +   4] = AIL_sequence *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sequence_volume,-1

@ end procedure SR_AIL_set_sequence_volume

SR_AIL_start_sequence:

@ [esp +   4] = AIL_sequence *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_start_sequence,-1

@ end procedure SR_AIL_start_sequence

SR_AIL_stop_sequence:

@ [esp +   4] = AIL_sequence *S
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_stop_sequence,-1

@ end procedure SR_AIL_stop_sequence


@ 0 params
SR___clock:
SR_j___clock:

        Game_Call_Asm_Reg0 Game_clock,-1

@ end procedure SR_clock


@ 1 param
SR___close:
SR_j___close:

@ eax = int handle

        Game_Call_Asm_Reg1 close,-1000

@ end procedure SR___close

SR_cputs:

@ eax = int handle

        Game_Call_Asm_Reg1 Game_cputs,-1000

@ end procedure SR_cputs

SR___delay:
SR_j___delay:

@ eax = unsigned milliseconds

        Game_Call_Asm_Reg1 Game_delay,-1

@ end procedure SR___delay

SR__dos_gettime:

@ eax = struct dostime_t *time

        Game_Call_Asm_Reg1 Game_dos_gettime,-1

@ end procedure SR__dos_gettime

SR_exit:

@ eax = FILE *fp

        b Game_ExitMain_Asm

@ end procedure SR_exit

SR_fclose:

@ eax = FILE *fp

        Game_Call_Asm_Reg1 fclose,-1000

@ end procedure SR_fclose

SR_fgetc:

@ eax = FILE *fp

        Game_Call_Asm_Reg1 fgetc,-1000

@ end procedure SR_fgetc

SR__nfree:

@ eax = void __near *ptr

        Game_Call_Asm_Reg1 free,-1

@ end procedure SR__nfree

SR_ftell:

@ eax = FILE *fp

        Game_Call_Asm_Reg1 ftell,-1000

@ end procedure SR_ftell

SR_getenv:

@ eax = const char *name

        Game_Call_Asm_Reg1 getenv,-1

@ end procedure SR_getenv

SR_inp:

@ eax = int port

        Game_Call_Asm_Reg1 Game_inp,-1

@ end procedure SR_inp

SR__nmalloc:

@ eax = size_t size

        Game_Call_Asm_Reg1 malloc,-1

@ end procedure SR__nmalloc

SR_puts:

@ eax = const char *buf

        Game_Call_Asm_Reg1 puts,-1000

@ end procedure SR_puts

SR_raise:

@ eax = int condition

        Game_Call_Asm_Reg1 Game_raise,-1

@ end procedure SR_raise

SR_strlen:

@ eax = const char *s

        Game_Call_Asm_Reg1 strlen,-1

@ end procedure SR_strlen

SR_time:

@ eax = time_t *tloc

        Game_Call_Asm_Reg1 time,-1

@ end procedure SR_time

SR_unlink:
SR_j_unlink:

@ eax = const char *path

@ errno_val is set inside Game_unlink

        Game_Call_Asm_Reg1 Game_unlink,-1

@ end procedure SR_unlink


@ 2 params
SR_fopen:

@ eax = const char *filename
@ edx = const char *mode

@ errno_val is set inside Game_fopen

        Game_Call_Asm_Reg2 Game_fopen,-1

@ end procedure SR_fopen

SR_fputc:

@ eax = int c
@ edx = FILE *fp

    Game_Call_Asm_Reg2 fputc,-1000

@ end procedure SR_fputc

SR_fputs:

@ eax = const char *buf
@ edx = FILE *fp

    Game_Call_Asm_Reg2 fputs,-1000

@ end procedure SR_fputs

SR_outp:

@ eax = int port
@ edx = int value

        Game_Call_Asm_Reg2 Game_outp,-1

@ end procedure SR_outp

SR_signal:

@ eax = int sig
@ edx = void (*func)(int))

        Game_Call_Asm_Reg2 Game_signal,-1

@ end procedure SR_signal

SR_strcat:

@ eax = char *dst
@ edx = const char *src

        Game_Call_Asm_Reg2 strcat,-1

@ end procedure SR_strcat

SR_strcmp:

@ eax = const char *s1
@ edx = const char *s2

        Game_Call_Asm_Reg2 strcmp,-1

@ end procedure SR_strcmp

SR_strcpy:

@ eax = char *dst
@ edx = const char *src

        Game_Call_Asm_Reg2 strcpy,-1

@ end procedure SR_strcpy

SR_strrchr:

@ eax = const char *s
@ edx = int c

        Game_Call_Asm_Reg2 strrchr,-1

@ end procedure SR_strrchr

SR_vprintf:

@ eax = const char *format
@ edx = va_list arg

        ldr edx, [edx]
        Game_Call_Asm_Reg2 vprintf,-1

@ end procedure SR_vprintf

@ 3 params
SR_fgets:

@ eax = char *buf
@ edx = int n
@ ebx = FILE *fp

        Game_Call_Asm_Reg3 fgets,-1000

@ end procedure SR_fgets

SR_fseek:

@ eax = FILE *fp
@ edx = long int offset
@ ebx = int where

        Game_Call_Asm_Reg3 fseek,-1000

@ end procedure SR_fseek

SR_lseek:

@ eax = int handle
@ edx = long int offset
@ ebx = int origin

        Game_Call_Asm_Reg3 lseek,-1000

@ end procedure SR_lseek

SR_memcmp:

@ eax = const void *s1
@ edx = const void *s2
@ ebx = size_t length

        Game_Call_Asm_Reg3 memcmp,-1

@ end procedure SR_memcmp

SR_memcpy:

@ eax = void *dst
@ edx = const void *src
@ ebx = size_t length

        Game_Call_Asm_Reg3 memcpy,-1

@ end procedure SR_memcpy

SR_memmove:

@ eax = void *dst
@ edx = const void *src
@ ebx = size_t length

        Game_Call_Asm_Reg3 memmove,-1

@ end procedure SR_memmove

SR_memset:

@ eax = void *dst
@ edx = int c
@ ebx = size_t length

        Game_Call_Asm_Reg3 memset,-1

@ end procedure SR_memset

SR_read:

@ eax = int handle
@ edx = void *buffer
@ ebx = unsigned len

        Game_Call_Asm_Reg3 read,-1000

@ end procedure SR_read

SR_strncmp:

@ eax = const char *s1
@ edx = const char *s2
@ ebx = size_t n

        Game_Call_Asm_Reg3 strncmp,-1

@ end procedure SR_strncmp

SR_strncpy:

@ eax = char *dst
@ edx = const char *src
@ ebx = size_t n

        Game_Call_Asm_Reg3 strncpy,-1

@ end procedure SR_strncpy

SR_strnicmp:

@ eax = const char *s1
@ edx = const char *s2
@ ebx = size_t len

        Game_Call_Asm_Reg3 strncasecmp,-1

@ end procedure SR_strnicmp

SR_strtoul:

@ eax = const char *ptr
@ edx = char **endptr
@ ebx = int base

        Game_Call_Asm_Reg3 strtoul,-1000

@ end procedure SR_strtoul


@ 4 params
SR__fmemset:

@ eax = void *dst
@  dx = segment of dst
@ ebx = int c
@ ecx = size_t length

        Game_Call_Asm_Reg4 Game_fmemset,-1

@ end procedure SR__fmemset

SR_fread:

@ eax = void *buf
@ edx = size_t elsize
@ ebx = size_t nelem
@ ecx = FILE *fp

        Game_Call_Asm_Reg4 fread,-1000

@ end procedure SR_fread

SR_fwrite:

@ eax = const void *buf
@ edx = size_t elsize
@ ebx = size_t nelem
@ ecx = FILE *fp

        Game_Call_Asm_Reg4 fwrite,-1000

@ end procedure SR_fwrite

SR_int386x:

@ eax = int inter_no
@ edx = const union REGS *in_regs
@ ebx = union REGS *out_regs
@ ecx = struct SREGS *seg regs

        Game_Call_Asm_Reg4 Game_int386x,-1

@ end procedure SR_int386x

SR_setvbuf:

@ eax = FILE *fp
@ edx = char *buf
@ ebx = int mode
@ ecx = size_t size

        Game_Call_Asm_Reg4 Game_setvbuf,-1000

@ end procedure SR_setvbuf

@ 5 params
SR__fmemcmp:

@ eax = void *dst
@  dx = segment of dst
@ ebx = const void *src
@  cx = segment of src
@ [esp + 4] = size_t length
@
@ [esp    ] = return address

        Game_Call_Asm_Reg5 Game_fmemcmp,-1

@ end procedure SR__fmemcmp

SR__fmemcpy:

@ eax = void *dst
@  dx = segment of dst
@ ebx = const void *src
@  cx = segment of src
@ [esp + 4] = size_t length
@
@ [esp    ] = return address

        Game_Call_Asm_Reg5 Game_fmemcpy,-1

@ end procedure SR__fmemcpy
