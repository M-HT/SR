.include "armconf.inc"
.include "arm.inc"
.include "arminc.inc"
.include "misc.inc"
.include "extern.inc"
.include "macros.inc"
.global tactical_update_timer
.global _tactical_update_timer
.global tactical_main_
.global _tactical_main_
.global tactical_data_begin
.global _tactical_data_begin
.global tactical_data_end
.global _tactical_data_end
.global tactical_bss_begin
.global _tactical_bss_begin
.global tactical_errno_val
.global _tactical_errno_val
.global tactical_bss_end
.global _tactical_bss_end

.section .note.GNU-stack,"",%progbits

.section .text
.include "gogver/seg01.inc"
.ifdef DEBUG
.include "code_armlinux.inc"
.endif

.section .data
.include "gogver/seg02.inc"
.ifdef DEBUG
.include "data_all.inc"
.endif

.section .data
.include "gogver/seg03.inc"

.section .bss
.include "gogver/seg04.inc"
