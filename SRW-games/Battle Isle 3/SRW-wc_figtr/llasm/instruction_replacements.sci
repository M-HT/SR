loc_410665,80, ; replace thunk function __Fight with function _Fight

loc_4129B1,5,;push dword 1|PUSH 1|;call FGT_CheckTicksDelay_asm2c|PUSH loc_4129B1_after1|tcall FGT_CheckTicksDelay_asm2c|endp|proc loc_4129B1_after1|;call SYSTEM_GetTicks|PUSH loc_4129B1_after2|tcall SYSTEM_GetTicks|endp|proc loc_4129B1_after2 ; insert delays into active waiting
loc_41B9D2,5,;push dword 2|PUSH 2|;call FGT_CheckTicksDelay_asm2c|PUSH loc_41B9D2_after1|tcall FGT_CheckTicksDelay_asm2c|endp|proc loc_41B9D2_after1|;call SYSTEM_GetTicks|PUSH loc_41B9D2_after2|tcall SYSTEM_GetTicks|endp|proc loc_41B9D2_after2 ; insert delays into active waiting

loc_41299E,7,;mov [loc_4A56AE], dx|store16 edx, loc_4A56AE, 2|;push dword 1|PUSH 1|;call FGT_CheckTicksDelay_asm2c|PUSH loc_41299E_after|tcall FGT_CheckTicksDelay_asm2c|endp|proc loc_41299E_after ; insert delays into active waiting
loc_41B9BF,7,;mov [loc_4A56AE], dx|store16 edx, loc_4A56AE, 2|;push dword 2|PUSH 2|;call FGT_CheckTicksDelay_asm2c|PUSH loc_41B9BF_after|tcall FGT_CheckTicksDelay_asm2c|endp|proc loc_41B9BF_after ; insert delays into active waiting

loc_451432,3017,and ecx, ecx, 0xfffffffc|ctcallz ecx, loc_451432_after|tcall loc_451432_loop|endp|proc loc_451432_loop|add tmpadr, edi, ecx|sub tmpadr, tmpadr, 4|load tmp1, tmpadr, 1|and tmp1, tmp1, 0x1f1f1f1f|or tmp1, tmp1, eax|store tmp1, tmpadr, 1|sub ecx, ecx, 4|ctcallnz ecx, loc_451432_loop|tcall loc_451432_after|endp|proc loc_451432_after ; roll back unrolled loop
loc_4520A7,801, ; remove unrolled loop offsets

loc_4524CB,8635,and ecx, ecx, 0xfffffffe|ctcallz ecx, loc_4524CB_after|tcall loc_4524CB_loop|endp|proc loc_4524CB_loop|add tmpadr, edi, ecx|sub tmpadr, tmpadr, 2|load16z tmp0, tmpadr, 1|ins8lh eax, eax, tmp0|load8z tmp2, eax, 1|ins8ll eax, eax, tmp0|load8z tmp1, eax, 1|shl tmp2, tmp2, 8|or tmp1, tmp1, tmp2|store16 tmp1, tmpadr, 1|sub ecx, ecx, 2|ctcallnz ecx, loc_4524CB_loop|tcall loc_4524CB_after|endp|proc loc_4524CB_after ; roll back unrolled loop
loc_454721,1738, ; remove unrolled loop offsets

loc_4729A4,10644,and eflags, eflags, ~CF|ctcallz ebx, loc_4729A4_after|tcall loc_4729A4_loop|endp|proc loc_4729A4_loop|ins8hl eax, eax, edx|ins8ll eax, eax, ecx|load8z tmp0, eax, 1|and tmp5, eflags, CF|and eflags, eflags, ~CF|add tmp3, esi, tmp5|add ecx, ecx, tmp3|cmovz tmp3, tmp5, tmp5, 0|cmovult ecx, tmp3, tmp5, CF, tmp5|add tmp3, ebp, tmp5|add edx, edx, tmp3|cmovz tmp3, tmp5, tmp5, 0|cmovult edx, tmp3, tmp5, CF, tmp5|or eflags, eflags, tmp5|add tmpadr, edi, ebx|sub tmpadr, tmpadr, 1|store8 tmp0, tmpadr, 1|sub ebx, ebx, 1|ctcallnz ebx, loc_4729A4_loop|tcall loc_4729A4_after|endp|proc loc_4729A4_after ; roll back unrolled loop

