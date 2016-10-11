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

#include <stdio.h>
#include <string.h>
#include "SR_defs.h"
#include "SR_vars.h"

typedef struct _Entry_FILE_ {
    uint_fast32_t Entry;
    FILE *fout;
} Entry_FILE;


static void SR_write_output_alias_global(alias_data *item, void *data)
{
#define DATA ((Entry_FILE *) data)

#if (OUTPUT_TYPE == OUT_ARM_LINUX)
    fprintf(DATA->fout, ".global %s\n", item->proc);
    fprintf(DATA->fout, ".global _%s\n", item->proc);
#else
    fprintf(DATA->fout, "global %s\n", item->proc);
    fprintf(DATA->fout, "global _%s\n", item->proc);
#endif

#undef DATA
}

static void SR_write_output__chk_hack(Word_t Index, void *data)
{
    char cbuf[32];

#define DATA ((Entry_FILE *) data)

#if (OUTPUT_TYPE == OUT_ARM_LINUX)
    fprintf(DATA->fout, "%s\n", ".balign 16, 0");

    SR_get_label(cbuf, section[DATA->Entry].start + Index);
    fprintf(DATA->fout, ".int %s\n", cbuf);
    cbuf[0] = 's';
    cbuf[1] = 'u';
    cbuf[2] = 'b';
    fprintf(DATA->fout, ".asciz \"%s\"\n", cbuf);
#else
    fprintf(DATA->fout, "%s\n", "align 16, db 0");

    SR_get_label(cbuf, section[DATA->Entry].start + Index);
    fprintf(DATA->fout, "dd %s\n", cbuf);
    cbuf[0] = 's';
    cbuf[1] = 'u';
    cbuf[2] = 'b';
    fprintf(DATA->fout, "db '%s',0\n", cbuf);
#endif

#undef DATA
}

static void SR_write_output_line(output_data *item, void *data)
{
    char cbuf[16];

#define DATA ((Entry_FILE *) data)

    if ( (item->str != NULL) || (item->type <= 0) || (item->type == OT_NONE))
    {
        if ( item->align != 0 )
        {
#if (OUTPUT_TYPE == OUT_ARM_LINUX)
            fprintf(DATA->fout, ".balign %i%s\n", item->align, ((section[DATA->Entry].type == ST_CODE)?"":", 0"));
#else
            fprintf(DATA->fout, "align %i%s\n", item->align, ((section[DATA->Entry].type == ST_CODE)?"":", db 0"));
#endif
        }

        if ( (item->ofs == 0) || (item->has_label != 0) )
        {
#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_DOS)
            alias_data *alias;

            alias = section_alias_list_FindEntryEqual(DATA->Entry, item->ofs);

            if (alias != NULL)
            {
                fprintf(DATA->fout, "%s:\n", alias->proc);
                fprintf(DATA->fout, "_%s:\n", alias->proc);
            }
            SR_get_label(cbuf, section[DATA->Entry].start + item->ofs);
            if ((alias == NULL) || strcmp(cbuf, alias->proc))
            {
                fprintf(DATA->fout, "%s:\n", cbuf);
            }
#else
            SR_get_label(cbuf, section[DATA->Entry].start + item->ofs);
            fprintf(DATA->fout, "%s:\n", cbuf);
#endif
        }

        if (item->str != NULL)
        {
#ifdef _CHK_HACK
    #if (OUTPUT_TYPE == OUT_DOS)
            if ( strcmp(item->str, "call loc_81EBA") == 0)
    #else
        #if (OUTPUT_TYPE == OUT_ARM_LINUX)
            if ( strncmp(item->str, "@call SR___CHK\n", 15) == 0)
        #else
            if ( strcmp(item->str, "call SR___CHK") == 0)
        #endif
    #endif
            {
                section_chk_list_Insert(DATA->Entry, item->ofs - 5);
            }
#endif
            fprintf(DATA->fout, "%s\n", item->str);
        }
        else
        {
            if (item->type == OT_UNKNOWN)
            {
#if (OUTPUT_TYPE == OUT_ARM_LINUX)
                fprintf(DATA->fout, ".byte %i\n", section[DATA->Entry].adr[item->ofs]);
#else
                fprintf(DATA->fout, "db %i\n", section[DATA->Entry].adr[item->ofs]);
#endif
            }
            else if (item->type != OT_NONE)
            {
#if (OUTPUT_TYPE == OUT_ARM_LINUX)
                fprintf(DATA->fout, ".skip 1\n");
#else
                fprintf(DATA->fout, "resb 1\n");
#endif
            }
        }
    }

#undef DATA
}

