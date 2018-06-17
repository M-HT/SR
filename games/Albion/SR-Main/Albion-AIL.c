/**
 *
 *  Copyright (C) 2018 Roman Pauer
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
#include "Albion-AIL.h"


int32_t AIL_preference[19] = {200, 1, 32768, 100, 16, 100, 655, 0, 0, 1, 0, 120, 8, 127, 1, 0, 2, 1, 1};
extern uint8_t AIL_error[256];


void *Game_AIL_mem_use_malloc(void * (*fn)(uint32_t))
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_mem_use_malloc: 0x%x\n", (uint32_t) fn);
#endif
    return NULL;
}

void *Game_AIL_mem_use_free(void (*fn)(void *))
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_mem_use_free: 0x%x\n", (uint32_t) fn);
#endif
    return NULL;
}

void Game_AIL_startup(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_startup\n");
#endif
    AIL_error[0] = 0;
}

int32_t Game_AIL_register_timer(void (*callback_fn)(uint32_t user))
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_register_timer: 0x%x\n", (uint32_t) callback_fn);
#endif
    return 1;
}

void Game_AIL_set_timer_frequency(int32_t timer, uint32_t hertz)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_timer_frequency: %i - %i\n", timer, hertz);
#endif
}

void Game_AIL_start_timer(int32_t timer)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_start_timer: %i\n", timer);
#endif
    Game_InterruptTable[8] = (void *)-1;
}

void Game_AIL_stop_timer(int32_t timer)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_stop_timer: %i\n", timer);
#endif
    Game_InterruptTable[8] = NULL;
}

void Game_AIL_release_timer_handle(int32_t timer)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_release_timer_handle: %i\n", timer);
#endif
}

void Game_AIL_shutdown(void)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_shutdown\n");
#endif
}

void Game_AIL_set_GTL_filename_prefix(char *prefix)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_GTL_filename_prefix: %s\n", prefix);
#endif
}

int32_t Game_AIL_install_MDI_INI(void *mdi)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_install_MDI_INI: 0x%x\n", (uint32_t) mdi);
#endif
    return (Game_Music)?0:1;
}

int32_t  Game_AIL_set_preference(uint32_t number, int32_t value)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_set_preference: 0x%x - %i\n", number, value);
#endif
    int32_t oldvalue;

    oldvalue = AIL_preference[number];
    AIL_preference[number] = value;
    return oldvalue;
/*
number:
   0 = DIG_SERVICE_RATE
   1 = DIG_HARDWARE_SAMPLE_RATE
   2 = DIG_DMA_BUFFER_SIZE
   3 = ???
   4 = DIG_MIXER_CHANNELS
   5 = DIG_DEFAULT_VOLUME
   6 = DIG_RESAMPLING_TOLERANCE
   7 = DIG_USE_STEREO
   8 = DIG_USE_16_BITS
   9 = DIG_ALLOW_16_BIT_DMA
   A = AIL_SCAN_FOR_HARDWARE
   B = MDI_SERVICE_RATE
   C = MDI_SEQUENCES
   D = MDI_DEFAULT_VOLUME
   E = MDI_QUANT_ADVANCE
   F = MDI_ALLOW_LOOP_BRANCHING
0x10 = MDI_DEFAULT_BEND_RANGE
0x11 = ???
0x12 = ???
*/
}

int32_t Game_AIL_install_DIG_INI(void *dig)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_install_DIG_INI: 0x%x\n", (uint32_t) dig);
#endif
    return (Game_Sound)?0:1;
}

void Game_AIL_uninstall_DIG_driver(void *dig)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_uninstall_DIG_driver: 0x%x\n", (uint32_t) dig);
#endif
}

void Game_AIL_uninstall_MDI_driver(void *mdi)
{
#if defined(__DEBUG__)
    fprintf(stderr, "AIL_uninstall_MDI_driver: 0x%x\n", (uint32_t) mdi);
#endif
}

/*
AIL_mem_use_malloc
AIL_mem_use_free
AIL_startup
AIL_register_timer
AIL_set_timer_frequency
AIL_start_timer
AIL_stop_timer
AIL_release_timer_handle
AIL_shutdown
AIL_set_GTL_filename_prefix
AIL_install_MDI_INI
AIL_set_preference
AIL_install_DIG_INI
AIL_uninstall_DIG_driver
AIL_uninstall_MDI_driver
*/
