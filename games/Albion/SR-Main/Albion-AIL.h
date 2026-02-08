/**
 *
 *  Copyright (C) 2018-2026 Roman Pauer
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

#if !defined(_ALBION_AIL_H_INCLUDED_)
#define _ALBION_AIL_H_INCLUDED_

#include "Game_defs.h"

#define DIG_DEFAULT_VOLUME 5
#define MDI_DEFAULT_VOLUME 0x0d

#ifdef __cplusplus
extern "C" {
#endif

extern void * CCALL Game_AIL_mem_use_malloc(void * (*fn)(uint32_t));
extern void * CCALL Game_AIL_mem_use_free(void (*fn)(void *));
extern void CCALL Game_AIL_startup(void);
extern int32_t CCALL Game_AIL_register_timer(void (*callback_fn)(uint32_t user));
extern void CCALL Game_AIL_set_timer_frequency(int32_t timer, uint32_t hertz);
extern void CCALL Game_AIL_start_timer(int32_t timer);
extern void CCALL Game_AIL_stop_timer(int32_t timer);
extern void CCALL Game_AIL_release_timer_handle(int32_t timer);
extern void CCALL Game_AIL_shutdown(void);
extern void CCALL Game_AIL_set_GTL_filename_prefix(const char *prefix);
extern int32_t CCALL Game_AIL_install_MDI_INI(void *mdi);
extern int32_t CCALL Game_AIL_set_preference(uint32_t number, int32_t value);
extern int32_t CCALL Game_AIL_install_DIG_INI(void *dig);
extern void CCALL Game_AIL_uninstall_DIG_driver(void *dig);
extern void CCALL Game_AIL_uninstall_MDI_driver(void *mdi);

#ifdef __cplusplus
}
#endif

#endif /* _ALBION_AIL_H_INCLUDED_ */
