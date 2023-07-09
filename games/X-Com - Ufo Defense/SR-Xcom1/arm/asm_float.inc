@part of static recompiler -- do not edit

@@
@@  Copyright (C) 2016 Roman Pauer
@@
@@  Permission is hereby granted, free of charge, to any person obtaining a copy of
@@  this software and associated documentation files (the "Software"), to deal in
@@  the Software without restriction, including without limitation the rights to
@@  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
@@  of the Software, and to permit persons to whom the Software is furnished to do
@@  so, subject to the following conditions:
@@
@@  The above copyright notice and this permission notice shall be included in all
@@  copies or substantial portions of the Software.
@@
@@  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
@@  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
@@  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
@@  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
@@  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
@@  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
@@  SOFTWARE.
@@

.extern x87_facos_void

.extern x87_faabs_void

.extern x87_fadd_float
.extern x87_fadd_double
.extern x87_fadd_st
.extern x87_faddp_st

.extern x87_fasin_void

.extern x87_fatan2_void

.extern x87_fchs_void

.extern x87_fcomp_float
.extern x87_fcomp_double
.extern x87_fcompp_void

.extern x87_fcos_void

.extern x87_fdiv_float
.extern x87_fdiv_double
.extern x87_fdiv_st
.extern x87_fdivp_st
.extern x87_fdivr_float
.extern x87_fdivr_double
.extern x87_fdivr_st
.extern x87_fdivrp_st

.extern x87_fild_int32
.extern x87_fild_int64

.extern x87_fninit_void

.extern x87_fistp_int32
.extern x87_fistp_int64

.extern x87_fld_float
.extern x87_fld_double
.extern x87_fld_st
.extern x87_fld1_void
.extern x87_fldln2_void
.extern x87_fldz_void

.extern x87_fldcw_uint16

.extern x87_flog10_void

.extern x87_fmul_float
.extern x87_fmul_double
.extern x87_fmul_st
.extern x87_fmulp_st

.extern x87_fpow_void

.extern x87_fround_void

.extern x87_fsin_void

.extern x87_fsqrt_void

.extern x87_fst_float
.extern x87_fst_double
.extern x87_fstp_float
.extern x87_fstp_double
.extern x87_fstp_st

.extern x87_fnstcw_void
.extern x87_fnstsw_void

.extern x87_fsub_float
.extern x87_fsub_double
.extern x87_fsub_st
.extern x87_fsub_to_st
.extern x87_fsubp_st
.extern x87_fsubr_float
.extern x87_fsubr_double
.extern x87_fsubr_st
.extern x87_fsubrp_st

.extern x87_fxch_st

.extern x87_fyl2x_void
