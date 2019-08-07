%include "x86inc.inc"
%include "misc.inc"
%include "extern.inc"
%include "macros.inc"
global main_
global _main_
global keyboard_interrupt
global _keyboard_interrupt
global update_timer
global _update_timer
global argv_val
global _argv_val
global errno_val
global _errno_val
global default_sample_volume
global _default_sample_volume
global default_sequence_volume
global _default_sequence_volume
global stack_start
global _stack_start

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

%ifidn __OUTPUT_FORMAT__, elf32
section cseg01 progbits alloc exec nowrite align=16
%else
section cseg01 code align=16
%endif
%include "cdgogver/seg01.inc"
%ifdef DEBUG
%ifidn __OUTPUT_FORMAT__, elf32
%include "code_linux.inc"
%else
%include "code_win32.inc"
%endif
%endif

%ifidn __OUTPUT_FORMAT__, elf32
section dseg02 progbits alloc noexec write align=4
%else
section dseg02 data align=4
%endif
%include "cdgogver/seg02.inc"
%ifdef DEBUG
%include "data_all.inc"
%endif

%ifidn __OUTPUT_FORMAT__, elf32
section useg03 nobits alloc noexec write align=4
%else
section useg03 bss align=4
%endif
%include "cdgogver/seg03.inc"
