// part of static recompiler -- do not edit

/**
 *
 *  Copyright (C) 2016 Roman Pauer
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

#if !defined(_SR_PROC_DIVIDE_H_INCLUDED_)
#define _SR_PROC_DIVIDE_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

extern uint64_t div_64(uint64_t dividend, uint32_t divisor);
extern uint64_t div_32(uint32_t dividend, uint32_t divisor);

extern uint64_t idiv_64(int64_t dividend, int32_t divisor);
extern uint64_t idiv_32(int32_t dividend, int32_t divisor);

#ifdef __cplusplus
}
#endif

#endif /* _SR_PROC_DIVIDE_H_INCLUDED_ */
