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

#if !defined(USE_JUDY)

#include <cstring>
#include <cstdlib>
#include "SR_defs.h"
#include "SR_vars.h"
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#if defined(__GNUC__)
    #define UNLIKELY_COND(x) __builtin_expect(!!(x), 0)
#else
    #define UNLIKELY_COND(x) x
#endif

#if __cplusplus >= 202002L || (_MSC_VER >= 1926 && _MSVC_LANG >= 202002L)
    #define UNLIKELY_ATTR [[unlikely]]
#else
    #define UNLIKELY_ATTR
#endif


Word_t *OMap_Ins(void **PMap, Word_t Index)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR
    {
        OMap = new std::map<Word_t,Word_t>();
        *PMap = static_cast<void *>(OMap);
    }

    auto result = OMap->insert(std::make_pair(Index, 0));

    // Get a reference to the value using the iterator from the pair result
    return &result.first->second;
}

void OMap_Del(void **PMap, Word_t Index)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return;

    OMap->erase(Index);
}

Word_t *OMap_Get(void **PMap, Word_t Index)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return nullptr;

    auto iter = OMap->find(Index);

    if (iter != OMap->end())
    {
        // Get a reference to the value using the iterator
        return &iter->second;
    }
    else return nullptr;
}

void OMap_Free(void **PMap)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return;

    delete OMap;
    *PMap = nullptr;
}

Word_t *OMap_First(void **PMap, Word_t Index, Word_t *FoundIndex)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return nullptr;

    // Find first element with key greater or equal than Index
    auto iter = OMap->lower_bound(Index);

    if (iter != OMap->end())
    {
        // Get the key value using the iterator
        if (FoundIndex != nullptr)
        {
            *FoundIndex = iter->first;
        }

        // Get a reference to the value using the iterator
        return &iter->second;
    }
    else return nullptr;
}

Word_t *OMap_First0(void **PMap, Word_t *FoundIndex)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return nullptr;

    if (!OMap->empty())
    {
        // Find first element
        auto iter = OMap->begin();

        // Get the key value using the iterator
        if (FoundIndex != nullptr)
        {
            *FoundIndex = iter->first;
        }

        // Get a reference to the value using the iterator
        return &iter->second;
    }
    else return nullptr;
}

Word_t *OMap_Next(void **PMap, Word_t Index, Word_t *FoundIndex)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return nullptr;

    // Find first element with key greater than Index
    auto iter = OMap->upper_bound(Index);

    if (iter != OMap->end())
    {
        // Get the key value using the iterator
        if (FoundIndex != nullptr)
        {
            *FoundIndex = iter->first;
        }

        // Get a reference to the value using the iterator
        return &iter->second;
    }
    else return nullptr;
}

Word_t *OMap_Last(void **PMap, Word_t Index, Word_t *FoundIndex)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return nullptr;

    // Find first element with key greater than Index
    auto iter = OMap->upper_bound(Index);

    if (iter != OMap->begin())
    {
        // Get previous element (with key less or equal than Index)
        iter--;

        // Get the key value using the iterator
        if (FoundIndex != nullptr)
        {
            *FoundIndex = iter->first;
        }

        // Get a reference to the value using the iterator
        return &iter->second;
    }
    else return nullptr;
}

Word_t *OMap_Last0(void **PMap, Word_t *FoundIndex)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return nullptr;

    if (!OMap->empty())
    {
        // Find last element
        auto iter = OMap->end();
        iter--;

        // Get the key value using the iterator
        if (FoundIndex != nullptr)
        {
            *FoundIndex = iter->first;
        }

        // Get a reference to the value using the iterator
        return &iter->second;
    }
    else return nullptr;
}

Word_t *OMap_Prev(void **PMap, Word_t Index, Word_t *FoundIndex)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return nullptr;

    // Find first element with key greater or equal than Index
    auto iter = OMap->lower_bound(Index);

    if (iter != OMap->begin())
    {
        // Get previous element (with key less or equal than Index)
        iter--;

        // Get the key value using the iterator
        if (FoundIndex != nullptr)
        {
            *FoundIndex = iter->first;
        }

        // Get a reference to the value using the iterator
        return &iter->second;
    }
    else return nullptr;
}

void OMap_ForEach(void **PMap, void (*proc)(Word_t value, void *data), void *data)
{
    auto OMap = static_cast<std::map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(OMap == nullptr)) UNLIKELY_ATTR return;

    for (auto iter = OMap->begin(); iter != OMap->end(); iter++)
    {
        proc(iter->second, data);
    }
}


Word_t *UMap_Ins(void **PMap, Word_t Index)
{
    auto UMap = static_cast<std::unordered_map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(UMap == nullptr)) UNLIKELY_ATTR
    {
        UMap = new std::unordered_map<Word_t,Word_t>();
        *PMap = static_cast<void *>(UMap);
    }

    auto result = UMap->insert(std::make_pair(Index, 0));

    // Get a reference to the value using the iterator from the pair result
    return &result.first->second;
}

