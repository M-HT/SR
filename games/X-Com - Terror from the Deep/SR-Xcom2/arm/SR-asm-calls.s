@@
@@  Copyright (C) 2016-2023 Roman Pauer
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
.include "armconf.inc"


.extern Game_Set_errno_val
.extern Game_Set_errno_val_num


@ stack params
.extern vfprintf
.extern vprintf
.extern Game_WaitVerticalRetraceTicks

.extern Game_dlseek
.extern Game_dopen
.extern Game_dread
.extern Game_dclose

.extern Game_DigPlay
.extern Game_AudioCapabilities
.extern Game_StopSound
.extern Game_PostAudioPending
.extern Game_AudioPendingStatus
.extern Game_SetPlayMode
.extern Game_PendingAddress
.extern Game_ReportSemaphoreAddress
.extern Game_SetBackFillMode
.extern Game_VerifyDMA
.extern Game_SetDPMIMode
.extern Game_FillSoundCfg
.extern Game_RealPtr
.extern Game_StartAnimVideo
.extern Game_StopAnimVideo
@ 0 params
.extern Game_checkch
.extern Game_getch
.extern Game_fcloseall
@ 1 param
.extern Game_ExitMain_Asm
.extern Game_fclose
.extern feof
.extern fflush
.extern fgetc
.extern Game_filelength2
.extern Game_free
.extern Game_inp
.extern Game_malloc
.extern time
@ 2 params
.extern Game_fopen
.extern fputc
.extern fputs
.extern Game_outp
.extern strcat
.extern strcmp
.extern strcpy
@ 3 params
.extern Game_ReadSong
.extern memcpy
.extern memset
.extern strncmp
@ 4 params
.extern fread
.extern fwrite
.extern Game_int386x
@ 5 params


@ null procedures
.global SR___CHK
.global SR___GETDS
@ stack params
.global SR_cprintf
.global SR_fprintf
.global SR_printf

.global SR_dlseek
.global SR_dopen
.global SR_dread
.global SR_dclose

.global SR_DigPlay
.global SR_AudioCapabilities
.global SR_StopSound
.global SR_PostAudioPending
.global SR_AudioPendingStatus
.global SR_SetPlayMode
.global SR_PendingAddress
.global SR_ReportSemaphoreAddress
.global SR_SetBackFillMode
.global SR_VerifyDMA
.global SR_SetDPMIMode
.global SR_FillSoundCfg
.global SR_RealPtr
.global SR_StartAnimVideo
.global SR_StopAnimVideo
@ 0 params
.global SR_checkch
.global SR_getch
.global SR_fcloseall
@ 1 param
.global SR_exit
.global SR_fclose
.global SR_feof
.global SR_fflush
.global SR_fgetc
.global SR_filelength2
.global SR__nfree
.global SR_inp
.global SR__nmalloc
.global SR_time
.global SR_WaitVerticalRetraceTicks2
@ 2 params
.global SR_fopen
.global SR_fputc
.global SR_fputs
.global SR_outp
.global SR_strcat
.global SR_strcmp
.global SR_strcpy
@ 3 params
.global SR_ReadSong
.global SR_memcpy
.global SR_memset
.global SR_strncmp
@ 4 params
.global SR_fread
.global SR_fwrite
.global SR_int386x
@ 5 params

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

        Game_Call_Asm_VariableStack2 vfprintf,-1000

@ end procedure SR_fprintf

SR_cprintf:
SR_printf:

@ [esp + 2*4] = ...
@ [esp +   4] = const char *format
@ [esp      ] = return address

        Game_Call_Asm_VariableStack1 vprintf,-1000

@ end procedure SR_printf

SR_dlseek:

@ [esp + 3*4] = int whence
@ [esp + 2*4] = int offset
@ [esp +   4] = int fd
@ [esp      ] = return address

        Game_Call_Asm_Stack3 Game_dlseek,-1

@ end procedure SR_dlseek

SR_dopen:

@ [esp + 2*4] = const char *mode
@ [esp +   4] = const char *path
@ [esp      ] = return address

        Game_Call_Asm_Stack2 Game_dopen,-1

@ end procedure SR_dopen

SR_dread:

@ [esp + 3*4] = int fd
@ [esp + 2*4] = int count
@ [esp +   4] = void *buf
@ [esp      ] = return address

        Game_Call_Asm_Stack3 Game_dread,-1

@ end procedure SR_dread

SR_dclose:

@ [esp +   4] = int fd
@ [esp      ] = return address

        Game_Call_Asm_Stack1 Game_dclose,-1

@ end procedure SR_dclose

SR_DigPlay:

@ [esp +   4] = SNDSTRUC *sndplay
@ [esp      ] = return address

        Game_Call_Asm_Stack1 Game_DigPlay,-1

@ end procedure SR_DigPlay

SR_AudioCapabilities:

        Game_Call_Asm_Reg0 Game_AudioCapabilities,-1

@ end procedure SR_AudioCapabilities

SR_StopSound:

        Game_Call_Asm_Reg0 Game_StopSound,-1

@ end procedure SR_StopSound

SR_PostAudioPending:

@ [esp +   4] = SNDSTRUC *sndplay
@ [esp      ] = return address

        Game_Call_Asm_Stack1 Game_PostAudioPending,-1

@ end procedure SR_PostAudioPending

SR_AudioPendingStatus:

        Game_Call_Asm_Reg0 Game_AudioPendingStatus,-1

@ end procedure SR_AudioPendingStatus

SR_SetPlayMode:

