;;
;;  Copyright (C) 2016 Roman Pauer
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

%macro ACTION_UNCONDITIONAL_SHORT_JUMP_BACKWARD 0
    call SR_CheckTimer
%endmacro

%macro ACTION_UNCONDITIONAL_SHORT_JUMP_FORWARD 0
    ; do nothing
%endmacro

%macro ACTION_UNCONDITIONAL_JUMP 0
    call SR_CheckTimer
%endmacro

%macro ACTION_LOOP_BACKWARD 0
    call SR_CheckTimer
%endmacro

%macro ACTION_LOOP_FORWARD 0
    ; do nothing
%endmacro

%macro ACTION_CONDITIONAL_JUMP_BACKWARD 0
    call SR_CheckTimer
%endmacro

%macro ACTION_CONDITIONAL_JUMP_FORWARD 0
    ; do nothing
%endmacro

%macro ACTION_CALL 0
    call SR_CheckTimer
%endmacro

; ------------------------------------------------------------------------------

%macro CALL___CHK 0
    add esp, 4
%endmacro

; ------------------------------------------------------------------------------

;; edx = [self_mod_width]
;;; ebp = ebp + (i-1) * edx

%macro self_mod_code_1 0
    mov bl, [esi]
    add edi, eax
    mov bl, [ebx]
    adc esi, ecx
    mov [ebp], bl
    add ebp, edx
%endmacro

%macro self_mod_code_2 0
    mov bl, [esi]
    adc esi, ecx
    or  bl, bl
    jz  short %%loc_B66A2
    mov al, [ebx]
    mov [ebp], al

%%loc_B66A2:
    add ebp, edx
    add edi, eax
%endmacro

%macro self_mod_code_3 0
    mov bl, [esi]
    or  bl, bl
    mov al, [ebx]
    jnz short %%loc_B8450
    mov dl, [ebp]
    mov al, [edx]

%%loc_B8450:
    add edi, eax
    adc esi, ecx
    mov [ebp], al
    add ebp, [self_mod_width]
%endmacro

%macro self_mod_code_4 0
    add edi, eax
    mov bl, [esi]
    adc esi, ecx
    or  bl, bl
    jz  short %%loc_BB046
    mov bl, [ebx]
    mov [edx], bl

%%loc_BB046:
    add edx, ebp

%endmacro

