/**
 *
 *  Copyright (C) 2016 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */


#define OUTPUT_PARAMSTRING(x, ...) { sprintf((char *) &(pOutput[strlen(pOutput)]), x, __VA_ARGS__); pOutput += strlen(pOutput); }
#define OUTPUT_STRING(x) { strcat(pOutput, x); pOutput += strlen(pOutput); }


// adc, sbb
// trashes AR_TMPLR, AR_TMP3
static void SR_arm_helper_adc_32(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src2, int32_t value)
{
    const char *updcond, *instr, *instr_nc;
    uint32_t carry_flag2;
    int calculate_flags;

    if (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW))
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Iadc)
    {
        instr = instr_adc;
        instr_nc = instr_add;
        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
    }
    else
    {
        instr = instr_sbc;
        instr_nc = instr_sub;
        carry_flag2 = FL_CARRY_SUB_INVERTED;
    }

    if (src2 != AR_NONE)
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, %s\n", ARMREGSTR(dst), ARMREGSTR(src2));
        }

        OUTPUT_PARAMSTRING("%s%s %s, %s, %s\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(src2));
    }
    else
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, #0x%x\n", ARMREGSTR(dst), value & 0xff);
        }

        if ( SR_disassemble_is_operand2(value) )
        {
            OUTPUT_PARAMSTRING("%s%s %s, %s, #%i\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(dst), value);
        }
        else
        {
            unsigned int len1, len2;

            len1 = SR_get_imm_length((uint32_t) value);
            len2 = SR_get_imm_length(~((uint32_t) value));

            if ( (len1 <= len2) && (len1 <= 3) && (!calculate_flags) )
            {
                int scale;
                uint32_t value2;

                scale = 0;
                value2 = (uint32_t) value;

                // value2 can't be zero here
                while ((value2 & 3) == 0)
                {
                    value2>>=2;
                    scale+=2;
                }

                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                value2>>=8;

                if (value2)
                {
                    scale+=8;
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }

                    OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr_nc, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                    value2>>=8;

                    if (value2)
                    {
                        scale+=8;
                        while ((value2 & 3) == 0)
                        {
                            value2>>=2;
                            scale+=2;
                        }

                        OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr_nc, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                    }
                }
            }
            else
            {
                SR_arm_load_imm_val(AR_TMP3, (uint32_t) value, len1, len2);
                OUTPUT_PARAMSTRING("%s%s %s, %s, tmp3\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(dst));
            }

        }

    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMPLR, 0, mnemonic, Tflags_to_write);
    }

    if (Tflags_to_write & carry_flag2)
    {
        SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
    }
}

// adc, sbb
// trashes AR_TMPLR, AR_TMP3, AR_TMP2, AR_TMP4
static unsigned int SR_arm_helper_adc_16(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src2, int32_t value)
{
    const char *updcond, *instr, *instr_nc, *instr_nc_rev;
    uint32_t carry_flag2;
    int calculate_flags, calc_carry, temp_reg;
    unsigned int res_shift;

#define uvalue ((uint32_t) value)

    if (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW))
    {
        updcond = updatecond;
        calculate_flags = 1;
        calc_carry = Tflags_to_write & FL_CARRY;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
        calc_carry = 0;
    }

    if (mnemonic == UD_Iadc)
    {
        instr = instr_adc;
        instr_nc = instr_add;
        instr_nc_rev = instr_add;
        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
    }
    else
    {
        instr = instr_sbc;
        instr_nc = instr_sub;
        instr_nc_rev = instr_rsb;
        carry_flag2 = FL_CARRY_SUB_INVERTED;
    }

    if (ARMTEMPREG(dst))
    {
        temp_reg = 1;
        res_shift = 16;
    }
    else
    {
        temp_reg = 0;
        res_shift = 0;
    }

    if (src2 != AR_NONE)
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, %s\n", ARMREGSTR(dst), ARMREGSTR(src2));
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;
                OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(src2));
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp3, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(src2));
                OUTPUT_STRING("mov tmp3, tmp3, lsl #16\n");
            }
        }
        else
        {
            if (calc_carry)
            {
                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src2));
                    OUTPUT_STRING("addcsS tmp3, tmp3, #0x10000\n");
                    OUTPUT_STRING("movcs tmp2, #0x20000000\n"); // carry
                    OUTPUT_STRING("movcc tmp2, #0\n");
                    OUTPUT_PARAMSTRING("addS %s, tmp3, %s, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
                }
                else
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(dst));
                    OUTPUT_STRING("subccS tmp3, tmp3, #0x10000\n");
                    OUTPUT_STRING("mvncc tmp2, #0x20000000\n"); // carry
                    OUTPUT_STRING("mvncs tmp2, #0\n");
                    OUTPUT_PARAMSTRING("subS %s, tmp3, %s, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
                }
            }
            else
            {
                OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src2));

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_STRING("addcs tmp3, tmp3, #0x10000\n");
                }
                else
                {
                    OUTPUT_STRING("addcc tmp3, tmp3, #0x10000\n");
                }

                OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #16\n", instr_nc_rev, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
            }
        }
    }
    else
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, #0x%x\n", ARMREGSTR(dst), (value >> 16) & 0xff);
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                int scale;
                uint32_t value2;

                res_shift = 0;
                scale = 0;
                value2 = uvalue >> 16;

                if (value2)
                {
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }
                }

                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                value2>>=8;

                if (value2)
                {
                    scale+=8;
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }

                    OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr_nc, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                }
            }
            else
            {
                int scale;
                uint32_t value2;

                scale = 0;
                value2 = uvalue >> 16;

                if (value2)
                {
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }
                }

                OUTPUT_PARAMSTRING("%s tmp3, %s, #0x%x\n", instr, ARMREGSTR(dst), (value2 & 0xff) << scale);
                value2>>=8;

                if (value2)
                {
                    scale+=8;
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }

                    OUTPUT_PARAMSTRING("%s tmp3, tmp3, #0x%x\n", instr_nc, (value2 & 0xff) << scale);
                }

                OUTPUT_STRING("mov tmp3, tmp3, lsl #16\n");
            }
        }
        else
        {
            int scale;
            uint32_t value2;

            scale = 16;
            value2 = uvalue >> 16;

            if (value2)
            {
                while ((value2 & 3) == 0)
                {
                    value2>>=2;
                    scale+=2;
                }
            }

            OUTPUT_PARAMSTRING("mov tmp3, #0x%x\n", (value2 & 0xff) << scale);
            value2>>=8;

            if (value2)
            {
                scale+=8;
                while ((value2 & 3) == 0)
                {
                    value2>>=2;
                    scale+=2;
                }

                OUTPUT_PARAMSTRING("add tmp3, tmp3, #0x%x\n", (value2 & 0xff) << scale);
            }

            if (calc_carry)
            {
                if (mnemonic == UD_Iadc)
                {
                    if ((uvalue >> 16) == 0xffff)
                    {
                        OUTPUT_STRING("addcsS tmp3, tmp3, #0x10000\n");
                        OUTPUT_STRING("movcs tmp2, #0x20000000\n"); // carry
                        OUTPUT_STRING("movcc tmp2, #0\n");
                    }
                    else
                    {
                        calc_carry = 0;

                        OUTPUT_STRING("addcs tmp3, tmp3, #0x10000\n");
                    }

                    OUTPUT_PARAMSTRING("addS %s, tmp3, %s, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
                }
                else
                {
                    if ((uvalue >> 16) == 0xffff)
                    {
                        OUTPUT_STRING("subccS tmp3, tmp3, #0x10000\n");
                        OUTPUT_STRING("mvncc tmp2, #0x20000000\n"); // carry
                        OUTPUT_STRING("mvncs tmp2, #0\n");
                        OUTPUT_PARAMSTRING("subS %s, tmp3, %s, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
                    }
                    else
                    {
                        calc_carry = 0;

                        OUTPUT_STRING("addcc tmp3, tmp3, #0x10000\n");
                        OUTPUT_PARAMSTRING("rsbS %s, tmp3, %s, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
                    }
                }
            }
            else
            {
                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_STRING("addcs tmp3, tmp3, #0x10000\n");
                }
                else
                {
                    OUTPUT_STRING("addcc tmp3, tmp3, #0x10000\n");
                }

                OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #16\n", instr_nc_rev, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
            }
        }

    }

    if (!temp_reg)
    {
        OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        if (res_shift)
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), res_shift);

            res_shift = 0;
        }
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMPLR, 0, mnemonic, Tflags_to_write);
    }

    if (calc_carry) {
        OUTPUT_STRING("mrs tmp3, cpsr\n");

        if (mnemonic == UD_Iadc)
        {
            OUTPUT_STRING("orr tmp3, tmp3, tmp2\n");
        }
        else
        {
            OUTPUT_STRING("and tmp3, tmp3, tmp2\n");
        }

        if (Tflags_to_write & carry_flag2)
        {
            OUTPUT_STRING("eor tmp3, tmp3, #0x20000000\n"); // carry
        }

        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
    else
    {
        if (Tflags_to_write & carry_flag2)
        {
            SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
        }
    }

    return res_shift;

#undef uvalue
}

