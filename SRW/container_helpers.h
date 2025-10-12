/**
 *
 *  Copyright (C) 2025 Roman Pauer
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

#if !defined(_CONTAINER_HELPERS_H_INCLUDED_)
#define _CONTAINER_HELPERS_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

Word_t *OMap_Ins(void **PMap, Word_t Index);
void OMap_Del(void **PMap, Word_t Index);
Word_t *OMap_Get(void **PMap, Word_t Index);
void OMap_Free(void **PMap);
Word_t *OMap_First(void **PMap, Word_t Index, Word_t *FoundIndex);
Word_t *OMap_First0(void **PMap, Word_t *FoundIndex);
Word_t *OMap_Next(void **PMap, Word_t Index, Word_t *FoundIndex);
Word_t *OMap_Last(void **PMap, Word_t Index, Word_t *FoundIndex);
Word_t *OMap_Last0(void **PMap, Word_t *FoundIndex);
Word_t *OMap_Prev(void **PMap, Word_t Index, Word_t *FoundIndex);
void OMap_ForEach(void **PMap, void (*proc)(Word_t value, void *data), void *data);

Word_t *UMap_Ins(void **PMap, Word_t Index);
Word_t *UMap_Get(void **PMap, Word_t Index);
void UMap_Free(void **PMap);

void OSet_Set(void **PSet, Word_t Index);
void OSet_Unset(void **PSet, Word_t Index);
void OSet_Free(void **PSet);
int OSet_First0(void **PSet, Word_t *FoundIndex);

void USet_Set(void **PSet, Word_t Index);
int USet_Test(void **PSet, Word_t Index);
void USet_Free(void **PSet);


/* code_list */
static INLINE void section_code_list_Insert(unsigned int SecNum, Word_t Index)
{
    USet_Set(&section[SecNum].code_list, Index);
}

static INLINE int section_code_list_CanFindEntryEqual(unsigned int SecNum, Word_t Index)
{
    return USet_Test(&section[SecNum].code_list, Index);
}

static INLINE void section_code_list_Free(unsigned int SecNum)
{
    USet_Free(&section[SecNum].code_list);
}


/* noret_list */
static INLINE void section_noret_list_Insert(unsigned int SecNum, Word_t Index)
{
    USet_Set(&section[SecNum].noret_list, Index);
}

static INLINE int section_noret_list_CanFindEntryEqual(unsigned int SecNum, Word_t Index)
{
    return USet_Test(&section[SecNum].noret_list, Index);
}

static INLINE void section_noret_list_Free(unsigned int SecNum)
{
    USet_Free(&section[SecNum].noret_list);
}


/* nocode_list */
static INLINE void section_nocode_list_Insert(unsigned int SecNum, Word_t Index)
{
    USet_Set(&section[SecNum].nocode_list, Index);
}

static INLINE int section_nocode_list_CanFindEntryEqual(unsigned int SecNum, Word_t Index)
{
    return USet_Test(&section[SecNum].nocode_list, Index);
}

static INLINE void section_nocode_list_Free(unsigned int SecNum)
{
    USet_Free(&section[SecNum].nocode_list);
}


/* entry_list */
static INLINE void section_entry_list_Insert(unsigned int SecNum, Word_t Index)
{
    OSet_Set(&section[SecNum].entry_list, Index);
}

static INLINE int section_entry_list_FindFirstIndex(unsigned int SecNum, Word_t *FoundIndex)
{
    return OSet_First0(&section[SecNum].entry_list, FoundIndex);
}

static INLINE void section_entry_list_Delete(unsigned int SecNum, Word_t Index)
{
    OSet_Unset(&section[SecNum].entry_list, Index);
}

static INLINE void section_entry_list_Free(unsigned int SecNum)
{
    OSet_Free(&section[SecNum].entry_list);
}


/* label_list */
static INLINE int *section_label_list_Insert(unsigned int SecNum, Word_t Index)
{
    return (int *)UMap_Ins(&section[SecNum].label_list, Index);
}

static INLINE int *section_label_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    return (int *)UMap_Get(&section[SecNum].label_list, Index);
}

