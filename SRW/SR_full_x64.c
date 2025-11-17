/**
 *
 *  Copyright (C) 2019-2025 Roman Pauer
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

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "SR_defs.h"
#include "SR_vars.h"

int SR_disassemble_region_x64(unsigned int Entry, region_data *region)
{
    output_data *output;
    uint_fast32_t cur_ofs;
    int finished;
    unsigned int ret;
    char *newstr;

    cur_ofs = region->end_ofs;
    finished = 0;

    output = section_output_list_FindEntryEqual(Entry, cur_ofs);
    while (!finished)
    {
        if (output == NULL)
        {
            fprintf(stderr, "Error: output not found - %i - %i (0x%x)\n", Entry, (unsigned int)cur_ofs, (unsigned int)(section[Entry].start + cur_ofs));

            return 1;
        }

        cur_ofs = output->ofs;

        if (cur_ofs < region->begin_ofs)
        {
            fprintf(stderr, "Error: output out of bounds - %i - %i (0x%x)\n", Entry, (unsigned int)cur_ofs, (unsigned int)(section[Entry].start + cur_ofs));

            return 2;
        }

        if (output->type == OT_NONE) break;

        if (output->str[0] == ';' && strchr(output->str, '\n') == NULL)
        {
            // instruction is commented out
            newstr = (char *) malloc(strlen(output->str) + 2);
            if (newstr == NULL)
            {
                fprintf(stderr, "Error: not enough memory - %i - %i (0x%x)\n", Entry, (unsigned int)cur_ofs, (unsigned int)(section[Entry].start + cur_ofs));

                return 3;
            }

            newstr[0] = ';';
            strcpy(&(newstr[1]), output->str);
            free(output->str);
            output->str = newstr;
        }
        else
        {
            ud_set_input_buffer(&ud_obj, &(section[Entry].adr[cur_ofs]), output->len + 1);
            ud_set_pc(&ud_obj, section[Entry].start + cur_ofs);

            ret = ud_disassemble(&ud_obj);


            if (ret == 0 || ret != output->len + 1)
            {
                fprintf(stderr, "Error: wrong disassembly - %i - %i (0x%x) - %i\n", Entry, (unsigned int)cur_ofs, (unsigned int)(section[Entry].start + cur_ofs), ret);

                finished = 3;
                break;
            }

            ret = SR_disassemble_x64_instruction(Entry, output, region);


            if (ret) return ret;

        }


        if (cur_ofs == region->begin_ofs)
        {
            finished = 1;
        }
        else
        {
            cur_ofs--;
            output = section_output_list_FindEntryEqualOrLower(Entry, cur_ofs);
        }
    }


    return 0;
}

