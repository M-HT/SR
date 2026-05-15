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

%include "x64inc.inc"
%include "asm-calls.inc"

extern memset_c
extern memcpy_c

extern _stricmp_c
extern strncpy_c
extern strncmp_c
extern strncat_c

extern malloc_c
extern free_c
extern calloc_c

extern atol_c
extern toupper_c

extern sprintf2_c
extern sscanf2_c

extern system_c
extern srand_c
extern rand_c

extern _time64_c

extern __report_gsfailure_c

extern _except_handler4_c

extern _beginthread_c

extern sync_c

extern _ftol2_c
extern _ftol_c

extern _CIcos_c
extern _CIsin_c
extern _CIatan2_c
extern _CIsqrt_c
extern _CIfmod_c
extern _CItan_c
extern _CIpow_c


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


%ifidn __OUTPUT_FORMAT__, elf64
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif
%ifidn __OUTPUT_FORMAT__, win64
section_prolog:
        SECTION_PROLOG
%endif

align 16
memset_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = int32_t c
; [esp +   4] = void *s
; [esp      ] = return address

        Call_Asm_Stack3 memset_c

; end procedure memset_asm2c


align 16
memcpy_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const void *src
; [esp +   4] = void *dest
; [esp      ] = return address

        Call_Asm_Stack3 memcpy_c

; end procedure memcpy_asm2c


align 16
_stricmp_asm2c:

; [esp + 2*4] = const char *s2
; [esp +   4] = const char *s1
; [esp      ] = return address

        Call_Asm_Stack2 _stricmp_c

; end procedure _stricmp_asm2c


align 16
strncpy_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const char *src
; [esp +   4] = char *dest
; [esp      ] = return address

        Call_Asm_Stack3 strncpy_c

; end procedure strncpy_asm2c


align 16
strncmp_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const char *s2
; [esp +   4] = const char *s1
; [esp      ] = return address

        Call_Asm_Stack3 strncmp_c

; end procedure strncmp_asm2c


align 16
strncat_asm2c:

; [esp + 3*4] = uint32_t n
; [esp + 2*4] = const char *src
; [esp +   4] = char *dest
; [esp      ] = return address

        Call_Asm_Stack3 strncat_c

; end procedure strncat_asm2c


align 16
malloc_asm2c:

; [esp + 4] = uint32_t size
; [esp    ] = return address

        Call_Asm_Stack1 malloc_c

; end procedure malloc_asm2c

align 16
free_asm2c:

; [esp + 4] = void *ptr
; [esp    ] = return address

        Call_Asm_Stack1 free_c

; end procedure free_asm2c


align 16
calloc_asm2c:

; [esp + 2*4] = uint32_t size
; [esp +   4] = uint32_t nmemb
; [esp      ] = return address

        Call_Asm_Stack2 calloc_c

; end procedure calloc_asm2c


align 16
_alloca_probe_asm2c:

; eax = uint32_t size
; [esp] = return address

        mov r8d, [r11d]
        mov r9d, r11d
        sub r9d, eax
        sbb eax, eax
        not eax
        and r9d, eax
        lea eax, [r11d-4]
        and eax, 0xfffff000

    .cs10:
        cmp r9d, eax
        jb short .cs20
        lea r11d, [r9d+4]
        jmp r8

    .cs20:
        sub eax, 0x1000
        test [eax], eax
        jmp short .cs10

; end procedure _alloca_probe_asm2c


align 16
atol_asm2c:

; [esp + 4] = const char *nptr
; [esp    ] = return address

        Call_Asm_Stack1 atol_c

; end procedure atol_asm2c


align 16
toupper_asm2c:

; [esp + 4] = int32_t c
; [esp    ] = return address

        Call_Asm_Stack1 toupper_c

; end procedure toupper_asm2c


align 16
sprintf_asm2c:

; [esp + 3*4] = ...
; [esp + 2*4] = const char *format
; [esp +   4] = char *str
; [esp      ] = return address

        Call_Asm_VariableStack2 sprintf2_c

; end procedure sprintf_asm2c


align 16
sscanf_asm2c:

; [esp + 3*4] = ...
; [esp + 2*4] = const char *format
; [esp +   4] = const char *str
; [esp      ] = return address

        Call_Asm_VariableStack2 sscanf2_c

; end procedure sscanf_asm2c


align 16
system_asm2c:

; [esp + 4] = const char *command
; [esp    ] = return address

        Call_Asm_Stack1 system_c

; end procedure system_asm2c


align 16
srand_asm2c:

; [esp + 4] = uint32_t seed
; [esp    ] = return address

        Call_Asm_Stack1 srand_c