static INLINE void section_label_list_Free(unsigned int SecNum)
{
    UMap_Free(&section[SecNum].label_list);
}


/* code16_list */
static INLINE int *section_code16_list_Insert(unsigned int SecNum, Word_t Index)
{
    return (int *)OMap_Ins(&section[SecNum].code16_list, Index);
}

static INLINE int *section_code16_list_FindEntryEqualOrLowerIndex(unsigned int SecNum, Word_t Index, Word_t *FoundIndex)
{
    return (int *)OMap_Last(&section[SecNum].code16_list, Index, FoundIndex);
}

static INLINE void section_code16_list_Free(unsigned int SecNum)
{
    OMap_Free(&section[SecNum].code16_list);
}


/* ua_ebp_list */
static INLINE int *section_ua_ebp_list_Insert(unsigned int SecNum, Word_t Index)
{
    return (int *)OMap_Ins(&section[SecNum].ua_ebp_list, Index);
}

static INLINE int *section_ua_ebp_list_FindEntryEqualOrLowerIndex(unsigned int SecNum, Word_t Index, Word_t *FoundIndex)
{
    return (int *)OMap_Last(&section[SecNum].ua_ebp_list, Index, FoundIndex);
}

static INLINE void section_ua_ebp_list_Free(unsigned int SecNum)
{
    OMap_Free(&section[SecNum].ua_ebp_list);
}


/* ua_esp_list */
static INLINE int *section_ua_esp_list_Insert(unsigned int SecNum, Word_t Index)
{
    return (int *)OMap_Ins(&section[SecNum].ua_esp_list, Index);
}

static INLINE int *section_ua_esp_list_FindEntryEqualOrLowerIndex(unsigned int SecNum, Word_t Index, Word_t *FoundIndex)
{
    return (int *)OMap_Last(&section[SecNum].ua_esp_list, Index, FoundIndex);
}

static INLINE void section_ua_esp_list_Free(unsigned int SecNum)
{
    OMap_Free(&section[SecNum].ua_esp_list);
}


/* extrn_list */
extrn_data *section_extrn_list_Insert(unsigned int SecNum, Word_t Index, const char *proc, const char *altaction);

static INLINE extrn_data *section_extrn_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    extrn_data **extrn_value;

    extrn_value = (extrn_data **)OMap_Get(&section[SecNum].extrn_list, Index);

    return (extrn_value)?*extrn_value:NULL;
}

static INLINE void section_extrn_list_ForEach(unsigned int SecNum, void (*proc)(extrn_data *extrn, void *data), void *data)
{
    OMap_ForEach(&section[SecNum].extrn_list, (void (*)(Word_t, void *))proc, data);
}

void section_extrn_list_Free(unsigned int SecNum);


/* alias_list */
alias_data *section_alias_list_Insert(unsigned int SecNum, Word_t Index, const char *proc);

static INLINE alias_data *section_alias_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    alias_data **alias_value;

    alias_value = (alias_data **)OMap_Get(&section[SecNum].alias_list, Index);

    return (alias_value)?*alias_value:NULL;
}

static INLINE void section_alias_list_ForEach(unsigned int SecNum, void (*proc)(alias_data *alias, void *data), void *data)
{
    OMap_ForEach(&section[SecNum].alias_list, (void (*)(Word_t, void *))proc, data);
}

void section_alias_list_Free(unsigned int SecNum);


/* ignored_list */
static INLINE int *section_ignored_list_Insert(unsigned int SecNum, Word_t Index)
{
    return (int *)OMap_Ins(&section[SecNum].ignored_list, Index);
}

void section_ignored_list_ForEach(unsigned int SecNum, void (*proc)(Word_t Index, int Value, void *data), void *data);

static INLINE void section_ignored_list_Free(unsigned int SecNum)
{
    OMap_Free(&section[SecNum].ignored_list);
}


/* replace_list */
replace_data *section_replace_list_Insert(unsigned int SecNum, Word_t Index, const char *instr, unsigned int instr_len, int instr_empty);

static INLINE replace_data *section_replace_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    replace_data **replace_value;

    replace_value = (replace_data **)OMap_Get(&section[SecNum].replace_list, Index);

    return (replace_value)?*replace_value:NULL;
}

