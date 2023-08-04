loc_12E95,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_12EA4,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_12F86,5,;mov edi, [Game_ScreenWindow]|load edi, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_20C17,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_396DF,5,;add eax, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add eax, eax, tmp1 ; use real address of screen window instead of 0x0a0000
loc_48128,6,;mov ebx, [Game_ScreenWindow]|load ebx, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000

loc_4D383,27,;mov eax, ecx|mov eax, ecx ; Allocate DOS Memory Block: instead of false real mode address use actual allocated address

loc_12538,22,;inc dword [loc_71BF4]|load tmp1, loc_71BF4, 4|add tmp1, tmp1, 1|store tmp1, loc_71BF4, 4 ; timer interrupt beginning
loc_12558,31,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_4DCBD,35, ; read sound.cfg
loc_4DCE8,21,;call SR_FillSoundCfg|PUSH loc_4DCE8_after|tcall SR_FillSoundCfg|endp|proc loc_4DCE8_after|;add esp, 2*4|add esp, esp, 2*4 ; read sound.cfg

loc_102D3,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D6C], 0|load tmp1, loc_70D6C, 4|;jnz loc_102D3|ctcallnz tmp1, loc_102D3|tcall loc_102DC|endp ; update timer - wait for ??? in timer interrupt
loc_1033C,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D6C], 0|load tmp1, loc_70D6C, 4|;jnz loc_1033C|ctcallnz tmp1, loc_1033C|tcall loc_10345|endp ; update timer
loc_39564,2,;call SR_RunTimerDelay|call SR_RunTimerDelay|;xor esi, esi|mov esi, 0 ; update timer

loc_29B4A,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jnz loc_29B4A|ctcallnz tmp1, loc_29B4A|tcall loc_29B53|endp ; update timer - wait for mouse click in timer interrupt
loc_2D52C,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jnz loc_2D52C|ctcallnz tmp1, loc_2D52C|tcall loc_2D535|endp ; update timer
loc_2D5F9,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jnz loc_2D5F9|ctcallnz tmp1, loc_2D5F9|tcall loc_2D602|endp ; update timer
loc_31104,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jz loc_31104|ctcallz tmp1, loc_31104|tcall loc_3110D|endp ; update timer
loc_32F5F,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jz loc_32F5F|ctcallz tmp1, loc_32F5F|tcall loc_32F68|endp ; update timer
loc_38EF8,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jz loc_38EE4|ctcallz tmp1, loc_38EE4|tcall loc_38F01|endp ; update timer
loc_39307,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jz loc_39307|ctcallz tmp1, loc_39307|tcall loc_39310|endp ; update timer
loc_3960E,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jnz loc_3960E|ctcallnz tmp1, loc_3960E|tcall loc_39617|endp ; update timer
loc_39AE9,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jnz loc_39AE9|ctcallnz tmp1, loc_39AE9|tcall loc_39AF2|endp ; update timer
loc_39FA7,8,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp ecx, [loc_70D40]|load tmp1, loc_70D40, 4|cmoveq ecx, tmp1, tmp2, 0, 1|;jz loc_39F9B|ctcallz tmp2, loc_39F9B|tcall loc_39FAF|endp ; update timer
loc_39F9B,4,;loc_39F9B:|;test edx, edx|;jz loc_39FAF|ctcallz edx, loc_39FAF|tcall loc_39F9B_after|endp|proc loc_39F9B_after ; define label
loc_3A5CF,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jz loc_3A5CA|ctcallz tmp1, loc_3A5CA|tcall loc_3A5D8|endp ; update timer
loc_3A740,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jz loc_3A73B|ctcallz tmp1, loc_3A73B|tcall loc_3A749|endp ; update timer
loc_3AA69,8,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp eax, [loc_70D40]|load tmp1, loc_70D40, 4|cmoveq eax, tmp1, tmp2, 0, 1|;jz loc_3AA64|ctcallz tmp2, loc_3AA64|tcall loc_3AA71|endp ; update timer
loc_3AA64,4,;loc_3AA64:|;test esi, esi|;jz loc_3AA71|ctcallz esi, loc_3AA71|tcall loc_3AA64_after|endp|proc loc_3AA64_after ; define label
loc_3AB50,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jz loc_3AB41|ctcallz tmp1, loc_3AB41|tcall loc_3AB59|endp ; update timer
loc_3ADE4,9,;call SR_RunTimerDelay|call SR_RunTimerDelay|;cmp dword [loc_70D40], 0|load tmp1, loc_70D40, 4|;jz loc_3ADE4|ctcallz tmp1, loc_3ADE4|tcall loc_3ADE4_after|endp|proc loc_3ADE4_after ; update timer

loc_113BC,10,;mov eax, ecx|mov eax, ecx|;call SR_feof|PUSH loc_113BC_after_1|tcall SR_feof|endp|proc loc_113BC_after_1|;test eax, eax|;jnz loc_114C6|ctcallnz eax, loc_114C6|tcall loc_113BC_after_2|endp|proc loc_113BC_after_2 ; fix reading FILE internals (eof flag)
loc_114C6,2,;loc_114C6:|proc loc_114C6|;mov eax, ecx|mov eax, ecx ; define label

loc_50162,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr
loc_50171,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr
loc_50182,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr
loc_50196,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; get stderr

