%include "x86inc.inc"
%include "misc.inc"
%include "extern.inc"
%include "macros.inc"
global tactical_update_timer
global _tactical_update_timer
global tactical_main_
global _tactical_main_
global tactical_data_begin
global _tactical_data_begin
global tactical_data_end
global _tactical_data_end
global tactical_bss_begin
global _tactical_bss_begin
global tactical_errno_val
global _tactical_errno_val
global tactical_bss_end
global _tactical_bss_end

%ifidn __OUTPUT_FORMAT__, elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

%ifidn __OUTPUT_FORMAT__, elf32
section cseg01 progbits alloc exec nowrite align=16
%else
section cseg01 code align=16
%endif
%include "gogver/seg01.inc"
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
%include "gogver/seg02.inc"
%ifdef DEBUG
%include "data_all.inc"
%endif

%ifidn __OUTPUT_FORMAT__, elf32
section dseg03 progbits alloc noexec write align=4
%else
section dseg03 data align=4
%endif
%include "gogver/seg03.inc"

%ifidn __OUTPUT_FORMAT__, elf32
section useg04 nobits alloc noexec write align=4
%else
section useg04 bss align=4
%endif
%include "gogver/seg04.inc"
