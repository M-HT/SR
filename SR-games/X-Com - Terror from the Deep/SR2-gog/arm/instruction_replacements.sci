loc_12E95,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_12EA4,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_12F86,5,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_20C17,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_396DF,5,@add eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add eax, eax, tmp1 @ use real address of screen window instead of 0x0a0000
loc_48128,6,@mov ebx, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr ebx, [tmpadr] @ get real address of screen window instead of 0x0a0000

loc_4D383,27,@mov eax, ecx|mov eax, ecx @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12538,22,@inc dword [loc_71BF4]|LDR tmpadr, =loc_71BF4|ldr tmp1, [tmpadr]|add tmp1, tmp1, #1|str tmp1, [tmpadr] @ timer interrupt beginning
loc_12558,31,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_4DCBD,35, @ read sound.cfg
loc_4DCE8,21,@call SR_FillSoundCfg|ADR tmp1, loc_4DCE8_after_call|stmfd esp!, {tmp1}|b SR_FillSoundCfg|LTORG_CALL|loc_4DCE8_after_call:|@add esp, 2*4|add esp, esp, #(2*4) @ read sound.cfg

loc_102D3,7,@call SR_RunTimerDelay|ADR tmp1, loc_102D3_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_102D3_after_call:|@cmp dword [loc_70D6C], 0|LDR tmpadr, =loc_70D6C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for ??? in timer interrupt
loc_1033C,7,@call SR_RunTimerDelay|ADR tmp1, loc_1033C_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_1033C_after_call:|@cmp dword [loc_70D6C], 0|LDR tmpadr, =loc_70D6C|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_39564,2,@call SR_RunTimerDelay|ADR tmp1, loc_39564_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_39564_after_call:|@xor esi, esi|mov esi, #0 @ update timer

loc_29B4A,7,@call SR_RunTimerDelay|ADR tmp1, loc_29B4A_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_29B4A_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer - wait for mouse click in timer interrupt
loc_2D52C,7,@call SR_RunTimerDelay|ADR tmp1, loc_2D52C_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_2D52C_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_2D5F9,7,@call SR_RunTimerDelay|ADR tmp1, loc_2D5F9_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_2D5F9_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_31104,7,@call SR_RunTimerDelay|ADR tmp1, loc_31104_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_31104_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_32F5F,7,@call SR_RunTimerDelay|ADR tmp1, loc_32F5F_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_32F5F_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_38EF8,7,@call SR_RunTimerDelay|ADR tmp1, loc_38EF8_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_38EF8_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_39307,7,@call SR_RunTimerDelay|ADR tmp1, loc_39307_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_39307_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3960E,7,@call SR_RunTimerDelay|ADR tmp1, loc_3960E_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3960E_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_39AE9,7,@call SR_RunTimerDelay|ADR tmp1, loc_39AE9_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_39AE9_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_39FA7,6,@call SR_RunTimerDelay|ADR tmp1, loc_39FA7_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_39FA7_after_call:|@cmp ecx, [loc_70D40]|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp ecx, tmp1 @ update timer
loc_3A5CF,7,@call SR_RunTimerDelay|ADR tmp1, loc_3A5CF_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3A5CF_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3A740,7,@call SR_RunTimerDelay|ADR tmp1, loc_3A740_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3A740_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3AA69,6,@call SR_RunTimerDelay|ADR tmp1, loc_3AA69_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3AA69_after_call:|@cmp eax, [loc_70D40]|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp eax, tmp1 @ update timer
loc_3AB50,7,@call SR_RunTimerDelay|ADR tmp1, loc_3AB50_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3AB50_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer
loc_3ADE4,7,@call SR_RunTimerDelay|ADR tmp1, loc_3ADE4_after_call|stmfd esp!, {tmp1}|b SR_RunTimerDelay|LTORG_CALL|loc_3ADE4_after_call:|@cmp dword [loc_70D40], 0|LDR tmpadr, =loc_70D40|ldr tmp1, [tmpadr]|cmp tmp1, #0 @ update timer

loc_113BC,4,@mov eax, ecx|mov eax, ecx|@call SR_feof|ADR tmp1, loc_113BC_after_call|stmfd esp!, {tmp1}|b SR_feof|LTORG_CALL|loc_113BC_after_call:|@test eax, eax|cmp eax, #0 @ fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))

loc_50162,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr
loc_50171,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr
loc_50182,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr
loc_50196,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ get stderr

loc_1E957,8,@mov eax, esi|mov eax, esi|@call SR_filelength2|ADR tmp1, loc_1E957_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_1E957_after_call: @ get file length
loc_1EB31,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_1EB31_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_1EB31_after_call: @ get file length
loc_1F392,8,@mov eax, edx|mov eax, edx|@call SR_filelength2|ADR tmp1, loc_1F392_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_1F392_after_call: @ get file length
loc_35131,8,@call SR_filelength2|ADR tmp1, loc_35131_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_35131_after_call: @ get file length
loc_353B5,13,@mov esi,0x18|mov esi, #0x18|@call SR_filelength2|ADR tmp1, loc_353B5_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_353B5_after_call: @ get file length
loc_355DE,13,@mov edx,1|mov edx, #1|@call SR_filelength2|ADR tmp1, loc_355DE_after_call|stmfd esp!, {tmp1}|b SR_filelength2|LTORG_CALL|loc_355DE_after_call: @ get file length

