/**
 *
 *  Copyright (C) 2016-2019 Roman Pauer
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

#if !defined(_UDIS86_DEP_H_INCLUDED_)
#define _UDIS86_DEP_H_INCLUDED_

#include <udis86.h>
#include "SR_defs.h"

#define MAX_MNEMONICS (UD_MAX_MNEMONIC_CODE)

#if defined(DEFINE_VARIABLES)
	#define EXTERNAL_VARIABLE
#else
	#define EXTERNAL_VARIABLE extern
#endif

typedef enum _x86_flags_ {
	FL_UNKNOWN   = 0xffffffff,

	FL_NONE      = 0,

	FL_CARRY     = 0x01,
	FL_PARITY    = 0x02,
	FL_ADJUST    = 0x04,
	FL_ZERO      = 0x08,
	FL_SIGN      = 0x10,
	FL_OVERFLOW  = 0x20,
	FL_DIRECTION = 0x40,
	FL_INTERRUPT = 0x80,
	FL_TASK      = 0x0100,

	FL_AH        = FL_CARRY | FL_PARITY | FL_ADJUST | FL_ZERO | FL_SIGN,
	FL_COND_SUB  = FL_CARRY | FL_PARITY | FL_ADJUST | FL_ZERO | FL_SIGN | FL_OVERFLOW,
	FL_COND      = FL_CARRY | FL_PARITY | FL_ADJUST | FL_ZERO | FL_SIGN | FL_OVERFLOW,
	FL_COND_ALL  = FL_CARRY | FL_PARITY | FL_ADJUST | FL_ZERO | FL_SIGN | FL_OVERFLOW,
	FL_ALL       = FL_CARRY | FL_PARITY | FL_ADJUST | FL_ZERO | FL_SIGN | FL_OVERFLOW | FL_DIRECTION | FL_INTERRUPT | FL_TASK,

	FL_WEAK      = 0x10000000,
	FL_SPECIFIC  = 0x80000000
} x86_flags;

EXTERNAL_VARIABLE uint_fast32_t flags_needed[MAX_MNEMONICS];
EXTERNAL_VARIABLE uint_fast32_t flags_modified[MAX_MNEMONICS];

EXTERNAL_VARIABLE int init_udis86_dep(void);

extern const char* ud_reg_tab[];

#undef EXTERNAL_VARIABLE

#endif
