loc_3E618,6,mov ebx, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_13024,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_3A3D9,5,add eax, [Game_ScreenWindow] ; use real address of screen window instead of 0x0a0000

loc_47693,27,mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12638,22,inc dword [loc_61C94] ; timer interrupt beginning
loc_12658,31,ret ; timer interrupt ending

loc_48150,35, ; read sound.cfg
loc_4817B,21,call SR_FillSoundCfg|add esp, 2*4 ; read sound.cfg

loc_153BB,5,mov ebx, 0x40 ; fix for the "Difficulty bug"

loc_102D3,7,call SR_RunTimerDelay|cmp dword [loc_60E0C], 0 ; update timer - wait for ??? in timer interrupt
loc_1033C,7,call SR_RunTimerDelay|cmp dword [loc_60E0C], 0 ; update timer
loc_3A255,2,call SR_RunTimerDelay|xor ebx, ebx ; update timer

loc_1A9EE,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer - wait for mouse click in timer interrupt
loc_1C630,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer
loc_2319E,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer
loc_24C2C,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer
loc_25619,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer
loc_25C97,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer
loc_25D98,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer
loc_25E30,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer
loc_25EDB,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer
loc_28209,6,call SR_RunTimerDelay|cmp esi, [loc_60DE0] ; update timer
loc_3A61D,7,call SR_RunTimerDelay|cmp dword [loc_60DE0], 0 ; update timer

loc_10641,6,call SR_RunTimerDelay|cmp eax, [loc_60E34] ; update timer - wait for ??? in timer interrupt

loc_10587,7,call SR_RunTimerDelay|cmp dword [loc_6100C], 0 ; update timer - wait for ??? in timer interrupt

loc_105CC,7,call SR_RunTimerDelay|cmp dword [loc_61010], 0 ; update timer - wait for ??? in timer interrupt

loc_114BE,4,mov eax, ecx|call SR_feof|test eax, eax ; fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))
loc_116AC,4,mov eax, ecx|call SR_feof|test eax, eax ; fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))

loc_3ED69,5,mov eax, [Game_stdout] ; get stdout

loc_22D1C,8,call SR_filelength2 ; get file length
loc_22E78,13,mov edx,1|call SR_filelength2 ; get file length

loc_14155,1,pop edi|call SR_Sync ; call sync after save
loc_1439F,1,pop ebp|call SR_Sync ; call sync after save
loc_147A5,1,pop edx|call SR_Sync ; call sync after save
loc_36B87,1,pop edx|call SR_Sync ; call sync after save

loc_29A9D,2,call SR_SlowDownMainLoop|test eax, eax ; slow down game main loop

loc_1EB28,2,xor edx, edx|mov [esp+0x0c], edx ; fix reading uninitialized variable

loc_2B101,2,sub eax, edx|mov esi, 0xffffffff ; fix reading not reinitialized variable

loc_48669,220,lea eax, [ebp-0x40]|mov edx, [ebp-8]|mov ebx, [loc_6C918]|call SR_ReadSong|or eax, eax|jz loc_48779 ; music - load midi or song from catalog

loc_44F20,1,push edx|movsx eax, word [loc_6C502]|cmp eax, 90|jge loc_44F20_after|mov word [loc_6C502], 90|loc_44F20_after: ; fix reading uninitialized/not reinitialized variable

loc_44F09,5,call loc_10180|mov eax, 5|call SR_WaitVerticalRetraceTicks2 ; slow down animation
loc_44F51,3,sub edx, 20|mov eax, 5|call SR_WaitVerticalRetraceTicks2 ; slow down animation

loc_4F684,2,adc eax, eax ; on ARM swap carry bit before instruction ('adc eax, eax' must set flags for 'adc')

loc_4F6AC,2,jnb short loc_4F6BF ; on ARM swap carry bit after instruction

loc_4F91B,2,jnb short loc_4F922 ; on ARM swap carry bit after instruction
loc_4F921,1,inc edi|loc_4F922: ; define label

loc_4FA25,3,adc eax, 0 ; on ARM swap carry bit before instruction ('adc eax, 0' must set flags for 'adc')

loc_4FA2B,2,jnb short loc_4FA3E ; on ARM swap carry bit after instruction

loc_4FE87,2,jnb short loc_4FE90 ; on ARM swap carry bit after instruction

loc_4FEE9,2,jnb short loc_4FEF2 ; on ARM swap carry bit after instruction

loc_50236,2,adc edi, edi ; on ARM swap carry bit before instruction


loc_476BA,2195, ; CLIB code
loc_49538,264, ; CLIB code
loc_49697,45, ; CLIB code
loc_496FE,2106, ; CLIB code
loc_49F55,124, ; CLIB code
loc_49FD2,2484, ; CLIB code
loc_4AF0A,10969, ; CLIB code
loc_50C0A,4828, ; CLIB code

loc_60A9C,368, ; CLIB data
loc_60D10,208, ; CLIB data
loc_690BC,636, ; CLIB data
loc_69340,380, ; CLIB data
loc_69538,192, ; CLIB data

loc_6C9FC,16, ; CLIB data
loc_6CA10,176, ; CLIB data
loc_6CAC0,4096, ; stack
