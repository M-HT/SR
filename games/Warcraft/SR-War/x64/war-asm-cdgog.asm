%include "x64inc.inc"
%include "misc.inc"
%include "extern.inc"
%include "macros.inc"
global main_
global keyboard_interrupt
global update_timer
global argv_val
global errno_val
global default_sample_volume
global default_sequence_volume
global stack_start

%ifidn __OUTPUT_FORMAT__, elf64
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

%ifidn __OUTPUT_FORMAT__, elf64
section cseg01 progbits alloc exec nowrite align=16
%else
section cseg01 code align=16
%endif
%ifidn __OUTPUT_FORMAT__, win64
section_prolog_0:
SECTION_PROLOG
%endif
%include "cdgogver/seg01.inc"
%ifidn __OUTPUT_FORMAT__, win64
section_end_0:
%endif

%ifidn __OUTPUT_FORMAT__, elf64
section dseg02 progbits alloc noexec write align=4
%else
section dseg02 data align=4
%endif
%include "cdgogver/seg02.inc"

%ifidn __OUTPUT_FORMAT__, elf64
section useg03 nobits alloc noexec write align=4
%else
section useg03 bss align=4
%endif
%include "cdgogver/seg03.inc"

%ifidn __OUTPUT_FORMAT__, win64
section .pdata rdata align=4
P_UNWIND_INFO section_prolog_0, section_end_0, x_common
section .xdata rdata align=8
align 8
x_common:
X_UNWIND_INFO section_prolog_0
%endif
