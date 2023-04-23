loc_113c3,5,;mov edx, 10000h|mov edx, 0x10000 ; Use VESA Video mode instead of x-mode

loc_8703e,3,;mov [loc_181FD0 + 4], eax|mov tmpadr, loc_181FD0|add tmpadr, tmpadr, 4|store eax, tmpadr, 4|;shr eax, 4|lshr eax, eax, 4 ; VESA: Return VBE Controller Information: set esi in call structure to actual address of VbeInfoBlock

loc_87140,3,;mov [loc_181FD0 + 4], eax|mov tmpadr, loc_181FD0|add tmpadr, tmpadr, 4|store eax, tmpadr, 4|;shr eax, 4|lshr eax, eax, 4 ; VESA: Return VBE mode Information: set esi in call structure to actual address of VbeInfoBlock

loc_8743e,17,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; VESA: Return VBE mode information: store real address of screen window instead of false address

loc_875a9,17,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; VESA: Return VBE mode information: store real address of screen window instead of false address

loc_2625D,7,;mov eax, [Game_stdout]|load eax, Game_stdout, 4|;mov [ebp-0x2c], eax|add tmpadr, ebp, -44|store eax, tmpadr, 4|tcall loc_26264|endp ; fix stdout

loc_364D9,4,;or eax, eax|;je loc_364DD|;mov ax, [eax+0x1e]|;loc_364DD:|ifnz eax|add tmpadr, eax, 30|load16z eax, tmpadr, 1|endif ; fix reading from NULL pointer

loc_F00E7,3,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; optimize bank switching
loc_F0139,3,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; optimize bank switching

loc_F0C3E,3,;add edi, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add edi, edi, tmp1 ; optimize bank switching
loc_F0D05,3,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; optimize bank switching
loc_F0D9B,3,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; optimize bank switching

loc_F1204,3,;add esi, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add esi, esi, tmp1 ; optimize bank switching
loc_F12CB,3,;mov esi, [Game_ScreenWindow]|load esi, Game_ScreenWindow, 4 ; optimize bank switching
loc_F1361,3,;mov esi, [Game_ScreenWindow]|load esi, Game_ScreenWindow, 4 ; optimize bank switching

loc_C63D1,11, ; fix length dependent code
loc_C6411,11526,;mov bl, 0|and ebx, ebx, 0xffffff00|;cmp dword [loc_144F4C], 0|;jz loc_C9117|load tmp1, loc_144F4C, 4|ctcallz tmp1, loc_C9117|tcall loc_C6417|endp|proc loc_C6417|;adc ecx, esi|and tmp5, eflags, CF|and eflags, eflags, ~CF|add tmp3, esi, tmp5|add ecx, ecx, tmp3|cmovz tmp3, tmp5, tmp5, 0|cmovult ecx, tmp3, tmp5, CF, tmp5|;adc edx, ebp|add tmp3, ebp, tmp5|add edx, edx, tmp3|cmovz tmp3, tmp5, tmp5, 0|cmovult edx, tmp3, tmp5, CF, tmp5|or eflags, eflags, tmp5|;mov bl, [eax]|load8z tmp1, eax, 1|;mov bl, [ebx]|add tmpadr, ebx, tmp1|load8z tmp1, tmpadr, 1|;mov al, dl|and eax, eax, 0xffff0000|and tmp2, edx, 0xff|or eax, eax, tmp2|;mov ah, cl|and tmp2, ecx, 0xff|shl tmp2, tmp2, 8|or eax, eax, tmp2|;mov [edi+256+640-[loc_144F4C]], bl|load tmp4, loc_144F4C, 4|add tmpadr, edi, 256+640|sub tmpadr, tmpadr, tmp4|store8 tmp1, tmpadr, 1|;dec dword [loc_144F4C]|sub tmp4, tmp4, 1|store tmp4, loc_144F4C, 4|;jnz loc_C6417|ctcallnz tmp4, loc_C6417|tcall loc_C9117|endp|proc loc_C9117 ; roll unrolled loop

