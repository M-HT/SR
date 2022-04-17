/**
 *
 *  Copyright (C) 2022 Roman Pauer
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

#if !defined(_PTR32_H_INCLUDED_)
#define _PTR32_H_INCLUDED_

#include <stdint.h>

typedef uint32_t ptr32_t;
#define PTR32(t) ptr32_t

#define FROMPTR(x) ((ptr32_t)(uintptr_t)(x))
#define TOPTR_0(x) ((void *)(uintptr_t)(x))
#define TOPTR_8(x) ((uint8_t *)(uintptr_t)(x))
#define TOPTR_T(t,x) ((t *)(uintptr_t)(x))

/**
 * Variables
 * ---------
 *
 * - variables defined in asm code which are used in c code:
 * -- don't define them as pointers, but as ptr32_t or PTR32(pointer type)
 * --- when writing a pointer value use macro FROMPTR
 * --- when read a pointer value use macros TOPTR_?
 * -- don't define them as types with inexact size, but as types with exact size
 * -- instead of variable definitions like this:
 *    extern void *asm_var1;
 *    extern int asm_var2;
 * -- use variable definitions like this:
 *    extern ptr32_t asm_var1;
 *    extern int32_t asm_var2;
 *
 * - variables defined in c code which are used in asm code:
 * -- the same rules apply
 *
 * Structure/Union members
 * -----------------
 *
 * - structure/union members in structures/unions which are used in asm code:
 * -- the same rules apply as for variables
 * -- the exceptions are opaque structures/unions (opaque parts of structures/unions)
 * --- which means the structure/union size is determined in c code and the structure/union (part of the structure/union) isn't used in asm code
 *
 * Function parameters
 * -------------------
 *
 * - function parameters in functions defined in c code which are called from asm code:
 * -- can be defined as pointers, but only one level pointers (i.e. not pointer to pointer)
 * -- don't define them as pointers to types with inexact size, but as pointers to types with exact size
 * -- for two (or more) level pointers, the same rules apply as for variables
 * -- define non-pointer parameters as 32-bit (int32_t/uint32_t)
 * --- if smaller size is required, handle it inside function
 * -- instead of parameter definitions like this:
 *    void **param1
 *    int *param2
 *    int param3
 * -- use parameter definitions like this:
 *    ptr32_t *param1
 *    int32_t *param2
 *    int32_t param3
 *
 * - functions with ... or va_list parameter are handled specially in llasm
 * -- the .../va_list parameter is defined as uint32_t *
 */

#endif /* _PTR32_H_INCLUDED_ */

