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

#include <stdint.h>
#include <alloca.h>
#include "hqx.h"
#include "hqx-common.h"
#include "hqx-interp.h"
#include "hqx-platform.h"


#define PIXEL00_1M  *dst = Interp1(w[5], w[1]);
#define PIXEL00_1U  *dst = Interp1(w[5], w[2]);
#define PIXEL00_1L  *dst = Interp1(w[5], w[4]);
#define PIXEL00_2   *dst = Interp2(w[5], w[4], w[2]);
#define PIXEL00_4   *dst = Interp4(w[5], w[4], w[2]);
#define PIXEL00_5   *dst = Interp5(w[4], w[2]);
#define PIXEL00_C   *dst = w[5];

#define PIXEL01_1   *(dst+1) = Interp1(w[5], w[2]);
#define PIXEL01_3   *(dst+1) = Interp3(w[5], w[2]);
#define PIXEL01_6   *(dst+1) = Interp1(w[2], w[5]);
#define PIXEL01_C   *(dst+1) = w[5];

#define PIXEL02_1M  *(dst+2) = Interp1(w[5], w[3]);
#define PIXEL02_1U  *(dst+2) = Interp1(w[5], w[2]);
#define PIXEL02_1R  *(dst+2) = Interp1(w[5], w[6]);
#define PIXEL02_2   *(dst+2) = Interp2(w[5], w[2], w[6]);
#define PIXEL02_4   *(dst+2) = Interp4(w[5], w[2], w[6]);
#define PIXEL02_5   *(dst+2) = Interp5(w[2], w[6]);
#define PIXEL02_C   *(dst+2) = w[5];

#define PIXEL10_1   *(DST2) = Interp1(w[5], w[4]);
#define PIXEL10_3   *(DST2) = Interp3(w[5], w[4]);
#define PIXEL10_6   *(DST2) = Interp1(w[4], w[5]);
#define PIXEL10_C   *(DST2) = w[5];

#define PIXEL11     *(DST2+1) = w[5];

#define PIXEL12_1   *(DST2+2) = Interp1(w[5], w[6]);
#define PIXEL12_3   *(DST2+2) = Interp3(w[5], w[6]);
#define PIXEL12_6   *(DST2+2) = Interp1(w[6], w[5]);
#define PIXEL12_C   *(DST2+2) = w[5];

#define PIXEL20_1M  *(DST3) = Interp1(w[5], w[7]);
#define PIXEL20_1D  *(DST3) = Interp1(w[5], w[8]);
#define PIXEL20_1L  *(DST3) = Interp1(w[5], w[4]);
#define PIXEL20_2   *(DST3) = Interp2(w[5], w[8], w[4]);
#define PIXEL20_4   *(DST3) = Interp4(w[5], w[8], w[4]);
#define PIXEL20_5   *(DST3) = Interp5(w[8], w[4]);
#define PIXEL20_C   *(DST3) = w[5];

#define PIXEL21_1   *(DST3+1) = Interp1(w[5], w[8]);
#define PIXEL21_3   *(DST3+1) = Interp3(w[5], w[8]);
#define PIXEL21_6   *(DST3+1) = Interp1(w[8], w[5]);
#define PIXEL21_C   *(DST3+1) = w[5];

