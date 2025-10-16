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
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
#include <malloc.h>
#elif defined(__linux__)
#include <alloca.h>
#else
#include <stdlib.h>
#endif
#include "hqx.h"
#include "hqx-common.h"
#include "hqx-platform.h"
#if defined(ARMV7) || defined(ARMV8)
#include "hqx-interp-alt.h"
#else
#include "hqx-interp.h"
#endif


#if defined(ARMV7) || defined(ARMV8)

#define PIXEL00_0     Interp0(dst, w[5]);
#define PIXEL00_10    Interp1(dst, w[5], w[1]);
#define PIXEL00_11    Interp1(dst, w[5], w[4]);
#define PIXEL00_12    Interp1(dst, w[5], w[2]);
#define PIXEL00_20    Interp2(dst, w[5], w[4], w[2]);
#define PIXEL00_21    Interp2(dst, w[5], w[1], w[2]);
#define PIXEL00_22    Interp2(dst, w[5], w[1], w[4]);
#define PIXEL00_60    Interp6(dst, w[5], w[2], w[4]);
#define PIXEL00_61    Interp6(dst, w[5], w[4], w[2]);
#define PIXEL00_70    Interp7(dst, w[5], w[4], w[2]);
#define PIXEL00_90    Interp9(dst, w[5], w[4], w[2]);
#define PIXEL00_100   Interp10(dst, w[5], w[4], w[2]);
#define PIXEL01_0     Interp0(dst+1, w[5]);
#define PIXEL01_10    Interp1(dst+1, w[5], w[3]);
#define PIXEL01_11    Interp1(dst+1, w[5], w[2]);
#define PIXEL01_12    Interp1(dst+1, w[5], w[6]);
#define PIXEL01_20    Interp2(dst+1, w[5], w[2], w[6]);
#define PIXEL01_21    Interp2(dst+1, w[5], w[3], w[6]);
#define PIXEL01_22    Interp2(dst+1, w[5], w[3], w[2]);
#define PIXEL01_60    Interp6(dst+1, w[5], w[6], w[2]);
#define PIXEL01_61    Interp6(dst+1, w[5], w[2], w[6]);
#define PIXEL01_70    Interp7(dst+1, w[5], w[2], w[6]);
#define PIXEL01_90    Interp9(dst+1, w[5], w[2], w[6]);
#define PIXEL01_100   Interp10(dst+1, w[5], w[2], w[6]);
#define PIXEL10_0     Interp0(DST2, w[5]);
#define PIXEL10_10    Interp1(DST2, w[5], w[7]);
#define PIXEL10_11    Interp1(DST2, w[5], w[8]);
#define PIXEL10_12    Interp1(DST2, w[5], w[4]);
#define PIXEL10_20    Interp2(DST2, w[5], w[8], w[4]);
#define PIXEL10_21    Interp2(DST2, w[5], w[7], w[4]);
#define PIXEL10_22    Interp2(DST2, w[5], w[7], w[8]);
#define PIXEL10_60    Interp6(DST2, w[5], w[4], w[8]);
#define PIXEL10_61    Interp6(DST2, w[5], w[8], w[4]);
#define PIXEL10_70    Interp7(DST2, w[5], w[8], w[4]);
#define PIXEL10_90    Interp9(DST2, w[5], w[8], w[4]);
#define PIXEL10_100   Interp10(DST2, w[5], w[8], w[4]);
#define PIXEL11_0     Interp0(DST2+1, w[5]);
#define PIXEL11_10    Interp1(DST2+1, w[5], w[9]);
#define PIXEL11_11    Interp1(DST2+1, w[5], w[6]);
#define PIXEL11_12    Interp1(DST2+1, w[5], w[8]);
#define PIXEL11_20    Interp2(DST2+1, w[5], w[6], w[8]);
#define PIXEL11_21    Interp2(DST2+1, w[5], w[9], w[8]);
#define PIXEL11_22    Interp2(DST2+1, w[5], w[9], w[6]);
#define PIXEL11_60    Interp6(DST2+1, w[5], w[8], w[6]);
#define PIXEL11_61    Interp6(DST2+1, w[5], w[6], w[8]);
#define PIXEL11_70    Interp7(DST2+1, w[5], w[6], w[8]);
#define PIXEL11_90    Interp9(DST2+1, w[5], w[6], w[8]);
#define PIXEL11_100   Interp10(DST2+1, w[5], w[6], w[8]);

#else

