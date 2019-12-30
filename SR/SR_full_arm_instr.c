/**
 *
 *  Copyright (C) 2016-2019 Roman Pauer
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

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "SR_defs.h"
#include "SR_vars.h"
#include "udis86_dep.h"

#undef DISPLAY_DISASSEMBLY


#if (OUTPUT_TYPE == OUT_ARM_LINUX)

static const char *arm_regs_str[] = {
    "error",
    "tmp1",
    "tmp2",
    "tmp3",
    "tmpadr",
    "edi",
    "esi",
    "ebp",
    "ebx",
    "edx",
    "tmp9",
    "ecx",
    "eax",
    "eflags",
    "esp",
    "tmplr",
    "eip"
};

enum arm_regs {
    AR_NONE,
    AR_TMP1,
    AR_TMP2,
    AR_TMP3,
    AR_TMPADR,
    AR_EDI,
    AR_ESI,
    AR_EBP,
    AR_EBX,
    AR_EDX,
    AR_TMP9,
    AR_ECX,
    AR_EAX,
    AR_EFLAGS,
    AR_ESP,
    AR_TMPLR,
    AR_EIP
};

static const enum arm_regs arm_regs_table[] = {
        AR_EAX,
        AR_ECX,
        AR_EDX,
        AR_EBX,
        AR_ESP,
        AR_EBP,
        AR_ESI,
        AR_EDI
};

static const char *instr_add = "add";
static const char *instr_sub = "sub";
static const char *instr_rsb = "rsb";
static const char *instr_adc = "adc";
static const char *instr_sbc = "sbc";
static const char *instr_rsc = "rsc";
static const char *instr_and = "and";
static const char *instr_orr = "orr";
static const char *instr_eor = "eor";
static const char *instr_bic = "bic";
static const char *instr_asr = "asr";
static const char *instr_lsr = "lsr";
static const char *instr_lsl = "lsl";


static const char *updatecond = "S";
static const char *empty = "";

static char *pOutput;
static uint_fast32_t Tflags_to_write;


/*
#define X86REGSTR(x) ud_reg_tab[(x) - UD_R_AL]
#define X87REGNUM(x) ((x) - UD_R_ST0)
#define ARMREGSTR(x) arm_regs_str[(x)]

#define X86322ARMREG(x) arm_regs_table[(x) - UD_R_EAX]
#define X86162ARMREG(x) arm_regs_table[(x) - UD_R_AX]
#define X868L2ARMREG(x) arm_regs_table[(x) - UD_R_AL]
#define X868H2ARMREG(x) arm_regs_table[(x) - UD_R_AH]

#define X862ARMREG(x) ( \
    ((x) >= UD_R_EAX && (x) <= UD_R_EDI)?(X86322ARMREG(x)):( \
    ((x) >= UD_R_AX && (x) <= UD_R_DI)?(X86162ARMREG(x)):( \
    ((x) >= UD_R_AL && (x) <= UD_R_BL)?(X868L2ARMREG(x)):( \
    ((x) >= UD_R_AH && (x) <= UD_R_BH)?(X868H2ARMREG(x)):( \
    AR_NONE \
    )))) \
)

#define X862ARMSTR(x) ARMREGSTR(X862ARMREG(x))
*/


static inline const char *x86regstr(enum ud_type reg) __attribute__ ((pure));
static inline int x87regnum(enum ud_type reg) __attribute__ ((const));
static inline const char *armregstr(enum arm_regs reg) __attribute__ ((pure));

static inline enum arm_regs x86322armreg(enum ud_type reg) __attribute__ ((pure));
static inline enum arm_regs x86162armreg(enum ud_type reg) __attribute__ ((pure));
static inline enum arm_regs x868l2armreg(enum ud_type reg) __attribute__ ((pure));
static inline enum arm_regs x868h2armreg(enum ud_type reg) __attribute__ ((pure));

static inline enum arm_regs x862armreg(enum ud_type reg) __attribute__ ((pure));
static inline const char *x862armstr(enum ud_type reg) __attribute__ ((pure));

static inline int armtempreg(enum arm_regs reg) __attribute__ ((const));

static inline const char *x86regstr(enum ud_type reg) { return ud_reg_tab[reg - UD_R_AL]; }
static inline int x87regnum(enum ud_type reg) { return reg - UD_R_ST0; }
static inline const char *armregstr(enum arm_regs reg) { return arm_regs_str[reg]; }

static inline enum arm_regs x86322armreg(enum ud_type reg) { return arm_regs_table[reg - UD_R_EAX]; }
static inline enum arm_regs x86162armreg(enum ud_type reg) { return arm_regs_table[reg - UD_R_AX]; }
static inline enum arm_regs x868l2armreg(enum ud_type reg) { return arm_regs_table[reg - UD_R_AL]; }
static inline enum arm_regs x868h2armreg(enum ud_type reg) { return arm_regs_table[reg - UD_R_AH]; }

static inline enum arm_regs x862armreg(enum ud_type reg)
{
    return (reg >= UD_R_EAX && reg <= UD_R_EDI)?( arm_regs_table[reg - UD_R_EAX] ):(
           (reg >= UD_R_AX && reg <= UD_R_DI)?( arm_regs_table[reg - UD_R_AX] ):(
           (reg >= UD_R_AL && reg <= UD_R_BL)?( arm_regs_table[reg - UD_R_AL] ):(
           (reg >= UD_R_AH && reg <= UD_R_BH)?( arm_regs_table[reg - UD_R_AH] ):(
           AR_NONE
           ))));
}
static inline const char *x862armstr(enum ud_type reg) { return arm_regs_str[x862armreg(reg)]; }

static inline int armtempreg(enum arm_regs reg) { return 0x0000841f & (1 << ( (unsigned int) reg) ); }

#define X86REGSTR(x) x86regstr(x)
#define X87REGNUM(x) x87regnum(x)
#define ARMREGSTR(x) armregstr(x)

#define X86322ARMREG(x) x86322armreg(x)
#define X86162ARMREG(x) x86162armreg(x)
#define X868L2ARMREG(x) x868l2armreg(x)
#define X868H2ARMREG(x) x868h2armreg(x)

#define X862ARMREG(x) x862armreg(x)
#define X862ARMSTR(x) x862armstr(x)

#define ARMTEMPREG(x) armtempreg(x)

enum madr_address_mode {
    MADR_REG      = 0,
    MADR_WRITE    = 1,
    MADR_READ     = 2,
    MADR_RW       = 3
};

enum extend_mode {
    ZERO_EXTEND = 0,
    SIGN_EXTEND = 1
};

enum halfword_read_mode {
    READ16TO16     = 0,
    READ16TO32SIGN = 1,
    READ16TO32ZERO = 2
};

enum byte_read_mode {
    READ8TO8LOW   = 0,
    READ8TO8HIGH  = 1,
    READ8TO16SIGN = 2,
    READ8TO16ZERO = 3,
    READ8TO32SIGN = 4,
    READ8TO32ZERO = 5
};

enum byte_write_mode {
    WRITE8LOW  = 0,
    WRITE8HIGH = 1
};

struct madr_result {
    char madr[64];
    int align;
    enum arm_regs memreg;
    char unaligned_madr[64];
};

static int SR_disassemble_fixup_operand(unsigned int Entry, const char *ostr, fixup_data *fixup, uint_fast32_t offset, int decoded_length)
{
    char cAdr[32];
    char *str1, *str2;
    int num;

    if (fixup->type == FT_SELFREL)
    {
        sprintf(cAdr, "0x%x", (unsigned int)(fixup->sofs + section[Entry].start + 4));
    }
    else
    {
        sprintf(cAdr, "0x%x", (unsigned int)(section[fixup->tsec].start + fixup->tofs));
    }


    // find substring (must be exactly one occurence)
    str1 = strstr(ostr, cAdr);
    if (str1 == NULL) return 0;

    str2 = str1 + strlen(cAdr);

    if ( strstr(str2, cAdr) != NULL )
    {
        if (decoded_length != 0)
        {
            // if two or more occurences in string, then do one's complement on searched data, and try searching for it

            uint8_t *tmpbuf;
            uint32_t *fixupofs;
            fixup_data tmpfixup;
            ud_t tmpud_obj;
            int ret, decoded_length2;

            tmpbuf = (uint8_t *) malloc(decoded_length + 4);
            if (tmpbuf != NULL)
            {
                memcpy(tmpbuf, &(section[Entry].adr[offset]), decoded_length + 4);
                fixupofs = (uint32_t *) &(tmpbuf[fixup->sofs - offset]);
                *fixupofs = ~(*fixupofs);
                tmpfixup = *fixup;
                tmpfixup.tofs = ~tmpfixup.tofs;

                ud_init(&tmpud_obj);
                ud_set_mode(&tmpud_obj, 32);
                ud_set_syntax(&tmpud_obj, UD_SYN_INTEL);
                ud_set_input_buffer(&tmpud_obj, tmpbuf, decoded_length + 4);
                ud_set_pc(&tmpud_obj, section[Entry].start + offset);

                decoded_length2 = ud_disassemble(&tmpud_obj);

                if (decoded_length2 != 0)
                {
                    ret = SR_disassemble_fixup_operand(Entry, ud_insn_asm(&tmpud_obj), &tmpfixup, 0, 0);

                    free(tmpbuf);

                    if (ret != 0) return ret;
                }
                else
                {
                    free(tmpbuf);
                }
            }
        }
        fprintf(stderr, "Two string occurences in fixup: 0x%x, %i - 0x%x\n%s\n%s\n", (unsigned int)fixup->sofs, (unsigned int)fixup->tsec, (unsigned int)fixup->tofs, cAdr, ostr);
        return 0;
    }

    if (decoded_length == 0)
    {
        // if searching for one's complement of searched data, return it to original value
        fixup->tofs = ~fixup->tofs;
    }

    num = 0;
    while (str1 != ostr)
    {
        str1--;
        if (*str1 == ',') num++;
    }

    return num + 1;
}

static void SR_get_fixup_label(char *cResult, const fixup_data *fixup, const extrn_data *extrn)
{
    char cLabel[32];
    int *label_value;
    output_data *output;
    uint_fast32_t sec, ofs;

    // locate label
    label_value = section_label_list_FindEntryEqual(fixup->tsec, fixup->tofs);

    sec = fixup->tsec;
    if (label_value != NULL)
    {
        if (SR_get_section_reladr(section[fixup->tsec].start + fixup->tofs + *label_value, &sec, &ofs))
        {
            output = section_output_list_FindEntryEqualOrLower(sec, ofs);
        }
        else output = NULL;
    }
    else if (fixup->tofs >= 0)
    {
        output = section_output_list_FindEntryEqualOrLower(fixup->tsec, fixup->tofs);
    }
    else
    {
        output = section_output_list_FindEntryEqualOrHigher(fixup->tsec, 0);
    }

    // set label to target address
    if (output != NULL)
    {
        output->has_label = 1;
        ofs = output->ofs;
    }
    else
    {
        sec = fixup->tsec;
        ofs = fixup->tofs;
    }

    // print label to string
    if (extrn != NULL)
    {
        strcpy(cLabel, extrn->proc);
    }
    else
    {
        SR_get_label(cLabel, section[sec].start + ofs);
    }

    if (((sec == fixup->tsec) && (ofs == fixup->tofs)) ||
        extrn != NULL)
    {
        strcpy(cResult, cLabel);
    }
    else
    {
        sprintf(cResult, "(%s + (%i))", cLabel, (int)((section[fixup->tsec].start + fixup->tofs) - (section[sec].start + ofs)));
    }

}

static unsigned int SR_get_imm_length(uint32_t imm)
{
    if (imm == 0) return 1;

    while ((imm & 3) == 0) imm>>=2;
    imm>>=8;

    if (imm == 0) return 1;

    while ((imm & 3) == 0) imm>>=2;
    imm>>=8;

    if (imm == 0) return 2;

    while ((imm & 3) == 0) imm>>=2;

    return ((imm >> 8) == 0)?3:4;
}

