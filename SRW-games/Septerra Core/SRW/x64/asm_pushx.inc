;part of static recompiler -- do not edit

;;
;;  Copyright (C) 2016-2025 Roman Pauer
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

%macro PUSH32 1
        lea r11d, [r11d-4]
        mov dword [r11d], %1
%endm

%macro PUSH32 2-*
        lea r11d, [r11d-4*%0]
  %rotate -1
        mov dword [r11d], %1
  %assign i 1
  %rep %0-1
    %rotate -1
        mov dword [r11d+4*i], %1
    %assign i i+1
  %endrep
%endm

%macro PUSH16 1
        lea r11d, [r11d-2]
        mov word [r11d], %1
%endm

%macro PUSH16 2-*
        lea r11d, [r11d-2*%0]
  %rotate -1
        mov word [r11d], %1
  %assign i 1
  %rep %0-1
    %rotate -1
        mov word [r11d+2*i], %1
    %assign i i+1
  %endrep
%endm

%macro PUSHAD 0
        mov r8d, r11d
        lea r11d, [r11d-32]
        mov [r11d], edi
        mov [r11d+4], esi
        mov [r11d+8], ebp
        mov [r11d+12], r8d
        mov [r11d+16], ebx
        mov [r11d+20], edx
        mov [r11d+24], ecx
        mov [r11d+28], eax
%endm

%macro PUSHAW 0
        mov r8w, r11w
        lea r11d, [r11d-16]
        mov [r11d], di
        mov [r11d+2], si
        mov [r11d+4], bp
        mov [r11d+6], r8w
        mov [r11d+8], bx
        mov [r11d+10], dx
        mov [r11d+12], cx
        mov [r11d+14], ax
%endm

%macro PUSHFD 0
        pushf
        pop r8
        lea r11d, [r11d-4]
        mov [r11d], r8d
%endm

%macro PUSHFW 0
        pushfw
        pop r8w
        lea r11d, [r11d-2]
        mov [r11d], r8w
%endm

%macro POP32 1
        mov %1, dword [r11d]
        lea r11d, [r11d+4]
%endm

%macro POP32 2-*
  %rotate -1
        mov %1, dword [r11d]
  %assign i 1
  %rep %0-1
    %rotate -1
        mov %1, dword [r11d+4*i]
    %assign i i+1
  %endrep
        lea r11d, [r11d+4*%0]
%endm

%macro POP16 1
        mov %1, word [r11d]
        lea r11d, [r11d+2]
%endm

%macro POP16 2-*
  %rotate -1
        mov %1, word [r11d]
  %assign i 1
  %rep %0-1
    %rotate -1
        mov %1, word [r11d+2*i]
    %assign i i+1
  %endrep
        lea r11d, [r11d+2*%0]
%endm

%macro POPAD 0
        mov edi, [r11d]
        mov esi, [r11d+4]
        mov ebp, [r11d+8]
        mov ebx, [r11d+16]
        mov edx, [r11d+20]
        mov ecx, [r11d+24]
        mov eax, [r11d+28]
        lea r11d, [r11d+32]
%endm

%macro POPAW 0
        mov di, [r11d]
        mov si, [r11d+2]
        mov bp, [r11d+4]
        mov bx, [r11d+8]
        mov dx, [r11d+10]
        mov cx, [r11d+12]
        mov ax, [r11d+14]
        lea r11d, [r11d+16]
%endm

%macro POPFD 0
        mov r8d, [r11d]
        lea r11d, [r11d+4]
        pushf
        mov [rsp], r8d
        popf
%endm

%macro POPFW 0
        mov r8w, [r11d]
        lea r11d, [r11d+2]
        push r8w
        popfw
%endm

