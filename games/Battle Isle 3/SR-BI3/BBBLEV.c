/**
 *
 *  Copyright (C) 2020-2025 Roman Pauer
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

#include "BBBLEV.h"
#include "BBSYSTEM.h"
#include <string.h>


static int BLEV_initialized = 0;
static BLEV_Event BLEV_events[32];
static int BLEV_readIndex;
static int BLEV_state;
static int BLEV_writeIndex;


int BLEV_Init_c(void)
{
    if ( BLEV_initialized )
    {
        return 1;
    }

    BLEV_state = 0;
    BLEV_readIndex = 0;
    BLEV_writeIndex = 0;

    BLEV_initialized = 1;

    return 1;
}

void BLEV_Exit_c(void)
{
    if (BLEV_initialized)
    {
        BLEV_initialized = 0;
    }
}

void BLEV_ClearAllEvents_c(void)
{
    int index;

    for (index = 0; index < 100; index++)
    {
        SYSTEM_SystemTask_c();
        BLEV_state = 0;
        BLEV_readIndex = 0;
        BLEV_writeIndex = 0;
    }
}

int BLEV_PutEvent_c(const BLEV_Event *event)
{
    if (BLEV_state == 2)
    {
        return 0;
    }

    BLEV_events[BLEV_writeIndex] = *event;

    BLEV_writeIndex++;
    if (BLEV_writeIndex == 32)
    {
        BLEV_writeIndex = 0;
    }

    if (BLEV_writeIndex == BLEV_readIndex)
    {
        BLEV_state = 2;
    }
    else
    {
        BLEV_state = 1;
    }

    return 1;
}

int BLEV_GetEvent_c(BLEV_Event *event)
{
    if (BLEV_state == 0)
    {
        return 0;
    }

    *event = BLEV_events[BLEV_readIndex];

    BLEV_readIndex++;
    if (BLEV_readIndex == 32)
    {
        BLEV_readIndex = 0;
    }

    BLEV_state = (BLEV_writeIndex != BLEV_readIndex)?1:0;

    return 1;
}

int BLEV_PeekEvent_c(BLEV_Event *event)
{
    if (BLEV_state == 0)
    {
        return 0;
    }

    *event = BLEV_events[BLEV_readIndex];

    return 1;
}

