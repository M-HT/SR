/**
 *
 *  Copyright (C) 2025-2026 Roman Pauer
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

#ifdef __cplusplus

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#elif defined(__APPLE__)
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <mach/mach_init.h>
#include <mach/mach_vm.h>
#include <pthread.h>
#else
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#if !defined(MAP_FIXED_NOREPLACE) && defined(MAP_EXCL)
#define MAP_FIXED_NOREPLACE (MAP_FIXED | MAP_EXCL)
#endif
#include <pthread.h>
#ifdef __linux__
#include <linux/prctl.h>
#include <sys/prctl.h>
#endif
#endif

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "Game-Memory.h"


#define HEAPBLOCK_BASE_SIZE (1024 * 1024)

typedef struct _memblock_t
{
    union
    {
        int32_t inverted_value; // allocated block: inverted value of block size; top bit is one, bottom three bits are one
        int32_t offset_next; // free block: offset (from start of heap block) to next free block; top bit is zero, bottom three bits are zero
    } u;
    int32_t block_size; // block size including header; top bit is zero, bottom three bits are zero
} memblock_t;

typedef struct _heapblock_t
{
    uint8_t *address; // address of mapped memory
    int32_t size; // size of mapped memory
    int32_t offset_first_free; // offset (from start of heap block) to first free block
    int32_t offset_largest_free; // offset (from start of heap block) to largest free block
    int32_t offset_largest_free_prev; // offset (from start of heap block) to last free block before largest free block
    struct _heapblock_t *prev_block; // previous heapblock
} heapblock_t;


static heapblock_t *last_heapblock = NULL;
static heapblock_t *last_released_heapblock = NULL;

#ifdef _WIN32
static CRITICAL_SECTION mutex;
#else
static pthread_mutex_t mutex;
#endif


#ifdef PTROFS_64BIT
extern "C" void (*ptr_initialize_pointers)(uint64_t pointer_offset);

uint64_t pointer_offset;
unsigned int pointer_reserved_length;
#else
#define pointer_offset 0
#endif


int x86_init_malloc(void)
{
    if (sizeof(void *) > 4)
    {
#ifdef _WIN32
        InitializeCriticalSection(&mutex);
        return 0;
#else
        pthread_mutexattr_t attr;

        if (pthread_mutexattr_init(&attr))
        {
            return 1;
        }

        if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE))
        {
            pthread_mutexattr_destroy(&attr);
            return 2;
        }

        if (pthread_mutex_init(&mutex, &attr))
        {
            pthread_mutexattr_destroy(&attr);
            return 3;
        }

        pthread_mutexattr_destroy(&attr);
        return 0;
#endif
    }
    else
    {
        return 0;
    }
}

void x86_deinit_malloc(void)
{
    if (sizeof(void *) > 4)
    {
#ifdef _WIN32
        DeleteCriticalSection(&mutex);
#else
        pthread_mutex_destroy(&mutex);
#endif
    }
}

#ifdef _WIN32
    #define LOCK_MUTEX EnterCriticalSection(&mutex);
    #define UNLOCK_MUTEX LeaveCriticalSection(&mutex);
#else
    #define LOCK_MUTEX pthread_mutex_lock(&mutex);
    #define UNLOCK_MUTEX pthread_mutex_unlock(&mutex);
#endif

static memblock_t *get_free_block(heapblock_t *heapblock, int32_t offset)
{
    memblock_t *freeblock;

    freeblock = (memblock_t *)(heapblock->address + offset);

    // check header integrity
    if ((freeblock->u.offset_next & 0x80000007u) != 0 ||
        (freeblock->block_size & 0x80000007u) != 0 ||
        freeblock->block_size < (long)sizeof(memblock_t) ||
        freeblock->block_size > heapblock->size - offset ||
        freeblock->u.offset_next > heapblock->size ||
        freeblock->u.offset_next < freeblock->block_size + offset
       )
    {
        fprintf(stderr, "Corrupted memory header\n");
        exit(1);
    }

    return freeblock;
}

static void find_largest_free_block(heapblock_t *heapblock)
{
    memblock_t *freeblock;
    int32_t largestsize, nextoffset, prevoffset;

    heapblock->offset_largest_free = heapblock->offset_first_free;
    heapblock->offset_largest_free_prev = -1;
    if (heapblock->offset_first_free >= 0)
    {
        freeblock = get_free_block(heapblock, heapblock->offset_first_free);
        largestsize = freeblock->block_size;

        prevoffset = heapblock->offset_first_free;
        nextoffset = freeblock->u.offset_next;
        while (nextoffset < heapblock->size)
        {
            freeblock = get_free_block(heapblock, nextoffset);
            if (freeblock->block_size > largestsize)
            {
                largestsize = freeblock->block_size;
                heapblock->offset_largest_free = nextoffset;
                heapblock->offset_largest_free_prev = prevoffset;
            }
            prevoffset = nextoffset;
            nextoffset = freeblock->u.offset_next;
        }
    }
}

static heapblock_t *find_alloc_heap_block(memblock_t *allocblock)
{
    heapblock_t *heapblock;

    for (heapblock = last_heapblock; heapblock != NULL; heapblock = heapblock->prev_block)
    {
        if ((uintptr_t)allocblock < (uintptr_t)heapblock->address) continue;
        if ((uintptr_t)allocblock >= heapblock->size + (uintptr_t)heapblock->address) continue;

        // check header integrity
        if (allocblock->block_size != ~allocblock->u.inverted_value ||
            (allocblock->block_size & 0x80000007u) != 0 ||
            allocblock->block_size < (long)sizeof(memblock_t) ||
            allocblock->block_size > (int32_t)((heapblock->size + (uintptr_t)heapblock->address) - ((uintptr_t)allocblock))
           )
        {
            fprintf(stderr, "Corrupted memory header\n");
            exit(1);
        }

        return heapblock;
    }

    fprintf(stderr, "Corrupted heap\n");
    exit(1);
}

static memblock_t *find_last_free_block_before(heapblock_t *heapblock, int32_t offset)
{
    memblock_t *freeblock;
    int32_t current_offset;

    if (heapblock->offset_largest_free < offset)
    {
        current_offset = heapblock->offset_largest_free;
    }
    else if (heapblock->offset_largest_free_prev < offset)
    {
        current_offset = heapblock->offset_largest_free_prev;
    }
    else
    {
        current_offset = heapblock->offset_first_free;
    }

    do
    {
        freeblock = get_free_block(heapblock, current_offset);
        if (freeblock->u.offset_next >= offset) return freeblock;

        current_offset = freeblock->u.offset_next;
    } while (current_offset < heapblock->size);

    fprintf(stderr, "Corrupted heap\n");
    exit(1);
}

void *x86_malloc(unsigned int size)
{
    if (sizeof(void *) > 4)
    {
        heapblock_t *heapblock;
        memblock_t *largestblock, *freeblock, *prevblock;

        if (size > 0x80000000u - (2 * sizeof(memblock_t) + HEAPBLOCK_BASE_SIZE))
        {
            errno = ENOMEM;
            return NULL;
        }

        // round up allocated size and add header size
        size = (size + 2 * sizeof(memblock_t) - 1) & ~(sizeof(memblock_t) - 1);

        LOCK_MUTEX

        // try to find free memory in existing heapblocks
        for (heapblock = last_heapblock; heapblock != NULL; heapblock = heapblock->prev_block)
        {
            if (heapblock->offset_largest_free < 0) continue; // no free memory in heapblock
            largestblock = get_free_block(heapblock, heapblock->offset_largest_free);
            if (largestblock->block_size < (int)size) continue; // size of largest free block in heapblock is smaller than required

            if (largestblock->block_size > (int)size)
            {
                // use the beginning of largest free block as allocated block and the rest as free block
                freeblock = (memblock_t *)(size + (uint8_t *)largestblock);
                freeblock->u.offset_next = largestblock->u.offset_next;
                freeblock->block_size = largestblock->block_size - size;
                largestblock->block_size = size;
                largestblock->u.inverted_value = ~largestblock->block_size;

                if (heapblock->offset_largest_free == heapblock->offset_first_free)
                {
                    heapblock->offset_first_free += size;
                }
                else
                {
                    prevblock = get_free_block(heapblock, heapblock->offset_largest_free_prev);
                    prevblock->u.offset_next += size;
                }

                if (freeblock->block_size >= heapblock->size >> 1)
                {
                    heapblock->offset_largest_free += size;
                }
                else
                {
                    find_largest_free_block(heapblock);
                }
            }
            else
            {
                // change largest free block into allocated block
                if (heapblock->offset_largest_free == heapblock->offset_first_free)
                {
                    heapblock->offset_first_free = largestblock->u.offset_next;
                    if (heapblock->offset_first_free >= heapblock->size)
                    {
                        heapblock->offset_first_free = -1;
                    }
                }
                else
                {
                    prevblock = get_free_block(heapblock, heapblock->offset_largest_free_prev);
                    prevblock->u.offset_next = largestblock->u.offset_next;
                }

                largestblock->u.inverted_value = ~largestblock->block_size;

                find_largest_free_block(heapblock);
            }

            UNLOCK_MUTEX
            errno = 0;
            return (void *)(sizeof(memblock_t) + (uint8_t *)largestblock);
        }

        do
        {
            if (last_released_heapblock != NULL)
            {
                // reuse last released heapblock
                heapblock = last_released_heapblock;
                last_released_heapblock = NULL;

                if (heapblock->size >= (int)((size + HEAPBLOCK_BASE_SIZE - 1) & ~(HEAPBLOCK_BASE_SIZE - 1))) break;

                unmap_memory_32bit(heapblock->address, heapblock->size);
            }
            else
            {
                // allocate new heapblock header
                heapblock = (heapblock_t *) malloc(sizeof(heapblock_t));
                if (heapblock == NULL)
                {
                    UNLOCK_MUTEX
                    errno = ENOMEM;
                    return NULL;
                }
            }

            // allocate heapblock memory
            heapblock->size = (size + HEAPBLOCK_BASE_SIZE - 1) & ~(HEAPBLOCK_BASE_SIZE - 1);
            heapblock->address = (uint8_t *)map_memory_32bit(heapblock->size, 0);
            if (heapblock->address == NULL)
            {
                free(heapblock);
                UNLOCK_MUTEX
                errno = ENOMEM;
                return NULL;
            }

#ifdef PR_SET_VMA_ANON_NAME
            prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, heapblock->address, heapblock->size, "heap32");
#endif
        } while (0);

        heapblock->prev_block = last_heapblock;
        last_heapblock = heapblock;

        // use the beginning of heapblock memory as allocated block
        largestblock = (memblock_t *)heapblock->address;
        largestblock->block_size = size;
        largestblock->u.inverted_value = ~largestblock->block_size;

        // use the rest of heapblock memory as free block
        if ((int)size < heapblock->size)
        {
            heapblock->offset_first_free = size;
            freeblock = (memblock_t *)(size + heapblock->address);
            freeblock->block_size = heapblock->size - size;
            freeblock->u.offset_next = heapblock->size;
        }
        else heapblock->offset_first_free = -1;

        heapblock->offset_largest_free = heapblock->offset_first_free;
        heapblock->offset_largest_free_prev = -1;

        UNLOCK_MUTEX
        errno = 0;
        return (void *)(sizeof(memblock_t) + (uint8_t *)largestblock);
    }
    else
    {
        return malloc(size);
    }
}

void x86_free(void *ptr)
{
    if (sizeof(void *) > 4)
    {
        memblock_t *allocblock, *freeblock, *largestblock, *prevblock, *oldblock;
        heapblock_t *heapblock, *prevheapblock;
        int32_t offsetalloc, offsetprev, offsetfree;

        if (ptr == NULL)
        {
            errno = 0;
            return;
        }

        allocblock = (memblock_t *)((uint8_t *)ptr - sizeof(memblock_t));

        LOCK_MUTEX

        heapblock = find_alloc_heap_block(allocblock);

        offsetalloc = (int32_t)((uintptr_t)allocblock - (uintptr_t)heapblock->address);

        if (heapblock->offset_first_free < 0)
        {
            // no free blocks in heapblock -> freed block will be the only free block
            allocblock->u.offset_next = heapblock->size;
            heapblock->offset_first_free = offsetalloc;
            heapblock->offset_largest_free = offsetalloc;
        }
        else if (heapblock->offset_first_free > offsetalloc)
        {
            // first free block in heapblock is after freed block -> freed block will be the first free block
            freeblock = get_free_block(heapblock, heapblock->offset_first_free);

            if (offsetalloc + allocblock->block_size == heapblock->offset_first_free)
            {
                // freed block is adjacent to first free block -> coalesce free blocks
                allocblock->u.offset_next = freeblock->u.offset_next;
                allocblock->block_size += freeblock->block_size;
                freeblock->u.offset_next = 0;
                freeblock->block_size = 0;
                get_free_block(heapblock, offsetalloc);

                if (heapblock->offset_first_free == heapblock->offset_largest_free)
                {
                    heapblock->offset_largest_free = offsetalloc;
                }
                else
                {
                    largestblock = get_free_block(heapblock, heapblock->offset_largest_free);

                    if (allocblock->block_size >= largestblock->block_size)
                    {
                        heapblock->offset_largest_free = offsetalloc;
                        heapblock->offset_largest_free_prev = -1;
                    }
                    else if (heapblock->offset_largest_free_prev == heapblock->offset_first_free)
                    {
                        heapblock->offset_largest_free_prev = offsetalloc;
                    }
                }
            }
            else
            {
                // freed block is not adjacent to first free block
                allocblock->u.offset_next = heapblock->offset_first_free;
                get_free_block(heapblock, offsetalloc);

                if (heapblock->offset_first_free == heapblock->offset_largest_free)
                {
                    largestblock = freeblock;
                }
                else
                {
                    largestblock = get_free_block(heapblock, heapblock->offset_largest_free);
                }

                if (allocblock->block_size >= largestblock->block_size)
                {
                    heapblock->offset_largest_free = offsetalloc;
                    heapblock->offset_largest_free_prev = -1;
                }
                else if (heapblock->offset_largest_free == heapblock->offset_first_free)
                {
                    heapblock->offset_largest_free_prev = offsetalloc;
                }
            }

            heapblock->offset_first_free = offsetalloc;
        }
        else
        {
            // first free block in heapblock is before freed block
            prevblock = find_last_free_block_before(heapblock, offsetalloc);

            offsetprev = (int32_t)((uintptr_t)prevblock - (uintptr_t)heapblock->address);

            if (offsetprev + prevblock->block_size == offsetalloc)
            {
                // previous free block is adjacent to freed block -> coalesce free blocks
                prevblock->block_size += allocblock->block_size;
                allocblock->u.offset_next = 0;
                allocblock->block_size = 0;

                if (offsetprev + prevblock->block_size == prevblock->u.offset_next && prevblock->u.offset_next < heapblock->size)
                {
                    // next free block is adjacent to freed block -> coalesce free blocks
                    offsetfree = prevblock->u.offset_next;
                    freeblock = get_free_block(heapblock, prevblock->u.offset_next);
                    prevblock->block_size += freeblock->block_size;
                    prevblock->u.offset_next = freeblock->u.offset_next;
                    oldblock = freeblock;

                    if (heapblock->offset_largest_free != offsetprev)
                    {
                        if (heapblock->offset_largest_free == offsetfree)
                        {
                            largestblock = freeblock;
                        }
                        else
                        {
                            largestblock = get_free_block(heapblock, heapblock->offset_largest_free);
                        }

                        if (prevblock->block_size > largestblock->block_size)
                        {
                            freeblock = find_last_free_block_before(heapblock, offsetprev);

                            heapblock->offset_largest_free = offsetprev;
                            heapblock->offset_largest_free_prev = (int32_t)((uintptr_t)freeblock - (uintptr_t)heapblock->address);
                        }
                        else if (heapblock->offset_largest_free_prev == offsetfree)
                        {
                            heapblock->offset_largest_free_prev = offsetprev;
                        }
                    }

                    oldblock->u.offset_next = 0;
                    oldblock->block_size = 0;
                }
                else
                {
                    // next free block is not adjacent to freed block

                    if (heapblock->offset_largest_free != offsetprev)
                    {
                        largestblock = get_free_block(heapblock, heapblock->offset_largest_free);
                        if (prevblock->block_size > largestblock->block_size)
                        {
                            freeblock = find_last_free_block_before(heapblock, offsetprev);

                            heapblock->offset_largest_free = offsetprev;
                            heapblock->offset_largest_free_prev = (int32_t)((uintptr_t)freeblock - (uintptr_t)heapblock->address);
                        }
                    }
                }
            }
            else
            {
                // previous free block is not adjacent to freed block
                allocblock->u.offset_next = prevblock->u.offset_next;
                get_free_block(heapblock, offsetalloc);
                prevblock->u.offset_next = offsetalloc;

                if (offsetalloc + allocblock->block_size == allocblock->u.offset_next && allocblock->u.offset_next < heapblock->size)
                {
                    // next free block is adjacent to freed block -> coalesce free blocks
                    offsetfree = allocblock->u.offset_next;
                    freeblock = get_free_block(heapblock, allocblock->u.offset_next);
                    allocblock->block_size += freeblock->block_size;
                    allocblock->u.offset_next = freeblock->u.offset_next;
                    freeblock->u.offset_next = 0;
                    freeblock->block_size = 0;

                    if (heapblock->offset_largest_free == offsetfree)
                    {
                        heapblock->offset_largest_free = offsetalloc;
                    }
                    else
                    {
                        if (heapblock->offset_largest_free == offsetprev)
                        {
                            largestblock = prevblock;
                        }
                        else
                        {
                            largestblock = get_free_block(heapblock, heapblock->offset_largest_free);
                        }

                        if (allocblock->block_size >= largestblock->block_size)
                        {
                            heapblock->offset_largest_free = offsetalloc;
                            heapblock->offset_largest_free_prev = offsetprev;
                        }
                        else if (heapblock->offset_largest_free_prev == offsetfree)
                        {
                            heapblock->offset_largest_free_prev = offsetalloc;
                        }
                    }
                }
                else
                {
                    // next free block is not adjacent to freed block

                    if (heapblock->offset_largest_free == offsetprev)
                    {
                        largestblock = prevblock;
                    }
                    else
                    {
                        largestblock = get_free_block(heapblock, heapblock->offset_largest_free);
                    }

                    if (allocblock->block_size >= largestblock->block_size)
                    {
                        heapblock->offset_largest_free = offsetalloc;
                        heapblock->offset_largest_free_prev = offsetprev;
                    }
                    else if (heapblock->offset_largest_free_prev == offsetprev)
                    {
                        heapblock->offset_largest_free_prev = offsetalloc;
                    }
                }
            }
        }

        if (heapblock == last_heapblock && heapblock->offset_largest_free == 0)
        {
            largestblock = (memblock_t *)heapblock->address;
            if (largestblock->block_size == heapblock->size)
            {
                // last heapblock is empty -> unmap it
                for (;;)
                {
                    prevheapblock = heapblock->prev_block;
                    last_heapblock = prevheapblock;
                    if (last_released_heapblock != NULL)
                    {
                        unmap_memory_32bit(last_released_heapblock->address, last_released_heapblock->size);
                        free(last_released_heapblock);
                    }
                    if (prevheapblock == NULL)
                    {
                        last_released_heapblock = NULL;
                        unmap_memory_32bit(heapblock->address, heapblock->size);
                        free(heapblock);
                        break;
                    }
                    else
                    {
                        last_released_heapblock = heapblock;
                        heapblock->prev_block = NULL;
                    }

                    if (prevheapblock->offset_largest_free != 0) break;
                    largestblock = get_free_block(prevheapblock, 0);
                    if (largestblock->block_size != prevheapblock->size) break;
                    heapblock = prevheapblock;
                }
            }
        }

        UNLOCK_MUTEX
        errno = 0;
        return;
    }
    else
    {
        free(ptr);
    }
}

void *x86_calloc(unsigned int nmemb, unsigned int size)
{
    if (sizeof(void *) > 4)
    {
        void *ptr;

        if (size > 1)
        {
            if ((0x80000000u - (2 * sizeof(memblock_t) + HEAPBLOCK_BASE_SIZE)) / size < nmemb)
            {
                errno = ENOMEM;
                return NULL;
            }
        }

        ptr = x86_malloc(nmemb * size);
        if (ptr != NULL)
        {
            memset(ptr, 0, nmemb * size);
        }

        errno = 0;
        return ptr;
    }
    else
    {
        return calloc(nmemb, size);
    }
}

void *x86_realloc(void *ptr, unsigned int size)
{
    if (sizeof(void *) > 4)
    {
        memblock_t *allocblock, *adjacentblock, *freeblock, *prevblock;
        heapblock_t *heapblock;
        int32_t offsetalloc, offsetadjacent, allocsize, copysize;
        void *ptr2;

        if (ptr == NULL)
        {
            return x86_malloc(size);
        }

        if (size == 0)
        {
            x86_free(ptr);
            return NULL;
        }

        if (size > 0x80000000u - (2 * sizeof(memblock_t) + HEAPBLOCK_BASE_SIZE))
        {
            errno = ENOMEM;
            return NULL;
        }

        // round up allocated size and add header size
        allocsize = (size + 2 * sizeof(memblock_t) - 1) & ~(sizeof(memblock_t) - 1);

        allocblock = (memblock_t *)((uint8_t *)ptr - sizeof(memblock_t));

        LOCK_MUTEX

        heapblock = find_alloc_heap_block(allocblock);

        if (allocblock->block_size == allocsize)
        {
            // existing block size is the same as new size -> do nothing
            UNLOCK_MUTEX
            errno = 0;
            return ptr;
        }
        else if (allocblock->block_size > allocsize)
        {
            // existing block size is larger than new size -> split block into two and free the second one
            freeblock = (memblock_t *)(allocsize + (uint8_t *)allocblock);
            freeblock->block_size = allocblock->block_size - allocsize;
            freeblock->u.inverted_value = ~freeblock->block_size;
            allocblock->block_size = allocsize;
            allocblock->u.inverted_value = ~allocblock->block_size;

            x86_free((void *)(sizeof(memblock_t) + (uint8_t *)freeblock));
            UNLOCK_MUTEX
            errno = 0;
            return ptr;
        }
        else
        {
            // existing block size is smaller than new size -> check if adjacent block is large enough free block

            offsetalloc = (int32_t)((uintptr_t)allocblock - (uintptr_t)heapblock->address);
            if (offsetalloc + allocblock->block_size < heapblock->size)
            {
                adjacentblock = (memblock_t *)(allocblock->block_size + (uint8_t *)allocblock);
                if (adjacentblock->u.inverted_value >= 0 && adjacentblock->block_size >= allocsize - allocblock->block_size)
                {
                    offsetadjacent = offsetalloc + allocblock->block_size;
                    adjacentblock = get_free_block(heapblock, offsetadjacent);

                    if (adjacentblock->block_size > allocsize - allocblock->block_size)
                    {
                        // append beginning of the free block to existing block
                        freeblock = (memblock_t *)(allocsize + (uint8_t *)allocblock);
                        freeblock->u.offset_next = adjacentblock->u.offset_next;
                        freeblock->block_size = allocblock->block_size + adjacentblock->block_size - allocsize;
                        allocblock->block_size = allocsize;
                        allocblock->u.inverted_value = ~allocblock->block_size;
                        adjacentblock->u.offset_next = 0;
                        adjacentblock->block_size = 0;

                        if (heapblock->offset_first_free == offsetadjacent)
                        {
                            heapblock->offset_first_free = offsetalloc + allocsize;
                        }
                        else
                        {
                            prevblock = find_last_free_block_before(heapblock, offsetalloc);
                            prevblock->u.offset_next = offsetalloc + allocsize;
                        }

                        if (heapblock->offset_largest_free == offsetadjacent)
                        {
                            if (freeblock->block_size >= heapblock->size >> 1)
                            {
                                heapblock->offset_largest_free = offsetalloc + allocsize;
                            }
                            else
                            {
                                find_largest_free_block(heapblock);
                            }
                        }
                        else if (heapblock->offset_largest_free_prev == offsetadjacent)
                        {
                            heapblock->offset_largest_free_prev = offsetalloc + allocsize;
                        }
                    }
                    else
                    {
                        // append whole free block to existing block
                        allocblock->block_size += adjacentblock->block_size;
                        allocblock->u.inverted_value = ~allocblock->block_size;

                        prevblock = NULL; // silence compiler warning

                        if (heapblock->offset_first_free == offsetadjacent)
                        {
                            heapblock->offset_first_free = adjacentblock->u.offset_next;
                            if (heapblock->offset_first_free >= heapblock->size)
                            {
                                heapblock->offset_first_free = -1;
                            }
                        }
                        else
                        {
                            prevblock = find_last_free_block_before(heapblock, offsetalloc);
                            prevblock->u.offset_next = adjacentblock->u.offset_next;
                        }

                        adjacentblock->u.offset_next = 0;
                        adjacentblock->block_size = 0;

                        if (heapblock->offset_largest_free == offsetadjacent)
                        {
                            find_largest_free_block(heapblock);
                        }
                        else if (heapblock->offset_largest_free_prev == offsetadjacent)
                        {
                            if (heapblock->offset_largest_free == heapblock->offset_first_free)
                            {
                                heapblock->offset_largest_free_prev = -1;
                            }
                            else
                            {
                                // if execution gets here, then the same statement was executed above, so it doesn't need to be executed again
                                //prevblock = find_last_free_block_before(heapblock, offsetalloc);
                                heapblock->offset_largest_free_prev = (int32_t)((uintptr_t)prevblock - (uintptr_t)heapblock->address);
                            }
                        }
                    }

                    UNLOCK_MUTEX
                    errno = 0;
                    return ptr;
                }
            }
        }

        copysize = allocblock->block_size - sizeof(memblock_t);

        ptr2 = x86_malloc(size);
        if (ptr2 == NULL)
        {
            UNLOCK_MUTEX
            errno = ENOMEM;
            return NULL;
        }

        memcpy(ptr2, ptr, copysize);
        x86_free(ptr);
        UNLOCK_MUTEX
        errno = 0;
        return ptr2;
    }
    else
    {
        return realloc(ptr, size);
    }
}


void *map_memory_32bit(unsigned int size, int only_address_space)
{
#ifdef _WIN32
    uint64_t maddr, reg_base, reg_size;
    void *mem;
    SYSTEM_INFO sinfo;
    MEMORY_BASIC_INFORMATION minfo;

    if (size == 0) return NULL;

    GetSystemInfo(&sinfo);

    // round up requested size to the next page boundary
    size = (size + (sinfo.dwPageSize - 1)) & ~(sinfo.dwPageSize - 1);

    // set starting memory address
    maddr = pointer_offset + 1024*1024 + 65536;

    // round up starting memory address to the nearest multiple of the allocation granularity
    maddr = (maddr + (sinfo.dwAllocationGranularity - 1)) & ~(uintptr_t)(sinfo.dwAllocationGranularity - 1);

    // look for unused memory up to 2GB from pointer_offset
    while (maddr < pointer_offset + UINT64_C(0x80000000) && maddr + size <= pointer_offset + UINT64_C(0x80000000))
    {
        if (0 == VirtualQuery((void *)maddr, &minfo, sizeof(minfo))) return NULL;

        if (minfo.State == MEM_FREE)
        {
            reg_base = (((uintptr_t)minfo.BaseAddress) + (sinfo.dwAllocationGranularity - 1)) & ~(uintptr_t)(sinfo.dwAllocationGranularity - 1);
            if (minfo.RegionSize >= reg_base - (uintptr_t)minfo.BaseAddress)
            {
                reg_size = minfo.RegionSize - (reg_base - (uintptr_t)minfo.BaseAddress);

                if (reg_size >= size)
                {
                    mem = VirtualAlloc((void *)reg_base, size, MEM_RESERVE | (only_address_space ? 0 : MEM_COMMIT), only_address_space ? PAGE_NOACCESS : PAGE_READWRITE);
                    if (mem != NULL) return mem;
                }
            }
        }

        maddr = ((minfo.RegionSize + (uintptr_t)minfo.BaseAddress) + (sinfo.dwAllocationGranularity - 1)) & ~(uintptr_t)(sinfo.dwAllocationGranularity - 1);
    }

    return NULL;
#elif defined(__APPLE__)
    void *mem, *start, *start_aligned;
    int prot, flags;
    long page_size;
    mach_port_t task;
    mach_vm_address_t region_address, free_region_start, free_region_end;
    mach_vm_size_t region_size;
    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t count;
    mach_port_t object_name;

    if (size == 0) return NULL;

    prot = only_address_space ? PROT_NONE : (PROT_READ | PROT_WRITE);
    flags = MAP_PRIVATE | MAP_ANONYMOUS | (only_address_space ? MAP_NORESERVE : 0);

    // if platform supports MAP_32BIT, then try mapping memory with it
#if defined(MAP_32BIT) && (MAP_32BIT != 0)
#ifdef PTROFS_64BIT
    if (pointer_offset == 0)
#endif
    {
        mem = mmap(0, size, prot, MAP_32BIT | flags, -1, 0);
        if (mem != MAP_FAILED)
        {
            if (((uintptr_t)mem >= UINT64_C(0x80000000)) || (size + (uintptr_t)mem > UINT64_C(0x80000000)))
            {
                // mapped memory is above 2GB
                munmap(mem, size);
            }
            else
            {
                return mem;
            }
        }
    }
#endif

    // look for unused memory up to 2GB from pointer_offset and try mapping memory there
    page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) page_size = 4096;

    task = current_task();

    region_address = pointer_offset;
    count = VM_REGION_BASIC_INFO_COUNT_64;
    if (KERN_SUCCESS != mach_vm_region(task, &region_address, &region_size, VM_REGION_BASIC_INFO_64, (vm_region_info_t) &info, &count, &object_name)) return NULL;

    // first free region (starting at address zero) belongs to segment __PAGEZERO, so don't try using memory here

    if (region_address >= pointer_offset + UINT64_C(0x80000000)) return NULL;

    free_region_start = region_address + region_size;
    while (free_region_start < pointer_offset + UINT64_C(0x80000000))
    {
        region_address = free_region_start;
        count = VM_REGION_BASIC_INFO_COUNT_64;
        if (KERN_SUCCESS != mach_vm_region(task, &region_address, &region_size, VM_REGION_BASIC_INFO_64, (vm_region_info_t) &info, &count, &object_name))
        {
            region_address = free_region_end = pointer_offset + UINT64_C(0x80000000);
            region_size = 0;
        }
        else
        {
            free_region_end = region_address;
            if (free_region_end >= pointer_offset + UINT64_C(0x80000000))
            {
                free_region_end = pointer_offset + UINT64_C(0x80000000);
                region_size = 0;
            }
        }

        if (free_region_end - free_region_start >= size)
        {
            // try using memory at the end of the free region
            start = (void *)((free_region_end - size) & ~(uintptr_t)(page_size - 1));
            if ((size & (HEAPBLOCK_BASE_SIZE - 1)) == 0)
            {
                start_aligned = (void *)(((uintptr_t)start) & ~(uintptr_t)(HEAPBLOCK_BASE_SIZE - 1));
                if ((uintptr_t)start_aligned >= free_region_start + HEAPBLOCK_BASE_SIZE) start = start_aligned;
            }
            if (start != (void *)free_region_start)
            {
                mem = mmap(start, size, prot, MAP_FIXED | flags, -1, 0);
                if (mem == start) return mem;
                if (mem != MAP_FAILED)
                {
                    munmap(start, size);
                    goto error1;
                }
            }

            // try using memory at the start of the free region
            start = (void *)free_region_start;
            if ((size & (HEAPBLOCK_BASE_SIZE - 1)) == 0)
            {
                start_aligned = (void *)(((HEAPBLOCK_BASE_SIZE - 1) + (uintptr_t)start) & ~(uintptr_t)(HEAPBLOCK_BASE_SIZE - 1));
                if (size + (uintptr_t)start_aligned <= free_region_end) start = start_aligned;
            }
            mem = mmap(start, size, prot, MAP_FIXED | flags, -1, 0);
            if (mem == start) return mem;
            if (mem != MAP_FAILED)
            {
                munmap(start, size);
                goto error1;
            }
        }

        free_region_start = region_address + region_size;
    }

    return NULL;

error1:
    fprintf(stderr, "Error: memory mapped at different address\n");
    return NULL;
#else
    void *mem, *start, *start_aligned;
    int num_matches, prot, flags;
    long page_size;
    FILE *f;
    uintmax_t num0, num1, num2;

    if (size == 0) return NULL;

    prot = only_address_space ? PROT_NONE : (PROT_READ | PROT_WRITE);

#if !defined(MAP_NORESERVE) && defined(MAP_GUARD)
    flags = only_address_space ? MAP_GUARD : (MAP_PRIVATE | MAP_ANONYMOUS);
#else
    flags = MAP_PRIVATE | MAP_ANONYMOUS | (only_address_space ? MAP_NORESERVE : 0);
#endif

    // if platform supports MAP_32BIT, then try mapping memory with it
#if defined(MAP_32BIT) && (MAP_32BIT != 0)
#ifdef PTROFS_64BIT
    if (pointer_offset == 0)
#endif
    {
        mem = mmap(0, size, prot, MAP_32BIT | flags, -1, 0);
        if (mem != MAP_FAILED)
        {
            if (((uintptr_t)mem >= UINT64_C(0x80000000)) || (size + (uintptr_t)mem > UINT64_C(0x80000000)))
            {
                // mapped memory is above 2GB
                munmap(mem, size);
            }
            else
            {
                return mem;
            }
        }
    }
#endif

    // look for unused memory up to 2GB from pointer_offset in memory maps and try mapping memory there
    f = fopen("/proc/self/maps", "rb");
    if (f == NULL)
    {
        char mapname[32];
        sprintf(mapname, "/proc/%" PRIuMAX "/map", (uintmax_t)getpid());
        f = fopen(mapname, "rb");
        if (f == NULL) return NULL;
    }

    page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) page_size = 4096;

    mem = NULL;
    num0 = (pointer_offset + 1024*1024 + 65536 + (page_size - 1)) & ~(uintptr_t)(page_size - 1);
    while (num0 < pointer_offset + UINT64_C(0x80000000))
    {
        num_matches = fscanf(f, "%" SCNxMAX "%*[ -]%" SCNxMAX " %*[^\n^\r]%*[\n\r]", &num1, &num2);
        if ((num_matches == EOF) || (num_matches < 2)) break;

        // num1-num2 block is used
        // num0-num1 block is not used

        if (num1 > pointer_offset + UINT64_C(0x80000000))
        {
            num1 = pointer_offset + UINT64_C(0x80000000);
        }

        // num0-num1 block is below 2GB

        if ((num0 < num1) && (num1 - num0 >= size))
        {
            // try using memory at the end of the block
            start = (void *)((num1 - size) & ~(uintptr_t)(page_size - 1));
            if ((size & (HEAPBLOCK_BASE_SIZE - 1)) == 0)
            {
                start_aligned = (void *)(((uintptr_t)start) & ~(uintptr_t)(HEAPBLOCK_BASE_SIZE - 1));
                if ((uintptr_t)start_aligned >= num0 + HEAPBLOCK_BASE_SIZE) start = start_aligned;
            }
            if (start != (void *)num0)
            {
                mem = mmap(start, size, prot, MAP_FIXED_NOREPLACE | flags, -1, 0);
                if (mem == start) break;
                if (mem != MAP_FAILED)
                {
                    munmap(start, size);
                    goto error2;
                }
            }

            // try using memory at the start of the block
            start = (void *)num0;
            if ((size & (HEAPBLOCK_BASE_SIZE - 1)) == 0)
            {
                start_aligned = (void *)(((HEAPBLOCK_BASE_SIZE - 1) + (uintptr_t)start) & ~(uintptr_t)(HEAPBLOCK_BASE_SIZE - 1));
                if (size + (uintptr_t)start_aligned <= num1) start = start_aligned;
            }
            mem = mmap(start, size, prot, MAP_FIXED_NOREPLACE | flags, -1, 0);
            if (mem == start) break;
            if (mem != MAP_FAILED)
            {
                munmap(start, size);
                goto error2;
            }

            mem = NULL;
        }

        if (num2 > num0)
        {
            num0 = num2;
        }
    }

    fclose(f);

    if (mem != NULL) return mem;

    if ((num0 < pointer_offset + UINT64_C(0x80000000)) && (num0 + size <= pointer_offset + UINT64_C(0x80000000)))
    {
        // try using memory at the end of 2GB
        start = (void *)((pointer_offset + UINT64_C(0x80000000) - size) & ~(uintptr_t)(page_size - 1));
        if ((size & (HEAPBLOCK_BASE_SIZE - 1)) == 0)
        {
            start_aligned = (void *)(((uintptr_t)start) & ~(uintptr_t)(HEAPBLOCK_BASE_SIZE - 1));
            if ((uintptr_t)start_aligned >= num0 + HEAPBLOCK_BASE_SIZE) start = start_aligned;
        }
        if (start != (void *)num0)
        {
            mem = mmap(start, size, prot, MAP_FIXED_NOREPLACE | flags, -1, 0);
            if (mem == start) return mem;
            if (mem != MAP_FAILED)
            {
                munmap(start, size);
                goto error1;
            }
        }

        // try using memory after the end of the last block
        start = (void *)num0;
        if ((size & (HEAPBLOCK_BASE_SIZE - 1)) == 0)
        {
            start_aligned = (void *)(((HEAPBLOCK_BASE_SIZE - 1) + (uintptr_t)start) & ~(uintptr_t)(HEAPBLOCK_BASE_SIZE - 1));
            if (size + (uintptr_t)start_aligned <= pointer_offset + UINT64_C(0x80000000)) start = start_aligned;
        }
        mem = mmap(start, size, prot, MAP_FIXED_NOREPLACE | flags, -1, 0);
        if (mem == start) return mem;
        if (mem != MAP_FAILED)
        {
            munmap(start, size);
            goto error1;
        }
    }

    return NULL;

error2:
    fclose(f);

error1:
    fprintf(stderr, "Error: memory mapped at different address\n");
    return NULL;
#endif
}

void unmap_memory_32bit(void *mem, unsigned int size)
{
    if (mem != NULL && size != 0)
    {
#ifdef _WIN32
        VirtualFree(mem, 0, MEM_RELEASE);
#else
        munmap(mem, size);
#endif
    }
}


#ifdef PTROFS_64BIT

int initialize_pointer_offset(void)
{
#ifdef _WIN32
    HMODULE hModule;
    uint64_t maddr, min_length, free_offset, free_length;
    void *mem;
    SYSTEM_INFO sinfo;
    MEMORY_BASIC_INFORMATION minfo;

    pointer_offset = 0;

    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&ptr_initialize_pointers, &hModule))
    {
        return 1;
    }

    if (hModule == NULL)
    {
        return 2;
    }

    GetSystemInfo(&sinfo);

    min_length = (1024*1024 + 65536 + (sinfo.dwAllocationGranularity - 1)) & ~(sinfo.dwAllocationGranularity - 1);

    if ((uintptr_t)hModule <= 2*min_length)
    {
        pointer_reserved_length = 0;
        if (ptr_initialize_pointers != NULL)
        {
            ptr_initialize_pointers(pointer_offset);
        }
        return 0;
    }

    min_length = (1024*1024 + 65536 + (sinfo.dwPageSize - 1)) & ~(sinfo.dwPageSize - 1);

    maddr = free_offset = 0;
    for (;;)
    {
        if (0 == VirtualQuery((void *)maddr, &minfo, sizeof(minfo))) return 3;

        if (minfo.State == MEM_FREE)
        {
            if ((uintptr_t)minfo.BaseAddress <= (((minfo.RegionSize + (uintptr_t)minfo.BaseAddress) - min_length) & ~(uint64_t)(sinfo.dwAllocationGranularity - 1)))
            {
                free_offset = ((minfo.RegionSize + (uintptr_t)minfo.BaseAddress) - min_length) & ~(uint64_t)(sinfo.dwAllocationGranularity - 1);
                free_length = (minfo.RegionSize + (uintptr_t)minfo.BaseAddress) - free_offset;
            }
        }

        maddr = ((minfo.RegionSize + (uintptr_t)minfo.BaseAddress) + (sinfo.dwAllocationGranularity - 1)) & ~(uint64_t)(sinfo.dwAllocationGranularity - 1);

        if ((uintptr_t)hModule < maddr)
        {
            if (free_offset == 0) break;

            pointer_offset = free_offset;
            pointer_reserved_length = 0;

            // try reserving memory
            mem = VirtualAlloc((void *)free_offset, free_length, MEM_RESERVE, PAGE_NOACCESS);
            if (mem == (void *)free_offset)
            {
                pointer_reserved_length = (unsigned int)free_length;
            }
            else
            {
                if (mem != NULL) VirtualFree(mem, 0, MEM_RELEASE);
            }

            if (ptr_initialize_pointers != NULL)
            {
                ptr_initialize_pointers(pointer_offset);
            }
            return 0;
        }
    }

    return 4;
#elif defined(__APPLE__)
    Dl_info info;
    void *mem;
    long page_size;
    mach_port_t task;
    mach_vm_address_t region_address, maddr, free_offset;
    mach_vm_size_t region_size, min_length;
    vm_region_basic_info_data_64_t rinfo;
    mach_msg_type_number_t count;
    mach_port_t object_name;

    pointer_offset = 0;

    if (!dladdr(&ptr_initialize_pointers, &info))
    {
        return 1;
    }

    if (info.dli_fbase == NULL)
    {
        return 2;
    }

    page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) page_size = 4096;

    min_length = (1024*1024 + 65536 + (page_size - 1)) & ~(page_size - 1);

    if ((uintptr_t)info.dli_fbase <= 2*min_length)
    {
        pointer_reserved_length = 0;
        if (ptr_initialize_pointers != NULL)
        {
            ptr_initialize_pointers(pointer_offset);
        }
        return 0;
    }

    task = current_task();

    maddr = free_offset = 0;
    for (;;)
    {
        region_address = maddr;
        count = VM_REGION_BASIC_INFO_COUNT_64;
        if (KERN_SUCCESS != mach_vm_region(task, &region_address, &region_size, VM_REGION_BASIC_INFO_64, (vm_region_info_t) &rinfo, &count, &object_name)) break;

        if (region_address - maddr >= min_length)
        {
            free_offset = region_address - min_length;
        }

        maddr = region_address + region_size;

        if ((uintptr_t)info.dli_fbase < maddr)
        {
            if (free_offset == 0) break;

            pointer_offset = free_offset;
            pointer_reserved_length = 0;

            // try reserving memory
            mem = mmap((void *)free_offset, min_length, PROT_NONE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
            if (mem == (void *)free_offset)
            {
                pointer_reserved_length = (unsigned int)min_length;
            }
            else
            {
                if (mem != MAP_FAILED) munmap(mem, min_length);
            }

            if (ptr_initialize_pointers != NULL)
            {
                ptr_initialize_pointers(pointer_offset);
            }
            return 0;
        }
    }

    return 4;
#else
    Dl_info info;
    void *mem;
    FILE *f;
    long page_size;
    int num_matches, flags;
    uintmax_t num0, num1, num2, min_length, free_offset;

    pointer_offset = 0;

    if (!dladdr(&ptr_initialize_pointers, &info))
    {
        return 1;
    }

    if (info.dli_fbase == NULL)
    {
        return 2;
    }

    f = fopen("/proc/self/maps", "rb");
    if (f == NULL)
    {
        char mapname[32];
        sprintf(mapname, "/proc/%" PRIuMAX "/map", (uintmax_t)getpid());
        f = fopen(mapname, "rb");
        if (f == NULL) return 3;
    }

    page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) page_size = 4096;

    min_length = (1024*1024 + 65536 + (page_size - 1)) & ~(page_size - 1);

    if ((uintptr_t)info.dli_fbase <= 2*min_length)
    {
        pointer_reserved_length = 0;
        if (ptr_initialize_pointers != NULL)
        {
            ptr_initialize_pointers(pointer_offset);
        }
        return 0;
    }

    num0 = free_offset = 0;
    for (;;)
    {
        num_matches = fscanf(f, "%" SCNxMAX "%*[ -]%" SCNxMAX " %*[^\n^\r]%*[\n\r]", &num1, &num2);
        if ((num_matches == EOF) || (num_matches < 2)) break;

        // [num1 .. num2) block is used
        // [num0 .. num1) block is not used

        if (num1 - num0 >= min_length)
        {
            free_offset = num1 - min_length;
        }

        if (num2 > num0)
        {
            num0 = num2;
        }

        if ((uintptr_t)info.dli_fbase < num2)
        {
            if (free_offset == 0) break;

            pointer_offset = free_offset;
            pointer_reserved_length = 0;

            // try reserving memory
#if !defined(MAP_NORESERVE) && defined(MAP_GUARD)
            flags = MAP_GUARD;
#else
            flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;
#endif

            mem = mmap((void *)free_offset, min_length, PROT_NONE, MAP_FIXED_NOREPLACE | flags, -1, 0);
            if (mem == (void *)free_offset)
            {
                pointer_reserved_length = (unsigned int)min_length;
#ifdef PR_SET_VMA_ANON_NAME
                prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, free_offset, min_length, "base32");
#endif
            }
            else
            {
                if (mem != MAP_FAILED) munmap(mem, min_length);
            }

            fclose(f);
            if (ptr_initialize_pointers != NULL)
            {
                ptr_initialize_pointers(pointer_offset);
            }
            return 0;
        }
    }

    fclose(f);
    return 4;
#endif
}

#endif /* PTROFS_64BIT */

#endif /* __cplusplus */