#define PIXEL22_1M  *(DST3+2) = Interp1(w[5], w[9]);
#define PIXEL22_1D  *(DST3+2) = Interp1(w[5], w[8]);
#define PIXEL22_1R  *(DST3+2) = Interp1(w[5], w[6]);
#define PIXEL22_2   *(DST3+2) = Interp2(w[5], w[6], w[8]);
#define PIXEL22_4   *(DST3+2) = Interp4(w[5], w[6], w[8]);
#define PIXEL22_5   *(DST3+2) = Interp5(w[6], w[8]);
#define PIXEL22_C   *(DST3+2) = w[5];

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
#define DST3 ((uint32_t *) (((uintptr_t) dst) + 2 * dststride))

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
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 2:
            case 34:
            case 130:
            case 162:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 16:
            case 17:
            case 48:
            case 49:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 64:
            case 65:
            case 68:
            case 69:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 8:
            case 12:
            case 136:
            case 140:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 3:
            case 35:
            case 131:
            case 163:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 6:
            case 38:
            case 134:
            case 166:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 20:
            case 21:
            case 52:
            case 53:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 144:
            case 145:
            case 176:
            case 177:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 192:
            case 193:
            case 196:
            case 197:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 96:
            case 97:
            case 100:
            case 101:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 40:
            case 44:
            case 168:
            case 172:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 9:
            case 13:
            case 137:
            case 141:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 18:
            case 50:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_1M
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 80:
            case 81:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 72:
            case 76:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_1M
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 10:
            case 138:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 66:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 24:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 7:
            case 39:
            case 135:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 148:
            case 149:
            case 180:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 224:
            case 228:
            case 225:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 41:
            case 169:
            case 45:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 22:
            case 54:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 208:
            case 209:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 104:
            case 108:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 11:
            case 139:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 19:
            case 51:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL00_1L
                        PIXEL01_C
                        PIXEL02_1M
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL00_2
                        PIXEL01_6
                        PIXEL02_5
                        PIXEL12_1
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 146:
            case 178:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_1M
                        PIXEL12_C
                        PIXEL22_1D
                    }
                    else
                    {
                        PIXEL01_1
                        PIXEL02_5
                        PIXEL12_6
                        PIXEL22_2
                    }
                    PIXEL00_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_2
                    PIXEL21_1
                    break;
                }
            case 84:
            case 85:
                {
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL02_1U
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL02_2
                        PIXEL12_6
                        PIXEL21_1
                        PIXEL22_5
                    }
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1M
                    break;
                }
            case 112:
            case 113:
                {
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL20_1L
                        PIXEL21_C
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL12_1
                        PIXEL20_2
                        PIXEL21_6
                        PIXEL22_5
                    }
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    break;
                }
            case 200:
            case 204:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_1M
                        PIXEL21_C
                        PIXEL22_1R
                    }
                    else
                    {
                        PIXEL10_1
                        PIXEL20_5
                        PIXEL21_6
                        PIXEL22_2
                    }
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL11
                    PIXEL12_1
                    break;
                }
            case 73:
            case 77:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL00_1U
                        PIXEL10_C
                        PIXEL20_1M
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL00_2
                        PIXEL10_6
                        PIXEL20_5
                        PIXEL21_1
                    }
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL11
                    PIXEL12_1
                    PIXEL22_1M
                    break;
                }
            case 42:
            case 170:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                        PIXEL01_C
                        PIXEL10_C
                        PIXEL20_1D
                    }
                    else
                    {
                        PIXEL00_5
                        PIXEL01_1
                        PIXEL10_6
                        PIXEL20_2
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_1
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 14:
            case 142:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                        PIXEL01_C
                        PIXEL02_1R
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_5
                        PIXEL01_6
                        PIXEL02_2
                        PIXEL10_1
                    }
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 67:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 70:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 28:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 152:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 194:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 98:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 56:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 25:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 26:
            case 31:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL10_3
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL11
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 82:
            case 214:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 88:
            case 248:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL22_4
                    }
                    break;
                }
            case 74:
            case 107:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                    }
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 27:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 86:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 216:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 106:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 30:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 210:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 120:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 75:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 29:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 198:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 184:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 99:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 57:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 71:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 156:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 226:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 60:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 195:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 102:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 153:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 58:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 83:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 92:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 202:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 78:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 154:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 114:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1L
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 89:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 90:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 55:
            case 23:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL00_1L
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL00_2
                        PIXEL01_6
                        PIXEL02_5
                        PIXEL12_1
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 182:
            case 150:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                        PIXEL22_1D
                    }
                    else
                    {
                        PIXEL01_1
                        PIXEL02_5
                        PIXEL12_6
                        PIXEL22_2
                    }
                    PIXEL00_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_2
                    PIXEL21_1
                    break;
                }
            case 213:
            case 212:
                {
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL02_1U
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL02_2
                        PIXEL12_6
                        PIXEL21_1
                        PIXEL22_5
                    }
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1M
                    break;
                }
            case 241:
            case 240:
                {
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL20_1L
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_1
                        PIXEL20_2
                        PIXEL21_6
                        PIXEL22_5
                    }
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    break;
                }
            case 236:
            case 232:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                        PIXEL22_1R
                    }
                    else
                    {
                        PIXEL10_1
                        PIXEL20_5
                        PIXEL21_6
                        PIXEL22_2
                    }
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL11
                    PIXEL12_1
                    break;
                }
            case 109:
            case 105:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL00_1U
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL00_2
                        PIXEL10_6
                        PIXEL20_5
                        PIXEL21_1
                    }
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL11
                    PIXEL12_1
                    PIXEL22_1M
                    break;
                }
            case 171:
            case 43:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                        PIXEL20_1D
                    }
                    else
                    {
                        PIXEL00_5
                        PIXEL01_1
                        PIXEL10_6
                        PIXEL20_2
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_1
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 143:
            case 15:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL02_1R
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_5
                        PIXEL01_6
                        PIXEL02_2
                        PIXEL10_1
                    }
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 124:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 203:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 62:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 211:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 118:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 217:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 110:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 155:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 188:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 185:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 61:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 157:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 103:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 227:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 230:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 199:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 220:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 158:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 234:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1R
                    break;
                }
            case 242:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1L
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 59:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 121:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 87:
                {
                    PIXEL00_1L
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1M
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 79:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    PIXEL02_1R
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 122:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 94:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_C
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 218:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_C
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 91:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 229:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 167:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 173:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 181:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 186:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 115:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1L
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 93:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 206:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 205:
            case 201:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_1M
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 174:
            case 46:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_1M
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 179:
            case 147:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_1M
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 117:
            case 116:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1L
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_1M
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 189:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 231:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 126:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 219:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                        PIXEL10_3
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 125:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL00_1U
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL00_2
                        PIXEL10_6
                        PIXEL20_5
                        PIXEL21_1
                    }
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL11
                    PIXEL12_C
                    PIXEL22_1M
                    break;
                }
            case 221:
                {
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL02_1U
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL02_2
                        PIXEL12_6
                        PIXEL21_1
                        PIXEL22_5
                    }
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL10_C
                    PIXEL11
                    PIXEL20_1M
                    break;
                }
            case 207:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL02_1R
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_5
                        PIXEL01_6
                        PIXEL02_2
                        PIXEL10_1
                    }
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 238:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                        PIXEL22_1R
                    }
                    else
                    {
                        PIXEL10_1
                        PIXEL20_5
                        PIXEL21_6
                        PIXEL22_2
                    }
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL11
                    PIXEL12_1
                    break;
                }
            case 190:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                        PIXEL22_1D
                    }
                    else
                    {
                        PIXEL01_1
                        PIXEL02_5
                        PIXEL12_6
                        PIXEL22_2
                    }
                    PIXEL00_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL20_1D
                    PIXEL21_1
                    break;
                }
            case 187:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                        PIXEL20_1D
                    }
                    else
                    {
                        PIXEL00_5
                        PIXEL01_1
                        PIXEL10_6
                        PIXEL20_2
                    }
                    PIXEL02_1M
                    PIXEL11
                    PIXEL12_C
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 243:
                {
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL20_1L
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_1
                        PIXEL20_2
                        PIXEL21_6
                        PIXEL22_5
                    }
                    PIXEL00_1L
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL10_1
                    PIXEL11
                    break;
                }
            case 119:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL00_1L
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL00_2
                        PIXEL01_6
                        PIXEL02_5
                        PIXEL12_1
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL20_1L
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 237:
            case 233:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_2
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 175:
            case 47:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_2
                    break;
                }
            case 183:
            case 151:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_2
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 245:
            case 244:
                {
                    PIXEL00_2
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1L
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 250:
                {
                    PIXEL00_1M
                    PIXEL01_C
                    PIXEL02_1M
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL22_4
                    }
                    break;
                }
            case 123:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                    }
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 95:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL10_3
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL11
                    PIXEL20_1M
                    PIXEL21_C
                    PIXEL22_1M
                    break;
                }
            case 222:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 252:
                {
                    PIXEL00_1M
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 249:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL22_4
                    }
                    break;
                }
            case 235:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                    }
                    PIXEL02_1M
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 111:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 63:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1M
                    break;
                }
            case 159:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL10_3
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 215:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 246:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1L
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 254:
                {
                    PIXEL00_1M
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_4
                    }
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_4
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL21_3
                        PIXEL22_2
                    }
                    break;
                }
            case 253:
                {
                    PIXEL00_1U
                    PIXEL01_1
                    PIXEL02_1U
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 251:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL01_3
                    }
                    PIXEL02_1M
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL10_C
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL10_3
                        PIXEL20_2
                        PIXEL21_3
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL12_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL12_3
                        PIXEL22_4
                    }
                    break;
                }
            case 239:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    PIXEL02_1R
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_1
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    PIXEL22_1R
                    break;
                }
            case 127:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL01_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_2
                        PIXEL01_3
                        PIXEL10_3
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL02_4
                        PIXEL12_3
                    }
                    PIXEL11
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                        PIXEL21_C
                    }
                    else
                    {
                        PIXEL20_4
                        PIXEL21_3
                    }
                    PIXEL22_1M
                    break;
                }
            case 191:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1D
                    PIXEL21_1
                    PIXEL22_1D
                    break;
                }
            case 223:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                        PIXEL10_C
                    }
                    else
                    {
                        PIXEL00_4
                        PIXEL10_3
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL01_C
                        PIXEL02_C
                        PIXEL12_C
                    }
                    else
                    {
                        PIXEL01_3
                        PIXEL02_2
                        PIXEL12_3
                    }
                    PIXEL11
                    PIXEL20_1M
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL21_C
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL21_3
                        PIXEL22_4
                    }
                    break;
                }
            case 247:
                {
                    PIXEL00_1L
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_1
                    PIXEL11
                    PIXEL12_C
                    PIXEL20_1L
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
            case 255:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_C
                    }
                    else
                    {
                        PIXEL00_2
                    }
                    PIXEL01_C
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_C
                    }
                    else
                    {
                        PIXEL02_2
                    }
                    PIXEL10_C
                    PIXEL11
                    PIXEL12_C
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_C
                    }
                    else
                    {
                        PIXEL20_2
                    }
                    PIXEL21_C
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_C
                    }
                    else
                    {
                        PIXEL22_2
                    }
                    break;
                }
        }
        rgbsrc1++;
        rgbsrc2++;
        rgbsrc3++;
        dst += 3;
    }

#undef DST3
#undef DST2
}

#ifdef __cplusplus
extern "C"
#endif
void hq3x_32(const uint32_t *src, uint32_t *dst, unsigned int width, unsigned int srcstride, unsigned int dststride, unsigned int height, unsigned int y_first, unsigned int y_last)
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
        dst = ((uint32_t *) (((uintptr_t) dst) + 3 * y_first * dststride));

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
    dst = ((uint32_t *) (((uintptr_t) dst) + 3 * dststride));

    // middle lines:
    for (j = (y_last - y_first) - 2; j != 0; j--)
    {
        // convert "third" line to yuv
        convert_yuv_32(NEXT_SRC_LINE, yuvsrc3, width);

        // calculate pattern for "second" line
        calculate_pattern(yuvsrc1, yuvsrc2, yuvsrc3, patternsrc1, width);

        fill_dst(PREV_SRC_LINE, src, NEXT_SRC_LINE, dst, width, dststride, patternsrc1);

        src = NEXT_SRC_LINE;
        dst = ((uint32_t *) (((uintptr_t) dst) + 3 * dststride));

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

