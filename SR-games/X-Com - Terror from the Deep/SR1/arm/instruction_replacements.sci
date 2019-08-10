loc_44128,6,@mov ebx, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr ebx, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_12F34,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_3FB4F,5,@add eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add eax, eax, tmp1 @ use real address of screen window instead of 0x0a0000
loc_4F5D2,10,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr]|@mov [loc_7CF34], eax|LDR tmpadr, =loc_7CF34|str eax, [tmpadr] @ use real address of screen window instead of 0x0a0000
loc_4FA48,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000

loc_4DD6F,27,@mov eax, ecx|mov eax, ecx @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address
loc_4E776,27,@mov eax, ecx|mov eax, ecx @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12548,22,@inc dword [loc_62210]|LDR tmpadr, =loc_62210|ldr tmp1, [tmpadr]|add tmp1, tmp1, #1|str tmp1, [tmpadr] @ timer interrupt beginning
loc_12568,31,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_503FE,35, @ read sound.cfg
loc_50429,21,@call SR_FillSoundCfg|ADR tmp1, loc_50429_after_call|stmfd esp!, {tmp1}|b SR_FillSoundCfg|LTORG_CALL|loc_50429_after_call:|@add esp, 2*4|add esp, esp, #(2*4) @ read sound.cfg

loc_102D3,7,@call SR_RunTimerDelay|ADR tmp1, loc_102D3_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_102D3_after_call:|@cmp dword [loc_61388], 0|LDR tmpadr, =loc_61388|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for ??? in timer interrupt
loc_1033C,7,@call SR_RunTimerDelay|ADR tmp1, loc_1033C_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1033C_after_call:|@cmp dword [loc_61388], 0|LDR tmpadr, =loc_61388|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3F9F5,2,@call SR_RunTimerDelay|ADR tmp1, loc_3F9F5_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3F9F5_after_call:|@xor ebx, ebx|mov ebx, #0 @ update timer

loc_1E2E1,7,@call SR_RunTimerDelay|ADR tmp1, loc_1E2E1_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1E2E1_after_call:|@cmp dword [loc_6135C], 0|LDR tmpadr, =loc_6135C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for mouse click in timer interrupt
loc_26953,7,@call SR_RunTimerDelay|ADR tmp1, loc_26953_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_26953_after_call:|@cmp dword [loc_6135C], 0|LDR tmpadr, =loc_6135C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_27379,7,@call SR_RunTimerDelay|ADR tmp1, loc_27379_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_27379_after_call:|@cmp dword [loc_6135C], 0|LDR tmpadr, =loc_6135C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_27A8E,7,@call SR_RunTimerDelay|ADR tmp1, loc_27A8E_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_27A8E_after_call:|@cmp dword [loc_6135C], 0|LDR tmpadr, =loc_6135C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_27B93,7,@call SR_RunTimerDelay|ADR tmp1, loc_27B93_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_27B93_after_call:|@cmp dword [loc_6135C], 0|LDR tmpadr, =loc_6135C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_27C3D,7,@call SR_RunTimerDelay|ADR tmp1, loc_27C3D_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_27C3D_after_call:|@cmp dword [loc_6135C], 0|LDR tmpadr, =loc_6135C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_29C10,6,@call SR_RunTimerDelay|ADR tmp1, loc_29C10_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_29C10_after_call:|@cmp edi, [loc_6135C]|LDR tmpadr, =loc_6135C|ldr tmp1, [tmpadr]|cmp edi, tmp1 @ update timer
loc_3FD37,7,@call SR_RunTimerDelay|ADR tmp1, loc_3FD37_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3FD37_after_call:|@cmp dword [loc_6135C], 0|LDR tmpadr, =loc_6135C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer

loc_113CC,4,@mov eax, ecx|mov eax, ecx|@call SR_feof|ADR tmp1, loc_113CC_after_call|stmfd esp!, {tmp1}|b SR_feof|LTORG_CALL|loc_113CC_after_call:|@test eax, eax|cmp eax, #0 @ fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))
loc_115BE,4,@mov eax, ecx|mov eax, ecx|@call SR_feof|ADR tmp1, loc_115BE_after_call|stmfd esp!, {tmp1}|b SR_feof|LTORG_CALL|loc_115BE_after_call:|@test eax, eax|cmp eax, #0 @ fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))

