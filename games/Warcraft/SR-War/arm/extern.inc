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

.extern Game_ScreenWindow
.extern Game_Sound
.extern Game_Music
.extern Game_VSyncTick
.extern Game_stdin
.extern Game_stdout
.extern Game_stderr

.extern SR_Sync
.extern SR_CheckTimer

@ null procedures
.extern SR___GETDS
.extern SR__harderr
@ stack params
.extern SR_fprintf
.extern SR_printf
.extern SR_sprintf
.extern SR_open

.extern SR_AIL_active_sample_count
.extern SR_AIL_allocate_sample_handle
.extern SR_AIL_end_sample
.extern SR_AIL_init_sample
.extern SR_AIL_register_EOS_callback
.extern SR_AIL_release_sample_handle
.extern SR_AIL_sample_status
.extern SR_AIL_set_sample_address
.extern SR_AIL_set_sample_loop_count
.extern SR_AIL_set_sample_pan
.extern SR_AIL_set_sample_playback_rate
.extern SR_AIL_set_sample_type
.extern SR_AIL_set_sample_volume
.extern SR_AIL_start_sample

.extern SR_AIL_allocate_sequence_handle
.extern SR_AIL_end_sequence
.extern SR_AIL_init_sequence
.extern SR_AIL_release_sequence_handle
.extern SR_AIL_resume_sequence
.extern SR_AIL_sequence_status
.extern SR_AIL_set_sequence_loop_count
.extern SR_AIL_set_sequence_volume
.extern SR_AIL_start_sequence
.extern SR_AIL_stop_sequence
@ 0 params
.extern SR___clock
.extern SR_j___clock
@ 1 param
.extern SR___close
.extern SR_j___close
.extern SR_cputs
.extern SR___delay
.extern SR_j___delay
.extern SR__dos_gettime
.extern SR_exit
.extern SR_fclose
.extern SR_fgetc
.extern SR__nfree
.extern SR_ftell
.extern SR_getenv
.extern SR_inp
.extern SR__nmalloc
.extern SR_puts
.extern SR_raise
.extern SR_strlen
.extern SR_time
.extern SR_unlink
.extern SR_j_unlink
@ 2 params
.extern SR_fopen
.extern SR_fputc
.extern SR_fputs
.extern SR_outp
.extern SR_signal
.extern SR_strcat
.extern SR_strcmp
.extern SR_strcpy
.extern SR_strrchr
.extern SR_vprintf
@ 3 params
.extern SR_fgets
.extern SR_fseek
.extern SR_lseek
.extern SR_memcmp
.extern SR_memcpy
.extern SR_memmove
.extern SR_memset
.extern SR_read
.extern SR_strncmp
.extern SR_strncpy
.extern SR_strnicmp
.extern SR_strtoul
@ 4 params
.extern SR__fmemset
.extern SR_fread
.extern SR_fwrite
.extern SR_int386x
.extern SR_setvbuf
@ 5 params
.extern SR__fmemcmp
.extern SR__fmemcpy
