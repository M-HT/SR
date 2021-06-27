;;
;;  Copyright (C) 2019-2021 Roman Pauer
;;
;;  Permission is hereby granted, free of charge, to any person obtaining a copy of
;;  this software and associated documentation files (the "Software"), to deal in
;;  the Software without restriction, including without limitation the rights to
;;  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
;;  of the Software, and to permit persons to whom the Software is furnished to do
;;  so, subject to the following conditions:
;;
;;  The above copyright notice and this permission notice shall be included in all
;;  copies or substantial portions of the Software.
;;
;;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;;  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;;  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;;  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;;  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;;  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;;  SOFTWARE.
;;

%include "asm-calls.inc"

%ifidn __OUTPUT_FORMAT__, win32
    %define memcmp_c _memcmp_c
    %define memcpy_c _memcpy_c
    %define memset_c _memset_c

    %define strcat_c _strcat_c
    %define strcmp_c _strcmp_c
    %define strcpy_c _strcpy_c
    %define strlen_c _strlen_c
    %define strncpy_c _strncpy_c
    %define _strnicmp_c __strnicmp_c
    %define strstr_c _strstr_c

    %define _alloca_probe_c __alloca_probe_c

    %define printf2_c _printf2_c
    %define sprintf2_c _sprintf2_c

    %define _ftol_c __ftol_c

    %define ms_srand_c _ms_srand_c
    %define ms_rand_c _ms_rand_c
    %define wc_rand_c _wc_rand_c

    %define atoi_c _atoi_c
    %define atol_c _atol_c
    %define _ltoa_c __ltoa_c

    %define isalnum_c _isalnum_c

    %define _except_handler3_c __except_handler3_c

    %define sync_c _sync_c
%endif

extern memcmp_c
extern memcpy_c
extern memset_c

extern strcat_c
extern strcmp_c
extern strcpy_c
extern strlen_c
extern strncpy_c
extern _strnicmp_c
extern strstr_c

extern _alloca_probe_c

extern printf2_c
extern sprintf2_c

extern _ftol_c

extern ms_srand_c
extern ms_rand_c
extern wc_rand_c

extern atoi_c
extern atol_c
extern _ltoa_c

extern isalnum_c

extern _except_handler3_c

extern sync_c


global ms_memcmp_asm2c
global ms_memcpy_asm2c
global ms_memset_asm2c
global wc_memset_asm2c

global ms_strcat_asm2c
global ms_strcmp_asm2c
global ms_strcpy_asm2c
global ms_strlen_asm2c
global ms_strncpy_asm2c
global ms__strnicmp_asm2c
global ms_strstr_asm2c
global wc_strcmp_asm2c
global wc_strcpy_asm2c

global ms__alloca_probe_asm2c

global ms_printf_asm2c
global wc_sprintf_asm2c

global ms__ftol_asm2c

global ms_srand_asm2c
global ms_rand_asm2c
global wc_rand_asm2c

global ms_atoi_asm2c
global ms_atol_asm2c
global ms__ltoa_asm2c

global ms_isalnum_asm2c

global ms__except_handler3_asm2c

global sync_asm2c


%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
ms_memcmp_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const void *s2
; [esp +   4] = const void *s1
; [esp      ] = return address

        Call_Asm_Stack3 memcmp_c

        retn

; end procedure ms_memcmp_asm2c


align 16
ms_memcpy_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const void *src
; [esp +   4] = void *dest
; [esp      ] = return address

        Call_Asm_Stack3 memcpy_c

        retn

; end procedure ms_memcpy_asm2c


align 16
ms_memset_asm2c:
wc_memset_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = int32_t c
; [esp +   4] = void *s
; [esp      ] = return address

        Call_Asm_Stack3 memset_c

        retn

; end procedure ms_memset_asm2c


align 16
ms_strcat_asm2c:

; [esp + 2*4] = const char *src
; [esp +   4] = char *dest
; [esp      ] = return address

        Call_Asm_Stack2 strcat_c

        retn

; end procedure ms_strcat_asm2c


align 16
ms_strcmp_asm2c:
wc_strcmp_asm2c:

; [esp + 2*4] = const char *s2
; [esp +   4] = const char *s1
; [esp      ] = return address

        Call_Asm_Stack2 strcmp_c

        retn

; end procedure ms_strcmp_asm2c


align 16
ms_strcpy_asm2c:
wc_strcpy_asm2c:

; [esp + 2*4] = const char *src
; [esp +   4] = char *dest
; [esp      ] = return address

        Call_Asm_Stack2 strcpy_c

        retn

