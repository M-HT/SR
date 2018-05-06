loc_102DA,0   ,0x08       ; jnz - FL_ZERO

loc_10343,0   ,0x08       ; jnz - FL_ZERO

loc_1058E,0   ,0x08       ; jnz - FL_ZERO

loc_105D3,0   ,0x08       ; jnz - FL_ZERO

loc_10647,0   ,0x10001       ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_115C0,0x08,0          ; jz - FL_ZERO
loc_115B1,0x08,0          ; jz - FL_ZERO
loc_115B3,0   ,0x08       ; jz - FL_ZERO

loc_11786,0x08,0          ; jz - FL_ZERO
loc_11777,0x08,0          ; jz - FL_ZERO
loc_11779,0   ,0x08       ; jz - FL_ZERO

loc_13241,0x08,0          ; jz - FL_ZERO
loc_13262,0   ,0x08       ; jz - FL_ZERO

loc_17F4C,0x08,0          ; jnz - FL_ZERO
loc_17F61,0   ,0x08       ; jnz - FL_ZERO

loc_18836,0x08,0          ; jnz - FL_ZERO
loc_1885B,0   ,0x08       ; jnz - FL_ZERO

loc_1A9F5,0   ,0x08       ; jz - FL_ZERO

loc_1C637,0   ,0x08       ; jz - FL_ZERO

loc_2026F,0x08,0          ; setnz - FL_ZERO
loc_20321,0x08,0          ; setnz - FL_ZERO
loc_204B8,0x08,0          ; setnz - FL_ZERO
loc_204BC,0   ,0x08       ; setnz - FL_ZERO

loc_231A5,0   ,0x08       ; jnz - FL_ZERO

loc_24BD0,0x08,0          ; jnz - FL_ZERO
loc_24BDA,0   ,0x08       ; jnz - FL_ZERO

loc_24C33,0   ,0x08       ; jz - FL_ZERO

loc_25620,0   ,0x08       ; jnz - FL_ZERO

loc_25C9E,0   ,0x08       ; jz - FL_ZERO

loc_25D9F,0   ,0x08       ; jz - FL_ZERO

loc_25E37,0   ,0x08       ; jz - FL_ZERO

loc_25EE2,0   ,0x08       ; jz - FL_ZERO

loc_2820F,0   ,0x08       ; jnz - FL_ZERO

loc_29A9F,0   ,0x08       ; jz - FL_ZERO

loc_311A7,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_311FB,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_3856D,0x08,0          ; jnz - FL_ZERO
loc_38592,0   ,0x08       ; jnz - FL_ZERO

loc_3A624,0   ,0x08       ; jnz - FL_ZERO

loc_42305,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_42314,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_4356D,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_4357B,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_4DE02,0x08,0          ; jz - FL_ZERO
loc_4DE78,0x08,0          ; jz - FL_ZERO
loc_4DE7C,0   ,0x08       ; jz - FL_ZERO

loc_4F576,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_4F58E,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4F67E,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_4F686,0   ,0x20001       ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4F69E,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_4F6A4,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_4F6A6,0   ,0x20001       ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4F6A9,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_4F6AE,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4F71B,0x20009,0          ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO
loc_4F724,0x20009,0          ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO
loc_4F72B,0x20009,0          ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO
loc_4F72D,0   ,0x20009       ; jz, rcr - FL_CARRY | FL_CARRY_SUB_INVERTED | FL_ZERO

loc_4F7F0,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_4F7F6,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4F919,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_4F91D,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4F97A,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_4F980,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4FA12,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_4FA1D,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_4FA23,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_4FA28,0x10001,0x20001    ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL, adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_4FA2D,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4FB00,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_4FB0F,0x20001,0          ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_4FB11,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4FB9E,0x08,0          ; jz - FL_ZERO
loc_4FBA6,0x08,0          ; jz - FL_ZERO
loc_4FBA8,0   ,0x08       ; jz - FL_ZERO

loc_4FE0C,0x08,0          ; jnz - FL_ZERO
loc_4FE13,0x08,0          ; jnz - FL_ZERO
loc_4FE18,0   ,0x08       ; jnz - FL_ZERO

loc_4FE6E,0x08,0          ; jz - FL_ZERO
loc_4FE73,0x08,0          ; jz - FL_ZERO
loc_4FE75,0   ,0x08       ; jz - FL_ZERO

loc_4FE83,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_4FE89,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_4FED3,0x08,0          ; jz - FL_ZERO
loc_4FED7,0x08,0          ; jz - FL_ZERO
loc_4FEDA,0   ,0x08       ; jz - FL_ZERO

loc_4FEE6,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_4FEEB,0   ,0x20001       ; rcr - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_500C2,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_500CB,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_500D4,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_500D9,0   ,0x10009       ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_50223,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_50227,0x10001,0          ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_5022F,0   ,0x10001       ; jnb - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_50245,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_50235,0x10001,0          ; adc (inverted) - FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_502EF,0x08,0          ; jnz - FL_ZERO
loc_502F7,0x08,0          ; jnz - FL_ZERO
loc_502FC,0   ,0x08       ; jnz - FL_ZERO

loc_5040E,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_50416,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_5041F,0x10009,0          ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL
loc_50425,0   ,0x10009       ; jbe - FL_ZERO | FL_CARRY | FL_CARRY_SUB_ORIGINAL

loc_50812,0x08,0          ; jz - FL_ZERO
loc_50820,0x08,0          ; jz - FL_ZERO
loc_50824,0   ,0x08       ; jz - FL_ZERO

loc_508C2,0x08,0          ; jz - FL_ZERO
loc_508C9,0x08,0          ; jz - FL_ZERO
loc_508CF,0x08,0          ; jz - FL_ZERO
loc_508D2,0   ,0x08       ; jz - FL_ZERO

loc_50913,0x08,0          ; jnz - FL_ZERO
loc_50917,0x08,0          ; jnz - FL_ZERO
loc_50919,0   ,0x08       ; jnz - FL_ZERO

loc_50B30,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_50B40,0x20001,0          ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED
loc_50B42,0   ,0x20001       ; adc - FL_CARRY | FL_CARRY_SUB_INVERTED

loc_114C2,0   ,0x08       ; jnz - FL_ZERO

loc_116B0,0   ,0x08       ; jnz - FL_ZERO

