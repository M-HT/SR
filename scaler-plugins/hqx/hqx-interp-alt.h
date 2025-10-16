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

#ifndef __HQX_INTERP_ALT_H_
#define __HQX_INTERP_ALT_H_

#include <stdint.h>
#include "hqx-platform.h"
#if defined(ARMV7) || defined(ARMV8)
#include <arm_neon.h>


static void INLINE Interp0(uint32_t *pc, uint32_t c1)
{
//    *pc = c1;
    uint8x8_t n01;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    vst1_lane_u32(pc, vreinterpret_u32_u8(n01), 0); // *pc = c1
}

static void INLINE Interp1(uint32_t *pc, uint32_t c1, uint32_t c2)
{
//    *pc = (c1*3+c2) >> 2;
    uint8x8_t n01, n02;
    uint16x4_t n03, n04, n05, n06;
    uint8x8_t n07;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vget_low_u16(vaddl_u8(n01, n02));         // n03 = c1 + c2, zero extended from 8 bits to 16 bits
    n04 = vget_low_u16(vaddl_u8(n01, n01));         // n04 = c1 * 2, zero extended from 8 bits to 16 bits
    n05 = vadd_u16(n03, n04);                       // n05 = c1 * 3 + c2
    n06 = vmov_n_u16(0);                            // n06 = 0
    n07 = vshrn_n_u16(vcombine_u16(n05, n06), 2);   // n07 = (c1 * 3 + c2) >> 2, narrowed from 16 bits to 8 bits
    vst1_lane_u32(pc, vreinterpret_u32_u8(n07), 0); // *pc = (c1 * 3 + c2) >> 2
}

static void INLINE Interp2(uint32_t *pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*2+c2+c3) >> 2;
    uint8x8_t n01, n02, n03;
    uint16x4_t n04, n05, n06, n07;
    uint8x8_t n08;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vreinterpret_u8_u32(vmov_n_u32(c3));      // n03 = c3
    n04 = vget_low_u16(vaddl_u8(n01, n01));         // n04 = c1 * 2, zero extended from 8 bits to 16 bits
    n05 = vget_low_u16(vaddl_u8(n02, n03));         // n05 = c2 + c3, zero extended from 8 bits to 16 bits
    n06 = vadd_u16(n04, n05);                       // n06 = c1 * 2 + c2 + c3
    n07 = vmov_n_u16(0);                            // n07 = 0
    n08 = vshrn_n_u16(vcombine_u16(n06, n07), 2);   // n08 = (c1 * 2 + c2 + c3) >> 2, narrowed from 16 bits to 8 bits
    vst1_lane_u32(pc, vreinterpret_u32_u8(n08), 0); // *pc = (c1 * 2 + c2 + c3) >> 2
}

static void INLINE Interp3(uint32_t *pc, uint32_t c1, uint32_t c2)
{
//    *pc = (c1*7+c2)/8;
    uint8x8_t n01, n02;
    uint16x4_t n03, n04, n05, n06;
    uint8x8_t n07;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vget_low_u16(vsubl_u8(n02, n01));         // n03 = -c1 + c2, zero extended from 8 bits to 16 bits
    n04 = vget_low_u16(vshll_n_u8(n01, 3));         // n04 = c1 * 8, zero extended from 8 bits to 16 bits
    n05 = vadd_u16(n03, n04);                       // n05 = c1 * 7 + c2
    n06 = vmov_n_u16(0);                            // n06 = 0
    n07 = vshrn_n_u16(vcombine_u16(n05, n06), 3);   // n07 = (c1 * 7 + c2) >> 3, narrowed from 16 bits to 8 bits
    vst1_lane_u32(pc, vreinterpret_u32_u8(n07), 0); // *pc = (c1 * 7 + c2) >> 3
}

