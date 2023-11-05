loc_477BD1,18, ; isalnum macro
loc_477BF6,52,;call ms_isalnum_asm2c|PUSH loc_477BF6_after|tcall ms_isalnum_asm2c|endp|proc loc_477BF6_after|;add esp, byte 4|add esp, esp, 4 ; isalnum macro

loc_403AD9,104, ; skip CDROM check
loc_4B8F20,64, ; skip CDROM check

loc_44BF06,82, ; skip looking for file on CD
loc_4011C7,10, ; skip looking for file on CD
loc_427A39,281, ; skip looking for file on CD
loc_4BFDCC,12, ; skip looking for file on CD

loc_47EBF0,47,;call midi_OpenSDIMusic_asm2c|PUSH loc_47EBF0_after|tcall midi_OpenSDIMusic_asm2c|endp|proc loc_47EBF0_after ; midi - OpenSong
loc_47EC77,5,;call midi_GetErrorString_asm2c|PUSH loc_47EC77_after|tcall midi_GetErrorString_asm2c|endp|proc loc_47EC77_after ; midi - OpenSong
loc_47ECD2,10,;call midi_GetSDIMusicID_asm2c|PUSH loc_47ECD2_after|tcall midi_GetSDIMusicID_asm2c|endp|proc loc_47ECD2_after ; midi - OpenSong
loc_4CF1A4,36, ; midi - OpenSong

loc_47ED3B,28, ; midi - IsSongPlaying
loc_47ED5D,65,;call midi_IsPlaying_asm2c|PUSH loc_47ED9E|tcall midi_IsPlaying_asm2c|endp ; midi - IsSongPlaying

loc_47EE4B,24,;call midi_CloseSDIMusic_asm2c|PUSH loc_47EE4B_after|tcall midi_CloseSDIMusic_asm2c|endp|proc loc_47EE4B_after ; midi - ProcessSong
loc_47EEA9,24,;call midi_CloseSDIMusic_asm2c|PUSH loc_47EEA9_after|tcall midi_CloseSDIMusic_asm2c|endp|proc loc_47EEA9_after ; midi - ProcessSong
loc_47EF96,24,;call midi_CloseSDIMusic_asm2c|PUSH loc_47EF96_after|tcall midi_CloseSDIMusic_asm2c|endp|proc loc_47EF96_after ; midi - ProcessSong
loc_47F013,24,;call midi_PlaySDIMusic_asm2c|PUSH loc_47F013_after|tcall midi_PlaySDIMusic_asm2c|endp|proc loc_47F013_after ; midi - ProcessSong
loc_47F05B,5,;call midi_GetErrorString_asm2c|PUSH loc_47F05B_after|tcall midi_GetErrorString_asm2c|endp|proc loc_47F05B_after ; midi - ProcessSong
loc_4CF250,72, ; midi - ProcessSong

loc_47F0C0,21,;call midi_CloseSDIMusic_asm2c|PUSH loc_47F0C0_after|tcall midi_CloseSDIMusic_asm2c|endp|proc loc_47F0C0_after ; midi - CloseSong
loc_4CF2A8,16, ; midi - CloseSong

loc_484E3A,24,;call midi_OpenTestMusic_asm2c|PUSH loc_484E3A_after|tcall midi_OpenTestMusic_asm2c|endp|proc loc_484E3A_after ; midi - TestSong
loc_484EEB,5,;call midi_GetErrorString_asm2c|PUSH loc_484EEB_after|tcall midi_GetErrorString_asm2c|endp|proc loc_484EEB_after ; midi - TestSong
loc_484F29,24,;call midi_PlayTestMusic_asm2c|PUSH loc_484F29_after|tcall midi_PlayTestMusic_asm2c|endp|proc loc_484F29_after ; midi - TestSong
loc_484FC9,5,;call midi_GetErrorString_asm2c|PUSH loc_484FC9_after|tcall midi_GetErrorString_asm2c|endp|proc loc_484FC9_after ; midi - TestSong
loc_4850AD,24,;call midi_CloseTestMusic_asm2c|PUSH loc_4850C5|tcall midi_CloseTestMusic_asm2c|endp ; midi - TestSong
loc_4D0318,64, ; midi - TestSong
loc_4D0394,16, ; midi - TestSong

