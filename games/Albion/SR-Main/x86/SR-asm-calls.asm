;;
;;  Copyright (C) 2016-2018 Roman Pauer
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

%ifndef ELF
    %define Game_errno _Game_errno
    %define Game_ReturnAddress _Game_ReturnAddress
    %define Game_Registers _Game_Registers

    %define Game_fprintf _Game_fprintf
    %define Game_printf _Game_printf
    %define Game_snprintf _Game_snprintf
    %define Game_sprintf _Game_sprintf
    %define Game_WaitVerticalRetraceTicks _Game_WaitVerticalRetraceTicks
    %define Game_open _Game_open
    %define Game_openFlags _Game_openFlags

    %define Game_AIL_mem_use_malloc _Game_AIL_mem_use_malloc
    %define Game_AIL_mem_use_free _Game_AIL_mem_use_free
    %define Game_AIL_startup _Game_AIL_startup
    %define Game_AIL_register_timer _Game_AIL_register_timer
    %define Game_AIL_set_timer_frequency _Game_AIL_set_timer_frequency
    %define Game_AIL_start_timer _Game_AIL_start_timer
    %define Game_AIL_stop_timer _Game_AIL_stop_timer
    %define Game_AIL_release_timer_handle _Game_AIL_release_timer_handle
    %define Game_AIL_shutdown _Game_AIL_shutdown
    %define Game_AIL_set_GTL_filename_prefix _Game_AIL_set_GTL_filename_prefix
    %define Game_AIL_install_MDI_INI _Game_AIL_install_MDI_INI
    %define Game_AIL_set_preference _Game_AIL_set_preference
    %define Game_AIL_install_DIG_INI _Game_AIL_install_DIG_INI
    %define Game_AIL_uninstall_DIG_driver _Game_AIL_uninstall_DIG_driver
    %define Game_AIL_uninstall_MDI_driver _Game_AIL_uninstall_MDI_driver

    %define Game_AIL_allocate_sample_handle _Game_AIL_allocate_sample_handle
    %define Game_AIL_end_sample _Game_AIL_end_sample
    %define Game_AIL_init_sample _Game_AIL_init_sample
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
    %define Game_AIL_create_wave_synthesizer _Game_AIL_create_wave_synthesizer
    %define Game_AIL_destroy_wave_synthesizer _Game_AIL_destroy_wave_synthesizer
    %define Game_AIL_end_sequence _Game_AIL_end_sequence
    %define Game_AIL_init_sequence _Game_AIL_init_sequence
    %define Game_AIL_release_sequence_handle _Game_AIL_release_sequence_handle
    %define Game_AIL_resume_sequence _Game_AIL_resume_sequence
    %define Game_AIL_sequence_status _Game_AIL_sequence_status
    %define Game_AIL_set_sequence_loop_count _Game_AIL_set_sequence_loop_count
    %define Game_AIL_set_sequence_volume _Game_AIL_set_sequence_volume
    %define Game_AIL_start_sequence _Game_AIL_start_sequence
    %define Game_AIL_stop_sequence _Game_AIL_stop_sequence

    %define ERROR_Init _ERROR_Init
    %define ERROR_ClearMessages _ERROR_ClearMessages
    %define ERROR_AddMessageDOS _ERROR_AddMessageDOS
    %define ERROR_RemoveMessage _ERROR_RemoveMessage
    %define ERROR_NumMessagesIsZero _ERROR_NumMessagesIsZero
    %define ERROR_PrintAndClearMessages _ERROR_PrintAndClearMessages

    %define BASEMEM_Init, _BASEMEM_Init
    %define BASEMEM_Deinit, _BASEMEM_Deinit
    %define BASEMEM_GetFreeMemSize, _BASEMEM_GetFreeMemSize
    %define BASEMEM_Alloc, _BASEMEM_Alloc
    %define BASEMEM_Free, _BASEMEM_Free
    %define BASEMEM_LockRegion, _BASEMEM_LockRegion
    %define BASEMEM_UnlockRegion, _BASEMEM_UnlockRegion
    %define BASEMEM_MemSetByte, _BASEMEM_MemSetByte
    %define BASEMEM_MemSetDword, _BASEMEM_MemSetDword
    %define BASEMEM_MemMove, _BASEMEM_MemMove
    %define BASEMEM_AlignMemptr, _BASEMEM_AlignMemptr
    %define BASEMEM_PrintReport, _BASEMEM_PrintReport,

    %define fcloseall __fcloseall
    %define Game_WaitAfterVerticalRetrace _Game_WaitAfterVerticalRetrace
    %define Game_WaitForVerticalRetrace _Game_WaitForVerticalRetrace
    %define Game_WaitAfter2ndVerticalRetrace _Game_WaitAfter2ndVerticalRetrace
    %define Game_WaitFor2ndVerticalRetrace _Game_WaitFor2ndVerticalRetrace

    %define asctime _asctime
    %define Game_chdir _Game_chdir
    %define close _close
    %define Game_closedir _Game_closedir
    %define ctime _ctime
    %define Game_dos_findclose _Game_dos_findclose
    %define Game_dos_findnext _Game_dos_findnext
    %define Game_dos_getvect _Game_dos_getvect
    %define Game_ExitMain_Asm _Game_ExitMain_Asm
    %define fclose _fclose
    %define Game_filelength _Game_filelength
    %define free _free
    %define ftime _ftime
    %define getenv _getenv
    %define isatty _isatty
    %define localtime _localtime
    %define malloc _malloc
    %define Game_opendir _Game_opendir
    %define strlen _strlen
    %define Game_tell _Game_tell
    %define time _time
    %define Game_unlink _Game_unlink
    %define Game_WaitTimerTicks _Game_WaitTimerTicks
    %define Game_save_screenshot _Game_save_screenshot

    %define Game_fopen _Game_fopen
    %define fputs _fputs
    %define Game_getcwd _Game_getcwd
    %define Game_outp _Game_outp
    %define Game_rename _Game_rename
    %define Game_setbuf _Game_setbuf
    %define strchr _strchr
    %define strcmp _strcmp
    %define strcpy _strcpy
    %define strcspn _strcspn
    %define strspn _strspn

    %define Game_dos_setvect _Game_dos_setvect

    %define Game_dos_findfirst _Game_dos_findfirst
    %define fgets _fgets
    %define lseek _lseek
    %define memcpy _memcpy
    %define memmove _memmove
    %define memset _memset
    %define read _read
    %define strncmp _strncmp
    %define strncpy _strncpy
    %define strncasecmp _strncasecmp
    %define write _write

    %define vsnprintf _vsnprintf
    %define Game_int386x _Game_int386x

    %define Game_splitpath _Game_splitpath
