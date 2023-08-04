loc_12EA5,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_12EB4,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_12F96,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_20FF1,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_3A4EF,5,;add eax, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add eax, eax, tmp1 ; use real address of screen window instead of 0x0a0000
loc_49378,6,;mov ebx, [Game_ScreenWindow]|load ebx, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000

loc_4E787,27,;mov eax, ecx|mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12548,22,;inc dword [loc_71B90]|load tmp1, loc_71B90, 4|add tmp1, tmp1, 1|store tmp1, loc_71B90, 4 ; timer interrupt beginning
loc_12568,31,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_4F0D2,35, ; read sound.cfg
loc_4F0FD,21,;call SR_FillSoundCfg|PUSH loc_4F0FD_after|tcall SR_FillSoundCfg|endp|proc loc_4F0FD_after|;add esp, 2*4|add esp, esp, 2*4 ; read sound.cfg

loc_102D3,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D08], 0|load tmp1, loc_70D08, 4|;jnz loc_102D3|ctcallnz tmp1, loc_102D3|tcall loc_102DC|endp ; update timer - wait for ??? in timer interrupt
loc_1033C,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D08], 0|load tmp1, loc_70D08, 4|;jnz loc_1033C|ctcallnz tmp1, loc_1033C|tcall loc_10345|endp ; update timer
loc_3A374,2,;call SR_RunTimerDelay|call SR_RunTimerDelay|;xor esi, esi|mov esi, 0 ; update timer

loc_2A04A,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jnz loc_2A04A|ctcallnz tmp1, loc_2A04A|tcall loc_2A053|endp ; update timer - wait for mouse click in timer interrupt
loc_2DC4C,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jnz loc_2DC4C|ctcallnz tmp1, loc_2DC4C|tcall loc_2DC55|endp ; update timer
loc_2DD19,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jnz loc_2DD19|ctcallnz tmp1, loc_2DD19|tcall loc_2DD22|endp ; update timer
loc_31C62,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jz loc_31C62|ctcallz tmp1, loc_31C62|tcall loc_31C6B|endp ; update timer
loc_33AA5,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jz loc_33AA5|ctcallz tmp1, loc_33AA5|tcall loc_33AAE|endp ; update timer
loc_39CE2,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jz loc_39CCE|ctcallz tmp1, loc_39CCE|tcall loc_39CEB|endp ; update timer
loc_3A0F9,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jz loc_3A0F9|ctcallz tmp1, loc_3A0F9|tcall loc_3A102|endp ; update timer
loc_3A41E,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jnz loc_3A41E|ctcallnz tmp1, loc_3A41E|tcall loc_3A427|endp ; update timer
loc_3A909,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jnz loc_3A909|ctcallnz tmp1, loc_3A909|tcall loc_3A912|endp ; update timer
loc_3ADD0,8,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp ebx, [loc_70CDC]|load tmp1, loc_70CDC, 4|cmoveq ebx, tmp1, tmp2, 0, 1|;jz loc_3ADC4|ctcallz tmp2, loc_3ADC4|tcall loc_3ADD8|endp ; update timer
loc_3ADC4,4,;loc_3ADC4:|;test edx, edx|;jz loc_3ADD8|ctcallz edx, loc_3ADD8|tcall loc_3ADC4_after|endp|proc loc_3ADC4_after ; define label
loc_3B3FF,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jz loc_3B3FA|ctcallz tmp1, loc_3B3FA|tcall loc_3B408|endp ; update timer
loc_3B570,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jz loc_3B56B|ctcallz tmp1, loc_3B56B|tcall loc_3B579|endp ; update timer
loc_3B89D,8,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp eax, [loc_70CDC]|load tmp1, loc_70CDC, 4|cmoveq eax, tmp1, tmp2, 0, 1|;jz loc_3B898|ctcallz tmp2, loc_3B898|tcall loc_3B8A5|endp ; update timer
loc_3B898,4,;loc_3B898:|;test esi, esi|;jz loc_3B8A5|ctcallz esi, loc_3B8A5|tcall loc_3B898_after|endp|proc loc_3B898_after ; define label
loc_3B995,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jz loc_3B986|ctcallz tmp1, loc_3B986|tcall loc_3B99E|endp ; update timer
loc_3BC84,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70CDC], 0|load tmp1, loc_70CDC, 4|;jz loc_3BC84|ctcallz tmp1, loc_3BC84|tcall loc_3BC84_after|endp|proc loc_3BC84_after ; update timer

loc_113CC,10,;mov eax, ecx|mov eax, ecx|;call SR_feof|PUSH loc_113CC_after_1|tcall SR_feof|endp|proc loc_113CC_after_1|;test eax, eax|;jnz loc_114D6|ctcallnz eax, loc_114D6|tcall loc_113CC_after_2|endp|proc loc_113CC_after_2 ; fix reading FILE internals (eof flag)
loc_114D6,2,;loc_114D6:|proc loc_114D6|;mov eax, ecx|mov eax, ecx ; define label

loc_51495,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr
loc_514A4,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr
loc_514B5,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr
loc_514C9,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr

loc_1EC4E,8,;mov eax, esi|mov eax, esi|;call SR_filelength2|PUSH loc_1EC4E_after|tcall SR_filelength2|endp|proc loc_1EC4E_after ; get file length
loc_1EE51,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_1EE51_after|tcall SR_filelength2|endp|proc loc_1EE51_after ; get file length
loc_1F6B9,8,;mov eax, ecx|mov eax, ecx|;call SR_filelength2|PUSH loc_1F6B9_after|tcall SR_filelength2|endp|proc loc_1F6B9_after ; get file length
loc_2F5E7,8,;call SR_filelength2|PUSH loc_2F5E7_after|tcall SR_filelength2|endp|proc loc_2F5E7_after ; get file length
loc_35D15,10,;xor edx, edx|mov edx, 0|;call SR_filelength2|PUSH loc_35D15_after|tcall SR_filelength2|endp|proc loc_35D15_after ; get file length
loc_35F96,13,;mov esi,0x18|mov esi, 0x18|;call SR_filelength2|PUSH loc_35F96_after|tcall SR_filelength2|endp|proc loc_35F96_after ; get file length
loc_361BE,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_361BE_after|tcall SR_filelength2|endp|proc loc_361BE_after ; get file length

loc_2D5F4,1,;pop edi|POP edi|;call SR_Sync|PUSH loc_2D5F4_after|tcall SR_Sync|endp|proc loc_2D5F4_after ; call sync after save
loc_2DEFC,1,;pop ebp|POP ebp|;call SR_Sync|PUSH loc_2DEFC_after|tcall SR_Sync|endp|proc loc_2DEFC_after ; call sync after save

loc_49460,4,;call SR_SlowDownMainLoop|call SR_SlowDownMainLoop|;test eax, eax|;jz loc_4948D|ctcallz eax, loc_4948D|tcall loc_49460_after|endp|proc loc_49460_after ; slow down game main loop

loc_43E95,3,;sub esp, 0x3c|sub esp, esp, 0x3c|;call SR_SlowDownScrolling|call SR_SlowDownScrolling ; slow down game scrolling, ...

loc_47876,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov edx, ecx|mov edx, ecx ; skip game scrolling slow down once
loc_47A0F,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov edx, esi|mov edx, esi ; skip game scrolling slow down once
loc_47BA5,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov edx, ecx|mov edx, ecx ; skip game scrolling slow down once
loc_47D11,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov eax, esi|mov eax, esi ; skip game scrolling slow down once

loc_4F829,245,;mov eax, loc_A344C|mov eax, loc_A344C|;mov edx, [ebp-12]|sub tmpadr, ebp, 12|load edx, tmpadr, 4|;mov ebx, [loc_A3434]|load ebx, loc_A3434, 4|;call SR_ReadSong|PUSH loc_4F829_after_1|tcall SR_ReadSong|endp|proc loc_4F829_after_1|;or eax, eax|;jz loc_4F952|ctcallz eax, loc_4F952|tcall loc_4F829_after_2|endp|proc loc_4F829_after_2 ; music - load midi or song from catalog

loc_42C02,6,;mov ebp, [loc_93B1C]|load ebp, loc_93B1C, 4|;or ebp, ebp|;jne loc_42C02|ifz ebp|;mov ebp, [Zero_Segment]|load ebp, Zero_Segment, 4|;loc_42C02:|endif ; fix reading from NULL pointer

loc_44A84,2,;mov al, bl|and eax, eax, 0xffffff00|and tmp1, ebx, 0xff|or eax, eax, tmp1|;or edx, edx|;jne loc_44A84|ifz edx|;mov edx, [Zero_Segment]|load edx, Zero_Segment, 4||;loc_44A84:|endif ; fix reading from NULL pointer (potential)

loc_44A99,2,;or ebx, ebx|;jne loc_44A99|ifz ebx|;mov ebx, [Zero_Segment]|load ebx, Zero_Segment, 4|;loc_44A99:|endif|;add ebx, ecx|add ebx, ebx, ecx ; fix reading from NULL pointer (potential)

loc_240BB,2,;xor ch, ch|and ecx, ecx, 0xffff00ff|;or edx, edx|;jne loc_240BB|ifz edx|;mov edx, [Zero_Segment]|load edx, Zero_Segment, 4|;loc_240BB:|endif ; fix reading from NULL pointer (potential)

loc_2FED3,2,;add edx, esi|add edx, edx, esi|;movsx eax, word [loc_95656]|load16s eax, loc_95656, 2|;movsx ecx, word [loc_95658]|load16s ecx, loc_95658, 4|;imul eax, ecx|mul eax, eax, ecx|;cmp edx, eax|cmovult edx, eax, tmp1, 0, 1|;jb loc_2FED5|ctcallz tmp1, loc_2FED5|tcall loc_2FED3_after|endp|proc loc_2FED3_after|;mov eax, [Zero_Segment]|load eax, Zero_Segment, 4|;mov dword [eax], 0|store 0, eax, 4|;jmp short loc_2FEE3|tcall loc_2FEE3|endp|;loc_2FED5:|proc loc_2FED5 ; fix out of bounds access
loc_2FEE3,1,;loc_2FEE3:|;pop esi|POP esi ; fix out of bounds access


loc_50B9E,2285, ; CLIB code
loc_514D5,661, ; CLIB code
loc_517FB,102, ; CLIB code
loc_5187C,3822, ; CLIB code
loc_52770,6, ; CLIB code
loc_5278D,5333, ; CLIB code
loc_53C8A,6117, ; CLIB code

loc_7587C,664, ; CLIB data
loc_75B54,336, ; CLIB data

loc_A34E8,12, ; CLIB data
loc_A34F8,52, ; CLIB data