loc_486058,6,proc loc_48605E ; midi - winapi
loc_486064,6,proc loc_48606A ; midi - winapi

loc_4830C0,103,tcall loc_483127|endp ; skip VfW (Video for Windows) installation
loc_4A40A0,16, ; skip VfW (Video for Windows) installation
loc_4CFC88,68, ; skip VfW (Video for Windows) installation

loc_401E06,5, ; cmdline - ContainsOption
loc_427905,308, ; cmdline - ContainsOption

loc_44BA35,147,;push loc_4C2E54|PUSH loc_4C2E54|;call cmdline_ReadLanguageOption_asm2c|PUSH loc_44BA35_after|tcall cmdline_ReadLanguageOption_asm2c|endp|proc loc_44BA35_after ; cmdline - ReadLanguageOption

loc_47FA4E,81,;call video_RegisterClass_PRE_Video_asm2c|PUSH loc_47FA4E_after|tcall video_RegisterClass_PRE_Video_asm2c|endp|proc loc_47FA4E_after ; video - RegisterClass_PRE_Video
loc_4CF654,16, ; video - RegisterClass_PRE_Video
loc_401AEB,5, ; video - PRE_VideoCallback
loc_47F17A,981, ; video - PRE_VideoCallback

loc_48176D,81,;call video_RegisterClass_POST_Video_asm2c|PUSH loc_48176D_after|tcall video_RegisterClass_POST_Video_asm2c|endp|proc loc_48176D_after ; video - RegisterClass_POST_Video
loc_4CFAF8,16, ; video - RegisterClass_POST_Video
loc_4019B0,5, ; video - POST_VideoCallback
loc_481270,471, ; video - POST_VideoCallback

loc_482DF5,114,;call video_RegisterClass_SS_Video_asm2c|PUSH loc_482DF5_after|tcall video_RegisterClass_SS_Video_asm2c|endp|proc loc_482DF5_after ; video - RegisterClass_SS_Video
loc_4CFEF0,12, ; video - RegisterClass_SS_Video
loc_40146F,10, ; video - SS_VideoCallback
loc_482A3E,159, ; video - SS_VideoCallback

loc_480B11,1,;cmp dword [Intro_Play], 0|load tmp0, Intro_Play, 4|;jnz short loc_480B11_after|ctcallnz tmp0, loc_480B11_after|tcall loc_480B11_after2|endp|proc loc_480B11_after2|;retn|POP tmp1|tcall tmp1|endp|;loc_480B11_after:|proc loc_480B11_after|;push ebp|PUSH ebp ; skip Intro videos
loc_48183D,1,;cmp dword [Outro_Play], 0|load tmp0, Outro_Play, 4|;jnz short loc_48183D_after|ctcallnz tmp0, loc_48183D_after|tcall loc_48183D_after2|endp|proc loc_48183D_after2|;retn|POP tmp1|tcall tmp1|endp|;loc_48183D_after:|proc loc_48183D_after|;push ebp|PUSH ebp ; skip Outro videos

loc_47F62C,3,;add esp, byte 8|add esp, esp, 8|;push dword [ebp+8]|add tmpadr, ebp, 8|load tmp0, tmpadr, 4|PUSH tmp0|;call video_Open_PRE_Video_asm2c|PUSH loc_47F62C_after|tcall video_Open_PRE_Video_asm2c|endp|proc loc_47F62C_after|;or eax, eax|cmovsgt eax, 0, tmp0, 1, 0|;jg loc_47F93F|ctcallnz tmp0, loc_47F93F|tcall loc_47F62C_after2|endp|proc loc_47F62C_after2 ; video - Open_PRE_Video
loc_47F973,1,;push edi|PUSH edi|;call video_Close_PRE_Video_asm2c|PUSH loc_47F973_after|tcall video_Close_PRE_Video_asm2c|endp|proc loc_47F973_after|;or eax, eax|cmovslt eax, 0, tmp0, 0, 1|;jge short loc_47F9DD|ctcallnz tmp0, loc_47F9DD|tcall loc_47F973_after2|endp|proc loc_47F973_after2 ; video - Close_PRE_Video
loc_47FC27,3,;add esp, byte 4|add esp, esp, 4|;push dword [ebp+12]|add tmpadr, ebp, 12|load tmp0, tmpadr, 4|PUSH tmp0|;call video_Play_PRE_Video_asm2c|PUSH loc_47FC27_after|tcall video_Play_PRE_Video_asm2c|endp|proc loc_47FC27_after|;or eax, eax|cmovslt eax, 0, tmp0, 0, 1|;jge loc_480ADC|ctcallnz tmp0, loc_480ADC|tcall loc_47FC27_after2|endp|proc loc_47FC27_after2 ; video - Play_PRE_Video

