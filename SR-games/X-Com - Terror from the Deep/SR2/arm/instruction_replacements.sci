loc_12EA5,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_12EB4,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_12F96,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_20FF1,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_3A4EF,5,@add eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add eax, eax, tmp1 @ use real address of screen window instead of 0x0a0000
loc_49378,6,@mov ebx, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr ebx, [tmpadr] @ get real address of screen window instead of 0x0a0000

loc_4E787,27,@mov eax, ecx|mov eax, ecx @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12548,22,@inc dword [loc_71B90]|LDR tmpadr, =loc_71B90|ldr tmp1, [tmpadr]|add tmp1, tmp1, #1|str tmp1, [tmpadr] @ timer interrupt beginning
loc_12568,31,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_4F0D2,35, @ read sound.cfg
loc_4F0FD,21,@call SR_FillSoundCfg|ADR tmp1, loc_4F0FD_after_call|stmfd esp!, {tmp1}|b SR_FillSoundCfg|LTORG_CALL|loc_4F0FD_after_call:|@add esp, 2*4|add esp, esp, #(2*4) @ read sound.cfg

loc_102D3,7,@call SR_RunTimerDelay|ADR tmp1, loc_102D3_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_102D3_after_call:|@cmp dword [loc_70D08], 0|LDR tmpadr, =loc_70D08|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for ??? in timer interrupt
loc_1033C,7,@call SR_RunTimerDelay|ADR tmp1, loc_1033C_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1033C_after_call:|@cmp dword [loc_70D08], 0|LDR tmpadr, =loc_70D08|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3A374,2,@call SR_RunTimerDelay|ADR tmp1, loc_3A374_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3A374_after_call:|@xor esi, esi|mov esi, #0 @ update timer

loc_2A04A,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for mouse click in timer interrupt
loc_2DC4C,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_2DD19,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_31C62,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_33AA5,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_39CE2,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3A0F9,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3A41E,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3A909,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3ADD0,6,@call SR_RunTimerDelay||@cmp ebx, [loc_70CDC]|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp ebx, tmp1 @ update timer
loc_3B3FF,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3B570,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3B89D,6,@call SR_RunTimerDelay||@cmp eax, [loc_70CDC]|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp eax, tmp1 @ update timer
loc_3B995,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3BC84,7,@call SR_RunTimerDelay||@cmp dword [loc_70CDC], 0|LDR tmpadr, =loc_70CDC|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer

loc_113CC,4,@mov eax, ecx|mov eax, ecx|@call SR_feof|ADR tmp1, loc_113CC_after_call|stmfd esp!, {tmp1}|b SR_feof|LTORG_CALL|loc_113CC_after_call:|@test eax, eax|cmp eax, #0 @ fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))

loc_51495,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr
loc_514A4,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr
loc_514B5,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr
loc_514C9,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr

loc_1EC4E,8,@mov eax, esi|mov eax, esi|@call SR_filelength2|ADR tmp1, loc_1EC4E_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_1EC4E_after_call: @ get file length
loc_1EE51,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_1EE51_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_1EE51_after_call: @ get file length
loc_1F6B9,8,@mov eax, ecx|mov eax, ecx|@call SR_filelength2|ADR tmp1, loc_1F6B9_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_1F6B9_after_call: @ get file length
loc_2F5E7,8,@call SR_filelength2|ADR tmp1, loc_2F5E7_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_2F5E7_after_call: @ get file length
loc_35D15,10,@xor edx, edx|mov edx, #0|@call SR_filelength2|ADR tmp1, loc_35D15_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_35D15_after_call: @ get file length
loc_35F96,13,@mov esi,0x18| mov esi, #0x18|@call SR_filelength2|ADR tmp1, loc_35F96_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_35F96_after_call: @ get file length
loc_361BE,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_361BE_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_361BE_after_call: @ get file length

loc_2D5F4,1,@pop edi|ldmfd esp!, {edi}|@call SR_Sync|ADR tmp1, loc_2D5F4_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_2D5F4_after_call: @ call sync after save
loc_2DEFC,1,@pop ebp|ldmfd esp!, {ebp}|@call SR_Sync|ADR tmp1, loc_2DEFC_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_2DEFC_after_call: @ call sync after save