// adc, sbb
// trashes AR_TMPLR, AR_TMP3, AR_TMP2, AR_TMP4
static unsigned int SR_arm_helper_adc_8l(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src2, unsigned int src2shiftvalue)
{
    const char *updcond, *instr, *instr_nc_rev;
    uint32_t carry_flag2;
    int calculate_flags, calc_carry, temp_reg;
    unsigned int res_shift;

#define src2shift src2shiftvalue
#define value src2shiftvalue

    if (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW))
    {
        updcond = updatecond;
        calculate_flags = 1;
        calc_carry = Tflags_to_write & FL_CARRY;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
        calc_carry = 0;
    }

    if (mnemonic == UD_Iadc)
    {
        instr = instr_adc;
        instr_nc_rev = instr_add;
        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
    }
    else
    {
        instr = instr_sbc;
        instr_nc_rev = instr_rsb;
        carry_flag2 = FL_CARRY_SUB_INVERTED;
    }

    if (ARMTEMPREG(dst))
    {
        temp_reg = 1;
        res_shift = 24;
    }
    else
    {
        temp_reg = 0;
        res_shift = 0;
    }

    if (src2 != AR_NONE)
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            if (src2shift)
            {
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsr #8\n", ARMREGSTR(dst), ARMREGSTR(src2));
            }
            else
            {
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s\n", ARMREGSTR(dst), ARMREGSTR(src2));
            }
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s, lsr #8\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(src2));
                }
            }
            else
            {
                temp_reg = 1;

                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("%s tmp3, %s, %s, lsr #8\n", instr, ARMREGSTR(dst), ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s tmp3, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(src2));
                }

                OUTPUT_STRING("and tmp3, tmp3, #0x00ff\n");
                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
        else
        {
            if (calc_carry)
            {
                if (mnemonic == UD_Iadc)
                {
                    if (src2shift)
                    {
                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src2));
                        OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src2));
                    }
                    OUTPUT_STRING("addcsS tmp3, tmp3, #0x1000000\n");
                    OUTPUT_STRING("movcs tmp2, #0x20000000\n"); // carry
                    OUTPUT_STRING("movcc tmp2, #0\n");
                    OUTPUT_PARAMSTRING("addS %s, tmp3, %s, lsl #24\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
                }
                else
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(dst));
                    OUTPUT_STRING("subccS tmp3, tmp3, #0x1000000\n");
                    OUTPUT_STRING("mvncc tmp2, #0x20000000\n"); // carry
                    OUTPUT_STRING("mvncs tmp2, #0\n");
                    if (src2shift)
                    {
                        OUTPUT_PARAMSTRING("and tmp4, %s, #0xff00\n", ARMREGSTR(src2));
                        OUTPUT_PARAMSTRING("subS %s, tmp3, tmp4, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3));
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("subS %s, tmp3, %s, lsl #24\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
                    }
                }
            }
            else
            {
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src2));
                    OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                }
                else
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src2));
                }

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_STRING("addcs tmp3, tmp3, #0x1000000\n");
                }
                else
                {
                    OUTPUT_STRING("addcc tmp3, tmp3, #0x1000000\n");
                }

                OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #24\n", instr_nc_rev, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
            }
        }
    }
    else
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, #0x%x\n", ARMREGSTR(dst), (value >> 24) & 0xff);
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;

                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), (value >> 24) & 0xff);
            }
            else
            {
                temp_reg = 1;

                OUTPUT_PARAMSTRING("%s tmp3, %s, #0x%x\n", instr, ARMREGSTR(dst), (value >> 24) & 0xff);

                OUTPUT_STRING("and tmp3, tmp3, #0x00ff\n");
                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
        else
        {
            if (calc_carry)
            {
                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, #0x%x\n", value);
                    if ((value >> 24) == 0xff)
                    {
                        OUTPUT_STRING("addcsS tmp3, tmp3, #0x1000000\n");
                        OUTPUT_STRING("movcs tmp2, #0x20000000\n"); // carry
                        OUTPUT_STRING("movcc tmp2, #0\n");
                    }
                    else
                    {
                        calc_carry = 0;

                        OUTPUT_STRING("addcs tmp3, tmp3, #0x1000000\n");
                    }
                    OUTPUT_PARAMSTRING("addS %s, tmp3, %s, lsl #24\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
                }
                else
                {
                    if ((value >> 24) == 0xff)
                    {
                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(dst));
                        OUTPUT_STRING("subccS tmp3, tmp3, #0x1000000\n");
                        OUTPUT_STRING("mvncc tmp2, #0x20000000\n"); // carry
                        OUTPUT_STRING("mvncs tmp2, #0\n");
                        OUTPUT_PARAMSTRING("subS %s, tmp3, #0x%x\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), value);
                    }
                    else
                    {
                        calc_carry = 0;

                        OUTPUT_PARAMSTRING("mov tmp3, #0x%x\n", value);
                        OUTPUT_STRING("addcc tmp3, tmp3, #0x1000000\n");
                        OUTPUT_PARAMSTRING("rsbS %s, tmp3, %s, lsl #24\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
                    }
                }
            }
            else
            {
                OUTPUT_PARAMSTRING("mov tmp3, #0x%x\n", value);

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_STRING("addcs tmp3, tmp3, #0x1000000\n");
                }
                else
                {
                    OUTPUT_STRING("addcc tmp3, tmp3, #0x1000000\n");
                }

                OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #24\n", instr_nc_rev, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(dst));
            }
        }

    }

    if (!temp_reg)
    {
        OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #24\n", ARMREGSTR(dst), ARMREGSTR(dst));
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        if (res_shift)
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), res_shift);

            res_shift = 0;
        }
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMPLR, 0, mnemonic, Tflags_to_write);
    }

    if (calc_carry) {
        OUTPUT_STRING("mrs tmp3, cpsr\n");

        if (mnemonic == UD_Iadc)
        {
            OUTPUT_STRING("orr tmp3, tmp3, tmp2\n");
        }
        else
        {
            OUTPUT_STRING("and tmp3, tmp3, tmp2\n");
        }

        if (Tflags_to_write & carry_flag2)
        {
            OUTPUT_STRING("eor tmp3, tmp3, #0x20000000\n"); // carry
        }

        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
    else
    {
        if (Tflags_to_write & carry_flag2)
        {
            SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
        }
    }

    return res_shift;

#undef value
#undef src2shift
}

// adc, sbb
// trashes AR_TMPLR, AR_TMP3, AR_TMP2, AR_TMP4
static unsigned int SR_arm_helper_adc_8h(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src2, unsigned int src2shiftvalue)
{
    const char *updcond, *instr, *instr_rev, *instr_nc_rev;
    uint32_t carry_flag2;
    int calculate_flags, calc_carry, temp_reg;
    unsigned int res_shift;

#define src2shift src2shiftvalue
#define value src2shiftvalue

    if (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW))
    {
        updcond = updatecond;
        calculate_flags = 1;
        calc_carry = Tflags_to_write & FL_CARRY;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
        calc_carry = 0;
    }

    if (mnemonic == UD_Iadc)
    {
        instr = instr_adc;
        instr_rev = instr_adc;
        instr_nc_rev = instr_add;
        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
    }
    else
    {
        instr = instr_sbc;
        instr_rev = instr_rsc;
        instr_nc_rev = instr_rsb;
        carry_flag2 = FL_CARRY_SUB_INVERTED;
    }

    if (ARMTEMPREG(dst))
    {
        temp_reg = 1;
        res_shift = 24;
    }
    else
    {
        temp_reg = 0;
        res_shift = 8;
    }

    if (src2 != AR_NONE)
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            if (src2shift)
            {
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s\n", ARMREGSTR(dst), ARMREGSTR(src2));
            }
            else
            {
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(src2));
            }
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsr #8\n", ARMREGSTR(dst));
                    OUTPUT_PARAMSTRING("%s %s, tmp3, %s, lsr #8\n", instr, ARMREGSTR(dst), ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s, lsr #8\n", instr_rev, ARMREGSTR(dst), ARMREGSTR(src2), ARMREGSTR(dst));
                }
            }
            else
            {
                temp_reg = 1;

                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsr #8\n", ARMREGSTR(dst));
                    OUTPUT_PARAMSTRING("%s tmp3, tmp3, %s, lsr #8\n", instr, ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s tmp3, %s, %s, lsr #8\n", instr_rev, ARMREGSTR(src2), ARMREGSTR(dst));
                }

                OUTPUT_STRING("and tmp3, tmp3, #0x00ff\n");
                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
        else
        {
            if (calc_carry)
            {
                if (mnemonic == UD_Iadc)
                {
                    if (src2shift)
                    {
                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src2));
                        OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src2));
                    }
                    OUTPUT_STRING("addcsS tmp3, tmp3, #0x1000000\n");
                    OUTPUT_STRING("movcs tmp4, #0x20000000\n"); // carry
                    OUTPUT_STRING("movcc tmp4, #0\n");
                    OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", ARMREGSTR(dst));
                    OUTPUT_PARAMSTRING("addS %s, tmp3, tmp2, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3));
                }
                else
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(dst));
                    OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                    OUTPUT_STRING("subccS tmp3, tmp3, #0x1000000\n");
                    OUTPUT_STRING("mvncc tmp4, #0x20000000\n"); // carry
                    OUTPUT_STRING("mvncs tmp4, #0\n");
                    if (src2shift)
                    {
                        OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", ARMREGSTR(src2));
                        OUTPUT_PARAMSTRING("subS %s, tmp3, tmp2, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3));
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("subS %s, tmp3, %s, lsl #24\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
                    }
                }
            }
            else
            {
                OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", ARMREGSTR(dst));

                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src2));
                    OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                }
                else
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src2));
                }

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_STRING("addcs tmp3, tmp3, #0x1000000\n");
                }
                else
                {
                    OUTPUT_STRING("addcc tmp3, tmp3, #0x1000000\n");
                }

                OUTPUT_PARAMSTRING("%s%s %s, tmp3, tmp2, lsl #16\n", instr_nc_rev, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3));
            }
        }
    }
    else
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, #0x%x\n", ARMREGSTR(dst), (value >> 16) & 0xff00);
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;

                OUTPUT_PARAMSTRING("mov tmp3, %s, lsr #8\n", ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("%s %s, tmp3, #0x%x\n", instr, ARMREGSTR(dst), (value >> 24) & 0xff);
            }
            else
            {
                temp_reg = 1;

                OUTPUT_PARAMSTRING("mov tmp3, %s, lsr #8\n", ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("%s tmp3, tmp3, #0x%x\n", instr, (value >> 24) & 0xff);

                OUTPUT_STRING("and tmp3, tmp3, #0x00ff\n");
                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
        else
        {
            if (calc_carry)
            {
                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, #0x%x\n", value);
                    if ((value >> 24) == 0xff)
                    {
                        OUTPUT_STRING("addcsS tmp3, tmp3, #0x1000000\n");
                        OUTPUT_STRING("movcs tmp4, #0x20000000\n"); // carry
                        OUTPUT_STRING("movcc tmp4, #0\n");
                    }
                    else
                    {
                        calc_carry = 0;

                        OUTPUT_STRING("addcs tmp3, tmp3, #0x1000000\n");
                    }
                    OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", ARMREGSTR(dst));
                    OUTPUT_PARAMSTRING("addS %s, tmp3, tmp2, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3));
                }
                else
                {
                    if ((value >> 24) == 0xff)
                    {
                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(dst));
                        OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                        OUTPUT_STRING("subccS tmp3, tmp3, #0x1000000\n");
                        OUTPUT_STRING("mvncc tmp4, #0x20000000\n"); // carry
                        OUTPUT_STRING("mvncs tmp4, #0\n");
                        OUTPUT_PARAMSTRING("subS %s, tmp3, #0x%x\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), value);
                    }
                    else
                    {
                        calc_carry = 0;

                        OUTPUT_PARAMSTRING("mov tmp3, #0x%x\n", value);
                        OUTPUT_STRING("addcc tmp3, tmp3, #0x1000000\n");
                        OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", ARMREGSTR(dst));
                        OUTPUT_PARAMSTRING("rsbS %s, tmp3, tmp2, lsl #16\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3));
                    }
                }
            }
            else
            {
                OUTPUT_PARAMSTRING("mov tmp3, #0x%x\n", value);

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_STRING("addcs tmp3, tmp3, #0x1000000\n");
                }
                else
                {
                    OUTPUT_STRING("addcc tmp3, tmp3, #0x1000000\n");
                }

                OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, tmp2, lsl #16\n", instr_nc_rev, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3));
            }
        }

    }

    if (!temp_reg)
    {
        OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        temp_reg = (ARMTEMPREG(dst))?1:0;

        if (temp_reg)
        {
            if (res_shift)
            {
                OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), res_shift);

                res_shift = 0;
            }
        }
        else
        {
            if (res_shift)
            {
                OUTPUT_PARAMSTRING("mov tmp2, %s, lsr #%i\n", ARMREGSTR(dst), res_shift);
            }
        }
        SR_disassemble_calculate_parity_adjust_flags(pOutput, (res_shift == 0)?dst:AR_TMP2, AR_TMPLR, 8, mnemonic, Tflags_to_write);
    }

    if (calc_carry) {
        OUTPUT_STRING("mrs tmp3, cpsr\n");

        if (mnemonic == UD_Iadc)
        {
            OUTPUT_STRING("orr tmp3, tmp3, tmp4\n");
        }
        else
        {
            OUTPUT_STRING("and tmp3, tmp3, tmp4\n");
        }

        if (Tflags_to_write & carry_flag2)
        {
            OUTPUT_STRING("eor tmp3, tmp3, #0x20000000\n"); // carry
        }

        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
    else
    {
        if (Tflags_to_write & carry_flag2)
        {
            SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
        }
    }

    return res_shift;

#undef value
#undef src2shift
}