int SR_write_output(const char *fname, int x86linux)
{
    Entry_FILE EF;
    int first_data, first_code;
    Word_t index;

    EF.fout = fopen(fname, "wt");

    if ( EF.fout == NULL ) return -1;

    first_data = 1;
    first_code = 1;

#if (OUTPUT_TYPE == OUT_ARM_LINUX)
    fprintf(EF.fout, "%s\n", ".include \"armconf.inc\"");
    fprintf(EF.fout, "%s\n", ".include \"arm.inc\"");
    fprintf(EF.fout, "%s\n", ".include \"arminc.inc\"");
    fprintf(EF.fout, "%s\n", ".include \"misc.inc\"");
    fprintf(EF.fout, "%s\n", ".include \"extern.inc\"");
    fprintf(EF.fout, "%s\n", ".include \"macros.inc\"");
#else
#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_DOS)
    fprintf(EF.fout, "%s\n", "%include \"x86inc.inc\"");
#endif
    fprintf(EF.fout, "%s\n", "%include \"misc.inc\"");
#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_DOS)
    fprintf(EF.fout, "%s\n", "%include \"extern.inc\"");
    fprintf(EF.fout, "%s\n", "%include \"macros.inc\"");
#endif
#endif

#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_DOS)
    {
        char cbuf[32];

        SR_get_label(cbuf, section[EIPObjectNum].start + EIP);
        fprintf(EF.fout, "global %s\n", cbuf);
    }

#else

    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        section_alias_list_ForEach(EF.Entry, &SR_write_output_alias_global, (void *) &EF);
    }
#endif


#ifdef _CHK_HACK
    #if (OUTPUT_TYPE == OUT_ARM_LINUX)
        fprintf(EF.fout, "%s\n", ".global procedure_list");
        fprintf(EF.fout, "%s\n", ".global _procedure_list");
    #else
        fprintf(EF.fout, "%s\n", "global procedure_list");
        fprintf(EF.fout, "%s\n", "global _procedure_list");
    #endif
#endif

    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        switch (section[EF.Entry].type)
        {
#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_DOS)
            case ST_CODE:
                fprintf(EF.fout, "\nsection %s private class=CODE USE32\n", section[EF.Entry].name);
                break;
            case ST_DATA:
            case ST_UDATA:
                fprintf(EF.fout, "\nsection %s private class=DATA USE32\n", section[EF.Entry].name);
                break;
            case ST_STACK:
                fprintf(EF.fout, "\nsection %s stack class=STACK USE32\n", section[EF.Entry].name);
                break;
#elif (OUTPUT_TYPE == OUT_X86)
            case ST_CODE:
                if (x86linux)
                {
                    fprintf(EF.fout, "\nsection %s progbits alloc exec nowrite align=16\n", section[EF.Entry].name);
                }
                else
                {
                    fprintf(EF.fout, "\nsection %s code align=16\n", section[EF.Entry].name);
                }
                break;
            case ST_DATA:
            case ST_STACK:
            case ST_UDATA:
                if (x86linux)
                {
                    fprintf(EF.fout, "\nsection %s progbits alloc noexec write align=4\n", section[EF.Entry].name);
                }
                else
                {
                    fprintf(EF.fout, "\nsection %s data align=4\n", section[EF.Entry].name);
                }
                break;
#else
            case ST_CODE:
/*				fprintf(EF.fout, "\n.section %s, \"x\", %%progbits\n", section[EF.Entry].name);*/
                fprintf(EF.fout, "\n.section %s\n", section[EF.Entry].name);
                break;
            case ST_DATA:
            case ST_STACK:
            case ST_UDATA:
/*				fprintf(EF.fout, "\n.section %s, \"w\", %%progbits\n", section[EF.Entry].name);*/
                fprintf(EF.fout, "\n.section %s\n", section[EF.Entry].name);
                break;
#endif
        }


        {
            FILE *fmain;
            char incname[12];

            sprintf((char *) &(incname[0]), "seg%.2i.inc", EF.Entry + 1);

        #if (OUTPUT_TYPE == OUT_ARM_LINUX)
            fprintf(EF.fout, ".include \"%s\"\n", (char *) &(incname[0]));
        #else
            fprintf(EF.fout, "%%include \"%s\"\n", (char *) &(incname[0]));
        #endif

        #if (OUTPUT_TYPE == OUT_X86)
            if (!x86linux)
        #endif
            {
                fmain = EF.fout;

                EF.fout = fopen((char *) &(incname[0]), "wt");

                if ( EF.fout == NULL ) return -1;

                section_output_list_ForEach(EF.Entry, &SR_write_output_line, (void *) &EF);

                fclose(EF.fout);

                EF.fout = fmain;
            }
        }