loc_C9CD4,3289,;jecxz loc_CA9AD|ctcallz ecx, loc_CA9AD|tcall loc_C9CDD|endp|proc loc_C9CDD|;mov al, [esi]|load8z tmp1, esi, 1|;add esi, ebx|add esi, esi, ebx|;mov [edi], al|store8 tmp1, edi, 1|;add edi, edx|add edi, edi, edx|;inc ecx|add ecx, ecx, 1|;jnz loc_C9CDD|ctcallnz ecx, loc_C9CDD|tcall loc_CA9AD|endp ; roll unrolled loop

loc_CAA59,4826,;jecxz loc_CBD33|ctcallz ecx, loc_CBD33|tcall loc_CAA67|endp|proc loc_CAA67|;mov al, [esi]|load8z tmp1, esi, 1|;mov [edi], al|store8 tmp1, edi, 1|;add esi, ebx|add esi, esi, ebx|;mov [edi+edx], al|;lea edi, [edi+edx*2]|add edi, edi, edx|store8 tmp1, edi, 1|add edi, edi, edx|;dec ecx|sub ecx, ecx, 1|;jnz loc_CAA67|ctcallnz ecx, loc_CAA67|tcall loc_CBD33|endp ; roll unrolled loop

loc_C11E9,7,;mov eax, [Game_VSyncTick]|load eax, Game_VSyncTick, 4 ; timer ticks since midnight
loc_C120F,7,;mov eax, [Game_VSyncTick]|load eax, Game_VSyncTick, 4 ; timer ticks since midnight
loc_C1229,2,;mov eax, [Game_VSyncTick]|load eax, Game_VSyncTick, 4 ; timer ticks since midnight
loc_C1251,7,;mov eax, [Game_VSyncTick]|load eax, Game_VSyncTick, 4 ; timer ticks since midnight
loc_C126B,2,;mov eax, [Game_VSyncTick]|load eax, Game_VSyncTick, 4 ; timer ticks since midnight
loc_C1278,7,;mov eax, [Game_VSyncTick]|load eax, Game_VSyncTick, 4 ; timer ticks since midnight

loc_B49B0,118,;mov eax, [loc_196D04]|load eax, loc_196D04, 4|;mov [self_mod_width], eax|store eax, self_mod_width, 4 ; fix self-modifying code

loc_B4EBA,19,;mov eax, 400|mov eax, 400|;mov ebp, [esi]|load ebp, esi, 1|;sub eax, ecx|sub eax, eax, ecx|;mul dword [self_mod_width]|load tmp1, self_mod_width, 4|mul eax, eax, tmp1|;add ebp, eax|add ebp, ebp, eax|mov eax, ebx ; fix self-modifying (and length dependent) code
loc_B4F1B,5607,;mov edx, [self_mod_width]|load edx, self_mod_width, 4|;mov bl, 0|and ebx, ebx, 0xffffff00|;cmp dword [loc_1414A4], 0|load tmp4, loc_1414A4, 4|;jz loc_B6502|ctcallz tmp4, loc_B6502|tcall loc_B4F22|endp|proc loc_B4F22|;mov bl, [esi]|load8z tmp1, esi, 1|;add edi, eax|add edi, edi, eax|cmovult edi, eax, tmp3, CF, 0|;mov bl, [ebx]|add tmpadr, ebx, tmp1|load8z tmp2, tmpadr, 1|;adc esi, ecx|add tmp3, tmp3, ecx|add esi, esi, tmp3|;mov [ebp], bl|store8 tmp2, ebp, 1|;add ebp, edx|add ebp, ebp, edx|;dec dword [loc_1414A4]|load tmp4, loc_1414A4, 4|sub tmp4, tmp4, 1|store tmp4, loc_1414A4, 4|;jnz loc_B4F22|ctcallnz tmp4, loc_B4F22|tcall loc_B6502|endp ; roll unrolled loop (self-modifying code)

