/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include "smack.h"

#if defined(_MSC_VER)

#undef BIG_ENDIAN_BYTE_ORDER

#elif defined(__BYTE_ORDER__)

#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define BIG_ENDIAN_BYTE_ORDER
#else
#undef BIG_ENDIAN_BYTE_ORDER
#endif

#else

#include <endian.h>
#if (__BYTE_ORDER == __BIG_ENDIAN)
#define BIG_ENDIAN_BYTE_ORDER
#else
#undef BIG_ENDIAN_BYTE_ORDER
#endif

#endif

#ifdef BIG_ENDIAN_BYTE_ORDER
	#define LE2NATIVE32(x) (  ((x) << 24)               | \
	                         (((x) <<  8) & 0x00ff0000) | \
	                         (((x) >>  8) & 0x0000ff00) | \
	                          ((x) >> 24)               )
#else
	#define LE2NATIVE32(x) (x)
#endif


/* palette map - convert 6bit palette color to 8bit palette color */
const static uint8_t palmap[64] = {
	0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C,
	0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C,
	0x41, 0x45, 0x49, 0x4D, 0x51, 0x55, 0x59, 0x5D,
	0x61, 0x65, 0x69, 0x6D, 0x71, 0x75, 0x79, 0x7D,
	0x82, 0x86, 0x8A, 0x8E, 0x92, 0x96, 0x9A, 0x9E,
	0xA2, 0xA6, 0xAA, 0xAE, 0xB2, 0xB6, 0xBA, 0xBE,
	0xC3, 0xC7, 0xCB, 0xCF, 0xD3, 0xD7, 0xDB, 0xDF,
	0xE3, 0xE7, 0xEB, 0xEF, 0xF3, 0xF7, 0xFB, 0xFF};

const static uint32_t sizetable[64] = {
	 1,    2,    3,    4,    5,    6,    7,    8,
	 9,   10,   11,   12,   13,   14,   15,   16,
	17,   18,   19,   20,   21,   22,   23,   24,
	25,   26,   27,   28,   29,   30,   31,   32,
	33,   34,   35,   36,   37,   38,   39,   40,
	41,   42,   43,   44,   45,   46,   47,   48,
	49,   50,   51,   52,   53,   54,   55,   56,
	57,   58,   59,  128,  256,  512, 1024, 2048};

static int SmackErrorCode;

typedef struct _Tree8_ {
	uint32_t NumNodes;
	uint32_t Nodes[511];
/*	Nodes:	Bit 31: 0 - Leaf
					1 - Node
*/
} Tree8;

typedef struct _Tree16_ {
	uint32_t Node1;
	uint32_t Node2;
	uint32_t Node3;
	uint32_t NumNodes;
	uint32_t LastDecoded;
	uint32_t LastDecoded2;
	uint32_t LastDecoded3;
	uint32_t Nodes[];
/*	Nodes:	Bit 31: 0 - Leaf
					1 - Node
*/
} Tree16;

typedef struct _BitStream_ {
	FILE *File;
	uint8_t *Buffer;
	uint32_t BufferSize;
	uint32_t BytesRead;
	uint32_t BufferPos;
	uint32_t BitsLeft;
	uint32_t BytesLeft;
	uint32_t Empty;
} BitStream;

static void BitStreamInitialize(BitStream *bitstream, FILE *File,
								uint8_t *Buffer, uint32_t BufferSize,
								uint32_t BitStreamSize)
{
	bitstream->File = File;
	bitstream->Buffer = Buffer;
	bitstream->BufferSize = BufferSize;
	bitstream->BytesRead = 0;
	bitstream->BufferPos = 0;
	bitstream->BitsLeft = 0;
	bitstream->BytesLeft = BitStreamSize;
	bitstream->Empty = !BitStreamSize;
}

#if !(defined(GP2X) && defined(__GNUC__))
static
#endif
void BitStreamFillBuffer(BitStream *bitstream)
{
	uint32_t count;

	if (bitstream->Empty) return;

	bitstream->BufferPos = 0;
	bitstream->BitsLeft = 0;

	count = bitstream->BytesLeft;
	if (count > bitstream->BufferSize) count = bitstream->BufferSize;

	bitstream->BytesRead = fread(bitstream->Buffer, 1, count, bitstream->File);

	if (bitstream->BytesRead != 0)
	{
		bitstream->BitsLeft = 8;
		bitstream->BytesLeft -= bitstream->BytesRead;
	}
}

#define BitStreamReadBitMacro(bitstream, result) \
{ \
	if ((bitstream)->Empty) \
	{ \
		result = 0; \
	} \
	else \
	{ \
		if ((bitstream)->BitsLeft == 0 && \
			(bitstream)->BufferPos + 1 >= (bitstream)->BytesRead) \
		{ \
			BitStreamFillBuffer(bitstream); \
		} \
		if ((bitstream)->BitsLeft == 0) \
		{ \
			(bitstream)->BitsLeft = 8; \
			(bitstream)->BufferPos++; \
		} \
		result = ((bitstream)->Buffer[(bitstream)->BufferPos] >> (8 - (bitstream)->BitsLeft)) & 1; \
		(bitstream)->BitsLeft--; \
		if ((bitstream)->BitsLeft == 0 && \
			(bitstream)->BufferPos + 1 >= (bitstream)->BytesRead && \
			(bitstream)->BytesLeft == 0) \
		{ \
			(bitstream)->Empty = 1; \
		} \
	} \
}

#if defined(GP2X) && defined(__GNUC__)

static uint32_t __attribute__ ((noinline, naked)) BitStreamReadBitAsm(BitStream *bitstream)
{
	asm(
		// BitStream offsets
		".equ BS_File, 0" "\n"
		".equ BS_Buffer, 4" "\n"
		".equ BS_BufferSize, 8" "\n"
		".equ BS_BytesRead, 12" "\n"
		".equ BS_BufferPos, 16" "\n"
		".equ BS_BitsLeft, 20" "\n"
		".equ BS_BytesLeft, 24" "\n"
		".equ BS_Empty, 28" "\n"

		"ldr r1, [r0, #BS_Empty]" "\n"
		"cmp r1, #0" "\n"
		"movne r0, #0" "\n"
		// exit
		"movne pc, lr" "\n"

		"mov r3, r0" "\n"

		"ldr r12, [r3, #BS_BytesRead]" "\n"
		"ldr r2, [r3, #BS_BufferPos]" "\n"
		"ldr r1, [r3, #BS_BitsLeft]" "\n"

		"cmp r1, #0" "\n"
		"bne 2f" "\n"
		"add r0, r2, #1" "\n"
		"cmp r0, r12" "\n"
		"bhs 3f" "\n"

		"1:" "\n"
		"mov r1, #8" "\n"
		"add r2, r2, #1" "\n"
		"str r2, [r3, #BS_BufferPos]" "\n"

		"2:" "\n"
		"ldr r0, [r3, #BS_Buffer]" "\n"
		"ldrb r0, [r0, r2]" "\n"
		"mov r0, r0, lsl r1" "\n"
		"mov r0, r0, lsr #8" "\n"
		"and r0, r0, #1" "\n"
		"subS r1, r1, #1" "\n"
		"str r1, [r3, #BS_BitsLeft]" "\n"
		// exit
		"movne pc, lr" "\n"
		"add r2, r2, #1" "\n"
		"cmp r2, r12" "\n"
		// exit
		"movlo pc, lr" "\n"
		"ldr r1, [r3, #BS_BytesLeft]" "\n"
		"cmp r1, #0" "\n"
		"moveq r1, #1" "\n"
		"streq r1, [r3, #BS_Empty]" "\n"
		// exit
		"bx lr" "\n"

		"3:" "\n"
		"stmfd sp!, {r3, lr}" "\n"
		"mov r0, r3" "\n"
		"bl BitStreamFillBuffer" "\n"
		"ldmfd sp!, {r3, lr}" "\n"
		"ldr r12, [r3, #BS_BytesRead]" "\n"
		"ldr r2, [r3, #BS_BufferPos]" "\n"
		"ldr r1, [r3, #BS_BitsLeft]" "\n"
		"cmp r1, #0" "\n"
		"bne 2b" "\n"
		"b 1b" "\n"
	);
}

