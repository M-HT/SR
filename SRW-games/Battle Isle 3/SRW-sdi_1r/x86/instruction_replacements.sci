loc_477BD1,18, ; isalnum macro
loc_477BF6,52,call ms_isalnum_asm2c|add esp, byte 4 ; isalnum macro

loc_403AD9,104, ; skip CDROM check
loc_4B8F20,64, ; skip CDROM check

loc_44BF06,82, ; skip looking for file on CD
loc_4011C7,10, ; skip looking for file on CD
loc_427A39,281, ; skip looking for file on CD
loc_4BFDCC,12, ; skip looking for file on CD

loc_47EBF0,47,call midi_OpenSDIMusic_asm2c ; midi - OpenSong
loc_47EC77,5,call midi_GetErrorString_asm2c ; midi - OpenSong
loc_47ECD2,10,call midi_GetSDIMusicID_asm2c ; midi - OpenSong
loc_4CF1A4,36, ; midi - OpenSong

loc_47ED3B,28, ; midi - IsSongPlaying
loc_47ED5D,65,call midi_IsPlaying_asm2c ; midi - IsSongPlaying

loc_47EE4B,24,call midi_CloseSDIMusic_asm2c ; midi - ProcessSong
loc_47EEA9,24,call midi_CloseSDIMusic_asm2c ; midi - ProcessSong
loc_47EF96,24,call midi_CloseSDIMusic_asm2c ; midi - ProcessSong
loc_47F013,24,call midi_PlaySDIMusic_asm2c ; midi - ProcessSong
loc_47F05B,5,call midi_GetErrorString_asm2c ; midi - ProcessSong
loc_4CF250,72, ; midi - ProcessSong

loc_47F0C0,21,call midi_CloseSDIMusic_asm2c ; midi - CloseSong
loc_4CF2A8,16, ; midi - CloseSong

loc_484E3A,24,call midi_OpenTestMusic_asm2c ; midi - TestSong
loc_484EEB,5,call midi_GetErrorString_asm2c ; midi - TestSong
loc_484F29,24,call midi_PlayTestMusic_asm2c ; midi - TestSong
loc_484FC9,5,call midi_GetErrorString_asm2c ; midi - TestSong
loc_4850AD,24,call midi_CloseTestMusic_asm2c ; midi - TestSong
loc_4D0318,64, ; midi - TestSong
loc_4D0394,16, ; midi - TestSong

loc_486058,6, ; midi - winapi
loc_486064,6, ; midi - winapi

loc_4830C0,103, ; skip VfW (Video for Windows) installation
loc_4A40A0,16, ; skip VfW (Video for Windows) installation
loc_4CFC88,68, ; skip VfW (Video for Windows) installation

loc_401E06,5, ; cmdline - ContainsOption
loc_427905,308, ; cmdline - ContainsOption

loc_44BA35,147,push loc_4C2E54|call cmdline_ReadLanguageOption_asm2c ; cmdline - ReadLanguageOption

loc_47FA4E,81,call video_RegisterClass_PRE_Video_asm2c ; video - RegisterClass_PRE_Video
loc_4CF654,16, ; video - RegisterClass_PRE_Video
loc_401AEB,5, ; video - PRE_VideoCallback
loc_47F17A,981, ; video - PRE_VideoCallback

loc_48176D,81,call video_RegisterClass_POST_Video_asm2c ; video - RegisterClass_POST_Video
loc_4CFAF8,16, ; video - RegisterClass_POST_Video
loc_4019B0,5, ; video - POST_VideoCallback
loc_481270,471, ; video - POST_VideoCallback

loc_482DF5,114,call video_RegisterClass_SS_Video_asm2c ; video - RegisterClass_SS_Video
loc_4CFEF0,12, ; video - RegisterClass_SS_Video
loc_40146F,10, ; video - SS_VideoCallback
loc_482A3E,159, ; video - SS_VideoCallback

loc_480B11,1,cmp dword [Intro_Play], 0|jnz short loc_480B11_after|retn|loc_480B11_after:|push ebp ; skip Intro videos
loc_48183D,1,cmp dword [Outro_Play], 0|jnz short loc_48183D_after|retn|loc_48183D_after:|push ebp ; skip Outro videos

loc_47F62C,3,add esp, byte 8|push dword [ebp+8]|call video_Open_PRE_Video_asm2c|or eax, eax|jge loc_47F93F ; video - Open_PRE_Video
loc_47F973,1,push edi|call video_Close_PRE_Video_asm2c|or eax, eax|jge short loc_47F9DD ; video - Close_PRE_Video
loc_47FC27,3,add esp, byte 4|push dword [ebp+12]|call video_Play_PRE_Video_asm2c|or eax, eax|jge loc_480ADC ; video - Play_PRE_Video

loc_481504,3,add esp, byte 8|push dword [ebp+8]|call video_Open_POST_Video_asm2c|or eax, eax|jge loc_4816D9 ; video - Open_POST_Video
loc_4816E6,1,push edi|call video_Close_POST_Video_asm2c|or eax, eax|jge short loc_481705 ; video - Close_POST_Video
loc_481705,2,loc_481705:|push byte 0 ; video - Close_POST_Video
loc_481954,3,add esp, byte 4|push dword [ebp+8]|call video_Play_POST_Video_asm2c|or eax, eax|jge loc_482332 ; video - Play_POST_Video
loc_482332,2,loc_482332:|push byte 1 ; video - Play_POST_Video

loc_407F73,3,add esp, byte 4|push dword 0|call SDI_CheckTicksDelay_asm2c ; insert delays into active waiting
loc_40FB6B,5,push dword 1|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting
loc_410BBD,5,push dword 2|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting
loc_410E4B,3,add esp, byte 3*4|push dword 3|call SDI_CheckTicksDelay_asm2c ; insert delays into active waiting
loc_411674,5,push dword 4|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting
loc_430FCF,5,push dword 5|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting
loc_44E706,5,push dword 6|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting
loc_4630CE,3,add ebx, byte 4|push dword 7|call SDI_CheckTicksDelay_asm2c ; insert delays into active waiting
loc_467353,5,push dword 8|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting
loc_47B0CA,5,push dword 9|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting
loc_47B197,5,push dword 10|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting
loc_47EF2B,5,push dword 11|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting
loc_484C8F,5,push dword 12|call SDI_CheckTicksDelay_asm2c|call SYSTEM_GetTicks ; insert delays into active waiting

loc_476A31,7,call sync_asm2c|mov byte [ebp-0x18c], 0x1 ; sync after save

loc_46B0BF,10,cmp dword [Patch_IncreaseMaxWindowSize], 0|jz short loc_46B0BF_after|mov byte [ecx+eax*8+0x3f], 0x40|loc_46B0BF_after:|movzx eax, word [ecx+eax*8+0x3e] ; increase max window size
loc_46B153,10,cmp dword [Patch_IncreaseMaxWindowSize], 0|jz short loc_46B153_after|mov byte [ecx+eax*8+0x7], 0x40|loc_46B153_after:|movzx eax, word [ecx+eax*8+0x6] ; increase max window size