loc_B6629,19,;mov eax, 400|mov eax, 400|;mov ebx, ecx|;mov ebp, [esi]|load ebp, esi, 1|;inc ebx|add ebx, ecx, 1|;sub eax, ecx|sub eax, eax, ecx|;mul dword [self_mod_width]|load tmp1, self_mod_width, 4|mul eax, eax, tmp1|;add ebp, eax|add ebp, ebp, eax|add eax, ecx, 1 ; fix self-modifying (and length dependent) code
loc_B668C,7206,;mov edx, [self_mod_width]|load edx, self_mod_width, 4|;mov bl, 0|and ebx, ebx, 0xffffff00|;cmp dword [loc_1414A4], 0|load tmp4, loc_1414A4, 4|;jz loc_B82B2|ctcallz tmp4, loc_B82B2|tcall loc_B6692|endp|proc loc_B6692|;mov bl, [esi]|load8z tmp1, esi, 1|;adc esi, ecx|and tmp3, eflags, CF|add tmp3, tmp3, ecx|add esi, esi, tmp3|;or  bl, bl|;jz  short %%loc_B66A2|ifnz tmp1|;mov al, [ebx]|add tmpadr, ebx, tmp1|load8z tmp2, tmpadr, 1|;mov [ebp], al|store8 tmp2, ebp, 1|endif|;%%loc_B66A2:|;add ebp, edx|add ebp, ebp, edx|;add edi, eax|and eflags, eflags, ~CF|add edi, edi, eax|cmovult edi, eax, tmp3, CF, 0|or eflags, eflags, tmp3|;dec dword [loc_1414A4]|load tmp4, loc_1414A4, 4|sub tmp4, tmp4, 1|store tmp4, loc_1414A4, 4|;jnz loc_B6692|ctcallnz tmp4, loc_B6692|tcall loc_B82B2|endp ; roll unrolled loop (self-modifying code)

loc_B83DC,10,;mov eax, 400|mov eax, 400|;mov ebp, [esi]|load ebp, esi, 1|;sub eax, ecx|sub eax, eax, ecx|;mul dword [self_mod_width]|load tmp1, self_mod_width, 4|mul eax, eax, tmp1|;add ebp, eax|add ebp, ebp, eax|mov eax, ebx ; fix self-modifying (and length dependent) code
loc_B8439,10407,;mov bl, 0|and ebx, ebx, 0xffffff00|;mov dl, 0|and edx, edx, 0xffffff00|;cmp dword [loc_1414A4], 0|load tmp4, loc_1414A4, 4|;jz loc_BACE0|ctcallz tmp4, loc_BACE0|tcall loc_B8440|endp|proc loc_B8440|;mov bl, [esi]|load8z tmp1, esi, 1|;or  bl, bl|;mov al, [ebx]|add tmpadr, ebx, tmp1|load8z tmp2, tmpadr, 1|;jnz short %%loc_B8450|ifz tmp1|;mov dl, [ebp]|load8z tmp5, ebp, 1|;mov al, [edx]|add tmpadr, edx, tmp5|load8z tmp2, tmpadr, 1|endif|;%%loc_B8450:|;add edi, eax|add edi, edi, eax|cmovult edi, eax, tmp3, CF, 0|;adc esi, ecx|add tmp3, tmp3, ecx|add esi, esi, tmp3|;mov [ebp], al|store8 tmp2, ebp, 1|;add ebp, [self_mod_width]|load tmp0, self_mod_width, 4|add ebp, ebp, tmp0|;dec dword [loc_1414A4]|load tmp4, loc_1414A4, 4|sub tmp4, tmp4, 1|store tmp4, loc_1414A4, 4|;jnz loc_B8440|ctcallnz tmp4, loc_B8440|tcall loc_BACE0|endp ; roll unrolled loop (self-modifying code)