@ [esp +   4] = short mode
@ [esp      ] = return address

        Game_Call_Asm_Stack1 Game_SetPlayMode,-1

@ end procedure SR_SetPlayMode

SR_PendingAddress:

        Game_Call_Asm_Reg0 Game_PendingAddress,-1

@ end procedure SR_PendingAddress

SR_ReportSemaphoreAddress:

        Game_Call_Asm_Reg0 Game_ReportSemaphoreAddress,-1

@ end procedure SR_ReportSemaphoreAddress

SR_SetBackFillMode:

@ [esp +   4] = short mode
@ [esp      ] = return address

        Game_Call_Asm_Stack1 Game_SetBackFillMode,-1

@ end procedure SR_SetBackFillMode

SR_VerifyDMA:

@ [esp +   8] = short length
@ [esp +   4] = char *data
@ [esp      ] = return address

        Game_Call_Asm_Stack2 Game_VerifyDMA,-1

@ end procedure SR_VerifyDMA

SR_SetDPMIMode:

@ [esp +   4] = short mode
@ [esp      ] = return address

        Game_Call_Asm_Stack1 Game_SetDPMIMode,-1

@ end procedure SR_SetDPMIMode

SR_FillSoundCfg:

@ [esp +   8] = int count
@ [esp +   4] = void *buf
@ [esp      ] = return address

        Game_Call_Asm_Stack2 Game_FillSoundCfg,-1

@ end procedure SR_FillSoundCfg

SR_RealPtr:

@ [esp +   4] = char *real
@ [esp      ] = return address

        Game_Call_Asm_Stack1 Game_RealPtr,-1

@ end procedure SR_RealPtr

SR_StartAnimVideo:

        Game_Call_Asm_Reg0 Game_StartAnimVideo,-1

@ end procedure SR_StartAnimVideo

SR_StopAnimVideo:

        Game_Call_Asm_Reg0 Game_StopAnimVideo,-1

@ end procedure SR_StopAnimVideo


@ 0 params
SR_checkch:

        Game_Call_Asm_Reg0 Game_checkch,-1

@ end procedure SR_checkch

SR_getch:

        Game_Call_Asm_Reg0 Game_getch,-1

@ end procedure SR_getch

SR_fcloseall:

        Game_Call_Asm_Reg0 Game_fcloseall,-1

@ end procedure SR_fcloseall


@ 1 param
SR_exit:

@ eax = int status

        b Game_ExitMain_Asm

@ end procedure SR_exit

SR_fclose:

@ eax = FILE *fp

@ errno_val is set inside Game_fclose

        Game_Call_Asm_Reg1 Game_fclose,-1

@ end procedure SR_fclose

SR_feof:

@ eax = FILE *fp

        Game_Call_Asm_Reg1 feof,-1

@ end procedure SR_feof

SR_fflush:

@ eax = FILE *fp

        Game_Call_Asm_Reg1 fflush,-1000

@ end procedure SR_fflush

SR_fgetc:

@ eax = FILE *fp

        Game_Call_Asm_Reg1 fgetc,-1000

@ end procedure SR_fgetc

SR_filelength2:

@ eax = FILE *fp

        Game_Call_Asm_Reg1 Game_filelength2,-1

@ end procedure SR_filelength2

SR__nfree:

@ eax = void __near *ptr

        Game_Call_Asm_Reg1 Game_free,-1

@ end procedure SR__nfree

SR_inp:

@ eax = int port

        Game_Call_Asm_Reg1 Game_inp,-1

@ end procedure SR_inp

SR__nmalloc:

@ eax = size_t size

        Game_Call_Asm_Reg1 Game_malloc,-1

@ end procedure SR__nmalloc

SR_time:

@ eax = time_t *tloc

        Game_Call_Asm_Reg1 time,-1

@ end procedure SR_time

SR_WaitVerticalRetraceTicks2:

@ eax = int ticks

        Game_Call_Asm_Reg1 Game_WaitVerticalRetraceTicks,-1

@ end procedure SR_WaitVerticalRetraceTicks2


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

@	return value:
@		<0 => -1
@		fallout expects -1 (on error)
@		fputs returns negative value

@ eax = const char *buf
@ edx = FILE *fp

@	Game_Call_Asm_Reg2 fputs,-1000
        stmfd esp!, {eflags}

        mov tmp1, eax
        mov tmp2, edx

        ALIGN_STACK

        bl fputs

        cmp tmp1, #0
        movGE eax, tmp1
        movLT eax, #-1

        bl Game_Set_errno_val

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

@ end procedure SR_fputs

SR_outp:

@ eax = int port
@ edx = int value

        Game_Call_Asm_Reg2 Game_outp,-1

@ end procedure SR_outp

SR_strcat:

@ eax = const char *dst
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


@ 3 params
SR_ReadSong:

@ eax = const char *catalog_name
@ edx = int index
@ ebx = uint8_t *buf

        Game_Call_Asm_Reg3 Game_ReadSong,-1

@ end procedure SR_ReadSong

SR_memcpy:

@ eax = void *dst
@ edx = const void *src
@ ebx = size_t length

        Game_Call_Asm_Reg3 memcpy,-1

@ end procedure SR_memcpy

SR_memset:

@ eax = void *dst
@ edx = int c
@ ebx = size_t length

        Game_Call_Asm_Reg3 memset,-1

@ end procedure SR_memset

SR_strncmp:

@ eax = const char *s1
@ edx = const char *s2
@ ebx = size_t n

        Game_Call_Asm_Reg3 strncmp,-1

@ end procedure SR_strncmp


@ 4 params
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


@ 5 params
