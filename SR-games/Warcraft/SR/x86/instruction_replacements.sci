loc_1EC32,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_20FF7,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_29F64,5,add eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_2A088,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_2AAD4,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_3D8F8,10,mov eax, [Game_ScreenWindow]|mov [loc_5AB08 + 4], eax ; get real address of screen window instead of 0x0a0000
loc_49174,6,add edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000

loc_21E4E,5,mov eax, edx ; skip drive check ?

loc_440A1,4,nop ; timer interrupt ???
loc_44120,1,add esp, 8|ret ; skip calling original timer interrupt
loc_4413C,1,ret ; timer interrupt ending
loc_4413D,7,ret ; timer interrupt ending

loc_3CE19,66,nop ; keyboard interrupt ending
loc_3CE64,1,ret ; keyboard interrupt ending

loc_21F11,5,mov ebx, [Game_stdin] ; fix stdin

loc_2276F,123,mov eax, 0x0a|mov edx, [Game_stdout]|call SR_fputc|mov [ebp-0x10], eax ; replace putchar macro
loc_2280E,123,mov eax, [ebp-0x0c]|mov edx, [Game_stdout]|call SR_fputc|mov [ebp-0x18], eax ; replace putchar macro
loc_2288E,123,mov eax, 0x0a|mov edx, [Game_stdout]|call SR_fputc|mov [ebp-0x20], eax ; replace putchar macro

loc_223F9,5,mov edx, [Game_stderr] ; fix stderr
loc_2E82E,5,mov edx, [Game_stderr] ; fix stderr
loc_2E8E9,5,mov edx, [Game_stderr] ; fix stderr
loc_2E8FF,5,mov edx, [Game_stderr] ; fix stderr

loc_2C193,133,mov eax, [ebp-0x10]|inc dword [ebp-0x10]|mov al, [eax]|and eax, 0xff|mov edx, [ebp-0x0c]|call SR_fputc|mov [ebp-0x18], eax ; replace putc macro

loc_16D63,1,pop ebp|call SR_Sync ; call sync after save

loc_3B748,2,mov eax, 70|call SR___delay|xor eax, eax ; insert delay into victory screen

loc_4D17B,10,test eax, eax ; fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_4D1D8,11,test eax, eax ; fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_4D272,10,test eax, eax ; fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_4D2B9,11,test eax, eax ; fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))

loc_33C45,3,or eax, eax|je loc_33C48|mov al, [eax+0x1b]|loc_33C48: ; fix reading from NULL pointer
loc_33C55,3,or eax, eax|je loc_33C58|mov al, [eax+0x1b]|loc_33C58: ; fix reading from NULL pointer

loc_22A15,5,xor eax, eax ; set default? config - no sound, no music
loc_22A2E,5,call SR_memset|cmp dword [Game_Sound], 0|je loc_22A2E_1|mov word [loc_597DE], 0|loc_22A2E_1:|cmp dword [Game_Music], 0|je loc_22A2E_2|mov word [loc_597EE], 0|loc_22A2E_2: ; enable/disable sound,music driver

loc_2F395,15,mov eax, 1 ; fix loading sound driver

loc_2EFB5,15,mov eax, 1 ; fix loading music driver

loc_46545,4,mov eax, [esp+0x14]|or eax, eax|je loc_46570 ; fix loading NULL samples
loc_46570,5,loc_46570:|mov ecx, 9 ; fix loading NULL samples


loc_26BF5,6,xor eax, eax ; disable network (IPX) game
loc_26BAE,6,xor eax, eax ; disable modem/direct connection game
loc_18898,5,nop ; disable network/modem/direct connection game

loc_44455,3,xor ecx, ecx ; skip loading selector access rights
loc_444A6,3,xor ecx, ecx ; skip loading selector access rights
loc_44509,3,xor ecx, ecx ; skip loading selector access rights
loc_4456C,3,xor ecx, ecx ; skip loading selector access rights

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
loc_4680C,227, ; CLIB code
loc_468EF,1569, ; CLIB code
loc_46FA4,1484, ; CLIB code
loc_475A1,4223, ; CLIB code
loc_488D5,859, ; CLIB code
loc_48F59,503, ; CLIB code
loc_491B0,2800, ; CLIB code
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
loc_6340C,8196, ; stack
