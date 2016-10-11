loc_3E618,6,@mov ebx, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr ebx, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_13024,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_3A3D9,5,@add eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add eax, eax, tmp1 @ use real address of screen window instead of 0x0a0000

loc_47693,27,@mov eax, ecx|mov eax, ecx @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12638,22,@inc dword [loc_61C94]|LDR tmpadr, =loc_61C94|ldr tmp1, [tmpadr]|add tmp1, tmp1, #1|str tmp1, [tmpadr] @ timer interrupt beginning
loc_12658,31,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_48150,35, @ read sound.cfg
loc_4817B,21,@call SR_FillSoundCfg|ADR tmp1, loc_4817B_after_call|stmfd esp!, {tmp1}|b SR_FillSoundCfg|LTORG_CALL|loc_4817B_after_call:|@add esp, 2*4|add esp, esp, #(2*4) @ read sound.cfg

loc_153BB,5,@mov ebx, 0x40|mov ebx, #0x40 @ fix for the "Difficulty bug"

loc_102D3,7,@call SR_RunTimerDelay|ADR tmp1, loc_102D3_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_102D3_after_call:|@cmp dword [loc_60E0C], 0|LDR tmpadr, =loc_60E0C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for ??? in timer interrupt
loc_1033C,7,@call SR_RunTimerDelay|ADR tmp1, loc_1033C_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1033C_after_call:|@cmp dword [loc_60E0C], 0|LDR tmpadr, =loc_60E0C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3A255,2,@call SR_RunTimerDelay|ADR tmp1, loc_3A255_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3A255_after_call:|@xor ebx, ebx|mov ebx, #0 @ update timer

loc_1A9EE,7,@call SR_RunTimerDelay|ADR tmp1, loc_1A9EE_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1A9EE_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for mouse click in timer interrupt
loc_1C630,7,@call SR_RunTimerDelay|ADR tmp1, loc_1C630_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1C630_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_2319E,7,@call SR_RunTimerDelay|ADR tmp1, loc_2319E_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_2319E_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_24C2C,7,@call SR_RunTimerDelay|ADR tmp1, loc_24C2C_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_24C2C_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_25619,7,@call SR_RunTimerDelay|ADR tmp1, loc_25619_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_25619_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_25C97,7,@call SR_RunTimerDelay|ADR tmp1, loc_25C97_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_25C97_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_25D98,7,@call SR_RunTimerDelay|ADR tmp1, loc_25D98_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_25D98_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_25E30,7,@call SR_RunTimerDelay|ADR tmp1, loc_25E30_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_25E30_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_25EDB,7,@call SR_RunTimerDelay|ADR tmp1, loc_25EDB_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_25EDB_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_28209,6,@call SR_RunTimerDelay|ADR tmp1, loc_28209_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_28209_after_call:|@cmp esi, [loc_60DE0]|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp esi, tmp1 @ update timer
loc_3A61D,7,@call SR_RunTimerDelay|ADR tmp1, loc_3A61D_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3A61D_after_call:|@cmp dword [loc_60DE0], 0|LDR tmpadr, =loc_60DE0|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer

loc_10641,6,@call SR_RunTimerDelay|ADR tmp1, loc_10641_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_10641_after_call:|@cmp eax, [loc_60E34]|LDR tmpadr, =loc_60E34|ldr tmp1, [tmpadr]|cmp eax, tmp1 @ update timer - wait for ??? in timer interrupt

loc_10587,7,@call SR_RunTimerDelay|ADR tmp1, loc_10587_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_10587_after_call:|@cmp dword [loc_6100C], 0|LDR tmpadr, =loc_6100C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for ??? in timer interrupt

loc_105CC,7,@call SR_RunTimerDelay|ADR tmp1, loc_105CC_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_105CC_after_call:|@cmp dword [loc_61010], 0|LDR tmpadr, =loc_61010|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for ??? in timer interrupt

loc_3ED69,5,@mov eax, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr eax, [tmpadr] @ get stdout

loc_22D1C,8,@call SR_filelength2|ADR tmp1, loc_22D1C_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_22D1C_after_call: @ get file length
loc_22E78,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_22E78_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_22E78_after_call: @ get file length

loc_14155,1,@pop edi|ldmfd esp!, {edi}|@call SR_Sync|ADR tmp1, loc_14155_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_14155_after_call: @ call sync after save
loc_1439F,1,@pop ebp|ldmfd esp!, {ebp}|@call SR_Sync|ADR tmp1, loc_1439F_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_1439F_after_call: @ call sync after save
loc_147A5,1,@pop edx|ldmfd esp!, {edx}|@call SR_Sync|ADR tmp1, loc_147A5_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_147A5_after_call: @ call sync after save
loc_36B87,1,@pop edx|ldmfd esp!, {edx}|@call SR_Sync|ADR tmp1, loc_36B87_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_36B87_after_call: @ call sync after save

loc_29A9D,2,@call SR_SlowDownMainLoop|ADR tmp1, loc_29A9D_after_call|stmfd esp!, {tmp1}|b SR_SlowDownMainLoop|LTORG_CALL|loc_29A9D_after_call:|@test eax, eax|andS tmp1, eax, eax @ slow down game main loop

loc_1EB28,2,@xor edx, edx|mov edx, #0|@mov [esp+0x0c], edx|str edx, [esp, #0x0c] @ fix reading uninitialized variable

loc_2B101,2,@sub eax, edx|sub eax, eax, edx|@mov esi, 0xffffffff|mvn esi, #0 @ fix reading not reinitialized variable

loc_48669,220,@lea eax, [ebp-0x40]|sub eax, ebp, #0x40|@mov edx, [ebp-8]|ldr edx, [ebp, #(-8)]|@mov ebx, [loc_6C918]|LDR tmpadr, =loc_6C918|ldr ebx, [tmpadr]|@call SR_ReadSong|ADR tmp1, loc_48669_after_call|stmfd esp!, {tmp1}|b SR_ReadSong|LTORG_CALL|loc_48669_after_call:|@or eax, eax|cmp eax, #0|@jz loc_48779|beq loc_48779 @ music - load midi or song from catalog

loc_4F684,2,@adc eax, eax|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adcS eax, eax, eax @ on ARM swap carry bit before instruction ('adc eax, eax' must set flags for 'adc')

loc_4F6AC,2,@jnb short loc_4F6BF|bhs loc_4F6BF|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_4F91B,2,@jnb short loc_4F922|bhs loc_4F922|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction
loc_4F921,1,@inc edi|add edi, edi, #1|loc_4F922: @ define label

loc_4FA25,3,@adc eax, 0|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adcS eax, eax, #0 @ on ARM swap carry bit before instruction ('adc eax, 0' must set flags for 'adc')

loc_4FA2B,2,@jnb short loc_4FA3E|bhs loc_4FA3E|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_4FE87,2,@jnb short loc_4FE90|bhs loc_4FE90|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_4FEE9,2,@jnb short loc_4FEF2|bhs loc_4FEF2|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_50236,2,@adc edi, edi|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adc edi, edi, edi @ on ARM swap carry bit before instruction

loc_20D5B,5,.ltorg @ insert .ltorg after retn