Word_t *UMap_Get(void **PMap, Word_t Index)
{
    auto UMap = static_cast<std::unordered_map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(UMap == nullptr)) UNLIKELY_ATTR return nullptr;

    auto iter = UMap->find(Index);

    if (iter != UMap->end())
    {
        // Get a reference to the value using the iterator
        return &iter->second;
    }
    else return nullptr;
}

void UMap_Free(void **PMap)
{
    auto UMap = static_cast<std::unordered_map<Word_t,Word_t> *>(*PMap);

    if (UNLIKELY_COND(UMap == nullptr)) UNLIKELY_ATTR return;

    delete UMap;
    *PMap = nullptr;
}


void OSet_Set(void **PSet, Word_t Index)
{
    auto OSet = static_cast<std::set<Word_t> *>(*PSet);

    if (UNLIKELY_COND(OSet == nullptr)) UNLIKELY_ATTR
    {
        OSet = new std::set<Word_t>();
        *PSet = static_cast<void *>(OSet);
    }

    OSet->insert(Index);
}

void OSet_Unset(void **PSet, Word_t Index)
{
    auto OSet = static_cast<std::set<Word_t> *>(*PSet);

    if (UNLIKELY_COND(OSet == nullptr)) UNLIKELY_ATTR return;

    OSet->erase(Index);
}

void OSet_Free(void **PSet)
{
    auto OSet = static_cast<std::set<Word_t> *>(*PSet);

    if (UNLIKELY_COND(OSet == nullptr)) UNLIKELY_ATTR return;

    delete OSet;
    *PSet = nullptr;
}

int OSet_First0(void **PSet, Word_t *FoundIndex)
{
    auto OSet = static_cast<std::set<Word_t> *>(*PSet);

    if (UNLIKELY_COND(OSet == nullptr)) UNLIKELY_ATTR return 0;

    if (!OSet->empty())
    {
        if (FoundIndex != nullptr)
        {
            *FoundIndex = *OSet->begin();
        }
        return 1;
    }
    else return 0;
}

void OSet_ForEach(void **PSet, void (*proc)(Word_t value, void *data), void *data)
{
    auto OSet = static_cast<std::set<Word_t> *>(*PSet);

    if (UNLIKELY_COND(OSet == nullptr)) UNLIKELY_ATTR return;

    for (auto iter = OSet->begin(); iter != OSet->end(); iter++)
    {
        proc(*iter, data);
    }
}


void USet_Set(void **PSet, Word_t Index)
{
    auto USet = static_cast<std::unordered_set<Word_t> *>(*PSet);

    if (UNLIKELY_COND(USet == nullptr)) UNLIKELY_ATTR
    {
        USet = new std::unordered_set<Word_t>();
        *PSet = static_cast<void *>(USet);
    }

    USet->insert(Index);
}

int USet_Test(void **PSet, Word_t Index)
{
    auto USet = static_cast<std::unordered_set<Word_t> *>(*PSet);

    if (UNLIKELY_COND(USet == nullptr)) UNLIKELY_ATTR return 0;

#if __cplusplus >= 202002L || _MSVC_LANG >= 202002L
    return USet->contains(Index);
#else
    return USet->count(Index) != 0;
#endif
}

void USet_Free(void **PSet)
{
    auto USet = static_cast<std::unordered_set<Word_t> *>(*PSet);

    if (UNLIKELY_COND(USet == nullptr)) UNLIKELY_ATTR return;

    delete USet;
    *PSet = nullptr;
}


/* extrn_list */
extrn_data *section_extrn_list_Insert(unsigned int SecNum, Word_t Index, const char *proc, const char *altaction)
{
    extrn_data *extrn, **extrn_value;

    extrn = static_cast<extrn_data *>(malloc(sizeof(extrn_data) + strlen(proc)));
    if (extrn == nullptr) return nullptr;

    extrn->ofs = static_cast<uint_fast32_t>(Index);
    strcpy(extrn->proc, proc);

    extrn_value = reinterpret_cast<extrn_data **>(OMap_Ins(&section[SecNum].extrn_list, Index));
    if (extrn_value == nullptr)
    {
        free(extrn);
        return nullptr;
    }

    if (*extrn_value != nullptr)
    {
        free(*extrn_value);
    }

    extrn->altaction = (altaction)?strdup(altaction):nullptr;

    *extrn_value = extrn;

    return extrn;
}

static void section_extrn_list_FreeCallback(Word_t value, void *data)
{
    extrn_data *extrn = reinterpret_cast<extrn_data *>(value);

    if (extrn->altaction != NULL)
    {
        free(extrn->altaction);
    }
    free(extrn);
}

