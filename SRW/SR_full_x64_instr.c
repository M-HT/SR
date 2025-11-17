/**
 *
 *  Copyright (C) 2019-2025 Roman Pauer
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


#if (OUTPUT_TYPE == OUT_X64)

#define X86REGSTR(x) ud_reg_tab[(x) - UD_R_AL]
#define HIGH2LOWREGSTR(x) ud_reg_tab[(x) - UD_R_AH]


enum extend_mode {
    ZERO_EXTEND = 0,
    SIGN_EXTEND = 1
};

enum copy_mode {
    ONLY_COPY_UNCHANGED = 0,
    COPY_ALWAYS = 1
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

static int SR_replace_stack_register(char *cResult, const char *cOrigOutput, enum copy_mode mode)
{
    int num_found, src_offset, index;
    int found_offset[3];
    char *found_ptr;
    static const char delimiters[] = " ,+-*[]()\n";

    if (cOrigOutput[0] == 0 || cOrigOutput[1] == 0) return 1;

    num_found = 0;
    src_offset = 1;

    for (;;)
    {
        found_ptr = strstr(cOrigOutput + src_offset, "sp");
        if (found_ptr == NULL) break;

        src_offset = ((uintptr_t)found_ptr - (uintptr_t)cOrigOutput) + 2;

        if (found_ptr[2] != 0 && strchr(delimiters, found_ptr[2]) == NULL) continue;

        if (found_ptr[-1] == 'e')
        {
            found_ptr--;
            if (found_ptr == cOrigOutput) continue;
        }

        if (found_ptr[-1] != 0 && strchr(delimiters, found_ptr[-1]) == NULL) continue;

        num_found++;
        if (num_found > 3) return 0;

        found_offset[num_found - 1] = (uintptr_t)found_ptr - (uintptr_t)cOrigOutput;
    }

    if ((num_found == 0) && (mode == ONLY_COPY_UNCHANGED)) return 1;

    src_offset = 0;
    for (index = 0; index < num_found; index++)
    {
        memcpy(cResult, cOrigOutput + src_offset, found_offset[index] - src_offset);
        cResult += found_offset[index] - src_offset;
        src_offset = found_offset[index];

        cResult[0] = 'r';
        cResult[1] = '1';
        cResult[2] = '1';
        cResult[3] = (cOrigOutput[src_offset] == 'e') ? 'd' : 'w';
        cResult += 4;
        src_offset += (cOrigOutput[src_offset] == 'e') ? 3 : 2;
    }

    strcpy(cResult, cOrigOutput + src_offset);
    return 1;
}

static int SR_merge_push_pop(enum ud_mnemonic_code mnemonic, enum ud_type *base_reg, int max_regs, unsigned int Entry, output_data *output, region_data *region)
{
    int num_regs;
    uint_fast32_t cur_ofs2;
    output_data *output2;

    base_reg[0] = ud_obj.operand[0].base;

    if (output->ofs != region->end_ofs)
    {
        output2 = section_output_list_FindEntryEqual(Entry, output->ofs + output->len + 1);
        if (output2 != NULL)
        {
            ud_set_input_buffer(&ud_obj, &(section[Entry].adr[output2->ofs]), output2->len + 1);
            ud_set_pc(&ud_obj, section[Entry].start + output2->ofs);

            if (ud_disassemble(&ud_obj))
            {
                if (ud_obj.mnemonic == mnemonic &&
                    ud_obj.operand[0].type == UD_OP_REG &&
                    ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI &&
                    ud_obj.operand[0].base != UD_R_ESP)
                {
                    return 0;
                }
            }
        }
    }

    num_regs = 1;
    cur_ofs2 = output->ofs;
    while (num_regs < max_regs && cur_ofs2 != region->begin_ofs)
    {
        cur_ofs2--;
        output2 = section_output_list_FindEntryEqualOrLower(Entry, cur_ofs2);
        if (output2 == NULL) break;

        cur_ofs2 = output2->ofs;

        ud_set_input_buffer(&ud_obj, &(section[Entry].adr[cur_ofs2]), output2->len + 1);
        ud_set_pc(&ud_obj, section[Entry].start + cur_ofs2);

        if (!ud_disassemble(&ud_obj)) break;

        if (ud_obj.mnemonic == mnemonic &&
            ud_obj.operand[0].type == UD_OP_REG &&
            ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI &&
            ud_obj.operand[0].base != UD_R_ESP)
        {
            base_reg[num_regs] = ud_obj.operand[0].base;
            num_regs++;
        }
        else break;
    }

    return num_regs;
}

int SR_disassemble_x64_instruction(unsigned int Entry, output_data *output, region_data *region)
{
    char cOutput[8192];
    char cOutPart[256];
    char *pOutput;
    fixup_data *fixup1, *fixup2, *fixup[3];
    uint_fast32_t cur_ofs;
    unsigned int decoded_length;

    cur_ofs = output->ofs;
    decoded_length = ud_insn_len(&ud_obj);


    fixup[0] = NULL; fixup[1] = NULL; fixup[2] = NULL;

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
            /* OS,SF,ZF,AF,PF,CF - modified */
        case UD_Iadd:
        case UD_Isub:
            /* OS,SF,ZF,AF,PF,CF - modified */
        case UD_Iand:
        case UD_Ior:
        case UD_Ixor:
            /* CF,OF cleared, AF undefined, SF,ZF,PF - modified */
        case UD_Ibsr:
            /* CF,OF,AF,SF,PF undefined, ZF - modified */
        case UD_Ibswap:
            /* no flags affected */
        case UD_Ibt:
            /* ZF,OF,AF,SF,PF undefined, CF - modified */
        case UD_Ibts:
            /* ZF,OF,AF,SF,PF undefined, CF - modified */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Icall:
            /* no flags affected */
            {
                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    OUTPUT_PARAMSTRING("mov r9d, %s\n", X86REGSTR(ud_obj.operand[0].base));
                    OUTPUT_STRING("CALL r9");
                }
                else
                {
                    int copy_call;

                    copy_call = 0;
                    if (ud_obj.operand[0].type == UD_OP_MEM)
                    {
                        char *mem_start;
                        mem_start = strchr(output->str, '[');
                        if (mem_start != NULL)
                        {
                            OUTPUT_STRING("mov r9d, ");
                            if (!SR_replace_stack_register(pOutput + strlen(pOutput), mem_start, COPY_ALWAYS))
                            {
                                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                                return 7;
                            }
                            OUTPUT_STRING("\n");
                            OUTPUT_STRING("CALL r9");
                            if (ud_obj.br_far)
                            {
                                OUTPUT_STRING(", 0");
                            }
                        }
                        else
                        {
                            copy_call = 1;
                        }
                    }
                    else if ((output->str[0] == 'c') && (output->str[1] == 'a') && (output->str[2] == 'l') && (output->str[3] == 'l') && (output->str[4] == ' '))
                    {
                        copy_call = 1;
                    }

                    if (copy_call)
                    {
                        if (!SR_replace_stack_register(pOutput, output->str, COPY_ALWAYS))
                        {
                            fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            return 7;
                        }
                        cOutput[0] = 'C';
                        cOutput[1] = 'A';
                        cOutput[2] = 'L';
                        cOutput[3] = 'L';
                    }
                }
            }

            break;
        case UD_Icbw:
            /* no flags affected */
        case UD_Icdq:
            /* no flags affected */
        case UD_Iclc:
            /* clear carry flag */
        case UD_Icld:
            /* clear direction flag */
        case UD_Icli:
            /* clear interrupt flag */
        case UD_Icmc:
            /* CF - inverted */
        case UD_Icmp:
            /* OS,SF,ZF,AF,PF,CF - modified */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Icmpsb:
            /* OS,SF,ZF,AF,PF,CF - modified */
            {
                if (ud_obj.pfx_rep)
                {
                    OUTPUT_STRING("rep a32 cmpsb");
                }
                else if (ud_obj.pfx_repe)
                {
                    OUTPUT_STRING("repe a32 cmpsb");
                }
                else if (ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("repne a32 cmpsb");
                }
                else
                {
                    OUTPUT_STRING("a32 cmpsb");
                }
            }

            break;
        case UD_Icwd:
            /* no flags affected */
        case UD_Icwde:
            /* no flags affected */
        case UD_Idec:
        case UD_Iinc:
            /* CF - not modified, OS,SF,ZF,AF,PF - modified */
        case UD_Idiv:
            /* CF,OF,SF,ZF,AF,PF undefined */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Ienter:
            /* no flags affected */
            {
                uint32_t NestingLevel, Size;

                Size = SR_disassemble_get_value(&(ud_obj.operand[0]), ZERO_EXTEND);
                NestingLevel = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND) % 32;

                OUTPUT_PARAMSTRING("ENTER %i, %i", Size, NestingLevel);
            }

            break;
        case UD_Iidiv:
            /* CF,OF,SF,ZF,AF,PF undefined */
        case UD_Iimul:
            /* SF,ZF,AF,PF undefined, CF,OF - modified */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Iin:
            /* no flags affected */
            {
                if (ud_obj.operand[1].type == UD_OP_REG)
                {
                    OUTPUT_PARAMSTRING("CALL x86_in_%s_dx", X86REGSTR(ud_obj.operand[0].base));
                }
                else
                {
                    OUTPUT_PARAMSTRING("mov r9d, 0x%x\n", ud_obj.operand[1].lval.ubyte);
                    OUTPUT_PARAMSTRING("CALL x86_in_%s_imm", X86REGSTR(ud_obj.operand[0].base));
                }
            }

            break;
        case UD_Iint:
        case UD_Iint1:
        case UD_Iint3:
            /* all flags modified */
            {
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

                OUTPUT_PARAMSTRING("mov r9d, %i\n", intno);
                OUTPUT_STRING("CALL x86_int");
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
            /* no flags affected */
            break;
        case UD_Ijmp:
            /* no flags affected */
            {
                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    OUTPUT_PARAMSTRING("mov r9d, %s\n", X86REGSTR(ud_obj.operand[0].base));
                    OUTPUT_STRING("jmp r9");
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    char *mem_start;
                    mem_start = strchr(output->str, '[');
                    if (mem_start != NULL)
                    {
                        OUTPUT_STRING("mov r9d, ");
                        if (!SR_replace_stack_register(pOutput + strlen(pOutput), mem_start, COPY_ALWAYS))
                        {
                            fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            return 7;
                        }
                        OUTPUT_STRING("\n");
                        OUTPUT_STRING("jmp r9");
                    }
                }
            }

            break;
        case UD_Ilahf:
            /* no flags affected */

            OUTPUT_STRING("LAHF");

            break;
        case UD_Ilea:
            /* no flags affected */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Ileave:
            /* no flags affected */

            OUTPUT_STRING("LEAVE");

            break;
        case UD_Ilodsb:
            /* no flags affected */
            {
                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("rep a32 lodsb");
                }
                else
                {
                    OUTPUT_STRING("a32 lodsb");
                }
            }

            break;
        case UD_Ilodsd:
            /* no flags affected */
            {
                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("rep a32 lodsd");
                }
                else
                {
                    OUTPUT_STRING("a32 lodsd");
                }
            }

            break;
        case UD_Ilodsw:
            /* no flags affected */
            {
                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("rep a32 lodsw");
                }
                else
                {
                    OUTPUT_STRING("a32 lodsw");
                }
            }

            break;
        case UD_Iloop:
        case UD_Iloope:
        //case UD_Iloopn:
        case UD_Iloopne:
        //case UD_Iloopnz:
        //case UD_Iloopz:
            /* no flags affected */

            OUTPUT_STRING("a32 ");
            OUTPUT_STRING(output->str);

            break;
        case UD_Ilds:
        case UD_Iles:
        case UD_Ilfs:
        case UD_Ilgs:
        case UD_Ilss:
            /* no flags affected */

            if (!SR_replace_stack_register(pOutput, output->str, COPY_ALWAYS))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }
            cOutput[0] = 'm';
            cOutput[1] = 'o';
            cOutput[2] = 'v';

            break;
        case UD_Imov:
            /* no flags affected */
            {
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
                    if (ud_obj.operand[1].type == UD_OP_MEM &&
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

                        OUTPUT_PARAMSTRING("mov r10d, 0x%x\n", ud_obj.operand[1].lval.udword);
                        OUTPUT_PARAMSTRING("CALL x86_mov_reg_mem_%i\n", ud_obj.operand[0].size);

                        if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                        {
                            OUTPUT_PARAMSTRING("mov %s, r9d", X86REGSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                        {
                            OUTPUT_PARAMSTRING("mov %s, r9w", X86REGSTR(ud_obj.operand[0].base));
                        }
                        else if (ud_obj.operand[0].base >= UD_R_AL && ud_obj.operand[0].base <= UD_R_BL)
                        {
                            OUTPUT_PARAMSTRING("mov %s, r9b", X86REGSTR(ud_obj.operand[0].base));
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("xchg %s, %s\n", HIGH2LOWREGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("mov %s, r9b\n", HIGH2LOWREGSTR(ud_obj.operand[0].base));
                            OUTPUT_PARAMSTRING("xchg %s, %s", HIGH2LOWREGSTR(ud_obj.operand[0].base), X86REGSTR(ud_obj.operand[0].base));
                        }
                    }
                    else if (ud_obj.operand[1].type == UD_OP_MEM &&
                        ud_obj.operand[1].base == UD_NONE &&
                        ud_obj.operand[1].index == UD_NONE &&
                        ud_obj.operand[1].size == 32 &&
                        fixup[1] == NULL &&
                        (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI) &&
                        ud_obj.pfx_seg == UD_R_FS
                       )
                    {
                        // mov reg, fs:[const]

                        OUTPUT_PARAMSTRING("mod r10d, 0x%x\n", ud_obj.operand[1].lval.udword);
                        OUTPUT_STRING("CALL x86_read_fs_dword\n");
                        OUTPUT_PARAMSTRING("mov %s, r9d", X86REGSTR(ud_obj.operand[0].base));
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

                        OUTPUT_PARAMSTRING("mov r10d, %s\n", X86REGSTR(ud_obj.operand[1].base));
                        OUTPUT_STRING("CALL x86_read_fs_dword\n");
                        OUTPUT_PARAMSTRING("mov %s, r9d", X86REGSTR(ud_obj.operand[0].base));
                    }
                }
                else if (ud_obj.operand[0].type == UD_OP_MEM)
                {
                    if (ud_obj.operand[0].base == UD_NONE &&
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
                            OUTPUT_PARAMSTRING("mov r10d, 0x%x\n", ud_obj.operand[0].lval.udword);

                            if (ud_obj.operand[1].base >= UD_R_AH && ud_obj.operand[1].base <= UD_R_BH)
                            {
                                OUTPUT_PARAMSTRING("xchg %s, %s\n", HIGH2LOWREGSTR(ud_obj.operand[1].base), X86REGSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("mov r9b, %s\n", HIGH2LOWREGSTR(ud_obj.operand[1].base));
                                OUTPUT_PARAMSTRING("xchg %s, %s\n", HIGH2LOWREGSTR(ud_obj.operand[1].base), X86REGSTR(ud_obj.operand[1].base));
                            }
                            else
                            {
                                OUTPUT_PARAMSTRING("mov r9%c, %s\n", (ud_obj.operand[1].size == 8) ? 'b' : (ud_obj.operand[1].size == 16) ? 'w' : 'd', X86REGSTR(ud_obj.operand[1].base));
                            }

                            OUTPUT_PARAMSTRING("CALL x86_mov_mem_reg_%i", ud_obj.operand[1].size);
                        }
                        else
                        {
                            uint32_t value;

                            value = SR_disassemble_get_value(&(ud_obj.operand[1]), ZERO_EXTEND);

                            OUTPUT_PARAMSTRING("mov r10d, 0x%x\n", ud_obj.operand[0].lval.udword);
                            OUTPUT_PARAMSTRING("mov r9%c, 0x%x\n", (ud_obj.operand[1].size == 8) ? 'b' : (ud_obj.operand[1].size == 16) ? 'w' : 'd', value);
                            OUTPUT_PARAMSTRING("CALL x86_mov_mem_reg_%i", ud_obj.operand[1].size);
                        }
                    }
                    else if (ud_obj.operand[0].base == UD_NONE &&
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

                        OUTPUT_PARAMSTRING("mov r10d, 0x%x\n", ud_obj.operand[0].lval.udword);
                        OUTPUT_PARAMSTRING("mov r9d, %s\n", X86REGSTR(ud_obj.operand[1].base));
                        OUTPUT_STRING("CALL x86_write_fs_dword");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI &&
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

                        OUTPUT_PARAMSTRING("mov r10d, %s\n", X86REGSTR(ud_obj.operand[0].base));
                        OUTPUT_PARAMSTRING("mov r9d, %s\n", X86REGSTR(ud_obj.operand[1].base));
                        OUTPUT_STRING("CALL x86_write_fs_dword");
                    }
                }

                if (cOutput[0] == 0)
                {
                    if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
                    {
                        fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                        return 7;
                    }
                }
            }

            break;
        case UD_Imovsb:
            /* no flags affected */
            {
                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("rep a32 movsb");
                }
                else
                {
                    OUTPUT_STRING("a32 movsb");
                }
            }

            break;
        case UD_Imovsd:
            /* no flags affected */
            {
                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("rep a32 movsd");
                }
                else
                {
                    OUTPUT_STRING("a32 movsd");
                }
            }

            break;
        case UD_Imovsw:
            /* no flags affected */
            {
                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("rep a32 movsw");
                }
                else
                {
                    OUTPUT_STRING("a32 movsw");
                }
            }

            break;
        case UD_Imovsx:
            /* no flags affected */
        case UD_Imovzx:
            /* no flags affected */
        case UD_Imul:
            /* SF,ZF,AF,PF undefined, CF,OF - modified */
        case UD_Ineg:
            /* OS,SF,ZF,AF,PF,CF - modified */
        case UD_Inop:
            /* no flags affected */
        case UD_Inot:
            /* no flags affected */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Iout:
            /* no flags affected */
            {
                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    OUTPUT_PARAMSTRING("CALL x86_out_dx_%s", X86REGSTR(ud_obj.operand[1].base));
                }
                else
                {
                    OUTPUT_PARAMSTRING("mov r9d, 0x%x\n", ud_obj.operand[0].lval.ubyte);
                    OUTPUT_PARAMSTRING("CALL x86_out_imm_%s", X86REGSTR(ud_obj.operand[1].base));
                }
            }

            break;
        case UD_Ipop:
            /* no flags affected */
            {
                if (fixup[0] == NULL &&
                    ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[0].base == UD_R_ESP)
                        {
                            OUTPUT_STRING("POP32 r9d\n");
                            OUTPUT_STRING("mov r11d, r9d");
                        }
                        else
                        {
                            enum ud_type base_reg[15];
                            int num_regs, reg_index;

                            // merge multiple pop instructions
                            num_regs = SR_merge_push_pop(UD_Ipop, base_reg, 15, Entry, output, region);

                            if (num_regs)
                            {
                                OUTPUT_STRING("POP32");
                                for (reg_index = 0; reg_index < num_regs; reg_index++)
                                {
                                    if (reg_index >= 1)
                                    {
                                        OUTPUT_STRING(",");
                                    }
                                    OUTPUT_PARAMSTRING(" %s", X86REGSTR(base_reg[reg_index]));
                                }
                            }
                            else
                            {
                                OUTPUT_STRING("\n");
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_ES && ud_obj.operand[0].base <= UD_R_GS)
                    {
                        OUTPUT_STRING("lea r11d, [r11d+4]");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[0].base == UD_R_SP)
                        {
                            OUTPUT_STRING("POP16 r9w\n");
                            OUTPUT_STRING("mov r11w, r9w");
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("POP16 %s", X86REGSTR(ud_obj.operand[0].base));
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
                        // pop dword fs:[const]

                        OUTPUT_STRING("POP32 r9d\n");
                        OUTPUT_PARAMSTRING("mov r10d, 0x%x\n", ud_obj.operand[0].lval.udword);
                        OUTPUT_STRING("CALL x86_write_fs_dword");
                    }
                    else if (ud_obj.operand[0].size == 32)
                    {
                        OUTPUT_STRING("POP32 r9d\n");
                        OUTPUT_STRING("mov ");
                        if (!SR_replace_stack_register(pOutput + strlen(pOutput), output->str + 4, COPY_ALWAYS))
                        {
                            fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            return 7;
                        }
                        OUTPUT_STRING(", r9d");
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        OUTPUT_STRING("POP16 r9w\n");
                        OUTPUT_STRING("mov ");
                        if (!SR_replace_stack_register(pOutput + strlen(pOutput), output->str + 4, COPY_ALWAYS))
                        {
                            fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            return 7;
                        }
                        OUTPUT_STRING(", r9w");
                    }
                }

                if (cOutput[0] == 0)
                {
                    fprintf(stderr, "Error: x64 instruction error - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);

                    return 7;
                }
            }

            break;
        case UD_Ipopa: // popaw
            /* no flags affected */

            OUTPUT_STRING("POPAW");

            break;
        case UD_Ipopad:
            /* no flags affected */

            OUTPUT_STRING("POPAD");

            break;
        case UD_Ipopfd:
            /* all flags modified */

            OUTPUT_STRING("POPFD");

            break;
        case UD_Ipush:
            /* no flags affected */
            {
                if (fixup[0] == NULL &&
                    ud_obj.operand[0].type == UD_OP_REG)
                {
                    if (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI)
                    {
                        if (ud_obj.operand[0].base == UD_R_ESP)
                        {
                            OUTPUT_STRING("mov r9d, r11d\n");
                            OUTPUT_STRING("PUSH32 r9d");
                        }
                        else
                        {
                            enum ud_type base_reg[15];
                            int num_regs;

                            // merge multiple push instructions
                            num_regs = SR_merge_push_pop(UD_Ipush, base_reg, 15, Entry, output, region);
                            if (num_regs)
                            {
                                OUTPUT_STRING("PUSH32");
                                for (; num_regs >= 1; num_regs--)
                                {
                                    OUTPUT_PARAMSTRING(" %s", X86REGSTR(base_reg[num_regs - 1]));
                                    if (num_regs >= 2)
                                    {
                                        OUTPUT_STRING(",");
                                    }
                                }
                            }
                            else
                            {
                                OUTPUT_STRING("\n");
                            }
                        }
                    }
                    else if (ud_obj.operand[0].base >= UD_R_ES && ud_obj.operand[0].base <= UD_R_GS)
                    {
                        OUTPUT_STRING("PUSH32 0\n");
                    }
                    else if (ud_obj.operand[0].base >= UD_R_AX && ud_obj.operand[0].base <= UD_R_DI)
                    {
                        if (ud_obj.operand[0].base == UD_R_SP)
                        {
                            OUTPUT_STRING("mov r9w, r11w\n");
                            OUTPUT_STRING("PUSH16 r9w");
                        }
                        else
                        {
                            OUTPUT_PARAMSTRING("PUSH16 %s", X86REGSTR(ud_obj.operand[0].base));
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

                        OUTPUT_PARAMSTRING("mov r10d, 0x%x\n", ud_obj.operand[0].lval.udword);
                        OUTPUT_STRING("CALL x86_read_fs_dword\n");
                        OUTPUT_STRING("PUSH32 r9d");
                    }
                    else if (ud_obj.operand[0].size == 32)
                    {
                        OUTPUT_STRING("mov r9d, ");
                        if (!SR_replace_stack_register(pOutput + strlen(pOutput), output->str + 5, COPY_ALWAYS))
                        {
                            fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            return 7;
                        }
                        OUTPUT_STRING("\n");
                        OUTPUT_STRING("PUSH32 r9d");
                    }
                    else if (ud_obj.operand[0].size == 16)
                    {
                        OUTPUT_STRING("mov r9w, ");
                        if (!SR_replace_stack_register(pOutput + strlen(pOutput), output->str + 5, COPY_ALWAYS))
                        {
                            fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                            return 7;
                        }
                        OUTPUT_STRING("\n");
                        OUTPUT_STRING("PUSH16 r9w");
                    }
                }
                else if (fixup[0] == NULL &&
                         ud_obj.operand[0].type == UD_OP_IMM)
                {
                    int32_t value;

                    value = (int32_t) SR_disassemble_get_value(&(ud_obj.operand[0]), SIGN_EXTEND);

                    OUTPUT_PARAMSTRING("PUSH32 0x%x", value);
                }
                else if (ud_obj.operand[0].type == UD_OP_IMM)
                {
                    OUTPUT_STRING("PUSH32 ");
                    OUTPUT_STRING(output->str + 5);
                }

                if (cOutput[0] == 0)
                {
                    fprintf(stderr, "Error: x64 instruction error - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);

                    return 7;
                }
            }

            break;
        case UD_Ipusha: // pushaw
            /* no flags affected */

            OUTPUT_STRING("PUSHAW");

            break;
        case UD_Ipushad:
            /* no flags affected */

            OUTPUT_STRING("PUSHAD");

            break;
        case UD_Ipushfd:
            /* no flags affected */

            OUTPUT_STRING("PUSHFD");

            break;
        case UD_Ipushfw:
            /* no flags affected */

            OUTPUT_STRING("PUSHFW");

            break;
        case UD_Ircl:
            /* CF - modified, OF - modified for 1-bit shifts, else not modified */
        case UD_Ircr:
            /* CF - modified, OF - modified for 1-bit shifts, else not modified */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Iret:
        case UD_Iretf:
            /* no flags affected */
            {
                int32_t value;

                if (ud_obj.operand[0].type == UD_OP_IMM)
                {
                    value = SR_disassemble_get_value(&(ud_obj.operand[0]), ZERO_EXTEND);
                }
                else
                {
                    value = 0;
                }

                if (ud_obj.mnemonic == UD_Iretf)
                {
                    value += 4;
                }

                OUTPUT_STRING("RET");
                if (value != 0)
                {
                    OUTPUT_PARAMSTRING(" %i", value);
                }
            }

            break;
        case UD_Irol:
        case UD_Iror:
            /* CF - modified, OF - modified for 1-bit shifts, else not modified */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Isahf:
            /* OF - not modified, CF,SF,ZF,AF,PF - modified */

            OUTPUT_STRING("SAHF");

            break;
        //case UD_Isal:
        case UD_Isar:
        case UD_Ishl:
        case UD_Ishr:
            /* SF,ZF,PF,CF - modified, AF - undefined, OF - modified for 1-bit shifts, else undefined */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Iscasb:
            /* OS,SF,ZF,AF,PF,CF - modified */
            {
                if (ud_obj.pfx_rep)
                {
                    OUTPUT_STRING("rep a32 scasb");
                }
                else if (ud_obj.pfx_repe)
                {
                    OUTPUT_STRING("repe a32 scasb");
                }
                else if (ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("repne a32 scasb");
                }
                else
                {
                    OUTPUT_STRING("a32 scasb");
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
            /* no flags affected */
        case UD_Ishld:
        case UD_Ishrd:
            /* SF,ZF,PF,CF - modified, AF - undefined, OF - modified for 1-bit shifts, else undefined */
        case UD_Istc:
            /* set carry flag */
        case UD_Istd:
            /* set direction flag */
        case UD_Isti:
            /* set interrupt flag */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Istosb:
            /* no flags affected */
            {
                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("rep a32 stosb");
                }
                else
                {
                    OUTPUT_STRING("a32 stosb");
                }
            }

            break;
        case UD_Istosd:
            /* no flags affected */
            {
                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("rep a32 stosd");
                }
                else
                {
                    OUTPUT_STRING("a32 stosd");
                }
            }

            break;
        case UD_Istosw:
            /* no flags affected */
            {
                if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                {
                    OUTPUT_STRING("rep a32 stosw");
                }
                else
                {
                    OUTPUT_STRING("a32 stosw");
                }
            }

            break;
        case UD_Itest:
            /* CF,OF cleared, AF undefined, SF,ZF,PF - modified */
        case UD_Iverr:
            /* ZF - modified */
        case UD_Ixchg:
            /* no flags affected */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;
        case UD_Ixlatb:
            /* no flags affected */

            OUTPUT_STRING("a32 xlatb");

            break;
        case UD_Ifadd:
        case UD_Ifdiv:
        case UD_Ifmul:
        case UD_Ifsub:
            /* no flags affected */
        case UD_Ifaddp:
        case UD_Ifdivp:
        case UD_Ifmulp:
        case UD_Ifsubp:
            /* no flags affected */
        case UD_Ifabs:
        case UD_Ifchs:
        case UD_Ifcompp:
        case UD_Ifcos:
        case UD_Ifld1:
        case UD_Ifldlg2:
        case UD_Ifldln2:
        case UD_Ifldz:
        case UD_Ifninit:
        case UD_Ifptan:
        case UD_Ifsin:
        case UD_Ifsqrt:
        case UD_Ifucompp:
        case UD_Ifyl2x:
            /* no flags affected */
        case UD_Ifcom:
        case UD_Ifcomp:
        case UD_Ifucom:
        case UD_Ifucomp:
            /* no flags affected */
        case UD_Ifdivr:
        case UD_Ifsubr:
            /* no flags affected */
        case UD_Ifdivrp:
        case UD_Ifsubrp:
            /* no flags affected */
        case UD_Ifiadd:
        case UD_Ifidiv:
        case UD_Ifidivr:
        case UD_Ifimul:
        case UD_Ifisub:
        case UD_Ifisubr:
            /* no flags affected */
        case UD_Ifild:
            /* no flags affected */
        case UD_Ifist:
            /* no flags affected */
        case UD_Ifistp:
            /* no flags affected */
        case UD_Ifld:
                /* no flags affected */
        case UD_Ifldcw:
            /* no flags affected */
        case UD_Ifnstcw:
        case UD_Ifnstsw:
            /* no flags affected */
        case UD_Ifst:
        case UD_Ifstp:
            /* no flags affected */
        case UD_Ifxch:
            /* no flags affected */
        case UD_Iwait:
            /* no flags affected */

            if (!SR_replace_stack_register(pOutput, output->str, ONLY_COPY_UNCHANGED))
            {
                fprintf(stderr, "Error: stack variable replacement - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);
                return 7;
            }

            break;

        default:
            fprintf(stderr, "Error: unknown instruction - %i - %i - %s\n", Entry, (unsigned int)cur_ofs, output->str);

            return 5;
    }

    if (cOutput[0] != 0)
    {
        char *newstr, *pos;

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

        if (cOutput[0] != '\n' || cOutput[1] != 0)
        {
            strcat(newstr, "\n");
            strcat(newstr, cOutput);
        }

        free(output->str);
        output->str = newstr;
    }


#ifdef DISPLAY_DISASSEMBLY
    printf("loc_%X: %s\n", section[Entry].start + cur_ofs, output->str);
#endif


    return 0;

#undef OUTPUT_PARAMSTRING
#undef OUTPUT_STRING
}

#else

int SR_disassemble_x64_instruction(unsigned int Entry, output_data *output, region_data *region)
{
    return 1;
}

#endif