#define BitStreamReadBit(x) BitStreamReadBitAsm(x)

#else

static uint32_t BitStreamReadBit(BitStream *bitstream)
{
	uint32_t ret;

	BitStreamReadBitMacro(bitstream, ret);
#if (0)
	if (bitstream->Empty) return 0;

/* check if buffer needs refilling */
	if (bitstream->BitsLeft == 0 &&
		bitstream->BufferPos + 1 >= bitstream->BytesRead)
	{
		BitStreamFillBuffer(bitstream);
	}

/* check if there are bits left to be read in the current byte */
	if (bitstream->BitsLeft == 0)
	{
		bitstream->BitsLeft = 8;
		bitstream->BufferPos++;
	}

/* read bit */
	ret = (bitstream->Buffer[bitstream->BufferPos] >> (8 - bitstream->BitsLeft)) & 1;
	bitstream->BitsLeft--;

/* set empty flag if needed */
	if (bitstream->BitsLeft == 0 &&
		bitstream->BufferPos + 1 >= bitstream->BytesRead &&
		bitstream->BytesLeft == 0)
	{
		bitstream->Empty = 1;
	}
#endif

	return ret;
}
#endif

static uint32_t BitStreamReadBits8(BitStream *bitstream)
{
	uint32_t ret;

	if (bitstream->Empty) return 0;

/* check if buffer needs refilling */
	if (bitstream->BytesRead == 0 ||
		(bitstream->BitsLeft == 0 &&
		 bitstream->BufferPos + 1 >= bitstream->BytesRead) )
	{
		BitStreamFillBuffer(bitstream);
	}

/* check if there are bits left to be read in the current byte */
	if (bitstream->BitsLeft == 0)
	{
		bitstream->BitsLeft = 8;
		bitstream->BufferPos++;
	}

	if (bitstream->BitsLeft == 8)
	{
/* there are 8 bits to be read for current byte -> read whole byte */
		ret = bitstream->Buffer[bitstream->BufferPos];
		bitstream->BitsLeft = 0;

/* set empty flag if needed */
		if (bitstream->BufferPos + 1 >= bitstream->BytesRead &&
			bitstream->BytesLeft == 0)
		{
			bitstream->Empty = 1;
		}
	}
	else
	{
		uint32_t bitsleft;

/* read all remaining bits from current byte */
		bitsleft = bitstream->BitsLeft;
		ret = bitstream->Buffer[bitstream->BufferPos] >> (8 - bitsleft);

/* check if buffer needs refilling */
		if (bitstream->BufferPos + 1 >= bitstream->BytesRead)
		{
			if (bitstream->BytesLeft == 0)
			{
				bitstream->BitsLeft = 0;
				bitstream->Empty = 1;
				return ret;
			}
			BitStreamFillBuffer(bitstream);
		}
		else
		{
/* advance to the next byte */
			bitstream->BitsLeft = 8;
			bitstream->BufferPos++;
		}

/* read needed bits from current byte */
/*		ret = ((bitstream->Buffer[bitstream->BufferPos] & (0xff >> bitsleft)) << bitsleft) | ret;*/
		ret = ((bitstream->Buffer[bitstream->BufferPos] << bitsleft) & 0xff) | ret;
		bitstream->BitsLeft = bitsleft;
	}

	return ret;
}

static uint32_t BitStreamReadBits16(BitStream *bitstream)
{
	uint32_t ret;

	ret = BitStreamReadBits8(bitstream);
	return (BitStreamReadBits8(bitstream) << 8) | ret;
}

static void CreateHuffmanTree8(BitStream *bitstream, Tree8 *Tree)
{
	uint32_t Bit, Leaf;
	uint32_t MaxNodes, NumRem, CurNodeNum;
	uint32_t RemNodes[256];

/* initialize tree */
	CurNodeNum = 0;
	MaxNodes = Tree->NumNodes;
	Tree->NumNodes = 1;
	Tree->Nodes[0] = 0;

	if (bitstream->Empty) return;
	Bit = BitStreamReadBit(bitstream);
/* Data not present */
	if (Bit == 0) return;

	NumRem = 0;
	while (!bitstream->Empty)
	{
		Bit = BitStreamReadBit(bitstream);

		if (Bit)
		{
/* remember current node */
/* advance to the '0' branch */
			if (NumRem >= 256) return;

			RemNodes[NumRem] = CurNodeNum;
			NumRem++;

			if (Tree->NumNodes + 1 >= MaxNodes) return;

			Tree->Nodes[Tree->NumNodes] = 0;
			Tree->Nodes[Tree->NumNodes + 1] = 0;
			Tree->Nodes[CurNodeNum] = 0x80000000 | Tree->NumNodes;

			CurNodeNum = Tree->NumNodes;
			Tree->NumNodes += 2;
		}
		else
		{
/* current node is leaf */
/* read leaf value */
/* set current node to the '1' branch of the last remembered node */
/* if no node remembered -> the tree is finished */
			Leaf = BitStreamReadBits8(bitstream);
			Tree->Nodes[CurNodeNum] = Leaf;

			if (NumRem == 0)
			{
				Bit = BitStreamReadBit(bitstream);
				return;
			}

			NumRem--;
			CurNodeNum = (Tree->Nodes[RemNodes[NumRem]] & 0x7fffffff) + 1;
		}
	}
}

static uint32_t DecodeHuffmanTree8(BitStream *bitstream, Tree8 *Tree)
{
	uint32_t CurNodeNum;

	CurNodeNum = 0;

	while ( !bitstream->Empty && (Tree->Nodes[CurNodeNum] & 0x80000000) != 0 )
	{
		uint32_t Bit;

		CurNodeNum = (Tree->Nodes[CurNodeNum] & 0x7fffffff);

/*		Bit = BitStreamReadBit(bitstream);*/
		BitStreamReadBitMacro(bitstream, Bit);

		CurNodeNum += Bit;
	}

	return ((Tree->Nodes[CurNodeNum] & 0x80000000) == 0)?(Tree->Nodes[CurNodeNum]):(0);
}

