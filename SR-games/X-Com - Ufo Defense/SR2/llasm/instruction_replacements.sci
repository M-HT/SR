loc_12F95,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_12FA4,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_13086,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_20AD9,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_37639,5,;add eax, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add eax, eax, tmp1 ; use real address of screen window instead of 0x0a0000
loc_44F51,6,;mov ebx, [Game_ScreenWindow]|load ebx, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000

loc_4AE37,27,;mov eax, ecx|mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12638,22,;inc dword [loc_71B30]|load tmp1, loc_71B30, 4|add tmp1, tmp1, 1|store tmp1, loc_71B30,4 ; timer interrupt beginning
loc_12658,31,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_4B8F4,35, ; read sound.cfg
loc_4B91F,21,;call SR_FillSoundCfg|PUSH loc_4B91F_after|tcall SR_FillSoundCfg|endp|proc loc_4B91F_after|;add esp, 2*4|add esp, esp, 2*4 ; read sound.cfg

loc_102D3,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CA8], 0|load tmp1, loc_70CA8, 4|;jnz loc_102D3|ctcallnz tmp1, loc_102D3|tcall loc_102DC|endp ; update timer - wait for ??? in timer interrupt
loc_1033C,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CA8], 0|load tmp1, loc_70CA8, 4|;jnz loc_1033C|ctcallnz tmp1, loc_1033C|tcall loc_10345|endp ; update timer
loc_374B5,2,;call SR_RunTimerDelay|call SR_RunTimerDelay|;xor esi, esi|mov esi, 0 ; update timer

loc_292DA,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jnz loc_292DA|ctcallnz tmp1, loc_292DA|tcall loc_292E3|endp ; update timer - wait for mouse click in timer interrupt
loc_2C2AC,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jnz loc_2C2AC|ctcallnz tmp1, loc_2C2AC|tcall loc_2C2B5|endp ; update timer
loc_2C373,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jnz loc_2C373|ctcallnz tmp1, loc_2C373|tcall loc_2C37C|endp ; update timer
loc_36E51,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jz loc_36E3D|ctcallz tmp1, loc_36E3D|tcall loc_36E5A|endp ; update timer
loc_3724E,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jz loc_3724E|ctcallz tmp1, loc_3724E|tcall loc_37257|endp ; update timer
loc_37562,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jnz loc_37562|ctcallnz tmp1, loc_37562|tcall loc_3756B|endp ; update timer
loc_37A79,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jnz loc_37A79|ctcallnz tmp1, loc_37A79|tcall loc_37A82|endp ; update timer
loc_37F13,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jz loc_37F04|ctcallz tmp1, loc_37F04|tcall loc_37F1C|endp ; update timer
loc_384C8,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jz loc_384C3|ctcallz tmp1, loc_384C3|tcall loc_384D1|endp ; update timer
loc_38634,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jz loc_3862F|ctcallz tmp1, loc_3862F|tcall loc_3863D|endp ; update timer
loc_38975,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jz loc_38970|ctcallz tmp1, loc_38970|tcall loc_3897E|endp ; update timer
loc_38A5D,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jz loc_38A4E|ctcallz tmp1, loc_38A4E|tcall loc_38A66|endp ; update timer
loc_38CEE,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70C7C], 0|load tmp1, loc_70C7C, 4|;jz loc_38CEE|ctcallz tmp1, loc_38CEE|tcall loc_38CEE_after|endp|proc loc_38CEE_after ; update timer

loc_114BE,10,;mov eax, ecx|mov eax, ecx|;call SR_feof|PUSH loc_114BE_after_1|tcall SR_feof|endp|proc loc_114BE_after_1|;test eax, eax|;jnz loc_115C3|ctcallnz eax, loc_115C3|tcall loc_114BE_after_2|endp|proc loc_114BE_after_2 ; fix reading FILE internals (eof flag)
loc_115C3,2,;loc_115C3:|proc loc_115C3|;mov eax, ecx|mov eax, ecx ; define label

loc_4D04C,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr
loc_4D05B,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr
loc_4D06C,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr
loc_4D080,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr

loc_1EA60,8,;mov eax, esi|mov eax, esi|;call SR_filelength2|PUSH loc_1EA60_after|tcall SR_filelength2|endp|proc loc_1EA60_after ; get file length
loc_1EC38,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_1EC38_after|tcall SR_filelength2|endp|proc loc_1EC38_after ; get file length
loc_1F216,8,;mov eax, edx|mov eax, edx|;call SR_filelength2|PUSH loc_1F216_after|tcall SR_filelength2|endp|proc loc_1F216_after ; get file length
loc_331A1,8,;call SR_filelength2|PUSH loc_331A1_after|tcall SR_filelength2|endp|proc loc_331A1_after ; get file length
loc_33412,13,;mov esi,0x18|mov esi, 0x18|;call SR_filelength2|PUSH loc_33412_after|tcall SR_filelength2|endp|proc loc_33412_after ; get file length
loc_33676,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_33676_after|tcall SR_filelength2|endp|proc loc_33676_after ; get file length
loc_4543D,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_4543D_after|tcall SR_filelength2|endp|proc loc_4543D_after ; get file length

