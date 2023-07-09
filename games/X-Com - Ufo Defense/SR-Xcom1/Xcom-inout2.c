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
#include "Xcom-inout2.h"
#include "Game_thread.h"
#include "display.h"

static uint32_t palette_index, color_index;

uint32_t Game_inp(const uint32_t port)
{
#if defined(__DEBUG__)
	fprintf(stderr, "in2 0x%x\n", port & 0xffff);
#endif

	switch (port & 0xffff)
	{
		case 0x03c8:
            // ????
			return 0;
			// case 0x03c8:
		case 0x03c9:
            // Get palette color

            if (color_index <= 2)
            {
                switch (color_index)
                {
                    case 0:
                        color_index = 1;

                        return (Game_Palette_Or[palette_index].s.r) >> 2;
                    case 1:
                        color_index = 2;

                        return (Game_Palette_Or[palette_index].s.g) >> 2;
                    case 2:
                        color_index = 0;

                        return (Game_Palette_Or[palette_index++].s.b) >> 2;
                }
            }

			break;
			// case 0x03c9:
		//default:
	} // switch (inter_no)


	Game_StopMain();

	return 0;
}

uint32_t Game_outp(const uint32_t port, const uint32_t value)
{
#if defined(__DEBUG__)
	fprintf(stderr, "out2 0x%x\n", port & 0xffff);
	fprintf(stderr, "value %i\n", value & 0xff);
#endif

	switch (port & 0xffff)
	{
        case 0x03c7:
            // Set palette index for reading

            palette_index = value;
            color_index = 0;

            break;
        case 0x03c8:
            // Set palette index for writing

            palette_index = value;
            color_index = 0;

            break;
        case 0x03c9:
            // Set palette color

            if (color_index <= 2)
            {
                uint32_t color;

                switch (color_index)
                {
                    case 0:
                        color_index = 1;
                        color = ( (uint32_t) value << 2) | ( (uint32_t) value >> 4);

                        Game_Palette_Or[palette_index].s.r = color;

                        break;
                    case 1:
                        color_index = 2;
                        color = ( (uint32_t) value << 2) | ( (uint32_t) value >> 4);

                        Game_Palette_Or[palette_index].s.g = color;

                        break;
                    case 2:
                        color_index = 0;
                        color = ( (uint32_t) value << 2) | ( (uint32_t) value >> 4);

                        Game_Palette_Or[palette_index].s.b = color;

                        Set_Palette_Value(palette_index, Game_Palette_Or[palette_index].s.r, Game_Palette_Or[palette_index].s.g, Game_Palette_Or[palette_index].s.b);

                        palette_index++;
                        break;
                }
            }

            break;
            // case 0x03c9:
	    default:
            Game_StopMain();
	}


	return value;
}
