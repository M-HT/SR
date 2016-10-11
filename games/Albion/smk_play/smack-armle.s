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

.arm

.extern BitStreamFillBuffer

.global BitStreamReadBitAsm

#typedef struct _BitStream_ {
#	FILE *File;
#	uint8_t *Buffer;
#	uint32_t BufferSize;
#	uint32_t BytesRead;
#	uint32_t BufferPos;
#	uint32_t BitsLeft;
#	uint32_t BytesLeft;
#	uint32_t Empty;
#} BitStream;
.equ BS_File, 0
.equ BS_Buffer, 4
.equ BS_BufferSize, 8
.equ BS_BytesRead, 12
.equ BS_BufferPos, 16
.equ BS_BitsLeft, 20
.equ BS_BytesLeft, 24
.equ BS_Empty, 28

.section .text

#uint32_t BitStreamReadBitAsm(BitStream *bitstream);
BitStreamReadBitAsm:
	ldr r1, [r0, #BS_Empty]
	cmp r1, #0
	movne r0, #0
#exit
	movne pc, lr

	mov r3, r0

	ldr r12, [r3, #BS_BytesRead]
	ldr r2, [r3, #BS_BufferPos]
	ldr r1, [r3, #BS_BitsLeft]

	cmp r1, #0
	bne BitStreamReadBitAsm_BitsLeft_After
	add r0, r2, #1
	cmp r0, r12
	bhs BitStreamReadBitAsm_FillBuffer

BitStreamReadBitAsm_FillBuffer_After:
	mov r1, #8
	add r2, r2, #1
	str r2, [r3, #BS_BufferPos]

BitStreamReadBitAsm_BitsLeft_After:
	ldr r0, [r3, #BS_Buffer]
	ldrb r0, [r0, r2]
	mov r0, r0, lsl r1
	mov r0, r0, lsr #8
	and r0, r0, #1
	subS r1, r1, #1
	str r1, [r3, #BS_BitsLeft]
#exit
	movne pc, lr
	add r2, r2, #1
	cmp r2, r12
#exit
	movlo pc, lr
	ldr r1, [r3, #BS_BytesLeft]
	cmp r1, #0
	moveq r1, #1
	streq r1, [r3, #BS_Empty]
#exit
	bx lr

BitStreamReadBitAsm_FillBuffer:
	stmfd sp!, {r3, lr}
	mov r0, r3
	bl BitStreamFillBuffer
	ldmfd sp!, {r3, lr}
	ldr r12, [r3, #BS_BytesRead]
	ldr r2, [r3, #BS_BufferPos]
	ldr r1, [r3, #BS_BitsLeft]
	cmp r1, #0
	bne BitStreamReadBitAsm_BitsLeft_After
	b BitStreamReadBitAsm_FillBuffer_After

# end procedure BitStreamReadBitAsm

