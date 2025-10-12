/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
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

#if defined(USE_JUDY)

#include <string.h>
#include "SR_defs.h"
#include "SR_vars.h"

/* _chk_list */
void section_chk_list_ForEach(unsigned int SecNum, void (*proc)(Word_t Index, void *data), void *data)
{
    int Rc_int;
    Word_t search_index;

    search_index = 0;
    J1F(Rc_int, section[SecNum]._chk_list, search_index);
    while (Rc_int)
    {
        proc(search_index, data);

        J1N(Rc_int, section[SecNum]._chk_list, search_index);
    }
}


/* extrn_list */
extrn_data *section_extrn_list_Insert(unsigned int SecNum, Word_t Index, const char *proc, const char *altaction)
{
    extrn_data *extrn, **extrn_value;

    extrn = (extrn_data *)malloc(sizeof(extrn_data) + strlen(proc));
    if (extrn == NULL) return NULL;

    extrn->ofs = (uint_fast32_t)Index;
    strcpy(extrn->proc, proc);

    JLI(extrn_value, section[SecNum].extrn_list, Index);
    if ((extrn_value == PJERR) || (extrn_value == NULL))
    {
        free(extrn);
        return NULL;
    }

    if (*extrn_value != NULL)
    {
        free(*extrn_value);
    }

    extrn->altaction = (altaction)?strdup(altaction):NULL;

    *extrn_value = extrn;

    return extrn;
}

void section_extrn_list_ForEach(unsigned int SecNum, void (*proc)(extrn_data *extrn, void *data), void *data)
{
    extrn_data **extrn_value;
    Word_t search_index;

    search_index = 0;
    JLF(extrn_value, section[SecNum].extrn_list, search_index);
    while (extrn_value != NULL)
    {
        proc(*extrn_value, data);

        JLN(extrn_value, section[SecNum].extrn_list, search_index);
    }
}

static void section_extrn_list_FreeCallback(extrn_data *extrn, void *data)
{
    if (extrn->altaction != NULL)
    {
        free(extrn->altaction);
    }
    free(extrn);
}

void section_extrn_list_Free(unsigned int SecNum)
{
    Word_t Rc_word;

    section_extrn_list_ForEach(SecNum, section_extrn_list_FreeCallback, NULL);

    JLFA(Rc_word, section[SecNum].extrn_list);
}

/* alias_list */
alias_data *section_alias_list_Insert(unsigned int SecNum, Word_t Index, const char *proc)
{
    alias_data *alias, **alias_value;

    alias = (alias_data *)malloc(sizeof(alias_data) + strlen(proc));
    if (alias == NULL) return NULL;

    alias->ofs = (uint_fast32_t)Index;
    strcpy(alias->proc, proc);

    JLI(alias_value, section[SecNum].alias_list, Index);
    if ((alias_value == PJERR) || (alias_value == NULL))
    {
        free(alias);
        return NULL;
    }

    if (*alias_value != NULL)
    {
        free(*alias_value);
    }

    *alias_value = alias;

    return alias;
}

void section_alias_list_ForEach(unsigned int SecNum, void (*proc)(alias_data *alias, void *data), void *data)
{
    alias_data **alias_value;
    Word_t search_index;

    search_index = 0;
    JLF(alias_value, section[SecNum].alias_list, search_index);
    while (alias_value != NULL)
    {
        proc(*alias_value, data);

        JLN(alias_value, section[SecNum].alias_list, search_index);
    }
}

static void section_alias_list_FreeCallback(alias_data *alias, void *data)
{
    free(alias);
}

void section_alias_list_Free(unsigned int SecNum)
{
    Word_t Rc_word;

    section_alias_list_ForEach(SecNum, section_alias_list_FreeCallback, NULL);

    JLFA(Rc_word, section[SecNum].alias_list);
}


/* replace_list */
replace_data *section_replace_list_Insert(unsigned int SecNum, Word_t Index, const char *instr, unsigned int instr_len, int instr_empty)
{
    replace_data *replace, **replace_value;

    replace = (replace_data *)malloc(sizeof(replace_data) + strlen(instr));
    if (replace == NULL) return NULL;

    replace->ofs = (uint_fast32_t)Index;
    replace->length = instr_len;
    replace->empty = instr_empty;
    strcpy(replace->instr, instr);

    JLI(replace_value, section[SecNum].replace_list, Index);
    if ((replace_value == PJERR) || (replace_value == NULL))
    {
        free(replace);
        return NULL;
    }

    if (*replace_value != NULL)
    {
        free(*replace_value);
    }

    *replace_value = replace;

    return replace;
}

