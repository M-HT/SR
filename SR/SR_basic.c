/**
 *
 *  Copyright (C) 2016-2019 Roman Pauer
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

typedef struct _Entry_FILE_ {
    uint_fast32_t Entry;
    FILE *fout;
} Entry_FILE;

int SR_initial_disassembly(void)
{
    uint_fast32_t Entry, ofs;
    output_data *output;

    for (Entry = 0; Entry < num_sections; Entry++)
    {
        for (ofs = 0; ofs < section[Entry].size; ofs++)
        {
            output = section_output_list_Insert(Entry, ofs);
            if ( output == NULL )
            {
                return -1;
            }
            else
            {
                if ( (section[Entry].type == ST_UDATA)
#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_DOS)
                  || (ofs >= section[Entry].size_in_file)
#endif
                )
                {
                    output->type = OT_UNITIALIZED;
                }
            }
        }

        output = section_output_list_Insert(Entry, section[Entry].size);
        if ( output == NULL )
        {
            return -1;
        }
        else
        {
            output->type = OT_NONE;
            output->has_label = 1;
            output->len = (uint_fast32_t) -1;
        }
    }

    return 0;
}

void SR_get_label(char *cbuf, uint_fast32_t Address)
{
    sprintf(cbuf, "loc_%X", (unsigned int) Address);
}

#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_DOS)
static void SR_apply_fixup_alias_init(alias_data *item, void *data)
{
    output_data *output;
    bound_data *bound;

#define DATA ((uint_fast32_t) data)

    output = section_output_list_FindEntryEqual(DATA, item->ofs);
    if (output != NULL)
    {
        output->has_label = 1;
    }

    if (section[DATA].type == ST_CODE)
    {
        section_entry_list_Insert(DATA, item->ofs);
        bound = section_bound_list_Insert(DATA, item->ofs);
        if (bound != NULL) bound->begin = 1;
    }

#undef DATA
}
#endif

static void SR_apply_fixup_data_init(fixup_data *item, void *data)
{
    output_data *output;

#define DATA ((uint_fast32_t) data)

    if (item->type == FT_NORMAL || item->type == FT_SELFREL)
    {
        section_output_list_Delete(DATA, item->sofs + 3);
        section_output_list_Delete(DATA, item->sofs + 2);
    }
    section_output_list_Delete(DATA, item->sofs + 1);

    output = section_output_list_FindEntryEqual(DATA, item->sofs);
    if (output != NULL)
    {
        output->len = (item->type == FT_NORMAL || item->type == FT_SELFREL)?3:1;
    }
#undef DATA
}

static void SR_apply_fixup_data_offset(fixup_data *item, void *data)
{
    char cbuf[128];
    char cbuf2[64];
    output_data *output;
    fixup_data *fixup;
    uint_fast32_t sec, ofs;
    int *label_value, *code16_area_value;
    Word_t code16_area_index;
    extrn_data *extrn;
    bound_data *bound;
    replace_data *replace;
    int validenttry;

#define DATA ((Entry_FILE *) data)

#if (OUTPUT_TYPE != OUT_ORIG)
    // update address in memory
    if ((item->type == FT_NORMAL) &&
        section[DATA->Entry].type == ST_CODE
       )
    {
        *((uint32_t *) &(section[DATA->Entry].adr[item->sofs])) = section[item->tsec].start + item->tofs;
    }

    replace = section_replace_list_FindEntryEqualOrLower(DATA->Entry, item->sofs);
    if ((replace != NULL) && (replace->empty) && (replace->ofs + replace->length >= item->sofs + 4)) // source is in empty replaced area
    {
        return;
    }
#endif


    if (item->type != FT_SEGMENT)
    {
#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_DOS)
        extrn = NULL;
#else
        if (item->type == FT_16BITOFS)
        {
            return;
        }

        extrn = section_extrn_list_FindEntryEqual(item->tsec, item->tofs);
#endif

        label_value = section_label_list_FindEntryEqual(item->tsec, item->tofs);

        sec = item->tsec;
        if (label_value != NULL)
        {
            if (SR_get_section_reladr(section[item->tsec].start + item->tofs + *label_value, &sec, &ofs))
            {
                output = section_output_list_FindEntryEqualOrLower(sec, ofs);
            }
            else output = NULL;
        }
        else
        {
            if ( item->tofs < 0 )
            {
                output = section_output_list_FindEntryFirst(item->tsec);
            }
            else
            {
                output = section_output_list_FindEntryEqualOrLower(item->tsec, item->tofs);
            }
        }

        // set label to target address
        if (output != NULL)
        {
            output->has_label = 1;
            ofs = output->ofs;
        }
        else
        {
            sec = item->tsec;
            ofs = item->tofs;
        }
    }
    else
    {
#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_DOS)
        extrn = NULL;
        sec = item->tsec;
        ofs = item->tofs;
#else
        return;
#endif
    }

    // if target is not external procedure and
    // target section is code section and ...
    if (item->type != FT_SEGMENT &&
        extrn == NULL &&
        section[item->tsec].type == ST_CODE ) // code section
    {
        // ... and target is not another fixup
        fixup = section_fixup_list_FindEntryEqual(item->tsec, item->tofs);
        if ( fixup == NULL )
        {
            // check if fixup target is in 16-bit code area
            code16_area_value = section_code16_list_FindEntryEqualOrLowerIndex(item->tsec, item->tofs, &code16_area_index);
            if (code16_area_value != NULL)
            {
                if (code16_area_index + *code16_area_value <= item->tofs)
                {
                    code16_area_value = NULL;
                }
            }

            if (code16_area_value != NULL)
            {
                validenttry = 0;
            }
            else if ( section[DATA->Entry].type == ST_CODE ) // code section
            {
                // fixups from code section to code section are valid only if originating from jump table,
                // or from inter-section function calls, interrupts, dynamic function calls, callback functions
                validenttry = (item->type == FT_SELFREL)?1:0;

                if (validenttry == 0)
                {
                    if (section_code_list_CanFindEntryEqual(item->tsec, item->tofs))
                    {
                        validenttry = 1;
                    }
                }

                if (validenttry == 0 && item->sofs != (uint_fast32_t)((int_fast32_t)(-1)))
                {
                    fixup = section_fixup_list_FindEntryHigher(DATA->Entry, item->sofs);
                    if (fixup != NULL)
                    {
                        if (fixup->sofs == item->sofs + 4)
                        {
                            validenttry = 1;
                        }
                    }
                }

                if (validenttry == 0 && item->sofs != 0)
                {
                    fixup = section_fixup_list_FindEntryLower(DATA->Entry, item->sofs);
                    if (fixup != NULL)
                    {
                        if (fixup->sofs + 4 == item->sofs)
                        {
                            validenttry = 1;
                        }
                    }
                }

                if (list_invalid_code_fixups)
                {
                    if (validenttry == 0)
                    {
                        SR_get_label(cbuf, section[item->tsec].start + item->tofs);
                        fprintf(DATA->fout, "%s\n", cbuf);
                    }
                }
            }
            else // data and stack section
            {
                // each fixup from data section to code section is valid
                validenttry = 1;

                if (list_data_to_code_fixups)
                {
                    SR_get_label(cbuf, section[item->tsec].start + item->tofs);
                    fprintf(DATA->fout, "%s\n", cbuf);
                }
            }

            if (validenttry)
            {
                if (section_nocode_list_CanFindEntryEqual(item->tsec, item->tofs))
                {
                    validenttry = 0;
                }
            }

            if (validenttry)
            {
                replace = section_replace_list_FindEntryEqualOrLower(item->tsec, item->tofs);
                if ((replace != NULL) && (replace->ofs + replace->length > item->tofs)) // target is in replaced area
                {
                    replace = section_replace_list_FindEntryEqualOrLower(DATA->Entry, item->sofs);
                    if ((replace != NULL) && (replace->ofs + replace->length > item->sofs)) // source is in replaced area
                    {
                        validenttry = 0;
                    }
                }
            }

            if (validenttry)
            {
                section_entry_list_Insert(item->tsec, item->tofs);
                bound = section_bound_list_Insert(item->tsec, item->tofs);
                if (bound != NULL) bound->begin = 1;
            }
        }
    }

    if (item->type == FT_SEGMENT)
    {
        sprintf(cbuf, "%s", section[item->tsec].name);
    }
    else if (extrn != NULL)
    {
        strcpy(cbuf, extrn->proc);
    }
    else
    {
        SR_get_label(cbuf, section[sec].start + ofs);
    }

    output = section_output_list_FindEntryEqual(DATA->Entry, item->sofs);
    if (output != NULL)
    {
        if (output->str != NULL)
        {
            free(output->str);
            output->str = NULL;
        }

        if (((sec == item->tsec) && (ofs == item->tofs)) ||
            extrn != NULL)
        {
            sprintf(cbuf2, " %s", cbuf);
        }
        else
        {
            sprintf(cbuf2, " (%s + (%i))", cbuf, (int)((section[item->tsec].start + item->tofs) - (section[sec].start + ofs)));
        }

        if (item->type == FT_SELFREL)
        {
            replace = section_replace_list_FindEntryEqualOrLower(DATA->Entry, item->sofs);
            if ((replace == NULL) || (replace->ofs + replace->length < item->sofs)) // source is not in replaced area
            {
                output->str = strdup(cbuf2);
                output->type = OT_OFFSET;

                // insert instruction address after call instruction into list of entries
                section_entry_list_Insert(DATA->Entry, item->sofs + 4);
                bound = section_bound_list_Insert(DATA->Entry, item->sofs + 4);
                if (bound != NULL) bound->begin = 1;

                output = section_output_list_FindEntryEqual(DATA->Entry, item->sofs - 1);
                if (output != NULL)
                {
                    if (output->str != NULL)
                    {
                        free(output->str);
                        output->str = NULL;
                    }

                    if ( section[DATA->Entry].adr[item->sofs - 1] == 0xe8 )
                    {
#if ((OUTPUT_TYPE == OUT_ARM_LINUX) || (OUTPUT_TYPE == OUT_LLASM))
                        // todo: arm
                        sprintf(cbuf, "@call%s\ncall%s", cbuf2, cbuf2);
#else
                        sprintf(cbuf, "call%s", cbuf2);
#endif

                        output->str = strdup(cbuf);


                        bound = section_bound_list_Insert(DATA->Entry, item->sofs - 1);
                        if (bound != NULL) bound->end = 1;
                    }
                    else
                    {
                        output->str = strdup("chyba");
                    }
                    output->type = OT_INSTRUCTION;
                    output->len = 4;

                    section_output_list_Delete(DATA->Entry, item->sofs);
                }
            }

        }
        else if (item->type == FT_SEGMENT || item->type == FT_16BITOFS)
        {
#if (OUTPUT_TYPE == OUT_LLASM)
            sprintf(cbuf, "dseg%s", cbuf2);
#elif (OUTPUT_TYPE == OUT_ARM_LINUX)
            sprintf(cbuf, ".hword%s", cbuf2);
#else
            sprintf(cbuf, "dw%s", cbuf2);
#endif

            output->str = strdup(cbuf);
            output->type = OT_OFFSET;
        }
        else
        {
#if (OUTPUT_TYPE == OUT_LLASM)
            sprintf(cbuf, "daddr%s", cbuf2);
#elif (OUTPUT_TYPE == OUT_ARM_LINUX)
            sprintf(cbuf, ".int%s", cbuf2);
#else
            sprintf(cbuf, "dd%s", cbuf2);
#endif

            output->str = strdup(cbuf);
            output->type = OT_OFFSET;
        }

    }

#undef DATA
}

int SR_apply_fixup_info(void)
{
    Entry_FILE EF;
    int fout_opened;

#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_DOS)
    output_data *output;

    output = section_output_list_FindEntryEqual(EIPObjectNum, EIP);
    if (output != NULL)
    {
        output->has_label = 1;
    }

    // insert program entry point into list of entries
    section_entry_list_Insert(EIPObjectNum, EIP);

#else

    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        section_alias_list_ForEach(EF.Entry, &SR_apply_fixup_alias_init, (void *) EF.Entry);
    }
#endif

    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        section_fixup_list_ForEach(EF.Entry, &SR_apply_fixup_data_init, (void *) EF.Entry);
    }

    if (list_invalid_code_fixups)
    {
        EF.fout = fopen(invalid_code_fixups_name, "wt");
        if (EF.fout != NULL)
        {
            fout_opened = 1;
        }
        else
        {
            fout_opened = 0;
            list_invalid_code_fixups = 0;
        }
    }
    else if (list_data_to_code_fixups)
    {
        EF.fout = fopen(data_to_code_fixups_name, "wt");
        if (EF.fout != NULL)
        {
            fout_opened = 1;
        }
        else
        {
            fout_opened = -2;
            list_data_to_code_fixups = 0;
        }
    }
    else
    {
        fout_opened = -1;
    }

    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        section_fixup_list_ForEach(EF.Entry, &SR_apply_fixup_data_offset, (void *) &EF);
    }

    if (fout_opened == 0)
    {
        list_invalid_code_fixups = 1;
    }
    else if (fout_opened == -2)
    {
        list_data_to_code_fixups = 1;
    }
    else if (fout_opened == 1)
    {
        fclose(EF.fout);
    }


    return 0;
}