static INLINE replace_data *section_replace_list_FindEntryEqualOrLower(unsigned int SecNum, Word_t Index)
{
    replace_data **replace_value;

    replace_value = (replace_data **)OMap_Last(&section[SecNum].replace_list, Index, NULL);

    return (replace_value)?*replace_value:NULL;
}

static INLINE void section_replace_list_ForEach(unsigned int SecNum, void (*proc)(replace_data *replace, void *data), void *data)
{
    OMap_ForEach(&section[SecNum].replace_list, (void (*)(Word_t, void *))proc, data);
}

void section_replace_list_Free(unsigned int SecNum);


/* iflags_list */
bound_data *section_iflags_list_Insert(unsigned int SecNum, Word_t Index, int begin, int end);

static INLINE bound_data *section_iflags_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    bound_data **iflags_value;

    iflags_value = (bound_data **)UMap_Get(&section[SecNum].iflags_list, Index);

    return (iflags_value)?*iflags_value:NULL;
}

void section_iflags_list_Free(unsigned int SecNum);


/* bound_list */
bound_data *section_bound_list_Insert(unsigned int SecNum, Word_t Index);

static INLINE void section_bound_list_ForEach(unsigned int SecNum, void (*proc)(bound_data *bound, void *data), void *data)
{
    OMap_ForEach(&section[SecNum].bound_list, (void (*)(Word_t, void *))proc, data);
}

void section_bound_list_Delete(unsigned int SecNum, Word_t Index);

void section_bound_list_Free(unsigned int SecNum);


/* fixup_list */
fixup_data *section_fixup_list_Insert(unsigned int SecNum, Word_t Index);

static INLINE fixup_data *section_fixup_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    fixup_data **fixup_value;

    fixup_value = (fixup_data **)OMap_Get(&section[SecNum].fixup_list, Index);

    return (fixup_value)?*fixup_value:NULL;
}

static INLINE fixup_data *section_fixup_list_FindEntryEqualOrHigher(unsigned int SecNum, Word_t Index)
{
    fixup_data **fixup_value;

    fixup_value = (fixup_data **)OMap_First(&section[SecNum].fixup_list, Index, NULL);

    return (fixup_value)?*fixup_value:NULL;
}

static INLINE fixup_data *section_fixup_list_FindEntryHigher(unsigned int SecNum, Word_t Index)
{
    fixup_data **fixup_value;

    fixup_value = (fixup_data **)OMap_Next(&section[SecNum].fixup_list, Index, NULL);

    return (fixup_value)?*fixup_value:NULL;
}

static INLINE fixup_data *section_fixup_list_FindEntryLower(unsigned int SecNum, Word_t Index)
{
    fixup_data **fixup_value;

    fixup_value = (fixup_data **)OMap_Prev(&section[SecNum].fixup_list, Index, NULL);

    return (fixup_value)?*fixup_value:NULL;
}

static INLINE void section_fixup_list_ForEach(unsigned int SecNum, void (*proc)(fixup_data *fixup, void *data), void *data)
{
    OMap_ForEach(&section[SecNum].fixup_list, (void (*)(Word_t, void *))proc, data);
}

void section_fixup_list_Free(unsigned int SecNum);


/* output_list */
output_data *section_output_list_Insert(unsigned int SecNum, Word_t Index);

static INLINE output_data *section_output_list_FindEntryEqual(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    output_value = (output_data **)OMap_Get(&section[SecNum].output_list, Index);

    return (output_value)?*output_value:NULL;
}

static INLINE output_data *section_output_list_FindEntryEqualOrHigher(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    output_value = (output_data **)OMap_First(&section[SecNum].output_list, Index, NULL);

    return (output_value)?*output_value:NULL;
}

static INLINE output_data *section_output_list_FindEntryEqualOrLower(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    output_value = (output_data **)OMap_Last(&section[SecNum].output_list, Index, NULL);

    return (output_value)?*output_value:NULL;
}

