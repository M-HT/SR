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

#undef DISPLAY_DISASSEMBLY

void SR_disassemble_add_address(unsigned int Entry, uint32_t address)
{
    output_data *output;
    bound_data *bound;
    uint_fast32_t SecNum, RelAdr;

    if (address >= section[Entry].start &&
        address <= section[Entry].start + section[Entry].size)
    {
        SecNum = Entry;
        RelAdr = address - section[Entry].start;
        section_entry_list_Insert(SecNum, RelAdr);
        bound = section_bound_list_Insert(SecNum, RelAdr);
        if (bound != NULL) bound->begin = 1;
    }
    else
    {
        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            section_entry_list_Insert(SecNum, RelAdr);
            bound = section_bound_list_Insert(SecNum, RelAdr);
            if (bound != NULL) bound->begin = 1;
        }
        else
        {
            SecNum = Entry;
            RelAdr = address - section[Entry].start;
        }
    }

    output = section_output_list_FindEntryEqual(SecNum, RelAdr);
    if (output != NULL)
    {
        output->has_label = 1;
    }
}

void SR_disassemble_align_fixup(const fixup_data *fixup)
{
    output_data *output;

    if (section[fixup->tsec].type == ST_CODE)
    {
        output = section_output_list_FindEntryEqual(fixup->tsec, fixup->tofs);

        if (output != NULL)
        {
            output->align = 4;
        }
    }
}

int SR_disassemble_convert_cjump(char *dst, const char *modifier, uint_fast32_t address, extrn_data *extrn)
{
    char cbuf[32];
    const char *ostr, *str1;
    int length1;

    ostr = ud_insn_asm(&ud_obj);

    str1 = strchr(ostr, ' ');
    if (str1 == NULL)
    {
        fprintf(stderr, "Error: error converting cjump: %i\n", 2);
        return 1;
    }

    length1 = str1 - ostr;

    // copy first part of the string
    strncpy(dst, ostr, length1);

    // copy second part of the string
    strcpy(&(dst[length1]), modifier);

    if (extrn != NULL)
    {
        extrn->is_used = 1;

        // print label to string
        sprintf(&(dst[strlen(dst)]), " %s", extrn->proc);
    }
    else
    {
        // print label to string
        SR_get_label(cbuf, address);

        sprintf(&(dst[strlen(dst)]), " %s", cbuf);
    }

    return 0;
}