loc_2BCD1,1,;pop edi|POP edi|;call SR_Sync|PUSH loc_2BCD1_after|tcall SR_Sync|endp|proc loc_2BCD1_after ; call sync after save
loc_2C563,1,;pop ebp|POP ebp|;call SR_Sync|PUSH loc_2C563_after|tcall SR_Sync|endp|proc loc_2C563_after ; call sync after save

loc_45091,8,;call SR_SlowDownMainLoop|call SR_SlowDownMainLoop|;test eax, eax|;jz loc_4510E|ctcallz eax, loc_4510E|tcall loc_45091_after|endp|proc loc_45091_after ; slow down game main loop

loc_3FE65,3,;sub esp, 0x34|sub esp, esp, 0x34|;call SR_SlowDownScrolling|call SR_SlowDownScrolling ; slow down game scrolling, ...

loc_43461,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov eax, ecx|mov eax, ecx ; skip game scrolling slow down once
loc_435EF,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov eax, ebx|mov eax, ebx ; skip game scrolling slow down once
loc_4377C,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov eax, ebx|mov eax, ebx ; skip game scrolling slow down once
loc_438DA,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov eax, esi|mov eax, esi ; skip game scrolling slow down once

loc_4BE13,220,;lea eax, [ebp-0x40]|sub eax, ebp, 0x40|;mov edx, [ebp-8]|sub tmpadr, ebp, 8|load edx, tmpadr, 4|;mov ebx, [loc_936E0]|load ebx, loc_936E0, 4|;call SR_ReadSong|PUSH loc_4BE13_after_1|tcall SR_ReadSong|endp|proc loc_4BE13_after_1|;or eax, eax|;jz loc_4BF23|ctcallz eax, loc_4BF23|tcall loc_4BE13_after_2|endp|proc loc_4BE13_after_2 ; music - load midi or song from catalog

loc_408F8,2,;add edx, edx|add edx, edx, edx|;or eax, eax|;jne loc_408F8|ifz eax|;mov eax, [Zero_Segment]|load eax, Zero_Segment, 4|;loc_408F8:|endif ; fix reading from NULL pointer

loc_40914,2,;or ebx, ebx|;jne loc_40914|ifz ebx|;mov ebx, [Zero_Segment]|load ebx, Zero_Segment, 4|;loc_40914:|endif|;add ebx, eax|add ebx, ebx, eax ; fix reading from NULL pointer

loc_23BEB,2,;xor ch, ch|and ecx, ecx, 0xffff00ff|;or edx, edx|;jne loc_23BEB|ifz edx|;mov edx, [Zero_Segment]|load edx, Zero_Segment, 4|;loc_23BEB:|endif ; fix reading from NULL pointer

loc_2E276,2,;add edx, ecx|add edx, edx, ecx|;movsx eax, word [loc_8D0F6]|load16s eax, loc_8D0F6, 2|;movsx ecx, word [loc_8D0F8]|load16s ecx, loc_8D0F8, 4|;imul eax, ecx|mul eax, eax, ecx|;cmp edx, eax|cmovult edx, eax, tmp1, 0, 1|;jb loc_2E278|ctcallz tmp1, loc_2E278|tcall loc_2E276_after|endp|proc loc_2E276_after|;mov eax, [Zero_Segment]|load eax, Zero_Segment, 4|;mov dword [eax], 0|store 0, eax, 4|;jmp short loc_2E286|tcall loc_2E286|endp|;loc_2E278:|proc loc_2E278 ; fix out of bounds access
loc_2E286,1,;loc_2E286:|;pop esi|POP esi ; fix out of bounds access


loc_4AE5E,2195, ; CLIB code
loc_4CE4E,500, ; CLIB code
loc_4D08C,661, ; CLIB code
loc_4D3B2,3439, ; CLIB code
loc_4E68E,2037, ; CLIB code
loc_4EE89,6, ; CLIB code
loc_4EEA6,8600, ; CLIB code

loc_74FB8,24, ; CLIB data
loc_75020,564, ; CLIB data
loc_75294,336, ; CLIB data

loc_937C4,16, ; CLIB data
loc_937D8,48, ; CLIB data
