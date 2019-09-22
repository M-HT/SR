/**
 *
 *  Copyright (C) 2019 Roman Pauer
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
#include "udis86_dep.h"

int SR_disassemble_region_llasm(unsigned int Entry, region_data *region)
{
    output_data *output;
    uint_fast32_t cur_ofs, flags_to_write, flags_write, flags_read;
    int finished, orig_ebp_dword_aligned, orig_esp_dword_aligned, last_instruction;
    unsigned int ret;
    char *newstr;
    bound_data *iflags;
    int *ua_ebp_area_value, *ua_esp_area_value;
    Word_t ua_ebp_area_index, ua_esp_area_index;
    // todo: arm

    cur_ofs = region->end_ofs;
    finished = 0;
    flags_to_write = 0;
    last_instruction = 2;
    //flags_to_write = region->flags_to_write;

    orig_ebp_dword_aligned = ebp_dword_aligned;
    orig_esp_dword_aligned = esp_dword_aligned;

    if (orig_ebp_dword_aligned)
    {
        ua_ebp_area_value = section_ua_ebp_list_FindEntryEqualOrLowerIndex(Entry, cur_ofs, &ua_ebp_area_index);
        if (ua_ebp_area_value != NULL)
        {
            if (ua_ebp_area_index + *ua_ebp_area_value <= region->begin_ofs)
            {
                ua_ebp_area_value = NULL;
            }
        }

    }
    else
    {
        ua_ebp_area_value = NULL;
    }

    if (orig_esp_dword_aligned)
    {
        ua_esp_area_value = section_ua_esp_list_FindEntryEqualOrLowerIndex(Entry, cur_ofs, &ua_esp_area_index);
        if (ua_esp_area_value != NULL)
        {
            if (ua_esp_area_index + *ua_esp_area_value <= region->begin_ofs)
            {
                ua_esp_area_value = NULL;
            }
        }
    }
    else
    {
        ua_esp_area_value = NULL;
    }

    output = section_output_list_FindEntryEqual(Entry, cur_ofs);
    while (!finished)
    {
        if (output == NULL)
        {
            fprintf(stderr, "Error: output not found - %i - %i (0x%x)\n", Entry, cur_ofs, section[Entry].start + cur_ofs);

            return 1;
        }

        cur_ofs = output->ofs;

        if (cur_ofs < region->begin_ofs)
        {
            fprintf(stderr, "Error: output out of bounds - %i - %i (0x%x)\n", Entry, cur_ofs, section[Entry].start + cur_ofs);

            return 2;
        }

        if (output->type == OT_NONE) break;

        if (output->str[0] == ';' && strchr(output->str, '\n') == NULL)
        {
            // instruction is commented out
            newstr = (char *) malloc(strlen(output->str) + 2);
            if (newstr == NULL)
            {
                fprintf(stderr, "Error: not enough memory - %i - %i (0x%x)\n", Entry, cur_ofs, section[Entry].start + cur_ofs);

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
                fprintf(stderr, "Error: wrong disassembly - %i - %i (0x%x) - %i\n", Entry, cur_ofs, section[Entry].start + cur_ofs, ret);

                finished = 3;
                break;
            }

            flags_read = flags_needed[ud_obj.mnemonic];
            flags_write = flags_modified[ud_obj.mnemonic];
            if (flags_read == FL_UNKNOWN ||
                flags_write == FL_UNKNOWN)
            {
                fprintf(stderr, "Error: unknown flags - %i - %i (0x%x) - %s\n", Entry, cur_ofs, section[Entry].start + cur_ofs, output->str);

                return 5;
            }

            iflags = section_iflags_list_FindEntryEqual(Entry, cur_ofs);
            if (iflags)
            {
                flags_to_write |= (uint32_t) iflags->begin & ~(FL_CARRY_SUB_ORIGINAL | FL_CARRY_SUB_INVERTED);
            }


            if (orig_ebp_dword_aligned)
            {
                if (ua_ebp_area_value != NULL)
                {
                    if (ua_ebp_area_index > cur_ofs)
                    {
                        ua_ebp_area_value = section_ua_ebp_list_FindEntryEqualOrLowerIndex(Entry, cur_ofs, &ua_ebp_area_index);
                    }

                    if (ua_ebp_area_value != NULL)
                    {
                        if (ua_ebp_area_index + *ua_ebp_area_value > cur_ofs)
                        {
                            ebp_dword_aligned = 0;
                        }
                    }
                }
            }

            if (orig_esp_dword_aligned)
            {
                if (ua_esp_area_value != NULL)
                {
                    if (ua_esp_area_index > cur_ofs)
                    {
                        ua_esp_area_value = section_ua_esp_list_FindEntryEqualOrLowerIndex(Entry, cur_ofs, &ua_esp_area_index);
                    }

                    if (ua_esp_area_value != NULL)
                    {
                        if (ua_esp_area_index + *ua_esp_area_value > cur_ofs)
                        {
                            esp_dword_aligned = 0;
                        }
                    }
                }
            }

            ret = SR_disassemble_llasm_instruction(Entry, output, flags_to_write, &flags_write, &flags_read, &last_instruction);

            if (orig_ebp_dword_aligned)
            {
                if (ua_ebp_area_value != NULL)
                {
                    ebp_dword_aligned = 1;
                }
            }

            if (orig_esp_dword_aligned)
            {
                if (ua_esp_area_value != NULL)
                {
                    esp_dword_aligned = 1;
                }
            }


            if (ret) return ret;

            if (flags_read == FL_SPECIFIC ||
                flags_write == FL_SPECIFIC)
            {
                fprintf(stderr, "Error: unknown specific flags - %i - %i (0x%x) - %s\n", Entry, cur_ofs, section[Entry].start + cur_ofs, output->str);

                return 5;
            }

            if ((last_instruction) && (cur_ofs != region->end_ofs))
            {
                fprintf(stderr, "Error: instruction ends in the middle of region - %i - %i (0x%x) - %s\n", Entry, cur_ofs, section[Entry].start + cur_ofs, output->str);

                return 5;
            }

            last_instruction = 0;

            flags_to_write &= ~flags_write;
            if (flags_to_write == FL_WEAK) flags_to_write = FL_NONE;
            flags_to_write |= flags_read;

            if (iflags)
            {
                flags_to_write &= ~((uint32_t) iflags->end | (FL_CARRY_SUB_ORIGINAL | FL_CARRY_SUB_INVERTED));
            }

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

    if (flags_to_write)
    {
        if (flags_to_write & FL_WEAK)
        {
            fprintf(stderr, "Warning: unknown weak flags on start of region - %i - %i (0x%x) - 0x%x\n", Entry, cur_ofs, section[Entry].start + cur_ofs, flags_to_write);
        }
        else
        {
            // this shouldn't be needed
            // only works if regions are processed from end to begin
            /*if (region->begin_ofs != 0)
            {
                output = section_output_list_FindEntryEqual(Entry, region->begin_ofs);
                if (output != NULL)
                {
                    if (!output->has_label)
                    {
                        prev_region = section_region_list_FindEntryLower(Entry, region->begin_ofs);
                        if (prev_region != NULL)
                        {
                            output = section_output_list_FindEntryEqual(Entry, prev_region->end_ofs);
                            if (output != NULL)
                            {
                                if (prev_region->end_ofs + output->len + 1 == region->begin_ofs)
                                {
                                    prev_region->flags_to_write |= flags_to_write;
                                    flags_to_write = 0;
                                }
                            }
                        }
                    }
                }
            }*/

            if (flags_to_write)
            {
                fprintf(stderr, "Error: unknown flags on start of region - %i - %i (0x%x) - 0x%x\n", Entry, cur_ofs, section[Entry].start + cur_ofs, flags_to_write);

                return 6;
            }
        }
    }


    return 0;
}

