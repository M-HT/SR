loc_43238,6,;mov ebx, [Game_ScreenWindow]|load ebx, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_12F24,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_3EDBF,5,;add eax, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add eax, eax, tmp1 ; use real address of screen window instead of 0x0a0000
loc_4E47D,10,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4|;mov [loc_7CF04], eax|store eax, loc_7CF04, 4 ; use real address of screen window instead of 0x0a0000
loc_4E8F3,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000

loc_4CC1F,27,;mov eax, ecx|mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address
loc_4D626,27,;mov eax, ecx|mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12538,22,;inc dword [loc_621E0]|load tmp1, loc_621E0, 4|add tmp1, tmp1, 1|store tmp1, loc_621E0, 4 ; timer interrupt beginning
loc_12558,31,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_4F295,35, ; read sound.cfg
loc_4F2C0,21,;call SR_FillSoundCfg|PUSH loc_4F2C0_after|tcall SR_FillSoundCfg|endp|proc loc_4F2C0_after|;add esp, 2*4|add esp, esp, 2*4 ; read sound.cfg

loc_102D3,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_61358], 0|load tmp1, loc_61358, 4|;jnz loc_102D3|ctcallnz tmp1, loc_102D3|tcall loc_102DC|endp ; update timer - wait for ??? in timer interrupt
loc_1033C,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_61358], 0|load tmp1, loc_61358, 4|;jnz loc_1033C|ctcallnz tmp1, loc_1033C|tcall loc_10345|endp ; update timer
loc_3EC65,2,;call SR_RunTimerDelay|call SR_RunTimerDelay|;xor ebx, ebx|mov ebx, 0 ; update timer

loc_1E197,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6132C], 0|load tmp1, loc_6132C, 4|;jz loc_1E1A7|ctcallz tmp1, loc_1E1A7|tcall loc_1E197_after|endp|proc loc_1E197_after ; update timer - wait for mouse click in timer interrupt
loc_264F4,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6132C], 0|load tmp1, loc_6132C, 4|;jz loc_264F4|ctcallz tmp1, loc_264F4|tcall loc_264FD|endp ; update timer
loc_26F09,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6132C], 0|load tmp1, loc_6132C, 4|;jnz loc_26F09|ctcallnz tmp1, loc_26F09|tcall loc_26F12|endp ; update timer
loc_27611,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6132C], 0|load tmp1, loc_6132C, 4|;jz loc_27628|ctcallz tmp1, loc_27628|tcall loc_27611_after|endp|proc loc_27611_after ; update timer
loc_27716,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6132C], 0|load tmp1, loc_6132C, 4|;jz loc_2772D|ctcallz tmp1, loc_2772D|tcall loc_27716_after|endp|proc loc_27716_after ; update timer
loc_277C0,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6132C], 0|load tmp1, loc_6132C, 4|;jz loc_277D7|ctcallz tmp1, loc_277D7|tcall loc_277C0_after|endp|proc loc_277C0_after ; update timer
loc_29910,8,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp edi, [loc_6132C]|load tmp1, loc_6132C, 4|cmoveq edi, tmp1, tmp2, 0, 1|;jnz loc_29910|ctcallnz tmp2, loc_29910|tcall loc_29918|endp ; update timer
loc_3EFB9,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6132C], 0|load tmp1, loc_6132C, 4|;jnz loc_3EFB9|ctcallnz tmp1, loc_3EFB9|tcall loc_3EFC2|endp ; update timer

loc_113BC,10,;mov eax, ecx|mov eax, ecx|;call SR_feof|PUSH loc_113BC_after_1|tcall SR_feof|endp|proc loc_113BC_after_1|;test eax, eax|;jnz loc_114C6|ctcallnz eax, loc_114C6|tcall loc_113BC_after_2|endp|proc loc_113BC_after_2 ; fix reading FILE internals (eof flag)
loc_114C6,2,;loc_114C6:|proc loc_114C6|;mov eax, ecx|mov eax, ecx ; define label
loc_115AE,10,;mov eax, ecx|mov eax, ecx|;call SR_feof|PUSH loc_115AE_after_1|tcall SR_feof|endp|proc loc_115AE_after_1|;test eax, eax|;jnz loc_11686|ctcallnz eax, loc_11686|tcall loc_115AE_after_2|endp|proc loc_115AE_after_2 ; fix reading FILE internals (eof flag)
loc_11686,2,;loc_11686:|proc loc_11686|;mov eax, ecx|mov eax, ecx ; define label

loc_439B3,5,;mov eax, [Game_stdout]|load eax, Game_stdout, 4 ; get stdout

