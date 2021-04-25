/**
 *
 *  Copyright (C) 2016-2021 Roman Pauer
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

typedef struct _Entry_Proc_ {
    uint_fast32_t Entry;
    const char *ProcName;
    int is_used;
} Entry_Proc;


#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_WINDOWS && OUTPUT_TYPE != OUT_LLASM)
static void SR_write_output_alias_global(alias_data *item, void *data)
{
#define DATA ((Entry_FILE *) data)

#if (OUTPUT_TYPE == OUT_LLASM)
    fprintf(DATA->fout, ".global %s\n", item->proc);
    fprintf(DATA->fout, ".global _%s\n", item->proc);
#else
    fprintf(DATA->fout, "global %s\n", item->proc);
    fprintf(DATA->fout, "global _%s\n", item->proc);
#endif

#undef DATA
}
#endif

static void SR_write_output_line(output_data *item, void *data)
{
    char cbuf[16];

#define DATA ((Entry_FILE *) data)

    if ( (item->str != NULL) || (item->type <= 0) || (item->type == OT_NONE))
    {
        if ( item->align != 0 )
        {
#if (OUTPUT_TYPE == OUT_LLASM)
            fprintf(DATA->fout, "dalign %i\n", item->align);
#else
            fprintf(DATA->fout, "align %i%s\n", item->align, ((section[DATA->Entry].type == ST_CODE)?"":", db 0"));
#endif
        }

#if (OUTPUT_TYPE == OUT_LLASM)
        if (item->type == OT_INSTRUCTION)
        {
            return;
        }
#endif

        if ( (item->ofs == 0) || (item->has_label != 0) )
        {
#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_WINDOWS)
            alias_data *alias;

            alias = section_alias_list_FindEntryEqual(DATA->Entry, item->ofs);

            if (alias != NULL)
            {
#if (OUTPUT_TYPE == OUT_LLASM)
                fprintf(DATA->fout, "dlabel %s global\n", alias->proc);
                fprintf(DATA->fout, "dlabel _%s global\n", alias->proc);
#else
                fprintf(DATA->fout, "%s:\n", alias->proc);
                fprintf(DATA->fout, "_%s:\n", alias->proc);
#endif
            }
            else
            {
                export_data *export;

                export = section_export_list_FindEntryEqual1(DATA->Entry, item->ofs);
                while ((export != NULL) && (export->name[0] != 0))
                {
#if (OUTPUT_TYPE == OUT_LLASM)
                    fprintf(DATA->fout, "dlabel %s global\n", (export->internal != NULL)?export->internal:export->name);
                    fprintf(DATA->fout, "dlabel _%s global\n", (export->internal != NULL)?export->internal:export->name);
#else
                    fprintf(DATA->fout, "%s:\n", (export->internal != NULL)?export->internal:export->name);
                    fprintf(DATA->fout, "_%s:\n", (export->internal != NULL)?export->internal:export->name);
#endif
                    export = export->next;
                };
            }

            SR_get_label(cbuf, section[DATA->Entry].start + item->ofs);
            if ((alias == NULL) || strcmp(cbuf, alias->proc))
            {
#if (OUTPUT_TYPE == OUT_LLASM)
                fprintf(DATA->fout, "dlabel %s\n", cbuf);
#else
                fprintf(DATA->fout, "%s:\n", cbuf);
#endif
            }
#else
            SR_get_label(cbuf, section[DATA->Entry].start + item->ofs);
            fprintf(DATA->fout, "%s:\n", cbuf);
#endif
            if (add_tracing_to_code && item->type == OT_INSTRUCTION)
            {
                fprintf(DATA->fout, "TRACE_LABEL 0x%x\n", (unsigned int)(section[DATA->Entry].start + item->ofs));
            }
        }

        if (item->str != NULL)
        {
            fprintf(DATA->fout, "%s\n", item->str);
        }
        else
        {
            if (item->type == OT_UNKNOWN)
            {
                fprintf(DATA->fout, "db %i\n", section[DATA->Entry].adr[item->ofs]);
            }
            else if (item->type != OT_NONE)
            {
#if (OUTPUT_TYPE == OUT_LLASM)
                fprintf(DATA->fout, "dskip 1\n");
#else
                fprintf(DATA->fout, "resb 1\n");
#endif
            }
        }
    }

#undef DATA
}

#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_WINDOWS)
static void SR_write_output_import_obj(import_data *item, void *data)
{
#define DATA ((Entry_FILE *) data)

    fprintf(DATA->fout, "extern %s\n", item->ProcName);
    fprintf(DATA->fout, "import %s %s\n", item->ProcName, item->DllName);

#undef DATA
}

static void SR_write_output_export_obj(export_data *item, void *data)
{
    char cbuf[16];

#define DATA ((Entry_FILE *) data)

    SR_get_label(cbuf, section[DATA->Entry].start + item->ofs);

    fprintf(DATA->fout, "extern %s\n", cbuf);

    if (item->name[0] == 0)
    {
        fprintf(DATA->fout, "export %s %i\n", cbuf, (unsigned int)item->ordinal);
    }
    else
    {
        fprintf(DATA->fout, "export %s %s %i\n", cbuf, item->name, (unsigned int)item->ordinal);
    }

#undef DATA
}
#elif (OUTPUT_TYPE != OUT_LLASM)
static void SR_write_output_export_def(export_data *item, void *data)
{
    char cbuf[16];
    alias_data *alias;

#define DATA ((Entry_FILE *) data)

    alias = section_alias_list_FindEntryEqual(DATA->Entry, item->ofs);

    if (alias != NULL)
    {
        if (item->name[0] != 0)
        {
            fprintf(DATA->fout, "%s = %s", item->name, alias->proc);
        }
        else
        {
            fprintf(DATA->fout, "%s", alias->proc);
        }
    }
    else if (item->name[0] == 0)
    {
        SR_get_label(cbuf, section[DATA->Entry].start + item->ofs);
        fprintf(DATA->fout, "%s", cbuf);
    }
    else if (item->internal != NULL)
    {
        fprintf(DATA->fout, "%s = %s", item->name, item->internal);
    }
    else
    {
        fprintf(DATA->fout, "%s", item->name);
    }

    if (item->ordinal != 0)
    {
        fprintf(DATA->fout, " @%i", (unsigned int)item->ordinal);
    }

    if (item->name[0] == 0)
    {
        fprintf(DATA->fout, " NONAME");
    }

    if (section[DATA->Entry].type != ST_CODE)
    {
        fprintf(DATA->fout, " DATA");
    }

    fprintf(DATA->fout, "\n");

#undef DATA
}
#endif

#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_WINDOWS)
void SR_write_import_is_used(extrn_data *extrn, void *data)
{
#define DATA ((Entry_Proc *) data)

    if (DATA->is_used) return;

    if (!extrn->is_used) return;
    if (extrn->altaction != NULL) return;

    if (strcmp(extrn->proc, DATA->ProcName) == 0)
    {
        DATA->is_used = 1;
    }

#undef DATA
}
#endif

static void SR_write_output_import(import_data *item, void *data)
{
#define DATA ((Entry_FILE *) data)

#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_WINDOWS)
    // is import used ?

    Entry_Proc EP;

    EP.ProcName = item->ProcName;
    EP.is_used = 0;
    for (EP.Entry = 0; EP.Entry < num_sections; EP.Entry++)
    {
        section_extrn_list_ForEach(EP.Entry, &SR_write_import_is_used, (void *)&EP);

        if (EP.is_used) break;
    }

    if (!EP.is_used) return;

#endif

#if (OUTPUT_TYPE == OUT_LLASM)
    fprintf(DATA->fout, "proc %s external\n", item->ProcName);
#else
    fprintf(DATA->fout, "extern %s ; %s\n", item->ProcName, item->DllName);
#endif

#undef DATA
}

#if (OUTPUT_TYPE != OUT_LLASM)
static void SR_write_output_export(export_data *item, void *data)
{
    char cbuf[16];

#define DATA ((Entry_FILE *) data)

#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_WINDOWS)
    if (item->name != NULL)
    {
        alias_data *alias;

        alias = section_alias_list_FindEntryEqual(DATA->Entry, item->ofs);

        if (alias == NULL)
        {
            fprintf(DATA->fout, "global %s\n", (item->internal != NULL)?item->internal:item->name);
            fprintf(DATA->fout, "global _%s\n", (item->internal != NULL)?item->internal:item->name);
        }
    }
    else
#endif
    {
        SR_get_label(cbuf, section[DATA->Entry].start + item->ofs);

        fprintf(DATA->fout, "global %s\n", cbuf);
    }

#undef DATA
}
#endif

#if (OUTPUT_TYPE == OUT_LLASM)

static void SR_write_llasm_output_hasdata(output_data *item, void *data)
{
#define HAS_DATA (*((uint_fast32_t *) data))

    if (HAS_DATA) return;
    if (item->type == OT_INSTRUCTION) return;
    if (item->type == OT_NONE) return;

    HAS_DATA = 1;

#undef HAS_DATA
}

static void SR_write_llasm_output_function(output_data *item, void *data)
{
    char cbuf[16], *export_name;

#define DATA ((Entry_FILE *) data)

    if (item->type == OT_INSTRUCTION)
    {
        if (item->has_label != 0)
        {
            alias_data *alias;
            export_data *export;

            SR_get_label(cbuf, section[DATA->Entry].start + item->ofs);

            alias = section_alias_list_FindEntryEqual(DATA->Entry, item->ofs);

            if (alias != NULL)
            {
                fprintf(DATA->fout, "define %s %s\n", cbuf, alias->proc);
            }
            else
            {
                export = section_export_list_FindEntryEqual1(DATA->Entry, item->ofs);

                if ((export != NULL) && (export->name[0] != 0))
                {
                    export_name = (export->internal != NULL)?export->internal:export->name;

                    fprintf(DATA->fout, "define %s %s\n", cbuf, export_name);

                    export = export->next;
                    while ((export != NULL) && (export->name[0] != 0))
                    {
                        fprintf(DATA->fout, "define %s %s\n", (export->internal != NULL)?export->internal:export->name, export_name);
                        fprintf(DATA->fout, "define c_%s c_%s\n", (export->internal != NULL)?export->internal:export->name, export_name);

                        export = export->next;
                    };
                }
            }
        }
    }

#undef DATA
}

static void SR_write_llasm_output_line(output_data *item, void *data)
{
    char cbuf[16];

#define DATA ((Entry_FILE *) data)

    if (item->type == OT_INSTRUCTION)
    {
        if (item->has_label != 0)
        {
            alias_data *alias;
            export_data *export;

            alias = section_alias_list_FindEntryEqual(DATA->Entry, item->ofs);
            if (alias != NULL)
            {
                fprintf(DATA->fout, "proc %s global c_%s\n", alias->proc, alias->proc);
            }
            else
            {
                export = section_export_list_FindEntryEqual1(DATA->Entry, item->ofs);
                if ((export != NULL) && (export->name[0] != 0))
                {
                    fprintf(DATA->fout, "proc %s global c_%s\n", (export->internal != NULL)?export->internal:export->name, (export->internal != NULL)?export->internal:export->name);
                }
                else
                {
                    SR_get_label(cbuf, section[DATA->Entry].start + item->ofs);

                    fprintf(DATA->fout, "proc %s\n", cbuf);
                }
            }
        }

        fprintf(DATA->fout, "%s\n", item->str);
    }

#undef DATA
}

#endif

int SR_write_output(const char *fname)
{
    Entry_FILE EF;
#if (OUTPUT_TYPE != OUT_ORIG)
    int first_data, first_code;
#endif

#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_WINDOWS)
    {
        char *fobjname, *dotptr;

        fobjname = (char *) malloc(strlen(fname) + 1 + 4);

        strcpy(fobjname, fname);

        dotptr = strrchr(fname, '.');
        if (dotptr != NULL)
        {
            strcpy((char *) &(fobjname[dotptr - fname]), "-obj");
            strcpy((char *) &(fobjname[4 + dotptr - fname]), dotptr);
        }
        else
        {
            strcat(fobjname, "-obj");
        }

        EF.fout = fopen(fobjname, "wt");
        free(fobjname);

        if ( EF.fout == NULL ) return -1;

        import_list_ForEach(&SR_write_output_import_obj, (void *) &EF);

        for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
        {
            if (section[EF.Entry].export_list != NULL)
            {
                section_export_list_ForEach(EF.Entry, &SR_write_output_export_obj, (void *) &EF);
            }
        }

        fclose(EF.fout);
    }
#elif (OUTPUT_TYPE != OUT_LLASM)
    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        if (section[EF.Entry].export_list != NULL) break;
    }
    if (EF.Entry < num_sections)
    {
        char *fdefname, *dotptr;

        fdefname = (char *) malloc(strlen(fname) + 1 + 4);

        strcpy(fdefname, fname);

        dotptr = strrchr(fname, '.');
        if (dotptr != NULL)
        {
            strcpy((char *) &(fdefname[dotptr - fname]), ".def");
        }
        else
        {
            strcat(fdefname, ".def");
        }

        EF.fout = fopen(fdefname, "wt");
        free(fdefname);

        if ( EF.fout == NULL ) return -1;

        fprintf(EF.fout, "EXPORTS\n");

        for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
        {
            if (section[EF.Entry].export_list != NULL)
            {
                section_export_list_ForEach(EF.Entry, &SR_write_output_export_def, (void *) &EF);
            }
        }

        fclose(EF.fout);
    }
#endif


    EF.fout = fopen(fname, "wt");

    if ( EF.fout == NULL ) return -1;

#if (OUTPUT_TYPE != OUT_ORIG)
    first_data = 1;
    first_code = 1;
#endif

#if (OUTPUT_TYPE == OUT_LLASM)
    fprintf(EF.fout, "%s\n", "include llasm.llinc");
    fprintf(EF.fout, "%s\n", "include extern.llinc");
    fprintf(EF.fout, "%s\n", "include macros.llinc");
#else
#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_WINDOWS)
    fprintf(EF.fout, "%s\n", "%include \"x86inc.inc\"");
#endif
    fprintf(EF.fout, "%s\n", "%include \"misc.inc\"");
#if (OUTPUT_TYPE != OUT_ORIG && OUTPUT_TYPE != OUT_WINDOWS)
    fprintf(EF.fout, "%s\n", "%include \"extern.inc\"");
    //fprintf(EF.fout, "%s\n", "%include \"macros.inc\"");
#endif
#endif

#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_WINDOWS)
    {
        char cbuf[32];

        SR_get_label(cbuf, section[EIPObjectNum].start + EIP);
        fprintf(EF.fout, "global %s\n", cbuf);
    }

#elif (OUTPUT_TYPE != OUT_LLASM)

    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        section_alias_list_ForEach(EF.Entry, &SR_write_output_alias_global, (void *) &EF);
    }
#endif

    fprintf(EF.fout, "\n");
    import_list_ForEach(&SR_write_output_import, (void *) &EF);

#if (OUTPUT_TYPE == OUT_X86)
    fprintf(EF.fout, "\n%%ifidn __OUTPUT_FORMAT__, elf32\n");
    fprintf(EF.fout, "section .note.GNU-stack noalloc noexec nowrite progbits\n");
    fprintf(EF.fout, "%%endif\n");
#endif

    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        switch (section[EF.Entry].type)
        {
#if (OUTPUT_TYPE == OUT_ORIG || OUTPUT_TYPE == OUT_WINDOWS)
            case ST_CODE:
                fprintf(EF.fout, "\nsection %s code align=16\n", section[EF.Entry].name);
                break;
            case ST_DATA:
            case ST_STACK:
                fprintf(EF.fout, "\nsection %s data align=4\n", section[EF.Entry].name);
                break;
            case ST_UDATA:
                fprintf(EF.fout, "\nsection %s bss align=4\n", section[EF.Entry].name);
                break;
            case ST_RDATA:
                fprintf(EF.fout, "\nsection %s rdata align=8\n", section[EF.Entry].name);
                break;
#elif (OUTPUT_TYPE == OUT_X86)
            case ST_CODE:
                fprintf(EF.fout, "\n%%ifidn __OUTPUT_FORMAT__, elf32\n");
                fprintf(EF.fout, "section %s progbits alloc exec nowrite align=16\n", section[EF.Entry].name);
                fprintf(EF.fout, "%%else\n");
                fprintf(EF.fout, "section %s code align=16\n", section[EF.Entry].name);
                fprintf(EF.fout, "%%endif\n");
                break;
            case ST_DATA:
            case ST_STACK:
                fprintf(EF.fout, "\n%%ifidn __OUTPUT_FORMAT__, elf32\n");
                fprintf(EF.fout, "section %s progbits alloc noexec write align=4\n", section[EF.Entry].name);
                fprintf(EF.fout, "%%else\n");
                fprintf(EF.fout, "section %s data align=4\n", section[EF.Entry].name);
                fprintf(EF.fout, "%%endif\n");
                break;
            case ST_UDATA:
                fprintf(EF.fout, "\n%%ifidn __OUTPUT_FORMAT__, elf32\n");
                fprintf(EF.fout, "section %s nobits alloc noexec write align=4\n", section[EF.Entry].name);
                fprintf(EF.fout, "%%else\n");
                fprintf(EF.fout, "section %s bss align=4\n", section[EF.Entry].name);
                fprintf(EF.fout, "%%endif\n");
                break;
            case ST_RDATA:
                fprintf(EF.fout, "\n%%ifidn __OUTPUT_FORMAT__, elf32\n");
                fprintf(EF.fout, "section %s progbits alloc noexec nowrite align=8\n", section[EF.Entry].name);
                fprintf(EF.fout, "%%else\n");
                fprintf(EF.fout, "section %s rdata align=8\n", section[EF.Entry].name);
                fprintf(EF.fout, "%%endif\n");
                break;
#elif (OUTPUT_TYPE == OUT_LLASM)
            case ST_CODE:
                {
                    uint_fast32_t has_data;

                    has_data = 0;
                    section_output_list_ForEach(EF.Entry, &SR_write_llasm_output_hasdata, (void *) &has_data);

                    if (!has_data) continue;
                }
                fprintf(EF.fout, "\ndatasegment %s constant\n", section[EF.Entry].name);
                break;
            case ST_DATA:
            case ST_STACK:
                fprintf(EF.fout, "\ndatasegment %s\n", section[EF.Entry].name);
                break;
            case ST_UDATA:
                fprintf(EF.fout, "\ndatasegment %s uninitialized\n", section[EF.Entry].name);
                break;
            case ST_RDATA:
                fprintf(EF.fout, "\ndatasegment %s constant\n", section[EF.Entry].name);
                break;
#else
            case ST_CODE:
/*				fprintf(EF.fout, "\n.section %s, \"x\", %%progbits\n", section[EF.Entry].name);*/
                fprintf(EF.fout, "\n.section %s\n", section[EF.Entry].name);
                break;
            case ST_DATA:
            case ST_STACK:
            case ST_UDATA:
            case ST_RDATA:
