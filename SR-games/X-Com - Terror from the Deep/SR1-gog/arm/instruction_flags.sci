loc_102DA,0   ,0x08       ; jnz - FL_ZERO

loc_10343,0   ,0x08       ; jnz - FL_ZERO

loc_113C0,0   ,0x08       ; jnz - FL_ZERO

loc_114C4,0x08,0          ; jz - FL_ZERO
loc_114B4,0x08,0          ; jz - FL_ZERO
loc_114B6,0   ,0x08       ; jz - FL_ZERO

loc_115B2,0   ,0x08       ; jnz - FL_ZERO

loc_11684,0x08,0          ; jz - FL_ZERO
loc_11674,0x08,0          ; jz - FL_ZERO
loc_11676,0   ,0x08       ; jz - FL_ZERO

loc_131A5,0x08,0          ; jz - FL_ZERO
loc_131C4,0   ,0x08       ; jz - FL_ZERO

loc_195AC,0x08,0          ; jnz - FL_ZERO
loc_195C5,0   ,0x08       ; jnz - FL_ZERO

loc_1981F,0x08,0          ; jnz - FL_ZERO
loc_19831,0   ,0x08       ; jnz - FL_ZERO

loc_1A0F1,0x08,0          ; jnz - FL_ZERO
loc_1A118,0   ,0x08       ; jnz - FL_ZERO

loc_1E19E,0   ,0x08       ; jz - FL_ZERO

loc_21DEA,0x08,0          ; setnz - FL_ZERO
loc_21CA0,0x08,0          ; setnz - FL_ZERO
loc_21E29,0x08,0          ; setnz - FL_ZERO
loc_21E2D,0   ,0x08       ; setnz - FL_ZERO

loc_24B1B,0x08,0          ; jnz - FL_ZERO
loc_24B1D,0x08,0          ; jnz - FL_ZERO
loc_24B22,0   ,0x08       ; jnz - FL_ZERO

loc_2649E,0x08,0          ; jnz - FL_ZERO
loc_264A4,0   ,0x08       ; jnz - FL_ZERO

loc_264FB,0   ,0x08       ; jz - FL_ZERO

loc_26F10,0   ,0x08       ; jnz - FL_ZERO

loc_27618,0   ,0x08       ; jz - FL_ZERO

loc_2771D,0   ,0x08       ; jz - FL_ZERO

loc_277C7,0   ,0x08       ; jz - FL_ZERO

loc_29916,0   ,0x08       ; jnz - FL_ZERO

loc_2B26F,0   ,0x08       ; jz - FL_ZERO

loc_3046C,0x08,0          ; jnz - FL_ZERO
loc_3046E,0x08,0          ; jnz - FL_ZERO
loc_30475,0   ,0x08       ; jnz - FL_ZERO

loc_326C4,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_32710,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_3CE81,0x08,0          ; jnz - FL_ZERO
loc_3CEA2,0   ,0x08       ; jnz - FL_ZERO

loc_3EFC0,0   ,0x08       ; jnz - FL_ZERO

loc_47102,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_4710E,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_48322,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_4832C,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_4AF5D,0x08,0          ; jz - FL_ZERO
loc_4AFB6,0x08,0          ; jz - FL_ZERO
loc_4AFBD,0   ,0x08       ; jz - FL_ZERO

loc_557B4,0x08,0          ; jz - FL_ZERO
loc_5582A,0x08,0          ; jz - FL_ZERO
loc_5582C,0   ,0x08       ; jz - FL_ZERO

loc_56F29,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_56F3E,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_5702E,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57036,0   ,0x20001       ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_57050,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_57054,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_57056,0   ,0x20001       ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_57059,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_5705E,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_570CF,0x20009,0          ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO
loc_570D7,0x20009,0          ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO
loc_570DB,0x20009,0          ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO
loc_570DD,0   ,0x20009       ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO

loc_571A3,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_571A6,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_572C9,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_572CD,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_5732D,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_57330,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_573C6,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_573CF,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_573D3,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_573D8,0x10001,0x20001    ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL, adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_573DD,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_574B2,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_574BF,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_574C1,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_57554,0x08,0          ; jz - FL_ZERO
loc_57556,0x08,0          ; jz - FL_ZERO
loc_57558,0   ,0x08       ; jz - FL_ZERO

loc_577C1,0x08,0          ; jnz - FL_ZERO
loc_577C3,0x08,0          ; jnz - FL_ZERO
loc_577C8,0   ,0x08       ; jnz - FL_ZERO

loc_57821,0x08,0          ; jz - FL_ZERO
loc_57823,0x08,0          ; jz - FL_ZERO
loc_57825,0   ,0x08       ; jz - FL_ZERO

loc_57833,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57839,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_57885,0x08,0          ; jz - FL_ZERO
loc_57887,0x08,0          ; jz - FL_ZERO
loc_5788A,0   ,0x08       ; jz - FL_ZERO

loc_57896,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_5789B,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_57A76,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57A80,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57A84,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57A89,0   ,0x10009       ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_57BD5,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57BD9,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57BDF,0   ,0x10001       ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58DA3,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57BE5,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_57CA2,0x08,0          ; jnz - FL_ZERO
loc_57CA7,0x08,0          ; jnz - FL_ZERO
loc_57CAC,0   ,0x08       ; jnz - FL_ZERO

loc_57DC4,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57DCD,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57DCF,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_57DD5,0   ,0x10009       ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_581C6,0x08,0          ; jz - FL_ZERO
loc_581D0,0x08,0          ; jz - FL_ZERO
loc_581D4,0   ,0x08       ; jz - FL_ZERO

loc_58277,0x08,0          ; jz - FL_ZERO
loc_5827D,0x08,0          ; jz - FL_ZERO
loc_5827F,0x08,0          ; jz - FL_ZERO
loc_58282,0   ,0x08       ; jz - FL_ZERO

loc_582C5,0x08,0          ; jnz - FL_ZERO
loc_582C7,0x08,0          ; jnz - FL_ZERO
loc_582C9,0   ,0x08       ; jnz - FL_ZERO

loc_584E2,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_584F0,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_584F2,0   ,0x20001       ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