loc_49460,2,@call SR_SlowDownMainLoop|ADR tmp1, loc_49460_after_call|stmfd esp!, {tmp1}|b SR_SlowDownMainLoop|LTORG_CALL|loc_49460_after_call:|@test eax, eax|andS tmp1, eax, eax @ slow down game main loop

loc_43E95,3,@sub esp, 0x3c|sub esp, esp, #60|@call SR_SlowDownScrolling|ADR tmp1, loc_43E95_after_call|stmfd esp!, {tmp1}|b SR_SlowDownScrolling|LTORG_CALL|loc_43E95_after_call: @ slow down game scrolling, ...

loc_47876,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov edx, ecx|mov edx, ecx @ skip game scrolling slow down once
loc_47A0F,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov edx, esi|mov edx, esi @ skip game scrolling slow down once
loc_47BA5,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov edx, ecx|mov edx, ecx @ skip game scrolling slow down once
loc_47D11,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov eax, esi|mov eax, esi @ skip game scrolling slow down once

loc_4F829,245,@mov eax, loc_A344C|LDR eax, =loc_A344C|@mov edx, [ebp-12]|ldr edx, [ebp, #(-12)]|@mov ebx, [loc_A3434]|LDR tmpadr, =loc_A3434|ldr ebx, [tmpadr]|@call SR_ReadSong|ADR tmp1, loc_4F829_after_call|stmfd esp!, {tmp1}|b SR_ReadSong|LTORG_CALL|loc_4F829_after_call:|@or eax, eax|cmp eax, #0|@jz loc_4F952|beq loc_4F952 @ music - load midi or song from catalog

loc_42C02,6,@mov ebp, [loc_93B1C]|LDR tmpadr, =loc_93B1C|ldr ebp, [tmpadr]|@or ebp, ebp|cmp ebp, #0|@jne loc_42C02|bne loc_42C02|@mov ebp, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr ebp, [tmpadr]|loc_42C02: @ fix reading from NULL pointer

loc_44A84,2,@mov al, bl|and eax, ebx, #0xff|@or edx, edx|cmp edx, #0|@jne loc_44A84|bne loc_44A84|@mov edx, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr edx, [tmpadr]|loc_44A84: @ fix reading from NULL pointer (potential)

loc_44A99,2,@or ebx, ebx|cmp ebx, #0|@jne loc_44A99|bne loc_44A99|@mov ebx, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr ebx, [tmpadr]|loc_44A99:|@add ebx, ecx|add ebx, ebx, ecx @ fix reading from NULL pointer (potential)

loc_240BB,2,@xor ch, ch|bic ecx, ecx, #0xff00|@or edx, edx|cmp edx, #0|@jne loc_240BB|bne loc_240BB|@mov edx, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr edx, [tmpadr]|loc_240BB: @ fix reading from NULL pointer (potential)

loc_2FED3,2,@add edx, esi|add edx, edx, esi|@movsx eax, word [loc_95656]|LDR tmpadr, =loc_95656|ldrsh eax, [tmpadr]|@movsx ecx, word [loc_95658]|LDR tmpadr, =loc_95658|ldrsh ecx, [tmpadr]|@imul eax, ecx|smull eax, tmp3, ecx, eax|@cmp edx, eax|cmp edx, eax|@jb loc_2FED5|blo loc_2FED5|@mov eax, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr eax, [tmpadr]|@mov dword [eax], 0|mov ecx, #0|str ecx, [eax]|@jmp short loc_2FEE3|b loc_2FEE3|loc_2FED5: @ fix out of bounds access
loc_2FEE3,1,loc_2FEE3:|@pop esi|POP_REG esi| @ fix out of bounds access

loc_417C1,3807,.ltorg @ insert .ltorg after retn


loc_50B9E,2285, @ CLIB code
loc_514D5,661, @ CLIB code
loc_517FB,102, @ CLIB code
loc_5187C,3822, @ CLIB code
loc_52770,6, @ CLIB code
loc_5278D,5333, @ CLIB code
loc_53C8A,6117, @ CLIB code

loc_7587C,664, @ CLIB data
loc_75B54,336, @ CLIB data

loc_A34E8,12, @ CLIB data
loc_A34F8,52, @ CLIB data
loc_A352C,4100, @ stack