// add, sub, cmp
// trashes AR_TMPLR, AR_TMP3
static void SR_arm_helper_add_32(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src1, enum arm_regs src2, int32_t value)
{
    const char *updcond, *instr, *instr_r;
    uint32_t carry_flag2;
    int calculate_flags;

    if ( (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW)) || (mnemonic == UD_Icmp) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Iadd)
    {
        instr = instr_add;
        instr_r = instr_sub;
        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
    }
    else
    {
        instr = instr_sub;
        instr_r = instr_add;
        carry_flag2 = FL_CARRY_SUB_INVERTED;
    }

    if (src2 != AR_NONE)
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, %s\n", ARMREGSTR(src1), ARMREGSTR(src2));
        }

        OUTPUT_PARAMSTRING("%s%s %s, %s, %s\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
    }
    else
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, #0x%x\n", ARMREGSTR(src1), value & 0xff);
        }

        if ( SR_disassemble_is_operand2(value) )
        {
            OUTPUT_PARAMSTRING("%s%s %s, %s, #%i\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(src1), value);
        }
        else if ( SR_disassemble_is_operand2(-value) )
        {
            OUTPUT_PARAMSTRING("%s%s %s, %s, #%i\n", instr_r, updcond, ARMREGSTR(dst), ARMREGSTR(src1), -value);
        }
        else
        {
            unsigned int len1, len2;

            len1 = SR_get_imm_length((uint32_t) value);
            len2 = SR_get_imm_length(~((uint32_t) value));

            if ( (len1 <= len2) && (len1 <= 3) && (!calculate_flags) )
            {
                int scale;
                uint32_t value2;

                scale = 0;
                value2 = (uint32_t) value;

                // value2 can't be zero here
                while ((value2 & 3) == 0)
                {
                    value2>>=2;
                    scale+=2;
                }

                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), (value2 & 0xff) << scale);
                value2>>=8;

                if (value2)
                {
                    scale+=8;
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }

                    OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                    value2>>=8;

                    if (value2)
                    {
                        scale+=8;
                        while ((value2 & 3) == 0)
                        {
                            value2>>=2;
                            scale+=2;
                        }

                        OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                    }
                }
            }
            else
            {
                SR_arm_load_imm_val(AR_TMP3, (uint32_t) value, len1, len2);
                OUTPUT_PARAMSTRING("%s%s %s, %s, tmp3\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(src1));
            }

        }

    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMPLR, 0, mnemonic, Tflags_to_write);
    }

    if (Tflags_to_write & carry_flag2)
    {
        SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
    }
}

// add, sub, cmp
// trashes AR_TMPLR, AR_TMP3
static unsigned int SR_arm_helper_add_16(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src1, enum arm_regs src2, int32_t value)
{
    const char *updcond, *instr, *instr_r, *instr_rev;
    uint32_t carry_flag2;
    int calculate_flags, temp_reg;
    unsigned int res_shift;

#define uvalue ((uint32_t) value)

    if ( (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW)) || (mnemonic == UD_Icmp) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Iadd)
    {
        instr = instr_add;
        instr_r = instr_sub;
        instr_rev = instr_add;
        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
    }
    else
    {
        instr = instr_sub;
        instr_r = instr_add;
        instr_rev = instr_rsb;
        carry_flag2 = FL_CARRY_SUB_INVERTED;
    }

    if (ARMTEMPREG(dst))
    {
        temp_reg = 1;
        res_shift = 16;
    }
    else
    {
        temp_reg = 0;
        res_shift = 0;
    }

    if (src2 != AR_NONE)
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, %s\n", ARMREGSTR(src1), ARMREGSTR(src2));
        }

        if (temp_reg && !calculate_flags)
        {
            res_shift = 0;
            OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
            OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #16\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
        }
    }
    else
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, #0x%x\n", ARMREGSTR(src1), (value >> 16) & 0xff);
        }

        if (temp_reg && !calculate_flags)
        {
            int scale;
            uint32_t value2;

            res_shift = 0;
            scale = 0;
            value2 = uvalue >> 16;

            if (value2)
            {
                while ((value2 & 3) == 0)
                {
                    value2>>=2;
                    scale+=2;
                }
            }

            OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), (value2 & 0xff) << scale);
            value2>>=8;

            if (value2)
            {
                scale+=8;
                while ((value2 & 3) == 0)
                {
                    value2>>=2;
                    scale+=2;
                }

                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
            }
        }
        else
        {
            if ( SR_disassemble_is_operand2( (int32_t) (uvalue >> 16) ) )
            {
                OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, #%i\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), value);
            }
            else if ( SR_disassemble_is_operand2( (int32_t) (((uint32_t) (-value)) >> 16) ) )
            {
                OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, #%i\n", instr_r, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), -value);
            }
            else
            {
                unsigned int len1;

                len1 = SR_get_imm_length(uvalue);

                SR_arm_load_imm_orig(AR_TMP3, uvalue, len1);
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #16\n", instr_rev, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src1));
            }
        }
    }

    if (!temp_reg)
    {
        OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        if (res_shift)
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), res_shift);

            res_shift = 0;
        }
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMPLR, 0, mnemonic, Tflags_to_write);
    }

    if (Tflags_to_write & carry_flag2)
    {
        SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
    }

    return res_shift;

#undef uvalue
}

// add, sub, cmp
// trashes AR_TMPLR, AR_TMP3
static unsigned int SR_arm_helper_add_8l(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src1, enum arm_regs src2, unsigned int src2shiftvalue)
{
    const char *updcond, *instr, *instr_rev;
    uint32_t carry_flag2;
    int calculate_flags, temp_reg;
    unsigned int res_shift;

#define src2shift src2shiftvalue
#define value src2shiftvalue

    if ( (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW)) || (mnemonic == UD_Icmp) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Iadd)
    {
        instr = instr_add;
        instr_rev = instr_add;
        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
    }
    else
    {
        instr = instr_sub;
        instr_rev = instr_rsb;
        carry_flag2 = FL_CARRY_SUB_INVERTED;
    }

    if (ARMTEMPREG(dst))
    {
        temp_reg = 1;
        res_shift = 24;
    }
    else
    {
        temp_reg = 0;
        res_shift = 0;
    }

    if (src2 != AR_NONE)
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            if (src2shift)
            {
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsr #8\n", ARMREGSTR(src1), ARMREGSTR(src2));
            }
            else
            {
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s\n", ARMREGSTR(src1), ARMREGSTR(src2));
            }
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s, lsr #8\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
                }
            }
            else
            {
                temp_reg = 1;
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("%s tmp3, %s, %s, lsr #8\n", instr, ARMREGSTR(src1), ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s tmp3, %s, %s\n", instr, ARMREGSTR(src1), ARMREGSTR(src2));
                }
                OUTPUT_PARAMSTRING("bic %s, %s, #0xff\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_STRING("and tmp3, tmp3, #0xff\n");
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
        else
        {
            if (src2shift)
            {
                OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src2));
                OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #24\n", instr_rev, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src1));
            }
            else
            {
                OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src1));
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #24\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
            }
        }

    }
    else
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, #0x%x\n", ARMREGSTR(src1), (value >> 24) & 0xff);
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;
                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), (value >> 24) & 0xff);
            }
            else
            {
                temp_reg = 1;
                OUTPUT_PARAMSTRING("%s tmp3, %s, #0x%x\n", instr, ARMREGSTR(src1), (value >> 24) & 0xff);
                OUTPUT_PARAMSTRING("bic %s, %s, #0xff\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_STRING("and tmp3, tmp3, #0xff\n");
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src1));
            OUTPUT_PARAMSTRING("%s%s %s, tmp3, #0x%x\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), value);
        }
    }

    if (!temp_reg)
    {
        OUTPUT_PARAMSTRING("bic %s, %s, #0xff\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #24\n", ARMREGSTR(dst), ARMREGSTR(dst));
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        if (res_shift)
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), res_shift);

            res_shift = 0;
        }
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMPLR, 0, mnemonic, Tflags_to_write);
    }

    if (Tflags_to_write & carry_flag2)
    {
        SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
    }

    return res_shift;
