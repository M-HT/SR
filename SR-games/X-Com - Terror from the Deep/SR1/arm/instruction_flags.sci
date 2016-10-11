loc_102DA,0   ,0x08       ; jnz - FL_ZERO

loc_10343,0   ,0x08       ; jnz - FL_ZERO

loc_114D3,0x08,0          ; jz - FL_ZERO
loc_114C4,0x08,0          ; jz - FL_ZERO
loc_114C6,0   ,0x08       ; jz - FL_ZERO

loc_11693,0x08,0          ; jz - FL_ZERO
loc_11684,0x08,0          ; jz - FL_ZERO
loc_11686,0   ,0x08       ; jz - FL_ZERO

loc_131B3,0x08,0          ; jz - FL_ZERO
loc_131D4,0   ,0x08       ; jz - FL_ZERO

loc_1969D,0x08,0          ; jnz - FL_ZERO
loc_196B9,0   ,0x08       ; jnz - FL_ZERO

loc_19930,0x08,0          ; jnz - FL_ZERO
loc_19945,0   ,0x08       ; jnz - FL_ZERO

loc_1A22F,0x08,0          ; jnz - FL_ZERO
loc_1A259,0   ,0x08       ; jnz - FL_ZERO

loc_1E2E8,0   ,0x08       ; jz - FL_ZERO

loc_21DEA,0x08,0          ; setnz - FL_ZERO
loc_21E95,0x08,0          ; setnz - FL_ZERO
loc_22023,0x08,0          ; setnz - FL_ZERO
loc_22027,0   ,0x08       ; setnz - FL_ZERO

loc_268FA,0x08,0          ; jnz - FL_ZERO
loc_26903,0   ,0x08       ; jnz - FL_ZERO

loc_2695A,0   ,0x08       ; jz - FL_ZERO

loc_27380,0   ,0x08       ; jnz - FL_ZERO

loc_27A95,0   ,0x08       ; jz - FL_ZERO

loc_27B9A,0   ,0x08       ; jz - FL_ZERO

loc_27C44,0   ,0x08       ; jz - FL_ZERO

loc_29C16,0   ,0x08       ; jnz - FL_ZERO

loc_2B5AF,0   ,0x08       ; jz - FL_ZERO

loc_32D05,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_32D5B,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_3DB04,0x08,0          ; jnz - FL_ZERO
loc_3DB24,0   ,0x08       ; jnz - FL_ZERO

loc_3FD3E,0   ,0x08       ; jnz - FL_ZERO

loc_480E6,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_480F2,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_49303,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_49310,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_4C083,0x08,0          ; jz - FL_ZERO
loc_4C0E3,0x08,0          ; jz - FL_ZERO
loc_4C0EA,0   ,0x08       ; jz - FL_ZERO

loc_5695E,0x08,0          ; jz - FL_ZERO
loc_569D4,0x08,0          ; jz - FL_ZERO
loc_569D8,0   ,0x08       ; jz - FL_ZERO

loc_580D2,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_580EA,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_581DA,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_581E2,0   ,0x20001       ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_581FA,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_58200,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_58202,0   ,0x20001       ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_58205,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_5820A,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_58277,0x20009,0          ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO
loc_58280,0x20009,0          ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO
loc_58287,0x20009,0          ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO
loc_58289,0   ,0x20009       ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO

loc_5834C,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_58352,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_58475,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58479,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_584D6,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_584DC,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_5856E,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58579,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_5857F,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58584,0x10001,0x20001    ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL, adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_58589,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_5865C,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_5866B,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_5866D,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_586FA,0x08,0          ; jz - FL_ZERO
loc_58702,0x08,0          ; jz - FL_ZERO
loc_58704,0   ,0x08       ; jz - FL_ZERO

loc_58968,0x08,0          ; jnz - FL_ZERO
loc_5896F,0x08,0          ; jnz - FL_ZERO
loc_58974,0   ,0x08       ; jnz - FL_ZERO

loc_589CA,0x08,0          ; jz - FL_ZERO
loc_589CF,0x08,0          ; jz - FL_ZERO
loc_589D1,0   ,0x08       ; jz - FL_ZERO

loc_589DF,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_589E5,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_58A2F,0x08,0          ; jz - FL_ZERO
loc_58A33,0x08,0          ; jz - FL_ZERO
loc_58A36,0   ,0x08       ; jz - FL_ZERO

loc_58A42,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58A47,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_58C1E,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58C27,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58C30,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58C35,0   ,0x10009       ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_58D7F,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58D83,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58D8B,0   ,0x10001       ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58DA1,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58D91,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_58E4B,0x08,0          ; jnz - FL_ZERO
loc_58E53,0x08,0          ; jnz - FL_ZERO
loc_58E58,0   ,0x08       ; jnz - FL_ZERO

loc_58F6A,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58F72,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58F7B,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_58F81,0   ,0x10009       ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_5936E,0x08,0          ; jz - FL_ZERO
loc_5937C,0x08,0          ; jz - FL_ZERO
loc_59380,0   ,0x08       ; jz - FL_ZERO

loc_5941E,0x08,0          ; jz - FL_ZERO
loc_59425,0x08,0          ; jz - FL_ZERO
loc_5942B,0x08,0          ; jz - FL_ZERO
loc_5942E,0   ,0x08       ; jz - FL_ZERO

loc_5946F,0x08,0          ; jnz - FL_ZERO
loc_59473,0x08,0          ; jnz - FL_ZERO
loc_59475,0   ,0x08       ; jnz - FL_ZERO

loc_5968C,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_5969C,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_5969E,0   ,0x20001       ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED

