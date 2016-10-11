loc_1EC32,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_20FF7,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_29F64,5,@add eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|@add eax, tmp1|add eax, eax, tmp1 @ get real address of screen window instead of 0x0a0000
loc_2A088,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_2AAD4,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_3D8F8,10,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|@mov [loc_5AB08 + 4], eax|LDR tmpadr, =(loc_5AB08 + 4)|str tmp1, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_49174,6,@add edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|@add edi, tmp1|add edi, edi, tmp1 @ get real address of screen window instead of 0x0a0000

loc_21E4E,5,mov eax, edx @ skip drive check ?

loc_440A1,4,nop @ timer interrupt ???
loc_44120,1,@add esp, 8|add esp, esp, #8|@ret|ldmfd esp!, {eip} @ skip calling original timer interrupt
loc_4413C,1,@ret|ldmfd esp!, {eip} @ timer interrupt ending
loc_4413D,7,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_3CE19,66,nop @ keyboard interrupt ending
loc_3CE64,1,@ret|ldmfd esp!, {eip} @ keyboard interrupt ending

loc_21F11,5,@mov ebx, [Game_stdin]|LDR tmpadr, =Game_stdin|ldr ebx, [tmpadr] @ fix stdin

loc_2276F,123,@mov eax, 0x0a|LDR eax, =0x0a|@mov edx, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr edx, [tmpadr]|@call SR_fputc|ADR tmp1, loc_2276F_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_2276F_after_call:|@mov [ebp-0x10], eax|str eax, [ebp, #(-0x10)] @ replace putchar macro
loc_2280E,123,@mov eax, [ebp-0x0c]|ldrb eax, [ebp, #(-0x0c)]|@mov edx, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr edx, [tmpadr]|@call SR_fputc|ADR tmp1, loc_2280E_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_2280E_after_call:|@mov [ebp-0x18], eax|str eax, [ebp, #(-0x18)] @ replace putchar macro
loc_2288E,123,@mov eax, 0x0a|LDR eax, =0x0a|@mov edx, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr edx, [tmpadr]|@call SR_fputc|ADR tmp1, loc_2288E_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_2288E_after_call:|@mov [ebp-0x20], eax|str eax, [ebp, #(-0x20)] @ replace putchar macro

loc_223F9,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr
loc_2E82E,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr
loc_2E8E9,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr
loc_2E8FF,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr

loc_2C193,133,@mov eax, [ebp-0x10]|@inc dword [ebp-0x10]|@mov al, [eax]|@and eax, 0xff|ldr tmp1, [ebp, #(-0x10)]|ldrb eax, [tmp1]|add tmp1, tmp1, #1|str tmp1, [ebp, #(-0x10)]|@mov edx, [ebp-0x0c]|ldr edx, [ebp, #(-0x0c)]|@call SR_fputc|ADR tmp1, loc_2C193_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_2C193_after_call:|@mov [ebp-0x18], eax|str eax, [ebp, #(-0x18)] @ replace putc macro

loc_16D63,1,@pop ebp|ldmfd esp!, {ebp}|@call SR_Sync|ADR tmp1, loc_16D63_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_16D63_after_call: @ call sync after save

loc_3B748,2,@mov eax, 70|mov eax, #70|@call SR___delay|ADR tmp1, loc_3B748_after_call|stmfd esp!, {tmp1}|b SR___delay|LTORG_CALL|loc_3B748_after_call:|@xor eax, eax|mov eax, #0 @ insert delay into victory screen

loc_4D17B,10,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_4D1D8,11,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_4D272,10,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_4D2B9,11,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))

loc_33C45,3,@or eax, eax|@je loc_33C48|@mov al, [eax+0x1b]|@loc_33C48:|cmp eax, #0|ldrneb eax, [eax, #27] @ fix reading from NULL pointer
loc_33C55,3,@or eax, eax|@je loc_33C58|@mov al, [eax+0x1b]|@loc_33C58:|cmp eax, #0|ldrneb eax, [eax, #27] @ fix reading from NULL pointer

loc_22A15,5,@xor eax, eax|mov eax, #0 @ set default? config - no sound, no music
loc_22A2E,5,@call SR_memset|ADR tmp1, loc_22A2E_after_call|stmfd esp!, {tmp1}|b SR_memset|LTORG_CALL|loc_22A2E_after_call:|@cmp dword [Game_Sound], 0|LDR tmpadr, =Game_Sound|ldr tmp1, [tmpadr]|cmp tmp1, #0|@je loc_22A2E_1|beq loc_22A2E_1|@mov word [loc_597DE], 0|LDR tmpadr, =loc_597DE|mov tmp2, #0|strh tmp2, [tmpadr]|loc_22A2E_1:|@cmp dword [Game_Music], 0|LDR tmpadr, =Game_Music|ldr tmp1, [tmpadr]|cmp tmp1, #0|@je loc_22A2E_2|beq loc_22A2E_2|@mov word [loc_597EE], 0|LDR tmpadr, =loc_597EE|mov tmp2, #0|strh tmp2, [tmpadr]|loc_22A2E_2: @ enable/disable sound,music driver

loc_2F395,15,@mov eax, 1|mov eax, #1 @ fix loading sound driver

loc_2EFB5,15,@mov eax, 1|mov eax, #1 @ fix loading music driver

loc_46545,4,@mov eax, [esp+0x14]|ldr eax, [esp, #0x14]|@or eax, eax|cmp eax, #0|@je loc_46570|beq loc_46570 @ fix loading NULL samples
loc_46570,5,loc_46570:|@mov ecx, 9|mov ecx, #9 @ fix loading NULL samples


loc_26BF5,6,@xor eax, eax|mov eax, #0 @ disable network (IPX) game
loc_26BAE,6,@xor eax, eax|mov eax, #0 @ disable modem/direct connection game
loc_18898,5,nop @ disable network/modem/direct connection game

loc_44455,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
loc_444A6,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
loc_44509,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
loc_4456C,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