/*				fprintf(EF.fout, "\n.section %s, \"w\", %%progbits\n", section[EF.Entry].name);*/
                fprintf(EF.fout, "\n.section %s\n", section[EF.Entry].name);
                break;
#endif
            default:
                break;
        }

        if (EF.Entry == 0)
        {
#if (OUTPUT_TYPE == OUT_LLASM)
            fprintf(EF.fout, "dlabel imagebase1000\n");
#else
            fprintf(EF.fout, "imagebase1000:\n");
#endif
        }

#if (OUTPUT_TYPE != OUT_LLASM)
        section_export_list_ForEach(EF.Entry, &SR_write_output_export, (void *) &EF);
#endif

        if (section[EF.Entry].type == ST_RSRC)
        {
            FILE *fres;
            char *resname, *dotptr;

            resname = (char *) malloc(strlen(fname) + 9);

            strcpy(resname, fname);

            dotptr = strrchr(resname, '.');
            if (dotptr != NULL) *dotptr = 0;
            strcat(resname, ".resdump");

            fres = fopen((char *) &(resname[0]), "wb");

            if ( fres == NULL ) return -1;

            fwrite(section[EF.Entry].adr, 1, section[EF.Entry].size, fres);

            fclose(fres);

            free(resname);
        }
        else if ((section[EF.Entry].type == ST_CODE) ||
                 (section[EF.Entry].type == ST_DATA) ||
                 (section[EF.Entry].type == ST_STACK) ||
                 (section[EF.Entry].type == ST_UDATA) ||
                 (section[EF.Entry].type == ST_RDATA)
                )
        {
            FILE *fmain;
    #if (OUTPUT_TYPE == OUT_LLASM)
            char incname[20];

            sprintf((char *) &(incname[0]), "seg%.2i_data.llinc", (unsigned int)(EF.Entry + 1));
            fprintf(EF.fout, "include %s\n", (char *) &(incname[0]));
            fprintf(EF.fout, "endd\n");
    #else
            char incname[12];

            sprintf((char *) &(incname[0]), "seg%.2i.inc", (unsigned int)(EF.Entry + 1));
            fprintf(EF.fout, "%%include \"%s\"\n", (char *) &(incname[0]));
    #endif

            fmain = EF.fout;

            EF.fout = fopen((char *) &(incname[0]), "wt");

            if ( EF.fout == NULL ) return -1;

            section_output_list_ForEach(EF.Entry, &SR_write_output_line, (void *) &EF);

            fclose(EF.fout);

            EF.fout = fmain;
        }

