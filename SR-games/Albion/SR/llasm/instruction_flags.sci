loc_17D4E,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_17D54,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_17D80,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_17D86,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_1891A,0x38,0          ; setnle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_18924,0x38,0          ; setnle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_18929,0x38,0          ; setnle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_1892C,0   ,0x38       ; setnle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_6CB8F,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_6CB95,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_6CBB5,0x38,0          ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW
loc_6CBBB,0   ,0x38       ; jle - FL_ZERO | FL_SIGN | FL_OVERFLOW

loc_BF26D,0   ,0x100000   ; jz - FL_INST_Z

loc_BF2D4,0   ,0x100000   ; jz - FL_INST_Z

loc_BF358,0   ,0x100000   ; jz - FL_INST_Z

loc_BF3CA,0   ,0x100000   ; jz - FL_INST_Z

loc_BF445,0   ,0x100000   ; jz - FL_INST_Z

loc_CC38C,0x08,0          ; jz - FL_ZERO
loc_CC402,0x08,0          ; jz - FL_ZERO
loc_CC404,0   ,0x08       ; jz - FL_ZERO

loc_CDB01,0x01,0          ; rcr - FL_CARRY
loc_CDB16,0   ,0x01       ; rcr - FL_CARRY

loc_CDC10,0x01,0          ; adc - FL_CARRY
loc_CDC06,0x01,0          ; adc - FL_CARRY
loc_CDC08,0   ,0x01       ; adc - FL_CARRY
loc_CDC0A,0x01,0          ; adc - FL_CARRY
loc_CDC0C,0   ,0x01       ; adc - FL_CARRY

loc_CDC28,0x01,0          ; adc - FL_CARRY
loc_CDC2C,0x01,0          ; adc - FL_CARRY
loc_CDC2E,0   ,0x01       ; adc - FL_CARRY

loc_CDC34,0x01,0          ; adc - FL_CARRY
loc_CDC36,0   ,0x01       ; adc - FL_CARRY

loc_CDCA7,0x09,0          ; rcr,jz - FL_ZERO | FL_CARRY
loc_CDCAF,0x09,0          ; rcr,jz - FL_ZERO | FL_CARRY
loc_CDCB3,0x09,0          ; rcr,jz - FL_ZERO | FL_CARRY
loc_CDCB5,0   ,0x09       ; jz - FL_ZERO
loc_CDCBA,0x01,0          ; rcr - FL_CARRY
loc_CDCBC,0   ,0x01       ; rcr - FL_CARRY

loc_CDD7B,0x01,0          ; rcr - FL_CARRY
loc_CDD7E,0   ,0x01       ; rcr - FL_CARRY

loc_CDEA3,0x01,0          ; rcr - FL_CARRY
loc_CDEA5,0   ,0x01       ; rcr - FL_CARRY

loc_CDF05,0x01,0          ; rcr - FL_CARRY
loc_CDF08,0   ,0x01       ; rcr - FL_CARRY

loc_CDF9C,0x09,0          ; adc,jnz - FL_ZERO | FL_CARRY
loc_CDF9E,0x01,0x09       ; adc - FL_CARRY, adc,jnz - FL_ZERO | FL_CARRY
loc_CDFA7,0x01,0          ; adc - FL_CARRY
loc_CDFAB,0x01,0          ; adc - FL_CARRY
loc_CDFAD,0   ,0x01       ; adc - FL_CARRY

loc_CDFB3,0x01,0          ; rcr - FL_CARRY
loc_CDFB5,0   ,0x01       ; rcr - FL_CARRY

loc_CE08A,0x01,0          ; rcr - FL_CARRY
loc_CE097,0x01,0          ; rcr - FL_CARRY
loc_CE099,0   ,0x01       ; rcr - FL_CARRY

loc_CE12C,0x08,0          ; jz - FL_ZERO
loc_CE12E,0x08,0          ; jz - FL_ZERO
loc_CE130,0   ,0x08       ; jz - FL_ZERO

loc_CE399,0x08,0          ; jnz - FL_ZERO
loc_CE39B,0x08,0          ; jnz - FL_ZERO
loc_CE3A0,0   ,0x08       ; jnz - FL_ZERO

loc_CE3F9,0x08,0          ; jz - FL_ZERO
loc_CE3FB,0x08,0          ; jz - FL_ZERO
loc_CE3FD,0   ,0x08       ; jz - FL_ZERO

loc_CE40F,0x01,0          ; rcr - FL_CARRY
loc_CE411,0   ,0x01       ; rcr - FL_CARRY

loc_CE45D,0x08,0          ; jz - FL_ZERO
loc_CE45F,0x08,0          ; jz - FL_ZERO
loc_CE462,0   ,0x08       ; jz - FL_ZERO

loc_CE471,0x01,0          ; rcr - FL_CARRY
loc_CE473,0   ,0x01       ; rcr - FL_CARRY

loc_CE64E,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_CE658,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_CE65C,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_CE661,0   ,0x09       ; jbe - FL_ZERO | FL_CARRY

loc_CE7CF,0x01,0          ; adc - FL_CARRY
loc_CE7D1,0x01,0x01       ; adc - FL_CARRY
loc_CE7B7,0x01,0          ; adc - FL_CARRY
loc_CE7BD,0x01,0          ; adc - FL_CARRY
loc_CE7BE,0   ,0x01       ; adc - FL_CARRY

loc_CE7E1,0x01,0          ; adc - FL_CARRY
loc_CE7E3,0   ,0x01       ; adc - FL_CARRY

loc_CE87A,0x08,0          ; jnz - FL_ZERO
loc_CE87F,0x08,0          ; jnz - FL_ZERO
loc_CE884,0   ,0x08       ; jnz - FL_ZERO

loc_CE99C,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_CE9A5,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_CE9A7,0x09,0          ; jbe - FL_ZERO | FL_CARRY
loc_CE9AD,0   ,0x09       ; jbe - FL_ZERO | FL_CARRY

loc_CED9E,0x08,0          ; jz - FL_ZERO
loc_CEDA8,0x08,0          ; jz - FL_ZERO
loc_CEDAC,0   ,0x08       ; jz - FL_ZERO

loc_CEE4F,0x08,0          ; jz - FL_ZERO
loc_CEE55,0x08,0          ; jz - FL_ZERO
loc_CEE57,0x08,0          ; jz - FL_ZERO
loc_CEE5A,0   ,0x08       ; jz - FL_ZERO

loc_CEE9D,0x08,0          ; jnz - FL_ZERO
loc_CEE9F,0x08,0          ; jnz - FL_ZERO
loc_CEEA1,0   ,0x08       ; jnz - FL_ZERO

loc_CF0BA,0x01,0          ; adc - FL_CARRY
loc_CF0C8,0x01,0          ; adc - FL_CARRY
loc_CF0CA,0   ,0x01       ; adc - FL_CARRY

loc_B668A,0x01,0          ; adc - FL_CARRY - self_mod_code_2
