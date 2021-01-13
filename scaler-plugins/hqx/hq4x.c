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
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include "hqx.h"
#include "hqx-common.h"
#include "hqx-interp.h"
#include "hqx-platform.h"


#define PIXEL00_0     *dst = w[5];
#define PIXEL00_11    *dst = Interp1(w[5], w[4]);
#define PIXEL00_12    *dst = Interp1(w[5], w[2]);
#define PIXEL00_20    *dst = Interp2(w[5], w[2], w[4]);
#define PIXEL00_50    *dst = Interp5(w[2], w[4]);
#define PIXEL00_80    *dst = Interp8(w[5], w[1]);
#define PIXEL00_81    *dst = Interp8(w[5], w[4]);
#define PIXEL00_82    *dst = Interp8(w[5], w[2]);
#define PIXEL01_0     *(dst+1) = w[5];
#define PIXEL01_10    *(dst+1) = Interp1(w[5], w[1]);
#define PIXEL01_12    *(dst+1) = Interp1(w[5], w[2]);
#define PIXEL01_14    *(dst+1) = Interp1(w[2], w[5]);
#define PIXEL01_21    *(dst+1) = Interp2(w[2], w[5], w[4]);
#define PIXEL01_31    *(dst+1) = Interp3(w[5], w[4]);
#define PIXEL01_50    *(dst+1) = Interp5(w[2], w[5]);
#define PIXEL01_60    *(dst+1) = Interp6(w[5], w[2], w[4]);
#define PIXEL01_61    *(dst+1) = Interp6(w[5], w[2], w[1]);
#define PIXEL01_82    *(dst+1) = Interp8(w[5], w[2]);
#define PIXEL01_83    *(dst+1) = Interp8(w[2], w[4]);
#define PIXEL02_0     *(dst+2) = w[5];
#define PIXEL02_10    *(dst+2) = Interp1(w[5], w[3]);
#define PIXEL02_11    *(dst+2) = Interp1(w[5], w[2]);
#define PIXEL02_13    *(dst+2) = Interp1(w[2], w[5]);
#define PIXEL02_21    *(dst+2) = Interp2(w[2], w[5], w[6]);
#define PIXEL02_32    *(dst+2) = Interp3(w[5], w[6]);
#define PIXEL02_50    *(dst+2) = Interp5(w[2], w[5]);
#define PIXEL02_60    *(dst+2) = Interp6(w[5], w[2], w[6]);
#define PIXEL02_61    *(dst+2) = Interp6(w[5], w[2], w[3]);
#define PIXEL02_81    *(dst+2) = Interp8(w[5], w[2]);
#define PIXEL02_83    *(dst+2) = Interp8(w[2], w[6]);
#define PIXEL03_0     *(dst+3) = w[5];
#define PIXEL03_11    *(dst+3) = Interp1(w[5], w[2]);
#define PIXEL03_12    *(dst+3) = Interp1(w[5], w[6]);
#define PIXEL03_20    *(dst+3) = Interp2(w[5], w[2], w[6]);
#define PIXEL03_50    *(dst+3) = Interp5(w[2], w[6]);
#define PIXEL03_80    *(dst+3) = Interp8(w[5], w[3]);
#define PIXEL03_81    *(dst+3) = Interp8(w[5], w[2]);
#define PIXEL03_82    *(dst+3) = Interp8(w[5], w[6]);
#define PIXEL10_0     *(DST2) = w[5];
#define PIXEL10_10    *(DST2) = Interp1(w[5], w[1]);
#define PIXEL10_11    *(DST2) = Interp1(w[5], w[4]);
#define PIXEL10_13    *(DST2) = Interp1(w[4], w[5]);
#define PIXEL10_21    *(DST2) = Interp2(w[4], w[5], w[2]);
#define PIXEL10_32    *(DST2) = Interp3(w[5], w[2]);
#define PIXEL10_50    *(DST2) = Interp5(w[4], w[5]);
#define PIXEL10_60    *(DST2) = Interp6(w[5], w[4], w[2]);
#define PIXEL10_61    *(DST2) = Interp6(w[5], w[4], w[1]);
#define PIXEL10_81    *(DST2) = Interp8(w[5], w[4]);
#define PIXEL10_83    *(DST2) = Interp8(w[4], w[2]);
#define PIXEL11_0     *(DST2+1) = w[5];
#define PIXEL11_30    *(DST2+1) = Interp3(w[5], w[1]);
#define PIXEL11_31    *(DST2+1) = Interp3(w[5], w[4]);
#define PIXEL11_32    *(DST2+1) = Interp3(w[5], w[2]);
#define PIXEL11_70    *(DST2+1) = Interp7(w[5], w[4], w[2]);
#define PIXEL12_0     *(DST2+2) = w[5];
#define PIXEL12_30    *(DST2+2) = Interp3(w[5], w[3]);
#define PIXEL12_31    *(DST2+2) = Interp3(w[5], w[2]);
#define PIXEL12_32    *(DST2+2) = Interp3(w[5], w[6]);
#define PIXEL12_70    *(DST2+2) = Interp7(w[5], w[6], w[2]);
#define PIXEL13_0     *(DST2+3) = w[5];
#define PIXEL13_10    *(DST2+3) = Interp1(w[5], w[3]);
#define PIXEL13_12    *(DST2+3) = Interp1(w[5], w[6]);
#define PIXEL13_14    *(DST2+3) = Interp1(w[6], w[5]);
#define PIXEL13_21    *(DST2+3) = Interp2(w[6], w[5], w[2]);
#define PIXEL13_31    *(DST2+3) = Interp3(w[5], w[2]);
#define PIXEL13_50    *(DST2+3) = Interp5(w[6], w[5]);
#define PIXEL13_60    *(DST2+3) = Interp6(w[5], w[6], w[2]);
#define PIXEL13_61    *(DST2+3) = Interp6(w[5], w[6], w[3]);
#define PIXEL13_82    *(DST2+3) = Interp8(w[5], w[6]);
#define PIXEL13_83    *(DST2+3) = Interp8(w[6], w[2]);
#define PIXEL20_0     *(DST3) = w[5];
#define PIXEL20_10    *(DST3) = Interp1(w[5], w[7]);
#define PIXEL20_12    *(DST3) = Interp1(w[5], w[4]);
#define PIXEL20_14    *(DST3) = Interp1(w[4], w[5]);
#define PIXEL20_21    *(DST3) = Interp2(w[4], w[5], w[8]);
#define PIXEL20_31    *(DST3) = Interp3(w[5], w[8]);
#define PIXEL20_50    *(DST3) = Interp5(w[4], w[5]);
#define PIXEL20_60    *(DST3) = Interp6(w[5], w[4], w[8]);
#define PIXEL20_61    *(DST3) = Interp6(w[5], w[4], w[7]);
#define PIXEL20_82    *(DST3) = Interp8(w[5], w[4]);
#define PIXEL20_83    *(DST3) = Interp8(w[4], w[8]);
#define PIXEL21_0     *(DST3+1) = w[5];
#define PIXEL21_30    *(DST3+1) = Interp3(w[5], w[7]);
#define PIXEL21_31    *(DST3+1) = Interp3(w[5], w[8]);
#define PIXEL21_32    *(DST3+1) = Interp3(w[5], w[4]);
#define PIXEL21_70    *(DST3+1) = Interp7(w[5], w[4], w[8]);
#define PIXEL22_0     *(DST3+2) = w[5];
#define PIXEL22_30    *(DST3+2) = Interp3(w[5], w[9]);
#define PIXEL22_31    *(DST3+2) = Interp3(w[5], w[6]);
#define PIXEL22_32    *(DST3+2) = Interp3(w[5], w[8]);
#define PIXEL22_70    *(DST3+2) = Interp7(w[5], w[6], w[8]);
#define PIXEL23_0     *(DST3+3) = w[5];
#define PIXEL23_10    *(DST3+3) = Interp1(w[5], w[9]);
#define PIXEL23_11    *(DST3+3) = Interp1(w[5], w[6]);
#define PIXEL23_13    *(DST3+3) = Interp1(w[6], w[5]);
#define PIXEL23_21    *(DST3+3) = Interp2(w[6], w[5], w[8]);
#define PIXEL23_32    *(DST3+3) = Interp3(w[5], w[8]);
#define PIXEL23_50    *(DST3+3) = Interp5(w[6], w[5]);
#define PIXEL23_60    *(DST3+3) = Interp6(w[5], w[6], w[8]);
#define PIXEL23_61    *(DST3+3) = Interp6(w[5], w[6], w[9]);
#define PIXEL23_81    *(DST3+3) = Interp8(w[5], w[6]);
#define PIXEL23_83    *(DST3+3) = Interp8(w[6], w[8]);
#define PIXEL30_0     *(DST4) = w[5];
#define PIXEL30_11    *(DST4) = Interp1(w[5], w[8]);
#define PIXEL30_12    *(DST4) = Interp1(w[5], w[4]);
#define PIXEL30_20    *(DST4) = Interp2(w[5], w[8], w[4]);
#define PIXEL30_50    *(DST4) = Interp5(w[8], w[4]);
#define PIXEL30_80    *(DST4) = Interp8(w[5], w[7]);
#define PIXEL30_81    *(DST4) = Interp8(w[5], w[8]);
#define PIXEL30_82    *(DST4) = Interp8(w[5], w[4]);
#define PIXEL31_0     *(DST4+1) = w[5];
#define PIXEL31_10    *(DST4+1) = Interp1(w[5], w[7]);
#define PIXEL31_11    *(DST4+1) = Interp1(w[5], w[8]);
#define PIXEL31_13    *(DST4+1) = Interp1(w[8], w[5]);
#define PIXEL31_21    *(DST4+1) = Interp2(w[8], w[5], w[4]);
#define PIXEL31_32    *(DST4+1) = Interp3(w[5], w[4]);
#define PIXEL31_50    *(DST4+1) = Interp5(w[8], w[5]);
#define PIXEL31_60    *(DST4+1) = Interp6(w[5], w[8], w[4]);
#define PIXEL31_61    *(DST4+1) = Interp6(w[5], w[8], w[7]);
#define PIXEL31_81    *(DST4+1) = Interp8(w[5], w[8]);
#define PIXEL31_83    *(DST4+1) = Interp8(w[8], w[4]);
#define PIXEL32_0     *(DST4+2) = w[5];
#define PIXEL32_10    *(DST4+2) = Interp1(w[5], w[9]);
#define PIXEL32_12    *(DST4+2) = Interp1(w[5], w[8]);
#define PIXEL32_14    *(DST4+2) = Interp1(w[8], w[5]);
#define PIXEL32_21    *(DST4+2) = Interp2(w[8], w[5], w[6]);
#define PIXEL32_31    *(DST4+2) = Interp3(w[5], w[6]);
#define PIXEL32_50    *(DST4+2) = Interp5(w[8], w[5]);
#define PIXEL32_60    *(DST4+2) = Interp6(w[5], w[8], w[6]);
#define PIXEL32_61    *(DST4+2) = Interp6(w[5], w[8], w[9]);
#define PIXEL32_82    *(DST4+2) = Interp8(w[5], w[8]);
#define PIXEL32_83    *(DST4+2) = Interp8(w[8], w[6]);
#define PIXEL33_0     *(DST4+3) = w[5];
#define PIXEL33_11    *(DST4+3) = Interp1(w[5], w[6]);
#define PIXEL33_12    *(DST4+3) = Interp1(w[5], w[8]);
#define PIXEL33_20    *(DST4+3) = Interp2(w[5], w[8], w[6]);
#define PIXEL33_50    *(DST4+3) = Interp5(w[8], w[6]);
#define PIXEL33_80    *(DST4+3) = Interp8(w[5], w[9]);
#define PIXEL33_81    *(DST4+3) = Interp8(w[5], w[6]);
#define PIXEL33_82    *(DST4+3) = Interp8(w[5], w[8]);

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
#define DST4 ((uint32_t *) (((uintptr_t) dst) + 3 * dststride))

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
                    PIXEL01_60
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 2:
            case 34:
            case 130:
            case 162:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 16:
            case 17:
            case 48:
            case 49:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 64:
            case 65:
            case 68:
            case 69:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 8:
            case 12:
            case 136:
            case 140:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 3:
            case 35:
            case 131:
            case 163:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 6:
            case 38:
            case 134:
            case 166:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 20:
            case 21:
            case 52:
            case 53:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 144:
            case 145:
            case 176:
            case 177:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 192:
            case 193:
            case 196:
            case 197:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 96:
            case 97:
            case 100:
            case 101:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 40:
            case 44:
            case 168:
            case 172:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 9:
            case 13:
            case 137:
            case 141:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 18:
            case 50:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL12_0
                        PIXEL13_50
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 80:
            case 81:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_61
                    PIXEL21_30
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 72:
            case 76:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_70
                    PIXEL13_60
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL21_0
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 10:
            case 138:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                        PIXEL11_0
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 66:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 24:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 7:
            case 39:
            case 135:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 148:
            case 149:
            case 180:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 224:
            case 228:
            case 225:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 41:
            case 169:
            case 45:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 22:
            case 54:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_0
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 208:
            case 209:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 104:
            case 108:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_70
                    PIXEL13_60
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 11:
            case 139:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 19:
            case 51:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL00_81
                        PIXEL01_31
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL00_12
                        PIXEL01_14
                        PIXEL02_83
                        PIXEL03_50
                        PIXEL12_70
                        PIXEL13_21
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 146:
            case 178:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                        PIXEL23_32
                        PIXEL33_82
                    }
                    else
                    {
                        PIXEL02_21
                        PIXEL03_50
                        PIXEL12_70
                        PIXEL13_83
                        PIXEL23_13
                        PIXEL33_11
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_32
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_82
                    break;
                }
            case 84:
            case 85:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_81
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL03_81
                        PIXEL13_31
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL03_12
                        PIXEL13_14
                        PIXEL22_70
                        PIXEL23_83
                        PIXEL32_21
                        PIXEL33_50
                    }
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_31
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 112:
            case 113:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_82
                    PIXEL21_32
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL30_82
                        PIXEL31_32
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_70
                        PIXEL23_21
                        PIXEL30_11
                        PIXEL31_13
                        PIXEL32_83
                        PIXEL33_50
                    }
                    break;
                }
            case 200:
            case 204:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_70
                    PIXEL13_60
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                        PIXEL32_31
                        PIXEL33_81
                    }
                    else
                    {
                        PIXEL20_21
                        PIXEL21_70
                        PIXEL30_50
                        PIXEL31_83
                        PIXEL32_14
                        PIXEL33_12
                    }
                    PIXEL22_31
                    PIXEL23_81
                    break;
                }
            case 73:
            case 77:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL00_82
                        PIXEL10_32
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL00_11
                        PIXEL10_13
                        PIXEL20_83
                        PIXEL21_70
                        PIXEL30_50
                        PIXEL31_21
                    }
                    PIXEL01_82
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL11_32
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 42:
            case 170:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                        PIXEL20_31
                        PIXEL30_81
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_21
                        PIXEL10_83
                        PIXEL11_70
                        PIXEL20_14
                        PIXEL30_12
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL21_31
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL31_81
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 14:
            case 142:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL02_32
                        PIXEL03_82
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_83
                        PIXEL02_13
                        PIXEL03_11
                        PIXEL10_21
                        PIXEL11_70
                    }
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 67:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 70:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 28:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 152:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 194:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 98:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 56:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 25:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 26:
            case 31:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL11_0
                    PIXEL12_0
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 82:
            case 214:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_0
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 88:
            case 248:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_10
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    break;
                }
            case 74:
            case 107:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_61
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 27:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 86:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_0
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 216:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 106:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_61
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 30:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_0
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 210:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 120:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_10
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 75:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 29:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 198:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 184:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 99:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 57:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 71:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 156:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 226:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 60:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 195:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 102:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 153:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 58:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 83:
                {
                    PIXEL00_81
                    PIXEL01_31
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL20_61
                    PIXEL21_30
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 92:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_31
                    PIXEL13_31
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    break;
                }
            case 202:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL12_30
                    PIXEL13_61
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 78:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL12_32
                    PIXEL13_82
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 154:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 114:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL20_82
                    PIXEL21_32
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    PIXEL30_82
                    PIXEL31_32
                    break;
                }
            case 89:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_30
                    PIXEL13_10
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    break;
                }
            case 90:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    break;
                }
            case 55:
            case 23:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL00_81
                        PIXEL01_31
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL12_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL00_12
                        PIXEL01_14
                        PIXEL02_83
                        PIXEL03_50
                        PIXEL12_70
                        PIXEL13_21
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 182:
            case 150:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL12_0
                        PIXEL13_0
                        PIXEL23_32
                        PIXEL33_82
                    }
                    else
                    {
                        PIXEL02_21
                        PIXEL03_50
                        PIXEL12_70
                        PIXEL13_83
                        PIXEL23_13
                        PIXEL33_11
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_32
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_82
                    break;
                }
            case 213:
            case 212:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_81
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL03_81
                        PIXEL13_31
                        PIXEL22_0
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL03_12
                        PIXEL13_14
                        PIXEL22_70
                        PIXEL23_83
                        PIXEL32_21
                        PIXEL33_50
                    }
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_31
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 241:
            case 240:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_82
                    PIXEL21_32
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_0
                        PIXEL23_0
                        PIXEL30_82
                        PIXEL31_32
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL22_70
                        PIXEL23_21
                        PIXEL30_11
                        PIXEL31_13
                        PIXEL32_83
                        PIXEL33_50
                    }
                    break;
                }
            case 236:
            case 232:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_70
                    PIXEL13_60
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL21_0
                        PIXEL30_0
                        PIXEL31_0
                        PIXEL32_31
                        PIXEL33_81
                    }
                    else
                    {
                        PIXEL20_21
                        PIXEL21_70
                        PIXEL30_50
                        PIXEL31_83
                        PIXEL32_14
                        PIXEL33_12
                    }
                    PIXEL22_31
                    PIXEL23_81
                    break;
                }
            case 109:
            case 105:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL00_82
                        PIXEL10_32
                        PIXEL20_0
                        PIXEL21_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL00_11
                        PIXEL10_13
                        PIXEL20_83
                        PIXEL21_70
                        PIXEL30_50
                        PIXEL31_21
                    }
                    PIXEL01_82
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL11_32
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 171:
            case 43:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                        PIXEL11_0
                        PIXEL20_31
                        PIXEL30_81
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_21
                        PIXEL10_83
                        PIXEL11_70
                        PIXEL20_14
                        PIXEL30_12
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL21_31
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL31_81
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 143:
            case 15:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL02_32
                        PIXEL03_82
                        PIXEL10_0
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_83
                        PIXEL02_13
                        PIXEL03_11
                        PIXEL10_21
                        PIXEL11_70
                    }
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 124:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_31
                    PIXEL13_31
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 203:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 62:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_0
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 211:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 118:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_0
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 217:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 110:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_32
                    PIXEL13_82
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 155:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 188:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 185:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 61:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 157:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 103:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 227:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 230:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 199:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 220:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_31
                    PIXEL13_31
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    break;
                }
            case 158:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL12_0
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 234:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL12_30
                    PIXEL13_61
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 242:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_82
                    PIXEL31_32
                    break;
                }
            case 59:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL11_0
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 121:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_30
                    PIXEL13_10
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    break;
                }
            case 87:
                {
                    PIXEL00_81
                    PIXEL01_31
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_0
                    PIXEL20_61
                    PIXEL21_30
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 79:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL11_0
                    PIXEL12_32
                    PIXEL13_82
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 122:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    break;
                }
            case 94:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL12_0
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    break;
                }
            case 218:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    break;
                }
            case 91:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL11_0
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    break;
                }
            case 229:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 167:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 173:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 181:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 186:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 115:
                {
                    PIXEL00_81
                    PIXEL01_31
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL20_82
                    PIXEL21_32
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    PIXEL30_82
                    PIXEL31_32
                    break;
                }
            case 93:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_31
                    PIXEL13_31
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    break;
                }
            case 206:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL12_32
                    PIXEL13_82
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 205:
            case 201:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_70
                    PIXEL13_60
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_10
                        PIXEL21_30
                        PIXEL30_80
                        PIXEL31_10
                    }
                    else
                    {
                        PIXEL20_12
                        PIXEL21_0
                        PIXEL30_20
                        PIXEL31_11
                    }
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 174:
            case 46:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_80
                        PIXEL01_10
                        PIXEL10_10
                        PIXEL11_30
                    }
                    else
                    {
                        PIXEL00_20
                        PIXEL01_12
                        PIXEL10_11
                        PIXEL11_0
                    }
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 179:
            case 147:
                {
                    PIXEL00_81
                    PIXEL01_31
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_10
                        PIXEL03_80
                        PIXEL12_30
                        PIXEL13_10
                    }
                    else
                    {
                        PIXEL02_11
                        PIXEL03_20
                        PIXEL12_0
                        PIXEL13_12
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 117:
            case 116:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_82
                    PIXEL21_32
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_30
                        PIXEL23_10
                        PIXEL32_10
                        PIXEL33_80
                    }
                    else
                    {
                        PIXEL22_0
                        PIXEL23_11
                        PIXEL32_12
                        PIXEL33_20
                    }
                    PIXEL30_82
                    PIXEL31_32
                    break;
                }
            case 189:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 231:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 126:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_0
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 219:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 125:
                {
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL00_82
                        PIXEL10_32
                        PIXEL20_0
                        PIXEL21_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL00_11
                        PIXEL10_13
                        PIXEL20_83
                        PIXEL21_70
                        PIXEL30_50
                        PIXEL31_21
                    }
                    PIXEL01_82
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL11_32
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 221:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_81
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL03_81
                        PIXEL13_31
                        PIXEL22_0
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL03_12
                        PIXEL13_14
                        PIXEL22_70
                        PIXEL23_83
                        PIXEL32_21
                        PIXEL33_50
                    }
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_31
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 207:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL02_32
                        PIXEL03_82
                        PIXEL10_0
                        PIXEL11_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_83
                        PIXEL02_13
                        PIXEL03_11
                        PIXEL10_21
                        PIXEL11_70
                    }
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_31
                    PIXEL23_81
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_31
                    PIXEL33_81
                    break;
                }
            case 238:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_32
                    PIXEL13_82
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL21_0
                        PIXEL30_0
                        PIXEL31_0
                        PIXEL32_31
                        PIXEL33_81
                    }
                    else
                    {
                        PIXEL20_21
                        PIXEL21_70
                        PIXEL30_50
                        PIXEL31_83
                        PIXEL32_14
                        PIXEL33_12
                    }
                    PIXEL22_31
                    PIXEL23_81
                    break;
                }
            case 190:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL12_0
                        PIXEL13_0
                        PIXEL23_32
                        PIXEL33_82
                    }
                    else
                    {
                        PIXEL02_21
                        PIXEL03_50
                        PIXEL12_70
                        PIXEL13_83
                        PIXEL23_13
                        PIXEL33_11
                    }
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_32
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_82
                    break;
                }
            case 187:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                        PIXEL11_0
                        PIXEL20_31
                        PIXEL30_81
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_21
                        PIXEL10_83
                        PIXEL11_70
                        PIXEL20_14
                        PIXEL30_12
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL21_31
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL31_81
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 243:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_82
                    PIXEL21_32
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL22_0
                        PIXEL23_0
                        PIXEL30_82
                        PIXEL31_32
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL22_70
                        PIXEL23_21
                        PIXEL30_11
                        PIXEL31_13
                        PIXEL32_83
                        PIXEL33_50
                    }
                    break;
                }
            case 119:
                {
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL00_81
                        PIXEL01_31
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL12_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL00_12
                        PIXEL01_14
                        PIXEL02_83
                        PIXEL03_50
                        PIXEL12_70
                        PIXEL13_21
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 237:
            case 233:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_60
                    PIXEL03_20
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_70
                    PIXEL13_60
                    PIXEL20_0
                    PIXEL21_0
                    PIXEL22_31
                    PIXEL23_81
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL30_0
                    }
                    else
                    {
                        PIXEL30_20
                    }
                    PIXEL31_0
                    PIXEL32_31
                    PIXEL33_81
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
                        PIXEL00_20
                    }
                    PIXEL01_0
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_0
                    PIXEL11_0
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_70
                    PIXEL23_60
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_60
                    PIXEL33_20
                    break;
                }
            case 183:
            case 151:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_0
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL03_0
                    }
                    else
                    {
                        PIXEL03_20
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_0
                    PIXEL13_0
                    PIXEL20_60
                    PIXEL21_70
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_20
                    PIXEL31_60
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 245:
            case 244:
                {
                    PIXEL00_20
                    PIXEL01_60
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_60
                    PIXEL11_70
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_0
                    PIXEL23_0
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL33_20
                    }
                    break;
                }
            case 250:
                {
                    PIXEL00_80
                    PIXEL01_10
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_30
                    PIXEL13_10
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    break;
                }
            case 123:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_10
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 95:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL11_0
                    PIXEL12_0
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_80
                    PIXEL31_10
                    PIXEL32_10
                    PIXEL33_80
                    break;
                }
            case 222:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_0
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 252:
                {
                    PIXEL00_80
                    PIXEL01_61
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_31
                    PIXEL13_31
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_0
                    PIXEL23_0
                    PIXEL32_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL33_20
                    }
                    break;
                }
            case 249:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_61
                    PIXEL03_80
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_0
                    PIXEL21_0
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL30_0
                    }
                    else
                    {
                        PIXEL30_20
                    }
                    PIXEL31_0
                    break;
                }
            case 235:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_61
                    PIXEL20_0
                    PIXEL21_0
                    PIXEL22_31
                    PIXEL23_81
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL30_0
                    }
                    else
                    {
                        PIXEL30_20
                    }
                    PIXEL31_0
                    PIXEL32_31
                    PIXEL33_81
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
                        PIXEL00_20
                    }
                    PIXEL01_0
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_0
                    PIXEL11_0
                    PIXEL12_32
                    PIXEL13_82
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_61
                    PIXEL32_10
                    PIXEL33_80
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
                        PIXEL00_20
                    }
                    PIXEL01_0
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_0
                    PIXEL11_0
                    PIXEL12_0
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_61
                    PIXEL33_80
                    break;
                }
            case 159:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_0
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL03_0
                    }
                    else
                    {
                        PIXEL03_20
                    }
                    PIXEL11_0
                    PIXEL12_0
                    PIXEL13_0
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_80
                    PIXEL31_61
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 215:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_0
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL03_0
                    }
                    else
                    {
                        PIXEL03_20
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_0
                    PIXEL13_0
                    PIXEL20_61
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 246:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_61
                    PIXEL11_30
                    PIXEL12_0
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_0
                    PIXEL23_0
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL33_20
                    }
                    break;
                }
            case 254:
                {
                    PIXEL00_80
                    PIXEL01_10
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_10
                    PIXEL11_30
                    PIXEL12_0
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_0
                    PIXEL23_0
                    PIXEL32_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL33_20
                    }
                    break;
                }
            case 253:
                {
                    PIXEL00_82
                    PIXEL01_82
                    PIXEL02_81
                    PIXEL03_81
                    PIXEL10_32
                    PIXEL11_32
                    PIXEL12_31
                    PIXEL13_31
                    PIXEL20_0
                    PIXEL21_0
                    PIXEL22_0
                    PIXEL23_0
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL30_0
                    }
                    else
                    {
                        PIXEL30_20
                    }
                    PIXEL31_0
                    PIXEL32_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL33_20
                    }
                    break;
                }
            case 251:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_10
                    PIXEL03_80
                    PIXEL11_0
                    PIXEL12_30
                    PIXEL13_10
                    PIXEL20_0
                    PIXEL21_0
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL30_0
                    }
                    else
                    {
                        PIXEL30_20
                    }
                    PIXEL31_0
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
                        PIXEL00_20
                    }
                    PIXEL01_0
                    PIXEL02_32
                    PIXEL03_82
                    PIXEL10_0
                    PIXEL11_0
                    PIXEL12_32
                    PIXEL13_82
                    PIXEL20_0
                    PIXEL21_0
                    PIXEL22_31
                    PIXEL23_81
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL30_0
                    }
                    else
                    {
                        PIXEL30_20
                    }
                    PIXEL31_0
                    PIXEL32_31
                    PIXEL33_81
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
                        PIXEL00_20
                    }
                    PIXEL01_0
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL02_0
                        PIXEL03_0
                        PIXEL13_0
                    }
                    else
                    {
                        PIXEL02_50
                        PIXEL03_50
                        PIXEL13_50
                    }
                    PIXEL10_0
                    PIXEL11_0
                    PIXEL12_0
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL20_0
                        PIXEL30_0
                        PIXEL31_0
                    }
                    else
                    {
                        PIXEL20_50
                        PIXEL30_50
                        PIXEL31_50
                    }
                    PIXEL21_0
                    PIXEL22_30
                    PIXEL23_10
                    PIXEL32_10
                    PIXEL33_80
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
                        PIXEL00_20
                    }
                    PIXEL01_0
                    PIXEL02_0
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL03_0
                    }
                    else
                    {
                        PIXEL03_20
                    }
                    PIXEL10_0
                    PIXEL11_0
                    PIXEL12_0
                    PIXEL13_0
                    PIXEL20_31
                    PIXEL21_31
                    PIXEL22_32
                    PIXEL23_32
                    PIXEL30_81
                    PIXEL31_81
                    PIXEL32_82
                    PIXEL33_82
                    break;
                }
            case 223:
                {
                    if (patternTable[i-1] & (1 << 2)) // Diff(w[4], w[2])
                    {
                        PIXEL00_0
                        PIXEL01_0
                        PIXEL10_0
                    }
                    else
                    {
                        PIXEL00_50
                        PIXEL01_50
                        PIXEL10_50
                    }
                    PIXEL02_0
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL03_0
                    }
                    else
                    {
                        PIXEL03_20
                    }
                    PIXEL11_0
                    PIXEL12_0
                    PIXEL13_0
                    PIXEL20_10
                    PIXEL21_30
                    PIXEL22_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL23_0
                        PIXEL32_0
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL23_50
                        PIXEL32_50
                        PIXEL33_50
                    }
                    PIXEL30_80
                    PIXEL31_10
                    break;
                }
            case 247:
                {
                    PIXEL00_81
                    PIXEL01_31
                    PIXEL02_0
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL03_0
                    }
                    else
                    {
                        PIXEL03_20
                    }
                    PIXEL10_81
                    PIXEL11_31
                    PIXEL12_0
                    PIXEL13_0
                    PIXEL20_82
                    PIXEL21_32
                    PIXEL22_0
                    PIXEL23_0
                    PIXEL30_82
                    PIXEL31_32
                    PIXEL32_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL33_20
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
                        PIXEL00_20
                    }
                    PIXEL01_0
                    PIXEL02_0
                    if (patternTable[i+1] & (1 << 0)) // Diff(w[2], w[6])
                    {
                        PIXEL03_0
                    }
                    else
                    {
                        PIXEL03_20
                    }
                    PIXEL10_0
                    PIXEL11_0
                    PIXEL12_0
                    PIXEL13_0
                    PIXEL20_0
                    PIXEL21_0
                    PIXEL22_0
                    PIXEL23_0
                    if (patternTable[i-1] & (1 << 7)) // Diff(w[8], w[4])
                    {
                        PIXEL30_0
                    }
                    else
                    {
                        PIXEL30_20
                    }
                    PIXEL31_0
                    PIXEL32_0
                    if (patternTable[i+1] & (1 << 5)) // Diff(w[6], w[8])
                    {
                        PIXEL33_0
                    }
                    else
                    {
                        PIXEL33_20
                    }
                    break;
                }
        }
        rgbsrc1++;
        rgbsrc2++;
        rgbsrc3++;
        dst += 4;
    }

#undef DST4
#undef DST3
#undef DST2
}

#ifdef __cplusplus
extern "C"
#endif
void hq4x_32(const uint32_t *src, uint32_t *dst, unsigned int width, unsigned int srcstride, unsigned int dststride, unsigned int height, unsigned int y_first, unsigned int y_last)
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
        dst = ((uint32_t *) (((uintptr_t) dst) + 4 * y_first * dststride));

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
    dst += dststride;

    // middle lines:
    for (j = (y_last - y_first) - 2; j != 0; j--)
    {
        // convert "third" line to yuv
        convert_yuv_32(NEXT_SRC_LINE, yuvsrc3, width);

        // calculate pattern for "second" line
        calculate_pattern(yuvsrc1, yuvsrc2, yuvsrc3, patternsrc1, width);

        fill_dst(PREV_SRC_LINE, src, NEXT_SRC_LINE, dst, width, dststride, patternsrc1);

        src = NEXT_SRC_LINE;
        dst += dststride;

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

