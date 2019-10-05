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

.arm

.global BufferToScreen320x240x16Asm
.global BufferToScreen320x240x16DoubleAsm

.section .note.GNU-stack,"",%progbits
.text

@input:
@ r0 - dst
@ r1 - src
@ r2 - palette
@ r3 - Height
@ [sp] - Width
@ [sp+4] - screen->w
@
@temp:
@ ip - Width
@ lr - screen->w - Width
@ v1 - horizontal counter
@ v2 - *src
@ v3 - *pal
@ v4 - *dst
@ v5 - *dst2
BufferToScreen320x240x16Asm:
	stmfd sp!, {v1-v5, lr}
	ldr ip, [sp, #(6*4)]
	ldr lr, [sp, #(7*4)]

	rsb v1, r3, #240
	mul v2, v1, lr
	add r0, r0, v2
	rsb v1, ip, #320
	add r0, r0, v1

	sub lr, lr, ip

BufferToScreen320x240x16Asm_OuterLoop:
	mov v1, ip, lsr #2

BufferToScreen320x240x16Asm_InnerLoop:
	ldr v2, [r1], #4
	and v4, v2, #0x00ff
	ldr v4, [r2, v4, lsl #2]

	and v3, v2, #0x00ff00
	ldr v3, [r2, v3, lsr #6]
	orr v4, v4, v3, lsl #16

	and v5, v2, #0x00ff0000
	ldr v5, [r2, v5, lsr #14]

	mov v3, v2, lsr #24
	ldr v3, [r2, v3, lsl #2]
	orr v5, v5, v3, lsl #16

	stmia r0!, {v4, v5}

	subS v1, v1, #1
	bne BufferToScreen320x240x16Asm_InnerLoop

	add r0, r0, lr

	subS r3, r3, #1
	bne BufferToScreen320x240x16Asm_OuterLoop

#exit
	ldmfd sp!, {v1-v5, pc}

# end procedure BufferToScreen320x240x16Asm


@input:
@ r0 - dst
@ r1 - src
@ r2 - palette
@ r3 - Height
@ [sp] - Width
@ [sp+4] - screen->w
@
@temp:
@ ip - Width
@ lr - screen->w - Width
@ v1 - horizontal counter
@ v2 - *src
@ v3 - *pal
@ v4 - *dst
@ v5 - *dst2
@ v6 - dst+w
BufferToScreen320x240x16DoubleAsm:
	stmfd sp!, {v1-v6, lr}
	ldr ip, [sp, #(7*4)]
	ldr lr, [sp, #(8*4)]

	rsb v1, r3, #120
	mul v2, v1, lr
	add r0, r0, v2, lsl #1
	rsb v1, ip, #320
	add r0, r0, v1

	add v6, r0, lr

	rsb lr, ip, lr, lsl #1

BufferToScreen320x240x16DoubleAsm_OuterLoop:
	mov v1, ip, lsr #2

BufferToScreen320x240x16DoubleAsm_InnerLoop:
	ldr v2, [r1], #4
	and v4, v2, #0x00ff
	ldr v4, [r2, v4, lsl #2]

	and v3, v2, #0x00ff00
	ldr v3, [r2, v3, lsr #6]
	orr v4, v4, v3, lsl #16

	and v5, v2, #0x00ff0000
	ldr v5, [r2, v5, lsr #14]

	mov v3, v2, lsr #24
	ldr v3, [r2, v3, lsl #2]
	orr v5, v5, v3, lsl #16

	stmia r0!, {v4, v5}
	stmia v6!, {v4, v5}

	subS v1, v1, #1
	bne BufferToScreen320x240x16DoubleAsm_InnerLoop

	add r0, r0, lr
	add v6, v6, lr

	subS r3, r3, #1
	bne BufferToScreen320x240x16DoubleAsm_OuterLoop

#exit
	ldmfd sp!, {v1-v6, pc}

# end procedure BufferToScreen320x240x16DoubleAsm

