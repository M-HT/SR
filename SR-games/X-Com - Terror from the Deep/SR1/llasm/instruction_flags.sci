loc_114D4,0x08,0          ; jz - FL_ZERO
loc_114C4,0x08,0          ; jz - FL_ZERO
loc_114C6,0   ,0x08       ; jz - FL_ZERO

loc_11694,0x08,0          ; jz - FL_ZERO
loc_11684,0x08,0          ; jz - FL_ZERO
loc_11686,0   ,0x08       ; jz - FL_ZERO

loc_131B5,0x08,0          ; jz - FL_ZERO
loc_131D4,0   ,0x08       ; jz - FL_ZERO

loc_196A0,0x08,0          ; jnz - FL_ZERO
loc_196B9,0   ,0x08       ; jnz - FL_ZERO

loc_19933,0x08,0          ; jnz - FL_ZERO
loc_19945,0   ,0x08       ; jnz - FL_ZERO

loc_1A232,0x08,0          ; jnz - FL_ZERO
loc_1A259,0   ,0x08       ; jnz - FL_ZERO

loc_21DEF,0x08,0          ; setnz - FL_ZERO
loc_21E9A,0x08,0          ; setnz - FL_ZERO
loc_22023,0x08,0          ; setnz - FL_ZERO
loc_22027,0   ,0x08       ; setnz - FL_ZERO

loc_268FD,0x08,0          ; jnz - FL_ZERO
loc_26903,0   ,0x08       ; jnz - FL_ZERO

loc_32D08,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_32D5B,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_3DB07,0x08,0          ; jnz - FL_ZERO
loc_3DB24,0   ,0x08       ; jnz - FL_ZERO

loc_480E9,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_480F2,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_49306,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_49310,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_4C087,0x08,0          ; jz - FL_ZERO
loc_4C0E3,0x08,0          ; jz - FL_ZERO
loc_4C0EA,0   ,0x08       ; jz - FL_ZERO

loc_56960,0x08,0          ; jz - FL_ZERO
loc_569D6,0x08,0          ; jz - FL_ZERO
loc_569D8,0   ,0x08       ; jz - FL_ZERO

loc_580D5,0x01,0          ; rcr - FL_CARRY
loc_580EA,0   ,0x01       ; rcr - FL_CARRY

loc_581E4,0x01,0          ; adc - FL_CARRY
loc_581DA,0x01,0          ; adc - FL_CARRY
loc_581E0,0   ,0x01       ; adc - FL_CARRY

loc_581FC,0x01,0          ; adc - FL_CARRY
loc_58200,0x01,0          ; adc - FL_CARRY
loc_58202,0   ,0x01       ; adc - FL_CARRY

loc_58208,0x01,0          ; rcr - FL_CARRY
loc_5820A,0   ,0x01       ; rcr - FL_CARRY

loc_5827B,0x09,0          ; jz, rcr - FL_CARRY | FL_ZERO
loc_58283,0x09,0          ; jz, rcr - FL_CARRY | FL_ZERO
loc_58287,0x09,0          ; jz, rcr - FL_CARRY | FL_ZERO
loc_58289,0   ,0x09       ; jz, rcr - FL_CARRY | FL_ZERO
loc_5828E,0x01,0          ; rcr - FL_CARRY
loc_58290,0   ,0x01       ; rcr - FL_CARRY

loc_5834F,0x01,0          ; rcr - FL_CARRY
loc_58352,0   ,0x01       ; rcr - FL_CARRY

loc_58477,0x01,0          ; rcr - FL_CARRY
loc_58479,0   ,0x01       ; rcr - FL_CARRY

loc_584D9,0x01,0          ; rcr - FL_CARRY
loc_584DC,0   ,0x01       ; rcr - FL_CARRY

loc_58570,0x09,0          ; adc - FL_CARRY, jnz - FL_ZERO
loc_58572,0x01,0x09       ; adc - FL_CARRY, jnz - FL_ZERO
loc_5857B,0x01,0          ; adc - FL_CARRY
loc_5857F,0x01,0          ; adc - FL_CARRY
loc_58581,0   ,0x01       ; adc - FL_CARRY
loc_58587,0x01,0          ; rcr - FL_CARRY
loc_58589,0   ,0x01       ; rcr - FL_CARRY

loc_5865E,0x01,0          ; rcr - FL_CARRY
loc_5866B,0x01,0          ; rcr - FL_CARRY
loc_5866D,0   ,0x01       ; rcr - FL_CARRY

loc_58700,0x08,0          ; jz - FL_ZERO
loc_58702,0x08,0          ; jz - FL_ZERO
loc_58704,0   ,0x08       ; jz - FL_ZERO

loc_5896D,0x08,0          ; jnz - FL_ZERO
loc_5896F,0x08,0          ; jnz - FL_ZERO
loc_58974,0   ,0x08       ; jnz - FL_ZERO

loc_589CD,0x08,0          ; jz - FL_ZERO
loc_589CF,0x08,0          ; jz - FL_ZERO
loc_589D1,0   ,0x08       ; jz - FL_ZERO

loc_589E3,0x01,0          ; rcr - FL_CARRY
loc_589E5,0   ,0x01       ; rcr - FL_CARRY

loc_58A31,0x08,0          ; jz - FL_ZERO
loc_58A33,0x08,0          ; jz - FL_ZERO
loc_58A36,0   ,0x08       ; jz - FL_ZERO

loc_58A45,0x01,0          ; rcr - FL_CARRY
loc_58A47,0   ,0x01       ; rcr - FL_CARRY

loc_58C22,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_58C2C,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_58C30,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_58C35,0   ,0x09       ; jbe - FL_ZERO | FL_CARRY

loc_58D81,0x01,0          ; jnb - FL_CARRY
loc_58D85,0x01,0          ; jnb - FL_CARRY
loc_58D8B,0x01,0x01       ; jnb - FL_CARRY
loc_58DA3,0x01,0          ; adc - FL_CARRY
loc_58DA5,0x01,0x01       ; adc - FL_CARRY
loc_58D91,0x01,0          ; adc - FL_CARRY
loc_58D92,0   ,0x01       ; adc - FL_CARRY

loc_58DB5,0x01,0          ; adc - FL_CARRY
loc_58DB7,0   ,0x01       ; adc - FL_CARRY

loc_58E4E,0x08,0          ; jnz - FL_ZERO
loc_58E53,0x08,0          ; jnz - FL_ZERO
loc_58E58,0   ,0x08       ; jnz - FL_ZERO

loc_58F70,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_58F79,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_58F7B,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_58F81,0   ,0x09       ; jbe - FL_ZERO | FL_CARRY

loc_59372,0x08,0          ; jz - FL_ZERO
loc_5937C,0x08,0          ; jz - FL_ZERO
loc_59380,0   ,0x08       ; jz - FL_ZERO

loc_59423,0x08,0          ; jz - FL_ZERO
loc_59429,0x08,0          ; jz - FL_ZERO
loc_5942B,0x08,0          ; jz - FL_ZERO
loc_5942E,0   ,0x08       ; jz - FL_ZERO

loc_59471,0x08,0          ; jnz - FL_ZERO
loc_59473,0x08,0          ; jnz - FL_ZERO
loc_59475,0   ,0x08       ; jnz - FL_ZERO

loc_5968E,0x01,0          ; adc - FL_CARRY
loc_5969C,0x01,0          ; adc - FL_CARRY
loc_5969E,0   ,0x01       ; adc - FL_CARRY
