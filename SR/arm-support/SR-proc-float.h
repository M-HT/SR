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

#if !defined(_SR_PROC_FLOAT_H_INCLUDED_)
#define _SR_PROC_FLOAT_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

extern void fabs_void(void);

extern void facos_void(void);

extern void fadd_float(float_int num);
extern void fadd_double(double_int num);
extern void fadd_st(int num);
extern void faddp_st(int num);

extern void fasin_void(void);

extern void fatan2_void(void);

extern void fchs_void(void);

extern void fcomp_float(float_int num);
extern void fcomp_double(double_int num);
extern void fcompp_void(void);

extern void fcos_void(void);

extern void fdiv_float(float_int num);
extern void fdiv_double(double_int num);
extern void fdiv_st(int num);
extern void fdivp_st(int num);
extern void fdivr_float(float_int num);
extern void fdivr_double(double_int num);
extern void fdivr_st(int num);
extern void fdivrp_st(int num);

extern void fild_int32(int32_t num);
extern void fild_int64(int64_t num);

extern void fninit_void(void);

extern int32_t fistp_int32(void);
extern int64_t fistp_int64(void);

extern void fld_float(float_int num);
extern void fld_double(double_int num);
extern void fld_st(int num);
extern void fld1_void(void);
extern void fldln2_void(void);
extern void fldz_void(void);

extern void fldcw_uint16(uint16_t new_cw);

extern void flog10_void(void);

extern void fmul_float(float_int num);
extern void fmul_double(double_int num);
extern void fmul_st(int num);
extern void fmulp_st(int num);

extern void fpow_void(void);

extern void fround_void(void);

extern void fsin_void(void);

extern void fsqrt_void(void);

extern int32_t fst_float(void);
extern int64_t fst_double(void);
extern int32_t fstp_float(void);
extern int64_t fstp_double(void);
extern void fstp_st(int num);

extern uint32_t fnstcw_void(void);
extern uint32_t fnstsw_void(void);

extern void fsub_float(float_int num);
extern void fsub_double(double_int num);
extern void fsub_st(int num);
extern void fsub_to_st(int num);
extern void fsubp_st(int num);
extern void fsubr_float(float_int num);
extern void fsubr_double(double_int num);
extern void fsubr_st(int num);
extern void fsubrp_st(int num);

extern void fxch_st(int num);

extern void fyl2x_void(void);

#ifdef __cplusplus
}
#endif

#endif /* _SR_PROC_FLOAT_H_INCLUDED_ */