static void INLINE Interp4(uint32_t *pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*2+(c2+c3)*7)/16;
    uint8x8_t n01, n02, n03;
    uint16x4_t n04, n05, n06, n07, n08, n09;
    uint8x8_t n10;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vreinterpret_u8_u32(vmov_n_u32(c3));      // n03 = c3
    n04 = vget_low_u16(vaddl_u8(n01, n01));         // n04 = c1 * 2, zero extended from 8 bits to 16 bits
    n05 = vget_low_u16(vaddl_u8(n02, n03));         // n05 = c2 + c3, zero extended from 8 bits to 16 bits
    n06 = vsub_u16(n04, n05);                       // n06 = c1 * 2 - (c2 + c3)
    n07 = vshl_n_u16(n05, 3);                       // n07 = (c2 + c3) * 8
    n08 = vadd_u16(n06, n07);                       // n08 = c1 * 2 + (c2 + c3) * 7
    n09 = vmov_n_u16(0);                            // n09 = 0
    n10 = vshrn_n_u16(vcombine_u16(n08, n09), 4);   // n10 = (c1 * 2 + (c2 + c3) * 7) >> 4, narrowed from 16 bits to 8 bits
    vst1_lane_u32(pc, vreinterpret_u32_u8(n10), 0); // *pc = (c1 * 2 + (c2 + c3) * 7) >> 4
}

static void INLINE Interp5(uint32_t *pc, uint32_t c1, uint32_t c2)
{
//    *pc = (c1+c2) >> 1;
    uint8x8_t n01, n02, n03;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vhadd_u8(n01, n02);                       // n03 = (c1 + c2) >> 1
    vst1_lane_u32(pc, vreinterpret_u32_u8(n03), 0); // *pc = (c1 + c2) >> 1
}

static void INLINE Interp6(uint32_t *pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*5+c2*2+c3)/8;
    uint8x8_t n01, n02, n03;
    uint16x4_t n04, n05, n06, n07, n08, n09;
    uint8x8_t n10;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vreinterpret_u8_u32(vmov_n_u32(c3));      // n03 = c3
    n04 = vget_low_u16(vaddl_u8(n02, n02));         // n04 = c2 * 2, zero extended from 8 bits to 16 bits
    n05 = vget_low_u16(vaddl_u8(n01, n03));         // n05 = c1 + c3, zero extended from 8 bits to 16 bits
    n06 = vget_low_u16(vshll_n_u8(n01, 2));         // n06 = c1 * 4, zero extended from 8 bits to 16 bits
    n07 = vadd_u16(n04, n05);                       // n07 = c1 + c2 * 2 + c3
    n08 = vadd_u16(n06, n07);                       // n08 = c1 * 5 + c2 * 2 + c3
    n09 = vmov_n_u16(0);                            // n09 = 0
    n10 = vshrn_n_u16(vcombine_u16(n08, n09), 3);   // n10 = (c1 * 5 + c2 * 2 + c3) >> 3, narrowed from 16 bits to 8 bits
    vst1_lane_u32(pc, vreinterpret_u32_u8(n10), 0); // *pc = (c1 * 5 + c2 * 2 + c3) >> 3
}

static void INLINE Interp7(uint32_t *pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*6+c2+c3)/8;
    uint8x8_t n01, n02, n03;
    uint16x4_t n04, n05, n06, n07, n08, n09;
    uint8x8_t n10;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vreinterpret_u8_u32(vmov_n_u32(c3));      // n03 = c3
    n04 = vget_low_u16(vaddl_u8(n01, n01));         // n04 = c1 * 2, zero extended from 8 bits to 16 bits
    n05 = vget_low_u16(vaddl_u8(n02, n03));         // n05 = c2 + c3, zero extended from 8 bits to 16 bits
    n06 = vadd_u16(n04, n05);                       // n06 = c1 * 2 + c2 + c3
    n07 = vadd_u16(n04, n04);                       // n07 = c1 * 4
    n08 = vadd_u16(n06, n07);                       // n08 = c1 * 6 + c2 + c3
    n09 = vmov_n_u16(0);                            // n09 = 0
    n10 = vshrn_n_u16(vcombine_u16(n08, n09), 3);   // n10 = (c1 * 6 + c2 + c3) >> 3, narrowed from 16 bits to 8 bits
    vst1_lane_u32(pc, vreinterpret_u32_u8(n10), 0); // *pc = (c1 * 6 + c2 + c3) >> 3
}

