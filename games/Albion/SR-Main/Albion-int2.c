/**
 *
 *  Copyright (C) 2016-2026 Roman Pauer
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
#include <string.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-int2.h"
#include "Albion-int3.h"
#include "Game_thread.h"
#include "Game-int2.h"

uint32_t Game_int386x(
    const uint32_t IntNum,
    const Game_REGS *in_regs,
    Game_REGS *out_regs,
    Game_SREGS *seg_regs)
{
    Game_REGS tmp_regs;
    SDL_Event event;

    // just in case *in_regs is not properly aligned
    memcpy(&tmp_regs, (void *)in_regs, sizeof(Game_REGS));
	//tmp_regs = *in_regs;

#if defined(__DEBUG__)
    fprintf(stderr, "int2 %xh\n", IntNum);
    fprintf(stderr, "eax 0x%x\n", EAX);
    fprintf(stderr, "ebx 0x%x\n", EBX);
    fprintf(stderr, "ecx 0x%x\n", ECX);
    fprintf(stderr, "edx 0x%x\n", EDX);
    fprintf(stderr, "esi 0x%x\n", ESI);
    fprintf(stderr, "edi 0x%x\n", EDI);
    fprintf(stderr, "ah 0x%x\n", AH);
#endif

    switch (IntNum)
    {
        case 0x10:
        // Video
            switch (AX)
            {
                case 0x0003:
                // Set Text Mode
#if defined(__DEBUG__)
                    fprintf(stderr, "Setting text mode\n");
#endif
                #if SDL_VERSION_ATLEAST(2,0,0)
                    if (Game_Window != NULL)
                #else
                    if (Game_Screen != NULL)
                #endif
                    {
                        event.type = SDL_USEREVENT;
                        event.user.code = EC_DISPLAY_DESTROY;
                        event.user.data1 = NULL;
                        event.user.data2 = NULL;

                        SDL_PushEvent(&event);

                        SDL_SemWait(Game_DisplaySem);
                    }

                    break;
                    // case 0x0003:
                case 0x4f02:
                // VESA: Set VBE Mode
#if defined(__DEBUG__)
                    fprintf(stderr, "Setting video mode: 0x%x\n", BX);
                    if (BX == 0x0157)
                    {
                        fprintf(stderr, "360x240x256\n");
                    }
#endif
                    if (BX == 0x0157)
                    {
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        if (Game_Window != NULL)
                    #else
                        if (Game_Screen != NULL)
                    #endif
                        {
                            event.type = SDL_USEREVENT;
                            event.user.code = EC_DISPLAY_DESTROY;
                            event.user.data1 = NULL;
                            event.user.data2 = NULL;

                            SDL_PushEvent(&event);

                            SDL_SemWait(Game_DisplaySem);
                        }

                        event.type = SDL_USEREVENT;
                        event.user.code = EC_DISPLAY_CREATE;
                        event.user.data1 = NULL;
                        event.user.data2 = NULL;

                        SDL_PushEvent(&event);

                        SDL_SemWait(Game_DisplaySem);

                    #if SDL_VERSION_ATLEAST(2,0,0)
                        if (Game_Window == NULL)
                    #else
                        if (Game_Screen == NULL)
                    #endif
                        {
#if defined(__DEBUG__)
                            fprintf (stderr, "Error: Couldn't set video mode\n");
#endif
                            Game_StopMain();
                        }

                        EAX = 0x004f;	// VESA OK
                    }
                    else if (BX == 3)
                    {
                    #if SDL_VERSION_ATLEAST(2,0,0)
                        if (Game_Window != NULL)
                    #else
                        if (Game_Screen != NULL)
                    #endif
                        {
                            event.type = SDL_USEREVENT;
                            event.user.code = EC_DISPLAY_DESTROY;
                            event.user.data1 = NULL;
                            event.user.data2 = NULL;

                            SDL_PushEvent(&event);

                            SDL_SemWait(Game_DisplaySem);
                        }

                        EAX = 0x004f;	// VESA OK
                    }
                    else
                    {
                        Game_StopMain();
                    }

                    break;
                    // case 0x4f02:
                case 0x4f03:
                // VESA: Return current VBE Mode
                    EAX = 0x004f;	// VESA OK
                    EBX = 0x03;		// Text mode: 80x25

                    break;
                    // case 0x4f03:
                case 0x4f06:
                // VESA: Set/Get Logical Scan Line Length
#if defined(__DEBUG__)
                    fprintf(stderr, "Set/Get Logical Scan Line Length\n");
#endif

                    if (BL == 1)
                    {
                        EAX = 0x004f;	// VESA OK
                        EBX = 360;		// Bytes Per Scan Line
                        ECX = 360;		// Actual Pixels Per Scan Line
                        EDX = 480;		// Maximum Number of Scan Lines
                    }
                    else
                    {
#if defined(__DEBUG__)
                        fprintf(stderr, "bl: %i\n", BL);
                        fprintf(stderr, "cx: %i\n", CX);
#endif
                        Game_StopMain();
                    }

                    break;
                    // case 0x4f06:
                case 0x4f07:
                // VESA: Set/Get Display Start
#if defined(__DEBUG__)
                    fprintf(stderr, "Set/Get Display Start: x:%i y:%i\n", CX, DX);
#endif

                    if (BX == 0)
                    {
                        if (Game_DisplayStart != DX)
                        {
#if defined(__DEBUG__)
                            if (Game_UseEnhanced3DEngine != Game_UseEnhanced3DEngineNewValue)
                            {
                                if (Game_UseEnhanced3DEngineNewValue)
                                {
                                    fprintf(stderr, "Enhanced 3d engine\n");
                                }
                                else
                                {
                                    fprintf(stderr, "Original 3d engine\n");
                                }
                            }
#endif

                            if (!Game_UseEnhanced3DEngine)
                            {
                                Game_OverlayDisplay.Enhanced3DEngineUsed = -1;
                            }
                            else
                            {
                                if (Game_OverlayDraw.Enhanced3DEngineUsed > 0)
                                {
                                    Game_OverlayDisplay = Game_OverlayDraw;
                                    Game_OverlayDraw.Enhanced3DEngineUsed = 0;
                                    if (Game_OverlayDraw.ScreenViewpartOriginal == Game_ScreenViewpartOriginal[0])
                                    {
                                        Game_OverlayDraw.ScreenViewpartOverlay = Game_ScreenViewpartOverlay[1];
                                        Game_OverlayDraw.ScreenViewpartOriginal = Game_ScreenViewpartOriginal[1];
                                    }
                                    else
                                    {
                                        Game_OverlayDraw.ScreenViewpartOverlay = Game_ScreenViewpartOverlay[0];
                                        Game_OverlayDraw.ScreenViewpartOriginal = Game_ScreenViewpartOriginal[0];
                                    }
                                }
                                else
                                {
                                    Game_OverlayDisplay.Enhanced3DEngineUsed = Game_OverlayDraw.Enhanced3DEngineUsed;
                                    Game_OverlayDraw.Enhanced3DEngineUsed = 0;
                                }
                            }

                            Game_UseEnhanced3DEngine = Game_UseEnhanced3DEngineNewValue;
                        }

                        Game_DisplayStart = DX;
                        EAX = 0x004f;	// VESA OK

                        /*memcpy(&(Game_FrameBuffer[Game_ScreenWindowNum << 16]), Game_ScreenWindow, (Game_ScreenWindowNum == 2)?41728:65536);*/

                        event.type = SDL_USEREVENT;
                        event.user.code = EC_DISPLAY_FLIP_START;
                        event.user.data1 = NULL;
                        event.user.data2 = NULL;

                        SDL_PushEvent(&event);
                    }
                    else
                    {
#if defined(__DEBUG__)
                        fprintf(stderr, "bx: %i\n", BX);
                        fprintf(stderr, "cx: %i\n", CX);
                        fprintf(stderr, "dx: %i\n", DX);
#endif
                        Game_StopMain();
                    }

                    break;
                    // case 0x4f07:
                default:
                    Game_StopMain();
            }

            break;
            // case 0x10:
        case 0x2f:
        // DOS Extended functions
            switch (AX)
            {
                case 0x150b:
                // CD-ROM v2.00+ - DRIVE CHECK
                    EAX = 0xffffffff;	// drive supported
                    EBX = 0xadad;		// MSCDEX.EXE installed

                    break;
                    // case 0x150b:
                default:
                    Game_StopMain();
            }

            break;
            // case 0x2f:
        case 0x31:
        // DPMI API
            switch (AX)
            {
                case 0x0000:
                // ALLOCATE LDT DESCRIPTORS
#if defined(__DEBUG__)
                    fprintf(stderr, "Allocating LDT Descriptors: %i\n", CX);
#endif
                    AX = 0;		// base selector
                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0000:
                case 0x0001:
                // FREE LDT DESCRIPTOR
                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0001:
                case 0x0007:
                // SET SEGMENT BASE ADDRESS
#if defined(__DEBUG__)
                    fprintf(stderr, "Setting Segment Base Address: 0x%x\n", ((uint32_t) CX << 16) + DX);
#endif
                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0007:
                case 0x0008:
                // SET SEGMENT LIMIT
#if defined(__DEBUG__)
                    fprintf(stderr, "Setting Segment Limit: 0x%x\n", ((uint32_t) CX << 16) + DX);
#endif
                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0008:
                case 0x0100:
                // ALLOCATE DOS MEMORY BLOCK
#if defined(__DEBUG__)
                    fprintf(stderr, "Allocating DOS Memory Block: %i\n", (uint32_t) BX << 4);
#endif
                    if (BX == 65535)
                    {
                        EAX = 8;				// Insufficient memory
                        EBX = 6570;			// Free memory (in paragraphs = 16 bytes)
                        SET_FLAG(CARRY_FLAG);
                    }
                    else
                    {
                        PTR_ECX = Game_AllocateMemory((uint32_t) BX << 4);
                        if (ECX)
                        {
#if defined(__DEBUG__)
                            fprintf(stderr, "Allocated address: 0x%x\n", ECX);
#endif

                            EAX = /*0xAEC0*/ 0x0100;    // Real mode segment of allocated block
                            DX = 0;                     // First selector for allocated block
                            CLEAR_FLAG(CARRY_FLAG);
                        }
                        else
                        {
                            SET_FLAG(CARRY_FLAG);
                        }
                    }

                    break;
                    // case 0x0100:
                case 0x0101:
                // FREE DOS MEMORY BLOCK
#if defined(__DEBUG__)
                    fprintf(stderr, "Free DOS Memory Block: 0x%x\n", EDX);
#endif

                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0101:
                case 0x0300:
                // Simulate Real Mode Interrupt
#if defined(__DEBUG__)
                    fprintf(stderr, "Simulating Real Mode Interrupt: %xh\n", BL);
#endif

                    if (CX != 0)
                    {
                        fprintf(stderr, "error: Request to copy words from stack: %i\n", CX);
                        Game_StopMain();
                    }

                    {

                        if (Game_InterruptTable[BL] == 0)
                        {
#if defined(__DEBUG__)
                            fprintf(stderr, "Running Original Interrupt...\n");
#endif
                            Game_intDPMI(BL, (Game_DPMIREGS *)(void *) PTR_EDI);
                        }
                        else
                        {
#if defined(__DEBUG__)
                            fprintf(stderr, "Running Modified Interrupt...\n");
#endif
                            Game_StopMain();
                        }

                    }

                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0300:
                case 0x0400:
                // GET DPMI VERSION
                    EAX = 0x0102;	// Version: 1.2
                    EBX = 0x0001;	// 386 DPMI, V86 mode interrupts, no virtual memory
                    ECX = 0x04;		// Processor: 486
                    EDX = 0;			// ??? PIC base interrupt

                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0400:
                case 0x0500:
                // GET FREE MEMORY INFORMATION
                    memset((void *)PTR_EDI, -1, 0x30);
                    *((uint32_t *)(void *)PTR_EDI) = GAME_MAX_FREE_MEMORY;

                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0500:
                case 0x0501:
                // ALLOCATE MEMORY BLOCK
#if defined(__DEBUG__)
                    fprintf(stderr, "Allocating Memory Block: %i\n", ((uint32_t) BX << 16) + CX);
#endif
                    PTR_ECX = Game_AllocateMemory(((uint32_t) BX << 16) + CX);
                    if (ECX)
                    {
#if defined(__DEBUG__)
                        fprintf(stderr, "Allocated address: 0x%x\n", ECX);
#endif
                        EDI = ECX;
                        EBX = ECX >> 16;
                        ESI = EBX;

                        CLEAR_FLAG(CARRY_FLAG);
                    }
                    else
                    {
                        SET_FLAG(CARRY_FLAG);
                    }

                    break;
                    // case 0x0501:
                case 0x0502:
                // Free Memory Block
                    EAX = ((uint32_t) SI << 16) + DI;
                    Game_FreeMemory((void *)PTR_EAX);
                    EAX = 0;

                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0502:
                case 0x0600:
                // LOCK LINEAR REGION
                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0600:
                case 0x0601:
                // UNLOCK LINEAR REGION
                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0601:
                case 0x0604:
                // GET PAGE SIZE
                    EBX = 0;		// ??? Page size: high word
                    ECX = 0;		// ??? Page size: low word

                    CLEAR_FLAG(CARRY_FLAG);

                    break;
                    // case 0x0604:
                default:
                    Game_StopMain();
            }
            break;
            // case 0x31:
        case 0x33:
        // Microsoft Mouse
            switch (AX)
            {
                case 0x0000:
                // Reset driver and read status
                    EAX = 0xffffffff;	// HW/driver installed
                    EBX = 0xffffffff;	// 2-button mouse

                    {
                        int i;

                        for (i = 0; i < 8; i++)
                        {
                            Game_MouseTable[i] = 0;
                        }
                    }

                    break;
                    // case 0x0000:
                case 0x000c:
                // Define interrupt subroutine parameters
#if defined(__DEBUG__)
                    fprintf(stderr, "Setting mouse interrupt subroutine parameters: %i\n", CX);
#endif
                    {
                        unsigned int mask, i;
                        mask = CX & 0x7F;

                        for (i = 0; i < 8; i++)
                        {
                            if (mask & 1)
                            {
                                Game_MouseTable[i] = EDX;
                            }
                            mask = mask >> 1;
                        }
                    }

                    break;
                    // case 0x000c:
                default:
                    Game_StopMain();
            }
            break;
            // case 0x33:
        default:
            Game_StopMain();
    } // switch (IntNum)

    // just in case *out_regs is not properly aligned
    memcpy((void *)out_regs, &tmp_regs, sizeof(Game_REGS));
	//*out_regs = tmp_regs;

    return EAX;
}
