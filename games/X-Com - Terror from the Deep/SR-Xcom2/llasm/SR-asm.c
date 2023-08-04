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

#include "llasm_cpu.h"

extern volatile uint32_t Game_SDLTicks;
extern volatile uint32_t Game_LastAudio;

#ifdef __cplusplus
extern "C" {
#endif

extern void Game_RunTimer(void);
extern void Game_RunTimerDelay(void);
extern void Game_SlowDownMainLoop(void);
extern void Game_SlowDownScrolling(void);

#ifdef __cplusplus
}
#endif


EXTERNC void SR_CheckTimer(void)
{
    if (Game_SDLTicks != Game_LastAudio) Game_RunTimer();
}

EXTERNC void SR_RunTimerDelay(void)
{
    Game_RunTimerDelay();
}

EXTERNC void SR_SlowDownMainLoop(void)
{
    Game_SlowDownMainLoop();
}

EXTERNC void SR_SlowDownScrolling(void)
{
    Game_SlowDownScrolling();
}