void section_replace_list_ForEach(unsigned int SecNum, void (*proc)(replace_data *replace, void *data), void *data)
{
    replace_data **replace_value;
    Word_t search_index;

    search_index = 0;
    JLF(replace_value, section[SecNum].replace_list, search_index);
    while (replace_value != NULL)
    {
        proc(*replace_value, data);

        JLN(replace_value, section[SecNum].replace_list, search_index);
    }
}

static void section_replace_list_FreeCallback(replace_data *replace, void *data)
{
    free(replace);
}

void section_replace_list_Free(unsigned int SecNum)
{
    Word_t Rc_word;

    section_replace_list_ForEach(SecNum, section_replace_list_FreeCallback, NULL);

    JLFA(Rc_word, section[SecNum].replace_list);
}


/* iflags_list */
bound_data *section_iflags_list_Insert(unsigned int SecNum, Word_t Index, int begin, int end)
{
    bound_data *iflags, **iflags_value;

    JLI(iflags_value, section[SecNum].iflags_list, Index);
    if ((iflags_value == PJERR) || (iflags_value == NULL))
    {
        return NULL;
    }

    iflags = *iflags_value;
    if (iflags == NULL)
    {
        iflags = (bound_data *)malloc(sizeof(bound_data));
        if (iflags == NULL) return NULL;

        *iflags_value = iflags;
    }

    iflags->ofs = (uint_fast32_t)Index;
    iflags->begin = begin;
    iflags->end = end;

    return iflags;
}

void section_iflags_list_Free(unsigned int SecNum)
{
    Word_t index, Rc_word;
    bound_data **iflags_value;

    index = 0;
    JLF(iflags_value, section[SecNum].iflags_list, index);
    while (iflags_value != NULL)
    {
        if (*iflags_value != NULL)
        {
            free(*iflags_value);
        }
        JLN(iflags_value, section[SecNum].iflags_list, index);
    }

    JLFA(Rc_word, section[SecNum].iflags_list);
}


/* bound_list */
bound_data *section_bound_list_Insert(unsigned int SecNum, Word_t Index)
{
    bound_data *bound, **bound_value;

    JLI(bound_value, section[SecNum].bound_list, Index);
    if ((bound_value == PJERR) || (bound_value == NULL))
    {
        return NULL;
    }

    bound = *bound_value;
    if (bound == NULL)
    {
        bound = (bound_data *)malloc(sizeof(bound_data));
        if (bound == NULL) return NULL;

        *bound_value = bound;

        bound->begin = 0;
        bound->end = 0;
    }

    bound->ofs = (uint_fast32_t)Index;

    return bound;
}

void section_bound_list_ForEach(unsigned int SecNum, void (*proc)(bound_data *bound, void *data), void *data)
{
    bound_data **bound_value;
    Word_t search_index;

    search_index = 0;
    JLF(bound_value, section[SecNum].bound_list, search_index);
    while (bound_value != NULL)
    {
        proc(*bound_value, data);

        JLN(bound_value, section[SecNum].bound_list, search_index);
    }
}

void section_bound_list_Delete(unsigned int SecNum, Word_t Index)
{
    bound_data *bound, **bound_value;
    int Rc_int;

    JLG(bound_value, section[SecNum].bound_list, Index);

    if (bound_value != NULL)
    {
        bound = *bound_value;
        if (bound != NULL)
        {
            free(bound);
        }

        JLD(Rc_int, section[SecNum].bound_list, Index);
    }
}

static void section_bound_list_FreeCallback(bound_data *bound, void *data)
{
    if (bound != NULL)
    {
        free(bound);
    }
}

void section_bound_list_Free(unsigned int SecNum)
{
    Word_t Rc_word;

    section_bound_list_ForEach(SecNum, section_bound_list_FreeCallback, NULL);

    JLFA(Rc_word, section[SecNum].bound_list);
}


