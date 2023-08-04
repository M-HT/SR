loc_3E618,6,;mov ebx, [Game_ScreenWindow]|load ebx, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_13024,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_3A3D9,5,;add eax, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add eax, eax, tmp1 ; use real address of screen window instead of 0x0a0000

loc_47693,27,;mov eax, ecx|mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12638,22,;inc dword [loc_61C94]|load tmp1, loc_61C94, 4|add tmp1, tmp1, 1|store tmp1, loc_61C94, 4 ; timer interrupt beginning
loc_12658,31,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_48150,35, ; read sound.cfg
loc_4817B,21,;call SR_FillSoundCfg|PUSH loc_4817B_after|tcall SR_FillSoundCfg|endp|proc loc_4817B_after|;add esp, 2*4|add esp, esp, 2*4 ; read sound.cfg

loc_153BB,5,;mov ebx, 0x40|mov ebx, 0x40 ; fix for the "Difficulty bug"

loc_102D3,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60E0C], 0|load tmp1, loc_60E0C, 4|;jnz loc_102D3|ctcallnz tmp1, loc_102D3|tcall loc_102DC|endp ; update timer - wait for ??? in timer interrupt
loc_1033C,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60E0C], 0|load tmp1, loc_60E0C, 4|;jnz loc_1033C|ctcallnz tmp1, loc_1033C|tcall loc_10345|endp ; update timer
loc_3A255,2,;call SR_RunTimerDelay|call SR_RunTimerDelay|;xor ebx, ebx|mov ebx, 0 ; update timer

loc_1A9EE,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jz loc_1A9FE|ctcallz tmp1, loc_1A9FE|tcall loc_1A9EE_after|endp|proc loc_1A9EE_after ; update timer - wait for mouse click in timer interrupt
loc_1C630,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jz loc_1C640|ctcallz tmp1, loc_1C640|tcall loc_1C630_after|endp|proc loc_1C630_after ; update timer
loc_2319E,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jnz loc_231AF|ctcallnz tmp1, loc_231AF|tcall loc_2319E_after|endp|proc loc_2319E_after ; update timer
loc_24C2C,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jz loc_24C2C|ctcallz tmp1, loc_24C2C|tcall loc_24C35|endp ; update timer
loc_25619,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jnz loc_25619|ctcallnz tmp1, loc_25619|tcall loc_25622|endp ; update timer
loc_25C97,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jz loc_25CAE|ctcallz tmp1, loc_25CAE|tcall loc_25C97_after|endp|proc loc_25C97_after ; update timer
loc_25D98,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jz loc_25DAF|ctcallz tmp1, loc_25DAF|tcall loc_25D98_after|endp|proc loc_25D98_after ; update timer
loc_25E30,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jz loc_25E47|ctcallz tmp1, loc_25E47|tcall loc_25E30_after|endp|proc loc_25E30_after ; update timer
loc_25EDB,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jz loc_25EF4|ctcallz tmp1, loc_25EF4|tcall loc_25EDB_after|endp|proc loc_25EDB_after ; update timer
loc_28209,8,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp esi, [loc_60DE0]|load tmp1, loc_60DE0, 4|cmoveq esi, tmp1, tmp2, 0, 1|;jnz loc_28209|ctcallnz tmp2, loc_28209|tcall loc_28211|endp ; update timer
loc_3A61D,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_60DE0], 0|load tmp1, loc_60DE0, 4|;jnz loc_3A61D|ctcallnz tmp1, loc_3A61D|tcall loc_3A626|endp ; update timer

loc_10587,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_6100C], 0|load tmp1, loc_6100C, 4|;jnz loc_10599|ctcallnz tmp1, loc_10599|tcall loc_10587_after|endp|proc loc_10587_after ; update timer - wait for ??? in timer interrupt

loc_105CC,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_61010], 0|load tmp1, loc_61010, 4|;jnz loc_105DE|ctcallnz tmp1, loc_105DE|tcall loc_105CC_after|endp|proc loc_105CC_after ; update timer - wait for ??? in timer interrupt

loc_114BE,10,;mov eax, ecx|mov eax, ecx|;call SR_feof|PUSH loc_114BE_after_1|tcall SR_feof|endp|proc loc_114BE_after_1|;test eax, eax|;jnz loc_115C3|ctcallnz eax, loc_115C3|tcall loc_114BE_after_2|endp|proc loc_114BE_after_2 ; fix reading FILE internals (eof flag)
loc_115C3,2,;loc_115C3:|proc loc_115C3|;mov eax, ecx|mov eax, ecx ; define label
loc_116AC,10,;mov eax, ecx|mov eax, ecx|;call SR_feof|PUSH loc_116AC_after_1|tcall SR_feof|endp|proc loc_116AC_after_1|;test eax, eax|;jnz loc_11789|ctcallnz eax, loc_11789|tcall loc_116AC_after_2|endp|proc loc_116AC_after_2 ; fix reading FILE internals (eof flag)
loc_11789,2,;loc_11789:|proc loc_11789|;mov eax, ecx|mov eax, ecx ; define label