static void SR_arm_load_imm_orig(enum arm_regs dst, uint32_t value, unsigned int len)
{
    pOutput += strlen(pOutput);

#define OUTPUT_PARAMSTRING(x, ...) { sprintf(pOutput, x, __VA_ARGS__); pOutput += strlen(pOutput); }

    if (len <= 2)
    {
        int scale;

        scale = 0;
        if (value)
        {
            while ((value & 3) == 0)
            {
                value>>=2;
                scale+=2;
            }
        }

        OUTPUT_PARAMSTRING("mov %s, #0x%x\n", ARMREGSTR(dst), (value & 0xff) << scale);
        value>>=8;

        if (value)
        {
            scale+=8;
            while ((value & 3) == 0)
            {
                value>>=2;
                scale+=2;
            }

            OUTPUT_PARAMSTRING("orr %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), (value & 0xff) << scale);
        }
    }
    else
    {
        OUTPUT_PARAMSTRING("LDR %s, =0x%x\n", ARMREGSTR(dst), value);
    }

#undef OUTPUT_PARAMSTRING
}

static void SR_arm_load_imm_inv(enum arm_regs dst, uint32_t value, unsigned int len)
{
    pOutput += strlen(pOutput);

#define OUTPUT_PARAMSTRING(x, ...) { sprintf(pOutput, x, __VA_ARGS__); pOutput += strlen(pOutput); }

    if (len <= 2)
    {
        int scale;

        scale = 0;
        value = ~value;
        if (value)
        {
            while ((value & 3) == 0)
            {
                value>>=2;
                scale+=2;
            }
        }

        OUTPUT_PARAMSTRING("mvn %s, #0x%x\n", ARMREGSTR(dst), (value & 0xff) << scale);
        value>>=8;

        if (value)
        {
            scale+=8;
            while ((value & 3) == 0)
            {
                value>>=2;
                scale+=2;
            }

            OUTPUT_PARAMSTRING("bic %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), (value & 0xff) << scale);
        }
    }
    else
    {
        OUTPUT_PARAMSTRING("LDR %s, =0x%x\n", ARMREGSTR(dst), value);
    }

#undef OUTPUT_PARAMSTRING
}

static inline void SR_arm_load_imm_val(enum arm_regs dst, uint32_t value, unsigned int len1, unsigned int len2)
{
    if (len1 <= len2)
    {
        SR_arm_load_imm_orig(dst, value, len1);
    }
    else
    {
        SR_arm_load_imm_inv(dst, value, len2);
    }
}

static inline void SR_arm_load_imm32(enum arm_regs dst, uint32_t value)
{
    SR_arm_load_imm_val(dst, value, SR_get_imm_length(value), SR_get_imm_length(~value));
}

static void SR_arm_load_imm16(enum arm_regs dst, uint32_t value)
{
    pOutput += strlen(pOutput);

#define OUTPUT_PARAMSTRING(x, ...) { sprintf(pOutput, x, __VA_ARGS__); pOutput += strlen(pOutput); }

    if ((~value) & 0xffff)
    {
        int scale;
        uint32_t value2;

        value2 = (~value) & 0xffff;
        scale = 0;

        while ((value2 & 3) == 0)
        {
            value2>>=2;
            scale+=2;
        }

        OUTPUT_PARAMSTRING("bic %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
        value2>>=8;

        if (value2)
        {
            scale+=8;
            while ((value2 & 3) == 0)
            {
                value2>>=2;
                scale+=2;
            }

            OUTPUT_PARAMSTRING("bic %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), (value2 & 0xff) << scale);
        }
    }

    if (value)
    {
        int scale;

        scale = 0;

        while ((value & 3) == 0)
        {
            value>>=2;
            scale+=2;
        }

        OUTPUT_PARAMSTRING("orr %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), (value & 0xff) << scale);
        value>>=8;

        if (value)
        {
            scale+=8;
            while ((value & 3) == 0)
            {
                value>>=2;
                scale+=2;
            }

            OUTPUT_PARAMSTRING("orr %s, %s, #0x%x\n", ARMREGSTR(dst), ARMREGSTR(dst), (value & 0xff) << scale);
        }
    }

#undef OUTPUT_PARAMSTRING
}

static int SR_disassemble_is_operand2(int32_t value)
{
    uint32_t value2;

    if (value == 0) return 1;

    value2 = (uint32_t) value;

    while ((value2 & 3) == 0) value2>>=2;

    if ((value2 >> 8) == 0) return 1;

    value2 = (((uint32_t) value) << 8) | (((uint32_t) value) >> 24);

    while ((value2 & 3) == 0) value2>>=2;

    return (value2 >> 8)?0:1;
}

static void SR_disassemble_find_distance(unsigned int Entry, const output_data *base_output, int_fast32_t *distance, int *distalign)
{
    const output_data *output, *output2;
    int cont;

    output = base_output;
    output2 = base_output;

    if (output != NULL)
    {
        if (output->align || output->type == OT_INSTRUCTION)
        {
            cont = 0;
        }
        else
        {
            cont = 1;
        }
    }
    else
    {
        cont = 0;
    }

    while (cont)
    {
        output2 = output;
        output = section_output_list_FindEntryLower(Entry, output2->ofs);

        if (output != NULL)
        {
            if (output->align || output->type == OT_INSTRUCTION)
            {
                cont = 0;
            }
            else
            {
                cont = 1;
            }
        }
        else
        {
            cont = 0;
        }
    }

    if (output == NULL) output = output2;

    if (output->type == OT_INSTRUCTION)
    {
        *distance = (base_output->ofs - (output->ofs + output->len + 1));
        *distalign = 4;
    }
    else
    {
        *distance = (base_output->ofs - output->ofs);
        *distalign = (output->align)?(output->align):1;
    }
}

static void SR_disassemble_get_memory_address(char *ostr, enum arm_regs madrreg, const ud_operand_t *op, const fixup_data *fixup, const extrn_data *extrn, enum ud_type reg, enum madr_address_mode mode, enum extend_mode mode2, struct madr_result *result)
{
    /*
        mode:
            0 = MADR_REG   - always put memory address into madrreg register
            1 = MADR_WRITE - instruction is writing to memory
            2 = MADR_READ  - instruction is reading from memory
            3 = MADR_RW    - instruction is reading from memory and writing to memory - in case of register overwrite, put memory address into madrreg register

        mode2:
            0 = ZERO_EXTEND - zero or no extend
            1 = SIGN_EXTEND - sign extend

        result->align:
             1 - address is divisible by 1 (address modulo 4 is 0 or 1 or 2 or 3)
             2 - address is divisible by 2 (address modulo 4 is 0 or 2)
             4 - address is divisible by 4 (address modulo 4 is 0)
            -1 - address modulo 4 is 1
            -2 - address modulo 4 is 2
            -3 - address modulo 4 is 3
    */

    char cFixupLabel[64];
    int *label_value;
    output_data *output;
    int done, unaligned_access, first, lshift, numop, regalign, distalign, reg1_dword_align, reg2_dword_align;
    int_fast32_t distance;
    int32_t displacement;
    uint_fast32_t sec, ofs;

    if (esp_dword_aligned)
    {
        reg1_dword_align = (int) UD_R_ESP;
        reg2_dword_align = (ebp_dword_aligned)?((int) UD_R_EBP):-1;
    }
    else
    {
        reg1_dword_align = (ebp_dword_aligned)?((int) UD_R_EBP):-1;
        reg2_dword_align = -1;
    }

#define DWORD_ALIGN(x) ( (((int) (x)) == reg1_dword_align) || (((int) (x)) == reg2_dword_align) )

    ostr += strlen(ostr);

#define ADD_OSTR(x, ...) { sprintf(ostr, x, __VA_ARGS__); ostr += strlen(ostr); }
#define ADD_OSTR0(x) { strcpy(ostr, x); ostr += strlen(ostr); }


    result->madr[0] = 0;
    result->align = 1;
    result->memreg = AR_NONE;
    result->unaligned_madr[0] = 0;
    done = 0;
    unaligned_access = 0;

    if (mode != MADR_REG && op->size != 8 && op->size != 16 && op->size != 32 && op->size != 64)
    {
        fprintf(stderr, "Error: address size error: %i\n", op->size);
        return;
    }

    // read displacement
    if (fixup == NULL)
    {
        if (op->offset == 8)
        {
            displacement = (int32_t) op->lval.sbyte;
        }
        else if (op->offset == 32)
        {
            displacement = (int32_t) op->lval.udword;
        }
        else if (op->offset == 0)
        {
            displacement = 0;
        }
        else
        {
            fprintf(stderr, "Error: address displacement error: %i\n", op->offset);
            return;
        }
    }
    else displacement = 0;

    if (op->scale == 0 || op->scale == 1) lshift = 0;
    else if (op->scale == 2) lshift = 1;
    else if (op->scale == 4) lshift = 2;
    else if (op->scale == 8) lshift = 3;
    else
    {
        fprintf(stderr, "Error: address scale error: %i\n", op->scale);
        return;
    }

    // check dst register
    if (mode == MADR_RW)
    {
        if (reg >= UD_R_AX && reg <= UD_R_DI)
        {
            reg = (enum ud_type) (reg + UD_R_EAX - UD_R_AX);
        }
        else if (reg >= UD_R_AL && reg <= UD_R_BL)
        {
            reg = (enum ud_type) (reg + UD_R_EAX - UD_R_AL);
        }
        else if (reg >= UD_R_AH && reg <= UD_R_BH)
        {
            reg = (enum ud_type) (reg + UD_R_EAX - UD_R_AH);
        }

        if ( (op->base && op->base == reg) ||
             (op->index && op->index == reg) )
        {
            mode = MADR_REG;
        }
    }

    // try direct access using ldr/str
    if (mode != MADR_REG)
    {
        if (op->size == 8)
        {
            if (mode2)
            {
                if (op->base)
                {
                    if (op->index)
                    {
                        if (lshift == 0 && displacement == 0 && fixup == NULL)
                        {
                            sprintf(result->madr, "%s, %s", X86REGSTR(op->base), X86REGSTR(op->index));
                            result->align = (DWORD_ALIGN(op->base) && DWORD_ALIGN(op->index))?4:( (op->base == op->index)?2:1 );
                            done = 1;
                        }
                    }
                    else
                    {
                        if (fixup == NULL && displacement <= 255 && displacement >= -255)
                        {
                            if (displacement == 0)
                            {
                                sprintf(result->madr, "%s", X86REGSTR(op->base));
                                result->memreg = X862ARMREG(op->base);
                            }
                            else
                            {
                                sprintf(result->madr, "%s, #%i", X86REGSTR(op->base), displacement);
                            }
                            result->align = (DWORD_ALIGN(op->base))?( (displacement & 3)?( -(displacement & 3) ):4 ):1;
                            done = 1;
                        }
                    }
                }
                else
                {
                    if (op->index)
                    {
                        if (lshift == 0 && fixup == NULL && displacement <= 255 && displacement >= -255)
                        {
                            if (displacement == 0)
                            {
                                sprintf(result->madr, "%s", X86REGSTR(op->index));
                                result->memreg = X862ARMREG(op->index);
                            }
                            else
                            {
                                sprintf(result->madr, "%s, #%i", X86REGSTR(op->index), displacement);
                            }
                            result->align = (DWORD_ALIGN(op->index))?( (displacement & 3)?( -(displacement & 3) ):4 ):1;
                            done = 1;
                        }
                        else if (lshift == 1 && displacement == 0 && fixup == NULL)
                        {
                            sprintf(result->madr, "%s, %s", X86REGSTR(op->index), X86REGSTR(op->index));
                            result->align = (DWORD_ALIGN(op->index))?4:2;
                            done = 1;
                        }
                    }
                }
            }
            else
            {
                if (op->base)
                {
                    if (op->index)
                    {
                        if (displacement == 0 && fixup == NULL)
                        {
                            if (lshift)
                            {
                                sprintf(result->madr, "%s, %s, lsl #%i", X86REGSTR(op->base), X86REGSTR(op->index), lshift);
                                result->align = (DWORD_ALIGN(op->base))?( ( DWORD_ALIGN(op->index) || (lshift > 1) )?4:2 ):1;
                                done = 1;
                            }
                            else
                            {
                                sprintf(result->madr, "%s, %s", X86REGSTR(op->base), X86REGSTR(op->index));
                                result->align = (DWORD_ALIGN(op->base) && DWORD_ALIGN(op->index))?4:( (op->base == op->index)?2:1 );
                                done = 1;
                            }
                        }
                    }
                    else
                    {
                        if (fixup == NULL && displacement <= 4095 && displacement >= -4095)
                        {
                            if (displacement == 0)
                            {
                                sprintf(result->madr, "%s", X86REGSTR(op->base));
                                result->memreg = X862ARMREG(op->base);
                            }
                            else
                            {
                                sprintf(result->madr, "%s, #%i", X86REGSTR(op->base), displacement);
                            }
                            result->align = (DWORD_ALIGN(op->base))?( (displacement & 3)?( -(displacement & 3) ):4 ):1;
                            done = 1;
                        }
                    }
                }
                else
                {
                    if (op->index)
                    {
                        if (lshift == 0 && fixup == NULL && displacement <= 4095 && displacement >= -4095)
                        {
                            if (displacement == 0)
                            {
                                sprintf(result->madr, "%s", X86REGSTR(op->index));
                                result->memreg = X862ARMREG(op->index);
                            }
                            else
                            {
                                sprintf(result->madr, "%s, #%i", X86REGSTR(op->index), displacement);
                            }
                            result->align = (DWORD_ALIGN(op->index))?( (displacement & 3)?( -(displacement & 3) ):4 ):1;
                            done = 1;
                        }
                        else if (lshift == 1 && displacement == 0 && fixup == NULL)
                        {
                            sprintf(result->madr, "%s, %s", X86REGSTR(op->index), X86REGSTR(op->index));
                            result->align = (DWORD_ALIGN(op->index))?4:2;
                            done = 1;
                        }
                    }
                }
            }
        }
        else if (op->size == 16)
        {
            if (op->base)
            {
                if (op->index)
                {
                    if (lshift == 0 && displacement == 0 && fixup == NULL)
                    {
                        result->align = (DWORD_ALIGN(op->base) && DWORD_ALIGN(op->index))?4:( (op->base == op->index)?2:1 );
                        if ( (result->align == 2) || (result->align == 4) )
                        {
                            sprintf(result->madr, "%s, %s", X86REGSTR(op->base), X86REGSTR(op->index));
                            done = 1;
                        }
                        else
                        {
                            sprintf(result->unaligned_madr, "%s, %s", X86REGSTR(op->base), X86REGSTR(op->index));
                            unaligned_access = 1;
                        }
                    }
                }
                else
                {
                    if (fixup == NULL && displacement <= 255 && displacement >= -255)
                    {
                        result->align = (DWORD_ALIGN(op->base))?( (displacement & 3)?( -(displacement & 3) ):4 ):1;
                        if ( (result->align == 2) || (result->align == 4) || (result->align == -2) )
                        {
                            if (displacement == 0)
                            {
                                sprintf(result->madr, "%s", X86REGSTR(op->base));
                                result->memreg = X862ARMREG(op->base);
                            }
                            else
                            {
                                sprintf(result->madr, "%s, #%i", X86REGSTR(op->base), displacement);
                            }
                            done = 1;
                        }
                        else
                        {
                            if (displacement != 0)
                            {
                                sprintf(result->unaligned_madr, "%s, #%i", X86REGSTR(op->base), displacement);
                                unaligned_access = 1;
                            }
                        }
                    }
                }
            }
            else
            {
                if (op->index)
                {
                    if (lshift == 0 && fixup == NULL && displacement <= 255 && displacement >= -255)
                    {
                        result->align = (DWORD_ALIGN(op->index))?( (displacement & 3)?( -(displacement & 3) ):4 ):1;
                        if ( (result->align == 2) || (result->align == 4) || (result->align == -2) )
                        {
                            if (displacement == 0)
                            {
                                sprintf(result->madr, "%s", X86REGSTR(op->index));
                                result->memreg = X862ARMREG(op->index);
                            }
                            else
                            {
                                sprintf(result->madr, "%s, #%i", X86REGSTR(op->index), displacement);
                            }
                            done = 1;
                        }
                        else
                        {
                            if (displacement != 0)
                            {
                                sprintf(result->unaligned_madr, "%s, #%i", X86REGSTR(op->index), displacement);
                                unaligned_access = 1;
                            }
                        }
                    }
                    else if (lshift == 1 && displacement == 0 && fixup == NULL)
                    {
                        result->align = (DWORD_ALIGN(op->index))?4:2;
                        sprintf(result->madr, "%s, %s", X86REGSTR(op->index), X86REGSTR(op->index));
                        done = 1;
                    }
                }
            }
        }
        else if (op->size == 32)
        {
            if (op->base)
            {
                if (op->index)
                {
                    if (displacement == 0 && fixup == NULL)
                    {
                        if (lshift)
                        {
                            result->align = (DWORD_ALIGN(op->base))?( ( DWORD_ALIGN(op->index) || (lshift > 1) )?4:2 ):1;
                            if (result->align == 4)
                            {
                                sprintf(result->madr, "%s, %s, lsl #%i", X86REGSTR(op->base), X86REGSTR(op->index), lshift);
                                done = 1;
                            }
                            else
                            {
                                sprintf(result->unaligned_madr, "%s, %s, lsl #%i", X86REGSTR(op->base), X86REGSTR(op->index), lshift);
                                unaligned_access = 1;
                            }
                        }
                        else
                        {
                            result->align = (DWORD_ALIGN(op->base) && DWORD_ALIGN(op->index))?4:( (op->base == op->index)?2:1 );
                            if (result->align == 4)
                            {
                                sprintf(result->madr, "%s, %s", X86REGSTR(op->base), X86REGSTR(op->index));
                                done = 1;
                            }
                            else
                            {
                                sprintf(result->unaligned_madr, "%s, %s", X86REGSTR(op->base), X86REGSTR(op->index));
                                unaligned_access = 1;
                            }
                        }
                    }
                }
                else
                {
                    if (fixup == NULL && displacement <= 4095 && displacement >= -4095)
                    {
                        result->align = (DWORD_ALIGN(op->base))?( (displacement & 3)?( -(displacement & 3) ):4 ):1;
                        if (result->align == 4)
                        {
                            if (displacement == 0)
                            {
                                sprintf(result->madr, "%s", X86REGSTR(op->base));
                                result->memreg = X862ARMREG(op->base);
                            }
                            else
                            {
                                sprintf(result->madr, "%s, #%i", X86REGSTR(op->base), displacement);
                            }
                            done = 1;
                        }
                        else
                        {
                            if (displacement != 0)
                            {
                                sprintf(result->unaligned_madr, "%s, #%i", X86REGSTR(op->base), displacement);
                                unaligned_access = 1;
                            }
                        }
                    }
                }
            }
            else
            {
                if (op->index)
                {
                    if (lshift == 0 && fixup == NULL && displacement <= 4095 && displacement >= -4095)
                    {
                        result->align = (DWORD_ALIGN(op->index))?( (displacement & 3)?( -(displacement & 3) ):4 ):1;
                        if (result->align == 4)
                        {
                            if (displacement == 0)
                            {
                                sprintf(result->madr, "%s", X86REGSTR(op->index));
                                result->memreg = X862ARMREG(op->index);
                            }
                            else
                            {
                                sprintf(result->madr, "%s, #%i", X86REGSTR(op->index), displacement);
                            }
                            done = 1;
                        }
                        else
                        {
                            if (displacement != 0)
                            {
                                sprintf(result->unaligned_madr, "%s, #%i", X86REGSTR(op->index), displacement);
                                unaligned_access = 1;
                            }
                        }
                    }
                    else if (lshift == 1 && displacement == 0 && fixup == NULL)
                    {
                        result->align = (DWORD_ALIGN(op->index))?4:2;
                        if (result->align == 4)
                        {
                            sprintf(result->madr, "%s, %s", X86REGSTR(op->index), X86REGSTR(op->index));
                            done = 1;
                        }
                        else
                        {
                            sprintf(result->unaligned_madr, "%s, %s", X86REGSTR(op->index), X86REGSTR(op->index));
                            unaligned_access = 1;
                        }
                    }
                }
            }
        }

        if (!done && !unaligned_access)
        {
            if (op->base)
            {
                if (op->index == UD_NONE && fixup == NULL && displacement == 0)
                {
                    result->align = (DWORD_ALIGN(op->base))?4:1;
                    result->memreg = X862ARMREG(op->base);
                    sprintf(result->madr, "%s", X86REGSTR(op->base));
                    done = 1;
                }
            }
            else
            {
                if (op->index)
                {
                    if (lshift == 0 && fixup == NULL && displacement == 0)
                    {
                        result->align = (DWORD_ALIGN(op->index))?4:1;
                        result->memreg = X862ARMREG(op->index);
                        sprintf(result->madr, "%s", X86REGSTR(op->index));
                        done = 1;
                    }
                }
            }
        }
    }

    if (!done)
    {
        if (unaligned_access)
        {
            ADD_OSTR0(".ifndef ALLOW_UNALIGNED_MEMORY_ACCESS\n");
        }

        strcpy(result->madr, ARMREGSTR(madrreg));
        result->memreg = madrreg;
        first = 1;

        numop = ( (op->base)?1:0 ) + ( (op->index)?1:0 );

        // calculate register alignment
        if (numop == 2)
        {
            if (lshift == 0)
            {
                regalign = (DWORD_ALIGN(op->base) && DWORD_ALIGN(op->index))?4:( (op->base == op->index)?2:1 );
            }
            else
            {
                regalign = (DWORD_ALIGN(op->base))?( ( DWORD_ALIGN(op->index) || (lshift > 1) )?4:2 ):1;
            }
        }
        else if (op->base)
        {
            regalign = (DWORD_ALIGN(op->base))?4:1;
        }
        else if (op->index)
        {
            if (lshift == 0)
            {
                regalign = (DWORD_ALIGN(op->index))?4:1;
            }
            else
            {
                regalign = ( DWORD_ALIGN(op->index) || (lshift > 1) )?4:2;
            }
        }
        else
        {
            regalign = 4;
        }

        if (fixup != NULL)
        {
            if (extrn == NULL)
            {
                if (section[fixup->tsec].type == ST_CODE)
                {
                    // locate label
                    label_value = section_label_list_FindEntryEqual(fixup->tsec, fixup->tofs);

                    sec = fixup->tsec;
                    if (label_value != NULL)
                    {
                        if (SR_get_section_reladr(section[fixup->tsec].start + fixup->tofs + *label_value, &sec, &ofs))
                        {
                            output = section_output_list_FindEntryEqualOrLower(sec, ofs);
                        }
                        else output = NULL;
                    }
                    else
                    {
                        output = section_output_list_FindEntryEqualOrLower(fixup->tsec, fixup->tofs);
                    }

                    if (output != NULL)
                    {
                        if (output->type == OT_INSTRUCTION)
                        {
                            if ((fixup->tsec == sec) && (fixup->tofs == output->ofs))
                            {
                                distance = 0;
                                distalign = 4;

                                fprintf(stderr, "Warning: reading instruction - %i - %i - %i\n", (unsigned int)fixup->sofs, (unsigned int)fixup->tsec, (unsigned int)fixup->tofs);
                            }
                            else
                            {
                                distance = (int)((section[fixup->tsec].start + fixup->tofs) - (section[sec].start + output->ofs));
                                distalign = 1;

                                fprintf(stderr, "Error: reading inside of instruction - %i - %i - %i\n", (unsigned int)fixup->sofs, (unsigned int)fixup->tsec, (unsigned int)fixup->tofs);
                            }
                        }
                        else
                        {
                            SR_disassemble_find_distance(fixup->tsec, output, &distance, &distalign);

                            distance += (int)((section[fixup->tsec].start + fixup->tofs) - (section[sec].start + output->ofs));
                        }

                        if (label_value != NULL)
                        {
                            distance += *label_value;
                        }

                        if (distalign >= 4)
                        {
                            if (distalign % 4 == 0)
                            {
                                result->align = (distance & 3)?( -(distance & 3) ):4;
                            }
                            else
                            {
                                result->align = 1;
                                fprintf(stderr, "Warning: suspicious alignment: %i - %i - %i - %i\n", distalign, (unsigned int)fixup->sofs, (unsigned int)fixup->tsec, (unsigned int)fixup->tofs);
                            }
                        }
                        else if (distalign == 2)
                        {
                            result->align = (distance % 2)?1:2;
                        }
                        else
                        {
                            result->align = 1;
                        }

                    }
                    else
                    {
                        result->align = 1;
                        fprintf(stderr, "Warning: output not found - %i - %i\n", (unsigned int)fixup->tsec, (unsigned int)fixup->tofs);
                    }

                }
                else
                {
                    result->align = (fixup->tofs & 3)?( -(fixup->tofs & 3) ):4;
                }
            }
            else result->align = 4;

            first = 0;
            SR_get_fixup_label(cFixupLabel, fixup, extrn);

            if (numop == 0) done = 1;

            ADD_OSTR("LDR %s, =%s\n", (numop)?(ARMREGSTR(AR_TMPADR)):(ARMREGSTR(madrreg)), cFixupLabel);
        }
        else
        {
            result->align = (displacement & 3)?( -(displacement & 3) ):4;

            if (displacement != 0)
            {
                unsigned int len1, len2, lenmin;

                len1 = SR_get_imm_length((uint32_t) displacement);

                if (op->base || (op->index && lshift == 0) )
                {
                    len2 = SR_get_imm_length((uint32_t) (-displacement));

                    if (len1 <= 3 || len2 <= 3)
                    {
                        const char *instr;
                        int scale;
                        uint32_t value;

                        done = 1;
                        first = 0;

                        scale = 0;
                        if (len1 <= len2)
                        {
                            lenmin = len1;
                            value = (uint32_t) displacement;
                            instr = instr_add;
                        }
                        else
                        {
                            lenmin = len2;
                            value = (uint32_t) (-displacement);
                            instr = instr_sub;
                        }

                        // value != 0
                        while ((value & 3) == 0)
                        {
                            value>>=2;
                            scale+=2;
                        }

                        lenmin--;

                        if (op->base)
                        {
                            ADD_OSTR("%s %s, %s, #0x%x\n", instr, (lenmin || op->index)?ARMREGSTR(AR_TMPADR):ARMREGSTR(madrreg), X86REGSTR(op->base), (value & 0xff) << scale);
                            value>>=8;

                            while (value)
                            {
                                scale+=8;
                                while ((value & 3) == 0)
                                {
                                    value>>=2;
                                    scale+=2;
                                }

                                lenmin--;
                                ADD_OSTR("%s %s, %s, #0x%x\n", instr, (lenmin || op->index)?ARMREGSTR(AR_TMPADR):ARMREGSTR(madrreg), ARMREGSTR(AR_TMPADR), (value & 0xff) << scale);
                                value>>=8;
                            }

                            if (op->index)
                            {
                                if (lshift == 0)
                                {
                                    ADD_OSTR("add %s, %s, %s\n", ARMREGSTR(madrreg), ARMREGSTR(AR_TMPADR), X86REGSTR(op->index));
                                }
                                else
                                {
                                    ADD_OSTR("add %s, %s, %s, lsl #%i\n", ARMREGSTR(madrreg), ARMREGSTR(AR_TMPADR), X86REGSTR(op->index), lshift);
                                }
                            }
                        }
                        else
                        {
                            ADD_OSTR("%s %s, %s, #0x%x\n", instr, (lenmin)?ARMREGSTR(AR_TMPADR):ARMREGSTR(madrreg), X86REGSTR(op->index), (value & 0xff) << scale);
                            value>>=8;

                            while (value)
                            {
                                scale+=8;
                                while ((value & 3) == 0)
                                {
                                    value>>=2;
                                    scale+=2;
                                }

                                lenmin--;
                                ADD_OSTR("%s %s, %s, #0x%x\n", instr, (lenmin)?ARMREGSTR(AR_TMPADR):ARMREGSTR(madrreg), ARMREGSTR(AR_TMPADR), (value & 0xff) << scale);
                                value>>=8;
                            }
                        }
                    }
                }

                if (first)
                {
                    if (numop == 0) done = 1;

                    if (len1 <= 2)
                    {
                        int scale;
                        uint32_t value;

                        scale = 0;
                        value = (uint32_t) displacement;

                        // value != 0
                        while ((value & 3) == 0)
                        {
                            value>>=2;
                            scale+=2;
                        }

                        len1--;
                        ADD_OSTR("mov %s, #0x%x\n", (numop || len1)?ARMREGSTR(AR_TMPADR):ARMREGSTR(madrreg), (value & 0xff) << scale);
                        value>>=8;

                        if (value)
                        {
                            scale+=8;
                            while ((value & 3) == 0)
                            {
                                value>>=2;
                                scale+=2;
                            }

                            ADD_OSTR("orr %s, %s, #0x%x\n", (numop)?ARMREGSTR(AR_TMPADR):ARMREGSTR(madrreg), ARMREGSTR(AR_TMPADR), (value & 0xff) << scale);
                        }
                    }
                    else
                    {
                        ADD_OSTR("LDR %s, =0x%x\n", (numop)?ARMREGSTR(AR_TMPADR):ARMREGSTR(madrreg), displacement);
                    }
                }

            }
        }

        // calculate final alignment
        if (regalign == 1)
        {
            result->align = 1;
        }
        else if (regalign == 2)
        {
            if (result->align == 4) result->align = 2;
            else if (result->align == -2) result->align = 2;
            else if (result->align < 0) result->align = 1;
        }

        if (!done)
        {
            if (!first)
            {
                // displacement
                if (op->base && op->base == op->index && lshift == 0)
                {
                    ADD_OSTR("add %s, %s, %s, lsl #1\n", ARMREGSTR(madrreg), ARMREGSTR(AR_TMPADR), X86REGSTR(op->base));
                }
                else
                {
                    if (op->base)
                    {
                        numop--;

                        ADD_OSTR("add %s, %s, %s\n", (numop)?(ARMREGSTR(AR_TMPADR)):(ARMREGSTR(madrreg)), ARMREGSTR(AR_TMPADR), X86REGSTR(op->base));
                    }

                    if (op->index)
                    {
                        numop--;

                        if (lshift == 0)
                        {
                            ADD_OSTR("add %s, %s, %s\n", ARMREGSTR(madrreg), ARMREGSTR(AR_TMPADR), X86REGSTR(op->index));
                        }
                        else
                        {
                            ADD_OSTR("add %s, %s, %s, lsl #%i\n", ARMREGSTR(madrreg), ARMREGSTR(AR_TMPADR), X86REGSTR(op->index), lshift);
                        }
                    }
                }
            }
            else
            {
                // no displacement
                if (op->base && op->base == op->index && lshift == 0)
                {
                    first = 0;

                    ADD_OSTR("mov %s, %s, lsl #1\n", ARMREGSTR(madrreg), X86REGSTR(op->base));
                }
                else if (op->base && op->index)
                {
                    first = 0;

                    if (lshift == 0)
                    {
                        ADD_OSTR("add %s, %s, %s\n", ARMREGSTR(madrreg), X86REGSTR(op->base), X86REGSTR(op->index));
                    }
                    else
                    {
                        ADD_OSTR("add %s, %s, %s, lsl #%i\n", ARMREGSTR(madrreg), X86REGSTR(op->base), X86REGSTR(op->index), lshift);
                    }
                }
                else if (op->base)
                {
                    first = 0;

                    ADD_OSTR("mov %s, %s\n", ARMREGSTR(madrreg), X86REGSTR(op->base));
                }
                else if (op->index)
                {
                    first = 0;

                    if (lshift == 0)
                    {
                        ADD_OSTR("mov %s, %s\n", ARMREGSTR(madrreg), X86REGSTR(op->index));
                    }
                    else
                    {
                        ADD_OSTR("mov %s, %s, lsl #%i\n", ARMREGSTR(madrreg), X86REGSTR(op->index), lshift);
                    }
                }
            }
        }

        if (first)
        {
            ADD_OSTR("mov %s, #0\n", ARMREGSTR(madrreg));
        }

        if (unaligned_access)
        {
            ADD_OSTR0(".endif\n");
        }
    }
#undef ADD_OSTR0
#undef ADD_OSTR
#undef DWORD_ALIGN
}

#define SR_disassemble_get_madr(ostr, op, fixup, extrn, reg, mode, mode2, result) SR_disassemble_get_memory_address((ostr), AR_TMPADR, (op), (fixup), (extrn), (reg), (mode), (mode2), (result))

// reads AR_TMP2:AR_TMP1 from memory
// trashes AR_TMPLR, AR_TMP3, AR_TMP9
static void SR_disassemble_read_mem_doubleword(char *cResult, struct madr_result *res, uint_fast32_t preserved_flags)
{
    int len;

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;
#define ADDRESSTR(x) strcpy(cResult, (x)); len = strlen(cResult); cResult += len;

    if (res->align == 4)
    {
        ADDRESULT("ldr %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
        ADDRESULT("ldr %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), res->madr);
    }
    else
    {
        ADDRESSTR(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

        ADDRESULT("ldr %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
        ADDRESULT("ldr %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), res->madr);

        ADDRESSTR(".else\n");

        if (res->align == 2)
        {
            ADDRESULT("ldrh %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
            ADDRESULT("ldrh %s, [%s, #2]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("ldrh %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), res->madr);
            ADDRESULT("ldrh %s, [%s, #6]\n", ARMREGSTR(AR_TMPLR), res->madr);
            ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMP3));
            ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMPLR));
        }
        else if (res->align == -2)
        {
            ADDRESULT("ldrh %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
            ADDRESULT("ldr %s, [%s, #2]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("ldrh %s, [%s, #6]\n", ARMREGSTR(AR_TMPLR), res->madr);
            ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMP3));
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMP3));
            ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMPLR));
        }
        else if (res->align == -3)
        {
            ADDRESULT("ldrb %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
            ADDRESULT("ldr %s, [%s, #1]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("ldr %s, [%s, #5]\n", ARMREGSTR(AR_TMPLR), res->madr);
            ADDRESULT("orr %s, %s, %s, lsl #8\n", ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMP3));
            ADDRESULT("mov %s, %s, lsr #24\n", ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMP3));
            ADDRESULT("orr %s, %s, %s, lsl #8\n", ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMPLR));
        }
        else if (res->align == -1)
        {
            ADDRESULT("ldr %s, [%s, #(-1)]\n", ARMREGSTR(AR_TMP1), res->madr);
            ADDRESULT("ldr %s, [%s, #3]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("ldrb %s, [%s, #7]\n", ARMREGSTR(AR_TMPLR), res->madr);
            ADDRESULT("mov %s, %s, lsr #8\n", ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMP1));
            ADDRESULT("orr %s, %s, %s, lsl #24\n", ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMP3));
            ADDRESULT("mov %s, %s, lsr #8\n", ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMP3));
            ADDRESULT("orr %s, %s, %s, lsl #24\n", ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMPLR));
        }
        else
        {
            if (preserved_flags & (FL_CARRY | FL_ZERO | FL_SIGN))
            {
                if (res->memreg != AR_TMPADR)
                {
                    ADDRESULT("mov %s, %s\n", ARMREGSTR(AR_TMPADR), res->madr);
                }
                ADDRESSTR(".ifdef ALLOW_UNALIGNED_PC\n");
                ADDRESULT("and %s, %s, #3\n", ARMREGSTR(AR_TMPLR), res->madr);
                ADDRESULT("sub eip, eip, %s\n", ARMREGSTR(AR_TMPLR));
                ADDRESSTR(".endif\n");
                ADDRESSTR("bl mem_read_dword_tmp1_tmp2\n");
                ADDRESULT("ldr %s, [%s]\n", ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMPADR));
                ADDRESULT("ldr %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMPADR));
            }
            else
            {
                ADDRESULT("tst %s, #3\n", res->madr);
                if (res->memreg != AR_TMPADR)
                {
                    ADDRESULT("movne %s, %s\n", ARMREGSTR(AR_TMPADR), res->madr);
                }
                ADDRESSTR("blne mem_read_dword_tmp1_tmp2\n");
                ADDRESULT("ldr %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
                ADDRESULT("ldr %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), res->madr);
            }
        }

        ADDRESSTR(".endif\n");
    }

#undef ADDRESSTR
#undef ADDRESULT
}

// trashes AR_TMPLR, AR_TMP9 (if dst != AR_TMP9), AR_TMP2 (or AR_TMP1 if dst == AR_TMP2), AR_TMP3 (or AR_TMP1 if dst == AR_TMP3)
static void SR_disassemble_read_mem_word(char *cResult, struct madr_result *res, enum arm_regs dst, uint_fast32_t preserved_flags)
{
/*
    dst != AR_TMPADR
    dst != AR_TMPLR
*/
    int len;
    enum arm_regs tmpreg1, tmpreg2;

    if (dst == AR_TMPADR || dst == AR_TMPLR)
    {
        fprintf(stderr, "Error: memory read register error: %s\n", ARMREGSTR(dst));
        return;
    }

    tmpreg1 = (dst == AR_TMP2)?AR_TMP1:AR_TMP2;
    tmpreg2 = (dst == AR_TMP3)?AR_TMP1:AR_TMP3;
    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;
#define ADDRESSTR(x) strcpy(cResult, (x)); len = strlen(cResult); cResult += len;

    if (res->align == 4)
    {
        ADDRESULT("ldr %s, [%s]\n", ARMREGSTR(dst), res->madr);
    }
    else
    {
        ADDRESSTR(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

        ADDRESULT("ldr %s, [%s]\n", ARMREGSTR(dst), (res->unaligned_madr[0])?(res->unaligned_madr):(res->madr));

        ADDRESSTR(".else\n");

        if ( (res->align == 2) || (res->align == -2) )
        {
            ADDRESULT("ldrh %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
            ADDRESULT("ldrh %s, [%s, #2]\n", ARMREGSTR(tmpreg2), res->madr);
            ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(dst), ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg2));
        }
        else if (res->align == -3)
        {
            ADDRESULT("ldrb %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
            ADDRESULT("ldr %s, [%s, #1]\n", ARMREGSTR(tmpreg2), res->madr);
            ADDRESULT("orr %s, %s, %s, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg2));
        }
        else if (res->align == -1)
        {
            ADDRESULT("ldr %s, [%s, #(-1)]\n", ARMREGSTR(tmpreg1), res->madr);
            ADDRESULT("ldrb %s, [%s, #3]\n", ARMREGSTR(tmpreg2), res->madr);
            ADDRESULT("mov %s, %s, lsr #8\n", ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1));
            ADDRESULT("orr %s, %s, %s, lsl #24\n", ARMREGSTR(dst), ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg2));
        }
        else
        {
            if (preserved_flags & (FL_CARRY | FL_ZERO | FL_SIGN))
            {
                if (res->memreg != AR_TMPADR)
                {
                    ADDRESULT("mov %s, %s\n", ARMREGSTR(AR_TMPADR), res->madr);
                }
                ADDRESSTR(".ifdef ALLOW_UNALIGNED_PC\n");
                ADDRESULT("and %s, %s, #3\n", ARMREGSTR(AR_TMPLR), res->madr);
                ADDRESULT("sub eip, eip, %s\n", ARMREGSTR(AR_TMPLR));
                ADDRESSTR(".endif\n");
                ADDRESULT("bl mem_read_word_%s\n", ARMREGSTR(dst));
                ADDRESULT("ldr %s, [%s]\n", ARMREGSTR(dst), ARMREGSTR(AR_TMPADR));
            }
            else
            {
                ADDRESULT("tst %s, #3\n", res->madr);
                if (res->memreg != AR_TMPADR)
                {
                    ADDRESULT("movne %s, %s\n", ARMREGSTR(AR_TMPADR), res->madr);
                }
                ADDRESULT("blne mem_read_word_%s\n", ARMREGSTR(dst));
                ADDRESULT("ldr %s, [%s]\n", ARMREGSTR(dst), res->madr);
            }
        }

        ADDRESSTR(".endif\n");
    }

#undef ADDRESSTR
#undef ADDRESULT
}

// trashes tmpreg1, tmpreg2
static void SR_disassemble_read_mem_halfword(char *cResult, struct madr_result *res, enum arm_regs dst, enum arm_regs tmpreg1, enum arm_regs tmpreg2, enum halfword_read_mode mode)
{
    /*
        mode:
            0 = READ16TO16     - target is 16-bit
            1 = READ16TO32SIGN - target is 32-bit sign-extended
            2 = READ16TO32ZERO - target is 32-bit zero-extended

        dst != tmpreg1
        dst != tmpreg2
        dst != AR_EIP
        tmpreg1 != tmpreg2
    */
    int len;

    if (dst == AR_EIP || dst == tmpreg1 || dst == tmpreg2 || tmpreg1 == tmpreg2)
    {
        fprintf(stderr, "Error: memory read register error: %s\n", ARMREGSTR(dst));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;
#define ADDRESSTR(x) strcpy(cResult, (x)); len = strlen(cResult); cResult += len;

    if ( (res->align == 4) || (res->align == 2) || (res->align == -2))
    {
        if (mode == READ16TO32ZERO)
        {
            ADDRESULT("ldrh %s, [%s]\n", ARMREGSTR(dst), res->madr);
        }
        else if (mode == READ16TO32SIGN)
        {
            ADDRESULT("ldrsh %s, [%s]\n", ARMREGSTR(dst), res->madr);
        }
        else
        {
            ADDRESULT("ldrh %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
            ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(dst), ARMREGSTR(tmpreg1), ARMREGSTR(dst));
        }
    }
    else
    {
        if (mode == READ16TO32ZERO)
        {
            ADDRESSTR(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

            ADDRESULT("ldrh %s, [%s]\n", ARMREGSTR(dst), (res->unaligned_madr[0])?(res->unaligned_madr):(res->madr));

            ADDRESSTR(".else\n");

            ADDRESULT("ldrb %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
            ADDRESULT("ldrb %s, [%s, #1]\n", ARMREGSTR(dst), res->madr);
            ADDRESULT("orr %s, %s, %s, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(tmpreg1), ARMREGSTR(dst));

            ADDRESSTR(".endif\n");
        }
        else if (mode == READ16TO32SIGN)
        {
            ADDRESSTR(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

            ADDRESULT("ldrsh %s, [%s]\n", ARMREGSTR(dst), (res->unaligned_madr[0])?(res->unaligned_madr):(res->madr));

            ADDRESSTR(".else\n");

            ADDRESULT("ldrb %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
            ADDRESULT("ldrsb %s, [%s, #1]\n", ARMREGSTR(dst), res->madr);
            ADDRESULT("orr %s, %s, %s, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(tmpreg1), ARMREGSTR(dst));

            ADDRESSTR(".endif\n");
        }
        else
        {
            ADDRESSTR(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

            ADDRESULT("ldrh %s, [%s]\n", ARMREGSTR(tmpreg1), (res->unaligned_madr[0])?(res->unaligned_madr):(res->madr));
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
            ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(dst), ARMREGSTR(tmpreg1), ARMREGSTR(dst));

            ADDRESSTR(".else\n");

            ADDRESULT("ldrb %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
            ADDRESULT("ldrb %s, [%s, #1]\n", ARMREGSTR(tmpreg2), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
            ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(dst), ARMREGSTR(tmpreg1), ARMREGSTR(dst));
            ADDRESULT("orr %s, %s, %s, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(tmpreg2));

            ADDRESSTR(".endif\n");
        }
    }

#undef ADDRESSTR
#undef ADDRESULT
}

// trashes tmpreg1
static void SR_disassemble_read_mem_byte(char *cResult, struct madr_result *res, enum arm_regs dst, enum arm_regs tmpreg1, enum byte_read_mode mode)
{
    /*
        mode:
            0 = READ8TO8LOW   - target is 8-bit
            1 = READ8TO8HIGH  - target is 8-bit (high)
            2 = READ8TO16SIGN - target is 16-bit sign-extended
            3 = READ8TO16ZERO - target is 16-bit zero-extended
            4 = READ8TO32SIGN - target is 32-bit sign-extended
            5 = READ8TO32ZERO - target is 32-bit zero-extended

        dst != tmpreg1
        dst != AR_EIP
    */
    int len;

    if (dst == AR_EIP || dst == tmpreg1)
    {
        fprintf(stderr, "Error: memory read register error: %s\n", ARMREGSTR(dst));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;

    if (mode == READ8TO32ZERO)
    {
        ADDRESULT("ldrb %s, [%s]\n", ARMREGSTR(dst), res->madr);
    }
    else if (mode == READ8TO32SIGN)
    {
        ADDRESULT("ldrsb %s, [%s]\n", ARMREGSTR(dst), res->madr);
    }
    else if (mode == READ8TO16ZERO)
    {
        ADDRESULT("ldrb %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
        ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst));
        ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(dst), ARMREGSTR(tmpreg1), ARMREGSTR(dst));
    }
    else if (mode == READ8TO16SIGN)
    {
        ADDRESULT("ldrsb %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
        ADDRESULT("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
        ADDRESULT("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
        ADDRESULT("mov %s, %s, lsl #16\n", ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1));
        ADDRESULT("orr %s, %s, %s, lsr #16\n", ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(tmpreg1));
    }
    else if (mode == READ8TO8HIGH)
    {
        ADDRESULT("ldrb %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
        ADDRESULT("bic %s, %s, #0xff00\n", ARMREGSTR(dst), ARMREGSTR(dst));
        ADDRESULT("orr %s, %s, %s, lsl #8\n", ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(tmpreg1));
    }
    else
    {
        ADDRESULT("ldrb %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
        ADDRESULT("bic %s, %s, #0x00ff\n", ARMREGSTR(dst), ARMREGSTR(dst));
        ADDRESULT("orr %s, %s, %s\n", ARMREGSTR(dst), ARMREGSTR(dst), ARMREGSTR(tmpreg1));
    }

#undef ADDRESULT
}

// writes AR_TMP2:AR_TMP1 into memory
// trashes AR_TMPLR, AR_TMP3
static void SR_disassemble_write_mem_doubleword(char *cResult, struct madr_result *res, uint_fast32_t preserved_flags)
{
    int len;

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, (x), __VA_ARGS__); len = strlen(cResult); cResult += len;
#define ADDRESSTR(x) strcpy(cResult, (x)); len = strlen(cResult); cResult += len;

    if (res->align == 4)
    {
        ADDRESULT("str %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
        ADDRESULT("str %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), res->madr);
    }
    else
    {
        ADDRESSTR(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

        ADDRESULT("str %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
        ADDRESULT("str %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), res->madr);

        ADDRESSTR(".else\n");

        if (res->align == 2)
        {
            ADDRESULT("strh %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP1));
            ADDRESULT("strh %s, [%s, #2]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("strh %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP2));
            ADDRESULT("strh %s, [%s, #6]\n", ARMREGSTR(AR_TMP3), res->madr);
        }
        else if (res->align == -2)
        {
            ADDRESULT("strh %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP1));
            ADDRESULT("orr %s, %s, %s, lsl #16\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP2));
            ADDRESULT("str %s, [%s, #2]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP2));
            ADDRESULT("strh %s, [%s, #6]\n", ARMREGSTR(AR_TMP3), res->madr);
        }
        else if (res->align == -3)
        {
            ADDRESULT("strb %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
            ADDRESULT("mov %s, %s, lsr #8\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP1));
            ADDRESULT("orr %s, %s, %s, lsl #24\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP2));
            ADDRESULT("str %s, [%s, #1]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("mov %s, %s, lsr #8\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP2));
            ADDRESULT("strh %s, [%s, #5]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP3));
            ADDRESULT("strb %s, [%s, #7]\n", ARMREGSTR(AR_TMP3), res->madr);
        }
        else if (res->align == -1)
        {
            ADDRESULT("strb %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
            ADDRESULT("mov %s, %s, lsr #8\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP1));
            ADDRESULT("strh %s, [%s, #1]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP3));
            ADDRESULT("orr %s, %s, %s, lsl #8\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP2));
            ADDRESULT("str %s, [%s, #3]\n", ARMREGSTR(AR_TMP3), res->madr);
            ADDRESULT("mov %s, %s, lsr #24\n", ARMREGSTR(AR_TMP3), ARMREGSTR(AR_TMP2));
            ADDRESULT("strb %s, [%s, #7]\n", ARMREGSTR(AR_TMP3), res->madr);
        }
        else
        {
            if (preserved_flags & (FL_CARRY | FL_ZERO | FL_SIGN))
            {
                if (res->memreg != AR_TMPADR)
                {
                    ADDRESULT("mov %s, %s\n", ARMREGSTR(AR_TMPADR), res->madr);
                }
                ADDRESSTR(".ifdef ALLOW_UNALIGNED_PC\n");
                ADDRESULT("and %s, %s, #3\n", ARMREGSTR(AR_TMPLR), res->madr);
                ADDRESULT("sub eip, eip, %s\n", ARMREGSTR(AR_TMPLR));
                ADDRESSTR(".endif\n");
                ADDRESSTR("bl mem_write_dword_tmp1_tmp2\n");
                ADDRESULT("str %s, [%s]\n", ARMREGSTR(AR_TMP1), ARMREGSTR(AR_TMPADR));
                ADDRESULT("str %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), ARMREGSTR(AR_TMPADR));
            }
            else
            {
                ADDRESULT("tst %s, #3\n", res->madr);
                if (res->memreg != AR_TMPADR)
                {
                    ADDRESULT("movne %s, %s\n", ARMREGSTR(AR_TMPADR), res->madr);
                }
                ADDRESSTR("blne mem_write_dword_tmp1_tmp2\n");
                ADDRESULT("str %s, [%s]\n", ARMREGSTR(AR_TMP1), res->madr);
                ADDRESULT("str %s, [%s, #4]\n", ARMREGSTR(AR_TMP2), res->madr);
            }
        }

        ADDRESSTR(".endif\n");
    }

#undef ADDRESSTR
#undef ADDRESULT
}

// trashes AR_TMPLR, AR_TMP3 (or AR_TMP2 if src == AR_TMP3)
static void SR_disassemble_write_mem_word(char *cResult, struct madr_result *res, enum arm_regs src, uint_fast32_t preserved_flags)
{
/*
    src != AR_TMPADR
    src != AR_TMPLR
    src != AR_EIP
*/
    int len;
    enum arm_regs tmpreg1;

    if (src == AR_TMPADR || src == AR_TMPLR || src == AR_EIP)
    {
        fprintf(stderr, "Error: memory write register error: %s\n", ARMREGSTR(src));
        return;
    }

    tmpreg1 = (src == AR_TMP3)?AR_TMP2:AR_TMP3;

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, (x), __VA_ARGS__); len = strlen(cResult); cResult += len;
#define ADDRESSTR(x) strcpy(cResult, (x)); len = strlen(cResult); cResult += len;

    if (res->align == 4)
    {
        ADDRESULT("str %s, [%s]\n", ARMREGSTR(src), res->madr);
    }
    else
    {
        ADDRESSTR(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

        ADDRESULT("str %s, [%s]\n", ARMREGSTR(src), (res->unaligned_madr[0])?(res->unaligned_madr):(res->madr));

        ADDRESSTR(".else\n");

        if ( (res->align == 2) || (res->align == -2) )
        {
            ADDRESULT("strh %s, [%s]\n", ARMREGSTR(src), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(tmpreg1), ARMREGSTR(src));
            ADDRESULT("strh %s, [%s, #2]\n", ARMREGSTR(tmpreg1), res->madr);
        }
        else if ( (res->align == -1) || (res->align == -3))
        {
            ADDRESULT("strb %s, [%s]\n", ARMREGSTR(src), res->madr);
            ADDRESULT("mov %s, %s, lsr #8\n", ARMREGSTR(tmpreg1), ARMREGSTR(src));
            ADDRESULT("strh %s, [%s, #1]\n", ARMREGSTR(tmpreg1), res->madr);
            ADDRESULT("mov %s, %s, lsr #16\n", ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1));
            ADDRESULT("strb %s, [%s, #3]\n", ARMREGSTR(tmpreg1), res->madr);
        }
        else
        {
            if (preserved_flags & (FL_CARRY | FL_ZERO | FL_SIGN))
            {
                if (res->memreg != AR_TMPADR)
                {
                    ADDRESULT("mov %s, %s\n", ARMREGSTR(AR_TMPADR), res->madr);
                }
                ADDRESSTR(".ifdef ALLOW_UNALIGNED_PC\n");
                ADDRESULT("and %s, %s, #3\n", ARMREGSTR(AR_TMPLR), res->madr);
                ADDRESULT("sub eip, eip, %s\n", ARMREGSTR(AR_TMPLR));
                ADDRESSTR(".endif\n");
                ADDRESULT("bl mem_write_word_%s\n", ARMREGSTR(src));
                ADDRESULT("str %s, [%s]\n", ARMREGSTR(src), ARMREGSTR(AR_TMPADR));
            }
            else
            {
                ADDRESULT("tst %s, #3\n", res->madr);
                if (res->memreg != AR_TMPADR)
                {
                    ADDRESULT("movne %s, %s\n", ARMREGSTR(AR_TMPADR), res->madr);
                }
                ADDRESULT("blne mem_write_word_%s\n", ARMREGSTR(src));
                ADDRESULT("str %s, [%s]\n", ARMREGSTR(src), res->madr);
            }
        }

        ADDRESSTR(".endif\n");
    }

#undef ADDRESSTR
#undef ADDRESULT
}

// trashes tmpreg1
static void SR_disassemble_write_mem_halfword(char *cResult, struct madr_result *res, enum arm_regs src, enum arm_regs tmpreg1)
{
    /*
        src != tmpreg1
        src != AR_EIP
    */
    int len;

    if (src == AR_EIP || src == tmpreg1)
    {
        fprintf(stderr, "Error: memory write register error: %s\n", ARMREGSTR(src));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, (x), __VA_ARGS__); len = strlen(cResult); cResult += len;
#define ADDRESSTR(x) strcpy(cResult, (x)); len = strlen(cResult); cResult += len;

    if ( (res->align == 4) || (res->align == 2) || (res->align == -2) )
    {
        ADDRESULT("strh %s, [%s]\n", ARMREGSTR(src), res->madr);
    }
    else
    {
        ADDRESSTR(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

        ADDRESULT("strh %s, [%s]\n", ARMREGSTR(src), (res->unaligned_madr[0])?(res->unaligned_madr):(res->madr));

        ADDRESSTR(".else\n");

        ADDRESULT("strb %s, [%s]\n", ARMREGSTR(src), res->madr);
        ADDRESULT("mov %s, %s, lsr #8\n", ARMREGSTR(tmpreg1), ARMREGSTR(src));
        ADDRESULT("strb %s, [%s, #1]\n", ARMREGSTR(tmpreg1), res->madr);

        ADDRESSTR(".endif\n");
    }

#undef ADDRESSTR
#undef ADDRESULT
}

// trashes tmpreg1
static void SR_disassemble_write_mem_byte(char *cResult, struct madr_result *res, enum arm_regs src, enum arm_regs tmpreg1, enum byte_write_mode mode)
{
    /*
        mode:
            0 = WRITE8LOW  - source is 8-bit (low)
            1 = WRITE8HIGH - source is 8-bit (high)

        src != tmpreg1
        src != AR_EIP
    */
    int len;

    if (src == AR_EIP || src == tmpreg1)
    {
        fprintf(stderr, "Error: memory write register error: %s\n", ARMREGSTR(src));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, (x), __VA_ARGS__); len = strlen(cResult); cResult += len;

    if (mode == WRITE8HIGH)
    {
        ADDRESULT("mov %s, %s, lsr #8\n", ARMREGSTR(tmpreg1), ARMREGSTR(src));
        ADDRESULT("strb %s, [%s]\n", ARMREGSTR(tmpreg1), res->madr);
    }
    else
    {
        ADDRESULT("strb %s, [%s]\n", ARMREGSTR(src), res->madr);
    }

#undef ADDRESULT
}

static uint32_t SR_disassemble_get_value(const ud_operand_t *op, enum extend_mode mode)
{
    /*
        mode:
            0 = ZERO_EXTEND - zero or no extend
            1 = SIGN_EXTEND - sign extend
    */
    if (mode == SIGN_EXTEND)
    {
        if (op->size == 8)
        {
            return (uint32_t) ( (int32_t) op->lval.sbyte );
        }
        else if (op->size == 16)
        {
            return (uint32_t) ( (int32_t) op->lval.sword );
        }
        else if (op->size == 32)
        {
            return op->lval.udword;
        }
        else
        {
            fprintf(stderr, "Error: value size error: %i\n", op->size);
            return 0;
        }
    }
    else
    {
        if (op->size == 8)
        {
            return (uint32_t) op->lval.ubyte;
        }
        else if (op->size == 16)
        {
            return (uint32_t) op->lval.uword;
        }
        else if (op->size == 32)
        {
            return op->lval.udword;
        }
        else
        {
            fprintf(stderr, "Error: value size error: %i\n", op->size);
            return 0;
        }
    }
}

// trashes tmpreg1
static void SR_disassemble_change_arm_flags(char *cResult, uint_fast32_t toclear, uint_fast32_t toset, uint_fast32_t toinvert, enum arm_regs tmpreg1)
{
    uint_fast32_t flags;
    int len;

    if ( ( (toclear | toset | toinvert) & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW) ) == 0) return;

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;

    ADDRESULT("mrs %s, cpsr\n", ARMREGSTR(tmpreg1));

    if (toclear)
    {
        flags = 0;
        if (toclear & FL_CARRY) flags |= 0x20000000;
        if (toclear & FL_ZERO) flags |= 0x40000000;
        if (toclear & FL_SIGN) flags |= 0x80000000;
        if (toclear & FL_OVERFLOW) flags |= 0x10000000;

        ADDRESULT("bic %s, %s, #0x%x\n", ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1), (unsigned int)flags);
    }

    if (toset)
    {
        flags = 0;
        if (toset & FL_CARRY) flags |= 0x20000000;
        if (toset & FL_ZERO) flags |= 0x40000000;
        if (toset & FL_SIGN) flags |= 0x80000000;
        if (toset & FL_OVERFLOW) flags |= 0x10000000;

        ADDRESULT("orr %s, %s, #0x%x\n", ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1), (unsigned int)flags);
    }

    if (toinvert)
    {
        flags = 0;
        if (toinvert & FL_CARRY) flags |= 0x20000000;
        if (toinvert & FL_ZERO) flags |= 0x40000000;
        if (toinvert & FL_SIGN) flags |= 0x80000000;
        if (toinvert & FL_OVERFLOW) flags |= 0x10000000;

        ADDRESULT("eor %s, %s, #0x%x\n", ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1), (unsigned int)flags);
    }

    ADDRESULT("msr cpsr_f, %s\n", ARMREGSTR(tmpreg1));

#undef ADDRESULT
}

// trashes opertmp
static void SR_disassemble_calculate_parity_adjust_flags(char *cResult, enum arm_regs res, enum arm_regs opertmp, uint_fast32_t opershift, enum ud_mnemonic_code mnemonic, uint_fast32_t tocalculate)
{
    /*
        res != opertmp
    */
    int len;

    if ((tocalculate & (FL_ADJUST | FL_PARITY)) == 0) return;

    if (res == opertmp)
    {
        fprintf(stderr, "Error: calculate parity/adjust flag register error: %s\n", ARMREGSTR(res));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;

    if (tocalculate & FL_ADJUST)
    {
        ADDRESULT("bic eflags, eflags, #0x%x\n", 16);

        if (mnemonic == UD_Iadd || mnemonic == UD_Isub || mnemonic == UD_Icmp || mnemonic == UD_Iscasb || mnemonic == UD_Iadc || mnemonic == UD_Isbb)
        {
            if (opershift)
            {
                ADDRESULT("eor %s, %s, %s, lsr #%i\n", ARMREGSTR(opertmp), ARMREGSTR(res), ARMREGSTR(opertmp), (unsigned int)opershift);
            }
            else
            {
                ADDRESULT("eor %s, %s, %s\n", ARMREGSTR(opertmp), ARMREGSTR(res), ARMREGSTR(opertmp));
            }
            ADDRESULT("and %s, %s, #0x10\n", ARMREGSTR(opertmp), ARMREGSTR(opertmp));

            ADDRESULT("orr eflags, eflags, %s\n", ARMREGSTR(opertmp));
        }
        else if (mnemonic == UD_Ineg)
        {
            ADDRESULT("rsb %s, %s, #0\n", ARMREGSTR(opertmp), ARMREGSTR(res));

            ADDRESULT("eor %s, %s, %s\n", ARMREGSTR(opertmp), ARMREGSTR(res), ARMREGSTR(opertmp));

            ADDRESULT("and %s, %s, #0x10\n", ARMREGSTR(opertmp), ARMREGSTR(opertmp));

            ADDRESULT("orr eflags, eflags, %s\n", ARMREGSTR(opertmp));
        }
        else if (mnemonic == UD_Iinc)
        {
            ADDRESULT("sub %s, %s, #1\n", ARMREGSTR(opertmp), ARMREGSTR(res));

            ADDRESULT("eor %s, %s, %s\n", ARMREGSTR(opertmp), ARMREGSTR(res), ARMREGSTR(opertmp));

            ADDRESULT("and %s, %s, #0x10\n", ARMREGSTR(opertmp), ARMREGSTR(opertmp));

            ADDRESULT("orr eflags, eflags, %s\n", ARMREGSTR(opertmp));
        }
        else if (mnemonic == UD_Idec)
        {
            ADDRESULT("add %s, %s, #1\n", ARMREGSTR(opertmp), ARMREGSTR(res));

            ADDRESULT("eor %s, %s, %s\n", ARMREGSTR(opertmp), ARMREGSTR(res), ARMREGSTR(opertmp));

            ADDRESULT("and %s, %s, #0x10\n", ARMREGSTR(opertmp), ARMREGSTR(opertmp));

            ADDRESULT("orr eflags, eflags, %s\n", ARMREGSTR(opertmp));
        }
        else
        {
            fprintf(stderr, "Error: calculate adjust flag mnemonic error: %i\n", (int) mnemonic);
            return;
        }
    }

    if (tocalculate & FL_PARITY)
    {
        ADDRESULT("orr eflags, eflags, #0x%x\n", 4);

        ADDRESULT("eor %s, %s, %s, lsr #4\n", ARMREGSTR(opertmp), ARMREGSTR(res), ARMREGSTR(res));

        ADDRESULT("eor %s, %s, %s, lsr #2\n", ARMREGSTR(opertmp), ARMREGSTR(opertmp), ARMREGSTR(opertmp));

        ADDRESULT("eor %s, %s, %s, lsr #1\n", ARMREGSTR(opertmp), ARMREGSTR(opertmp), ARMREGSTR(opertmp));

        ADDRESULT("and %s, %s, #1\n", ARMREGSTR(opertmp), ARMREGSTR(opertmp));

        ADDRESULT("eor eflags, eflags, %s, lsl #2\n", ARMREGSTR(opertmp));
    }

#undef ADDRESULT
}

// trashes tmpreg1
static void SR_disassemble_calculate_parity_flag(char *cResult, enum arm_regs res, enum arm_regs tmpreg1, uint_fast32_t opershift)
{
    /*
        res != tmpreg1
    */
    int len;

    if (res == tmpreg1)
    {
        fprintf(stderr, "Error: calculate parity flag register error: %s\n", ARMREGSTR(res));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;

    ADDRESULT("eor %s, %s, %s, lsr #4\n", ARMREGSTR(tmpreg1), ARMREGSTR(res), ARMREGSTR(res));

    ADDRESULT("eor %s, %s, %s, lsr #2\n", ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1));

    ADDRESULT("eor %s, %s, %s, lsr #1\n", ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1));

    ADDRESULT("and %s, %s, #0x%x\n", ARMREGSTR(tmpreg1), ARMREGSTR(tmpreg1), (uint32_t) 1 << opershift);

    ADDRESULT("orr eflags, eflags, #0x%x\n", 4);

    if (opershift < 2)
    {
        ADDRESULT("eor eflags, eflags, %s, lsl #%i\n", ARMREGSTR(tmpreg1), (unsigned int)(2 - opershift));
    }
    else if (opershift > 2)
    {
        ADDRESULT("eor eflags, eflags, %s, lsr #%i\n", ARMREGSTR(tmpreg1), (unsigned int)(opershift - 2));
    }
    else
    {
        ADDRESULT("eor eflags, eflags, %s\n", ARMREGSTR(tmpreg1));
    }

#undef ADDRESULT
}

#include "SR_full_arm_instr_helper.h"

int SR_disassemble_arm_instruction(unsigned int Entry, output_data *output, uint_fast32_t flags_to_write, uint_fast32_t *pflags_write, uint_fast32_t *pflags_read)
{
    char cOutput[8192];
    char cOutPart[128];
    char cAddress[128];
    char cLabel[128];
    fixup_data *fixup1, *fixup2, *fixup[3];
    extrn_data *extrn[3];
    uint_fast32_t cur_ofs, flags_write, flags_read;
    unsigned int decoded_length;
    char *updcond;
    struct madr_result memadr;

    cur_ofs = output->ofs;
    decoded_length = ud_insn_len(&ud_obj);
    flags_write = *pflags_write;
    flags_read = *pflags_read;
    Tflags_to_write = flags_to_write;


    fixup[0] = NULL; fixup[1] = NULL; fixup[2] = NULL;
    extrn[0] = NULL; extrn[1] = NULL; extrn[2] = NULL;

    fixup2 = NULL;
    fixup1 = section_fixup_list_FindEntryEqualOrHigher(Entry, cur_ofs);

    if (fixup1 != NULL)
    {
        if (fixup1->sofs >= cur_ofs + decoded_length) fixup1 = NULL;
        else
        {
            if (fixup1->sofs > cur_ofs + decoded_length - 4)
            {
                fprintf(stderr, "Error: decoding fixup mismatch - %i - %i\n", Entry, (unsigned int)cur_ofs);

                return 4;
            }

            if (fixup1->sofs != cur_ofs + decoded_length - 4)
            {
                fixup2 = section_fixup_list_FindEntryHigher(Entry, fixup1->sofs);

                if (fixup2 != NULL)
                {
                    if (fixup2->sofs >= cur_ofs + decoded_length) fixup2 = NULL;
                    else
                    {
                        if (fixup2->sofs > cur_ofs + decoded_length - 4)
                        {
                            fprintf(stderr, "Error: decoding fixup mismatch - %i - %i\n", Entry, (unsigned int)cur_ofs);

                            return 4;
                        }
                    }
                }
            }
        }

        if (fixup1 != NULL)
        {
            int i, j;

            i = SR_disassemble_fixup_operand(Entry, ud_insn_asm(&ud_obj), fixup1, cur_ofs, decoded_length);

            if (i == 0)
            {
                fprintf(stderr, "Error: fixup operand mismatch - %i - %i\n", Entry, (unsigned int)cur_ofs);

                return 6;
            }

            fixup[i - 1] = fixup1;


            if (fixup2 != NULL)
            {
                j = SR_disassemble_fixup_operand(Entry, ud_insn_asm(&ud_obj), fixup2, cur_ofs, decoded_length);

                if (j == 0 || j == i)
                {
                    fprintf(stderr, "Error: fixup operand mismatch - %i - %i\n", Entry, (unsigned int)cur_ofs);

                    return 6;
                }

                fixup[j - 1] = fixup2;
            }

            for (i = 0; i <= 2; i++)
            {
                if (fixup[i] != NULL)
                {
                    extrn[i] = section_extrn_list_FindEntryEqual(fixup[i]->tsec, fixup[i]->tofs);
                }
            }
        }
    }

    cOutput[0] = 0;
    pOutput = &(cOutput[0]);

#define OUTPUT_PARAMSTRING(x, ...) sprintf(cOutPart, x, __VA_ARGS__); strcat(cOutput, cOutPart);
#define OUTPUT_STRING(x) strcat(cOutput, x)

    switch (ud_obj.mnemonic)
    {
        case UD_Iaam:
            {
                /* SF,ZF,PF - modified, CF,OF,AF - undefined */

                uint32_t value;

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.operand[0].type == UD_NONE || ud_obj.operand[0].type == UD_OP_IMM)
                {
                    if (ud_obj.operand[0].type == UD_OP_IMM)
                    {
                        value = (uint32_t) ud_obj.operand[0].lval.ubyte;
                    }
                    else
                    {
                        value = 10;
                    }

                    OUTPUT_PARAMSTRING("mov tmp2, #%i\n", value);
                    OUTPUT_STRING("and tmp1, eax, #0xff\n");
                    OUTPUT_STRING("bl x86_div_32\n");
                    OUTPUT_STRING("mov eax, eax, lsr #16\n");
                    OUTPUT_STRING("orr eax, tmp1, eax, lsl #8\n");
                    OUTPUT_STRING("orr eax, tmp2, eax, lsl #8\n");

                    if (flags_to_write & (FL_ZERO | FL_SIGN))
                    {
                        OUTPUT_STRING("movS tmplr, tmp2, lsl #24\n");
                    }

                    if (flags_to_write & FL_PARITY)
                    {
                        SR_disassemble_calculate_parity_flag(cOutput, AR_TMP2, AR_TMP1, 0);
                    }

                }
            }
            break;
        case UD_Iadc:
        case UD_Isbb:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                {
                    register uint32_t carry_flag2;

                    if (ud_obj.mnemonic == UD_Iadc)
                    {
                        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
                    }
                    else
                    {
                        carry_flag2 = FL_CARRY_SUB_INVERTED;
                    }

                    if (flags_to_write & carry_flag2) flags_write |= carry_flag2;
                }

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_adc_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            SR_arm_helper_adc_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_arm_helper_adc_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_adc_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[1].base), 0);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) << 24;

                            SR_arm_helper_adc_8l(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) << 24;

                            SR_arm_helper_adc_8h(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            SR_arm_helper_adc_32(ud_obj.mnemonic, AR_TMP1, X86322ARMREG(ud_obj.operand[1].base), 0);

                            SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                                SR_arm_helper_adc_32(ud_obj.mnemonic, AR_TMP1, AR_NONE, value);

                                SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                            }
                        }
                    }
                }
            }
            break;
        case UD_Iadd:
        case UD_Isub:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                {
                    register uint32_t carry_flag2;

                    if (ud_obj.mnemonic == UD_Iadd)
                    {
                        carry_flag2 = FL_CARRY_SUB_ORIGINAL;
                    }
                    else
                    {
                        carry_flag2 = FL_CARRY_SUB_INVERTED;
                    }

                    if (flags_to_write & carry_flag2) flags_write |= carry_flag2;
                }

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_add_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            SR_arm_helper_add_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_arm_helper_add_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                            }
                            else
                            {
                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("LDR tmp1, =%s\n", cAddress);

                                SR_arm_helper_add_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_add_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            SR_arm_helper_add_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            SR_arm_helper_add_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_arm_helper_add_8l(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_arm_helper_add_8l(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            SR_arm_helper_add_8l(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_arm_helper_add_8l(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_arm_helper_add_8h(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_arm_helper_add_8h(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            SR_arm_helper_add_8h(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_arm_helper_add_8h(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            SR_arm_helper_add_32(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X86322ARMREG(ud_obj.operand[1].base), 0);

                            SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                                SR_arm_helper_add_32(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);

                                SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                            }

                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            unsigned int res_shift;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            res_shift = SR_arm_helper_add_16(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X86162ARMREG(ud_obj.operand[1].base), 0);

                            if (res_shift)
                            {
                                OUTPUT_STRING("mov tmp1, tmp1, lsr #16\n"); // res_shift == 16
                            }

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            unsigned int res_shift;
                            int32_t value;

                            value = ((int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND)) << 16;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            res_shift = SR_arm_helper_add_16(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);

                            if (res_shift)
                            {
                                OUTPUT_STRING("mov tmp1, tmp1, lsr #16\n"); // res_shift == 16
                            }

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                unsigned int res_shift;

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                                res_shift = SR_arm_helper_add_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X868L2ARMREG(ud_obj.operand[1].base), 0);

                                if (res_shift)
                                {
                                    OUTPUT_PARAMSTRING("mov tmp1, tmp1, lsr #%i\n", res_shift);
                                }

                                SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                unsigned int res_shift;

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                                res_shift = SR_arm_helper_add_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X868H2ARMREG(ud_obj.operand[1].base), 8);

                                if (res_shift)
                                {
                                    OUTPUT_PARAMSTRING("mov tmp1, tmp1, lsr #%i\n", res_shift);
                                }

                                SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            unsigned int res_shift;
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            res_shift = SR_arm_helper_add_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);

                            if (res_shift)
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, tmp1, lsr #%i\n", res_shift);
                            }

                            SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                        }
                    }
                }
            }
            break;
        case UD_Iand:
        case UD_Ior:
        case UD_Ixor:
            {
                /* CF,OF cleared, AF undefined, SF,ZF,PF - modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_and_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            SR_arm_helper_and_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_arm_helper_and_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_and_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            SR_arm_helper_and_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            SR_arm_helper_and_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_arm_helper_and_8l(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_arm_helper_and_8l(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            SR_arm_helper_and_8l(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_arm_helper_and_8l(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_arm_helper_and_8h(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_arm_helper_and_8h(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            SR_arm_helper_and_8h(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_arm_helper_and_8h(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            SR_arm_helper_and_32(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X86322ARMREG(ud_obj.operand[1].base), 0);

                            SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_CARRY | FL_PARITY | FL_ADJUST | FL_OVERFLOW));
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                                SR_arm_helper_and_32(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);

                                SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_CARRY | FL_PARITY | FL_ADJUST | FL_OVERFLOW));
                            }
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            unsigned int res_shift;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            res_shift = SR_arm_helper_and_16(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X86162ARMREG(ud_obj.operand[1].base), 0);

                            if (res_shift)
                            {
                                OUTPUT_STRING("mov tmp1, tmp1, lsr #16\n"); // res_shift == 16
                            }

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            unsigned int res_shift;
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            res_shift = SR_arm_helper_and_16(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);

                            if (res_shift)
                            {
                                OUTPUT_STRING("mov tmp1, tmp1, lsr #16\n"); // res_shift == 16
                            }

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                unsigned int res_shift;

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                                res_shift = SR_arm_helper_and_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X868L2ARMREG(ud_obj.operand[1].base), 0);

                                if (res_shift)
                                {
                                    OUTPUT_PARAMSTRING("mov tmp1, tmp1, lsr #%i\n", res_shift);
                                }

                                SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            unsigned int res_shift;
                            uint32_t value;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            res_shift = SR_arm_helper_and_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);

                            if (res_shift)
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, tmp1, lsr #%i\n", res_shift);
                            }

                            SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                        }
                    }
                }
            }
            break;
        case UD_Ibsr:
            {
                /* CF,OF,AF,SF,PF undefined, ZF - modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                SR_get_label(cLabel, section[Entry].start + cur_ofs);

                if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                {
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        OUTPUT_PARAMSTRING("cmp %s, #0\n", X86REGSTR(ud_obj.operand[1].base));
                        OUTPUT_PARAMSTRING("beq %s_after_bsr\n", cLabel);

                        OUTPUT_PARAMSTRING("mov %s, #31\n", X86REGSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("movS tmp1, %s, lsl #1\n", X86REGSTR(ud_obj.operand[1].base));

                        OUTPUT_PARAMSTRING("bcs %s_after_loop\n", cLabel);

                        OUTPUT_PARAMSTRING("%s_loop:\n", cLabel);
                        OUTPUT_PARAMSTRING("sub %s, %s, #1\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("movS tmp1, tmp1, lsl #1\n");
                        OUTPUT_PARAMSTRING("bcc %s_loop\n", cLabel);

                        OUTPUT_PARAMSTRING("%s_after_loop:\n", cLabel);

                        if (flags_to_write & FL_ZERO)
                        {
                            OUTPUT_STRING("movS tmp1, #1\n");
                        }

                        OUTPUT_PARAMSTRING("%s_after_bsr:\n", cLabel);
                    }
                }
            }
            break;
        case UD_Ibt:
            {
                /* ZF,OF,AF,SF,PF undefined, CF - modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_REG, ZERO_EXTEND, &memadr);

                            OUTPUT_PARAMSTRING("mov tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_STRING("add tmpadr, tmpadr, tmp1, asr #19\n");

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);
                            OUTPUT_PARAMSTRING("and tmp2, %s, #7\n", X862ARMSTR(ud_obj.operand[1].base));

                            OUTPUT_STRING("mov tmp1, tmp1, lsr tmp2\n");

                            if ( flags_to_write & FL_CARRY_SUB_ORIGINAL )
                            {
                                OUTPUT_STRING("eor tmp1, tmp1, #1\n");
                            }
                            OUTPUT_STRING("mov tmp1, tmp1, rrx\n");
                        }
                    }
                }

            }
            break;
        case UD_Icall:
            {
                /* no flags affected */
                SR_get_label(cLabel, section[Entry].start + cur_ofs);
                strcat(cLabel, "_after_call");

                if (fixup[0] == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    ud_obj.operand[0].size == 32)
                {
                    uint32_t address;
                    extrn_data *extrnadr;
                    char *altaction;

                    address = ud_obj.operand[0].lval.sdword + (uint32_t) ud_obj.pc;

                    extrnadr = SR_disassemble_find_proc(Entry, address);
                    altaction = NULL;

                    if (extrnadr != NULL)
                    {
                        altaction = extrnadr->altaction;
                        strcpy(cAddress, extrnadr->proc);
                    }
                    else
                    {
                        SR_get_label(cAddress, address);
                    }

                    if (altaction == NULL)
                    {
                        OUTPUT_STRING("ACTION_CALL\n");

                        OUTPUT_PARAMSTRING("ADR tmp1, %s\n", cLabel);

                        OUTPUT_STRING("PUSH_REG tmp1\n");

                        OUTPUT_PARAMSTRING("b %s\n", cAddress);

                        OUTPUT_STRING("LTORG_CALL\n");

                        OUTPUT_PARAMSTRING("%s:\n", cLabel);
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("%s\n", altaction);
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM &&
                         ud_obj.operand[0].size == 32)
                {
                    enum madr_address_mode madr_mode;

                    if (ud_obj.operand[0].base == UD_R_ESP ||
                        ud_obj.operand[0].index == UD_R_ESP)
                    {
                        madr_mode = MADR_REG;
                    }
                    else
                    {
                        madr_mode = MADR_READ;
                    }

                    OUTPUT_STRING("ACTION_CALL\n");

                    SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, madr_mode, ZERO_EXTEND, &memadr);

                    if (ud_obj.br_far)
                    {
                        OUTPUT_STRING("sub esp, esp, #4\n");
                    }

                    OUTPUT_PARAMSTRING("ADR tmp1, %s\n", cLabel);

                    OUTPUT_STRING("PUSH_REG tmp1\n");

                    SR_disassemble_read_mem_word(cOutput, &memadr, AR_EIP, flags_to_write);

                    OUTPUT_STRING("LTORG_CALL\n");

                    OUTPUT_PARAMSTRING("%s:\n", cLabel);
                }
                else if (fixup[0] == NULL &&
                         ud_obj.operand[0].type == UD_OP_REG &&
                         ud_obj.operand[0].size == 32)
                {
                    OUTPUT_STRING("ACTION_CALL\n");

                    OUTPUT_PARAMSTRING("ADR tmp1, %s\n", cLabel);

                    OUTPUT_STRING("PUSH_REG tmp1\n");

                    OUTPUT_PARAMSTRING("bx %s\n", X86REGSTR(ud_obj.operand[0].base));

                    OUTPUT_STRING("LTORG_CALL\n");

                    OUTPUT_PARAMSTRING("%s:\n", cLabel);
                }
                else if (fixup[0] != NULL &&
                         ud_obj.operand[0].type == UD_OP_JIMM &&
                         ud_obj.operand[0].size == 32)
                {
                    extrn_data *extrnadr;
                    char *altaction;

                    extrnadr = SR_disassemble_find_proc(fixup[0]->tsec, fixup[0]->tofs);
                    altaction = NULL;

                    if (extrnadr != NULL)
                    {
                        altaction = extrnadr->altaction;
                        strcpy(cAddress, extrnadr->proc);
                    }
                    else
                    {
                        SR_get_fixup_label(cAddress, fixup[0], extrn[0]);
                    }

                    if (altaction == NULL)
                    {
                        OUTPUT_STRING("ACTION_CALL\n");

                        OUTPUT_PARAMSTRING("ADR tmp1, %s\n", cLabel);

                        OUTPUT_STRING("PUSH_REG tmp1\n");

                        OUTPUT_PARAMSTRING("b %s\n", cAddress);

                        OUTPUT_STRING("LTORG_CALL\n");

                        OUTPUT_PARAMSTRING("%s:\n", cLabel);
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("%s\n", altaction);
                    }

                }
            }
            break;
        case UD_Icbw:
            {
                /* no flags affected */

                OUTPUT_STRING("mov tmp1, eax, lsl #24\n");
                OUTPUT_STRING("mov tmp1, tmp1, asr #8\n");
                OUTPUT_STRING("bic eax, eax, #0x00ff\n");
                OUTPUT_STRING("bic eax, eax, #0xff00\n");
                OUTPUT_STRING("orr eax, eax, tmp1, lsr #16\n");
            }
            break;
        case UD_Icdq:
            {
                /* no flags affected */

                OUTPUT_STRING("mov edx, eax, asr #31\n");
            }
            break;
        case UD_Iclc:
            {
                /* clear carry flag */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if ( (flags_to_write & FL_CARRY_SUB_INVERTED) || !(flags_to_write & FL_CARRY_SUB_ORIGINAL))
                {
                    SR_disassemble_change_arm_flags(cOutput, FL_CARRY, 0, 0, AR_TMP1);
                }
                else
                {
                    SR_disassemble_change_arm_flags(cOutput, 0, FL_CARRY, 0, AR_TMP1);
                }

            }
            break;
        case UD_Icld:
            {
                /* clear direction flag */

                OUTPUT_STRING("bic eflags, eflags, #0x0400\n");
            }
            break;
        case UD_Icli:
            {
                /* clear interrupt flag */

                OUTPUT_STRING("CLI\n");
            }
            break;
        case UD_Icmc:
            {
                /* CF - inverted */

                if (flags_to_write & FL_CARRY_SUB_INVERTED)
                {
                    flags_read |= FL_CARRY_SUB_INVERTED;
                    flags_write |= FL_CARRY_SUB_INVERTED;
                }

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                {
                    flags_read |= FL_CARRY_SUB_ORIGINAL;
                    flags_write |= FL_CARRY_SUB_ORIGINAL;
                }

                SR_disassemble_change_arm_flags(cOutput, 0, 0, FL_CARRY, AR_TMP1);

            }
            break;
        case UD_Icmp:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                if (flags_to_write & FL_CARRY_SUB_INVERTED) flags_write |= FL_CARRY_SUB_INVERTED;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_add_32(ud_obj.mnemonic, AR_TMP1, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            SR_arm_helper_add_32(ud_obj.mnemonic, AR_TMP1, X86322ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_arm_helper_add_32(ud_obj.mnemonic, AR_TMP1, X86322ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                            }
                            else
                            {
                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("LDR tmp1, =%s\n", cAddress);

                                SR_arm_helper_add_32(ud_obj.mnemonic, AR_TMP1, X862ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_add_16(ud_obj.mnemonic, AR_TMP1, X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            SR_arm_helper_add_16(ud_obj.mnemonic, AR_TMP1, X86162ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = ((int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND)) << 16;

                            SR_arm_helper_add_16(ud_obj.mnemonic, AR_TMP1, X86162ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_arm_helper_add_8l(ud_obj.mnemonic, AR_TMP1, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_arm_helper_add_8l(ud_obj.mnemonic, AR_TMP1, X868L2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            SR_arm_helper_add_8l(ud_obj.mnemonic, AR_TMP1, X868L2ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_arm_helper_add_8l(ud_obj.mnemonic, AR_TMP1, X868L2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_arm_helper_add_8h(ud_obj.mnemonic, AR_TMP1, X868H2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_arm_helper_add_8h(ud_obj.mnemonic, AR_TMP1, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            SR_arm_helper_add_8h(ud_obj.mnemonic, AR_TMP1, X868H2ARMREG(ud_obj.operand[0].base), AR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_arm_helper_add_8h(ud_obj.mnemonic, AR_TMP1, X868H2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            SR_arm_helper_add_32(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X862ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                                SR_arm_helper_add_32(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);
                            }
                            else
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("LDR tmp2, =%s\n", cAddress);

                                SR_arm_helper_add_32(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_TMP2, 0);
                            }

                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            SR_arm_helper_add_16(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X86162ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = ((int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND)) << 16;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            SR_arm_helper_add_16(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                                SR_arm_helper_add_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X868L2ARMREG(ud_obj.operand[1].base), 0);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            SR_arm_helper_add_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);
                        }
                    }
                }

            }
            break;
        case UD_Icmpsb:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                if (flags_to_write & FL_CARRY_SUB_INVERTED) flags_write |= FL_CARRY_SUB_INVERTED;

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    SR_get_label(cLabel, section[Entry].start + cur_ofs);

                    OUTPUT_STRING("cmp ecx, #0\n");
                    OUTPUT_PARAMSTRING("beq %s_after_cmpsb\n", cLabel);

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #9\n");  // tmpadr = 0 or 2
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #1\n");      // tmpadr = 1 or -1

                    OUTPUT_PARAMSTRING("%s_loop:\n", cLabel);

                    OUTPUT_STRING("ldrb tmp1, [esi], tmpadr\n");
                    OUTPUT_STRING("ldrb tmp2, [edi], tmpadr\n");

                    OUTPUT_STRING("cmp tmp1, tmp2\n");

                    if (ud_obj.pfx_repne)
                    {
                        OUTPUT_PARAMSTRING("beq %s_set_flags\n", cLabel);
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("bne %s_set_flags\n", cLabel);
                    }

                    OUTPUT_STRING("subS ecx, ecx, #1\n");
                    OUTPUT_PARAMSTRING("bne %s_loop\n", cLabel);

                    OUTPUT_STRING("add ecx, ecx, #1\n");

                    OUTPUT_PARAMSTRING("%s_set_flags:\n", cLabel);

                    OUTPUT_STRING("sub ecx, ecx, #1\n");

                    if (flags_to_write)
                    {
                        SR_arm_helper_add_8l(UD_Icmp, AR_TMP1, AR_TMP1, AR_TMP2, 0);
                    }

                    OUTPUT_PARAMSTRING("%s_after_cmpsb:\n", cLabel);
                }
                else
                {
                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #9\n");  // tmpadr = 0 or 2
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #1\n");      // tmpadr = 1 or -1

                    if (flags_to_write)
                    {
                        OUTPUT_STRING("ldrb tmp1, [esi], tmpadr\n");
                        OUTPUT_STRING("ldrb tmp2, [edi], tmpadr\n");

                        SR_arm_helper_add_8l(UD_Icmp, AR_TMP1, AR_TMP1, AR_TMP2, 0);
                    }
                    else
                    {
                        OUTPUT_STRING("add esi, esi, tmpadr\n");
                        OUTPUT_STRING("add edi, edi, tmpadr\n");
                    }
                }

            }
            break;
        case UD_Icwd:
            {
                /* no flags affected */

                OUTPUT_STRING("mov tmp1, eax, lsl #16\n");
                OUTPUT_STRING("mov tmp1, tmp1, asr #16\n");
                OUTPUT_STRING("bic edx, edx, #0x00ff\n");
                OUTPUT_STRING("bic edx, edx, #0xff00\n");
                OUTPUT_STRING("orr edx, edx, tmp1, lsr #16\n");
            }
            break;
        case UD_Icwde:
            {
                /* no flags affected */

                OUTPUT_STRING("mov eax, eax, lsl #16\n");
                OUTPUT_STRING("mov eax, eax, asr #16\n");
            }
            break;
        case UD_Idec:
        case UD_Iinc:
            {
                /* CF - not modified, OS,SF,ZF,AF,PF - modified */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        SR_arm_helper_dec_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        SR_arm_helper_dec_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        SR_arm_helper_dec_8(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), 0);
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        SR_arm_helper_dec_8(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), 8);
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_PARITY | FL_ADJUST | FL_ZERO | FL_SIGN | FL_OVERFLOW));

                        SR_arm_helper_dec_32(ud_obj.mnemonic, AR_TMP1);

                        SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_PARITY | FL_ADJUST));
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                        SR_arm_helper_dec_16(ud_obj.mnemonic, AR_TMP1);

                        SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                        SR_arm_helper_dec_8(ud_obj.mnemonic, AR_TMP1, 0);

                        SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                    }
                }
            }
            break;
        case UD_Idiv:
            {
                /* CF,OF,SF,ZF,AF,PF undefined */
                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        OUTPUT_PARAMSTRING("mov tmp3, %s\n", X86REGSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("bl x86_div_64\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_STRING("mov tmp1, eax, lsl #16\n");
                        OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("mov tmp1, tmp1, lsr #16\n");
                        OUTPUT_STRING("mov tmp2, tmp2, lsr #16\n");
                        OUTPUT_STRING("orr tmp1, tmp1, edx, lsl #16\n");
                        OUTPUT_STRING("bl x86_div_32\n");
                        OUTPUT_STRING("mov eax, eax, lsr #16\n");
                        OUTPUT_STRING("LDR tmp3, =0xffff\n");
                        OUTPUT_STRING("mov edx, edx, lsr #16\n");
                        OUTPUT_STRING("and tmp1, tmp1, tmp3\n");
                        OUTPUT_STRING("and tmp2, tmp2, tmp3\n");
                        OUTPUT_STRING("orr eax, tmp1, eax, lsl #16\n");
                        OUTPUT_STRING("orr edx, tmp2, edx, lsl #16\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        OUTPUT_PARAMSTRING("and tmp2, %s, #0xff\n", X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("mov tmp1, eax\n");
                        OUTPUT_STRING("bl x86_div_16\n");
                        OUTPUT_STRING("mov eax, eax, lsr #16\n");
                        OUTPUT_STRING("and tmp2, tmp2, #0xff\n");
                        OUTPUT_STRING("and tmp1, tmp1, #0xff\n");
                        OUTPUT_STRING("orr eax, tmp2, eax, lsl #8\n");
                        OUTPUT_STRING("orr eax, tmp1, eax, lsl #8\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("mov tmp1, eax\n");
                        OUTPUT_STRING("mov tmp2, tmp2, lsr #8\n");
                        OUTPUT_STRING("bl x86_div_16\n");
                        OUTPUT_STRING("mov eax, eax, lsr #16\n");
                        OUTPUT_STRING("and tmp2, tmp2, #0xff\n");
                        OUTPUT_STRING("and tmp1, tmp1, #0xff\n");
                        OUTPUT_STRING("orr eax, tmp2, eax, lsl #8\n");
                        OUTPUT_STRING("orr eax, tmp1, eax, lsl #8\n");
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP3, flags_to_write & ~(FL_COND_ALL));

                        OUTPUT_STRING("bl x86_div_64\n");
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP2, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                        OUTPUT_STRING("mov tmp1, eax, lsl #16\n");
                        OUTPUT_STRING("mov tmp1, tmp1, lsr #16\n");
                        OUTPUT_STRING("orr tmp1, tmp1, edx, lsl #16\n");
                        OUTPUT_STRING("bl x86_div_32\n");
                        OUTPUT_STRING("mov eax, eax, lsr #16\n");
                        OUTPUT_STRING("LDR tmp3, =0xffff\n");
                        OUTPUT_STRING("mov edx, edx, lsr #16\n");
                        OUTPUT_STRING("and tmp1, tmp1, tmp3\n");
                        OUTPUT_STRING("and tmp2, tmp2, tmp3\n");
                        OUTPUT_STRING("orr eax, tmp1, eax, lsl #16\n");
                        OUTPUT_STRING("orr edx, tmp2, edx, lsl #16\n");
                    }
                }
            }
            break;
        case UD_Ienter:
            {
                /* no flags affected */
                uint32_t NestingLevel, Size, Level;

                Size = SR_disassemble_get_value(&(ud_obj.operand[0]), ZERO_EXTEND);
                NestingLevel = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) % 32;

                if (NestingLevel == 0)
                {
                    OUTPUT_STRING("PUSH_REG ebp\n");
                    OUTPUT_STRING("mov ebp, esp\n");
                    if (Size != 0)
                    {
                        if ( SR_disassemble_is_operand2(Size) )
                        {
                            OUTPUT_PARAMSTRING("sub esp, esp, #0x%x\n", Size);
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("LDR tmp1, =0x%x\n", Size);
                            OUTPUT_STRING("sub esp, esp, tmp1\n");
                        }
                    }
                }
                else
                {
                    OUTPUT_STRING("PUSH_REG ebp\n");
                    OUTPUT_STRING("mov tmp1, esp\n");

                    if (NestingLevel > 1)
                    {
                        for (Level = 1; Level < NestingLevel; Level++)
                        {
                            OUTPUT_STRING("ldmdb ebp!, {tmp2}\n");
                            OUTPUT_STRING("PUSH_REG tmp2\n");
                        }
                    }

                    OUTPUT_STRING("PUSH_REG tmp1\n");
                    OUTPUT_STRING("mov ebp, tmp1\n");

                    if (Size != 0)
                    {
                        if ( SR_disassemble_is_operand2(Size) )
                        {
                            OUTPUT_PARAMSTRING("sub esp, esp, #0x%x\n", Size);
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("LDR tmp1, =0x%x\n", Size);
                            OUTPUT_STRING("sub esp, esp, tmp1\n");
                        }
                    }
                }
            }
            break;
        case UD_Iidiv:
            {
                /* CF,OF,SF,ZF,AF,PF undefined */
                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        OUTPUT_PARAMSTRING("mov tmp3, %s\n", X86REGSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("bl x86_idiv_64\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_STRING("mov tmp1, eax, lsl #16\n");
                        OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("mov tmp1, tmp1, lsr #16\n");
                        OUTPUT_STRING("mov tmp2, tmp2, asr #16\n");
                        OUTPUT_STRING("orr tmp1, tmp1, edx, lsl #16\n");
                        OUTPUT_STRING("bl x86_idiv_32\n");
                        OUTPUT_STRING("mov eax, eax, lsr #16\n");
                        OUTPUT_STRING("LDR tmp3, =0xffff\n");
                        OUTPUT_STRING("mov edx, edx, lsr #16\n");
                        OUTPUT_STRING("and tmp1, tmp1, tmp3\n");
                        OUTPUT_STRING("and tmp2, tmp2, tmp3\n");
                        OUTPUT_STRING("orr eax, tmp1, eax, lsl #16\n");
                        OUTPUT_STRING("orr edx, tmp2, edx, lsl #16\n");
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP3, flags_to_write & ~(FL_COND_ALL));

                        OUTPUT_STRING("bl x86_idiv_64\n");
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP2, AR_TMP3, AR_TMPLR, READ16TO32SIGN);

                        OUTPUT_STRING("mov tmp1, eax, lsl #16\n");
                        OUTPUT_STRING("mov tmp1, tmp1, lsr #16\n");
                        OUTPUT_STRING("orr tmp1, tmp1, edx, lsl #16\n");
                        OUTPUT_STRING("bl x86_idiv_32\n");
                        OUTPUT_STRING("mov eax, eax, lsr #16\n");
                        OUTPUT_STRING("LDR tmp3, =0xffff\n");
                        OUTPUT_STRING("mov edx, edx, lsr #16\n");
                        OUTPUT_STRING("and tmp1, tmp1, tmp3\n");
                        OUTPUT_STRING("and tmp2, tmp2, tmp3\n");
                        OUTPUT_STRING("orr eax, tmp1, eax, lsl #16\n");
                        OUTPUT_STRING("orr edx, tmp2, edx, lsl #16\n");

                    }
                }
            }
            break;
        case UD_Iimul:
            {
                /* SF,ZF,AF,PF undefined, CF,OF - modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                {
                    fprintf(stderr, "Error: imul sets carry flag sub-inverted\n");
                }

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    /* one-operand form */
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                        {
                            if ( (ud_obj.operand[0].base == UD_R_EAX) || (ud_obj.operand[0].base == UD_R_EDX) )
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, %s\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("smull eax, edx, tmp1, eax\n");
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("smull eax, edx, %s, eax\n", X86REGSTR(ud_obj.operand[0].base));
                            }

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp2, eax, asr #31\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, edx\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                        {
                            OUTPUT_STRING("mov tmp1, eax, lsl #16\n");
                            OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_STRING("mov tmp2, tmp2, asr #16\n");

                            OUTPUT_STRING("smull tmp3, tmplr, tmp1, tmp2\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp2, tmp3, asr #31\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmplr\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }


                            OUTPUT_STRING("LDR tmp1, =0xffff\n");
                            OUTPUT_STRING("bic eax, eax, tmp1\n");
                            OUTPUT_STRING("orr eax, eax, tmp3, lsr #16\n");
                            OUTPUT_STRING("bic edx, edx, tmp1\n");
                            OUTPUT_STRING("and tmplr, tmplr, tmp1\n");
                            OUTPUT_STRING("orr edx, edx, tmplr\n");
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                        {
                            OUTPUT_STRING("mov tmp1, eax, lsl #24\n");
                            OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #24\n", X862ARMSTR(ud_obj.operand[0].base));

                            OUTPUT_STRING("smull tmp3, tmplr, tmp1, tmp2\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp2, tmplr, lsl #8\n");
                                OUTPUT_STRING("mov tmp2, tmp2, asr #8\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmplr\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }


                            OUTPUT_STRING("bic eax, eax, #0x00ff\n");
                            OUTPUT_STRING("bic eax, eax, #0xff00\n");
                            OUTPUT_STRING("orr eax, eax, tmplr, lsr #16\n");
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                        {
                            OUTPUT_STRING("mov tmp1, eax, lsl #24\n");
                            OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_STRING("and tmp2, tmp2, #0xff000000\n");

                            OUTPUT_STRING("smull tmp3, tmplr, tmp1, tmp2\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp2, tmplr, lsl #8\n");
                                OUTPUT_STRING("mov tmp2, tmp2, asr #8\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmplr\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }


                            OUTPUT_STRING("bic eax, eax, #0x00ff\n");
                            OUTPUT_STRING("bic eax, eax, #0xff00\n");
                            OUTPUT_STRING("orr eax, eax, tmplr, lsr #16\n");
                        }
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            OUTPUT_STRING("smull eax, edx, tmp1, eax\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp2, eax, asr #31\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, edx\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }
                        }
                        else if (ud_obj.operand[0].size == 16)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP2, AR_TMP3, AR_TMPLR, READ16TO32SIGN);


                            OUTPUT_STRING("mov tmp1, eax, lsl #16\n");

                            OUTPUT_STRING("smull tmp3, tmplr, tmp1, tmp2\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp2, tmp3, asr #31\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmplr\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }

                            OUTPUT_STRING("LDR tmp1, =0xffff\n");
                            OUTPUT_STRING("bic eax, eax, tmp1\n");
                            OUTPUT_STRING("orr eax, eax, tmp3, lsr #16\n");
                            OUTPUT_STRING("bic edx, edx, tmp1\n");
                            OUTPUT_STRING("and tmplr, tmplr, tmp1\n");
                            OUTPUT_STRING("orr edx, edx, tmplr\n");
                        }
                    }
                }
                else if (ud_obj.operand[2].type != UD_NONE)
                {
                    /* three-operand form */
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[2]), SIGN_EXTEND);

                            OUTPUT_PARAMSTRING("LDR tmp2, =%i\n", value);
                            OUTPUT_PARAMSTRING("smull %s, tmp3, tmp2, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base));

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("mov tmp2, %s, asr #31\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmp3\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[2]), SIGN_EXTEND);

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            OUTPUT_PARAMSTRING("LDR tmp2, =%i\n", value);
                            OUTPUT_PARAMSTRING("smull %s, tmp3, tmp2, tmp1\n", X86REGSTR(ud_obj.operand[0].base));

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("mov tmp2, %s, asr #31\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmp3\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[2]), SIGN_EXTEND);

                            OUTPUT_PARAMSTRING("LDR tmp2, =%i\n", value);
                            OUTPUT_PARAMSTRING("mov tmp3, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_STRING("smull tmp3, tmplr, tmp2, tmp3\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp2, tmp3, asr #31\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmplr\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }

                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[2]), SIGN_EXTEND);

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP2, AR_TMP3, AR_TMPLR, READ16TO32SIGN);

                            OUTPUT_PARAMSTRING("LDR tmp1, =%i\n", value << 16);
                            OUTPUT_STRING("smull tmp2, tmplr, tmp1, tmp2\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp3, tmp2, asr #31\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp3, tmplr\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }

                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                    }
                }
                else
                {
                    /* two-operand form */
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {

                            if (ud_obj.operand[0].base == ud_obj.operand[1].base)
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, %s\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("smull %s, tmp3, tmp1, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("smull %s, tmp3, %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base), X86REGSTR(ud_obj.operand[0].base));
                            }

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("mov tmp2, %s, asr #31\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmp3\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            OUTPUT_PARAMSTRING("smull %s, tmp3, tmp1, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("mov tmp2, %s, asr #31\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmp3\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("mov tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_STRING("mov tmp2, tmp2, asr #16\n");

                            OUTPUT_STRING("smull tmp3, tmplr, tmp1, tmp2\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp2, tmp3, asr #31\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp2, tmplr\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }

                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp3, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP2, AR_TMP3, AR_TMPLR, READ16TO32SIGN);

                            OUTPUT_PARAMSTRING("mov tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));

                            OUTPUT_STRING("smull tmp2, tmplr, tmp1, tmp2\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_STRING("mov tmp3, tmp2, asr #31\n");
                                OUTPUT_STRING("mrs tmp1, cpsr\n");
                                OUTPUT_STRING("cmp tmp3, tmplr\n");
                                OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                                OUTPUT_STRING("msr cpsr_f, tmp1\n");
                            }

                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                    }
                }
            }
            break;
        case UD_Iin:
            {
                /* no flags affected */
                SR_get_label(cLabel, section[Entry].start + cur_ofs);
                strcat(cLabel, "_after_call");

                if (ud_obj.operand[1].type == UD_OP_REG)
                {
                    OUTPUT_PARAMSTRING("ADR tmp1, %s\n", cLabel);

                    OUTPUT_STRING("PUSH_REG tmp1\n");

                    //OUTPUT_PARAMSTRING("b x86_in_%s_dx\n", (ud_obj.operand[0].base == UD_R_AL)?("al"):( (ud_obj.operand[0].base == UD_R_AX)?("ax"):("eax") ) );
                    OUTPUT_PARAMSTRING("b x86_in_%s_dx\n", ud_reg_tab[ud_obj.operand[0].base - UD_R_AL] );

                    OUTPUT_STRING("LTORG_IN\n");

                    OUTPUT_PARAMSTRING("%s:\n", cLabel);
                }
                else
                {
                    OUTPUT_PARAMSTRING("ADR tmp2, %s\n", cLabel);

                    OUTPUT_PARAMSTRING("mov tmp1, #0x%x\n", ud_obj.operand[1].lval.ubyte);

                    OUTPUT_STRING("PUSH_REG tmp2\n");

                    //OUTPUT_PARAMSTRING("b x86_in_%s_imm\n", (ud_obj.operand[0].base == UD_R_AL)?("al"):( (ud_obj.operand[0].base == UD_R_AX)?("ax"):("eax") ) );
                    OUTPUT_PARAMSTRING("b x86_in_%s_imm\n", ud_reg_tab[ud_obj.operand[0].base - UD_R_AL] );

                    OUTPUT_STRING("LTORG_IN\n");

                    OUTPUT_PARAMSTRING("%s:\n", cLabel);
                }
            }
            break;
        case UD_Iint:
        case UD_Iint1:
        case UD_Iint3:
            {
                /* all flags modified */

                unsigned int intno;

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.mnemonic == UD_Iint1)
                {
                    intno = 1;
                }
                else if (ud_obj.mnemonic == UD_Iint3)
                {
                    intno = 3;
                }
                else
                {
                    intno = ud_obj.operand[0].lval.ubyte;
                }

                SR_get_label(cLabel, section[Entry].start + cur_ofs);
                strcat(cLabel, "_after_call");

                OUTPUT_PARAMSTRING("ADR tmp2, %s\n", cLabel);

                OUTPUT_PARAMSTRING("mov tmp1, #%i\n", intno);

                OUTPUT_STRING("PUSH_REG tmp2\n");

                OUTPUT_STRING("b x86_int\n");

                OUTPUT_STRING("LTORG_INT\n");

                OUTPUT_PARAMSTRING("%s:\n", cLabel);

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                {
                    SR_disassemble_change_arm_flags(cOutput, 0, 0, FL_CARRY, AR_TMP2);
                }
            }
            break;
        case UD_Ija:
        case UD_Ijae:
        case UD_Ijb:
        case UD_Ijbe:
        case UD_Ijc:
        case UD_Ijg:
        case UD_Ijge:
        case UD_Ijl:
        case UD_Ijle:
        case UD_Ijnb:
        case UD_Ijnbe:
        case UD_Ijnc:
        case UD_Ijnl:
        case UD_Ijnle:
        case UD_Ijns:
        case UD_Ijnz:
        case UD_Ijs:
        case UD_Ijz:
            {
                /* no flags affected */

                char *instr;

                if (ud_obj.mnemonic == UD_Ijb) instr = "blo";
                else if (ud_obj.mnemonic == UD_Ijc) instr = "bcc";
                else if (ud_obj.mnemonic == UD_Ijae || ud_obj.mnemonic == UD_Ijnb) instr = "bhs";
                else if (ud_obj.mnemonic == UD_Ijnc) instr = "bcs";
                else if (ud_obj.mnemonic == UD_Ijbe) instr = "bls";
                else if (ud_obj.mnemonic == UD_Ija || ud_obj.mnemonic == UD_Ijnbe) instr = "bhi";
                else if (ud_obj.mnemonic == UD_Ijg || ud_obj.mnemonic == UD_Ijnle) instr = "bgt";
                else if (ud_obj.mnemonic == UD_Ijl) instr = "blt";
                else if (ud_obj.mnemonic == UD_Ijle) instr = "ble";
                else if (ud_obj.mnemonic == UD_Ijz) instr = "beq";
                else if (ud_obj.mnemonic == UD_Ijnz) instr = "bne";
                else if (ud_obj.mnemonic == UD_Ijs) instr = "bmi";
                else if (ud_obj.mnemonic == UD_Ijns) instr = "bpl";
                else /*if (ud_obj.mnemonic == UD_Ijge || ud_obj.mnemonic == UD_Ijnl)*/ instr = "bge";

                if (fixup[0] == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    (ud_obj.operand[0].size == 32 || ud_obj.operand[0].size == 8)
                    )
                {
                    uint32_t address;
                    extrn_data *extrnadr;
                    int backward;

                    if (ud_obj.operand[0].size == 32)
                    {
                        backward = (ud_obj.operand[0].lval.sdword < 0)?1:0;
                        address = ud_obj.operand[0].lval.sdword + (uint32_t) ud_obj.pc;
                    }
                    else
                    {
                        backward = (ud_obj.operand[0].lval.sbyte < 0)?1:0;
                        address = ud_obj.operand[0].lval.sbyte + (uint32_t) ud_obj.pc;
                    }

                    extrnadr = SR_disassemble_find_proc(Entry, address);

                    if (extrnadr != NULL)
                    {
                        strcpy(cAddress, extrnadr->proc);
                    }
                    else
                    {
                        SR_get_label(cAddress, address);
                    }

                    if (backward)
                    {
                        OUTPUT_STRING("ACTION_CONDITIONAL_JUMP_BACKWARD\n");
                    }
                    else
                    {
                        OUTPUT_STRING("ACTION_CONDITIONAL_JUMP_FORWARD\n");
                    }

                    OUTPUT_PARAMSTRING("%s %s\n", instr, cAddress);
                }
            }
            break;
        case UD_Ijecxz:
            {
                /* no flags affected */


                uint32_t address;
                extrn_data *extrnadr;
                int backward;

                if (ud_obj.operand[0].size == 32)
                {
                    backward = (ud_obj.operand[0].lval.sdword < 0)?1:0;
                    address = ud_obj.operand[0].lval.sdword + (uint32_t) ud_obj.pc;
                }
                else
                {
                    backward = (ud_obj.operand[0].lval.sbyte < 0)?1:0;
                    address = ud_obj.operand[0].lval.sbyte + (uint32_t) ud_obj.pc;
                }

                extrnadr = SR_disassemble_find_proc(Entry, address);

                if (extrnadr != NULL)
                {
                    strcpy(cAddress, extrnadr->proc);
                }
                else
                {
                    SR_get_label(cAddress, address);
                }

                SR_get_label(cLabel, section[Entry].start + cur_ofs);


                if (backward)
                {
                    OUTPUT_STRING("ACTION_CONDITIONAL_JUMP_BACKWARD\n");
                }
                else
                {
                    OUTPUT_STRING("ACTION_CONDITIONAL_JUMP_FORWARD\n");
                }

                OUTPUT_STRING("mrs tmp2, cpsr\n");

                OUTPUT_STRING("cmp ecx, #0\n");

                OUTPUT_PARAMSTRING("%s_rel1:\n", cLabel);
                OUTPUT_PARAMSTRING("addne tmp1, pc, #((%s_after_jecxz - %s_rel1) - 8)\n", cLabel, cLabel);

                OUTPUT_PARAMSTRING("%s_rel2:\n", cLabel);
                OUTPUT_PARAMSTRING("addeq tmp1, pc, #((%s - %s_rel2) - 8)\n", cAddress, cLabel);

                OUTPUT_STRING("msr cpsr_f, tmp2\n");

                OUTPUT_STRING("bx tmp1\n");

                OUTPUT_STRING("LTORG_JECXZ\n");

                OUTPUT_PARAMSTRING("%s_after_jecxz:\n", cLabel);
            }
            break;
        case UD_Ijmp:
            {
                /* no flags affected */

                if (fixup[0] == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    (ud_obj.operand[0].size == 32 || ud_obj.operand[0].size == 8)
                    )
                {
                    uint32_t address;
                    extrn_data *extrnadr;
                    int backward, shortjump;

                    if (ud_obj.operand[0].size == 32)
                    {
                        shortjump = 0;
                        backward = (ud_obj.operand[0].lval.sdword < 0)?1:0;
                        address = ud_obj.operand[0].lval.sdword + (uint32_t) ud_obj.pc;
                    }
                    else
                    {
                        shortjump = 1;
                        backward = (ud_obj.operand[0].lval.sbyte < 0)?1:0;
                        address = ud_obj.operand[0].lval.sbyte + (uint32_t) ud_obj.pc;
                    }

                    extrnadr = SR_disassemble_find_proc(Entry, address);

                    if (extrnadr != NULL)
                    {
                        strcpy(cAddress, extrnadr->proc);
                    }
                    else
                    {
                        SR_get_label(cAddress, address);
                    }

                    if (shortjump)
                    {
                        if (backward)
                        {
                            OUTPUT_STRING("ACTION_UNCONDITIONAL_SHORT_JUMP_BACKWARD\n");
                        }
                        else
                        {
                            OUTPUT_STRING("ACTION_UNCONDITIONAL_SHORT_JUMP_FORWARD\n");
                        }
                    }
                    else
                    {
                        OUTPUT_STRING("ACTION_UNCONDITIONAL_JUMP\n");
                    }

                    OUTPUT_PARAMSTRING("b %s\n", cAddress);

                    OUTPUT_STRING("LTORG_JMP\n");
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM &&
                         ud_obj.operand[0].size == 32)
                {
                    OUTPUT_STRING("ACTION_UNCONDITIONAL_JUMP\n");

                    SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                    SR_disassemble_read_mem_word(cOutput, &memadr, AR_EIP, flags_to_write);

                    OUTPUT_STRING("LTORG_JMP\n");
                }
                else if (fixup[0] == NULL &&
                         ud_obj.operand[0].type == UD_OP_REG &&
                         ud_obj.operand[0].size == 32)
                {
                    OUTPUT_STRING("ACTION_UNCONDITIONAL_JUMP\n");

                    OUTPUT_PARAMSTRING("bx %s\n", X86REGSTR(ud_obj.operand[0].base));

                    OUTPUT_STRING("LTORG_JMP\n");
                }
            }
            break;
        case UD_Ilahf:
            {
                /* no flags affected */
                OUTPUT_STRING("mrs tmp1, cpsr\n");
                OUTPUT_STRING("and tmp3, eflags, #0x003e\n");
                OUTPUT_STRING("and tmp2, tmp1, #0xc0000000\n");
                OUTPUT_STRING("orr tmp3, tmp3, tmp2, lsr #24\n");
                OUTPUT_STRING("and tmp2, tmp1, #0x20000000\n");
                OUTPUT_STRING("orr tmp3, tmp3, tmp2, lsr #29\n");
                OUTPUT_STRING("bic eax, eax, #0xff00\n");
                OUTPUT_STRING("orr eax, eax, tmp3, lsl #8\n");
            }
            break;
        case UD_Ilea:
            {
                /* no flags affected */

                if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                {
                    SR_disassemble_get_memory_address(cOutput, X862ARMREG(ud_obj.operand[0].base), &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_REG, ZERO_EXTEND, &memadr);
                }
            }
            break;
        case UD_Ileave:
            {
                /* no flags affected */

                OUTPUT_STRING("mov esp, ebp\n");
                OUTPUT_STRING("POP_REG ebp\n");
            }
            break;
        case UD_Ilodsb:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                }
                else
                {
                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #9\n");  // tmpadr = 0 or 2
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #1\n");      // tmpadr = 1 or -1
                    OUTPUT_STRING("ldrb tmp1, [esi], tmpadr\n");
                    OUTPUT_STRING("bic eax, eax, #0x00ff\n");
                    OUTPUT_STRING("orr eax, eax, tmp1\n");
                }


            }
            break;
        case UD_Ilodsd:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                }
                else
                {
                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #7\n");  // tmpadr = 0 or 8
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #4\n");      // tmpadr = 4 or -4
                    OUTPUT_STRING("ldr eax, [esi], tmpadr\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("ldrb eax, [esi]\n");
                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("ldrb tmp2, [esi, #1]\n");
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #7\n");  // tmpadr = 0 or 8
                    OUTPUT_STRING("ldrb tmp3, [esi, #2]\n");
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #4\n");      // tmpadr = 4 or -4
                    OUTPUT_STRING("ldrb tmplr, [esi, #3]\n");
                    OUTPUT_STRING("add esi, esi, tmpadr\n");
                    OUTPUT_STRING("orr eax, eax, tmp2, lsl #8\n");
                    OUTPUT_STRING("orr eax, eax, tmp3, lsl #16\n");
                    OUTPUT_STRING("orr eax, eax, tmplr, lsl #24\n");

                    OUTPUT_STRING(".endif\n");
                }
            }
            break;
        case UD_Ilodsw:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                }
                else
                {
                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #8\n");  // tmpadr = 0 or 4
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #2\n");      // tmpadr = 2 or -2
                    OUTPUT_STRING("ldrh tmp1, [esi], tmpadr\n");
                    OUTPUT_STRING("mov eax, eax, lsr #16\n");
                    OUTPUT_STRING("orr eax, tmp1, eax, lsl #16\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("ldrb tmp2, [esi, #1]\n");
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #8\n");  // tmpadr = 0 or 4
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #2\n");      // tmpadr = 2 or -2
                    OUTPUT_STRING("ldrb tmp1, [esi], tmpadr\n");
                    OUTPUT_STRING("mov eax, eax, lsr #16\n");
                    OUTPUT_STRING("orr eax, tmp2, eax, lsl #8\n");
                    OUTPUT_STRING("orr eax, tmp1, eax, lsl #8\n");

                    OUTPUT_STRING(".endif\n");
                }
            }
            break;
        case UD_Iloop:
        case UD_Iloope:
        case UD_Iloopnz:
            {
                /* no flags affected */

                uint32_t address;
                extrn_data *extrnadr;
                int backward;

                if (ud_obj.operand[0].size == 32)
                {
                    backward = (ud_obj.operand[0].lval.sdword < 0)?1:0;
                    address = ud_obj.operand[0].lval.sdword + (uint32_t) ud_obj.pc;
                }
                else
                {
                    backward = (ud_obj.operand[0].lval.sbyte < 0)?1:0;
                    address = ud_obj.operand[0].lval.sbyte + (uint32_t) ud_obj.pc;
                }

                extrnadr = SR_disassemble_find_proc(Entry, address);

                if (extrnadr != NULL)
                {
                    strcpy(cAddress, extrnadr->proc);
                }
                else
                {
                    SR_get_label(cAddress, address);
                }

                SR_get_label(cLabel, section[Entry].start + cur_ofs);

                if (backward)
                {
                    OUTPUT_STRING("ACTION_LOOP_BACKWARD\n");
                }
                else
                {
                    OUTPUT_STRING("ACTION_LOOP_FORWARD\n");
                }

                if (ud_obj.mnemonic == UD_Iloope)
                {
                    OUTPUT_STRING("subne ecx, ecx, #1\n");
                    OUTPUT_PARAMSTRING("bne %s_after_loop\n", cLabel);
                }
                else if (ud_obj.mnemonic == UD_Iloopnz)
                {
                    OUTPUT_STRING("subeq ecx, ecx, #1\n");
                    OUTPUT_PARAMSTRING("beq %s_after_loop\n", cLabel);
                }

                OUTPUT_STRING("mrs tmp2, cpsr\n");

                OUTPUT_STRING("subS ecx, ecx, #1\n");

                OUTPUT_PARAMSTRING("%s_rel1:\n", cLabel);
                OUTPUT_PARAMSTRING("addeq tmp1, pc, #((%s_after_loop - %s_rel1) - 8)\n", cLabel, cLabel);

                OUTPUT_PARAMSTRING("%s_rel2:\n", cLabel);
                OUTPUT_PARAMSTRING("addne tmp1, pc, #((%s - %s_rel2) - 8)\n", cAddress, cLabel);

                OUTPUT_STRING("msr cpsr_f, tmp2\n");

                OUTPUT_STRING("bx tmp1\n");

                OUTPUT_STRING("LTORG_LOOP\n");

                OUTPUT_PARAMSTRING("%s_after_loop:\n", cLabel);
            }
            break;
        case UD_Imov:
        case UD_Ilds:
        case UD_Iles:
        case UD_Ilfs:
        case UD_Ilgs:
        case UD_Ilss:
            {
                if (ud_obj.mnemonic == UD_Imov &&
                    ud_obj.operand[0].type == UD_OP_REG &&
                    ud_obj.operand[1].type == UD_OP_REG &&
                    ( (ud_obj.operand[0].base >= UD_R_CR0 && ud_obj.operand[0].base <= UD_R_DR15) ||
                      (ud_obj.operand[1].base >= UD_R_CR0 && ud_obj.operand[1].base <= UD_R_DR15)
                    ))
                {
                    /* flags undefined */
                    flags_write = FL_COND;
                }
                else
                {
                    /* no flags affected */
                    flags_write = FL_NONE;
                }

                if (ud_obj.mnemonic != UD_Imov &&
                    ud_obj.operand[0].type == UD_OP_REG &&
                    ud_obj.operand[1].type == UD_OP_MEM &&
                    ud_obj.operand[1].size == 0
                    )
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        ud_obj.operand[1].size = 32;
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        ud_obj.operand[1].size = 16;
                    }
                }

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.mnemonic == UD_Imov &&
                        ud_obj.operand[1].type == UD_OP_MEM &&
                        ud_obj.operand[1].base == UD_NONE &&
                        ud_obj.operand[1].index == UD_NONE &&
                        fixup[1] == NULL &&
                        ((ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BH) ||
                         (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI) ||
                         (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                        ) &&
                        (
                         ud_obj.pfx_seg == UD_R_CS ||
                         ud_obj.pfx_seg == UD_R_DS ||
                         ud_obj.pfx_seg == UD_R_ES ||
                         ud_obj.pfx_seg == UD_R_SS ||
                         ud_obj.pfx_seg == UD_NONE
                        )
                       )
                    {
                        // mov reg, [] - memory access with constant address

                        SR_get_label(cLabel, section[Entry].start + cur_ofs);
                        strcat(cLabel, "_after_call");

                        OUTPUT_PARAMSTRING("ADR tmp2, %s\n", cLabel);

                        SR_arm_load_imm32(AR_TMP1, ud_obj.operand[1].lval.udword);

                        OUTPUT_STRING("PUSH_REG tmp2\n");

                        OUTPUT_PARAMSTRING("b x86_mov_reg_mem_%i\n", ud_obj.operand[0].size);

                        OUTPUT_STRING("LTORG_IN\n");

                        OUTPUT_PARAMSTRING("%s:\n", cLabel);

                        if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                        {
                            OUTPUT_PARAMSTRING("mov %s, tmp1\n", X86REGSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                        {
                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsl #8\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_EAX && ud_obj.operand[1].base <= UD_R_EDI)
                            {
                                OUTPUT_PARAMSTRING("mov %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_ES && ud_obj.operand[1].base <= UD_R_GS)
                            {
                                OUTPUT_PARAMSTRING("mov %s, #0\n", X86REGSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), flags_to_write);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] != NULL)
                            {
                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("LDR %s, =%s\n", X86REGSTR(ud_obj.operand[0].base), cAddress);
                            }
                            else
                            {
                                uint32_t value;

                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                                SR_arm_load_imm32(X862ARMREG(ud_obj.operand[0].base), value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_ES && ud_obj.operand[1].base <= UD_R_GS)
                            {
                                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AX && ud_obj.operand[1].base <= UD_R_DI)
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), AR_TMP3, AR_TMPLR, READ16TO16);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                            SR_arm_load_imm16(X86162ARMREG(ud_obj.operand[0].base), value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp1\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsr #8\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), AR_TMP3, READ8TO8LOW);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, #0x%x\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base), ((uint32_t) ud_obj.operand[1].lval.ubyte));
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsl #8\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp1\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), AR_TMP3, READ8TO8HIGH);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, #0x%x\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base), ((uint32_t) ud_obj.operand[1].lval.ubyte) << 8);
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.mnemonic == UD_Imov &&
                        ud_obj.operand[0].base == UD_NONE &&
                        ud_obj.operand[0].index == UD_NONE &&
                        fixup[0] == NULL &&
                        fixup[1] == NULL &&
                        ((ud_obj.operand[1].type == UD_OP_REG &&
                          ((ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BH) ||
                           (ud_obj.operand[1].base >= UD_R_AX && ud_obj.operand[1].base <= UD_R_DI) ||
                           (ud_obj.operand[1].base >= UD_R_EAX && ud_obj.operand[1].base <= UD_R_EDI)
                          )
                         ) ||
                         ud_obj.operand[1].type == UD_OP_IMM
                        ) &&
                        (
                         ud_obj.pfx_seg == UD_R_CS ||
                         ud_obj.pfx_seg == UD_R_DS ||
                         ud_obj.pfx_seg == UD_R_ES ||
                         ud_obj.pfx_seg == UD_R_SS ||
                         ud_obj.pfx_seg == UD_NONE
                        )
                       )
                    {
                        // mov [], reg/imm - memory access with constant address

                        SR_get_label(cLabel, section[Entry].start + cur_ofs);
                        strcat(cLabel, "_after_call");

                        OUTPUT_PARAMSTRING("ADR tmp3, %s\n", cLabel);

                        SR_arm_load_imm32(AR_TMP1, ud_obj.operand[0].lval.udword);

                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                OUTPUT_PARAMSTRING("mov tmp2, %s, lsr #8\n", X862ARMSTR(ud_obj.operand[1].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mov tmp2, %s\n", X862ARMSTR(ud_obj.operand[1].base));
                            }
                        }
                        else
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                            SR_arm_load_imm32(AR_TMP2, value);
                        }

                        OUTPUT_STRING("PUSH_REG tmp3\n");

                        OUTPUT_PARAMSTRING("b x86_mov_mem_reg_%i\n", ud_obj.operand[1].size);

                        OUTPUT_STRING("LTORG_IN\n");

                        OUTPUT_PARAMSTRING("%s:\n", cLabel);
                    }
                    else if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_ES && ud_obj.operand[1].base <= UD_R_GS)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                                OUTPUT_STRING("mov tmp1, #0\n");

                                SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_EAX && ud_obj.operand[1].base <= UD_R_EDI)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], ud_obj.operand[1].base, MADR_WRITE, ZERO_EXTEND, &memadr);

                                SR_disassemble_write_mem_word(cOutput, &memadr, X862ARMREG(ud_obj.operand[1].base), flags_to_write);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                uint32_t value;

                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                                SR_arm_load_imm32(AR_TMP1, value);

                                SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                            }
                            else
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("LDR tmp1, =%s\n", cAddress);

                                SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_ES && ud_obj.operand[1].base <= UD_R_GS)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                                OUTPUT_STRING("mov tmp1, #0\n");

                                SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AX && ud_obj.operand[1].base <= UD_R_DI)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], ud_obj.operand[1].base, MADR_WRITE, ZERO_EXTEND, &memadr);

                                SR_disassemble_write_mem_halfword(cOutput, &memadr, X862ARMREG(ud_obj.operand[1].base), AR_TMP3);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_arm_load_imm32(AR_TMP1, value);

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            enum byte_write_mode highlow;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], ud_obj.operand[1].base, MADR_WRITE, ZERO_EXTEND, &memadr);

                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                highlow = WRITE8LOW;
                            }
                            else
                            {
                                highlow = WRITE8HIGH;
                            }

                            SR_disassemble_write_mem_byte(cOutput, &memadr, X862ARMREG(ud_obj.operand[1].base), AR_TMP3, highlow);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            OUTPUT_PARAMSTRING("LDR tmp1, =0x%x\n", value);

                            SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                        }
                    }
                }
            }
            break;
        case UD_Imovsb:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("mrs tmp3, cpsr\n");
                    }

                    OUTPUT_STRING("teq ecx, #0\n");
                    OUTPUT_STRING("blne x86_rep_movsb\n");

                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("msr cpsr_f, tmp3\n");
                    }
                }
                else
                {
                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("ldrb tmp1, [esi]\n");
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #9\n");  // tmpadr = 0 or 2
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #1\n");      // tmpadr = 1 or -1
                    OUTPUT_STRING("add esi, esi, tmpadr\n");
                    OUTPUT_STRING("strb tmp1, [edi], tmpadr\n");
                }

            }
            break;
        case UD_Imovsd:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("mrs tmp3, cpsr\n");
                    }

                    OUTPUT_STRING("teq ecx, #0\n");

                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("blne x86_rep_movsd_4_4\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("blne x86_rep_movsd_1_1\n");

                    OUTPUT_STRING(".endif\n");

                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("msr cpsr_f, tmp3\n");
                    }
                }
                else
                {
                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("ldr tmp1, [esi]\n");
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #7\n");  // tmpadr = 0 or 8
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #4\n");      // tmpadr = 4 or -4
                    OUTPUT_STRING("add esi, esi, tmpadr\n");
                    OUTPUT_STRING("str tmp1, [edi], tmpadr\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("ldrb tmp2, [esi, #1]\n");
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #7\n");  // tmpadr = 0 or 8
                    OUTPUT_STRING("ldrb tmp3, [esi, #2]\n");
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #4\n");      // tmpadr = 4 or -4
                    OUTPUT_STRING("ldrb tmplr, [esi, #3]\n");
                    OUTPUT_STRING("ldrb tmp1, [esi], tmpadr\n");
                    OUTPUT_STRING("strb tmp2, [edi, #1]\n");
                    OUTPUT_STRING("strb tmp3, [edi, #2]\n");
                    OUTPUT_STRING("strb tmplr, [edi, #3]\n");
                    OUTPUT_STRING("strb tmp1, [edi], tmpadr\n");

                    OUTPUT_STRING(".endif\n");
                }

            }
            break;
        case UD_Imovsw:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("mrs tmp3, cpsr\n");
                    }

                    OUTPUT_STRING("teq ecx, #0\n");

                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("blne x86_rep_movsw_2_2\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("blne x86_rep_movsw_1_1\n");

                    OUTPUT_STRING(".endif\n");

                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("msr cpsr_f, tmp3\n");
                    }
                }
                else
                {
                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("ldrh tmp1, [esi]\n");
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #8\n");  // tmpadr = 0 or 4
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #2\n");      // tmpadr = 2 or -2
                    OUTPUT_STRING("add esi, esi, tmpadr\n");
                    OUTPUT_STRING("strh tmp1, [edi], tmpadr\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("ldrb tmp2, [esi, #1]\n");
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #8\n");  // tmpadr = 0 or 4
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #2\n");      // tmpadr = 2 or -2
                    OUTPUT_STRING("ldrb tmp1, [esi], tmpadr\n");
                    OUTPUT_STRING("strb tmp2, [edi, #1]\n");
                    OUTPUT_STRING("strb tmp1, [edi], tmpadr\n");

                    OUTPUT_STRING(".endif\n");
                }
            }
            break;
        case UD_Imovsx:
            {
                /* no flags affected */

                if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                {
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[1].base >= UD_R_AX && ud_obj.operand[1].base <= UD_R_DI)
                        {
                            OUTPUT_PARAMSTRING("mov %s, %s, lsl #16\n", X86REGSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("mov %s, %s, asr #16\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("mov %s, %s, lsl #24\n", X86REGSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("mov %s, %s, asr #24\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                        {
                            OUTPUT_PARAMSTRING("mov %s, %s, lsl #16\n", X86REGSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("mov %s, %s, asr #24\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[1].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[1].size == 16)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), AR_TMP3, AR_TMPLR, READ16TO32SIGN);
                        }
                        else if (ud_obj.operand[1].size == 8)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), AR_TMP3, READ8TO32SIGN);
                        }
                    }
                }
                else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                {
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, %s, lsl #24\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("and %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_STRING("mov tmp1, tmp1, asr #8\n");
                            OUTPUT_PARAMSTRING("and %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("and %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_STRING("mov tmp1, tmp1, asr #8\n");
                            OUTPUT_PARAMSTRING("and %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[1].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[1].size == 8)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), AR_TMP3, READ8TO16SIGN);
                        }
                    }
                }

            }
            break;
        case UD_Imovzx:
            {
                /* no flags affected */

                if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                {
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[1].base >= UD_R_AX && ud_obj.operand[1].base <= UD_R_DI)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("mov %s, tmp1, lsr #16\n", X86REGSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("and %s, %s, #0x00ff\n", X86REGSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[1].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("mov %s, tmp1, lsr #24\n", X86REGSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[1].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[1].size == 16)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), AR_TMP3, AR_TMPLR, READ16TO32ZERO);
                        }
                        else if (ud_obj.operand[1].size == 8)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), AR_TMP3, READ8TO32ZERO);
                        }
                    }
                }
                else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                {
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("and tmp1, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                        {
                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("and tmp1, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsr #8\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[1].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[1].size == 8)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862ARMREG(ud_obj.operand[0].base), AR_TMP3, READ8TO16ZERO);
                        }
                    }
                }

            }
            break;
        case UD_Imul:
            {
                /* SF,ZF,AF,PF undefined, CF,OF - modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                {
                    fprintf(stderr, "Error: mul sets carry flag sub-inverted\n");
                }

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if ( (ud_obj.operand[0].base == UD_R_EAX) || (ud_obj.operand[0].base == UD_R_EDX) )
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, %s\n", X86REGSTR(ud_obj.operand[0].base));
                            OUTPUT_STRING("umull eax, edx, tmp1, eax\n");
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("umull eax, edx, %s, eax\n", X86REGSTR(ud_obj.operand[0].base));
                        }

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            OUTPUT_STRING("mrs tmp1, cpsr\n");
                            OUTPUT_STRING("cmp edx, #0\n");
                            OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("msr cpsr_f, tmp1\n");
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("mov tmp1, eax, lsl #16\n");
                        OUTPUT_STRING("mov tmp2, tmp2, lsr #16\n");

                        OUTPUT_STRING("umull tmp3, tmplr, tmp1, tmp2\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            OUTPUT_STRING("mrs tmp1, cpsr\n");
                            OUTPUT_STRING("movS tmp2, tmplr, lsl #16\n");
                            OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("msr cpsr_f, tmp1\n");
                        }

                        OUTPUT_STRING("LDR tmp1, =0xffff\n");
                        OUTPUT_STRING("bic eax, eax, tmp1\n");
                        OUTPUT_STRING("orr eax, eax, tmp3, lsr #16\n");
                        OUTPUT_STRING("bic edx, edx, tmp1\n");
                        OUTPUT_STRING("and tmplr, tmplr, tmp1\n");
                        OUTPUT_STRING("orr edx, edx, tmplr\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        OUTPUT_STRING("mov tmp1, eax, lsl #24\n");
                        OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("and tmp2, tmp2, #0xff000000\n");

                        OUTPUT_STRING("umull tmp3, tmplr, tmp1, tmp2\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            OUTPUT_STRING("mrs tmp1, cpsr\n");
                            OUTPUT_STRING("movS tmp2, tmplr, lsr #24\n");
                            OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("msr cpsr_f, tmp1\n");
                        }


                        OUTPUT_STRING("bic eax, eax, #0x00ff\n");
                        OUTPUT_STRING("bic eax, eax, #0xff00\n");
                        OUTPUT_STRING("orr eax, eax, tmplr, lsr #16\n");
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                        OUTPUT_STRING("umull eax, edx, tmp1, eax\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            OUTPUT_STRING("mrs tmp1, cpsr\n");
                            OUTPUT_STRING("cmp edx, #0\n");
                            OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("msr cpsr_f, tmp1\n");
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP2, AR_TMP3, AR_TMPLR, READ16TO32ZERO);


                        OUTPUT_STRING("mov tmp1, eax, lsl #16\n");

                        OUTPUT_STRING("umull tmp3, tmplr, tmp1, tmp2\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            OUTPUT_STRING("mrs tmp1, cpsr\n");
                            OUTPUT_STRING("movS tmp2, tmplr, lsl #16\n");
                            OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("msr cpsr_f, tmp1\n");
                        }

                        OUTPUT_STRING("LDR tmp1, =0xffff\n");
                        OUTPUT_STRING("bic eax, eax, tmp1\n");
                        OUTPUT_STRING("orr eax, eax, tmp3, lsr #16\n");
                        OUTPUT_STRING("bic edx, edx, tmp1\n");
                        OUTPUT_STRING("and tmplr, tmplr, tmp1\n");
                        OUTPUT_STRING("orr edx, edx, tmplr\n");
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP2, AR_TMP3, READ8TO32ZERO);

                        OUTPUT_STRING("mov tmp1, eax, lsl #24\n");
                        OUTPUT_STRING("mov tmp2, tmp2, lsl #24\n");

                        OUTPUT_STRING("umull tmp3, tmplr, tmp1, tmp2\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            OUTPUT_STRING("mrs tmp1, cpsr\n");
                            OUTPUT_STRING("movS tmp2, tmplr, lsr #24\n");
                            OUTPUT_STRING("bic tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("orrne tmp1, tmp1, #0x30000000\n");
                            OUTPUT_STRING("msr cpsr_f, tmp1\n");
                        }


                        OUTPUT_STRING("bic eax, eax, #0x00ff\n");
                        OUTPUT_STRING("bic eax, eax, #0xff00\n");
                        OUTPUT_STRING("orr eax, eax, tmplr, lsr #16\n");
                    }
                }
            }
            break;
        case UD_Ineg:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */
                if (flags_to_write & FL_CARRY_SUB_INVERTED) flags_write |= FL_CARRY_SUB_INVERTED;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        SR_arm_helper_neg_32(X86322ARMREG(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        SR_arm_helper_neg_16(X86162ARMREG(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        SR_arm_helper_neg_8(X868L2ARMREG(ud_obj.operand[0].base), 0);
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        SR_arm_helper_neg_8(X868H2ARMREG(ud_obj.operand[0].base), 8);
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                        SR_arm_helper_neg_32(AR_TMP1);

                        SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                        SR_arm_helper_neg_16(AR_TMP1);

                        SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                        SR_arm_helper_neg_8(AR_TMP1, 0);

                        SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                    }
                }
            }
            break;
        case UD_Inop:
            {
                /* no flags affected */

                OUTPUT_STRING("NOP\n");
            }
            break;
        case UD_Inot:
            {
                /* no flags affected */
                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        OUTPUT_PARAMSTRING("mvn %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        OUTPUT_PARAMSTRING("mvn tmp1, %s, lsl #24\n", X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsr #24\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);

                        OUTPUT_STRING("mvn tmp1, tmp1\n");

                        SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                        OUTPUT_STRING("mvn tmp1, tmp1\n");

                        SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                        OUTPUT_STRING("mvn tmp1, tmp1\n");

                        SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                    }
                }
            }
            break;
        case UD_Iout:
            {
                /* no flags affected */
                SR_get_label(cLabel, section[Entry].start + cur_ofs);
                strcat(cLabel, "_after_call");

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    OUTPUT_PARAMSTRING("ADR tmp1, %s\n", cLabel);

                    OUTPUT_STRING("PUSH_REG tmp1\n");

                    //OUTPUT_PARAMSTRING("b x86_out_dx_%s\n", (ud_obj.operand[1].base == UD_R_AL)?("al"):( (ud_obj.operand[1].base == UD_R_AX)?("ax"):("eax") ) );
                    OUTPUT_PARAMSTRING("b x86_out_dx_%s\n", ud_reg_tab[ud_obj.operand[1].base - UD_R_AL] );

                    OUTPUT_STRING("LTORG_OUT\n");

                    OUTPUT_PARAMSTRING("%s:\n", cLabel);
                }
                else
                {
                    OUTPUT_PARAMSTRING("ADR tmp2, %s\n", cLabel);

                    OUTPUT_PARAMSTRING("mov tmp1, #0x%x\n", ud_obj.operand[0].lval.ubyte);

                    OUTPUT_STRING("PUSH_REG tmp2\n");

                    //OUTPUT_PARAMSTRING("b x86_out_imm_%s\n", (ud_obj.operand[1].base == UD_R_AL)?("al"):( (ud_obj.operand[1].base == UD_R_AX)?("ax"):("eax") ) );
                    OUTPUT_PARAMSTRING("b x86_out_imm_%s\n", ud_reg_tab[ud_obj.operand[1].base - UD_R_AL] );

                    OUTPUT_STRING("LTORG_OUT\n");

                    OUTPUT_PARAMSTRING("%s:\n", cLabel);
                }
            }
            break;
        case UD_Ioutsb:
            {
                /* no flags affected */

                SR_get_label(cLabel, section[Entry].start + cur_ofs);

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("mrs tmp3, cpsr\n");
                    }

                    OUTPUT_STRING("teq ecx, #0\n");
                    OUTPUT_PARAMSTRING("beq %s_restore_flags\n", cLabel);

                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("stmfd esp!, {tmp3,edi,ebx,eax}\n");
                    }
                    else
                    {
                        OUTPUT_STRING("stmfd esp!, {edi,ebx,eax}\n");
                    }

                    OUTPUT_PARAMSTRING("ADR edi, %s_after_call\n", cLabel);

                    OUTPUT_STRING("and ebx, eflags, #0x0400\n");    // ebx = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov ebx, ebx, lsr #9\n");        // ebx = 0 or 2
                    OUTPUT_STRING("rsb ebx, ebx, #1\n");            // ebx = 1 or -1

                    OUTPUT_PARAMSTRING("%s_loop:\n", cLabel);

                    OUTPUT_STRING("ldrb eax, [esi], ebx\n");

                    OUTPUT_STRING("PUSH_REG edi\n");

                    OUTPUT_STRING("b x86_out_dx_al\n");

                    OUTPUT_STRING("LTORG_OUT\n");

                    OUTPUT_PARAMSTRING("%s_after_call:\n", cLabel);

                    OUTPUT_STRING("subS ecx, ecx, #1\n");
                    OUTPUT_PARAMSTRING("bne %s_loop\n", cLabel);

                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("ldmfd esp!, {tmp3,edi,ebx,eax}\n");
                    }
                    else
                    {
                        OUTPUT_STRING("ldmfd esp!, {edi,ebx,eax}\n");
                    }

                    OUTPUT_PARAMSTRING("%s_restore_flags:\n", cLabel);

                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("msr cpsr_f, tmp3\n");
                    }
                }
                else
                {
                    OUTPUT_STRING("PUSH_REG eax\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #9\n");  // tmpadr = 0 or 2
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #1\n");      // tmpadr = 1 or -1

                    OUTPUT_STRING("ldrb eax, [esi], tmpadr\n");

                    OUTPUT_PARAMSTRING("ADR tmp2, %s_after_call\n", cLabel);

                    OUTPUT_STRING("PUSH_REG tmp2\n");

                    OUTPUT_STRING("b x86_out_dx_al\n");

                    OUTPUT_STRING("LTORG_OUT\n");

                    OUTPUT_PARAMSTRING("%s_after_call:\n", cLabel);
                    OUTPUT_STRING("POP_REG eax\n");
                }

            }
            break;
        case UD_Ipop:
            {
                /* no flags affected */
                if (fixup[0] == NULL &&
                    ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        OUTPUT_PARAMSTRING("POP_REG %s\n", X86REGSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_ES && ud_obj.operand[0].base <= UD_R_GS)
                    {
                        OUTPUT_STRING("add esp, esp, #4\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_STRING("POP_REG tmp1\n");
                        OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("mov tmp1, tmp1, lsl #16\n");
                        OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM &&
                        ud_obj.operand[0].size == 32)
                {
                    SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                    OUTPUT_STRING("POP_REG tmp1\n");

                    SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                }
            }
            break;
        case UD_Ipopa: // popaw
            {
                /* no flags affected */

                OUTPUT_STRING("POPAW\n");

            }
            break;
        case UD_Ipopad:
            {
                /* no flags affected */

                OUTPUT_STRING("POPAD\n");

/*				OUTPUT_STRING("ldmfd esp!, {edi}\n");
                OUTPUT_STRING("ldmfd esp!, {esi}\n");
                OUTPUT_STRING("ldmfd esp!, {ebp}\n");
                OUTPUT_STRING("ldmfd esp!, {tmp1}\n");
                OUTPUT_STRING("ldmfd esp!, {ebx}\n");
                OUTPUT_STRING("ldmfd esp!, {edx}\n");
                OUTPUT_STRING("ldmfd esp!, {ecx}\n");
                OUTPUT_STRING("ldmfd esp!, {eax}\n");*/
            }
            break;
        case UD_Ipopfd:
            {
                /* all flags modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                {
                    fprintf(stderr, "Error: popf sets carry flag sub-inverted\n");
                }

                OUTPUT_STRING("POPFD\n");
            }
            break;
        case UD_Ipush:
            {
                /* no flags affected */
                if (fixup[0] == NULL &&
                    ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        OUTPUT_PARAMSTRING("PUSH_REG %s\n", X86REGSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_ES && ud_obj.operand[0].base <= UD_R_GS)
                    {
                        OUTPUT_STRING("mov tmp1, #0\n");
                        OUTPUT_STRING("PUSH_REG tmp1\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_PARAMSTRING("PUSH_REG %s\n", X862ARMSTR(ud_obj.operand[0].base));
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM &&
                         ud_obj.operand[0].size == 32)
                {
                    SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                    SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);

                    OUTPUT_STRING("PUSH_REG tmp1\n");
                }
                else if (fixup[0] == NULL &&
                         ud_obj.operand[0].type == UD_OP_IMM)
                {
                    int32_t value;

                    value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[0]), SIGN_EXTEND);

                    OUTPUT_PARAMSTRING("LDR tmp1, =0x%x\n", value);
                    OUTPUT_STRING("PUSH_REG tmp1\n");
                }
                else if (ud_obj.operand[0].type == UD_OP_IMM)
                {
                    SR_get_fixup_label(cAddress, fixup[0], extrn[0]);

                    OUTPUT_PARAMSTRING("LDR tmp1, =%s\n", cAddress);
                    OUTPUT_STRING("PUSH_REG tmp1\n");
                }
            }
            break;
        case UD_Ipusha: // pushaw
            {
                /* no flags affected */

                OUTPUT_STRING("PUSHAW\n");
            }
            break;
        case UD_Ipushad:
            {
                /* no flags affected */

                OUTPUT_STRING("PUSHAD\n");

/*				OUTPUT_STRING("mov tmp1, esp\n");
                OUTPUT_STRING("stmfd esp!, {eax}\n");
                OUTPUT_STRING("stmfd esp!, {ecx}\n");
                OUTPUT_STRING("stmfd esp!, {edx}\n");
                OUTPUT_STRING("stmfd esp!, {ebx}\n");
                OUTPUT_STRING("stmfd esp!, {tmp1}\n");
                OUTPUT_STRING("stmfd esp!, {ebp}\n");
                OUTPUT_STRING("stmfd esp!, {esi}\n");
                OUTPUT_STRING("stmfd esp!, {edi}\n");*/
            }
            break;
        case UD_Ipushfd:
        case UD_Ipushfw:
            {
                /* no flags affected */
                OUTPUT_STRING("PUSHFD\n");
            }
            break;
        case UD_Ircl:
            {
                /* CF - modified, OF - modified for 1-bit shifts, else not modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }

                            if (value != 0)
                            {
                                if (value == 1)
                                {
                                    OUTPUT_STRING("mov tmp1, tmp1, rrx\n");

                                    OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #1\n", X86REGSTR(ud_obj.operand[0].base));
                                    if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsr #31\n", X86REGSTR(ud_obj.operand[0].base));
                                    }
                                    OUTPUT_PARAMSTRING("orr %s, tmp2, tmp1, lsr #31\n", X86REGSTR(ud_obj.operand[0].base));

                                    if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        OUTPUT_STRING("mrs tmp1, cpsr\n");

                                        if (flags_to_write & FL_CARRY)
                                        {
                                            //OUTPUT_STRING("and tmp3, tmp3, #1\n");
                                            if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                                            {
                                                OUTPUT_STRING("orr tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("eor tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                            else
                                            {
                                                OUTPUT_STRING("bic tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("orr tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                        }

                                        if (flags_to_write & FL_OVERFLOW)
                                        {
                                            /* update overflow flag */
                                            /* OF = MSB(modified reg) XOR CF */
                                            OUTPUT_STRING("bic tmp1, tmp1, #0x10000000\n");
                                            OUTPUT_PARAMSTRING("eor tmp2, tmp3, %s, lsr #31\n", X86REGSTR(ud_obj.operand[0].base));
                                            //OUTPUT_STRING("and tmp2, tmp2, #1\n");
                                            OUTPUT_STRING("orr tmp1, tmp1, tmp2, lsl #28\n");
                                        }
                                        OUTPUT_STRING("msr cpsr_f, tmp1\n");
                                    }

                                }
                                else
                                {
                                    fprintf(stderr, "Error: rcl with shifts > 1 not implemented");
                                }

                            }
                        }

                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = (SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f) % 17;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }

                            if (value != 0)
                            {
                                if (value == 1)
                                {
                                    OUTPUT_STRING("mov tmp1, tmp1, rrx\n");

                                    OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #17\n", X862ARMSTR(ud_obj.operand[0].base));
                                    if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsr #15\n", X862ARMSTR(ud_obj.operand[0].base));
                                    }
                                    OUTPUT_STRING("orr tmp2, tmp2, tmp1, lsr #15\n");
                                    OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));

                                    if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        OUTPUT_STRING("mrs tmp1, cpsr\n");

                                        if (flags_to_write & FL_CARRY)
                                        {
                                            OUTPUT_STRING("and tmp3, tmp3, #1\n");
                                            if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                                            {
                                                OUTPUT_STRING("orr tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("eor tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                            else
                                            {
                                                OUTPUT_STRING("bic tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("orr tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                        }

                                        if (flags_to_write & FL_OVERFLOW)
                                        {
                                            /* update overflow flag */
                                            /* OF = MSB(modified reg) XOR CF */
                                            OUTPUT_STRING("bic tmp1, tmp1, #0x10000000\n");
                                            OUTPUT_PARAMSTRING("eor tmp2, tmp3, %s, lsr #15\n", X862ARMSTR(ud_obj.operand[0].base));
                                            OUTPUT_STRING("and tmp2, tmp2, #1\n");
                                            OUTPUT_STRING("orr tmp1, tmp1, tmp2, lsl #28\n");
                                        }
                                        OUTPUT_STRING("msr cpsr_f, tmp1\n");
                                    }

                                }
                                else
                                {
                                    fprintf(stderr, "Error: rcl with shifts > 1 not implemented");
                                }

                            }

                        }
                    }
                }


            }
            break;
        case UD_Ircr:
            {
                /* CF - modified, OF - modified for 1-bit shifts, else not modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }

                            if (value != 0)
                            {
                                if (value == 1)
                                {
                                    if ( !(flags_to_write & ~(FL_CARRY | FL_CARRY_SUB_INVERTED | FL_CARRY_SUB_ORIGINAL)) )
                                    {
                                        OUTPUT_PARAMSTRING("movS %s, %s, rrx\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));

                                        if ( flags_to_write & FL_CARRY_SUB_ORIGINAL )
                                        {
                                            SR_disassemble_change_arm_flags(cOutput, 0, 0, FL_CARRY, AR_TMP1);
                                        }
                                    }
                                    else
                                    {
                                        OUTPUT_STRING("mov tmp1, tmp1, rrx\n");

                                        if (flags_to_write & FL_OVERFLOW)
                                        {
                                            /* OF = MSB(reg) XOR CF */
                                            OUTPUT_PARAMSTRING("eor tmplr, tmp1, %s\n", X86REGSTR(ud_obj.operand[0].base));
                                            OUTPUT_STRING("and tmplr, tmplr, #0x80000000\n");
                                        }

                                        OUTPUT_STRING("and tmp1, tmp1, #0x80000000\n");
                                        if (flags_to_write & FL_CARRY)
                                        {
                                            OUTPUT_PARAMSTRING("mov tmp3, %s\n", X86REGSTR(ud_obj.operand[0].base));
                                        }
                                        OUTPUT_PARAMSTRING("orr %s, tmp1, %s, lsr #1\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));

                                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                                        {
                                            OUTPUT_STRING("mrs tmp1, cpsr\n");

                                            if (flags_to_write & FL_CARRY)
                                            {
                                                OUTPUT_STRING("and tmp3, tmp3, #1\n");
                                                if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                                                {
                                                    OUTPUT_STRING("orr tmp1, tmp1, #0x20000000\n");
                                                    OUTPUT_STRING("eor tmp1, tmp1, tmp3, lsl #29\n");
                                                }
                                                else
                                                {
                                                    OUTPUT_STRING("bic tmp1, tmp1, #0x20000000\n");
                                                    OUTPUT_STRING("orr tmp1, tmp1, tmp3, lsl #29\n");
                                                }
                                            }

                                            if (flags_to_write & FL_OVERFLOW)
                                            {
                                                /* update overflow flag */
                                                OUTPUT_STRING("bic tmp1, tmp1, #0x10000000\n");
                                                OUTPUT_STRING("orr tmp1, tmp1, tmplr, lsr #3\n");
                                            }
                                            OUTPUT_STRING("msr cpsr_f, tmp1\n");
                                        }
                                    }
                                }
                                else
                                {
                                    fprintf(stderr, "Error: rcr with shifts > 1 not implemented");
                                }

                            }

                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = (SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f) % 17;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }

                            if (value != 0)
                            {
                                if (value == 1)
                                {
                                    OUTPUT_STRING("mov tmp1, tmp1, rrx\n");

                                    if (flags_to_write & FL_OVERFLOW)
                                    {
                                        /* OF = MSB(reg) XOR CF */
                                        OUTPUT_PARAMSTRING("eor tmplr, tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmplr, tmplr, #0x80000000\n");
                                    }

                                    OUTPUT_STRING("and tmp1, tmp1, #0x80000000\n");
                                    OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                                    if (flags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp3, %s\n", X862ARMSTR(ud_obj.operand[0].base));
                                    }
                                    OUTPUT_STRING("orr tmp2, tmp1, tmp2, lsr #1\n");
                                    OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));

                                    if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        OUTPUT_STRING("mrs tmp1, cpsr\n");

                                        if (flags_to_write & FL_CARRY)
                                        {
                                            OUTPUT_STRING("and tmp3, tmp3, #1\n");
                                            if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                                            {
                                                OUTPUT_STRING("orr tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("eor tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                            else
                                            {
                                                OUTPUT_STRING("bic tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("orr tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                        }

                                        if (flags_to_write & FL_OVERFLOW)
                                        {
                                            /* update overflow flag */
                                            OUTPUT_STRING("bic tmp1, tmp1, #0x10000000\n");
                                            OUTPUT_STRING("orr tmp1, tmp1, tmplr, lsr #3\n");
                                        }
                                        OUTPUT_STRING("msr cpsr_f, tmp1\n");
                                    }

                                }
                                else
                                {
                                    fprintf(stderr, "Error: rcr with shifts > 1 not implemented");
                                }

                            }

                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = (SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f) % 9;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }

                            if (value != 0)
                            {
                                if (value == 1)
                                {
                                    OUTPUT_STRING("mov tmp1, tmp1, rrx\n");

                                    if (flags_to_write & FL_OVERFLOW)
                                    {
                                        /* OF = MSB(reg) XOR CF */
                                        OUTPUT_PARAMSTRING("eor tmplr, tmp1, %s, lsl #24\n", X862ARMSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmplr, tmplr, #0x80000000\n");
                                    }

                                    OUTPUT_STRING("and tmp1, tmp1, #0x80000000\n");
                                    OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #24\n", X862ARMSTR(ud_obj.operand[0].base));
                                    if (flags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp3, %s\n", X862ARMSTR(ud_obj.operand[0].base));
                                    }
                                    OUTPUT_STRING("orr tmp2, tmp1, tmp2, lsr #1\n");
                                    OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #24\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));

                                    if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        OUTPUT_STRING("mrs tmp1, cpsr\n");

                                        if (flags_to_write & FL_CARRY)
                                        {
                                            OUTPUT_STRING("and tmp3, tmp3, #1\n");
                                            if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                                            {
                                                OUTPUT_STRING("orr tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("eor tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                            else
                                            {
                                                OUTPUT_STRING("bic tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("orr tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                        }

                                        if (flags_to_write & FL_OVERFLOW)
                                        {
                                            /* update overflow flag */
                                            OUTPUT_STRING("bic tmp1, tmp1, #0x10000000\n");
                                            OUTPUT_STRING("orr tmp1, tmp1, tmplr, lsr #3\n");
                                        }
                                        OUTPUT_STRING("msr cpsr_f, tmp1\n");
                                    }

                                }
                                else
                                {
                                    fprintf(stderr, "Error: rcr with shifts > 1 not implemented");
                                }

                            }

                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = (SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f) % 9;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }

                            if (value != 0)
                            {
                                if (value == 1)
                                {
                                    OUTPUT_STRING("mov tmp1, tmp1, rrx\n");

                                    if (flags_to_write & FL_OVERFLOW)
                                    {
                                        /* OF = MSB(reg) XOR CF */
                                        OUTPUT_PARAMSTRING("eor tmplr, tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmplr, tmplr, #0x80000000\n");
                                    }

                                    OUTPUT_STRING("and tmp1, tmp1, #0x80000000\n");
                                    OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                                    if (flags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp3, %s, lsr #8\n", X862ARMSTR(ud_obj.operand[0].base));
                                    }
                                    OUTPUT_STRING("orr tmp2, tmp1, tmp2, lsr #1\n");
                                    OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                    OUTPUT_STRING("and tmp2, tmp2, #0xff000000\n");
                                    OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));

                                    if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        OUTPUT_STRING("mrs tmp1, cpsr\n");

                                        if (flags_to_write & FL_CARRY)
                                        {
                                            OUTPUT_STRING("and tmp3, tmp3, #1\n");
                                            if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                                            {
                                                OUTPUT_STRING("orr tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("eor tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                            else
                                            {
                                                OUTPUT_STRING("bic tmp1, tmp1, #0x20000000\n");
                                                OUTPUT_STRING("orr tmp1, tmp1, tmp3, lsl #29\n");
                                            }
                                        }

                                        if (flags_to_write & FL_OVERFLOW)
                                        {
                                            /* update overflow flag */
                                            OUTPUT_STRING("bic tmp1, tmp1, #0x10000000\n");
                                            OUTPUT_STRING("orr tmp1, tmp1, tmplr, lsr #3\n");
                                        }
                                        OUTPUT_STRING("msr cpsr_f, tmp1\n");
                                    }
                                }
                                else
                                {
                                    fprintf(stderr, "Error: rcr with shifts > 1 not implemented");
                                }

                            }

                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }

                            if (value != 0)
                            {
                                if (value == 1)
                                {
                                    SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                    SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_CARRY | FL_OVERFLOW));

                                    if ( !(flags_to_write & ~(FL_CARRY | FL_CARRY_SUB_INVERTED | FL_CARRY_SUB_ORIGINAL)) )
                                    {
                                        OUTPUT_STRING("movS tmp1, tmp1, rrx\n");

                                        if ( flags_to_write & FL_CARRY_SUB_ORIGINAL )
                                        {
                                            SR_disassemble_change_arm_flags(cOutput, 0, 0, FL_CARRY, AR_TMP2);
                                        }
                                    }
                                    else
                                    {
                                        OUTPUT_STRING("mov tmp2, tmp2, rrx\n");

                                        if (flags_to_write & FL_OVERFLOW)
                                        {
                                            /* OF = MSB(reg) XOR CF */
                                            OUTPUT_STRING("eor tmplr, tmp2, tmp2\n");
                                            OUTPUT_STRING("and tmplr, tmplr, #0x80000000\n");
                                        }

                                        OUTPUT_STRING("and tmp2, tmp2, #0x80000000\n");
                                        if (flags_to_write & FL_CARRY)
                                        {
                                            OUTPUT_STRING("mov tmp3, tmp1\n");
                                        }
                                        OUTPUT_STRING("orr tmp1, tmp2, tmp1, lsr #1\n");

                                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                                        {
                                            OUTPUT_STRING("mrs tmp2, cpsr\n");

                                            if (flags_to_write & FL_CARRY)
                                            {
                                                OUTPUT_STRING("and tmp3, tmp3, #1\n");
                                                if (flags_to_write & FL_CARRY_SUB_ORIGINAL)
                                                {
                                                    OUTPUT_STRING("orr tmp2, tmp2, #0x20000000\n");
                                                    OUTPUT_STRING("eor tmp2, tmp2, tmp3, lsl #29\n");
                                                }
                                                else
                                                {
                                                    OUTPUT_STRING("bic tmp2, tmp2, #0x20000000\n");
                                                    OUTPUT_STRING("orr tmp2, tmp2, tmp3, lsl #29\n");
                                                }
                                            }

                                            if (flags_to_write & FL_OVERFLOW)
                                            {
                                                /* update overflow flag */
                                                OUTPUT_STRING("bic tmp2, tmp2, #0x10000000\n");
                                                OUTPUT_STRING("orr tmp2, tmp2, tmplr, lsr #3\n");
                                            }
                                            OUTPUT_STRING("msr cpsr_f, tmp2\n");
                                        }
                                    }
                                    SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                                }
                                else
                                {
                                    fprintf(stderr, "Error: rcr with shifts > 1 not implemented");
                                }

                            }

                        }
                    }
                }

            }
            break;
        case UD_Iret:
        case UD_Iretf:
            {
                /* no flags affected */

                int32_t value;

                value = 0;

                if (ud_obj.operand[0].type == UD_NONE ||
                    ud_obj.operand[0].type == UD_OP_IMM)
                {
                    if (ud_obj.operand[0].type == UD_OP_IMM)
                    {
                        value = SR_disassemble_get_value(&(ud_obj.operand[0]), ZERO_EXTEND);
                    }

                    if (ud_obj.mnemonic == UD_Iretf)
                    {
                        value += 4;
                    }

                    if (value == 0)
                    {
                        OUTPUT_STRING("POP_REG eip\n");
                    }
                    else if ( SR_disassemble_is_operand2(value + 4) )
                    {
                        OUTPUT_STRING("ldr tmp1, [esp]\n");
                        OUTPUT_PARAMSTRING("add esp, esp, #%i\n", value + 4);
                        OUTPUT_STRING("bx tmp1\n");
                    }
                    else if ( SR_disassemble_is_operand2(value) )
                    {
                        OUTPUT_STRING("POP_REG tmp1\n");
                        OUTPUT_PARAMSTRING("add esp, esp, #%i\n", value);
                        OUTPUT_STRING("bx tmp1\n");
                    }
                    else
                    {
                        OUTPUT_STRING("ldr tmp1, [esp]\n");
                        OUTPUT_PARAMSTRING("LDR tmp2, =%i\n", value + 4);
                        OUTPUT_STRING("add esp, esp, tmp2\n");
                        OUTPUT_STRING("bx tmp1\n");
                    }
                    OUTPUT_STRING("LTORG_RET\n");
                }
            }
            break;
        case UD_Irol:
        case UD_Iror:
            {
                /* CF - modified, OF - modified for 1-bit shifts, else not modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (flags_to_write)
                            {
                                fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            }

                            OUTPUT_STRING("and tmp1, ecx, #0x1f\n");
                            if (ud_obj.mnemonic == UD_Irol)
                            {
                                OUTPUT_STRING("rsb tmp1, tmp1, #32\n");
                            }
                            OUTPUT_PARAMSTRING("mov %s, %s, ror tmp1\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));

                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_arm_helper_rol_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = (SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f) % 16;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_arm_helper_rol_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base), value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = (SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f) % 8;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_arm_helper_rol_8(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), 8, value);
                            }
                        }
                    }
                }

            }
            break;
        case UD_Isahf:
            {
                /* OF - not modified, CF,SF,ZF,AF,PF - modified */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (flags_to_write & (FL_ADJUST | FL_PARITY))
                {
                    OUTPUT_STRING("bic eflags, eflags, #0x0014\n");
                    OUTPUT_STRING("and tmp1, eax, #0x1400\n");
                    OUTPUT_STRING("orr eflags, eflags, tmp1, lsr #8\n");
                }
                if (flags_to_write & (FL_SIGN | FL_ZERO | FL_CARRY))
                {
                    OUTPUT_STRING("mrs tmp1, cpsr\n");
                    OUTPUT_STRING("bic tmp1, tmp1, #0xe0000000\n");

                    if (flags_to_write & (FL_SIGN | FL_ZERO))
                    {
                        OUTPUT_STRING("and tmp2, eax, #0xc000\n");
                        OUTPUT_STRING("orr tmp1, tmp1, tmp2, lsl #16\n");
                    }
                    if (flags_to_write & FL_CARRY)
                    {
                        if (flags_to_write & FL_CARRY_SUB_INVERTED)
                        {
                            OUTPUT_STRING("and tmp2, eax, #0x0100\n");
                            OUTPUT_STRING("orr tmp1, tmp1, tmp2, lsl #21\n");
                        }
                        else
                        {
                            OUTPUT_STRING("orr tmp1, tmp1, #0x20000000\n");
                            OUTPUT_STRING("and tmp2, eax, #0x0100\n");
                            OUTPUT_STRING("eor tmp1, tmp1, tmp2, lsl #21\n");
                        }
                    }

                    OUTPUT_STRING("msr cpsr_f, tmp1\n");
                }

            }
            break;
        case UD_Isal:
        case UD_Isar:
        case UD_Ishl:
        case UD_Ishr:
            {
                /* SF,ZF,PF,CF - modified, AF - undefined, OF - modified for 1-bit shifts, else undefined */
                char *ishift;

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.mnemonic == UD_Isar) ishift = "asr";
                else if (ud_obj.mnemonic == UD_Ishr) ishift = "lsr";
                else ishift = "lsl";

                updcond = (char *) ((flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN))?updatecond:empty);

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (flags_to_write)
                            {
                                fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            }

                            OUTPUT_STRING("and tmp1, ecx, #0x1f\n");
                            OUTPUT_PARAMSTRING("mov%s %s, %s, %s tmp1\n", updcond, X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base), ishift);

                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_arm_helper_sal_32(ud_obj.mnemonic, X86322ARMREG(ud_obj.operand[0].base), value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (flags_to_write)
                            {
                                fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            }

                            OUTPUT_STRING("and tmp1, ecx, #0x1f\n");

                            if (ud_obj.mnemonic == UD_Isar || ud_obj.mnemonic == UD_Ishr)
                            {
                                OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("and %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("mov tmp2, tmp2, %s tmp1\n", ishift);
                                OUTPUT_PARAMSTRING("and %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mov%s tmp2, %s, %s tmp1\n", updcond, X862ARMSTR(ud_obj.operand[0].base), ishift);
                                OUTPUT_PARAMSTRING("and %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("mov tmp2, tmp2, lsl #16\n");
                                OUTPUT_PARAMSTRING("and %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_arm_helper_sal_16(ud_obj.mnemonic, X86162ARMREG(ud_obj.operand[0].base), value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (flags_to_write)
                            {
                                fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            }

                            OUTPUT_STRING("and tmp1, ecx, #0x1f\n");
                            if (ud_obj.mnemonic == UD_Isar || ud_obj.mnemonic == UD_Ishr)
                            {
                                OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #24\n", X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("mov tmp2, tmp2, %s #24\n", ishift);
                                OUTPUT_PARAMSTRING("mov%s tmp2, tmp2, %s tmp1\n", updcond, ishift);
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mov%s tmp2, %s, %s tmp1\n", updcond, X862ARMSTR(ud_obj.operand[0].base), ishift);
                            }
                            OUTPUT_STRING("and tmp2, tmp2, #0x00ff\n");
                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp2\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_arm_helper_sal_8(ud_obj.mnemonic, X868L2ARMREG(ud_obj.operand[0].base), 0, value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (flags_to_write)
                            {
                                fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            }

                            OUTPUT_STRING("and tmp1, ecx, #0x1f\n");
                            if (ud_obj.mnemonic == UD_Isar || ud_obj.mnemonic == UD_Ishr)
                            {
                                OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("mov tmp2, tmp2, %s #16\n", ishift);
                                OUTPUT_PARAMSTRING("mov%s tmp2, tmp2, %s tmp1\n", updcond, ishift);
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("mov%s tmp2, tmp2, %s tmp1\n", updcond, ishift);
                            }

                            OUTPUT_STRING("and tmp2, tmp2, #0xff00\n");
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp2\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_arm_helper_sal_8(ud_obj.mnemonic, X868H2ARMREG(ud_obj.operand[0].base), 8, value);
                            }
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {

                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, (value)?(flags_to_write & ~(FL_COND_ALL)):flags_to_write);

                                SR_arm_helper_sal_32(ud_obj.mnemonic, AR_TMP1, value);

                                SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {

                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                                SR_arm_helper_sal_16(ud_obj.mnemonic, AR_TMP1, value);

                                SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {

                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM || ud_obj.operand[1].type == UD_OP_CONST)
                        {
                            uint32_t value;

                            if (ud_obj.operand[1].type == UD_OP_CONST)
                            {
                                value = 1;
                            }
                            else
                            {
                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) & 0x1f;
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                                SR_arm_helper_sal_8(ud_obj.mnemonic, AR_TMP1, 0, value);

                                SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                            }
                        }
                    }

                }

            }
            break;
        case UD_Iscasb:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                if (flags_to_write & FL_CARRY_SUB_INVERTED) flags_write |= FL_CARRY_SUB_INVERTED;

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    SR_get_label(cLabel, section[Entry].start + cur_ofs);

                    OUTPUT_STRING("cmp ecx, #0\n");
                    OUTPUT_PARAMSTRING("beq %s_after_scasb\n", cLabel);

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #9\n");  // tmpadr = 0 or 2
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #1\n");      // tmpadr = 1 or -1

                    OUTPUT_STRING("and tmp2, eax, #0x00ff\n");

                    OUTPUT_PARAMSTRING("%s_loop:\n", cLabel);

                    OUTPUT_STRING("ldrb tmp1, [edi], tmpadr\n");

                    OUTPUT_STRING("cmp tmp2, tmp1\n");

                    if (ud_obj.pfx_repne)
                    {
                        OUTPUT_PARAMSTRING("beq %s_set_flags\n", cLabel);
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("bne %s_set_flags\n", cLabel);
                    }

                    OUTPUT_STRING("subS ecx, ecx, #1\n");
                    OUTPUT_PARAMSTRING("bne %s_loop\n", cLabel);

                    OUTPUT_STRING("add ecx, ecx, #1\n");

                    OUTPUT_PARAMSTRING("%s_set_flags:\n", cLabel);

                    OUTPUT_STRING("sub ecx, ecx, #1\n");

                    if (flags_to_write)
                    {
                        SR_arm_helper_add_8l(UD_Icmp, AR_TMP1, AR_EAX, AR_TMP1, 0);
                    }

                    OUTPUT_PARAMSTRING("%s_after_scasb:\n", cLabel);
                }
                else
                {
                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #9\n");  // tmpadr = 0 or 2
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #1\n");      // tmpadr = 1 or -1

                    if (flags_to_write)
                    {
                        OUTPUT_STRING("ldrb tmp1, [edi], tmpadr\n");

                        SR_arm_helper_add_8l(UD_Icmp, AR_TMP1, AR_EAX, AR_TMP1, 0);
                    }
                    else
                    {
                        OUTPUT_STRING("add edi, edi, tmpadr\n");
                    }
                }

            }
            break;
        case UD_Iseta:
        case UD_Isetb:
        case UD_Isetbe:
        case UD_Isetnb:
        case UD_Isetnbe:
        case UD_Isetg:
        case UD_Isetge:
        case UD_Isetl:
        case UD_Isetle:
        case UD_Isetnl:
        case UD_Isetnle:
        case UD_Isetnz:
        case UD_Isetz:
            {
                /* no flags affected */

                char *cond;

                if (ud_obj.mnemonic == UD_Isetb) cond = "lo";
                else if (ud_obj.mnemonic == UD_Isetnb) cond = "hs";
                else if (ud_obj.mnemonic == UD_Isetbe) cond = "ls";
                else if (ud_obj.mnemonic == UD_Iseta || ud_obj.mnemonic == UD_Isetnbe) cond = "hi";
                else if (ud_obj.mnemonic == UD_Isetg || ud_obj.mnemonic == UD_Isetnle) cond = "gt";
                else if (ud_obj.mnemonic == UD_Isetl) cond = "lt";
                else if (ud_obj.mnemonic == UD_Isetle) cond = "le";
                else if (ud_obj.mnemonic == UD_Isetz) cond = "eq";
                else if (ud_obj.mnemonic == UD_Isetnz) cond = "ne";
                else /*if (ud_obj.mnemonic == UD_Isetge || ud_obj.mnemonic == UD_Isetnl)*/ cond = "ge";

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("orr%s %s, %s, #1\n", cond, X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                    OUTPUT_STRING("mov tmp1, #0\n");
                    OUTPUT_PARAMSTRING("orr%s tmp1, tmp1, #1\n", cond);

                    SR_disassemble_write_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, WRITE8LOW);
                }
            }
            break;
        case UD_Ishld:
        case UD_Ishrd:
            {
                /* SF,ZF,PF,CF - modified, AF - undefined, OF - modified for 1-bit shifts, else undefined */
                char *ishift, *ishift_r;

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.mnemonic == UD_Ishld)
                {
                    ishift = "lsl";
                    ishift_r = "lsr";
                }
                else
                {
                    ishift = "lsr";
                    ishift_r = "lsl";
                }


                updcond = (char *) ((flags_to_write & (FL_ZERO | FL_SIGN))?updatecond:empty);

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[2].type == UD_OP_REG)
                        {
                            if (flags_to_write)
                            {
                                fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            }

                            OUTPUT_STRING("and tmp1, ecx, #0x1f\n");
                            OUTPUT_STRING("rsb tmp2, tmp1, #32\n");
                            OUTPUT_PARAMSTRING("mov %s, %s, %s tmp1\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base), ishift);
                            OUTPUT_PARAMSTRING("mov tmp3, %s, %s tmp2\n", X86REGSTR(ud_obj.operand[1].base), ishift_r);
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp3\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));

                        }
                        else if (ud_obj.operand[2].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[2]), ZERO_EXTEND) & 0x1f;

                            if (value == 0)
                            {
                                flags_write = 0;
                                updcond = (char *) empty;
                            }

                            if (flags_to_write)
                            {
                                fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            }

                            OUTPUT_PARAMSTRING("mov %s, %s, %s #%i\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base), ishift, value);
                            OUTPUT_PARAMSTRING("mov tmp1, %s, %s #%i\n", X86REGSTR(ud_obj.operand[1].base), ishift_r, 32 - value);
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                        }
                    }
                }

            }
            break;
        case UD_Istc:
            {
                /* set carry flag */

                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if ( (flags_to_write & FL_CARRY_SUB_INVERTED) || !(flags_to_write & FL_CARRY_SUB_ORIGINAL))
                {
                    SR_disassemble_change_arm_flags(cOutput, 0, FL_CARRY, 0, AR_TMP1);
                }
                else
                {
                    SR_disassemble_change_arm_flags(cOutput, FL_CARRY, 0, 0, AR_TMP1);
                }

            }
            break;
        case UD_Istd:
            {
                /* set direction flag */

                OUTPUT_STRING("orr eflags, eflags, #0x0400\n");
            }
            break;
        case UD_Isti:
            {
                /* set interrupt flag */

                OUTPUT_STRING("STI\n");
            }
            break;
        case UD_Istosb:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("mrs tmp3, cpsr\n");
                    }

                    OUTPUT_STRING("teq ecx, #0\n");
                    OUTPUT_STRING("blne x86_rep_stosb\n");

                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("msr cpsr_f, tmp3\n");
                    }
                }
                else
                {
                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #9\n");  // tmpadr = 0 or 2
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #1\n");      // tmpadr = 1 or -1
                    OUTPUT_STRING("strb eax, [edi], tmpadr\n");
                }
            }
            break;
        case UD_Istosd:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("mrs tmp3, cpsr\n");
                    }

                    OUTPUT_STRING("teq ecx, #0\n");

                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("blne x86_rep_stosd_4\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("blne x86_rep_stosd_1\n");

                    OUTPUT_STRING(".endif\n");

                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("msr cpsr_f, tmp3\n");
                    }
                }
                else
                {
                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #7\n");  // tmpadr = 0 or 8
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #4\n");      // tmpadr = 4 or -4
                    OUTPUT_STRING("str eax, [edi], tmpadr\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("mov tmp1, eax, lsr #8\n");
                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("strb tmp1, [edi, #1]\n");
                    OUTPUT_STRING("mov tmp1, eax, lsr #16\n");
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #7\n");  // tmpadr = 0 or 8
                    OUTPUT_STRING("strb tmp1, [edi, #2]\n");
                    OUTPUT_STRING("mov tmp1, eax, lsr #24\n");
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #4\n");      // tmpadr = 4 or -4
                    OUTPUT_STRING("strb tmp1, [edi, #3]\n");
                    OUTPUT_STRING("strb eax, [edi], tmpadr\n");

                    OUTPUT_STRING(".endif\n");
                }
            }
            break;
        case UD_Istosw:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("mrs tmp3, cpsr\n");
                    }

                    OUTPUT_STRING("teq ecx, #0\n");

                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("blne x86_rep_stosw_2\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("blne x86_rep_stosw_1\n");

                    OUTPUT_STRING(".endif\n");

                    if (flags_to_write & (FL_CARRY | FL_ZERO | FL_SIGN) )
                    {
                        OUTPUT_STRING("msr cpsr_f, tmp3\n");
                    }
                }
                else
                {
                    OUTPUT_STRING(".ifdef ALLOW_UNALIGNED_MEMORY_ACCESS\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #8\n");  // tmpadr = 0 or 4
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #2\n");      // tmpadr = 2 or -2
                    OUTPUT_STRING("strh eax, [edi], tmpadr\n");

                    OUTPUT_STRING(".else\n");

                    OUTPUT_STRING("and tmpadr, eflags, #0x0400\n"); // tmpadr = 0 or 0x400     (direction flag)
                    OUTPUT_STRING("mov tmp1, eax, lsr #8\n");
                    OUTPUT_STRING("mov tmpadr, tmpadr, lsr #8\n");  // tmpadr = 0 or 4
                    OUTPUT_STRING("strb tmp1, [edi, #1]\n");
                    OUTPUT_STRING("rsb tmpadr, tmpadr, #2\n");      // tmpadr = 2 or -2
                    OUTPUT_STRING("strb eax, [edi], tmpadr\n");

                    OUTPUT_STRING(".endif\n");
                }
            }
            break;
        case UD_Itest:
            {
                /* CF,OF cleared, AF undefined, SF,ZF,PF - modified */
                if (flags_to_write & FL_CARRY_SUB_ORIGINAL) flags_write |= FL_CARRY_SUB_ORIGINAL;

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_and_32(ud_obj.mnemonic, AR_TMP1, X86322ARMREG(ud_obj.operand[0].base), X86322ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_arm_helper_and_32(ud_obj.mnemonic, AR_TMP1, X86322ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_arm_helper_and_16(ud_obj.mnemonic, AR_TMP1, X86162ARMREG(ud_obj.operand[0].base), X86162ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            SR_arm_helper_and_16(ud_obj.mnemonic, AR_TMP1, X86162ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_arm_helper_and_8l(ud_obj.mnemonic, AR_TMP1, X868L2ARMREG(ud_obj.operand[0].base), X868L2ARMREG(ud_obj.operand[1].base), 0);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_arm_helper_and_8l(ud_obj.mnemonic, AR_TMP1, X868L2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_arm_helper_and_8h(ud_obj.mnemonic, AR_TMP1, X868H2ARMREG(ud_obj.operand[0].base), X868H2ARMREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_arm_helper_and_8h(ud_obj.mnemonic, AR_TMP1, X868H2ARMREG(ud_obj.operand[0].base), AR_NONE, value);
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                            SR_arm_helper_and_32(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X86322ARMREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write & ~(FL_COND_ALL));

                                SR_arm_helper_and_32(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            SR_arm_helper_and_16(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                                SR_arm_helper_and_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X868L2ARMREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                                SR_arm_helper_and_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, X868H2ARMREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_arm_helper_and_8l(ud_obj.mnemonic, AR_TMP1, AR_TMP1, AR_NONE, value);
                        }
                    }
                }
            }
            break;
        case UD_Iverr:
            {
                /* ZF - modified */

                SR_disassemble_change_arm_flags(cOutput, 0, FL_ZERO, 0, AR_TMP1);
            }
            break;
        case UD_Ixchg:
            {
                /* no flags affected */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, %s\n", X86REGSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("mov %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("mov %s, tmp1\n", X86REGSTR(ud_obj.operand[1].base));
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("mov tmp2, %s, lsl #16\n", X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_STRING("LDR tmp3, =0xffff\n");
                            OUTPUT_PARAMSTRING("bic %s, %s, tmp3\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bic %s, %s, tmp3\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsr #16\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("and tmp2, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[1].base));

                                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));

                                OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsr #8\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsl #8\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));
                            }
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("and tmp2, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[1].base));

                                OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));

                                OUTPUT_PARAMSTRING("orr %s, %s, tmp2, lsl #8\n", X862ARMSTR(ud_obj.operand[0].base), X862ARMSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp1, lsr #8\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                            }
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);

                            SR_disassemble_write_mem_word(cOutput, &memadr, X862ARMREG(ud_obj.operand[1].base), flags_to_write);

                            OUTPUT_PARAMSTRING("mov %s, tmp1\n", X86REGSTR(ud_obj.operand[1].base));
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, X862ARMREG(ud_obj.operand[1].base), AR_TMP3);

                            OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("bic %s, %s, #0xff00\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("orr %s, %s, tmp1\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, AR_TMP1, AR_TMP3, READ8TO32ZERO);

                                SR_disassemble_write_mem_byte(cOutput, &memadr, X862ARMREG(ud_obj.operand[1].base), AR_TMP3, WRITE8LOW);

                                OUTPUT_PARAMSTRING("bic %s, %s, #0x00ff\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("orr %s, %s, tmp1\n", X862ARMSTR(ud_obj.operand[1].base), X862ARMSTR(ud_obj.operand[1].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                            }

                        }
                    }

                }
            }
            break;