loc_1E957,8,;mov eax, esi|mov eax, esi|;call SR_filelength2|PUSH loc_1E957_after|tcall SR_filelength2|endp|proc loc_1E957_after ; get file length
loc_1EB31,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_1EB31_after|tcall SR_filelength2|endp|proc loc_1EB31_after ; get file length
loc_1F392,8,;mov eax, edx|mov eax, edx|;call SR_filelength2|PUSH loc_1F392_after|tcall SR_filelength2|endp|proc loc_1F392_after ; get file length
loc_35131,8,;call SR_filelength2|PUSH loc_35131_after|tcall SR_filelength2|endp|proc loc_35131_after ; get file length
loc_353B5,13,;mov esi,0x18|mov esi, 0x18|;call SR_filelength2|PUSH loc_353B5_after|tcall SR_filelength2|endp|proc loc_353B5_after ; get file length
loc_355DE,13,;mov edx,1|mov edx, 1|;call SR_filelength2|PUSH loc_355DE_after|tcall SR_filelength2|endp|proc loc_355DE_after ; get file length

loc_2CEF4,1,;pop edi|POP edi|;call SR_Sync|PUSH loc_2CEF4_after|tcall SR_Sync|endp|proc loc_2CEF4_after ; call sync after save
loc_2D7DE,1,;pop ebp|POP edi|;call SR_Sync|PUSH loc_2D7DE_after|tcall SR_Sync|endp|proc loc_2D7DE_after ; call sync after save

loc_48210,4,;call SR_SlowDownMainLoop|call SR_SlowDownMainLoop|;test eax, eax|;jz loc_48233|ctcallz eax, loc_48233|tcall loc_48210_after|endp|proc loc_48210_after ; slow down game main loop

loc_42E55,3,;sub esp, 0x3c|sub esp, esp, 0x3c|;call SR_SlowDownScrolling|call SR_SlowDownScrolling ; slow down game scrolling, ...

loc_4668C,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov edx, ecx|mov edx, ecx ; skip game scrolling slow down once
loc_4681A,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov edx, ebx|mov edx, ebx ; skip game scrolling slow down once
loc_469B0,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov eax, ecx|mov eax, ecx ; skip game scrolling slow down once
loc_46B10,2,;mov dword [Game_Skip_Scrolling_SlowDown], 1|store 1, Game_Skip_Scrolling_SlowDown, 4|;mov edx, esi|mov edx, esi ; skip game scrolling slow down once

loc_4E40A,245,;mov eax, loc_A499C|mov eax, loc_A499C|;mov edx, [ebp-12]|sub tmpadr, ebp, 12|load edx, tmpadr, 4|;mov ebx, [loc_A4984]|load ebx, loc_A4984, 4|;call SR_ReadSong|PUSH loc_4E40A_after_1|tcall SR_ReadSong|endp|proc loc_4E40A_after_1|;or eax, eax|;jz loc_4E533|ctcallz eax, loc_4E533|tcall loc_4E40A_after_2|endp|proc loc_4E40A_after_2 ; music - load midi or song from catalog

loc_41BDD,6,;mov ebp, [loc_95370]|load ebp, loc_95370, 4|;or ebp, ebp|;jne loc_41BDD|ifz ebp|;mov ebp, [Zero_Segment]|load ebp, Zero_Segment, 4|;loc_41BDD:|endif ; fix reading from NULL pointer

loc_439D8,2,;mov al, bl|and eax, eax, 0xffffff00|and tmp1, ebx, 0xff|or eax, eax, tmp1|;or edx, edx|;jne loc_439D8|ifz edx|;mov edx, [Zero_Segment]|load edx, Zero_Segment, 4|;loc_439D8:|endif ; fix reading from NULL pointer (potential)

loc_439EB,2,;or ebx, ebx|;jne loc_439EB|ifz ebx|;mov ebx, [Zero_Segment]|load ebx, Zero_Segment, 4|;loc_439EB:|endif|;add ebx, ecx|add ebx, ebx, ecx ; fix reading from NULL pointer (potential)

loc_23C8B,2,;xor ch, ch|and ecx, ecx, 0xffff00ff|;or edx, edx|;jne loc_23C8B|ifz edx|;mov edx, [Zero_Segment]|load edx, Zero_Segment, 4|;loc_23C8B:|endif ; fix reading from NULL pointer (potential)

loc_2F583,2,;add edx, esi|add edx, edx, esi|;movsx eax, word [loc_9543A]|load16s eax, loc_9543A, 2|;movsx ecx, word [loc_9543C]|load16s ecx, loc_9543C, 4|;imul eax, ecx|mul eax, eax, ecx|;cmp edx, eax|cmovult edx, eax, tmp1, 0, 1|;jb loc_2F585|ctcallz tmp1, loc_2F585|tcall loc_2F583_after|endp|proc loc_2F583_after|;mov eax, [Zero_Segment]|load eax, Zero_Segment, 4|;mov dword [eax], 0|store 0, eax, 4|;jmp short loc_2F593|tcall loc_2F593|endp|;loc_2F585:|proc loc_2F585 ; fix out of bounds access
loc_2F593,1,;loc_2F593:|;pop esi|POP esi ; fix out of bounds access


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
