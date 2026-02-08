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
#include "Game_vars.h"
#include "Xcom-int2.h"
#include "Game_thread.h"
#include "Game-int2.h"

uint32_t CCALL Game_int386x(
    const uint32_t IntNum,
    const Game_REGS *in_regs,
    Game_REGS *out_regs,
    Game_SREGS *seg_regs)
{
    Game_REGS tmp_regs;
    SDL_Event event;

    tmp_regs = *in_regs;

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
                case 0x0013:
                // Set Video Mode
#if defined(__DEBUG__)
                    fprintf(stderr, "Setting video mode: 0x%x\n", AL);
                    fprintf(stderr, "320x200x256\n");
#endif

#if (EXE_BUILD != EXE_COMBINED)
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
#endif

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

                    break;
                default:
                    Game_StopMain();
            }

            break;
            // case 0x10:
        default:
            Game_StopMain();
    } // switch (IntNum)

    *out_regs = tmp_regs;

    return EAX;
}
