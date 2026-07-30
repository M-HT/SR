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

#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-engine.h"

#pragma pack(1)
typedef struct PACKED {
    uint16_t Flags;
    uint16_t Type;
    uint16_t Screen_type;
    PTR32(void) MainLoop_function;
    PTR32(void) ModInit_function;
    PTR32(void) ModExit_function;
    PTR32(void) DisInit_function;
    PTR32(void) DisExit_function;
    PTR32(void) DisUpd_function;
} Game_Module;
#pragma pack()

extern Game_Module loc_179164[8]; // stack of screen modules
extern uint16_t loc_13EEEE; // stack top

uint16_t Game_ScreenType(void)
{
    return loc_179164[loc_13EEEE].Screen_type;
}

uint16_t Game_RootScreenType(void)
{
    for (int i = (int) loc_13EEEE; i >= 0; i--)
    {
        if (loc_179164[i].Type == 0) /* SCREEN_MOD */
        {
            return loc_179164[i].Screen_type;
        }
    }

    return GAME_SCREEN_NO_SCREEN;
}
