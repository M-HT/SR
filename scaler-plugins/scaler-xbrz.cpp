/**
 *
 *  Copyright (C) 2021-2023 Roman Pauer
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

#include "scaler-plugins.h"
#include "xbrz/xbrz.h"

static void scale(int factor, const void *src, void *dst, int src_width, int src_height, int y_first, int y_last)
{
    xbrz::scale(factor, (const uint32_t *)src, (uint32_t *)dst, src_width, src_height, xbrz::ColorFormat::RGB, xbrz::ScalerCfg(), y_first, y_last);
}

static int get_maximum_scale_factor(void)
{
    return xbrz::SCALE_FACTOR_MAX;
}

static void shutdown_plugin(void)
{
}

extern "C"
__attribute__ ((visibility ("default")))
int initialize_scaler_plugin(scaler_plugin_functions *functions)
{
    uint32_t src[8*8], dst[8*2*8*2];

    if (functions == NULL) return -2;

    // one-time buffer creation
    xbrz::scale(2, src, dst, 8, 8, xbrz::ColorFormat::RGB);

    functions->scale = &scale;
    functions->get_maximum_scale_factor = &get_maximum_scale_factor;
    functions->shutdown_plugin = &shutdown_plugin;

    return 0;
}

