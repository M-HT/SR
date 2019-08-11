loc_12F95,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_12FA4,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_13086,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_20AD9,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_37639,5,add eax, [Game_ScreenWindow] ; use real address of screen window instead of 0x0a0000
loc_44F51,6,mov ebx, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000

loc_4AE37,27,mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12638,22,inc dword [loc_71B30] ; timer interrupt beginning
loc_12658,31,ret ; timer interrupt ending

loc_4B8F4,35, ; read sound.cfg
loc_4B91F,21,call SR_FillSoundCfg|add esp, 2*4 ; read sound.cfg

loc_102D3,7,call SR_RunTimerDelay|cmp dword [loc_70CA8], 0 ; update timer - wait for ??? in timer interrupt
loc_1033C,7,call SR_RunTimerDelay|cmp dword [loc_70CA8], 0 ; update timer
loc_374B5,2,call SR_RunTimerDelay|xor esi, esi ; update timer

loc_292DA,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer - wait for mouse click in timer interrupt
loc_2C2AC,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_2C373,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_36E51,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_3724E,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_37562,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_37A79,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_37F13,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_384C8,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_38634,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_38975,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_38A5D,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer
loc_38CEE,7,call SR_RunTimerDelay|cmp dword [loc_70C7C], 0 ; update timer

loc_114BE,4,mov eax, ecx|call SR_feof|test eax, eax ; fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))

loc_4D04C,5,mov edx, [Game_stderr] ; get stderr
loc_4D05B,5,mov edx, [Game_stderr] ; get stderr
loc_4D06C,5,mov edx, [Game_stderr] ; get stderr
loc_4D080,5,mov edx, [Game_stderr] ; get stderr

loc_1EA60,8,mov eax, esi|call SR_filelength2 ; get file length
loc_1EC38,13,mov edx,1|call SR_filelength2 ; get file length
loc_1F216,8,mov eax, edx|call SR_filelength2 ; get file length
loc_331A1,8,call SR_filelength2 ; get file length
loc_33412,13,mov esi,0x18|call SR_filelength2 ; get file length
loc_33676,13,mov edx,1|call SR_filelength2 ; get file length
loc_4543D,13,mov edx,1|call SR_filelength2 ; get file length

loc_2BCD1,1,pop edi|call SR_Sync ; call sync after save
loc_2C563,1,pop ebp|call SR_Sync ; call sync after save

loc_45091,2,call SR_SlowDownMainLoop|test eax, eax ; slow down game main loop

loc_3FE65,3,sub esp, 0x34|call SR_SlowDownScrolling ; slow down game scrolling, ...

loc_43461,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov eax, ecx ; skip game scrolling slow down once
loc_435EF,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov eax, ebx ; skip game scrolling slow down once
loc_4377C,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov eax, ebx ; skip game scrolling slow down once
loc_438DA,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov eax, esi ; skip game scrolling slow down once

loc_4BE13,220,lea eax, [ebp-0x40]|mov edx, [ebp-8]|mov ebx, [loc_936E0]|call SR_ReadSong|or eax, eax|jz loc_4BF23 ; music - load midi or song from catalog

loc_408F8,2,add edx, edx|or eax, eax|jne loc_408F8|mov eax, [Zero_Segment]|loc_408F8: ; fix reading from NULL pointer

loc_40914,2,or ebx, ebx|jne loc_40914|mov ebx, [Zero_Segment]|loc_40914:|add ebx, eax ; fix reading from NULL pointer

loc_23BEB,2,xor ch, ch|or edx, edx|jne loc_23BEB|mov edx, [Zero_Segment]|loc_23BEB: ; fix reading from NULL pointer

loc_2E276,2,add edx, ecx|movsx eax, word [loc_8D0F6]|movsx ecx, word [loc_8D0F8]|imul eax, ecx|cmp edx, eax|jb loc_2E278|mov eax, [Zero_Segment]|mov dword [eax], 0|jmp short loc_2E286|loc_2E278: ; fix out of bounds access
loc_2E286,1,loc_2E286:|pop esi ; fix out of bounds access


loc_4AE5E,2195, ; CLIB code
loc_4CE4E,500, ; CLIB code
loc_4D08C,661, ; CLIB code
loc_4D3B2,3416, ; CLIB code
loc_4E68E,2037, ; CLIB code
loc_4EE89,6, ; CLIB code
loc_4EEA6,8600, ; CLIB code

loc_74FB8,24, ; CLIB data
loc_75020,564, ; CLIB data
loc_75290,340, ; CLIB data

loc_937C4,16, ; CLIB data
loc_937D8,48, ; CLIB data
loc_93808,4104, ; stack
