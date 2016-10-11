loc_12EA5,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_12EB4,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_12F96,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_20FF1,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_3A4EF,5,add eax, [Game_ScreenWindow] ; use real address of screen window instead of 0x0a0000
loc_49378,6,mov ebx, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000

loc_4E787,27,mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12548,22,inc dword [loc_71B90] ; timer interrupt beginning
loc_12568,31,ret ; timer interrupt ending

loc_4F0D2,35, ; read sound.cfg
loc_4F0FD,21,call SR_FillSoundCfg|add esp, 2*4 ; read sound.cfg

loc_102D3,7,call SR_RunTimerDelay|cmp dword [loc_70D08], 0 ; update timer - wait for ??? in timer interrupt
loc_1033C,7,call SR_RunTimerDelay|cmp dword [loc_70D08], 0 ; update timer
loc_3A374,2,call SR_RunTimerDelay|xor esi, esi ; update timer

loc_2A04A,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer - wait for mouse click in timer interrupt
loc_2DC4C,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_2DD19,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_31C62,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_33AA5,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_39CE2,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_3A0F9,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_3A41E,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_3A909,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_3ADD0,6,call SR_RunTimerDelay|cmp ebx, [loc_70CDC] ; update timer
loc_3B3FF,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_3B570,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_3B89D,6,call SR_RunTimerDelay|cmp eax, [loc_70CDC] ; update timer
loc_3B995,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer
loc_3BC84,7,call SR_RunTimerDelay|cmp dword [loc_70CDC], 0 ; update timer

loc_51495,5,mov edx, [Game_stderr] ; get stderr
loc_514A4,5,mov edx, [Game_stderr] ; get stderr
loc_514B5,5,mov edx, [Game_stderr] ; get stderr
loc_514C9,5,mov edx, [Game_stderr] ; get stderr

loc_1EC4E,8,mov eax, esi|call SR_filelength2 ; get file length
loc_1EE51,13,mov edx,1|call SR_filelength2 ; get file length
loc_1F6B9,8,mov eax, ecx|call SR_filelength2 ; get file length
loc_2F5E7,8,call SR_filelength2 ; get file length
loc_35D15,10,xor edx, edx|call SR_filelength2 ; get file length
loc_35F96,13,mov esi,0x18|call SR_filelength2 ; get file length
loc_361BE,13,mov edx,1|call SR_filelength2 ; get file length

loc_2D5F4,1,pop edi|call SR_Sync ; call sync after save
loc_2DEFC,1,pop ebp|call SR_Sync ; call sync after save

loc_49460,2,call SR_SlowDownMainLoop|test eax, eax ; slow down game main loop

loc_43E95,3,sub esp, 0x3c|call SR_SlowDownScrolling ; slow down game scrolling, ...

loc_47876,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov edx, ecx ; skip game scrolling slow down once
loc_47A0F,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov edx, esi ; skip game scrolling slow down once
loc_47BA5,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov edx, ecx ; skip game scrolling slow down once
loc_47D11,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov eax, esi ; skip game scrolling slow down once

loc_4F829,245,mov eax, loc_A344C|mov edx, [ebp-12]|mov ebx, [loc_A3434]|call SR_ReadSong|or eax, eax|jz loc_4F952 ; music - load midi or song from catalog

loc_42C02,6,mov ebp, [loc_93B1C]|or ebp, ebp|jne loc_42C02|mov ebp, [Zero_Segment]|loc_42C02: ; fix reading from NULL pointer

loc_44A84,2,mov al, bl|or edx, edx|jne loc_44A84|mov edx, [Zero_Segment]|loc_44A84: ; fix reading from NULL pointer (potential)

loc_44A99,2,or ebx, ebx|jne loc_44A99|mov ebx, [Zero_Segment]|loc_44A99:|add ebx, ecx ; fix reading from NULL pointer (potential)

loc_240BB,2,xor ch, ch|or edx, edx|jne loc_240BB|mov edx, [Zero_Segment]|loc_240BB: ; fix reading from NULL pointer (potential)

loc_2FED3,2,add edx, esi|movsx eax, word [loc_95656]|movsx ecx, word [loc_95658]|imul eax, ecx|cmp edx, eax|jb loc_2FED5|mov eax, [Zero_Segment]|mov dword [eax], 0|jmp short loc_2FEE3|loc_2FED5: ; fix out of bounds access
loc_2FEE3,1,loc_2FEE3:|pop esi ; fix out of bounds access

