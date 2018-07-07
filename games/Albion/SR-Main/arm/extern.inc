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

.extern Game_ScreenWindow
.extern Game_Sound
.extern Game_Music
.extern Game_VSyncTick
.extern Game_stdin
.extern Game_stdout
.extern Game_stderr
.extern self_mod_width


.extern draw_3dscene_proc
.extern Game_UseEnhanced3DEngine

.extern SR_Call_Asm_Float1

.extern SR_Sync
.extern SR_CheckTimer
.extern SR_RunTimerDelay

@ null procedures
.extern SR___CHK
.extern SR___GETDS
.extern SR_nosound
.extern SR_sound
.extern SR__harderr
@ stack params
.extern SR_fprintf
.extern SR_printf
.extern SR__bprintf
.extern SR_sprintf
.extern SR_WaitVerticalRetraceTicks
.extern SR_open

.extern SR_AIL_mem_use_malloc
.extern SR_AIL_mem_use_free
.extern SR_AIL_startup
.extern SR_AIL_register_timer
.extern SR_AIL_set_timer_frequency
.extern SR_AIL_start_timer
.extern SR_AIL_stop_timer
.extern SR_AIL_release_timer_handle
.extern SR_AIL_shutdown
.extern SR_AIL_set_GTL_filename_prefix
.extern SR_AIL_install_MDI_INI
.extern SR_AIL_set_preference
.extern SR_AIL_install_DIG_INI
.extern SR_AIL_uninstall_DIG_driver
.extern SR_AIL_uninstall_MDI_driver

.extern SR_AIL_allocate_sample_handle
.extern SR_AIL_end_sample
.extern SR_AIL_init_sample
.extern SR_AIL_release_sample_handle
.extern SR_AIL_sample_status
.extern SR_AIL_set_digital_master_volume
.extern SR_AIL_set_sample_address
.extern SR_AIL_set_sample_loop_count
.extern SR_AIL_set_sample_pan
.extern SR_AIL_set_sample_playback_rate
.extern SR_AIL_set_sample_type
.extern SR_AIL_set_sample_volume
.extern SR_AIL_start_sample

.extern SR_AIL_allocate_sequence_handle
.extern SR_AIL_create_wave_synthesizer
.extern SR_AIL_destroy_wave_synthesizer
.extern SR_AIL_end_sequence
.extern SR_AIL_init_sequence
.extern SR_AIL_release_sequence_handle
.extern SR_AIL_resume_sequence
.extern SR_AIL_sequence_status
.extern SR_AIL_set_sequence_loop_count
.extern SR_AIL_set_sequence_volume
.extern SR_AIL_start_sequence
.extern SR_AIL_stop_sequence

.extern SR_ERROR_Init
.extern SR_ERROR_ClearMessages
.extern SR_ERROR_AddMessage
.extern SR_ERROR_RemoveMessage
.extern SR_ERROR_NumMessagesIsZero
.extern SR_ERROR_PrintAndClearMessages

.extern SR_BASEMEM_Init
.extern SR_BASEMEM_Deinit
.extern SR_BASEMEM_GetFreeMemSize
.extern SR_BASEMEM_Alloc
.extern SR_BASEMEM_Free
.extern SR_BASEMEM_LockRegion
.extern SR_BASEMEM_UnlockRegion
.extern SR_BASEMEM_MemSetByte
.extern SR_BASEMEM_MemSetDword
.extern SR_BASEMEM_MemMove
.extern SR_BASEMEM_AlignMemptr
.extern SR_BASEMEM_PrintReport

.extern SR_DOS_Init
.extern SR_DOS_DeInit
.extern SR_DOS_Open
.extern SR_DOS_Close
.extern SR_DOS_Read
.extern SR_DOS_Write
.extern SR_DOS_Seek
.extern SR_DOS_GetFileLength
.extern SR_DOS_exists
.extern SR_DOS_setcurrentdir
.extern SR_DOS_GetSeekPosition
@ 0 params
.extern SR_fcloseall
.extern SR_WaitAfterVerticalRetrace
.extern SR_WaitForVerticalRetrace
.extern SR_WaitAfter2ndVerticalRetrace
.extern SR_WaitFor2ndVerticalRetrace
@ 1 param
.extern SR_asctime
.extern SR_chdir
.extern SR___close
.extern SR_j___close
.extern SR_closedir
.extern SR_ctime
.extern SR__dos_findclose
.extern SR__dos_findnext
.extern SR__dos_getvect
.extern SR_exit
.extern SR_fclose
.extern SR_filelength
.extern SR__nfree
.extern SR_ftime
.extern SR_getenv
.extern SR_isatty
.extern SR_localtime
.extern SR__nmalloc
.extern SR_opendir
.extern SR__setjmp
.extern SR_strlen
.extern SR_tell
.extern SR_time
.extern SR_unlink
.extern SR_j_unlink
.extern SR_WaitTimerTicks
.extern SR_save_screenshot
@ 2 params
.extern SR_fopen
.extern SR_fputs
.extern SR_getcwd
.extern SR_longjmp
.extern SR_outp
.extern SR_rename
.extern SR_setbuf
.extern SR_strchr
.extern SR_strcmp
.extern SR_strcpy
.extern SR_strcspn
.extern SR_strspn
@ 2/3 params
.extern SR__dos_setvect
@ 3 params
.extern SR__dos_findfirst
.extern SR_fgets
.extern SR_lseek
.extern SR_memcpy
.extern SR_memmove
.extern SR_memset
.extern SR_read
.extern SR_strncmp
.extern SR_strncpy
.extern SR_strnicmp
.extern SR_write
@ 4 params
.extern SR_int386x
.extern SR__vbprintf
@ 5 params
.extern SR__splitpath