%endif

extern Game_errno
extern Game_ReturnAddress
extern Game_Registers

; stack params
extern Game_fprintf
extern Game_printf
extern Game_snprintf
extern Game_sprintf
extern Game_WaitVerticalRetraceTicks
extern Game_open
extern Game_openFlags

extern Game_AIL_mem_use_malloc
extern Game_AIL_mem_use_free
extern Game_AIL_startup
extern Game_AIL_register_timer
extern Game_AIL_set_timer_frequency
extern Game_AIL_start_timer
extern Game_AIL_stop_timer
extern Game_AIL_release_timer_handle
extern Game_AIL_shutdown
extern Game_AIL_set_GTL_filename_prefix
extern Game_AIL_install_MDI_INI
extern Game_AIL_set_preference
extern Game_AIL_install_DIG_INI
extern Game_AIL_uninstall_DIG_driver
extern Game_AIL_uninstall_MDI_driver

extern Game_AIL_allocate_sample_handle
extern Game_AIL_end_sample
extern Game_AIL_init_sample
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
extern Game_AIL_create_wave_synthesizer
extern Game_AIL_destroy_wave_synthesizer
extern Game_AIL_end_sequence
extern Game_AIL_init_sequence
extern Game_AIL_release_sequence_handle
extern Game_AIL_resume_sequence
extern Game_AIL_sequence_status
extern Game_AIL_set_sequence_loop_count
extern Game_AIL_set_sequence_volume
extern Game_AIL_start_sequence
extern Game_AIL_stop_sequence

extern ERROR_Init
extern ERROR_ClearMessages
extern ERROR_AddMessageDOS
extern ERROR_RemoveMessage
extern ERROR_NumMessagesIsZero
extern ERROR_PrintAndClearMessages

extern BASEMEM_Init
extern BASEMEM_Deinit
extern BASEMEM_GetFreeMemSize
extern BASEMEM_Alloc
extern BASEMEM_Free
extern BASEMEM_LockRegion
extern BASEMEM_UnlockRegion
extern BASEMEM_MemSetByte
extern BASEMEM_MemSetDword
extern BASEMEM_MemMove
extern BASEMEM_AlignMemptr
extern BASEMEM_PrintReport
; 0 params
extern fcloseall
extern Game_WaitAfterVerticalRetrace
extern Game_WaitForVerticalRetrace
extern Game_WaitAfter2ndVerticalRetrace
extern Game_WaitFor2ndVerticalRetrace
; 1 param
extern asctime
extern Game_chdir
extern close
extern Game_closedir
extern ctime
extern Game_dos_findclose
extern Game_dos_findnext
extern Game_dos_getvect
extern Game_ExitMain_Asm
extern fclose
extern Game_filelength
extern free
extern ftime
extern getenv
extern isatty
extern localtime
extern malloc
extern Game_opendir
extern strlen
extern Game_tell
extern time
extern Game_unlink
extern Game_WaitTimerTicks
extern Game_save_screenshot
; 2 params
extern Game_fopen
extern fputs
extern Game_getcwd
extern Game_outp
extern Game_rename
extern Game_setbuf
extern strchr
extern strcmp
extern strcpy
extern strcspn
extern strspn
; 2/3 params
extern Game_dos_setvect
; 3 params
extern Game_dos_findfirst
extern fgets
extern lseek
extern memcpy
extern memmove
extern memset
extern read
extern strncmp
extern strncpy
extern strncasecmp
extern write
; 4 params
extern vsnprintf
extern Game_int386x
; 5 params
extern Game_splitpath

