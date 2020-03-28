loc_113c3,5,mov edx, 10000h ; Use VESA Video mode instead of x-mode

loc_8703e,3,mov [loc_181FD0 + 4], eax|shr eax, 4 ; VESA: Return VBE Controller Information: set esi in call structure to actual address of VbeInfoBlock

loc_87140,3,mov [loc_181FD0 + 4], eax|shr eax, 4 ; VESA: Return VBE mode Information: set esi in call structure to actual address of VbeInfoBlock

loc_8743e,17,mov eax, [Game_ScreenWindow] ; VESA: Return VBE mode information: store real address of screen window instead of false address

loc_875a9,17,mov eax, [Game_ScreenWindow] ; VESA: Return VBE mode information: store real address of screen window instead of false address

loc_2625D,7,mov eax, [Game_stdout]|mov [ebp-0x2c], eax ; fix stdout

loc_364D9,4,or eax, eax|je loc_364DD|mov ax, [eax+0x1e]|loc_364DD: ; fix reading from NULL pointer

loc_F00E7,3,mov edi, [Game_ScreenWindow] ; optimize bank switching
loc_F0139,3,mov edi, [Game_ScreenWindow] ; optimize bank switching

loc_F0C3E,3,add edi, [Game_ScreenWindow] ; optimize bank switching
loc_F0D05,3,mov edi, [Game_ScreenWindow] ; optimize bank switching
loc_F0D9B,3,mov edi, [Game_ScreenWindow] ; optimize bank switching

loc_F1204,3,add esi, [Game_ScreenWindow] ; optimize bank switching
loc_F12CB,3,mov esi, [Game_ScreenWindow] ; optimize bank switching
loc_F1361,3,mov esi, [Game_ScreenWindow] ; optimize bank switching

loc_C63D1,11,imul ecx, byte (- ((loc_C9117 - loc_C6417) / 640))|add ecx, loc_C9117 ; fix length dependent code

loc_C9CD4,7,imul ecx, byte ((loc_CA9AD - loc_C9CDD) / 410)|add ecx, loc_CA9AD ; fix length dependent code

loc_CAA59,12,imul ecx, byte (- ((loc_CBD33 - loc_CAA67) / 401))|add ecx, loc_CBD33 ; fix length dependent code

loc_C11E9,7,mov eax, [Game_VSyncTick] ; timer ticks since midnight
loc_C120F,7,mov eax, [Game_VSyncTick] ; timer ticks since midnight
loc_C1229,2,mov eax, [Game_VSyncTick] ; timer ticks since midnight
loc_C1251,7,mov eax, [Game_VSyncTick] ; timer ticks since midnight
loc_C126B,2,mov eax, [Game_VSyncTick] ; timer ticks since midnight
loc_C1278,7,mov eax, [Game_VSyncTick] ; timer ticks since midnight

loc_B49B0,118,mov eax, [loc_196D04]|mov [self_mod_width], eax ; fix self-modifying code

loc_B4EBA,19,mov eax, 400|mov ebp, [esi]|sub eax, ecx|mul dword [self_mod_width]|add ebp, eax|imul eax, ebx, byte (- ((loc_B6502 - loc_B4F22) / 400))|add eax, loc_B6502 ; fix self-modifying (and length dependent) code
loc_B4EEF,2,div ecx|mov edx, [self_mod_width] ; fix self-modifying (and length dependent) code
loc_B4F21,5601,align 4|loc_B4F22:|%rep 400|self_mod_code_1|%endrep ; fix self-modifying (and length dependent) code

loc_B6629,19,mov eax, 400|mov ebx, ecx|mov ebp, [esi]|inc ebx|sub eax, ecx|mul dword [self_mod_width]|add ebp, eax|imul eax, ebx, byte (- ((loc_B82B2 - loc_B6692) / 400))|add eax, loc_B82B2 ; fix self-modifying (and length dependent) code
loc_B665E,2,div ecx|mov edx, [self_mod_width] ; fix self-modifying (and length dependent) code
loc_B6692,7200,align 4|loc_B6692:|%rep 400|self_mod_code_2|%endrep ; fix self-modifying (and length dependent) code