loc_44893,5,@mov eax, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr eax, [tmpadr] @ get stdout

loc_15237,8,@call SR_filelength2|ADR tmp1, loc_15237_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_15237_after_call: @ get file length
loc_248C4,8,@call SR_filelength2|ADR tmp1, loc_248C4_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_248C4_after_call: @ get file length
loc_249E9,8,@call SR_filelength2|ADR tmp1, loc_249E9_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_249E9_after_call: @ get file length
loc_24B31,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_24B31_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_24B31_after_call: @ get file length

loc_1571E,1,@pop edi|ldmfd esp!, {edi}|@call SR_Sync|ADR tmp1, loc_1571E_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_1571E_after_call: @ call sync after save
loc_1596D,1,@pop ebp|ldmfd esp!, {ebp}|@call SR_Sync|ADR tmp1, loc_1596D_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_1596D_after_call: @ call sync after save
loc_15D56,1,@pop edx|ldmfd esp!, {edx}|@call SR_Sync|ADR tmp1, loc_15D56_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_15D56_after_call: @ call sync after save
loc_3AE62,1,@pop edx|ldmfd esp!, {edx}|@call SR_Sync|ADR tmp1, loc_3AE62_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_3AE62_after_call: @ call sync after save

loc_2B5AD,2,@call SR_SlowDownMainLoop|ADR tmp1, loc_2B5AD_after_call|stmfd esp!, {tmp1}|b SR_SlowDownMainLoop|LTORG_CALL|loc_2B5AD_after_call:|@test eax, eax|andS tmp1, eax, eax @ slow down game main loop

loc_2CA8E,2,@add edx, eax|add edx, edx, eax|@mov esi, 0xffffffff|mvn esi, #0 @ fix reading not reinitialized variable

loc_50B5E,245,@mov eax, loc_7D540|LDR eax, =loc_7D540|@mov edx, [ebp-12]|ldr edx, [ebp, #(-12)]|@mov ebx, [loc_7D52C]|LDR tmpadr, =loc_7D52C|ldr ebx, [tmpadr]|@call SR_ReadSong|ADR tmp1, loc_50B5E_after_call|stmfd esp!, {tmp1}|b SR_ReadSong|LTORG_CALL|loc_50B5E_after_call:|@or eax, eax|cmp eax, #0|@jz loc_50C87|beq loc_50C87 @ music - load midi or song from catalog

loc_581E0,2,@adc eax, eax|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adcS eax, eax, eax @ on ARM swap carry bit before instruction ('adc eax, eax' must set flags for 'adc')

loc_58208,2,@jnb short loc_5821B|bhs loc_5821B|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_58477,2,@jnb short loc_5847E|bhs loc_5847E|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction
loc_5847D,1,@inc edi|add edi, edi, #1|loc_5847E: @ define label

loc_58581,3,@adc eax, 0|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adcS eax, eax, #0 @ on ARM swap carry bit before instruction ('adc eax, 0' must set flags for 'adc')
loc_58587,2,@jnb short loc_5859A|bhs loc_5859A|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_589E3,2,@jnb short loc_589EC|bhs loc_589EC|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_58A45,2,@jnb short loc_58A4E|bhs loc_58A4E|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_58D92,2,@adc edi, edi|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adc edi, edi, edi @ on ARM swap carry bit before instruction


loc_51DB9,2323, @ CLIB code
loc_52723,45, @ CLIB code
loc_5278A,1614, @ CLIB code
loc_52DF5,1110, @ CLIB code
loc_5324C,13043, @ CLIB code
loc_568B8,148, @ CLIB code
loc_59766,4842, @ CLIB code

loc_6100C,380, @ CLIB data
loc_6128C,208, @ CLIB data
loc_74E88,656, @ CLIB data
loc_75120,360, @ CLIB data
loc_75304,192, @ CLIB data

loc_7D5DC,16, @ CLIB data
loc_7D5F0,176, @ CLIB data
loc_7D6A0,4096, @ stack
