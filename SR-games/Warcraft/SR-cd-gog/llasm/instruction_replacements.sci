loc_1AE00,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_1C91B,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_22FCB,5,;add eax, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add eax, eax, tmp1 ; get real address of screen window instead of 0x0a0000
loc_23060,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_2378A,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_31D68,10,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4|;mov [loc_5ACE0 + 4], eax|mov tmpadr, loc_5ACE0|add tmpadr, tmpadr, 4|store eax, tmpadr, 4 ; get real address of screen window instead of 0x0a0000
loc_3D584,6,;add edi, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add edi, edi, tmp1 ; get real address of screen window instead of 0x0a0000

loc_384F1,4,;nop| ; timer interrupt ???
loc_38570,1,;add esp, 8|add esp, esp, 8|;ret|POP tmp1|tcall tmp1|endp ; skip calling original timer interrupt
loc_3858C,1,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending
loc_3858D,7,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_31289,66,;nop| ; keyboard interrupt ending
loc_312D4,4,;ret|POP tmp1|tcall tmp1|endp ; keyboard interrupt ending

loc_1D880,14, ; replace putchar macro (part 1)
loc_1D893,96,;mov eax, 0x0a|mov eax, 0x0a|;mov edx, [Game_stdout]|load edx, Game_stdout, 4|;call SR_fputc|;jmp short loc_1D95F|PUSH loc_1D95F|tcall SR_fputc|endp ; replace putchar macro (part 2)
loc_1D90B,84,;mov edx, [Game_stdout]|load edx, Game_stdout, 4|;call SR_fputc|PUSH loc_1D95F|tcall SR_fputc|endp ; replace putchar macro
loc_1D96B,98,;mov eax, 0x0a|mov eax, 0x0a|;mov edx, [Game_stdout]|load edx, Game_stdout, 4|;call SR_fputc|PUSH loc_1D9CD|tcall SR_fputc|endp ; replace putchar macro

loc_1D601,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr
loc_26572,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr
loc_26618,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr
loc_2662C,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr

loc_24A05,80,;xor eax, eax|;mov al, [ecx]|load8z eax, ecx, 1|;inc ecx|add ecx, ecx, 1|;mov edx, ebx|mov edx, ebx|;call SR_fputc|PUSH loc_24A05_after|tcall SR_fputc|endp|proc loc_24A05_after ; replace putc macro

loc_14C6D,1,;pop ebp|POP ebp|;call SR_Sync|PUSH loc_14C6D_after|tcall SR_Sync|endp|proc loc_14C6D_after ; call sync after save

loc_2FC1B,2,;mov eax, 70|mov eax, 70|;call SR___delay|PUSH loc_2FC1B_after|tcall SR___delay|endp|proc loc_2FC1B_after|;xor eax, eax|mov eax, 0 ; insert delay into victory screen

loc_4158B,12,;test eax, eax|;jnz short loc_415AB|ctcallnz eax, loc_415AB|tcall loc_4158B_after|endp|proc loc_4158B_after ; fix reading FILE internals (modified getc macro)
loc_415E8,13,;test eax, eax|;jnz short loc_41609|ctcallnz eax, loc_41609|tcall loc_415E8_after|endp|proc loc_415E8_after ; fix reading FILE internals (modified getc macro)
loc_41682,12,;test eax, eax|;jnz short loc_416A2|ctcallnz eax, loc_416A2|tcall loc_41682_after|endp|proc loc_41682_after ; fix reading FILE internals (modified getc macro)
loc_416C9,13,;test eax, eax|;jnz short loc_416EA|ctcallnz eax, loc_416EA|tcall loc_416C9_after|endp|proc loc_416C9_after ; fix reading FILE internals (modified getc macro)

loc_2A07F,4,;xor edi, edi|mov edi, 0|;or ebp, ebp|;je loc_2A083|ifnz ebp|;movzx edi, byte [ebp+0x1b]|add tmpadr, ebp, 0x1b|load8z edi, tmpadr, 1|;loc_2A083:|endif ; fix reading from NULL pointer

loc_1DAA2,5,;xor eax, eax|mov eax, 0 ; set default? config - no sound, no music
loc_1DABB,5,;call SR_memset|PUSH loc_1DABB_0|tcall SR_memset|endp|proc loc_1DABB_0|;cmp dword [Game_Sound], 0|load tmp1, Game_Sound, 4|;je loc_1DABB_1|ifnz tmp1|;mov word [loc_59902], 0|store16 0, loc_59902, 2|;loc_1DABB_1:|endif|;cmp dword [Game_Music], 0|load tmp2, Game_Music, 4|;je loc_1DABB_2|ifnz tmp2|;mov word [loc_59912], 0|store16 0, loc_59912, 2|;loc_1DABB_2:|endif|tcall loc_1DAC0|endp ; enable/disable sound,music driver

loc_26F65,15,;mov eax, 1|mov eax, 1 ; fix loading sound driver

loc_26B85,15,;mov eax, 1|mov eax, 1 ; fix loading music driver

loc_3A995,4,;mov eax, [esp+0x14]|add tmpadr, esp, 0x14|load eax, tmpadr, 4|;or eax, eax|;je loc_3A9C0|ctcallz eax, loc_3A9C0|tcall loc_3A995_after|endp|proc loc_3A995_after ; fix loading NULL samples

loc_20B20,6,;xor eax, eax|mov eax, 0 ; disable network (IPX) game
loc_20ADD,6,;xor eax, eax|mov eax, 0 ; disable modem/direct connection game
loc_16174,5,;nop| ; disable network/modem/direct connection game

loc_31783,45, ; CLIB code
loc_31BCA,72, ; CLIB code
loc_31CEF,31, ; CLIB code
loc_31D14,44, ; CLIB code
loc_31F96,42, ; CLIB code
loc_31FEB,69, ; CLIB code
loc_33887,425, ; CLIB code
loc_33B41,26, ; CLIB code
loc_33B85,875, ; CLIB code
loc_33F75,203, ; CLIB code
loc_340A7,441, ; CLIB code
loc_34583,733, ; CLIB code
loc_3492D,139, ; CLIB code
loc_349BC,14, ; CLIB code
loc_3AC5B,1797,;ret|POP tmp1|tcall tmp1|endp ; CLIB code
loc_3B3F4,1420, ; CLIB code
loc_3B9B1,4223, ; CLIB code
loc_3CCE5,859, ; CLIB code
loc_3D369,503, ; CLIB code
loc_3D5BF,2801, ; CLIB code
loc_4011A,2182, ; CLIB code
loc_409D9,2903, ; CLIB code
loc_41819,2322, ; CLIB code

loc_555C4,532, ; CLIB data
loc_5650C,24, ; CLIB data
loc_565E4,100, ; CLIB data
loc_5666C,272, ; CLIB data
loc_56780,596, ; CLIB data
loc_584D6,106, ; CLIB data

loc_63184,1100, ; CLIB data
loc_635D8,12, ; CLIB data
