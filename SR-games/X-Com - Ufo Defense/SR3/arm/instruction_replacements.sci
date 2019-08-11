loc_109D4,7,@push eax|@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|@mov [ebp-8], eax|@pop eax|str tmp1, [ebp, #-8] @ get real address of screen window instead of 0x0a0000
loc_10A27,7,@push eax|@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|@mov [ebp-8], eax|@pop eax|str tmp1, [ebp, #-8] @ get real address of screen window instead of 0x0a0000
loc_10B74,7,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|@mov [ebp-0x2c], eax|str tmp1, [ebp, #-0x2c] @ get real address of screen window instead of 0x0a0000
loc_10ED4,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_1100F,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000

loc_1386A,27,@mov eax, ecx|mov eax, ecx @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_111A5,22,@inc dword [loc_2076A]|LDR tmpadr, =loc_2076A|ldrh tmp1, [tmpadr]|ldrh tmp2, [tmpadr, #2]|orr tmp1, tmp1, tmp2, lsl #16|add tmp1, tmp1, #1|strh tmp1, [tmpadr]|mov tmp2, tmp1, lsr #16|strh tmp2, [tmpadr, #2] @ timer interrupt beginning
loc_111C1,39,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_11447,35, @ read sound.cfg
loc_11472,21,@call SR_FillSoundCfg|ADR tmp1, loc_11472_after_call|stmfd esp!, {tmp1}|b SR_FillSoundCfg|LTORG_CALL|loc_11472_after_call:|@add esp, 2*4|add esp, esp, #(2*4) @ read sound.cfg

loc_10486,2,@call SR_RunTimerDelay|ADR tmp1, loc_10486_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_10486_after_call:|@cmp edx, eax|cmp edx, eax @ update timer - wait for ??? in timer interrupt
loc_10520,2,@call SR_RunTimerDelay|ADR tmp1, loc_10520_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_10520_after_call:|@cmp edx, eax|cmp edx, eax @ update timer
loc_107FE,4,@call SR_RunTimerDelay|ADR tmp1, loc_107FE_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_107FE_after_call:|@cmp ax, [ebp-0x18]|ldrh tmp1, [ebp, #-0x18]|mov tmp2, eax, lsl #16|cmp tmp2, tmp1, lsl #16 @ update timer
loc_10922,4,@call SR_RunTimerDelay|ADR tmp1, loc_10922_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_10922_after_call:|@cmp ax, [ebp-8]|ldrh tmp1, [ebp, #-8]|mov tmp2, eax, lsl #16|cmp tmp2, tmp1, lsl #16 @ update timer
loc_10B31,2,@call SR_RunTimerDelay|ADR tmp1, loc_10B31_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_10B31_after_call:|@cmp edx, eax|cmp edx, eax @ update timer

loc_11B11,220,@lea eax, [ebp-0x40]|sub eax, ebp, #0x40|@mov edx, [ebp-8]|ldr edx, [ebp, #(-8)]|@mov ebx, [loc_21774]|LDR tmpadr, =loc_21774|ldr ebx, [tmpadr]|@call SR_ReadSong|ADR tmp1, loc_11B11_after_call|stmfd esp!, {tmp1}|b SR_ReadSong|LTORG_CALL|loc_11B11_after_call:|@or eax, eax|cmp eax, #0|@jz loc_11C21|beq loc_11C21 @ music - load midi or song from catalog


loc_111E8,72, @ CLIB code
loc_12B02,258, @ CLIB code
loc_12C1F,2409, @ CLIB code
loc_13AB2,2312, @ CLIB code
loc_143E2,8452, @ CLIB code

loc_20BC8,96, @ CLIB data
loc_20C3C,712, @ CLIB data

loc_218BC,20, @ CLIB data
loc_218D4,8, @ CLIB data
loc_218DC,4100, @ stack
