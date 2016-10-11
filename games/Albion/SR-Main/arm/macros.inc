@@
@@  Copyright (C) 2016 Roman Pauer
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

.macro ACTION_UNCONDITIONAL_SHORT_JUMP_BACKWARD
    bl SR_CheckTimer
.endm

.macro ACTION_UNCONDITIONAL_SHORT_JUMP_FORWARD
    @ do nothing
.endm

.macro ACTION_UNCONDITIONAL_JUMP
    bl SR_CheckTimer
.endm

.macro ACTION_LOOP_BACKWARD
    bl SR_CheckTimer
.endm

.macro ACTION_LOOP_FORWARD
    @ do nothing
.endm

.macro ACTION_CONDITIONAL_JUMP_BACKWARD
    bl SR_CheckTimer
.endm

.macro ACTION_CONDITIONAL_JUMP_FORWARD
    @ do nothing
.endm

.macro ACTION_CALL
    bl SR_CheckTimer
.endm

@ ------------------------------------------------------------------------------

.macro CALL___CHK
    add esp, esp, #4
.endm

@ ------------------------------------------------------------------------------

.macro LTORG_CALL
    @ do nothing
.endm

.macro LTORG_IN
    @ do nothing
.endm

.macro LTORG_INT
    @ do nothing
.endm

.macro LTORG_JECXZ
    @ do nothing
.endm

.macro LTORG_JMP
    .ltorg
.endm

.macro LTORG_LOOP
    @ do nothing
.endm

.macro LTORG_OUT
    @ do nothing
.endm

.macro LTORG_RET
    .ltorg
.endm

@#define LTORG_CALL   0
@#define LTORG_IN     0
@#define LTORG_INT    0
@#define LTORG_JECXZ  0
@#define LTORG_JMP    1
@#define LTORG_LOOP   0
@#define LTORG_OUT    0
@#define LTORG_RET    1

@ ------------------------------------------------------------------------------

@@@ edx = [self_mod_width]
@@@ ebp = ebp + (i-1) * edx

.macro self_mod_code_1
    @mov bl, [esi]
        ldrb tmp2, [esi]
    @add edi, eax
        addS edi, edi, eax
    @mov bl, [ebx]
        ldrb tmp1, [ebx, tmp2]	@ bl is cleared before loop
    @adc esi, ecx
        adc esi, esi, ecx
    @mov [ebp], bl
        strb tmp1, [ebp], edx
    @add ebp, edx
        @add ebp, ebp, edx
.endm

.macro self_mod_code_2
    @mov bl, [esi]
        ldrb tmp2, [esi]
    @adc esi, ecx
        adc esi, esi, ecx
    @or  bl, bl
        cmp tmp2, #0
    @jz  short %%loc_B66A2
        beq 1f
    @mov al, [ebx]
        ldrb tmp1, [ebx, tmp2]	@ bl is cleared before loop
        bic eax, eax, #0x00ff
        orr eax, eax, tmp1
    @mov [ebp], al
        strb tmp1, [ebp]

@%%loc_B66A2:
1:
    @add ebp, edx
        add ebp, ebp, edx
    @add edi, eax
        addS edi, edi, eax
.endm

.macro self_mod_code_3
    @mov bl, [esi]
        ldrb tmp2, [esi]
    @or  bl, bl
        cmp tmp2, #0
    @mov al, [ebx]
        ldrb tmp1, [ebx, tmp2]	@ bl is cleared before loop
    @jnz short %%loc_B8450
        bne 1f
    @mov dl, [ebp]
        ldrb tmp2, [ebp]
    @mov al, [edx]
        ldrb tmp1, [edx, tmp2]	@ dl is always cleared

@%%loc_B8450:
1:
        bic eax, eax, #0x00ff
        orr eax, eax, tmp1
    @add edi, eax
        addS edi, edi, eax
    @adc esi, ecx
        adc esi, esi, ecx
    @mov [ebp], al
        strb eax, [ebp], tmp3
    @add ebp, [self_mod_width]
        @add ebp, ebp, tmp3
.endm

.macro self_mod_code_4
    @add edi, eax
        addS edi, edi, eax
    @mov bl, [esi]
        ldrb tmp2, [esi]
    @adc esi, ecx
        adc esi, esi, ecx
    @or  bl, bl
        cmp tmp2, #0
    @jz  short %%loc_BB046
        beq 1f
    @mov bl, [ebx]
        ldrb tmp1, [ebx, tmp2]	@ bl is cleared before loop
    @mov [edx], bl
        strb tmp1, [edx]

@%%loc_BB046:
1:
    @add edx, ebp
        add edx, edx, ebp
.endm

