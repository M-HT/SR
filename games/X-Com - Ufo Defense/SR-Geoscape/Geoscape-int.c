/**
 *
 *  Copyright (C) 2016-2020 Roman Pauer
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
#include <time.h>
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
#else
    #include <SDL/SDL.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Geoscape-int.h"
#include "Geoscape-int3.h"
#include "Geoscape-proc.h"
#include "Geoscape-proc-events.h"
#include "Game_thread.h"
#include "Game-int.h"

void X86_InterruptProcedure(
    const uint8_t IntNum,
    _cpu_regs *regs)
{
    SDL_Event event;

    switch (IntNum)
    {
        case 0x10:
        // Video
            switch (AH)
            {
                case 0x00:
                // Set Video Mode

#if defined(__DEBUG__)
                    fprintf(stderr, "Setting video mode: 0x%x\n", AL);
                    if (AL == 0x13)
                    {
                        fprintf(stderr, "320x200x256\n");
                    }
#endif
                    if (AL == 0x13)
                    {
                    #ifdef USE_SDL2
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

                    #ifdef USE_SDL2
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

                        AL = 0x20;	// OK
                    }
                    else if (AL == 0x03)
                    {
                    #ifdef USE_SDL2
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

                        AL = 0x30;	// OK
                    }
                    else
                    {
						Game_StopMain();
                    }

                    return;
                    // case 0x00:
                //default:
            } // switch (AH)
            break;
            // case 0x10:
        case 0x1a:
        // TIME functions
            switch (AH)
            {
                case 0x02:
                // Get real-time clock time
                    {
                        time_t cur_time;
                        struct tm *t;

                        cur_time = time(NULL);
                        t = localtime(&cur_time);

                        CH = ((t->tm_hour / 10) << 16) + t->tm_hour % 10;
                        CL = ((t->tm_min / 10) << 16) + t->tm_min % 10;
                        DH = ((t->tm_sec / 10) << 16) + t->tm_sec % 10;
                        DL = (t->tm_isdst)?1:0;

                        CLEAR_FLAG(CARRY_FLAG);
                    }

                    return;
                    // case 0x02:
                //default:
            } // switch (AH)
            break;
            // case 0x1a:
        case 0x21:
        // DOS functions
            switch (AH)
            {
                case 0x25:
                // Set interrupt vector
#if defined(__DEBUG__)
                    fprintf(stderr, "Setting interrupt vector: %i\n", AL);
#endif
                    Game_InterruptTable[AL] = (void *) EDX;

                    return;
                    // case 0x25:
                case 0x2d:
                // Set system time

                    AL = 0xff; // don't set time

                    return;
                    // case 0x2d:
                case 0x35:
                // Get interrupt vector

                    EBX = (uint32_t) Game_InterruptTable[AL];

                    return;
                    // case 0x35:
                //default:
            } // switch (AH)
            break;
            // case 0x21:
        case 0x31:
        // DPMI API
            switch (AX)
            {
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
                        ECX = (uint32_t) Game_AllocateMemory((uint32_t) BX << 4);
                        if (ECX)
                        {
#if defined(__DEBUG__)
                            fprintf(stderr, "Allocated address: 0x%x\n", ECX);
#endif

                            EAX = /*0xAEC0*/ 0x0100;	// Real mode segment of allocated block
                            DX = 0;					// First selector for allocated block
                            CLEAR_FLAG(CARRY_FLAG);
                        }
                        else
                        {
                            SET_FLAG(CARRY_FLAG);
                        }
                    }

                    return;
                    // case 0x0100:
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

                        if (Game_InterruptTable[BL] == NULL)
                        {
#if defined(__DEBUG__)
                            fprintf(stderr, "Running Original Interrupt...\n");
#endif
                            Game_intDPMI(BL, (Game_DPMIREGS *) EDI);
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

                    return;
                    // case 0x0300:
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
                            Game_MouseTable[i] = NULL;
                        }
                    }

                    return;
                    // case 0x0000:
                case 0x0003:
                // Return position and button status

                    SDL_Delay(0);

                    {
                        int mousex, mousey;

                        BX = Game_MouseButtons | Game_MousePressedButtons;
                        Game_MousePressedButtons = 0;

                        mousex = (Game_Device2PictureX(Game_MouseX) * Game_VideoAspectX + 32767) >> 16;
                        mousey = (Game_Device2PictureY(Game_MouseY) * Game_VideoAspectY + 32767) >> 16;

                        CX = mousex;
                        DX = mousey;
                    }

                    return;
                    // case 0x0003:
                case 0x0007:
                // Define horizontal cursor range
                    return;
                    // case 0x0007:
                case 0x0008:
                // Define vertical cursor range
                    return;
                    // case 0x0008:
                //default:
            }
            break;
            // case 0x33:
        //default:
    } // switch (asm_int_num)

#if defined(__DEBUG__)
    fprintf(stderr, "int %xh\n", IntNum);
    fprintf(stderr, "eax 0x%x\n", EAX);
    fprintf(stderr, "ah 0x%x\n", AH);
#endif

    Game_StopMain();
}