loc_151C7,8,;call SR_filelength2|PUSH loc_151C7_after|tcall SR_filelength2|endp|proc loc_151C7_after ; get file length
loc_245C4,8,;call SR_filelength2|PUSH loc_245C4_after|tcall SR_filelength2|endp|proc loc_245C4_after ; get file length
loc_24711,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_24711_after|tcall SR_filelength2|endp|proc loc_24711_after ; get file length

loc_1569C,1,;pop edi|POP edi|;call SR_Sync|PUSH loc_1569C_after|tcall SR_Sync|endp|proc loc_1569C_after ; call sync after save
loc_158F0,1,;pop ebp|POP ebp|;call SR_Sync|PUSH loc_158F0_after|tcall SR_Sync|endp|proc loc_158F0_after ; call sync after save
loc_15CF5,1,;pop edx|POP edx|;call SR_Sync|PUSH loc_15CF5_after|tcall SR_Sync|endp|proc loc_15CF5_after ; call sync after save
loc_3A4F2,1,;pop edx|POP edx|;call SR_Sync|PUSH loc_3A4F2_after|tcall SR_Sync|endp|proc loc_3A4F2_after ; call sync after save

loc_2B26D,8,;call SR_SlowDownMainLoop|call SR_SlowDownMainLoop|;test eax, eax|;jz loc_2B2D8|ctcallz eax, loc_2B2D8|tcall loc_2B26D_after|endp|proc loc_2B26D_after ; slow down game main loop

loc_2C708,2,;add edx, eax|add edx, edx, eax|;mov esi, 0xffffffff|mov esi, 0xffffffff ; fix reading not reinitialized variable

loc_4F9EB,245,;mov eax, loc_7D510|mov eax, loc_7D510|;mov edx, [ebp-12]|sub tmpadr, ebp, 12|load edx, tmpadr, 4|;mov ebx, [loc_7D4FC]|load ebx, loc_7D4FC, 4|;call SR_ReadSong|PUSH loc_4F9EB_after_1|tcall SR_ReadSong|endp|proc loc_4F9EB_after_1|;or eax, eax|;jz loc_4FB14|ctcallz eax, loc_4FB14|tcall loc_4F9EB_after_2|endp|proc loc_4F9EB_after_2 ; music - load midi or song from catalog

loc_4E285,107, ; disable checks to allow animated videos
loc_501C6,3,;add esp, 4|add esp, esp, 4|;call SR_StartAnimVideo|PUSH loc_501C6_after|tcall SR_StartAnimVideo|endp|proc loc_501C6_after ; set sound on start of animated video
loc_50239,3,;add esp, 3*4|add esp, esp, 3*4|;call SR_StopAnimVideo|PUSH loc_50239_after|tcall SR_StopAnimVideo|endp|proc loc_50239_after ; restore sound on stop of animated video

loc_13B36,1,;cmp dword [Game_PlayIntro], 0|load tmp1, Game_PlayIntro, 4|;je loc_13C24|ctcallz tmp1, loc_13C24|tcall loc_13B36_after|endp|proc loc_13B36_after|;push edi|PUSH edi|tcall loc_13B37|endp ; skip intro

loc_49EE9,5,;call loc_10170|PUSH loc_49EE9_after_1|tcall loc_10170|endp|proc loc_49EE9_after_1|;mov eax, 5|mov eax, 5|;call SR_WaitVerticalRetraceTicks2|PUSH loc_49EE9_after_2|tcall SR_WaitVerticalRetraceTicks2|endp|proc loc_49EE9_after_2 ; slow down animation
loc_49F57,3,;sub edx, 20|sub edx, edx, 20|;mov eax, 5|mov eax, 5|;call SR_WaitVerticalRetraceTicks2|PUSH loc_49F57_after|tcall SR_WaitVerticalRetraceTicks2|endp|proc loc_49F57_after ; slow down animation


loc_50C20,2323, ; CLIB code
loc_5158A,45, ; CLIB code
loc_515F1,1623, ; CLIB code
loc_51C65,1110, ; CLIB code
loc_520BC,13012,proc loc_55390 ; CLIB code
loc_5570C,148,tcall loc_5570C|endp ; CLIB code
loc_585BA,4838, ; CLIB code

loc_60FDC,380, ; CLIB data
loc_6125C,208, ; CLIB data
loc_74E58,656, ; CLIB data
loc_750F0,360, ; CLIB data
loc_752D4,192, ; CLIB data

loc_7D5AC,16, ; CLIB data
loc_7D5C0,176, ; CLIB data
