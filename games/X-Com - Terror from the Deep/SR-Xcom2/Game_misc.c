/**
 *
 *  Copyright (C) 2023 Roman Pauer
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

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>
#else
    #include <sys/mman.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "Game_misc.h"


typedef struct _node_t {
    struct _node_t *next;
    uintptr_t value;
} node_t;


void *Game_area_copy(void *area, unsigned int length)
{
    void *newarea;

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    DWORD oldprotect;

    newarea = VirtualAlloc(NULL, length, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (newarea == NULL) return NULL;

    memcpy(newarea, area, length);

    VirtualProtect(newarea, length, PAGE_READONLY, &oldprotect);
#else
    newarea = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (newarea == MAP_FAILED) return NULL;

    memcpy(newarea, area, length);

    mprotect(newarea, length, PROT_READ);
#endif

    return newarea;
}

void Game_area_free(void *area, unsigned int length)
{
#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
    VirtualFree(area, 0, MEM_RELEASE);
#else
    munmap(area, length);
#endif
}


int Game_list_insert(void **list, uintptr_t value)
{
    node_t *newnode, *curnode, *prevnode;

    if (list == NULL) return 0;

    newnode = (node_t *)malloc(sizeof(node_t));
    if (newnode == NULL) return 0;

    newnode->value = value;

    curnode = (node_t *) *list;
    prevnode = NULL;
    while ((curnode != NULL) && (curnode->value < value))
    {
        prevnode = curnode;
        curnode = curnode->next;
    }

    if ((curnode != NULL) && (curnode->value == value))
    {
        free(newnode);
        return 1;
    }

    if (prevnode == NULL)
    {
        *list = newnode;
    }
    else
    {
        prevnode->next = newnode;
    }
    newnode->next = curnode;
    return 1;
}

int Game_list_remove(void **list, uintptr_t value)
{
    node_t *curnode, *prevnode;

    if (list == NULL) return 0;

    curnode = (node_t *) *list;
    prevnode = NULL;
    while ((curnode != NULL) && (curnode->value < value))
    {
        prevnode = curnode;
        curnode = curnode->next;
    }

    if ((curnode != NULL) && (curnode->value == value))
    {
        if (prevnode == NULL)
        {
            *list = curnode->next;
        }
        else
        {
            prevnode->next = curnode->next;
        }

        free(curnode);
        return 1;
    }

    return 0;
}

void Game_list_clear(void **list, void (*func)(uintptr_t value))
{
    node_t *curnode, *prevnode;

    if (list == NULL) return;

    curnode = (node_t *) *list;
    *list = NULL;

    while (curnode != NULL)
    {
        if (func != NULL)
        {
            func(curnode->value);
        }
        prevnode = curnode;
        curnode = curnode->next;
        free(prevnode);
    }
}