static void CreateHuffmanTree16(BitStream *bitstream, Tree16 *Tree)
{
	Tree8 TreeLow;
	Tree8 TreeHigh;

	uint32_t Bit, Leaf, MaxNodes;
	uint32_t NumRem, CurNodeNum;
	uint32_t Leaf1, Leaf2, Leaf3;
	uint32_t RemNodes[1024];

/* initialize tree */
	CurNodeNum = 0;
	MaxNodes = Tree->NumNodes;
	Tree->NumNodes = 1;
	Tree->LastDecoded = 0;
	Tree->LastDecoded2 = 0;
	Tree->LastDecoded3 = 0;
	Tree->Node1 = 0xffffffff;
	Tree->Node2 = 0xffffffff;
	Tree->Node3 = 0xffffffff;
	Tree->Nodes[0] = 0;

	if (bitstream->Empty) return;
	Bit = BitStreamReadBit(bitstream);
/* Data not present */
	if (Bit == 0) return;

	TreeLow.NumNodes = 511;
	TreeHigh.NumNodes = 511;

/* read 8 bit trees for low and high bytes of 16 bit tree */
	CreateHuffmanTree8(bitstream, &TreeLow);
	CreateHuffmanTree8(bitstream, &TreeHigh);

/* read special values for nodes with last decoded values */
	Leaf1 = BitStreamReadBits16(bitstream);
	Leaf2 = BitStreamReadBits16(bitstream);
	Leaf3 = BitStreamReadBits16(bitstream);

	NumRem = 0;
	while (!bitstream->Empty)
	{
		Bit = BitStreamReadBit(bitstream);

		if (Bit)
		{
/* remember current node */
/* advance to the '0' branch */
			if (NumRem >= 1024) return;

			RemNodes[NumRem] = CurNodeNum;
			NumRem++;

			if (Tree->NumNodes + 1 >= MaxNodes) return;

			Tree->Nodes[Tree->NumNodes] = 0;
			Tree->Nodes[Tree->NumNodes + 1] = 0;
			Tree->Nodes[CurNodeNum] = 0x80000000 | Tree->NumNodes;
			CurNodeNum = Tree->NumNodes;

			Tree->NumNodes += 2;
		}
		else
		{
/* current node is leaf */
/* read leaf value */
/* if leaf value matches one of 3 special values -> set leaf to 0 and remember special node */
/* set current node to the '1' branch of the last remembered node */
/* if no node remembered -> the tree is finished */
			Leaf = DecodeHuffmanTree8(bitstream, &TreeLow);
			Leaf = (DecodeHuffmanTree8(bitstream, &TreeHigh) << 8) | Leaf;
			Tree->Nodes[CurNodeNum] = Leaf;

			if (Leaf == Leaf1)
			{
				if (Tree->Node1 == 0xffffffff)
				{
					Tree->Node1 = CurNodeNum;
					Tree->Nodes[CurNodeNum] = 0;
				}
			}
			if (Leaf == Leaf2)
			{
				if (Tree->Node2 == 0xffffffff)
				{
					Tree->Node2 = CurNodeNum;
					Tree->Nodes[CurNodeNum] = 0;
				}
			}
			if (Leaf == Leaf3)
			{
				if (Tree->Node3 == 0xffffffff)
				{
					Tree->Node3 = CurNodeNum;
					Tree->Nodes[CurNodeNum] = 0;
				}
			}

			if (NumRem == 0)
			{
				Bit = BitStreamReadBit(bitstream);
				return;
			}

			NumRem--;
			CurNodeNum = (Tree->Nodes[RemNodes[NumRem]] & 0x7fffffff) + 1;
		}
	}
}

static uint32_t DecodeHuffmanTree16(BitStream *bitstream, Tree16 *Tree)
{
	uint32_t CurNodeNum;

	CurNodeNum = 0;

	while (!bitstream->Empty && (Tree->Nodes[CurNodeNum] & 0x80000000) != 0)
	{
		uint32_t Bit;

		CurNodeNum = (Tree->Nodes[CurNodeNum] & 0x7fffffff);

/*		Bit = BitStreamReadBit(bitstream);*/
		BitStreamReadBitMacro(bitstream, Bit);

		CurNodeNum += Bit;
	}

	{
		uint32_t ret;

		ret = ((Tree->Nodes[CurNodeNum] & 0x80000000) == 0)?(Tree->Nodes[CurNodeNum]):(0);

		if (ret != Tree->LastDecoded)
		{
			Tree->LastDecoded3 = Tree->LastDecoded2;
			Tree->LastDecoded2 = Tree->LastDecoded;
			Tree->LastDecoded = ret;

			if (Tree->Node1 != 0xffffffff) Tree->Nodes[Tree->Node1] = ret;
			if (Tree->Node2 != 0xffffffff) Tree->Nodes[Tree->Node2] = Tree->LastDecoded2;
			if (Tree->Node3 != 0xffffffff) Tree->Nodes[Tree->Node3] = Tree->LastDecoded3;
		}

		return ret;
	}
}

static void ResetTree16(Tree16 *Tree)
{
/* reset last three remembered values */
	if (Tree->Node1 != 0xffffffff) Tree->Nodes[Tree->Node1] = 0;
	if (Tree->Node2 != 0xffffffff) Tree->Nodes[Tree->Node2] = 0;
	if (Tree->Node3 != 0xffffffff) Tree->Nodes[Tree->Node3] = 0;
	Tree->LastDecoded = 0;
	Tree->LastDecoded2 = 0;
	Tree->LastDecoded3 = 0;
}



int SmackError()
{
	return SmackErrorCode;
}