static INLINE output_data *section_output_list_FindEntryFirst(unsigned int SecNum)
{
    output_data **output_value;

    output_value = (output_data **)OMap_First0(&section[SecNum].output_list, NULL);

    return (output_value)?*output_value:NULL;
}

static INLINE output_data *section_output_list_FindEntryHigher(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    output_value = (output_data **)OMap_Next(&section[SecNum].output_list, Index, NULL);

    return (output_value)?*output_value:NULL;
}

static INLINE output_data *section_output_list_FindEntryLast(unsigned int SecNum)
{
    output_data **output_value;

    output_value = (output_data **)OMap_Last0(&section[SecNum].output_list, NULL);

    return (output_value)?*output_value:NULL;
}

static INLINE output_data *section_output_list_FindEntryLower(unsigned int SecNum, Word_t Index)
{
    output_data **output_value;

    output_value = (output_data **)OMap_Prev(&section[SecNum].output_list, Index, NULL);

    return (output_value)?*output_value:NULL;
}

static INLINE void section_output_list_ForEach(unsigned int SecNum, void (*proc)(output_data *output, void *data), void *data)
{
    OMap_ForEach(&section[SecNum].output_list, (void (*)(Word_t, void *))proc, data);
}

void section_output_list_Delete(unsigned int SecNum, Word_t Index);

void section_output_list_Free(unsigned int SecNum);


/* region_list */
region_data *section_region_list_Insert(unsigned int SecNum, Word_t Index, uint_fast32_t end_ofs);

static INLINE region_data *section_region_list_FindEntryFirst(unsigned int SecNum)
{
    region_data **region_value;

    region_value = (region_data **)OMap_First0(&section[SecNum].region_list, NULL);

    return (region_value)?*region_value:NULL;
}

static INLINE region_data *section_region_list_FindEntryHigher(unsigned int SecNum, Word_t Index)
{
    region_data **region_value;

    region_value = (region_data **)OMap_Next(&section[SecNum].region_list, Index, NULL);

    return (region_value)?*region_value:NULL;
}

/*static INLINE region_data *section_region_list_FindEntryLast(unsigned int SecNum)
{
    region_data **region_value;

    region_value = (region_data **)OMap_Last0(&section[SecNum].region_list, NULL);

    return (region_value)?*region_value:NULL;
}

static INLINE region_data *section_region_list_FindEntryLower(unsigned int SecNum, Word_t Index)
{
    region_data **region_value;

    region_value = (region_data **)OMap_Prev(&section[SecNum].region_list, Index, NULL);

    return (region_value)?*region_value:NULL;
}*/

void section_region_list_Free(unsigned int SecNum);


/* export_list */
export_data *section_export_list_Insert(unsigned int SecNum, Word_t Index, uint_fast32_t ordinal, const char *name);

static INLINE export_data *section_export_list_FindEntryEqual1(unsigned int SecNum, Word_t Index)
{
    export_data **export_value;

    export_value = (export_data **)OMap_Get(&section[SecNum].export_list, Index);

    return (export_value)?*export_value:NULL;
}

static INLINE export_data *section_export_list_FindEntryEqual2(unsigned int SecNum, Word_t Index, uint_fast32_t ordinal)
{
    export_data **export_value, *export1;

    export_value = (export_data **)OMap_Get(&section[SecNum].export_list, Index);

    if (export_value == NULL) return NULL;

    export1 = *export_value;

    while ((export1 != NULL) && (export1->ordinal != ordinal))
    {
        export1 = export1->next;
    };

    return export1;
}

static INLINE void section_export_list_ForEach(unsigned int SecNum, void (*proc)(export_data *export1, void *data), void *data)
{
    OMap_ForEach(&section[SecNum].export_list, (void (*)(Word_t, void *))proc, data);
}

void section_export_list_Delete(unsigned int SecNum, Word_t Index);

void section_export_list_Free(unsigned int SecNum);


/* import_list */
import_data *import_list_Insert(Word_t Index);

static INLINE void import_list_ForEach(void (*proc)(import_data *import, void *data), void *data)
{
    OMap_ForEach(&import_list, (void (*)(Word_t, void *))proc, data);
}

void import_list_Free(void);

#ifdef __cplusplus
}
#endif

#endif

