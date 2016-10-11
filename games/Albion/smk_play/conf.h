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

#ifndef _CONF_H_INCLUDED_
#define _CONF_H_INCLUDED_

#if defined(GP2X) || defined(PANDORA)
	#define IS_BIG_ENDIAN 0
#elif defined(__WIN32__)
	#define IS_BIG_ENDIAN 0
#else
	#define IS_BIG_ENDIAN 0
#endif

/* ************************************************************************** */
#define ASM_NOASM 0
#define ASM_X86 1
#define ASM_ARMLE 2
#define ASM_ARMBE 3

#if defined(GP2X)
	#define USE_ASM ASM_ARMLE
#elif defined(__WIN32__)
	#define USE_ASM ASM_X86
#else
	#define USE_ASM ASM_NOASM
#endif

/* ************************************************************************** */

#if (IS_BIG_ENDIAN == 0)
	#define LE2NATIVE32(x) (x)
	#define LE2NATIVE16(x) (x)
#else
	#define LE2NATIVE32(x) (  ((x) << 24)               | \
	                         (((x) <<  8) & 0x00ff0000) | \
	                         (((x) >>  8) & 0x0000ff00) | \
	                          ((x) >> 24)               )

	#define LE2NATIVE16(x) ( ((x) <<  8)  | \
	                         ((x) >>  8)  )
#endif

#endif /* _CONF_H_INCLUDED_ */
