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
#ifdef USE_SDL2
    #include <SDL2/SDL.h>
#else
    #include <SDL/SDL.h>
#endif
#include "Game_defs.h"
#include "Game_vars.h"
#include "Warcraft-int.h"
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
					else if (AL == 3)
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

						AL = 0x30;	// VESA OK
					}
					else
					{
						Game_StopMain();
					}

					return;
					// case 0x0f:
				case 0x0f:
				// Get Current Video Mode

				#ifdef USE_SDL2
					if (Game_Window == NULL)
				#else
					if (Game_Screen == NULL)
				#endif
					{
						AH = 80;	// number of character columns
						AL = 3;		// display mode
						BH = 0;		// active oage
					}
					else
					{
						AH = 80;	// number of character columns
						AL = 0x13;	// display mode
						BH = 0;		// active page
					}

					return;
					// case 0x0f:
				//default:
			} // switch (AH)

			break;
			// case 0x10:
		case 0x21:
		// DOS functions
			switch (AH)
			{
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