extern errno_val

; null procedures
global SR___CHK
global SR___GETDS
global SR_nosound
global SR_sound
global SR__harderr
; stack params
global SR_fprintf
global SR_printf
global SR__bprintf
global SR_sprintf
global SR_WaitVerticalRetraceTicks
global SR_open

global SR_AIL_mem_use_malloc
global SR_AIL_mem_use_free
global SR_AIL_startup
global SR_AIL_register_timer
global SR_AIL_set_timer_frequency
global SR_AIL_start_timer
global SR_AIL_stop_timer
global SR_AIL_release_timer_handle
global SR_AIL_shutdown
global SR_AIL_set_GTL_filename_prefix
global SR_AIL_install_MDI_INI
global SR_AIL_set_preference
global SR_AIL_install_DIG_INI
global SR_AIL_uninstall_DIG_driver
global SR_AIL_uninstall_MDI_driver

global SR_AIL_allocate_sample_handle
global SR_AIL_end_sample
global SR_AIL_init_sample
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
global SR_AIL_create_wave_synthesizer
global SR_AIL_destroy_wave_synthesizer
global SR_AIL_end_sequence
global SR_AIL_init_sequence
global SR_AIL_release_sequence_handle
global SR_AIL_resume_sequence
global SR_AIL_sequence_status
global SR_AIL_set_sequence_loop_count
global SR_AIL_set_sequence_volume
global SR_AIL_start_sequence
global SR_AIL_stop_sequence

global SR_ERROR_Init
global SR_ERROR_ClearMessages
global SR_ERROR_AddMessage
global SR_ERROR_RemoveMessage
global SR_ERROR_NumMessagesIsZero
global SR_ERROR_PrintAndClearMessages

global SR_BASEMEM_Init
global SR_BASEMEM_Deinit
global SR_BASEMEM_GetFreeMemSize
global SR_BASEMEM_Alloc
global SR_BASEMEM_Free
global SR_BASEMEM_LockRegion
global SR_BASEMEM_UnlockRegion
global SR_BASEMEM_MemSetByte
global SR_BASEMEM_MemSetDword
global SR_BASEMEM_MemMove
global SR_BASEMEM_AlignMemptr
global SR_BASEMEM_PrintReport
; 0 params
global SR_fcloseall
global SR_WaitAfterVerticalRetrace
global SR_WaitForVerticalRetrace
global SR_WaitAfter2ndVerticalRetrace
global SR_WaitFor2ndVerticalRetrace
; 1 param
global SR_asctime
global SR_chdir
global SR___close
global SR_j___close
global SR_closedir
global SR_ctime
global SR__dos_findclose
global SR__dos_findnext
global SR__dos_getvect
global SR_exit
global SR_fclose
global SR_filelength
global SR__nfree
global SR_ftime
global SR_getenv
global SR_isatty
global SR_localtime
global SR__nmalloc
global SR_opendir
global SR__setjmp
global SR_strlen
global SR_tell
global SR_time
global SR_unlink
global SR_j_unlink
global SR_WaitTimerTicks
global SR_save_screenshot
; 2 params
global SR_fopen
global SR_fputs
global SR_getcwd
global SR_longjmp
global SR_outp
global SR_rename
global SR_setbuf
global SR_strchr
global SR_strcmp
global SR_strcpy
global SR_strcspn
global SR_strspn
; 2/3 params
global SR__dos_setvect
; 3 params
global SR__dos_findfirst
global SR_fgets
global SR_lseek
global SR_memcpy
global SR_memmove
global SR_memset
global SR_read
global SR_strncmp
global SR_strncpy
global SR_strnicmp
global SR_write
; 4 params
global SR__vbprintf
global SR_int386x
; 5 params
global SR__splitpath

%include "SR-asm-calls.inc"

%ifdef ELF
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
Game_Set_errno_Asm:

        push eax
        push ecx
        push edx

        call Game_errno

        mov [errno_val], eax

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

align 16
SR___GETDS:
SR_nosound:
SR_sound:
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

        ;Game_Call_Asm_Stack fprintf,'get_errno_val'
        Game_Call_Asm_Stack0 Game_fprintf,'get_errno_val'

