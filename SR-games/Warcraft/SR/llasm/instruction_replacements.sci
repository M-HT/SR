loc_1EC32,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_20FF7,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_29F64,5,;add eax, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add eax, eax, tmp1 ; get real address of screen window instead of 0x0a0000
loc_2A088,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_2AAD4,5,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4 ; get real address of screen window instead of 0x0a0000
loc_3D8F8,10,;mov eax, [Game_ScreenWindow]|load eax, Game_ScreenWindow, 4|;mov [loc_5AB08 + 4], eax|mov tmpadr, loc_5AB08|add tmpadr, tmpadr, 4|store eax, tmpadr, 4 ; get real address of screen window instead of 0x0a0000
loc_49174,6,;add edi, [Game_ScreenWindow]|load tmp1, Game_ScreenWindow, 4|add edi, edi, tmp1 ; get real address of screen window instead of 0x0a0000

loc_21E4E,5,;mov eax, edx|mov eax, edx ; skip drive check ?

loc_440A1,4,;nop| ; timer interrupt ???
loc_44120,1,;add esp, 8|add esp, esp, 8|;ret|POP tmp1|tcall tmp1|endp ; skip calling original timer interrupt
loc_4413C,1,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending
loc_4413D,7,;ret|POP tmp1|tcall tmp1|endp ; timer interrupt ending

loc_3CE19,66,;nop| ; keyboard interrupt ending
loc_3CE64,4,;ret|POP tmp1|tcall tmp1|endp ; keyboard interrupt ending

loc_21F11,5,;mov ebx, [Game_stdin]|load ebx, Game_stdin, 4 ; fix stdin

loc_2276F,123,;mov eax, 0x0a|mov eax, 0x0a|;mov edx, [Game_stdout]|load edx, Game_stdout, 4|;call SR_fputc|PUSH loc_2276F_after|tcall SR_fputc|endp|proc loc_2276F_after|;mov [ebp-0x10], eax|sub tmpadr, ebp, 0x10|store eax, tmpadr, 4 ; replace putchar macro
loc_2280E,123,;mov eax, [ebp-0x0c]|sub tmpadr, ebp, 0x0c|load eax, tmpadr, 4|;mov edx, [Game_stdout]|load edx, Game_stdout, 4|;call SR_fputc|PUSH loc_2280E_after|tcall SR_fputc|endp|proc loc_2280E_after|;mov [ebp-0x18], eax|sub tmpadr, ebp, 0x18|store eax, tmpadr, 4|tcall loc_22889|endp ; replace putchar macro
loc_2288E,123,;mov eax, 0x0a|mov eax, 0x0a|;mov edx, [Game_stdout]|load edx, Game_stdout, 4|;call SR_fputc|PUSH loc_2288E_after|tcall SR_fputc|endp|proc loc_2288E_after|;mov [ebp-0x20], eax|sub tmpadr, ebp, 0x20|store eax, tmpadr, 4|tcall loc_22909|endp ; replace putchar macro

loc_223F9,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr
loc_2E82E,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr
loc_2E8E9,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr
loc_2E8FF,5,;mov edx, [Game_stderr]|load edx, Game_stderr, 4 ; fix stderr

loc_2C193,133,;mov eax, [ebp-0x10]|sub tmpadr, ebp, 0x10|load eax, tmpadr, 4|;inc dword [ebp-0x10]|add tmp1, eax, 1|store tmp1, tmpadr, 4|;mov al, [eax]|;and eax, 0xff|load8z eax, eax, 1|;mov edx, [ebp-0x0c]|sub tmpadr, ebp, 0x0c|load edx, tmpadr, 4|;call SR_fputc|PUSH loc_2C193_after|tcall SR_fputc|endp|proc loc_2C193_after|;mov [ebp-0x18], eax|sub tmpadr, ebp, 0x18|store eax, tmpadr, 4 ; replace putc macro

loc_16D63,1,;pop ebp|POP ebp|;call SR_Sync|PUSH loc_16D63_after|tcall SR_Sync|endp|proc loc_16D63_after ; call sync after save

loc_3B748,2,;mov eax, 70|mov eax, 70|;call SR___delay|PUSH loc_3B748_after|tcall SR___delay|endp|proc loc_3B748_after|;xor eax, eax|mov eax, 0 ; insert delay into victory screen

