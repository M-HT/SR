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

#if (OUTPUT_TYPE == OUT_LLASM)
#include "udis86_dep.h"
#else
// from udis86
extern const char* ud_reg_tab[];
#endif

int SR_disassemble_remove_segment(const char *ostr, char *dst, uint8_t pfx_seg)
{
    char *seg, *str1, *str2;
    int length1;

    switch (pfx_seg)
    {
        case UD_R_ES:
            seg = "es:";
            break;
        case UD_R_CS:
            seg = "cs:";
            break;
        case UD_R_SS:
            seg = "ss:";
            break;
        case UD_R_DS:
            seg = "ds:";
            break;
        case UD_R_FS:
            seg = "fs:";
            return -101;
            break;
        case UD_R_GS:
            seg = "gs:";
            return -102;
            break;
        default:
            seg = NULL;
            return -100;
    }

    if (seg != NULL)
    {
        str1 = strstr(ostr, seg);
    }
    else
    {
        str1 = NULL;
    }

    if (str1 == NULL)
    {
        if (seg != NULL && ostr[0] == seg[0] && ostr[1] == seg[1] && ostr[2] == ' ')
        {
            strcpy(dst, &(ostr[3]));
            return 0;
        }
        else
        {
            //strcpy(dst, ostr);
            return -105;
        }
    }

    length1 = str1 - ostr;
    str2 = str1 + 3;

    // copy first part of the string
    strncpy(dst, ostr, length1);

    // copy second part of the string
    strcpy(&(dst[length1]), str2);

    return 0;
}

int SR_disassemble_remove_segreg2(const char *ostr, char *dst, const struct ud_operand *op)
{
    char *str1, *str2;
    int length1, length2;

    str1 = strchr(ostr, ' ');
    str2 = strchr(ostr, ',');

    if (str1 == NULL || str2 == NULL)
    {
        strcpy(dst, ostr);

        return 0;
    }

    length1 = str1 - ostr;
    length2 = str2 - str1;

    if (op->type == UD_OP_REG)
    {
        strcpy(dst, "xor");
        strncat(dst, str1, length2 + 1);
        strncat(dst, str1, length2);
    }
    else // UD_OP_MEM
    {
        strncpy(dst, ostr, length1 + 1);
        strcpy(&(dst[length1 + 1]), "word");
        strncat(dst, str1, length2 + 1);
        strcat(dst, " 0");
    }

    return 0;

}

int SR_disassemble_convert_farcall(const char *ostr, char *dst)
{
    char *str1, *str2;
    int length1;

    str1 = strstr(ostr, " far ");

    if (str1 == NULL)
    {
        strcpy(dst, ostr);
        return 0;
    }

    length1 = str1 - ostr;
    str2 = str1 + 5;


    // emulate far call
    strcpy(dst, "push byte 0\n");

    // sopy first part of the string
    strncat(dst, ostr, length1);

    // change far call to near call
    strcat(dst, " near ");

    // copy second part of the string
    strcat(dst, str2);

    return 0;
}

extrn_data *SR_disassemble_find_proc(unsigned int Entry, uint_fast32_t address)
{
    extrn_data *extrn;
    uint_fast32_t SecNum, RelAdr;

    extrn = NULL;

    if (address >= section[Entry].start &&
        address <= section[Entry].start + section[Entry].size)
    {
        extrn = section_extrn_list_FindEntryEqual(Entry, (address - section[Entry].start));
    }
    else
    {
        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            extrn = section_extrn_list_FindEntryEqual(SecNum, RelAdr);
        }
    }

    if (extrn != NULL)
    {
        extrn->is_used = 1;
    }

    return extrn;
}

#if (OUTPUT_TYPE == OUT_LLASM)
static int SR_disassemble_change_cjump_iflags(unsigned int Entry, uint_fast32_t offset, int prev_cjump_length, enum ud_mnemonic_code mnemonic)
{
    bound_data *iflags;
    uint_fast32_t flags;

    flags = flags_needed[mnemonic];

    iflags = section_iflags_list_FindEntryEqual(Entry, offset);
    if (iflags == NULL)
    {
        section_iflags_list_Insert(Entry, offset, 0, flags);
    }
    else
    {
        if (iflags->end == 0)
        {
            iflags->end = flags;
        }
        else
        {
            if (iflags->end != (iflags->end | flags))
            {
                fprintf(stderr, "Error: iflags->end conflict - %i - %i\n", Entry, (unsigned int)offset);

                return 0;
            }
        }
    }

    iflags = section_iflags_list_FindEntryEqual(Entry, offset - prev_cjump_length);
    if (iflags == NULL)
    {
        section_iflags_list_Insert(Entry, offset - prev_cjump_length, flags, 0);
    }
    else
    {
        if (iflags->begin == 0)
        {
            iflags->begin = flags;
        }
        else
        {
            if (iflags->begin != (iflags->begin | flags))
            {
                fprintf(stderr, "Error: iflags->begin conflict - %i - %i\n", Entry, (unsigned int)offset);

                return 0;
            }
        }
    }

    return 1;
}
#endif

