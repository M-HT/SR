loc_12F95,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_12FA4,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_13086,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_20AD9,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_37639,5,@add eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add eax, eax, tmp1 @ use real address of screen window instead of 0x0a0000
loc_44F51,6,@mov ebx, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr ebx, [tmpadr] @ get real address of screen window instead of 0x0a0000

loc_4AE37,27,@mov eax, ecx|mov eax, ecx @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12638,22,@inc dword [loc_71B30]|LDR tmpadr, =loc_71B30|ldr tmp1, [tmpadr]|add tmp1, tmp1, #1|str tmp1, [tmpadr] @ timer interrupt beginning
loc_12658,31,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_4B8F4,35, @ read sound.cfg
loc_4B91F,21,@call SR_FillSoundCfg|ADR tmp1, loc_4B91F_after_call|stmfd esp!, {tmp1}|b SR_FillSoundCfg|LTORG_CALL|loc_4B91F_after_call:|@add esp, 2*4|add esp, esp, #(2*4) @ read sound.cfg

loc_102D3,7,@call SR_RunTimerDelay|ADR tmp1, loc_102D3_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_102D3_after_call:|@cmp dword [loc_70CA8], 0|LDR tmpadr, =loc_70CA8|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for ??? in timer interrupt
loc_1033C,7,@call SR_RunTimerDelay|ADR tmp1, loc_1033C_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1033C_after_call:|@cmp dword [loc_70CA8], 0|LDR tmpadr, =loc_70CA8|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_374B5,2,@call SR_RunTimerDelay|ADR tmp1, loc_374B5_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_374B5_after_call:|@xor esi, esi|mov esi, #0 @ update timer

loc_292DA,7,@call SR_RunTimerDelay|ADR tmp1, loc_292DA_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_292DA_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for mouse click in timer interrupt
loc_2C2AC,7,@call SR_RunTimerDelay|ADR tmp1, loc_2C2AC_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_2C2AC_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_2C373,7,@call SR_RunTimerDelay|ADR tmp1, loc_2C373_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_2C373_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_36E51,7,@call SR_RunTimerDelay|ADR tmp1, loc_36E51_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_36E51_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3724E,7,@call SR_RunTimerDelay|ADR tmp1, loc_3724E_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3724E_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_37562,7,@call SR_RunTimerDelay|ADR tmp1, loc_37562_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_37562_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_37A79,7,@call SR_RunTimerDelay|ADR tmp1, loc_37A79_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_37A79_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_37F13,7,@call SR_RunTimerDelay|ADR tmp1, loc_37F13_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_37F13_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_384C8,7,@call SR_RunTimerDelay|ADR tmp1, loc_384C8_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_384C8_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_38634,7,@call SR_RunTimerDelay|ADR tmp1, loc_38634_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_38634_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_38975,7,@call SR_RunTimerDelay|ADR tmp1, loc_38975_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_38975_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_38A5D,7,@call SR_RunTimerDelay|ADR tmp1, loc_38A5D_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_38A5D_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_38CEE,7,@call SR_RunTimerDelay|ADR tmp1, loc_38CEE_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_38CEE_after_call:|@cmp dword [loc_70C7C], 0|LDR tmpadr, =loc_70C7C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer

loc_114BE,4,@mov eax, ecx|mov eax, ecx|@call SR_feof|ADR tmp1, loc_114BE_after_call|stmfd esp!, {tmp1}|b SR_feof|LTORG_CALL|loc_114BE_after_call:|@test eax, eax|cmp eax, #0 @ fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))

loc_4D04C,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr
loc_4D05B,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr
loc_4D06C,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr
loc_4D080,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr

loc_1EA60,8,@mov eax, esi|mov eax, esi|@call SR_filelength2|ADR tmp1, loc_1EA60_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_1EA60_after_call: @ get file length
loc_1EC38,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_1EC38_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_1EC38_after_call: @ get file length
loc_1F216,8,@mov eax, edx|mov eax, edx|@call SR_filelength2|ADR tmp1, loc_1F216_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_1F216_after_call: @ get file length
loc_331A1,8,@call SR_filelength2|ADR tmp1, loc_331A1_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_331A1_after_call: @ get file length
loc_33412,13,@mov esi,0x18|mov esi, #0x18|@call SR_filelength2|ADR tmp1, loc_33412_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_33412_after_call: @ get file length
loc_33676,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_33676_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_33676_after_call: @ get file length
loc_4543D,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_4543D_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_4543D_after_call: @ get file length