#define PIXEL00_0     *dst = w[5];
#define PIXEL00_10    *dst = Interp1(w[5], w[1]);
#define PIXEL00_11    *dst = Interp1(w[5], w[4]);
#define PIXEL00_12    *dst = Interp1(w[5], w[2]);
#define PIXEL00_20    *dst = Interp2(w[5], w[4], w[2]);
#define PIXEL00_21    *dst = Interp2(w[5], w[1], w[2]);
#define PIXEL00_22    *dst = Interp2(w[5], w[1], w[4]);
#define PIXEL00_60    *dst = Interp6(w[5], w[2], w[4]);
#define PIXEL00_61    *dst = Interp6(w[5], w[4], w[2]);
#define PIXEL00_70    *dst = Interp7(w[5], w[4], w[2]);
#define PIXEL00_90    *dst = Interp9(w[5], w[4], w[2]);
#define PIXEL00_100   *dst = Interp10(w[5], w[4], w[2]);
#define PIXEL01_0     *(dst+1) = w[5];
#define PIXEL01_10    *(dst+1) = Interp1(w[5], w[3]);
#define PIXEL01_11    *(dst+1) = Interp1(w[5], w[2]);
#define PIXEL01_12    *(dst+1) = Interp1(w[5], w[6]);
#define PIXEL01_20    *(dst+1) = Interp2(w[5], w[2], w[6]);
#define PIXEL01_21    *(dst+1) = Interp2(w[5], w[3], w[6]);
#define PIXEL01_22    *(dst+1) = Interp2(w[5], w[3], w[2]);
#define PIXEL01_60    *(dst+1) = Interp6(w[5], w[6], w[2]);
#define PIXEL01_61    *(dst+1) = Interp6(w[5], w[2], w[6]);
#define PIXEL01_70    *(dst+1) = Interp7(w[5], w[2], w[6]);
#define PIXEL01_90    *(dst+1) = Interp9(w[5], w[2], w[6]);
#define PIXEL01_100   *(dst+1) = Interp10(w[5], w[2], w[6]);
#define PIXEL10_0     *(DST2) = w[5];
#define PIXEL10_10    *(DST2) = Interp1(w[5], w[7]);
#define PIXEL10_11    *(DST2) = Interp1(w[5], w[8]);
#define PIXEL10_12    *(DST2) = Interp1(w[5], w[4]);
#define PIXEL10_20    *(DST2) = Interp2(w[5], w[8], w[4]);
#define PIXEL10_21    *(DST2) = Interp2(w[5], w[7], w[4]);
#define PIXEL10_22    *(DST2) = Interp2(w[5], w[7], w[8]);
#define PIXEL10_60    *(DST2) = Interp6(w[5], w[4], w[8]);
#define PIXEL10_61    *(DST2) = Interp6(w[5], w[8], w[4]);
#define PIXEL10_70    *(DST2) = Interp7(w[5], w[8], w[4]);
#define PIXEL10_90    *(DST2) = Interp9(w[5], w[8], w[4]);
#define PIXEL10_100   *(DST2) = Interp10(w[5], w[8], w[4]);
#define PIXEL11_0     *(DST2+1) = w[5];
#define PIXEL11_10    *(DST2+1) = Interp1(w[5], w[9]);
#define PIXEL11_11    *(DST2+1) = Interp1(w[5], w[6]);
#define PIXEL11_12    *(DST2+1) = Interp1(w[5], w[8]);
#define PIXEL11_20    *(DST2+1) = Interp2(w[5], w[6], w[8]);
#define PIXEL11_21    *(DST2+1) = Interp2(w[5], w[9], w[8]);
#define PIXEL11_22    *(DST2+1) = Interp2(w[5], w[9], w[6]);
#define PIXEL11_60    *(DST2+1) = Interp6(w[5], w[8], w[6]);
#define PIXEL11_61    *(DST2+1) = Interp6(w[5], w[6], w[8]);
#define PIXEL11_70    *(DST2+1) = Interp7(w[5], w[6], w[8]);
#define PIXEL11_90    *(DST2+1) = Interp9(w[5], w[6], w[8]);
#define PIXEL11_100   *(DST2+1) = Interp10(w[5], w[6], w[8]);

#endif