void section_extrn_list_Free(unsigned int SecNum)
{
    OMap_ForEach(&section[SecNum].extrn_list, section_extrn_list_FreeCallback, nullptr);
    OMap_Free(&section[SecNum].extrn_list);
}


/* alias_list */
alias_data *section_alias_list_Insert(unsigned int SecNum, Word_t Index, const char *proc)
{
    alias_data *alias, **alias_value;

    alias = static_cast<alias_data *>(malloc(sizeof(alias_data) + strlen(proc)));
    if (alias == nullptr) return nullptr;

    alias->ofs = static_cast<uint_fast32_t>(Index);
    strcpy(alias->proc, proc);

    alias_value = reinterpret_cast<alias_data **>(OMap_Ins(&section[SecNum].alias_list, Index));
    if (alias_value == nullptr)
    {
        free(alias);
        return nullptr;
    }

    if (*alias_value != nullptr)
    {
        free(*alias_value);
    }

    *alias_value = alias;

    return alias;
}

static void section_alias_list_FreeCallback(Word_t value, void *data)
{
    alias_data *alias = reinterpret_cast<alias_data *>(value);

    free(alias);
}

void section_alias_list_Free(unsigned int SecNum)
{
    OMap_ForEach(&section[SecNum].alias_list, section_alias_list_FreeCallback, nullptr);
    OMap_Free(&section[SecNum].alias_list);
}


/* replace_list */
replace_data *section_replace_list_Insert(unsigned int SecNum, Word_t Index, const char *instr, unsigned int instr_len, int instr_empty)
{
    replace_data *replace, **replace_value;

    replace = static_cast<replace_data *>(malloc(sizeof(replace_data) + strlen(instr)));
    if (replace == nullptr) return nullptr;

    replace->ofs = static_cast<uint_fast32_t>(Index);
    replace->length = instr_len;
    replace->empty = instr_empty;
    strcpy(replace->instr, instr);

    replace_value = reinterpret_cast<replace_data **>(OMap_Ins(&section[SecNum].replace_list, Index));
    if (replace_value == nullptr)
    {
        free(replace);
        return nullptr;
    }

    if (*replace_value != nullptr)
    {
        free(*replace_value);
    }

    *replace_value = replace;

    return replace;
}

static void section_replace_list_FreeCallback(Word_t value, void *data)
{
    replace_data *replace = reinterpret_cast<replace_data *>(value);

    free(replace);
}

void section_replace_list_Free(unsigned int SecNum)
{
    OMap_ForEach(&section[SecNum].replace_list, section_replace_list_FreeCallback, nullptr);
    OMap_Free(&section[SecNum].replace_list);
}


/* iflags_list */
bound_data *section_iflags_list_Insert(unsigned int SecNum, Word_t Index, int begin, int end)
{
    bound_data *iflags, **iflags_value;

    iflags_value = reinterpret_cast<bound_data **>(UMap_Ins(&section[SecNum].iflags_list, Index));
    if (iflags_value == nullptr)
    {
        return nullptr;
    }

    iflags = *iflags_value;
    if (iflags == nullptr)
    {
        iflags = static_cast<bound_data *>(malloc(sizeof(bound_data)));
        if (iflags == nullptr) return nullptr;

        *iflags_value = iflags;
    }

    iflags->ofs = static_cast<uint_fast32_t>(Index);
    iflags->begin = begin;
    iflags->end = end;

    return iflags;
}

static void section_iflags_list_FreeCallback(Word_t value, void *data)
{
    bound_data *iflags = reinterpret_cast<bound_data *>(value);

    if (iflags != nullptr)
    {
        free(iflags);
    }
}

void section_iflags_list_Free(unsigned int SecNum)
{
    OMap_ForEach(&section[SecNum].iflags_list, section_iflags_list_FreeCallback, nullptr);
    OMap_Free(&section[SecNum].iflags_list);
}


/* bound_list */
bound_data *section_bound_list_Insert(unsigned int SecNum, Word_t Index)
{
    bound_data *bound, **bound_value;

    bound_value = reinterpret_cast<bound_data **>(OMap_Ins(&section[SecNum].bound_list, Index));
    if (bound_value == nullptr)
    {
        return nullptr;
    }

    bound = *bound_value;
    if (bound == nullptr)
    {
        bound = static_cast<bound_data *>(malloc(sizeof(bound_data)));
        if (bound == nullptr) return nullptr;

        *bound_value = bound;

        bound->begin = 0;
        bound->end = 0;
    }

    bound->ofs = static_cast<uint_fast32_t>(Index);

    return bound;
}

