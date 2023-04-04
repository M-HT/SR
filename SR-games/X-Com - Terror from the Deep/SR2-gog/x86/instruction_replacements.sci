loc_12E95,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_12EA4,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_12F86,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_20C17,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_396DF,5,add eax, [Game_ScreenWindow] ; use real address of screen window instead of 0x0a0000
loc_48128,6,mov ebx, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000

loc_4D383,27,mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12538,22,inc dword [loc_71BF4] ; timer interrupt beginning
loc_12558,31,ret ; timer interrupt ending

loc_4DCBD,35, ; read sound.cfg
loc_4DCE8,21,call SR_FillSoundCfg|add esp, 2*4 ; read sound.cfg

loc_102D3,7,call SR_RunTimerDelay|cmp dword [loc_70D6C], 0 ; update timer - wait for ??? in timer interrupt
loc_1033C,7,call SR_RunTimerDelay|cmp dword [loc_70D6C], 0 ; update timer
loc_39564,2,call SR_RunTimerDelay|xor esi, esi ; update timer

loc_29B4A,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer - wait for mouse click in timer interrupt
loc_2D52C,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_2D5F9,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_31104,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_32F5F,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_38EF8,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_39307,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_3960E,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_39AE9,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_39FA7,6,call SR_RunTimerDelay|cmp ecx, [loc_70D40] ; update timer
loc_3A5CF,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_3A740,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_3AA69,6,call SR_RunTimerDelay|cmp eax, [loc_70D40] ; update timer
loc_3AB50,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer
loc_3ADE4,7,call SR_RunTimerDelay|cmp dword [loc_70D40], 0 ; update timer

loc_113BC,4,mov eax, ecx|call SR_feof|test eax, eax ; fix reading FILE internals (eof flag) (('test ...' must set flags for 'jnz'))

loc_50162,5,mov edx, [Game_stderr] ; get stderr
loc_50171,5,mov edx, [Game_stderr] ; get stderr
loc_50182,5,mov edx, [Game_stderr] ; get stderr
loc_50196,5,mov edx, [Game_stderr] ; get stderr

loc_1E957,8,mov eax, esi|call SR_filelength2 ; get file length
loc_1EB31,13,mov edx,1|call SR_filelength2 ; get file length
loc_1F392,8,mov eax, edx|call SR_filelength2 ; get file length
loc_35131,8,call SR_filelength2 ; get file length
loc_353B5,13,mov esi,0x18|call SR_filelength2 ; get file length
loc_355DE,13,mov edx,1|call SR_filelength2 ; get file length

loc_2CEF4,1,pop edi|call SR_Sync ; call sync after save
loc_2D7DE,1,pop ebp|call SR_Sync ; call sync after save

loc_48210,2,call SR_SlowDownMainLoop|test eax, eax ; slow down game main loop

loc_42E55,3,sub esp, 0x3c|call SR_SlowDownScrolling ; slow down game scrolling, ...

loc_4668C,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov edx, ecx ; skip game scrolling slow down once
loc_4681A,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov edx, ebx ; skip game scrolling slow down once
loc_469B0,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov eax, ecx ; skip game scrolling slow down once
loc_46B10,2,mov dword [Game_Skip_Scrolling_SlowDown], 1|mov edx, esi ; skip game scrolling slow down once

loc_4E40A,245,mov eax, loc_A499C|mov edx, [ebp-12]|mov ebx, [loc_A4984]|call SR_ReadSong|or eax, eax|jz loc_4E533 ; music - load midi or song from catalog

loc_41BDD,6,mov ebp, [loc_95370]|or ebp, ebp|jne loc_41BDD|mov ebp, [Zero_Segment]|loc_41BDD: ; fix reading from NULL pointer

loc_439D8,2,mov al, bl|or edx, edx|jne loc_439D8|mov edx, [Zero_Segment]|loc_439D8: ; fix reading from NULL pointer (potential)

loc_439EB,2,or ebx, ebx|jne loc_439EB|mov ebx, [Zero_Segment]|loc_439EB:|add ebx, ecx ; fix reading from NULL pointer (potential)

loc_23C8B,2,xor ch, ch|or edx, edx|jne loc_23C8B|mov edx, [Zero_Segment]|loc_23C8B: ; fix reading from NULL pointer (potential)

loc_2F583,2,add edx, esi|movsx eax, word [loc_9543A]|movsx ecx, word [loc_9543C]|imul eax, ecx|cmp edx, eax|jb loc_2F585|mov eax, [Zero_Segment]|mov dword [eax], 0|jmp short loc_2F593|loc_2F585: ; fix out of bounds access
loc_2F593,1,loc_2F593:|pop esi ; fix out of bounds access


loc_4F759,2559, ; CLIB code
loc_501A2,661, ; CLIB code
loc_504C8,5455, ; CLIB code
loc_51A1D,6, ; CLIB code
loc_51A3A,8590, ; CLIB code

loc_756F8,24, ; CLIB data
loc_75714,640, ; CLIB data
loc_759D4,336, ; CLIB data

loc_A4A38,16, ; CLIB data
loc_A4A4C,48, ; CLIB data
loc_A4A7C,4100, ; stack
