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

#include "BBRNG.h"
#include "BBMEM.h"
#include <stddef.h>
#include <string.h>


typedef struct _RNG_struct {
    void *elements;
    int num_elements;
    int element_size;
    int read_index;
    int write_index;
} RNG_struct;


static int RNG_initialized = 0;


int RNG_Init_c(void)
{
    RNG_initialized++;
    return 1;
}

void RNG_Exit_c(void)
{
    RNG_initialized--;
}

void *RNG_NewBuffer_c(int element_size, int num_elements)
{
    RNG_struct *ring;

    ring = (RNG_struct *)MEM_malloc_c(sizeof(RNG_struct), NULL, NULL, 0, 0);
    if (ring == NULL)
    {
        return NULL;
    }

    ring->elements = MEM_malloc_c(element_size * num_elements, NULL, NULL, 0, 0);
    if (ring->elements == NULL)
    {
        MEM_free_c(ring);
        return NULL;
    }

    ring->num_elements = num_elements;
    ring->element_size = element_size;
    ring->read_index = -1;
    ring->write_index = -1;

    return ring;
}

void RNG_DelBuffer_c(void *buffer)
{
    RNG_struct *ring;

    ring = (RNG_struct *)buffer;

    MEM_free_c(ring->elements);
    MEM_free_c(ring);
}

int RNG_In_c(void *buffer, const void *element)
{
    RNG_struct *ring;

    ring = (RNG_struct *)buffer;

    if (ring->read_index == ring->write_index)
    {
        if (ring->read_index != -1)
        {
            return 0;
        }

        ring->read_index = 0;
        ring->write_index = 1;

        memmove(ring->elements, element, ring->element_size);
    }
    else
    {
        memmove((void *)(((uintptr_t)ring->elements) + (ring->write_index * ring->element_size)), element, ring->element_size);

        ring->write_index++;
        if (ring->write_index >= ring->num_elements)
        {
            ring->write_index = 0;
        }
    }

    return 1;
}

int RNG_Out_c(void *buffer, void *element)
{
    RNG_struct *ring;

    ring = (RNG_struct *)buffer;

    if (ring->read_index == -1)
    {
        return 0;
    }

    if (element != NULL)
    {
        memmove(element, (void *)(((uintptr_t)ring->elements) + (ring->read_index * ring->element_size)), ring->element_size);
    }

    ring->read_index++;
    if (ring->read_index >= ring->num_elements)
    {
        ring->read_index = 0;
    }
    if (ring->read_index == ring->write_index)
    {
        ring->read_index = -1;
        ring->write_index = -1;
    }

    return 1;
}

int RNG_Peek_c(void *buffer, void *element)
{
    RNG_struct *ring;

    ring = (RNG_struct *)buffer;

    if (ring->read_index == -1)
    {
        return 0;
    }

    if (element != NULL)
    {
        memmove(element, (void *)(((uintptr_t)ring->elements) + (ring->read_index * ring->element_size)), ring->element_size);
    }

    return 1;
}

void RNG_Replace_c(void *buffer, const void *element)
{
    RNG_struct *ring;

    ring = (RNG_struct *)buffer;

    if (ring->read_index == -1)
    {
        RNG_In_c(ring, element);
    }
    else
    {
        memmove((void *)(((uintptr_t)ring->elements) + (ring->read_index * ring->element_size)), element, ring->element_size);
    }
}

void RNG_PutFirst_c(void *buffer, const void *element)
{
    RNG_struct *ring;
    int full;

    ring = (RNG_struct *)buffer;

    if (ring->read_index == -1)
    {
        RNG_In_c(ring, element);
    }
    else
    {
        full = (ring->read_index == ring->write_index)?1:0;

        if (ring->read_index <= 0)
        {
            ring->read_index = ring->num_elements - 1;
        }
        else
        {
            ring->read_index--;
        }

        if (full)
        {
            ring->write_index = ring->read_index;
        }

        memmove((void *)(((uintptr_t)ring->elements) + (ring->read_index * ring->element_size)), element, ring->element_size);
    }
}