; end procedure SR_fprintf

align 16
SR_printf:

; [esp + 2*4] = ...
; [esp +   4] = const char *format
; [esp      ] = return address

        ;Game_Call_Asm_Stack printf,'get_errno_val'
        Game_Call_Asm_Stack0 Game_printf,'get_errno_val'

; end procedure SR_printf

align 16
SR__bprintf:

; [esp + 4*4] = ...
; [esp + 3*4] = const char *format
; [esp + 2*4] = size_t bufsize
; [esp +   4] = char *buf
; [esp      ] = return address

        ;Game_Call_Asm_Stack snprintf,'get_errno_val'
        Game_Call_Asm_Stack0 Game_snprintf,'get_errno_val'

; end procedure SR__bprintf

align 16
SR_sprintf:

; [esp + 3*4] = ...
; [esp + 2*4] = const char *format
; [esp +   4] = char *buf
; [esp      ] = return address

        ;Game_Call_Asm_Stack sprintf,'get_errno_val'
        Game_Call_Asm_Stack0 Game_sprintf,'get_errno_val'

; end procedure SR_sprintf

align 16
SR_WaitVerticalRetraceTicks:

; [esp +   4] = int ticks
; [esp      ] = return address

        Game_Call_Asm_Stack Game_WaitVerticalRetraceTicks,-1

; end procedure SR_WaitVerticalRetraceTicks

align 16
SR_open:

; [esp + 3*4] = ...
; [esp + 2*4] = int access
; [esp +   4] = const char *path
; [esp      ] = return address

; errno_val is set inside Game_open

        push ecx
        push edx
        push dword [esp+5*4]
        push dword [esp+5*4]				; access (flags)
        call Game_openFlags
        mov [esp], eax						; access (flags)
        push dword [esp+5*4]				; path

        call Game_open

        add esp, byte 3*4

        pop edx
        pop ecx

        retn

; end procedure SR_open

align 16
SR_AIL_mem_use_malloc:

; [esp +   4] = void * (*fn)(uint32_t)
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_mem_use_malloc,-1

; end procedure SR_AIL_mem_use_malloc

align 16
SR_AIL_mem_use_free:

; [esp +   4] = void (*fn)(void *)
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_mem_use_free,-1

; end procedure SR_AIL_mem_use_free

align 16
SR_AIL_startup:

; [esp      ] = return address

        Game_Call_Asm_Stack0 Game_AIL_startup,-1

; end procedure SR_AIL_startup

align 16
SR_AIL_register_timer:

; [esp +   4] = void (*callback_fn)(uint32_t user)
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_register_timer,-1

; end procedure SR_AIL_register_timer

align 16
SR_AIL_set_timer_frequency:

; [esp + 2*4] = uint32_t hertz
; [esp +   4] = int32_t timer
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_timer_frequency,-1

; end procedure SR_AIL_set_timer_frequency

align 16
SR_AIL_start_timer:

; [esp +   4] = int32_t timer
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_start_timer,-1

; end procedure SR_AIL_start_timer

align 16
SR_AIL_stop_timer:

; [esp +   4] = int32_t timer
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_stop_timer,-1

; end procedure SR_AIL_stop_timer

align 16
SR_AIL_release_timer_handle:

; [esp +   4] = int32_t timer
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_release_timer_handle,-1

; end procedure SR_AIL_release_timer_handle

align 16
SR_AIL_shutdown:

; [esp      ] = return address

        Game_Call_Asm_Stack0 Game_AIL_shutdown,-1

; end procedure SR_AIL_shutdown

align 16
SR_AIL_set_GTL_filename_prefix:

; [esp +   4] = char *prefix
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_GTL_filename_prefix,-1

; end procedure SR_AIL_set_GTL_filename_prefix

align 16
SR_AIL_install_MDI_INI:

; [esp +   4] = void *mdi
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_install_MDI_INI,-1

; end procedure SR_AIL_install_MDI_INI

align 16
SR_AIL_set_preference:

; [esp + 2*4] = int32_t value
; [esp +   4] = uint32_t number
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_preference,-1

; end procedure SR_AIL_set_preference

align 16
SR_AIL_install_DIG_INI:

; [esp +   4] = void *dig
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_install_DIG_INI,-1

; end procedure SR_AIL_install_DIG_INI

align 16
SR_AIL_uninstall_DIG_driver:

; [esp +   4] = void *dig
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_uninstall_DIG_driver,-1

; end procedure SR_AIL_uninstall_DIG_driver

align 16
SR_AIL_uninstall_MDI_driver:

; [esp +   4] = void *mdi
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_uninstall_MDI_driver,-1

; end procedure SR_AIL_uninstall_MDI_driver

align 16
SR_AIL_allocate_sample_handle:

; [esp +   4] = void *dig
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_allocate_sample_handle,-1