#if (OUTPUT_TYPE != OUT_ORIG)
        if (first_data && (section[EF.Entry].type == ST_DATA || section[EF.Entry].type == ST_STACK) )
        {
            first_data = 0;

    #if (OUTPUT_TYPE == OUT_ARM_LINUX)
            fprintf(EF.fout, "%s\n", ".ifdef DEBUG");
            fprintf(EF.fout, "%s\n", ".include \"data_all.inc\"");
    #else
            fprintf(EF.fout, "%s\n", "%ifdef DEBUG");
            fprintf(EF.fout, "%s\n", "%include \"data_all.inc\"");
    #endif

#ifdef _CHK_HACK
    #if (OUTPUT_TYPE == OUT_ARM_LINUX)
            fprintf(EF.fout, "%s\n", ".balign 16, 0");
    #else
            fprintf(EF.fout, "%s\n", "align 16, db 0");
    #endif
            fprintf(EF.fout, "%s\n", "procedure_list:");
            fprintf(EF.fout, "%s\n", "_procedure_list:");

            Entry_FILE EF2;

            EF2.fout = EF.fout;

            for (EF2.Entry = 0; EF2.Entry < num_sections; EF2.Entry++)
            {
                section_chk_list_ForEach(EF2.Entry, &SR_write_output__chk_hack, (void *) &EF2);
            }
    #if (OUTPUT_TYPE == OUT_ARM_LINUX)
            fprintf(EF.fout, "%s\n", ".balign 16, 0");
            fprintf(EF.fout, "%s\n", ".int 0");
            fprintf(EF.fout, "%s\n", ".byte 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0");
    #else
            fprintf(EF.fout, "%s\n", "align 16, db 0");
            fprintf(EF.fout, "%s\n", "dd 0");
            fprintf(EF.fout, "%s\n", "times 12 db 0");
    #endif
#endif

    #if (OUTPUT_TYPE == OUT_ARM_LINUX)
            fprintf(EF.fout, "%s\n", ".endif");
    #else
            fprintf(EF.fout, "%s\n", "%endif");
    #endif
        }
        if (first_code && section[EF.Entry].type == ST_CODE)
        {
            first_code = 0;
    #if (OUTPUT_TYPE == OUT_ARM_LINUX)
            fprintf(EF.fout, "%s\n", ".ifdef DEBUG");
            fprintf(EF.fout, "%s\n", ".include \"code_armlinux.inc\"");
            fprintf(EF.fout, "%s\n", ".endif");
    #else
            fprintf(EF.fout, "%s\n", "%ifdef DEBUG");
        #if (OUTPUT_TYPE == OUT_DOS)
            fprintf(EF.fout, "%s\n", "%include \"code_dos.inc\"");
        #endif
        #if (OUTPUT_TYPE == OUT_X86)
            if (x86linux)
            {
                fprintf(EF.fout, "%s\n", "%include \"code_linux.inc\"");
            }
            else
            {
                fprintf(EF.fout, "%s\n", "%include \"code_win32.inc\"");
            }
        #endif
            fprintf(EF.fout, "%s\n", "%endif");
    #endif

        }
#endif
    }

    fclose(EF.fout);

    return 0;
}

