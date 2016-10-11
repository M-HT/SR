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
#include "Tactical-inout.h"
#include "Game_thread.h"
#include "display.h"

#define EAX (_eax.e)
#define AX (_eax.w.x)
#define AL (_eax.b.l)
#define AH (_eax.b.h)


static uint32_t palette_index, color_index;

uint32_t X86_InPortProcedure(
    const uint16_t PortNum,
    const uint32_t PortSize)
{
    switch (PortNum)
    {
        case 0x0040:
            // 8253-5 Channel 0
            // ????

            return 0;
            // case 0x0040:
        //default:
    } // switch (PortNum)

#if defined(__DEBUG__)
    fprintf(stderr, "in 0x%x\n", PortNum);
#endif

    Game_StopMain();
}

void X86_OutPortProcedure(
    const uint16_t PortNum,
    const uint32_t PortSize,
    const Game_register _eax)
{
    switch (PortNum)
    {
        case 0x0021:
            // PIC master interrupt mask register
            if (AL == 0x00 || AL == 0xff)
            {
                return;
            }
            return;
            // case 0x0040:
        case 0x0040:
            // 8253-5 Channel 0
            // ????
            // timer: 1234DDh / set value = x times per second
            return;
            // case 0x0040:
        case 0x0043:
            // 8253-5 Mode control
            // ????
            if (AL == 0x00 || AL == 0x36)
            {
                return;
            }
            break;
            // case 0x0043:
        case 0x03c8:
            // Set palette index

            palette_index = AL;
            color_index = 0;

            return;
            // case 0x03c8:
        case 0x03c9:
            // Set palette color

            if (color_index <= 2)
            {
                uint32_t color;

                switch (color_index)
                {
                    case 0:
                        color_index = 1;
                        color = ( (uint32_t) AL << 2) | ( (uint32_t) AL >> 4);

                        Game_Palette_Or[palette_index].s.r = color;

                        break;
                    case 1:
                        color_index = 2;
                        color = ( (uint32_t) AL << 2) | ( (uint32_t) AL >> 4);

                        Game_Palette_Or[palette_index].s.g = color;

                        break;
                    case 2:
                        color_index = 0;
                        color = ( (uint32_t) AL << 2) | ( (uint32_t) AL >> 4);

                        Game_Palette_Or[palette_index].s.b = color;

                        Set_Palette_Value(palette_index, Game_Palette_Or[palette_index].s.r, Game_Palette_Or[palette_index].s.g, Game_Palette_Or[palette_index].s.b);

                        palette_index++;
                        break;
                }
            }

            return;
            // case 0x03c9:
        //default:
    } // switch (PortNum)

#if defined(__DEBUG__)
    fprintf(stderr, "out 0x%x\n", PortNum);
    fprintf(stderr, "size %i\n", PortSize);
    fprintf(stderr, "eax 0x%x\n", EAX);
    fprintf(stderr, "al 0x%x\n", AL);
#endif

    Game_StopMain();
}
