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
#include "Albion-int.h"
#include "Albion-timer.h"
#include "Game_thread.h"
#include "Game-int.h"

void X86_InterruptProcedure(
    const uint8_t IntNum,
    _cpu_regs *regs)
{

    switch (IntNum)
    {
        case 0x10:
        // Video
            switch (AX)
            {
                case 0x4f05:
                // VESA: VBE Display Window Control

#if defined(__DEBUG__)
                    fprintf(stderr, "Setting memory window: %i\n", DX);
#endif
                    if (BX == 0)
                    {
                        if (DX != Game_ScreenWindowNum)
                        {
                            /*memcpy(&(Game_FrameBuffer[Game_ScreenWindowNum << 16]), Game_ScreenWindow, (Game_ScreenWindowNum == 2)?41728:65536);*/

                            Game_ScreenWindowNum = DX;

                            /*memcpy(Game_ScreenWindow, &(Game_FrameBuffer[Game_ScreenWindowNum << 16]), (Game_ScreenWindowNum == 2)?41728:65536);*/

                            Game_ScreenWindow = &(Game_FrameBuffer[Game_ScreenWindowNum << 16]);

                            screen_window_ptr[0] = (uint32_t) Game_ScreenWindow;
                            screen_window_ptr[1] = (uint32_t) Game_ScreenWindow;
                            screen_window_ptr[2] = (uint32_t) Game_ScreenWindow;
                            screen_window_ptr[3] = (uint32_t) Game_ScreenWindow;
                        }

                        EAX = 0x004F;	// VESA OK
                    }
                    else
                    {
#if defined(__DEBUG__)
                        fprintf(stderr, "bx %i\n", BX);
                        fprintf(stderr, "dx %i\n", DX);
#endif
                        Game_StopMain();
                    }

                    return;
                    // case 0x4f05:
                //default:
            } // switch (AX)

            break;
            // case 0x10:
        case 0x16:
        // Keyboard
            switch (AH)
            {
                case 0x10:
                // Get enhanced keystroke
                    if (Thread_Exit) Game_StopMain();

                    while (Game_KBufferWrite == Game_KBufferRead || SDL_GetTicks() - Game_LastKeyStroke < GAME_KEYBOARD_TYPE_RATE)
                    {
                        Game_RunTimerDelay();
                        if (Thread_Exit) Game_StopMain();
                    };

                    Game_LastKeyStroke = SDL_GetTicks();
                    AX = Game_KBuffer[Game_KBufferRead] & 0xffff;

                    Game_KBufferRead = (Game_KBufferRead + 1) & (GAME_KBUFFER_LENGTH - 1);

                    return;
                case 0x11:
                // Check for enhanced keystroke
                    if (Thread_Exit) Game_StopMain();

                    if (Game_KBufferWrite == Game_KBufferRead || SDL_GetTicks() - Game_LastKeyStroke < GAME_KEYBOARD_TYPE_RATE)
                    {
                        SET_FLAG(ZERO_FLAG); // kbd buffer empty
                    }
                    else
                    {
                        CLEAR_FLAG(ZERO_FLAG); // kbd buffer not empty

                        AX = Game_KBuffer[Game_KBufferRead] & 0xffff;
                    }

                    return;
                //default:
            } // switch (AH)
            break;
            // case 0x16:
        case 0x21:
        // DOS functions
            switch (AH)
            {
                case 0x0e:
                // Select disc
#if defined(__DEBUG__)
                    fprintf(stderr, "Selecting disc: %i\n", DL);
#endif
                    AL = 4;		// ??? 4 drives

                    return;
                    // case 0x0e:
                case 0x19:
                // Get default disc number
                    AL = 2;		// ??? drive C

                    return;
                    // case 0x19:
                case 0x25:
                // Set interrupt vector
#if defined(__DEBUG__)
                    fprintf(stderr, "Setting interrupt vector: %i\n", AL);
#endif
                    Game_InterruptTable[AL] = EDX;

                    return;
                    // case 0x25:
                case 0x35:
                // Get interrupt vector

                    EBX = Game_InterruptTable[AL];

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
                case 0x0200:
                // Get Real Mode Interrupt Vector
                    ECX = 0;		// ??? Interrupt segment
                    EDX = 0;		// ??? Interrupt offset

                    return;
                    // case 0x0200:
            }

            break;
            // case 0x31:
        //default:
    } // switch (asm_int_num)

#if defined(__DEBUG__)
    fprintf(stderr, "int %xh\n", IntNum);
    fprintf(stderr, "eax 0x%x\n", EAX);
    fprintf(stderr, "ah 0x%x\n", AH);
#endif

    Game_StopMain();
}