/* SmackOpen initializes a Smacker file for playback */
SmackStruct *SmackOpen(FILE *SmackFile)
{
	SmackStruct *Smack;
	struct SmackHeader {
		uint32_t Signature;
		uint32_t Width;
		uint32_t Height;
		uint32_t Frames;
		int32_t  FrameRate;
		uint32_t Flags;
		uint32_t AudioSize[7];
		uint32_t TreesSize;
		uint32_t MMap_Size;
		uint32_t MClr_Size;
		uint32_t Full_Size;
		uint32_t Type_Size;
		uint32_t AudioRate[7];
		uint32_t Dummy;
	} Header;
	uint8_t buf[1024];
	BitStream bitstream;

	SmackErrorCode = SM_OK;

	/* test endianness */
	{
		uint32_t test;

		test = 0x12345678UL;

#ifdef BIG_ENDIAN_BYTE_ORDER
		/* big endian */
		if ( *((uint8_t *) (&test)) != 0x12 )
		{
			SmackErrorCode = SM_WRONG_ENDIANNESS;
			return NULL;
		}
#else
		/* little endian */
		if ( *((uint8_t *) (&test)) != 0x78 )
		{
			SmackErrorCode = SM_WRONG_ENDIANNESS;
			return NULL;
		}
#endif
	}

	if (SmackFile == NULL)
	{
		SmackErrorCode = SM_FILE_NOT_FOUND;
		return NULL;
	}

/* read Smacker file header */
	{
		size_t ItemsRead;

		ItemsRead = fread(&Header, 1, sizeof(struct SmackHeader), SmackFile);
		if (ItemsRead != sizeof(struct SmackHeader))
		{
			SmackErrorCode = SM_FILE_READ_ERROR;
			return NULL;
		}
	}

#ifdef BIG_ENDIAN_BYTE_ORDER
	{
		/* big endian */
		int i;
		for (i = 0; i < sizeof(struct SmackHeader) / sizeof(uint32_t); i++ )
		{
			((uint32_t *) (&Header))[i] = LE2NATIVE32( ((uint32_t *) (&Header))[i] );
		}
	}
#endif
/* for now only version 2 Smacker files supported - 'SMK2' */
	if (Header.Signature != SMACKER_VER_2)
	{
		if ((Header.Signature & 0x00ffffff) == SMACKER_VER_UNK)
		{
			SmackErrorCode = SM_UNKNOWN_VERSION;
		}
		else
		{
			SmackErrorCode = SM_INVALID_FILE;
		}
		return NULL;
	}

/* invalid flags */
	if ((Header.Flags & 0xfffffff8) != 0)
	{
		SmackErrorCode = SM_INVALID_FILE;
		return NULL;
	}

/* interlaced and double flag cannot be set at the same time */
	if ( (Header.Flags & 4) && (Header.Flags & 2))
	{
		SmackErrorCode = SM_INVALID_FILE;
		return NULL;
	}

/* unsupported flags */
/* currently only doble flag supported (interlace and ring frame not supported) */
	if (Header.Flags & ~0x04)
	{
		SmackErrorCode = SM_UNKNOWN_FLAGS;
		return NULL;
	}

/* zero frames - nothing to do */
	if (Header.Frames == 0)
	{
		SmackErrorCode = SM_INVALID_FILE;
		return NULL;
	}

/* no video */
	if (Header.Width == 0 || Header.Height == 0)
	{
		SmackErrorCode = SM_INVALID_FILE;
		return NULL;
	}

/* allocate memory for SmackStruct */
	Smack = (SmackStruct *) malloc(sizeof(SmackStruct));
	if (Smack == NULL)
	{
		SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
		return NULL;
	}

/* fill SmackStruct */
	memset(Smack, 0, sizeof(SmackStruct));
	memcpy(Smack, &Header, sizeof(struct SmackHeader));
	Smack->File = SmackFile;
	Smack->PaddedWidth = Header.Width = (Header.Width + 3) & 0xfffffffc;
	Smack->PaddedHeight = Header.Height = (Header.Height + 3) & 0xfffffffc;

/* allocate frame sizes */
	Smack->FrameSizes = (uint32_t *) malloc(Header.Frames * sizeof(uint32_t));
	if (Smack->FrameSizes == NULL)
	{
		SmackClose(Smack);
		SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
		return NULL;
	}

/* allocate frame types */
	Smack->FrameTypes = (uint8_t *) malloc(Header.Frames);
	if (Smack->FrameTypes == NULL)
	{
		SmackClose(Smack);
		SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
		return NULL;
	}

/* allocate frame positions */
	Smack->FramePositions = (uint32_t *) malloc(Header.Frames * sizeof(uint32_t));
	if (Smack->FramePositions == NULL)
	{
		SmackClose(Smack);
		SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
		return NULL;
	}

/* read frame sizes */
	{
		size_t ItemsRead;

		ItemsRead = fread(Smack->FrameSizes, sizeof(uint32_t), Header.Frames, SmackFile);
		if (ItemsRead != Header.Frames)
		{
			SmackClose(Smack);
			SmackErrorCode = SM_FILE_READ_ERROR;
			return NULL;
		}
	}

/* read frame types */
	{
		size_t ItemsRead;

		ItemsRead = fread(Smack->FrameTypes, 1, Header.Frames, SmackFile);
		if (ItemsRead != Header.Frames)
		{
			SmackClose(Smack);
			SmackErrorCode = SM_FILE_READ_ERROR;
			return NULL;
		}
	}

/* calculate frame positions */
	{
		unsigned int Frame;

		Smack->FramePositions[0] = sizeof(struct SmackHeader) +
									Header.Frames * (1 + sizeof(uint32_t)) +
									Header.TreesSize;
		Smack->FrameSizes[0] = LE2NATIVE32(Smack->FrameSizes[0]) & (~3);		/* clear keyframe flag & unknown flag */
		for (Frame = 1; Frame < Header.Frames; Frame++)
		{
			Smack->FramePositions[Frame] = Smack->FramePositions[Frame - 1] +
											Smack->FrameSizes[Frame - 1];
			Smack->FrameSizes[Frame] = LE2NATIVE32(Smack->FrameSizes[Frame]) & (~3);		/* clear keyframe flag & unknown flag */
		}
	}

/* check file size */
	{
		long curpos, filesize;

		curpos = ftell(SmackFile);
		if (curpos == -1)
		{
			SmackErrorCode = SM_FILE_READ_ERROR;
			return NULL;
		}
		if ( fseek(SmackFile, 0, SEEK_END) )
		{
			SmackErrorCode = SM_FILE_READ_ERROR;
			return NULL;
		}
		filesize = ftell(SmackFile);
		if (filesize == -1)
		{
			SmackErrorCode = SM_FILE_READ_ERROR;
			return NULL;
		}

	/* file too short - corrupted ? */
		if ((unsigned long) filesize < Smack->FramePositions[Header.Frames - 1] + Smack->FrameSizes[Header.Frames - 1])
		{
			SmackClose(Smack);
			SmackErrorCode = SM_INVALID_FILE;
			return NULL;
		}
		if ( fseek(SmackFile, curpos, SEEK_SET) )
		{
			SmackErrorCode = SM_FILE_READ_ERROR;
			return NULL;
		}
	}

/* create huffman trees */
	BitStreamInitialize(&bitstream, SmackFile, buf, 1024, Header.TreesSize);

	Smack->MMap_Tree = (uint8_t *) malloc(Header.MMap_Size + sizeof(Tree16) - 12);
	if (Smack->MMap_Tree == NULL)
	{
		SmackClose(Smack);
		SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
		return NULL;
	}
	((Tree16 *) Smack->MMap_Tree)->NumNodes = (Header.MMap_Size - 12) >> 2;
	CreateHuffmanTree16(&bitstream, (Tree16 *) Smack->MMap_Tree);

	Smack->MClr_Tree = (uint8_t *) malloc(Header.MClr_Size + sizeof(Tree16) - 12);
	if (Smack->MClr_Tree == NULL)
	{
		SmackClose(Smack);
		SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
		return NULL;
	}
	((Tree16 *) Smack->MClr_Tree)->NumNodes = (Header.MClr_Size - 12) >> 2;
	CreateHuffmanTree16(&bitstream, (Tree16 *) Smack->MClr_Tree);

	Smack->Full_Tree = (uint8_t *) malloc(Header.Full_Size + sizeof(Tree16) - 12);
	if (Smack->Full_Tree == NULL)
	{
		SmackClose(Smack);
		SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
		return NULL;
	}
	((Tree16 *) Smack->Full_Tree)->NumNodes = (Header.Full_Size - 12) >> 2;
	CreateHuffmanTree16(&bitstream, (Tree16 *) Smack->Full_Tree);

	Smack->Type_Tree = (uint8_t *) malloc(Header.Type_Size + sizeof(Tree16) - 12);
	if (Smack->Type_Tree == NULL)
	{
		SmackClose(Smack);
		SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
		return NULL;
	}
	((Tree16 *) Smack->Type_Tree)->NumNodes = (Header.Type_Size - 12) >> 2;
	CreateHuffmanTree16(&bitstream, (Tree16 *) Smack->Type_Tree);


/* go to first frame */
	if ( fseek(SmackFile, Smack->FramePositions[0], SEEK_SET) )
	{
		SmackErrorCode = SM_FILE_READ_ERROR;
		return NULL;
	}

	return Smack;
}