#undef value
#undef src2shift
}

// add, sub, cmp
// trashes AR_TMPLR, AR_TMP3, AR_TMP2
static unsigned int SR_arm_helper_add_8h(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src1, enum arm_regs src2, unsigned int src2shiftvalue)
{
    const char *updcond, *instr, *instr_rev;
    uint32_t carry_flag2;
    int calculate_flags, temp_reg;
    unsigned int res_shift;

#define src2shift src2shiftvalue
#define value src2shiftvalue

    if ( (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW)) || (mnemonic == UD_Icmp) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Iadd)
    {
        instr = instr_add;
        instr_rev = instr_add;
        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
    }
    else
    {
        instr = instr_sub;
        instr_rev = instr_rsb;
        carry_flag2 = FL_CARRY_SUB_INVERTED;
    }

    if (ARMTEMPREG(dst))
    {
        temp_reg = 1;
        res_shift = 24;
    }
    else
    {
        temp_reg = 0;
        res_shift = 8;
    }

    if (src2 != AR_NONE)
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            if (src2shift)
            {
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s\n", ARMREGSTR(src1), ARMREGSTR(src2));
            }
            else
            {
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsl #8\n", ARMREGSTR(src1), ARMREGSTR(src2));
            }
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsr #8\n", ARMREGSTR(src1));
                    OUTPUT_PARAMSTRING("%s %s, tmp3, %s, lsr #8\n", instr, ARMREGSTR(dst), ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s, lsr #8\n", instr_rev, ARMREGSTR(dst), ARMREGSTR(src2), ARMREGSTR(src1));
                }
            }
            else
            {
                temp_reg = 1;
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsr #8\n", ARMREGSTR(src1));
                    OUTPUT_PARAMSTRING("%s tmp3, tmp3, %s, lsr #8\n", instr, ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s tmp3, %s, %s, lsr #8\n", instr_rev, ARMREGSTR(src2), ARMREGSTR(src1));
                }
                OUTPUT_STRING("and tmp3, tmp3, #0x00ff\n");
                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
        else
        {
            if (src2shift)
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, #0xff00\n", ARMREGSTR(src1));
                OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", ARMREGSTR(src2));
                OUTPUT_STRING("mov tmp3, tmp3, lsl #16\n");
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, tmp2, lsl #16\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3));
            }
            else
            {
                OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
                OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #24\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
            }
        }
    }
    else
    {
        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("eor tmplr, %s, #0x%x\n", ARMREGSTR(src1), (value >> 16) & 0xff00);
        }

        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 8;
                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), (value >> 16) & 0xff00);
            }
            else
            {
                temp_reg = 1;
                OUTPUT_PARAMSTRING("%s tmp3, %s, #0x%x\n", instr, ARMREGSTR(src1), (value >> 16) & 0xff00);
                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_STRING("and tmp3, tmp3, #0xff00\n");
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
            OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
            OUTPUT_PARAMSTRING("%s%s %s, tmp3, #0x%x\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), value);
        }
    }

    if (!temp_reg)
    {
        OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        temp_reg = (ARMTEMPREG(dst))?1:0;

        if (temp_reg)
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), res_shift);

            res_shift = 0;
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmp2, %s, lsr #%i\n", ARMREGSTR(dst), res_shift);
        }
        SR_disassemble_calculate_parity_adjust_flags(pOutput, (temp_reg)?dst:AR_TMP2, AR_TMPLR, 8, mnemonic, Tflags_to_write);
    }

    if (Tflags_to_write & carry_flag2)
    {
        SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
    }

    return res_shift;
#undef value
#undef src2shift
}

// and, or, xor, test
// trashes AR_TMP3
static void SR_arm_helper_and_32(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src1, enum arm_regs src2, int32_t value)
{
    const char *updcond, *instr, *instr_inv;

    if ( (Tflags_to_write & (FL_ZERO | FL_SIGN)) || (mnemonic == UD_Itest) )
    {
        updcond = updatecond;
    }
    else
    {
        updcond = empty;
    }

    if (mnemonic == UD_Ior)
    {
        instr = instr_orr;
        instr_inv = NULL;
    }
    else if (mnemonic == UD_Ixor)
    {
        instr = instr_eor;
        instr_inv = NULL;
    }
    else
    {
        instr = instr_and;
        instr_inv = instr_bic;
    }

    if (src2 != AR_NONE)
    {
        OUTPUT_PARAMSTRING("%s%s %s, %s, %s\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
    }
    else
    {
        if ( SR_disassemble_is_operand2(value) )
        {
            OUTPUT_PARAMSTRING("%s%s %s, %s, #%i\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(src1), value);
        }
        else
        {
            unsigned int len1, len2, lenmin;

            len1 = SR_get_imm_length((uint32_t) value);
            len2 = SR_get_imm_length(~((uint32_t) value));

            if ( ((instr_inv == NULL) && (len1 <= len2) && (len1 <= 3)) || ((instr_inv != NULL) && (len2 <= len1) && (len2 <= 3)) )
            {
                int scale;
                uint32_t value2;

                scale = 0;
                if (instr_inv == NULL)
                {
                    lenmin = len1;
                    value2 = (uint32_t) value;
                }
                else
                {
                    lenmin = len2;
                    value2 = ~((uint32_t) value);
                    instr = instr_inv;
                }

                if (value2)
                {
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }
                }

                lenmin--;
                OUTPUT_PARAMSTRING("%s%s %s, %s, #0x%x\n", instr, (lenmin)?empty:updcond, ARMREGSTR(dst), ARMREGSTR(src1), (value2 & 0xff) << scale);
                value2>>=8;

                if (value2)
                {
                    scale+=8;
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }

                    lenmin--;
                    OUTPUT_PARAMSTRING("%s%s %s, %s, #0x%x\n", instr, (lenmin)?empty:updcond, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                    value2>>=8;

                    if (value2)
                    {
                        scale+=8;
                        while ((value2 & 3) == 0)
                        {
                            value2>>=2;
                            scale+=2;
                        }

                        OUTPUT_PARAMSTRING("%s%s %s, %s, #0x%x\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                    }
                }
            }
            else
            {
                SR_arm_load_imm_val(AR_TMP3, (uint32_t) value, len1, len2);
                OUTPUT_PARAMSTRING("%s%s %s, %s, tmp3\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(src1));
            }

        }

    }

    if (Tflags_to_write & FL_PARITY)
    {
        SR_disassemble_calculate_parity_flag(pOutput, dst, AR_TMP3, 0);
    }

    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
    {
        if (Tflags_to_write & FL_CARRY_SUB_INVERTED)
        {
            SR_disassemble_change_arm_flags(pOutput, FL_CARRY | FL_OVERFLOW, 0, 0, AR_TMP3);
        }
        else
        {
            SR_disassemble_change_arm_flags(pOutput, Tflags_to_write & FL_OVERFLOW, Tflags_to_write & FL_CARRY, 0, AR_TMP3);
        }
    }
}

// and, or, xor, test
// trashes AR_TMP3
static unsigned int SR_arm_helper_and_16(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src1, enum arm_regs src2, int32_t value)
{
    const char *updcond, *instr, *instr_inv;
    int calculate_flags, temp_reg;
    unsigned int res_shift;

#define uvalue ((uint32_t) value)

    if ( (Tflags_to_write & (FL_ZERO | FL_SIGN)) || (mnemonic == UD_Itest) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Ior)
    {
        instr = instr_orr;
        instr_inv = NULL;
    }
    else if (mnemonic == UD_Ixor)
    {
        instr = instr_eor;
        instr_inv = NULL;
    }
    else
    {
        instr = instr_and;
        instr_inv = instr_bic;
    }

    if (ARMTEMPREG(dst))
    {
        temp_reg = 1;
        res_shift = 16;
    }
    else
    {
        temp_reg = 0;
        res_shift = 0;
    }

    if (src2 != AR_NONE)
    {
        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;
                OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
            }
            else
            {
                temp_reg = 1;
                if (instr_inv == NULL)
                {
                    if (ARMTEMPREG(src2)) // upper 16 bits of src2 are zeros
                    {
                        OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(src2));
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src2));
                        OUTPUT_PARAMSTRING("%s %s, %s, tmp3, lsr #16\n", instr, ARMREGSTR(dst), ARMREGSTR(dst));
                    }
                }
                else
                {
                    OUTPUT_PARAMSTRING("mvn tmp3, %s, lsl #16\n", ARMREGSTR(src2));
                    OUTPUT_PARAMSTRING("%s %s, %s, tmp3, lsr #16\n", instr_inv, ARMREGSTR(dst), ARMREGSTR(dst));
                }
            }
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
            OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #16\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
        }
    }
    else
    {
        if ( /*( temp_reg || (dst == src1) ) &&*/ !calculate_flags) // the first condition should be always true
        {
            res_shift = 0;
            if ( temp_reg && SR_disassemble_is_operand2( (int32_t) (((uint32_t) value) >> 16) ) )
            {
                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ((uint32_t) value) >> 16);
            }
            else
            {
                int scale;
                uint32_t value2;

                temp_reg = 1;
                scale = 0;
                if (instr_inv == NULL)
                {
                    value2 = ((uint32_t) value) >> 16;
                }
                else
                {
                    value2 = (~((uint32_t) value)) >> 16;
                    instr = instr_inv;
                }

                if (value2)
                {
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }
                }

                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), (value2 & 0xff) << scale);
                value2>>=8;

                if (value2)
                {
                    scale+=8;
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }

                    OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
                }
            }
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));

            if ( SR_disassemble_is_operand2( (int32_t) (uvalue >> 16) ) )
            {
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, #0x%x\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), uvalue);
            }
            else
            {
                int scale;
                uint32_t value2;

                scale = 16;
                if (instr_inv == NULL)
                {
                    value2 = uvalue >> 16;
                }
                else
                {
                    value2 = (~uvalue) >> 16;
                    instr = instr_inv;
                }

                if (value2)
                {
                    while ((value2 & 3) == 0)
                    {
                        value2>>=2;
                        scale+=2;
                    }
                }

                OUTPUT_PARAMSTRING("%s%s %s, tmp3, #0x%x\n", instr, (value2 >> 8)?empty:updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), (value2 & 0xff) << scale);
                value2>>=8;

                if (value2)
                {
                    scale+=8;
                    while ((value2 & 3) == 0) {
                        value2>>=2;
                        scale+=2;
                    }

                    OUTPUT_PARAMSTRING("%s%s %s, %s, #0x%x\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), (value2 & 0xff) << scale);
                }

            }

        }

    }

    if (!temp_reg)
    {
        if ( (mnemonic == UD_Ixor) || (dst != src1) || (src1 != src2) )
        {
            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
    }

    if (Tflags_to_write & FL_PARITY)
    {
        if (res_shift)
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), res_shift);

            res_shift = 0;
        }
        SR_disassemble_calculate_parity_flag(pOutput, dst, AR_TMP3, 0);
    }

    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
    {
        if (Tflags_to_write & FL_CARRY_SUB_INVERTED)
        {
            SR_disassemble_change_arm_flags(pOutput, FL_CARRY | FL_OVERFLOW, 0, 0, AR_TMP3);
        }
        else
        {
            SR_disassemble_change_arm_flags(pOutput, Tflags_to_write & FL_OVERFLOW, Tflags_to_write & FL_CARRY, 0, AR_TMP3);
        }
    }

    return res_shift;