static void fill_dst(const uint32_t *rgbsrc1, const uint32_t *rgbsrc2, const uint32_t *rgbsrc3, uint32_t *dst, unsigned int width, unsigned int dststride, const uint8_t *patternTable)
{
    unsigned int  i;
    uint32_t  w[10];

    //   +----+----+----+
    //   |    |    |    |
    //   | w1 | w2 | w3 |
    //   +----+----+----+
    //   |    |    |    |
    //   | w4 | w5 | w6 |
    //   +----+----+----+
    //   |    |    |    |
    //   | w7 | w8 | w9 |
    //   +----+----+----+

#define DST2 ((uint32_t *) (((uintptr_t) dst) + dststride))

    w[2] = *rgbsrc1;
    w[5] = *rgbsrc2;
    w[8] = *rgbsrc3;

    for (i=0; i<width; i++)
    {
        w[1] = w[2];
        w[4] = w[5];
        w[7] = w[8];

        w[2] = *rgbsrc1;
        w[5] = *rgbsrc2;
        w[8] = *rgbsrc3;

        if (i<width-1)
        {
            w[3] = *(rgbsrc1 + 1);
            w[6] = *(rgbsrc2 + 1);
            w[9] = *(rgbsrc3 + 1);
        }
        else
        {
            w[3] = w[2];
            w[6] = w[5];
            w[9] = w[8];
        }

        switch (patternTable[i])
        {
            case 0:
            case 1:
            case 4:
            case 32:
            case 128:
            case 5:
            case 132:
            case 160:
            case 33:
            case 129:
            case 36:
            case 133:
            case 164:
            case 161:
            case 37:
            case 165:
                {
                    PIXEL00_20
                    PIXEL01_20
                    PIXEL10_20
                    PIXEL11_20
                    break;
                }
            case 2:
            case 34:
            case 130:
            case 162:
                {
                    PIXEL00_22
                    PIXEL01_21
                    PIXEL10_20
                    PIXEL11_20
                    break;
                }
            case 16:
            case 17:
            case 48:
            case 49:
                {
                    PIXEL00_20
                    PIXEL01_22
                    PIXEL10_20
                    PIXEL11_21
                    break;
                }
            case 64:
            case 65:
            case 68:
            case 69:
                {
                    PIXEL00_20
                    PIXEL01_20
                    PIXEL10_21
                    PIXEL11_22
                    break;
                }
            case 8:
            case 12:
            case 136:
            case 140:
                {
                    PIXEL00_21
                    PIXEL01_20
                    PIXEL10_22
                    PIXEL11_20
                    break;
                }
            case 3:
            case 35:
            case 131:
            case 163:
                {
                    PIXEL00_11
                    PIXEL01_21
                    PIXEL10_20
                    PIXEL11_20
                    break;
                }
            case 6:
            case 38:
            case 134:
            case 166:
                {
                    PIXEL00_22
                    PIXEL01_12
                    PIXEL10_20
                    PIXEL11_20
                    break;
                }
            case 20:
            case 21:
            case 52:
            case 53:
                {
                    PIXEL00_20
                    PIXEL01_11
                    PIXEL10_20
                    PIXEL11_21
                    break;
                }
            case 144:
            case 145:
            case 176:
            case 177:
                {
                    PIXEL00_20
                    PIXEL01_22
                    PIXEL10_20
                    PIXEL11_12
                    break;
                }
            case 192:
            case 193:
            case 196:
            case 197:
                {
                    PIXEL00_20
                    PIXEL01_20
                    PIXEL10_21
                    PIXEL11_11
                    break;
                }
            case 96:
            case 97:
            case 100:
            case 101:
                {
                    PIXEL00_20
                    PIXEL01_20
                    PIXEL10_12
                    PIXEL11_22
                    break;
                }
            case 40:
            case 44:
            case 168:
            case 172:
                {
                    PIXEL00_21
                    PIXEL01_20
                    PIXEL10_11
                    PIXEL11_20
                    break;
                }
            case 9:
            case 13:
            case 137:
            case 141:
                {
                    PIXEL00_12
                    PIXEL01_20
                    PIXEL10_22
                    PIXEL11_20
                    break;
                }
            case 18:
            case 50:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_20
                    PIXEL11_21
                    break;
                }
            case 80:
            case 81:
                {
                    PIXEL00_20
                    PIXEL01_22
                    PIXEL10_21
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 72:
            case 76:
                {
                    PIXEL00_21
                    PIXEL01_20
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_22
                    break;
                }
            case 10:
            case 138:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_21
                    PIXEL10_22
                    PIXEL11_20
                    break;
                }
            case 66:
                {
                    PIXEL00_22
                    PIXEL01_21
                    PIXEL10_21
                    PIXEL11_22
                    break;
                }
            case 24:
                {
                    PIXEL00_21
                    PIXEL01_22
                    PIXEL10_22
                    PIXEL11_21
                    break;
                }
            case 7:
            case 39:
            case 135:
                {
                    PIXEL00_11
                    PIXEL01_12
                    PIXEL10_20
                    PIXEL11_20
                    break;
                }
            case 148:
            case 149:
            case 180:
                {
                    PIXEL00_20
                    PIXEL01_11
                    PIXEL10_20
                    PIXEL11_12
                    break;
                }
            case 224:
            case 228:
            case 225:
                {
                    PIXEL00_20
                    PIXEL01_20
                    PIXEL10_12
                    PIXEL11_11
                    break;
                }
            case 41:
            case 169:
            case 45:
                {
                    PIXEL00_12
                    PIXEL01_20
                    PIXEL10_11
                    PIXEL11_20
                    break;
                }
            case 22:
            case 54:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_20
                    PIXEL11_21
                    break;
                }
            case 208:
            case 209:
                {
                    PIXEL00_20
                    PIXEL01_22
                    PIXEL10_21
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 104:
            case 108:
                {
                    PIXEL00_21
                    PIXEL01_20
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_22
                    break;
                }
            case 11:
            case 139:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_21
                    PIXEL10_22
                    PIXEL11_20
                    break;
                }
            case 19:
            case 51:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL00_11
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL00_60
                        PIXEL01_90
                    }
                    PIXEL10_20
                    PIXEL11_21
                    break;
                }
            case 146:
            case 178:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                        PIXEL11_12
                    }
                    else
                    {
                        PIXEL01_90
                        PIXEL11_61
                    }
                    PIXEL10_20
                    break;
                }
            case 84:
            case 85:
                {
                    PIXEL00_20
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL01_11
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL01_60
                        PIXEL11_90
                    }
                    PIXEL10_21
                    break;
                }
            case 112:
            case 113:
                {
                    PIXEL00_20
                    PIXEL01_22
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL10_12
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL10_61
                        PIXEL11_90
                    }
                    break;
                }
            case 200:
            case 204:
                {
                    PIXEL00_21
                    PIXEL01_20
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                        PIXEL11_11
                    }
                    else
                    {
                        PIXEL10_90
                        PIXEL11_60
                    }
                    break;
                }
            case 73:
            case 77:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL00_12
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL00_61
                        PIXEL10_90
                    }
                    PIXEL01_20
                    PIXEL11_22
                    break;
                }
            case 42:
            case 170:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                        PIXEL10_11
                    }
                    else
                    {
                        PIXEL00_90
                        PIXEL10_60
                    }
                    PIXEL01_21
                    PIXEL11_20
                    break;
                }
            case 14:
            case 142:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                        PIXEL01_12
                    }
                    else
                    {
                        PIXEL00_90
                        PIXEL01_61
                    }
                    PIXEL10_22
                    PIXEL11_20
                    break;
                }
            case 67:
                {
                    PIXEL00_11
                    PIXEL01_21
                    PIXEL10_21
                    PIXEL11_22
                    break;
                }
            case 70:
                {
                    PIXEL00_22
                    PIXEL01_12
                    PIXEL10_21
                    PIXEL11_22
                    break;
                }
            case 28:
                {
                    PIXEL00_21
                    PIXEL01_11
                    PIXEL10_22
                    PIXEL11_21
                    break;
                }
            case 152:
                {
                    PIXEL00_21
                    PIXEL01_22
                    PIXEL10_22
                    PIXEL11_12
                    break;
                }
            case 194:
                {
                    PIXEL00_22
                    PIXEL01_21
                    PIXEL10_21
                    PIXEL11_11
                    break;
                }
            case 98:
                {
                    PIXEL00_22
                    PIXEL01_21
                    PIXEL10_12
                    PIXEL11_22
                    break;
                }
            case 56:
                {
                    PIXEL00_21
                    PIXEL01_22
                    PIXEL10_11
                    PIXEL11_21
                    break;
                }
            case 25:
                {
                    PIXEL00_12
                    PIXEL01_22
                    PIXEL10_22
                    PIXEL11_21
                    break;
                }
            case 26:
            case 31:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_22
                    PIXEL11_21
                    break;
                }
            case 82:
            case 214:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_21
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 88:
            case 248:
                {
                    PIXEL00_21
                    PIXEL01_22
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 74:
            case 107:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_21
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_22
                    break;
                }
            case 27:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_10
                    PIXEL10_22
                    PIXEL11_21
                    break;
                }
            case 86:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_21
                    PIXEL11_10
                    break;
                }
            case 216:
                {
                    PIXEL00_21
                    PIXEL01_22
                    PIXEL10_10
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 106:
                {
                    PIXEL00_10
                    PIXEL01_21
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_22
                    break;
                }
            case 30:
                {
                    PIXEL00_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_22
                    PIXEL11_21
                    break;
                }
            case 210:
                {
                    PIXEL00_22
                    PIXEL01_10
                    PIXEL10_21
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 120:
                {
                    PIXEL00_21
                    PIXEL01_22
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_10
                    break;
                }
            case 75:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_21
                    PIXEL10_10
                    PIXEL11_22
                    break;
                }
            case 29:
                {
                    PIXEL00_12
                    PIXEL01_11
                    PIXEL10_22
                    PIXEL11_21
                    break;
                }
            case 198:
                {
                    PIXEL00_22
                    PIXEL01_12
                    PIXEL10_21
                    PIXEL11_11
                    break;
                }
            case 184:
                {
                    PIXEL00_21
                    PIXEL01_22
                    PIXEL10_11
                    PIXEL11_12
                    break;
                }
            case 99:
                {
                    PIXEL00_11
                    PIXEL01_21
                    PIXEL10_12
                    PIXEL11_22
                    break;
                }
            case 57:
                {
                    PIXEL00_12
                    PIXEL01_22
                    PIXEL10_11
                    PIXEL11_21
                    break;
                }
            case 71:
                {
                    PIXEL00_11
                    PIXEL01_12
                    PIXEL10_21
                    PIXEL11_22
                    break;
                }
            case 156:
                {
                    PIXEL00_21
                    PIXEL01_11
                    PIXEL10_22
                    PIXEL11_12
                    break;
                }
            case 226:
                {
                    PIXEL00_22
                    PIXEL01_21
                    PIXEL10_12
                    PIXEL11_11
                    break;
                }
            case 60:
                {
                    PIXEL00_21
                    PIXEL01_11
                    PIXEL10_11
                    PIXEL11_21
                    break;
                }
            case 195:
                {
                    PIXEL00_11
                    PIXEL01_21
                    PIXEL10_21
                    PIXEL11_11
                    break;
                }
            case 102:
                {
                    PIXEL00_22
                    PIXEL01_12
                    PIXEL10_12
                    PIXEL11_22
                    break;
                }
            case 153:
                {
                    PIXEL00_12
                    PIXEL01_22
                    PIXEL10_22
                    PIXEL11_12
                    break;
                }
            case 58:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    PIXEL10_11
                    PIXEL11_21
                    break;
                }
            case 83:
                {
                    PIXEL00_11
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    PIXEL10_21
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 92:
                {
                    PIXEL00_21
                    PIXEL01_11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 202:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    PIXEL01_21
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    PIXEL11_11
                    break;
                }
            case 78:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    PIXEL01_12
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    PIXEL11_22
                    break;
                }
            case 154:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    PIXEL10_22
                    PIXEL11_12
                    break;
                }
            case 114:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    PIXEL10_12
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 89:
                {
                    PIXEL00_12
                    PIXEL01_22
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 90:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 55:
            case 23:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL00_11
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL00_60
                        PIXEL01_90
                    }
                    PIXEL10_20
                    PIXEL11_21
                    break;
                }
            case 182:
            case 150:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                        PIXEL11_12
                    }
                    else
                    {
                        PIXEL01_90
                        PIXEL11_61
                    }
                    PIXEL10_20
                    break;
                }
            case 213:
            case 212:
                {
                    PIXEL00_20
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL01_11
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL01_60
                        PIXEL11_90
                    }
                    PIXEL10_21
                    break;
                }
            case 241:
            case 240:
                {
                    PIXEL00_20
                    PIXEL01_22
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL10_12
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL10_61
                        PIXEL11_90
                    }
                    break;
                }
            case 236:
            case 232:
                {
                    PIXEL00_21
                    PIXEL01_20
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                        PIXEL11_11
                    }
                    else
                    {
                        PIXEL10_90
                        PIXEL11_60
                    }
                    break;
                }
            case 109:
            case 105:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL00_12
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_61
                        PIXEL10_90
                    }
                    PIXEL01_20
                    PIXEL11_22
                    break;
                }
            case 171:
            case 43:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL10_11
                    }
                    else
                    {
                        PIXEL00_90
                        PIXEL10_60
                    }
                    PIXEL01_21
                    PIXEL11_20
                    break;
                }
            case 143:
            case 15:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_12
                    }
                    else
                    {
                        PIXEL00_90
                        PIXEL01_61
                    }
                    PIXEL10_22
                    PIXEL11_20
                    break;
                }
            case 124:
                {
                    PIXEL00_21
                    PIXEL01_11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_10
                    break;
                }
            case 203:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_21
                    PIXEL10_10
                    PIXEL11_11
                    break;
                }
            case 62:
                {
                    PIXEL00_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_11
                    PIXEL11_21
                    break;
                }
            case 211:
                {
                    PIXEL00_11
                    PIXEL01_10
                    PIXEL10_21
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 118:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_12
                    PIXEL11_10
                    break;
                }
            case 217:
                {
                    PIXEL00_12
                    PIXEL01_22
                    PIXEL10_10
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 110:
                {
                    PIXEL00_10
                    PIXEL01_12
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_22
                    break;
                }
            case 155:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_10
                    PIXEL10_22
                    PIXEL11_12
                    break;
                }
            case 188:
                {
                    PIXEL00_21
                    PIXEL01_11
                    PIXEL10_11
                    PIXEL11_12
                    break;
                }
            case 185:
                {
                    PIXEL00_12
                    PIXEL01_22
                    PIXEL10_11
                    PIXEL11_12
                    break;
                }
            case 61:
                {
                    PIXEL00_12
                    PIXEL01_11
                    PIXEL10_11
                    PIXEL11_21
                    break;
                }
            case 157:
                {
                    PIXEL00_12
                    PIXEL01_11
                    PIXEL10_22
                    PIXEL11_12
                    break;
                }
            case 103:
                {
                    PIXEL00_11
                    PIXEL01_12
                    PIXEL10_12
                    PIXEL11_22
                    break;
                }
            case 227:
                {
                    PIXEL00_11
                    PIXEL01_21
                    PIXEL10_12
                    PIXEL11_11
                    break;
                }
            case 230:
                {
                    PIXEL00_22
                    PIXEL01_12
                    PIXEL10_12
                    PIXEL11_11
                    break;
                }
            case 199:
                {
                    PIXEL00_11
                    PIXEL01_12
                    PIXEL10_21
                    PIXEL11_11
                    break;
                }
            case 220:
                {
                    PIXEL00_21
                    PIXEL01_11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 158:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_22
                    PIXEL11_12
                    break;
                }
            case 234:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    PIXEL01_21
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_11
                    break;
                }
            case 242:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    PIXEL10_12
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 59:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    PIXEL10_11
                    PIXEL11_21
                    break;
                }
            case 121:
                {
                    PIXEL00_12
                    PIXEL01_22
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 87:
                {
                    PIXEL00_11
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_21
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 79:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_12
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    PIXEL11_22
                    break;
                }
            case 122:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 94:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 218:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 91:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 229:
                {
                    PIXEL00_20
                    PIXEL01_20
                    PIXEL10_12
                    PIXEL11_11
                    break;
                }
            case 167:
                {
                    PIXEL00_11
                    PIXEL01_12
                    PIXEL10_20
                    PIXEL11_20
                    break;
                }
            case 173:
                {
                    PIXEL00_12
                    PIXEL01_20
                    PIXEL10_11
                    PIXEL11_20
                    break;
                }
            case 181:
                {
                    PIXEL00_20
                    PIXEL01_11
                    PIXEL10_20
                    PIXEL11_12
                    break;
                }
            case 186:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    PIXEL10_11
                    PIXEL11_12
                    break;
                }
            case 115:
                {
                    PIXEL00_11
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    PIXEL10_12
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 93:
                {
                    PIXEL00_12
                    PIXEL01_11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 206:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    PIXEL01_12
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    PIXEL11_11
                    break;
                }
            case 205:
            case 201:
                {
                    PIXEL00_12
                    PIXEL01_20
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_10
                    }
                    else
                    {
                        PIXEL10_70
                    }
                    PIXEL11_11
                    break;
                }
            case 174:
            case 46:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_10
                    }
                    else
                    {
                        PIXEL00_70
                    }
                    PIXEL01_12
                    PIXEL10_11
                    PIXEL11_20
                    break;
                }
            case 179:
            case 147:
                {
                    PIXEL00_11
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_10
                    }
                    else
                    {
                        PIXEL01_70
                    }
                    PIXEL10_20
                    PIXEL11_12
                    break;
                }
            case 117:
            case 116:
                {
                    PIXEL00_20
                    PIXEL01_11
                    PIXEL10_12
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_10
                    }
                    else
                    {
                        PIXEL11_70
                    }
                    break;
                }
            case 189:
                {
                    PIXEL00_12
                    PIXEL01_11
                    PIXEL10_11
                    PIXEL11_12
                    break;
                }
            case 231:
                {
                    PIXEL00_11
                    PIXEL01_12
                    PIXEL10_12
                    PIXEL11_11
                    break;
                }
            case 126:
                {
                    PIXEL00_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_10
                    break;
                }
            case 219:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_10
                    PIXEL10_10
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 125:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL00_12
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_61
                        PIXEL10_90
                    }
                    PIXEL01_11
                    PIXEL11_10
                    break;
                }
            case 221:
                {
                    PIXEL00_12
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL01_11
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL01_60
                        PIXEL11_90
                    }
                    PIXEL10_10
                    break;
                }
            case 207:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_12
                    }
                    else
                    {
                        PIXEL00_90
                        PIXEL01_61
                    }
                    PIXEL10_10
                    PIXEL11_11
                    break;
                }
            case 238:
                {
                    PIXEL00_10
                    PIXEL01_12
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                        PIXEL11_11
                    }
                    else
                    {
                        PIXEL10_90
                        PIXEL11_60
                    }
                    break;
                }
            case 190:
                {
                    PIXEL00_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                        PIXEL11_12
                    }
                    else
                    {
                        PIXEL01_90
                        PIXEL11_61
                    }
                    PIXEL10_11
                    break;
                }
            case 187:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL10_11
                    }
                    else
                    {
                        PIXEL00_90
                        PIXEL10_60
                    }
                    PIXEL01_10
                    PIXEL11_12
                    break;
                }
            case 243:
                {
                    PIXEL00_11
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL10_12
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL10_61
                        PIXEL11_90
                    }
                    break;
                }
            case 119:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL00_11
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL00_60
                        PIXEL01_90
                    }
                    PIXEL10_12
                    PIXEL11_10
                    break;
                }
            case 237:
            case 233:
                {
                    PIXEL00_12
                    PIXEL01_20
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_100
                    }
                    PIXEL11_11
                    break;
                }
            case 175:
            case 47:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_100
                    }
                    PIXEL01_12
                    PIXEL10_11
                    PIXEL11_20
                    break;
                }
            case 183:
            case 151:
                {
                    PIXEL00_11
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_100
                    }
                    PIXEL10_20
                    PIXEL11_12
                    break;
                }
            case 245:
            case 244:
                {
                    PIXEL00_20
                    PIXEL01_11
                    PIXEL10_12
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_100
                    }
                    break;
                }
            case 250:
                {
                    PIXEL00_10
                    PIXEL01_10
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 123:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_10
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_10
                    break;
                }
            case 95:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_10
                    PIXEL11_10
                    break;
                }
            case 222:
                {
                    PIXEL00_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_10
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 252:
                {
                    PIXEL00_21
                    PIXEL01_11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_100
                    }
                    break;
                }
            case 249:
                {
                    PIXEL00_12
                    PIXEL01_22
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_100
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 235:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_21
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_100
                    }
                    PIXEL11_11
                    break;
                }
            case 111:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_100
                    }
                    PIXEL01_12
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_22
                    break;
                }
            case 63:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_100
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_11
                    PIXEL11_21
                    break;
                }
            case 159:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_100
                    }
                    PIXEL10_22
                    PIXEL11_12
                    break;
                }
            case 215:
                {
                    PIXEL00_11
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_100
                    }
                    PIXEL10_21
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 246:
                {
                    PIXEL00_22
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    PIXEL10_12
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_100
                    }
                    break;
                }
            case 254:
                {
                    PIXEL00_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_100
                    }
                    break;
                }
            case 253:
                {
                    PIXEL00_12
                    PIXEL01_11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_100
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_100
                    }
                    break;
                }
            case 251:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    PIXEL01_10
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_100
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 239:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_100
                    }
                    PIXEL01_12
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_100
                    }
                    PIXEL11_11
                    break;
                }
            case 127:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_100
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_20
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_20
                    }
                    PIXEL11_10
                    break;
                }
            case 191:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_100
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_100
                    }
                    PIXEL10_11
                    PIXEL11_12
                    break;
                }
            case 223:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_20
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_100
                    }
                    PIXEL10_10
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_20
                    }
                    break;
                }
            case 247:
                {
                    PIXEL00_11
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_100
                    }
                    PIXEL10_12
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_100
                    }
                    break;
                }
            case 255:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                    }
                    else
                    {
                        PIXEL00_100
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_0
                    }
                    else
                    {
                        PIXEL01_100
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL10_100
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL11_100
                    }
                    break;
                }
        }
        rgbsrc1++;
        rgbsrc2++;
        rgbsrc3++;
        dst += 2;
    }

