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
.extern Game_stdin
.extern Game_stdout
.extern Game_stderr

.extern SR_SlowDownMainLoop
.extern SR_Sync
.extern SR_CheckTimer
.extern SR_RunTimerDelay

@ null procedures
.extern SR___CHK
.extern SR___GETDS
@ stack params
.extern SR_cprintf
.extern SR_fprintf
.extern SR_printf

.extern SR_dlseek
.extern SR_dopen
.extern SR_dread
.extern SR_dclose

.extern SR_DigPlay
.extern SR_AudioCapabilities
.extern SR_StopSound
.extern SR_PostAudioPending
.extern SR_SetPlayMode
.extern SR_PendingAddress
.extern SR_ReportSemaphoreAddress
.extern SR_SetBackFillMode
.extern SR_VerifyDMA
.extern SR_SetDPMIMode
.extern SR_FillSoundCfg
.extern SR_RealPtr
@ 0 params
.extern SR_checkch
.extern SR_getch
.extern SR_fcloseall
@ 1 param
.extern SR_exit
.extern SR_fclose
.extern SR_feof
.extern SR_fflush
.extern SR_fgetc
.extern SR_filelength2
.extern SR__nfree
.extern SR_getenv
.extern SR_inp
.extern SR_isatty
.extern SR__nmalloc
.extern SR_time
.extern SR_WaitVerticalRetraceTicks2
@ 2 params
.extern SR_fopen
.extern SR_fputs
.extern SR_outp
.extern SR_strcat
.extern SR_strcmp
.extern SR_strcpy
@ 3 params
.extern SR_ReadSong
.extern SR_memcpy
.extern SR_memset
.extern SR_strncmp
@ 4 params
.extern SR_fread
.extern SR_fwrite
@ 5 params
