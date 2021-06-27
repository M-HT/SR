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

global sl_left_capture
global _sl_left_capture
global sl_top_capture
global _sl_top_capture
global sl_bottom_capture
global _sl_bottom_capture
global sl_right_capture
global _sl_right_capture

global p_last_touched_screen
global _p_last_touched_screen
global sl_screenwidth
global _sl_screenwidth
global sl_screenheight
global _sl_screenheight
global p_mouse_gui
global _p_mouse_gui
global h_mouse_win
global _h_mouse_win
global b_mouse_capture_on
global _b_mouse_capture_on
global b_application_active
global _b_application_active
global w_avi_device_id
global _w_avi_device_id

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

%ifidn __OUTPUT_FORMAT__, elf32
section .bss nobits alloc noexec write align=4
%else
section .bss bss align=4
%endif

sl_left_capture:
_sl_left_capture:
resb 4
sl_top_capture:
_sl_top_capture:
resb 4
sl_bottom_capture:
_sl_bottom_capture:
resb 4
sl_right_capture:
_sl_right_capture:
resb 4


%ifidn __OUTPUT_FORMAT__, elf32
section .data progbits alloc noexec write align=4
%else
section .data data align=4
%endif

p_last_touched_screen:
_p_last_touched_screen:
dd 0
sl_screenwidth:
_sl_screenwidth:
dd 0
sl_screenheight:
_sl_screenheight:
dd 0
p_mouse_gui:
_p_mouse_gui:
dd 0
h_mouse_win:
_h_mouse_win:
dd 0
b_mouse_capture_on:
_b_mouse_capture_on:
dd 0
b_application_active:
_b_application_active:
dd 0
w_avi_device_id:
_w_avi_device_id:
dd 0xffff