/* SmackAllocateFrame allocates memory for storing decoded frame */
SmackFrame *SmackAllocateFrame(SmackStruct *Smack, void *AllocatedMem, uint32_t AllocatedSize, uint32_t UserDataSize, int DecodeAudio, int AudioUseAllocatedMem, int VideoUseAllocatedMem, int PaletteUseAllocatedMem)
{
	SmackFrame *Frame;

	SmackErrorCode = SM_OK;

/* allocate frame */
	Frame = (SmackFrame *) malloc(sizeof(SmackFrame) + UserDataSize);
	if (Frame == NULL)
	{
		SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
		return NULL;
	}

/* set frame data */
	memset(Frame, 0, sizeof(SmackFrame) + UserDataSize);
	Frame->UserData = (UserDataSize)?( &(((uint8_t *) Frame)[sizeof(SmackFrame)]) ):NULL;

	Frame->PaletteSize = sizeof(SmackPalette);
	Frame->VideoSize = Smack->PaddedWidth * Smack->PaddedHeight;

	if (DecodeAudio)
	{
		unsigned int AudioTrack;

		for (AudioTrack = 0; AudioTrack < 7; AudioTrack++)
		{
			if (Smack->AudioSize[AudioTrack] != 0 &&
				(Smack->AudioRate[AudioTrack] & 0x40000000) != 0) /* data present */
			{
				if (Smack->AudioSize[AudioTrack] > Frame->AudioSize)
				{
					Frame->AudioSize = Smack->AudioSize[AudioTrack];
				}
			}
		}

		Frame->AudioSize = (Frame->AudioSize + 3) & 0xfffffffc;
	}

	Frame->DecodeAudio = (DecodeAudio)?1:0;
	Frame->AudioAllocated = (AudioUseAllocatedMem || !DecodeAudio)?0:1;
	Frame->PaletteAllocated = (VideoUseAllocatedMem)?0:1;
	Frame->VideoAllocated = (VideoUseAllocatedMem)?0:1;

/* allocate palette */
	if (PaletteUseAllocatedMem)
	{
		if (AllocatedMem == NULL || AllocatedSize < Frame->PaletteSize)
		{
			SmackDeallocateFrame(Frame);
			SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
			return NULL;
		}

		Frame->Palette = (SmackPalette *) AllocatedMem;

		AllocatedMem = &(((uint8_t *) AllocatedMem)[Frame->PaletteSize]);
		AllocatedSize -= Frame->PaletteSize;
	}
	else
	{
		Frame->Palette = (SmackPalette *) malloc (Frame->PaletteSize);
		if (Frame->Palette == NULL)
		{
			SmackDeallocateFrame(Frame);
			SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
			return NULL;
		}
	}

/* allocate video */
	if (VideoUseAllocatedMem)
	{
		if (AllocatedMem == NULL || AllocatedSize < Frame->VideoSize)
		{
			SmackDeallocateFrame(Frame);
			SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
			return NULL;
		}

		Frame->Video = (uint8_t *) AllocatedMem;

		AllocatedMem = &(((uint8_t *) AllocatedMem)[Frame->VideoSize]);
		AllocatedSize -= Frame->VideoSize;
	}
	else
	{
		Frame->Video = (uint8_t *) malloc (Frame->VideoSize);
		if (Frame->Video == NULL)
		{
			SmackDeallocateFrame(Frame);
			SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
			return NULL;
		}
	}

	memset(Frame->Palette, 0, Frame->PaletteSize);
	memset(Frame->Video, 0, Frame->VideoSize);

/* allocate audio */
	if (DecodeAudio)
	{
		if (AudioUseAllocatedMem)
		{
			if (AllocatedMem == NULL || AllocatedSize < Frame->AudioSize)
			{
				SmackDeallocateFrame(Frame);
				SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
				return NULL;
			}

			Frame->Audio = (uint8_t *) AllocatedMem;

			AllocatedMem = &(((uint8_t *) AllocatedMem)[Frame->AudioSize]);
			AllocatedSize -= Frame->AudioSize;
		}
		else
		{
			Frame->Audio = (uint8_t *) malloc (Frame->AudioSize);
			if (Frame->Audio == NULL)
			{
				SmackDeallocateFrame(Frame);
				SmackErrorCode = SM_NOT_ENOUGH_MEMORY;
				return NULL;
			}
		}
	}

	return Frame;
}