int SR_disassemble_convert_fixup(const char *ostr, char *dst, fixup_data *fixup, extrn_data *extrn, int Entry, uint_fast32_t offset, int decoded_length)
{
    char cbuf[32];
    char *str1, *str2;
    int *label_value;
    output_data *output;
    uint_fast32_t ofs;
    int length1;

    if (fixup->type == FT_SEGMENT)
    {
        sprintf(cbuf, "0x%x", 0);
    }
    else if (fixup->type == FT_IMAGEBASE)
    {
        sprintf(cbuf, "0x%x", (unsigned int)(fixup->tsec + fixup->tofs));
    }
    else if (fixup->type == FT_16BITOFS)
    {
        sprintf(cbuf, "0x%x", (unsigned int)((section[fixup->tsec].start + fixup->tofs) & 0xffff));
    }
    else
    {
        sprintf(cbuf, "0x%x", (unsigned int)(section[fixup->tsec].start + fixup->tofs));
    }

    // find substring (must be exactly one occurence)
    str1 = strstr(ostr, cbuf);
    if (str1 == NULL)
    {
        fprintf(stderr, "Error: error converting fixup: %i - %s - %s (%i: 0x%x)\n", 1, ostr, cbuf, Entry, (unsigned int)offset);

        return 1;
    }
    length1 = str1 - ostr;

    str2 = str1 + strlen(cbuf);

    if ( strstr(str2, cbuf) != NULL )
    {
        if ((decoded_length != 0) && ((fixup->type == FT_NORMAL) || (fixup->type == FT_SEGOFS32)))
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
                *fixupofs = (*fixupofs - fixup->tofs) + ~fixup->tofs;
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
                    ret = SR_disassemble_convert_fixup(ud_insn_asm(&tmpud_obj), dst, &tmpfixup, extrn, 0, 0, 0);

                    free(tmpbuf);

                    if (ret == 0) return 0;
                }
                else
                {
                    free(tmpbuf);
                }
            }
        }
        fprintf(stderr, "Two string occurences in fixup: 0x%x, %i - 0x%x\n%s\n%s\n", (unsigned int)fixup->sofs, (unsigned int)fixup->tsec, (unsigned int)fixup->tofs, cbuf, ostr);
        return 2;
    }

    if (decoded_length == 0)
    {
        // if searching for one's complement of searched data, return it to original value
        fixup->tofs = ~fixup->tofs;
    }

    // copy first part of the string
    strncpy(dst, ostr, length1);

    // locate label
    if (fixup->type != FT_IMAGEBASE)
    {
        label_value = section_label_list_FindEntryEqual(fixup->tsec, fixup->tofs);
    }
    else
    {
        label_value = NULL;
    }

    if (fixup->type == FT_SEGMENT || fixup->type == FT_IMAGEBASE)
    {
        output = NULL;
    }
    else if (label_value != NULL)
    {
        output = section_output_list_FindEntryEqualOrLower(fixup->tsec, fixup->tofs + *label_value);
    }
    else
    {
        if ( fixup->tofs < 0 )
        {
            output = section_output_list_FindEntryFirst(fixup->tsec);
        }
        else
        {
            output = section_output_list_FindEntryEqualOrLower(fixup->tsec, fixup->tofs);
        }
    }

    // set label to target address
    if (output != NULL)
    {
        if (extrn == NULL)
        {
            output->has_label = 1;
        }
        ofs = output->ofs;
    }
    else
    {
        ofs = fixup->tofs;
    }

    // print label to string
    if (extrn != NULL)
    {
        extrn->is_used = 1;

        strcpy(cbuf, extrn->proc);
    }
    else if (fixup->type == FT_SEGMENT)
    {
        sprintf(cbuf, "%s", section[fixup->tsec].name);
    }
    else if (fixup->type == FT_IMAGEBASE)
    {
        sprintf(cbuf, "0x%x + imagebase1000 - 0x1000", (unsigned int)fixup->tofs);
    }
    else
    {
        SR_get_label(cbuf, section[fixup->tsec].start + ofs);
    }

    if (ofs == fixup->tofs ||
        extrn != NULL)
    {
        sprintf(&(dst[length1]), "%s", cbuf);
    }
    else
    {
        sprintf(&(dst[length1]), "(%s + (%i))", cbuf, (int)(fixup->tofs - (int_fast32_t)ofs));
    }

    // copy second part of the string
    strcat(&(dst[length1]), str2);

    return 0;
}

int SR_disassemble_find_noret(uint_fast32_t address)
{
    uint_fast32_t SecNum, RelAdr;

    if (SR_get_section_reladr(address, &SecNum, &RelAdr))
    {
        if (section_noret_list_CanFindEntryEqual(SecNum, RelAdr))
        {
            return 1;
        }
    }

    return 0;
}