void section_bound_list_Delete(unsigned int SecNum, Word_t Index)
{
    bound_data *bound, **bound_value;

    bound_value = reinterpret_cast<bound_data **>(OMap_Get(&section[SecNum].bound_list, Index));

    if (bound_value != nullptr)
    {
        bound = *bound_value;
        if (bound != nullptr)
        {
            free(bound);
        }

        OMap_Del(&section[SecNum].bound_list, Index);
    }
}

static void section_bound_list_FreeCallback(Word_t value, void *data)
{
    bound_data *bound = reinterpret_cast<bound_data *>(value);

    if (bound != nullptr)
    {
        free(bound);
    }
}

void section_bound_list_Free(unsigned int SecNum)
{
    OMap_ForEach(&section[SecNum].bound_list, section_bound_list_FreeCallback, nullptr);
    OMap_Free(&section[SecNum].bound_list);
}


/* fixup_list */
fixup_data *section_fixup_list_Insert(unsigned int SecNum, Word_t Index)
{
    fixup_data *fixup, **fixup_value;

    fixup_value = reinterpret_cast<fixup_data **>(OMap_Ins(&section[SecNum].fixup_list, Index));
    if (fixup_value == nullptr)
    {
        return nullptr;
    }

    fixup = *fixup_value;
    if (fixup == nullptr)
    {
        fixup = static_cast<fixup_data *>(malloc(sizeof(fixup_data)));
        if (fixup == nullptr) return nullptr;

        *fixup_value = fixup;

        fixup->tofs = 0;
        fixup->tsec = 0;
        fixup->type = FT_NORMAL;
    }

    fixup->sofs = static_cast<uint_fast32_t>(Index);

    return fixup;
}

static void section_fixup_list_FreeCallback(Word_t value, void *data)
{
    fixup_data *fixup = reinterpret_cast<fixup_data *>(value);

    if (fixup != nullptr)
    {
        free(fixup);
    }
}

void section_fixup_list_Free(unsigned int SecNum)
{
    OMap_ForEach(&section[SecNum].fixup_list, section_fixup_list_FreeCallback, nullptr);
    OMap_Free(&section[SecNum].fixup_list);
}


/* output_list */
output_data *section_output_list_Insert(unsigned int SecNum, Word_t Index)
{
    output_data *output, **output_value;

    output_value = reinterpret_cast<output_data **>(OMap_Ins(&section[SecNum].output_list, Index));
    if (output_value == nullptr)
    {
        return nullptr;
    }

    output = *output_value;
    if (output == nullptr)
    {
        output = static_cast<output_data *>(malloc(sizeof(output_data)));
        if (output == nullptr) return nullptr;

        *output_value = output;

        output->len = 0;
        output->str = nullptr;
        output->has_label = 0;
        output->align = 0;
        output->type = OT_UNKNOWN;
    }

    output->ofs = static_cast<uint_fast32_t>(Index);

    return output;
}

void section_output_list_Delete(unsigned int SecNum, Word_t Index)
{
    output_data *output, **output_value;

    output_value = reinterpret_cast<output_data **>(OMap_Get(&section[SecNum].output_list, Index));

    if (output_value != NULL)
    {
        output = *output_value;
        if (output != nullptr)
        {
            if (output->str != nullptr)
            {
                free(output->str);
            }
            free(output);
        }

        OMap_Del(&section[SecNum].output_list, Index);
    }
}

static void section_output_list_FreeCallback(Word_t value, void *data)
{
    output_data *output = reinterpret_cast<output_data *>(value);

    if (output != nullptr)
    {
        if (output->str != nullptr)
        {
            free(output->str);
        }
        free(output);
    }
}

void section_output_list_Free(unsigned int SecNum)
{
    OMap_ForEach(&section[SecNum].output_list, section_output_list_FreeCallback, nullptr);
    OMap_Free(&section[SecNum].output_list);
}


/* region_list */
region_data *section_region_list_Insert(unsigned int SecNum, Word_t Index, uint_fast32_t end_ofs)
{
    region_data *region, **region_value;

    region = static_cast<region_data *>(malloc(sizeof(region_data)));
    if (region == nullptr) return nullptr;

    region->begin_ofs = static_cast<uint_fast32_t>(Index);
    region->end_ofs = end_ofs;

    region_value = reinterpret_cast<region_data **>(OMap_Ins(&section[SecNum].region_list, Index));
    if (region_value == nullptr)
    {
        free(region);
        return nullptr;
    }

    if (*region_value != nullptr)
    {
        free(*region_value);
    }

    *region_value = region;

    return region;
}

static void section_region_list_FreeCallback(Word_t value, void *data)
{
    region_data *region = reinterpret_cast<region_data *>(value);

    free(region);
}

void section_region_list_Free(unsigned int SecNum)
{
    OMap_ForEach(&section[SecNum].region_list, section_region_list_FreeCallback, nullptr);
    OMap_Free(&section[SecNum].region_list);
}

#endif

