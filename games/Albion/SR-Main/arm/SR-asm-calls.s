@@
@@  Copyright (C) 2016-2018 Roman Pauer
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
.extern Game_ReturnAddress
.extern Game_Registers

@ stack params
.extern Game_fprintf
.extern Game_printf
.extern Game_snprintf
.extern Game_sprintf
.extern Game_WaitVerticalRetraceTicks
.extern Game_open
.extern Game_openFlags

.extern Game_AIL_mem_use_malloc
.extern Game_AIL_mem_use_free
.extern Game_AIL_startup
.extern Game_AIL_register_timer
.extern Game_AIL_set_timer_frequency
.extern Game_AIL_start_timer
.extern Game_AIL_stop_timer
.extern Game_AIL_release_timer_handle
.extern Game_AIL_shutdown
.extern Game_AIL_set_GTL_filename_prefix
.extern Game_AIL_install_MDI_INI
.extern Game_AIL_set_preference
.extern Game_AIL_install_DIG_INI
.extern Game_AIL_uninstall_DIG_driver
.extern Game_AIL_uninstall_MDI_driver

.extern Game_AIL_allocate_sample_handle
.extern Game_AIL_end_sample
.extern Game_AIL_init_sample
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
.extern Game_AIL_create_wave_synthesizer
.extern Game_AIL_destroy_wave_synthesizer
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
.extern fcloseall
.extern Game_WaitAfterVerticalRetrace
.extern Game_WaitForVerticalRetrace
.extern Game_WaitAfter2ndVerticalRetrace
.extern Game_WaitFor2ndVerticalRetrace
@ 1 param
.extern asctime
.extern Game_chdir
.extern close
.extern Game_closedir
.extern ctime
.extern Game_dos_findclose
.extern Game_dos_findnext
.extern Game_dos_getvect
.extern Game_ExitMain_Asm
.extern fclose
.extern Game_filelength
.extern free
.extern ftime
.extern getenv
.extern isatty
.extern localtime
.extern malloc
.extern Game_opendir
.extern strlen
.extern Game_tell
.extern time
.extern Game_unlink
.extern Game_WaitTimerTicks
.extern Game_save_screenshot
@ 2 params
.extern Game_fopen
.extern fputs
.extern Game_getcwd
.extern Game_outp
.extern Game_rename
.extern Game_setbuf
.extern strchr
.extern strcmp
.extern strcpy
.extern strcspn
.extern strspn
@ 2/3 params
.extern Game_dos_setvect
@ 3 params
.extern Game_dos_findfirst
.extern fgets
.extern lseek
.extern memcpy
.extern memmove
.extern memset
.extern read
.extern strncmp
.extern strncpy
.extern strncasecmp
.extern write
@ 4 params
.extern vsnprintf
.extern Game_int386x
@ 5 params
.extern Game_splitpath

.extern errno_val

@ null procedures
.global SR___CHK
.global SR___GETDS
.global SR_nosound
.global SR_sound
.global SR__harderr
@ stack params
.global SR_fprintf
.global SR_printf
.global SR__bprintf
.global SR_sprintf
.global SR_WaitVerticalRetraceTicks
.global SR_open

.global SR_AIL_mem_use_malloc
.global SR_AIL_mem_use_free
.global SR_AIL_startup
.global SR_AIL_register_timer
.global SR_AIL_set_timer_frequency
.global SR_AIL_start_timer
.global SR_AIL_stop_timer
.global SR_AIL_release_timer_handle
.global SR_AIL_shutdown
.global SR_AIL_set_GTL_filename_prefix
.global SR_AIL_install_MDI_INI
.global SR_AIL_set_preference
.global SR_AIL_install_DIG_INI
.global SR_AIL_uninstall_DIG_driver
.global SR_AIL_uninstall_MDI_driver