loc_BAFA4,8,;mov eax, ecx|;inc eax|add eax, ecx, 1 ; fix self-modifying (and length dependent) code
loc_BAFEB,6,;mov ebp, [esp]|load ebp, esp, 4|;mov eax, 400|mov eax, 400|;sub eax, [ebp-24h]|add tmpadr, ebp, -0x24|load tmp1, tmpadr, 4|sub eax, eax, tmp1|add tmp1, tmp1, 1|store tmp1, loc_1414A4, 4|;mov ebp, [self_mod_width]|load ebp, self_mod_width, 4|;mul ebp|mul eax, eax, ebp|;mov edx, [loc_1414AC]|load edx, loc_1414AC, 4|;add edx, eax|add edx, edx, eax ; fix self-modifying (and length dependent) code
loc_BB02D,7207,;mov bl, 0|and ebx, ebx, 0xffffff00|;cmp dword [loc_1414A4], 0|load tmp4, loc_1414A4, 4|;jz loc_BCC54|ctcallz tmp4, loc_BCC54|tcall loc_BB034|endp|proc loc_BB034|;add edi, eax|add edi, edi, eax|cmovult edi, eax, tmp3, CF, 0|;mov bl, [esi]|load8z tmp1, esi, 1|;adc esi, ecx|add tmp3, tmp3, ecx|add esi, esi, tmp3|;or  bl, bl|;jz  short %%loc_BB046|ifnz tmp1|;mov bl, [ebx]|add tmpadr, ebx, tmp1|load8z tmp2, tmpadr, 1|;mov [edx], bl|store8 tmp2, edx, 1|endif|;%%loc_BB046:|;add edx, ebp|add edx, edx, ebp|;dec dword [loc_1414A4]|load tmp4, loc_1414A4, 4|sub tmp4, tmp4, 1|store tmp4, loc_1414A4, 4|;jnz loc_BB034|ctcallnz tmp4, loc_BB034|tcall loc_BCC54|endp ; roll unrolled loop (self-modifying code)
loc_BCC7B,13,;mov eax, ecx|;inc eax|add eax, ecx, 1 ; fix self-modifying (and length dependent) code

loc_6562E,3,;or eax, eax|;je loc_65631|;mov al, [eax+0x1b]|;loc_65631:|ifnz eax|add tmpadr, eax, 27|load8z eax, tmpadr, 1|endif ; fix reading from NULL pointer
loc_657BE,3,;or eax, eax|;je loc_657C1|;mov al, [eax+0x1b]|;loc_657C1:|ifnz eax|add tmpadr, eax, 27|load8z eax, tmpadr, 1|endif ; fix reading from NULL pointer

loc_4A4B7,3,;cmp eax, 0x400|cmovult eax, 0x400, tmpcnd, 0, 1|;jb loc_4A4DB|ctcallz tmpcnd, loc_4A4DB|tcall loc_4A4BA|endp|proc loc_4A4BA ; fix reading from NULL pointer (probably)

loc_52BD4,3,;xor eax, eax|mov eax, 0|;or edx, edx|;je loc_52BD7|;mov al, [edx+0xe]|;loc_52BD7:|ifnz edx|add tmpadr, edx, 14|load8z eax, tmpadr, 1|endif ; fix reading from NULL pointer
loc_52C40,3,;xor eax, eax|mov eax, 0|;or edx, edx|;je loc_52C43|;mov al, [edx+0x14]|;loc_52C43:|ifnz edx|add tmpadr, edx, 20|load8z eax, tmpadr, 1|endif ; fix reading from NULL pointer

loc_4F43A,3,;xor eax, eax|mov eax, 0|;or edx, edx|;je loc_4F43D|;mov al, [edx+0xe]|;loc_4F43D:|ifnz edx|add tmpadr, edx, 14|load8z eax, tmpadr, 1|endif ; fix reading from NULL pointer
loc_4F4A6,3,;or eax, eax|;je loc_4F4A9|;mov al, [eax+0x1b]|;loc_4F4A9:|ifnz eax|add tmpadr, eax, 27|load8z eax, tmpadr, 1|endif ; fix reading from NULL pointer

loc_656F3,3,;or eax, eax|;je loc_656F6|;mov al, [eax+0x1b]|;loc_656F6:|ifnz eax|add tmpadr, eax, 27|load8z eax, tmpadr, 1|endif ; fix reading from NULL pointer