int SR_disassemble_offset_win32(unsigned int Entry, uint_fast32_t offset)
{
    output_data *output;
    fixup_data *fixup, *fixup2;
    extrn_data *extrn, *extrn2;
    bound_data *bound;
    int finished, decoded_length, ret;
    char cLabel[32];
    char cResult[128];
    char cResPart[128];
#if (OUTPUT_TYPE == OUT_LLASM)
    int prev_cjump_length, cur_cjump_length;

    prev_cjump_length = 0;
    cur_cjump_length = 0;
#endif

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
            /*if (output->str != NULL)
            {
                printf("loc_%X: %s", section[Entry].start + offset, output->str);
            }
            else
            {
                printf("loc_%X: unknown string\n", section[Entry].start + offset);
            }*/
            finished = 1;
            break;
        }

        decoded_length = ud_disassemble(&ud_obj);

        if (decoded_length == 0)
        {
            finished = 2;
            break;
        }

#if (OUTPUT_TYPE == OUT_LLASM)
        prev_cjump_length = cur_cjump_length;
        cur_cjump_length = 0;
#endif

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
                if (fixup->sofs > offset + decoded_length - 4)
                {
                    fprintf(stderr, "Error: decoding fixup mismatch - %i - %i\n", Entry, (unsigned int)offset);

                    return 2;
                }

                if (fixup->sofs != offset + decoded_length - 4)
                {
                    fixup2 = section_fixup_list_FindEntryHigher(Entry, fixup->sofs);

                    if (fixup2 != NULL)
                    {
                        if (fixup2->sofs >= offset + decoded_length) fixup2 = NULL;
                        else
                        {
                            if (fixup2->sofs > offset + decoded_length - 4)
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
                bound = section_bound_list_Insert(Entry, offset);
                if (bound != NULL) bound->end = 1;

                if (fixup == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    ud_obj.operand[0].size == 32
                   )
                {
                    uint_fast32_t address;

                    address = ud_obj.operand[0].lval.sdword + (uint_fast32_t) ud_obj.pc;

                    extrn = SR_disassemble_find_proc(Entry, address);

                    if (extrn != NULL)
                    {
                        if (extrn->altaction == NULL)
                        {
                            //sprintf(cResult, "%scall %s", "ACTION_CALL\n", extrn->proc);
                            sprintf(cResult, "call %s", extrn->proc);
                        }
                        else
                        {
                            sprintf(cResult, "%s", extrn->altaction);
                        }
                    }
                    else
                    {
                        SR_disassemble_add_address(Entry, address);

                        if ( SR_disassemble_find_noret(address) )
                        {
                            finished = 1;
                        }

                        SR_get_label(cLabel, address);
                        //sprintf(cResult, "%scall %s", "ACTION_CALL\n", cLabel);
                        sprintf(cResult, "call %s", cLabel);
                    }

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

                    if (extrn != NULL && extrn->is_import)
                    {
                        if (ud_obj.operand[0].base == UD_NONE &&
                            ud_obj.operand[0].index == UD_NONE &&
                            (ud_obj.operand[0].size == 32 || ud_obj.operand[0].size == 0)
                           )
                        {
                            extrn->is_used = 1;
                            sprintf(cResult, "call %s", extrn->proc);
                            output->str = strdup(cResult);
                        }
                        else
                        {
                            printf("unhandled import: %s\n", extrn->proc);

                            return -11;
                        }
                    }
                    else
                    {
                        //strcpy(cResPart, "ACTION_CALL\n");
                        cResPart[0] = 0;

                        ret = SR_disassemble_convert_fixup(ud_insn_asm(&ud_obj), &(cResPart[strlen(cResPart)]), fixup, extrn, Entry, offset, decoded_length);

                        if (ret) return ret;

                        if (ud_obj.pfx_seg != UD_NONE)
                        {
                            ret = SR_disassemble_remove_segment(cResPart, cResult, ud_obj.pfx_seg);

                            if (ret) return ret;

                            if (ud_obj.br_far)
                            {
                                ret = SR_disassemble_convert_farcall(cResult, cResPart);

                                if (ret) return ret;

                                output->str = strdup(cResPart);
                            }
                            else
                            {
                                output->str = strdup(cResult);
                            }
                        }
                        else
                        {
                            if (ud_obj.br_far)
                            {
                                ret = SR_disassemble_convert_farcall(cResPart, cResult);

                                if (ret) return ret;

                                output->str = strdup(cResult);
                            }
                            else
                            {
                                output->str = strdup(cResPart);
                            }
                        }
                    }


#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                else if (fixup == NULL &&
                         (ud_obj.operand[0].type == UD_OP_MEM || ud_obj.operand[0].type == UD_OP_REG))
                {
                    if (ud_obj.pfx_seg != UD_NONE)
                    {
                        //strcpy(cResPart, "ACTION_CALL\n");
                        cResPart[0] = 0;
                        ret = SR_disassemble_remove_segment(ud_insn_asm(&ud_obj), &(cResPart[strlen(cResPart)]), ud_obj.pfx_seg);

                        if (ret) return ret;

                        if (ud_obj.operand[0].type == UD_OP_MEM &&
                            ud_obj.br_far)
                        {
                            ret = SR_disassemble_convert_farcall(cResPart, cResult);

                            if (ret) return ret;

                            output->str = strdup(cResult);
                        }
                        else
                        {
                            output->str = strdup(cResPart);
                        }

#ifdef DISPLAY_DISASSEMBLY
                        printf("\t%s\n", output->str);
#endif
                    }
                    else
                    {
                        if (ud_obj.operand[0].type == UD_OP_MEM &&
                            ud_obj.br_far)
                        {
                            //strcpy(cResPart, "ACTION_CALL\n");
                            cResPart[0] = 0;
                            ret = SR_disassemble_convert_farcall(ud_insn_asm(&ud_obj), &(cResPart[strlen(cResPart)]));

                            if (ret) return ret;

                            output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                            printf("\t%s\n", output->str);
#endif
                        }
                        else
                        {
                            //sprintf(cResult, "%s%s","ACTION_CALL\n", ud_insn_asm(&ud_obj));
                            sprintf(cResult, "%s", ud_insn_asm(&ud_obj));
                            output->str = strdup(cResult);
                        }
                    }
                }
                else
                {
                    printf("fixup: %i\n", (int)(intptr_t)fixup);
                    printf("op type: %i - %i - %i\n", ud_obj.operand[0].type, ud_obj.operand[1].type, ud_obj.operand[2].type);
                    printf("op size: %i - %i - %i\n", ud_obj.operand[0].size, ud_obj.operand[1].size, ud_obj.operand[2].size);

                    return -10;
                }

                if (!finished)
                {
                    bound = section_bound_list_Insert(Entry, offset + decoded_length);
                    if (bound != NULL) bound->begin = 1;
                }


                break;

            case UD_Iiretw:
            case UD_Iiretd:
            case UD_Iiretq:
                bound = section_bound_list_Insert(Entry, offset);
                if (bound != NULL) bound->end = 1;

                // this won't work for retf 0xfffc or higher
                strcpy(cResult, "push dword [esp+8]\npopfd\nretn 8");

                output->str = strdup(cResult);

                finished = 1;

#ifdef DISPLAY_DISASSEMBLY
                printf("\t%s\n", output->str);
#endif
                break;

            case UD_Iret:
                bound = section_bound_list_Insert(Entry, offset);
                if (bound != NULL) bound->end = 1;

                output->str = strdup(ud_insn_asm(&ud_obj));

                finished = 1;

                break;

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
            #if (OUTPUT_TYPE == OUT_LLASM)
                cur_cjump_length = decoded_length;

                if (prev_cjump_length)
                {
                    if (!SR_disassemble_change_cjump_iflags(Entry, offset, prev_cjump_length, ud_obj.mnemonic))
                    {
                        return 3;
                    }
                }
            #endif
                // fallthrough
            case UD_Ijcxz:
            case UD_Ijecxz:
            case UD_Ijrcxz:
            #if (OUTPUT_TYPE == OUT_LLASM)
                bound = section_bound_list_Insert(Entry, offset);
                if (bound != NULL) bound->end = 1;
                bound = section_bound_list_Insert(Entry, offset + decoded_length);
                if (bound != NULL) bound->begin = 1;
            #endif

                if (fixup == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    (ud_obj.operand[0].size == 32 || ud_obj.operand[0].size == 8)
                   )
                {
                    uint_fast32_t address;
                    //int backward;

                    if (ud_obj.operand[0].size == 32)
                    {
                        //backward = (ud_obj.operand[0].lval.sdword < 0)?1:0;
                        address = ud_obj.operand[0].lval.sdword + (uint_fast32_t) ud_obj.pc;
                    }
                    else
                    {
                        //backward = (ud_obj.operand[0].lval.sbyte < 0)?1:0;
                        address = ud_obj.operand[0].lval.sbyte + (uint_fast32_t) ud_obj.pc;
                    }

                    extrn = SR_disassemble_find_proc(Entry, address);

                    if (extrn == NULL)
                    {
                        SR_disassemble_add_address(Entry, address);
                    }

                    //if (backward)
                    //{
                    //    strcpy(cResult, "ACTION_CONDITIONAL_JUMP_BACKWARD\n");
                    //}
                    //else
                    //{
                    //    strcpy(cResult, "ACTION_CONDITIONAL_JUMP_FORWARD\n");
                    //}
                    cResult[0] = 0;

                    ret = SR_disassemble_convert_cjump(&(cResult[strlen(cResult)]), ((ud_obj.operand[0].size == 32)?" near":""), address, extrn);

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
                bound = section_bound_list_Insert(Entry, offset);
                if (bound != NULL) bound->end = 1;

                if (fixup == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    (ud_obj.operand[0].size == 32 || ud_obj.operand[0].size == 8)
                   )
                {
                    uint_fast32_t address;
                    //int backward, shortjump;

                    if (ud_obj.operand[0].size == 32)
                    {
                        //shortjump = 0;
                        //backward = (ud_obj.operand[0].lval.sdword < 0)?1:0;
                        address = ud_obj.operand[0].lval.sdword + (uint_fast32_t) ud_obj.pc;
                    }
                    else
                    {
                        //shortjump = 1;
                        //backward = (ud_obj.operand[0].lval.sbyte < 0)?1:0;
                        address = ud_obj.operand[0].lval.sbyte + (uint_fast32_t) ud_obj.pc;
                    }

                    extrn = SR_disassemble_find_proc(Entry, address);

                    //if (shortjump)
                    //{
                    //    if (backward)
                    //    {
                    //        strcpy(cResult, "ACTION_UNCONDITIONAL_SHORT_JUMP_BACKWARD\n");
                    //    }
                    //    else
                    //    {
                    //        strcpy(cResult, "ACTION_UNCONDITIONAL_SHORT_JUMP_FORWARD\n");
                    //    }
                    //}
                    //else
                    //{
                    //    strcpy(cResult, "ACTION_UNCONDITIONAL_JUMP\n");
                    //}
                    cResult[0] = 0;

                    if (extrn != NULL)
                    {
                        sprintf(&(cResult[strlen(cResult)]), "jmp %s", extrn->proc);
                    }
                    else
                    {
                        SR_disassemble_add_address(Entry, address);

                        SR_get_label(cLabel, address);
                        sprintf(&(cResult[strlen(cResult)]), "jmp %s%s", ((ud_obj.operand[0].size == 32)?"":"short "), cLabel);
                    }


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

                    if (extrn != NULL && extrn->is_import)
                    {
                        if (ud_obj.operand[0].base == UD_NONE &&
                            ud_obj.operand[0].index == UD_NONE &&
                            ud_obj.operand[0].size == 32
                           )
                        {
                            extrn->is_used = 1;
                            sprintf(cResult, "jmp %s", extrn->proc);
                            output->str = strdup(cResult);
                        }
                        else
                        {
                            printf("unhandled import: %s\n", extrn->proc);

                            return -31;
                        }
                    }
                    else
                    {
                        //strcpy(cResPart, "ACTION_UNCONDITIONAL_JUMP\n");
                        cResPart[0] = 0;
                        ret = SR_disassemble_convert_fixup(ud_insn_asm(&ud_obj), &(cResPart[strlen(cResPart)]), fixup, extrn, Entry, offset, decoded_length);

                        if (ret) return ret;

                        if (ud_obj.pfx_seg != UD_NONE)
                        {
                            ret = SR_disassemble_remove_segment(cResPart, cResult, ud_obj.pfx_seg);

                            if (ret) return ret;

                            output->str = strdup(cResult);
                        }
                        else
                        {
                            output->str = strdup(cResPart);
                        }
                    }

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                else if (fixup == NULL &&
                         (ud_obj.operand[0].type == UD_OP_MEM || ud_obj.operand[0].type == UD_OP_REG))
                {
                    if (ud_obj.pfx_seg != UD_NONE)
                    {
                        //strcpy(cResult, "ACTION_UNCONDITIONAL_JUMP\n");
                        cResult[0] = 0;
                        ret = SR_disassemble_remove_segment(ud_insn_asm(&ud_obj), &(cResult[strlen(cResult)]), ud_obj.pfx_seg);

                        if (ret) return ret;

                        output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                        printf("\t%s\n", output->str);
#endif
                    }
                    else
                    {
                        //sprintf(cResult, "%s%s","ACTION_UNCONDITIONAL_JUMP\n", ud_insn_asm(&ud_obj));
                        sprintf(cResult, "%s", ud_insn_asm(&ud_obj));
                        output->str = strdup(cResult);
                    }
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
            #if (OUTPUT_TYPE == OUT_LLASM)
                bound = section_bound_list_Insert(Entry, offset);
                if (bound != NULL) bound->end = 1;
                bound = section_bound_list_Insert(Entry, offset + decoded_length);
                if (bound != NULL) bound->begin = 1;
            #endif

                if (fixup == NULL &&
                    ud_obj.operand[0].type == UD_OP_JIMM &&
                    ud_obj.operand[0].size == 8
                   )
                {
                    uint_fast32_t address;
                    //int backward;

                    //backward = (ud_obj.operand[0].lval.sbyte < 0)?1:0;
                    address = ud_obj.operand[0].lval.sbyte + (uint_fast32_t) ud_obj.pc;

                    extrn = SR_disassemble_find_proc(Entry, address);

                    if (extrn == NULL)
                    {
                        SR_disassemble_add_address(Entry, address);
                    }

                    //if (backward)
                    //{
                    //    strcpy(cResult, "ACTION_LOOP_BACKWARD\n");
                    //}
                    //else
                    //{
                    //    strcpy(cResult, "ACTION_LOOP_FORWARD\n");
                    //}
                    cResult[0] = 0;

                    ret = SR_disassemble_convert_cjump(&(cResult[strlen(cResult)]), "", address, extrn);

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

            case UD_Icli:
                    output->str = strdup("CLI");

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                break;

            case UD_Isti:
                    output->str = strdup("STI");

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                break;

            case UD_Iint:
            case UD_Iint1:
            case UD_Iint3:
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

                        if ((intno == 3) || (intno == 6))
                        {
                            sprintf(cResult, "%s", ud_insn_asm(&ud_obj));
                        }
                        else
                        {
                            sprintf(cResult, "push dword 0x%x\ncall x86_int", intno);
                        }
                    }

                    output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                break;

            case UD_Iinto:
                    sprintf(cResult, "jno loc_%X_1\npush byte 4\ncall x86_int\nloc_%X_1", (unsigned int)(section[Entry].start + offset), (unsigned int)(section[Entry].start + offset));

                    output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                break;

            case UD_Iretf:
                bound = section_bound_list_Insert(Entry, offset);
                if (bound != NULL) bound->end = 1;

                // this won't work for retf 0xfffc or higher
                sprintf(cResult, "ret 0x%x", (ud_obj.operand[0].type == UD_NONE)?4:((uint32_t) ud_obj.operand[0].lval.uword + 4) );

                output->str = strdup(cResult);

                finished = 1;

#ifdef DISPLAY_DISASSEMBLY
                printf("\t%s\n", output->str);
#endif
                break;

            case UD_Iin:
                if (ud_obj.operand[1].type == UD_OP_REG)
                {
                    //sprintf(cResult, "call x86_in_%s_dx", (ud_obj.operand[0].base == UD_R_AL)?("al"):( (ud_obj.operand[0].base == UD_R_AX)?("ax"):("eax") ) );
                    sprintf(cResult, "call x86_in_%s_dx", ud_reg_tab[ud_obj.operand[0].base - UD_R_AL] );

                }
                else
                {
                    //sprintf(cResult, "push dword 0x%x\ncall x86_in_%s_imm", ud_obj.operand[1].lval.ubyte, (ud_obj.operand[0].base == UD_R_AL)?("al"):( (ud_obj.operand[0].base == UD_R_AX)?("ax"):("eax") ) );
                    sprintf(cResult, "push dword 0x%x\ncall x86_in_%s_imm", ud_obj.operand[1].lval.ubyte, ud_reg_tab[ud_obj.operand[0].base - UD_R_AL] );
                }

                output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                printf("\t%s\n", output->str);
#endif
                break;

            case UD_Iout:
                if (ud_obj.operand[0].type == UD_OP_REG)
                {
                    //sprintf(cResult, "call x86_out_dx_%s", (ud_obj.operand[1].base == UD_R_AL)?("al"):( (ud_obj.operand[1].base == UD_R_AX)?("ax"):("eax") ) );
                    sprintf(cResult, "call x86_out_dx_%s", ud_reg_tab[ud_obj.operand[1].base - UD_R_AL] );
                }
                else
                {
                    //sprintf(cResult, "push dword 0x%x\ncall x86_out_imm_%s", ud_obj.operand[0].lval.ubyte, (ud_obj.operand[1].base == UD_R_AL)?("al"):( (ud_obj.operand[1].base == UD_R_AX)?("ax"):("eax") ) );
                    sprintf(cResult, "push dword 0x%x\ncall x86_out_imm_%s", ud_obj.operand[0].lval.ubyte, ud_reg_tab[ud_obj.operand[1].base - UD_R_AL] );
                }

                output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                printf("\t%s\n", output->str);
#endif
                break;

            case UD_Ioutsb:
                {
                    if (ud_obj.pfx_rep || ud_obj.pfx_repe || ud_obj.pfx_repne)
                    {
                        char *tmpstr;

                        tmpstr = (char *) malloc(8192);
                        if (tmpstr != NULL)
                        {
                            *tmpstr = 0;
                            sprintf(cResPart, ";rep outsb\n"); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "pushfd\n"); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "or ecx, ecx\n"); strcat(tmpstr, cResPart);
                            sprintf(cResPart, "jz loc_%X_restore_flags\n", (unsigned int)(section[Entry].start + offset)); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "push eax\n"); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "loc_%X_loop:\n", (unsigned int)(section[Entry].start + offset)); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "lodsb\n"); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "call x86_out_dx_al\n"); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "dec ecx\n"); strcat(tmpstr, cResPart);
                            sprintf(cResPart, "jnz loc_%X_loop\n", (unsigned int)(section[Entry].start + offset)); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "pop eax\n"); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "loc_%X_restore_flags:\n", (unsigned int)(section[Entry].start + offset)); strcat(tmpstr, cResPart);
                            sprintf(cResPart, "popfd\n"); strcat(tmpstr, cResPart);

                            sprintf(cResPart, "loc_%X_after_outsb:\n", (unsigned int)(section[Entry].start + offset)); strcat(tmpstr, cResPart);

                            output->str = strdup(tmpstr);

                            free(tmpstr);
                        }
                        else
                        {
                            output->str = strdup("rep outsb - chyba");
                        }
                    }
                    else
                    {
                        sprintf(cResult, "push eax\nlodsb\ncall x86_out_dx_al\npop eax");

                        output->str = strdup(cResult);
                    }
