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

.ifdef ALLOW_UNALIGNED_STACK

.macro ALIGN_STACK
    @ do nothing
.endm

.macro RESTORE_STACK
    @ do nothing
.endm

.else

.macro ALIGN_STACK
    @ remember original esp value
        mov eflags, esp
    @ reserve 4 bytes on stack
        sub lr, esp, #4
    @ align stack to 8 bytes
        bic esp, lr, #7
    @ save original esp value on stack
        str eflags, [esp]
.endm

.macro RESTORE_STACK
    @ restore original esp value from stack
        ldr esp, [esp]
.endm

.endif


.macro Game_Call_Asm_VariableStack1 proc_name, errno_val=-1000

        ldr tmp1, [esp, #4]
        add tmp2, esp, #(2*4)
        stmfd esp!, {eflags}

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_VariableStack2 proc_name, errno_val=-1000

        add tmp3, esp, #4
        ldmia tmp3!, {tmp1, tmp2}
        stmfd esp!, {eflags}

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Stack1 proc_name, errno_val=-1000

        ldr tmp1, [esp, #4]
        stmfd esp!, {eflags}

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Stack2 proc_name, errno_val=-1000

        add tmpadr, esp, #4
        ldmia tmpadr, {tmp1, tmp2}
        stmfd esp!, {eflags}

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Stack3 proc_name, errno_val=-1000

        add tmpadr, esp, #4
        ldmia tmpadr, {tmp1, tmp2, tmp3}
        stmfd esp!, {eflags}

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg0 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg1 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        mov tmp1, eax

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg2 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        mov tmp1, eax
        mov tmp2, edx

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg3 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        mov tmp1, eax
        mov tmp2, edx
        mov tmp3, ebx

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg4 proc_name, errno_val=-1000

        stmfd esp!, {eflags}

        mov tmp1, eax
        mov tmp2, edx
        mov tmp3, ebx
        mov tmpadr, ecx

        ALIGN_STACK

        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

        RESTORE_STACK

        ldmfd esp!, {eflags, eip}

.endm


.macro Game_Call_Asm_Reg5 proc_name, errno_val=-1000

.ifdef ALLOW_UNALIGNED_STACK
        ldr tmp1, [esp, #4]
        stmfd esp!, {tmp1, eflags}
.else
    @ get function argument from original stack
        ldr tmp1, [esp, #4]
    @ remember original esp value
        mov lr, esp
    @ reserve 12 bytes on stack
        sub tmp2, esp, #(3*4)
    @ align stack to 8 bytes
        bic esp, tmp2, #7
    @ put function argument, eflags, original esp value to stack
        stmia esp, {tmp1, eflags, lr}
.endif

        mov tmp1, eax
        mov tmp2, edx
        mov tmp3, ebx
        mov tmpadr, ecx
        bl \proc_name
        mov eax, tmp1

.ifeq \errno_val + 1000
        bl Game_Set_errno_val
.else
.ifge \errno_val
        LDR tmp1, =(\errno_val)
        bl Game_Set_errno_val_num
.endif
.endif

.ifdef ALLOW_UNALIGNED_STACK
        add tmp2, esp, #4
        ldmia tmp2, {eflags, lr}
        add esp, esp, #(4*4)
        bx lr
.else
        add tmp2, esp, #4
    @ restore eflags, original esp value from stack
        ldmia tmp2, {eflags, esp}
    @ load return value from stack; adjust stack
        ldr lr, [esp], #(2*4)
        bx lr
.endif

.endm