loc_BF258,2,;cmp edx, 1024|cmovult edx, 1024, tmpcnd, 0, 1|;jb loc_BF25A|;mov ah, [edx]|;loc_BF25A:|ifnz tmpcnd|load8z tmp1, edx, 1|shl tmp1, tmp1, 8|or eax, eax, tmp1|endif ; fix reading from NULL pointer
loc_BF26A,3,;cmp edx, 1024|cmovult edx, 1024, tmpcnd, 0, 1|;jb loc_BF28E|ctcallz tmpcnd, loc_BF28E|tcall loc_BF26A_1|endp|proc loc_BF26A_1|;test ah, 2|and tmpcnd, eax, 0x200 ; fix reading from NULL pointer ('test ah, 2' must set flags for 'jz')

loc_BF2D1,3,;cmp eax, 1024|cmovult eax, 1024, tmpcnd, 0, 1|;jb loc_BF31C|ctcallz tmpcnd, loc_BF31C|tcall loc_BF2D1_1|endp|proc loc_BF2D1_1|;test byte [eax], 60h|load8z tmp0, eax, 1|and tmpcnd, tmp0, 0x60 ; fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_BF355,3,;cmp eax, 1024|cmovult eax, 1024, tmpcnd, 0, 1|;jb loc_BF393|ctcallz tmpcnd, loc_BF393|tcall loc_BF355_1|endp|proc loc_BF355_1|;test byte [eax], 60h|load8z tmp0, eax, 1|and tmpcnd, tmp0, 0x60 ; fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_C014C,4,;cmp eax, 1024|cmovult eax, 1024, tmpcnd, 0, 1|;jb loc_C0150|;mov dx, [eax+0xa]|;loc_C0150:|ifnz tmpcnd|add tmpadr, eax, 10|load16z edx, tmpadr, 1|endif ; fix reading from NULL pointer
loc_C0156,4,;cmp eax, 1024|cmovult eax, 1024, tmpcnd, 0, 1|;jb loc_C015a|;mov ax, [eax+0xc]|;loc_C015a:|ifnz tmpcnd|add tmpadr, eax, 12|load16z eax, tmpadr, 1|endif ; fix reading from NULL pointer

loc_BF442,3,;cmp eax, 1024|cmovult eax, 1024, tmpcnd, 0, 1|;jb loc_BF481|ctcallz tmpcnd, loc_BF481|tcall loc_BF442_1|endp|proc loc_BF442_1|;test byte [eax], 60h|load8z tmp0, eax, 1|and tmpcnd, tmp0, 0x60 ; fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_BF3C7,3,;cmp eax, 1024|cmovult eax, 1024, tmpcnd, 0, 1|;jb loc_BF411|ctcallz tmpcnd, loc_BF411|tcall loc_BF3C7_1|endp|proc loc_BF3C7_1|;test byte [eax], 60h|load8z tmp0, eax, 1|and tmpcnd, tmp0, 0x60 ; fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_4D505,4,;or eax, eax|;je loc_4D509|;mov [eax+0x28], dx|ifnz eax|add tmpadr, eax, 40|store16 edx, tmpadr, 1|endif|tcall loc_4D509|endp ; fix reading from NULL pointer

loc_3664B,3,;or edx, edx|;je loc_3664E|;mov al, [edx+0x1]|;loc_3664E:|ifnz edx|add tmpadr, edx, 1|store8 eax, tmpadr, 1|endif ; fix reading from NULL pointer

loc_548D4,3,;or eax, eax|;je loc_548D7|;mov al, [eax+0x5]|;loc_548D7:|ifnz eax|add tmpadr, eax, 5|load8z eax, tmpadr, 1|endif ; fix reading from NULL pointer
loc_5491D,3,;or eax, eax|;je loc_54912|ctcallz eax, loc_54912|tcall loc_54920|endp|proc loc_54920|;add eax, 0x6|add eax, eax, 6 ; fix reading from NULL pointer
loc_54950,3,;xor edx, edx|mov edx, 0|;or eax, eax|;je loc_54953|;mov dx, [eax]|;loc_54953:|ifnz eax|load16z edx, eax, 1|endif ; fix reading from NULL pointer
loc_5495C,4,;xor edx, edx|mov edx, 0|;or eax, eax|;je loc_54960|;mov dx, [eax+0x2]|;loc_54960:|ifnz eax|add tmpadr, eax, 2|load16z edx, tmpadr, 1|endif ; fix reading from NULL pointer

