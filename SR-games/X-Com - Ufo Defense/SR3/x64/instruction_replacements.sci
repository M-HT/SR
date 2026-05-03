loc_109D4,7,;push eax|PUSH32 eax|mov eax, [Game_ScreenWindow]|mov [ebp-8], eax|;pop eax|POP32 eax ; get real address of screen window instead of 0x0a0000
loc_10A27,7,;push eax|PUSH32 eax|mov eax, [Game_ScreenWindow]|mov [ebp-8], eax|;pop eax|POP32 eax ; get real address of screen window instead of 0x0a0000
loc_10B74,7,mov eax, [Game_ScreenWindow]|mov [ebp-0x2c], eax ; get real address of screen window instead of 0x0a0000
loc_10ED4,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_1100F,5,mov edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000

loc_1386A,27,mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_111A5,22,inc dword [loc_2076A] ; timer interrupt beginning
loc_111C1,39,;ret|RET ; timer interrupt ending

loc_11447,35, ; read sound.cfg
loc_11472,21,;call SR_FillSoundCfg|CALL SR_FillSoundCfg|;add esp, 2*4|add r11d, 2*4 ; read sound.cfg

loc_10486,2,;call SR_RunTimerDelay|CALL SR_RunTimerDelay|cmp edx, eax ; update timer - wait for ??? in timer interrupt
loc_10520,2,;call SR_RunTimerDelay|CALL SR_RunTimerDelay|cmp edx, eax ; update timer
loc_107FE,4,;call SR_RunTimerDelay|CALL SR_RunTimerDelay|cmp ax, [ebp-0x18] ; update timer
loc_10922,4,;call SR_RunTimerDelay|CALL SR_RunTimerDelay|cmp ax, [ebp-8] ; update timer
loc_10B31,2,;call SR_RunTimerDelay|CALL SR_RunTimerDelay|cmp edx, eax ; update timer

loc_11B11,220,lea eax, [ebp-0x40]|mov edx, [ebp-8]|mov ebx, [loc_21774]|;call SR_ReadSong|CALL SR_ReadSong|or eax, eax|jz loc_11C21 ; music - load midi or song from catalog

loc_1108A,2,loc_1108A:|mov al, 0x0 ; fix long loop
loc_110B0,2,ACTION_LOOP_BACKWARD|;loop loc_1108A|dec ecx|jnz loc_1108A ; fix long loop

loc_111E8,72, ; CLIB code
loc_12B02,258, ; CLIB code
loc_12C1F,2409, ; CLIB code
loc_13AB2,2312, ; CLIB code
loc_143E2,8452, ; CLIB code

loc_20BC8,96, ; CLIB data
loc_20C3C,712, ; CLIB data

loc_218BC,20, ; CLIB data
loc_218D4,8, ; CLIB data