/* fixup_list */
fixup_data *section_fixup_list_Insert(unsigned int SecNum, Word_t Index)
{
    fixup_data *fixup, **fixup_value;

    JLI(fixup_value, section[SecNum].fixup_list, Index);
    if ((fixup_value == PJERR) || (fixup_value == NULL))
    {
        return NULL;
    }

    fixup = *fixup_value;
    if (fixup == NULL)
    {
        fixup = (fixup_data *)malloc(sizeof(fixup_data));
        if (fixup == NULL) return NULL;

        *fixup_value = fixup;

        fixup->tofs = 0;
        fixup->tsec = 0;
        fixup->type = FT_NORMAL;
    }

    fixup->sofs = (uint_fast32_t)Index;

    return fixup;
}

void section_fixup_list_ForEach(unsigned int SecNum, void (*proc)(fixup_data *fixup, void *data), void *data)
{
    fixup_data **fixup_value;
    Word_t search_index;

    search_index = 0;
    JLF(fixup_value, section[SecNum].fixup_list, search_index);
    while (fixup_value != NULL)
    {
        proc(*fixup_value, data);

        JLN(fixup_value, section[SecNum].fixup_list, search_index);
    }
}

static void section_fixup_list_FreeCallback(fixup_data *fixup, void *data)
{
    if (fixup != NULL)
    {
        free(fixup);
    }
}

void section_fixup_list_Free(unsigned int SecNum)
{
    Word_t Rc_word;

    section_fixup_list_ForEach(SecNum, section_fixup_list_FreeCallback, NULL);

    JLFA(Rc_word, section[SecNum].fixup_list);
}


/* output_list */
output_data *section_output_list_Insert(unsigned int SecNum, Word_t Index)
{
    output_data *output, **output_value;

    JLI(output_value, section[SecNum].output_list, Index);
    if ((output_value == PJERR) || (output_value == NULL))
    {
        return NULL;
    }

    output = *output_value;
    if (output == NULL)
    {
        output = (output_data *)malloc(sizeof(output_data));
        if (output == NULL) return NULL;

        *output_value = output;

        output->len = 0;
        output->str = NULL;
        output->has_label = 0;
        output->align = 0;
        output->type = OT_UNKNOWN;
    }

    output->ofs = (uint_fast32_t)Index;

    return output;
}

void section_output_list_ForEach(unsigned int SecNum, void (*proc)(output_data *output, void *data), void *data)
{
    output_data **output_value;
    Word_t search_index;

    search_index = 0;
    JLF(output_value, section[SecNum].output_list, search_index);
    while (output_value != NULL)
    {
        proc(*output_value, data);

        JLN(output_value, section[SecNum].output_list, search_index);
    }
}

void section_output_list_Delete(unsigned int SecNum, Word_t Index)
{
    output_data *output, **output_value;
    int Rc_int;

    JLG(output_value, section[SecNum].output_list, Index);

    if (output_value != NULL)
    {
        output = *output_value;
        if (output != NULL)
        {
            if (output->str != NULL)
            {
                free(output->str);
            }
            free(output);
        }

        JLD(Rc_int, section[SecNum].output_list, Index);
    }
}

static void section_output_list_FreeCallback(output_data *output, void *data)
{
    if (output != NULL)
    {
        if (output->str != NULL)
        {
            free(output->str);
        }
        free(output);
    }
}

void section_output_list_Free(unsigned int SecNum)
{
    Word_t Rc_word;

    section_output_list_ForEach(SecNum, section_output_list_FreeCallback, NULL);

    JLFA(Rc_word, section[SecNum].output_list);
}


/* region_list */
region_data *section_region_list_Insert(unsigned int SecNum, Word_t Index, uint_fast32_t end_ofs)
{
    region_data *region, **region_value;

    region = (region_data *)malloc(sizeof(region_data));
    if (region == NULL) return NULL;

    region->begin_ofs = (uint_fast32_t)Index;
    region->end_ofs = end_ofs;

    JLI(region_value, section[SecNum].region_list, Index);
    if ((region_value == PJERR) || (region_value == NULL))
    {
        free(region);
        return NULL;
    }

    if (*region_value != NULL)
    {
        free(*region_value);
    }

    *region_value = region;

    return region;
}

void section_region_list_Free(unsigned int SecNum)
{
    Word_t index, Rc_word;
    region_data **region_value;

    index = 0;
    JLF(region_value, section[SecNum].region_list, index);
    while (region_value != NULL)
    {
        free(*region_value);
        JLN(region_value, section[SecNum].region_list, index);
    }

    JLFA(Rc_word, section[SecNum].region_list);
}

#endif

