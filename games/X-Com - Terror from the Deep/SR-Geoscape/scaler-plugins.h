/**
 *
 *  Copyright (C) 2021 Roman Pauer
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

#if !defined(_SCALER_PLUGINS_H_INCLUDED_)
#define _SCALER_PLUGINS_H_INCLUDED_

typedef struct _scaler_plugin_functions_ {
    void (*scale) (int factor, const void *src, void *dst, int src_width, int src_height, int y_first, int y_last);
    int (*get_maximum_scale_factor) (void);
    void (*shutdown_plugin) (void);
} scaler_plugin_functions;

typedef int (*scaler_plugin_initialize)(scaler_plugin_functions *functions);

#define SCALER_PLUGIN_INITIALIZE "initialize_scaler_plugin"

#endif /* _SCALER_PLUGINS_H_INCLUDED_ */

