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

#include "BBLL.h"
#include "BBMEM.h"
#include <string.h>


typedef struct _LL_Element {
    struct _LL_Element *prev;
    struct _LL_Element *next;
    void *data;
} LL_Element;

typedef struct _LL_List {
    int used;
    LL_Element *before_first;
    LL_Element *after_last;
    LL_Element *current;
    unsigned int num_elements;
} LL_List;


static int LL_initialized = 0;
static unsigned int LL_numLists = 0;
static LL_List *LL_lists;


int LL_Init_c(void)
{
    if (LL_initialized)
    {
        return 1;
    }

    LL_lists = (LL_List *)MEM_malloc_c(sizeof(LL_List), NULL, NULL, 0, 0);
    if (LL_lists == NULL)
    {
        return 0;
    }

    LL_lists->used = 0;
    LL_numLists = 1;
    LL_initialized = 1;

    return 1;
}

void LL_Exit_c(void)
{
    unsigned int index;

    if (!LL_initialized)
    {
        return;
    }

    for (index = 0; index < LL_numLists; index++)
    {
        if (LL_lists[index].used)
        {
            LL_DestroyList_c(index + 1);
        }
    }

    MEM_free_c(LL_lists);
    LL_initialized = 0;
}

unsigned int LL_NewList_c(void)
{
    LL_Element *before_first, *after_last;
    unsigned int index;
    LL_List *new_lists;

    before_first = (LL_Element *)MEM_malloc_c(sizeof(LL_Element), NULL, NULL, 0, 0);
    if (before_first == NULL)
    {
        return 0;
    }

    after_last = (LL_Element *)MEM_malloc_c(sizeof(LL_Element), NULL, NULL, 0, 0);
    if (after_last == NULL)
    {
        MEM_free_c(before_first);
        return 0;
    }

    for (index = 0; index < LL_numLists; index++)
    {
        if (!LL_lists[index].used) break;
    }

    if (index == LL_numLists)
    {
        new_lists = (LL_List *)MEM_malloc_c(sizeof(LL_List) * (LL_numLists + 1), NULL, NULL, 0, 0);
        if (new_lists == NULL)
        {
            MEM_free_c(before_first);
            MEM_free_c(after_last);
            return 0;
        }

        memmove(new_lists, LL_lists, sizeof(LL_List) * LL_numLists);
        MEM_free_c(LL_lists);
        LL_numLists++;
        LL_lists = new_lists;
    }

    LL_lists[index].before_first = before_first;
    LL_lists[index].after_last = after_last;
    LL_lists[index].current = NULL;
    LL_lists[index].num_elements = 0;
    LL_lists[index].used = 1;

    before_first->prev = NULL;
    before_first->next = after_last;
    after_last->prev = before_first;
    after_last->next = NULL;
    before_first->data = NULL;
    after_last->data = NULL;

    return index + 1;
}

int LL_AppendElement_c(unsigned int list_handle, void *data)
{
    unsigned int index;
    LL_Element *prev, *new;

    if ((list_handle == 0) || ((list_handle - 1) >= LL_numLists))
    {
        return 0;
    }

    index = list_handle - 1;

    if (!LL_lists[index].used)
    {
        return 0;
    }

    prev = LL_lists[index].after_last->prev;
    new = (LL_Element *)MEM_malloc_c(sizeof(LL_Element), NULL, NULL, 0, 0);
    if (new == NULL)
    {
        return 0;
    }

    new->prev = prev;
    new->next = prev->next;
    prev->next->prev = new;
    prev->next = new;
    new->data = data;
    LL_lists[index].num_elements++;

    return 1;
}

void LL_Reset_c(unsigned int list_handle)
{
    unsigned int index;

    if ((list_handle == 0) || ((list_handle - 1) >= LL_numLists))
    {
        return;
    }

    index = list_handle - 1;

    if (!LL_lists[index].used)
    {
        return;
    }

    if (LL_lists[index].num_elements != 0)
    {
        LL_lists[index].current = LL_lists[index].before_first->next;
    }
    else
    {
        LL_lists[index].current = NULL;
    }
}

void *LL_GetData_c(unsigned int list_handle)
{
    unsigned int index;

    if ((list_handle == 0) || ((list_handle - 1) >= LL_numLists))
    {
        return NULL;
    }

    index = list_handle - 1;

    if (!LL_lists[index].used)
    {
        return NULL;
    }

    if (LL_lists[index].current == NULL)
    {
        return NULL;
    }

    return LL_lists[index].current->data;
}

int LL_GotoNext_c(unsigned int list_handle)
{
    unsigned int index;

    if ((list_handle == 0) || ((list_handle - 1) >= LL_numLists))
    {
        return 0;
    }

    index = list_handle - 1;

    if (!LL_lists[index].used)
    {
        return 0;
    }

    if (LL_lists[index].current != NULL)
    {
        LL_lists[index].current = LL_lists[index].current->next;
        if (LL_lists[index].current == LL_lists[index].after_last)
        {
            LL_lists[index].current = NULL;
        }
    }

    return (LL_lists[index].current != NULL)?1:0;
}

int LL_DeleteElement_c(unsigned int list_handle)
{
    unsigned int index;
    LL_Element *current, *next;

    if ((list_handle == 0) || ((list_handle - 1) >= LL_numLists))
    {
        return 0;
    }

    index = list_handle - 1;

    if (!LL_lists[index].used)
    {
        return 0;
    }

    current = LL_lists[index].current;
    if (current == NULL)
    {
        return 0;
    }

    next = current->next;
    current->prev->next = current->next;
    current->next->prev = current->prev;
    MEM_free_c(current);
    LL_lists[index].current = next;
    if (next == LL_lists[index].after_last)
    {
        LL_lists[index].current = NULL;
    }
    LL_lists[index].num_elements--;

    return 1;
}

void LL_DestroyList_c(unsigned int list_handle)
{
    unsigned int index;
    LL_Element *element, *next;

    if ((list_handle == 0) || ((list_handle - 1) >= LL_numLists))
    {
        return;
    }

    index = list_handle - 1;

    if (!LL_lists[index].used)
    {
        return;
    }

    element = LL_lists[index].before_first;
    while (element != NULL)
    {
        next = element->next;
        MEM_free_c(element);
        element = next;
    }

    LL_lists[index].used = 0;
}

