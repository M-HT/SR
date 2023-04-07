loc_43238,6,mov ebx, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_12F24,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_3EDBF,5,add eax, [Game_ScreenWindow] ; use real address of screen window instead of 0x0a0000
loc_4E47D,10,mov eax, [Game_ScreenWindow]|mov [loc_7CF04], eax ; use real address of screen window instead of 0x0a0000
loc_4E8F3,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000

loc_4CC1F,27,mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address
loc_4D626,27,mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12538,22,inc dword [loc_621E0] ; timer interrupt beginning
loc_12558,31,ret ; timer interrupt ending

loc_4F295,35, ; read sound.cfg
loc_4F2C0,21,call SR_FillSoundCfg|add esp, 2*4 ; read sound.cfg

loc_102D3,7,call SR_RunTimerDelay|cmp dword [loc_61358], 0 ; update timer - wait for ??? in timer interrupt
loc_1033C,7,call SR_RunTimerDelay|cmp dword [loc_61358], 0 ; update timer
loc_3EC65,2,call SR_RunTimerDelay|xor ebx, ebx ; update timer

loc_1E197,7,call SR_RunTimerDelay|cmp dword [loc_6132C], 0 ; update timer - wait for mouse click in timer interrupt
loc_264F4,7,call SR_RunTimerDelay|cmp dword [loc_6132C], 0 ; update timer
loc_26F09,7,call SR_RunTimerDelay|cmp dword [loc_6132C], 0 ; update timer
loc_27611,7,call SR_RunTimerDelay|cmp dword [loc_6132C], 0 ; update timer
loc_27716,7,call SR_RunTimerDelay|cmp dword [loc_6132C], 0 ; update timer
loc_277C0,7,call SR_RunTimerDelay|cmp dword [loc_6132C], 0 ; update timer
loc_29910,6,call SR_RunTimerDelay|cmp edi, [loc_6132C] ; update timer
loc_3EFB9,7,call SR_RunTimerDelay|cmp dword [loc_6132C], 0 ; update timer

loc_113BC,4,mov eax, ecx|call SR_feof|test eax, eax ; fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))
loc_115AE,4,mov eax, ecx|call SR_feof|test eax, eax ; fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))

loc_439B3,5,mov eax, [Game_stdout] ; get stdout

loc_151C7,8,call SR_filelength2 ; get file length
loc_245C4,8,call SR_filelength2 ; get file length
loc_24711,13,mov edx,1|call SR_filelength2 ; get file length

loc_1569C,1,pop edi|call SR_Sync ; call sync after save
loc_158F0,1,pop ebp|call SR_Sync ; call sync after save
loc_15CF5,1,pop edx|call SR_Sync ; call sync after save
loc_3A4F2,1,pop edx|call SR_Sync ; call sync after save

loc_2B26D,2,call SR_SlowDownMainLoop|test eax, eax ; slow down game main loop

loc_2C708,2,add edx, eax|mov esi, 0xffffffff ; fix reading not reinitialized variable

loc_4F9EB,245,mov eax, loc_7D510|mov edx, [ebp-12]|mov ebx, [loc_7D4FC]|call SR_ReadSong|or eax, eax|jz loc_4FB14 ; music - load midi or song from catalog

loc_4E285,107, ; disable checks to allow animated videos
loc_501C6,3,add esp, 4|call SR_StartAnimVideo ; set sound on start of animated video
loc_50239,3,add esp, 3*4|call SR_StopAnimVideo ; restore sound on stop of animated video

loc_13B36,1,cmp dword [Game_PlayIntro], 0|je loc_13C24|push edi ; skip intro

loc_57034,2,adc eax, eax ; on ARM swap carry bit before instruction ('adc eax, eax' must set flags for 'adc')

loc_5705C,2,jnb short loc_5706F ; on ARM swap carry bit after instruction

loc_572CB,2,jnb short loc_572D2 ; on ARM swap carry bit after instruction
loc_572D1,1,inc edi|loc_572D2: ; define label

loc_573D5,3,adc eax, 0 ; on ARM swap carry bit before instruction ('adc eax, 0' must set flags for 'adc')
loc_573DB,2,jnb short loc_573EE ; on ARM swap carry bit after instruction

loc_57837,2,jnb short loc_57840 ; on ARM swap carry bit after instruction

loc_57899,2,jnb short loc_578A2 ; on ARM swap carry bit after instruction

loc_57BE6,2,adc edi, edi ; on ARM swap carry bit before instruction


loc_50C20,2323, ; CLIB code
loc_5158A,45, ; CLIB code
loc_515F1,1623, ; CLIB code
loc_51C65,1110, ; CLIB code
loc_520BC,13012, ; CLIB code
loc_5570C,148, ; CLIB code
loc_585BA,4838, ; CLIB code

loc_60FDC,380, ; CLIB data
loc_6125C,208, ; CLIB data
loc_74E58,656, ; CLIB data
loc_750F0,360, ; CLIB data
loc_752D4,192, ; CLIB data

loc_7D5AC,16, ; CLIB data
loc_7D5C0,176, ; CLIB data
loc_7D670,4096, ; stack
