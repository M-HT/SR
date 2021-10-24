/**
 *
 *  Copyright (C) 2019-2021 Roman Pauer
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


#if (OUTPUT_TYPE == OUT_LLASM)

static const char *ll_regs_str[] = {
    "error",
    "eax",
    "ecx",
    "edx",
    "ebx",
    "esp",
    "ebp",
    "esi",
    "edi",
    "eflags",
    "tmpadr",
    "tmp0",
    "tmp1",
    "tmp2",
    "tmp3",
    "tmp4",
    "tmp5",
    "tmp6",
    "tmp7",
    "0"
};

enum ll_regs {
    LR_NONE,
    LR_EAX,
    LR_ECX,
    LR_EDX,
    LR_EBX,
    LR_ESP,
    LR_EBP,
    LR_ESI,
    LR_EDI,
    LR_EFLAGS,
    LR_TMPADR,
    LR_TMP0,
    LR_TMP1,
    LR_TMP2,
    LR_TMP3,
    LR_TMP4,
    LR_TMP5,
    LR_TMP6,
    LR_TMP7,
    LR_ZERO
};

static const enum ll_regs ll_regs_table[] = {
        LR_EAX,
        LR_ECX,
        LR_EDX,
        LR_EBX,
        LR_ESP,
        LR_EBP,
        LR_ESI,
        LR_EDI
};

static char *pOutput;
static uint_fast32_t Tflags_to_write;


/*
#define X86REGSTR(x) ud_reg_tab[(x) - UD_R_AL]
#define X87REGNUM(x) ((x) - UD_R_ST0)
#define LLREGSTR(x) ll_regs_str[(x)]

#define X86322LLREG(x) ll_regs_table[(x) - UD_R_EAX]
#define X86162LLREG(x) ll_regs_table[(x) - UD_R_AX]
#define X868L2LLREG(x) ll_regs_table[(x) - UD_R_AL]
#define X868H2LLREG(x) ll_regs_table[(x) - UD_R_AH]

#define X862LLREG(x) ( \
    ((x) >= UD_R_EAX && (x) <= UD_R_EDI)?(X86322LLREG(x)):( \
    ((x) >= UD_R_AX && (x) <= UD_R_DI)?(X86162LLREG(x)):( \
    ((x) >= UD_R_AL && (x) <= UD_R_BL)?(X868L2LLREG(x)):( \
    ((x) >= UD_R_AH && (x) <= UD_R_BH)?(X868H2LLREG(x)):( \
    LR_NONE \
    )))) \
)

#define X862LLSTR(x) LLREGSTR(X862LLREG(x))
*/


static inline const char *x86regstr(enum ud_type reg) __attribute__ ((pure));
static inline int x87regnum(enum ud_type reg) __attribute__ ((const));
static inline const char *llregstr(enum ll_regs reg) __attribute__ ((pure));

static inline enum ll_regs x86322llreg(enum ud_type reg) __attribute__ ((pure));
static inline enum ll_regs x86162llreg(enum ud_type reg) __attribute__ ((pure));
static inline enum ll_regs x868l2llreg(enum ud_type reg) __attribute__ ((pure));
static inline enum ll_regs x868h2llreg(enum ud_type reg) __attribute__ ((pure));

static inline enum ll_regs x862llreg(enum ud_type reg) __attribute__ ((pure));
static inline const char *x862llstr(enum ud_type reg) __attribute__ ((pure));

static inline int lltempreg(enum ll_regs reg) __attribute__ ((const));

static inline const char *x86regstr(enum ud_type reg) { return ud_reg_tab[reg - UD_R_AL]; }
static inline int x87regnum(enum ud_type reg) { return reg - UD_R_ST0; }
static inline const char *llregstr(enum ll_regs reg) { return ll_regs_str[reg]; }

static inline enum ll_regs x86322llreg(enum ud_type reg) { return ll_regs_table[reg - UD_R_EAX]; }
static inline enum ll_regs x86162llreg(enum ud_type reg) { return ll_regs_table[reg - UD_R_AX]; }
static inline enum ll_regs x868l2llreg(enum ud_type reg) { return ll_regs_table[reg - UD_R_AL]; }
static inline enum ll_regs x868h2llreg(enum ud_type reg) { return ll_regs_table[reg - UD_R_AH]; }

static inline enum ll_regs x862llreg(enum ud_type reg)
{
    return (reg >= UD_R_EAX && reg <= UD_R_EDI)?( ll_regs_table[reg - UD_R_EAX] ):(
           (reg >= UD_R_AX && reg <= UD_R_DI)?( ll_regs_table[reg - UD_R_AX] ):(
           (reg >= UD_R_AL && reg <= UD_R_BL)?( ll_regs_table[reg - UD_R_AL] ):(
           (reg >= UD_R_AH && reg <= UD_R_BH)?( ll_regs_table[reg - UD_R_AH] ):(
           LR_NONE
           ))));
}
static inline const char *x862llstr(enum ud_type reg) { return ll_regs_str[x862llreg(reg)]; }

static inline int lltempreg(enum ll_regs reg) { return 0x0001fc00 & (1 << ( (unsigned int) reg) ); }

#define X86REGSTR(x) x86regstr(x)
#define X87REGNUM(x) x87regnum(x)
#define LLREGSTR(x) llregstr(x)

#define X86322LLREG(x) x86322llreg(x)
#define X86162LLREG(x) x86162llreg(x)
#define X868L2LLREG(x) x868l2llreg(x)
#define X868H2LLREG(x) x868h2llreg(x)

#define X862LLREG(x) x862llreg(x)
#define X862LLSTR(x) x862llstr(x)

#define LLTEMPREG(x) lltempreg(x)

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
    enum ll_regs memreg;
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

    if (extrn != NULL)
    {
        strcpy(cResult, extrn->proc);
        return;
    }

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
    SR_get_label(cLabel, section[sec].start + ofs);

    if ((sec == fixup->tsec) && (ofs == fixup->tofs))
    {
        strcpy(cResult, cLabel);
    }
    else
    {
        sprintf(cResult, "%s[%i]", cLabel, (int)((section[fixup->tsec].start + fixup->tofs) - (section[sec].start + ofs)));
    }

}