loc_3ED69,5,;mov eax, [Game_stdout]|load eax, Game_stdout, 4 ; get stdout

loc_22D1C,8,;call SR_filelength2|PUSH loc_22D1C_after|tcall SR_filelength2|endp|proc loc_22D1C_after ; get file length
loc_22E78,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_22E78_after|tcall SR_filelength2|endp|proc loc_22E78_after ; get file length

loc_14155,1,;pop edi|POP edi|;call SR_Sync|PUSH loc_14155_after|tcall SR_Sync|endp|proc loc_14155_after ; call sync after save
loc_1439F,1,;pop ebp|POP ebp|;call SR_Sync|PUSH loc_1439F_after|tcall SR_Sync|endp|proc loc_1439F_after ; call sync after save
loc_147A5,1,;pop edx|POP edx|;call SR_Sync|PUSH loc_147A5_after|tcall SR_Sync|endp|proc loc_147A5_after ; call sync after save
loc_36B87,1,;pop edx|POP edx|;call SR_Sync|PUSH loc_36B87_after|tcall SR_Sync|endp|proc loc_36B87_after ; call sync after save

loc_29A9D,8,;call SR_SlowDownMainLoop|call SR_SlowDownMainLoop|;test eax, eax|;jz loc_29B19|ctcallz eax, loc_29B19|tcall loc_29A9D_after|endp|proc loc_29A9D_after; slow down game main loop

loc_1EB28,2,;xor edx, edx|mov edx, 0|;mov [esp+0x0c], edx| add tmpadr, esp, 0x0c|store edx, tmpadr, 4 ; fix reading uninitialized variable

loc_2B101,2,;sub eax, edx|sub eax, eax, edx|;mov esi, 0xffffffff|mov esi, 0xffffffff ; fix reading not reinitialized variable

loc_48669,220,;lea eax, [ebp-0x40]|sub eax, ebp, 0x40|;mov edx, [ebp-8]|sub tmpadr, ebp, 8|load edx, tmpadr, 4|;mov ebx, [loc_6C918]|load ebx, loc_6C918, 4|;call SR_ReadSong|PUSH loc_48669_after_1|tcall SR_ReadSong|endp|proc loc_48669_after_1|;or eax, eax|;jz loc_48779|ctcallz eax, loc_48779|tcall loc_48669_after_2|endp|proc loc_48669_after_2 ; music - load midi or song from catalog

loc_44F20,1,;push edx|PUSH edx|;movsx eax, word [loc_6C502]|load16s eax, loc_6C502, 2|;cmp eax, 90|cmovslt eax, 90, tmp1, 1, 0|;jge loc_44F20_after|ifnz tmp1|;mov word [loc_6C502], 90|store16 90, loc_6C502, 2|;loc_44F20_after:|endif ; fix reading uninitialized/not reinitialized variable

loc_44F09,5,;call loc_10180|PUSH loc_44F09_after_1|tcall loc_10180|endp|proc loc_44F09_after_1|;mov eax, 5|mov eax, 5|;call SR_WaitVerticalRetraceTicks2|PUSH loc_44F09_after_2|tcall SR_WaitVerticalRetraceTicks2|endp|proc loc_44F09_after_2 ; slow down animation
loc_44F51,3,;sub edx, 20|sub edx, edx, 20|;mov eax, 5|mov eax, 5|;call SR_WaitVerticalRetraceTicks2|PUSH loc_44F51_after|tcall SR_WaitVerticalRetraceTicks2|endp|proc loc_44F51_after ; slow down animation


loc_476BA,2195, ; CLIB code
loc_49538,264, ; CLIB code
loc_49697,45, ; CLIB code
loc_496FE,2106, ; CLIB code
loc_49F55,124, ; CLIB code
loc_49FD2,2507, ; CLIB code
loc_4AF0A,10969,proc loc_4D9E3 ; CLIB code
loc_50C0A,4828, ; CLIB code

loc_60A9C,368, ; CLIB data
loc_60D10,208, ; CLIB data
loc_690BC,636, ; CLIB data
loc_69340,380, ; CLIB data
loc_69538,192, ; CLIB data

loc_6C9FC,16, ; CLIB data
loc_6CA10,176, ; CLIB data
