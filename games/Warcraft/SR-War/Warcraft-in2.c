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
#include "Game_vars.h"
#include "Warcraft-in2.h"
#include "Game_thread.h"

uint32_t CCALL Game_inp(const uint32_t port)
{
    uint32_t ret;
#if defined(__DEBUG__)
    fprintf(stderr, "in2 0x%x\n", port & 0xffff);
#endif

    switch (port & 0xffff)
    {
        case 0x60:
        // keyboard

            if (Game_KBufferWrite != Game_KBufferRead)
            {
                ret = Game_KBuffer[Game_KBufferRead] >> 24;

                Game_KBufferRead = (Game_KBufferRead + 1) & (GAME_KBUFFER_LENGTH - 1);

                return ret;
            }
            else
            {
                return 0;
            }

            // case 0x21:
        //default:
    } // switch (inter_no)

    Game_StopMain();

    return 0;
}
