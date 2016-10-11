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

#include <stdint.h>

typedef union {
	struct {
	    int32_t quotient;
	    int32_t remainder;
	} r;
	uint64_t i;
} result_int;

typedef union {
	struct {
	    uint32_t quotient;
	    uint32_t remainder;
	} r;
	uint64_t i;
} result_uint;


#include "SR-proc-divide.h"


uint64_t div_64(uint64_t dividend, uint32_t divisor)
{
	register result_uint ret;

	ret.r.quotient = (uint32_t) (dividend / divisor);
	ret.r.remainder = (uint32_t) (dividend % divisor);

	return ret.i;
}

uint64_t div_32(uint32_t dividend, uint32_t divisor)
{
	register result_uint ret;

	ret.r.quotient = dividend / divisor;
	ret.r.remainder = dividend % divisor;

	return ret.i;
}


uint64_t idiv_64(int64_t dividend, int32_t divisor)
{
	register result_int ret;

	ret.r.quotient = (int32_t) (dividend / divisor);
	ret.r.remainder = (int32_t) (dividend % divisor);

	return ret.i;
}

uint64_t idiv_32(int32_t dividend, int32_t divisor)
{
	register result_int ret;

	ret.r.quotient = dividend / divisor;
	ret.r.remainder = dividend % divisor;

	return ret.i;
}

