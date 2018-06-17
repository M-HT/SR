/**
 *
 *  Copyright (C) 2016-2018 Roman Pauer
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

#if !defined(_JUDY_HELPERS_H_INCLUDED_)
#define _JUDY_HELPERS_H_INCLUDED_

/* code_list */
static inline void section_code_list_Insert(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1S(Rc_int, section[SecNum].code_list, Index);
}

static inline int section_code_list_CanFindEntryEqual(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1T(Rc_int, section[SecNum].code_list, Index);

    return Rc_int;
}


/* noret_list */
static inline void section_noret_list_Insert(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1S(Rc_int, section[SecNum].noret_list, Index);
}

static inline int section_noret_list_CanFindEntryEqual(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1T(Rc_int, section[SecNum].noret_list, Index);

    return Rc_int;
}


/* nocode_list */
static inline void section_nocode_list_Insert(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1S(Rc_int, section[SecNum].nocode_list, Index);
}

static inline int section_nocode_list_CanFindEntryEqual(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1T(Rc_int, section[SecNum].nocode_list, Index);

    return Rc_int;
}


/* entry_list */
static inline void section_entry_list_Insert(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1S(Rc_int, section[SecNum].entry_list, Index);
}

static inline int section_entry_list_FindFirstIndex(unsigned int SecNum, Word_t *FoundIndex)
{
    int Rc_int;
    Word_t search_index;

    search_index = 0;
    J1F(Rc_int, section[SecNum].entry_list, search_index);

    *FoundIndex = search_index;

    return Rc_int;
}

static inline void section_entry_list_Delete(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1U(Rc_int, section[SecNum].entry_list, Index);
}


/* _chk_list */
static inline void section_chk_list_Insert(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1S(Rc_int, section[SecNum]._chk_list, Index);
}

void section_chk_list_ForEach(unsigned int SecNum, void (*proc)(Word_t Index, void *data), void *data);


/* label_list */
static inline int *section_label_list_Insert(unsigned int SecNum, Word_t Index)
{
    int *ValuePtr;

    JLI(ValuePtr, section[SecNum].label_list, Index);

    return (ValuePtr == PJERR)?NULL:ValuePtr;
}

static inline int *section_label_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    int *ValuePtr;

    JLG(ValuePtr, section[SecNum].label_list, Index);

    return ValuePtr;
}


/* code16_list */
static inline int *section_code16_list_Insert(unsigned int SecNum, Word_t Index)
{
    int *ValuePtr;

    JLI(ValuePtr, section[SecNum].code16_list, Index);

    return (ValuePtr == PJERR)?NULL:ValuePtr;
}

static inline int *section_code16_list_FindEntryEqualOrLowerIndex(unsigned int SecNum, Word_t Index, Word_t *FoundIndex)
{
    int *ValuePtr;
    Word_t search_index;

    search_index = Index;
    JLL(ValuePtr, section[SecNum].code16_list, search_index);

    *FoundIndex = search_index;

    return ValuePtr;
}


/* ua_ebp_list */
static inline int *section_ua_ebp_list_Insert(unsigned int SecNum, Word_t Index)
{
    int *ValuePtr;

    JLI(ValuePtr, section[SecNum].ua_ebp_list, Index);

    return (ValuePtr == PJERR)?NULL:ValuePtr;
}

static inline int *section_ua_ebp_list_FindEntryEqualOrLowerIndex(unsigned int SecNum, Word_t Index, Word_t *FoundIndex)
{
    int *ValuePtr;
    Word_t search_index;

    search_index = Index;
    JLL(ValuePtr, section[SecNum].ua_ebp_list, search_index);

    *FoundIndex = search_index;

    return ValuePtr;
}


/* ua_esp_list */
static inline int *section_ua_esp_list_Insert(unsigned int SecNum, Word_t Index)
{
    int *ValuePtr;

    JLI(ValuePtr, section[SecNum].ua_esp_list, Index);

    return (ValuePtr == PJERR)?NULL:ValuePtr;
}

static inline int *section_ua_esp_list_FindEntryEqualOrLowerIndex(unsigned int SecNum, Word_t Index, Word_t *FoundIndex)
{
    int *ValuePtr;
    Word_t search_index;

    search_index = Index;
    JLL(ValuePtr, section[SecNum].ua_esp_list, search_index);

    *FoundIndex = search_index;

    return ValuePtr;
}


/* code2data_list */
static inline void section_code2data_list_Insert(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1S(Rc_int, section[SecNum].code2data_list, Index);
}

void section_code2data_list_ForEach(unsigned int SecNum, void (*proc)(Word_t Index, void *data), void *data);

/* data2code_list */
static inline void section_data2code_list_Insert(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1S(Rc_int, section[SecNum].data2code_list, Index);
}

static inline int section_data2code_list_CanFindEntryEqual(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    J1T(Rc_int, section[SecNum].data2code_list, Index);

    return Rc_int;
}


/* extrn_list */
extrn_data *section_extrn_list_Insert(unsigned int SecNum, Word_t Index, const char *proc, const char *altaction);

static inline extrn_data *section_extrn_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    extrn_data **extrn_value;

    JLG(extrn_value, section[SecNum].extrn_list, Index);

    return (extrn_value)?*extrn_value:NULL;
}


/* alias_list */
alias_data *section_alias_list_Insert(unsigned int SecNum, Word_t Index, const char *proc);

static inline alias_data *section_alias_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    alias_data **alias_value;

    JLG(alias_value, section[SecNum].alias_list, Index);

    return (alias_value)?*alias_value:NULL;
}

void section_alias_list_ForEach(unsigned int SecNum, void (*proc)(alias_data *alias, void *data), void *data);