.global SR_AIL_allocate_sample_handle
.global SR_AIL_end_sample
.global SR_AIL_init_sample
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
.global SR_AIL_create_wave_synthesizer
.global SR_AIL_destroy_wave_synthesizer
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
.global SR_fcloseall
.global SR_WaitAfterVerticalRetrace
.global SR_WaitForVerticalRetrace
.global SR_WaitAfter2ndVerticalRetrace
.global SR_WaitFor2ndVerticalRetrace
@ 1 param
.global SR_asctime
.global SR_chdir
.global SR___close
.global SR_j___close
.global SR_closedir
.global SR_ctime
.global SR__dos_findclose
.global SR__dos_findnext
.global SR__dos_getvect
.global SR_exit
.global SR_fclose
.global SR_filelength
.global SR__nfree
.global SR_ftime
.global SR_getenv
.global SR_isatty
.global SR_localtime
.global SR__nmalloc
.global SR_opendir
.global SR__setjmp
.global SR_strlen
.global SR_tell
.global SR_time
.global SR_unlink
.global SR_j_unlink
.global SR_WaitTimerTicks
.global SR_save_screenshot
@ 2 params
.global SR_fopen
.global SR_fputs
.global SR_getcwd
.global SR_longjmp
.global SR_outp
.global SR_rename
.global SR_setbuf
.global SR_strchr
.global SR_strcmp
.global SR_strcpy
.global SR_strcspn
.global SR_strspn
@ 2/3 params
.global SR__dos_setvect
@ 3 params
.global SR__dos_findfirst
.global SR_fgets
.global SR_lseek
.global SR_memcpy
.global SR_memmove
.global SR_memset
.global SR_read
.global SR_strncmp
.global SR_strncpy
.global SR_strnicmp
.global SR_write
@ 4 params
.global SR__vbprintf
.global SR_int386x
@ 5 params
.global SR__splitpath

.include "SR-asm-calls.inc"

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
SR_nosound:
SR_sound:
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

SR__bprintf:

@ [esp + 4*4] = ...
@ [esp + 3*4] = const char *format
@ [esp + 2*4] = size_t bufsize
@ [esp +   4] = char *buf
@ [esp      ] = return address

        @Game_Call_Asm_Stack snprintf,-1000
        Game_Call_Asm_Stack0 Game_snprintf,-1000

@ end procedure SR__bprintf

SR_sprintf:

@ [esp + 3*4] = ...
@ [esp + 2*4] = const char *format
@ [esp +   4] = char *buf
@ [esp      ] = return address

        @Game_Call_Asm_Stack sprintf,-1000
        Game_Call_Asm_Stack0 Game_sprintf,-1000

@ end procedure SR_sprintf

SR_WaitVerticalRetraceTicks:

@ [esp +   4] = int ticks
@ [esp      ] = return address

        @Game_Call_Asm_Stack Game_WaitVerticalRetraceTicks,-1
        ldr eax, [esp, #4]
        Game_Call_Asm_Reg1 Game_WaitVerticalRetraceTicks,-1

@ end procedure SR_WaitVerticalRetraceTicks

SR_open:

@ [esp + 3*4] = ...
@ [esp + 2*4] = int access
@ [esp +   4] = const char *path
@ [esp      ] = return address

@ errno_val is set inside Game_fopen

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

SR_AIL_mem_use_malloc:

@ [esp +   4] = void * (*fn)(uint32_t)
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_mem_use_malloc,-1

@ end procedure SR_AIL_mem_use_malloc

SR_AIL_mem_use_free:

@ [esp +   4] = void (*fn)(void *)
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_mem_use_free,-1

@ end procedure SR_AIL_mem_use_free

SR_AIL_startup:

@ [esp      ] = return address

        Game_Call_Asm_Stack0 Game_AIL_startup,-1

@ end procedure SR_AIL_startup

SR_AIL_register_timer:

@ [esp +   4] = void (*callback_fn)(uint32_t user)
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_register_timer,-1

@ end procedure SR_AIL_register_timer

SR_AIL_set_timer_frequency:

@ [esp + 2*4] = uint32_t hertz
@ [esp +   4] = int32_t timer
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_timer_frequency,-1

@ end procedure SR_AIL_set_timer_frequency

SR_AIL_start_timer:

@ [esp +   4] = int32_t timer
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_start_timer,-1

@ end procedure SR_AIL_start_timer

SR_AIL_stop_timer:

@ [esp +   4] = int32_t timer
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_stop_timer,-1

@ end procedure SR_AIL_stop_timer

SR_AIL_release_timer_handle:

@ [esp +   4] = int32_t timer
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_release_timer_handle,-1

@ end procedure SR_AIL_release_timer_handle

SR_AIL_shutdown:

@ [esp      ] = return address

        Game_Call_Asm_Stack0 Game_AIL_shutdown,-1

@ end procedure SR_AIL_shutdown

SR_AIL_set_GTL_filename_prefix:

@ [esp +   4] = char *prefix
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_GTL_filename_prefix,-1

@ end procedure SR_AIL_set_GTL_filename_prefix

SR_AIL_install_MDI_INI:

@ [esp +   4] = void *mdi
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_install_MDI_INI,-1

@ end procedure SR_AIL_install_MDI_INI

SR_AIL_set_preference:

@ [esp + 2*4] = int32_t value
@ [esp +   4] = uint32_t number
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_preference,-1

@ end procedure SR_AIL_set_preference

SR_AIL_install_DIG_INI:

@ [esp +   4] = void *dig
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_install_DIG_INI,-1

@ end procedure SR_AIL_install_DIG_INI

SR_AIL_uninstall_DIG_driver:

@ [esp +   4] = void *dig
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_uninstall_DIG_driver,-1

@ end procedure SR_AIL_uninstall_DIG_driver

SR_AIL_uninstall_MDI_driver:

@ [esp +   4] = void *mdi
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_uninstall_MDI_driver,-1

@ end procedure SR_AIL_uninstall_MDI_driver

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

SR_AIL_create_wave_synthesizer:


@ [esp + 4*4] = int32_t polyphony
@ [esp + 3*4] = void *wave_lib
@ [esp + 2*4] = void *mdi
@ [esp +   4] = void *dig
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_create_wave_synthesizer,-1

@ end procedure SR_AIL_create_wave_synthesizer

SR_AIL_destroy_wave_synthesizer:

@ [esp +   4] = void *W
@ [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_destroy_wave_synthesizer,-1

@ end procedure SR_AIL_destroy_wave_synthesizer

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
SR_fcloseall:
@ closes also stdin, stdout, stderr (but shouldn't)

        Game_Call_Asm_Reg0 fcloseall,-1

@ end procedure SR_fcloseall

SR_WaitAfterVerticalRetrace:

        Game_Call_Asm_Reg0 Game_WaitAfterVerticalRetrace,-1

@ end procedure SR_WaitAfterVerticalRetrace

SR_WaitForVerticalRetrace:

        Game_Call_Asm_Reg0 Game_WaitForVerticalRetrace,-1

@ end procedure SR_WaitForVerticalRetrace

SR_WaitAfter2ndVerticalRetrace:

        Game_Call_Asm_Reg0 Game_WaitAfter2ndVerticalRetrace,-1

@ end procedure SR_WaitAfter2ndVerticalRetrace

SR_WaitFor2ndVerticalRetrace:

        Game_Call_Asm_Reg0 Game_WaitFor2ndVerticalRetrace,-1

@ end procedure SR_WaitFor2ndVerticalRetrace

@ 1 param
SR_asctime:

@ eax = const struct tm *timeptr

        Game_Call_Asm_Reg1 asctime,-1

@ end procedure SR_asctime

SR_chdir:

@ eax = const char *path

@ errno_val is set inside Game_chdir

        Game_Call_Asm_Reg1 Game_chdir,-1

@ end procedure SR_chdir

SR___close:
SR_j___close:

@ eax = int handle

        Game_Call_Asm_Reg1 close,-1000

@ end procedure SR___close

SR_closedir:

@ eax = struct dirent *dirp

@ errno_val is set inside Game_closedir

        Game_Call_Asm_Reg1 Game_closedir,-1

@ end procedure SR_closedir

SR_ctime:

@ eax = const time_t *timer

        Game_Call_Asm_Reg1 ctime,-1

@ end procedure SR_ctime

SR__dos_findclose:

@ eax = struct find_t *buffer

        Game_Call_Asm_Reg1 Game_dos_findclose,0

@ end procedure SR__dos_findclose

SR__dos_findnext:

@ eax = struct find_t *buffer

@ errno_val is set inside Game_dos_findnext

        Game_Call_Asm_Reg1 Game_dos_findnext,-1

@ end procedure SR__dos_findnext

SR__dos_getvect:

@ eax = unsigned intnum
@ return: edx:eax = interrupt vector

@		Game_Call_Asm_Reg1 Game_dos_getvect,-1
        stmfd esp!, {eflags}

        mov tmp1, eax
        bl Game_dos_getvect
        mov eax, tmp1
        mov edx, tmp2

        ldmfd esp!, {eflags, eip}

@ end procedure SR__dos_getvect

SR_exit:

@ eax = int status

        b Game_ExitMain_Asm

@ end procedure SR_exit

SR_fclose:

@ eax = FILE *fp

        Game_Call_Asm_Reg1 fclose,-1000

@ end procedure SR_fclose

SR_filelength:

@ eax = int handle

        Game_Call_Asm_Reg1 Game_filelength,-1000

@ end procedure SR_filelength

SR__nfree:

@ eax = void __near *ptr

        Game_Call_Asm_Reg1 free,-1

@ end procedure SR__nfree

SR_ftime:

@ eax = struct timeb *timeptr

        Game_Call_Asm_Reg1 ftime,-1

@ end procedure SR_ftime

SR_getenv:

@ eax = const char *name

        Game_Call_Asm_Reg1 getenv,-1

@ end procedure SR_getenv

SR_isatty:

@ eax = int handle

        Game_Call_Asm_Reg1 isatty,-1000

@ end procedure SR_isatty

SR_localtime:

@ eax = const time_t *timer

        Game_Call_Asm_Reg1 localtime,-1

@ end procedure SR_localtime

SR__nmalloc:

@ eax = size_t size

        Game_Call_Asm_Reg1 malloc,-1

@ end procedure SR__nmalloc

SR_opendir:

@ eax = const char *dirname

@ errno_val is set inside Game_opendir

        Game_Call_Asm_Reg1 Game_opendir,-1

@ end procedure SR_opendir

SR__setjmp:

@ eax = jmp_buf env

        add eax, eax, #3
        bic eax, eax, #3

        @mov [eax], ebx
        @mov [eax + 1*4], ecx
        @mov [eax + 2*4], edx
        @mov [eax + 3*4], esi
        @mov [eax + 4*4], edi
        @mov [eax + 5*4], ebp
        str ebx, [eax]
        str ecx, [eax, #(1*4)]
        str edx, [eax, #(2*4)]
        str esi, [eax, #(3*4)]
        str edi, [eax, #(4*4)]
        str ebp, [eax, #(5*4)]

        @pop dword [eax + 6*4]
        ldmfd esp!, {tmp1}
        str tmp1, [eax, #(6*4)]

        @mov [eax + 7*4], esp
        str esp, [eax, #(7*4)]

        @push dword [eax + 6*4]
        stmfd esp!, {tmp1}

        @xor eax, eax
        @retn
        mov eax, #0
        ldmfd esp!, {eip}

@ end procedure SR__setjmp

SR_strlen:

@ eax = const char *s

        Game_Call_Asm_Reg1 strlen,-1

@ end procedure SR_strlen

SR_tell:

@ eax = int handle

        Game_Call_Asm_Reg1 Game_tell,-1000

@ end procedure SR_tell

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

SR_WaitTimerTicks:

@ eax = int ticks

        Game_Call_Asm_Reg1 Game_WaitTimerTicks,-1

@ end procedure SR_WaitTimerTicks

SR_save_screenshot:

@ eax = const char *filename

        Game_Call_Asm_Reg1 Game_save_screenshot,-1

@ end procedure SR_save_screenshot

@ 2 params
SR_fopen:

@ eax = const char *filename
@ edx = const char *mode

@ errno_val is set inside Game_fopen

        Game_Call_Asm_Reg2 Game_fopen,-1

@ end procedure SR_fopen

SR_fputs:

@	return value:
@		0 => 1
@		albion expects positive value (on success)
@		fputs returns non-negative value

@ eax = const char *buf
@ edx = FILE *fp

@	Game_Call_Asm_Reg2 fputs,-1000
        stmfd esp!, {eflags}

        mov tmp1, eax
        mov tmp2, edx
        bl fputs

        movS eax, tmp1
        movEQ eax, #1

        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]

        ldmfd esp!, {eflags, eip}

@ end procedure SR_fputs

SR_getcwd:

@ eax = char *buffer
@ edx = size_t size

@ errno_val is set inside Game_getcwd

        Game_Call_Asm_Reg2 Game_getcwd,-1

@ end procedure SR_getcwd

SR_longjmp:

@ eax = jmp_buf env
@ edx = int return_value

        add eax, eax, #3
        bic eax, eax, #3

        @mov esp, [eax + 7*4]
        ldr esp, [eax, #(7*4)]

        @push dword [eax + 6*4]
        ldr tmp1, [eax, #(6*4)]
        stmfd esp!, {tmp1}

        @or edx, edx
        @jnz SR_longjmp_1
        @inc edx
    @SR_longjmp_1:
        @push edx
        movS tmp2, edx
        movEQ tmp2, #1

        @mov ebx, [eax]
        @mov ecx, [eax + 1*4]
        @mov edx, [eax + 2*4]
        @mov esi, [eax + 3*4]
        @mov edi, [eax + 4*4]
        @mov ebp, [eax + 5*4]
        ldr ebx, [eax]
        ldr ecx, [eax, #(1*4)]
        ldr edx, [eax, #(2*4)]
        ldr esi, [eax, #(3*4)]
        ldr edi, [eax, #(4*4)]
        ldr ebp, [eax, #(5*4)]

        @pop eax
        @retn
        mov eax, tmp2
        ldmfd esp!, {eip}

@ end procedure SR_longjmp

SR_outp:

@ eax = int port
@ edx = int value

        Game_Call_Asm_Reg2 Game_outp,-1

@ end procedure SR_outp

SR_rename:

@ eax = const char *old
@ edx = const char *new

@ errno_val is set inside Game_rename

        Game_Call_Asm_Reg2 Game_rename,-1

@ end procedure SR_rename

SR_setbuf:

@ eax = FILE *fp
@ edx = char *buf

        Game_Call_Asm_Reg2 Game_setbuf,-1000

@ end procedure SR_setbuf

SR_strchr:

@ eax = const char *s
@ edx = int c

        Game_Call_Asm_Reg2 strchr,-1

@ end procedure SR_strchr

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

SR_strcspn:

@ eax = const char *str
@ edx = const char *charset

        Game_Call_Asm_Reg2 strcspn,-1

@ end procedure SR_strcspn

SR_strspn:

@ eax = const char *str
@ edx = const char *charset

        Game_Call_Asm_Reg2 strspn,-1

@ end procedure SR_strspn

@ 2/3 params
SR__dos_setvect:

@ eax = unsigned intnum
@ ecx:ebx = void (__interrupt __far *handler)()

        stmfd esp!, {eflags}

        mov tmp1, eax
        mov tmp2, ebx
        mov tmp3, ecx

        bl Game_dos_setvect

        ldmfd esp!, {eflags, eip}

@ end procedure SR__dos_setvect

@ 3 params
SR__dos_findfirst:

@ eax = const char *path
@ edx = unsigned attributes
@ ebx = struct find_t *buffer

@ errno_val is set inside Game_fopen

        Game_Call_Asm_Reg3 Game_dos_findfirst,-1

@ end procedure SR__dos_findfirst

SR_fgets:

@ eax = char *buf
@ edx = int n
@ ebx = FILE *fp

        Game_Call_Asm_Reg3 fgets,-1000

@ end procedure SR_fgets

SR_lseek:

@ eax = int handle
@ edx = long int offset
@ ebx = int origin

        Game_Call_Asm_Reg3 lseek,-1000

@ end procedure SR_lseek

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

SR_write:

@ eax = int handle
@ edx = void *buffer
@ ebx = unsigned len

        Game_Call_Asm_Reg3 write,-1

@ end procedure SR_write

@ 4 params
SR__vbprintf:

@ eax = char *buf
@ edx = size_t bufsize
@ ebx = const char *format
@ ecx = va_list arg

        @mov ecx, [ecx]
        ldrb tmp1, [ecx]
        ldrb tmp2, [ecx, #1]
        ldrb tmp3, [ecx, #2]
        ldrb tmpadr, [ecx, #3]
        orr tmp1, tmp1, tmp2, lsl #8
        orr tmp1, tmp1, tmp3, lsl #16
        orr ecx, tmp1, tmpadr, lsl #24
        Game_Call_Asm_Reg4 vsnprintf,-1

@ end procedure SR__vbprintf

SR_int386x:

@ eax = int inter_no
@ edx = const union REGS *in_regs
@ ebx = union REGS *out_regs
@ ecx = struct SREGS *seg regs

        Game_Call_Asm_Reg4 Game_int386x,-1

@ end procedure SR_int386x

@ 5 params
SR__splitpath:

@ eax = const char *path
@ edx = char *drive
@ ebx = char *dir
@ ecx = char *fname
@ [esp + 4] = char *ext
@
@ [esp    ] = return address

        Game_Call_Asm_Reg5 Game_splitpath,-1

@ end procedure SR__splitpath
