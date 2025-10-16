/**
 *
 *  Copyright (C) 2021-2025 Roman Pauer
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

#include <stdint.h>
#include <string.h>
#include "scaler-plugins.h"
#include "hqx/hqx.h"

#ifdef _MSC_VER
    #define EXPORT __declspec(dllexport)
#elif defined __GNUC__
    #define EXPORT __attribute__ ((visibility ("default")))
#else
    #define EXPORT
#endif

static void scale(int factor, const void *src, void *dst, int src_width, int src_height, int y_first, int y_last)
{
    if (y_first < 0) y_first = 0;
    if (y_last > src_height) y_last = src_height;
    if ((y_first >= y_last) || (src_width <= 0)) return;

    switch (factor)
    {
        case 1:
            memcpy((void *) ((uintptr_t)dst + src_width * y_first * sizeof(uint32_t)), (void *) ((uintptr_t)src + src_width * y_first * sizeof(uint32_t)), src_width * (y_last - y_first) * sizeof(uint32_t));
            break;
        case 2:
            hq2x_32((const uint32_t *)src, (uint32_t *)dst, src_width, sizeof(uint32_t) * src_width, 2 * sizeof(uint32_t) * src_width, src_height, y_first, y_last);
            break;
        case 3:
            hq3x_32((const uint32_t *)src, (uint32_t *)dst, src_width, sizeof(uint32_t) * src_width, 3 * sizeof(uint32_t) * src_width, src_height, y_first, y_last);
            break;
        case 4:
            hq4x_32((const uint32_t *)src, (uint32_t *)dst, src_width, sizeof(uint32_t) * src_width, 4 * sizeof(uint32_t) * src_width, src_height, y_first, y_last);
            break;
        default:
            break;
    }
}

static int get_maximum_scale_factor(void)
{
    return 4;
}

static void shutdown_plugin(void)
{
}

EXPORT
int initialize_scaler_plugin(scaler_plugin_functions *functions)
{
    if (functions == NULL) return -2;

    functions->scale = &scale;
    functions->get_maximum_scale_factor = &get_maximum_scale_factor;
    functions->shutdown_plugin = &shutdown_plugin;

    return 0;
}

