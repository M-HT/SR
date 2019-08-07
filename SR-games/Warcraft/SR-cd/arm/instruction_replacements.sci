loc_1AE00,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_1C91B,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_230FB,5,@add eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add eax, eax, tmp1 @ get real address of screen window instead of 0x0a0000
loc_23190,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_238BA,5,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_31E85,10,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|@mov [loc_5AE20 + 4], eax|LDR tmpadr, =(loc_5AE20 + 4)|str tmp1, [tmpadr] @ get real address of screen window instead of 0x0a0000
loc_3D5F2,6,@add edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add edi, edi, tmp1 @ get real address of screen window instead of 0x0a0000

loc_1D2E8,5,mov eax, edx @ skip drive check ?

loc_385E1,4,nop @ timer interrupt ???
loc_38660,1,@add esp, 8|add esp, esp, #8|@ret|ldmfd esp!, {eip} @ skip calling original timer interrupt
loc_3867C,1,@ret|ldmfd esp!, {eip} @ timer interrupt ending
loc_3867D,7,@ret|ldmfd esp!, {eip} @ timer interrupt ending

loc_313B9,66,nop @ keyboard interrupt ending
loc_31404,1,@ret|ldmfd esp!, {eip} @ keyboard interrupt ending

loc_1D37B,5,@mov ebx, [Game_stdin]|LDR tmpadr, =Game_stdin|ldr ebx, [tmpadr] @ fix stdin

loc_1D9B4,14, @ replace putchar macro (part 1)
loc_1D9C7,96,@mov eax, 0x0a|mov eax, #0x0a|@mov edx, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr edx, [tmpadr]|@call SR_fputc|ADR tmp1, loc_1D9C7_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_1D9C7_after_call:|@jmp short loc_1DA93|b loc_1DA93 @ replace putchar macro (part 2)
loc_1DA3F,84,@mov edx, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr edx, [tmpadr]|@call SR_fputc|ADR tmp1, loc_1DA3F_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_1DA3F_after_call: @ replace putchar macro
loc_1DA9F,98,@mov eax, 0x0a|mov eax, #0x0a|@mov edx, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr edx, [tmpadr]|@call SR_fputc|ADR tmp1, loc_1DA9F_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_1DA9F_after_call: @ replace putchar macro

loc_1D735,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr
loc_266A2,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr
loc_26748,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr
loc_2675C,5,@mov edx, [Game_stderr]|LDR tmpadr, =Game_stderr|ldr edx, [tmpadr] @ fix stderr

loc_24B35,80,@xor eax, eax|@mov al, [ecx]|ldrb eax, [ecx]|@inc ecx|add ecx, ecx, #1|@mov edx, ebx|mov edx, ebx|@call SR_fputc|ADR tmp1, loc_24B35_after_call|stmfd esp!, {tmp1}|b SR_fputc|LTORG_CALL|loc_24B35_after_call: @ replace putc macro

loc_14C6D,1,@pop ebp|ldmfd esp!, {ebp}|@call SR_Sync|ADR tmp1, loc_14C6D_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_14C6D_after_call: @ call sync after save

loc_2FD4B,2,@mov eax, 70|mov eax, #70|@call SR___delay|ADR tmp1, loc_2FD4B_after_call|stmfd esp!, {tmp1}|b SR___delay|LTORG_CALL|loc_2FD4B_after_call:|@xor eax, eax|mov eax, #0 @ insert delay into victory screen

loc_415F3,10,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_4165E,10,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_416F0,11,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_41740,11,@test eax, eax|cmp eax, #0 @ fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))

loc_2A1AF,4,@xor edi, edi|mov edi, #0|@or ebp, ebp|cmp ebp, #0|@je loc_2A1B3|@movzx edi, byte [ebp+0x1b]|@loc_2A1B3:|ldrneb edi, [ebp, #27] @ fix reading from NULL pointer

loc_1DBD6,5,@xor eax, eax|mov eax, #0 @ set default? config - no sound, no music
loc_1DBEF,5,@call SR_memset|ADR tmp1, loc_1DBEF_after_call|stmfd esp!, {tmp1}|b SR_memset|LTORG_CALL|loc_1DBEF_after_call:|@cmp dword [Game_Sound], 0|LDR tmpadr, =Game_Sound|ldr tmp1, [tmpadr]|cmp tmp1, #0|@je loc_1DBEF_1|beq loc_1DBEF_1|@mov word [loc_59A42], 0|LDR tmpadr, =loc_59A42|mov tmp2, #0|strh tmp2, [tmpadr]|loc_1DBEF_1:|@cmp dword [Game_Music], 0|LDR tmpadr, =Game_Music|ldr tmp1, [tmpadr]|cmp tmp1, #0|@je loc_1DBEF_2|beq loc_1DBEF_2|@mov word [loc_59A52], 0|LDR tmpadr, =loc_59A52|mov tmp2, #0|strh tmp2, [tmpadr]|loc_1DBEF_2: @ enable/disable sound,music driver

loc_27095,15,@mov eax, 1|mov eax, #1 @ fix loading sound driver

loc_26CB5,15,@mov eax, 1|mov eax, #1 @ fix loading music driver

loc_3AA85,4,@mov eax, [esp+0x14]|ldr eax, [esp, #0x14]|@or eax, eax|cmp eax, #0|@je loc_3AAB0|beq loc_3AAB0 @ fix loading NULL samples
loc_3AAB0,5,loc_3AAB0:|@mov ecx, 9|mov ecx, #9 @ fix loading NULL samples


loc_20C50,6,@xor eax, eax|mov eax, #0 @ disable network (IPX) game
loc_20C0D,6,@xor eax, eax|mov eax, #0 @ disable modem/direct connection game
loc_16174,5,nop @ disable network/modem/direct connection game

loc_38995,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
loc_389E6,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
loc_38A49,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights
loc_38AAC,3,@xor ecx, ecx|mov ecx, #0 @ skip loading selector access rights


loc_318B3,45, @ CLIB code
loc_31CEA,72, @ CLIB code
loc_31E0F,31, @ CLIB code
loc_31E34,44, @ CLIB code
loc_320A2,30, @ CLIB code
loc_320E7,57, @ CLIB code
loc_33877,425, @ CLIB code
loc_33B32,26, @ CLIB code
loc_33B94,1052, @ CLIB code
loc_34045,203, @ CLIB code
loc_3416A,438, @ CLIB code
loc_34617,729, @ CLIB code
loc_349C3,139, @ CLIB code
loc_34A52,14, @ CLIB code
loc_3AD3B,227, @ CLIB code
loc_3AE1E,1570, @ CLIB code
loc_3B4D0,1328, @ CLIB code
loc_3BA2D,4227, @ CLIB code
loc_3CD5F,865, @ CLIB code
loc_3D3D7,503, @ CLIB code
loc_3D62E,2802, @ CLIB code
loc_4018A,2182, @ CLIB code
loc_40A41,2911, @ CLIB code
loc_41885,2322, @ CLIB code

loc_555C4,544, @ CLIB data
loc_565E4,100, @ CLIB data
loc_5666C,272, @ CLIB data
loc_56780,596, @ CLIB data
loc_5861A,102, @ CLIB data

loc_632C4,1100, @ CLIB data
loc_63718,12, @ CLIB data
loc_63724,8204, @ stack
