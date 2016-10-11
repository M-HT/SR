@part of static recompiler -- do not edit

@@
@@  Copyright (C) 2016 Roman Pauer
@@
@@  Permission is hereby granted, free of charge, to any person obtaining a copy of
@@  this software and associated documentation files (the "Software"), to deal in
@@  the Software without restriction, including without limitation the rights to
@@  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
@@  of the Software, and to permit persons to whom the Software is furnished to do
@@  so, subject to the following conditions:
@@
@@  The above copyright notice and this permission notice shall be included in all
@@  copies or substantial portions of the Software.
@@
@@  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
@@  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
@@  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
@@  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
@@  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
@@  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
@@  SOFTWARE.
@@

.extern mem_read_word_tmp1
.extern mem_read_word_tmp2
.extern mem_read_word_tmp3
@tmpadr
.extern mem_read_word_edi
.extern mem_read_word_esi
.extern mem_read_word_ebp
.extern mem_read_word_ebx
.extern mem_read_word_edx
.extern mem_read_word_tmp9
.extern mem_read_word_ecx
.extern mem_read_word_eax
.extern mem_read_word_eflags
.extern mem_read_word_esp
@tmplr
.extern mem_read_word_eip

.extern mem_read_dword_tmp1_tmp2

.extern mem_write_word_tmp1
.extern mem_write_word_tmp2
.extern mem_write_word_tmp3
@tmpadr
.extern mem_write_word_edi
.extern mem_write_word_esi
.extern mem_write_word_ebp
.extern mem_write_word_ebx
.extern mem_write_word_edx
.extern mem_write_word_tmp9
.extern mem_write_word_ecx
.extern mem_write_word_eax
.extern mem_write_word_eflags
.extern mem_write_word_esp
@tmplr
@eip

.extern mem_write_dword_tmp1_tmp2
