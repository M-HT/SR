/**
 *
 *  Copyright (C) 2022-2025 Roman Pauer
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

#if !defined(_PTR32_H_INCLUDED_)
#define _PTR32_H_INCLUDED_

#include <stdint.h>

#ifdef __cplusplus

#include <cstddef>
#include <type_traits>

template<class T, int A=4>
class ptr32_t
{
private:
    uint32_t value __attribute__ ((packed)) __attribute__ ((aligned (A)));

public:
    ptr32_t<T,A> ()
    {
    }

    ptr32_t<T,A> (void *p)
    {
        value = (uint32_t)(uintptr_t)p;
    }

    template<class Q = T, typename = typename std::enable_if<!std::is_void<Q>::value>::type>
    ptr32_t<T,A> (T* p) // constructor for T* if T isn't void
    {
        value = (uint32_t)(uintptr_t)p;
    }

    ptr32_t<T,A> (std::nullptr_t)
    {
        value = 0;
    }

    ptr32_t<T,A> (uintptr_t p)
    {
        value = p;
    }

// Unary operators

    ptr32_t<T,A>& operator ++ () // Overload ++ when used as prefix
    {
        value += sizeof(T);
        return *this;
    }

    ptr32_t<T,A> operator ++ (int) // Overload ++ when used as postfix
    {
        ptr32_t<T,A> temp;

        temp.value = value;
        value += sizeof(T);

        return temp;
    }

    ptr32_t<T,A>& operator -- () // Overload -- when used as prefix
    {
        value -= sizeof(T);
        return *this;
    }

    ptr32_t<T,A> operator -- (int) // Overload -- when used as postfix
    {
        ptr32_t<T,A> temp;

        temp.value = value;
        value -= sizeof(T);

        return temp;
    }

    bool operator ! () const // Overload !
    {
        return value == 0;
    }

    operator uintptr_t () const // Overload uintptr_t (cast)
    {
        return (uintptr_t)value;
    }

    operator void* () const // Overload void* (cast)
    {
        return (void*)(uintptr_t)value;
    }

    template<class Q = T, typename = typename std::enable_if<!std::is_void<Q>::value>::type>
    operator T* () const // Overload T* (cast) if T isn't void
    {
        return (T*)(uintptr_t)value;
    }

// Member access

    typename std::add_lvalue_reference<T>::type operator * () // Overload * (indirection)
    {
        return *(T*)(uintptr_t)value;
    }

    typename std::add_lvalue_reference<T>::type operator [] (int32_t index) const // Overload []
    {
        return ((T*)(uintptr_t)value)[index];
    }

    typename std::add_lvalue_reference<T>::type operator [] (uint32_t index) const // Overload []
    {
        return ((T*)(uintptr_t)value)[index];
    }

    typename std::add_lvalue_reference<T>::type operator [] (int64_t index) const // Overload []
    {
        return ((T*)(uintptr_t)value)[index];
    }

    typename std::add_lvalue_reference<T>::type operator [] (uint64_t index) const // Overload []
    {
        return ((T*)(uintptr_t)value)[index];
    }

    T* operator -> () const // Overload ->
    {
        return (T*)(uintptr_t)value;
    }

// Binary operators

    bool operator == (ptr32_t<T,A> const& p) const
    {
        return value == p.value;
    }

    bool operator == (void *p) const
    {
        return value == (uintptr_t)p;
    }

    template<class Q = T, typename = typename std::enable_if<!std::is_void<Q>::value>::type>
    bool operator == (T *p) const // Overload == if T isn't void
    {
        return value == (uintptr_t)p;
    }

    bool operator == (std::nullptr_t) const
    {
        return value == 0;
    }

    bool operator != (ptr32_t<T,A> const& p) const
    {
        return value != p.value;
    }

    bool operator != (void *p) const
    {
        return value != (uintptr_t)p;
    }

    template<class Q = T, typename = typename std::enable_if<!std::is_void<Q>::value>::type>
    bool operator != (T *p) const // Overload != if T isn't void
    {
        return value != (uintptr_t)p;
    }

    bool operator != (std::nullptr_t) const
    {
        return value != 0;
    }

    bool operator < (ptr32_t<T,A> const& p) const
    {
        return value < p.value;
    }

    bool operator < (T *p) const
    {
        return value < (uintptr_t)p;
    }

    bool operator > (ptr32_t<T,A> const& p) const
    {
        return value > p.value;
    }

    bool operator > (T *p) const
    {
        return value > (uintptr_t)p;
    }

    bool operator <= (ptr32_t<T,A> const& p) const
    {
        return value <= p.value;
    }

    bool operator <= (T *p) const
    {
        return value < (uintptr_t)p;
    }

    bool operator >= (ptr32_t<T,A> const& p) const
    {
        return value >= p.value;
    }

    bool operator >= (T *p) const
    {
        return value >= (uintptr_t)p;
    }

    ptr32_t<T,A> operator + (int32_t n) const
    {
        ptr32_t<T,A> temp;

        temp.value = value + n * sizeof(T);

        return temp;
    }

    ptr32_t<T,A> operator + (uint32_t n) const
    {
        ptr32_t<T,A> temp;

        temp.value = value + n * sizeof(T);

        return temp;
    }

    ptr32_t<T,A> operator + (int64_t n) const
    {
        ptr32_t<T,A> temp;

        temp.value = value + n * sizeof(T);

        return temp;
    }

    ptr32_t<T,A> operator + (uint64_t n) const
    {
        ptr32_t<T,A> temp;

        temp.value = value + n * sizeof(T);

        return temp;
    }

    ptr32_t<T,A> operator - (int32_t n) const
    {
        ptr32_t<T,A> temp;

        temp.value = value - n * sizeof(T);

        return temp;
    }

    ptr32_t<T,A> operator - (uint32_t n) const
    {
        ptr32_t<T,A> temp;

        temp.value = value - n * sizeof(T);

        return temp;
    }

    ptr32_t<T,A> operator - (int64_t n) const
    {
        ptr32_t<T,A> temp;

        temp.value = value - n * sizeof(T);

        return temp;
    }

    ptr32_t<T,A> operator - (uint64_t n) const
    {
        ptr32_t<T,A> temp;

        temp.value = value - n * sizeof(T);

        return temp;
    }

    ptr32_t<T,A>& operator += (int32_t n)
    {
        value += n * sizeof(T);
        return *this;
    }

    ptr32_t<T,A>& operator += (uint32_t n)
    {
        value += n * sizeof(T);
        return *this;
    }

    ptr32_t<T,A>& operator += (int64_t n)
    {
        value += n * sizeof(T);
        return *this;
    }

    ptr32_t<T,A>& operator += (uint64_t n)
    {
        value += n * sizeof(T);
        return *this;
    }

    ptr32_t<T,A>& operator -= (int32_t n)
    {
        value -= n * sizeof(T);
        return *this;
    }

    ptr32_t<T,A>& operator -= (uint32_t n)
    {
        value -= n * sizeof(T);
        return *this;
    }

    ptr32_t<T,A>& operator -= (int64_t n)
    {
        value -= n * sizeof(T);
        return *this;
    }

    ptr32_t<T,A>& operator -= (uint64_t n)
    {
        value -= n * sizeof(T);
        return *this;
    }

    bool operator && (ptr32_t const& p) const
    {
        return (value & p.value) != 0;
    }

    bool operator && (bool b) const
    {
        return (value != 0) && b;
    }

    bool operator || (ptr32_t const& p) const
    {
        return (value | p.value) != 0;
    }

    bool operator || (bool b) const
    {
        return (value != 0) || b;
    }

};

#define PTR32(t) ptr32_t<t>
#define PTR32_ALIGN(t,a) ptr32_t<t,a>

#undef NULL
#define NULL nullptr

#else

#define PTR32(t) t*
#if defined(_MSC_VER)
#define PTR32_ALIGN(t,a) t*
#else
#define PTR32_ALIGN(t,a) t* __attribute__ ((aligned (a)))
#endif

#endif

/**
 * Compilation
 * -----------
 *
 * - for 64-bits, the program must be compiled as c++ code
 * - for 32-bits, the program should be compiled as c code (compiling as c++ should work)
 *
 * Variables
 * ---------
 *
 * - variables defined in asm code which are used in c code:
 * -- don't define them as pointers, but as PTR32(base type)
 * -- don't define them as types with inexact size, but as types with exact size
 * -- instead of variable definitions like this:
 *    extern void *asm_var1;
 *    extern int asm_var2;
 * -- use variable definitions like this:
 *    extern PTR32(void) asm_var1;
 *    extern int32_t asm_var2;
 *
 * - variables defined in c code which are used in asm code:
 * -- the same rules apply
 *
 * Structure/Union members
 * -----------------
 *
 * - structure/union members in structures/unions which are used in asm code:
 * -- the same rules apply as for variables
 * -- the size of structures/unions and the alignment of structure/union members in c code must be the same as in asm code
 * -- the exceptions are opaque structures/unions (opaque parts of structures/unions)
 * --- which means the structure/union size is determined in c code and the structure/union (part of the structure/union) isn't used in asm code
 *
 * Function parameters
 * -------------------
 *
 * - function parameters in functions defined in c code which are called from asm code:
 * -- can be defined as pointers, but only one level pointers (i.e. not pointer to pointer)
 * -- don't define them as pointers to types with inexact size, but as pointers to types with exact size
 * -- for two (or more) level pointers, the same rules apply as for variables
 * -- don't define non-pointer parameters as types with inexact size, but as types with exact size
 * -- pass non-pointer parameters larger than 32 bits either as two 32-bit parameters or by reference
 * -- define non-pointer parameters smaller than 32 bits as:
 * --- 32-bit (int32_t/uint32_t) and the size conversion must be handled inside function
 * --- 8/16-bit and the value must be sign or zero extended to 32 bits by callers in asm code
 * -- instead of parameter definitions like this:
 *    void **param1
 *    int *param2
 *    int param3
 * -- use parameter definitions like this:
 *    PTR32(void) *param1
 *    int32_t *param2
 *    int32_t param3
 *
 * - functions with ... or va_list parameter are handled specially in llasm
 * -- the .../va_list parameter is defined as uint32_t *
 *
 * Function return values
 * ----------------------
 *
 * - function return values in functions defined in c code which are called from asm code:
 * -- for pointer return values, the same rules apply as for function parameters
 * -- for non-pointer return values, types with exact size are preferred, but not required
 * -- return non-pointer return values larger than 32 bits by reference as function parameter
 * -- return non-pointer return values smaller than 32 bits as:
 * --- 32-bit (int32_t/uint32_t) and the value must be sign or zero extended to 32 bits inside function
 * --- 8/16-bit and the value must not be expected by callers in asm code to have been sign or zero extended to 32 bits
 */

#endif /* _PTR32_H_INCLUDED_ */