static void INLINE Interp8(uint32_t *pc, uint32_t c1, uint32_t c2)
{
//    *pc = (c1*5+c2*3)/8;
    uint8x8_t n01, n02;
    uint16x4_t n03, n04, n05, n06, n07, n08;
    uint8x8_t n09;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vget_low_u16(vshll_n_u8(n01, 2));         // n03 = c1 * 4, zero extended from 8 bits to 16 bits
    n04 = vget_low_u16(vaddl_u8(n02, n02));         // n04 = c2 * 2, zero extended from 8 bits to 16 bits
    n05 = vget_low_u16(vaddl_u8(n01, n02));         // n05 = c1 + c2, zero extended from 8 bits to 16 bits
    n06 = vadd_u16(n03, n04);                       // n06 = c1 * 4 + c2 * 2
    n07 = vadd_u16(n05, n06);                       // n07 = c1 * 5 + c2 * 3
    n08 = vmov_n_u16(0);                            // n08 = 0
    n09 = vshrn_n_u16(vcombine_u16(n07, n08), 3);   // n09 = (c1 * 5 + c2 * 3) >> 3, narrowed from 16 bits to 8 bits
    vst1_lane_u32(pc, vreinterpret_u32_u8(n09), 0); // *pc = (c1 * 5 + c2 * 3) >> 3
}

static void INLINE Interp9(uint32_t *pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*2+(c2+c3)*3)/8;
    uint8x8_t n01, n02, n03;
    uint16x4_t n04, n05, n06, n07, n08, n09;
    uint8x8_t n10;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vreinterpret_u8_u32(vmov_n_u32(c3));      // n03 = c3
    n04 = vget_low_u16(vaddl_u8(n01, n01));         // n04 = c1 * 2, zero extended from 8 bits to 16 bits
    n05 = vget_low_u16(vaddl_u8(n02, n03));         // n05 = c2 + c3, zero extended from 8 bits to 16 bits
    n06 = vadd_u16(n04, n05);                       // n06 = c1 * 2 + c2 + c3
    n07 = vadd_u16(n05, n05);                       // n07 = (c2 + c3) * 2
    n08 = vadd_u16(n06, n07);                       // n08 = c1 * 2 + (c2 + c3) * 3
    n09 = vmov_n_u16(0);                            // n09 = 0
    n10 = vshrn_n_u16(vcombine_u16(n08, n09), 3);   // n10 = (c1 * 2 + (c2 + c3) * 3) >> 3, narrowed from 16 bits to 8 bits
    vst1_lane_u32(pc, vreinterpret_u32_u8(n10), 0); // *pc = (c1 * 2 + (c2 + c3) * 3) >> 3
}

static void INLINE Interp10(uint32_t *pc, uint32_t c1, uint32_t c2, uint32_t c3)
{
//    *pc = (c1*14+c2+c3)/16;
    uint8x8_t n01, n02, n03;
    uint16x4_t n04, n05, n06, n07, n08, n09;
    uint8x8_t n10;
    n01 = vreinterpret_u8_u32(vmov_n_u32(c1));      // n01 = c1
    n02 = vreinterpret_u8_u32(vmov_n_u32(c2));      // n02 = c2
    n03 = vreinterpret_u8_u32(vmov_n_u32(c3));      // n03 = c3
    n04 = vget_low_u16(vaddl_u8(n01, n01));         // n04 = c1 * 2, zero extended from 8 bits to 16 bits
    n05 = vget_low_u16(vaddl_u8(n02, n03));         // n05 = c2 + c3, zero extended from 8 bits to 16 bits
    n06 = vsub_u16(n05, n04);                       // n06 = -c1 * 2 + c2 + c3
    n07 = vshl_n_u16(n04, 3);                       // n07 = c1 * 16
    n08 = vadd_u16(n06, n07);                       // n08 = c1 * 14 + c2 + c3
    n09 = vmov_n_u16(0);                            // n09 = 0
    n10 = vshrn_n_u16(vcombine_u16(n08, n09), 4);   // n10 = (c1 * 14 + c2 + c3) >> 4, narrowed from 16 bits to 8 bits
    vst1_lane_u32(pc, vreinterpret_u32_u8(n10), 0); // *pc = (c1 * 14 + c2 + c3) >> 4
}

#endif

#endif

