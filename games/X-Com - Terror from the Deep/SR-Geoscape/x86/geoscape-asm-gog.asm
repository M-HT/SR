%include "x86inc.inc"
%include "misc.inc"
%include "extern.inc"
%include "macros.inc"
global geoscape_update_timer
global _geoscape_update_timer
global geoscape_main_
global _geoscape_main_
global geoscape_data_begin
global _geoscape_data_begin
global geoscape_data_end
global _geoscape_data_end
global geoscape_bss_begin
global _geoscape_bss_begin
global geoscape_errno_val
global _geoscape_errno_val
global geoscape_bss_end
global _geoscape_bss_end

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
section useg03 nobits alloc noexec write align=4
%else
section useg03 bss align=4
%endif
%include "gogver/seg03.inc"