; end procedure SR_AIL_allocate_sample_handle

align 16
SR_AIL_end_sample:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_end_sample,-1

; end procedure SR_AIL_end_sample

align 16
SR_AIL_init_sample:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_init_sample,-1

; end procedure SR_AIL_init_sample

align 16
SR_AIL_release_sample_handle:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_release_sample_handle,-1

; end procedure SR_AIL_release_sample_handle

align 16
SR_AIL_sample_status:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_sample_status,-1

; end procedure SR_AIL_sample_status

align 16
SR_AIL_set_digital_master_volume:

; [esp + 2*4] = int32_t master_volume
; [esp +   4] = void *dig
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_digital_master_volume,-1

; end procedure SR_AIL_set_digital_master_volume

align 16
SR_AIL_set_sample_address:

; [esp + 3*4] = uint32_t len
; [esp + 2*4] = void *start
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_address,-1

; end procedure SR_AIL_set_sample_address

align 16
SR_AIL_set_sample_loop_count:

; [esp + 2*4] = int32_t loop_count
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_loop_count,-1

; end procedure SR_AIL_set_sample_loop_count

align 16
SR_AIL_set_sample_pan:

; [esp + 2*4] = int32_t pan
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_pan,-1

; end procedure SR_AIL_set_sample_pan

align 16
SR_AIL_set_sample_playback_rate:

; [esp + 2*4] = int32_t playback_rate
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_playback_rate,-1

; end procedure SR_AIL_set_sample_playback_rate

align 16
SR_AIL_set_sample_type:

; [esp + 3*4] = uint32_t flags
; [esp + 2*4] = int32_t format
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_type,-1

; end procedure SR_AIL_set_sample_type

align 16
SR_AIL_set_sample_volume:

; [esp + 2*4] = int32_t volume
; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sample_volume,-1

; end procedure SR_AIL_set_sample_volume

align 16
SR_AIL_start_sample:

; [esp +   4] = AIL_sample *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_start_sample,-1

; end procedure SR_AIL_start_sample


align 16
SR_AIL_allocate_sequence_handle:

; [esp +   4] = void *mdi
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_allocate_sequence_handle,-1

; end procedure SR_AIL_allocate_sequence_handle

align 16
SR_AIL_create_wave_synthesizer:


; [esp + 4*4] = int32_t polyphony
; [esp + 3*4] = void *wave_lib
; [esp + 2*4] = void *mdi
; [esp +   4] = void *dig
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_create_wave_synthesizer,-1

; end procedure SR_AIL_create_wave_synthesizer

align 16
SR_AIL_destroy_wave_synthesizer:


; [esp +   4] = void *W
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_destroy_wave_synthesizer,-1

; end procedure SR_AIL_destroy_wave_synthesizer

align 16
SR_AIL_end_sequence:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_end_sequence,-1

; end procedure SR_AIL_end_sequence

align 16
SR_AIL_init_sequence:

; [esp + 3*4] = int32_t sequence_num
; [esp + 2*4] = void *start
; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_init_sequence,-1

; end procedure SR_AIL_init_sequence

align 16
SR_AIL_release_sequence_handle:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_release_sequence_handle,-1

; end procedure SR_AIL_release_sequence_handle

align 16
SR_AIL_resume_sequence:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_resume_sequence,-1

; end procedure SR_AIL_resume_sequence

align 16
SR_AIL_sequence_status:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_sequence_status,-1

; end procedure SR_AIL_sequence_status

align 16
SR_AIL_set_sequence_loop_count:

; [esp + 2*4] = int32_t loop_count
; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sequence_loop_count,-1

; end procedure SR_AIL_set_sequence_loop_count

align 16
SR_AIL_set_sequence_volume:

; [esp + 3*4] = int32_t ms
; [esp + 2*4] = int32_t volume
; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_set_sequence_volume,-1

; end procedure SR_AIL_set_sequence_volume

align 16
SR_AIL_start_sequence:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_start_sequence,-1

; end procedure SR_AIL_start_sequence

align 16
SR_AIL_stop_sequence:

; [esp +   4] = AIL_sequence *S
; [esp      ] = return address

        Game_Call_Asm_Stack Game_AIL_stop_sequence,-1

; end procedure SR_AIL_stop_sequence


align 16
SR_ERROR_Init:

; eax = void *logprint_proc

        Game_Call_Asm_Reg1 ERROR_Init,-1

; end procedure SR_ERROR_Init

align 16
SR_ERROR_ClearMessages:

        Game_Call_Asm_Reg0 ERROR_ClearMessages,-1

; end procedure SR_ERROR_ClearMessages

align 16
SR_ERROR_AddMessage:

; eax = void *msg_printdata_proc
; edx = char *msg_prefix
; ebx = int msg_data_len
; ecx = char *msg_data

        Game_Call_Asm_Reg4 ERROR_AddMessageDOS,-1