#if (EMULATE_FPU)
        case UD_Ifadd:
        case UD_Ifdiv:
        case UD_Ifmul:
        case UD_Ifsub:
            {
                /* no flags affected */

                char *instr;

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.mnemonic == UD_Ifadd) instr = "fadd";
                else if (ud_obj.mnemonic == UD_Ifdiv) instr = "fdiv";
                else if (ud_obj.mnemonic == UD_Ifmul) instr = "fmul";
                else /*if (ud_obj.mnemonic == UD_Ifsub)*/ instr = "fsub";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bl x87_%s_st\n", instr);
                        }
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr, flags_to_write);

                            OUTPUT_PARAMSTRING("bl x87_%s_double\n", instr);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);

                            OUTPUT_PARAMSTRING("bl x87_%s_float\n", instr);
                        }
                    }
                }
                else
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            if (ud_obj.operand[1].type == UD_OP_REG)
                            {
                                if (ud_obj.operand[1].base >= UD_R_ST0 && ud_obj.operand[1].base <= UD_R_ST7)
                                {
                                    if (ud_obj.operand[0].base == UD_R_ST0)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[1].base));
                                        OUTPUT_PARAMSTRING("bl x87_%s_st\n", instr);
                                    }
                                    else if (ud_obj.operand[1].base == UD_R_ST0)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[0].base));
                                        OUTPUT_PARAMSTRING("bl x87_%s_to_st\n", instr);
                                    }
                                }
                            }
                        }
                    }
                }

            }
            break;
        case UD_Ifaddp:
        case UD_Ifdivp:
        case UD_Ifmulp:
        case UD_Ifsubp:
            {
                /* no flags affected */

                char *instr;

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.mnemonic == UD_Ifaddp) instr = "faddp";
                else if (ud_obj.mnemonic == UD_Ifdivp) instr = "fdivp";
                else if (ud_obj.mnemonic == UD_Ifmulp) instr = "fmulp";
                else /*if (ud_obj.mnemonic == UD_Ifsubp)*/ instr = "fsubp";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bl x87_%s_st\n", instr);
                        }
                    }
                }
                else
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            if (ud_obj.operand[1].type == UD_OP_REG)
                            {
                                if (ud_obj.operand[1].base >= UD_R_ST0 && ud_obj.operand[1].base <= UD_R_ST7)
                                {
                                    if (ud_obj.operand[1].base == UD_R_ST0)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[0].base));
                                        OUTPUT_PARAMSTRING("bl x87_%s_st\n", instr);
                                    }
                                }
                            }
                        }
                    }
                }

            }
            break;
        case UD_Ifabs:
        case UD_Ifchs:
        case UD_Ifcompp:
        case UD_Ifcos:
        case UD_Ifld1:
        case UD_Ifldln2:
        case UD_Ifldz:
        case UD_Ifninit:
        case UD_Ifsin:
        case UD_Ifsqrt:
        case UD_Ifyl2x:
            {
                /* no flags affected */

                char *instr;

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.mnemonic == UD_Ifabs) instr = "fabs";
                else if (ud_obj.mnemonic == UD_Ifchs) instr = "fchs";
                else if (ud_obj.mnemonic == UD_Ifcompp) instr = "fcompp";
                else if (ud_obj.mnemonic == UD_Ifcos) instr = "fcos";
                else if (ud_obj.mnemonic == UD_Ifld1) instr = "fld1";
                else if (ud_obj.mnemonic == UD_Ifldln2) instr = "fldln2";
                else if (ud_obj.mnemonic == UD_Ifldz) instr = "fldz";
                else if (ud_obj.mnemonic == UD_Ifninit) instr = "fninit";
                else if (ud_obj.mnemonic == UD_Ifsin) instr = "fsin";
                else if (ud_obj.mnemonic == UD_Ifsqrt) instr = "fsqrt";
                else /*if (ud_obj.mnemonic == UD_Ifyl2x)*/ instr = "fyl2x";

                if (ud_obj.operand[0].type == UD_NONE)
                {
                    OUTPUT_PARAMSTRING("bl x87_%s_void\n", instr);
                }

            }
            break;
        case UD_Ifcomp:
            {
                /* no flags affected */

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr, flags_to_write);

                            OUTPUT_STRING("bl x87_fcomp_double\n");
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);

                            OUTPUT_STRING("bl x87_fcomp_float\n");
                        }
                    }
                }

            }
            break;
        case UD_Ifdivr:
        case UD_Ifsubr:
            {
                /* no flags affected */

                char *instr;

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.mnemonic == UD_Ifdivr) instr = "fdivr";
                else /*if (ud_obj.mnemonic == UD_Ifsubr)*/ instr = "fsubr";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr, flags_to_write);

                            OUTPUT_PARAMSTRING("bl x87_%s_double\n", instr);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);

                            OUTPUT_PARAMSTRING("bl x87_%s_float\n", instr);
                        }
                    }
                }
                else
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            if (ud_obj.operand[1].type == UD_OP_REG)
                            {
                                if (ud_obj.operand[1].base >= UD_R_ST0 && ud_obj.operand[1].base <= UD_R_ST7)
                                {
                                    if (ud_obj.operand[0].base == UD_R_ST0)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[1].base));
                                        OUTPUT_PARAMSTRING("bl x87_%s_st\n", instr);
                                    }
                                }
                            }
                        }
                    }
                }

            }
            break;
        case UD_Ifdivrp:
        case UD_Ifsubrp:
            {
                /* no flags affected */

                char *instr;

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.mnemonic == UD_Ifdivrp) instr = "fdivrp";
                else /*if (ud_obj.mnemonic == UD_Ifsubrp)*/ instr = "fsubrp";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("bl x87_%s_st\n", instr);
                        }
                    }
                }
                else
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            if (ud_obj.operand[1].type == UD_OP_REG)
                            {
                                if (ud_obj.operand[1].base >= UD_R_ST0 && ud_obj.operand[1].base <= UD_R_ST7)
                                {
                                    if (ud_obj.operand[1].base == UD_R_ST0)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[0].base));
                                        OUTPUT_PARAMSTRING("bl x87_%s_st\n", instr);
                                    }
                                }
                            }
                        }
                    }
                }

            }
            break;
        case UD_Ifild:
            {
                /* no flags affected */

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr, flags_to_write);

                            OUTPUT_STRING("bl x87_fild_int64\n");
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);

                            OUTPUT_STRING("bl x87_fild_int32\n");
                        }
                        else if (ud_obj.operand[0].size == 16)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32SIGN);

                            OUTPUT_STRING("bl x87_fild_int32\n");
                        }
                    }
                }

            }
            break;
        case UD_Ifistp:
            {
                /* no flags affected */

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            OUTPUT_STRING("bl x87_fistp_int64\n");

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_doubleword(cOutput, &memadr, flags_to_write);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            OUTPUT_STRING("bl x87_fistp_int32\n");

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                        }
                    }
                }

            }
            break;
        case UD_Ifld:
            {
                /* no flags affected */

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[0].base));
                            OUTPUT_STRING("bl x87_fld_st\n");
                        }
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr, flags_to_write);

                            OUTPUT_STRING("bl x87_fld_double\n");
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);

                            OUTPUT_STRING("bl x87_fld_float\n");
                        }
                    }
                }
                else
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            if (ud_obj.operand[1].type == UD_OP_REG)
                            {
                                if (ud_obj.operand[1].base >= UD_R_ST0 && ud_obj.operand[1].base <= UD_R_ST7)
                                {
                                    if (ud_obj.operand[0].base == UD_R_ST0)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[1].base));
                                        OUTPUT_STRING("bl x87_fld_st\n");
                                    }
                                }
                            }
                        }
                    }
                }

            }
            break;
        case UD_Ifldcw:
            {
                /* no flags affected */

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3, AR_TMPLR, READ16TO32ZERO);

                        OUTPUT_STRING("bl x87_fldcw_uint16\n");
                    }
                }
            }
            break;
        case UD_Ifnstcw:
        case UD_Ifnstsw:
            {
                /* no flags affected */

                char *instr;

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.mnemonic == UD_Ifnstcw) instr = "fnstcw";
                else /*if (ud_obj.mnemonic == UD_Ifnstsw)*/ instr = "fnstsw";

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_PARAMSTRING("bl x87_%s_void\n", instr);
                        OUTPUT_PARAMSTRING("mov tmp2, %s, lsr #16\n", X862ARMSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("orr %s, tmp1, tmp2, lsl #16\n", X862ARMSTR(ud_obj.operand[0].base));
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 16)
                    {
                        OUTPUT_PARAMSTRING("bl x87_%s_void\n", instr);

                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                        SR_disassemble_write_mem_halfword(cOutput, &memadr, AR_TMP1, AR_TMP3);
                    }
                }
            }
        case UD_Ifst:
            {
                /* no flags affected */

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            OUTPUT_STRING("bl x87_fst_double\n");

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_doubleword(cOutput, &memadr, flags_to_write);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            OUTPUT_STRING("bl x87_fst_float\n");

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                        }
                    }
                }

            }
            break;
        case UD_Ifstp:
            {
                /* no flags affected */

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[0].base));
                            OUTPUT_STRING("bl x87_fstp_st\n");
                        }
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            OUTPUT_STRING("bl x87_fstp_double\n");

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_doubleword(cOutput, &memadr, flags_to_write);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            OUTPUT_STRING("bl x87_fstp_float\n");

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_word(cOutput, &memadr, AR_TMP1, flags_to_write);
                        }
                    }
                }

            }
            break;
        case UD_Ifxch:
            {
                /* no flags affected */

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[0].base));
                            OUTPUT_STRING("bl x87_fxch_st\n");
                        }
                    }
                }
                else
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            if (ud_obj.operand[1].type == UD_OP_REG)
                            {
                                if (ud_obj.operand[1].base >= UD_R_ST0 && ud_obj.operand[1].base <= UD_R_ST7)
                                {
                                    if (ud_obj.operand[0].base == UD_R_ST0)
                                    {
                                        OUTPUT_PARAMSTRING("mov tmp1, #%i\n", X87REGNUM(ud_obj.operand[1].base));
                                        OUTPUT_STRING("bl x87_fxch_st\n");
                                    }
                                }
                            }
                        }
                    }
                }

            }
            break;
        case UD_Iwait:
            {
                /* no flags affected */

                if (flags_to_write)
                {
                    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                }

                OUTPUT_STRING("@ wait\n");
            }
            break;

