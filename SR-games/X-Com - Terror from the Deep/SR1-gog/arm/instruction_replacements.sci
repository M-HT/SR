loc_43238,6,@mov ebx, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr ebx, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_12F24,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_3EDBF,5,@add eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add eax, eax, tmp1 @ use real address of screen window instead of 0x0a0000
loc_4E47D,10,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr]|@mov [loc_7CF04], eax|LDR tmpadr, =loc_7CF04|str eax, [tmpadr] @ use real address of screen window instead of 0x0a0000
loc_4E8F3,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000

loc_4CC1F,27,@mov eax, ecx;mov eax, ecx @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address
loc_4D626,27,@mov eax, ecx;mov eax, ecx @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12538,22,@inc dword [loc_621E0]|LDR tmpadr, =loc_621E0|ldr tmp1, [tmpadr]|add tmp1, tmp1, #1|str tmp1, [tmpadr] @ timer interrupt beginning
loc_12558,31,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_4F295,35, @ read sound.cfg
loc_4F2C0,21,@call SR_FillSoundCfg|ADR tmp1, loc_4F2C0_after_call|stmfd esp!, {tmp1}|b SR_FillSoundCfg|LTORG_CALL|loc_4F2C0_after_call:|@add esp, 2*4|add esp, esp, #(2*4) @ read sound.cfg

loc_102D3,7,@call SR_RunTimerDelay|ADR tmp1, loc_102D3_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_102D3_after_call:|@cmp dword [loc_61358], 0|LDR tmpadr, =loc_61358|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for ??? in timer interrupt
loc_1033C,7,@call SR_RunTimerDelay|ADR tmp1, loc_1033C_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1033C_after_call:|@cmp dword [loc_61358], 0|LDR tmpadr, =loc_61358|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3EC65,2,@call SR_RunTimerDelay|ADR tmp1, loc_3EC65_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3EC65_after_call:|@xor ebx, ebx|mov ebx, #0 @ update timer

loc_1E197,7,@call SR_RunTimerDelay|ADR tmp1, loc_1E197_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1E197_after_call:|@cmp dword [loc_6132C], 0|LDR tmpadr, =loc_6132C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for mouse click in timer interrupt
loc_264F4,7,@call SR_RunTimerDelay|ADR tmp1, loc_264F4_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_264F4_after_call:|@cmp dword [loc_6132C], 0|LDR tmpadr, =loc_6132C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_26F09,7,@call SR_RunTimerDelay|ADR tmp1, loc_26F09_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_26F09_after_call:|@cmp dword [loc_6132C], 0|LDR tmpadr, =loc_6132C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_27611,7,@call SR_RunTimerDelay|ADR tmp1, loc_27611_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_27611_after_call:|@cmp dword [loc_6132C], 0|LDR tmpadr, =loc_6132C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_27716,7,@call SR_RunTimerDelay|ADR tmp1, loc_27716_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_27716_after_call:|@cmp dword [loc_6132C], 0|LDR tmpadr, =loc_6132C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_277C0,7,@call SR_RunTimerDelay|ADR tmp1, loc_277C0_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_277C0_after_call:|@cmp dword [loc_6132C], 0|LDR tmpadr, =loc_6132C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_29910,6,@call SR_RunTimerDelay|ADR tmp1, loc_29910_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_29910_after_call:|@cmp edi, [loc_6132C]|LDR tmpadr, =loc_6132C|ldr tmp1, [tmpadr]|cmp edi, tmp1 @ update timer
loc_3EFB9,7,@call SR_RunTimerDelay|ADR tmp1, loc_3EFB9_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3EFB9_after_call:|@cmp dword [loc_6132C], 0|LDR tmpadr, =loc_6132C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer

loc_113BC,4,@mov eax, ecx|mov eax, ecx|@call SR_feof|ADR tmp1, loc_113BC_after_call|stmfd esp!, {tmp1}|b SR_feof|LTORG_CALL|loc_113BC_after_call:|@test eax, eax|cmp eax, #0 @ fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))
loc_115AE,4,@mov eax, ecx|mov eax, ecx|@call SR_feof|ADR tmp1, loc_115AE_after_call|stmfd esp!, {tmp1}|b SR_feof|LTORG_CALL|loc_115AE_after_call:|@test eax, eax|cmp eax, #0 @ fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))

loc_439B3,5,@mov eax, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr eax, [tmpadr] @ get stdout

loc_151C7,8,@call SR_filelength2|ADR tmp1, loc_151C7_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_151C7_after_call: @ get file length
loc_245C4,8,@call SR_filelength2|ADR tmp1, loc_245C4_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_245C4_after_call: @ get file length
loc_24711,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_24711_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_24711_after_call: @ get file length

