/**
 *
 *  Copyright (C) 2016-2023 Roman Pauer
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

#include <math.h>
#include "Game_defs.h"
#include "Game_vars.h"
#include "Albion-proc-float.h"

//const static double dconst_360_0 = 360.0;
const static double dconst_2_0 = 2.0;
const static double dconst_pi = M_PI;
const static double dconst_11_0 = 11.0;
const static double dconst_12_0 = 12.0;
const static double dconst_100_0 = 100.0;
const static double dconst_16384_0 = 16384.0;
const static double dconst_180_0 = 180.0;

void Game_FloatProcSin(int32_t *ValuePtr)
{
//	x = sin((double) x * 2 * pi / 360) * 11 + 12

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sin - %i\n", *ValuePtr);
#endif

//	*ValuePtr = (int32_t) ( ( sin( ((double) (*ValuePtr) * dconst_2_0 * dconst_pi) / dconst_360_0 ) * dconst_11_0 ) + dconst_12_0 );
// faster version
	*ValuePtr = (int32_t) ( ( sin( ((double) (*ValuePtr) * dconst_pi) / dconst_180_0 ) * dconst_11_0 ) + dconst_12_0 );
}

void Game_FloatProcSin2(int32_t *ValuePtr)
{
//	x1 = sin((double) x0 * 2 * pi / 360) * 100

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sin2 - %i\n", ValuePtr[0]);
#endif

//	ValuePtr[1] = (int32_t) ( sin( ((double) (ValuePtr[0]) * dconst_2_0 * dconst_pi) / dconst_360_0 ) * dconst_100_0 );
// faster version
	ValuePtr[1] = (int32_t) ( sin( ((double) (ValuePtr[0]) * dconst_pi) / dconst_180_0 ) * dconst_100_0 );
}

void Game_FloatProcSin3(int32_t *ValuePtr)
{
//	a = x3
//	y1 = (double)a18
//	x0 = 2 * z7
//	yt1 = (double)x0 * pi
//	x0 = z8
//	x5 = sin(yt1 / (double)x0) * y1

#pragma pack(2)

    typedef struct {
        int32_t result2;
        double result1;
        PTR32(uint8_t) ptr1;
        int32_t reserved1;
        int32_t result3;
        int32_t reserved2;
        uint16_t op1;
        int16_t reserved3;
        uint16_t op2;
    } value_struct;

#pragma pack()

#define VALUES ((value_struct *) ValuePtr)

//	uint8_t *a;
	double yt1;

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sin3\n");
#endif

//	a = (uint8_t *) ValuePtr[3];
//	*((double *) (&(ValuePtr[1]))) = (double) ( (int32_t) ( ((uint32_t) a[18]) | ( ((uint32_t) a[19]) << 8 ) | ( ((uint32_t) a[20]) << 16 ) | ( ((uint32_t) a[21]) << 24 ) ) );
//	ValuePtr[0] = (int32_t) (( (uint32_t) *((uint16_t *) &(ValuePtr[7])) ) * 2);
//	yt1 = ( (double) ValuePtr[0] ) * dconst_pi;
//	ValuePtr[0] = (int32_t) ( (uint32_t) *((uint16_t *) &(ValuePtr[8]))) ;
//	ValuePtr[5] = (int32_t) ( sin( yt1 / ( (double) ValuePtr[0] ) ) * ( *((double *) (&(ValuePtr[1]))) ) );
    {
        uint8_t *a;

        a = VALUES->ptr1 + 18;
        VALUES->result1 = (double) ( (int32_t) ( ((uint32_t) a[0]) | ( ((uint32_t) a[1]) << 8 ) | ( ((uint32_t) a[2]) << 16 ) | ( ((uint32_t) a[3]) << 24 ) ) );
    }
    VALUES->result2 = (int32_t) (((uint32_t) (VALUES->op1)) * 2);
	yt1 = ((double) (VALUES->result2)) * dconst_pi;
    VALUES->result2 = (int32_t) ( ((uint32_t) (VALUES->op2)) );
	VALUES->result3 = (int32_t) ( sin( yt1 / ((double) (VALUES->result2)) ) * VALUES->result1 );

#undef VALUES
}

void Game_FloatProcSin4(int32_t *ValuePtr)
{
//	a = x3
//	y1 = (double)a18
//	x0 = 2 * z8
//	yt1 = (double)x0 * pi
//	x0 = z9
//	x5 = sin(yt1 / (double)x0) * y1

#pragma pack(2)

    typedef struct {
        int32_t result2;
        double result1;
        PTR32(uint8_t) ptr1;
        int32_t reserved1;
        int32_t result3;
        int32_t reserved2[2];
        uint16_t op1;
        int16_t reserved3;
        uint16_t op2;
    } value_struct;

#pragma pack()

#define VALUES ((value_struct *) ValuePtr)

//	uint8_t *a;
	double yt1;

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sin4\n");
#endif

//	a = (uint8_t *) ValuePtr[3];
//	*((double *) (&(ValuePtr[1]))) = (double) ( (int32_t) ( ((uint32_t) a[18]) | ( ((uint32_t) a[19]) << 8 ) | ( ((uint32_t) a[20]) << 16 ) | ( ((uint32_t) a[21]) << 24 ) ) );
//	ValuePtr[0] = (int32_t) (( (uint32_t) *((uint16_t *) &(ValuePtr[8])) ) * 2);
//	yt1 = ( (double) ValuePtr[0] ) * dconst_pi;
//	ValuePtr[0] = (int32_t) ( (uint32_t) *((uint16_t *) &(ValuePtr[9]))) ;
//	ValuePtr[5] = (int32_t) ( sin( yt1 / ( (double) ValuePtr[0] ) ) * ( *((double *) (&(ValuePtr[1]))) ) );
    {
        uint8_t *a;

        a = VALUES->ptr1 + 18;
        VALUES->result1 = (double) ( (int32_t) ( ((uint32_t) a[0]) | ( ((uint32_t) a[1]) << 8 ) | ( ((uint32_t) a[2]) << 16 ) | ( ((uint32_t) a[3]) << 24 ) ) );
    }
    VALUES->result2 = (int32_t) (((uint32_t) (VALUES->op1)) * 2);
	yt1 = ((double) (VALUES->result2)) * dconst_pi;
    VALUES->result2 = (int32_t) ( ((uint32_t) (VALUES->op2)) );
	VALUES->result3 = (int32_t) ( sin( yt1 / ((double) (VALUES->result2)) ) * VALUES->result1 );

#undef VALUES
}

void Game_FloatProcCos(int32_t *ValuePtr)
{
//	x = 12 - cos((double) x * 2 * pi / 360) * 11

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Cos - %i\n", *ValuePtr);
#endif

//	*ValuePtr = (int32_t) ( dconst_12_0 - ( cos( ((double) (*ValuePtr) * dconst_2_0 * dconst_pi) / dconst_360_0 ) * dconst_11_0 ) );
// faster version
	*ValuePtr = (int32_t) ( dconst_12_0 - ( cos( ((double) (*ValuePtr) * dconst_pi) / dconst_180_0 ) * dconst_11_0 ) );
}

void Game_FloatProcCos2(int32_t *ValuePtr)
{
//	x2 = cos((double) x0 * 2 * pi / 360) * 100

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Cos2 - %i\n", ValuePtr[0]);
#endif

//	ValuePtr[2] = (int32_t) ( cos( ((double) (ValuePtr[0]) * dconst_2_0 * dconst_pi) / dconst_360_0 ) * dconst_100_0 );
// faster version
	ValuePtr[2] = (int32_t) ( cos( ((double) (ValuePtr[0]) * dconst_pi) / dconst_180_0 ) * dconst_100_0 );
}

void Game_FloatProcSqrt(int32_t *ValuePtr)
{
//	x4 = sqrt(sqr(x0) + sqr(x1))

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sqrt\n");
#endif

	ValuePtr[4] = (int32_t) sqrt( ( ((double) ValuePtr[0]) * ((double) ValuePtr[0]) ) + ( ((double) ValuePtr[1]) * ((double) ValuePtr[1]) ) );
}


void Game_FloatProcSqrt2(int32_t *ValuePtr)
{
//	x0 = sqrt(sqr(z3) + sqr(z2))

#pragma pack(2)

    typedef struct __attribute__ ((__packed__)) {
        int32_t result;
        int32_t reserved1;
        int16_t op2;
        int16_t reserved2;
        int16_t op1;
    } value_struct;

#pragma pack()

#define VALUES ((value_struct *) ValuePtr)

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sqrt2\n");
#endif

//	ValuePtr[0] = (int32_t) sqrt( ( ((double) *((int16_t *) &(ValuePtr[3])) ) * ((double) *((int16_t *) &(ValuePtr[3])) ) ) + ( ((double) *((int16_t *) &(ValuePtr[2]))) * ((double) *((int16_t *) &(ValuePtr[2]))) ) );
	VALUES->result = (int32_t) sqrt( ( ((double) (VALUES->op1) ) * ((double) (VALUES->op1) ) ) + ( ((double) (VALUES->op2) ) * ((double) (VALUES->op2) ) ) );

#undef VALUES
}

void Game_FloatProcSqrt3(int32_t *ValuePtr)
{
//	x2 = sqrt(sqr(x0) + sqr(x3))

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sqrt3 - %i - %i\n", ValuePtr[0], ValuePtr[3]);
#endif

	ValuePtr[2] = (int32_t) sqrt( ( ((double) ValuePtr[0]) * ((double) ValuePtr[0]) ) + ( ((double) ValuePtr[3]) * ((double) ValuePtr[3]) ) );
}

void Game_FloatProcSqrt4(int32_t *ValuePtr)
{
//	x0 = sqrt(sqr(z3) + sqr(z4))

#pragma pack(2)

    typedef struct __attribute__ ((__packed__)) {
        int32_t result;
        int32_t reserved1;
        int32_t reserved2;
        int16_t op1;
        int16_t reserved3;
        int16_t op2;
    } value_struct;

#pragma pack()

#define VALUES ((value_struct *) ValuePtr)

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sqrt4\n");
#endif

//	ValuePtr[0] = (int32_t) sqrt( ( ((double) *((int16_t *) &(ValuePtr[3])) ) * ((double) *((int16_t *) &(ValuePtr[3])) ) ) + ( ((double) *((int16_t *) &(ValuePtr[4]))) * ((double) *((int16_t *) &(ValuePtr[4]))) ) );
	VALUES->result = (int32_t) sqrt( ( ((double) (VALUES->op1) ) * ((double) (VALUES->op1) ) ) + ( ((double) (VALUES->op2) ) * ((double) (VALUES->op2) ) ) );

#undef VALUES
}

void Game_FloatProcSqrt5(int32_t *ValuePtr)
{
//	x1 = sqrt(sqr(x2) + sqr(x0))

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sqrt5 - %i - %i\n", ValuePtr[2], ValuePtr[0]);
#endif

	ValuePtr[1] = (int32_t) sqrt( ( ((double) ValuePtr[2]) * ((double) ValuePtr[2]) ) + ( ((double) ValuePtr[0]) * ((double) ValuePtr[0]) ) );
}

void Game_FloatProcSqrt6(int32_t *ValuePtr)
{
//	x3 = sqrt(sqr(x1) + sqr(x2) + sqr(x0))

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sqrt6\n");
#endif

	ValuePtr[3] = (int32_t) sqrt( ( ((double) ValuePtr[1]) * ((double) ValuePtr[1]) ) + ( ((double) ValuePtr[2]) * ((double) ValuePtr[2]) )  + ( ((double) ValuePtr[0]) * ((double) ValuePtr[0]) ) );
}

void Game_FloatProcSqrt7(int32_t *ValuePtr)
{
//	x4 = sqrt(sqr(x3) + sqr(x0))

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sqrt7 - %i - %i\n", ValuePtr[3], ValuePtr[0]);
#endif

	ValuePtr[4] = (int32_t) sqrt( ( ((double) ValuePtr[3]) * ((double) ValuePtr[3]) ) + ( ((double) ValuePtr[0]) * ((double) ValuePtr[0]) ) );
}

void Game_FloatProcSqrt8(int32_t *ValuePtr)
{
//	x0 = sqrt(sqr(x2) + sqr(x3) + sqr(x4))

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Sqrt8\n");
#endif

	ValuePtr[0] = (int32_t) sqrt( ( ((double) ValuePtr[2]) * ((double) ValuePtr[2]) ) + ( ((double) ValuePtr[3]) * ((double) ValuePtr[3]) )  + ( ((double) ValuePtr[4]) * ((double) ValuePtr[4]) ) );
}

void Game_FloatProcCosSin(int32_t *ValuePtr)
{
//	y3 = (double) x2 * 2 * pi / 360
//	y0 = cos(y3) * (double) x5
//	x7 = y0 - (sin(y3) * (double) x6)

#pragma pack(4)

    typedef struct {
        double result2;
        int32_t op1;
        double result1;
        int32_t op2;
        int32_t op3;
        int32_t result3;
    } value_struct;

#pragma pack()

#define VALUES ((value_struct *) ValuePtr)

#if defined(__DEBUG__)
	fprintf(stderr, "float proc CosSin\n");
#endif

//	*((double *) (&(ValuePtr[3]))) = ( (double) (ValuePtr[2]) * dconst_2_0 * dconst_pi ) / dconst_360_0;
// faster version
//	*((double *) (&(ValuePtr[3]))) = ( (double) (ValuePtr[2]) * dconst_pi ) / dconst_180_0;
//	*((double *) (&(ValuePtr[0]))) = cos( *((double *) (&(ValuePtr[3]))) ) * (double) ValuePtr[5];
//	ValuePtr[7] = (int32_t) ( *((double *) (&(ValuePtr[0]))) - ( sin( *((double *) (&(ValuePtr[3]))) ) * (double) ValuePtr[6] ) );
	VALUES->result1 = ( ((double) (VALUES->op1)) * dconst_pi ) / dconst_180_0;
	VALUES->result2 = cos(VALUES->result1) * ((double) (VALUES->op2));
	VALUES->result3 = (int32_t) ( VALUES->result2 - ( sin(VALUES->result1) * ((double) (VALUES->op3)) ) );

#undef VALUES
}

void Game_FloatProcAtan(int32_t *ValuePtr)
{
//	x0 = atan2((double)x3, (double)x2) * 16384
//	x5 = (double)x0 / (pi*2)

#if defined(__DEBUG__)
	fprintf(stderr, "float proc Atan\n");
#endif

	ValuePtr[0] = (int32_t) ( atan2((double) (ValuePtr[3]), (double) (ValuePtr[2])) * dconst_16384_0 );
	ValuePtr[5] = (int32_t) ( ( (double) ValuePtr[0] ) / ( dconst_pi * dconst_2_0) );
}

void Game_FloatProcSinCos(int32_t *ValuePtr)
{
//	y114 = (double)x0 * pi / 180
//	x119 = sin(y114) * 100
//	x120 = cos(y114) * 100

#pragma pack(4)

    typedef struct {
        int32_t op;
        int32_t reserved1[113];
        double result1;
        int32_t reserved2[3];
        int32_t result2;
        int32_t result3;
    } value_struct;

#pragma pack()

#define VALUES ((value_struct *) ValuePtr)

#if defined(__DEBUG__)
	fprintf(stderr, "float proc SinCos\n");
#endif

//	*((double *) (&(ValuePtr[114]))) = ( (double) (ValuePtr[0]) * dconst_pi ) / dconst_180_0;
//	ValuePtr[119] = (int32_t) ( sin( *((double *) (&(ValuePtr[114]))) ) * dconst_100_0 );
//	ValuePtr[120] = (int32_t) ( cos( *((double *) (&(ValuePtr[114]))) ) * dconst_100_0 );
	VALUES->result1 = ( ((double) (VALUES->op)) * dconst_pi ) / dconst_180_0;
	VALUES->result2 = (int32_t) ( sin(VALUES->result1) * dconst_100_0 );
	VALUES->result3 = (int32_t) ( cos(VALUES->result1) * dconst_100_0 );

#undef VALUES
}