static void SR_add_label(unsigned int Entry, uint_fast32_t offset)
{
    output_data *output;

    output = section_output_list_FindEntryEqual(Entry, offset);
    if (output != NULL)
    {
        output->has_label = 1;
    }
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

// trashes LR_TMPADR, LR_TMP0
static void SR_disassemble_get_memory_address(char *ostr, enum ll_regs madrreg, const ud_operand_t *op, const fixup_data *fixup, const extrn_data *extrn, enum ud_type reg, enum madr_address_mode mode, struct madr_result *result)
{
    /*
        mode:
            0 = MADR_REG   - always put memory address into madrreg register
            1 = MADR_WRITE - instruction is writing to memory
            2 = MADR_READ  - instruction is reading from memory
            3 = MADR_RW    - instruction is reading from memory and writing to memory - in case of register overwrite, put memory address into madrreg register

        result->align:
             1 - address is divisible by 1 (address modulo 4 is 0 or 1 or 2 or 3)
             2 - address is divisible by 2 (address modulo 4 is 0 or 2)
             4 - address is divisible by 4 (address modulo 4 is 0)
            -1 - address modulo 4 is 1
            -2 - address modulo 4 is 2
            -3 - address modulo 4 is 3
    */

    char cFixupLabel[64];
    char *address_str;
    int *label_value;
    output_data *output;
    int use_madrreg, first, lshift, numop, regalign, distalign, reg1_dword_align, reg2_dword_align;
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

#define ADD_OSTR(x, ...) { sprintf(address_str, x, __VA_ARGS__); address_str += strlen(address_str); }
#define ADD_OSTR0(x) { strcpy(address_str, x); address_str += strlen(address_str); }


    result->madr[0] = 0;
    result->align = 1;
    result->memreg = LR_NONE;

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

    use_madrreg = 1;

    // try direct access
    if (mode != MADR_REG)
    {
        if (op->base)
        {
            if (op->index == UD_NONE && fixup == NULL && displacement == 0)
            {
                result->align = (DWORD_ALIGN(op->base))?4:1;
                result->memreg = X862LLREG(op->base);
                sprintf(result->madr, "%s", X86REGSTR(op->base));
                return;
            }
        }
        else if (op->index)
        {
            if (lshift == 0 && fixup == NULL && displacement == 0)
            {
                result->align = (DWORD_ALIGN(op->index))?4:1;
                result->memreg = X862LLREG(op->index);
                sprintf(result->madr, "%s", X86REGSTR(op->index));
                return;
            }
        }
        else if (fixup != NULL)
        {
            if ((op->size == 8) || (op->size == 16) || (op->size == 32))
            {
                use_madrreg = 0;
            }
        }
    }

    if (use_madrreg)
    {
        sprintf(result->madr, "%s", LLREGSTR(madrreg));
        result->memreg = madrreg;

        address_str = ostr;
    }
    else
    {
        address_str = &(result->madr[0]);
    }



    numop = ( (op->base)?1:0 ) + ( (op->index)?1:0 );
    first = 1;

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

                            fprintf(stderr, "Warning: reading instruction - %i - %i - %i (0x%x)\n", (unsigned int)fixup->sofs, (unsigned int)fixup->tsec, (unsigned int)fixup->tofs, (unsigned int)(section[fixup->tsec].start + fixup->tofs));
                        }
                        else
                        {
                            distance = (int)((section[fixup->tsec].start + fixup->tofs) - (section[sec].start + output->ofs));
                            distalign = 1;

                            fprintf(stderr, "Error: reading inside of instruction - %i - %i - %i (0x%x)\n", (unsigned int)fixup->sofs, (unsigned int)fixup->tsec, (unsigned int)fixup->tofs, (unsigned int)(section[fixup->tsec].start + fixup->tofs));
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
                            fprintf(stderr, "Warning: suspicious alignment: %i - %i - %i - %i (0x%x)\n", distalign, (unsigned int)fixup->sofs, (unsigned int)fixup->tsec, (unsigned int)fixup->tofs, (unsigned int)(section[fixup->tsec].start + fixup->tofs));
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
                    fprintf(stderr, "Warning: output not found - %i - %i (0x%x)\n", (unsigned int)fixup->tsec, (unsigned int)fixup->tofs, (unsigned int)(section[fixup->tsec].start + fixup->tofs));
                }

            }
            else
            {
                result->align = (fixup->tofs & 3)?( -(fixup->tofs & 3) ):4;
            }
        }
        else result->align = 4;

        SR_get_fixup_label(cFixupLabel, fixup, extrn);

        first = 0;
        if (use_madrreg)
        {
            ADD_OSTR("mov %s, %s\n", (numop)?LLREGSTR(LR_TMPADR):LLREGSTR(madrreg), cFixupLabel)
        }
        else
        {
            ADD_OSTR("%s", cFixupLabel)
        }
    }
    else
    {
        result->align = (displacement & 3)?( -(displacement & 3) ):4;

        if (displacement != 0)
        {
            first = 0;
            numop = 0;

            if (op->base && op->index)
            {
                if (lshift == 0)
                {
                    ADD_OSTR("add tmpadr, %s, %s\n", X86REGSTR(op->base), X86REGSTR(op->index))
                }
                else
                {
                    ADD_OSTR("shl tmpadr, %s, %i\n", X86REGSTR(op->index), lshift)
                    ADD_OSTR("add tmpadr, tmpadr, %s\n", X86REGSTR(op->base))
                }
                ADD_OSTR("add %s, tmpadr, %i\n", LLREGSTR(madrreg), displacement)
            }
            else if (op->base)
            {
                ADD_OSTR("add %s, %s, %i\n", LLREGSTR(madrreg), X86REGSTR(op->base), displacement)
            }
            else if (op->index)
            {
                if (lshift == 0)
                {
                    ADD_OSTR("add %s, %s, %i\n", LLREGSTR(madrreg), X86REGSTR(op->index), displacement)
                }
                else
                {
                    ADD_OSTR("shl tmpadr, %s, %i\n", X86REGSTR(op->index), lshift)
                    ADD_OSTR("add %s, tmpadr, %i\n", LLREGSTR(madrreg), displacement)
                }
            }
            else
            {
                ADD_OSTR("mov %s, %i\n", LLREGSTR(madrreg), displacement)
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


    if (numop)
    {
        if (first && op->base && op->index)
        {
            first = 0;
            numop = 0;

            if (lshift == 0)
            {
                ADD_OSTR("add %s, %s, %s\n", LLREGSTR(madrreg), X86REGSTR(op->base), X86REGSTR(op->index))
            }
            else
            {
                ADD_OSTR("shl tmpadr, %s, %i\n", X86REGSTR(op->index), lshift)
                ADD_OSTR("add %s, tmpadr, %s\n", LLREGSTR(madrreg), X86REGSTR(op->base))
            }
        }
        else
        {
            if (op->base)
            {
                numop--;

                if (first)
                {
                    first = 0;
                    ADD_OSTR("mov %s, %s\n", (numop)?LLREGSTR(LR_TMPADR):LLREGSTR(madrreg), X86REGSTR(op->base))
                }
                else
                {
                    ADD_OSTR("add %s, tmpadr, %s\n", (numop)?LLREGSTR(LR_TMPADR):LLREGSTR(madrreg), X86REGSTR(op->base))
                }
            }

            if (op->index)
            {
                numop--;

                if (lshift == 0)
                {
                    if (first)
                    {
                        first = 0;
                        ADD_OSTR("mov %s, %s\n", (numop)?LLREGSTR(LR_TMPADR):LLREGSTR(madrreg), X86REGSTR(op->index))
                    }
                    else
                    {
                        ADD_OSTR("add %s, tmpadr, %s\n", (numop)?LLREGSTR(LR_TMPADR):LLREGSTR(madrreg), X86REGSTR(op->index))
                    }
                }
                else
                {
                    if (first)
                    {
                        first = 0;
                        ADD_OSTR("shl %s, %s, %i\n", (numop)?LLREGSTR(LR_TMPADR):LLREGSTR(madrreg), X86REGSTR(op->index), lshift)
                    }
                    else
                    {
                        ADD_OSTR("shl tmp0, %s, %i\n", X86REGSTR(op->index), lshift)
                        ADD_OSTR("add %s, tmpadr, tmp0\n", (numop)?LLREGSTR(LR_TMPADR):LLREGSTR(madrreg))
                    }
                }
            }
        }
    }

    if (first)
    {
        ADD_OSTR("mov %s, 0\n", LLREGSTR(madrreg))
    }

#undef ADD_OSTR0
#undef ADD_OSTR
#undef DWORD_ALIGN
}

// todo: odstranit parameter mode2 ?
#define SR_disassemble_get_madr(ostr, op, fixup, extrn, reg, mode, mode2, result) SR_disassemble_get_memory_address((ostr), LR_TMPADR, (op), (fixup), (extrn), (reg), (mode), (result))

// reads LR_TMP2:LR_TMP1 from memory
// trashes LR_TMP3
static void SR_disassemble_read_mem_doubleword(char *cResult, const struct madr_result *res)
{
    int len;

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;

    ADDRESULT("load tmp1, %s, %i\n", res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
    ADDRESULT("add tmp3, %s, 4\n", res->madr)
    ADDRESULT("load tmp2, tmp3, %i\n", (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))

#undef ADDRESULT
}

static void SR_disassemble_read_mem_word(char *cResult, const struct madr_result *res, enum ll_regs dst)
{
/*
    dst != LR_TMPADR
*/
    int len;

    if (dst == LR_TMPADR)
    {
        fprintf(stderr, "Error: memory read register error: %s\n", LLREGSTR(dst));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;

    ADDRESULT("load %s, %s, %i\n", LLREGSTR(dst), res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))

#undef ADDRESULT
}

// trashes LR_TMP0
static void SR_disassemble_read_mem_halfword(char *cResult, const struct madr_result *res, enum ll_regs dst, enum halfword_read_mode mode)
{
    /*
        mode:
            0 = READ16TO16     - target is 16-bit
            1 = READ16TO32SIGN - target is 32-bit sign-extended
            2 = READ16TO32ZERO - target is 32-bit zero-extended

        dst != LR_TMPADR
        dst != LR_TMP0
    */
    int len;

    if ((dst == LR_TMPADR) || (dst == LR_TMP0))
    {
        fprintf(stderr, "Error: memory read register error: %s\n", LLREGSTR(dst));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;

    if (mode == READ16TO32ZERO)
    {
        ADDRESULT("load16z %s, %s, %i\n", LLREGSTR(dst), res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
    }
    else if (mode == READ16TO32SIGN)
    {
        ADDRESULT("load16s %s, %s, %i\n", LLREGSTR(dst), res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
    }
    else
    {
        ADDRESULT("load16z tmp0, %s, %i\n", res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
        ADDRESULT("and %s, %s, 0xffff0000\n", LLREGSTR(dst), LLREGSTR(dst))
        ADDRESULT("or %s, %s, tmp0\n", LLREGSTR(dst), LLREGSTR(dst))
    }

#undef ADDRESULT
}

// trashes LR_TMP0
static void SR_disassemble_read_mem_byte(char *cResult, const struct madr_result *res, enum ll_regs dst, enum byte_read_mode mode)
{
    /*
        mode:
            0 = READ8TO8LOW   - target is 8-bit
            1 = READ8TO8HIGH  - target is 8-bit (high)
            2 = READ8TO16SIGN - target is 16-bit sign-extended
            3 = READ8TO16ZERO - target is 16-bit zero-extended
            4 = READ8TO32SIGN - target is 32-bit sign-extended
            5 = READ8TO32ZERO - target is 32-bit zero-extended

        dst != LR_TMPADR
        dst != LR_TMP0
    */
    int len;

    if ((dst == LR_TMPADR) || (dst == LR_TMP0))
    {
        fprintf(stderr, "Error: memory read register error: %s\n", LLREGSTR(dst));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;
#define ADDRESULT0(x) strcpy(cResult, x); len = strlen(cResult); cResult += len;

    if (mode == READ8TO32ZERO)
    {
        ADDRESULT("load8z %s, %s, %i\n", LLREGSTR(dst), res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
    }
    else if (mode == READ8TO32SIGN)
    {
        ADDRESULT("load8s %s, %s, %i\n", LLREGSTR(dst), res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
    }
    else if (mode == READ8TO16ZERO)
    {
        ADDRESULT("load8z tmp0, %s, %i\n", res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
        ADDRESULT("and %s, %s, 0xffff0000\n", LLREGSTR(dst), LLREGSTR(dst))
        ADDRESULT("or %s, %s, tmp0\n", LLREGSTR(dst), LLREGSTR(dst))
    }
    else if (mode == READ8TO16SIGN)
    {
        ADDRESULT("load8s tmp0, %s, %i\n", res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
        ADDRESULT("ins16 %s, %s, tmp0\n", LLREGSTR(dst), LLREGSTR(dst))
    }
    else if (mode == READ8TO8HIGH)
    {
        ADDRESULT("load8z tmp0, %s, %i\n", res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
        ADDRESULT("and %s, %s, 0xffff00ff\n", LLREGSTR(dst), LLREGSTR(dst))
        ADDRESULT0("shl tmp0, tmp0, 8\n")
        ADDRESULT("or %s, %s, tmp0\n", LLREGSTR(dst), LLREGSTR(dst))
    }
    else
    {
        ADDRESULT("load8z tmp0, %s, %i\n", res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
        ADDRESULT("and %s, %s, 0xffffff00\n", LLREGSTR(dst), LLREGSTR(dst))
        ADDRESULT("or %s, %s, tmp0\n", LLREGSTR(dst), LLREGSTR(dst))
    }

#undef ADDRESULT0
#undef ADDRESULT
}

// writes LR_TMP2:LR_TMP1 into memory
// trashes LR_TMP3
static void SR_disassemble_write_mem_doubleword(char *cResult, const struct madr_result *res)
{
    int len;

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, (x), __VA_ARGS__); len = strlen(cResult); cResult += len;

    ADDRESULT("store tmp1, %s, %i\n", res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
    ADDRESULT("add tmp3, %s, 4\n", res->madr)
    ADDRESULT("store tmp2, tmp3, %i\n", (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))

#undef ADDRESULT
}

static void SR_disassemble_write_mem_word(char *cResult, const struct madr_result *res, enum ll_regs src)
{
    /*
        src != LR_TMPADR
    */
    int len;

    if (src == LR_TMPADR)
    {
        fprintf(stderr, "Error: memory write register error: %s\n", LLREGSTR(src));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, (x), __VA_ARGS__); len = strlen(cResult); cResult += len;

    ADDRESULT("store %s, %s, %i\n", LLREGSTR(src), res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))

#undef ADDRESULT
}

static void SR_disassemble_write_mem_halfword(char *cResult, const struct madr_result *res, enum ll_regs src)
{
    /*
        src != LR_TMPADR
    */
    int len;

    if (src == LR_TMPADR)
    {
        fprintf(stderr, "Error: memory write register error: %s\n", LLREGSTR(src));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, (x), __VA_ARGS__); len = strlen(cResult); cResult += len;

    ADDRESULT("store16 %s, %s, %i\n", LLREGSTR(src), res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))

#undef ADDRESULT
}

// trashes LR_TMP0
static void SR_disassemble_write_mem_byte(char *cResult, const struct madr_result *res, enum ll_regs src, enum byte_write_mode mode)
{
    /*
        mode:
            0 = WRITE8LOW  - source is 8-bit (low)
            1 = WRITE8HIGH - source is 8-bit (high)

        src != LR_TMPADR
        src != LR_TMP0
    */
    int len;

    if ((src == LR_TMPADR) || (src == LR_TMP0))
    {
        fprintf(stderr, "Error: memory write register error: %s\n", LLREGSTR(src));
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, (x), __VA_ARGS__); len = strlen(cResult); cResult += len;

    if (mode == WRITE8HIGH)
    {
        ADDRESULT("lshr tmp0, %s, 8\n", LLREGSTR(src))
        ADDRESULT("store8 tmp0, %s, %i\n", res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
    }
    else
    {
        ADDRESULT("store8 %s, %s, %i\n", LLREGSTR(src), res->madr, (res->align == 4)?4:( ((res->align == 2) || (res->align == -2))?2:1 ))
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

static void SR_disassemble_change_flags(char *cResult, uint_fast32_t toclear, uint_fast32_t toset, uint_fast32_t toinvert)
{
    uint_fast32_t flags[3];
    int numops, numflags, numflags2, len, counter;

    flags[0] = toclear  & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW | FL_PARITY | FL_ADJUST);
    flags[1] = toset    & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW | FL_PARITY | FL_ADJUST);
    flags[2] = toinvert & (FL_CARRY | FL_ZERO | FL_SIGN | FL_OVERFLOW | FL_PARITY | FL_ADJUST);

    numops = 0;
    if (flags[0]) numops++;
    if (flags[1]) numops++;
    if (flags[2]) numops++;

    if (numops == 0) return;

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;
#define ADDRESULT0(x) strcpy(cResult, x); len = strlen(cResult); cResult += len;

    for (counter = 0; counter <= 2; counter++)
    {
        if (flags[counter] == 0) continue;

        if (counter == 0)
        {
            ADDRESULT0("and")
        }
        else if (counter == 1)
        {
            ADDRESULT0("or")
        }
        else
        {
            ADDRESULT0("xor")
        }

        ADDRESULT0(" eflags, eflags, ")

        if (counter == 0)
        {
            ADDRESULT0("~")
        }

        // flags
        numflags = 0;
        if (flags[counter] & FL_CARRY) numflags++;
        if (flags[counter] & FL_ZERO) numflags++;
        if (flags[counter] & FL_SIGN) numflags++;
        if (flags[counter] & FL_OVERFLOW) numflags++;
        if (flags[counter] & FL_PARITY) numflags++;
        if (flags[counter] & FL_ADJUST) numflags++;

        numflags2 = numflags;

        if (numflags2 > 1)
        {
            ADDRESULT0("(")
        }

        if (flags[counter] & FL_CARRY)
        {
            ADDRESULT0("CF")
            numflags--;
            if (numflags)
            {
                ADDRESULT0("|")
            }
        }

        if (flags[counter] & FL_ZERO)
        {
            ADDRESULT0("ZF")
            numflags--;
            if (numflags)
            {
                ADDRESULT0("|")
            }
        }

        if (flags[counter] & FL_SIGN)
        {
            ADDRESULT0("SF")
            numflags--;
            if (numflags)
            {
                ADDRESULT0("|")
            }
        }

        if (flags[counter] & FL_OVERFLOW)
        {
            ADDRESULT0("OF")
            numflags--;
            if (numflags)
            {
                ADDRESULT0("|")
            }
        }

        if (flags[counter] & FL_PARITY)
        {
            ADDRESULT0("PF")
            numflags--;
            if (numflags)
            {
                ADDRESULT0("|")
            }
        }

        if (flags[counter] & FL_ADJUST)
        {
            ADDRESULT0("AF")
            numflags--;
            if (numflags)
            {
                ADDRESULT0("|")
            }
        }


        if (numflags2 > 1)
        {
            ADDRESULT0(")")
        }

        ADDRESULT0("\n")
    }

#undef ADDRESULT0
#undef ADDRESULT
}

// trashes opertmp, LR_TMP0
static void SR_disassemble_set_flags_AZSP(char *cResult, enum ll_regs res, enum ll_regs opertmp, uint_fast32_t opersize, uint_fast32_t opershift, enum ud_mnemonic_code mnemonic, uint_fast32_t tocalculate)
{
    /*
        res != opertmp
        res != LR_TMP0
        opertmp != LR_TMP0
    */
    int len;

    if ((tocalculate & (FL_ADJUST | FL_ZERO | FL_SIGN | FL_PARITY )) == 0) return;

    if ((res == opertmp) || (res == LR_TMP0) || (opertmp == LR_TMP0))
    {
        fprintf(stderr, "Error: calculate adjust/zero/sign/parity flag register error: %s, %s\n", LLREGSTR(res), LLREGSTR(opertmp));
        return;
    }

    if (((opersize != 8) && (opersize != 16) && (opersize != 32)) || ((opershift != 0) && (opershift != 8)) || ((opershift != 0) && (opersize != 8)))
    {
        fprintf(stderr, "Error: calculate adjust/zero/sign/parity flag opersize/opershift error: %i, %i\n", (unsigned int)opersize, (unsigned int)opershift);
        return;
    }

    len = strlen(cResult);
    cResult += len;

#define ADDRESULT(x, ...) sprintf(cResult, x, __VA_ARGS__); len = strlen(cResult); cResult += len;
#define ADDRESULT0(x) strcpy(cResult, x); len = strlen(cResult); cResult += len;

    if ( tocalculate & FL_ADJUST )
    {
        if ((mnemonic == UD_Iinc) || (mnemonic == UD_Idec) || (mnemonic == UD_Iadd) || (mnemonic == UD_Isub) || (mnemonic == UD_Icmp) || (mnemonic == UD_Ineg) || (mnemonic == UD_Iadc) || (mnemonic == UD_Isbb))
        {
            ADDRESULT("xor tmp0, %s, %s\n", LLREGSTR(opertmp), LLREGSTR(res))
            if (opershift)
            {
                ADDRESULT("lshr tmp0, tmp0, %i\n", (unsigned int)opershift)
            }
            ADDRESULT0("and tmp0, tmp0, AF\n")
            ADDRESULT0("or eflags, eflags, tmp0\n")
        }
        else
        {
            fprintf(stderr, "Error: calculate adjust/zero/sign/parity flag mnemonic error: %i - %i\n", (int)mnemonic, (unsigned int)opersize);
            return;
        }
    }

    if ( tocalculate & FL_ZERO )
    {
        if (opersize == 32)
        {
            ADDRESULT("cmovz %s, tmp0, ZF, 0\n", LLREGSTR(res))
        }
        else if (opersize == 16)
        {
            ADDRESULT("and tmp0, %s, 0xffff\n", LLREGSTR(res))
            ADDRESULT0("cmovz tmp0, tmp0, ZF, 0\n")
        }
        else //if (opersize == 8)
        {
            ADDRESULT("and tmp0, %s, 0x%x\n", LLREGSTR(res), 0xff << opershift)
            ADDRESULT0("cmovz tmp0, tmp0, ZF, 0\n")
        }
        ADDRESULT0("or eflags, eflags, tmp0\n")
    }

    if ( tocalculate & FL_SIGN )
    {
        ADDRESULT("and tmp0, %s, 0x%x\n", LLREGSTR(res), ((uint32_t)0x80000000) >> (32 - (opersize + opershift)))
        ADDRESULT0("cmovz tmp0, tmp0, 0, SF\n")
        ADDRESULT0("or eflags, eflags, tmp0\n")
    }

    if ( tocalculate & FL_PARITY )
    {
/*
unsigned int v;  // word value to compute the parity of
v ^= v >> 16;
v ^= v >> 8;
v ^= v >> 4;
v &= 0xf;
return (0x6996 >> v) & 1;

0110 1001 1001 0110 (0x6996 in hex)
1001 0110 0110 1001  0x9669
*/

        ADDRESULT("lshr tmp0, %s, 4\n", LLREGSTR(res));
        ADDRESULT("xor tmp0, %s, tmp0\n", LLREGSTR(res));

        if (opershift)
        {
            ADDRESULT("lshr tmp0, tmp0, %i\n", (unsigned int)opershift)
        }

        ADDRESULT0("and tmp0, tmp0, 0xf\n");
        ADDRESULT("mov %s, 0x9669 << PF_SHIFT\n", LLREGSTR(opertmp));
        ADDRESULT("lshr tmp0, %s, tmp0\n", LLREGSTR(opertmp));

        ADDRESULT0("and tmp0, tmp0, PF\n")
        ADDRESULT0("or eflags, eflags, tmp0\n")
    }

#undef ADDRESULT0
#undef ADDRESULT
}

#include "SR_full_llasm_instr_helper.h"

int SR_disassemble_llasm_instruction(unsigned int Entry, output_data *output, uint_fast32_t flags_to_write, uint_fast32_t *pflags_write, uint_fast32_t *pflags_read, int *plast_instruction)
{
    char cOutput[8192];
    char cOutPart[256];
    char cAddress[128];
    char cLabel[128];
    fixup_data *fixup1, *fixup2, *fixup[3];
    extrn_data *extrn[3];
    uint_fast32_t cur_ofs, flags_write, flags_read;
    unsigned int decoded_length;
    int last_instruction;
    struct madr_result memadr;

    cur_ofs = output->ofs;
    decoded_length = ud_insn_len(&ud_obj);
    last_instruction = 0;
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
        case UD_Iadc:
        case UD_Isbb:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_llasm_helper_adc_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            SR_llasm_helper_adc_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), LR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_llasm_helper_adc_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), LR_NONE, value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_llasm_helper_adc_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[1].base), 0);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_llasm_helper_adc_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) << 24;

                            SR_llasm_helper_adc_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), LR_NONE, value);
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
                                OUTPUT_PARAMSTRING("lshr tmp1, %s, 8\n", X862LLSTR(ud_obj.operand[0].base));

                                if (ud_obj.operand[0].base == ud_obj.operand[1].base)
                                {
                                    SR_llasm_helper_adc_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, 0);
                                }
                                else
                                {
                                    SR_llasm_helper_adc_8l(ud_obj.mnemonic, LR_TMP1, X868H2LLREG(ud_obj.operand[1].base), 8);
                                }

                                OUTPUT_PARAMSTRING("ins8hl %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) << 24;

                            OUTPUT_PARAMSTRING("lshr tmp1, %s, 8\n", X862LLSTR(ud_obj.operand[0].base));

                            SR_llasm_helper_adc_8l(ud_obj.mnemonic, LR_TMP1, LR_NONE, value);

                            OUTPUT_PARAMSTRING("ins8hl %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
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

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            SR_llasm_helper_adc_32(ud_obj.mnemonic, LR_TMP1, X86322LLREG(ud_obj.operand[1].base), 0);

                            SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                                SR_llasm_helper_adc_32(ud_obj.mnemonic, LR_TMP1, LR_NONE, value);

                                SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
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

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (SR_llasm_helper_add_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            if (SR_llasm_helper_add_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[0].base), LR_TMP1, 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                if (SR_llasm_helper_add_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[0].base), LR_NONE, value))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                            else
                            {
                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("mov tmp1, %s\n", cAddress);

                                if (SR_llasm_helper_add_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[0].base), LR_TMP1, 0))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_llasm_helper_add_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            SR_llasm_helper_add_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[0].base), LR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            SR_llasm_helper_add_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[0].base), LR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_llasm_helper_add_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_llasm_helper_add_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            SR_llasm_helper_add_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[0].base), LR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_llasm_helper_add_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[0].base), LR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_llasm_helper_add_8h(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_llasm_helper_add_8h(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            SR_llasm_helper_add_8h(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[0].base), LR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_llasm_helper_add_8h(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[0].base), LR_NONE, value);
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

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            if (SR_llasm_helper_add_32(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X86322LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }

                            SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                                if (SR_llasm_helper_add_32(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value))
                                {
                                    flags_write = flags_to_write;
                                }

                                SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                            }

                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            SR_llasm_helper_add_16(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X86162LLREG(ud_obj.operand[1].base), 0);

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = ((int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND)) << 16;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            SR_llasm_helper_add_16(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value);

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                SR_llasm_helper_add_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X868L2LLREG(ud_obj.operand[1].base), 0);

                                SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                SR_llasm_helper_add_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X868H2LLREG(ud_obj.operand[1].base), 8);

                                SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            SR_llasm_helper_add_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value);

                            SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
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

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (SR_llasm_helper_and_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            if (SR_llasm_helper_and_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[0].base), LR_TMP1, 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                if (SR_llasm_helper_and_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[0].base), LR_NONE, value))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_llasm_helper_and_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[1].base), 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            SR_llasm_helper_and_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[0].base), LR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            SR_llasm_helper_and_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[0].base), LR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_llasm_helper_and_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_llasm_helper_and_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            SR_llasm_helper_and_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[0].base), LR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_llasm_helper_and_8l(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[0].base), LR_NONE, value);
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_llasm_helper_and_8h(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[1].base), 0);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_llasm_helper_and_8h(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[1].base), 8);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            SR_llasm_helper_and_8h(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[0].base), LR_TMP1, 0);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_llasm_helper_and_8h(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[0].base), LR_NONE, value);
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

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            if (SR_llasm_helper_and_32(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X86322LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }

                            SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                                if (SR_llasm_helper_and_32(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value))
                                {
                                    flags_write = flags_to_write;
                                }

                                SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            SR_llasm_helper_and_16(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X86162LLREG(ud_obj.operand[1].base), 0);

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            SR_llasm_helper_and_16(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value);

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                SR_llasm_helper_and_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X868L2LLREG(ud_obj.operand[1].base), 0);

                                SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_llasm_helper_and_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value);

                            SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
                        }
                    }
                }
            }
            break;
        case UD_Ibsr:
            {
                /* CF,OF,AF,SF,PF undefined, ZF - modified */

                if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                {
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        if (flags_to_write & FL_ZERO)
                        {
                            SR_disassemble_change_flags(pOutput, flags_to_write, 0, 0);
                        }

                        OUTPUT_PARAMSTRING("ctlz tmp1, %s\n", X86REGSTR(ud_obj.operand[1].base));
                        OUTPUT_PARAMSTRING("sub %s, 31, tmp1\n", X86REGSTR(ud_obj.operand[0].base));

                        if (flags_to_write & FL_ZERO)
                        {
                            OUTPUT_STRING("and tmp2, tmp1, 0x20\n");
                            OUTPUT_STRING("shl tmp2, tmp2, ZF_SHIFT - 5\n");
                            OUTPUT_STRING("or eflags, eflags, tmp2\n");
                        }
                    }
                }
            }
            break;
        case UD_Ibswap:
            {
                /* no flags affected */
                if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                {
                    OUTPUT_PARAMSTRING("bswap %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                }
            }
            break;
        case UD_Ibt:
            {
                /* ZF,OF,AF,SF,PF undefined, CF - modified */

                if (flags_to_write & FL_CARRY)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 32)
                        {
                            if (ud_obj.operand[1].type == UD_OP_IMM)
                            {
                                uint32_t value;

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_REG, ZERO_EXTEND, &memadr);
                                memadr.align = 1;

                                value = ud_obj.operand[1].lval.ubyte & 0x1f;

                                if (value >= 8)
                                {
                                    OUTPUT_PARAMSTRING("add tmpadr, tmpadr, %i\n", value >> 3);

                                    value &= 7;
                                }

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                SR_disassemble_change_flags(pOutput, FL_CARRY, 0, 0);

                                if (value != 0)
                                {
                                    OUTPUT_PARAMSTRING("lshr tmp1, tmp1, %i\n", value);
                                }
                                OUTPUT_STRING("and tmp1, tmp1, CF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp1\n");
                            }
                        }
                        else if (ud_obj.operand[0].size == 16)
                        {
                            if (ud_obj.operand[1].type == UD_OP_REG)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_REG, ZERO_EXTEND, &memadr);
                                memadr.align = 1;

                                OUTPUT_PARAMSTRING("ext16s tmp1, %s\n", X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_STRING("ashr tmp1, tmp1, 3\n");
                                OUTPUT_STRING("add tmpadr, tmpadr, tmp1\n");

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                SR_disassemble_change_flags(pOutput, FL_CARRY, 0, 0);

                                OUTPUT_PARAMSTRING("and tmp2, %s, 7\n", X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_STRING("lshr tmp1, tmp1, tmp2\n");
                                OUTPUT_STRING("and tmp1, tmp1, CF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp1\n");
                            }
                        }
                    }
                }
                else
                {
                    OUTPUT_STRING("NOP\n");
                }

            }
            break;
        case UD_Ibts:
            {
                /* ZF,OF,AF,SF,PF undefined, CF - modified */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (flags_to_write & FL_CARRY)
                            {
                                SR_disassemble_change_flags(pOutput, FL_CARRY, 0, 0);

                                if (ud_obj.operand[1].lval.ubyte & 0x1f)
                                {
                                    OUTPUT_PARAMSTRING("lshr tmp1, %s, %i\n", X86REGSTR(ud_obj.operand[0].base), ud_obj.operand[1].lval.ubyte & 0x1f);
                                    OUTPUT_STRING("and tmp1, tmp1, CF\n");
                                }
                                else
                                {
                                    OUTPUT_PARAMSTRING("and tmp1, %s, CF\n", X86REGSTR(ud_obj.operand[0].base));
                                }
                                OUTPUT_STRING("or eflags, eflags, tmp1\n");
                            }

                            OUTPUT_PARAMSTRING("or %s, %s, (1 << %i)\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base), ud_obj.operand[1].lval.ubyte & 0x1f);
                        }
                    }
                }
            }
            break;
        case UD_Icall:
            {
                /* no flags affected */
                SR_get_label(cLabel, section[Entry].start + cur_ofs + decoded_length);

                last_instruction = -1;

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
                        if ( SR_disassemble_find_noret(address) )
                        {
                            strcpy(cLabel, "0 ; no return");
                            last_instruction = -2;
                        }

                        SR_get_label(cAddress, address);
                    }

                    if (altaction == NULL)
                    {
                        //OUTPUT_STRING("ACTION_CALL\n");

                        OUTPUT_PARAMSTRING("PUSH %s\n", cLabel);

                        OUTPUT_PARAMSTRING("tcall %s\n", cAddress);
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("%s\n", altaction);
                        last_instruction = 1;
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM &&
                         ud_obj.operand[0].size == 32)
                {
                    if (extrn[0] != NULL && extrn[0]->is_import)
                    {
                        if (ud_obj.operand[0].base == UD_NONE &&
                            ud_obj.operand[0].index == UD_NONE
                           )
                        {
                            //OUTPUT_STRING("ACTION_CALL\n");
                            OUTPUT_PARAMSTRING("PUSH %s\n", cLabel);
                            OUTPUT_PARAMSTRING("tcall %s\n", extrn[0]->proc);
                        }
                    }
                    else
                    {
                        //OUTPUT_STRING("ACTION_CALL\n");

                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP2);

                        if (ud_obj.br_far)
                        {
                            OUTPUT_STRING("PUSH 0\n");
                        }

                        OUTPUT_PARAMSTRING("PUSH %s\n", cLabel);

                        OUTPUT_STRING("tcall tmp2\n");
                    }
                }
                else if (fixup[0] == NULL &&
                         ud_obj.operand[0].type == UD_OP_REG &&
                         ud_obj.operand[0].size == 32)
                {
                    //OUTPUT_STRING("ACTION_CALL\n");

                    OUTPUT_PARAMSTRING("PUSH %s\n", cLabel);

                    OUTPUT_PARAMSTRING("tcall %s\n", X86REGSTR(ud_obj.operand[0].base));
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
                        //OUTPUT_STRING("ACTION_CALL\n");

                        OUTPUT_PARAMSTRING("PUSH %s\n", cLabel);

                        OUTPUT_PARAMSTRING("tcall %s\n", cAddress);
                    }
                    else
                    {
                        OUTPUT_PARAMSTRING("%s\n", altaction);
                        last_instruction = 1;
                    }

                }

                if ((last_instruction == -1) && (cOutput[0] != 0))
                {
                    SR_add_label(Entry, cur_ofs + decoded_length);
                }
            }
            break;
        case UD_Icbw:
            {
                /* no flags affected */

                OUTPUT_STRING("ext8s tmp1, eax\n");
                OUTPUT_STRING("ins16 eax, eax, tmp1\n");
            }
            break;
        case UD_Icdq:
            {
                /* no flags affected */

                OUTPUT_STRING("ashr edx, eax, 31\n");
            }
            break;
        case UD_Iclc:
            {
                /* clear carry flag */

                SR_disassemble_change_flags(cOutput, FL_CARRY, 0, 0);
            }
            break;
        case UD_Icld:
            {
                /* clear direction flag */

                OUTPUT_STRING("and eflags, eflags, ~DF\n");
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

                SR_disassemble_change_flags(cOutput, 0, 0, FL_CARRY);

            }
            break;
        case UD_Icmp:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (SR_llasm_helper_add_32(ud_obj.mnemonic, LR_TMP1, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            if (SR_llasm_helper_add_32(ud_obj.mnemonic, LR_TMP2, X86322LLREG(ud_obj.operand[0].base), LR_TMP1, 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                if (SR_llasm_helper_add_32(ud_obj.mnemonic, LR_TMP1, X86322LLREG(ud_obj.operand[0].base), LR_NONE, value))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                            else
                            {
                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("mov tmp1, %s\n", cAddress);

                                if (SR_llasm_helper_add_32(ud_obj.mnemonic, LR_TMP2, X862LLREG(ud_obj.operand[0].base), LR_TMP1, 0))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (SR_llasm_helper_add_16(ud_obj.mnemonic, LR_TMP1, X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            if (SR_llasm_helper_add_16(ud_obj.mnemonic, LR_TMP2, X86162LLREG(ud_obj.operand[0].base), LR_TMP1, 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = ((int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND)) << 16;

                            if (SR_llasm_helper_add_16(ud_obj.mnemonic, LR_TMP1, X86162LLREG(ud_obj.operand[0].base), LR_NONE, value))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                if (SR_llasm_helper_add_8l(ud_obj.mnemonic, LR_TMP1, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[1].base), 0))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                if (SR_llasm_helper_add_8l(ud_obj.mnemonic, LR_TMP1, X868L2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[1].base), 8))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            if (SR_llasm_helper_add_8l(ud_obj.mnemonic, LR_TMP2, X868L2LLREG(ud_obj.operand[0].base), LR_TMP1, 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            if (SR_llasm_helper_add_8l(ud_obj.mnemonic, LR_TMP1, X868L2LLREG(ud_obj.operand[0].base), LR_NONE, value))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                if (SR_llasm_helper_add_8h(ud_obj.mnemonic, LR_TMP1, X868H2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[1].base), 0))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                if (SR_llasm_helper_add_8h(ud_obj.mnemonic, LR_TMP1, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[1].base), 8))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            if (SR_llasm_helper_add_8h(ud_obj.mnemonic, LR_TMP2, X868H2LLREG(ud_obj.operand[0].base), LR_TMP1, 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            if (SR_llasm_helper_add_8h(ud_obj.mnemonic, LR_TMP1, X868H2LLREG(ud_obj.operand[0].base), LR_NONE, value))
                            {
                                flags_write = flags_to_write;
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
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            if (SR_llasm_helper_add_32(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X862LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                                if (SR_llasm_helper_add_32(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                            else
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("mov tmp2, %s\n", cAddress);

                                if (SR_llasm_helper_add_32(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_TMP2, 0))
                                {
                                    flags_write = flags_to_write;
                                }
                            }

                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            if (SR_llasm_helper_add_16(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X86162LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = ((int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND)) << 16;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            if (SR_llasm_helper_add_16(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                if (SR_llasm_helper_add_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X868L2LLREG(ud_obj.operand[1].base), 0))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            if (SR_llasm_helper_add_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                    }
                }

            }
            break;
        case UD_Icmpsb:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("ifnz ecx\n");

                    if (ud_obj.pfx_repne)
                    {
                        OUTPUT_STRING("REPNE_CMPSB\n");
                    }
                    else
                    {
                        OUTPUT_STRING("REPE_CMPSB\n");
                    }

                    if (flags_to_write)
                    {
                        SR_llasm_helper_add_8l(UD_Icmp, LR_TMP1, LR_TMP0, LR_TMP0, 8);
                    }

                    OUTPUT_STRING("endif\n");
                }
                else
                {
                    if (flags_to_write)
                    {
                        OUTPUT_STRING("load8z tmp1, esi, 1\n");
                        OUTPUT_STRING("load8z tmp2, edi, 1\n");
                        OUTPUT_STRING("and tmp3, eflags, DF\n");
                        OUTPUT_STRING("cmovz tmp3, tmp3, 1, -1\n");
                        OUTPUT_STRING("add esi, esi, tmp3\n");
                        OUTPUT_STRING("add edi, edi, tmp3\n");

                        SR_llasm_helper_add_8l(UD_Icmp, LR_TMP3, LR_TMP1, LR_TMP2, 0);
                    }
                    else
                    {
                        OUTPUT_STRING("and tmp3, eflags, DF\n");
                        OUTPUT_STRING("cmovz tmp3, tmp3, 1, -1\n");
                        OUTPUT_STRING("add esi, esi, tmp3\n");
                        OUTPUT_STRING("add edi, edi, tmp3\n");
                    }
                }

            }
            break;
        case UD_Icwd:
            {
                /* no flags affected */

                OUTPUT_STRING("ext16s tmp1, eax\n");
                OUTPUT_STRING("lshr tmp1, tmp1, 16\n");
                OUTPUT_STRING("and edx, edx, 0xffff0000\n");
                OUTPUT_STRING("or edx, edx, tmp1\n");
            }
            break;
        case UD_Icwde:
            {
                /* no flags affected */

                OUTPUT_STRING("ext16s eax, eax\n");
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
                        if (SR_llasm_helper_dec_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base)))
                        {
                            flags_write = flags_to_write;
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (SR_llasm_helper_dec_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base)))
                        {
                            flags_write = flags_to_write;
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (SR_llasm_helper_dec_8(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), 0))
                        {
                            flags_write = flags_to_write;
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (SR_llasm_helper_dec_8(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), 8))
                        {
                            flags_write = flags_to_write;
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                        if (SR_llasm_helper_dec_32(ud_obj.mnemonic, LR_TMP1))
                        {
                            flags_write = flags_to_write;
                        }

                        SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                        if (SR_llasm_helper_dec_16(ud_obj.mnemonic, LR_TMP1))
                        {
                            flags_write = flags_to_write;
                        }

                        SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                        if (SR_llasm_helper_dec_8(ud_obj.mnemonic, LR_TMP1, 0))
                        {
                            flags_write = flags_to_write;
                        }

                        SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
                    }
                }
            }
            break;
        case UD_Idiv:
            {
                /* CF,OF,SF,ZF,AF,PF undefined */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        OUTPUT_PARAMSTRING("DIV_64 %s\n", X86REGSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_PARAMSTRING("and tmp1, %s, 0xffff\n", X862LLSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("DIV_32 tmp1\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        OUTPUT_PARAMSTRING("and tmp1, %s, 0xff\n", X862LLSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("DIV_16 tmp1\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        OUTPUT_PARAMSTRING("lshr tmp1, %s, 8\n", X862LLSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("and tmp1, tmp1, 0xff\n");
                        OUTPUT_STRING("DIV_16 tmp1\n");
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP3);

                        OUTPUT_STRING("DIV_64 tmp3\n");
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP2, READ16TO32ZERO);

                        OUTPUT_STRING("DIV_32 tmp2\n");
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
                    OUTPUT_STRING("PUSH ebp\n");
                    OUTPUT_STRING("mov ebp, esp\n");
                    if (Size != 0)
                    {
                        OUTPUT_PARAMSTRING("sub esp, esp, 0x%x\n", Size);
                    }
                }
                else
                {
                    OUTPUT_STRING("PUSH ebp\n");
                    OUTPUT_STRING("mov tmp1, esp\n");

                    if (NestingLevel > 1)
                    {
                        for (Level = 1; Level < NestingLevel; Level++)
                        {
                            OUTPUT_STRING("sub ebp, ebp, 4\n");
                            OUTPUT_STRING("load tmp2, ebp, 4\n");
                            OUTPUT_STRING("PUSH tmp2\n");
                        }
                    }

                    OUTPUT_STRING("PUSH tmp1\n");
                    OUTPUT_STRING("mov ebp, tmp1\n");

                    if (Size != 0)
                    {
                        OUTPUT_PARAMSTRING("sub esp, esp, 0x%x\n", Size);
                    }
                }
            }
            break;
        case UD_Iidiv:
            {
                /* CF,OF,SF,ZF,AF,PF undefined */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        OUTPUT_PARAMSTRING("IDIV_64 %s\n", X86REGSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_PARAMSTRING("ext16s tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("IDIV_32 tmp1\n");
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP3);

                        OUTPUT_STRING("IDIV_64 tmp3\n");
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP2, READ16TO32SIGN);

                        OUTPUT_STRING("IDIV_32 tmp3\n");
                    }
                }
            }
            break;
        case UD_Iimul:
            {
                /* SF,ZF,AF,PF undefined, CF,OF - modified */

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    /* one-operand form */
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                        {
                            OUTPUT_PARAMSTRING("imul eax, edx, eax, %s\n", X86REGSTR(ud_obj.operand[0].base));

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ashr tmp2, eax, 31\n");
                                OUTPUT_STRING("cmoveq edx, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                        {
                            OUTPUT_STRING("ext16s tmp1, eax\n");
                            OUTPUT_PARAMSTRING("ext16s tmp2, %s\n", X862LLSTR(ud_obj.operand[0].base));
                            OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                            OUTPUT_STRING("ins16 eax, eax, tmp1\n");
                            OUTPUT_STRING("lshr tmp2, tmp1, 16\n");
                            OUTPUT_STRING("and edx, edx, 0xffff0000\n");
                            OUTPUT_STRING("or edx, edx, tmp2\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ext16s tmp2, tmp1\n");
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                        {
                            OUTPUT_STRING("ext8s tmp1, eax\n");
                            OUTPUT_PARAMSTRING("ext8s tmp2, %s\n", X862LLSTR(ud_obj.operand[0].base));
                            OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                            OUTPUT_STRING("ins16 eax, eax, tmp1\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ext8s tmp2, tmp1\n");
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                        {
                            OUTPUT_STRING("ext8s tmp1, eax\n");
                            OUTPUT_PARAMSTRING("lshr tmp2, %s, 8\n", X862LLSTR(ud_obj.operand[0].base));
                            OUTPUT_STRING("ext8s tmp2, tmp2\n");
                            OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                            OUTPUT_STRING("ins16 eax, eax, tmp1\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ext8s tmp2, tmp1\n");
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                        }
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            OUTPUT_STRING("imul eax, edx, eax, tmp1\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ashr tmp2, eax, 31\n");
                                OUTPUT_STRING("cmoveq edx, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                        }
                        else if (ud_obj.operand[0].size == 16)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP2, READ16TO32SIGN);

                            OUTPUT_STRING("ext16s tmp1, eax\n");
                            OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                            OUTPUT_STRING("ins16 eax, eax, tmp1\n");
                            OUTPUT_STRING("lshr tmp2, tmp1, 16\n");
                            OUTPUT_STRING("and edx, edx, 0xffff0000\n");
                            OUTPUT_STRING("or edx, edx, tmp2\n");

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ext16s tmp2, tmp1\n");
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
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

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("imul %s, tmp1, %s, %i\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base), value);

                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_PARAMSTRING("ashr tmp2, %s, 31\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mul %s, %s, %i\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base), value);
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[2]), SIGN_EXTEND);

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("imul %s, tmp1, tmp1, %i\n", X86REGSTR(ud_obj.operand[0].base), value);

                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_PARAMSTRING("ashr tmp2, %s, 31\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mul %s, tmp1, %i\n", X86REGSTR(ud_obj.operand[0].base), value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[2]), SIGN_EXTEND);


                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("ext16s tmp1, %s\n", X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("mul tmp1, tmp1, %i\n", value);
                                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));

                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ext16s tmp2, tmp1\n");
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mul tmp1, %s, %i\n", X862LLSTR(ud_obj.operand[1].base), value);
                                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[2]), SIGN_EXTEND);

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32SIGN);

                            OUTPUT_PARAMSTRING("mul tmp1, tmp1, %i\n", value);
                            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ext16s tmp2, tmp1\n");
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
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
                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("imul %s, tmp1, %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base));

                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_PARAMSTRING("ashr tmp2, %s, 31\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mul %s, %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("imul %s, tmp1, %s, tmp1\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));

                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_PARAMSTRING("ashr tmp2, %s, 31\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mul %s, %s, tmp1\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("ext16s tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("ext16s tmp2, %s\n", X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));

                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ext16s tmp2, tmp1\n");
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mul tmp1, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP2, READ16TO32SIGN);

                            if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                            {
                                OUTPUT_PARAMSTRING("ext16s tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));

                                SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                                OUTPUT_STRING("ext16s tmp2, tmp1\n");
                                OUTPUT_STRING("cmoveq tmp1, tmp2, tmp3, 0, CF | OF\n");
                                OUTPUT_STRING("or eflags, eflags, tmp3\n");
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mul tmp1, %s, tmp2\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                        }
                    }
                }
            }
            break;
        case UD_Iin:
            {
                /* no flags affected */

                if (ud_obj.operand[1].type == UD_OP_REG)
                {
                    OUTPUT_PARAMSTRING("IN_%s_DX\n", X86REGSTR(ud_obj.operand[0].base));
                }
                else
                {
                    OUTPUT_PARAMSTRING("IN_%s_IMM 0x%x\n", X86REGSTR(ud_obj.operand[0].base), ud_obj.operand[1].lval.ubyte);
                }
            }
            break;
        case UD_Iint:
        case UD_Iint1:
        case UD_Iint3:
            {
                /* all flags modified */

                unsigned int intno;

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

                OUTPUT_PARAMSTRING("INT %i\n", intno);
            }
            break;
        case UD_Ija:
        case UD_Ijae:
        case UD_Ijb:
        case UD_Ijbe:
        //case UD_Ijc:
        case UD_Ijg:
        case UD_Ijge:
        case UD_Ijl:
        case UD_Ijle:
        //case UD_Ijnb:
        //case UD_Ijnbe:
        //case UD_Ijnc:
        //case UD_Ijnl:
        //case UD_Ijnle:
        case UD_Ijp:
        case UD_Ijnp:
        case UD_Ijns:
        case UD_Ijnz:
        case UD_Ijs:
        case UD_Ijz:
            {
                /* no flags affected */

                int optimize_cond, jumpifzero, prev_inst_ok;
                uint_fast32_t opt_flags;

                last_instruction = 1;

                optimize_cond = 0;
                if ((flags_to_write == 0) && (output->has_label == 0))
                {
                    output_data *prev_output;

                    prev_output = section_output_list_FindEntryEqualOrLower(Entry, cur_ofs - 1);

                    jumpifzero = 0;
                    switch (ud_obj.mnemonic)
                    {
                        case UD_Ijz:
                            jumpifzero = 1;
                            // fallthrough
                        case UD_Ijnz:
                            opt_flags = FL_INST_Z;
                            break;

                        case UD_Ijs:
                            jumpifzero = 1;
                            // fallthrough
                        case UD_Ijns:
                            opt_flags = FL_INST_S;
                            break;

                        case UD_Ijb:
                        //case UD_Ijc:
                            jumpifzero = 1;
                            // fallthrough
                        case UD_Ijae:
                        //case UD_Ijnb:
                        //case UD_Ijnc:
                            opt_flags = FL_INST_B;
                            break;

                        case UD_Ija:
                        //case UD_Ijnbe:
                            jumpifzero = 1;
                            // fallthrough
                        case UD_Ijbe:
                            opt_flags = FL_INST_A;
                            break;

                        case UD_Ijl:
                            jumpifzero = 1;
                            // fallthrough
                        case UD_Ijge:
                        //case UD_Ijnl:
                            opt_flags = FL_INST_L;
                            break;

                        case UD_Ijg:
                        //case UD_Ijnle:
                            jumpifzero = 1;
                            // fallthrough
                        case UD_Ijle:
                            opt_flags = FL_INST_G;
                            break;

                        default:
                            opt_flags = 0;
                            break;
                    }

                    if ((opt_flags != 0) && (prev_output != NULL))
                    {
                        ud_t ud_prev;

                        ud_init(&ud_prev);
                        ud_set_mode(&ud_prev, 32);
                        ud_set_syntax(&ud_prev, UD_SYN_INTEL);

                        ud_set_input_buffer(&ud_prev, &(section[Entry].adr[prev_output->ofs]), prev_output->len + 1);
                        ud_set_pc(&ud_prev, section[Entry].start + prev_output->ofs);

                        prev_inst_ok = 0;
                        if ((prev_output->len + 1) == ud_disassemble(&ud_prev))
                        {
                            prev_inst_ok = 1;
                        }

                        while (prev_inst_ok)
                        {
                            if ((prev_output->has_label == 0) &&
                                ((ud_prev.mnemonic == UD_Ilea) ||
                                 (ud_prev.mnemonic == UD_Imovsx) ||
                                 (ud_prev.mnemonic == UD_Imovzx) ||
                                 ((ud_prev.mnemonic == UD_Imov) &&
                                  (ud_obj.pfx_seg == UD_R_CS ||
                                   ud_obj.pfx_seg == UD_R_DS ||
                                   ud_obj.pfx_seg == UD_R_ES ||
                                   ud_obj.pfx_seg == UD_R_SS ||
                                   ud_obj.pfx_seg == UD_NONE
                                  ) &&
                                  (((ud_prev.operand[0].type == UD_OP_REG) &&
                                    ((ud_prev.operand[0].base >= UD_R_EAX && ud_prev.operand[0].base <= UD_R_EDI) ||
                                     (ud_prev.operand[0].base >= UD_R_AX && ud_prev.operand[0].base <= UD_R_DI) ||
                                     (ud_prev.operand[0].base >= UD_R_AL && ud_prev.operand[0].base <= UD_R_BL) ||
                                     (ud_prev.operand[0].base >= UD_R_AH && ud_prev.operand[0].base <= UD_R_BH)
                                    )
                                   ) ||
                                   (ud_prev.operand[0].type == UD_OP_MEM)
                                  ) &&
                                  (((ud_prev.operand[1].type == UD_OP_REG) &&
                                    ((ud_prev.operand[1].base >= UD_R_EAX && ud_prev.operand[1].base <= UD_R_EDI) ||
                                     (ud_prev.operand[1].base >= UD_R_AX && ud_prev.operand[1].base <= UD_R_DI) ||
                                     (ud_prev.operand[1].base >= UD_R_AL && ud_prev.operand[1].base <= UD_R_BL) ||
                                     (ud_prev.operand[1].base >= UD_R_AH && ud_prev.operand[1].base <= UD_R_BH)
                                    )
                                   ) ||
                                   (ud_prev.operand[1].type == UD_OP_MEM) ||
                                   (ud_prev.operand[1].type == UD_OP_IMM)
                                  )
                                 ) ||
                                 (((ud_prev.mnemonic == UD_Ipush) ||
                                   (ud_prev.mnemonic == UD_Ipop)
                                  ) &&
                                  (ud_obj.pfx_seg == UD_R_CS ||
                                   ud_obj.pfx_seg == UD_R_DS ||
                                   ud_obj.pfx_seg == UD_R_ES ||
                                   ud_obj.pfx_seg == UD_R_SS ||
                                   ud_obj.pfx_seg == UD_NONE
                                  )
                                 )
                                )
                               )
                            {
                                prev_inst_ok = 0;

                                prev_output = section_output_list_FindEntryEqualOrLower(Entry, prev_output->ofs - 1);
                                if (prev_output != NULL)
                                {
                                    ud_set_input_buffer(&ud_prev, &(section[Entry].adr[prev_output->ofs]), prev_output->len + 1);
                                    ud_set_pc(&ud_prev, section[Entry].start + prev_output->ofs);

                                    if ((prev_output->len + 1) == ud_disassemble(&ud_prev))
                                    {
                                        prev_inst_ok = 1;
                                    }
                                }
                            }
                            else
                            {
                                break;
                            }
                        }

                        if (prev_inst_ok)
                        {
                            // optimize conditional jump without using eflags register

                            if (((ud_prev.mnemonic == UD_Icmp) ||
                                 (ud_prev.mnemonic == UD_Isub) ||
                                 (((opt_flags == FL_INST_Z) ||
                                   (opt_flags == FL_INST_S)
                                  ) &&
                                  ((ud_prev.mnemonic == UD_Iadd) ||
                                   (ud_prev.mnemonic == UD_Idec) ||
                                   (ud_prev.mnemonic == UD_Iinc)
                                  )
                                 ) ||
                                 (((opt_flags == FL_INST_L) ||
                                   (opt_flags == FL_INST_G)
                                  ) &&
                                  (ud_prev.mnemonic == UD_Idec)
                                 )
                                ) &&
                                (((ud_prev.operand[0].type == UD_OP_REG) &&
                                  (ud_prev.operand[0].base >= UD_R_EAX && ud_prev.operand[0].base <= UD_R_EDI)
                                 ) ||
                                 ((ud_prev.operand[0].type == UD_OP_MEM) &&
                                  (ud_prev.operand[0].size == 32)
                                 )
                                )
                               )
                            {
                                // 32-bit cmp, sub, add, dec, inc
                                optimize_cond = 1;
                            }
                            else
                            if (((ud_prev.mnemonic == UD_Itest) ||
                                 (ud_prev.mnemonic == UD_Iand) ||
                                 (ud_prev.mnemonic == UD_Ior) ||
                                 (ud_prev.mnemonic == UD_Ixor)
                                ) &&
                                (((ud_prev.operand[0].type == UD_OP_REG) &&
                                  (ud_prev.operand[0].base >= UD_R_EAX && ud_prev.operand[0].base <= UD_R_EDI)
                                 ) ||
                                 ((ud_prev.operand[0].type == UD_OP_MEM) &&
                                  (ud_prev.operand[0].size == 32)
                                 )
                                )
                               )
                            {
                                // 32-bit test, and, or, xor
                                if (opt_flags == FL_INST_A)
                                {
                                    jumpifzero = !jumpifzero;
                                }
                                optimize_cond = 1;
                            }
                            else
                            if (((ud_prev.mnemonic == UD_Icmp) ||
                                 (ud_prev.mnemonic == UD_Idec) ||
                                 (ud_prev.mnemonic == UD_Iinc)
                                ) &&
                                (opt_flags == FL_INST_Z)
                               )
                            {
                                // 8/16-bit cmp, dec, inc
                                optimize_cond = 1;
                            }
                            else
                            if ((ud_prev.mnemonic == UD_Itest) &&
                                ((opt_flags == FL_INST_Z) ||
                                 (opt_flags == FL_INST_A)
                                )
                               )
                            {
                                // 8/16-bit test
                                if (opt_flags == FL_INST_A)
                                {
                                    jumpifzero = !jumpifzero;
                                }
                                optimize_cond = 1;
                            }
                        }

                        if (!optimize_cond)
                        {
                            //printf("loc_%x: %s ; %s\n", (uint32_t)(section[Entry].start + prev_output->ofs), prev_output->str, output->str);
                        }
                    }
                }

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

                    if (optimize_cond)
                    {
                        flags_read = opt_flags;

                        if (backward)
                        {
                            //OUTPUT_STRING("ACTION_OPTIMIZED_CONDITIONAL_JUMP_BACKWARD\n");
                        }
                        else
                        {
                            //OUTPUT_STRING("ACTION_OPTIMIZED_CONDITIONAL_JUMP_FORWARD\n");
                        }

                        if (jumpifzero)
                        {
                            OUTPUT_PARAMSTRING("ctcallz tmpcnd, %s\n", cAddress);
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("ctcallnz tmpcnd, %s\n", cAddress);
                        }
                    }
                    else
                    {
                        if (backward)
                        {
                            //OUTPUT_STRING("ACTION_CONDITIONAL_JUMP_BACKWARD\n");
                        }
                        else
                        {
                            //OUTPUT_STRING("ACTION_CONDITIONAL_JUMP_FORWARD\n");
                        }


                        if ((ud_obj.mnemonic == UD_Ijz) || (ud_obj.mnemonic == UD_Ijnz))
                        {
                            OUTPUT_STRING("and tmp1, eflags, ZF\n");
                            jumpifzero = (ud_obj.mnemonic == UD_Ijz)?0:1;
                        }
                        else if ((ud_obj.mnemonic == UD_Ijs) || (ud_obj.mnemonic == UD_Ijns))
                        {
                            OUTPUT_STRING("and tmp1, eflags, SF\n");
                            jumpifzero = (ud_obj.mnemonic == UD_Ijs)?0:1;
                        }
                        else if ((ud_obj.mnemonic == UD_Ijp) || (ud_obj.mnemonic == UD_Ijnp))
                        {
                            OUTPUT_STRING("and tmp1, eflags, PF\n");
                            jumpifzero = (ud_obj.mnemonic == UD_Ijp)?0:1;
                        }
                        else if ((ud_obj.mnemonic == UD_Ijb) || //(ud_obj.mnemonic == UD_Ijc) ||
                                 (ud_obj.mnemonic == UD_Ijae) //|| (ud_obj.mnemonic == UD_Ijnb) || (ud_obj.mnemonic == UD_Ijnc)
                                )
                        {
                            OUTPUT_STRING("and tmp1, eflags, CF\n");
                            jumpifzero = ((ud_obj.mnemonic == UD_Ijb) //|| (ud_obj.mnemonic == UD_Ijc)
                                         )?0:1;
                        }
                        else if ((ud_obj.mnemonic == UD_Ijbe) || (ud_obj.mnemonic == UD_Ija) //|| (ud_obj.mnemonic == UD_Ijnbe)
                                )
                        {
                            OUTPUT_STRING("and tmp1, eflags, ZF | CF\n");
                            jumpifzero = (ud_obj.mnemonic == UD_Ijbe)?0:1;
                        }
                        else if ((ud_obj.mnemonic == UD_Ijl) || (ud_obj.mnemonic == UD_Ijge) //|| (ud_obj.mnemonic == UD_Ijnl)
                                 )
                        {
                            OUTPUT_STRING("lshr tmp1, eflags, OF_SHIFT - SF_SHIFT\n");
                            OUTPUT_STRING("xor tmp1, tmp1, eflags\n");
                            OUTPUT_STRING("and tmp1, tmp1, SF\n");
                            jumpifzero = (ud_obj.mnemonic == UD_Ijl)?0:1;
                        }
                        else //if ((ud_obj.mnemonic == UD_Ijle) || (ud_obj.mnemonic == UD_Ijg) || (ud_obj.mnemonic == UD_Ijnle))
                        {
                            OUTPUT_STRING("lshr tmp1, eflags, SF_SHIFT - ZF_SHIFT\n");
                            OUTPUT_STRING("lshr tmp2, eflags, OF_SHIFT - ZF_SHIFT\n");
                            OUTPUT_STRING("xor tmp1, tmp1, tmp2\n");
                            OUTPUT_STRING("or tmp1, tmp1, eflags\n");
                            OUTPUT_STRING("and tmp1, tmp1, ZF\n");
                            jumpifzero = (ud_obj.mnemonic == UD_Ijle)?0:1;
                        }


                        if (jumpifzero)
                        {
                            OUTPUT_PARAMSTRING("ctcallz tmp1, %s\n", cAddress);
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("ctcallnz tmp1, %s\n", cAddress);
                        }
                    }


                    SR_add_label(Entry, cur_ofs + decoded_length);
                }
            }
            break;
        case UD_Ijmp:
            {
                /* no flags affected */

                last_instruction = -1;

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
                            //OUTPUT_STRING("ACTION_UNCONDITIONAL_SHORT_JUMP_BACKWARD\n");
                        }
                        else
                        {
                            //OUTPUT_STRING("ACTION_UNCONDITIONAL_SHORT_JUMP_FORWARD\n");
                        }
                    }
                    else
                    {
                        //OUTPUT_STRING("ACTION_UNCONDITIONAL_JUMP\n");
                    }

                    OUTPUT_PARAMSTRING("tcall %s\n", cAddress);
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM &&
                         ud_obj.operand[0].size == 32)
                {
                    if (extrn[0] != NULL && extrn[0]->is_import)
                    {
                        if (ud_obj.operand[0].base == UD_NONE &&
                            ud_obj.operand[0].index == UD_NONE
                           )
                        {
                            //OUTPUT_STRING("ACTION_UNCONDITIONAL_JUMP\n");
                            OUTPUT_PARAMSTRING("tcall %s\n", extrn[0]->proc);
                        }
                    }
                    else
                    {
                        //OUTPUT_STRING("ACTION_UNCONDITIONAL_JUMP\n");

                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                        OUTPUT_STRING("tcall tmp1\n");
                    }
                }
                else if (fixup[0] == NULL &&
                         ud_obj.operand[0].type == UD_OP_REG &&
                         ud_obj.operand[0].size == 32)
                {
                    //OUTPUT_STRING("ACTION_UNCONDITIONAL_JUMP\n");

                    OUTPUT_PARAMSTRING("tcall %s\n", X86REGSTR(ud_obj.operand[0].base));
                }
            }
            break;
        case UD_Ilahf:
            {
                /* no flags affected */
                OUTPUT_STRING("ins8hl eax, eax, eflags\n");
            }
            break;
        case UD_Ilea:
            {
                /* no flags affected */

                if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                {
                    SR_disassemble_get_memory_address(cOutput, X862LLREG(ud_obj.operand[0].base), &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_REG, &memadr);
                }
            }
            break;
        case UD_Ileave:
            {
                /* no flags affected */

                OUTPUT_STRING("mov esp, ebp\n");
                OUTPUT_STRING("POP ebp\n");
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
                    OUTPUT_STRING("load8z tmp1, esi, 1\n");
                    OUTPUT_STRING("and eax, eax, 0xffffff00\n");
                    OUTPUT_STRING("or eax, eax, tmp1\n");
                    OUTPUT_STRING("and tmp2, eflags, DF\n");
                    OUTPUT_STRING("cmovz tmp2, tmp2, 1, -1\n");
                    OUTPUT_STRING("add esi, esi, tmp2\n");
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
                    OUTPUT_STRING("load eax, esi, 1\n");
                    OUTPUT_STRING("and tmp2, eflags, DF\n");
                    OUTPUT_STRING("cmovz tmp2, tmp2, 4, -4\n");
                    OUTPUT_STRING("add esi, esi, tmp2\n");
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
                    OUTPUT_STRING("load16z tmp1, esi, 1\n");
                    OUTPUT_STRING("and eax, eax, 0xffff0000\n");
                    OUTPUT_STRING("or eax, eax, tmp1\n");
                    OUTPUT_STRING("and tmp2, eflags, DF\n");
                    OUTPUT_STRING("cmovz tmp2, tmp2, 2, -2\n");
                    OUTPUT_STRING("add esi, esi, tmp2\n");
                }
            }
            break;
        case UD_Iloop:
        case UD_Iloope:
        //case UD_Iloopn:
        case UD_Iloopne:
        //case UD_Iloopnz:
        //case UD_Iloopz:
            {
                /* no flags affected */

                uint32_t address;
                extrn_data *extrnadr;
                int backward;

                last_instruction = 1;

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
                    //OUTPUT_STRING("ACTION_LOOP_BACKWARD\n");
                }
                else
                {
                    //OUTPUT_STRING("ACTION_LOOP_FORWARD\n");
                }

                OUTPUT_STRING("sub ecx, ecx, 1\n");

                if ((ud_obj.mnemonic == UD_Iloope) //|| (ud_obj.mnemonic == UD_Iloopz)
                   )
                {
                    OUTPUT_STRING("and tmp1, eflags, ZF\n");
                    OUTPUT_STRING("cmovz tmp1, tmp1, 0, ecx\n");
                    OUTPUT_PARAMSTRING("ctcallnz tmp1, %s\n", cAddress);
                }
                else if ((ud_obj.mnemonic == UD_Iloopne) //|| (ud_obj.mnemonic == UD_Iloopnz) || (ud_obj.mnemonic == UD_Iloopn)
                        )
                {
                    OUTPUT_STRING("and tmp1, eflags, ZF\n");
                    OUTPUT_STRING("cmovz tmp1, tmp1, ecx, 0\n");
                    OUTPUT_PARAMSTRING("ctcallnz tmp1, %s\n", cAddress);
                }
                else
                {
                    OUTPUT_PARAMSTRING("ctcallnz ecx, %s\n", cAddress);
                }

                SR_add_label(Entry, cur_ofs + decoded_length);
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

                        OUTPUT_PARAMSTRING("call x86_mov_reg_mem_%i 0x%x\n", ud_obj.operand[0].size, ud_obj.operand[1].lval.udword);

                        if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                        {
                            OUTPUT_PARAMSTRING("mov %s, tmp0\n", X86REGSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                        {
                            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp0\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp0\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("ins8hl %s, %s, tmp0\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.mnemonic == UD_Imov &&
                        ud_obj.operand[1].type == UD_OP_MEM &&
                        ud_obj.operand[1].base == UD_NONE &&
                        ud_obj.operand[1].index == UD_NONE &&
                        ud_obj.operand[1].size == 32 &&
                        fixup[1] == NULL &&
                        (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI) &&
                        ud_obj.pfx_seg == UD_R_FS
                       )
                    {
                        // mov reg, fs:[const]

                        OUTPUT_PARAMSTRING("call x86_read_fs_dword 0x%x\n", ud_obj.operand[1].lval.udword);
                        OUTPUT_PARAMSTRING("mov %s, tmp0\n", X86REGSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.mnemonic == UD_Imov &&
                        ud_obj.operand[1].type == UD_OP_MEM &&
                        (ud_obj.operand[1].base >= UD_R_EAX && ud_obj.operand[1].base <= UD_R_EDI) &&
                        ud_obj.operand[1].index == UD_NONE &&
                        ud_obj.operand[1].lval.udword == 0 &&
                        fixup[1] == NULL &&
                        (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI) &&
                        ud_obj.pfx_seg == UD_R_FS
                       )
                    {
                        // mov reg, fs:[reg]

                        OUTPUT_PARAMSTRING("call x86_read_fs_dword %s\n", X86REGSTR(ud_obj.operand[1].base));
                        OUTPUT_PARAMSTRING("mov %s, tmp0\n", X86REGSTR(ud_obj.operand[0].base));
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
                                OUTPUT_PARAMSTRING("mov %s, 0\n", X86REGSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            if (extrn[1] != NULL && extrn[1]->is_import)
                            {
                                if (ud_obj.operand[1].base == UD_NONE &&
                                    ud_obj.operand[1].index == UD_NONE
                                   )
                                {
                                    OUTPUT_PARAMSTRING("mov %s, %s\n", X86REGSTR(ud_obj.operand[0].base), extrn[1]->proc);
                                }
                            }
                            else
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] != NULL)
                            {
                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("mov %s, %s\n", X86REGSTR(ud_obj.operand[0].base), cAddress);
                            }
                            else
                            {
                                uint32_t value;

                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                                OUTPUT_PARAMSTRING("mov %s, 0x%x\n", X86REGSTR(ud_obj.operand[0].base), value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_ES && ud_obj.operand[1].base <= UD_R_GS)
                            {
                                OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AX && ud_obj.operand[1].base <= UD_R_DI)
                            {
                                OUTPUT_PARAMSTRING("ins16 %s, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base), READ16TO16);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                            OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            if (value != 0)
                            {
                                OUTPUT_PARAMSTRING("or %s, %s, 0x%x\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), value);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                OUTPUT_PARAMSTRING("ins8ll %s, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                OUTPUT_PARAMSTRING("ins8lh %s, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base), READ8TO8LOW);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            OUTPUT_PARAMSTRING("and %s, %s, 0xffffff00\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            if (ud_obj.operand[1].lval.ubyte != 0)
                            {
                                OUTPUT_PARAMSTRING("or %s, %s, 0x%x\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), ud_obj.operand[1].lval.ubyte);
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                OUTPUT_PARAMSTRING("ins8hl %s, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                OUTPUT_PARAMSTRING("ins8hh %s, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base), READ8TO8HIGH);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            OUTPUT_PARAMSTRING("and %s, %s, 0xffff00ff\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            if (ud_obj.operand[1].lval.ubyte != 0)
                            {
                                OUTPUT_PARAMSTRING("or %s, %s, 0x%x << 8\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), ud_obj.operand[1].lval.ubyte);
                            }
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

                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                OUTPUT_PARAMSTRING("lshl tmp1, %s, 8\n", X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("call x86_mov_mem_reg_%i 0x%x, tmp1\n", ud_obj.operand[1].size, ud_obj.operand[0].lval.udword);
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("call x86_mov_mem_reg_%i 0x%x, %s\n", ud_obj.operand[1].size, ud_obj.operand[0].lval.udword, X862LLSTR(ud_obj.operand[1].base));
                            }
                        }
                        else
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                            OUTPUT_PARAMSTRING("call x86_mov_mem_reg_%i 0x%x, 0x%x\n", ud_obj.operand[1].size, ud_obj.operand[0].lval.udword, value);
                        }
                    }
                    else if (ud_obj.mnemonic == UD_Imov &&
                        ud_obj.operand[0].base == UD_NONE &&
                        ud_obj.operand[0].index == UD_NONE &&
                        ud_obj.operand[0].size == 32 &&
                        fixup[0] == NULL &&
                        fixup[1] == NULL &&
                        ud_obj.operand[1].type == UD_OP_REG &&
                        ud_obj.operand[1].base >= UD_R_EAX && ud_obj.operand[1].base <= UD_R_EDI &&
                        ud_obj.pfx_seg == UD_R_FS
                       )
                    {
                        // mov fs:[const], reg

                        OUTPUT_PARAMSTRING("call x86_write_fs_dword 0x%x, %s\n", ud_obj.operand[0].lval.udword, X86REGSTR(ud_obj.operand[1].base));
                    }
                    else if (ud_obj.mnemonic == UD_Imov &&
                        ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI &&
                        ud_obj.operand[0].index == UD_NONE &&
                        ud_obj.operand[0].lval.udword == 0 &&
                        fixup[0] == NULL &&
                        fixup[1] == NULL &&
                        ud_obj.operand[1].type == UD_OP_REG &&
                        ud_obj.operand[1].base >= UD_R_EAX && ud_obj.operand[1].base <= UD_R_EDI &&
                        ud_obj.pfx_seg == UD_R_FS
                       )
                    {
                        // mov fs:[reg], reg

                        OUTPUT_PARAMSTRING("call x86_write_fs_dword %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[1].base));
                    }
                    else if (ud_obj.operand[0].size == 32)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_ES && ud_obj.operand[1].base <= UD_R_GS)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                                SR_disassemble_write_mem_word(cOutput, &memadr, LR_ZERO);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_EAX && ud_obj.operand[1].base <= UD_R_EDI)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], ud_obj.operand[1].base, MADR_WRITE, ZERO_EXTEND, &memadr);

                                SR_disassemble_write_mem_word(cOutput, &memadr, X862LLREG(ud_obj.operand[1].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                uint32_t value;

                                value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                                if (value == 0)
                                {
                                    SR_disassemble_write_mem_word(cOutput, &memadr, LR_ZERO);
                                }
                                else
                                {
                                    OUTPUT_PARAMSTRING("mov tmp1, 0x%x\n", value);

                                    SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                                }
                            }
                            else
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                                SR_get_fixup_label(cAddress, fixup[1], extrn[1]);

                                OUTPUT_PARAMSTRING("mov tmp1, %s\n", cAddress);

                                SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
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

                                SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_ZERO);
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AX && ud_obj.operand[1].base <= UD_R_DI)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], ud_obj.operand[1].base, MADR_WRITE, ZERO_EXTEND, &memadr);

                                SR_disassemble_write_mem_halfword(cOutput, &memadr, X862LLREG(ud_obj.operand[1].base));
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            if (value == 0)
                            {
                                SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_ZERO);
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, 0x%x\n", value);

                                SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
                            }
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

                            SR_disassemble_write_mem_byte(cOutput, &memadr, X862LLREG(ud_obj.operand[1].base), highlow);
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            if (value == 0)
                            {
                                SR_disassemble_write_mem_byte(cOutput, &memadr, LR_ZERO, WRITE8LOW);
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, 0x%x\n", value);

                                SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
                            }
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
                    OUTPUT_STRING("REP_MOVSB\n");
                }
                else
                {
                    OUTPUT_STRING("load8z tmp1, esi, 1\n");
                    OUTPUT_STRING("store8 tmp1, edi, 1\n");
                    OUTPUT_STRING("and tmp2, eflags, DF\n");
                    OUTPUT_STRING("cmovz tmp2, tmp2, 1, -1\n");
                    OUTPUT_STRING("add esi, esi, tmp2\n");
                    OUTPUT_STRING("add edi, edi, tmp2\n");
                }

            }
            break;
        case UD_Imovsd:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("REP_MOVSD\n");
                }
                else
                {
                    OUTPUT_STRING("load tmp1, esi, 1\n");
                    OUTPUT_STRING("store tmp1, edi, 1\n");
                    OUTPUT_STRING("and tmp2, eflags, DF\n");
                    OUTPUT_STRING("cmovz tmp2, tmp2, 4, -4\n");
                    OUTPUT_STRING("add esi, esi, tmp2\n");
                    OUTPUT_STRING("add edi, edi, tmp2\n");
                }

            }
            break;
        case UD_Imovsw:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("REP_MOVSW\n");
                }
                else
                {
                    OUTPUT_STRING("load16z tmp1, esi, 1\n");
                    OUTPUT_STRING("store16 tmp1, edi, 1\n");
                    OUTPUT_STRING("and tmp2, eflags, DF\n");
                    OUTPUT_STRING("cmovz tmp2, tmp2, 2, -2\n");
                    OUTPUT_STRING("add esi, esi, tmp2\n");
                    OUTPUT_STRING("add edi, edi, tmp2\n");
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
                            OUTPUT_PARAMSTRING("ext16s %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("ext8s %s, %s\n", X86REGSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                        {
                            OUTPUT_PARAMSTRING("lshr tmp1, %s\n", X862LLSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("ext8s %s, tmp1\n", X86REGSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[1].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[1].size == 16)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base), READ16TO32SIGN);
                        }
                        else if (ud_obj.operand[1].size == 8)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base), READ8TO32SIGN);
                        }
                    }
                }
                else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                {
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("ext8s tmp1, %s\n", X862LLSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                        {
                            OUTPUT_PARAMSTRING("lshr tmp1, %s\n", X862LLSTR(ud_obj.operand[1].base));
                            OUTPUT_STRING("ext8s tmp1, tmp1\n");
                            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[1].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[1].size == 8)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base), READ8TO16SIGN);
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
                            OUTPUT_PARAMSTRING("and %s, %s, 0xffff\n", X86REGSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("and %s, %s, 0xff\n", X86REGSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                        {
                            OUTPUT_PARAMSTRING("lshr %s, %s, 8\n", X86REGSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("and %s, %s, 0xff\n", X86REGSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[1].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[1].size == 16)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base), READ16TO32ZERO);
                        }
                        else if (ud_obj.operand[1].size == 8)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base), READ8TO32ZERO);
                        }
                    }
                }
                else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                {
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                        {
                            if (X862LLREG(ud_obj.operand[0].base) == X862LLREG(ud_obj.operand[1].base))
                            {
                                OUTPUT_PARAMSTRING("and %s, %s, 0xffff00ff\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("and tmp1, %s, 0xff\n", X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                        }
                        else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                        {
                            OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("lshr tmp1, %s, 8\n", X862LLSTR(ud_obj.operand[1].base));
                            OUTPUT_STRING("and tmp1, tmp1, 0xff\n");
                            OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[1].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[1].size == 8)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[1]), fixup[1], extrn[1], ud_obj.operand[0].base, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, X862LLREG(ud_obj.operand[0].base), READ8TO16ZERO);
                        }
                    }
                }

            }
            break;
        case UD_Imul:
            {
                /* SF,ZF,AF,PF undefined, CF,OF - modified */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        OUTPUT_PARAMSTRING("umul eax, edx, eax, %s\n", X86REGSTR(ud_obj.operand[0].base));

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                            OUTPUT_STRING("cmovz edx, tmp3, 0, CF | OF\n");
                            OUTPUT_STRING("or eflags, eflags, tmp3\n");
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_STRING("and tmp1, eax, 0xffff\n");
                        OUTPUT_PARAMSTRING("and tmp2, %s, 0xffff\n", X862LLSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                        OUTPUT_STRING("ins16 eax, eax, tmp1\n");
                        OUTPUT_STRING("lshr tmp2, tmp1, 16\n");
                        OUTPUT_STRING("and edx, edx, 0xffff0000\n");
                        OUTPUT_STRING("or edx, edx, tmp2\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                            OUTPUT_STRING("cmovz tmp2, tmp3, 0, CF | OF\n");
                            OUTPUT_STRING("or eflags, eflags, tmp3\n");
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        OUTPUT_STRING("and tmp1, eax, 0xff\n");
                        OUTPUT_PARAMSTRING("lshr tmp2, %s, 8\n", X862LLSTR(ud_obj.operand[0].base));
                        OUTPUT_STRING("and tmp2, tmp2, 0xff\n");
                        OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                        OUTPUT_STRING("and eax, eax, 0xffff0000\n");
                        OUTPUT_STRING("or eax, eax, tmp1\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                            OUTPUT_STRING("and tmp2, tmp1, 0xff00\n");
                            OUTPUT_STRING("cmovz tmp2, tmp3, 0, CF | OF\n");
                            OUTPUT_STRING("or eflags, eflags, tmp3\n");
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                        OUTPUT_STRING("umul eax, edx, eax, tmp1\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                            OUTPUT_STRING("cmovz edx, tmp3, 0, CF | OF\n");
                            OUTPUT_STRING("or eflags, eflags, tmp3\n");
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP2, READ16TO32ZERO);

                        OUTPUT_STRING("and tmp1, eax, 0xffff\n");
                        OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                        OUTPUT_STRING("ins16 eax, eax, tmp1\n");
                        OUTPUT_STRING("lshr tmp2, tmp1, 16\n");
                        OUTPUT_STRING("and edx, edx, 0xffff0000\n");
                        OUTPUT_STRING("or edx, edx, tmp2\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                            OUTPUT_STRING("cmovz tmp2, tmp3, 0, CF | OF\n");
                            OUTPUT_STRING("or eflags, eflags, tmp3\n");
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP2, READ8TO32ZERO);

                        OUTPUT_STRING("and tmp1, eax, 0xff\n");
                        OUTPUT_STRING("mul tmp1, tmp1, tmp2\n");
                        OUTPUT_STRING("and eax, eax, 0xffff0000\n");
                        OUTPUT_STRING("or eax, eax, tmp1\n");

                        if (flags_to_write & (FL_CARRY | FL_OVERFLOW))
                        {
                            SR_disassemble_change_flags(pOutput, Tflags_to_write, 0, 0);

                            OUTPUT_STRING("and tmp2, tmp1, 0xff00\n");
                            OUTPUT_STRING("cmovz tmp2, tmp3, 0, CF | OF\n");
                            OUTPUT_STRING("or eflags, eflags, tmp3\n");
                        }
                    }
                }
            }
            break;
        case UD_Ineg:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        SR_llasm_helper_neg_32(X86322LLREG(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        SR_llasm_helper_neg_16(X86162LLREG(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        SR_llasm_helper_neg_8(X868L2LLREG(ud_obj.operand[0].base), 0);
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        SR_llasm_helper_neg_8(X868H2LLREG(ud_obj.operand[0].base), 8);
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                        SR_llasm_helper_neg_32(LR_TMP1);

                        SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                        SR_llasm_helper_neg_16(LR_TMP1);

                        SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                        SR_llasm_helper_neg_8(LR_TMP1, 0);

                        SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
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
                        OUTPUT_PARAMSTRING("xor %s, %s, 0xffffffff\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_PARAMSTRING("xor %s, %s, 0xffff\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        OUTPUT_PARAMSTRING("xor %s, %s, 0xff\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
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

                        SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                        OUTPUT_STRING("xor tmp1, tmp1, 0xffffffff\n");

                        SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                        OUTPUT_STRING("xor tmp1, tmp1, 0xffffffff\n");

                        SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                        OUTPUT_STRING("xor tmp1, tmp1, 0xffffffff\n");

                        SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
                    }
                }
            }
            break;
        case UD_Iout:
            {
                /* no flags affected */
                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    OUTPUT_PARAMSTRING("OUT_DX_%s\n", X86REGSTR(ud_obj.operand[1].base));
                }
                else
                {
                    OUTPUT_PARAMSTRING("OUT_IMM_%s 0x%x\n", X86REGSTR(ud_obj.operand[1].base), ud_obj.operand[0].lval.ubyte);
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
                        if (ud_obj.operand[0].base == UD_R_ESP)
                        {
                            OUTPUT_STRING("POP tmp1\n");
                            OUTPUT_STRING("mov esp, tmp1\n");
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("POP %s\n", X86REGSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_ES && ud_obj.operand[0].base <= UD_R_GS)
                    {
                        OUTPUT_STRING("add esp, esp, 4\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_STRING("POP tmp1\n");
                        OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].base == UD_NONE &&
                        ud_obj.operand[0].index == UD_NONE &&
                        ud_obj.operand[0].size == 32 &&
                        fixup[0] == NULL &&
                        ud_obj.pfx_seg == UD_R_FS
                       )
                    {
                        // pop dword fs:[const]

                        OUTPUT_STRING("POP tmp1\n");
                        OUTPUT_PARAMSTRING("call x86_write_fs_dword 0x%x, tmp1\n", ud_obj.operand[0].lval.udword);
                    }
                    else if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                        OUTPUT_STRING("POP tmp1\n");

                        SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                        OUTPUT_STRING("POP tmp1\n");

                        SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
                    }
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
            }
            break;
        case UD_Ipopfd:
            {
                /* all flags modified */

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
                        if (ud_obj.operand[0].base == UD_R_ESP)
                        {
                            OUTPUT_STRING("mov tmp1, esp\n");
                            OUTPUT_STRING("PUSH tmp1\n");
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("PUSH %s\n", X86REGSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_ES && ud_obj.operand[0].base <= UD_R_GS)
                    {
                        OUTPUT_STRING("PUSH 0\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[0].base == UD_R_SP)
                        {
                            OUTPUT_STRING("mov tmp1, esp\n");
                            OUTPUT_STRING("PUSH tmp1\n");
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("PUSH %s\n", X862LLSTR(ud_obj.operand[0].base));
                        }
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].base == UD_NONE &&
                        ud_obj.operand[0].index == UD_NONE &&
                        ud_obj.operand[0].size == 32 &&
                        fixup[0] == NULL &&
                        ud_obj.pfx_seg == UD_R_FS
                       )
                    {
                        // push dword fs:[const]

                        OUTPUT_PARAMSTRING("call x86_read_fs_dword 0x%x\n", ud_obj.operand[0].lval.udword);
                        OUTPUT_STRING("PUSH tmp0\n");
                    }
                    else if (ud_obj.operand[0].size == 32)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                        OUTPUT_STRING("PUSH tmp1\n");
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                        OUTPUT_STRING("PUSH tmp1\n");
                    }
                }
                else if (fixup[0] == NULL &&
                         ud_obj.operand[0].type == UD_OP_IMM)
                {
                    int32_t value;

                    value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[0]), SIGN_EXTEND);

                    OUTPUT_PARAMSTRING("PUSH 0x%x\n", value);
                }
                else if (ud_obj.operand[0].type == UD_OP_IMM)
                {
                    SR_get_fixup_label(cAddress, fixup[0], extrn[0]);

                    OUTPUT_PARAMSTRING("PUSH %s\n", cAddress);
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
                                    OUTPUT_STRING("and tmp3, eflags, CF\n");

                                    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
                                    }

                                    if (Tflags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_PARAMSTRING("lshr tmp1, %s, 31 - CF_SHIFT\n", X86REGSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING(";and tmp1, tmp1, CF\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    if (Tflags_to_write & FL_OVERFLOW)
                                    {
                                        OUTPUT_PARAMSTRING("shl tmp1, %s, 1\n", X86REGSTR(ud_obj.operand[0].base));
                                        OUTPUT_PARAMSTRING("xor tmp1, tmp1, %s\n", X86REGSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmp1, tmp1, 0x80000000\n");
                                        OUTPUT_STRING("lshr tmp1, tmp1, 31 - OF_SHIFT\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_PARAMSTRING("shl %s, %s, 1\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("or %s, %s, tmp3\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
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
                                    OUTPUT_STRING("and tmp3, eflags, CF\n");

                                    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
                                    }

                                    if (Tflags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_PARAMSTRING("lshr tmp1, %s, 15 - CF_SHIFT\n", X862LLSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmp1, tmp1, CF\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    if (Tflags_to_write & FL_OVERFLOW)
                                    {
                                        OUTPUT_PARAMSTRING("shl tmp1, %s, 1\n", X862LLSTR(ud_obj.operand[0].base));
                                        OUTPUT_PARAMSTRING("xor tmp1, tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmp1, tmp1, 0x8000\n");
                                        OUTPUT_STRING("lshr tmp1, tmp1, 15 - OF_SHIFT\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_PARAMSTRING("shl tmp1, %s, 1\n", X862LLSTR(ud_obj.operand[0].base));
                                    OUTPUT_STRING("or tmp1, tmp1, tmp3\n");
                                    OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
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
                                    OUTPUT_STRING("and tmp3, eflags, CF\n");

                                    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
                                    }

                                    if (Tflags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_PARAMSTRING("and tmp1, %s, CF\n", X86REGSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_STRING("shl tmp3, tmp3, 31\n");

                                    if (Tflags_to_write & FL_OVERFLOW)
                                    {
                                        /* OF = MSB(reg) XOR CF */
                                        OUTPUT_PARAMSTRING("xor tmp1, %s, tmp3\n", X86REGSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmp1, tmp1, 0x80000000\n");
                                        OUTPUT_STRING("lshr tmp1, tmp1, 31 - OF_SHIFT\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_PARAMSTRING("lshr %s, %s, 1\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("or %s, %s, tmp3\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
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
                                    OUTPUT_STRING("and tmp3, eflags, CF\n");

                                    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
                                    }

                                    if (Tflags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_PARAMSTRING("and tmp1, %s, CF\n", X862LLSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_STRING("shl tmp3, tmp3, 15\n");

                                    if (Tflags_to_write & FL_OVERFLOW)
                                    {
                                        /* OF = MSB(reg) XOR CF */
                                        OUTPUT_PARAMSTRING("xor tmp1, %s, tmp3\n", X862LLSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmp1, tmp1, 0x8000\n");
                                        OUTPUT_STRING("lshr tmp1, tmp1, 15 - OF_SHIFT\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_PARAMSTRING("and tmp1, %s, 0xffff\n", X862LLSTR(ud_obj.operand[0].base));
                                    OUTPUT_STRING("lshr tmp1, tmp1, 1\n");
                                    OUTPUT_STRING("or tmp1, tmp1, tmp3\n");
                                    OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
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
                                    OUTPUT_STRING("and tmp3, eflags, CF\n");

                                    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
                                    }

                                    if (Tflags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_PARAMSTRING("and tmp1, %s, CF\n", X862LLSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_STRING("shl tmp3, tmp3, 7\n");

                                    if (Tflags_to_write & FL_OVERFLOW)
                                    {
                                        /* OF = MSB(reg) XOR CF */
                                        OUTPUT_PARAMSTRING("xor tmp1, %s, tmp3\n", X862LLSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmp1, tmp1, 0x80\n");
                                        OUTPUT_STRING("shl tmp1, tmp1, OF_SHIFT - 7\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_PARAMSTRING("and tmp1, %s, 0xff\n", X862LLSTR(ud_obj.operand[0].base));
                                    OUTPUT_STRING("lshr tmp1, tmp1, 1\n");
                                    OUTPUT_STRING("or tmp1, tmp1, tmp3\n");
                                    OUTPUT_PARAMSTRING("and %s, %s, 0xffffff00\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
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
                                    OUTPUT_STRING("and tmp3, eflags, CF\n");

                                    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
                                    }

                                    if (Tflags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_PARAMSTRING("lshr tmp1, %s, 8\n", X862LLSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmp1, tmp1, CF\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_STRING("shl tmp3, tmp3, 15\n");

                                    if (Tflags_to_write & FL_OVERFLOW)
                                    {
                                        /* OF = MSB(reg) XOR CF */
                                        OUTPUT_PARAMSTRING("xor tmp1, %s, tmp3\n", X862LLSTR(ud_obj.operand[0].base));
                                        OUTPUT_STRING("and tmp1, tmp1, 0x8000\n");
                                        OUTPUT_STRING("lshr tmp1, tmp1, 15 - OF_SHIFT\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp1\n");
                                    }

                                    OUTPUT_PARAMSTRING("and tmp1, %s, 0xfe00\n", X862LLSTR(ud_obj.operand[0].base));
                                    OUTPUT_STRING("lshr tmp1, tmp1, 1\n");
                                    OUTPUT_STRING("or tmp1, tmp1, tmp3\n");
                                    OUTPUT_PARAMSTRING("and %s, %s, 0xffff00ff\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                                    OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
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

                                    SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                                    OUTPUT_STRING("and tmp3, eflags, CF\n");

                                    if (Tflags_to_write & (FL_CARRY | FL_OVERFLOW))
                                    {
                                        SR_disassemble_change_flags(pOutput, Tflags_to_write & (FL_CARRY | FL_OVERFLOW), 0, 0);
                                    }

                                    if (Tflags_to_write & FL_CARRY)
                                    {
                                        OUTPUT_STRING("and tmp2, tmp1, CF\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp2\n");
                                    }

                                    OUTPUT_STRING("shl tmp3, tmp3, 31\n");

                                    if (Tflags_to_write & FL_OVERFLOW)
                                    {
                                        /* OF = MSB(reg) XOR CF */
                                        OUTPUT_STRING("xor tmp2, tmp1, tmp3\n");
                                        OUTPUT_STRING("and tmp2, tmp2, 0x80000000\n");
                                        OUTPUT_STRING("lshr tmp2, tmp2, 31 - OF_SHIFT\n");
                                        OUTPUT_STRING("or eflags, eflags, tmp2\n");
                                    }

                                    OUTPUT_STRING("lshr tmp1, tmp1, 1\n");
                                    OUTPUT_STRING("or tmp1, tmp1, tmp3\n");

                                    SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
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
                    last_instruction = -1;

                    if (ud_obj.operand[0].type == UD_OP_IMM)
                    {
                        value = SR_disassemble_get_value(&(ud_obj.operand[0]), ZERO_EXTEND);
                    }

                    if (ud_obj.mnemonic == UD_Iretf)
                    {
                        value += 4;
                    }

                    OUTPUT_STRING("POP tmp1\n");

                    if (value != 0)
                    {
                        OUTPUT_PARAMSTRING("add esp, esp, %i\n", value);
                    }

                    OUTPUT_STRING("tcall tmp1\n");
                }
            }
            break;
        case UD_Irol:
        case UD_Iror:
            {
                /* CF - modified, OF - modified for 1-bit shifts, else not modified */

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

                            OUTPUT_STRING("and tmp1, ecx, 0x1f\n");
                            OUTPUT_STRING("sub tmp2, 32, ecx\n");
                            OUTPUT_STRING("and tmp2, tmp2, 0x1f\n");


                            if (ud_obj.mnemonic == UD_Irol)
                            {
                                OUTPUT_PARAMSTRING("shl tmp1, %s, tmp1\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("lshr tmp2, %s, tmp2\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("or %s, tmp1, tmp2\n", X86REGSTR(ud_obj.operand[0].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("lshr tmp1, %s, tmp1\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("shl tmp2, %s, tmp2\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("or %s, tmp1, tmp2\n", X86REGSTR(ud_obj.operand[0].base));
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
                                SR_llasm_helper_rol_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), value);
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
                                SR_llasm_helper_rol_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base), value);
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
                                SR_llasm_helper_rol_8(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), 8, value);
                            }
                        }
                    }
                }

            }
            break;
        case UD_Isahf:
            {
                /* OF - not modified, CF,SF,ZF,AF,PF - modified */

                OUTPUT_STRING("lshr tmp1, eax, 8\n");
                OUTPUT_STRING("and tmp1, tmp1, (CF | SF | ZF | AF | PF)\n");
                OUTPUT_STRING("and eflags, eflags, ~(CF | SF | ZF | AF | PF)\n");
                OUTPUT_STRING("or eflags, eflags, tmp1\n");

            }
            break;
        //case UD_Isal:
        case UD_Isar:
        case UD_Ishl:
        case UD_Ishr:
            {
                /* SF,ZF,PF,CF - modified, AF - undefined, OF - modified for 1-bit shifts, else undefined */

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

                            OUTPUT_STRING("and tmp2, ecx, 0x1f\n");
                            if (ud_obj.mnemonic == UD_Isar)
                            {
                                OUTPUT_PARAMSTRING("ashr %s, %s, tmp2\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                            }
                            else if (ud_obj.mnemonic == UD_Ishr)
                            {
                                OUTPUT_PARAMSTRING("lshr %s, %s, tmp2\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("shl %s, %s, tmp2\n", X86REGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
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
                                SR_llasm_helper_sal_32(ud_obj.mnemonic, X86322LLREG(ud_obj.operand[0].base), value);
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

                            OUTPUT_STRING("and tmp2, ecx, 0x1f\n");
                            if (ud_obj.mnemonic == UD_Isar)
                            {
                                OUTPUT_PARAMSTRING("ext16s tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("ashr tmp1, tmp1, tmp2\n");
                                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                            else if (ud_obj.mnemonic == UD_Ishr)
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, 0xffff\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("lshr tmp1, tmp1, tmp2\n");
                                OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("shl tmp1, %s, tmp2\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
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
                                SR_llasm_helper_sal_16(ud_obj.mnemonic, X86162LLREG(ud_obj.operand[0].base), value);
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

                            OUTPUT_STRING("and tmp2, ecx, 0x1f\n");
                            if (ud_obj.mnemonic == UD_Isar)
                            {
                                OUTPUT_PARAMSTRING("ext8s tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("ashr tmp1, tmp1, tmp2\n");
                                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                            else if (ud_obj.mnemonic == UD_Ishr)
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, 0xff\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("lshr tmp1, tmp1, tmp2\n");
                                OUTPUT_PARAMSTRING("and %s, %s, 0xffffff00\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("shl tmp1, %s, tmp2\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
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
                                SR_llasm_helper_sal_8(ud_obj.mnemonic, X868L2LLREG(ud_obj.operand[0].base), 0, value);
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

                            OUTPUT_STRING("and tmp2, ecx, 0x1f\n");
                            if (ud_obj.mnemonic == UD_Isar)
                            {
                                OUTPUT_PARAMSTRING("ext16s tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("ashr tmp1, tmp1, tmp2\n");
                                OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                            else if (ud_obj.mnemonic == UD_Ishr)
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, 0xff00\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("lshr tmp1, tmp1, tmp2\n");
                                OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("and tmp1, %s, 0xff00\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_STRING("shl tmp1, tmp1, tmp2\n");
                                OUTPUT_PARAMSTRING("ins8hh %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
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
                                SR_llasm_helper_sal_8(ud_obj.mnemonic, X868H2LLREG(ud_obj.operand[0].base), 8, value);
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

                                SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                                SR_llasm_helper_sal_32(ud_obj.mnemonic, LR_TMP1, value);

                                SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP1);
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

                                SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                                SR_llasm_helper_sal_16(ud_obj.mnemonic, LR_TMP1, value);

                                SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP1);
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

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                SR_llasm_helper_sal_8(ud_obj.mnemonic, LR_TMP1, 0, value);

                                SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
                            }
                        }
                    }

                }

            }
            break;
        case UD_Iscasb:
            {
                /* OS,SF,ZF,AF,PF,CF - modified */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("ifnz ecx\n");

                    if (ud_obj.pfx_repne)
                    {
                        OUTPUT_STRING("REPNE_SCASB\n");
                    }
                    else
                    {
                        OUTPUT_STRING("REPE_SCASB\n");
                    }

                    if (flags_to_write)
                    {
                        SR_llasm_helper_add_8l(UD_Icmp, LR_TMP1, LR_EAX, LR_TMP0, 0);
                    }

                    OUTPUT_STRING("endif\n");
                }
                else
                {
                    if (flags_to_write)
                    {
                        OUTPUT_STRING("load8z tmp1, edi, 1\n");
                        OUTPUT_STRING("and tmp2, eflags, DF\n");
                        OUTPUT_STRING("cmovz tmp2, tmp2, 1, -1\n");
                        OUTPUT_STRING("add edi, edi, tmp2\n");

                        SR_llasm_helper_add_8l(UD_Icmp, LR_TMP2, LR_EAX, LR_TMP1, 0);
                    }
                    else
                    {
                        OUTPUT_STRING("and tmp2, eflags, DF\n");
                        OUTPUT_STRING("cmovz tmp2, tmp2, 1, -1\n");
                        OUTPUT_STRING("add edi, edi, tmp2\n");
                    }
                }

            }
            break;
        case UD_Iseta:
        case UD_Isetb:
        case UD_Isetbe:
        //case UD_Isetnb:
        //case UD_Isetnbe:
        case UD_Isetg:
        case UD_Isetge:
        case UD_Isetl:
        case UD_Isetle:
        //case UD_Isetnl:
        //case UD_Isetnle:
        case UD_Isetns:
        case UD_Isetnz:
        case UD_Isets:
        case UD_Isetz:
            {
                /* no flags affected */

                int setifzero;

                if ((ud_obj.mnemonic == UD_Isetz) || (ud_obj.mnemonic == UD_Isetnz))
                {
                    OUTPUT_STRING("and tmp1, eflags, ZF\n");
                    setifzero = (ud_obj.mnemonic == UD_Isetz)?0:1;
                }
                else if ((ud_obj.mnemonic == UD_Isets) || (ud_obj.mnemonic == UD_Isetns))
                {
                    OUTPUT_STRING("and tmp1, eflags, SF\n");
                    setifzero = (ud_obj.mnemonic == UD_Isets)?0:1;
                }
                else if ((ud_obj.mnemonic == UD_Isetb) //|| (ud_obj.mnemonic == UD_Isetnb)
                        )
                {
                    OUTPUT_STRING("and tmp1, eflags, CF\n");
                    setifzero = (ud_obj.mnemonic == UD_Isetb)?0:1;
                }
                else if ((ud_obj.mnemonic == UD_Isetbe) || (ud_obj.mnemonic == UD_Iseta) //|| (ud_obj.mnemonic == UD_Isetnbe)
                        )
                {
                    OUTPUT_STRING("and tmp1, eflags, ZF | CF\n");
                    setifzero = (ud_obj.mnemonic == UD_Isetbe)?0:1;
                }
                else if ((ud_obj.mnemonic == UD_Isetl) || (ud_obj.mnemonic == UD_Isetge))
                {
                    OUTPUT_STRING("lshr tmp1, eflags, OF_SHIFT - SF_SHIFT\n");
                    OUTPUT_STRING("xor tmp1, tmp1, eflags\n");
                    OUTPUT_STRING("and tmp1, tmp1, SF\n");
                    setifzero = (ud_obj.mnemonic == UD_Isetl)?0:1;
                }
                else //if ((ud_obj.mnemonic == UD_Isetle) || (ud_obj.mnemonic == UD_Isetg) || (ud_obj.mnemonic == UD_Isetnle))
                {
                    OUTPUT_STRING("lshr tmp1, eflags, SF_SHIFT - ZF_SHIFT\n");
                    OUTPUT_STRING("lshr tmp2, eflags, OF_SHIFT - ZF_SHIFT\n");
                    OUTPUT_STRING("xor tmp1, tmp1, tmp2\n");
                    OUTPUT_STRING("or tmp1, tmp1, eflags\n");
                    OUTPUT_STRING("and tmp1, tmp1, ZF\n");
                    setifzero = (ud_obj.mnemonic == UD_Isetle)?0:1;
                }


                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (setifzero)
                        {
                            OUTPUT_STRING("cmovz tmp1, tmp1, 1, 0\n");
                        }
                        else
                        {
                            OUTPUT_STRING("cmovz tmp1, tmp1, 0, 1\n");
                        }

                        OUTPUT_PARAMSTRING("and %s, %s, 0xffffff00\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                    {
                        if (setifzero)
                        {
                            OUTPUT_STRING("cmovz tmp1, tmp1, 0x100, 0\n");
                        }
                        else
                        {
                            OUTPUT_STRING("cmovz tmp1, tmp1, 0, 0x100\n");
                        }

                        OUTPUT_PARAMSTRING("and %s, %s, 0xffff00ff\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                    if (setifzero)
                    {
                        OUTPUT_STRING("cmovz tmp1, tmp1, 1, 0\n");
                    }
                    else
                    {
                        OUTPUT_STRING("cmovz tmp1, tmp1, 0, 1\n");
                    }

                    SR_disassemble_write_mem_byte(cOutput, &memadr, LR_TMP1, WRITE8LOW);
                }
            }
            break;
        case UD_Ishld:
        case UD_Ishrd:
            {
                /* SF,ZF,PF,CF - modified, AF - undefined, OF - modified for 1-bit shifts, else undefined */

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

                            OUTPUT_STRING("and tmp1, ecx, 0x1f\n");
                            OUTPUT_STRING("ifnz tmp1\n");

                            OUTPUT_STRING("sub tmp2, 32, ecx\n");

                            if (ud_obj.mnemonic == UD_Ishld)
                            {
                                OUTPUT_PARAMSTRING("shl tmp1, %s, tmp1\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("lshr tmp2, %s, tmp2\n", X86REGSTR(ud_obj.operand[1].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("lshr tmp1, %s, tmp1\n", X86REGSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("shl tmp2, %s, tmp2\n", X86REGSTR(ud_obj.operand[1].base));
                            }

                            OUTPUT_PARAMSTRING("or %s, tmp1, tmp2\n", X86REGSTR(ud_obj.operand[0].base));

                            OUTPUT_STRING("endif\n");

                        }
                        else if (ud_obj.operand[2].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[2]), ZERO_EXTEND) & 0x1f;

                            if (flags_to_write)
                            {
                                fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            }

                            if (value == 0)
                            {
                                flags_write = 0;
                            }
                            else
                            {
                                if (ud_obj.mnemonic == UD_Ishld)
                                {
                                    OUTPUT_PARAMSTRING("shl tmp1, %s, %i\n", X86REGSTR(ud_obj.operand[0].base), value);
                                    OUTPUT_PARAMSTRING("lshr tmp2, %s, %i\n", X86REGSTR(ud_obj.operand[1].base), 32 - value);
                                }
                                else
                                {
                                    OUTPUT_PARAMSTRING("lshr tmp1, %s, %i\n", X86REGSTR(ud_obj.operand[0].base), value);
                                    OUTPUT_PARAMSTRING("shl tmp2, %s, %i\n", X86REGSTR(ud_obj.operand[1].base), 32 - value);
                                }

                                OUTPUT_PARAMSTRING("or %s, tmp1, tmp2\n", X86REGSTR(ud_obj.operand[0].base));
                            }
                        }
                    }
                }

            }
            break;
        case UD_Istc:
            {
                /* set carry flag */

                SR_disassemble_change_flags(cOutput, 0, FL_CARRY, 0);
            }
            break;
        case UD_Istd:
            {
                /* set direction flag */

                OUTPUT_STRING("or eflags, eflags, DF\n");
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
                    OUTPUT_STRING("REP_STOSB\n");
                }
                else
                {
                    OUTPUT_STRING("store8 eax, edi, 1\n");
                    OUTPUT_STRING("and tmp2, eflags, DF\n");
                    OUTPUT_STRING("cmovz tmp2, tmp2, 1, -1\n");
                    OUTPUT_STRING("add edi, edi, tmp2\n");
                }
            }
            break;
        case UD_Istosd:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("REP_STOSD\n");
                }
                else
                {
                    OUTPUT_STRING("store eax, edi, 1\n");
                    OUTPUT_STRING("and tmp2, eflags, DF\n");
                    OUTPUT_STRING("cmovz tmp2, tmp2, 4, -4\n");
                    OUTPUT_STRING("add edi, edi, tmp2\n");
                }
            }
            break;
        case UD_Istosw:
            {
                /* no flags affected */

                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("REP_STOSW\n");
                }
                else
                {
                    OUTPUT_STRING("store16 eax, edi, 1\n");
                    OUTPUT_STRING("and tmp2, eflags, DF\n");
                    OUTPUT_STRING("cmovz tmp2, tmp2, 2, -2\n");
                    OUTPUT_STRING("add edi, edi, tmp2\n");
                }
            }
            break;
        case UD_Itest:
            {
                /* CF,OF cleared, AF undefined, SF,ZF,PF - modified */

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (SR_llasm_helper_and_32(ud_obj.mnemonic, LR_TMP1, X86322LLREG(ud_obj.operand[0].base), X86322LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
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

                                if (SR_llasm_helper_and_32(ud_obj.mnemonic, LR_TMP1, X86322LLREG(ud_obj.operand[0].base), LR_NONE, value))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (SR_llasm_helper_and_16(ud_obj.mnemonic, LR_TMP1, X86162LLREG(ud_obj.operand[0].base), X86162LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            if (SR_llasm_helper_and_16(ud_obj.mnemonic, LR_TMP1, X86162LLREG(ud_obj.operand[0].base), LR_NONE, value))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                if (SR_llasm_helper_and_8l(ud_obj.mnemonic, LR_TMP1, X868L2LLREG(ud_obj.operand[0].base), X868L2LLREG(ud_obj.operand[1].base), 0))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            if (SR_llasm_helper_and_8l(ud_obj.mnemonic, LR_TMP1, X868L2LLREG(ud_obj.operand[0].base), LR_NONE, value))
                            {
                                flags_write = flags_to_write;
                            }
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
                                if (SR_llasm_helper_and_8h(ud_obj.mnemonic, LR_TMP1, X868H2LLREG(ud_obj.operand[0].base), X868H2LLREG(ud_obj.operand[1].base), 8))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_MEM)
                        {
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            if (SR_llasm_helper_and_8h(ud_obj.mnemonic, LR_TMP1, X868H2LLREG(ud_obj.operand[0].base), LR_NONE, value))
                            {
                                flags_write = flags_to_write;
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
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            if (SR_llasm_helper_and_32(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X86322LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            if (fixup[1] == NULL)
                            {
                                int32_t value;

                                value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND);

                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                                if (SR_llasm_helper_and_32(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            if (SR_llasm_helper_and_16(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X86162LLREG(ud_obj.operand[1].base), 0))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            int32_t value;

                            value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[1]), SIGN_EXTEND) << 16;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            if (SR_llasm_helper_and_16(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                if (SR_llasm_helper_and_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X868L2LLREG(ud_obj.operand[1].base), 0))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                if (SR_llasm_helper_and_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, X868H2LLREG(ud_obj.operand[1].base), 8))
                                {
                                    flags_write = flags_to_write;
                                }
                            }
                        }
                        else if (ud_obj.operand[1].type == UD_OP_IMM)
                        {
                            uint32_t value;

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                            value = ((uint32_t) ud_obj.operand[1].lval.ubyte) << 24;

                            if (SR_llasm_helper_and_8l(ud_obj.mnemonic, LR_TMP1, LR_TMP1, LR_NONE, value))
                            {
                                flags_write = flags_to_write;
                            }
                        }
                    }
                }
            }
            break;
        case UD_Iverr:
            {
                /* ZF - modified */

                SR_disassemble_change_flags(cOutput, 0, FL_ZERO, 0);
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
                            OUTPUT_PARAMSTRING("mov tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("ins16 %s, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("ins16 %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[1].base), X862LLSTR(ud_obj.operand[1].base));
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("ins8ll %s, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("ins8ll %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[1].base), X862LLSTR(ud_obj.operand[1].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("ins8lh %s, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("ins8hl %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[1].base), X862LLSTR(ud_obj.operand[1].base));
                            }
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AH && ud_obj.operand[0].base <= UD_R_BH)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                OUTPUT_PARAMSTRING("mov tmp1, %s\n", X862LLSTR(ud_obj.operand[0].base));
                                OUTPUT_PARAMSTRING("ins8hl %s, %s, %s\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("ins8lh %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[1].base), X862LLSTR(ud_obj.operand[1].base));
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

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            SR_disassemble_write_mem_word(cOutput, &memadr, X862LLREG(ud_obj.operand[1].base));

                            OUTPUT_PARAMSTRING("mov %s, tmp1\n", X86REGSTR(ud_obj.operand[1].base));
                        }
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                            SR_disassemble_write_mem_halfword(cOutput, &memadr, X862LLREG(ud_obj.operand[1].base));

                            OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", X862LLSTR(ud_obj.operand[1].base), X862LLSTR(ud_obj.operand[1].base));
                            OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[1].base), X862LLSTR(ud_obj.operand[1].base));
                        }
                    }
                    else if (ud_obj.operand[0].size == 8)
                    {
                        if (ud_obj.operand[1].type == UD_OP_REG)
                        {
                            if (ud_obj.operand[1].base >= UD_R_AL && ud_obj.operand[1].base <= UD_R_BL)
                            {
                                SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_RW, ZERO_EXTEND, &memadr);

                                SR_disassemble_read_mem_byte(cOutput, &memadr, LR_TMP1, READ8TO32ZERO);

                                SR_disassemble_write_mem_byte(cOutput, &memadr, X862LLREG(ud_obj.operand[1].base), WRITE8LOW);

                                OUTPUT_PARAMSTRING("and %s, %s, 0xffffff00\n", X862LLSTR(ud_obj.operand[1].base), X862LLSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("or %s, %s, tmp1\n", X862LLSTR(ud_obj.operand[1].base), X862LLSTR(ud_obj.operand[1].base));
                            }
                            else if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                            }

                        }
                    }

                }
            }
            break;
        case UD_Ixlatb:
            {
                /* no flags affected */

                OUTPUT_STRING("and tmp1, eax, 0xff\n");
                OUTPUT_STRING("add tmp2, ebx, tmp1\n");
                OUTPUT_STRING("load8z tmp0, tmp2, 1\n");
                OUTPUT_STRING("and eax, eax, 0xffffff00\n");
                OUTPUT_STRING("or eax, eax, tmp0\n");
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

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.mnemonic == UD_Ifadd) instr = "FADD";
                else if (ud_obj.mnemonic == UD_Ifdiv) instr = "FDIV";
                else if (ud_obj.mnemonic == UD_Ifmul) instr = "FMUL";
                else /*if (ud_obj.mnemonic == UD_Ifsub)*/ instr = "FSUB";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr);

                            OUTPUT_PARAMSTRING("%s_DOUBLE tmp1, tmp2\n", instr);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            OUTPUT_PARAMSTRING("%s_FLOAT tmp1\n", instr);
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
                                        OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[1].base));
                                    }
                                    else if (ud_obj.operand[1].base == UD_R_ST0)
                                    {
                                        OUTPUT_PARAMSTRING("%s_TO_ST %i\n", instr, X87REGNUM(ud_obj.operand[0].base));
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

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.mnemonic == UD_Ifaddp) instr = "FADDP";
                else if (ud_obj.mnemonic == UD_Ifdivp) instr = "FDIVP";
                else if (ud_obj.mnemonic == UD_Ifmulp) instr = "FMULP";
                else /*if (ud_obj.mnemonic == UD_Ifsubp)*/ instr = "FSUBP";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[0].base));
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
                                        OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[0].base));
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
        case UD_Ifldlg2:
        case UD_Ifldln2:
        case UD_Ifldz:
        case UD_Ifninit:
        case UD_Ifsin:
        case UD_Ifsqrt:
        case UD_Ifucompp:
        case UD_Ifyl2x:
            {
                /* no flags affected */

                char *instr;

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.mnemonic == UD_Ifabs) instr = "FABS";
                else if (ud_obj.mnemonic == UD_Ifchs) instr = "FCHS";
                else if (ud_obj.mnemonic == UD_Ifcompp) instr = "FCOMPP";
                else if (ud_obj.mnemonic == UD_Ifcos) instr = "FCOS";
                else if (ud_obj.mnemonic == UD_Ifld1) instr = "FLD1";
                else if (ud_obj.mnemonic == UD_Ifldlg2) instr = "FLDLG2";
                else if (ud_obj.mnemonic == UD_Ifldln2) instr = "FLDLN2";
                else if (ud_obj.mnemonic == UD_Ifldz) instr = "FLDZ";
                else if (ud_obj.mnemonic == UD_Ifninit) instr = "FNINIT";
                else if (ud_obj.mnemonic == UD_Ifsin) instr = "FSIN";
                else if (ud_obj.mnemonic == UD_Ifsqrt) instr = "FSQRT";
                else if (ud_obj.mnemonic == UD_Ifucompp) instr = "FUCOMPP";
                else /*if (ud_obj.mnemonic == UD_Ifyl2x)*/ instr = "FYL2X";

                if (ud_obj.operand[0].type == UD_NONE)
                {
                    OUTPUT_PARAMSTRING("%s_VOID\n", instr);
                }

            }
            break;
        case UD_Ifcom:
        case UD_Ifcomp:
        case UD_Ifucom:
        case UD_Ifucomp:
            {
                /* no flags affected */

                char *instr;

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.mnemonic == UD_Ifcom) instr = "FCOM";
                else if (ud_obj.mnemonic == UD_Ifcomp) instr = "FCOMP";
                else if (ud_obj.mnemonic == UD_Ifucom) instr = "FUCOM";
                else /*if (ud_obj.mnemonic == UD_Ifucomp)*/ instr = "FUCOMP";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr);

                            OUTPUT_PARAMSTRING("%s_DOUBLE tmp1, tmp2\n", instr);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            OUTPUT_PARAMSTRING("%s_FLOAT tmp1\n", instr);
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
                                        OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[1].base));
                                    }
                                }
                            }
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

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.mnemonic == UD_Ifdivr) instr = "FDIVR";
                else /*if (ud_obj.mnemonic == UD_Ifsubr)*/ instr = "FSUBR";

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

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr);

                            OUTPUT_PARAMSTRING("%s_DOUBLE tmp1, tmp2\n", instr);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            OUTPUT_PARAMSTRING("%s_FLOAT tmp1\n", instr);
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
                                        OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[1].base));
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

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.mnemonic == UD_Ifdivrp) instr = "FDIVRP";
                else /*if (ud_obj.mnemonic == UD_Ifsubrp)*/ instr = "FSUBRP";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[0].base));
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
                                        OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[0].base));
                                    }
                                }
                            }
                        }
                    }
                }

            }
            break;
        case UD_Ifiadd:
        case UD_Ifidiv:
        case UD_Ifidivr:
        case UD_Ifimul:
        case UD_Ifisub:
        case UD_Ifisubr:
            {
                /* no flags affected */

                char *instr;

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.mnemonic == UD_Ifiadd) instr = "FIADD";
                else if (ud_obj.mnemonic == UD_Ifidiv) instr = "FIDIV";
                else if (ud_obj.mnemonic == UD_Ifidivr) instr = "FIDIVR";
                else if (ud_obj.mnemonic == UD_Ifimul) instr = "FIMUL";
                else if (ud_obj.mnemonic == UD_Ifisub) instr = "FISUB";
                else /*if (ud_obj.mnemonic == UD_Ifisubr)*/ instr = "FISUBR";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            OUTPUT_PARAMSTRING("%s_INT32 tmp1\n", instr);
                        }
                        else if (ud_obj.operand[0].size == 16)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32SIGN);

                            OUTPUT_PARAMSTRING("%s_INT32 tmp1\n", instr);
                        }
                    }
                }

            }
            break;
        case UD_Ifild:
            {
                /* no flags affected */

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

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

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr);

                            OUTPUT_STRING("FILD_INT64 tmp1, tmp2\n");
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            OUTPUT_STRING("FILD_INT32 tmp1\n");
                        }
                        else if (ud_obj.operand[0].size == 16)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, SIGN_EXTEND, &memadr);

                            SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32SIGN);

                            OUTPUT_STRING("FILD_INT32 tmp1\n");
                        }
                    }
                }

            }
            break;
        case UD_Ifistp:
            {
                /* no flags affected */

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            OUTPUT_STRING("FISTP_INT64\n");

                            OUTPUT_STRING("load tmp1, tmp0, 4\n");
                            OUTPUT_STRING("add tmpadr, tmp0, 4\n");
                            OUTPUT_STRING("load tmp2, tmpadr, 4\n");

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_doubleword(cOutput, &memadr);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            OUTPUT_STRING("FISTP_INT32\n");

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP0);
                        }
                    }
                }

            }
            break;
        case UD_Ifld:
            {
                /* no flags affected */

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("FLD_ST %i\n", X87REGNUM(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_doubleword(cOutput, &memadr);

                            OUTPUT_STRING("FLD_DOUBLE tmp1, tmp2\n");
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                            SR_disassemble_read_mem_word(cOutput, &memadr, LR_TMP1);

                            OUTPUT_STRING("FLD_FLOAT tmp1\n");
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
                                        OUTPUT_PARAMSTRING("FLD_ST %i\n", X87REGNUM(ud_obj.operand[1].base));
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

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 16)
                    {
                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_READ, ZERO_EXTEND, &memadr);

                        SR_disassemble_read_mem_halfword(cOutput, &memadr, LR_TMP1, READ16TO32ZERO);

                        OUTPUT_STRING("FLDCW_UINT16 tmp1\n");
                    }
                }
            }
            break;
        case UD_Ifnstcw:
        case UD_Ifnstsw:
            {
                /* no flags affected */

                char *instr;

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.mnemonic == UD_Ifnstcw) instr = "FNSTCW";
                else /*if (ud_obj.mnemonic == UD_Ifnstsw)*/ instr = "FNSTSW";

                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        OUTPUT_PARAMSTRING("%s_VOID\n", instr);
                        OUTPUT_PARAMSTRING("and %s, %s, 0xffff0000\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("or %s, %s, tmp0\n", X862LLSTR(ud_obj.operand[0].base), X862LLSTR(ud_obj.operand[0].base));
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].size == 16)
                    {
                        OUTPUT_PARAMSTRING("%s_VOID\n", instr);

                        SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                        SR_disassemble_write_mem_halfword(cOutput, &memadr, LR_TMP0);
                    }
                }
            }
        case UD_Ifst:
        case UD_Ifstp:
            {
                /* no flags affected */

                char *instr;

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.mnemonic == UD_Ifst) instr = "FST";
                else /*if (ud_obj.mnemonic == UD_Ifstp)*/ instr = "FSTP";

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("%s_ST %i\n", instr, X87REGNUM(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        if (ud_obj.operand[0].size == 64)
                        {
                            OUTPUT_PARAMSTRING("%s_DOUBLE\n", instr);

                            OUTPUT_STRING("load tmp1, tmp0, 4\n");
                            OUTPUT_STRING("add tmpadr, tmp0, 4\n");
                            OUTPUT_STRING("load tmp2, tmpadr, 4\n");

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_doubleword(cOutput, &memadr);
                        }
                        else if (ud_obj.operand[0].size == 32)
                        {
                            OUTPUT_PARAMSTRING("%s_FLOAT\n", instr);

                            SR_disassemble_get_madr(cOutput, &(ud_obj.operand[0]), fixup[0], extrn[0], UD_NONE, MADR_WRITE, ZERO_EXTEND, &memadr);

                            SR_disassemble_write_mem_word(cOutput, &memadr, LR_TMP0);
                        }
                    }
                }

            }
            break;
        case UD_Ifxch:
            {
                /* no flags affected */

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                if (ud_obj.operand[1].type == UD_NONE)
                {
                    if (ud_obj.operand[0].type == UD_OP_REG)
                    {
                        if (ud_obj.operand[0].base >= UD_R_ST0 && ud_obj.operand[0].base <= UD_R_ST7)
                        {
                            OUTPUT_PARAMSTRING("FXCH_ST %i\n", X87REGNUM(ud_obj.operand[0].base));
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
                                        OUTPUT_PARAMSTRING("FXCH_ST %i\n", X87REGNUM(ud_obj.operand[1].base));
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

                //if (flags_to_write)
                //{
                //    fprintf(stderr, "Error: flags not calculated - %i - %i - %s\n", Entry, cur_ofs, output->str);
                //}

                OUTPUT_STRING("; wait\n");
            }
            break;

#endif
        default:
            fprintf(stderr, "Error: unknown instruction - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);

            return 5;
    }

    if (cOutput[0] == 0)
    {
            fprintf(stderr, "Error: llasm instruction error - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);

            return 7;
    }

    {
        char *newstr, *pos;

        if ((last_instruction != 0) || ((last_instruction == 0) && (*plast_instruction != 0)))
        {
            if (last_instruction == 0)
            {
                SR_add_label(Entry, cur_ofs + decoded_length);
            }

            if (last_instruction >= 0)
            {
                SR_get_label(cLabel, section[Entry].start + cur_ofs + decoded_length);

                OUTPUT_PARAMSTRING("\ntcall %s\n", cLabel);
            }

            OUTPUT_STRING("endp\n");
        }

        newstr = (char *) malloc(strlen(cOutput) + strlen(output->str) + 3);
        if (newstr == NULL)
        {
            fprintf(stderr, "Error: not enough memory - %i - %i\n", Entry, (unsigned int)cur_ofs);

            return 3;
        }

        newstr[0] = ';';
        strcpy(&(newstr[1]), output->str);

        pos = strchr(newstr, '\n');
        while (pos != NULL)
        {
            *pos = ';';

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
    *plast_instruction = last_instruction;

    return 0;

#undef OUTPUT_PARAMSTRING
#undef OUTPUT_STRING
}

#else

int SR_disassemble_llasm_instruction(unsigned int Entry, output_data *output, uint_fast32_t flags_to_write, uint_fast32_t *pflags_write, uint_fast32_t *pflags_read, int *plast_instruction)
{
    return 1;
}

#endif
