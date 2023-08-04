loc_115C1,0x08,0          ; jz - FL_ZERO
loc_115B1,0x08,0          ; jz - FL_ZERO
loc_115B3,0   ,0x08       ; jz - FL_ZERO

loc_11787,0x08,0          ; jz - FL_ZERO
loc_11777,0x08,0          ; jz - FL_ZERO
loc_11779,0   ,0x08       ; jz - FL_ZERO

loc_13243,0x08,0          ; jz - FL_ZERO
loc_13262,0   ,0x08       ; jz - FL_ZERO

loc_17F4F,0x08,0          ; jnz - FL_ZERO
loc_17F61,0   ,0x08       ; jnz - FL_ZERO

loc_1883A,0x08,0          ; jnz - FL_ZERO
loc_1885B,0   ,0x08       ; jnz - FL_ZERO

loc_20274,0x08,0          ; setnz - FL_ZERO
loc_20326,0x08,0          ; setnz - FL_ZERO
loc_204B8,0x08,0          ; setnz - FL_ZERO
loc_204BC,0   ,0x08       ; setnz - FL_ZERO

loc_24BD4,0x08,0          ; jnz - FL_ZERO
loc_24BDA,0   ,0x08       ; jnz - FL_ZERO

loc_311AA,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_311FB,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_38571,0x08,0          ; jnz - FL_ZERO
loc_38592,0   ,0x08       ; jnz - FL_ZERO

loc_42308,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_42314,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_43571,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_4357B,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_4DE04,0x08,0          ; jz - FL_ZERO
loc_4DE7A,0x08,0          ; jz - FL_ZERO
loc_4DE7C,0   ,0x08       ; jz - FL_ZERO

loc_4F579,0x01,0          ; rcr - FL_CARRY
loc_4F58E,0   ,0x01       ; rcr - FL_CARRY

loc_4F688,0x01,0          ; adc - FL_CARRY
loc_4F67E,0x01,0          ; adc - FL_CARRY
loc_4F684,0   ,0x01       ; adc - FL_CARRY

loc_4F6A0,0x01,0          ; adc - FL_CARRY
loc_4F6A4,0x01,0          ; adc - FL_CARRY
loc_4F6A6,0   ,0x01       ; adc - FL_CARRY

loc_4F6AC,0x01,0          ; rcr - FL_CARRY
loc_4F6AE,0   ,0x01       ; rcr - FL_CARRY

loc_4F71F,0x09,0          ; jz, rcr - FL_CARRY | FL_ZERO
loc_4F727,0x09,0          ; jz, rcr - FL_CARRY | FL_ZERO
loc_4F72B,0x09,0          ; jz, rcr - FL_CARRY | FL_ZERO
loc_4F72D,0   ,0x09       ; jz, rcr - FL_CARRY | FL_ZERO
loc_4F732,0x01,0          ; rcr - FL_CARRY
loc_4F734,0   ,0x01       ; rcr - FL_CARRY

loc_4F7F3,0x01,0          ; rcr - FL_CARRY
loc_4F7F6,0   ,0x01       ; rcr - FL_CARRY

loc_4F91B,0x01,0          ; rcr - FL_CARRY
loc_4F91D,0   ,0x01       ; rcr - FL_CARRY

loc_4F97D,0x01,0          ; rcr - FL_CARRY
loc_4F980,0   ,0x01       ; rcr - FL_CARRY

loc_4FA14,0x09,0          ; adc - FL_CARRY, jnz - FL_ZERO
loc_4FA16,0x01,0x09       ; adc - FL_CARRY, jnz - FL_ZERO
loc_4FA1F,0x01,0          ; adc - FL_CARRY
loc_4FA23,0x01,0          ; adc - FL_CARRY
loc_4FA25,0   ,0x01       ; adc - FL_CARRY

loc_4FA2B,0x01,0          ; rcr - FL_CARRY
loc_4FA2D,0   ,0x01       ; rcr - FL_CARRY

loc_4FB02,0x01,0          ; rcr - FL_CARRY
loc_4FB0F,0x01,0          ; rcr - FL_CARRY
loc_4FB11,0   ,0x01       ; rcr - FL_CARRY

loc_4FBA4,0x08,0          ; jz - FL_ZERO
loc_4FBA6,0x08,0          ; jz - FL_ZERO
loc_4FBA8,0   ,0x08       ; jz - FL_ZERO

loc_4FE11,0x08,0          ; jnz - FL_ZERO
loc_4FE13,0x08,0          ; jnz - FL_ZERO
loc_4FE18,0   ,0x08       ; jnz - FL_ZERO

loc_4FE71,0x08,0          ; jz - FL_ZERO
loc_4FE73,0x08,0          ; jz - FL_ZERO
loc_4FE75,0   ,0x08       ; jz - FL_ZERO

loc_4FE87,0x01,0          ; rcr - FL_CARRY
loc_4FE89,0   ,0x01       ; rcr - FL_CARRY

loc_4FED5,0x08,0          ; jz - FL_ZERO
loc_4FED7,0x08,0          ; jz - FL_ZERO
loc_4FEDA,0   ,0x08       ; jz - FL_ZERO

loc_4FEE9,0x01,0          ; rcr - FL_CARRY
loc_4FEEB,0   ,0x01       ; rcr - FL_CARRY

loc_500C6,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_500D0,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_500D4,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_500D9,0   ,0x09       ; jbe - FL_ZERO | FL_CARRY

loc_50225,0x01,0          ; adc - FL_CARRY, jnb - FL_CARRY
loc_50229,0x01,0          ; adc - FL_CARRY, jnb - FL_CARRY
loc_5022F,0x01,0x01       ; adc - FL_CARRY, jnb - FL_CARRY
loc_50247,0x01,0          ; adc - FL_CARRY
loc_50235,0x01,0          ; adc - FL_CARRY
loc_50236,0,0x01          ; adc - FL_CARRY

loc_50259,0x01,0          ; adc - FL_CARRY
loc_5025B,0   ,0x01       ; adc - FL_CARRY

loc_502F2,0x08,0          ; jnz - FL_ZERO
loc_502F7,0x08,0          ; jnz - FL_ZERO
loc_502FC,0   ,0x08       ; jnz - FL_ZERO

loc_50414,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_5041D,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_5041F,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_50425,0   ,0x09       ; jbe - FL_ZERO | FL_CARRY

loc_50816,0x08,0          ; jz - FL_ZERO
loc_50820,0x08,0          ; jz - FL_ZERO
loc_50824,0   ,0x08       ; jz - FL_ZERO

loc_508C7,0x08,0          ; jz - FL_ZERO
loc_508CD,0x08,0          ; jz - FL_ZERO
loc_508CF,0x08,0          ; jz - FL_ZERO
loc_508D2,0   ,0x08       ; jz - FL_ZERO

loc_50915,0x08,0          ; jnz - FL_ZERO
loc_50917,0x08,0          ; jnz - FL_ZERO
loc_50919,0   ,0x08       ; jnz - FL_ZERO

loc_50B32,0x01,0          ; adc - FL_CARRY
loc_50B40,0x01,0          ; adc - FL_CARRY
loc_50B42,0   ,0x01       ; adc - FL_CARRY
