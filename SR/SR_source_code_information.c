/**
 *
 *  Copyright (C) 2016-2018 Roman Pauer
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

static int SR_LoadSCI_fixup_interpret_as_code(FILE *file)
{
    char buf[8192];
    uint_fast32_t SecNum, RelAdr;
    int length;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            section_code_list_Insert(SecNum, RelAdr);
        }
    }

    return 0;
}

static int SR_LoadSCI_displaced_labels(FILE *file)
{
    char buf[8192];
    int *label_value;
    uint_fast32_t SecNum, RelAdr;
    int length, displacement;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X,%i", &address, &displacement);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            label_value = section_label_list_Insert(SecNum, RelAdr);

            if (label_value == NULL) return 1;

            *label_value = displacement;
        }
    }

    return 0;
}

static int SR_LoadSCI_noret_procedures(FILE *file)
{
    char buf[8192];
    uint_fast32_t SecNum, RelAdr;
    int length;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            section_noret_list_Insert(SecNum, RelAdr);
        }
    }

    return 0;
}

static int SR_LoadSCI_code16_areas(FILE *file)
{
    char buf[8192];
    int *code16_area_value;
    uint_fast32_t SecNum, RelAdr;
    int length, arealength;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X,%i", &address, &arealength);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            code16_area_value = section_code16_list_Insert(SecNum, RelAdr);

            if (code16_area_value == NULL) return 1;

            *code16_area_value = arealength;
        }
    }

    return 0;
}

static int SR_LoadSCI_external_procedures(FILE *file)
{
    char buf[8192];
    char *str1, *str2;
    extrn_data *extrn;
    uint_fast32_t SecNum, RelAdr;
    int length;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 == NULL) continue;
        *str1 = 0;
        str1++;

        str2 = strchr(str1, ',');
        if (str2 != NULL)
        {
            *str2 = 0;
            str2++;

            while (*str2 == ' ') str2++;
            if (*str2 == 0) str2 = NULL;
        }

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            extrn = section_extrn_list_Insert(SecNum, RelAdr, str1, str2);
            if (extrn == NULL) return 1;
        }
    }

    return 0;
}

static int SR_LoadSCI_global_aliases(FILE *file)
{
    char buf[8192];
    char *str1;
    alias_data *alias;
    uint_fast32_t SecNum, RelAdr;
    int length;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 == NULL) continue;
        *str1 = 0;
        str1++;

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            alias = section_alias_list_Insert(SecNum, RelAdr, str1);
            if (alias == NULL) return 1;
        }
    }

    return 0;
}

static int SR_LoadSCI_instruction_replacements(FILE *file)
{
    char buf[8192];
    char *str1, *str2, *str3;
    replace_data *replace, **replace_value;
    uint_fast32_t SecNum, RelAdr;
    int length;
    unsigned int address, instr_len;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 == NULL) continue;
        *str1 = 0;
        str1++;

        str2 = strchr(str1, ',');

        if (str2 == NULL) continue;
        *str2 = 0;
        str2++;

        str3 = strchr(str2, '|');
        while (str3 != NULL)
        {
            *str3 = '\n';
            str3++;

            str3 = strchr(str3, '|');
        }

        sscanf(buf, "loc_%X", &address);
        sscanf(str1, "%i", &instr_len);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            replace = section_replace_list_Insert(SecNum, RelAdr, str2, instr_len);
            if (replace == NULL) return 1;
        }
    }

    return 0;
}

static int SR_LoadSCI_unaligned_ebp_areas(FILE *file)
{
    char buf[8192];
    int *ua_ebp_area_value;
    uint_fast32_t SecNum, RelAdr;
    int length, arealength;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X,%i", &address, &arealength);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            ua_ebp_area_value = section_ua_ebp_list_Insert(SecNum, RelAdr);

            if (ua_ebp_area_value == NULL) return 1;

            *ua_ebp_area_value = arealength;
        }
    }

    return 0;
}

static int SR_LoadSCI_unaligned_esp_areas(FILE *file)
{
    char buf[8192];
    int *ua_esp_area_value;
    uint_fast32_t SecNum, RelAdr;
    int length, arealength;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X,%i", &address, &arealength);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            ua_esp_area_value = section_ua_esp_list_Insert(SecNum, RelAdr);

            if (ua_esp_area_value == NULL) return 1;

            *ua_esp_area_value = arealength;
        }
    }

    return 0;
}

static int SR_LoadSCI_fixup_do_not_interpret_as_code(FILE *file)
{
    char buf[8192];
    uint_fast32_t SecNum, RelAdr;
    int length;
    unsigned int address;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        sscanf(buf, "loc_%X", &address);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            section_nocode_list_Insert(SecNum, RelAdr);
        }
    }

    return 0;
}

static int SR_LoadSCI_instruction_flags(FILE *file)
{
    char buf[8192];
    char *str1, *str2, *str3;
    bound_data *iflags;
    uint_fast32_t SecNum, RelAdr;
    int length;
    unsigned int address, to_set, to_clear;

    while (!feof(file))
    {
        // read enters
        fscanf(file, "%8192[\n]", buf);
        // read line
        buf[0] = 0;
        fscanf(file, "%8192[^\n]", buf);
        length = strlen(buf);
        if (length != 0 && buf[length - 1] == '\r')
        {
            length--;
            buf[length] = 0;
        }

        if (length == 0) continue;

        str1 = strchr(buf, ',');

        if (str1 == NULL) continue;
        *str1 = 0;
        str1++;

        str2 = strchr(str1, ',');

        if (str2 == NULL) continue;
        *str2 = 0;
        str2++;

        str3 = strchr(str2, ';');

        if (str3 != NULL)
        {
            *str3 = 0;
        }

        sscanf(buf, "loc_%X", &address);
        sscanf(str1, "%i", &to_set);
        sscanf(str2, "%i", &to_clear);

        if (SR_get_section_reladr(address, &SecNum, &RelAdr))
        {
            iflags = section_iflags_list_Insert(SecNum, RelAdr, to_set, to_clear);
            if (iflags == NULL) return 1;
        }
    }

    return 0;
}

int SR_LoadSCI(void)
{
    const static char fixup_interpret_as_code[] = "fixup_interpret_as_code.sci";
    const static char displaced_labels[] = "displaced_labels.sci";
    const static char noret_procedures[] = "noret_procedures.sci";
    const static char code16_areas[] = "code16_areas.sci";
    const static char external_procedures[] = "external_procedures.sci";
    const static char global_aliases[] = "global_aliases.sci";
    const static char instruction_replacements[] = "instruction_replacements.sci";
    const static char instruction_replacements_FPU[] = "instruction_replacements_FPU.sci";
    const static char unaligned_ebp_areas[] = "unaligned_ebp_areas.sci";
    const static char unaligned_esp_areas[] = "unaligned_esp_areas.sci";
    const static char fixup_do_not_interpret_as_code[] = "fixup_do_not_interpret_as_code.sci";
    const static char instruction_flags[] = "instruction_flags.sci";
    FILE *f;
    int ret;

    f = fopen(fixup_interpret_as_code, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", fixup_interpret_as_code);

        ret = SR_LoadSCI_fixup_interpret_as_code(f);

        fclose(f);

        if (ret) return ret;
    }

    f = fopen(displaced_labels, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", displaced_labels);

        ret = SR_LoadSCI_displaced_labels(f);

        fclose(f);

        if (ret) return ret;
    }

    f = fopen(noret_procedures, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", noret_procedures);

        ret = SR_LoadSCI_noret_procedures(f);

        fclose(f);

        if (ret) return ret;
    }

    f = fopen(code16_areas, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", code16_areas);

        ret = SR_LoadSCI_code16_areas(f);

        fclose(f);

        if (ret) return ret;
    }

    f = fopen(fixup_do_not_interpret_as_code, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", fixup_do_not_interpret_as_code);

        ret = SR_LoadSCI_fixup_do_not_interpret_as_code(f);

        fclose(f);

        if (ret) return ret;
    }

#if (OUTPUT_TYPE != OUT_DOS)
    f = fopen(external_procedures, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", external_procedures);

        ret = SR_LoadSCI_external_procedures(f);

        fclose(f);

        if (ret) return ret;
    }

    f = fopen(global_aliases, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", global_aliases);

        ret = SR_LoadSCI_global_aliases(f);

        fclose(f);

        if (ret) return ret;
    }

    f = fopen(instruction_replacements, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", instruction_replacements);

        ret = SR_LoadSCI_instruction_replacements(f);

        fclose(f);

        if (ret) return ret;
    }

    f = fopen(instruction_replacements_FPU, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", instruction_replacements_FPU);

        ret = SR_LoadSCI_instruction_replacements(f);

        fclose(f);

        if (ret) return ret;
    }

#if (OUTPUT_TYPE == OUT_ARM_LINUX)

    f = fopen(unaligned_ebp_areas, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", unaligned_ebp_areas);

        ret = SR_LoadSCI_unaligned_ebp_areas(f);

        fclose(f);

        if (ret) return ret;
    }

    f = fopen(unaligned_esp_areas, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", unaligned_esp_areas);

        ret = SR_LoadSCI_unaligned_esp_areas(f);

        fclose(f);

        if (ret) return ret;
    }


#endif

    f = fopen(instruction_flags, "rt");

    if (f != NULL)
    {
        fprintf(stderr, "\tLoading %s...\n", instruction_flags);

        ret = SR_LoadSCI_instruction_flags(f);

        fclose(f);

        if (ret) return ret;
    }
#endif

    return 0;
}
