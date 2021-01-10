/*
 * Copyright (C) 2021  Roman Pauer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __HQX_PLATFORM_H_
#define __HQX_PLATFORM_H_

#if ( \
    defined(__ARM_ARCH_6__) || \
    defined(__ARM_ARCH_6J__) || \
    defined(__ARM_ARCH_6K__) || \
    defined(__ARM_ARCH_6Z__) || \
    defined(__ARM_ARCH_6ZK__) || \
    defined(__ARM_ARCH_6T2__) || \
    defined(__ARM_ARCH_7__) || \
    defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7R__) || \
    defined(__ARM_ARCH_7M__) || \
    defined(__ARM_ARCH_7S__) || \
    (defined(_M_ARM) && (_M_ARM >= 6)) || \
    (defined(__TARGET_ARCH_ARM) && (__TARGET_ARCH_ARM >= 6)) || \
    (defined(__TARGET_ARCH_THUMB) && (__TARGET_ARCH_THUMB >= 3)) \
)
    #define ARMV6 1
#else
    #undef ARMV6
#endif

#if ( \
    defined(__ARM_ARCH_7__) || \
    defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7R__) || \
    defined(__ARM_ARCH_7M__) || \
    defined(__ARM_ARCH_7S__) || \
    (defined(_M_ARM) && (_M_ARM >= 7)) || \
    (defined(__TARGET_ARCH_ARM) && (__TARGET_ARCH_ARM >= 7)) || \
    (defined(__TARGET_ARCH_THUMB) && (__TARGET_ARCH_THUMB >= 4)) \
)
    #define ARMV7 1
#else
    #undef ARMV7
#endif

#if ( \
    defined(__aarch64__) \
)
    #define ARMV8 1
#else
    #undef ARMV8
#endif

#if ( \
    defined(__i386) || \
    defined(_M_IX86) || \
    defined(_X86_) || \
    defined(__THW_INTEL__) || \
    defined(__I86__) || \
    defined(__INTEL__) || \
    defined(__386) \
)
    #define X86SSE2 1
#else
    #undef X86SSE2
#endif

#if ( \
    defined(__amd64__) || \
    defined(__amd64) || \
    defined(__x86_64__) || \
    defined(__x86_64) || \
    defined(_M_X64) || \
    defined(_M_AMD64) \
)
    #define X64SSE2 1
#else
    #undef X64SSE2
#endif


#if defined(X86SSE2) || defined(X64SSE2)
#define ALIGNMENT 16
#else
#define ALIGNMENT 8
#endif
#define ALIGNPTR(ptr) ((void *) ( (((uintptr_t) (ptr)) + (ALIGNMENT - 1)) & ~((uintptr_t)(ALIGNMENT - 1)) ))


#endif