loc_481504,3,;add esp, byte 8|add esp, esp, 8|;push dword [ebp+8]|add tmpadr, ebp, 8|load tmp0, tmpadr, 4|PUSH tmp0|;call video_Open_POST_Video_asm2c|PUSH loc_481504_after|tcall video_Open_POST_Video_asm2c|endp|proc loc_481504_after|;or eax, eax|cmovsgt eax, 0, tmp0, 1, 0|;jg loc_4816D9|ctcallnz tmp0, loc_4816D9|tcall loc_481504_after2|endp|proc loc_481504_after2 ; video - Open_POST_Video
loc_4816E6,1,;push edi|PUSH edi|;call video_Close_POST_Video_asm2c|PUSH loc_4816E6_after|tcall video_Close_POST_Video_asm2c|endp|proc loc_4816E6_after|;or eax, eax|cmovslt eax, 0, tmp0, 0, 1|;jge short loc_481705|ctcallnz tmp0, loc_481705|tcall loc_4816E6_after2|endp|proc loc_4816E6_after2 ; video - Close_POST_Video
loc_481705,2,;loc_481705:|;push byte 0|PUSH 0 ; video - Close_POST_Video
loc_481954,3,;add esp, byte 4|add esp, esp, 4|;push dword [ebp+8]|add tmpadr, ebp, 8|load tmp0, tmpadr, 4|PUSH tmp0|;call video_Play_POST_Video_asm2c|PUSH loc_481954_after|tcall video_Play_POST_Video_asm2c|endp|proc loc_481954_after|;or eax, eax|cmovslt eax, 0, tmp0, 0, 1|;jge loc_482332|ctcallnz tmp0, loc_482332|tcall loc_481954_after2|endp|proc loc_481954_after2 ; video - Play_POST_Video
loc_482332,2,;loc_482332:|;push byte 1|PUSH 1 ; video - Play_POST_Video