#undef uvalue
}

// and, or, xor, test
// trashes AR_TMP3
static unsigned int SR_arm_helper_and_8l(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src1, enum arm_regs src2, unsigned int src2shiftvalue)
{
    const char *updcond, *instr, *instr_inv;
    int calculate_flags, temp_reg;
    unsigned int res_shift;

#define src2shift src2shiftvalue
#define value src2shiftvalue

    if ( (Tflags_to_write & (FL_ZERO | FL_SIGN)) || (mnemonic == UD_Itest) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Ior)
    {
        instr = instr_orr;
        instr_inv = NULL;
    }
    else if (mnemonic == UD_Ixor)
    {
        instr = instr_eor;
        instr_inv = NULL;
    }
    else
    {
        instr = instr_and;
        instr_inv = instr_bic;
    }

    if (ARMTEMPREG(dst))
    {
        temp_reg = 1;
        res_shift = 24;
    }
    else
    {
        temp_reg = 0;
        res_shift = 0;
    }

    if (src2 != AR_NONE)
    {
        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 0;
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s, lsr #8\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
                }
            }
            else
            {
                temp_reg = 1;
                if (instr_inv == NULL)
                {
                    if (src2shift)
                    {
                        OUTPUT_PARAMSTRING("and tmp3, %s, #0xff00\n", ARMREGSTR(src2));
                        OUTPUT_PARAMSTRING("%s %s, %s, tmp3, lsr #8\n", instr, ARMREGSTR(dst), ARMREGSTR(src1));
                    }
                    else
                    {
                        if (ARMTEMPREG(src2)) // upper 24 bits of src2 are zeros
                        {
                            OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
                        }
                        else
                        {
                            if (mnemonic == UD_Ixor && (src1 == src2))
                            {
                                OUTPUT_PARAMSTRING("bic %s, %s, #0xff\n", ARMREGSTR(dst), ARMREGSTR(dst));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("and tmp3, %s, #0x00ff\n", ARMREGSTR(src2));
                                OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, ARMREGSTR(dst), ARMREGSTR(src1));
                            }
                        }
                    }
                }
                else
                {
                    OUTPUT_PARAMSTRING("mvn tmp3, %s, lsl #%i\n", ARMREGSTR(src2), 24 - src2shift);
                    OUTPUT_PARAMSTRING("%s %s, %s, tmp3, lsr #24\n", instr_inv, ARMREGSTR(dst), ARMREGSTR(src1));
                }
            }
        }
        else
        {
            if (src2shift)
            {
                if (instr_inv == NULL)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src2));
                    OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                    OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #24\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src1));
                }
                else // and, test
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src1));
                    OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #16\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
                }
            }
            else
            {
                OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src1));
                OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #24\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
            }
        }
    }
    else
    {
        if (!calculate_flags)
        {
            temp_reg = 1;
            res_shift = 0;
            if (instr_inv == NULL)
            {
                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), (value >> 24) & 0xff);
            }
            else
            {
                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr_inv, ARMREGSTR(dst), ARMREGSTR(src1), ((~value) >> 24) & 0xff);
            }
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src1));
            OUTPUT_PARAMSTRING("%s%s %s, tmp3, #0x%x\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), value);
        }
    }

    if (!temp_reg)
    {
        if ( (mnemonic == UD_Ixor) || (dst != src1) || (src1 != src2) || (src2shift != 0) )
        {
            OUTPUT_PARAMSTRING("bic %s, %s, #0xff\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #24\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
    }

    if (Tflags_to_write & FL_PARITY)
    {
        if (res_shift)
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), res_shift);

            res_shift = 0;
        }
        SR_disassemble_calculate_parity_flag(pOutput, dst, AR_TMP3, 0);
    }

    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
    {
        if (Tflags_to_write & FL_CARRY_SUB_INVERTED)
        {
            SR_disassemble_change_arm_flags(pOutput, FL_CARRY | FL_OVERFLOW, 0, 0, AR_TMP3);
        }
        else
        {
            SR_disassemble_change_arm_flags(pOutput, Tflags_to_write & FL_OVERFLOW, Tflags_to_write & FL_CARRY, 0, AR_TMP3);
        }
    }

    return res_shift;

#undef src2shift
#undef value
}

// and, or, xor, test
// trashes AR_TMPLR, AR_TMP3
static unsigned int SR_arm_helper_and_8h(enum ud_mnemonic_code mnemonic, enum arm_regs dst, enum arm_regs src1, enum arm_regs src2, unsigned int src2shiftvalue)
{
    const char *updcond, *instr, *instr_inv;
    int calculate_flags, temp_reg;
    unsigned int res_shift;

#define src2shift src2shiftvalue
#define value src2shiftvalue

    if ( (Tflags_to_write & (FL_ZERO | FL_SIGN)) || (mnemonic == UD_Itest) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Ior)
    {
        instr = instr_orr;
        instr_inv = NULL;
    }
    else if (mnemonic == UD_Ixor)
    {
        instr = instr_eor;
        instr_inv = NULL;
    }
    else
    {
        instr = instr_and;
        instr_inv = instr_bic;
    }

    if (ARMTEMPREG(dst))
    {
        temp_reg = 1;
        res_shift = 24;
    }
    else
    {
        temp_reg = 0;
        res_shift = 8;
    }

    if (src2 != AR_NONE)
    {
        if (!calculate_flags)
        {
            if (temp_reg)
            {
                res_shift = 8;
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, %s, lsl #8\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
                }
            }
            else
            {
                temp_reg = 1;

                if (src2shift)
                {
                    if (instr_inv == NULL)
                    {
                        if (mnemonic == UD_Ixor && (src1 == src2))
                        {
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("and tmp3, %s, #0xff00\n", ARMREGSTR(src2));
                            OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, ARMREGSTR(dst), ARMREGSTR(src1));
                        }
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("mvn tmp3, %s\n", ARMREGSTR(src2));
                        OUTPUT_STRING("and tmp3, tmp3, #0xff00\n");
                        OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr_inv, ARMREGSTR(dst), ARMREGSTR(src1));
                    }
                }
                else
                {
                    if (instr_inv == NULL)
                    {
                        if (ARMTEMPREG(src2)) // upper 24 bits of src2 are zeros
                        {
                            OUTPUT_PARAMSTRING("%s %s, %s, %s, lsl #8\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), ARMREGSTR(src2));
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("and tmp3, %s, #0x00ff\n", ARMREGSTR(src2));
                            OUTPUT_PARAMSTRING("%s %s, %s, tmp3, lsl #8\n", instr, ARMREGSTR(dst), ARMREGSTR(src1));
                        }
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("mvn tmp3, %s, lsl #8\n", ARMREGSTR(src2));
                        OUTPUT_STRING("and tmp3, tmp3, #0xff00\n");
                        OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr_inv, ARMREGSTR(dst), ARMREGSTR(src1));
                    }
                }
            }
        }
        else
        {
            if (src2shift)
            {
                if (instr_inv == NULL)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
                    OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                    OUTPUT_PARAMSTRING("and tmplr, %s, #0xff00\n", ARMREGSTR(src2));
                    OUTPUT_PARAMSTRING("%s%s %s, tmp3, tmplr, lsl #16\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3));
                }
                else // and, test
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
                    OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                    OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #16\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
                }
            }
            else
            {
                if (instr_inv == NULL)
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
                    OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                    OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #24\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src2));
                }
                else // and, test
                {
                    OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #24\n", ARMREGSTR(src2));
                    OUTPUT_PARAMSTRING("%s%s %s, tmp3, %s, lsl #16\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), ARMREGSTR(src1));
                }
            }
        }
    }
    else
    {
        if (!calculate_flags)
        {
            temp_reg = 1;
            res_shift = 8;
            if (instr_inv == NULL)
            {
                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr, ARMREGSTR(dst), ARMREGSTR(src1), (value >> 16) & 0xff00);
            }
            else
            {
                OUTPUT_PARAMSTRING("%s %s, %s, #0x%x\n", instr_inv, ARMREGSTR(dst), ARMREGSTR(src1), ((~value) >> 16) & 0xff00);
            }
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(src1));
            if (instr_inv == NULL)
            {
                OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
            }
            OUTPUT_PARAMSTRING("%s%s %s, tmp3, #0x%x\n", instr, updcond, (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP3), value);
        }
    }

    if (!temp_reg)
    {
        if ( (mnemonic == UD_Ixor) || (dst != src1) || (src1 != src2) || (src2shift == 0) )
        {
            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
    }

    if (Tflags_to_write & FL_PARITY)
    {
        temp_reg = ARMTEMPREG(dst);

        if (temp_reg)
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), res_shift);

            res_shift = 0;
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmplr, %s, lsr #%i\n", ARMREGSTR(dst), res_shift);
        }
        SR_disassemble_calculate_parity_flag(pOutput, (temp_reg)?dst:AR_TMPLR, AR_TMP3, 8);
    }

    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
    {
        if (Tflags_to_write & FL_CARRY_SUB_INVERTED)
        {
            SR_disassemble_change_arm_flags(pOutput, FL_CARRY | FL_OVERFLOW, 0, 0, AR_TMP3);
        }
        else
        {
            SR_disassemble_change_arm_flags(pOutput, Tflags_to_write & FL_OVERFLOW, Tflags_to_write & FL_CARRY, 0, AR_TMP3);
        }
    }

    return res_shift;