loc_C5EBB,4,;add esp, 4|add esp, esp, 4 ; fix esp instead of sp (probably mistake ???)

loc_94C79,6,;mov [loc_13FFB0], esi|store esi, loc_13FFB0, 4|;cmp dword [Game_UseEnhanced3DEngine], 0|load tmp1, Game_UseEnhanced3DEngine, 4|;jz _NonEnhanced3DEngine|ctcallz tmp1, loc_94C7F|tcall loc_94C79_1|endp|proc loc_94C79_1|;call draw_3dscene_proc|;jmp _After3DEngine|PUSH loc_94C84|tcall draw_3dscene_proc|endp|;_NonEnhanced3DEngine:|proc loc_94C7F ; call enhanced 3d engine
loc_94C84,5,;_After3DEngine:|;mov eax, [loc_14A4DA]|load eax, loc_14A4DA, 2 ; label after 3d engine

loc_7C738,2,;mov ebp, esp|mov ebp, esp|;cmp ebx, 99|cmovugt ebx, 99, tmpcnd, 1, 0|;jbe loc_7C73A|;cmp ebx, 255|cmovugt ebx, 255, tmpcnd, 0, tmpcnd|;ja loc_7C73A|ifnz tmpcnd|;mov ebx, 99|mov ebx, 99|endif|;loc_7C73A: ; limit number of items to 99

loc_25156,1,;pop ebx|POP ebx|;call SR_Sync|PUSH loc_25157|tcall SR_Sync|endp|proc loc_25157 ; call sync after save

loc_22F61,83,;mov eax, 1|mov eax, 1 ; skip cdrom check

loc_25517,7,;mov dword [ebp-8], 1|add tmpadr, ebp, -8|mov tmp1, 1|store tmp1, tmpadr, 4|;cmp dword [Game_ScreenshotEnabled], 0|load tmp1, Game_ScreenshotEnabled, 4|;je loc_25517_end|ctcallz tmp1, loc_25517_2|tcall loc_25517_1|endp|proc loc_25517_1|;cmp dword [eax+12], 0x104|add tmpadr, eax, 12|load tmp1, tmpadr, 1|sub tmp1, tmp1, 0x104|;je loc_255EF|ctcallz tmp1, loc_255EF|tcall loc_25517_2|endp|;loc_25517_end:|proc loc_25517_2 ; screenshot on F4 key

loc_255EF,4,;cmp dword [Game_ScreenshotAutomaticFilename], 0|load tmp1, Game_ScreenshotAutomaticFilename, 4|;jne loc_25615_skip|ctcallnz tmp1, loc_25615|tcall loc_255F3|endp|proc loc_255F3|;mov byte [ebp-0x74], 0|add tmpadr, ebp, -0x74|mov tmp1, 0|store8 tmp1, tmpadr, 1; skip getting filename for screenshot
loc_25615,2,;loc_25615_skip:|;xor eax, eax|mov eax, 0 ; skip getting filename for screenshot

loc_33A02,6,;call SR_SMK_ShowMenu|PUSH loc_33A08|tcall SR_SMK_ShowMenu|endp|proc loc_33A08 ; play intro video on start in game
loc_33EB5,18,;call SR_SMK_PlayIntro|PUSH loc_33EC7|tcall SR_SMK_PlayIntro|endp|proc loc_33EC7 ; play intro video in game
loc_33F10,18,;call SR_SMK_PlayCredits|PUSH loc_33F22|tcall SR_SMK_PlayCredits|endp|proc loc_33F22 ; play credits video in game

loc_8151C,2454, ; AIL code 1
loc_81EB2,183, ; CLIB code 1

