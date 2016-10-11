loc_44128,6,mov ebx, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_12F34,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_3FB4F,5,add eax, [Game_ScreenWindow] ; use real address of screen window instead of 0x0a0000
loc_4F5D2,10,mov eax, [Game_ScreenWindow]|mov [loc_7CF34], eax ; use real address of screen window instead of 0x0a0000
loc_4FA48,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000

loc_4DD6F,27,mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address
loc_4E776,27,mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12548,22,inc dword [loc_62210] ; timer interrupt beginning
loc_12568,31,ret ; timer interrupt ending

loc_503FE,35, ; read sound.cfg
loc_50429,21,call SR_FillSoundCfg|add esp, 2*4 ; read sound.cfg

loc_102D3,7,call SR_RunTimerDelay|cmp dword [loc_61388], 0 ; update timer - wait for ??? in timer interrupt
loc_1033C,7,call SR_RunTimerDelay|cmp dword [loc_61388], 0 ; update timer
loc_3F9F5,2,call SR_RunTimerDelay|xor ebx, ebx ; update timer

loc_1E2E1,7,call SR_RunTimerDelay|cmp dword [loc_6135C], 0 ; update timer - wait for mouse click in timer interrupt
loc_26953,7,call SR_RunTimerDelay|cmp dword [loc_6135C], 0 ; update timer
loc_27379,7,call SR_RunTimerDelay|cmp dword [loc_6135C], 0 ; update timer
loc_27A8E,7,call SR_RunTimerDelay|cmp dword [loc_6135C], 0 ; update timer
loc_27B93,7,call SR_RunTimerDelay|cmp dword [loc_6135C], 0 ; update timer
loc_27C3D,7,call SR_RunTimerDelay|cmp dword [loc_6135C], 0 ; update timer
loc_29C10,6,call SR_RunTimerDelay|cmp edi, [loc_6135C] ; update timer
loc_3FD37,7,call SR_RunTimerDelay|cmp dword [loc_6135C], 0 ; update timer

loc_44893,5,mov eax, [Game_stdout] ; get stdout

loc_15237,8,call SR_filelength2 ; get file length
loc_248C4,8,call SR_filelength2 ; get file length
loc_249E9,8,call SR_filelength2 ; get file length
loc_24B31,13,mov edx,1|call SR_filelength2 ; get file length

loc_1571E,1,pop edi|call SR_Sync ; call sync after save
loc_1596D,1,pop ebp|call SR_Sync ; call sync after save
loc_15D56,1,pop edx|call SR_Sync ; call sync after save
loc_3AE62,1,pop edx|call SR_Sync ; call sync after save

loc_2B5AD,2,call SR_SlowDownMainLoop|test eax, eax ; slow down game main loop

loc_2CA8E,2,add edx, eax|mov esi, 0xffffffff ; fix reading not reinitialized variable

loc_50B5E,245,mov eax, loc_7D540|mov edx, [ebp-12]|mov ebx, [loc_7D52C]|call SR_ReadSong|or eax, eax|jz loc_50C87 ; music - load midi or song from catalog

loc_581E0,2,adc eax, eax ; on ARM swap carry bit before instruction ('adc eax, eax' must set flags for 'adc')

loc_58208,2,jnb short loc_5821B ; on ARM swap carry bit after instruction

loc_58477,2,jnb short loc_5847E ; on ARM swap carry bit after instruction
loc_5847D,1,inc edi|loc_5847E: ; define label

loc_58581,3,adc eax, 0 ; on ARM swap carry bit before instruction ('adc eax, 0' must set flags for 'adc')
loc_58587,2,jnb short loc_5859A ; on ARM swap carry bit after instruction

loc_589E3,2,jnb short loc_589EC ; on ARM swap carry bit after instruction

loc_58A45,2,jnb short loc_58A4E ; on ARM swap carry bit after instruction

loc_58D92,2,adc edi, edi ; on ARM swap carry bit before instruction

