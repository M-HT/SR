loc_109D4,7,;push eax|PUSH eax|;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4|;mov [ebp-8], eax|add tmpadr, ebp, -8|store eax, tmpadr, 4|;pop eax|POP eax ; get real address of screen window instead of 0x0a0000
loc_10A27,7,;push eax|PUSH eax|;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4|;mov [ebp-8], eax|add tmpadr, ebp, -8|store eax, tmpadr, 4|;pop eax|POP eax ; get real address of screen window instead of 0x0a0000
loc_10B74,7,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4|;mov [ebp-0x2c], eax|add tmpadr, ebp, -44|store eax, tmpadr, 4 ; get real address of screen window instead of 0x0a0000
loc_10ED4,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_1100F,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000

loc_1386A,27,;mov eax, ecx|mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_111A5,22,;inc dword [loc_2076A]|load tmp1, loc_2076A, 2|add tmp1, tmp1, 1|store tmp1, loc_2076A, 2 ; timer interrupt beginning
loc_111C1,39,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_11447,35, ; read sound.cfg
loc_11472,21,;call SR_FillSoundCfg|PUSH loc_11487|tcall SR_FillSoundCfg|endp|proc loc_11487|;add esp, 2*4|add esp, esp, 2*4 ; read sound.cfg

loc_1047F,7,;movsx edx, word [loc_212E2]|load16s edx, loc_212E2, 2|;call SR_RunTimerDelay|call SR_RunTimerDelay ; update timer - wait for ??? in timer interrupt
loc_10519,7,;movsx edx, word [loc_212E2]|load16s edx, loc_212E2, 2|;call SR_RunTimerDelay|call SR_RunTimerDelay ; update timer
loc_107F8,6,;mov ax, [loc_212E2]|load16z tmp1, loc_212E2, 2|and eax, eax, 0xffff0000|or eax, eax, tmp1|;call SR_RunTimerDelay|call SR_RunTimerDelay ; update timer
loc_1091C,6,;mov ax, [loc_212E2]|load16z tmp1, loc_212E2, 2|and eax, eax, 0xffff0000|or eax, eax, tmp1|;call SR_RunTimerDelay|call SR_RunTimerDelay ; update timer
loc_10B2D,4,;movzx eax, word [ebp-0xc]|add tmpadr, ebp, -12|load16z eax, tmpadr, 2|;call SR_RunTimerDelay|call SR_RunTimerDelay ; update timer

loc_11B11,220,;lea eax, [ebp-0x40]|add eax, ebp, -64|;mov edx, [ebp-8]|add tmpadr, ebp, -8|load edx, tmpadr, 4|;mov ebx, [loc_21774]|load ebx, loc_21774, 4|;call SR_ReadSong|PUSH loc_11BEC|tcall SR_ReadSong|endp|proc loc_11BEC|;or eax, eax|;jz loc_11C21|ctcallz eax, loc_11C21|tcall loc_11BED|endp|proc loc_11BED ; music - load midi or song from catalog


loc_111E8,72, ; CLIB code
loc_12B02,258, ; CLIB code
loc_12C1F,2409, ; CLIB code
loc_13AB2,2312, ; CLIB code
loc_143E2,8452, ; CLIB code

loc_20BC8,96, ; CLIB data
loc_20C3C,712, ; CLIB data

loc_218BC,20, ; CLIB data
loc_218D4,8, ; CLIB data