loc_47543C,13054,and eflags, eflags, ~CF|ctcallz ebx, loc_47543C_after_loop|tcall loc_47543C_loop|endp|proc loc_47543C_loop|ins8hl eax, eax, edx|ins8ll eax, eax, ecx|and tmp5, eflags, CF|and eflags, eflags, ~CF|add tmp3, esi, tmp5|add ecx, ecx, tmp3|cmovz tmp3, tmp5, tmp5, 0|cmovult ecx, tmp3, tmp5, CF, tmp5|add tmp3, ebp, tmp5|add edx, edx, tmp3|cmovz tmp3, tmp5, tmp5, 0|cmovult edx, tmp3, tmp5, CF, tmp5|or eflags, eflags, tmp5|load tmp0, loc_4C32A8, 4|load8z tmp1, eax, 1|and tmp0, tmp0, 0xffffff00|or tmp0, tmp0, tmp1|load8z tmp2, tmp0, 1|add tmpadr, edi, ebx|sub tmpadr, tmpadr, 1|store8 tmp2, tmpadr, 1|sub ebx, ebx, 1|ctcallnz ebx, loc_47543C_loop|tcall loc_47543C_after_loop|endp|proc loc_47543C_after_loop|load ebx, loc_4C32A8, 4 ; roll back unrolled loop
loc_4787BA,6000, ; remove unrolled loop offsets

loc_479FEE,13054,and eflags, eflags, ~CF|ctcallz ebx, loc_479FEE_after_loop|tcall loc_479FEE_loop|endp|proc loc_479FEE_loop|ins8hl eax, eax, edx|ins8ll eax, eax, ecx|and tmp5, eflags, CF|and eflags, eflags, ~CF|add tmp3, esi, tmp5|add ecx, ecx, tmp3|cmovz tmp3, tmp5, tmp5, 0|cmovult ecx, tmp3, tmp5, CF, tmp5|add tmp3, ebp, tmp5|add edx, edx, tmp3|cmovz tmp3, tmp5, tmp5, 0|cmovult edx, tmp3, tmp5, CF, tmp5|or eflags, eflags, tmp5|load tmp0, loc_4C32A8, 4|load8z tmp1, eax, 1|and tmp0, tmp0, 0xffffff00|or tmp0, tmp0, tmp1|load8z tmp2, tmp0, 1|add tmpadr, edi, ebx|sub tmpadr, tmpadr, 1|store8 tmp2, tmpadr, 1|sub ebx, ebx, 1|ctcallnz ebx, loc_479FEE_loop|tcall loc_479FEE_after_loop|endp|proc loc_479FEE_after_loop|load ebx, loc_4C32A8, 4 ; roll back unrolled loop
loc_47D36C,7600, ; remove unrolled loop offsets

loc_480221,16426,ctcallz edx, loc_48424B|tcall loc_480221_loop|endp|proc loc_480221_loop|add eax, eax, ebx|cmovult eax, ebx, tmp2, 1, 0|add eax, eax, tmp2|ext16s tmp1, eax|add tmpadr, tmp1, esi|load8z tmp0, tmpadr, 1|ifnz tmp0|add tmpadr, edi, edx|sub tmpadr, tmpadr, 1|store8 tmp0, tmpadr, 1|endif|sub edx, edx, 1|ctcallnz edx, loc_480221_loop|tcall loc_48424B|endp ; roll back unrolled loop
loc_484266,3394,;pop ebp|POP ebp|;retn|POP tmp1|tcall tmp1|endp ; remove unrolled loop offsets