; end procedure srand_asm2c


align 16
rand_asm2c:

; [esp] = return address

        Call_Asm_Stack0 rand_c

; end procedure rand_asm2c


align 16
_aulldiv_asm2c:

; [esp + 4*4] =
; [esp + 3*4] = uint64_t y
; [esp + 2*4] =
; [esp +   4] = uint64_t x
; [esp      ] = return address

        mov rax, [r11d+4]
        xor rdx, rdx
        div qword [r11d+3*4]
        mov rdx, rax
        shr rdx, 32
        mov eax, eax
        RET 4*4

; end procedure _aulldiv_asm2c


align 16
_alldiv_asm2c:

; [esp + 4*4] =
; [esp + 3*4] = int64_t y
; [esp + 2*4] =
; [esp +   4] = int64_t x
; [esp      ] = return address

        mov rax, [r11d+4]
        cqo
        idiv qword [r11d+3*4]
        mov rdx, rax
        shr rdx, 32
        mov eax, eax
        RET 4*4

; end procedure _alldiv_asm2c


align 16
_time64_asm2c:

; [esp + 4] = uint64_t *t64
; [esp    ] = return address

        ;Call_Asm_Stack1 _time64_c

        Call_Asm_Prologue_0

%ifidn __OUTPUT_FORMAT__, win64
        mov ecx, [r11d+4]
%else
        mov edi, [r11d+3*4]
%endif

        call _time64_c

        mov rdx, rax
        shr rdx, 32
        mov eax, eax

        Call_Asm_Epilogue_0 0

; end procedure _time64_asm2c


align 16
_check_security_cookie_asm2c:

; ecx = uint32_t cookie
; [esp] = return address

        cmp ecx, [security_cookie_]
        jne .1
        RET
    .1:
        Call_Asm_Stack0 __report_gsfailure_c

; end procedure _check_security_cookie_asm2c


align 16
_except_handler4_asm2c:

; [esp + 3*4] = int32_t
; [esp + 2*4] = void *TargetFrame
; [esp +   4] = int32_t
; [esp      ] = return address

        Call_Asm_Stack3 _except_handler4_c

; end procedure _except_handler4_asm2c


align 16
_beginthread_asm2c:

; [esp + 3*4] = void *arglist
; [esp + 2*4] = uint32_t stack_size
; [esp +   4] = void(*start_address)(void *)
; [esp      ] = return address

        Call_Asm_Stack3 _beginthread_c

; end procedure _beginthread_asm2c


align 16
sync_asm2c:

; [esp] = return address

        Call_Asm_Stack0 sync_c

; end procedure sync_asm2c


align 16
_ftol2_sse_asm2c:

; st0   = num
; [esp] = return address

%ifidn __OUTPUT_FORMAT__, win64
        fstp qword [rsp+FIRST_PARAMETER_OFFSET+8]
        cvttsd2si eax, [rsp+FIRST_PARAMETER_OFFSET+8]
%else
        sub rsp, byte 16
        fstp qword [rsp]
        cvttsd2si eax, [rsp]
        add rsp, byte 16
%endif

        RET

; end procedure _ftol2_sse_asm2c


align 16
_ftol2_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Int _ftol2_c

; end procedure _ftol2_asm2c


align 16
_ftol_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Int _ftol_c

; end procedure _ftol_asm2c


align 16
_CIcos_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Float _CIcos_c

; end procedure _CIcos_asm2c


align 16
_CIsin_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Float _CIsin_c

; end procedure _CIsin_asm2c


align 16
_CIatan2_asm2c:

; st1   = y
; st0   = x
; [esp] = return address

        Call_Asm_Float2_Float _CIatan2_c

; end procedure _CIatan2_asm2c


align 16
_CIsqrt_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Float _CIsqrt_c

; end procedure _CIsqrt_asm2c


align 16
_CIfmod_asm2c:

; st1   = x
; st0   = y
; [esp] = return address

        Call_Asm_Float2_Float _CIfmod_c

; end procedure _CIfmod_asm2c


align 16
_CItan_asm2c:

; st0   = num
; [esp] = return address

        Call_Asm_Float1_Float _CItan_c

; end procedure _CItan_asm2c


align 16
_CIpow_asm2c:

; st1   = x
; st0   = y
; [esp] = return address

        Call_Asm_Float2_Float _CIpow_c

; end procedure _CIpow_asm2c


%ifidn __OUTPUT_FORMAT__, win64
section_end:

section .pdata rdata align=4
        P_UNWIND_INFO section_prolog, section_end, x_common
section .xdata rdata align=8
align 8
x_common:
        X_UNWIND_INFO section_prolog
%endif