/* SmackDecodeFrame processes the next frame in the Smack file. */
void SmackDecodeFrame(SmackStruct *Smack, SmackFrame *PrevFrame, SmackFrame *Frame, int DesiredAudioTrack)
{
	uint32_t vidsize;
	size_t ItemsRead;

	SmackErrorCode = SM_OK;

	if (DesiredAudioTrack >= 0)
	{
		if (DesiredAudioTrack >= 7)
		{
			SmackErrorCode = SM_INVALID_AUDIO_TRACK;
			return;
		}

		if (Smack->AudioSize[DesiredAudioTrack] == 0 ||
			(Smack->AudioRate[DesiredAudioTrack] & 0x40000000) == 0) /* data not present */
		{
			SmackErrorCode = SM_INVALID_AUDIO_TRACK;
			return;
		}
	}

	vidsize = Smack->FrameSizes[Smack->CurrentFrame];

	if (vidsize == 0)
	{
		SmackErrorCode = SM_INVALID_FILE;
		return;
	}

/* decode palette */
	if ((Smack->FrameTypes[Smack->CurrentFrame] & 1) != 0)
	{
		unsigned int palsize, palptr, curcolor, count, start, count2;
		SmackPalette Palette;
		uint8_t buffer[1020];

		ItemsRead = fread(&buffer, 1, 1, Smack->File);
		if (ItemsRead != 1)
		{
			SmackErrorCode = SM_FILE_READ_ERROR;
			return;
		}
		if (buffer[0] == 0)
		{
			SmackErrorCode = SM_INVALID_FILE;
			return;
		}

		palsize = (buffer[0] << 2) - 1;
		if (vidsize < palsize + 1)
		{
			SmackErrorCode = SM_INVALID_FILE;
			return;
		}
		vidsize = (vidsize - palsize) - 1;

		ItemsRead = fread(&buffer, 1, palsize, Smack->File);
		if (ItemsRead != palsize)
		{
			SmackErrorCode = SM_FILE_READ_ERROR;
			return;
		}

		palptr = 0;
		curcolor = 0;
		while (palptr < palsize)
		{
			if ((buffer[palptr] & 0x80) != 0)
			{
				count = (buffer[palptr] & 0x7f) + 1;
				if (count > 256 - curcolor) count = 256 - curcolor;

				memcpy(&(Palette[curcolor]), &((*(PrevFrame->Palette))[curcolor]), count * sizeof(SmackColor));

				curcolor += count;
				palptr++;
			}
			else if ((buffer[palptr] & 0x40) != 0)
			{
				if (palptr + 1 < palsize)
				{
					count = (buffer[palptr] & 0x3f) + 1;
					if (count > 256 - curcolor) count = 256 - curcolor;

					count2 = count;

					start = buffer[palptr + 1];
					if (count2 > 256 - start) count2 = 256 - start;

					memcpy(&(Palette[curcolor]), &((*(PrevFrame->Palette))[start]), count2 * sizeof(SmackColor));

					curcolor += count;
				}
				palptr += 2;
			}
			else
			{
				if (palptr + 2 < palsize)
				{
					if (curcolor < 256)
					{
						Palette[curcolor].r = palmap[buffer[palptr]];				/* r */
						Palette[curcolor].g = palmap[buffer[palptr + 1] & 0x3f];	/* g */
						Palette[curcolor].b = palmap[buffer[palptr + 2] & 0x3f];	/* b */
						curcolor++;
					}
				}
				palptr += 3;
			}
		}

		if (curcolor < 256)
		{
			memcpy(&(Palette[curcolor]), &((*(PrevFrame->Palette))[curcolor]), (256 - curcolor) * sizeof(SmackColor));
		}

		memcpy(Frame->Palette, Palette, sizeof(SmackPalette));
	}
	else
	{
		if (Frame != PrevFrame)
		{
			memcpy(Frame->Palette, PrevFrame->Palette, sizeof(SmackPalette));
		}
	}

/* decode audio */
	{
		int AudioTrack;
		uint32_t audsize, uaudsize;
		uint32_t DataPresent, IsStereo, Is16Bits;
		uint8_t *AudioBuffer;

		uint32_t Base[4];
		BitStream bitstream;
		uint8_t bitstreamBuffer[1024];
		Tree8 Tree[4];


		Frame->AudioLength = 0;

		for (AudioTrack = 0; AudioTrack < 7; AudioTrack++)
			if ((Smack->FrameTypes[Smack->CurrentFrame] & (2 << AudioTrack)) != 0)
		{
			ItemsRead = fread(&audsize, sizeof(uint32_t), 1, Smack->File);
			if (ItemsRead != 1)
			{
				SmackErrorCode = SM_FILE_READ_ERROR;
				return;
			}
			audsize = LE2NATIVE32(audsize);
			if (audsize < 4)
			{
				SmackErrorCode = SM_INVALID_FILE;
				return;
			}

			if (vidsize < audsize)
			{
				SmackErrorCode = SM_INVALID_FILE;
				return;
			}
			vidsize = vidsize - audsize;

			if (AudioTrack == DesiredAudioTrack &&
				Frame->DecodeAudio)
			{
				AudioBuffer = Frame->Audio;

				if ((Smack->AudioRate[AudioTrack] & 0x80000000) != 0)
				{
					/* compressed audio */

					if (audsize < 8)
					{
						SmackErrorCode = SM_INVALID_FILE;
						return;
					}
					ItemsRead = fread(&uaudsize, sizeof(uint32_t), 1, Smack->File);
					if (ItemsRead != 1)
					{
						SmackErrorCode = SM_FILE_READ_ERROR;
						return;
					}
					uaudsize = LE2NATIVE32(uaudsize);

					if ((Smack->AudioRate[AudioTrack] & 0x0c000000) == 0 &&
						audsize > 8)
					{
						/* Smacker ver2 audio */

						if (uaudsize > Frame->AudioSize)
						{
							SmackErrorCode = SM_INVALID_FILE;
							return;
						}

						BitStreamInitialize(&bitstream, Smack->File, &(bitstreamBuffer[0]), 1024, audsize - 8);

						DataPresent = BitStreamReadBit(&bitstream);

						if ( DataPresent )
						{
							IsStereo = BitStreamReadBit(&bitstream);
							Is16Bits = BitStreamReadBit(&bitstream);

							if ( (IsStereo == 0) != ((Smack->AudioRate[AudioTrack] & 0x10000000) == 0) )
							{
								SmackErrorCode = SM_INVALID_FILE;
								return;
							}

							if ( (Is16Bits == 0) != ((Smack->AudioRate[AudioTrack] & 0x20000000) == 0) )
							{
								SmackErrorCode = SM_INVALID_FILE;
								return;
							}

							Frame->AudioLength = uaudsize;

							if (Is16Bits == 0)
							{
								if (IsStereo == 0)
								{
									/* 8-bit mono */

									Tree[0].NumNodes = 511;
									CreateHuffmanTree8(&bitstream, &(Tree[0]));

									Base[0] = BitStreamReadBits8(&bitstream);

									/* write base bytes */
									{
										*AudioBuffer = Base[0];
										AudioBuffer++;
										uaudsize--;
									}

									while (!bitstream.Empty && uaudsize != 0)
									{
										Base[0] += DecodeHuffmanTree8(&bitstream, &(Tree[0])); /* Base[0] += Delta[0]; */

										*AudioBuffer = Base[0];
										AudioBuffer++;
										uaudsize--;
									}
								}
								else
								{
									/* 8-bit stereo */

									Tree[0].NumNodes = 511;
									CreateHuffmanTree8(&bitstream, &(Tree[0]));
									Tree[1].NumNodes = 511;
									CreateHuffmanTree8(&bitstream, &(Tree[1]));

									Base[1] = BitStreamReadBits8(&bitstream);
									Base[0] = BitStreamReadBits8(&bitstream);

									/* write base bytes */
									{
										if (uaudsize >= 2)
										{
											uaudsize -= 2;
											AudioBuffer[0] = Base[0];
											AudioBuffer[1] = Base[1];
											AudioBuffer += 2;
										}
										else
										{
											uaudsize = 0;
										}
									}

									while (!bitstream.Empty && uaudsize != 0)
									{
										Base[0] += DecodeHuffmanTree8(&bitstream, &(Tree[0])); /* Base[0] += Delta[0]; */
										Base[1] += DecodeHuffmanTree8(&bitstream, &(Tree[1])); /* Base[1] += Delta[1]; */

										if (uaudsize >= 2)
										{
											uaudsize -= 2;
											AudioBuffer[0] = Base[0];
											AudioBuffer[1] = Base[1];
											AudioBuffer += 2;
										}
										else
										{
											uaudsize = 0;
										}
									}
								}
							}
							else
							{
								if (IsStereo == 0)
								{
									/* 16-bit mono */

									Tree[0].NumNodes = 511;
									CreateHuffmanTree8(&bitstream, &(Tree[0]));
									Tree[1].NumNodes = 511;
									CreateHuffmanTree8(&bitstream, &(Tree[1]));

									Base[1] = BitStreamReadBits8(&bitstream);
									Base[0] = BitStreamReadBits8(&bitstream);

									/* write base bytes */
									{
										if (uaudsize >= 2)
										{
											uaudsize -= 2;
											AudioBuffer[0] = Base[0];
											AudioBuffer[1] = Base[1];
											AudioBuffer += 2;
										}
										else
										{
											uaudsize = 0;
										}
									}

									while (!bitstream.Empty && uaudsize != 0)
									{

										Base[0] += DecodeHuffmanTree8(&bitstream, &(Tree[0])); /* Base[0] += Delta[0]; */
										Base[1] += DecodeHuffmanTree8(&bitstream, &(Tree[1])) + (Base[0] >> 8); /* Base[1] += Delta[1] + (Base[0] >> 8); */
										Base[0] &= 0x00ff;

										if (uaudsize >= 2)
										{
											uaudsize -= 2;
											AudioBuffer[0] = Base[0];
											AudioBuffer[1] = Base[1];
											AudioBuffer += 2;
										}
										else
										{
											uaudsize = 0;
										}
									}
								}
								else
								{
									/* 16-bit stereo */

									Tree[0].NumNodes = 511;
									CreateHuffmanTree8(&bitstream, &(Tree[0]));
									Tree[1].NumNodes = 511;
									CreateHuffmanTree8(&bitstream, &(Tree[1]));
									Tree[2].NumNodes = 511;
									CreateHuffmanTree8(&bitstream, &(Tree[2]));
									Tree[3].NumNodes = 511;
									CreateHuffmanTree8(&bitstream, &(Tree[3]));

									Base[3] = BitStreamReadBits8(&bitstream);
									Base[2] = BitStreamReadBits8(&bitstream);
									Base[1] = BitStreamReadBits8(&bitstream);
									Base[0] = BitStreamReadBits8(&bitstream);

									/* write base bytes */
									{
										if (uaudsize >= 4)
										{
											uaudsize -= 4;
											AudioBuffer[0] = Base[0];
											AudioBuffer[1] = Base[1];
											AudioBuffer[2] = Base[2];
											AudioBuffer[3] = Base[3];
											AudioBuffer += 4;
										}
										else
										{
											uaudsize = 0;
										}
									}

									while (!bitstream.Empty && uaudsize != 0)
									{
										Base[0] += DecodeHuffmanTree8(&bitstream, &(Tree[0])); /* Base[0] += Delta[0]; */
										Base[1] += DecodeHuffmanTree8(&bitstream, &(Tree[1])) + (Base[0] >> 8); /* Base[1] += Delta[1] + (Base[0] >> 8); */
										Base[0] &= 0x00ff;
										Base[2] += DecodeHuffmanTree8(&bitstream, &(Tree[2])); /* Base[2] += Delta[2]; */
										Base[3] += DecodeHuffmanTree8(&bitstream, &(Tree[3])) + (Base[2] >> 8); /* Base[3] += Delta[3] + (Base[2] >> 8); */
										Base[2] &= 0x00ff;

										if (uaudsize >= 4)
										{
											uaudsize -= 4;
											AudioBuffer[0] = Base[0];
											AudioBuffer[1] = Base[1];
											AudioBuffer[2] = Base[2];
											AudioBuffer[3] = Base[3];
											AudioBuffer += 4;
										}
										else
										{
											uaudsize = 0;
										}
									}
								}
							}

/*							if (uaudsize)
							{
								memset(AudioBuffer, 0x80, uaudsize);
							}*/

						}
						fseek(Smack->File, Smack->FramePositions[Smack->CurrentFrame] + Smack->FrameSizes[Smack->CurrentFrame] - vidsize, SEEK_SET);
					}
					else
					{
						/* skip audio */
						fseek(Smack->File, audsize - 8, SEEK_CUR);
					}
				}
				else
				{
					/* uncompressed audio */

					uaudsize = audsize - 4;

					if (uaudsize > Frame->AudioSize)
					{
						SmackErrorCode = SM_INVALID_FILE;
						return;
					}

					ItemsRead = fread(AudioBuffer, 1, uaudsize, Smack->File);
					if (ItemsRead != uaudsize)
					{
						SmackErrorCode = SM_FILE_READ_ERROR;
						return;
					}

					Frame->AudioLength = uaudsize;
				}
			}
			else
			{
/* skip audio */
				if (fseek(Smack->File, audsize - 4, SEEK_CUR) )
				{
					SmackErrorCode = SM_FILE_READ_ERROR;
					return;
				}
			}
		}
	}

/* decode video */
	if (vidsize == 0)
	{
		SmackErrorCode = SM_INVALID_FILE;
		return;
	}

	{
		Tree16 *MMap_Tree, *MClr_Tree, *Full_Tree, *Type_Tree;
		uint8_t *FrameBuffer, *PrevFrameBuffer;
		BitStream bitstream;
		uint8_t bitstreamBuffer[8192];
		uint32_t BlocksRemaining, DstAddress, Line1, Line2, Line3, Line4, BlocksX, CurBlockX;
		uint32_t TypeDescriptor, ChainLength;
		uint32_t ColorMap, Colors2[2];
		uint32_t Color0, Color1, Color2, Color3;

		MMap_Tree = (Tree16 *) Smack->MMap_Tree;
		MClr_Tree = (Tree16 *) Smack->MClr_Tree;
		Full_Tree = (Tree16 *) Smack->Full_Tree;
		Type_Tree = (Tree16 *) Smack->Type_Tree;
		FrameBuffer = Frame->Video;
		PrevFrameBuffer = PrevFrame->Video;

		ResetTree16(MMap_Tree);
		ResetTree16(MClr_Tree);
		ResetTree16(Full_Tree);
		ResetTree16(Type_Tree);

		BlocksRemaining = (Smack->PaddedHeight * Smack->PaddedWidth) >> 4;
		CurBlockX = BlocksX = Smack->PaddedWidth >> 2;
		DstAddress = 0;
		Line1 = Smack->PaddedWidth;
		Line2 = Line1 << 1;
		Line3 = Line1 + Line2;
		Line4 = Line2 << 1;

		BitStreamInitialize(&bitstream, Smack->File, &(bitstreamBuffer[0]), 8192, vidsize);

		while (!bitstream.Empty && BlocksRemaining > 0)
		{
			TypeDescriptor = DecodeHuffmanTree16(&bitstream, Type_Tree);
			ChainLength = sizetable[(TypeDescriptor >> 2) & 0x3f];
			if (ChainLength > BlocksRemaining) ChainLength = BlocksRemaining;
			BlocksRemaining -= ChainLength;

			if ((TypeDescriptor & 0x03) == 0)
			{
/* Mono Block */
				for (; ChainLength != 0; ChainLength--)
				{
					Color0 = DecodeHuffmanTree16(&bitstream, MClr_Tree);
					Colors2[1] = Color0 >> 8;
					Colors2[0] = Color0 & 0x00ff;
					ColorMap = DecodeHuffmanTree16(&bitstream, MMap_Tree);

					Color0 = (Colors2[ColorMap & 1]) |
							 (Colors2[(ColorMap >> 1) & 1] << 8) |
							 (Colors2[(ColorMap >> 2) & 1] << 16) |
							 (Colors2[(ColorMap >> 3) & 1] << 24);

					Color1 = (Colors2[(ColorMap >> 4) & 1]) |
							 (Colors2[(ColorMap >> 5) & 1] << 8) |
							 (Colors2[(ColorMap >> 6) & 1] << 16) |
							 (Colors2[(ColorMap >> 7) & 1] << 24);

					Color2 = (Colors2[(ColorMap >> 8) & 1]) |
							 (Colors2[(ColorMap >> 9) & 1] << 8) |
							 (Colors2[(ColorMap >> 10) & 1] << 16) |
							 (Colors2[(ColorMap >> 11) & 1] << 24);

					Color3 = (Colors2[(ColorMap >> 12) & 1]) |
							 (Colors2[(ColorMap >> 13) & 1] << 8) |
							 (Colors2[(ColorMap >> 14) & 1] << 16) |
							 (Colors2[(ColorMap >> 15) & 1] << 24);

					*((uint32_t *)(&(FrameBuffer[DstAddress]))) = LE2NATIVE32(Color0);
					*((uint32_t *)(&(FrameBuffer[DstAddress + Line1]))) = LE2NATIVE32(Color1);
					*((uint32_t *)(&(FrameBuffer[DstAddress + Line2]))) = LE2NATIVE32(Color2);
					*((uint32_t *)(&(FrameBuffer[DstAddress + Line3]))) = LE2NATIVE32(Color3);

					CurBlockX--;
					if (CurBlockX == 0)
					{
						CurBlockX = BlocksX;
						DstAddress += Line3 + 4;
					}
					else
					{
						DstAddress += 4;
					}
				}
			}
			else if ((TypeDescriptor & 0x03) == 1)
			{
/* Full Block */
				for (; ChainLength != 0; ChainLength--)
				{
					Color0 = DecodeHuffmanTree16(&bitstream, Full_Tree) << 16;
					Color0 = Color0 | DecodeHuffmanTree16(&bitstream, Full_Tree);

					Color1 = DecodeHuffmanTree16(&bitstream, Full_Tree) << 16;
					Color1 = Color1 | DecodeHuffmanTree16(&bitstream, Full_Tree);

					Color2 = DecodeHuffmanTree16(&bitstream, Full_Tree) << 16;
					Color2 = Color2 | DecodeHuffmanTree16(&bitstream, Full_Tree);

					Color3 = DecodeHuffmanTree16(&bitstream, Full_Tree) << 16;
					Color3 = Color3 | DecodeHuffmanTree16(&bitstream, Full_Tree);

					*((uint32_t *)(&(FrameBuffer[DstAddress]))) = LE2NATIVE32(Color0);
					*((uint32_t *)(&(FrameBuffer[DstAddress + Line1]))) = LE2NATIVE32(Color1);
					*((uint32_t *)(&(FrameBuffer[DstAddress + Line2]))) = LE2NATIVE32(Color2);
					*((uint32_t *)(&(FrameBuffer[DstAddress + Line3]))) = LE2NATIVE32(Color3);

					CurBlockX--;
					if (CurBlockX == 0)
					{
						CurBlockX = BlocksX;
						DstAddress += Line3 + 4;
					}
					else
					{
						DstAddress += 4;
					}
				}
			}
			else if ((TypeDescriptor & 0x03) == 2)
			{
/* Void Block */
				if (ChainLength >= CurBlockX)
				{
					uint32_t Lines;

					if (PrevFrameBuffer != FrameBuffer)
					{
						memcpy(&(FrameBuffer[DstAddress]), &(PrevFrameBuffer[DstAddress]), CurBlockX << 2);
						memcpy(&(FrameBuffer[DstAddress + Line1]), &(PrevFrameBuffer[DstAddress + Line1]), CurBlockX << 2);
						memcpy(&(FrameBuffer[DstAddress + Line2]), &(PrevFrameBuffer[DstAddress + Line2]), CurBlockX << 2);
						memcpy(&(FrameBuffer[DstAddress + Line3]), &(PrevFrameBuffer[DstAddress + Line3]), CurBlockX << 2);
					}

					ChainLength -= CurBlockX;
					DstAddress += (CurBlockX << 2) + Line3;
					CurBlockX = BlocksX;

					Lines = ChainLength / BlocksX;
					ChainLength -= BlocksX * Lines;

					if (PrevFrameBuffer != FrameBuffer)
					{
						memcpy(&(FrameBuffer[DstAddress]), &(PrevFrameBuffer[DstAddress]), (BlocksX * Lines) << 4);
					}

					DstAddress += Line4 * Lines;
				}

				if (PrevFrameBuffer != FrameBuffer)
				{
					memcpy(&(FrameBuffer[DstAddress]), &(PrevFrameBuffer[DstAddress]), ChainLength << 2);
					memcpy(&(FrameBuffer[DstAddress + Line1]), &(PrevFrameBuffer[DstAddress + Line1]), ChainLength << 2);
					memcpy(&(FrameBuffer[DstAddress + Line2]), &(PrevFrameBuffer[DstAddress + Line2]), ChainLength << 2);
					memcpy(&(FrameBuffer[DstAddress + Line3]), &(PrevFrameBuffer[DstAddress + Line3]), ChainLength << 2);
				}

				DstAddress += (ChainLength << 2);
				CurBlockX -= ChainLength;

/*
				for (; ChainLength != 0; ChainLength--)
				{
					CurBlockX--;
					if (CurBlockX == 0)
					{
						CurBlockX = BlocksX;
						DstAddress += Line3 + 4;
					}
					else
					{
						DstAddress += 4;
					}
				}
*/
			}
			else
			{
/* Solid Block */
				Color0 = TypeDescriptor >> 8;

				if (ChainLength >= CurBlockX)
				{
					uint32_t Lines;

					memset(&(FrameBuffer[DstAddress]), Color0, CurBlockX << 2);
					memset(&(FrameBuffer[DstAddress + Line1]), Color0, CurBlockX << 2);
					memset(&(FrameBuffer[DstAddress + Line2]), Color0, CurBlockX << 2);
					memset(&(FrameBuffer[DstAddress + Line3]), Color0, CurBlockX << 2);

					ChainLength -= CurBlockX;
					DstAddress += (CurBlockX << 2) + Line3;
					CurBlockX = BlocksX;

					Lines = ChainLength / BlocksX;
					ChainLength -= BlocksX * Lines;
					memset(&(FrameBuffer[DstAddress]), Color0, (BlocksX * Lines) << 4);
					DstAddress += Line4 * Lines;
				}

				memset(&(FrameBuffer[DstAddress]), Color0, ChainLength << 2);
				memset(&(FrameBuffer[DstAddress + Line1]), Color0, ChainLength << 2);
				memset(&(FrameBuffer[DstAddress + Line2]), Color0, ChainLength << 2);
				memset(&(FrameBuffer[DstAddress + Line3]), Color0, ChainLength << 2);

				DstAddress += (ChainLength << 2);
				CurBlockX -= ChainLength;

/*
				Color0 = TypeDescriptor & 0x0000ff00;
				Color0 = (Color0 >> 8) | Color0 | (Color0 << 8) | (Color0 << 16);

				for (; ChainLength != 0; ChainLength--)
				{
					*((uint32_t *)(&(FrameBuffer[DstAddress]))) = Color0;
					*((uint32_t *)(&(FrameBuffer[DstAddress + Line1]))) = Color0;
					*((uint32_t *)(&(FrameBuffer[DstAddress + Line2]))) = Color0;
					*((uint32_t *)(&(FrameBuffer[DstAddress + Line3]))) = Color0;

					CurBlockX--;
					if (CurBlockX == 0)
					{
						CurBlockX = BlocksX;
						DstAddress += Line3 + 4;
					}
					else
					{
						DstAddress += 4;
					}
				}
*/
			}
		}
	}
}

