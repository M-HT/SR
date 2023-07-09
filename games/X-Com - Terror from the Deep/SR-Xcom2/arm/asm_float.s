@part of static recompiler -- do not edit

@@
@@  Copyright (C) 2016-2019 Roman Pauer
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

.include "arm.inc"
.include "armconf.inc"
.include "asm_float_macros.inc"


.extern fabs_void

.extern facos_void

.extern fadd_float
.extern fadd_double
.extern fadd_st
.extern faddp_st

.extern fasin_void

.extern fatan2_void

.extern fchs_void

.extern fcomp_float
.extern fcomp_double
.extern fcompp_void

.extern fcos_void

.extern fdiv_float
.extern fdiv_double
.extern fdiv_st
.extern fdivp_st
.extern fdivr_float
.extern fdivr_double
.extern fdivr_st
.extern fdivrp_st

.extern fild_int32
.extern fild_int64

.extern fninit_void

.extern fistp_int32
.extern fistp_int64

.extern fld_float
.extern fld_double
.extern fld_st
.extern fld1_void
.extern fldln2_void
.extern fldz_void

.extern fldcw_uint16

.extern flog10_void

.extern fmul_float
.extern fmul_double
.extern fmul_st
.extern fmulp_st

.extern fpow_void

.extern fround_void

.extern fsin_void

.extern fsqrt_void

.extern fst_float
.extern fst_double
.extern fstp_float
.extern fstp_double
.extern fstp_st

.extern fnstcw_void
.extern fnstsw_void

.extern fsub_float
.extern fsub_double
.extern fsub_st
.extern fsub_to_st
.extern fsubp_st
.extern fsubr_float
.extern fsubr_double
.extern fsubr_st
.extern fsubrp_st

.extern fxch_st

.extern fyl2x_void

.section .note.GNU-stack,"",%progbits
.section .text

.global x87_fabs_void

.global x87_facos_void

.global x87_fadd_float
.global x87_fadd_double
.global x87_fadd_st
.global x87_faddp_st

.global x87_fasin_void

.global x87_fatan2_void

.global x87_fchs_void

.global x87_fcomp_float
.global x87_fcomp_double
.global x87_fcompp_void

.global x87_fcos_void

.global x87_fdiv_float
.global x87_fdiv_double
.global x87_fdiv_st
.global x87_fdivp_st
.global x87_fdivr_float
.global x87_fdivr_double
.global x87_fdivr_st
.global x87_fdivrp_st

.global x87_fild_int32
.global x87_fild_int64

.global x87_fninit_void

.global x87_fistp_int32
.global x87_fistp_int64

.global x87_fld_float
.global x87_fld_double
.global x87_fld_st
.global x87_fld1_void
.global x87_fldln2_void
.global x87_fldz_void

.global x87_fldcw_uint16

.global x87_flog10_void

.global x87_fmul_float
.global x87_fmul_double
.global x87_fmul_st
.global x87_fmulp_st

.global x87_fpow_void

.global x87_fround_void

.global x87_fsin_void

.global x87_fsqrt_void

.global x87_fst_float
.global x87_fst_double
.global x87_fstp_float
.global x87_fstp_double
.global x87_fstp_st

.global x87_fnstcw_void
.global x87_fnstsw_void

.global x87_fsub_float
.global x87_fsub_double
.global x87_fsub_st
.global x87_fsub_to_st
.global x87_fsubp_st
.global x87_fsubr_float
.global x87_fsubr_double
.global x87_fsubr_st
.global x87_fsubrp_st

.global x87_fxch_st

.global x87_fyl2x_void

x87_facos_void:

	Call_void_proc_void facos_void

@ end procedure x87_facos_void

x87_fabs_void:

	Call_void_proc_void fabs_void

@ end procedure x87_fabs_void

x87_fadd_float:

	Call_void_proc_int32 fadd_float

@ end procedure x87_fadd_float

x87_fadd_double:

	Call_void_proc_int64 fadd_double

@ end procedure x87_fadd_double

x87_fadd_st:

	Call_void_proc_int32 fadd_st

@ end procedure x87_fadd_st

x87_faddp_st:

	Call_void_proc_int32 faddp_st

@ end procedure x87_faddp_st

x87_fasin_void:

	Call_void_proc_void fasin_void

@ end procedure x87_fasin_void

x87_fatan2_void:

	Call_void_proc_void fatan2_void

@ end procedure x87_fatan2_void

x87_fchs_void:

	Call_void_proc_void fchs_void

@ end procedure x87_fchs_void

x87_fcomp_float:

	Call_void_proc_int32 fcomp_float

@ end procedure x87_fcomp_float

x87_fcomp_double:

	Call_void_proc_int64 fcomp_double

@ end procedure x87_fcomp_double

x87_fcompp_void:

	Call_void_proc_void fcompp_void

@ end procedure x87_fcompp_void

x87_fcos_void:

	Call_void_proc_void fcos_void

@ end procedure x87_fcos_void

x87_fdiv_float:

	Call_void_proc_int32 fdiv_float

@ end procedure x87_fdiv_float

