.include "armconf.inc"
.include "arm.inc"
.include "arminc.inc"
.include "misc.inc"
.include "extern.inc"
.include "macros.inc"
.global geoscape_update_timer
.global _geoscape_update_timer
.global geoscape_main_
.global _geoscape_main_
.global geoscape_data_begin
.global _geoscape_data_begin
.global geoscape_data_end
.global _geoscape_data_end
.global geoscape_bss_begin
.global _geoscape_bss_begin
.global geoscape_errno_val
.global _geoscape_errno_val
.global geoscape_bss_end
.global _geoscape_bss_end

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

.section .bss
.include "gogver/seg03.inc"
