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

%macro CALL 1-*
        lea r8d, [rel %%after_call]
        lea r11d, [r11d-4*%0]
        mov [r11d], r8d
  %assign i 1
  %rep %0-1
    %rotate 1
        mov dword [r11d+4*i], %1
    %assign i i+1
  %endrep
  %rotate 1
        jmp %1
    %%after_call:
%endm

%macro ENTER 2
        lea r11d, [r11d-4]
        mov dword [r11d], ebp
  %ifn %2
        ; nesting level 0
        mov ebp, r11d
    %if %1
        lea r11d, [r11d-%1]
    %endif
  %elifn %2-1
        ; nesting level 1
        mov ebp, r11d
    %ifn %1
        lea r11d, [r11d-4]
        mov dword [r11d], ebp
    %else
        lea r11d, [r11d-(4+%1)]
        mov dword [r11d+%1], ebp
    %endif
  %else
        ; nesting level 2 or higher
        lea r8d, [ebp-4*(%2-1)]
        mov ebp, r11d
        lea r11d, [r11d-(4*(%2-1)+4+%1)]
    %ifn %1
        mov dword [r11d], ebp
    %else
        mov dword [r11d+%1], ebp
    %endif
        mov r9d, [r8d]
        mov dword [r11d+4+%1], r9d
    %assign i 1
    %rep %2-2
        mov r9d, [r8d+4*i]
        mov dword [r11d+4*i+4+%1], r9d
        %assign i i+1
    %endrep
  %endif
%endm

%macro LEAVE 0
        lea r11d, [ebp+4]
        mov ebp, [r11d-4]
%endm

%macro RET 0-1 0
        mov r8d, [r11d]
        lea r11d, [r11d+4+%1]
        jmp r8
%endm