loc_B83DC,10,mov eax, 400|mov ebp, [esi]|sub eax, ecx|mul dword [self_mod_width]|add ebp, eax|imul eax, ebx, byte (- ((loc_BACE0 - loc_B8440) / 400))|add eax, loc_BACE0 ; fix self-modifying (and length dependent) code
loc_B843F,10401,align 4|loc_B8440:|%rep 400|self_mod_code_3|%endrep ; fix self-modifying (and length dependent) code

loc_BAFA4,8,mov eax, ecx|inc eax|imul eax, byte (- ((loc_BCC54 - loc_BB034) / 400))|add eax, loc_BCC54 ; fix self-modifying (and length dependent) code
loc_BAFEB,6,mov ebp, [esp]|mov eax, 400|sub eax, [ebp-24h]|mov ebp, [self_mod_width]|mul ebp|mov edx, [loc_1414AC]|add edx, eax ; fix self-modifying (and length dependent) code
loc_BB033,7183,align 4|loc_BB034:|%rep 399|self_mod_code_4|%endrep ; fix self-modifying (and length dependent) code
loc_BCC42,18,self_mod_code_4 ; fix self-modifying (and length dependent) code
loc_BCC7B,13,mov eax, ecx|inc eax|imul eax, byte (- ((loc_BCC54 - loc_BB034) / 400))|add eax, loc_BCC54 ; fix self-modifying (and length dependent) code

loc_6562E,3,or eax, eax|je loc_65631|mov al, [eax+0x1b]|loc_65631: ; fix reading from NULL pointer
loc_657BE,3,or eax, eax|je loc_657C1|mov al, [eax+0x1b]|loc_657C1: ; fix reading from NULL pointer

loc_4A4B7,3,cmp eax, 0x400|jb loc_4A4DB ; fix reading from NULL pointer (probably)

loc_52BD4,3,xor eax, eax|or edx, edx|je loc_52BD7|mov al, [edx+0xe]|loc_52BD7: ; fix reading from NULL pointer
loc_52C40,3,xor eax, eax|or edx, edx|je loc_52C43|mov al, [edx+0x14]|loc_52C43: ; fix reading from NULL pointer

loc_4F43A,3,xor eax, eax|or edx, edx|je loc_4F43D|mov al, [edx+0xe]|loc_4F43D: ; fix reading from NULL pointer
loc_4F4A6,3,or eax, eax|je loc_4F4A9|mov al, [eax+0x1b]|loc_4F4A9: ; fix reading from NULL pointer

loc_656F3,3,or eax, eax|je loc_656F6|mov al, [eax+0x1b]|loc_656F6: ; fix reading from NULL pointer

loc_BF258,2,cmp edx, 1024|jb loc_BF25A|mov ah, [edx]|loc_BF25A: ; fix reading from NULL pointer
loc_BF26A,3,cmp edx, 1024|jb loc_BF28E|test ah, 2 ; fix reading from NULL pointer ('test ah, 2' must set flags for 'jz')

loc_BF2D1,3,cmp eax, 1024|jb loc_BF31C|test byte [eax], 60h ; fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_BF355,3,cmp eax, 1024|jb loc_BF393|test byte [eax], 60h ; fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_C014C,4,cmp eax, 1024|jb loc_C0150|mov dx, [eax+0xa]|loc_C0150: ; fix reading from NULL pointer
loc_C0156,4,cmp eax, 1024|jb loc_C015a|mov ax, [eax+0xc]|loc_C015a: ; fix reading from NULL pointer

loc_BF442,3,cmp eax, 1024|jb loc_BF481|test byte [eax], 60h ; fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_BF3C7,3,cmp eax, 1024|jb loc_BF411|test byte [eax], 60h ; fix reading from NULL pointer ('test byte [eax], 60h' must set flags for 'jz')

loc_4D505,4,or eax, eax|je loc_4D509|mov [eax+0x28], dx ; fix reading from NULL pointer

