/*
 * Copyright (C) 2003  MaxSt ( maxst@hiend3d.com )
 *
 * Copyright (C) 2021-2025  Roman Pauer
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

#include <stdint.h>
#include <stdlib.h>
#include "hqx-common.h"
#include "hqx-platform.h"

#if defined(X86SSE2) || defined(X64SSE2)
#include <emmintrin.h>
#elif defined(ARMV6) && defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
#include <arm_acle.h>
#endif


#define Ymask 0x00FF0000
#define Umask 0x0000FF00
#define Vmask 0x000000FF
#define trY   0x00300000
#define trU   0x00000700
#define trV   0x00000006

#if !defined(ARMV8) && !defined(ARMV7)
static INLINE uint32_t yuv_diff(uint32_t yuv1, uint32_t yuv2)
{
#if defined(X86SSE2) || defined(X64SSE2)
    // using mmx registers might be faster than using sse registers, but mmx instructions are deprecated and shouldn't be used in new code
    __m128i value1, value2, tmp1, tmp2;

    value1 = _mm_cvtsi32_si128(yuv1);       // value1 = yuv1
    value2 = _mm_cvtsi32_si128(yuv2);       // value2 = yuv2
    tmp1 = value1;                          // tmp1 = yuv1
    tmp2 = _mm_cvtsi32_si128(0x300706);     // tmp2 = 0x300706
    tmp1 = _mm_max_epu8(tmp1, value2);      // tmp1 = max(yuv1, yuv2)
    value2 = _mm_min_epu8(value2, value1);  // value2 = min(yuv1, yuv2)
    tmp1 = _mm_sub_epi8(tmp1, value2);      // tmp1 = abs(yuv1 - yuv2)
    tmp1 = _mm_max_epu8(tmp1, tmp2);        // tmp1 = max(0x300706, abs(yuv1 - yuv2))
    tmp1 = _mm_sub_epi8(tmp1, tmp2);        // tmp1 = max(0x300706, abs(yuv1 - yuv2)) - 0x300706

    return _mm_cvtsi128_si32(tmp1);
#elif defined(ARMV6)
#if defined(__ARM_ACLE) && __ARM_FEATURE_SIMD32
    uint8x4_t tmp1, tmp2, dist;

    dist = 0x300706;
    tmp1 = __usub8(yuv1, yuv2);     // tmp1 = yuv1 - yuv2
    tmp2 = __usub8(yuv2, yuv1);     // tmp2 = yuv2 - yuv1
    tmp2 = __sel(tmp2, tmp1);       // tmp2 = (tmp2 >= 0)?tmp2:tmp1     // tmp2 = abs(yuv1 - yuv2)
    tmp1 = __usub8(dist, tmp2);     // tmp1 = 0x300706 - abs(yuv1 - yuv2)
    tmp1 = __sel(0, dist);          // tmp1 = (tmp1 >= 0)?0:0x300706    // tmp1 = (0x300706 >= abs(yuv1 - yuv2))?0:0x300706

    return tmp1;
#else
    uint32_t tmp1, tmp2, zero, dist;

    zero = 0;
    dist = 0x300706;

    asm (
        "usub8 %[tmp1], %[value1], %[value2]    \n\t"   // tmp1 = value1 - value2           // tmp1 = yuv1 - yuv2
        : [tmp1] "=r" (tmp1)
        : [value1] "r" (yuv1), [value2] "r" (yuv2)
        : "cc"
    );
    asm (
        "usub8 %[tmp2], %[value2], %[value1]    \n\t"   // tmp2 = value2 - value1           // tmp2 = yuv2 - yuv1
        "sel %[tmp2], %[tmp2], %[tmp1]          \n\t"   // tmp2 = (tmp2 >= 0)?tmp2:tmp1     // tmp2 = abs(yuv1 - yuv2)
        : [tmp2] "=r" (tmp2)
        : [value1] "r" (yuv1), [value2] "r" (yuv2), [tmp1] "r" (tmp1)
        : "cc"
    );
    asm (
        "usub8 %[tmp1], %[dist], %[tmp2]        \n\t"   // tmp1 = dist - tmp2               // tmp1 = 0x300706 - abs(yuv1 - yuv2)
        "sel %[tmp1], %[zero], %[dist]          \n\t"   // tmp1 = (tmp1 >= 0)?zero:dist     // tmp1 = (0x300706 >= abs(yuv1 - yuv2))?0:0x300706
        : [tmp1] "=r" (tmp1)
        : [zero] "r" (zero), [dist] "r" (dist), [tmp2] "r" (tmp2)
        : "cc"
    );

    return tmp1;
#endif
#else
    return ( ( abs((yuv1 & Ymask) - (yuv2 & Ymask)) > trY ) ||
             ( abs((yuv1 & Umask) - (yuv2 & Umask)) > trU ) ||
             ( abs((yuv1 & Vmask) - (yuv2 & Vmask)) > trV ) );
#endif
}
#endif

#ifdef __cplusplus
extern "C"
#endif
void convert_yuv_32(const uint32_t *src, uint32_t *dst, unsigned int width) // dst is aligned using ALIGNPTR
{
    unsigned int index;

    for (index = 0; index < width; index++)
    {
        unsigned int rgbvalue;
        int r, g, b;
        unsigned int Y, u, v;

        rgbvalue = src[index];

        b = (rgbvalue      ) & 0xff;
        g = (rgbvalue >>  8) & 0xff;
        r = (rgbvalue >> 16) & 0xff;

        Y = (r + g + b) >> 2;
        u = 128 + ((r - b) >> 2);
        v = 128 + ((-r + 2*g -b)>>3);

        dst[index] = (Y << 16) + (u << 8) + v;
    }

    dst[-1] = dst[0];
    dst[width] = dst[width - 1];
}

#if !defined(ARMV8) && !defined(ARMV7)
#ifdef __cplusplus
extern "C"
#endif
void calculate_pattern(const uint32_t *yuvsrc1, const uint32_t *yuvsrc2, const uint32_t *yuvsrc3, uint8_t *dst, unsigned int width)
{
    uint32_t e, newb, newe, newh;
    uint8_t pattern;

    // before first
    {
        newb = yuvsrc1[-1];
        newe = yuvsrc2[-1];
        newh = yuvsrc3[-1];
        pattern = 0;
        if (yuv_diff(newe, newb    )) pattern |= (1 << 0) | (1 << 1) | (1 << 2);
        if (yuv_diff(newe, newh    )) pattern |= (1 << 5) | (1 << 6) | (1 << 7);

        dst[-1] = pattern;
    }

    for (; width != 0; width--)
    {
        e = newe;
        pattern = 0;
        if (yuv_diff(e, yuvsrc1[-1])) pattern |= 1 << 0;
        if (yuv_diff(e, newb       )) pattern |= 1 << 1;
        newb = yuvsrc1[ 1];
        if (yuv_diff(e, newb       )) pattern |= 1 << 2;
        if (yuv_diff(e, yuvsrc2[-1])) pattern |= 1 << 3;
        newe = yuvsrc2[ 1];
        if (yuv_diff(e, newe       )) pattern |= 1 << 4;
        if (yuv_diff(e, yuvsrc3[-1])) pattern |= 1 << 5;
        if (yuv_diff(e, newh       )) pattern |= 1 << 6;
        newh = yuvsrc3[ 1];
        if (yuv_diff(e, newh       )) pattern |= 1 << 7;

        *dst = pattern;

        yuvsrc1++;
        yuvsrc2++;
        yuvsrc3++;
        dst++;
    }

    // after last
    {
        pattern = 0;
        if (yuv_diff(newe, newb    )) pattern |= (1 << 0) | (1 << 1) | (1 << 2);
        if (yuv_diff(newe, newh    )) pattern |= (1 << 5) | (1 << 6) | (1 << 7);

        *dst = pattern;
    }
}
#endif

