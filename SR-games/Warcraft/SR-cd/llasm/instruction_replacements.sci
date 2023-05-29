loc_1AE00,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_1C91B,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_230FB,5,;add eax, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add eax, eax, tmp1 ; get real address of screen window instead of 0x0a0000
loc_23190,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_238BA,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_31E85,10,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4|;mov [loc_5AE20 + 4], eax|mov tmpadr, loc_5AE20|add tmpadr, tmpadr, 4|store eax, tmpadr, 4 ; get real address of screen window instead of 0x0a0000
loc_3D5F2,6,;add edi, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add edi, edi, tmp1 ; get real address of screen window instead of 0x0a0000

loc_1D2E8,5,;mov eax, edx|mov eax, edx ; skip drive check ?

loc_385E1,4,;nop| ; timer interrupt ???
loc_38660,1,;add esp, 8|add esp, esp, 8|;ret|POP tmp1|tcall tmp1|endp ; skip calling original timer interrupt
loc_3867C,1,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending
loc_3867D,7,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_313B9,66,;nop| ; keyboard interrupt ending
loc_31404,4,;ret|POP tmp1|tcall tmp1|endp ; keyboard interrupt ending

loc_1D37B,5,;mov ebx, [Game_stdin]|load ebx, Game_stdin, 4 ; fix stdin

loc_1D9B4,14, ; replace putchar macro (part 1)
loc_1D9C7,96,;mov eax, 0x0a|mov eax, 0x0a|;mov edx, [Game_stdout]|load edx, Game_stdout, 4|;call SR_fputc|;jmp short loc_1DA93|PUSH loc_1DA93|tcall SR_fputc|endp ; replace putchar macro (part 2)
loc_1DA3F,84,;mov edx, [Game_stdout]|load edx, Game_stdout, 4|;call SR_fputc|PUSH loc_1DA93|tcall SR_fputc|endp ; replace putchar macro
loc_1DA9F,98,;mov eax, 0x0a|mov eax, 0x0a|;mov edx, [Game_stdout]|load edx, Game_stdout, 4|;call SR_fputc|PUSH loc_1DB01|tcall SR_fputc|endp ; replace putchar macro

loc_1D735,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr
loc_266A2,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr
loc_26748,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr
loc_2675C,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr

loc_24B35,80,;xor eax, eax|;mov al, [ecx]|load8z eax, ecx, 1|;inc ecx|add ecx, ecx, 1|;mov edx, ebx|mov edx, ebx|;call SR_fputc|PUSH loc_24B35_after|tcall SR_fputc|endp|proc loc_24B35_after ; replace putc macro

loc_14C6D,1,;pop ebp|POP ebp|;call SR_Sync|PUSH loc_14C6D_after|tcall SR_Sync|endp|proc loc_14C6D_after ; call sync after save

loc_2FD4B,2,;mov eax, 70|mov eax, 70|;call SR___delay|PUSH loc_2FD4B_after|tcall SR___delay|endp|proc loc_2FD4B_after|;xor eax, eax|mov eax, 0 ; insert delay into victory screen

loc_415F3,12,;test eax, eax|;jnz short loc_41613|ctcallnz eax, loc_41613|tcall loc_415F3_after|endp|proc loc_415F3_after ; fix reading FILE internals (modified getc macro)
loc_4165E,12,;test eax, eax|;jnz short loc_4167E|ctcallnz eax, loc_4167E|tcall loc_4165E_after|endp|proc loc_4165E_after ; fix reading FILE internals (modified getc macro)
loc_416F0,13,;test eax, eax|;jnz short loc_41711|ctcallnz eax, loc_41711|tcall loc_416F0_after|endp|proc loc_416F0_after ; fix reading FILE internals (modified getc macro)
loc_41740,13,;test eax, eax|;jnz short loc_41761|ctcallnz eax, loc_41761|tcall loc_41740_after|endp|proc loc_41740_after ; fix reading FILE internals (modified getc macro)

loc_2A1AF,4,;xor edi, edi|mov edi, 0|;or ebp, ebp|;je loc_2A1B3|ifnz ebp|;movzx edi, byte [ebp+0x1b]|add tmpadr, ebp, 0x1b|load8z edi, tmpadr, 1|;loc_2A1B3:|endif ; fix reading from NULL pointer

loc_1DBD6,5,;xor eax, eax|mov eax, 0 ; set default? config - no sound, no music
loc_1DBEF,5,;call SR_memset|PUSH loc_1DBEF_0|tcall SR_memset|endp|proc loc_1DBEF_0|;cmp dword [Game_Sound], 0|load tmp1, Game_Sound, 4|;je loc_1DBEF_1|ifnz tmp1|;mov word [loc_59A42], 0|store16 0, loc_59A42, 2|;loc_1DBEF_1:|endif|;cmp dword [Game_Music], 0|load tmp2, Game_Music, 4|;je loc_1DBEF_2|ifnz tmp2|;mov word [loc_59A52], 0|store16 0, loc_59A52, 2|;loc_1DBEF_2:|endif|tcall loc_1DBF4|endp ; enable/disable sound,music driver

loc_27095,15,;mov eax, 1|mov eax, 1 ; fix loading sound driver

loc_26CB5,15,;mov eax, 1|mov eax, 1 ; fix loading music driver

loc_3AA85,4,;mov eax, [esp+0x14]|add tmpadr, esp, 0x14|load eax, tmpadr, 4|;or eax, eax|;je loc_3AAB0|ctcallz eax, loc_3AAB0|tcall loc_3AA85_after|endp|proc loc_3AA85_after ; fix loading NULL samples

loc_20C50,6,;xor eax, eax|mov eax, 0 ; disable network (IPX) game
loc_20C0D,6,;xor eax, eax|mov eax, 0 ; disable modem/direct connection game
loc_16174,5,;nop| ; disable network/modem/direct connection game

loc_318B3,45, ; CLIB code
loc_31CEA,72, ; CLIB code
loc_31E0F,31, ; CLIB code
loc_31E34,44, ; CLIB code
loc_320A2,30, ; CLIB code
loc_320E7,57, ; CLIB code
loc_33877,425, ; CLIB code
loc_33B32,26, ; CLIB code
loc_33B94,1052, ; CLIB code
loc_34045,203, ; CLIB code
loc_3416A,438, ; CLIB code
loc_34617,729, ; CLIB code
loc_349C3,139, ; CLIB code
loc_34A52,14, ; CLIB code
loc_3AD3A,1798,;ret|POP tmp1|tcall tmp1|endp ; CLIB code
loc_3B4D0,1328, ; CLIB code
loc_3BA2D,4227, ; CLIB code
loc_3CD5F,865, ; CLIB code
loc_3D3D7,503, ; CLIB code
loc_3D62E,2802,;nop| ; CLIB code
loc_4018A,2182, ; CLIB code
loc_40A41,2911, ; CLIB code
loc_41885,2322, ; CLIB code

loc_555C4,544, ; CLIB data
loc_565E4,100, ; CLIB data
loc_5666C,272, ; CLIB data
loc_56780,596, ; CLIB data
loc_5861A,102, ; CLIB data

loc_632C4,1100, ; CLIB data
loc_63718,12, ; CLIB data