#undef src2shift
#undef value
}

// dec, inc
// trashes AR_TMPLR, AR_TMP3
static void SR_arm_helper_dec_32(enum ud_mnemonic_code mnemonic, enum arm_regs dst)
{
    const char *updcond, *instr;
    int calculate_flags;

    if ( Tflags_to_write & (FL_ZERO | FL_SIGN | FL_OVERFLOW) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Iinc)
    {
        instr = instr_add;
    }
    else
    {
        instr = instr_sub;
    }

    if (calculate_flags && (Tflags_to_write & FL_CARRY))
    {
        // save carry flag
        OUTPUT_STRING("mrs tmplr, cpsr\n");
    }

    OUTPUT_PARAMSTRING("%s%s %s, %s, #1\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(dst));

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMP3, 0, mnemonic, Tflags_to_write);
    }

    if (calculate_flags && (Tflags_to_write & FL_CARRY))
    {
        // restore carry flag
        OUTPUT_STRING("mrs tmp3, cpsr\n");
        OUTPUT_STRING("and tmplr, tmplr, #0x20000000\n");
        OUTPUT_STRING("bic tmp3, tmp3, #0x20000000\n");
        OUTPUT_STRING("orr tmp3, tmp3, tmplr\n");
        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
}

// dec, inc
// trashes AR_TMPLR, AR_TMP3
static void SR_arm_helper_dec_16(enum ud_mnemonic_code mnemonic, enum arm_regs dst)
{
    const char *updcond, *instr;
    int calculate_flags;

    if ( Tflags_to_write & (FL_ZERO | FL_SIGN | FL_OVERFLOW) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Iinc)
    {
        instr = instr_add;
    }
    else
    {
        instr = instr_sub;
    }

    if (calculate_flags && (Tflags_to_write & FL_CARRY))
    {
        // save carry flag
        OUTPUT_STRING("mrs tmplr, cpsr\n");
    }

    if (ARMTEMPREG(dst))
    {
        if (!calculate_flags)
        {
            OUTPUT_PARAMSTRING("%s%s %s, %s, #1\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsl #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("%s%s %s, %s, #0x10000\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
    }
    else
    {
        if (calculate_flags || (Tflags_to_write & FL_CARRY) || (mnemonic == UD_Iinc))
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("%s%s tmp3, tmp3, #0x10000\n", instr, updcond);
            OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("movS tmp3, %s, lsl #16\n", ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("orreq %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("orreq %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("subne %s, %s, #1\n", ARMREGSTR(dst), ARMREGSTR(dst));        // mnemonic == UD_Idec
        }
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMP3, 0, mnemonic, Tflags_to_write);
    }

    if (calculate_flags && (Tflags_to_write & FL_CARRY))
    {
        // restore carry flag
        OUTPUT_STRING("mrs tmp3, cpsr\n");
        OUTPUT_STRING("and tmplr, tmplr, #0x20000000\n");
        OUTPUT_STRING("bic tmp3, tmp3, #0x20000000\n");
        OUTPUT_STRING("orr tmp3, tmp3, tmplr\n");
        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
}

// dec, inc
// trashes AR_TMPLR, AR_TMP3, AR_TMP2
static void SR_arm_helper_dec_8(enum ud_mnemonic_code mnemonic, enum arm_regs dst, unsigned int dstshift)
{
    const char *updcond, *instr;
    int calculate_flags;

    if ( Tflags_to_write & (FL_ZERO | FL_SIGN | FL_OVERFLOW) )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (mnemonic == UD_Iinc)
    {
        instr = instr_add;
    }
    else
    {
        instr = instr_sub;
    }

    if (calculate_flags && (Tflags_to_write & FL_CARRY))
    {
        // save carry flag
        OUTPUT_STRING("mrs tmplr, cpsr\n");
    }

    if (!dstshift && ARMTEMPREG(dst))
    {
        if (!calculate_flags)
        {
            OUTPUT_PARAMSTRING("%s%s %s, %s, #1\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsl #24\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("%s%s %s, %s, #0x1000000\n", instr, updcond, ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #24\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
    }
    else
    {
        if (calculate_flags || (Tflags_to_write & FL_CARRY) || (mnemonic == UD_Iinc))
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #%i\n", ARMREGSTR(dst), 24 - dstshift);
            if (dstshift)
            {
                OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
            }
            OUTPUT_PARAMSTRING("bic %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), 0xff << dstshift);
            OUTPUT_PARAMSTRING("%s%s tmp3, tmp3, #0x1000000\n", instr, updcond);
            OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), 24 - dstshift);
        }
        else
        {
            OUTPUT_PARAMSTRING("tst %s, #0x%x\n", ARMREGSTR(dst), 0xff << dstshift);
            OUTPUT_PARAMSTRING("orreq %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), 0xff << dstshift);
            OUTPUT_PARAMSTRING("subne %s, %s, #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), 1 << dstshift);        // mnemonic == UD_Idec
        }
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        if (dstshift)
        {
            OUTPUT_STRING("mov tmp2, dst, lsr #8\n");
        }
        SR_disassemble_calculate_parity_adjust_flags(pOutput, (dstshift)?AR_TMP2:dst, AR_TMP3, 0, mnemonic, Tflags_to_write);
    }

    if (calculate_flags && (Tflags_to_write & FL_CARRY))
    {
        // restore carry flag
        OUTPUT_STRING("mrs tmp3, cpsr\n");
        OUTPUT_STRING("and tmplr, tmplr, #0x20000000\n");
        OUTPUT_STRING("bic tmp3, tmp3, #0x20000000\n");
        OUTPUT_STRING("orr tmp3, tmp3, tmplr\n");
        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
}

// neg
// trashes AR_TMP3
static void SR_arm_helper_neg_32(enum arm_regs dst)
{
    const char *updcond;

    if (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW))
    {
        updcond = updatecond;
    }
    else
    {
        updcond = empty;
    }

    OUTPUT_PARAMSTRING("rsb%s %s, %s, #0\n", updcond, ARMREGSTR(dst), ARMREGSTR(dst));

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMP3, 0, UD_Ineg, Tflags_to_write);
    }

    if (Tflags_to_write & FL_CARRY_SUB_INVERTED)
    {
        SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
    }
}

// neg
// trashes AR_TMP3
static void SR_arm_helper_neg_16(enum arm_regs dst)
{
    const char *updcond;
    int calculate_flags;

    if (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW))
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (ARMTEMPREG(dst))
    {
        if (!calculate_flags)
        {
            OUTPUT_PARAMSTRING("rsb %s, %s, #0\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsl #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("rsb%s %s, %s, #0\n", updcond, ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
    }
    else
    {
        OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("rsb%s tmp3, tmp3, #0\n", updcond);
        OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        SR_disassemble_calculate_parity_adjust_flags(pOutput, dst, AR_TMP3, 0, UD_Ineg, Tflags_to_write);
    }

    if (Tflags_to_write & FL_CARRY_SUB_INVERTED)
    {
        SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
    }
}

// neg
// trashes AR_TMPLR, AR_TMP3
static void SR_arm_helper_neg_8(enum arm_regs dst, unsigned int dstshift)
{
    const char *updcond;
    int calculate_flags;

    if (Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW))
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (!dstshift && ARMTEMPREG(dst))
    {
        if (!calculate_flags)
        {
            OUTPUT_PARAMSTRING("rsb %s, %s, #0\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("mov %s, %s, lsl #24\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("rsb%s %s, %s, #0\n", updcond, ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #24\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
    }
    else
    {
        OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #%i\n", ARMREGSTR(dst), 24 - dstshift);
        if (dstshift)
        {
            OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
        }
        OUTPUT_PARAMSTRING("rsb%s tmp3, tmp3, #0\n", updcond);
        OUTPUT_PARAMSTRING("bic %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), 0xff << dstshift);
        OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), 24 - dstshift);
    }

    if (Tflags_to_write & (FL_PARITY | FL_ADJUST))
    {
        if (dstshift)
        {
            OUTPUT_STRING("mov tmplr, tmp3, lsr #24\n");
        }
        SR_disassemble_calculate_parity_adjust_flags(pOutput, (dstshift)?AR_TMPLR:dst, AR_TMP3, 0, UD_Ineg, Tflags_to_write);
    }

    if (Tflags_to_write & FL_CARRY_SUB_INVERTED)
    {
        SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
    }
}

// rol, ror
// trashes AR_TMPLR, AR_TMP3
static void SR_arm_helper_rol_32(enum ud_mnemonic_code mnemonic, enum arm_regs dst, uint32_t value)
{
    int calculate_overflow, calculate_carry, rotate_right;
    uint32_t value2;

    if (value == 0) return;

    if (mnemonic == UD_Iror)
    {
        rotate_right = 1;
        value2 = value;
    }
    else
    {
        rotate_right = 0;
        value2 = 32 - value;
    }

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        calculate_overflow = 1;
    }
    else
    {
        calculate_overflow = 0;
    }

    if (Tflags_to_write & FL_CARRY)
    {
        calculate_carry = 1;
    }
    else
    {
        calculate_carry = 0;
    }

    OUTPUT_PARAMSTRING("mov %s, %s, ror #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), value2);

    if (calculate_carry || calculate_overflow)
    {
        OUTPUT_STRING("mrs tmp3, cpsr\n");

        if (calculate_carry)
        {
            if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
            {
                OUTPUT_STRING("orr tmp3, tmp3, #0x20000000\n");
            }
            else
            {
                OUTPUT_STRING("bic tmp3, tmp3, #0x30000000\n");
            }

            if (rotate_right)
            {
                // CF = MSB(modified reg)
                OUTPUT_PARAMSTRING("mov tmplr, %s, lsr #31\n", ARMREGSTR(dst));
            }
            else
            {
                // CF = LSB(modified reg)
                OUTPUT_PARAMSTRING("and tmplr, %s, #1\n", ARMREGSTR(dst));
            }

            if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
            {
                OUTPUT_STRING("eor tmp3, tmp3, tmplr, lsl #29\n");
            }
            else
            {
                OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsl #29\n");
            }
        }

        if (calculate_overflow)
        {
            if ((Tflags_to_write & FL_CARRY_SUB_ORIGINAL) || !calculate_carry)
            {
                OUTPUT_STRING("bic tmp3, tmp3, #0x10000000\n");
            }

            if (rotate_right)
            {
                // OF = MSB-1(modified reg) XOR CF (CF = MSB(modified reg))
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsl #1\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_STRING("and tmplr, tmplr, #0x80000000\n");
                OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsr #3\n");
            }
            else
            {
                // OF = MSB(modified reg) XOR CF (CF = LSB(modified reg))
                if (calculate_carry)
                {
                    OUTPUT_PARAMSTRING("eor tmplr, tmplr, %s, lsr #31\n", ARMREGSTR(dst));
                    OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsl #28\n");
                }
                else
                {
                    OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsl #31\n", ARMREGSTR(dst), ARMREGSTR(dst));
                    OUTPUT_STRING("and tmplr, tmplr, #0x80000000\n");
                    OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsr #3\n");
                }
            }
        }

        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
}

// rol, ror
// trashes AR_TMPLR, AR_TMP3, AR_TMP2
static void SR_arm_helper_rol_16(enum ud_mnemonic_code mnemonic, enum arm_regs dst, uint32_t value)
{
    int calculate_overflow, calculate_carry, rotate_right, temp_reg;
    uint32_t value2;

    if (value == 0) return;

    if (mnemonic == UD_Iror)
    {
        rotate_right = 1;
        value2 = value;
    }
    else
    {
        rotate_right = 0;
        value2 = 16 - value;
    }

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        calculate_overflow = 1;
    }
    else
    {
        calculate_overflow = 0;
    }

    if (Tflags_to_write & FL_CARRY)
    {
        calculate_carry = 1;
    }
    else
    {
        calculate_carry = 0;
    }

    temp_reg = ARMTEMPREG(dst);

    if (temp_reg) // upper 16 bits are zero
    {
        OUTPUT_PARAMSTRING("orr %s, %s, %s, lsl #16\n", ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("mov %s, %s, ror #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), value2);
    }
    else
    {
        OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", ARMREGSTR(dst));
        OUTPUT_STRING("orr tmp2, tmp2, tmp2, lsr #16\n");
        OUTPUT_PARAMSTRING("mov tmp2, tmp2, ror #%i\n", value2);
        OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
        OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
    }


    if (calculate_carry || calculate_overflow)
    {
        OUTPUT_STRING("mrs tmp3, cpsr\n");

        if (calculate_carry)
        {
            if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
            {
                OUTPUT_STRING("orr tmp3, tmp3, #0x20000000\n");
            }
            else
            {
                OUTPUT_STRING("bic tmp3, tmp3, #0x30000000\n");
            }

            if (rotate_right)
            {
                // CF = MSB(modified reg)
                OUTPUT_PARAMSTRING("mov tmplr, %s, lsr #31\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP2));
            }
            else
            {
                // CF = LSB(modified reg)
                OUTPUT_PARAMSTRING("and tmplr, %s, #1\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP2));
            }

            if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
            {
                OUTPUT_STRING("eor tmp3, tmp3, tmplr, lsl #29\n");
            }
            else
            {
                OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsl #29\n");
            }
        }

        if (calculate_overflow)
        {
            if ((Tflags_to_write & FL_CARRY_SUB_ORIGINAL) || !calculate_carry)
            {
                OUTPUT_STRING("bic tmp3, tmp3, #0x10000000\n");
            }

            if (rotate_right)
            {
                // OF = MSB-1(modified reg) XOR CF (CF = MSB(modified reg))
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsl #1\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP2), (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP2));
                OUTPUT_STRING("and tmplr, tmplr, #0x80000000\n");
                OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsr #3\n");
            }
            else
            {
                // OF = MSB(modified reg) XOR CF (CF = LSB(modified reg))
                if (calculate_carry)
                {
                    OUTPUT_PARAMSTRING("eor tmplr, tmplr, %s, lsr #31\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP2));
                    OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsl #28\n");
                }
                else
                {
                    OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsl #31\n", (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP2), (temp_reg)?ARMREGSTR(dst):ARMREGSTR(AR_TMP2));
                    OUTPUT_STRING("and tmplr, tmplr, #0x80000000\n");
                    OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsr #3\n");
                }
            }
        }

        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
}

// rol, ror
// trashes AR_TMPLR, AR_TMP3, AR_TMP2
static void SR_arm_helper_rol_8(enum ud_mnemonic_code mnemonic, enum arm_regs dst, unsigned int dstshift, uint32_t value)
{
    int calculate_overflow, calculate_carry, rotate_right, temp_reg;

    if (value == 0) return;

    if (mnemonic == UD_Iror)
    {
        rotate_right = 1;
    }
    else
    {
        rotate_right = 0;
    }

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        calculate_overflow = 1;
    }
    else
    {
        calculate_overflow = 0;
    }

    if (Tflags_to_write & FL_CARRY)
    {
        calculate_carry = 1;
    }
    else
    {
        calculate_carry = 0;
    }

    temp_reg = ARMTEMPREG(dst);

    if (temp_reg && !dstshift) // upper 24 bits are zero
    {
        if (rotate_right)
        {
            OUTPUT_PARAMSTRING("orr %s, %s, %s, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("mov %s, %s, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), value);
        }
        else
        {
            OUTPUT_PARAMSTRING("orr %s, %s, %s, lsl #24\n", ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("mov %s, %s, ror #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), 32 - value);
        }
    }
    else
    {
        OUTPUT_PARAMSTRING("and tmp3, %s, #0x%x\n", ARMREGSTR(dst), 0xff << dstshift);
        if (rotate_right)
        {
            OUTPUT_STRING("orr tmp3, tmp3, tmp3, lsl #8\n");
            OUTPUT_PARAMSTRING("mov tmp3, tmp3, lsr #%i\n", value);
        }
        else
        {
            if (dstshift)
            {
                OUTPUT_STRING("orr tmp3, tmp3, tmp3, lsr #8\n");
                OUTPUT_PARAMSTRING("mov tmp3, tmp3, lsl #%i\n", value);
            }
            else
            {
                OUTPUT_STRING("orr tmp3, tmp3, tmp3, lsl #24\n");
                OUTPUT_PARAMSTRING("mov tmp3, tmp3, ror #%i\n", 32 - value);
            }
        }
        OUTPUT_PARAMSTRING("and tmp3, tmp3, #0x%x\n", 0xff << dstshift);
        OUTPUT_PARAMSTRING("bic %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), 0xff << dstshift);
        OUTPUT_PARAMSTRING("orr %s, %s, tmp3\n", ARMREGSTR(dst), ARMREGSTR(dst));
    }


    if (calculate_carry || calculate_overflow)
    {
        OUTPUT_STRING("mrs tmp3, cpsr\n");

        if (calculate_carry)
        {
            if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
            {
                OUTPUT_STRING("orr tmp3, tmp3, #0x20000000\n");
            }
            else
            {
                OUTPUT_STRING("bic tmp3, tmp3, #0x30000000\n");
            }

            if (rotate_right)
            {
                // CF = MSB(modified reg)
                OUTPUT_PARAMSTRING("and tmplr, %s, #0x%x\n", ARMREGSTR(dst), 0x80 << dstshift);
            }
            else
            {
                // CF = LSB(modified reg)
                OUTPUT_PARAMSTRING("and tmplr, %s, #%i\n", ARMREGSTR(dst), 1 << dstshift);
            }

            if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
            {
                OUTPUT_PARAMSTRING("eor tmp3, tmp3, tmplr, lsl #%i\n", (29 - dstshift) - ( (rotate_right)?7:0 ));
            }
            else
            {
                OUTPUT_PARAMSTRING("orr tmp3, tmp3, tmplr, lsl #%i\n", (29 - dstshift) - ( (rotate_right)?7:0 ));
            }
        }

        if (calculate_overflow)
        {
            if ((Tflags_to_write & FL_CARRY_SUB_ORIGINAL) || !calculate_carry)
            {
                OUTPUT_STRING("bic tmp3, tmp3, #0x10000000\n");
            }

            if (rotate_right)
            {
                // OF = MSB-1(modified reg) XOR CF (CF = MSB(modified reg))
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsl #1\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
            else
            {
                // OF = MSB(modified reg) XOR CF (CF = LSB(modified reg))
                OUTPUT_PARAMSTRING("eor tmplr, %s, %s, lsl #7\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
            OUTPUT_PARAMSTRING("and tmplr, tmplr, #0x%x\n", 0x80 << dstshift);
            OUTPUT_PARAMSTRING("orr tmp3, tmp3, tmplr, lsl #%i\n", 21 - dstshift);
        }

        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
}

// sal, sar, shl, shr
// trashes AR_TMPLR, AR_TMP3
static void SR_arm_helper_sal_32(enum ud_mnemonic_code mnemonic, enum arm_regs dst, uint32_t value)
{
    const char *updcond, *ishift;
    int calculate_overflow, shift_right;

    if (value == 0) return;

    if (mnemonic == UD_Isar)
    {
        ishift = instr_asr;
        shift_right = 1;
    }
    else if (mnemonic == UD_Ishr)
    {
        ishift = instr_lsr;
        shift_right = 1;
    }
    else
    {
        ishift = instr_lsl;
        shift_right = 0;
    }

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        calculate_overflow = 1;
    }
    else
    {
        calculate_overflow = 0;
    }

    if ( ( Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) ) ||
         ( calculate_overflow && (shift_right == 0) )
       )
    {
        updcond = updatecond;
    }
    else
    {
        updcond = empty;
    }

    if (calculate_overflow && shift_right)
    {
        // update overflow flag for sar, shr
        if (mnemonic == UD_Isar)
        {
            // OF = 0
            OUTPUT_STRING("mov tmplr, #0\n");
        }
        else
        {
            // OF = MSB(original reg)
            OUTPUT_PARAMSTRING("and tmplr, %s, #0x80000000\n", ARMREGSTR(dst));
        }
    }

    OUTPUT_PARAMSTRING("mov%s %s, %s, %s #%i\n", updcond, ARMREGSTR(dst), ARMREGSTR(dst), ishift, value);

    if (Tflags_to_write & FL_PARITY)
    {
        SR_disassemble_calculate_parity_flag(pOutput, dst, AR_TMP3, 0);
    }

    if (calculate_overflow)
    {
        OUTPUT_STRING("mrs tmp3, cpsr\n");

        if (shift_right)
        {
            OUTPUT_STRING("bic tmp3, tmp3, #0x10000000\n");
            OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsr #3\n");
        }
        else
        {
            // update overflow flag for sal, shl
            // OF = MSB(modified reg) XOR CF

            OUTPUT_PARAMSTRING("eor tmplr, tmp3, %s, lsr #2\n", ARMREGSTR(dst));
            OUTPUT_STRING("and tmplr, tmplr, #0x20000000\n");
            OUTPUT_STRING("bic tmp3, tmp3, #0x10000000\n");
            OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsr #1\n");
        }

        if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
        {
            OUTPUT_STRING("eor tmp3, tmp3, #0x20000000\n");
        }
        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
    else
    {
        if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
        {
            SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
        }
    }
}

// sal, sar, shl, shr
// trashes AR_TMPLR, AR_TMP3, AR_TMP2
static void SR_arm_helper_sal_16(enum ud_mnemonic_code mnemonic, enum arm_regs dst, uint32_t value)
{
    const char *updcond;
    int calculate_flags, calculate_overflow, shift_right, temp_reg;

    if (value == 0) return;

    if (mnemonic == UD_Isar)
    {
        shift_right = 1;
    }
    else if (mnemonic == UD_Ishr)
    {
        shift_right = 1;
    }
    else
    {
        shift_right = 0;
    }

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        calculate_overflow = 1;
    }
    else
    {
        calculate_overflow = 0;
    }

    if ( ( Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) ) ||
         ( calculate_overflow && (shift_right == 0) )
       )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (calculate_overflow && shift_right)
    {
        // update overflow flag for sar, shr
        if (mnemonic == UD_Isar)
        {
            // OF = 0
            OUTPUT_STRING("mov tmplr, #0\n");
        }
        else
        {
            // OF = MSB(original reg)
            OUTPUT_PARAMSTRING("and tmplr, %s, #0x8000\n", ARMREGSTR(dst));
        }
    }

    temp_reg = ARMTEMPREG(dst);

    if (shift_right)
    {
        if (mnemonic == UD_Isar)
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(dst));
            if (temp_reg)
            {
                OUTPUT_PARAMSTRING("mov%s %s, tmp3, asr #%i\n", updcond, ARMREGSTR(dst), value + 16);
            }
            else
            {
                OUTPUT_PARAMSTRING("mov%s tmp3, tmp3, asr #%i\n", updcond, value + 16);
                OUTPUT_STRING("mov tmp3, tmp3, lsl #16\n");
                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
        else
        {
            if (temp_reg) // upper 16 bits are zero
            {
                OUTPUT_PARAMSTRING("mov%s %s, %s, lsr #%i\n", updcond, ARMREGSTR(dst), ARMREGSTR(dst), value);
            }
            else
            {
                OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("mov%s tmp3, tmp3, lsr #%i\n", updcond, value + 16);
                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3\n", ARMREGSTR(dst), ARMREGSTR(dst));
            }
        }
    }
    else
    {
        if (temp_reg)
        {
            if (!calculate_flags)
            {
                OUTPUT_PARAMSTRING("mov %s, %s, lsl #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), value);
            }
            else
            {
                OUTPUT_PARAMSTRING("mov%s tmp3, %s, lsl #%i\n", updcond, ARMREGSTR(dst), value + 16);
                OUTPUT_PARAMSTRING("mov %s, tmp3, lsr #16\n", ARMREGSTR(dst));
            }
        }
        else
        {
            OUTPUT_PARAMSTRING("mov%s tmp3, %s, lsl #%i\n", updcond, ARMREGSTR(dst), value + 16);
            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
            OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
        }
    }

    if (Tflags_to_write & FL_PARITY)
    {
        SR_disassemble_calculate_parity_flag(pOutput, dst, AR_TMP3, 0);
    }

    if (calculate_overflow)
    {
        OUTPUT_STRING("mrs tmp3, cpsr\n");

        if (shift_right)
        {
            OUTPUT_STRING("bic tmp3, tmp3, #0x10000000\n");
            OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsl #13\n");
        }
        else
        {
            // update overflow flag for sal, shl
            // OF = MSB(modified reg) XOR CF

            OUTPUT_PARAMSTRING("eor tmplr, tmp3, %s, lsl #14\n", ARMREGSTR(dst));
            OUTPUT_STRING("and tmplr, tmplr, #0x20000000\n");
            OUTPUT_STRING("bic tmp3, tmp3, #0x10000000\n");
            OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsr #1\n");
        }

        if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
        {
            OUTPUT_STRING("eor tmp3, tmp3, #0x20000000\n");
        }
        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
    else
    {
        if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
        {
            SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
        }
    }
}

// sal, sar, shl, shr
// trashes AR_TMPLR, AR_TMP3
static void SR_arm_helper_sal_8(enum ud_mnemonic_code mnemonic, enum arm_regs dst, unsigned int dstshift, uint32_t value)
{
    const char *updcond;
    int calculate_flags, calculate_overflow, shift_right, temp_reg;

    if (value == 0) return;

    if (mnemonic == UD_Isar)
    {
        shift_right = 1;
    }
    else if (mnemonic == UD_Ishr)
    {
        shift_right = 1;
    }
    else
    {
        shift_right = 0;
    }

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        calculate_overflow = 1;
    }
    else
    {
        calculate_overflow = 0;
    }

    if ( ( Tflags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) ) ||
         ( calculate_overflow && (shift_right == 0) )
       )
    {
        updcond = updatecond;
        calculate_flags = 1;
    }
    else
    {
        updcond = empty;
        calculate_flags = 0;
    }

    if (calculate_overflow && shift_right)
    {
        // update overflow flag for sar, shr
        if (mnemonic == UD_Isar)
        {
            // OF = 0
            OUTPUT_STRING("mov tmplr, #0\n");
        }
        else
        {
            // OF = MSB(original reg)
            OUTPUT_PARAMSTRING("and tmplr, %s, #0x%x\n", ARMREGSTR(dst), 0x80 << dstshift);
        }
    }

    temp_reg = ARMTEMPREG(dst);

    if (shift_right)
    {
        if (mnemonic == UD_Isar)
        {
            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #%i\n", ARMREGSTR(dst), 24 - dstshift);
            if (temp_reg && !dstshift)
            {
                OUTPUT_PARAMSTRING("mov%s %s, tmp3, asr #%i\n", updcond, ARMREGSTR(dst), value + 24);
            }
            else
            {
                if (dstshift)
                {
                    OUTPUT_STRING("and tmp3, tmp3, #0xff000000\n");
                }
                OUTPUT_PARAMSTRING("mov%s tmp3, tmp3, asr #%i\n", updcond, value + 24);
                OUTPUT_STRING("mov tmp3, tmp3, lsl #24\n");
                OUTPUT_PARAMSTRING("bic %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), 0xff << dstshift);
                OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), 24 - dstshift);
            }
        }
        else
        {
            if (temp_reg && !dstshift) // upper 24 bits are zero
            {
                OUTPUT_PARAMSTRING("mov%s %s, %s, lsr #%i\n", updcond, ARMREGSTR(dst), ARMREGSTR(dst), value);
            }
            else
            {
                OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #%i\n", ARMREGSTR(dst), 24 - dstshift);
                OUTPUT_PARAMSTRING("mov%s tmp3, tmp3, lsr #%i\n", updcond, value + 24);
                OUTPUT_PARAMSTRING("bic %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), 0xff << dstshift);
                if (dstshift)
                {
                    OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(dst));
                }
                else
                {
                    OUTPUT_PARAMSTRING("orr %s, %s, tmp3\n", ARMREGSTR(dst), ARMREGSTR(dst));
                }
            }
        }
    }
    else
    {
        if (temp_reg && !dstshift)
        {
            if (!calculate_flags)
            {
                OUTPUT_PARAMSTRING("mov %s, %s, lsl #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), value);
            }
            else
            {
                OUTPUT_PARAMSTRING("mov%s tmp3, %s, lsl #%i\n", updcond, ARMREGSTR(dst), value + 24);
                OUTPUT_PARAMSTRING("mov %s, tmp3, lsr #24\n", ARMREGSTR(dst));
            }
        }
        else
        {
            if (dstshift)
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, #0xff00\n", ARMREGSTR(dst));
                OUTPUT_PARAMSTRING("mov%s tmp3, tmp3, lsl #%i\n", updcond, value + 16);
            }
            else
            {
                OUTPUT_PARAMSTRING("mov%s tmp3, %s, lsl #%i\n", updcond, ARMREGSTR(dst), value + 24);
            }

            OUTPUT_PARAMSTRING("bic %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), 0xff << dstshift);
            OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #%i\n", ARMREGSTR(dst), ARMREGSTR(dst), 24 - dstshift);
        }
    }

    if (Tflags_to_write & FL_PARITY)
    {
        if (dstshift)
        {
            OUTPUT_PARAMSTRING("mov tmp2, %s, lsr #8\n", ARMREGSTR(dst));
        }
        SR_disassemble_calculate_parity_flag(pOutput, (dstshift)?AR_TMP2:dst, AR_TMP3, 0);
    }

    if (calculate_overflow)
    {
        OUTPUT_STRING("mrs tmp3, cpsr\n");

        if (shift_right)
        {
            OUTPUT_STRING("bic tmp3, tmp3, #0x10000000\n");
            OUTPUT_PARAMSTRING("orr tmp3, tmp3, tmplr, lsl #%i\n", 21 - dstshift);
        }
        else
        {
            // update overflow flag for sal, shl
            // OF = MSB(modified reg) XOR CF

            OUTPUT_PARAMSTRING("eor tmplr, tmp3, %s, lsl #%i\n", ARMREGSTR(dst), 22 - dstshift);
            OUTPUT_STRING("and tmplr, tmplr, #0x20000000\n");
            OUTPUT_STRING("bic tmp3, tmp3, #0x10000000\n");
            OUTPUT_STRING("orr tmp3, tmp3, tmplr, lsr #1\n");
        }

        if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
        {
            OUTPUT_STRING("eor tmp3, tmp3, #0x20000000\n");
        }
        OUTPUT_STRING("msr cpsr_f, tmp3\n");
    }
    else
    {
        if (Tflags_to_write & FL_CARRY_SUB_ORIGINAL)
        {
            SR_disassemble_change_arm_flags(pOutput, 0, 0, FL_CARRY, AR_TMP3);
        }
    }
}

#undef OUTPUT_PARAMSTRING
#undef OUTPUT_STRING