; end procedure SR_ERROR_AddMessage

align 16
SR_ERROR_RemoveMessage:

        Game_Call_Asm_Reg0 ERROR_RemoveMessage,-1

; end procedure SR_ERROR_RemoveMessage

align 16
SR_ERROR_NumMessagesIsZero:

        Game_Call_Asm_Reg0 ERROR_NumMessagesIsZero,-1

; end procedure SR_ERROR_NumMessagesIsZero

align 16
SR_ERROR_PrintAndClearMessages:

; eax = int flags

        Game_Call_Asm_Reg1 ERROR_PrintAndClearMessages,-1

; end procedure SR_ERROR_PrintAndClearMessages


align 16
SR_BASEMEM_Init:

        Game_Call_Asm_Reg0 BASEMEM_Init,-1

; end procedure SR_BASEMEM_Init

align 16
SR_BASEMEM_Deinit:

        Game_Call_Asm_Reg0 BASEMEM_Deinit,-1

; end procedure SR_BASEMEM_Deinit

align 16
SR_BASEMEM_GetFreeMemSize:

; eax = unsigned int memory_flags

        Game_Call_Asm_Reg1 BASEMEM_GetFreeMemSize,-1

; end procedure SR_BASEMEM_GetFreeMemSize

align 16
SR_BASEMEM_Alloc:

; eax = unsigned int size
; edx = unsigned int memory_flags

        Game_Call_Asm_Reg2 BASEMEM_Alloc,-1

; end procedure SR_BASEMEM_Alloc

align 16
SR_BASEMEM_Free:

; eax = void *mem_ptr

        Game_Call_Asm_Reg1 BASEMEM_Free,-1

; end procedure SR_BASEMEM_Free

align 16
SR_BASEMEM_LockRegion:

; eax = void *mem_ptr
; edx = unsigned int length

        Game_Call_Asm_Reg2 BASEMEM_LockRegion,-1

; end procedure SR_BASEMEM_LockRegion

align 16
SR_BASEMEM_UnlockRegion:

; eax = void *mem_ptr
; edx = unsigned int length

        Game_Call_Asm_Reg2 BASEMEM_UnlockRegion,-1

; end procedure SR_BASEMEM_UnlockRegion

align 16
SR_BASEMEM_MemSetByte:

; eax = void *dst
; edx = unsigned int length
; ebx = int c

        Game_Call_Asm_Reg3 BASEMEM_MemSetByte,-1

; end procedure SR_BASEMEM_MemSetByte

align 16
SR_BASEMEM_MemSetDword:

; eax = void *dst
; edx = unsigned int length
; ebx = unsigned int c

        Game_Call_Asm_Reg3 BASEMEM_MemSetDword,-1

; end procedure SR_BASEMEM_MemSetDword

align 16
SR_BASEMEM_MemMove:

; eax = const void *src
; edx = void *dst
; ebx = unsigned int length

        Game_Call_Asm_Reg3 BASEMEM_MemMove,-1

; end procedure SR_BASEMEM_MemMove

align 16
SR_BASEMEM_AlignMemptr:

; eax = void *mem_ptr

        Game_Call_Asm_Reg1 BASEMEM_AlignMemptr,-1

; end procedure SR_BASEMEM_AlignMemptr

align 16
SR_BASEMEM_PrintReport:

; eax = FILE *fp

        Game_Call_Asm_Reg1 BASEMEM_PrintReport,-1

; end procedure SR_BASEMEM_PrintReport