/* replace_list */
replace_data *section_replace_list_Insert(unsigned int SecNum, Word_t Index, const char *instr, unsigned int instr_len);

static inline replace_data *section_replace_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    replace_data **replace_value;

    JLG(replace_value, section[SecNum].replace_list, Index);

    return (replace_value)?*replace_value:NULL;
}

static inline replace_data *section_replace_list_FindEntryEqualOrLower(unsigned int SecNum, Word_t Index)
{
    replace_data **replace_value;

    JLL(replace_value, section[SecNum].replace_list, Index);

    return (replace_value)?*replace_value:NULL;
}

void section_replace_list_ForEach(unsigned int SecNum, void (*proc)(replace_data *replace, void *data), void *data);


/* iflags_list */
bound_data *section_iflags_list_Insert(unsigned int SecNum, Word_t Index, int begin, int end);

static inline bound_data *section_iflags_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    bound_data **iflags_value;

    JLG(iflags_value, section[SecNum].iflags_list, Index);

    return (iflags_value)?*iflags_value:NULL;
}


/* bound_list */
bound_data *section_bound_list_Insert(unsigned int SecNum, Word_t Index);

void section_bound_list_ForEach(unsigned int SecNum, void (*proc)(bound_data *bound, void *data), void *data);

static inline void section_bound_list_Delete(unsigned int SecNum, Word_t Index)
{
    int Rc_int;

    JLD(Rc_int, section[SecNum].bound_list, Index);
}


/* fixup_list */
fixup_data *section_fixup_list_Insert(unsigned int SecNum, Word_t Index);

static inline fixup_data *section_fixup_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    fixup_data **fixup_value;

    JLG(fixup_value, section[SecNum].fixup_list, Index);

    return (fixup_value)?*fixup_value:NULL;
}

static inline fixup_data *section_fixup_list_FindEntryEqualOrHigher(unsigned int SecNum, Word_t Index)
{
    fixup_data **fixup_value;

    JLF(fixup_value, section[SecNum].fixup_list, Index);

    return (fixup_value)?*fixup_value:NULL;
}

static inline fixup_data *section_fixup_list_FindEntryHigher(unsigned int SecNum, Word_t Index)
{
    fixup_data **fixup_value;

    JLN(fixup_value, section[SecNum].fixup_list, Index);

    return (fixup_value)?*fixup_value:NULL;
}

static inline fixup_data *section_fixup_list_FindEntryLower(unsigned int SecNum, Word_t Index)
{
    fixup_data **fixup_value;

    JLP(fixup_value, section[SecNum].fixup_list, Index);

    return (fixup_value)?*fixup_value:NULL;
}

void section_fixup_list_ForEach(unsigned int SecNum, void (*proc)(fixup_data *fixup, void *data), void *data);


/* output_list */
output_data *section_output_list_Insert(unsigned int SecNum, Word_t Index);

static inline output_data *section_output_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    JLG(output_value, section[SecNum].output_list, Index);

    return (output_value)?*output_value:NULL;
}

static inline output_data *section_output_list_FindEntryEqualOrHigher(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    JLF(output_value, section[SecNum].output_list, Index);

    return (output_value)?*output_value:NULL;
}

static inline output_data *section_output_list_FindEntryEqualOrLower(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    JLL(output_value, section[SecNum].output_list, Index);

    return (output_value)?*output_value:NULL;
}

static inline output_data *section_output_list_FindEntryFirst(unsigned int SecNum)
{
    output_data **output_value;
    Word_t search_index;

    search_index = 0;
    JLF(output_value, section[SecNum].output_list, search_index);

    return (output_value)?*output_value:NULL;
}

static inline output_data *section_output_list_FindEntryHigher(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    JLN(output_value, section[SecNum].output_list, Index);

    return (output_value)?*output_value:NULL;
}

static inline output_data *section_output_list_FindEntryLast(unsigned int SecNum)
{
    output_data **output_value;
    Word_t search_index;

    search_index = -1;
    JLL(output_value, section[SecNum].output_list, search_index);

    return (output_value)?*output_value:NULL;
}

static inline output_data *section_output_list_FindEntryLower(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    JLP(output_value, section[SecNum].output_list, Index);

    return (output_value)?*output_value:NULL;
}

void section_output_list_ForEach(unsigned int SecNum, void (*proc)(output_data *output, void *data), void *data);

void section_output_list_Delete(unsigned int SecNum, Word_t Index);


/* region_list */
region_data *section_region_list_Insert(unsigned int SecNum, Word_t Index, uint_fast32_t end_ofs);

static inline region_data *section_region_list_FindEntryFirst(unsigned int SecNum)
{
    region_data **region_value;
    Word_t search_index;

    search_index = 0;
    JLF(region_value, section[SecNum].region_list, search_index);

    return (region_value)?*region_value:NULL;
}

static inline region_data *section_region_list_FindEntryHigher(unsigned int SecNum, Word_t Index)
{
    region_data **region_value;

    JLN(region_value, section[SecNum].region_list, Index);

    return (region_value)?*region_value:NULL;
}

/*static inline region_data *section_region_list_FindEntryLast(unsigned int SecNum)
{
    region_data **region_value;
    Word_t search_index;

    search_index = -1;
    JLL(region_value, section[SecNum].region_list, search_index);

    return (region_value)?*region_value:NULL;
}

static inline region_data *section_region_list_FindEntryLower(unsigned int SecNum, Word_t Index)
{
    region_data **region_value;

    JLP(region_value, section[SecNum].region_list, Index);

    return (region_value)?*region_value:NULL;
}*/


#endif