loc_1569C,1,@pop edi|ldmfd esp!, {edi}|@call SR_Sync|ADR tmp1, loc_1569C_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_1569C_after_call: @ call sync after save
loc_158F0,1,@pop ebp|ldmfd esp!, {ebp}|@call SR_Sync|ADR tmp1, loc_158F0_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_158F0_after_call: @ call sync after save
loc_15CF5,1,@pop edx|ldmfd esp!, {edx}|@call SR_Sync|ADR tmp1, loc_15CF5_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_15CF5_after_call: @ call sync after save
loc_3A4F2,1,@pop edx|ldmfd esp!, {edx}|@call SR_Sync|ADR tmp1, loc_3A4F2_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_3A4F2_after_call: @ call sync after save

loc_2B26D,2,@call SR_SlowDownMainLoop|ADR tmp1, loc_2B26D_after_call|stmfd esp!, {tmp1}|b SR_SlowDownMainLoop|LTORG_CALL|loc_2B26D_after_call:|@test eax, eax|andS tmp1, eax, eax @ slow down game main loop

loc_2C708,2,@add edx, eax|add edx, edx, eax|@mov esi, 0xffffffff|mvn esi, #0 @ fix reading not reinitialized variable

loc_4F9EB,245,@mov eax, loc_7D510|LDR eax, =loc_7D510|@mov edx, [ebp-12]|ldr edx, [ebp, #(-12)]|@mov ebx, [loc_7D4FC]|LDR tmpadr, =loc_7D4FC|ldr ebx, [tmpadr]|@call SR_ReadSong|ADR tmp1, loc_4F9EB_after_call|stmfd esp!, {tmp1}|b SR_ReadSong|LTORG_CALL|loc_4F9EB_after_call:|@or eax, eax|cmp eax, #0|@jz loc_4FB14|beq loc_4FB14 @ music - load midi or song from catalog

loc_4E285,107, @ disable checks to allow animated video
loc_501C6,3,@add esp, 4|add esp, esp, #4|@call SR_StartAnimVideo|ADR tmp1, loc_501C6_after_call|stmfd esp!, {tmp1}|b SR_StartAnimVideo|LTORG_CALL|loc_501C6_after_call: @ set sound on start of animated video
loc_50239,3,@add esp, 3*4|add esp, esp, #3*4|@call SR_StopAnimVideo|ADR tmp1, loc_50239_after_call|stmfd esp!, {tmp1}|b SR_StopAnimVideo|LTORG_CALL|loc_50239_after_call: @ restore sound on stop of animated video

loc_13B36,1,@cmp dword [Game_PlayIntro], 0|LDR tmpadr, =Game_PlayIntro|ldr tmp1, [tmpadr]|cmp tmp1, #0|@je loc_13C24|beq loc_13C24|@push edi|PUSH_REG edi @ skip intro

loc_57034,2,@adc eax, eax|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adcS eax, eax, eax @ on ARM swap carry bit before instruction ('adc eax, eax' must set flags for 'adc')

loc_5705C,2,@jnb short loc_5706F|bhs loc_5706F|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_572CB,2,@jnb short loc_572D2|bhs loc_572D2|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction
loc_572D1,1,@inc edi|add edi, edi, #1|loc_572D2: @ define label

loc_573D5,3,@adc eax, 0|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adcS eax, eax, #0 @ on ARM swap carry bit before instruction ('adc eax, 0' must set flags for 'adc')
loc_573DB,2,@jnb short loc_573EE|bhs loc_573EE|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_57837,2,@jnb short loc_57840|bhs loc_57840|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_57899,2,@jnb short loc_578A2|bhs loc_578A2|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_57BE6,2,@adc edi, edi|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adc edi, edi, edi @ on ARM swap carry bit before instruction


loc_50C20,2323, @ CLIB code
loc_5158A,45, @ CLIB code
loc_515F1,1623, @ CLIB code
loc_51C65,1110, @ CLIB code
loc_520BC,13012, @ CLIB code
loc_5570C,148, @ CLIB code
loc_585BA,4838, @ CLIB code

loc_60FDC,380, @ CLIB data
loc_6125C,208, @ CLIB data
loc_74E58,656, @ CLIB data
loc_750F0,360, @ CLIB data
loc_752D4,192, @ CLIB data

loc_7D5AC,16, @ CLIB data
loc_7D5C0,176, @ CLIB data
loc_7D670,4096, @ stack