; 0 params
align 16
SR_fcloseall:
; closes also stdin, stdout, stderr (but shouldn't)

        Game_Call_Asm_Reg0 fcloseall,-1

; end procedure SR_fcloseall

align 16
SR_WaitAfterVerticalRetrace:

        Game_Call_Asm_Reg0 Game_WaitAfterVerticalRetrace,-1

; end procedure SR_WaitAfterVerticalRetrace

align 16
SR_WaitForVerticalRetrace:

        Game_Call_Asm_Reg0 Game_WaitForVerticalRetrace,-1

; end procedure SR_WaitForVerticalRetrace

align 16
SR_WaitAfter2ndVerticalRetrace:

        Game_Call_Asm_Reg0 Game_WaitAfter2ndVerticalRetrace,-1

; end procedure SR_WaitAfter2ndVerticalRetrace

align 16
SR_WaitFor2ndVerticalRetrace:

        Game_Call_Asm_Reg0 Game_WaitFor2ndVerticalRetrace,-1

; end procedure SR_WaitFor2ndVerticalRetrace

; 1 param
align 16
SR_asctime:

; eax = const struct tm *timeptr

        Game_Call_Asm_Reg1 asctime,-1

; end procedure SR_asctime

align 16
SR_chdir:

; eax = const char *path

; errno_val is set inside Game_chdir

        Game_Call_Asm_Reg1 Game_chdir,-1

; end procedure SR_chdir

align 16
SR___close:
SR_j___close:

; eax = int handle

        Game_Call_Asm_Reg1 close,'get_errno_val'

; end procedure SR___close

align 16
SR_closedir:

; eax = struct dirent *dirp

; errno_val is set inside Game_closedir

        Game_Call_Asm_Reg1 Game_closedir,-1

; end procedure SR_closedir

align 16
SR_ctime:

; eax = const time_t *timer

        Game_Call_Asm_Reg1 ctime,-1

; end procedure SR_ctime

align 16
SR__dos_findclose:

; eax = struct find_t *buffer

        Game_Call_Asm_Reg1 Game_dos_findclose,0

; end procedure SR__dos_findclose

align 16
SR__dos_findnext:

; eax = struct find_t *buffer

; errno_val is set inside Game_dos_findnext

        Game_Call_Asm_Reg1 Game_dos_findnext,-1

; end procedure SR__dos_findnext

align 16
SR__dos_getvect:

; eax = unsigned intnum
; return: edx:eax = interrupt vector

;		Game_Call_Asm_Reg1 Game_dos_getvect,-1
        push ecx
        push eax

        call Game_dos_getvect

        add esp, byte 4
        pop ecx

        retn

; end procedure SR__dos_getvect

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
SR_filelength:

; eax = int handle

        Game_Call_Asm_Reg1 Game_filelength,'get_errno_val'

; end procedure SR_filelength

align 16
SR__nfree:

; eax = void __near *ptr

        Game_Call_Asm_Reg1 free,-1

; end procedure SR__nfree

align 16
SR_ftime:

; eax = struct timeb *timeptr

        Game_Call_Asm_Reg1 ftime,-1

; end procedure SR_ftime

align 16
SR_getenv:

; eax = const char *name

        Game_Call_Asm_Reg1 getenv,-1

; end procedure SR_getenv

align 16
SR_isatty:

; eax = int handle

        Game_Call_Asm_Reg1 isatty,'get_errno_val'

; end procedure SR_isatty

align 16
SR_localtime:

; eax = const time_t *timer

        Game_Call_Asm_Reg1 localtime,-1

; end procedure SR_localtime

align 16
SR__nmalloc:

; eax = size_t size

        Game_Call_Asm_Reg1 malloc,-1

; end procedure SR__nmalloc

align 16
SR_opendir:

; eax = const char *dirname

; errno_val is set inside Game_opendir

        Game_Call_Asm_Reg1 Game_opendir,-1

; end procedure SR_opendir

align 16
SR__setjmp:

; eax = jmp_buf env

		mov [eax], ebx
		mov [eax + 1*4], ecx
		mov [eax + 2*4], edx
		mov [eax + 3*4], esi
		mov [eax + 4*4], edi
		mov [eax + 5*4], ebp
		pop dword [eax + 6*4]
		mov [eax + 7*4], esp
		push dword [eax + 6*4]

		xor eax, eax
		retn

; end procedure SR__setjmp

align 16
SR_strlen:

; eax = const char *s

        Game_Call_Asm_Reg1 strlen,-1

; end procedure SR_strlen

align 16
SR_tell:

; eax = int handle

        Game_Call_Asm_Reg1 Game_tell,'get_errno_val'

; end procedure SR_tell

align 16
SR_time:

; eax = time_t *tloc

        Game_Call_Asm_Reg1 time,-1

; end procedure SR_time

align 16
SR_unlink:
SR_j_unlink:

; eax = const char *path

; errno_val is set inside Game_unlink

        Game_Call_Asm_Reg1 Game_unlink,-1

; end procedure SR_unlink

align 16
SR_WaitTimerTicks:

; eax = int ticks

        Game_Call_Asm_Reg1 Game_WaitTimerTicks,-1

; end procedure SR_WaitTimerTicks

align 16
SR_save_screenshot:

; eax = const char *filename

        Game_Call_Asm_Reg1 Game_save_screenshot,-1

; end procedure SR_save_screenshot

; 2 params
align 16
SR_fopen:

; eax = const char *filename
; edx = const char *mode

; errno_val is set inside Game_fopen

        Game_Call_Asm_Reg2 Game_fopen,-1

; end procedure SR_fopen

align 16
SR_fputs:

;	return value:
;		0 => 1
;		albion expects positive value (on success)
;		fputs returns non-negative value

; eax = const char *buf
; edx = FILE *fp

;	Game_Call_Asm_Reg2 fputs,'get_errno_val'
        push ecx
        push edx
        push eax

        call fputs

        or eax, eax
        jnz SR_fputs_1
        inc eax
    SR_fputs_1:

        call Game_Set_errno_Asm

        add esp, 2*4
        pop ecx

        retn

; end procedure SR_fputs

align 16
SR_getcwd:

; eax = char *buffer
; edx = size_t size

; errno_val is set inside Game_getcwd

        Game_Call_Asm_Reg2 Game_getcwd,-1

; end procedure SR_getcwd

align 16
SR_longjmp:

; eax = jmp_buf env
; edx = int return_value

		mov esp, [eax + 7*4]
		push dword [eax + 6*4]
		or edx, edx
		jnz SR_longjmp_1
		inc edx
	SR_longjmp_1:
		push edx
		mov ebx, [eax]
		mov ecx, [eax + 1*4]
		mov edx, [eax + 2*4]
		mov esi, [eax + 3*4]
		mov edi, [eax + 4*4]
		mov ebp, [eax + 5*4]

		pop eax
		retn

; end procedure SR_longjmp

align 16
SR_outp:

; eax = int port
; edx = int value

        Game_Call_Asm_Reg2 Game_outp,-1

; end procedure SR_outp

align 16
SR_rename:

; eax = const char *old
; edx = const char *new

; errno_val is set inside Game_rename

        Game_Call_Asm_Reg2 Game_rename,-1

; end procedure SR_rename

align 16
SR_setbuf:

; eax = FILE *fp
; edx = char *buf

        Game_Call_Asm_Reg2 Game_setbuf,'get_errno_val'

; end procedure SR_setbuf

align 16
SR_strchr:

; eax = const char *s
; edx = int c

        Game_Call_Asm_Reg2 strchr,-1

; end procedure SR_strchr

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
SR_strcspn:

; eax = const char *str
; edx = const char *charset

        Game_Call_Asm_Reg2 strcspn,-1

; end procedure SR_strcspn

align 16
SR_strspn:

; eax = const char *str
; edx = const char *charset

        Game_Call_Asm_Reg2 strspn,-1

; end procedure SR_strspn

; 2/3 params
align 16
SR__dos_setvect:

; eax = unsigned intnum
; ecx:ebx = void (__interrupt __far *handler)()

        push edx
        push ecx
        push ebx
        push eax

        call Game_dos_setvect

        add esp, 3*4
        pop edx

        retn

; end procedure SR__dos_setvect

; 3 params
align 16
SR__dos_findfirst:

; eax = const char *path
; edx = unsigned attributes
; ebx = struct find_t *buffer

; errno_val is set inside Game_fopen

        Game_Call_Asm_Reg3 Game_dos_findfirst,-1

; end procedure SR__dos_findfirst

align 16
SR_fgets:

; eax = char *buf
; edx = int n
; ebx = FILE *fp

        Game_Call_Asm_Reg3 fgets,'get_errno_val'

; end procedure SR_fgets

align 16
SR_lseek:

; eax = int handle
; edx = long int offset
; ebx = int origin

        Game_Call_Asm_Reg3 lseek,'get_errno_val'

; end procedure SR_lseek

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
SR_read:

; eax = int handle
; edx = void *buffer
; ebx = unsigned len

        Game_Call_Asm_Reg3 read,'get_errno_val'

; end procedure SR_read

align 16
SR_strncmp:

; eax = const char *s1
; edx = const char *s2
; ebx = size_t n

        Game_Call_Asm_Reg3 strncmp,-1

; end procedure SR_strncmp

align 16
SR_strncpy:

; eax = char *dst
; edx = const char *src
; ebx = size_t n

        Game_Call_Asm_Reg3 strncpy,-1

; end procedure SR_strncpy

align 16
SR_strnicmp:

; eax = const char *s1
; edx = const char *s2
; ebx = size_t len

        Game_Call_Asm_Reg3 strncasecmp,-1

; end procedure SR_strnicmp

align 16
SR_write:

; eax = int handle
; edx = void *buffer
; ebx = unsigned len

        Game_Call_Asm_Reg3 write,-1

; end procedure SR_write

; 4 params
align 16
SR__vbprintf:

; eax = char *buf
; edx = size_t bufsize
; ebx = const char *format
; ecx = va_list arg

        mov ecx, [ecx]
        Game_Call_Asm_Reg4 vsnprintf,-1

; end procedure SR__vbprintf

align 16
SR_int386x:

; eax = int inter_no
; edx = const union REGS *in_regs
; ebx = union REGS *out_regs
; ecx = struct SREGS *seg regs

        Game_Call_Asm_Reg4 Game_int386x,-1

; end procedure SR_int386x

; 5 params
align 16
SR__splitpath:

; eax = const char *path
; edx = char *drive
; ebx = char *dir
; ecx = char *fname
; [esp + 4] = char *ext
;
; [esp    ] = return address

        Game_Call_Asm_Reg5 Game_splitpath,-1

; end procedure SR__splitpath