loc_2BCD1,1,@pop edi|ldmfd esp!, {edi}|@call SR_Sync|ADR tmp1, loc_2BCD1_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_2BCD1_after_call: @ call sync after save
loc_2C563,1,@pop ebp|ldmfd esp!, {ebp}|@call SR_Sync|ADR tmp1, loc_2C563_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_2C563_after_call: @ call sync after save

loc_45091,2,@call SR_SlowDownMainLoop|ADR tmp1, loc_45091_after_call|stmfd esp!, {tmp1}|b SR_SlowDownMainLoop|LTORG_CALL|loc_45091_after_call:|@test eax, eax|andS tmp1, eax, eax @ slow down game main loop

loc_3FE65,3,@sub esp, 0x34|sub esp, esp, #52|@call SR_SlowDownScrolling|ADR tmp1, loc_3FE65_after_call|stmfd esp!, {tmp1}|b SR_SlowDownScrolling|LTORG_CALL|loc_3FE65_after_call: @ slow down game scrolling, ...

loc_43461,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov eax, ecx|mov eax, ecx @ skip game scrolling slow down once
loc_435EF,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov eax, ebx|mov eax, ebx @ skip game scrolling slow down once
loc_4377C,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov eax, ebx|mov eax, ebx @ skip game scrolling slow down once
loc_438DA,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov eax, esi|mov eax, esi @ skip game scrolling slow down once

loc_4BE13,220,@lea eax, [ebp-0x40]|sub eax, ebp, #0x40|@mov edx, [ebp-8]|ldr edx, [ebp, #(-8)]|@mov ebx, [loc_936E0]|LDR tmpadr, =loc_936E0|ldr ebx, [tmpadr]|@call SR_ReadSong|ADR tmp1, loc_4BE13_after_call|stmfd esp!, {tmp1}|b SR_ReadSong|LTORG_CALL|loc_4BE13_after_call:|@or eax, eax|cmp eax, #0|@jz loc_4BF23|beq loc_4BF23 @ music - load midi or song from catalog

loc_408F8,2,@add edx, edx|add edx, edx, edx|@or eax, eax|cmp eax, #0|@jne loc_408F8|bne loc_408F8|@mov eax, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr eax, [tmpadr]|loc_408F8: @ fix reading from NULL pointer

loc_40914,2,@or ebx, ebx|cmp ebx, #0|@jne loc_40914|bne loc_40914|@mov ebx, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr ebx, [tmpadr]|loc_40914:|@add ebx, eax|add ebx, ebx, eax @ fix reading from NULL pointer

loc_23BEB,2,@xor ch, ch|bic ecx, ecx, #0xff00|@or edx, edx|cmp edx, #0|@jne loc_23BEB|bne loc_23BEB|@mov edx, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr edx, [tmpadr]|loc_23BEB: @ fix reading from NULL pointer

loc_2E276,2,@add edx, ecx|add edx, edx, ecx|@movsx eax, word [loc_8D0F6]|LDR tmpadr, =loc_8D0F6|ldrsh eax, [tmpadr]|@movsx ecx, word [loc_8D0F8]|LDR tmpadr, =loc_8D0F8|ldrsh ecx, [tmpadr]|@imul eax, ecx|smull eax, tmp3, ecx, eax|@cmp edx, eax|cmp edx, eax|@jb loc_2E278|blo loc_2E278|@mov eax, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr eax, [tmpadr]|@mov dword [eax], 0|mov ecx, #0|str ecx, [eax]|@jmp short loc_2E286|b loc_2E286|loc_2E278: @ fix out of bounds access
loc_2E286,1,loc_2E286:|@pop esi|POP_REG esi| @ fix out of bounds access

loc_3E4F0,3376,.ltorg @ insert .ltorg after retn


loc_4AE5E,2195, @ CLIB code
loc_4CE4E,500, @ CLIB code
loc_4D08C,661, @ CLIB code
loc_4D3B2,3416, @ CLIB code
loc_4E68E,2037, @ CLIB code
loc_4EE89,6, @ CLIB code
loc_4EEA6,8600, @ CLIB code

loc_74FB8,24, @ CLIB data
loc_75020,564, @ CLIB data
loc_75294,336, @ CLIB data

loc_937C4,16, @ CLIB data
loc_937D8,48, @ CLIB data
loc_93808,4104, @ stack
