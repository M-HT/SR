//part of static recompiler -- do not edit

/**
 *
 *  Copyright (C) 2019-2026 Roman Pauer
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

#include <stdint.h>

typedef struct {
    uint32_t _eax, _ecx, _edx, _ebx, _esp, _ebp, _esi, _edi, _eflags;
    uint32_t _st_top, _st_sw_cond, _st_cw;
    double _st[8];
#ifdef PTROFS_64BIT
    uint64_t _pointer_offset;
#else
    uint64_t _reserved1;
#endif
    int64_t _st_result;
    void *stack_bottom, *stack_top;
} _cpu;

#define CPU _cpu *cpu

#define eax cpu->_eax
#define ebx cpu->_ebx
#define ecx cpu->_ecx
#define edx cpu->_edx
#define esi cpu->_esi
#define edi cpu->_edi
#define ebp cpu->_ebp
#define esp cpu->_esp
#define eflags cpu->_eflags

#define CF_SHIFT 0
#define PF_SHIFT 2
#define AF_SHIFT 4
#define ZF_SHIFT 6
#define SF_SHIFT 7
#define OF_SHIFT 11

#define IF_SHIFT 9
#define DF_SHIFT 10

#define CF 0x01
#define PF 0x04
#define AF 0x10
#define ZF 0x40
#define SF 0x80
#define OF 0x0800

#define IF 0x0200
#define DF 0x0400

#ifdef PTROFS_64BIT

#define REG2PTR(x) ((void *)((uintptr_t)(x) + cpu->_pointer_offset))
#define PTR2REG(x) ((uint32_t)((uintptr_t)(x) - cpu->_pointer_offset))

#else

#define REG2PTR(x) ((void *)(uintptr_t)(x))
#define PTR2REG(x) ((uint32_t)(uintptr_t)(x))

#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#define EXTERNCVAR extern "C"
#else
#define EXTERNC
#define EXTERNCVAR extern
#endif

#if defined(__GNUC__) && (defined(__i386) || (defined(__x86_64) && defined(_WIN32)))
    #define CCALL __attribute__ ((__cdecl__))
#elif defined(_MSC_VER)
    #define CCALL __cdecl
#else
    #define CCALL
#endif

// *********************************************************************

#if defined(_MSC_VER)

static __inline uint32_t unaligned_read_32(void *adr)
{
    uint32_t __unaligned *uptr = (uint32_t __unaligned *)adr;
    return *uptr;
}

static __inline void unaligned_write_32(void *adr, uint32_t val)
{
    uint32_t __unaligned *uptr = (uint32_t __unaligned *)adr;
    *uptr = val;
}

#else

typedef struct {
    uint32_t u;
} __attribute__((__packed__)) _unaligned32;

static inline uint32_t unaligned_read_32(void *adr)
{
    _unaligned32 *uptr = (_unaligned32 *)adr;
    return uptr->u;
}

static inline void unaligned_write_32(void *adr, uint32_t val)
{
    _unaligned32 *uptr = (_unaligned32 *)adr;
    uptr->u = val;
}

#endif

#define UNALIGNED_READ_32(adr) (unaligned_read_32(REG2PTR(adr)))
#define UNALIGNED_WRITE_32(adr, val) unaligned_write_32(REG2PTR(adr), (val));

// *********************************************************************

#if defined(_MSC_VER)

static __inline uint16_t unaligned_read_16(void *adr)
{
    uint16_t __unaligned *uptr = (uint16_t __unaligned *)adr;
    return *uptr;
}

static __inline void unaligned_write_16(void *adr, uint16_t val)
{
    uint16_t __unaligned *uptr = (uint16_t __unaligned *)adr;
    *uptr = val;
}

#else

typedef struct {
    uint16_t u;
} __attribute__((__packed__)) _unaligned16;

static inline uint16_t unaligned_read_16(void *adr)
{
    _unaligned16 *uptr = (_unaligned16 *)adr;
    return uptr->u;
}

static inline void unaligned_write_16(void *adr, uint16_t val)
{
    _unaligned16 *uptr = (_unaligned16 *)adr;
    uptr->u = val;
}

#endif

#define UNALIGNED_READ_16(adr) (unaligned_read_16(REG2PTR(adr)))
#define UNALIGNED_WRITE_16(adr, val) unaligned_write_16(REG2PTR(adr), (val));

