loc_1AE00,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_1C91B,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_230FB,5,add eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_23190,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_238BA,5,mov eax, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000
loc_31E85,10,mov eax, [Game_ScreenWindow]|mov [loc_5AE20 + 4], eax ; get real address of screen window instead of 0x0a0000
loc_3D5F2,6,add edi, [Game_ScreenWindow] ; get real address of screen window instead of 0x0a0000

loc_1D2E8,5,mov eax, edx ; skip drive check ?

loc_385E1,4,nop ; timer interrupt ???
loc_38660,1,add esp, 8|ret ; skip calling original timer interrupt
loc_3867C,1,ret ; timer interrupt ending
loc_3867D,7,ret ; timer interrupt ending

loc_313B9,66,nop ; keyboard interrupt ending
loc_31404,1,ret ; keyboard interrupt ending

loc_1D37B,5,mov ebx, [Game_stdin] ; fix stdin

loc_1D9B4,14, ; replace putchar macro (part 1)
loc_1D9C7,96,mov eax, 0x0a|mov edx, [Game_stdout]|call SR_fputc|jmp short loc_1DA93 ; replace putchar macro (part 2)
loc_1DA3F,84,mov edx, [Game_stdout]|call SR_fputc ; replace putchar macro
loc_1DA9F,98,mov eax, 0x0a|mov edx, [Game_stdout]|call SR_fputc ; replace putchar macro

loc_1D735,5,mov edx, [Game_stderr] ; fix stderr
loc_266A2,5,mov edx, [Game_stderr] ; fix stderr
loc_26748,5,mov edx, [Game_stderr] ; fix stderr
loc_2675C,5,mov edx, [Game_stderr] ; fix stderr

loc_24B35,80,xor eax, eax|mov al, [ecx]|inc ecx|mov edx, ebx|call SR_fputc ; replace putc macro

loc_14C6D,1,pop ebp|call SR_Sync ; call sync after save

loc_2FD4B,2,mov eax, 70|call SR___delay|xor eax, eax ; insert delay into victory screen

loc_415F3,10,test eax, eax ; fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_4165E,10,test eax, eax ; fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_416F0,11,test eax, eax ; fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))
loc_41740,11,test eax, eax ; fix reading FILE internals (modified getc macro) (('test ...' must set flags for 'jnz'))

loc_2A1AF,4,xor edi, edi|or ebp, ebp|je loc_2A1B3|movzx edi, byte [ebp+0x1b]|loc_2A1B3: ; fix reading from NULL pointer

loc_1DBD6,5,xor eax, eax ; set default? config - no sound, no music
loc_1DBEF,5,call SR_memset|cmp dword [Game_Sound], 0|je loc_1DBEF_1|mov word [loc_59A42], 0|loc_1DBEF_1:|cmp dword [Game_Music], 0|je loc_1DBEF_2|mov word [loc_59A52], 0|loc_1DBEF_2: ; enable/disable sound,music driver

loc_27095,15,mov eax, 1 ; fix loading sound driver

loc_26CB5,15,mov eax, 1 ; fix loading music driver

loc_3AA85,4,mov eax, [esp+0x14]|or eax, eax|je loc_3AAB0 ; fix loading NULL samples
loc_3AAB0,5,loc_3AAB0:|mov ecx, 9 ; fix loading NULL samples


loc_20C50,6,xor eax, eax ; disable network (IPX) game
loc_20C0D,6,xor eax, eax ; disable modem/direct connection game
loc_16174,5,nop ; disable network/modem/direct connection game

loc_38995,3,xor ecx, ecx ; skip loading selector access rights
loc_389E6,3,xor ecx, ecx ; skip loading selector access rights
loc_38A49,3,xor ecx, ecx ; skip loading selector access rights
loc_38AAC,3,xor ecx, ecx ; skip loading selector access rights