#undef DST2
}

#ifdef __cplusplus
extern "C"
#endif
void hq2x_32(const uint32_t *src, uint32_t *dst, unsigned int width, unsigned int srcstride, unsigned int dststride, unsigned int height, unsigned int y_first, unsigned int y_last)
{
#define NEXT_SRC_LINE ((uint32_t *) (((uintptr_t) src) + srcstride))
#define PREV_SRC_LINE ((uint32_t *) (((uintptr_t) src) - srcstride))

    int  j;
    uint8_t *bp;

    uint32_t *yuvsrc1;
    uint32_t *yuvsrc2;
    uint32_t *yuvsrc3;

    uint8_t *patternsrc1;

    if (y_last > height) y_last = height;
    if ((y_first >= y_last) || (width == 0)) return;

    bp = (uint8_t *) alloca(3 * sizeof(uint32_t) * (width + 2) + 1 * sizeof(uint8_t) * (width + 2) + (3+1) * ALIGNMENT);

    yuvsrc1 = (uint32_t *) ALIGNPTR(bp + 4); // yuvsrc1 is aligned and has place for yuvsrc1[-1] and yuvsrc1[width]
    yuvsrc2 = (uint32_t *) ALIGNPTR(yuvsrc1 + width + 2); // yuvsrc2 is aligned and has place for yuvsrc2[-1] and yuvsrc2[width]
    yuvsrc3 = (uint32_t *) ALIGNPTR(yuvsrc2 + width + 2); // yuvsrc3 is aligned and has place for yuvsrc3[-1] and yuvsrc3[width]

    patternsrc1 = (uint8_t *) ALIGNPTR(yuvsrc3 + width + 2); // patternsrc1 is aligned and has place for patternsrc1[-1] and patternsrc1[width]


    // first line:
    if (y_first != 0)
    {
        src = ((uint32_t *) (((uintptr_t) src) + y_first * srcstride));
        dst = ((uint32_t *) (((uintptr_t) dst) + 2 * y_first * dststride));

        // convert first-1 line to yuv
        convert_yuv_32(PREV_SRC_LINE, yuvsrc3, width);
    }

    // convert first line to yuv
    convert_yuv_32(src, yuvsrc1, width);

    if ((y_last - y_first) == 1)
    {
        if (y_last != height)
        {
            // convert second line to yuv
            convert_yuv_32(NEXT_SRC_LINE, yuvsrc2, width);
        }

        // calculate pattern for first line
        calculate_pattern((y_first != 0)?yuvsrc3:yuvsrc1, yuvsrc1, (y_last != height)?yuvsrc2:yuvsrc1, patternsrc1, width);

        fill_dst((y_first != 0)?PREV_SRC_LINE:src, src, (y_last != height)?NEXT_SRC_LINE:src, dst, width, dststride, patternsrc1);

        return;
    }

    // convert second line to yuv
    convert_yuv_32(NEXT_SRC_LINE, yuvsrc2, width);

    // calculate pattern for first line
    calculate_pattern((y_first != 0)?yuvsrc3:yuvsrc1, yuvsrc1, yuvsrc2, patternsrc1, width);

    fill_dst((y_first != 0)?PREV_SRC_LINE:src, src, NEXT_SRC_LINE, dst, width, dststride, patternsrc1);

    src = NEXT_SRC_LINE;
    dst = ((uint32_t *) (((uintptr_t) dst) + 2 * dststride));

    // middle lines:
    for (j = (y_last - y_first) - 2; j != 0; j--)
    {
        // convert "third" line to yuv
        convert_yuv_32(NEXT_SRC_LINE, yuvsrc3, width);

        // calculate pattern for "second" line
        calculate_pattern(yuvsrc1, yuvsrc2, yuvsrc3, patternsrc1, width);

        fill_dst(PREV_SRC_LINE, src, NEXT_SRC_LINE, dst, width, dststride, patternsrc1);

        src = NEXT_SRC_LINE;
        dst = ((uint32_t *) (((uintptr_t) dst) + 2 * dststride));

        // "rename" buffers
        {
            uint32_t *yuvsrc;
            yuvsrc = yuvsrc1;
            yuvsrc1 = yuvsrc2;
            yuvsrc2 = yuvsrc3;
            yuvsrc3 = yuvsrc;
        }
    }

    // last line:
    if (y_last != height)
    {
        // convert last+1 line to yuv
        convert_yuv_32(NEXT_SRC_LINE, yuvsrc3, width);
    }

    // calculate pattern for last line
    calculate_pattern(yuvsrc1, yuvsrc2, (y_last != height)?yuvsrc3:yuvsrc2, patternsrc1, width);

    fill_dst(PREV_SRC_LINE, src, (y_last != height)?NEXT_SRC_LINE:src, dst, width, dststride, patternsrc1);

#undef PREV_SRC_LINE
#undef NEXT_SRC_LINE
}

