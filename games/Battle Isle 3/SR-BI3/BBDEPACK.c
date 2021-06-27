/**
 *
 *  Copyright (C) 2020-2021 Roman Pauer
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

#include "BBDEPACK.h"


unsigned int DEPACK_c(const uint8_t *src, uint8_t *dst)
{
    unsigned int multivalue;
    unsigned int dstcounter;
    unsigned int mask;
    unsigned int copyvalue;
    int copyoffset;
    int copycounter;
    uint8_t *copyptr;
    uint8_t *endptr;
    uint8_t backup[2];
    unsigned int srccounter;
    unsigned int length;

    if (!((src[0] == 'T') && (src[1] == 'P') && (src[2] == 'W') && (src[3] == 'M')))
    {
        return 0;
    }

    length = src[4] | (src[5] << 8) | (src[6] << 16) | (src[7] << 24);

    src += 8;
    srccounter = length;
    endptr = &dst[length];
    backup[0] = endptr[0];
    backup[1] = endptr[1];
    dstcounter = length + 1;

    while (1)
    {
        multivalue = *src++;

        for (mask = 0x80; mask != 0; mask >>= 1)
        {
            if (multivalue & mask)
            {
                copyvalue = *src++;
                copyoffset = ((copyvalue & 0xf0) << 4) | (*src++);
                copycounter = (copyvalue & 0x0f) + 3;
                copyptr = &dst[-copyoffset];

                for (; copycounter != 0; copycounter--)
                {
                    *dst++ = *copyptr++;

                    dstcounter--;
                    if (dstcounter == 0)
                    {
                        endptr[0] = backup[0];
                        endptr[1] = backup[1];

                        return length;
                    }

                    srccounter--;
                    if (srccounter == 0)
                    {
                        return length;
                    }
                }
            }
            else
            {
                *dst++ = *src++;

                dstcounter--;
                if (dstcounter == 0)
                {
                    endptr[0] = backup[0];
                    endptr[1] = backup[1];

                    return length;
                }

                srccounter--;
                if (srccounter == 0)
                {
                    return length;
                }
            }
        }
    }
}

