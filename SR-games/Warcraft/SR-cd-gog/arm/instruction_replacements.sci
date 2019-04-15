loc_1AE00,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_1C91B,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_22FCB,5,@add eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add eax, eax, tmp1 @ get real address of screen window instead of 0x0a0000
loc_23060,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_2378A,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_31D68,10,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|@mov [loc_5ACE0 + 4], eax|LDR tmpadr, =(loc_5ACE0 + 4)|str tmp1, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_3D584,6,@add edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add edi, edi, tmp1 @ get real address of screen window instead of 0x0a0000

loc_384F1,4,nop @ timer interrupt ???
loc_38570,1,@add esp, 8|add esp, esp, #8|@ret|ldmfd esp!, {eip} @ skip calling original timer interrupt
loc_3858C,1,@ret|ldmfd esp!, {eip} @ timer interrupt ending
loc_3858D,7,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_31289,66,nop @ keyboard interrupt ending
loc_312D4,1,@ret|ldmfd esp!, {eip} @ keyboard interrupt ending

loc_1D880,14, @ replace putchar macro (part 1)
loc_1D893,96,@mov eax, 0x0a|mov eax, #0x0a|@mov edx, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr edx, [tmpadr]|@call SR_fputc|ADR tmp1, loc_1D893_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_1D893_after_call:|@jmp short loc_1D95F|b loc_1D95F @ replace putchar macro (part 2)
loc_1D90B,84,@mov edx, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr edx, [tmpadr]|@call SR_fputc|ADR tmp1, loc_1D90B_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_1D90B_after_call: @ replace putchar macro
loc_1D96B,98,@mov eax, 0x0a|mov eax, #0x0a|@mov edx, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr edx, [tmpadr]|@call SR_fputc|ADR tmp1, loc_1D96B_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_1D96B_after_call: @ replace putchar macro

loc_1D601,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr
loc_26572,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr
loc_26618,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr
loc_2662C,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr

loc_24A05,80,@xor eax, eax|@mov al, [ecx]|ldrb eax, [ecx]|@inc ecx|add ecx, ecx, #1|@mov edx, ebx|mov edx, ebx|@call SR_fputc|ADR tmp1, loc_24A05_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_24A05_after_call: @ replace putc macro

loc_14C6D,1,@pop ebp|ldmfd esp!, {ebp}|@call SR_Sync|ADR tmp1, loc_14C6D_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_14C6D_after_call: @ call sync after save

loc_2FC1B,2,@mov eax, 70|mov eax, #70|@call SR___delay|ADR tmp1, loc_2FC1B_after_call|stmfd esp!, {tmp1}|b SR___delay|LTORG_CALL|loc_2FC1B_after_call:|@xor eax, eax|mov eax, #0 @ insert delay into victory screen

loc_4158B,10,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_415E8,11,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_41682,10,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_416C9,11,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))

loc_2A07F,4,@xor edi, edi|mov edi, #0|@or ebp, ebp|cmp ebp, #0|@je loc_2A083|@movzx edi, byte [ebp+0x1b]|@loc_2A083:|ldrneb edi, [ebp, #27] @ fix reading from NULL pointer

loc_1DAA2,5,@xor eax, eax|mov eax, #0 @ set default? config - no sound, no music
loc_1DABB,5,@call SR_memset|ADR tmp1, loc_1DABB_after_call|stmfd esp!, {tmp1}|b SR_memset|LTORG_CALL|loc_1DABB_after_call:|@cmp dword [Game_Sound], 0|LDR tmpadr, =Game_Sound|ldr tmp1, [tmpadr]|cmp tmp1, #0|@je loc_1DABB_1|beq loc_1DABB_1|@mov word [loc_59902], 0|LDR tmpadr, =loc_59902|mov tmp2, #0|strh tmp2, [tmpadr]|loc_1DABB_1:|@cmp dword [Game_Music], 0|LDR tmpadr, =Game_Music|ldr tmp1, [tmpadr]|cmp tmp1, #0|@je loc_1DABB_2|beq loc_1DABB_2|@mov word [loc_59912], 0|LDR tmpadr, =loc_59912|mov tmp2, #0|strh tmp2, [tmpadr]|loc_1DABB_2: @ enable/disable sound,music driver

loc_26F65,15,@mov eax, 1|mov eax, #1 @ fix loading sound driver

loc_26B85,15,@mov eax, 1|mov eax, #1 @ fix loading music driver

loc_3A995,4,@mov eax, [esp+0x14]|ldr eax, [esp, #0x14]|@or eax, eax|cmp eax, #0|@je loc_3A9C0|beq loc_3A9C0 @ fix loading NULL samples
loc_3A9C0,5,loc_3A9C0:|@mov ecx, 9|mov ecx, #9 @ fix loading NULL samples


loc_20B20,6,@xor eax, eax|mov eax, #0 @ disable network (IPX) game
loc_20ADD,6,@xor eax, eax|mov eax, #0 @ disable modem/direct connection game
loc_16174,5,nop @ disable network/modem/direct connection game

loc_388A5,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
loc_388F6,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
loc_38959,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
loc_389BC,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
