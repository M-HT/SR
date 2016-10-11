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
#if !defined(__USE_ISOC99)
    #define __USE_ISOC99
#endif
#include <math.h>

typedef union {
	float f;
	int32_t i;
} float_int;

typedef union {
	double d;
	int64_t i;
} double_int;


#include "SR-proc-float.h"

typedef double fp_format;

const static fp_format const_0_0 = 0.0;
const static fp_format const_1_0 = 1.0;
const static fp_format const_ln2 = M_LN2;

static fp_format st[8];

static unsigned int st_top = 0;
static unsigned int st_sw_cond = 0;
static unsigned int st_cw = 0x037f;

#define X87_CF 0x0100
#define X87_ZF 0x4000

#define X87_C0 0x0100
#define X87_C1 0x0200
#define X87_C2 0x0400
#define X87_C3 0x4000

#define X87_CX 0x4700

#define CLEAR_X87_FLAG(x) { st_sw_cond &= (~(x)) & X87_CX; }

#define CLEAR_X87_FLAGS { st_sw_cond = 0; }

#define X87_CMP(x, y)  { \
	if ( (x) < (y) ) { \
		st_sw_cond = X87_CF; \
	} else if ( (x) == (y) ) { \
		st_sw_cond = X87_ZF; \
	} else { \
		st_sw_cond = 0; \
	} \
}

#define ST(i) st[(st_top+i) & 7]
#define ST0 st[st_top]
#define ST1 ST(1)


#define PUSH_REGS { { CLEAR_X87_FLAG(X87_C1); st_top = (st_top - 1) & 7; } }
#define POP_REGS { st_top = (st_top + 1) & 7; CLEAR_X87_FLAG(X87_C1); }
#define POP2_REGS { st_top = (st_top + 2) & 7; CLEAR_X87_FLAG(X87_C1); }


void facos_void(void)
{
	ST0 = acos(ST0);
}

void fabs_void(void)
{
	ST0 = fabs(ST0);
	CLEAR_X87_FLAGS;
}

void fadd_float(float_int num)
{
	ST0 += num.f;
	CLEAR_X87_FLAGS;
}

void fadd_double(double_int num)
{
	ST0 += num.d;
	CLEAR_X87_FLAGS;
}

void fadd_st(int num)
{
	ST0 += ST(num);
	CLEAR_X87_FLAGS;
}

void faddp_st(int num)
{
	ST(num) += ST0;
	POP_REGS;
}

void fasin_void(void)
{
	ST0 = asin(ST0);
}

void fatan2_void(void)
{
	ST1 = atan2(ST0, ST1);
	POP_REGS;
}

void fchs_void(void)
{
	ST0 = -ST0;
	CLEAR_X87_FLAGS;
}

void fcomp_float(float_int num)
{
	X87_CMP(ST0, num.f)
	POP_REGS;
}

void fcomp_double(double_int num)
{
	X87_CMP(ST0, num.d)
	POP_REGS;
}

void fcompp_void(void)
{
	X87_CMP(ST0, ST1)
	POP2_REGS;
}

void fcos_void(void)
{
	ST0 = cos(ST0);
	CLEAR_X87_FLAGS;
}

void fdiv_float(float_int num)
{
	ST0 /= num.f;
	CLEAR_X87_FLAGS;
}

void fdiv_double(double_int num)
{
	ST0 /= num.d;
	CLEAR_X87_FLAGS;
}

void fdiv_st(int num)
{
	ST0 /= ST(num);
	CLEAR_X87_FLAGS;
}

void fdivp_st(int num)
{
	ST(num) /= ST0;
	POP_REGS;
}

void fdivr_float(float_int num)
{
	ST0 = num.f / ST0;
	CLEAR_X87_FLAGS;
}

void fdivr_double(double_int num)
{
	ST0 = num.d / ST0;
	CLEAR_X87_FLAGS;
}

void fdivr_st(int num)
{
	ST0 = ST(num) / ST0;
}

void fdivrp_st(int num)
{
	ST(num) = ST0 / ST(num);
	POP_REGS;
}

void fild_int32(int32_t num)
{
	PUSH_REGS;
	ST0 = num;
}

void fild_int64(int64_t num)
{
	PUSH_REGS;
	ST0 = num;
}

