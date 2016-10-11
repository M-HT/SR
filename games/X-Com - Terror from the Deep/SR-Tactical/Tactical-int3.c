/**
 *
 *  Copyright (C) 2016 Roman Pauer
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
#include "Tactical-int3.h"
#include "Tactical-music.h"
#include "Game_thread.h"
#include "Game-int3.h"

void Game_intDPMI(
    const uint32_t IntNum,
    Game_DPMIREGS *regs)
{
    switch (IntNum)
    {
        case 0x66:
        // sound driver
            switch (AX)
            {
                case 0x0100:
                    // init music
#if defined(__DEBUG__)
                    fprintf(stderr, "Running function: eax=0x%x\n", EAX );
#endif
                    // edx=address of Game_SoundConfig (14 bytes)
                    return;
                    // case 0x0100:
                case 0x0101:
#if defined(__DEBUG__)
                    fprintf(stderr, "Running function: eax=0x%x\n", EAX );
#endif
                    // stop music
                    return;
                    // case 0x0101:
                case 0x0103:
#if defined(__DEBUG__)
                    fprintf(stderr, "Running function: eax=0x%x\n", EAX );
#endif
                    // process sequence
                    SDL_Delay(0);
                    return;
                    // case 0x0103:
                case 0x0104:
#if defined(__DEBUG__)
                    fprintf(stderr, "Running function: eax=0x%x\n", EAX );
#endif
                    // edx=address in 1st MB (read from file - max 33000 bytes)
                    // play sequence

                    Game_start_sequence((uint8_t *) EDX);

                    return;
                    // case 0x0104:
                case 0x0105:
#if defined(__DEBUG__)
                    fprintf(stderr, "Running function: eax=0x%x\n", EAX );
#endif
                    // is sequence playing ?
                    // eax=result (1 = yes, 0 = no)

                    EAX = Game_sequence_playing();

                    return;
                    // case 0x0105:
                case 0x0106:
#if defined(__DEBUG__)
                    fprintf(stderr, "Running function: eax=0x%x\n", EAX );
#endif
                    // ebx=volume 0-128

                    Game_set_sequence_volume(EBX);

                    return;
                    // case 0x0106:
                case 0x0108:
#if defined(__DEBUG__)
                    fprintf(stderr, "Running function: eax=0x%x\n", EAX );
#endif
                    // stop sequence

                    Game_stop_sequence();

                    return;
                    // case 0x0108:
                case 0x010d:
#if defined(__DEBUG__)
                    fprintf(stderr, "Running function: eax=0x%x\n", EAX );
#endif
                    // ebx=quantization rate in Hz - equals timer frequency
                    return;
                    // case 0x010d:
                //default:
            } // switch (AX)

            break;
            // case 0x66:
        //default:
    } // switch (int_number)

#if defined(__DEBUG__)
    fprintf(stderr, "int3 %xh\n", IntNum);
    fprintf(stderr, "eax 0x%x\n", EAX);
    fprintf(stderr, "ah 0x%x\n", AH);
#endif

    Game_StopMain();
} // void Game_intDPMI(...)
