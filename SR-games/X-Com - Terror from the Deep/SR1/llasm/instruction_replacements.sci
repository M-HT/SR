loc_44128,6,;mov ebx, [Game_ScreenWindow]|load ebx, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_12F34,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_3FB4F,5,;add eax, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add eax, eax, tmp1 ; use real address of screen window instead of 0x0a0000
loc_4F5D2,10,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4|;mov [loc_7CF34], eax|store eax, loc_7CF34, 4 ; use real address of screen window instead of 0x0a0000
loc_4FA48,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000

loc_4DD6F,27,;mov eax, ecx|mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address
loc_4E776,27,;mov eax, ecx|mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12548,22,;inc dword [loc_62210]|load tmp1, loc_62210, 4|add tmp1, tmp1, 1|store tmp1, loc_62210, 4 ; timer interrupt beginning
loc_12568,31,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_503FE,35, ; read sound.cfg
loc_50429,21,;call SR_FillSoundCfg|PUSH loc_50429_after|tcall SR_FillSoundCfg|endp|proc loc_50429_after|;add esp, 2*4|add esp, esp, 2*4 ; read sound.cfg

loc_102D3,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_61388], 0|load tmp1, loc_61388, 4|;jnz loc_102D3|ctcallnz tmp1, loc_102D3|tcall loc_102DC|endp ; update timer - wait for ??? in timer interrupt
loc_1033C,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_61388], 0|load tmp1, loc_61388, 4|;jnz loc_1033C|ctcallnz tmp1, loc_1033C|tcall loc_10345|endp ; update timer
loc_3F9F5,2,;call SR_RunTimerDelay|call SR_RunTimerDelay|;xor ebx, ebx|mov ebx, 0 ; update timer

loc_1E2E1,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6135C], 0|load tmp1, loc_6135C, 4|;jz loc_1E2F3|ctcallz tmp1, loc_1E2F3|tcall loc_1E2E1_after|endp|proc loc_1E2E1_after; update timer - wait for mouse click in timer interrupt
loc_26953,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6135C], 0|load tmp1, loc_6135C, 4|;jz loc_26953|ctcallz tmp1, loc_26953|tcall loc_2695C|endp ; update timer
loc_27379,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6135C], 0|load tmp1, loc_6135C, 4|;jnz loc_27379|ctcallnz tmp1, loc_27379|tcall loc_27382|endp ; update timer
loc_27A8E,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6135C], 0|load tmp1, loc_6135C, 4|;jz loc_27AA5|ctcallz tmp1, loc_27AA5|tcall loc_27A8E_after|endp|proc loc_27A8E_after ; update timer
loc_27B93,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6135C], 0|load tmp1, loc_6135C, 4|;jz loc_27BAA|ctcallz tmp1, loc_27BAA|tcall loc_27B93_after|endp|proc loc_27B93_after ; update timer
loc_27C3D,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6135C], 0|load tmp1, loc_6135C, 4|;jz loc_27C54|ctcallz tmp1, loc_27C54|tcall loc_27C3D_after|endp|proc loc_27C3D_after ; update timer
loc_29C10,8,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp edi, [loc_6135C]|load tmp1, loc_6135C, 4|cmoveq edi, tmp1, tmp2, 0, 1|;jnz loc_29C10|ctcallnz tmp2, loc_29C10|tcall loc_29C18|endp ; update timer
loc_3FD37,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6135C], 0|load tmp1, loc_6135C, 4|;jnz loc_3FD37|ctcallnz tmp1, loc_3FD37|tcall loc_3FD40|endp ; update timer

loc_113CC,10,;mov eax, ecx|mov eax, ecx|;call SR_feof|PUSH loc_113CC_after_1|tcall SR_feof|endp|proc loc_113CC_after_1|;test eax, eax|;jnz loc_114D6|ctcallnz eax, loc_114D6|tcall loc_113CC_after_2|endp|proc loc_113CC_after_2 ; fix reading FILE internals (eof flag)
loc_114D6,2,;loc_114D6:|proc loc_114D6|;mov eax, ecx|mov eax, ecx ; define label
loc_115BE,10,;mov eax, ecx|mov eax, ecx|;call SR_feof|PUSH loc_115BE_after_1|tcall SR_feof|endp|proc loc_115BE_after_1|;test eax, eax|;jnz loc_11696|ctcallnz eax, loc_11696|tcall loc_115BE_after_2|endp|proc loc_115BE_after_2 ; fix reading FILE internals (eof flag)
loc_11696,2,;loc_11696:|proc loc_11696|;mov eax, ecx|mov eax, ecx ; define label

loc_44893,5,;mov eax, [Game_stdout]|load eax, Game_stdout, 4 ; get stdout

