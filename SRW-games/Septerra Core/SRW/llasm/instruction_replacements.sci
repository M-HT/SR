loc_412391,5,;mov eax, [loc_4AE91C]|load eax, loc_4AE91C, 4|;test eax, eax|;jz loc_4124B7|ctcallz eax, loc_4124B7|tcall loc_412396|endp|proc loc_412396 ; fix reading from NULL pointer

loc_46436B,19,;call SoundEngine_StartTimer_asm2c|PUSH loc_46437E|tcall SoundEngine_StartTimer_asm2c|endp|proc loc_46437E ; replace Sound Engine start timer
loc_464494,6,;call SoundEngine_StopTimer_asm2c|PUSH loc_46449A|tcall SoundEngine_StopTimer_asm2c|endp|proc loc_46449A|;add esp, byte 4|add esp, esp, 4 ; replace Sound Engine stop timer

loc_4327CE,7,PUSH loc_4327D5|tcall sync_asm2c|endp|proc loc_4327D5|;mov ecx, [esp+0x10c]|add tmpadr, esp, 268|load ecx, tmpadr, 4 ; sync after save

loc_4105FD,3,;ret|POP tmp1|tcall tmp1|endp ; dead code
