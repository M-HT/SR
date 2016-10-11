loc_113c3,5,@mov edx, 10000h|mov edx, #0x10000 @ Use VESA Video mode instead of x-mode

loc_83e4e,9,@mov eax, [ebp-50h]|ldr eax, [ebp, #-80] @ Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_8703e,3,@mov [loc_181FD0 + 4], eax|LDR tmpadr, =(loc_181FD0 + 4)|str eax, [tmpadr]|@shr eax, 4|mov eax, eax, lsr #4 @ VESA: Return VBE Controller Information: set esi in call structure to actual address of VbeInfoBlock

loc_87140,3,@mov [loc_181FD0 + 4], eax|LDR tmpadr, =(loc_181FD0 + 4)|str eax, [tmpadr]|@shr eax, 4|mov eax, eax, lsr #4 @ VESA: Return VBE mode Information: set esi in call structure to actual address of VbeInfoBlock

loc_8743e,17,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ VESA: Return VBE mode information: store real address of screen window instead of false address

loc_875a9,17,@mov eax, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr eax, [tmpadr] @ VESA: Return VBE mode information: store real address of screen window instead of false address

loc_2625D,7,@mov eax, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr tmp1, [tmpadr]|@mov [ebp-0x2c], eax|str tmp1, [ebp, #-44] @ fix stdout

loc_84FA8,7,@mov eax, [Game_stdout]|LDR tmpadr, =Game_stdout|ldr tmp1, [tmpadr]|@mov [ebp-0x4], eax|str tmp1, [ebp, #-4] @ fix stdout

loc_364D9,4,@or eax, eax|cmp eax, #0|@je loc_364DD|beq loc_364DD|@mov ax, [eax+0x1e]|ldrb tmp1, [eax, #30]|ldrb tmp2, [eax, #31]|mov eax, eax, lsr #16|orr eax, tmp2, eax, lsl #8|orr eax, tmp1, eax, lsl #8|loc_364DD: @ fix reading from NULL pointer

loc_F00E7,3,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ optimize bank switching
loc_F0139,3,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ optimize bank switching

loc_F0C3E,3,@add edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add edi, edi, tmp1 @ optimize bank switching
loc_F0D05,3,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ optimize bank switching
loc_F0D9B,3,@mov edi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr edi, [tmpadr] @ optimize bank switching

loc_F1204,3,@add esi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr tmp1, [tmpadr]|add esi, esi, tmp1 @ optimize bank switching
loc_F12CB,3,@mov esi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr esi, [tmpadr] @ optimize bank switching
loc_F1361,3,@mov esi, [Game_ScreenWindow]|LDR tmpadr, =Game_ScreenWindow|ldr esi, [tmpadr] @ optimize bank switching

loc_C63D1,11,@imul ecx, byte (- ((loc_C9117 - loc_C6417) / 640))|mov tmp1, #(- ((loc_C9117 - loc_C6417) / 640))|smull ecx, tmp2, tmp1, ecx|@add ecx, loc_C9117|LDR tmp1, =loc_C9117|add ecx, ecx, tmp1 @ fix length dependent code

loc_C9CD4,7,@imul ecx, byte ((loc_CA9AD - loc_C9CDD) / 410)|mov tmp1, #((loc_CA9AD - loc_C9CDD) / 410)|smull ecx, tmp2, tmp1, ecx|@add ecx, loc_CA9AD|LDR tmp1, =loc_CA9AD|add ecx, ecx, tmp1 @ fix length dependent code

loc_CAA59,12,@imul ecx, byte (- ((loc_CBD33 - loc_CAA67) / 401))|mov tmp1, #(- ((loc_CBD33 - loc_CAA67) / 401))|smull ecx, tmp2, tmp1, ecx|@add ecx, loc_CBD33|LDR tmp1, =loc_CBD33|add ecx, ecx, tmp1 @ fix length dependent code

loc_C11E9,7,@mov eax, [Game_VSyncTick]|LDR tmpadr, =Game_VSyncTick|ldr eax, [tmpadr] @ timer ticks since midnight
loc_C120F,7,@mov eax, [Game_VSyncTick]|LDR tmpadr, =Game_VSyncTick|ldr eax, [tmpadr] @ timer ticks since midnight
loc_C1229,2,@mov eax, [Game_VSyncTick]|LDR tmpadr, =Game_VSyncTick|ldr eax, [tmpadr] @ timer ticks since midnight
loc_C1251,7,@mov eax, [Game_VSyncTick]|LDR tmpadr, =Game_VSyncTick|ldr eax, [tmpadr] @ timer ticks since midnight
loc_C126B,2,@mov eax, [Game_VSyncTick]|LDR tmpadr, =Game_VSyncTick|ldr eax, [tmpadr] @ timer ticks since midnight
loc_C1278,7,@mov eax, [Game_VSyncTick]|LDR tmpadr, =Game_VSyncTick|ldr eax, [tmpadr] @ timer ticks since midnight

loc_B49B0,118,@mov eax, [loc_196D04]|LDR tmpadr, =loc_196D04|ldr eax, [tmpadr]|@mov [self_mod_width], eax|LDR tmpadr, =self_mod_width|str eax, [tmpadr] @ fix self-modifying code

loc_B4EBA,19,@mov eax, 400|mov eax, #400|@mov ebp, [esi]|ldr ebp, [esi]|@sub eax, ecx|sub eax, eax, ecx|@mul dword [self_mod_width]|LDR tmpadr, =self_mod_width|ldr tmp1, [tmpadr]|umull eax, edx, tmp1, eax|@add ebp, eax|add ebp, ebp, eax|@imul eax, ebx, byte (- ((loc_B6502 - loc_B4F22) / 400))|mov tmp1, #(- ((loc_B6502 - loc_B4F22) / 400))|smull eax, tmp2, tmp1, ebx|@add eax, loc_B6502|LDR tmp1, =loc_B6502|add eax, eax, tmp1 @ fix self-modifying (and length dependent) code
loc_B4EEF,2,@div ecx|mov tmp3, ecx|bl x86_div_64|@mov edx, [self_mod_width]|LDR tmpadr, =self_mod_width|ldr edx, [tmpadr] @ fix self-modifying (and length dependent) code
loc_B4F18,3,@shl edi, 10h|mov edi, edi, lsl #16|bic ebx, ebx, #0x00ff @ clear bl for self_mod_code_1
loc_B4F21,5601,@align 4|.balign 4, 0|loc_B4F22:|@rep 400|.rept 400|self_mod_code_1|@%endrep|.endr @ fix self-modifying (and length dependent) code

loc_B6629,19,@mov eax, 400|mov eax, #400|@mov ebx, ecx|mov ebx, ecx|@mov ebp, [esi]|ldr ebp, [esi]|@inc ebx|add ebx, ebx, #1|@sub eax, ecx|sub eax, eax, ecx|@mul dword [self_mod_width]|LDR tmpadr, =self_mod_width|ldr tmp1, [tmpadr]|umull eax, edx, tmp1, eax|@add ebp, eax|add ebp, ebp, eax|@imul eax, ebx, byte (- ((loc_B82B2 - loc_B6692) / 400))|mov tmp1, #(- ((loc_B82B2 - loc_B6692) / 400))|smull eax, tmp2, tmp1, ebx|@add eax, loc_B82B2|LDR tmp1, =loc_B82B2|add eax, eax, tmp1 @ fix self-modifying (and length dependent) code
loc_B665E,2,@div ecx|mov tmp3, ecx|bl x86_div_64|@mov edx, [self_mod_width]|LDR tmpadr, =self_mod_width|ldr edx, [tmpadr] @ fix self-modifying (and length dependent) code
loc_B6687,3,@shl edi, 10h|mov edi, edi, lsl #16|bic ebx, ebx, #0x00ff @ clear bl for self_mod_code_2
loc_B6692,7200,@align 4|.balign 4, 0|loc_B6692:|@%rep 400|.rept 400|self_mod_code_2|@%endrep|.endr @ fix self-modifying (and length dependent) code

loc_B83DC,10,@mov eax, 400|mov eax, #400|@mov ebp, [esi]|ldr ebp, [esi]|@sub eax, ecx|sub eax, eax, ecx|@mul dword [self_mod_width]|LDR tmpadr, =self_mod_width|ldr tmp1, [tmpadr]|umull eax, edx, tmp1, eax|@add ebp, eax|add ebp, ebp, eax|@imul eax, ebx, byte (- ((loc_BACE0 - loc_B8440) / 400))|mov tmp1, #(- ((loc_BACE0 - loc_B8440) / 400))|smull eax, tmp2, tmp1, ebx|@add eax, loc_BACE0|LDR tmp1, =loc_BACE0|add eax, eax, tmp1 @ fix self-modifying (and length dependent) code
loc_B8439,6,@jmp dword near [loc_1414A4]|ACTION_UNCONDITIONAL_JUMP|LDR tmplr, =self_mod_width|LDR tmpadr, =loc_1414A4|bic ebx, ebx, #0x00ff @ clear bl for self_mod_code_3|ldr tmp3, [tmplr] @ optimize self_mod_code_3 macro (copy memory to register)|ldr eip, [tmpadr]|LTORG_JMP
loc_B843F,10401,@align 4|.balign 4, 0|loc_B8440:|@%rep 400|.rept 400|self_mod_code_3|@%endrep|.endr @ fix self-modifying (and length dependent) code

loc_BAFA4,8,@mov eax, ecx|mov eax, ecx|@inc eax|add eax, eax, #1|@imul eax, byte (- ((loc_BCC54 - loc_BB034) / 400))|mov tmp1, #(- ((loc_BCC54 - loc_BB034) / 400))|smull eax, tmp2, tmp1, eax|@add eax, loc_BCC54|LDR tmp1, =loc_BCC54|add eax, eax, tmp1 @ fix self-modifying (and length dependent) code
loc_BAFEB,6,@mov ebp, [esp]|ldr ebp, [esp]|@mov eax, 400|mov eax, #400|@sub eax, [ebp-24h]|ldr tmp1, [ebp, #-36]|sub eax, eax, tmp1|@mov ebp, [self_mod_width]|LDR tmpadr, =self_mod_width|ldr ebp, [tmpadr]|@mul ebp|umull eax, edx, ebp, eax|@mov edx, [loc_1414AC]|LDR tmpadr, =loc_1414AC|ldr edx, [tmpadr]|@add edx, eax|add edx, edx, eax @ fix self-modifying (and length dependent) code
loc_BB02A,3,@shl edi, 10h|mov edi, edi, lsl #16|bic ebx, ebx, #0x00ff @ clear bl for self_mod_code_4
loc_BB033,7183,@align 4|.balign 4, 0|loc_BB034:|@%rep 399|.rept 399|self_mod_code_4|@%endrep|.endr @ fix self-modifying (and length dependent) code
loc_BCC42,18,self_mod_code_4 @ fix self-modifying (and length dependent) code
loc_BCC7B,13,@mov eax, ecx|mov eax, ecx|@inc eax|add eax, eax, #1|@imul eax, byte (- ((loc_BCC54 - loc_BB034) / 400))|mov tmp1, #(- ((loc_BCC54 - loc_BB034) / 400))|smull eax, tmp2, tmp1, eax|@add eax, loc_BCC54|LDR tmp1, =loc_BCC54|add eax, eax, tmp1 @ fix self-modifying (and length dependent) code

loc_8EF99,15,@xor esi, esi|mov esi, #0|@cmp dword [Game_Music], 0|LDR tmp1, =Game_Music|ldr tmp2, [tmp1]|cmp tmp2, #0|@jne loc_8EF99_1|@inc esi|@loc_8EF99_1:|movEQ esi, #1 @ enable/disable music driver

loc_8F16A,10,@push eax|stmfd esp!, {eax} @ fix uninstalling music driver

loc_8D45D,15,@xor esi, esi|mov esi, #0|@cmp dword [Game_Sound], 0|LDR tmp1, =Game_Sound|ldr tmp2, [tmp1]|cmp tmp2, #0|@jne loc_8D45D_1|@inc esi|@loc_8D45D_1:|movEQ esi, #1 @ enable/disable sound driver

loc_8D63A,10,@push eax|stmfd esp!, {eax} @ fix uninstalling sound driver

loc_6562E,3,@or eax, eax|@je loc_65631|@mov al, [eax+0x1b]|@loc_65631:|cmp eax, #0|ldrneb eax, [eax, #27] @ fix reading from NULL pointer
loc_657BE,3,@or eax, eax|@je loc_657C1|@mov al, [eax+0x1b]|@loc_657C1:|cmp eax, #0|ldrneb eax, [eax, #27] @ fix reading from NULL pointer

loc_4A4B7,3,@cmp eax, 0x400|cmp eax, #0x400|@jb loc_4A4DB|blo loc_4A4DB @ fix reading from NULL pointer (probably)

loc_52BD4,3,@xor eax, eax|mov eax, #0|@or edx, edx|@je loc_52BD7|@mov al, [edx+0xe]|@loc_52BD7:|cmp edx, #0|ldrneb eax, [edx, #14] @ fix reading from NULL pointer
loc_52C40,3,@xor eax, eax|mov eax ,#0|@or edx, edx|@je loc_52C43|@mov al, [edx+0x14]|@loc_52C43:|cmp edx, #0|ldrneb eax, [edx, #20] @ fix reading from NULL pointer

loc_4F43A,3,@xor eax, eax|mov eax, #0|@or edx, edx|@je loc_4F43D|@mov al, [edx+0xe]|@loc_4F43D:|cmp edx, #0|ldrneb eax, [edx, #14] @ fix reading from NULL pointer
loc_4F4A6,3,@or eax, eax|@je loc_4F4A9|@mov al, [eax+0x1b]|@loc_4F4A9:|cmp eax, #0|ldrneb eax, [eax, #27] @ fix reading from NULL pointer

loc_656F3,3,@or eax, eax|@je loc_656F6|@mov al, [eax+0x1b]|@loc_656F6:|cmp eax, #0|ldrneb eax, [eax, #27] @ ; fix reading from NULL pointer

loc_BF258,2,@cmp edx, 1024|cmp edx, #1024|@je loc_BF25A|blo loc_BF25A|@mov ah, [edx]|ldrb tmp1, [edx]|bic eax, eax, #0xff00|orr eax, eax, tmp1, lsl #8|loc_BF25A: @ fix reading from NULL pointer
loc_BF26A,3,@cmp edx, 1024|cmp edx, #1024|@je loc_BF28E|blo loc_BF28E|@test ah, 2|andS tmp1, eax, #0x200 @ fix reading from NULL pointer ('test ah, 2' must set flags for 'jz')

loc_BF2D1,3,@cmp eax, 1024|cmp eax, #1024|@jb loc_BF31C|blo loc_BF31C|@test byte [eax], 60h|ldrb tmp1, [eax]| andS tmp1, tmp1, #0x60 @ fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_BF355,3,@cmp eax, 1024|cmp eax, #1024|@jb loc_BF393|blo loc_BF393|@test byte [eax], 60h|ldrb tmp1, [eax]| andS tmp1, tmp1, #0x60 @ fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_C014C,4,@cmp eax, 1024|@jb loc_C0150|@mov dx, [eax+0xa]|@loc_C0150:|cmp eax, #1024|ldrhsh edx, [eax, #10] @ fix reading from NULL pointer
loc_C0156,4,@cmp eax, 1024|@jb loc_C015a|@mov ax, [eax+0xc]|@loc_C015a:|cmp eax, #1024|ldrhsh eax, [eax, #12] @ fix reading from NULL pointer

loc_BF442,3,@cmp eax, 1024|cmp eax, #1024|@jb loc_BF481|blo loc_BF481|@test byte [eax], 60h|ldrb tmp1, [eax]| andS tmp1, tmp1, #0x60 @ fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_BF3C7,3,@cmp eax, 1024|cmp eax, #1024|@jb loc_BF411|blo loc_BF411|@test byte [eax], 60h|ldrb tmp1, [eax]| andS tmp1, tmp1, #0x60 @ fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_4D505,4,@or eax, eax|@je loc_4D509|@mov [eax+0x28], dx|cmp eax, #0|strneh edx, [eax, #40] @ fix reading from NULL pointer

loc_3664B,3,@or edx, edx|cmp edx, #0|@je loc_3664E|@mov al, [edx+0x1]|ldrneb eax, [edx, #1]|@loc_3664E: @ fix reading from NULL pointer

loc_C5EBB,4,@add esp, 4|add esp, esp, #4 @ fix esp instead of sp (probably mistake in original code ???)

loc_94C79,6,@mov [loc_13FFB0], esi|LDR tmpadr, =loc_13FFB0|str esi, [tmpadr]|@cmp dword [Game_UseEnhanced3DEngine], 0|LDR tmp1, =Game_UseEnhanced3DEngine|ldr tmp2, [tmp1]|cmp tmp2, #0|beq _NonEnhanced3DEngine|@call draw_3dscene_proc|ADR tmp1, loc_94C7F_after_call|stmfd esp!, {tmp1}|b draw_3dscene_proc|LTORG_CALL|_NonEnhanced3DEngine: @ call enhanced 3d engine

loc_7C738,2,@mov ebp, esp|mov ebp, esp|@cmp ebx, 99|cmp ebx, #99|@jbe loc_7C73A|bls loc_7C73A|@cmp ebx, 255|cmp ebx, #255|@ja loc_7C73A|bhi loc_7C73A|@mov ebx, 99|mov ebx, #99|loc_7C73A: @ limit number of items to 99

loc_CDC08,2,@dec bp|mrs tmplr, cpsr|mov tmp1, ebp, lsl #16|bic ebp, ebp, #0x00ff|bic ebp, ebp, #0xff00|subS tmp1, tmp1, #0x00010000|orr ebp, ebp, tmp1, lsr #16|mrs tmp1, cpsr|and tmplr, tmplr, #0x20000000|orr tmp1, tmp1, #0x20000000|eor tmp1, tmp1, tmplr|msr cpsr_f, tmp1 @ on ARM swap carry bit before or after instruction ('dec bp' must set flags for 'jz')

loc_CDC36,2,@rcr edx, 1|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|mov edx, edx, rrx @ on ARM swap carry bit before instruction ('rcr edx, 1' must set flags for 'rcr')

loc_CDEA5,2,@rcr edx, 1|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|mov edx, edx, rrx @ on ARM swap carry bit before instruction ('rcr edx, 1' must set flags for 'rcr')

loc_CDFAD,3,@adc eax, 0|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adcS eax, eax, #0 @ on ARM swap carry bit before instruction ('adc eax, 0' must set flags for 'adc')

loc_CDFB5,2,@rcr edx, 1|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|mov edx, edx, rrx @ on ARM swap carry bit before instruction ('rcr edx, 1' must set flags for 'rcr')

loc_CE411,3,@rcr dword [edi+4], 1|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|ldrb tmp1, [edi, #4]|ldrb tmp2, [edi, #5]|orr tmp1, tmp2, tmp1, lsl #8|ldrb tmp2, [edi, #6]|orr tmp1, tmp2, tmp1, lsl #8|ldrb tmp2, [edi, #7]|orr tmp1, tmp2, tmp1, lsl #8|mov tmp1, tmp1, rrx|strb tmp1, [edi, #4]|mov tmp2, tmp1, lsr #8|strb tmp2, [edi, #5]|mov tmp2, tmp1, lsr #16|strb tmp2, [edi, #6]|mov tmp2, tmp1, lsr #24|strb tmp2, [edi, #7] @ on ARM swap carry bit before instruction

loc_CE473,3,@rcr dword [edi+4], 1|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|ldrb tmp1, [edi, #4]|ldrb tmp2, [edi, #5]|orr tmp1, tmp2, tmp1, lsl #8|ldrb tmp2, [edi, #6]|orr tmp1, tmp2, tmp1, lsl #8|ldrb tmp2, [edi, #7]|orr tmp1, tmp2, tmp1, lsl #8|mov tmp1, tmp1, rrx|strb tmp1, [edi, #4]|mov tmp2, tmp1, lsr #8|strb tmp2, [edi, #5]|mov tmp2, tmp1, lsr #16|strb tmp2, [edi, #6]|mov tmp2, tmp1, lsr #24|strb tmp2, [edi, #7] @ on ARM swap carry bit before instruction

loc_CE7BD,1,@stc|mrs tmp1, cpsr|bic tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1 @ on ARM swap carry bit after instruction

loc_CE7BE,2,@adc edi, edi|mrs tmp1, cpsr|eor tmp1, tmp1, #0x20000000|msr cpsr_f, tmp1|adc edi, edi, edi @ on ARM swap carry bit before instruction

loc_25156,1,@pop ebx|ldmfd esp!, {ebx}|@call SR_Sync|ADR tmp1, loc_25156_after_call|stmfd esp!, {tmp1}|b SR_Sync|LTORG_CALL|loc_25156_after_call: @ call sync after save