loc_15237,8,;call SR_filelength2|PUSH loc_15237_after|tcall SR_filelength2|endp|proc loc_15237_after ; get file length
loc_248C4,8,;call SR_filelength2|PUSH loc_248C4_after|tcall SR_filelength2|endp|proc loc_248C4_after ; get file length
loc_249E9,8,;call SR_filelength2|PUSH loc_249E9_after|tcall SR_filelength2|endp|proc loc_249E9_after ; get file length
loc_24B31,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_24B31_after|tcall SR_filelength2|endp|proc loc_24B31_after ; get file length

loc_1571E,1,;pop edi|POP edi|;call SR_Sync|PUSH loc_1571E_after|tcall SR_Sync|endp|proc loc_1571E_after ; call sync after save
loc_1596D,1,;pop ebp|POP ebp|;call SR_Sync|PUSH loc_1596D_after|tcall SR_Sync|endp|proc loc_1596D_after ; call sync after save
loc_15D56,1,;pop edx|POP edx|;call SR_Sync|PUSH loc_15D56_after|tcall SR_Sync|endp|proc loc_15D56_after ; call sync after save
loc_3AE62,1,;pop edx|POP edx|;call SR_Sync|PUSH loc_3AE62_after|tcall SR_Sync|endp|proc loc_3AE62_after ; call sync after save

loc_2B5AD,8,;call SR_SlowDownMainLoop|call SR_SlowDownMainLoop|;test eax, eax|;jz loc_2B615|ctcallz eax, loc_2B615|tcall loc_2B5AD_after|endp|proc loc_2B5AD_after ; slow down game main loop

loc_2CA8E,2,;add edx, eax|add edx, edx, eax|;mov esi, 0xffffffff|mov esi, 0xffffffff ; fix reading not reinitialized variable

loc_50B5E,245,;mov eax, loc_7D540|mov eax, loc_7D540|;mov edx, [ebp-12]|sub tmpadr, ebp, 12|load edx, tmpadr, 4|;mov ebx, [loc_7D52C]|load ebx, loc_7D52C, 4|;call SR_ReadSong|PUSH loc_50B5E_after_1|tcall SR_ReadSong|endp|proc loc_50B5E_after_1|;or eax, eax|;jz loc_50C87|ctcallz eax, loc_50C87|tcall loc_50B5E_after_2|endp|proc loc_50B5E_after_2 ; music - load midi or song from catalog

loc_4F3D5,107, ; disable checks to allow animated videos
loc_5133F,3,;add esp, 4|add esp, esp, 4|;call SR_StartAnimVideo|PUSH loc_5133F_after|tcall SR_StartAnimVideo|endp|proc loc_5133F_after ; set sound on start of animated video
loc_513B9,3,;add esp, 3*4|add esp, esp, 3*4|;call SR_StopAnimVideo|PUSH loc_513B9_after|tcall SR_StopAnimVideo|endp|proc loc_513B9_after ; restore sound on stop of animated video

loc_13B48,1,;cmp dword [Game_PlayIntro], 0|load tmp1, Game_PlayIntro, 4|;je loc_13C36|ctcallz tmp1, loc_13C36|tcall loc_13B48_after|endp|proc loc_13B48_after|;push edi|PUSH edi|tcall loc_13B49|endp ; skip intro

loc_4B002,5,;call loc_10170|PUSH loc_4B002_after_1|tcall loc_10170|endp|proc loc_4B002_after_1|;mov eax, 5|mov eax, 5|;call SR_WaitVerticalRetraceTicks2|PUSH loc_4B002_after_2|tcall SR_WaitVerticalRetraceTicks2|endp|proc loc_4B002_after_2 ; slow down animation
loc_4B079,3,;sub edx, 20|sub edx, edx, 20|;mov eax, 5|mov eax, 5|;call SR_WaitVerticalRetraceTicks2|PUSH loc_4B079_after|tcall SR_WaitVerticalRetraceTicks2|endp|proc loc_4B079_after ; slow down animation


loc_51DB9,2323, ; CLIB code
loc_52723,45, ; CLIB code
loc_5278A,1614, ; CLIB code
loc_52DF5,1110, ; CLIB code
loc_5324C,13043,proc loc_5653F ; CLIB code
loc_568B9,147, ; CLIB code
loc_59766,4842, ; CLIB code

loc_6100C,380, ; CLIB data
loc_6128C,208, ; CLIB data
loc_74E88,656, ; CLIB data
loc_75120,360, ; CLIB data
loc_75304,192, ; CLIB data

loc_7D5DC,16, ; CLIB data
loc_7D5F0,176, ; CLIB data
