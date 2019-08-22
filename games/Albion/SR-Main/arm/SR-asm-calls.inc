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

.macro  Game_Call_Asm_Stack proc_name, errno_val=-1000
        ldmfd esp!, {tmp1}
        LDR tmplr, =Game_Registers
        stmea tmplr!, {tmp1, eflags}
        ldmfd esp!, {tmp1,tmp2,tmp3,tmpadr}
        stmea tmplr!, {tmp1,tmp2,tmp3,tmpadr}

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        LDR tmplr, =(Game_Registers + 6*4)
        ldmea tmplr!, {tmp1,tmp2,tmp3,tmpadr}
        stmfd esp!, {tmp1,tmp2,tmp3,tmpadr}
        ldmea tmplr!, {tmp1, eflags}
        bx tmp1

.endm


.macro  Game_Call_Asm_Stack0 proc_name, errno_val=-1000
        ldmfd esp!, {tmp1}
        LDR tmp2, =Game_Registers
        stmia tmp2!, {tmp1, eflags}

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        LDR tmp2, =Game_Registers
        ldmia tmp2!, {tmp1, eflags}
        bx tmp1

.endm


.macro Game_Call_Asm_Reg0 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg1 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        mov tmp1, eax
        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg2 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        mov tmp1, eax
        mov tmp2, edx
        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg3 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        mov tmp1, eax
        mov tmp2, edx
        mov tmp3, ebx
        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg4 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        mov tmp1, eax
        mov tmp2, edx
        mov tmp3, ebx
        mov tmpadr, ecx
        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg5 proc_name, errno_val=-1000

        ldr tmp1, [esp, #4]
        stmfd esp!, {eflags}
        stmfd esp!, {tmp1}

        mov tmp1, eax
        mov tmp2, edx
        mov tmp3, ebx
        mov tmpadr, ecx
        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        add esp, esp, #4
        ldmfd esp!, {eflags}
        ldmfd esp!, {tmp1}
        add esp, esp, #4
        bx tmp1
.endm


.macro Game_Call_Asm_Reg6 proc_name, errno_val=-1000

        add tmpadr, esp, #4
        ldmfd tmpadr, {tmp1, tmp2}
        stmfd esp!, {eflags}
        stmfd esp!, {tmp1, tmp2}

        mov tmp1, eax
        mov tmp2, edx
        mov tmp3, ebx
        mov tmpadr, ecx
        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        add esp, esp, #(2*4)
        ldmfd esp!, {eflags}
        ldmfd esp!, {tmp1}
        add esp, esp, #(2*4)
        bx tmp1
.endm


.macro Game_Call_Asm_Reg8 proc_name, errno_val=-1000

        stmfd esp!, {eflags}
        add eflags, esp, #(2*4)
        ldmfd eflags, {tmp1, tmp2, tmp3, tmpadr}
        stmfd esp!, {tmp1, tmp2, tmp3, tmpadr}

        mov tmp1, eax
        mov tmp2, edx
        mov tmp3, ebx
        mov tmpadr, ecx
        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_errno
        LDR tmpadr, =errno_val
        str tmp1, [tmpadr]
.else
.ifge \errno_val
        LDR tmpadr, =errno_val
        LDR tmp3, =(\errno_val)
        str tmp3, [tmpadr]
.endif
.endif

        add esp, esp, #(4*4)
        ldmfd esp!, {eflags}
        ldmfd esp!, {tmp1}
        add esp, esp, #(4*4)
        bx tmp1
.endm