#if (OUTPUT_TYPE != OUT_ORIG)
        if (first_data && (section[EF.Entry].type == ST_DATA || section[EF.Entry].type == ST_STACK) )
        {
            first_data = 0;

    #if (OUTPUT_TYPE == OUT_LLASM)
            //fprintf(EF.fout, "%s\n", ".ifdef DEBUG");
    #else
            //fprintf(EF.fout, "%s\n", "%ifdef DEBUG");
            //fprintf(EF.fout, "%s\n", "%include \"data_all.inc\"");
    #endif

    #if (OUTPUT_TYPE == OUT_LLASM)
            //fprintf(EF.fout, "%s\n", ".endif");
    #else
            //fprintf(EF.fout, "%s\n", "%endif");
    #endif
        }
        if (first_code && section[EF.Entry].type == ST_CODE)
        {
            first_code = 0;
    #if (OUTPUT_TYPE == OUT_LLASM)
            //fprintf(EF.fout, "%s\n", ".ifdef DEBUG");
            //fprintf(EF.fout, "%s\n", ".endif");
    #else
            //fprintf(EF.fout, "%s\n", "%ifdef DEBUG");
        #if (OUTPUT_TYPE == OUT_WINDOWS)
            //fprintf(EF.fout, "%s\n", "%include \"code_window.inc\"");
        #endif
        #if (OUTPUT_TYPE == OUT_X86)
            //fprintf(EF.fout, "%%ifidn __OUTPUT_FORMAT__, elf32\n");
            //fprintf(EF.fout, "%s\n", "%include \"code_linux.inc\"");
            //fprintf(EF.fout, "%%else\n");
            //fprintf(EF.fout, "%s\n", "%include \"code_win32.inc\"");
            //fprintf(EF.fout, "%%endif\n");
        #endif
            //fprintf(EF.fout, "%s\n", "%endif");
    #endif

        }
#endif
    }

#if (OUTPUT_TYPE != OUT_LLASM)
    fclose(EF.fout);

    return 0;
#endif

#if (OUTPUT_TYPE == OUT_LLASM)
    fprintf(EF.fout, "\n");

    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        if (section[EF.Entry].type == ST_CODE)
        {
            section_output_list_ForEach(EF.Entry, &SR_write_llasm_output_function, (void *) &EF);
            fprintf(EF.fout, "\n");
        }
    }

    for (EF.Entry = 0; EF.Entry < num_sections; EF.Entry++)
    {
        if (section[EF.Entry].type == ST_CODE)
        {
            FILE *fmain;
            char incname[20];

            sprintf((char *) &(incname[0]), "seg%.2i_code.llinc", (unsigned int)(EF.Entry + 1));

            fprintf(EF.fout, "include %s\n", (char *) &(incname[0]));

            fmain = EF.fout;

            EF.fout = fopen((char *) &(incname[0]), "wt");

            if ( EF.fout == NULL ) return -3;

            section_output_list_ForEach(EF.Entry, &SR_write_llasm_output_line, (void *) &EF);

            fclose(EF.fout);

            EF.fout = fmain;
        }
    }

    fclose(EF.fout);

    return 0;
#endif
}