loc_3664B,3,or edx, edx|je loc_3664E|mov al, [edx+0x1]|loc_3664E: ; fix reading from NULL pointer

loc_548D4,3,or eax, eax|je loc_548D7|mov al, [eax+0x5]|loc_548D7: ; fix reading from NULL pointer
loc_5491D,3,or eax, eax|je loc_54912|add eax, 0x6 ; fix reading from NULL pointer
loc_54950,3,xor edx, edx|or eax, eax|je loc_54953|mov dx, [eax]|loc_54953: ; fix reading from NULL pointer
loc_5495C,4,xor edx, edx|or eax, eax|je loc_54960|mov dx, [eax+0x2]|loc_54960: ; fix reading from NULL pointer

loc_C5EBB,4,add esp, 4 ; fix esp instead of sp (probably mistake ???)

loc_94C79,6,mov [loc_13FFB0], esi|cmp dword [Game_UseEnhanced3DEngine], 0|jz _NonEnhanced3DEngine|call draw_3dscene_proc|jmp _After3DEngine|_NonEnhanced3DEngine: ; call enhanced 3d engine
loc_94C84,5,_After3DEngine:|mov eax, [loc_14A4DA] ; label after 3d engine

loc_7C738,2,mov ebp, esp|cmp ebx, 99|jbe loc_7C73A|cmp ebx, 255|ja loc_7C73A|mov ebx, 99|loc_7C73A: ; limit number of items to 99

loc_CDC08,2,dec bp ; on ARM swap carry bit before or after instruction ('dec bp' must set flags for 'jz')

loc_CDC36,2,rcr edx, 1 ; on ARM swap carry bit before instruction ('rcr edx, 1' must set flags for 'rcr')

loc_CDEA5,2,rcr edx, 1 ; on ARM swap carry bit before instruction ('rcr edx, 1' must set flags for 'rcr')

loc_CDFAD,3,adc eax, 0 ; on ARM swap carry bit before instruction ('adc eax, 0' must set flags for 'adc')

loc_CDFB5,2,rcr edx, 1 ; on ARM swap carry bit before instruction ('rcr edx, 1' must set flags for 'rcr')

loc_CE411,3,rcr dword [edi+4], 1 ; on ARM swap carry bit before instruction

loc_CE473,3,rcr dword [edi+4], 1 ; on ARM swap carry bit before instruction

loc_CE7BD,1,stc ; on ARM swap carry bit after instruction

loc_CE7BE,2,adc edi, edi ; on ARM swap carry bit before instruction

loc_25156,1,pop ebx|call SR_Sync ; call sync after save

loc_22F61,83,mov eax, 1 ; skip cdrom check

loc_25517,7,mov dword [ebp-8], 1|cmp dword [Game_ScreenshotEnabled], 0|je loc_25517_end|cmp dword [eax+12], 0x104|je loc_255EF|loc_25517_end: ; screenshot on F4 key

loc_255EF,4,cmp dword [Game_ScreenshotAutomaticFilename], 0|jne loc_25615_skip|mov byte [ebp-0x74], 0 ; skip getting filename for screenshot
loc_25615,2,loc_25615_skip:|xor eax, eax ; skip getting filename for screenshot

loc_8151C,2454, ; AIL code 1
loc_81EB2,183, ; CLIB code 1

loc_8326A,63, ; CLIB code 2
loc_832A9,1057, ; BBERROR code
loc_836CA,221, ; CLIB code 3.1

loc_837FE,34, ; CLIB code 3.3
loc_83820,6476, ; BASEMEM code
loc_8516C,1531, ; CLIB code 4
loc_85767,4674, ; BBDOS code

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
loc_B31D2,698, ; CLIB code
loc_B348C,4,db 48|db 48|db 48|db 0

loc_C128B,886, ; CLIB code

loc_C1629,6083, ; CLIB code
loc_C2DEC,8501, ; AIL code 6
loc_C4F21,79,align 16 ; CLIB code

loc_CBD53,535, ; CLIB code

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
loc_1A5F20,16384, ; stack

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
