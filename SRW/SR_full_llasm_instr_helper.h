/**
 *
 *  Copyright (C) 2019 Roman Pauer
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
// trashes LR_TMP3, LR_TMP4, LR_TMP5
static void SR_llasm_helper_adc_32(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src2, int32_t value)
{
    const char *instr;
    int32_t overflow_value;

    if (mnemonic == UD_Iadc)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    OUTPUT_STRING("and tmp5, eflags, CF\n");

    if ((src2 != LR_NONE) && (dst == src2) && (Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadc))
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write & ~FL_CARRY, 0, 0);
    }
    else
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);
    }

    if (src2 != LR_NONE)
    {
        if (dst != src2)
        {
            if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
            {
                OUTPUT_PARAMSTRING("xor tmp4, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
            }

            OUTPUT_PARAMSTRING("add tmp3, %s, tmp5\n", LLREGSTR(src2));

            if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadc))
            {
                OUTPUT_STRING("cmovz tmp3, tmp5, tmp5, 0\n");
                OUTPUT_PARAMSTRING("cmovult %s, tmp3, tmp5, CF, tmp5\n", LLREGSTR(dst));
                OUTPUT_STRING("or eflags, eflags, tmp5\n");
            }

            OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, LLREGSTR(dst), LLREGSTR(dst));

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadc))
            {
                OUTPUT_STRING("cmovz tmp3, tmp5, tmp5, 0\n");
                OUTPUT_PARAMSTRING("cmovult %s, tmp3, tmp5, CF, tmp5\n", LLREGSTR(dst));
                OUTPUT_STRING("or eflags, eflags, tmp5\n");
            }

            if (Tflags_to_write & FL_OVERFLOW)
            {
                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
                    OUTPUT_STRING("xor tmp5, tmp4, 0x80000000\n");
                    OUTPUT_STRING("and tmp3, tmp3, tmp5\n");
                }
                else
                {
                    OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
                    OUTPUT_STRING("xor tmp3, tmp3, 0x80000000\n");
                    OUTPUT_STRING("and tmp3, tmp3, tmp4\n");
                }
                OUTPUT_STRING("and tmp3, tmp3, 0x80000000\n");
                OUTPUT_STRING("lshr tmp3, tmp3, 31 - OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
        else
        {
            if (Tflags_to_write & FL_ADJUST)
            {
                OUTPUT_STRING("mov tmp4, 0\n");
            }

            OUTPUT_PARAMSTRING("add tmp3, %s, tmp5\n", LLREGSTR(src2));

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("lshr tmp5, %s, 31 - CF_SHIFT\n", LLREGSTR(src2));
                OUTPUT_STRING(";and tmp5, tmp5, CF\n");
                OUTPUT_STRING("or eflags, eflags, tmp5\n")
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("mov tmp5, %s\n", LLREGSTR(src2));
            }

            OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, LLREGSTR(dst), LLREGSTR(dst));

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("xor tmp3, %s, tmp5\n", LLREGSTR(dst));

                OUTPUT_STRING("and tmp3, tmp3, 0x80000000\n");
                OUTPUT_STRING("lshr tmp3, tmp3, 31 - OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
    }
    else
    {
        if (Tflags_to_write & FL_OVERFLOW)
        {
            if (((mnemonic == UD_Iadc) && (value >= 0)) || ((mnemonic != UD_Iadc) && (value < 0)))
            {
                if (value >= 0)
                {
                    overflow_value = 0x7fffffff - value;
                }
                else
                {
                    overflow_value = 0x7fffffff + value;
                }

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("sub tmp3, %i, tmp5\n", overflow_value);
                }
                else
                {
                    OUTPUT_PARAMSTRING("add tmp3, tmp5, %i\n", overflow_value);
                }
                OUTPUT_PARAMSTRING("cmovsgt %s, tmp3, tmp3, OF, 0\n", LLREGSTR(dst));
            }
            else
            {
                if (value >= 0)
                {
                    overflow_value = 0x80000000 + value;
                }
                else
                {
                    overflow_value = 0x80000000 - value;
                }

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("sub tmp3, %i, tmp5\n", overflow_value);
                }
                else
                {
                    OUTPUT_PARAMSTRING("add tmp3, tmp5, %i\n", overflow_value);
                }
                OUTPUT_PARAMSTRING("cmovslt %s, tmp3, tmp3, OF, 0\n", LLREGSTR(dst));
            }
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }

        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("xor tmp4, %s, 0x%x & AF\n", LLREGSTR(dst), value);
        }

        OUTPUT_PARAMSTRING("add tmp3, tmp5, %i\n", value);

        if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadc))
        {
            if (value == -1)
            {
                OUTPUT_STRING(";cmovz tmp3, tmp5, tmp5, 0\n");
                OUTPUT_PARAMSTRING("cmovult %s, tmp3, tmp5, CF, tmp5\n", LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("cmovult %s, tmp3, tmp5, CF, 0\n", LLREGSTR(dst));
            }
            OUTPUT_STRING("or eflags, eflags, tmp5\n");
        }

        OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, LLREGSTR(dst), LLREGSTR(dst));

        if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadc))
        {
            if (value == -1)
            {
                OUTPUT_STRING(";cmovz tmp3, tmp5, tmp5, 0\n");
                OUTPUT_PARAMSTRING("cmovult %s, tmp3, tmp5, CF, tmp5\n", LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("cmovult %s, tmp3, tmp5, CF, 0\n", LLREGSTR(dst));
            }
            OUTPUT_STRING("or eflags, eflags, tmp5\n");
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 32, 0, mnemonic, Tflags_to_write);
}

// adc, sbb
// trashes LR_TMP3, LR_TMP4, LR_TMP5
static void SR_llasm_helper_adc_16(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src2, int32_t value)
{
    const char *instr;
    int32_t overflow_value;
    enum ll_regs dstreg;

#define uvalue ((uint32_t) value)

    if (mnemonic == UD_Iadc)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    OUTPUT_STRING("and tmp5, eflags, CF\n");

    if ((src2 != LR_NONE) && (dst == src2) && (Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadc))
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write & ~FL_CARRY, 0, 0);
    }
    else
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);
    }

    if (src2 != LR_NONE)
    {
        if (dst != src2)
        {
            if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
            {
                OUTPUT_PARAMSTRING("xor tmp4, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
            }

            if (Tflags_to_write & FL_CARRY)
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, 0xffff\n", LLREGSTR(src2));
                OUTPUT_STRING("add tmp3, tmp3, tmp5\n");
            }
            else
            {
                OUTPUT_PARAMSTRING("add tmp3, %s, tmp5\n", LLREGSTR(src2));
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadc))
            {
                OUTPUT_PARAMSTRING("and tmp5, %s, 0xffff\n", LLREGSTR(dst));
                OUTPUT_STRING("cmovult tmp5, tmp3, tmp5, CF, 0\n");
                OUTPUT_STRING("or eflags, eflags, tmp5\n");
            }

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, LLREGSTR(dst), LLREGSTR(dst));
                dstreg = dst;
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp5, %s, tmp3\n", instr, LLREGSTR(dst));
                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp5\n", LLREGSTR(dst), LLREGSTR(dst));
                dstreg = LR_TMP5;
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("and tmp5, %s, 0xffff\n", LLREGSTR(dstreg));
                OUTPUT_STRING("cmovult tmp5, tmp3, tmp5, CF, 0\n");
                OUTPUT_STRING("or eflags, eflags, tmp5\n");
            }

            if (Tflags_to_write & FL_OVERFLOW)
            {
                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
                    OUTPUT_STRING("xor tmp5, tmp4, 0x8000\n");
                    OUTPUT_STRING("and tmp3, tmp3, tmp5\n");
                }
                else
                {
                    OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
                    OUTPUT_STRING("xor tmp3, tmp3, 0x8000\n");
                    OUTPUT_STRING("and tmp3, tmp3, tmp4\n");
                }
                OUTPUT_STRING("and tmp3, tmp3, 0x8000\n");
                OUTPUT_STRING("lshr tmp3, tmp3, 15 - OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
        else
        {
            if (Tflags_to_write & FL_ADJUST)
            {
                OUTPUT_STRING("mov tmp4, 0\n");
            }

            OUTPUT_PARAMSTRING("add tmp3, %s, tmp5\n", LLREGSTR(src2));

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("lshr tmp5, %s, 15 - CF_SHIFT\n", LLREGSTR(src2));
                OUTPUT_STRING("and tmp5, tmp5, CF\n");
                OUTPUT_STRING("or eflags, eflags, tmp5\n")
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("mov tmp5, %s\n", LLREGSTR(src2));
            }

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, LLREGSTR(dst), LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp3, %s, tmp3\n", instr, LLREGSTR(dst));
                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("xor tmp3, %s, tmp5\n", LLREGSTR(dst));

                OUTPUT_STRING("and tmp3, tmp3, 0x8000\n");
                OUTPUT_STRING("lshr tmp3, tmp3, 15 - OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
    }
    else
    {
        if (Tflags_to_write & FL_OVERFLOW)
        {
            if (((mnemonic == UD_Iadc) && (value >= 0)) || ((mnemonic != UD_Iadc) && (value < 0)))
            {
                if (value >= 0)
                {
                    overflow_value = 32767 - (value >> 16);
                }
                else
                {
                    overflow_value = 32767 + (value >> 16);
                }

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("sub tmp3, %i, tmp5\n", overflow_value);
                }
                else
                {
                    OUTPUT_PARAMSTRING("add tmp3, tmp5, %i\n", overflow_value);
                }
                OUTPUT_PARAMSTRING("ext16s tmp4, %s\n", LLREGSTR(dst));
                OUTPUT_STRING("cmovsgt tmp4, tmp3, tmp3, OF, 0\n");
            }
            else
            {
                if (value >= 0)
                {
                    overflow_value = -32768 + (value >> 16);
                }
                else
                {
                    overflow_value = -32768 - (value >> 16);
                }

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("sub tmp3, %i, tmp5\n", overflow_value);
                }
                else
                {
                    OUTPUT_PARAMSTRING("add tmp3, tmp5, %i\n", overflow_value);
                }
                OUTPUT_PARAMSTRING("ext16s tmp4, %s\n", LLREGSTR(dst));
                OUTPUT_STRING("cmovslt tmp4, tmp3, tmp3, OF, 0\n");
            }
        }

        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("xor tmp4, %s, 0x%x & AF\n", LLREGSTR(dst), uvalue >> 16);
        }

        OUTPUT_PARAMSTRING("add tmp3, tmp5, %i\n", uvalue >> 16);

        if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadc))
        {
            OUTPUT_PARAMSTRING("and tmp5, %s, 0xffff\n", LLREGSTR(dst));
            OUTPUT_STRING("cmovult tmp5, tmp3, tmp5, CF, 0\n");
            OUTPUT_STRING("or eflags, eflags, tmp5\n");
        }

        if (LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, LLREGSTR(dst), LLREGSTR(dst));
            dstreg = dst;
        }
        else
        {
            OUTPUT_PARAMSTRING("%s tmp5, %s, tmp3\n", instr, LLREGSTR(dst));
            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp5\n", LLREGSTR(dst), LLREGSTR(dst));
            dstreg = LR_TMP5;
        }

        if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadc))
        {
            OUTPUT_PARAMSTRING("and tmp5, %s, 0xffff\n", LLREGSTR(dstreg));
            OUTPUT_STRING("cmovult tmp5, tmp3, tmp5, CF, 0\n");
            OUTPUT_STRING("or eflags, eflags, tmp5\n");
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 16, 0, mnemonic, Tflags_to_write);

#undef uvalue
}

// adc, sbb
// trashes LR_TMP3, LR_TMP4, LR_TMP5, LR_TMP6
static void SR_llasm_helper_adc_8l(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src2, uint32_t src2shiftvalue)
{
    const char *instr;
    enum ll_regs src2reg, dstreg;
    int32_t value, overflow_value;

#define src2shift src2shiftvalue
#define uvalue src2shiftvalue

    if (mnemonic == UD_Iadc)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    OUTPUT_STRING("and tmp5, eflags, CF\n");

    if ((src2 != LR_NONE) && (dst == src2) && (src2shift == 0) && (Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadc))
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write & ~FL_CARRY, 0, 0);
    }
    else
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);
    }

    if (src2 != LR_NONE)
    {
        if ((dst != src2) || src2shift)
        {
            if (src2shift)
            {
                OUTPUT_PARAMSTRING("lshr tmp6, %s, %i\n", LLREGSTR(src2), src2shift);
                src2reg = LR_TMP6;
            }
            else
            {
                src2reg = src2;
            }

            if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
            {
                OUTPUT_PARAMSTRING("xor tmp4, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2reg));
            }

            if (Tflags_to_write & FL_CARRY)
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, 0xff\n", LLREGSTR(src2reg));
                OUTPUT_STRING("add tmp3, tmp3, tmp5\n");
            }
            else
            {
                OUTPUT_PARAMSTRING("add tmp3, %s, tmp5\n", LLREGSTR(src2reg));
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadc))
            {
                OUTPUT_PARAMSTRING("and tmp5, %s, 0xff\n", LLREGSTR(dst));
                OUTPUT_STRING("cmovult tmp5, tmp3, tmp5, CF, 0\n");
                OUTPUT_STRING("or eflags, eflags, tmp5\n");
            }

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, LLREGSTR(dst), LLREGSTR(dst));
                dstreg = dst;
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp5, %s, tmp3\n", instr, LLREGSTR(dst));
                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp5\n", LLREGSTR(dst), LLREGSTR(dst));
                dstreg = LR_TMP5;
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("and tmp5, %s, 0xff\n", LLREGSTR(dstreg));
                OUTPUT_STRING("cmovult tmp5, tmp3, tmp5, CF, 0\n");
                OUTPUT_STRING("or eflags, eflags, tmp5\n");
            }

            if (Tflags_to_write & FL_OVERFLOW)
            {
                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2reg));
                    OUTPUT_STRING("xor tmp5, tmp4, 0x80\n");
                    OUTPUT_STRING("and tmp3, tmp3, tmp5\n");
                }
                else
                {
                    OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2reg));
                    OUTPUT_STRING("xor tmp3, tmp3, 0x80\n");
                    OUTPUT_STRING("and tmp3, tmp3, tmp4\n");
                }
                OUTPUT_STRING("and tmp3, tmp3, 0x80\n");
                OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT - 7\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
        else
        {
            if (Tflags_to_write & FL_ADJUST)
            {
                OUTPUT_STRING("mov tmp4, 0\n");
            }

            OUTPUT_PARAMSTRING("add tmp3, %s, tmp5\n", LLREGSTR(src2));

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("and tmp5, %s, 0x80\n", LLREGSTR(src2));
                OUTPUT_STRING("lshr tmp5, tmp5, 7 - CF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp5\n")
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("mov tmp5, %s\n", LLREGSTR(src2));
            }

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, LLREGSTR(dst), LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp3, %s, tmp3\n", instr, LLREGSTR(dst));
                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadc))
            {
                OUTPUT_PARAMSTRING("xor tmp3, %s, tmp5\n", LLREGSTR(dst));

                OUTPUT_STRING("and tmp3, tmp3, 0x80\n");
                OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT - 7\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
    }
    else
    {
        value = uvalue;

        if (Tflags_to_write & FL_OVERFLOW)
        {
            if (((mnemonic == UD_Iadc) && (value >= 0)) || ((mnemonic != UD_Iadc) && (value < 0)))
            {
                if (value >= 0)
                {
                    overflow_value = 127 - (value >> 24);
                }
                else
                {
                    overflow_value = 127 + (value >> 24);
                }

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("sub tmp3, %i, tmp5\n", overflow_value);
                }
                else
                {
                    OUTPUT_PARAMSTRING("add tmp3, tmp5, %i\n", overflow_value);
                }
                OUTPUT_PARAMSTRING("ext8s tmp4, %s\n", LLREGSTR(dst));
                OUTPUT_STRING("cmovsgt tmp4, tmp3, tmp3, OF, 0\n");
            }
            else
            {
                if (value >= 0)
                {
                    overflow_value = -128 + (value >> 24);
                }
                else
                {
                    overflow_value = -128 - (value >> 24);
                }

                if (mnemonic == UD_Iadc)
                {
                    OUTPUT_PARAMSTRING("sub tmp3, %i, tmp5\n", overflow_value);
                }
                else
                {
                    OUTPUT_PARAMSTRING("add tmp3, tmp5, %i\n", overflow_value);
                }
                OUTPUT_PARAMSTRING("ext16s tmp4, %s\n", LLREGSTR(dst));
                OUTPUT_STRING("cmovslt tmp4, tmp3, tmp3, OF, 0\n");
            }
        }

        if (Tflags_to_write & FL_ADJUST)
        {
            OUTPUT_PARAMSTRING("xor tmp4, %s, 0x%x & AF\n", LLREGSTR(dst), uvalue >> 24);
        }

        OUTPUT_PARAMSTRING("add tmp3, tmp5, %i\n", uvalue >> 24);

        if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadc))
        {
            OUTPUT_PARAMSTRING("and tmp5, %s, 0xff\n", LLREGSTR(dst));
            OUTPUT_STRING("cmovult tmp5, tmp3, tmp5, CF, 0\n");
            OUTPUT_STRING("or eflags, eflags, tmp5\n");
        }

        if (LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, tmp3\n", instr, LLREGSTR(dst), LLREGSTR(dst));
            dstreg = dst;
        }
        else
        {
            OUTPUT_PARAMSTRING("%s tmp5, %s, tmp3\n", instr, LLREGSTR(dst));
            OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp5\n", LLREGSTR(dst), LLREGSTR(dst));
            dstreg = LR_TMP5;
        }

        if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadc))
        {
            OUTPUT_PARAMSTRING("and tmp5, %s, 0xff\n", LLREGSTR(dstreg));
            OUTPUT_STRING("cmovult tmp5, tmp3, tmp5, CF, 0\n");
            OUTPUT_STRING("or eflags, eflags, tmp5\n");
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 8, 0, mnemonic, Tflags_to_write);

#undef uvalue
#undef src2shift
}

// add, sub, cmp
// trashes LR_TMP3, LR_TMP4, LR_TMP5
static void SR_llasm_helper_add_32(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src1, enum ll_regs src2, int32_t value)
{
    const char *instr;
    int32_t overflow_value;
    enum ll_regs origsrc;

    if (mnemonic == UD_Iadd)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

    if (src2 != LR_NONE)
    {
        if (src1 != src2)
        {
            if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
            {
                OUTPUT_PARAMSTRING("xor tmp4, %s, %s\n", LLREGSTR(src1), LLREGSTR(src2));
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadd))
            {
                OUTPUT_PARAMSTRING("cmovult %s, %s, tmp3, CF, 0\n", LLREGSTR(src1), LLREGSTR(src2));
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2));

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("cmovult %s, %s, tmp3, CF, 0\n", LLREGSTR(dst), LLREGSTR(src2));
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            if (Tflags_to_write & FL_OVERFLOW)
            {
                if (mnemonic == UD_Iadd)
                {
                    OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
                    OUTPUT_STRING("xor tmp5, tmp4, 0x80000000\n");
                    OUTPUT_STRING("and tmp3, tmp3, tmp5\n");
                }
                else
                {
                    if (dst == src1)
                    {
                        OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
                        OUTPUT_STRING("xor tmp3, tmp3, 0x80000000\n");
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src1));
                    }
                    OUTPUT_STRING("and tmp3, tmp3, tmp4\n");
                }
                OUTPUT_STRING("and tmp3, tmp3, 0x80000000\n");
                OUTPUT_STRING("lshr tmp3, tmp3, 31 - OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
        else
        {
            if (Tflags_to_write & FL_ADJUST)
            {
                OUTPUT_STRING("mov tmp4, 0\n");
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("lshr tmp3, %s, 31 - CF_SHIFT\n", LLREGSTR(src1));
                OUTPUT_STRING(";and tmp3, tmp3, CF\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadd) && (dst == src2))
            {
                OUTPUT_PARAMSTRING("mov tmp5, %s\n", LLREGSTR(src2));
                origsrc = LR_TMP5;
            }
            else
            {
                origsrc = src2;
            }

            OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2));

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(origsrc));

                OUTPUT_STRING("and tmp3, tmp3, 0x80000000\n");
                OUTPUT_STRING("lshr tmp3, tmp3, 31 - OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
    }
    else
    {
        if ((Tflags_to_write & FL_ADJUST) && (value != 0))
        {
            OUTPUT_PARAMSTRING("xor tmp4, %s, 0x%x & AF\n", LLREGSTR(src1), value);
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value != 0))
        {
            if (((mnemonic == UD_Iadd) && (value >= 0)) || ((mnemonic != UD_Iadd) && (value < 0)))
            {
                if (value >= 0)
                {
                    overflow_value = 0x7fffffff - value;
                }
                else
                {
                    overflow_value = 0x7fffffff + value;
                }

                OUTPUT_PARAMSTRING("cmovsgt %s, %i, tmp3, OF, 0\n", LLREGSTR(src1), overflow_value);
            }
            else
            {
                if (value >= 0)
                {
                    overflow_value = 0x80000000 + value;
                }
                else
                {
                    overflow_value = 0x80000000 - value;
                }

                OUTPUT_PARAMSTRING("cmovslt %s, %i, tmp3, OF, 0\n", LLREGSTR(src1), overflow_value);
            }
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }

        if ((Tflags_to_write & FL_CARRY) && (value != 0) && (mnemonic != UD_Iadd))
        {
            OUTPUT_PARAMSTRING("cmovult %s, 0x%x, tmp3, CF, 0\n", LLREGSTR(src1), value);
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }

        OUTPUT_PARAMSTRING("%s %s, %s, %i\n", instr, LLREGSTR(dst), LLREGSTR(src1), value);

        if ((Tflags_to_write & FL_CARRY) && (value != 0) && (mnemonic == UD_Iadd))
        {
            OUTPUT_PARAMSTRING("cmovult %s, 0x%x, tmp3, CF, 0\n", LLREGSTR(dst), value);
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 32, 0, mnemonic, Tflags_to_write & ~( ((src2 != LR_NONE) || (value != 0))?0:FL_ADJUST ));
}

// add, sub, cmp
// trashes LR_TMP3, LR_TMP4, LR_TMP5
static void SR_llasm_helper_add_16(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src1, enum ll_regs src2, int32_t value)
{
    const char *instr;
    int32_t overflow_value;
    enum ll_regs dstreg, origsrc;

#define uvalue ((uint32_t) value)

    if (mnemonic == UD_Iadd)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

    if (src2 != LR_NONE)
    {
        if (src1 != src2)
        {
            if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
            {
                OUTPUT_PARAMSTRING("xor tmp4, %s, %s\n", LLREGSTR(src1), LLREGSTR(src2));
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadd))
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, 0xffff\n", LLREGSTR(src1));
                OUTPUT_PARAMSTRING("and tmp5, %s, 0xffff\n", LLREGSTR(src2));
                OUTPUT_STRING("cmovult tmp3, tmp5, tmp3, CF, 0\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2));
                dstreg = dst;
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp3, %s, %s\n", instr, LLREGSTR(src1), LLREGSTR(src2));
                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
                dstreg = LR_TMP3;
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, 0xffff\n", LLREGSTR(dstreg));
                OUTPUT_PARAMSTRING("and tmp5, %s, 0xffff\n", LLREGSTR(src2));
                OUTPUT_STRING("cmovult tmp3, tmp5, tmp3, CF, 0\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            if (Tflags_to_write & FL_OVERFLOW)
            {
                if (mnemonic == UD_Iadd)
                {
                    OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
                    OUTPUT_STRING("xor tmp5, tmp4, 0x8000\n");
                    OUTPUT_STRING("and tmp3, tmp3, tmp5\n");
                }
                else
                {
                    if (dst == src1)
                    {
                        OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2));
                        OUTPUT_STRING("xor tmp3, tmp3, 0x8000\n");
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src1));
                    }
                    OUTPUT_STRING("and tmp3, tmp3, tmp4\n");
                }
                OUTPUT_STRING("and tmp3, tmp3, 0x8000\n");
                OUTPUT_STRING("lshr tmp3, tmp3, 15 - OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
        else
        {
            if (Tflags_to_write & FL_ADJUST)
            {
                OUTPUT_STRING("mov tmp4, 0\n");
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("lshr tmp3, %s, 15 - CF_SHIFT\n", LLREGSTR(src1));
                OUTPUT_STRING("and tmp3, tmp3, CF\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadd) && (dst == src2))
            {
                OUTPUT_PARAMSTRING("mov tmp5, %s\n", LLREGSTR(src2));
                origsrc = LR_TMP5;
            }
            else
            {
                origsrc = src2;
            }

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2));
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp3, %s, %s\n", instr, LLREGSTR(src1), LLREGSTR(src2));
                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(origsrc));

                OUTPUT_STRING("and tmp3, tmp3, 0x8000\n");
                OUTPUT_STRING("lshr tmp3, tmp3, 15 - OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
    }
    else
    {
        if ((Tflags_to_write & FL_ADJUST) && (value != 0))
        {
            OUTPUT_PARAMSTRING("xor tmp4, %s, 0x%x & AF\n", LLREGSTR(src1), uvalue >> 16);
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value != 0))
        {
            if (((mnemonic == UD_Iadd) && (value >= 0)) || ((mnemonic != UD_Iadd) && (value < 0)))
            {
                if (value >= 0)
                {
                    overflow_value = 32767 - (value >> 16);
                }
                else
                {
                    overflow_value = 32767 + (value >> 16);
                }

                OUTPUT_PARAMSTRING("ext16s tmp3, %s\n", LLREGSTR(src1));
                OUTPUT_PARAMSTRING("cmovsgt tmp3, %i, tmp3, OF, 0\n", overflow_value);
            }
            else
            {
                if (value >= 0)
                {
                    overflow_value = -32768 + (value >> 16);
                }
                else
                {
                    overflow_value = -32768 - (value >> 16);
                }

                OUTPUT_PARAMSTRING("ext16s tmp3, %s\n", LLREGSTR(src1));
                OUTPUT_PARAMSTRING("cmovslt tmp3, %i, tmp3, OF, 0\n", overflow_value);
            }
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }

        if ((Tflags_to_write & FL_CARRY) && (value != 0) && (mnemonic != UD_Iadd))
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, 0xffff\n", LLREGSTR(src1));
            OUTPUT_PARAMSTRING("cmovult tmp3, 0x%x, tmp3, CF, 0\n", uvalue >> 16);
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }

        if (LLTEMPREG(dst) || ((value == 0) && (dst == src1)))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, %i\n", instr, LLREGSTR(dst), LLREGSTR(src1), value >> 16);
            dstreg = dst;
        }
        else
        {
            OUTPUT_PARAMSTRING("%s tmp3, %s, %i\n", instr, LLREGSTR(src1), value >> 16);
            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            dstreg = LR_TMP3;
        }

        if ((Tflags_to_write & FL_CARRY) && (value != 0) && (mnemonic == UD_Iadd))
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, 0xffff\n", LLREGSTR(dstreg));
            OUTPUT_PARAMSTRING("cmovult tmp3, 0x%x, tmp3, CF, 0\n", uvalue >> 16);
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 16, 0, mnemonic, Tflags_to_write & ~( ((src2 != LR_NONE) || (value != 0))?0:FL_ADJUST ));

#undef uvalue
}

// add, sub, cmp
// trashes LR_TMP3, LR_TMP4, LR_TMP5
static void SR_llasm_helper_add_8l(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src1, enum ll_regs src2, uint32_t src2shiftvalue)
{
    const char *instr;
    enum ll_regs src2reg, dstreg, origsrc;
    int32_t value, overflow_value;

#define src2shift src2shiftvalue
#define uvalue src2shiftvalue

    if (mnemonic == UD_Iadd)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

    if (src2 != LR_NONE)
    {
        if ((src1 != src2) || src2shift)
        {
            if (src2shift)
            {
                OUTPUT_PARAMSTRING("lshr tmp5, %s, %i\n", LLREGSTR(src2), src2shift);
                src2reg = LR_TMP5;
            }
            else
            {
                src2reg = src2;
            }

            if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
            {
                OUTPUT_PARAMSTRING("xor tmp4, %s, %s\n", LLREGSTR(src1), LLREGSTR(src2reg));
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadd))
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, 0xff\n", LLREGSTR(src1));
                OUTPUT_PARAMSTRING("and tmp5, %s, 0xff\n", LLREGSTR(src2reg));
                OUTPUT_STRING("cmovult tmp3, tmp5, tmp3, CF, 0\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2reg));
                dstreg = dst;
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp3, %s, %s\n", instr, LLREGSTR(src1), LLREGSTR(src2reg));
                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
                dstreg = LR_TMP3;
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, 0xff\n", LLREGSTR(dstreg));
                OUTPUT_PARAMSTRING("and tmp5, %s, 0xff\n", LLREGSTR(src2reg));
                OUTPUT_STRING("cmovult tmp3, tmp5, tmp3, CF, 0\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            if (Tflags_to_write & FL_OVERFLOW)
            {
                if (mnemonic == UD_Iadd)
                {
                    OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2reg));
                    OUTPUT_STRING("xor tmp5, tmp4, 0x80\n");
                    OUTPUT_STRING("and tmp3, tmp3, tmp5\n");
                }
                else
                {
                    if (dst == src1)
                    {
                        OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src2reg));
                        OUTPUT_STRING("xor tmp3, tmp3, 0x80\n");
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src1));
                    }
                    OUTPUT_STRING("and tmp3, tmp3, tmp4\n");
                }
                OUTPUT_STRING("and tmp3, tmp3, 0x80\n");
                OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT - 7\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
        else
        {
            if (Tflags_to_write & FL_ADJUST)
            {
                OUTPUT_STRING("mov tmp4, 0\n");
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, 0x80\n", LLREGSTR(src1));
                OUTPUT_STRING("lshr tmp3, tmp3, 7 - CF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadd) && (dst == src2))
            {
                OUTPUT_PARAMSTRING("mov tmp5, %s\n", LLREGSTR(src2));
                origsrc = LR_TMP5;
            }
            else
            {
                origsrc = src2;
            }

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2));
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp3, %s, %s\n", instr, LLREGSTR(src1), LLREGSTR(src2));
                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(origsrc));

                OUTPUT_STRING("and tmp3, tmp3, 0x80\n");
                OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT - 7\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
    }
    else
    {
        value = uvalue;

        if ((Tflags_to_write & FL_ADJUST) && (value != 0))
        {
            OUTPUT_PARAMSTRING("xor tmp4, %s, 0x%x & AF\n", LLREGSTR(src1), uvalue >> 24);
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value != 0))
        {
            if (((mnemonic == UD_Iadd) && (value >= 0)) || ((mnemonic != UD_Iadd) && (value < 0)))
            {
                if (value >= 0)
                {
                    overflow_value = 127 - (value >> 24);
                }
                else
                {
                    overflow_value = 127 + (value >> 24);
                }

                OUTPUT_PARAMSTRING("ext8s tmp3, %s\n", LLREGSTR(src1));
                OUTPUT_PARAMSTRING("cmovsgt tmp3, %i, tmp3, OF, 0\n", overflow_value);
            }
            else
            {
                if (value >= 0)
                {
                    overflow_value = -128 + (value >> 24);
                }
                else
                {
                    overflow_value = -128 - (value >> 24);
                }

                OUTPUT_PARAMSTRING("ext8s tmp3, %s\n", LLREGSTR(src1));
                OUTPUT_PARAMSTRING("cmovslt tmp3, %i, tmp3, OF, 0\n", overflow_value);
            }
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }

        if ((Tflags_to_write & FL_CARRY) && (value != 0) && (mnemonic != UD_Iadd))
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, 0xff\n", LLREGSTR(src1));
            OUTPUT_PARAMSTRING("cmovult tmp3, 0x%x, tmp3, CF, 0\n", uvalue >> 24);
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }

        if (LLTEMPREG(dst) || ((value == 0) && (dst == src1)))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, %i\n", instr, LLREGSTR(dst), LLREGSTR(src1), value >> 24);
            dstreg = dst;
        }
        else
        {
            OUTPUT_PARAMSTRING("%s tmp3, %s, %i\n", instr, LLREGSTR(src1), value >> 24);
            OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            dstreg = LR_TMP3;
        }

        if ((Tflags_to_write & FL_CARRY) && (value != 0) && (mnemonic == UD_Iadd))
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, 0xff\n", LLREGSTR(dstreg));
            OUTPUT_PARAMSTRING("cmovult tmp3, 0x%x, tmp3, CF, 0\n", uvalue >> 24);
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 8, 0, mnemonic, Tflags_to_write & ~( ((src2 != LR_NONE) || (value != 0))?0:FL_ADJUST ));

#undef uvalue
#undef src2shift
}

// add, sub, cmp
// trashes LR_TMP3, LR_TMP4, LR_TMP5
static void SR_llasm_helper_add_8h(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src1, enum ll_regs src2, uint8_t src2shiftvalue)
{
    const char *instr;
    int32_t value, overflow_value;
    enum ll_regs dstreg, origsrc;

#define src2shift src2shiftvalue
#define uvalue src2shiftvalue

    if (mnemonic == UD_Iadd)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

    if (src2 != LR_NONE)
    {
        if ((src1 != src2) || (src2shift == 0))
        {
            if ((Tflags_to_write & (FL_ADJUST | FL_OVERFLOW | FL_CARRY)) || LLTEMPREG(dst))
            {
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("and tmp5, %s, 0xff00\n", LLREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("shl tmp5, %s, 8\n", LLREGSTR(src2));
                }

                if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
                {
                    OUTPUT_PARAMSTRING("xor tmp4, %s, tmp5\n", LLREGSTR(src1));
                }

                if ((Tflags_to_write & FL_CARRY) && (mnemonic != UD_Iadd))
                {
                    OUTPUT_PARAMSTRING("and tmp3, %s, 0xff00\n", LLREGSTR(src1));
                    if (!src2shift)
                    {
                        OUTPUT_STRING("and tmp5, tmp5, 0xff00\n");
                    }
                    OUTPUT_STRING("cmovult tmp3, tmp5, tmp3, CF, 0\n");
                    OUTPUT_STRING("or eflags, eflags, tmp3\n")
                }

                if (LLTEMPREG(dst))
                {
                    OUTPUT_PARAMSTRING("%s %s, %s, tmp5\n", instr, LLREGSTR(dst), LLREGSTR(src1));
                    dstreg = dst;
                }
                else
                {
                    OUTPUT_PARAMSTRING("%s tmp3, %s, tmp5\n", instr, LLREGSTR(src1));
                    OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
                    dstreg = LR_TMP3;
                }

                if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadd))
                {
                    OUTPUT_PARAMSTRING("and tmp3, %s, 0xff00\n", LLREGSTR(dstreg));
                    if (!src2shift)
                    {
                        OUTPUT_STRING("and tmp5, tmp5, 0xff00\n");
                    }
                    OUTPUT_STRING("cmovult tmp3, tmp5, tmp3, CF, 0\n");
                    OUTPUT_STRING("or eflags, eflags, tmp3\n")
                }

                if (Tflags_to_write & FL_OVERFLOW)
                {
                    if (mnemonic == UD_Iadd)
                    {
                        OUTPUT_PARAMSTRING("xor tmp3, %s, tmp5\n", LLREGSTR(dst));
                        OUTPUT_STRING("xor tmp5, tmp4, 0x8000\n");
                        OUTPUT_STRING("and tmp3, tmp3, tmp5\n");
                    }
                    else
                    {
                        if (dst == src1)
                        {
                            OUTPUT_PARAMSTRING("xor tmp3, %s, tmp5\n", LLREGSTR(dst));
                            OUTPUT_STRING("xor tmp3, tmp3, 0x8000\n");
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(src1));
                        }
                        OUTPUT_STRING("and tmp3, tmp3, tmp4\n");
                    }
                    OUTPUT_STRING("and tmp3, tmp3, 0x8000\n");
                    OUTPUT_STRING("lshr tmp3, tmp3, 15 - OF_SHIFT\n");
                    OUTPUT_STRING("or eflags, eflags, tmp3\n")
                }
            }
            else
            {
                if (src2shift)
                {
                    OUTPUT_PARAMSTRING("and tmp3, %s, 0xff00\n", LLREGSTR(src2));
                }
                else
                {
                    OUTPUT_PARAMSTRING("shl tmp3, %s, 8\n", LLREGSTR(src2));
                }
                OUTPUT_PARAMSTRING("%s tmp3, %s, tmp3\n", instr, LLREGSTR(src1));
                OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            }
        }
        else
        {
            if (Tflags_to_write & FL_ADJUST)
            {
                OUTPUT_STRING("mov tmp4, 0\n");
            }

            if ((Tflags_to_write & FL_CARRY) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("lshr tmp3, %s, 15 - CF_SHIFT\n", LLREGSTR(src1));
                OUTPUT_STRING("and tmp3, tmp3, CF\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }

            OUTPUT_PARAMSTRING("and tmp5, %s, 0xff00\n", LLREGSTR(src2));
            if (dst == src2)
            {
                origsrc = LR_TMP5;
            }
            else
            {
                origsrc = src2;
            }

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, tmp5, tmp5\n", instr, LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("%s tmp3, tmp5, tmp5\n", instr);
                OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            }

            if ((Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iadd))
            {
                OUTPUT_PARAMSTRING("xor tmp3, %s, %s\n", LLREGSTR(dst), LLREGSTR(origsrc));

                OUTPUT_STRING("and tmp3, tmp3, 0x8000\n");
                OUTPUT_STRING("lshr tmp3, tmp3, 15 - OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n")
            }
        }
    }
    else
    {
        value = uvalue;

        if ((Tflags_to_write & FL_ADJUST) && (value != 0))
        {
            OUTPUT_PARAMSTRING("xor tmp4, %s, 0x%x & (AF << 8)\n", LLREGSTR(src1), (uvalue >> 16) & 0xff00);
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value != 0))
        {
            if (((mnemonic == UD_Iadd) && (value >= 0)) || ((mnemonic != UD_Iadd) && (value < 0)))
            {
                if (value >= 0)
                {
                    overflow_value = 127 - (value >> 24);
                }
                else
                {
                    overflow_value = 127 + (value >> 24);
                }

                OUTPUT_PARAMSTRING("lshr tmp3, %s, 8\n", LLREGSTR(src1));
                OUTPUT_STRING("ext8s tmp3, tmp3\n");
                OUTPUT_PARAMSTRING("cmovsgt tmp3, %i, tmp3, OF, 0\n", overflow_value);
            }
            else
            {
                if (value >= 0)
                {
                    overflow_value = -128 + (value >> 24);
                }
                else
                {
                    overflow_value = -128 - (value >> 24);
                }

                OUTPUT_PARAMSTRING("lshr tmp3, %s, 8\n", LLREGSTR(src1));
                OUTPUT_STRING("ext8s tmp3, tmp3\n");
                OUTPUT_PARAMSTRING("cmovslt tmp3, %i, tmp3, OF, 0\n", overflow_value);
            }
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }

        if ((Tflags_to_write & FL_CARRY) && (value != 0) && (mnemonic != UD_Iadd))
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, 0xff00\n", LLREGSTR(src1));
            OUTPUT_PARAMSTRING("cmovult tmp3, 0x%x, tmp3, CF, 0\n", (uvalue >> 16) & 0xff00);
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }

        if (LLTEMPREG(dst) || ((value == 0) && (dst == src1)))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, %i << 8\n", instr, LLREGSTR(dst), LLREGSTR(src1), value >> 24);
            dstreg = dst;
        }
        else
        {
            OUTPUT_PARAMSTRING("%s tmp3, %s, %i << 8\n", instr, LLREGSTR(src1), value >> 24);
            OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            dstreg = LR_TMP3;
        }

        if ((Tflags_to_write & FL_CARRY) && (value != 0) && (mnemonic == UD_Iadd))
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, 0xff00\n", LLREGSTR(dstreg));
            OUTPUT_PARAMSTRING("cmovult tmp3, 0x%x, tmp3, CF, 0\n", (uvalue >> 16) & 0xff00);
            OUTPUT_STRING("or eflags, eflags, tmp3\n")
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 8, 8, mnemonic, Tflags_to_write & ~( ((src2 != LR_NONE) || (value != 0))?0:FL_ADJUST ));

#undef uvalue
#undef src2shift
}

// and, or, xor, test
// trashes LR_TMP4
static void SR_llasm_helper_and_32(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src1, enum ll_regs src2, int32_t value)
{
    const char *instr;

    if (mnemonic == UD_Ior)
    {
        instr = "or";
    }
    else if (mnemonic == UD_Ixor)
    {
        instr = "xor";
    }
    else
    {
        instr = "and";
    }

    if (src2 != LR_NONE)
    {
        OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2));
    }
    else
    {
        OUTPUT_PARAMSTRING("%s %s, %s, 0x%x\n", instr, LLREGSTR(dst), LLREGSTR(src1), value);
    }

    if (Tflags_to_write & ~FL_ADJUST)
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

        SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 32, 0, mnemonic, Tflags_to_write & ~FL_ADJUST);
    }
}

// and, or, xor, test
// trashes LR_TMP4
static void SR_llasm_helper_and_16(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src1, enum ll_regs src2, int32_t value)
{
    const char *instr;

#define uvalue ((uint32_t) value)

    if (mnemonic == UD_Ior)
    {
        instr = "or";
    }
    else if (mnemonic == UD_Ixor)
    {
        instr = "xor";
    }
    else
    {
        instr = "and";
    }

    if (src2 != LR_NONE)
    {
        if ((src1 == src2) && (mnemonic == UD_Ixor) && !LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", LLREGSTR(dst), LLREGSTR(src1));
        }
        else if (LLTEMPREG(dst) || (src1 == src2))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2));
        }
        else if ((mnemonic == UD_Ior) || (mnemonic == UD_Ixor))
        {
            OUTPUT_PARAMSTRING("and tmp4, %s, 0xffff\n", LLREGSTR(src2));
            OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
        }
        else
        {
            OUTPUT_PARAMSTRING("or tmp4, %s, 0xffff0000\n", LLREGSTR(src2));
            OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
        }
    }
    else
    {
        if ((mnemonic == UD_Ior) || (mnemonic == UD_Ixor) || LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, 0x%x\n", instr, LLREGSTR(dst), LLREGSTR(src1), uvalue >> 16);
        }
        else
        {
            OUTPUT_PARAMSTRING("%s %s, %s, 0x%x\n", instr, LLREGSTR(dst), LLREGSTR(src1), 0xffff0000 | (uvalue >> 16));
        }
    }

    if (Tflags_to_write & ~FL_ADJUST)
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

        SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 16, 0, mnemonic, Tflags_to_write & ~FL_ADJUST);
    }

#undef uvalue
}

// and, or, xor, test
// trashes LR_TMP4
static void SR_llasm_helper_and_8l(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src1, enum ll_regs src2, unsigned int src2shiftvalue)
{
    const char *instr;

#define src2shift src2shiftvalue
#define value src2shiftvalue

    if (mnemonic == UD_Ior)
    {
        instr = "or";
    }
    else if (mnemonic == UD_Ixor)
    {
        instr = "xor";
    }
    else
    {
        instr = "and";
    }

    if (src2 != LR_NONE)
    {
        if (src2shift)
        {
            OUTPUT_PARAMSTRING("lshr tmp4, %s, 8\n", LLREGSTR(src2));

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
            else if ((mnemonic == UD_Ior) || (mnemonic == UD_Ixor))
            {
                OUTPUT_STRING("and tmp4, tmp4, 0xff\n");
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
            else
            {
                OUTPUT_STRING("or tmp4, tmp4, 0xffffff00\n");
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
        }
        else
        {
            if ((src1 == src2) && (mnemonic == UD_Ixor) && !LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("and %s, %s, 0xffffff00\n", LLREGSTR(dst), LLREGSTR(src1));
            }
            else if (LLTEMPREG(dst) || (src1 == src2))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2));
            }
            else if ((mnemonic == UD_Ior) || (mnemonic == UD_Ixor))
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, 0xff\n", LLREGSTR(src2));
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
            else
            {
                OUTPUT_PARAMSTRING("or tmp4, %s, 0xffffff00\n", LLREGSTR(src2));
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
        }
    }
    else
    {
        if ((mnemonic == UD_Ior) || (mnemonic == UD_Ixor) || LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, 0x%x\n", instr, LLREGSTR(dst), LLREGSTR(src1), value >> 24);
        }
        else
        {
            OUTPUT_PARAMSTRING("%s %s, %s, 0x%x\n", instr, LLREGSTR(dst), LLREGSTR(src1), 0xffffff00 | (value >> 24));
        }
    }

    if (Tflags_to_write & ~FL_ADJUST)
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

        SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 8, 0, mnemonic, Tflags_to_write & ~FL_ADJUST);
    }

#undef src2shift
#undef value
}

// and, or, xor, test
// trashes LR_TMP4
static void SR_llasm_helper_and_8h(enum ud_mnemonic_code mnemonic, enum ll_regs dst, enum ll_regs src1, enum ll_regs src2, unsigned int src2shiftvalue)
{
    const char *instr;

#define src2shift src2shiftvalue
#define value src2shiftvalue

    if (mnemonic == UD_Ior)
    {
        instr = "or";
    }
    else if (mnemonic == UD_Ixor)
    {
        instr = "xor";
    }
    else
    {
        instr = "and";
    }

    if (src2 != LR_NONE)
    {
        if (src2shift)
        {
            if ((src1 == src2) && (mnemonic == UD_Ixor) && !LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("and %s, %s, 0xffff00ff\n", LLREGSTR(dst), LLREGSTR(src1));
            }
            else if (LLTEMPREG(dst) || (src1 == src2))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, %s\n", instr, LLREGSTR(dst), LLREGSTR(src1), LLREGSTR(src2));
            }
            else if ((mnemonic == UD_Ior) || (mnemonic == UD_Ixor))
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, 0xff00\n", LLREGSTR(src2));
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
            else
            {
                OUTPUT_PARAMSTRING("or tmp4, %s, 0xffff00ff\n", LLREGSTR(src2));
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
        }
        else
        {
            OUTPUT_PARAMSTRING("shl tmp4, %s, 8\n", LLREGSTR(src2));

            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
            else if ((mnemonic == UD_Ior) || (mnemonic == UD_Ixor))
            {
                OUTPUT_STRING("and tmp4, tmp4, 0xff00\n");
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
            else
            {
                OUTPUT_STRING("or tmp4, tmp4, 0xffff00ff\n");
                OUTPUT_PARAMSTRING("%s %s, %s, tmp4\n", instr, LLREGSTR(dst), LLREGSTR(src1));
            }
        }
    }
    else
    {
        if ((mnemonic == UD_Ior) || (mnemonic == UD_Ixor) || LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, 0x%x\n", instr, LLREGSTR(dst), LLREGSTR(src1), (value >> 16) & 0xff00);
        }
        else
        {
            OUTPUT_PARAMSTRING("%s %s, %s, 0x%x\n", instr, LLREGSTR(dst), LLREGSTR(src1), 0xffff00ff | ((value >> 16) & 0xff00));
        }
    }

    if (Tflags_to_write & ~FL_ADJUST)
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

        SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 8, 8, mnemonic, Tflags_to_write & ~FL_ADJUST);
    }

#undef src2shift
#undef value
}

// dec, inc
// trashes LR_TMP3, LR_TMP4
static void SR_llasm_helper_dec_32(enum ud_mnemonic_code mnemonic, enum ll_regs dst)
{
    const char *instr;

    if (mnemonic == UD_Iinc)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    SR_disassemble_change_flags(pOutput, Tflags_to_write & ~FL_CARRY, 0, 0);

    if ( Tflags_to_write & FL_ADJUST )
    {
        OUTPUT_PARAMSTRING("mov tmp4, %s\n", LLREGSTR(dst));
    }

    if ( (Tflags_to_write & FL_OVERFLOW) && (mnemonic != UD_Iinc))
    {
        OUTPUT_PARAMSTRING("cmoveq %s, 0x80000000, tmp3, OF, 0\n", LLREGSTR(dst));
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    OUTPUT_PARAMSTRING("%s %s, %s, 1\n", instr, LLREGSTR(dst), LLREGSTR(dst));

    if ( (Tflags_to_write & FL_OVERFLOW) && (mnemonic == UD_Iinc))
    {
        OUTPUT_PARAMSTRING("cmoveq %s, 0x80000000, tmp3, OF, 0\n", LLREGSTR(dst));
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 32, 0, mnemonic, Tflags_to_write);
}

// dec, inc
// trashes LR_TMP3, LR_TMP4
static void SR_llasm_helper_dec_16(enum ud_mnemonic_code mnemonic, enum ll_regs dst)
{
    const char *instr;

    if (mnemonic == UD_Iinc)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    SR_disassemble_change_flags(pOutput, Tflags_to_write & ~FL_CARRY, 0, 0);

    if ( Tflags_to_write & FL_ADJUST )
    {
        OUTPUT_PARAMSTRING("mov tmp4, %s\n", LLREGSTR(dst));
    }

    if (LLTEMPREG(dst))
    {
        OUTPUT_PARAMSTRING("%s %s, %s, 1\n", instr, LLREGSTR(dst), LLREGSTR(dst));
    }
    else
    {
        OUTPUT_PARAMSTRING("%s tmp3, %s, 1\n", instr, LLREGSTR(dst));
        OUTPUT_PARAMSTRING("ins16 %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
    }

    if (Tflags_to_write & FL_OVERFLOW)
    {
        OUTPUT_PARAMSTRING("and tmp3, %s, 0xffff\n", LLREGSTR(dst));
        OUTPUT_PARAMSTRING("cmoveq tmp3, 0x%x, tmp3, OF, 0\n", (mnemonic == UD_Iinc)?0x8000:0x7fff);
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 16, 0, mnemonic, Tflags_to_write);
}

// dec, inc
// trashes LR_TMP3, LR_TMP4
static void SR_llasm_helper_dec_8(enum ud_mnemonic_code mnemonic, enum ll_regs dst, unsigned int dstshift)
{
    const char *instr;

    if (mnemonic == UD_Iinc)
    {
        instr = "add";
    }
    else
    {
        instr = "sub";
    }

    SR_disassemble_change_flags(pOutput, Tflags_to_write & ~FL_CARRY, 0, 0);

    if ( Tflags_to_write & FL_ADJUST )
    {
        OUTPUT_PARAMSTRING("mov tmp4, %s\n", LLREGSTR(dst));
    }


    if (!dstshift)
    {
        if (LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, 1\n", instr, LLREGSTR(dst), LLREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("%s tmp3, %s, 1\n", instr, LLREGSTR(dst));
            OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
        }
    }
    else
    {
        if (LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("%s %s, %s, 0x100\n", instr, LLREGSTR(dst), LLREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("%s tmp3, %s, 0x100\n", instr, LLREGSTR(dst));
            OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
        }
    }

    if (Tflags_to_write & FL_OVERFLOW)
    {
        OUTPUT_PARAMSTRING("and tmp3, %s, 0x%x\n", LLREGSTR(dst), 0xff << dstshift);
        OUTPUT_PARAMSTRING("cmoveq tmp3, 0x%x, tmp3, OF, 0\n", ((mnemonic == UD_Iinc)?0x80:0x7f) << dstshift);
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 8, dstshift, mnemonic, Tflags_to_write);
}

// neg
// trashes LR_TMP3, LR_TMP4
static void SR_llasm_helper_neg_32(enum ll_regs dst)
{
    SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

    if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
    {
        OUTPUT_PARAMSTRING("mov tmp4, %s\n", LLREGSTR(dst));
    }

    if (Tflags_to_write & FL_CARRY)
    {
        OUTPUT_PARAMSTRING("cmovz %s, tmp3, 0, CF\n", LLREGSTR(dst));
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    OUTPUT_PARAMSTRING("sub %s, 0, %s\n", LLREGSTR(dst), LLREGSTR(dst));

    if (Tflags_to_write & FL_OVERFLOW)
    {
        OUTPUT_PARAMSTRING("and tmp3, %s, tmp4\n", LLREGSTR(dst));
        OUTPUT_STRING("and tmp3, tmp3, 0x80000000\n");
        OUTPUT_STRING("cmovz tmp3, tmp3, 0, OF\n");
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 32, 0, UD_Ineg, Tflags_to_write);
}

// neg
// trashes LR_TMP3, LR_TMP4
static void SR_llasm_helper_neg_16(enum ll_regs dst)
{
    SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

    if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
    {
        OUTPUT_PARAMSTRING("mov tmp4, %s\n", LLREGSTR(dst));
    }

    if (Tflags_to_write & FL_CARRY)
    {
        OUTPUT_PARAMSTRING("and tmp3, %s, 0xffff\n", LLREGSTR(dst));
        OUTPUT_STRING("cmovz tmp3, tmp3, 0, CF\n");
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    if (LLTEMPREG(dst))
    {
        OUTPUT_PARAMSTRING("sub %s, 0, %s\n", LLREGSTR(dst), LLREGSTR(dst));
    }
    else
    {
        OUTPUT_PARAMSTRING("sub tmp3, 0, %s\n", LLREGSTR(dst));
        OUTPUT_PARAMSTRING("ins16 %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
    }

    if (Tflags_to_write & FL_OVERFLOW)
    {
        OUTPUT_PARAMSTRING("and tmp3, %s, tmp4\n", LLREGSTR(dst));
        OUTPUT_STRING("and tmp3, tmp3, 0x8000\n");
        OUTPUT_STRING("cmovz tmp3, tmp3, 0, OF\n");
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 16, 0, UD_Ineg, Tflags_to_write);
}

// neg
// trashes LR_TMP3, LR_TMP4
static void SR_llasm_helper_neg_8(enum ll_regs dst, unsigned int dstshift)
{
    SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

    if (Tflags_to_write & (FL_ADJUST | FL_OVERFLOW))
    {
        OUTPUT_PARAMSTRING("mov tmp4, %s\n", LLREGSTR(dst));
    }

    if (Tflags_to_write & FL_CARRY)
    {
        OUTPUT_PARAMSTRING("and tmp3, %s, 0x%x\n", LLREGSTR(dst), 0xff << dstshift);
        OUTPUT_STRING("cmovz tmp3, tmp3, 0, CF\n");
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    if (!dstshift)
    {
        if (LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("sub %s, 0, %s\n", LLREGSTR(dst), LLREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("sub tmp3, 0, %s\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
        }
    }
    else
    {
        OUTPUT_PARAMSTRING("and tmp3, %s, 0xff00\n", LLREGSTR(dst));
        OUTPUT_STRING("sub tmp3, 0, tmp3\n");
        OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
    }

    if ( Tflags_to_write & FL_OVERFLOW )
    {
        OUTPUT_PARAMSTRING("and tmp3, %s, tmp4\n", LLREGSTR(dst));
        OUTPUT_PARAMSTRING("and tmp3, tmp3, 0x%x\n", 0x80 << dstshift);
        OUTPUT_STRING("cmovz tmp3, tmp3, 0, OF\n");
        OUTPUT_STRING("or eflags, eflags, tmp3\n")
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 8, dstshift, UD_Ineg, Tflags_to_write);
}

// rol, ror
static void SR_llasm_helper_rol_32(enum ud_mnemonic_code mnemonic, enum ll_regs dst, uint32_t value)
{
    if (value == 0) return;

    if ((Tflags_to_write & FL_CARRY) || ((Tflags_to_write & FL_OVERFLOW) && (value == 1)))
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
    }

    if (mnemonic == UD_Iror)
    {
        if ((Tflags_to_write & FL_CARRY) && (value == 1))
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, CF\n", LLREGSTR(dst));
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
        {
            OUTPUT_PARAMSTRING("lshr tmp3, %s, 31\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("xor tmp3, tmp3, %s\n", LLREGSTR(dst));
            OUTPUT_STRING("and tmp3, tmp3, 1\n");
            OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT\n");
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }

        OUTPUT_PARAMSTRING("lshr tmp3, %s, %i\n", LLREGSTR(dst), value);
        OUTPUT_PARAMSTRING("shl tmp4, %s, %i\n", LLREGSTR(dst), 32 - value);
        OUTPUT_PARAMSTRING("or %s, tmp3, tmp4\n", LLREGSTR(dst));

        if ((Tflags_to_write & FL_CARRY) && (value != 1))
        {
            OUTPUT_PARAMSTRING("lshr tmp3, %s, 31 - CF_SHIFT\n", LLREGSTR(dst));
            OUTPUT_STRING(";and tmp3, tmp3, CF\n");
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }
    }
    else
    {
        OUTPUT_PARAMSTRING("shl tmp3, %s, %i\n", LLREGSTR(dst), value);
        OUTPUT_PARAMSTRING("lshr tmp4, %s, %i\n", LLREGSTR(dst), 32 - value);
        OUTPUT_PARAMSTRING("or %s, tmp3, tmp4\n", LLREGSTR(dst));

        if (Tflags_to_write & FL_CARRY)
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, CF\n", LLREGSTR(dst));
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
        {
            OUTPUT_PARAMSTRING("lshr tmp3, %s, 31\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("xor tmp3, tmp3, %s\n", LLREGSTR(dst));
            OUTPUT_STRING("and tmp3, tmp3, 1\n");
            OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT\n");
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }
    }
}

// rol, ror
static void SR_llasm_helper_rol_16(enum ud_mnemonic_code mnemonic, enum ll_regs dst, uint32_t value)
{
    if (value == 0) return;

    if ((Tflags_to_write & FL_CARRY) || ((Tflags_to_write & FL_OVERFLOW) && (value == 1)))
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
    }

    if (mnemonic == UD_Iror)
    {
        if ((Tflags_to_write & FL_CARRY) && (value == 1))
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, CF\n", LLREGSTR(dst));
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
        {
            OUTPUT_PARAMSTRING("lshr tmp3, %s, 15\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("xor tmp3, tmp3, %s\n", LLREGSTR(dst));
            OUTPUT_STRING("and tmp3, tmp3, 1\n");
            OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT\n");
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }

        OUTPUT_PARAMSTRING("and tmp3, %s, 0xffff\n", LLREGSTR(dst));
        OUTPUT_PARAMSTRING("lshr tmp3, tmp3, %i\n", value);
        OUTPUT_PARAMSTRING("shl tmp4, %s, %i\n", LLREGSTR(dst), 16 - value);
        if (LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("or %s, tmp3, tmp4\n", LLREGSTR(dst));
        }
        else
        {
            OUTPUT_STRING("or tmp3, tmp3, tmp4\n");
            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
        }

        if ((Tflags_to_write & FL_CARRY) && (value != 1))
        {
            OUTPUT_PARAMSTRING("lshr tmp3, %s, 15 - CF_SHIFT\n", LLREGSTR(dst));
            OUTPUT_STRING("and tmp3, tmp3, CF\n");
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }
    }
    else
    {
        OUTPUT_PARAMSTRING("shl tmp3, %s, %i\n", LLREGSTR(dst), value);
        OUTPUT_PARAMSTRING("and tmp4, %s, 0xffff\n", LLREGSTR(dst));
        OUTPUT_PARAMSTRING("lshr tmp4, tmp4, %i\n", 16 - value);
        if (LLTEMPREG(dst))
        {
            OUTPUT_PARAMSTRING("or %s, tmp3, tmp4\n", LLREGSTR(dst));
        }
        else
        {
            OUTPUT_STRING("or tmp3, tmp3, tmp4\n");
            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
        }

        if (Tflags_to_write & FL_CARRY)
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, CF\n", LLREGSTR(dst));
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
        {
            OUTPUT_PARAMSTRING("lshr tmp3, %s, 15\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("xor tmp3, tmp3, %s\n", LLREGSTR(dst));
            OUTPUT_STRING("and tmp3, tmp3, 1\n");
            OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT\n");
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }
    }
}

// rol, ror
static void SR_llasm_helper_rol_8(enum ud_mnemonic_code mnemonic, enum ll_regs dst, unsigned int dstshift, uint32_t value)
{
    if (value == 0) return;

    if ((Tflags_to_write & FL_CARRY) || ((Tflags_to_write & FL_OVERFLOW) && (value == 1)))
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
    }

    if (mnemonic == UD_Iror)
    {
        if ((Tflags_to_write & FL_CARRY) && (value == 1) && !dstshift)
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, CF\n", LLREGSTR(dst));
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
        {
            OUTPUT_PARAMSTRING("lshr tmp3, %s, 7\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("xor tmp3, tmp3, %s\n", LLREGSTR(dst));
            if (dstshift)
            {
                OUTPUT_STRING("and tmp3, tmp3, 0x100\n");
                OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT - 8\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n");
            }
            else
            {
                OUTPUT_STRING("and tmp3, tmp3, 1\n");
                OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n");
            }
        }

        if (dstshift)
        {
            OUTPUT_PARAMSTRING("and tmp4, %s, 0xff00\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("lshr tmp3, tmp4, %i\n", value);
            OUTPUT_PARAMSTRING("shl tmp4, tmp4, %i\n", 8 - value);
            OUTPUT_STRING("or tmp3, tmp3, tmp4\n");
            OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("and tmp3, %s, 0xff\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("lshr tmp3, tmp3, %i\n", value);
            OUTPUT_PARAMSTRING("shl tmp4, %s, %i\n", LLREGSTR(dst), 8 - value);
            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("or %s, tmp3, tmp4\n", LLREGSTR(dst));
            }
            else
            {
                OUTPUT_STRING("or tmp3, tmp3, tmp4\n");
                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            }
        }

        if ((Tflags_to_write & FL_CARRY) && ((value != 1) || dstshift))
        {
            OUTPUT_PARAMSTRING("lshr tmp3, %s, %i - CF_SHIFT\n", LLREGSTR(dst), 7 + dstshift);
            OUTPUT_STRING("and tmp3, tmp3, CF\n");
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }
    }
    else
    {
        if (dstshift)
        {
            OUTPUT_PARAMSTRING("and tmp4, %s, 0xff00\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("shl tmp3, tmp4, %i\n", value);
            OUTPUT_PARAMSTRING("lshr tmp4, tmp4, %i\n", 8 - value);
            OUTPUT_STRING("or tmp3, tmp3, tmp4\n");
            OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
        }
        else
        {
            OUTPUT_PARAMSTRING("shl tmp3, %s, %i\n", LLREGSTR(dst), value);
            OUTPUT_PARAMSTRING("and tmp4, %s, 0xff\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("lshr tmp4, tmp4, %i\n", 8 - value);
            if (LLTEMPREG(dst))
            {
                OUTPUT_PARAMSTRING("or %s, tmp3, tmp4\n", LLREGSTR(dst));
            }
            else
            {
                OUTPUT_STRING("or tmp3, tmp3, tmp4\n");
                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp3\n", LLREGSTR(dst), LLREGSTR(dst));
            }
        }

        if (Tflags_to_write & FL_CARRY)
        {
            if (dstshift)
            {
                OUTPUT_PARAMSTRING("lshr tmp3, %s, %i\n", LLREGSTR(dst), dstshift);
                OUTPUT_STRING("and tmp3, tmp3, CF\n");
            }
            else
            {
                OUTPUT_PARAMSTRING("and tmp3, %s, CF\n", LLREGSTR(dst));
            }
            OUTPUT_STRING("or eflags, eflags, tmp3\n");
        }

        if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
        {
            OUTPUT_PARAMSTRING("lshr tmp3, %s, 7\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("xor tmp3, tmp3, %s\n", LLREGSTR(dst));
            if (dstshift)
            {
                OUTPUT_STRING("and tmp3, tmp3, 0x100\n");
                OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT - 8\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n");
            }
            else
            {
                OUTPUT_STRING("and tmp3, tmp3, 1\n");
                OUTPUT_STRING("shl tmp3, tmp3, OF_SHIFT\n");
                OUTPUT_STRING("or eflags, eflags, tmp3\n");
            }
        }
    }
}

// sal, sar, shl, shr
// trashes LR_TMP4
static void SR_llasm_helper_sal_32(enum ud_mnemonic_code mnemonic, enum ll_regs dst, uint32_t value)
{
    const char *instr;

    if (mnemonic == UD_Isar) instr = "ashr";
    else if (mnemonic == UD_Ishr) instr = "lshr";
    else instr = "shl";

    if ((Tflags_to_write & ~(FL_ADJUST | FL_OVERFLOW)) || ((Tflags_to_write & FL_OVERFLOW) && (value == 1)))
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);
    }

    if (Tflags_to_write & FL_CARRY)
    {
        if ((mnemonic == UD_Isar) || (mnemonic == UD_Ishr))
        {
            if (value == 1)
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, CF\n", LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("lshr tmp4, %s, %i\n", LLREGSTR(dst), value - 1);
                OUTPUT_STRING("and tmp4, tmp4, CF\n");
            }
        }
        else
        {
            OUTPUT_PARAMSTRING("lshr tmp4, %s, %i\n", LLREGSTR(dst), 32 - value);
            if (value != 1)
            {
                OUTPUT_STRING("and tmp4, tmp4, CF\n");
            }
        }
        OUTPUT_STRING("or eflags, eflags, tmp4\n")
    }

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        if (mnemonic == UD_Isar)
        {
        }
        else if (mnemonic == UD_Ishr)
        {
            OUTPUT_PARAMSTRING("and tmp4, %s, 0x80000000\n", LLREGSTR(dst));
            OUTPUT_STRING("cmovz tmp4, tmp4, 0, OF\n");
            OUTPUT_STRING("or eflags, eflags, tmp4\n");
        }
        else
        {
            OUTPUT_PARAMSTRING("mov tmp4, %s\n", LLREGSTR(dst));
        }
    }

    OUTPUT_PARAMSTRING("%s %s, %s, %i\n", instr, LLREGSTR(dst), LLREGSTR(dst), value);

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        if (mnemonic == UD_Isar)
        {
        }
        else if (mnemonic == UD_Ishr)
        {
        }
        else
        {
            OUTPUT_PARAMSTRING("xor tmp4, tmp4, %s\n", LLREGSTR(dst));
            OUTPUT_STRING("and tmp4, tmp4, 0x80000000\n");
            OUTPUT_STRING("cmovz tmp4, tmp4, 0, OF\n");
            OUTPUT_STRING("or eflags, eflags, tmp4\n");
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 32, 0, mnemonic, Tflags_to_write & ~FL_ADJUST);
}

// sal, sar, shl, shr
// trashes LR_TMP4
static void SR_llasm_helper_sal_16(enum ud_mnemonic_code mnemonic, enum ll_regs dst, uint32_t value)
{
    if ((Tflags_to_write & ~(FL_ADJUST | FL_OVERFLOW)) || ((Tflags_to_write & FL_OVERFLOW) && (value == 1)))
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);
    }

    if (Tflags_to_write & FL_CARRY)
    {
        if ((mnemonic == UD_Isar) || (mnemonic == UD_Ishr))
        {
            if (value == 1)
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, CF\n", LLREGSTR(dst));
                OUTPUT_STRING("or eflags, eflags, tmp4\n")
            }
            else if (value <= 16)
            {
                OUTPUT_PARAMSTRING("lshr tmp4, %s, %i\n", LLREGSTR(dst), value - 1);
                OUTPUT_STRING("and tmp4, tmp4, CF\n");
                OUTPUT_STRING("or eflags, eflags, tmp4\n")
            }
            else
            {
                if (mnemonic == UD_Isar)
                {
                    OUTPUT_PARAMSTRING("lshr tmp4, %s, 15\n", LLREGSTR(dst));
                    OUTPUT_STRING("and tmp4, tmp4, CF\n");
                    OUTPUT_STRING("or eflags, eflags, tmp4\n")
                }
            }
        }
        else
        {
            if (value == 16)
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, CF\n", LLREGSTR(dst));
                OUTPUT_STRING("or eflags, eflags, tmp4\n")
            }
            else if (value < 16)
            {
                OUTPUT_PARAMSTRING("lshr tmp4, %s, %i\n", LLREGSTR(dst), 16 - value);
                OUTPUT_STRING("and tmp4, tmp4, CF\n");
                OUTPUT_STRING("or eflags, eflags, tmp4\n")
            }
        }
    }

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        if (mnemonic == UD_Isar)
        {
        }
        else if (mnemonic == UD_Ishr)
        {
            OUTPUT_PARAMSTRING("and tmp4, %s, 0x8000\n", LLREGSTR(dst));
            OUTPUT_STRING("cmovz tmp4, tmp4, 0, OF\n");
            OUTPUT_STRING("or eflags, eflags, tmp4\n");
        }
        else
        {
            OUTPUT_PARAMSTRING("shl tmp4, %s, 1\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("xor tmp4, tmp4, %s\n", LLREGSTR(dst));
            OUTPUT_STRING("and tmp4, tmp4, 0x8000\n");
            OUTPUT_STRING("cmovz tmp4, tmp4, 0, OF\n");
            OUTPUT_STRING("or eflags, eflags, tmp4\n");
        }
    }

    if (LLTEMPREG(dst))
    {
        if (mnemonic == UD_Isar)
        {
            OUTPUT_PARAMSTRING("ext16s %s, %s\n", LLREGSTR(dst), LLREGSTR(dst));
            OUTPUT_PARAMSTRING("ashr %s, %s, %i\n", LLREGSTR(dst), LLREGSTR(dst), value);
        }
        else if (mnemonic == UD_Ishr)
        {
            if (value >= 16)
            {
                OUTPUT_PARAMSTRING("mov %s, 0\n", LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("and %s, %s, 0xffff\n", LLREGSTR(dst), LLREGSTR(dst));
                OUTPUT_PARAMSTRING("lshr %s, %s, %i\n", LLREGSTR(dst), LLREGSTR(dst), value);
            }
        }
        else
        {
            if (value >= 16)
            {
                OUTPUT_PARAMSTRING("mov %s, 0\n", LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("shl %s, %s, %i\n", LLREGSTR(dst), LLREGSTR(dst), value);
            }
        }
    }
    else
    {
        if (mnemonic == UD_Isar)
        {
            OUTPUT_PARAMSTRING("ext16s tmp4, %s\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("ashr tmp4, tmp4, %i\n", value);
            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp4\n", LLREGSTR(dst), LLREGSTR(dst));
        }
        else if (mnemonic == UD_Ishr)
        {
            if (value >= 16)
            {
                OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", LLREGSTR(dst), LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, 0xffff\n", LLREGSTR(dst));
                OUTPUT_PARAMSTRING("lshr tmp4, tmp4, %i\n", value);
                OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", LLREGSTR(dst), LLREGSTR(dst));
                OUTPUT_PARAMSTRING("or %s, %s, tmp4\n", LLREGSTR(dst), LLREGSTR(dst));
            }
        }
        else
        {
            if (value >= 16)
            {
                OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", LLREGSTR(dst), LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("shl tmp4, %s, %i\n", LLREGSTR(dst), value);
                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp4\n", LLREGSTR(dst), LLREGSTR(dst));
            }
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 16, 0, mnemonic, Tflags_to_write & ~FL_ADJUST);
}

// sal, sar, shl, shr
// trashes LR_TMP4
static void SR_llasm_helper_sal_8(enum ud_mnemonic_code mnemonic, enum ll_regs dst, unsigned int dstshift, uint32_t value)
{
    if ((Tflags_to_write & ~(FL_ADJUST | FL_OVERFLOW)) || ((Tflags_to_write & FL_OVERFLOW) && (value == 1)))
    {
        SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);
    }

    if (Tflags_to_write & FL_CARRY)
    {
        if ((mnemonic == UD_Isar) || (mnemonic == UD_Ishr))
        {
            if ((value == 1) && (dstshift == 0))
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, CF\n", LLREGSTR(dst));
                OUTPUT_STRING("or eflags, eflags, tmp4\n")
            }
            else if (value <= 8)
            {
                OUTPUT_PARAMSTRING("lshr tmp4, %s, %i\n", LLREGSTR(dst), dstshift + value - 1);
                OUTPUT_STRING("and tmp4, tmp4, CF\n");
                OUTPUT_STRING("or eflags, eflags, tmp4\n")
            }
            else
            {
                if (mnemonic == UD_Isar)
                {
                    OUTPUT_PARAMSTRING("lshr tmp4, %s, %i\n", LLREGSTR(dst), 7 + dstshift);
                    OUTPUT_STRING("and tmp4, tmp4, CF\n");
                    OUTPUT_STRING("or eflags, eflags, tmp4\n")
                }
            }
        }
        else
        {
            if ((value == 8) && (dstshift == 0))
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, CF\n", LLREGSTR(dst));
                OUTPUT_STRING("or eflags, eflags, tmp4\n")
            }
            else if (value <= 8)
            {
                OUTPUT_PARAMSTRING("lshr tmp4, %s, %i\n", LLREGSTR(dst), dstshift + 8 - value);
                OUTPUT_STRING("and tmp4, tmp4, CF\n");
                OUTPUT_STRING("or eflags, eflags, tmp4\n")
            }
        }
    }

    if ((Tflags_to_write & FL_OVERFLOW) && (value == 1))
    {
        if (mnemonic == UD_Isar)
        {
        }
        else if (mnemonic == UD_Ishr)
        {
            OUTPUT_PARAMSTRING("and tmp4, %s, 0x%x\n", LLREGSTR(dst), 0x80 << dstshift);
            OUTPUT_STRING("cmovz tmp4, tmp4, 0, OF\n");
            OUTPUT_STRING("or eflags, eflags, tmp4\n");
        }
        else
        {
            OUTPUT_PARAMSTRING("shl tmp4, %s, 1\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("xor tmp4, tmp4, %s\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("and tmp4, tmp4, 0x%x\n", 0x80 << dstshift);
            OUTPUT_STRING("cmovz tmp4, tmp4, 0, OF\n");
            OUTPUT_STRING("or eflags, eflags, tmp4\n");
        }
    }


    if (dstshift)
    {
        if (mnemonic == UD_Isar)
        {
            OUTPUT_PARAMSTRING("ext16s tmp4, %s\n", LLREGSTR(dst));
            OUTPUT_PARAMSTRING("ashr tmp4, tmp4, %i\n", value);
            OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp4\n", LLREGSTR(dst), LLREGSTR(dst));
        }
        else if (mnemonic == UD_Ishr)
        {
            if (value >= 8)
            {
                OUTPUT_PARAMSTRING("and %s, %s, 0xffff00ff\n", LLREGSTR(dst), LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, 0xff00\n", LLREGSTR(dst));
                OUTPUT_PARAMSTRING("lshr tmp4, tmp4, %i\n", value);
                OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp4\n", LLREGSTR(dst), LLREGSTR(dst));
            }
        }
        else
        {
            if (value >= 8)
            {
                OUTPUT_PARAMSTRING("and %s, %s, 0xffff00ff\n", LLREGSTR(dst), LLREGSTR(dst));
            }
            else
            {
                OUTPUT_PARAMSTRING("and tmp4, %s, 0xff00\n", LLREGSTR(dst));
                OUTPUT_PARAMSTRING("shl tmp4, tmp4, %i\n", value);
                OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp4\n", LLREGSTR(dst), LLREGSTR(dst));
            }
        }
    }
    else
    {
        if (LLTEMPREG(dst))
        {
            if (mnemonic == UD_Isar)
            {
                OUTPUT_PARAMSTRING("ext8s %s, %s\n", LLREGSTR(dst), LLREGSTR(dst));
                OUTPUT_PARAMSTRING("ashr %s, %s, %i\n", LLREGSTR(dst), LLREGSTR(dst), value);
            }
            else if (mnemonic == UD_Ishr)
            {
                if (value >= 8)
                {
                    OUTPUT_PARAMSTRING("mov %s, 0\n", LLREGSTR(dst));
                }
                else
                {
                    OUTPUT_PARAMSTRING("and %s, %s, 0xff\n", LLREGSTR(dst), LLREGSTR(dst));
                    OUTPUT_PARAMSTRING("lshr %s, %s, %i\n", LLREGSTR(dst), LLREGSTR(dst), value);
                }
            }
            else
            {
                if (value >= 8)
                {
                    OUTPUT_PARAMSTRING("mov %s, 0\n", LLREGSTR(dst));
                }
                else
                {
                    OUTPUT_PARAMSTRING("shl %s, %s, %i\n", LLREGSTR(dst), LLREGSTR(dst), value);
                }
            }
        }
        else
        {
            if (mnemonic == UD_Isar)
            {
                OUTPUT_PARAMSTRING("ext8s tmp4, %s\n", LLREGSTR(dst));
                OUTPUT_PARAMSTRING("ashr tmp4, tmp4, %i\n", value);
                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp4\n", LLREGSTR(dst), LLREGSTR(dst));
            }
            else if (mnemonic == UD_Ishr)
            {
                if (value >= 8)
                {
                    OUTPUT_PARAMSTRING("and %s, %s, 0xffffff00\n", LLREGSTR(dst), LLREGSTR(dst));
                }
                else
                {
                    OUTPUT_PARAMSTRING("and tmp4, %s, 0xff\n", LLREGSTR(dst));
                    OUTPUT_PARAMSTRING("lshr tmp4, tmp4, %i\n", value);
                    OUTPUT_PARAMSTRING("and %s, %s, 0xffffff00\n", LLREGSTR(dst), LLREGSTR(dst));
                    OUTPUT_PARAMSTRING("or %s, %s, tmp4\n", LLREGSTR(dst), LLREGSTR(dst));
                }
            }
            else
            {
                if (value >= 8)
                {
                    OUTPUT_PARAMSTRING("and %s, %s, 0xffffff00\n", LLREGSTR(dst), LLREGSTR(dst));
                }
                else
                {
                    OUTPUT_PARAMSTRING("shl tmp4, %s, %i\n", LLREGSTR(dst), value);
                    OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp4\n", LLREGSTR(dst), LLREGSTR(dst));
                }
            }
        }
    }

    SR_disassemble_set_flags_AZSP(pOutput, dst, LR_TMP4, 8, dstshift, mnemonic, Tflags_to_write & ~FL_ADJUST);
}

#undef OUTPUT_PARAMSTRING
#undef OUTPUT_STRING