loc_4D17B,12,;test eax, eax|;jnz short loc_4D19B|ctcallnz eax, loc_4D19B|tcall loc_4D17B_after|endp|proc loc_4D17B_after ; fix reading FILE internals (modified getc macro)
loc_4D1D8,13,;test eax, eax|;jnz short loc_4D1F9|ctcallnz eax, loc_4D1F9|tcall loc_4D1D8_after|endp|proc loc_4D1D8_after ; fix reading FILE internals (modified getc macro)
loc_4D272,12,;test eax, eax|;jnz short loc_4D292|ctcallnz eax, loc_4D292|tcall loc_4D272_after|endp|proc loc_4D272_after ; fix reading FILE internals (modified getc macro)
loc_4D2B9,13,;test eax, eax|;jnz short loc_4D2DA|ctcallnz eax, loc_4D2DA|tcall loc_4D2B9_after|endp|proc loc_4D2B9_after ; fix reading FILE internals (modified getc macro)

loc_33C45,3,;or eax, eax|;je loc_33C48|ifnz eax|;mov al, [eax+0x1b]|add tmpadr, eax, 0x1b|load8z eax, tmpadr, 1|;loc_33C48:|endif ; fix reading from NULL pointer
loc_33C55,3,;or eax, eax|;je loc_33C58|ifnz eax|;mov al, [eax+0x1b]|add tmpadr, eax, 0x1b|load8z eax, tmpadr, 1|;loc_33C58:|endif ; fix reading from NULL pointer

loc_22A15,5,;xor eax, eax|mov eax, 0 ; set default? config - no sound, no music
loc_22A2E,5,;call SR_memset|PUSH loc_22A2E_0|tcall SR_memset|endp|proc loc_22A2E_0|;cmp dword [Game_Sound], 0|load tmp1, Game_Sound, 4|;je loc_22A2E_1|ifnz tmp1|;mov word [loc_597DE], 0|store16 0, loc_597DE, 2|;loc_22A2E_1:|endif|;cmp dword [Game_Music], 0|load tmp2, Game_Music, 4|;je loc_22A2E_2|ifnz tmp2|;mov word [loc_597EE], 0|store16 0, loc_597EE, 2|;loc_22A2E_2:|endif|tcall loc_22A33|endp ; enable/disable sound,music driver

loc_2F395,15,;mov eax, 1|mov eax, 1 ; fix loading sound driver

loc_2EFB5,15,;mov eax, 1|mov eax, 1 ; fix loading music driver

loc_46545,4,;mov eax, [esp+0x14]|add tmpadr, esp, 0x14|load eax, tmpadr, 4|;or eax, eax|;je loc_46570|ctcallz eax, loc_46570|tcall loc_46545_after|endp|proc loc_46545_after ; fix loading NULL samples

loc_4BCA9,5,;push dword loc_565A4|mov tmpadr, loc_56584|add tmpadr, tmpadr, 32|PUSH tmpadr

loc_26BF5,6,;xor eax, eax|mov eax, 0 ; disable network (IPX) game
loc_26BAE,6,;xor eax, eax|mov eax, 0 ; disable modem/direct connection game
loc_18898,5,;nop| ; disable network/modem/direct connection game

loc_3D87F,31, ; CLIB code
loc_3D8A4,44, ; CLIB code
loc_3DB26,42, ; CLIB code
loc_3DB7B,69, ; CLIB code
loc_3F417,425, ; CLIB code
loc_3F733,1053, ; CLIB code
loc_3FBD5,203, ; CLIB code
loc_3FD45,379, ; CLIB code
loc_40133,733, ; CLIB code
loc_404DD,139, ; CLIB code
loc_4056C,14, ; CLIB code
loc_4680B,1797,;ret|POP tmp1|tcall tmp1|endp ; CLIB code
loc_46FA4,1484, ; CLIB code
loc_475A1,4223, ; CLIB code
loc_488D5,859, ; CLIB code
loc_48F59,503, ; CLIB code
loc_491B0,2800,;nop| ; CLIB code
loc_4BD0A,2182, ; CLIB code
loc_4C5C9,2903, ; CLIB code
loc_4D409,2322, ; CLIB code

loc_55500,544, ; CLIB data
loc_56520,100, ; CLIB data
loc_565A4,276, ; CLIB data
loc_566BC,596, ; CLIB data
loc_58432,102, ; CLIB data

loc_62B98,2144, ; CLIB data
loc_63400,12, ; CLIB data