void fninit_void(void)
{
    st_top = 0;
    st_sw_cond = 0;
    st_cw = 0x037f;
}

int32_t fistp_int32(void)
{
	register int32_t ret;

	ret = (int32_t) ST0;
	POP_REGS;
	return ret;
}

int64_t fistp_int64(void)
{
	register int64_t ret;

	ret = (int64_t) ST0;
	POP_REGS;
	return ret;
}

void fld_float(float_int num)
{
	PUSH_REGS;
	ST0 = num.f;
}

void fld_double(double_int num)
{
	PUSH_REGS;
	ST0 = num.d;
}

void fld_st(int num)
{
	register fp_format newval;

	newval = ST(num);
	PUSH_REGS;
	ST0 = newval;
}

void fld1_void(void)
{
	PUSH_REGS;
	ST0 = const_1_0;
}

void fldln2_void(void)
{
	PUSH_REGS;
	ST0 = const_ln2;
}

void fldz_void(void)
{
	PUSH_REGS;
	ST0 = const_0_0;
}

void fldcw_uint16(uint16_t new_cw)
{
    st_cw = (uint32_t) new_cw;
}

void flog10_void(void)
{
	ST0 = log10(ST0);
}

void fmul_float(float_int num)
{
	ST0 *= num.f;
	CLEAR_X87_FLAGS;
}

void fmul_double(double_int num)
{
	ST0 *= num.d;
	CLEAR_X87_FLAGS;
}

void fmul_st(int num)
{
	ST0 *= ST(num);
	CLEAR_X87_FLAGS;
}

void fmulp_st(int num)
{
	ST(num) *= ST0;
	POP_REGS;
}

void fpow_void(void)
{
	ST1 = pow(ST0, ST1);
	POP_REGS;
}

void fround_void(void)
{
	ST0 = round(ST0);
}

void fsin_void(void)
{
	ST0 = sin(ST0);
	CLEAR_X87_FLAGS;
}

void fsqrt_void(void)
{
	ST0 = sqrt(ST0);
}

int32_t fst_float(void)
{
	register float_int ret;

	CLEAR_X87_FLAGS;
	ret.f = ST0;

	return ret.i;
}

int64_t fst_double(void)
{
	register double_int ret;

	CLEAR_X87_FLAGS;
	ret.d = ST0;

	return ret.i;
}

int32_t fstp_float(void)
{
	register float_int ret;

	ret.f = ST0;
	POP_REGS;
	return ret.i;
}

int64_t fstp_double(void)
{
	register double_int ret;

	ret.d = ST0;
	POP_REGS;
	return ret.i;
}

void fstp_st(int num)
{
	ST(num) = ST0;
	POP_REGS;
}

uint32_t fnstcw_void(void)
{
	return st_cw;
}

uint32_t fnstsw_void(void)
{
	return st_sw_cond | (st_top << 11);
}

void fsub_float(float_int num)
{
	ST0 -= num.f;
	CLEAR_X87_FLAGS;
}

void fsub_double(double_int num)
{
	ST0 -= num.d;
	CLEAR_X87_FLAGS;
}

void fsub_st(int num)
{
	ST0 -= ST(num);
	CLEAR_X87_FLAGS;
}

void fsub_to_st(int num)
{
	ST(num) -= ST0;
	CLEAR_X87_FLAGS;
}

void fsubp_st(int num)
{
	ST(num) -= ST0;
	POP_REGS;
}

void fsubr_float(float_int num)
{
	ST0 = num.f - ST0;
	CLEAR_X87_FLAGS;
}

void fsubr_double(double_int num)
{
	ST0 = num.d - ST0;
	CLEAR_X87_FLAGS;
}

void fsubr_st(int num)
{
	ST0 = ST(num) - ST0;
}

void fsubrp_st(int num)
{
	ST(num) = ST0 - ST(num);
	POP_REGS;
}

void fxch_st(int num)
{
	register fp_format tmpst;

	tmpst = ST0;
	ST0 = ST(num);
	ST(num) = tmpst;
	CLEAR_X87_FLAGS;
}

void fyl2x_void(void)
{
	ST1 *= log2(ST0);
	POP_REGS;
}