; end procedure ms_strcpy_asm2c


align 16
ms_strlen_asm2c:

; [esp +   4] = const char *s
; [esp      ] = return address

        Call_Asm_Stack1 strlen_c

        retn

; end procedure ms_strlen_asm2c


align 16
ms_strncpy_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const char *src
; [esp +   4] = char *dest
; [esp      ] = return address

        Call_Asm_Stack3 strncpy_c

        retn

; end procedure ms_strncpy_asm2c


align 16
ms__strnicmp_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const char *s2
; [esp +   4] = const char *s1
; [esp      ] = return address

        Call_Asm_Stack3 _strnicmp_c

        retn

; end procedure ms__strnicmp_asm2c


align 16
ms_strstr_asm2c:

; [esp + 2*4] = const char *needle
; [esp +   4] = const char *haystack
; [esp      ] = return address

        Call_Asm_Stack2 strstr_c

        retn

; end procedure ms_strstr_asm2c


align 16
ms__alloca_probe_asm2c:

; eax = uint32_t size
; [esp] = return address

    ; save ecx and edx to stack
        push ecx
        push edx

    ; remember original esp value
        mov ecx, esp
    ; align stack to 16 bytes
        and esp, 0FFFFFFF0h
        sub esp, byte 4*4

    ; save original esp value on stack
        mov [esp + 3*4], ecx
    ; put function argument to stack
        mov [esp], eax

        call _alloca_probe_c

    ; restore function argument to ecx
        mov ecx, [esp]

    ; restore original esp value from stack
        mov esp, [esp + 3*4]

    ; restore original edx value from stack
        pop edx

        lea eax, [esp+4]
        sub eax, ecx

    ; restore original ecx value from stack
        pop ecx

        xchg esp, eax
        mov eax, [eax]
        mov [esp], eax
        retn

; end procedure ms__alloca_probe_asm2c


align 16
ms_printf_asm2c:

; [esp + 2*4] = ...
; [esp +   4] = const char *format
; [esp      ] = return address

        Call_Asm_VariableStack1 printf2_c

        retn

; end procedure ms_printf_asm2c


align 16
wc_sprintf_asm2c:

; [esp + 3*4] = ...
; [esp + 2*4] = const char *format
; [esp +   4] = char *str
; [esp      ] = return address

        Call_Asm_VariableStack2 sprintf2_c

        retn

; end procedure wc_sprintf_asm2c


align 16
ms__ftol_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Int _ftol_c

        retn

; end procedure ms__ftol_asm2c


align 16
ms_srand_asm2c:

; [esp + 4] = uint32_t seed
; [esp    ] = return address

        Call_Asm_Stack1 ms_srand_c

        retn

; end procedure ms_srand_asm2c


align 16
ms_rand_asm2c:

; [esp] = return address

        Call_Asm_Stack0 ms_rand_c

        retn

; end procedure ms_rand_asm2c


align 16
wc_rand_asm2c:

; [esp] = return address

        Call_Asm_Stack0 wc_rand_c

        retn

; end procedure wc_rand_asm2c


align 16
ms_atoi_asm2c:

; [esp + 4] = const char *nptr
; [esp    ] = return address

        Call_Asm_Stack1 atoi_c

        retn

; end procedure ms_atoi_asm2c


align 16
ms_atol_asm2c:

; [esp + 4] = const char *nptr
; [esp    ] = return address

        Call_Asm_Stack1 atol_c

        retn

; end procedure ms_atol_asm2c


align 16
ms__ltoa_asm2c:

; [esp + 3*4] = int radix
; [esp + 2*4] = char *buffer
; [esp +   4] = long value
; [esp      ] = return address

        Call_Asm_Stack3 _ltoa_c

        retn

; end procedure ms__ltoa_asm2c


align 16
ms_isalnum_asm2c:

; [esp +   4] = int c
; [esp      ] = return address

        Call_Asm_Stack1 isalnum_c

        retn

; end procedure ms_isalnum_asm2c


align 16
ms__except_handler3_asm2c:

; [esp + 4*4] = PEXCEPTION_REGISTRATION dispatcher
; [esp + 3*4] = PCONTEXT context
; [esp + 2*4] = PEXCEPTION_REGISTRATION registration
; [esp +   4] = PEXCEPTION_RECORD exception_record
; [esp      ] = return address

        Call_Asm_Stack4 _except_handler3_c

        retn

; end procedure ms__except_handler3_asm2c


align 16
sync_asm2c:

; [esp] = return address

        Call_Asm_Stack0 sync_c

        retn

; end procedure sync_asm2c


