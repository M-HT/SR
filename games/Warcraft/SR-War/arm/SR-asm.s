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

.arm

.macro pushfd2
    mrs tmp1, cpsr
    stmfd esp!, {tmp1, eflags}
.endm

.macro popfd2
    ldmfd esp!, {tmp1, eflags}
    msr cpsr_f, tmp1
.endm

.extern Game_TimerTick
.extern Game_TimerRun

.extern Game_Sync
.extern Game_RunTimer

.global SR_Sync
.global _SR_Sync

.global SR_CheckTimer
.global _SR_CheckTimer

.section .note.GNU-stack,"",%progbits
.section .text

SR_Sync:
_SR_Sync:

@ [esp       ] = return address

        pushfd2

@ [esp +  2*4] = return address

        @call Game_Sync
        bl Game_Sync

        popfd2

        @retn
        ldmfd esp!, {eip}

@ end procedure SR_Sync

SR_CheckTimer:
_SR_CheckTimer:

@ [esp       ] = return address

        mrs tmp1, cpsr

        LDR tmp2, =Game_TimerRun
        LDR tmp3, =Game_TimerTick
        ldr tmp2, [tmp2]
        ldr tmp3, [tmp3]
        cmp tmp2, tmp3
        bne SR_CheckTimer_RunTimer

        msr cpsr_f, tmp1

        bx lr

    SR_CheckTimer_RunTimer:
        stmfd esp!, {tmp1, eflags, lr}

        bl Game_RunTimer

        ldmfd esp!, {tmp1, eflags, lr}
        msr cpsr_f, tmp1

        bx lr

@ end procedure SR_CheckTimer