/* SmackNextFrame signals the playback engine to advance the Smacker file
   to the next frame.
*/
int SmackNextFrame(SmackStruct *Smack)
{
	int ret;

	SmackErrorCode = SM_OK;

	if (Smack->CurrentFrame + 1 < Smack->Frames)
	{
		ret = 1;
		Smack->CurrentFrame++;
	}
	else
	{
		ret = 0;
		Smack->CurrentFrame = 0;
	}

	if ( fseek(Smack->File, Smack->FramePositions[Smack->CurrentFrame], SEEK_SET) )
	{
		SmackErrorCode = SM_FILE_READ_ERROR;
		return 0;
	}

	return ret;
}

/* SmackDeallocateFrame deallocates memory for storing decoded frame */
void SmackDeallocateFrame(SmackFrame *Frame)
{
	SmackErrorCode = SM_OK;

	if (Frame == NULL) return;

/* free palette */
	if (Frame->Palette != NULL && Frame->PaletteAllocated) free(Frame->Palette);

/* free audio */
	if (Frame->Audio != NULL && Frame->AudioAllocated) free(Frame->Audio);

/* free video */
	if (Frame->Video != NULL && Frame->VideoAllocated) free(Frame->Video);

/* free SmackFrame */
	free(Frame);
}

/* SmackClose releases any allocated resources. */
void SmackClose(SmackStruct *Smack)
{
	SmackErrorCode = SM_OK;

	if (Smack == NULL) return;

/* free frame sizes */
	if (Smack->FrameSizes != NULL) free(Smack->FrameSizes);

/* free frame types */
	if (Smack->FrameTypes != NULL) free(Smack->FrameTypes);

/* free frame positions */
	if (Smack->FramePositions != NULL) free(Smack->FramePositions);

/* free huffman trees */
	if (Smack->MMap_Tree != NULL) free(Smack->MMap_Tree);
	if (Smack->MClr_Tree != NULL) free(Smack->MClr_Tree);
	if (Smack->Full_Tree != NULL) free(Smack->Full_Tree);
	if (Smack->Type_Tree != NULL) free(Smack->Type_Tree);

/* free SmackStruct */
	free(Smack);
}