x87_fdiv_double:

	Call_void_proc_int64 fdiv_double

@ end procedure x87_fdiv_double

x87_fdiv_st:

	Call_void_proc_int32 fdiv_st

@ end procedure x87_fdiv_st

x87_fdivp_st:

	Call_void_proc_int32 fdivp_st

@ end procedure x87_fdivp_st

x87_fdivr_float:

	Call_void_proc_int32 fdivr_float

@ end procedure x87_fdivr_float

x87_fdivr_double:

	Call_void_proc_int64 fdivr_double

@ end procedure x87_fdivr_double

x87_fdivr_st:

	Call_void_proc_int32 fdivr_st

@ end procedure x87_fdivr_st

x87_fdivrp_st:

	Call_void_proc_int32 fdivrp_st

@ end procedure x87_fdivrp_st

x87_fild_int32:

	Call_void_proc_int32 fild_int32

@ end procedure x87_fild_int32

x87_fild_int64:

	Call_void_proc_int64 fild_int64

@ end procedure x87_fild_int64

x87_fninit_void:

	Call_void_proc_void fninit_void

@ end procedure x87_fninit_void

x87_fistp_int32:

	Call_int32_proc_void fistp_int32

@ end procedure x87_fistp_int32

x87_fistp_int64:

	Call_int64_proc_void fistp_int64

@ end procedure x87_fistp_int64

x87_fld_float:

	Call_void_proc_int32 fld_float

@ end procedure x87_fld_float

x87_fld_double:

	Call_void_proc_int64 fld_double

@ end procedure x87_fld_double

x87_fld_st:

	Call_void_proc_int32 fld_st

@ end procedure x87_fld_st

x87_fld1_void:

	Call_void_proc_void fld1_void

@ end procedure x87_fld1_void

x87_fldln2_void:

	Call_void_proc_void fldln2_void

@ end procedure x87_fldln2_void

x87_fldz_void:

	Call_void_proc_void fldz_void

@ end procedure x87_fldz_void

x87_fldcw_uint16:

	Call_void_proc_int32 fldcw_uint16

@ end procedure x87_fldcw_uint16

x87_flog10_void:

	Call_void_proc_void flog10_void

@ end procedure x87_flog10_void

x87_fmul_float:

	Call_void_proc_int32 fmul_float

@ end procedure x87_fmul_float

x87_fmul_double:

	Call_void_proc_int64 fmul_double

@ end procedure x87_fmul_double

x87_fmul_st:

	Call_void_proc_int32 fmul_st

@ end procedure x87_fmul_st

x87_fmulp_st:

	Call_void_proc_int32 fmulp_st

@ end procedure x87_fmulp_st

x87_fpow_void:

	Call_void_proc_void fpow_void

@ end procedure x87_fpow_void

x87_fround_void:

	Call_void_proc_void fround_void

@ end procedure x87_fround_void

x87_fsin_void:

	Call_void_proc_void fsin_void

@ end procedure x87_fsin_void

x87_fsqrt_void:

	Call_void_proc_void fsqrt_void

@ end procedure x87_fsqrt_void

x87_fst_float:

	Call_int32_proc_void fst_float

@ end procedure x87_fst_float

x87_fst_double:

	Call_int64_proc_void fst_double

@ end procedure x87_fst_double

x87_fstp_float:

	Call_int32_proc_void fstp_float

@ end procedure x87_fstp_float

x87_fstp_double:

	Call_int64_proc_void fstp_double

@ end procedure x87_fstp_double

x87_fstp_st:

	Call_void_proc_int32 fstp_st

@ end procedure x87_fstp_st

x87_fnstcw_void:

	Call_int32_proc_void fnstcw_void

@ end procedure x87_fnstcw_void

x87_fnstsw_void:

	Call_int32_proc_void fnstsw_void

@ end procedure x87_fnstsw_void

x87_fsub_float:

	Call_void_proc_int32 fsub_float

@ end procedure x87_fsub_float

x87_fsub_double:

	Call_void_proc_int64 fsub_double

@ end procedure x87_fsub_double

x87_fsub_st:

	Call_void_proc_int32 fsub_st

@ end procedure x87_fsub_st

x87_fsub_to_st:

	Call_void_proc_int32 fsub_to_st

@ end procedure x87_fsub_to_st

x87_fsubp_st:

	Call_void_proc_int32 fsubp_st

@ end procedure x87_fsubp_st

x87_fsubr_float:

	Call_void_proc_int32 fsubr_float

@ end procedure x87_fsubr_float

x87_fsubr_double:

	Call_void_proc_int64 fsubr_double

@ end procedure x87_fsubr_double

x87_fsubr_st:

	Call_void_proc_int32 fsubr_st

@ end procedure x87_fsubr_st

x87_fsubrp_st:

	Call_void_proc_int32 fsubrp_st

@ end procedure x87_fsubrp_st

x87_fxch_st:

	Call_void_proc_int32 fxch_st

@ end procedure x87_fxch_st

x87_fyl2x_void:

	Call_void_proc_void fyl2x_void

@ end procedure x87_fyl2x_void
