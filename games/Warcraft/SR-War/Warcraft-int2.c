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
#include <string.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Warcraft-int2.h"
#include "Game_thread.h"
#include "Game-int2.h"

uint32_t Game_int386x(
	const uint32_t IntNum,
	const Game_REGS *in_regs,
	Game_REGS *out_regs,
	Game_SREGS *seg_regs)
{
	Game_REGS tmp_regs;

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

					break;
					// case 0x25:
				case 0x35:
				// Get interrupt vector

					EBX = (uint32_t) Game_InterruptTable[AL];

					break;
					// case 0x35:
				default:
					Game_StopMain();
			}
			break;
			// case 0x21:
		case 0x31:
		// DPMI API
			switch (AX)
			{
				case 0x0500:
				// GET FREE MEMORY INFORMATION
					memset((void *) EDI, -1, 0x30);
					*((uint32_t *)EDI) = GAME_MAX_FREE_MEMORY;

                    CLEAR_FLAG(CARRY_FLAG);

					break;
					// case 0x0500:
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
							Game_MouseTable[i] = NULL;
						}
					}

					break;
					// case 0x0000:
				case 0x0007:
				// Define horizontal cursor range
#if defined(__DEBUG__)
					fprintf(stderr, "Setting horizontal cursor range: %i - %i\n", CX, DX);
#endif
					break;
					// case 0x0007:
				case 0x0008:
				// Define vertical cursor range
#if defined(__DEBUG__)
					fprintf(stderr, "Setting vertical cursor range: %i - %i\n", CX, DX);
#endif
					break;
					// case 0x0008:
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
								Game_MouseTable[i] = (void *) EDX;
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
	} // switch (inter_no)

    // just in case *out_regs is not properly aligned
    memcpy((void *)out_regs, &tmp_regs, sizeof(Game_REGS));
	//*out_regs = tmp_regs;

	return EAX;
}