loc_407F73,3,;add esp, byte 4|add esp, esp, 4|;push dword 0|PUSH 0|;call SDI_CheckTicksDelay_asm2c|PUSH loc_407F73_after|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_407F73_after ; insert delays into active waiting
loc_40FB6B,5,;push dword 1|PUSH 1|;call SDI_CheckTicksDelay_asm2c|PUSH loc_40FB5C_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_40FB5C_after1|;call SYSTEM_GetTicks|PUSH loc_40FB5C_after2|tcall SYSTEM_GetTicks|endp|proc loc_40FB5C_after2 ; insert delays into active waiting
loc_410BBD,5,;push dword 2|PUSH 2|;call SDI_CheckTicksDelay_asm2c|PUSH loc_410BBD_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_410BBD_after1|;call SYSTEM_GetTicks|PUSH loc_410BBD_after2|tcall SYSTEM_GetTicks|endp|proc loc_410BBD_after2 ; insert delays into active waiting
loc_410E4B,3,;add esp, byte 3*4|add esp, esp, 3*4|;push dword 3|PUSH 3|;call SDI_CheckTicksDelay_asm2c|PUSH loc_410E4B_after|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_410E4B_after ; insert delays into active waiting
loc_411674,5,;push dword 4|PUSH 4|;call SDI_CheckTicksDelay_asm2c|PUSH loc_411674_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_411674_after1|;call SYSTEM_GetTicks|PUSH loc_411674_after2|tcall SYSTEM_GetTicks|endp|proc loc_411674_after2 ; insert delays into active waiting
loc_430FCF,5,;push dword 5|PUSH 5|;call SDI_CheckTicksDelay_asm2c|PUSH loc_430FCF_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_430FCF_after1|;call SYSTEM_GetTicks|PUSH loc_430FCF_after2|tcall SYSTEM_GetTicks|endp|proc loc_430FCF_after2 ; insert delays into active waiting
loc_44E706,5,;push dword 6|PUSH 6|;call SDI_CheckTicksDelay_asm2c|PUSH loc_44E706_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_44E706_after1|;call SYSTEM_GetTicks|PUSH loc_44E706_after2|tcall SYSTEM_GetTicks|endp|proc loc_44E706_after2 ; insert delays into active waiting
loc_4630CE,3,;add ebx, byte 4|add ebx, ebx, 4|;push dword 7|PUSH 7|;call SDI_CheckTicksDelay_asm2c|PUSH loc_4630CE_after|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_4630CE_after ; insert delays into active waiting
loc_467353,5,;push dword 8|PUSH 8|;call SDI_CheckTicksDelay_asm2c|PUSH loc_467353_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_467353_after1|;call SYSTEM_GetTicks|PUSH loc_467353_after2|tcall SYSTEM_GetTicks|endp|proc loc_467353_after2 ; insert delays into active waiting
loc_47B0CA,5,;push dword 9|PUSH 9|;call SDI_CheckTicksDelay_asm2c|PUSH loc_47B0CA_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_47B0CA_after1|;call SYSTEM_GetTicks|PUSH loc_47B0CA_after2|tcall SYSTEM_GetTicks|endp|proc loc_47B0CA_after2 ; insert delays into active waiting
loc_47B197,5,;push dword 10|PUSH 10|;call SDI_CheckTicksDelay_asm2c|PUSH loc_47B197_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_47B197_after1|;call SYSTEM_GetTicks|PUSH loc_47B197_after2|tcall SYSTEM_GetTicks|endp|proc loc_47B197_after2 ; insert delays into active waiting
loc_47EF2B,5,;push dword 11|PUSH 11|;call SDI_CheckTicksDelay_asm2c|PUSH loc_47EF2B_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_47EF2B_after1|;call SYSTEM_GetTicks|PUSH loc_47EF2B_after2|tcall SYSTEM_GetTicks|endp|proc loc_47EF2B_after2 ; insert delays into active waiting
loc_484C8F,5,;push dword 12|PUSH 12|;call SDI_CheckTicksDelay_asm2c|PUSH loc_484C8F_after1|tcall SDI_CheckTicksDelay_asm2c|endp|proc loc_484C8F_after1|;call SYSTEM_GetTicks|PUSH loc_484C8F_after2|tcall SYSTEM_GetTicks|endp|proc loc_484C8F_after2 ; insert delays into active waiting

loc_476A31,7,PUSH loc_476A31_after|tcall sync_asm2c|endp|proc loc_476A31_after|;mov byte [ebp-0x18c], 0x1|add tmpadr, ebp, -396|mov tmp1, 0x1|store8 tmp1, tmpadr, 4|tcall loc_476A38|endp ; sync after save

loc_46B0BF,10,;cmp dword [Patch_IncreaseMaxWindowSize], 0|load tmp1, Patch_IncreaseMaxWindowSize, 4|;jz short loc_46B0BF_after|ifnz tmp1|;mov byte [ecx+eax*8+0x3f], 0x40|shl tmpadr, eax, 3|add tmpadr, tmpadr, ecx|add tmpadr, tmpadr, 0x3f|store8 0x40, tmpadr, 1|;loc_46B0BF_after:|endif|;movzx eax, word [ecx+eax*8+0x3e]|shl tmpadr, eax, 3|add tmpadr, tmpadr, ecx|add tmpadr, tmpadr, 0x3e|load16z eax, tmpadr, 1 ; increase max window size
loc_46B153,10,;cmp dword [Patch_IncreaseMaxWindowSize], 0|load tmp1, Patch_IncreaseMaxWindowSize, 4|;jz short loc_46B153_after|ifnz tmp1|;mov byte [ecx+eax*8+0x7], 0x40|shl tmpadr, eax, 3|add tmpadr, tmpadr, ecx|add tmpadr, tmpadr, 0x7|store8 0x40, tmpadr, 1|;loc_46B153_after:|endif|;movzx eax, word [ecx+eax*8+0x6]|shl tmpadr, eax, 3|add tmpadr, tmpadr, ecx|add tmpadr, tmpadr, 0x6|load16z eax, tmpadr, 1 ; increase max window size