#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                }
                break;

            case UD_Ilds:
            case UD_Iles:
            case UD_Ilfs:
            case UD_Ilgs:
            case UD_Ilss:
                if (fixup != NULL)
                {
                    if (fixup->type != FT_NORMAL)
                    {
                        return -52;
                    }
                    else
                    {

                        extrn = section_extrn_list_FindEntryEqual(fixup->tsec, fixup->tofs);

                        ret = SR_disassemble_convert_fixup(ud_insn_asm(&ud_obj), cResult, fixup, extrn, Entry, offset, decoded_length);

                        if (ret) return ret;

                        output->str = strdup(cResult);
                    }
                }
                else
                {
                    output->str = strdup(ud_insn_asm(&ud_obj));
                }

                output->str[0] = 'm';
                output->str[1] = 'o';
                output->str[2] = 'v';
#ifdef DISPLAY_DISASSEMBLY
                printf("\t%s\n", output->str);
#endif
                break;
            case UD_Iinvalid:
                printf("decoded length: %i\n", decoded_length);
                return 3;
                break;

        #if (OUTPUT_TYPE == OUT_LLASM)
            case UD_Iseto:
            case UD_Isetno:
            case UD_Isetb:
            //case UD_Isetnb:
            case UD_Isetz:
            case UD_Isetnz:
            case UD_Isetbe:
            //case UD_Isetnbe:
            case UD_Isets:
            case UD_Isetns:
            case UD_Isetp:
            case UD_Isetnp:
            case UD_Isetl:
            //case UD_Isetnl:
            case UD_Isetle:
            //case UD_Isetnle:
            case UD_Iseta:
            case UD_Isetge:
            case UD_Isetg:
                if (prev_cjump_length)
                {
                    if (!SR_disassemble_change_cjump_iflags(Entry, offset, prev_cjump_length, ud_obj.mnemonic))
                    {
                        return 3;
                    }
                }
        #endif
                // fallthrough to default
            default:
                // mov reg, fs:[]
                if (ud_obj.mnemonic == UD_Imov &&
                    ud_obj.operand[0].type == UD_OP_REG &&
                    ud_obj.operand[1].type == UD_OP_MEM &&
                    ud_obj.operand[1].base == UD_NONE &&
                    ud_obj.operand[1].index == UD_NONE &&
                    ud_obj.operand[1].size == 32 &&
                    fixup == NULL &&
                    (ud_obj.operand[0].base >= UD_R_EAX && ud_obj.operand[0].base <= UD_R_EDI) &&
                    ud_obj.pfx_seg == UD_R_FS
                   )
                {
                    sprintf(cResult, "push dword 0x%x\ncall x86_read_fs_dword\npop %s", ud_obj.operand[1].lval.udword, ud_reg_tab[ud_obj.operand[0].base - UD_R_AL]);

                    output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                    break;
                }

                // mov fs:[], reg
                if (ud_obj.mnemonic == UD_Imov &&
                    ud_obj.operand[1].type == UD_OP_REG &&
                    ud_obj.operand[0].type == UD_OP_MEM &&
                    ud_obj.operand[0].base == UD_NONE &&
                    ud_obj.operand[0].index == UD_NONE &&
                    ud_obj.operand[0].size == 32 &&
                    fixup == NULL &&
                    (ud_obj.operand[1].base >= UD_R_EAX && ud_obj.operand[1].base <= UD_R_EDI) &&
                    ud_obj.pfx_seg == UD_R_FS
                   )
                {
                    sprintf(cResult, "push %s\npush dword 0x%x\ncall x86_write_fs_dword", ud_reg_tab[ud_obj.operand[1].base - UD_R_AL], ud_obj.operand[0].lval.udword);

                    output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                    break;
                }

                // mov reg, [] - memory access with constant address
                if (ud_obj.mnemonic == UD_Imov &&
                    ud_obj.operand[0].type == UD_OP_REG &&
                    ud_obj.operand[1].type == UD_OP_MEM &&
                    ud_obj.operand[1].base == UD_NONE &&
                    ud_obj.operand[1].index == UD_NONE &&
                    fixup == NULL &&
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
                    sprintf(cResult, "push dword 0x%x\ncall x86_mov_reg_mem_%i\nmov %s, [esp - 4]", ud_obj.operand[1].lval.udword, ud_obj.operand[0].size, ud_reg_tab[ud_obj.operand[0].base - UD_R_AL] );

                    output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                    break;
                }

                // mov [], reg/imm - memory access with constant address
                if (ud_obj.mnemonic == UD_Imov &&
                    ud_obj.operand[0].type == UD_OP_MEM &&
                    ud_obj.operand[0].base == UD_NONE &&
                    ud_obj.operand[0].index == UD_NONE &&
                    fixup == NULL &&
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
                    if (ud_obj.operand[1].type == UD_OP_REG)
                    {
                        sprintf(cResult, "push %s\npush dword 0x%x\ncall x86_mov_mem_reg_%i", ud_reg_tab[(ud_obj.operand[1].base - UD_R_AL) + UD_R_EAX], ud_obj.operand[0].lval.udword, ud_obj.operand[1].size );
                    }
                    else
                    {
                        sprintf(cResult, "push dword 0x%x\npush dword 0x%x\ncall x86_mov_mem_reg_%i", (ud_obj.operand[1].size == 8)?(ud_obj.operand[1].lval.ubyte):((ud_obj.operand[1].size == 16)?(ud_obj.operand[1].lval.uword):(ud_obj.operand[1].lval.udword)), ud_obj.operand[0].lval.udword, ud_obj.operand[1].size );
                    }

                    output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                    break;
                }

                // push/pop fs:[]
                if (((ud_obj.mnemonic == UD_Ipush) || (ud_obj.mnemonic == UD_Ipop)) &&
                    ud_obj.operand[0].type == UD_OP_MEM &&
                    ud_obj.operand[0].base == UD_NONE &&
                    ud_obj.operand[0].index == UD_NONE &&
                    ud_obj.operand[0].size == 32 &&
                    fixup == NULL &&
                    ud_obj.pfx_seg == UD_R_FS
                   )
                {
                    if (ud_obj.mnemonic == UD_Ipush)
                    {
                        sprintf(cResult, "push dword 0x%x\ncall x86_read_fs_dword", ud_obj.operand[0].lval.udword);
                    }
                    else
                    {
                        sprintf(cResult, "push dword 0x%x\ncall x86_write_fs_dword", ud_obj.operand[0].lval.udword);
                    }

                    output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                    break;
                }

                /* remove segment registers from push an pop */
                if ((ud_obj.mnemonic == UD_Ipush || ud_obj.mnemonic == UD_Ipop) &&
                    ud_obj.operand[0].type == UD_OP_REG &&
                    (ud_obj.operand[0].base == UD_R_ES ||
                     ud_obj.operand[0].base == UD_R_FS ||
                     ud_obj.operand[0].base == UD_R_GS ||
                     ud_obj.operand[0].base == UD_R_CS ||
                     ud_obj.operand[0].base == UD_R_SS ||
                     ud_obj.operand[0].base == UD_R_DS))
                {
                    if (ud_obj.mnemonic == UD_Ipush)
                    {
                        output->str = strdup("push byte 0");
                    }
                    else
                    {
                        output->str = strdup("add esp, 4");
                    }
#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                    break;
                }

                /* remove segment registers from mov (1st operand) */
                if (ud_obj.mnemonic == UD_Imov &&
                    (ud_obj.operand[0].type == UD_OP_REG &&
                     (ud_obj.operand[0].base == UD_R_ES ||
                      ud_obj.operand[0].base == UD_R_FS ||
                      ud_obj.operand[0].base == UD_R_GS ||
                      ud_obj.operand[0].base == UD_R_CS ||
                      ud_obj.operand[0].base == UD_R_SS ||
                      ud_obj.operand[0].base == UD_R_DS)))
                {
                    output->str = strdup("; mov seg, ...");
#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif

                    break;
                }

                /* remove segment registers from mov (2nd operand) */
                if (ud_obj.mnemonic == UD_Imov &&
                    (ud_obj.operand[1].type == UD_OP_REG &&
                     (ud_obj.operand[1].base == UD_R_ES ||
                      ud_obj.operand[1].base == UD_R_FS ||
                      ud_obj.operand[1].base == UD_R_GS ||
                      ud_obj.operand[1].base == UD_R_CS ||
                      ud_obj.operand[1].base == UD_R_SS ||
                      ud_obj.operand[1].base == UD_R_DS)))
                {
                    if (fixup != NULL)
                    {
                        if (fixup->type != FT_NORMAL)
                        {
                            return -51;
                        }
                        else
                        {
                            ret = SR_disassemble_remove_segreg2(ud_insn_asm(&ud_obj), cResPart, &(ud_obj.operand[0]));

                            if (ret) return ret;

                            extrn = section_extrn_list_FindEntryEqual(fixup->tsec, fixup->tofs);

                            ret = SR_disassemble_convert_fixup(cResPart, cResult, fixup, extrn, Entry, offset, decoded_length);

                            if (ret) return ret;
                        }
                    }
                    else
                    {
                        ret = SR_disassemble_remove_segreg2(ud_insn_asm(&ud_obj), cResult, &(ud_obj.operand[0]));

                        if (ret) return ret;
                    }

                    output->str = strdup(cResult);
#ifdef DISPLAY_DISASSEMBLY
                    printf("\t%s\n", output->str);
#endif
                    break;
                }

                if (fixup != NULL)
                {
                    if (fixup->type != FT_NORMAL && fixup->type != FT_IMAGEBASE)
                    {
                        return -50;
                    }
                    else
                    {
                        if (((ud_obj.operand[0].type == UD_OP_IMM && ud_obj.operand[0].size == 32) || ud_obj.operand[0].type == UD_OP_MEM) ||
                            ((ud_obj.operand[1].type == UD_OP_IMM && ud_obj.operand[1].size == 32) || ud_obj.operand[1].type == UD_OP_MEM))
                        {
                            if (fixup->type != FT_IMAGEBASE)
                            {
                                extrn = section_extrn_list_FindEntryEqual(fixup->tsec, fixup->tofs);
                            }
                            else
                            {
                                extrn = NULL;
                            }

                            if (extrn != NULL && extrn->is_import)
                            {
                                if (ud_obj.mnemonic == UD_Imov &&
                                    ud_obj.operand[0].type == UD_OP_REG &&
                                    ud_obj.operand[0].base >= UD_R_EAX &&
                                    ud_obj.operand[0].base <= UD_R_EDI &&
                                    ud_obj.operand[1].type == UD_OP_MEM &&
                                    ud_obj.operand[1].base == UD_NONE &&
                                    ud_obj.operand[1].index == UD_NONE &&
                                    ud_obj.operand[1].size == 32
                                   )
                                {
                                    extrn->is_used = 1;
                                    sprintf(cResult, "mov %s, %s", ud_reg_tab[ud_obj.operand[0].base - UD_R_AL], extrn->proc);
                                    output->str = strdup(cResult);
                                }
                                else
                                {
                                    printf("unhandled import: %s\n", extrn->proc);

                                    return -53;
                                }
                            }
                            else
                            {
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

                                    if (extrn2 != NULL && extrn2->is_import)
                                    {
                                        printf("unhandled import: %s\n", extrn2->proc);

                                        return -52;
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


                                if (ud_obj.pfx_seg != UD_NONE)
                                {
                                    ret = SR_disassemble_remove_segment(cResult, cResPart, ud_obj.pfx_seg);

                                    if (ret) return ret;

                                    output->str = strdup(cResPart);

                                }
                                else
                                {
                                    output->str = strdup(cResult);
                                }
                            }

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
                        if (ud_obj.pfx_seg != UD_NONE)
                        {
                            ret = SR_disassemble_remove_segment(ud_insn_asm(&ud_obj), cResult, ud_obj.pfx_seg);

                            if (ret) return ret;

                            output->str = strdup(cResult);

#ifdef DISPLAY_DISASSEMBLY
                            printf("\t%s\n", output->str);
#endif
                        }
                        else
                        {
                            output->str = strdup(ud_insn_asm(&ud_obj));
                        }
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

