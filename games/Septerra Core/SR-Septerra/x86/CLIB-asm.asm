;;
;;  Copyright (C) 2019-2026 Roman Pauer
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
    %define memset_c _memset_c
    %define memcpy_c _memcpy_c

    %define _stricmp_c __stricmp_c
    %define strncpy_c _strncpy_c
    %define strncmp_c _strncmp_c
    %define strncat_c _strncat_c

    %define malloc_c _malloc_c
    %define free_c _free_c
    %define calloc_c _calloc_c
    %define _alloca_probe_c __alloca_probe_c

    %define atol_c _atol_c
    %define toupper_c _toupper_c

    %define sprintf2_c _sprintf2_c
    %define sscanf2_c _sscanf2_c

    %define system_c _system_c
    %define srand_c _srand_c
    %define rand_c _rand_c

    %define _aulldiv_c __aulldiv_c
    %define _alldiv_c __alldiv_c
    %define _time64_c __time64_c

    %define __report_gsfailure_c ___report_gsfailure_c

    %define _except_handler4_c __except_handler4_c

    %define _beginthread_c __beginthread_c

    %define sync_c _sync_c

    %define _ftol2_sse_c __ftol2_sse_c
    %define _ftol2_c __ftol2_c
    %define _ftol_c __ftol_c

    %define _CIcos_c __CIcos_c
    %define _CIsin_c __CIsin_c
    %define _CIatan2_c __CIatan2_c
    %define _CIsqrt_c __CIsqrt_c
    %define _CIfmod_c __CIfmod_c
    %define _CItan_c __CItan_c
    %define _CIpow_c __CIpow_c
%endif

extern memset_c
extern memcpy_c

extern _stricmp_c
extern strncpy_c
extern strncmp_c
extern strncat_c

extern malloc_c
extern free_c
extern calloc_c
extern _alloca_probe_c

extern atol_c
extern toupper_c

extern sprintf2_c
extern sscanf2_c

extern system_c
extern srand_c
extern rand_c

extern _aulldiv_c
extern _alldiv_c
extern _time64_c

extern __report_gsfailure_c

extern _except_handler4_c

extern _beginthread_c

extern sync_c

extern _ftol2_sse_c
extern _ftol2_c
extern _ftol_c

extern _CIcos_c
extern _CIsin_c
extern _CIatan2_c
extern _CIsqrt_c
extern _CIfmod_c
extern _CItan_c
extern _CIpow_c


global run_thread_asm
global _run_thread_asm

global memset_asm2c
global memcpy_asm2c

global _stricmp_asm2c
global strncpy_asm2c
global strncmp_asm2c
global strncat_asm2c

global malloc_asm2c
global free_asm2c
global calloc_asm2c
global _alloca_probe_asm2c

global atol_asm2c
global toupper_asm2c

global sprintf_asm2c
global sscanf_asm2c

global system_asm2c
global srand_asm2c
global rand_asm2c

global _aulldiv_asm2c
global _alldiv_asm2c
global _time64_asm2c

global _check_security_cookie_asm2c

global _except_handler4_asm2c

global _beginthread_asm2c

global sync_asm2c

global _ftol2_sse_asm2c
global _ftol2_asm2c
global _ftol_asm2c

global _CIcos_asm2c
global _CIsin_asm2c
global _CIatan2_asm2c
global _CIsqrt_asm2c
global _CIfmod_asm2c
global _CItan_asm2c
global _CIpow_asm2c


extern security_cookie_


%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
run_thread_asm:
_run_thread_asm:

; [esp + 2*4] = void(*start_address)(void *)
; [esp +   4] = void *arglist
; [esp      ] = return address

        jmp dword [esp + 2*4]

; end procedure run_thread_asm


align 16
memset_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = int32_t c
; [esp +   4] = void *s
; [esp      ] = return address

        Call_Asm_Stack3 memset_c

        retn

; end procedure memset_asm2c


align 16
memcpy_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const void *src
; [esp +   4] = void *dest
; [esp      ] = return address

        Call_Asm_Stack3 memcpy_c

        retn

; end procedure memcpy_asm2c


align 16
_stricmp_asm2c:

; [esp + 2*4] = const char *s2
; [esp +   4] = const char *s1
; [esp      ] = return address

        Call_Asm_Stack2 _stricmp_c

        retn

; end procedure _stricmp_asm2c


align 16
strncpy_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const char *src
; [esp +   4] = char *dest
; [esp      ] = return address

        Call_Asm_Stack3 strncpy_c

        retn

; end procedure strncpy_asm2c


align 16
strncmp_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const char *s2
; [esp +   4] = const char *s1
; [esp      ] = return address

        Call_Asm_Stack3 strncmp_c

        retn

; end procedure strncmp_asm2c


align 16
strncat_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const char *src
; [esp +   4] = char *dest
; [esp      ] = return address

        Call_Asm_Stack3 strncat_c

        retn

; end procedure strncat_asm2c


align 16
malloc_asm2c:

; [esp + 4] = uint32_t size
; [esp    ] = return address

        Call_Asm_Stack1 malloc_c

        retn

; end procedure malloc_asm2c

align 16
free_asm2c:

; [esp + 4] = void *ptr
; [esp    ] = return address

        Call_Asm_Stack1 free_c

        retn

; end procedure free_asm2c


align 16
calloc_asm2c:

; [esp + 2*4] = uint32_t size
; [esp +   4] = uint32_t nmemb
; [esp      ] = return address

        Call_Asm_Stack2 calloc_c

        retn

; end procedure calloc_asm2c


align 16
_alloca_probe_asm2c:

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

; end procedure _alloca_probe_asm2c


align 16
atol_asm2c:

; [esp + 4] = const char *nptr
; [esp    ] = return address

        Call_Asm_Stack1 atol_c

        retn

; end procedure atol_asm2c


align 16
toupper_asm2c:

; [esp + 4] = int32_t c
; [esp    ] = return address

        Call_Asm_Stack1 toupper_c

        retn

; end procedure toupper_asm2c


align 16
sprintf_asm2c:

; [esp + 3*4] = ...
; [esp + 2*4] = const char *format
; [esp +   4] = char *str
; [esp      ] = return address

        Call_Asm_VariableStack2 sprintf2_c

        retn

; end procedure sprintf_asm2c


align 16
sscanf_asm2c:

; [esp + 3*4] = ...
; [esp + 2*4] = const char *format
; [esp +   4] = const char *str
; [esp      ] = return address

        Call_Asm_VariableStack2 sscanf2_c

        retn

; end procedure sscanf_asm2c


align 16
system_asm2c:

; [esp + 4] = const char *command
; [esp    ] = return address

        Call_Asm_Stack1 system_c

        retn

; end procedure system_asm2c


align 16
srand_asm2c:

; [esp + 4] = uint32_t seed
; [esp    ] = return address

        Call_Asm_Stack1 srand_c

        retn

; end procedure srand_asm2c


align 16
rand_asm2c:

; [esp] = return address

        Call_Asm_Stack0 rand_c

        retn

; end procedure rand_asm2c


align 16
_aulldiv_asm2c:

; [esp + 4*4] =
; [esp + 3*4] = uint64_t y
; [esp + 2*4] =
; [esp +   4] = uint64_t x
; [esp      ] = return address

        Call_Asm_Stack4 _aulldiv_c

        retn 4*4

; end procedure _aulldiv_asm2c


align 16
_alldiv_asm2c:

; [esp + 4*4] =
; [esp + 3*4] = int64_t y
; [esp + 2*4] =
; [esp +   4] = int64_t x
; [esp      ] = return address

        Call_Asm_Stack4 _alldiv_c

        retn 4*4

; end procedure _alldiv_asm2c


align 16
_time64_asm2c:

; [esp + 4] = uint64_t *t64
; [esp    ] = return address

        Call_Asm_Stack1 _time64_c

        retn

; end procedure _time64_asm2c


align 16
_check_security_cookie_asm2c:

; ecx = uint32_t cookie
; [esp] = return address

        cmp ecx, [security_cookie_]
        jne .1
        retn
    .1:
        Call_Asm_Stack0 __report_gsfailure_c

        retn

; end procedure _check_security_cookie_asm2c


align 16
_except_handler4_asm2c:

; [esp + 3*4] = int32_t
; [esp + 2*4] = void *TargetFrame
; [esp +   4] = int32_t
; [esp      ] = return address

        Call_Asm_Stack3 _except_handler4_c

        retn

; end procedure _except_handler4_asm2c


align 16
_beginthread_asm2c:

; [esp + 3*4] = void *arglist
; [esp + 2*4] = uint32_t stack_size
; [esp +   4] = void(*start_address)(void *)
; [esp      ] = return address

        Call_Asm_Stack3 _beginthread_c

        retn

; end procedure _beginthread_asm2c


align 16
sync_asm2c:

; [esp] = return address

        Call_Asm_Stack0 sync_c

        retn

; end procedure sync_asm2c


align 16
_ftol2_sse_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Int _ftol2_sse_c

        retn

; end procedure _ftol2_sse_asm2c


align 16
_ftol2_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Int _ftol2_c

        retn

; end procedure _ftol2_asm2c


align 16
_ftol_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Int _ftol_c

        retn

; end procedure _ftol_asm2c


align 16
_CIcos_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Float _CIcos_c

        retn

; end procedure _CIcos_asm2c


align 16
_CIsin_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Float _CIsin_c

        retn

; end procedure _CIsin_asm2c


align 16
_CIatan2_asm2c:

; st1   = y
; st0   = x
; [esp] = return address

        Call_Asm_Float2_Float _CIatan2_c

        retn

; end procedure _CIatan2_asm2c


align 16
_CIsqrt_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Float _CIsqrt_c

        retn

; end procedure _CIsqrt_asm2c


align 16
_CIfmod_asm2c:

; st1   = x
; st0   = y
; [esp] = return address

        Call_Asm_Float2_Float _CIfmod_c

        retn

; end procedure _CIfmod_asm2c


align 16
_CItan_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Float _CItan_c

        retn

; end procedure _CItan_asm2c


align 16
_CIpow_asm2c:

; st1   = x
; st0   = y
; [esp] = return address

        Call_Asm_Float2_Float _CIpow_c

        retn

; end procedure _CIpow_asm2c