int SR_disassemble_offset_windows(unsigned int Entry, uint_fast32_t offset)
{
    output_data *output;
    fixup_data *fixup, *fixup2;
    extrn_data *extrn, *extrn2;
    int finished, decoded_length, ret;
    char cLabel[32];
    char cResult[128];
    char cResPart[128];

    ud_set_input_buffer(&ud_obj, &(section[Entry].adr[offset]), section[Entry].size - offset);
    ud_set_pc(&ud_obj, section[Entry].start + offset);

    finished = 0;

    while (!finished)
    {
        output = section_output_list_FindEntryEqual(Entry, offset);

        if (output == NULL)
        {
            fprintf(stderr, "Error: output not found - %i - 0x%x\n", Entry, (unsigned int)offset);

            return 1;
        }

        if (output->type > 0)
        {
            finished = 1;
            break;
        }

        decoded_length = ud_disassemble(&ud_obj);

        if (decoded_length == 0)
        {
            finished = 2;
            break;
        }

#ifdef DISPLAY_DISASSEMBLY
        printf("loc_%X: %s\n", section[Entry].start + offset, ud_insn_asm(&ud_obj));
#endif


        fixup2 = NULL;
        fixup = section_fixup_list_FindEntryEqualOrHigher(Entry, offset);

        if (fixup != NULL)
        {
            if (fixup->sofs >= offset + decoded_length) fixup = NULL;
            else
            {
                int fixuplen, fixuplen2;

                fixuplen = (fixup->type == FT_NORMAL || fixup->type == FT_SELFREL || fixup->type == FT_IMAGEBASE)?4:2;

                if (fixup->sofs > offset + decoded_length - fixuplen)
                {
                    fprintf(stderr, "Error: decoding fixup mismatch - %i - %i\n", Entry, (unsigned int)offset);

                    return 2;
                }

                if (fixup->sofs != offset + decoded_length - fixuplen)
                {
                    fixup2 = section_fixup_list_FindEntryHigher(Entry, fixup->sofs);

                    if (fixup2 != NULL)
                    {
                        if (fixup2->sofs >= offset + decoded_length) fixup2 = NULL;
                        else
                        {
                            fixuplen2 = (fixup2->type == FT_NORMAL || fixup2->type == FT_SELFREL || fixup2->type == FT_IMAGEBASE)?4:2;

                            if (fixup2->sofs > offset + decoded_length - fixuplen2)
                            {
                                fprintf(stderr, "Error: decoding fixup mismatch - %i - %i\n", Entry, (unsigned int)offset);

                                return 2;
                            }
                        }
                    }

                }
            }
        }

        output = SR_disassemble_offset_init_output(Entry, offset, decoded_length);

        switch (ud_obj.mnemonic)
        {
            case UD_Icall:
                if (fixup == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    ud_obj.operand[0].size == 32
                   )
                {
                    uint_fast32_t address;

                    address = ud_obj.operand[0].lval.sdword + (uint_fast32_t) ud_obj.pc;

                    SR_disassemble_add_address(Entry, address);

                    if ( SR_disassemble_find_noret(address) )
                    {
                        finished = 1;
                    }

                    SR_get_label(cLabel, address);
                    sprintf(cResult, "call %s", cLabel);

                    output->str = strdup(cResult);
#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                else if (fixup != NULL &&
                         ud_obj.operand[0].type == UD_OP_MEM)
                {
                    SR_disassemble_align_fixup(fixup);

                    extrn = section_extrn_list_FindEntryEqual(fixup->tsec, fixup->tofs);

                    ret = SR_disassemble_convert_fixup(ud_insn_asm(&ud_obj), cResult, fixup, extrn, Entry, offset, decoded_length);

                    if (ret) return ret;

                    output->str = strdup(cResult);
#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                else if (fixup == NULL &&
                         (ud_obj.operand[0].type == UD_OP_MEM || ud_obj.operand[0].type == UD_OP_REG))
                {
                    output->str = strdup(ud_insn_asm(&ud_obj));
                }
                else
                {
                    printf("fixup: %i\n", (int)(intptr_t)fixup);
                    printf("op type: %i - %i - %i\n", ud_obj.operand[0].type, ud_obj.operand[1].type, ud_obj.operand[2].type);
                    printf("op size: %i - %i - %i\n", ud_obj.operand[0].size, ud_obj.operand[1].size, ud_obj.operand[2].size);

                    return -10;
                }

                break;

            case UD_Iiretw:
            case UD_Iiretd:
            case UD_Iiretq:
            case UD_Iret:
            case UD_Iretf:

                output->str = strdup(ud_insn_asm(&ud_obj));

                finished = 1;

                break;

            case UD_Ijcxz:
            case UD_Ijecxz:
            case UD_Ijrcxz:
            case UD_Ijo:
            case UD_Ijno:
            case UD_Ijb:
            //case UD_Ijnb:
            case UD_Ijz:
            case UD_Ijnz:
            case UD_Ijbe:
            //case UD_Ijnbe:
            case UD_Ijs:
            case UD_Ijns:
            case UD_Ijp:
            case UD_Ijnp:
            case UD_Ijl:
            //case UD_Ijnl:
            case UD_Ijle:
            //case UD_Ijnle:
            case UD_Ija:
            case UD_Ijae:
            case UD_Ijg:
            case UD_Ijge:
            //case UD_Ijc:
            //case UD_Ijnc:
                if (fixup == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    (ud_obj.operand[0].size == 32 || ud_obj.operand[0].size == 8)
                   )
                {
                    uint_fast32_t address;

                    if (ud_obj.operand[0].size == 32)
                    {
                        address = ud_obj.operand[0].lval.sdword + (uint_fast32_t) ud_obj.pc;
                    }
                    else
                    {
                        address = ud_obj.operand[0].lval.sbyte + (uint_fast32_t) ud_obj.pc;
                    }

                    SR_disassemble_add_address(Entry, address);

                    ret = SR_disassemble_convert_cjump(cResult, ((ud_obj.operand[0].size == 32)?" near":""), address, NULL);

                    if (ret) return ret;

                    output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                else
                {
                    printf("fixup: %i\n", (int)(intptr_t)fixup);
                    printf("op type: %i - %i - %i\n", ud_obj.operand[0].type, ud_obj.operand[1].type, ud_obj.operand[2].type);
                    printf("op size: %i - %i - %i\n", ud_obj.operand[0].size, ud_obj.operand[1].size, ud_obj.operand[2].size);
                    return -20;
                }

                break;

            case UD_Ijmp:
                if (fixup == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    (ud_obj.operand[0].size == 32 || ud_obj.operand[0].size == 8)
                   )
                {
                    uint_fast32_t address;

                    if (ud_obj.operand[0].size == 32)
                    {
                        address = ud_obj.operand[0].lval.sdword + (uint_fast32_t) ud_obj.pc;
                    }
                    else
                    {
                        address = ud_obj.operand[0].lval.sbyte + (uint_fast32_t) ud_obj.pc;
                    }

                    SR_disassemble_add_address(Entry, address);

                    SR_get_label(cLabel, address);
                    sprintf(cResult, "jmp %s%s", ((ud_obj.operand[0].size == 32)?"":"short "), cLabel);

                    output->str = strdup(cResult);
#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                else if (fixup != NULL &&
                         ud_obj.operand[0].type == UD_OP_MEM)
                {
                    SR_disassemble_align_fixup(fixup);

                    extrn = section_extrn_list_FindEntryEqual(fixup->tsec, fixup->tofs);

                    ret = SR_disassemble_convert_fixup(ud_insn_asm(&ud_obj), cResult, fixup, extrn, Entry, offset, decoded_length);

                    if (ret) return ret;

                    output->str = strdup(cResult);
#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                else if (fixup == NULL &&
                         (ud_obj.operand[0].type == UD_OP_MEM || ud_obj.operand[0].type == UD_OP_REG))
                {
                    output->str = strdup(ud_insn_asm(&ud_obj));
                }
                else if (fixup != NULL &&
                         fixup2 != NULL &&
                         fixup->type == FT_NORMAL &&
                         fixup2->type == FT_SEGMENT &&
                         ud_obj.operand[0].type == UD_OP_PTR)
                {
                    extrn = section_extrn_list_FindEntryEqual(fixup->tsec, fixup->tofs);

                    ret = SR_disassemble_convert_fixup(ud_insn_asm(&ud_obj), cResult, fixup, extrn, Entry, offset, decoded_length);

                    if (ret) return ret;

                    {
                        int index;

                        cResult[4] = 'f';
                        cResult[5] = 'a';
                        cResult[6] = 'r';
                        cResult[7] = ' ';

                        index = 8;
                        while ((cResult[index] != ':') && (cResult[index] != 0))
                        {
                            cResult[index] = ' ';
                            index++;
                        }
                        if (cResult[index] == ':') cResult[index] = ' ';
                    }

                    output->str = strdup(cResult);
#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                else
                {
                    printf("fixup: %i\n", (int)(intptr_t)fixup);
                    printf("op type: %i - %i - %i\n", ud_obj.operand[0].type, ud_obj.operand[1].type, ud_obj.operand[2].type);
                    printf("op size: %i - %i - %i\n", ud_obj.operand[0].size, ud_obj.operand[1].size, ud_obj.operand[2].size);

                    return -30;
                }

                finished = 1;

                break;

            case UD_Iloop:
            case UD_Iloope:
            //case UD_Iloopn:
            case UD_Iloopne:
            //case UD_Iloopnz:
            //case UD_Iloopz:
                if (fixup == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    ud_obj.operand[0].size == 8
                   )
                {
                    uint_fast32_t address;

                    address = ud_obj.operand[0].lval.sbyte + (uint_fast32_t) ud_obj.pc;

                    SR_disassemble_add_address(Entry, address);

                    ret = SR_disassemble_convert_cjump(cResult, "", address, NULL);

                    if (ret) return ret;

                    output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                else
                {
                    printf("fixup: %i\n", (int)(intptr_t)fixup);
                    printf("op type: %i - %i - %i\n", ud_obj.operand[0].type, ud_obj.operand[1].type, ud_obj.operand[2].type);
                    printf("op size: %i - %i - %i\n", ud_obj.operand[0].size, ud_obj.operand[1].size, ud_obj.operand[2].size);
                    return -40;
                }
                break;

            case UD_Iinvalid:
                printf("decoded length: %i (%i - 0x%x)\n", decoded_length, Entry, (unsigned int)offset);
                return 3;
                break;

            default:

                if (fixup != NULL)
                {
                    if (fixup->type == FT_SELFREL)
                    {
                        return -50;
                    }
                    else
                    {
                        int fixuplen8;

                        fixuplen8 = (fixup->type == FT_NORMAL || fixup->type == FT_SELFREL || fixup->type == FT_IMAGEBASE)?32:16;

                        if (((ud_obj.operand[0].type == UD_OP_IMM && ud_obj.operand[0].size == fixuplen8) || ud_obj.operand[0].type == UD_OP_MEM) ||
                            ((ud_obj.operand[1].type == UD_OP_IMM && ud_obj.operand[1].size == fixuplen8) || ud_obj.operand[1].type == UD_OP_MEM))
                        {
                            if (fixup->type != FT_IMAGEBASE)
                            {
                                extrn = section_extrn_list_FindEntryEqual(fixup->tsec, fixup->tofs);
                            }
                            else
                            {
                                extrn = NULL;
                            }
                            if (fixup2 != NULL)
                            {
                                if (fixup2->type != FT_IMAGEBASE)
                                {
                                    extrn2 = section_extrn_list_FindEntryEqual(fixup2->tsec, fixup2->tofs);
                                }
                                else
                                {
                                    extrn2 = NULL;
                                }

                                ret = SR_disassemble_convert_fixup(ud_insn_asm(&ud_obj), cResPart, fixup, extrn, Entry, offset, decoded_length);

                                if (ret) return ret;

                                ret = SR_disassemble_convert_fixup(cResPart, cResult, fixup2, extrn2, Entry, offset, decoded_length);

                                if (ret) return ret;
                            }
                            else
                            {
                                ret = SR_disassemble_convert_fixup(ud_insn_asm(&ud_obj), cResult, fixup, extrn, Entry, offset, decoded_length);

                                if (ret) return ret;
                            }


                            output->str = strdup(cResult);
#ifdef DISPLAY_DISASSEMBLY
                            printf("\t%s\n", output->str);
#endif
                        }
                        else
                        {
                            printf("fixup: %i - 0x%x - 0x%x\n", (unsigned int)fixup->tsec, (unsigned int)fixup->tofs, (unsigned int)(section[fixup->tsec].start + fixup->tofs));
                            printf("op type: %i - %i - %i\n", ud_obj.operand[0].type, ud_obj.operand[1].type, ud_obj.operand[2].type);
                            printf("op size: %i - %i - %i\n", ud_obj.operand[0].size, ud_obj.operand[1].size, ud_obj.operand[2].size);
                            return -51;
                        }

                    }
                }
                else
                {
                    if (ud_obj.mnemonic == UD_Ifld &&
                        ud_obj.operand[0].type == UD_OP_REG &&
                        ud_obj.operand[1].type == UD_OP_REG &&
                        ud_obj.operand[0].base == UD_R_ST0 &&
                        ud_obj.operand[1].base >= UD_R_ST0 &&
                        ud_obj.operand[1].base <= UD_R_ST7)
                    {
                        output->str = strdup("fld st0");
                        output->str[6] = '0' + ud_obj.operand[1].base - UD_R_ST0;
#ifdef DISPLAY_DISASSEMBLY
                        printf("\t%s\n", output->str);
#endif
                    }
                    else
                    if (ud_obj.mnemonic == UD_Ilsl &&
                        ud_obj.operand[0].type == UD_OP_REG &&
                        ud_obj.operand[1].type == UD_OP_REG &&
                        ud_obj.operand[0].base == UD_R_EAX &&
                        ud_obj.operand[1].base == UD_R_AX)
                    {
                        output->str = strdup("lsl eax, eax");
#ifdef DISPLAY_DISASSEMBLY
                        printf("\t%s\n", output->str);
#endif
                    }
                    /*else
                    if (ud_obj.mnemonic == UD_Ifldlpi)
                    {
                        output->str = strdup("fldpi");
#ifdef DISPLAY_DISASSEMBLY
                        printf("\t%s\n", output->str);
#endif
                    }*/
                    else
                    {
                        output->str = strdup(ud_insn_asm(&ud_obj));
/*						if (ud_obj.mnemonic == UD_Imov && offset == 0xc25)
                        {
                            printf("\t%s\n", output->str);
                            printf("op type: %i - %i - %i\n", ud_obj.operand[0].type, ud_obj.operand[1].type, ud_obj.operand[2].type);
                            printf("op size: %i - %i - %i\n", ud_obj.operand[0].size, ud_obj.operand[1].size, ud_obj.operand[2].size);
                        }*/

                    }
                }

                break;
        }

        offset += decoded_length;
    }

    return (finished == 2)?4:0;
}

