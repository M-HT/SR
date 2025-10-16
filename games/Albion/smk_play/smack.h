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

#if !defined(_SMACK_H_INCLUDED_)
#define _SMACK_H_INCLUDED_

#include <stdio.h>
#include <stdint.h>

#define SMACKER_VER_UNK 0x004b4d53
#define SMACKER_VER_2 0x324b4d53
#define SMACKER_VER_4 0x344b4d53

#define SM_OK					0
#define SM_WRONG_ENDIANNESS		(-1)
#define SM_FILE_NOT_FOUND		(-2)
#define SM_FILE_READ_ERROR		(-3)
#define SM_UNKNOWN_VERSION		(-4)
#define SM_INVALID_FILE			(-5)
#define SM_UNKNOWN_FLAGS		(-6)
#define SM_NOT_ENOUGH_MEMORY	(-7)
#define SM_INVALID_AUDIO_TRACK	(-8)

#pragma pack(4)

typedef struct _SmackStruct_ {
	uint32_t Signature;
	uint32_t OriginalWidth;
	uint32_t OriginalHeight;
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
	FILE     *File;
	uint32_t PaddedWidth;
	uint32_t PaddedHeight;
	uint32_t CurrentFrame;
	uint32_t *FrameSizes;
	uint8_t  *FrameTypes;
	uint32_t *FramePositions;
	uint8_t  *MMap_Tree;
	uint8_t  *MClr_Tree;
	uint8_t  *Full_Tree;
	uint8_t  *Type_Tree;
} SmackStruct;


typedef struct _SmackColor_ {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} SmackColor;

typedef SmackColor SmackPalette[256];

typedef struct _SmackFrame_ {
	SmackPalette *Palette;		/* pointer to palette */
	uint8_t *Audio;				/* pointer to audio data */
	uint8_t *Video;				/* pointer to video data */
	uint32_t PaletteSize;		/* allocated palette size */
	uint32_t AudioSize;			/* allocated audio size */
	uint32_t VideoSize;			/* allocated video size */
	uint32_t AudioLength;		/* decoded audio length */
	uint8_t DecodeAudio;		/* decode audio data ? */
	uint8_t PaletteAllocated;	/* palette allocated from heap */
	uint8_t AudioAllocated;		/* audio data allocated from heap */
	uint8_t VideoAllocated;		/* video data allocated from heap */
	void *UserData;				/* pointer to user data */
} SmackFrame;

#pragma pack()

/* status of last function call */
int SmackError(void);

/* SmackOpen initializes a Smacker file for playback */
SmackStruct *SmackOpen(FILE *SmackFile);

/* SmackAllocateFrame allocates memory for storing decoded frame */
SmackFrame *SmackAllocateFrame(SmackStruct *Smack, void *AllocatedMem, uint32_t AllocatedSize, uint32_t UserDataSize, int DecodeAudio, int AudioUseAllocatedMem, int VideoUseAllocatedMem, int PaletteUseAllocatedMem);

/* SmackDecodeFrame processes the next frame in the Smack file. */
void SmackDecodeFrame(SmackStruct *Smack, SmackFrame *PrevFrame, SmackFrame *Frame, int DesiredAudioTrack);

/* SmackNextFrame signals the playback engine to advance the Smacker file
   to the next frame.
*/
int SmackNextFrame(SmackStruct *Smack);

/* SmackDeallocateFrame deallocates memory for storing decoded frame */
void SmackDeallocateFrame(SmackFrame *Frame);

/* SmackClose releases any allocated resources. */
void SmackClose(SmackStruct *Smack);


#endif
