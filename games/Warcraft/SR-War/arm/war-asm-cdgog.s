.include "armconf.inc"
.include "arm.inc"
.include "arminc.inc"
.include "misc.inc"
.include "extern.inc"
.include "macros.inc"
.global main_
.global _main_
.global keyboard_interrupt
.global _keyboard_interrupt
.global update_timer
.global _update_timer
.global argv_val
.global _argv_val
.global errno_val
.global _errno_val
.global default_sample_volume
.global _default_sample_volume
.global default_sequence_volume
.global _default_sequence_volume
.global stack_start
.global _stack_start

.section .note.GNU-stack,"",%progbits

.section .text
.include "cdgogver/seg01.inc"
.ifdef DEBUG
.include "code_armlinux.inc"
.endif

.section .data
.include "cdgogver/seg02.inc"
.ifdef DEBUG
.include "data_all.inc"
.endif
