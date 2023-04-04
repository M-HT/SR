.include "armconf.inc"
.include "arm.inc"
.include "arminc.inc"
.include "misc.inc"
.include "extern.inc"
.include "macros.inc"
.global update_timer
.global _update_timer
.global main_
.global _main_
.global errno_val
.global _errno_val
.global stack_start
.global _stack_start

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
