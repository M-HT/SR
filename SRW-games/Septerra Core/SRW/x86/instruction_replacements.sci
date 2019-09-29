loc_412391,5,mov eax, [loc_4AE91C]|test eax, eax|jz loc_4124B7 ; fix reading from NULL pointer

loc_46436B,19,call SoundEngine_StartTimer_asm2c ; replace Sound Engine start timer
loc_464494,6,call SoundEngine_StopTimer_asm2c|add esp, byte 4 ; replace Sound Engine stop timer

loc_4327CE,7,call sync_asm2c|mov ecx, [esp+0x10c] ; sync after save

loc_4105FD,3,ret ; dead code
