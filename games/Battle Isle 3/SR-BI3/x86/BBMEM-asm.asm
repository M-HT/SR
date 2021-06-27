;;
;;  Copyright (C) 2020-2021 Roman Pauer
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
    %define MEM_Init_c _MEM_Init_c
    %define MEM_Exit_c _MEM_Exit_c
    %define MEM_malloc_c _MEM_malloc_c
    %define MEM_free_c _MEM_free_c
    %define MEM_Take_Snapshot_c _MEM_Take_Snapshot_c
    %define MEM_Check_Snapshot_c _MEM_Check_Snapshot_c
    %define MEM_SwitchSecurity_c _MEM_SwitchSecurity_c
    %define BASEMEM_Alloc_c _BASEMEM_Alloc_c
    %define BASEMEM_Free_c _BASEMEM_Free_c
    %define BASEMEM_CopyMem_c _BASEMEM_CopyMem_c
    %define BASEMEM_FillMemByte_c _BASEMEM_FillMemByte_c
    %define BBMEM_GetPoolPointer_c _BBMEM_GetPoolPointer_c
%endif

extern MEM_Init_c
extern MEM_Exit_c
extern MEM_malloc_c
extern MEM_free_c
extern MEM_Take_Snapshot_c
extern MEM_Check_Snapshot_c
extern MEM_SwitchSecurity_c
extern BASEMEM_Alloc_c
extern BASEMEM_Free_c
extern BASEMEM_CopyMem_c
extern BASEMEM_FillMemByte_c
extern BBMEM_GetPoolPointer_c

global MEM_Init
global MEM_Exit
global MEM_malloc
global MEM_free
global MEM_Take_Snapshot
global MEM_Check_Snapshot
global MEM_SwitchSecurity
global BASEMEM_Alloc
global BASEMEM_Free
global BASEMEM_CopyMem
global BASEMEM_FillMemByte
global BBMEM_GetPoolPointer

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text progbits alloc exec nowrite align=16
%else
section .text code align=16
%endif

align 16
MEM_Init:

; [esp      ] = return address

        Call_Asm_Stack0 MEM_Init_c

        retn

; end procedure MEM_Init


align 16
MEM_Exit:

; [esp      ] = return address

        Call_Asm_Stack0 MEM_Exit_c

        retn

; end procedure MEM_Exit


align 16
MEM_malloc:

; [esp + 5*4] = int type
; [esp + 4*4] = unsigned int line
; [esp + 3*4] = const char * object_type
; [esp + 2*4] = const char * module_name
; [esp +   4] = unsigned int size
; [esp      ] = return address

        Call_Asm_Stack5 MEM_malloc_c

        retn

; end procedure MEM_malloc


align 16
MEM_free:

; [esp +   4] = void * mem_ptr
; [esp      ] = return address

        Call_Asm_Stack1 MEM_free_c

        retn

; end procedure MEM_free


align 16
MEM_Take_Snapshot:

; [esp +   4] = const char * name
; [esp      ] = return address

        Call_Asm_Stack1 MEM_Take_Snapshot_c

        retn

; end procedure MEM_Take_Snapshot


align 16
MEM_Check_Snapshot:

; [esp      ] = return address

        Call_Asm_Stack0 MEM_Check_Snapshot_c

        retn

; end procedure MEM_Check_Snapshot


align 16
MEM_SwitchSecurity:

; [esp +   4] = unsigned int security
; [esp      ] = return address

        Call_Asm_Stack1 MEM_SwitchSecurity_c

        retn

; end procedure MEM_SwitchSecurity


align 16
BASEMEM_Alloc:

; [esp +   4] = unsigned int size
; [esp      ] = return address

        Call_Asm_Stack1 BASEMEM_Alloc_c

        retn

; end procedure BASEMEM_Alloc


align 16
BASEMEM_Free:

; [esp +   4] = void * mem_ptr
; [esp      ] = return address

        Call_Asm_Stack1 BASEMEM_Free_c

        retn

; end procedure BASEMEM_Free


align 16
BASEMEM_CopyMem:

; [esp + 3*4] = unsigned int length
; [esp + 2*4] = void * dst
; [esp +   4] = const void * src
; [esp      ] = return address

        Call_Asm_Stack3 BASEMEM_CopyMem_c

        retn

; end procedure BASEMEM_CopyMem


align 16
BASEMEM_FillMemByte:

; [esp + 3*4] = int c
; [esp + 2*4] = unsigned int length
; [esp +   4] = void * dst
; [esp      ] = return address

        Call_Asm_Stack3 BASEMEM_FillMemByte_c

        retn

; end procedure BASEMEM_FillMemByte


align 16
BBMEM_GetPoolPointer:

; [esp      ] = return address

        Call_Asm_Stack0 BBMEM_GetPoolPointer_c

        retn

; end procedure BBMEM_GetPoolPointer