loc_8326A,63, ; CLIB code 2
loc_832A9,1057, ; BBERROR code
loc_836CA,221, ; CLIB code 3.1

loc_837FE,34, ; CLIB code 3.3
loc_83820,6476, ; BASEMEM code
loc_8516C,1531, ; CLIB code 4
loc_85767,4674, ; BBDOS code

loc_87882,72, ; DEAD code
loc_882CE,1127, ; DEAD code

loc_8BF0B,21323, ; AIL code 2
loc_91256,842, ; CLIB code 5
loc_915A0,5677, ; BBOPM code
loc_92BCD,74, ; CLIB code 6.1

loc_9434A,102, ; CLIB code 7.2

loc_94E5F,355, ; CLIB code 8.1

loc_94FDD,496, ; CLIB code 8.3

loc_952AA,176, ; CLIB code 8.5

loc_95360,6, ; CLIB code 8.7

loc_9665A,41, ; CLIB code 9

loc_9764F,131, ; CLIB code 10.1

loc_976E0,154, ; CLIB code 10.3

loc_A9B04,2402, ; AIL code 3
loc_AA466,2448, ; CLIB code

loc_AADF7,4314, ; CLIB code

loc_ABEDD,894, ; CLIB code

loc_AC270,176, ; CLIB code
loc_AC320,2551, ; AIL code 4
loc_ACD17,201, ; CLIB code
loc_ACDE0,25586, ; AIL code 5
loc_B31D2,697,proc loc_B348B ; CLIB code

loc_C128B,886, ; CLIB code

loc_C1629,6083, ; CLIB code
loc_C2DEC,8501, ; AIL code 6
loc_C4F21,79, ; CLIB code

loc_CBD53,535,proc loc_CBF6A ; CLIB code

loc_CF192,1074, ; CLIB code
loc_CF5C4,990, ; DEAD code
loc_CF9A2,3383, ; CLIB code

loc_E0000,5307, ; ASM code 2.1

loc_E14D2,512, ; ASM code 2.3

loc_110000,745, ; ASM code 5

loc_131DD4,4, ; AIL data
loc_132ACC,3992, ; AIL data
loc_133B00,240, ; AIL data
loc_133BF8,1156, ; AIL data
loc_13FC3C,12, ; AIL data
loc_13FF00,4, ; AIL data
loc_14032C,8, ; AIL data
loc_1405DC,3284, ; AIL data
loc_1890FC,68, ; AIL data
loc_199440,64, ; AIL data
loc_199580,128, ; AIL data
loc_199628,600, ; AIL data

loc_133BF0,8, ; CLIB data
loc_13413C,424, ; CLIB data
loc_1343E8,208, ; CLIB data
loc_13FC48,20, ; CLIB data
loc_13FC88,16, ; CLIB data
loc_13FCE8,12, ; CLIB data
loc_13FF04,16, ; CLIB data
loc_140030,536, ; CLIB data
loc_140334,72, ; CLIB data
loc_140384,160, ; CLIB data
loc_143D94,208, ; CLIB data
loc_146EA8,92, ; CLIB data
loc_146F80,88, ; CLIB data
loc_17E8F0,1024, ; CLIB data
loc_17F0E4,28, ; CLIB data
loc_196D14,4, ; CLIB data
loc_196D1C,4, ; CLIB data
loc_199600,40, ; CLIB data
loc_1A5E40,184, ; CLIB data
loc_1A5EF8,40, ; DEAD data

loc_131DE4,112, ; BBERROR data
loc_13FC5C,44, ; BBERROR data
loc_17E578,884, ; BBERROR data

loc_131E54,1284, ; BASEMEM data
loc_13FC98,80, ; BASEMEM data
loc_17ECF0,1012, ; BASEMEM data

loc_132358,792, ; BBDOS data
loc_13FCF4,32, ; BBDOS data
loc_17F100,10840, ; BBDOS data

loc_133A64,116, ; BBOPM data

loc_1B0000,76, ; ASM data
loc_1B0050,7776, ; ASM data