#endif
        default:
            fprintf(stderr, "Error: unknown instruction - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);

            return 5;
    }

    if (cOutput[0] == 0)
    {
            fprintf(stderr, "Error: arm instruction error - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);

            return 7;
    }

    {
        char *newstr, *pos;

        newstr = (char *) malloc(strlen(cOutput) + strlen(output->str) + 3);
        if (newstr == NULL)
        {
            fprintf(stderr, "Error: not enough memory - %i - %i\n", Entry, (unsigned int)cur_ofs);

            return 3;
        }

        //sprintf(newstr, "@%s\n%s", output->str, cOutput);
        newstr[0] = '@';
        strcpy(&(newstr[1]), output->str);

        pos = strchr(newstr, '\n');
        while (pos != NULL)
        {
            *pos = '@';

            pos = strchr(pos, '\n');
        }

        strcat(newstr, "\n");
        strcat(newstr, cOutput);

        free(output->str);
        output->str = newstr;
    }


#ifdef DISPLAY_DISASSEMBLY
    printf("loc_%X: %s\n", section[Entry].start + cur_ofs, output->str);
#endif


    *pflags_write = flags_write;
    *pflags_read = flags_read;

    return 0;

#undef OUTPUT_PARAMSTRING
#undef OUTPUT_STRING
}

#else

int SR_disassemble_arm_instruction(unsigned int Entry, output_data *output, uint_fast32_t flags_to_write, uint_fast32_t *pflags_write, uint_fast32_t *pflags_read)
{
    return 1;
}

#endif