loc_2CEF4,1,@pop edi|ldmfd esp!, {edi}|@call SR_Sync|ADR tmp1, loc_2CEF4_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_2CEF4_after_call: @ call sync after save
loc_2D7DE,1,@pop edi|ldmfd esp!, {edi}|@call SR_Sync|ADR tmp1, loc_2D7DE_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_2D7DE_after_call: @ call sync after save

loc_48210,2,@call SR_SlowDownMainLoop|ADR tmp1, loc_48210_after_call|stmfd esp!, {tmp1}|b SR_SlowDownMainLoop|LTORG_CALL|loc_48210_after_call:|@test eax, eax|andS tmp1, eax, eax @ slow down game main loop

loc_42E55,3,@sub esp, 0x3c|sub esp, esp, #60|@call SR_SlowDownScrolling|ADR tmp1, loc_42E55_after_call|stmfd esp!, {tmp1}|b SR_SlowDownScrolling|LTORG_CALL|loc_42E55_after_call: @ slow down game scrolling, ...

loc_4668C,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov edx, ecx|mov edx, ecx @ skip game scrolling slow down once
loc_4681A,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov edx, ebx|mov edx, ebx @ skip game scrolling slow down once
loc_469B0,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov eax, ecx|mov eax, ecx @ skip game scrolling slow down once
loc_46B10,2,@mov dword [Game_Skip_Scrolling_SlowDown], 1|LDR tmpadr, =Game_Skip_Scrolling_SlowDown|mov tmp1, #1|str tmp1, [tmpadr]|@mov edx, esi|mov edx, esi @ skip game scrolling slow down once

loc_4E40A,245,@mov eax, loc_A499C|LDR eax, =loc_A499C|@mov edx, [ebp-12]|ldr edx, [ebp, #(-12)]|@mov ebx, [loc_A4984]|LDR tmpadr, =loc_A4984|ldr ebx, [tmpadr]|@call SR_ReadSong|ADR tmp1, loc_4E40A_after_call|stmfd esp!, {tmp1}|b SR_ReadSong|LTORG_CALL|loc_4E40A_after_call:|@or eax, eax|cmp eax, #0|@jz loc_4E533|beq loc_4E533 @ music - load midi or song from catalog

loc_41BDD,6,@mov ebp, [loc_95370]|LDR tmpadr, =loc_95370|ldr ebp, [tmpadr]|@or ebp, ebp|cmp ebp, #0|@jne loc_41BDD|bne loc_41BDD|@mov ebp, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr ebp, [tmpadr]|loc_41BDD: @ fix reading from NULL pointer

loc_439D8,2,@mov al, bl|and eax, ebx, #0xff|@or edx, edx|cmp edx, #0|@jne loc_439D8|bne loc_439D8|@mov edx, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr edx, [tmpadr]|loc_439D8: @ fix reading from NULL pointer (potential)

loc_439EB,2,@or ebx, ebx|cmp ebx, #0|@jne loc_439EB|bne loc_439EB|@mov ebx, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr ebx, [tmpadr]|loc_439EB:|@add ebx, ecx|add ebx, ebx, ecx @ fix reading from NULL pointer (potential)

loc_23C8B,2,@xor ch, ch|bic ecx, ecx, #0xff00|@or edx, edx|cmp edx, #0|@jne loc_23C8B|bne loc_23C8B|@mov edx, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr edx, [tmpadr]|loc_23C8B: @ fix reading from NULL pointer (potential)

loc_2F583,2,@add edx, esi|add edx, edx, esi|@movsx eax, word [loc_9543A]|LDR tmpadr, =loc_9543A|ldrsh eax, [tmpadr]|@movsx ecx, word [loc_9543C]|LDR tmpadr, =loc_9543C|ldrsh ecx, [tmpadr]|@imul eax, ecx|smull eax, tmp3, ecx, eax|@cmp edx, eax|cmp edx, eax|@jb loc_2F585|blo loc_2F585|@mov eax, [Zero_Segment]|LDR tmpadr, =Zero_Segment|ldr eax, [tmpadr]|@mov dword [eax], 0|mov ecx, #0|str ecx, [eax]|@jmp short loc_2F593|b loc_2F593|loc_2F585: @ fix out of bounds access
loc_2F593,1,loc_2F593:|@pop esi|POP_REG esi| @ fix out of bounds access

loc_407D1,3631,.ltorg @ insert .ltorg after retn


loc_4F759,2559, @ CLIB code
loc_501A2,661, @ CLIB code
loc_504C8,5455, @ CLIB code
loc_51A1D,6, @ CLIB code
loc_51A3A,8590, @ CLIB code

loc_756F8,24, @ CLIB data
loc_75714,640, @ CLIB data
loc_759D4,336, @ CLIB data

loc_A4A38,16, @ CLIB data
loc_A4A4C,48, @ CLIB data
loc_A4A7C,4100, @ stack
