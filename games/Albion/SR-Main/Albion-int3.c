/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-int3.h"
#include "Game_thread.h"
#include "Game-int3.h"

#pragma pack(1)

typedef struct __attribute__ ((__packed__)) _VbeInfoBlock_
{
    uint8_t  VbeSignature[4];
    uint16_t VbeVersion;
    uint32_t OemStringPtr;
    uint8_t  Capabilities[4];
    uint32_t VideoModePtr;
    uint16_t TotalMemory;
    uint16_t OemSoftwareRev;
    uint32_t OemVendorNamePtr;
    uint32_t OemProductNamePtr;
    uint32_t OemProductRevPtr;
    uint8_t  Reserved[222];
    uint8_t  OemData[256];
} VbeInfoBlock;

typedef struct __attribute__ ((__packed__)) _ModeInfoBlock_
{
// Mandatory information for all VBE revisions
    uint16_t ModeAttributes;
    uint8_t  WinAAttributes;
    uint8_t  WinBAttributes;
    uint16_t WinGranularity;
    uint16_t WinSize;
    uint16_t WinASegment;
    uint16_t WinBSegment;
    uint32_t WinFuncPtr;
    uint16_t BytesPerScanLine;

// Mandatory information for VBE 1.2 and above
    uint16_t XResolution;
    uint16_t YResolution;
    uint8_t  XCharSize;
    uint8_t  YCharSize;
    uint8_t  NumberOfPlanes;
    uint8_t  BitsPerPixel;
    uint8_t  NumberOfBanks;
    uint8_t  MemoryModel;
    uint8_t  BankSize;
    uint8_t  NumberOfImagePages;
    uint8_t  Reserved;

// Direct Color fields (required for direct/6 and YUV/7 memory models)
    uint8_t  RedMaskSize;
    uint8_t  RedFieldPosition;
    uint8_t  GreenMaskSize;
    uint8_t  GreenFieldPosition;
    uint8_t  BlueMaskSize;
    uint8_t  BlueFieldPosition;
    uint8_t  RsvdMaskSize;
    uint8_t  RsvdFieldPosition;
    uint8_t  DirectColorModeInfo;

// Mandatory information for VBE 2.0 and above
    uint32_t PhysBasePtr;
    uint32_t OffScreenMemOffset;
    uint16_t OffScreenMemSize;
    uint8_t  Reserved2[206];
} ModeInfoBlock;

#pragma pack()

void Game_intDPMI(
    const uint32_t IntNum,
    Game_DPMIREGS *regs)
{
    switch (IntNum)
    {
        case 0x10:
        // Video
            switch (AX)
            {
                case 0x4f00:
                // VESA: Return VBE Controller Information
#if defined(__DEBUG__)
                    fprintf(stderr, "Returning VBE Controller Information\n");
#endif
                    EAX = 0x004f;	// VESA OK
                    // fill structure - not needed
                    {
                        VbeInfoBlock *VIB;

                        VIB = (VbeInfoBlock *)(uintptr_t) ESI;

                        VIB->VbeSignature[0] = 'V';
                        VIB->VbeSignature[1] = 'E';
                        VIB->VbeSignature[2] = 'S';
                        VIB->VbeSignature[3] = 'A';
                        VIB->VbeVersion = 0x0101;		// VESA version 1.1
                        VIB->OemStringPtr = 0;
                        VIB->Capabilities[0] = 0;
                        VIB->Capabilities[1] = 0;
                        VIB->Capabilities[2] = 0;
                        VIB->Capabilities[3] = 0;
                        VIB->VideoModePtr = 0;
                        VIB->TotalMemory = 3;
                    }

                    return;
                    // case 0x4f00:
                case 0x4f01:
                // VESA: Return VBE mode information
#if defined(__DEBUG__)
                    fprintf(stderr, "Returning VBE mode information: 0x%x\n", CX);
#endif

                    EAX = 0x004f;	// VESA OK
                    // fill structure

                    if (CX == 0x157)
                    {
                        ModeInfoBlock *MIB;

                        MIB = (ModeInfoBlock *)(uintptr_t) ESI;

                        MIB->ModeAttributes = 0x19;
                        MIB->WinAAttributes = 0x07;
                        MIB->WinBAttributes = 0;
                        MIB->WinGranularity = /*96*/ /*169*/ 64;		// Window granularity in KB
                        MIB->WinSize = /*96*/ /*169*/ 64;				// Window size in KB
                        MIB->WinASegment = 0xA000;
                        MIB->WinBSegment = 0x0000;
                        MIB->WinFuncPtr = 0;
                        MIB->BytesPerScanLine = 360;

                        MIB->XResolution = 360;
                        MIB->YResolution = 240;
                        MIB->XCharSize = 9;
                        MIB->YCharSize = 16;
                        MIB->NumberOfPlanes = 1;
                        MIB->BitsPerPixel = 8;
                        MIB->NumberOfBanks = 3;
                        MIB->MemoryModel = 5;
                        MIB->BankSize = 64;
                        MIB->NumberOfImagePages = 1;
                        MIB->Reserved = 0;

                    } else {
                        Game_StopMain();
                    }

                    return;
                    // case 0x4f01:
                //default:
            } // switch (AX)

            break;
            // case 0x10:
        //default:
    } // switch (int_number)

#if defined(__DEBUG__)
    fprintf(stderr, "int3 %xh\n", IntNum);
    fprintf(stderr, "eax 0x%x\n", EAX);
    fprintf(stderr, "ah 0x%x\n", AH);
#endif

    Game_StopMain();
} // void Game_intDPMI(...)
