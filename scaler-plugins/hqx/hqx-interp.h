/*
 * Copyright (C) 2003  MaxSt ( maxst@hiend3d.com )
 *
 * Copyright (C) 2021  Roman Pauer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __HQX_INTERP_H_
#define __HQX_INTERP_H_

#include <stdint.h>
#include "hqx-platform.h"

/* Interpolate functions */
#if defined(X64SSE2) || defined(ARMV8)
#define GROW(n64, n32) n64 = (n32 | ((uint64_t)n32) << 32) & UINT64_C(0x0000FF0000FF00FF)
static uint32_t inline pack(uint64_t n)
{
    n &= UINT64_C(0x0000FF0000FF00FF);
    return n | (n >> 32);
}
#endif

static uint32_t inline Interp1(uint32_t c1, uint32_t c2)
{
//    *pc = (c1*3+c2) >> 2;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2;
    GROW(cc1, c1); GROW(cc2, c2);
    return pack((cc1 * 3 + cc2) >> 2);
#else
    return ((((c1 & 0x00FF00)*3 + (c2 & 0x00FF00) ) & 0x0003FC00) +
            (((c1 & 0xFF00FF)*3 + (c2 & 0xFF00FF) ) & 0x03FC03FC)) >> 2;
#endif
}

static uint32_t inline Interp2(uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*2+c2+c3) >> 2;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2, cc3;
    GROW(cc1, c1); GROW(cc2, c2); GROW(cc3, c3);
    return pack((cc1 * 2 + cc2 + cc3) >> 2);
#else
    return ((((c1 & 0x00FF00)*2 + (c2 & 0x00FF00) + (c3 & 0x00FF00) ) & 0x0003FC00) +
            (((c1 & 0xFF00FF)*2 + (c2 & 0xFF00FF) + (c3 & 0xFF00FF) ) & 0x03FC03FC)) >> 2;
#endif
}

static uint32_t inline Interp3(uint32_t c1, uint32_t c2)
{
//    *pc = (c1*7+c2)/8;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2;
    GROW(cc1, c1); GROW(cc2, c2);
    return pack((cc1 * 7 + cc2) >> 3);
#else
    return ((((c1 & 0x00FF00)*7 + (c2 & 0x00FF00) ) & 0x0007F800) +
            (((c1 & 0xFF00FF)*7 + (c2 & 0xFF00FF) ) & 0x07F807F8)) >> 3;
#endif
}

static uint32_t inline Interp4(uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*2+(c2+c3)*7)/16;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2, cc3;
    GROW(cc1, c1); GROW(cc2, c2); GROW(cc3, c3);
    return pack((cc1 * 2 + (cc2 + cc3) * 7) >> 4);
#else
    return ((((c1 & 0x00FF00)*2 + ((c2 & 0x00FF00) + (c3 & 0x00FF00))*7 ) & 0x000FF000) +
            (((c1 & 0xFF00FF)*2 + ((c2 & 0xFF00FF) + (c3 & 0xFF00FF))*7 ) & 0x0FF00FF0)) >> 4;
#endif
}

static uint32_t inline Interp5(uint32_t c1, uint32_t c2)
{
//    *pc = (c1+c2) >> 1;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2;
    GROW(cc1, c1); GROW(cc2, c2);
    return pack((cc1 + cc2) >> 1);
#else
    return ((((c1 & 0x00FF00) + (c2 & 0x00FF00) ) & 0x0001FE00) +
            (((c1 & 0xFF00FF) + (c2 & 0xFF00FF) ) & 0x01FE01FE)) >> 1;
#endif
}

static uint32_t inline Interp6(uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*5+c2*2+c3)/8;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2, cc3;
    GROW(cc1, c1); GROW(cc2, c2); GROW(cc3, c3);
    return pack((cc1 * 5 + cc2 * 2 + cc3) >> 3);
#else
    return ((((c1 & 0x00FF00)*5 + (c2 & 0x00FF00)*2 + (c3 & 0x00FF00) ) & 0x0007F800) +
            (((c1 & 0xFF00FF)*5 + (c2 & 0xFF00FF)*2 + (c3 & 0xFF00FF) ) & 0x07F807F8)) >> 3;
#endif
}

static uint32_t inline Interp7(uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*6+c2+c3)/8;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2, cc3;
    GROW(cc1, c1); GROW(cc2, c2); GROW(cc3, c3);
    return pack((cc1 * 6 + cc2 + cc3) >> 3);
#else
    return ((((c1 & 0x00FF00)*6 + (c2 & 0x00FF00) + (c3 & 0x00FF00) ) & 0x0007F800) +
            (((c1 & 0xFF00FF)*6 + (c2 & 0xFF00FF) + (c3 & 0xFF00FF) ) & 0x07F807F8)) >> 3;
#endif
}

static uint32_t inline Interp8(uint32_t c1, uint32_t c2)
{
//    *pc = (c1*5+c2*3)/8;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2;
    GROW(cc1, c1); GROW(cc2, c2);
    return pack((cc1 * 5 + cc2 * 3) >> 3);
#else
    return ((((c1 & 0x00FF00)*5 + (c2 & 0x00FF00)*3 ) & 0x0007F800) +
            (((c1 & 0xFF00FF)*5 + (c2 & 0xFF00FF)*3 ) & 0x07F807F8)) >> 3;
#endif
}

static uint32_t inline Interp9(uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*2+(c2+c3)*3)/8;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2, cc3;
    GROW(cc1, c1); GROW(cc2, c2); GROW(cc3, c3);
    return pack((cc1 * 2 + (cc2 + cc3) * 3) >> 3);
#else
    return ((((c1 & 0x00FF00)*2 + ((c2 & 0x00FF00) + (c3 & 0x00FF00))*3 ) & 0x0007F800) +
            (((c1 & 0xFF00FF)*2 + ((c2 & 0xFF00FF) + (c3 & 0xFF00FF))*3 ) & 0x07F807F8)) >> 3;
#endif
}

static uint32_t inline Interp10(uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*14+c2+c3)/16;
#if defined(X64SSE2) || defined(ARMV8)
    uint64_t cc1, cc2, cc3;
    GROW(cc1, c1); GROW(cc2, c2); GROW(cc3, c3);
    return pack((cc1 * 14 + cc2 + cc3) >> 4);
#else
    return ((((c1 & 0x00FF00)*14 + (c2 & 0x00FF00) + (c3 & 0x00FF00) ) & 0x000FF000) +
            (((c1 & 0xFF00FF)*14 + (c2 & 0xFF00FF) + (c3 & 0xFF00FF) ) & 0x0FF00FF0)) >> 4;
#endif
}

#endif

