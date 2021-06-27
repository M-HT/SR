loc_100016B4,13,tcall loc_100016C1|endp ; unused stock object
loc_10001832,12,tcall loc_1000183E|endp ; unused stock object
loc_1000B01C,28, ; unused stock object

loc_100027BE,71, ; unused device context
loc_10002B74,18, ; unused device context
loc_10008D80,4, ; unused device context
loc_1000B284,32, ; unused device context

loc_1000277E,64, ; unused window handle
loc_100045BE,6, ; unused window handle
loc_10008D84,4, ; unused window handle
loc_1000B264,32, ; unused window handle

loc_10001510,6,;call __Fight|PUSH loc_10001510_after|tcall __Fight|endp|proc loc_10001510_after ; call __Fight directly
loc_100017BE,6,;call __Fight|PUSH loc_100017BE_after|tcall __Fight|endp|proc loc_100017BE_after ; call __Fight directly
loc_10002B26,6,;call __Fight|PUSH loc_10002B26_after|tcall __Fight|endp|proc loc_10002B26_after ; call __Fight directly

loc_10001487,83, ; remove dynamic load of __Fight
loc_1000B00C,4, ; remove dynamic load of __Fight
loc_1000B164,32, ; remove dynamic load of __Fight

loc_10001450,55, ; remove dynamic load of wc_figtr.dll
loc_1000158B,12, ; remove dynamic load of wc_figtr.dll
loc_100015FD,12, ; remove dynamic load of wc_figtr.dll
loc_1000180C,25, ; remove dynamic load of wc_figtr.dll
loc_10002AC5,12, ; remove dynamic load of wc_figtr.dll
loc_1000B008,4, ; remove dynamic load of wc_figtr.dll
loc_1000B134,48, ; remove dynamic load of wc_figtr.dll

loc_10003FF2,63,;push eax|PUSH eax|;call FGT_SystemTask_End_asm2c|PUSH loc_10003FF2_after|tcall FGT_SystemTask_End_asm2c|endp|proc loc_10003FF2_after ; replace end of function FGT_SystemTask
loc_10004036,49,;push dword 0|PUSH 0|;call FGT_SystemTask_End_asm2c|PUSH loc_10004036_after|tcall FGT_SystemTask_End_asm2c|endp|proc loc_10004036_after|tcall loc_10004067|endp ; replace end of function FGT_SystemTask

loc_10002C1B,5,;push dword 0|PUSH 0|;call FGT_CheckTicksDelay_asm2c|PUSH loc_10002C1B_after1|tcall FGT_CheckTicksDelay_asm2c|endp|proc loc_10002C1B_after1|;call SYSTEM_GetTicks|PUSH loc_10002C1B_after2|tcall SYSTEM_GetTicks|endp|proc loc_10002C1B_after2 ; insert delays into active waiting
