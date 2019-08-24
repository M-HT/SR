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
.extern Game_dlseek
.extern Game_dopen
.extern Game_dread
.extern Game_dclose
.extern Game_DigPlay
.extern Game_AudioCapabilities
.extern Game_StopSound
.extern Game_PostAudioPending
.extern Game_SetPlayMode
.extern Game_PendingAddress
.extern Game_ReportSemaphoreAddress
.extern Game_SetBackFillMode
.extern Game_VerifyDMA
.extern Game_SetDPMIMode
.extern Game_FillSoundCfg
.extern Game_RealPtr

@ stack params
.extern vprintf
@ 0 params
.extern Game_checkch
.extern Game_getch
@ 1 param
.extern fclose
.extern free
.extern Game_inp
.extern malloc
.extern strlen
@ 2 params
.extern Game_fopen
.extern Game_outp
.extern strcat
.extern strcpy
.extern strcasecmp
@ 3 params
.extern Game_ReadSong
@ 4 params
.extern fread
.extern Game_int386x
@ 5 params

.extern errno_val

@ null procedures
.global SR___CHK
@ stack params
.global SR_printf

.global SR_dlseek
.global SR_dopen
.global SR_dread
.global SR_dclose

.global SR_DigPlay
.global SR_AudioCapabilities
.global SR_StopSound
.global SR_PostAudioPending
.global SR_SetPlayMode
.global SR_PendingAddress
.global SR_ReportSemaphoreAddress
.global SR_SetBackFillMode
.global SR_VerifyDMA
.global SR_SetDPMIMode
.global SR_FillSoundCfg
.global SR_RealPtr
@ 0 params
.global SR_checkch
.global SR_getch
@ 1 param
.global SR_fclose
.global SR__nfree
.global SR_inp
.global SR__nmalloc
.global SR_strlen
@ 2 params
.global SR_fopen
.global SR_outp
.global SR_strcat
.global SR_strcpy
.global SR_stricmp
@ 3 params
.global SR_ReadSong
@ 4 params
.global SR_fread
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

null_procedure:

        @retn
        ldmfd esp!, {eip}

@ end procedure null_procedure

@ stack params
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


@ 0 params
SR_checkch:

        Game_Call_Asm_Reg0 Game_checkch,-1

@ end procedure SR_checkch

SR_getch:

        Game_Call_Asm_Reg0 Game_getch,-1

@ end procedure SR_getch


@ 1 param
SR_fclose:

@ eax = FILE *fp

        Game_Call_Asm_Reg1 fclose,-1000

@ end procedure SR_fclose

SR__nfree:

@ eax = void __near *ptr

        Game_Call_Asm_Reg1 free,-1

@ end procedure SR__nfree

SR_inp:

@ eax = int port

        Game_Call_Asm_Reg1 Game_inp,-1

@ end procedure SR_inp

SR__nmalloc:

@ eax = size_t size

        Game_Call_Asm_Reg1 malloc,-1

@ end procedure SR__nmalloc

SR_strlen:

@ eax = const char *s

        Game_Call_Asm_Reg1 strlen,-1

@ end procedure SR_strlen


@ 2 params
SR_fopen:

@ eax = const char *filename
@ edx = const char *mode

@ errno_val is set inside Game_fopen

        Game_Call_Asm_Reg2 Game_fopen,-1

@ end procedure SR_fopen

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

SR_strcpy:

@ eax = char *dst
@ edx = const char *src

        Game_Call_Asm_Reg2 strcpy,-1

@ end procedure SR_strcpy

SR_stricmp:

@ eax = const char *s1
@ edx = const char *s2

        Game_Call_Asm_Reg2 strcasecmp,-1

@ end procedure SR_stricmp


@ 3 params
SR_ReadSong:

@ eax = const char *catalog_name
@ edx = int index
@ ebx = uint8_t *buf

        Game_Call_Asm_Reg3 Game_ReadSong,-1

@ end procedure SR_ReadSong


@ 4 params
SR_fread:

@ eax = void *buf
@ edx = size_t elsize
@ ebx = size_t nelem
@ ecx = FILE *fp

        Game_Call_Asm_Reg4 fread,-1000

@ end procedure SR_fread

SR_int386x:

@ eax = int inter_no
@ edx = const union REGS *in_regs
@ ebx = union REGS *out_regs
@ ecx = struct SREGS *seg regs

        Game_Call_Asm_Reg4 Game_int386x,-1

@ end procedure SR_int386x


@ 5 params
