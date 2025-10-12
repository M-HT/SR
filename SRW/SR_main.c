/**
 *
 *  Copyright (C) 2016-2025 Roman Pauer
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
#include <time.h>
#include <string.h>

#if (defined(__WIN32__) || defined(__WINDOWS__)) && !defined(_WIN32)
#define _WIN32
#endif

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN

	#include <windows.h>
#else
	#include <unistd.h>
#endif

#include "SR_defs.h"

#define DEFINE_VARIABLES
	#include "SR_vars.h"
#undef DEFINE_VARIABLES

#if defined(_WIN32)

static void write_log_time(FILE *fout)
{
	SYSTEMTIME st;

	GetSystemTime(&st);

	fprintf(fout, "%04i-%02i-%02i %02i:%02i:%02i.%03i: ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

#else
	#include <sys/time.h>

static void write_log_time(FILE *fout)
{
	struct tm *newtime;
	struct timeval current_time;
	char str[63];

	gettimeofday(&current_time, NULL);						// Get time in seconds and microseconds
	newtime = gmtime( &current_time.tv_sec );				// Convert time (seconds) to struct
	strftime (str, 63, "%Y-%m-%d %H:%M:%S", newtime);		// Write date and time (seconds) to string (UTC)

	fprintf(fout, "%s.%03i: ", str, (int) (current_time.tv_usec / 1000));
}

#endif


int SR_get_section_reladr(uint_fast32_t Address, uint_fast32_t *SecNum, uint_fast32_t *RelAdr)
{
	unsigned int Entry;

	for (Entry = 0; Entry < num_sections; Entry++)
	{
		if (Address >= section[Entry].start &&
			Address < section[Entry].start + section[Entry].size)
		{
			*SecNum = Entry;
			*RelAdr = Address - section[Entry].start;

			return 1;
		}
	}
	for (Entry = 0; Entry < num_sections; Entry++)
	{
		if (Address == section[Entry].start + section[Entry].size)
		{
			*SecNum = Entry;
			*RelAdr = Address - section[Entry].start;

			return 1;
		}
	}
	for (Entry = num_sections; Entry > 0; Entry--)
	{
		if (Address >= section[Entry - 1].start)
		{
			if (Entry == num_sections) break;

			*SecNum = Entry - 1;
			*RelAdr = Address - section[Entry - 1].start;

			return 1;
		}
	}
	*SecNum = 0;
	*RelAdr = 0;
	return 0;
}

static char *trim_string(char *buf)
{
    size_t len;

    while (*buf == ' ') buf++;

    len = strlen(buf);
    if (len != 0 && buf[len - 1] == '\r')
    {
        len--;
        buf[len] = 0;
    }

    while (len != 0 && buf[len - 1] == ' ')
    {
        len--;
        buf[len] = 0;
    }

    return buf;
}

static void read_cfg(void)
{
    const static char config_filename[] = "SR.cfg";

    FILE *f;
    char buf[8192];
    char *str, *param;
    int items;

    f = fopen(config_filename, "rt");

    if (f == NULL) return;

    while (!feof(f))
    {
        /* skip empty lines */
        items = fscanf(f, "%8191[\n\r]", buf);

        /* read line */
        buf[0] = 0;
        items = fscanf(f, "%8191[^\n^\r]", buf);
        if (items <= 0) continue;

        /* trim line */
        str = trim_string(buf);

        if (str[0] == 0) continue;
        if (str[0] == '#') continue;

        /* find parameter (after '=') */
        param = strchr(str, '=');

        if (param == NULL) continue;

        /* split string into two strings */
        *param = 0;
        param++;

        /* trim them */
        str = trim_string(str);
        param = trim_string(param);

        if ( strcasecmp(str, "esp_dword_aligned") == 0 ) /* str equals "esp_dword_aligned" */
        {
            if ( strcasecmp(param, "yes") == 0 ) /* param equals "yes" */
            {
                esp_dword_aligned = 1;
            }
            else if ( strcasecmp(param, "no") == 0 ) /* param equals "no" */
            {
                esp_dword_aligned = 0;
            }
        }
        else if ( strcasecmp(str, "ebp_dword_aligned") == 0 ) /* str equals "ebp_dword_aligned" */
        {
            if ( strcasecmp(param, "yes") == 0 ) /* param equals "yes" */
            {
                ebp_dword_aligned = 1;
            }
            else if ( strcasecmp(param, "no") == 0 ) /* param equals "no" */
            {
                ebp_dword_aligned = 0;
            }
        }
    }

    fclose(f);
}

static void deinitialize_values(void)
{
	unsigned int Entry;

	for (Entry = 0; Entry < num_sections; Entry++)
	{
		section_fixup_list_Free(Entry);
		section_output_list_Free(Entry);
		section_entry_list_Free(Entry);
		section_code_list_Free(Entry);
		section_label_list_Free(Entry);
		section_noret_list_Free(Entry);
		section_extrn_list_Free(Entry);
		section_alias_list_Free(Entry);
		section_ignored_list_Free(Entry);
		section_replace_list_Free(Entry);
		section_bound_list_Free(Entry);
		section_region_list_Free(Entry);
		section_nocode_list_Free(Entry);
		section_iflags_list_Free(Entry);
		section_code16_list_Free(Entry);
		section_ua_ebp_list_Free(Entry);
		section_ua_esp_list_Free(Entry);
		section_export_list_Free(Entry);
	}
	num_sections = 0;

	import_list_Free();

	if (SR_CodeBase != NULL)
	{
		free(SR_CodeBase);
		SR_CodeBase = NULL;
	}
}

static void initialize_values(void)
{
	num_sections = 0;
	SR_CodeBase = NULL;
	list_invalid_code_fixups = 0;
	invalid_code_fixups_name = NULL;
	list_data_to_code_fixups = 0;
	data_to_code_fixups_name = NULL;
	add_tracing_to_code = 0;

	esp_dword_aligned = 0;
	ebp_dword_aligned = 0;

	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
}

int main (int argc, char *argv[])
{
	int ret;
	char *input_name, *output_name;

#ifdef _WIN32
	SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
#else
	if (-1 == nice(1)) {}
#endif

#if (OUTPUT_TYPE == OUT_ORIG)
	fprintf(stderr, "Output type: Original\n");
#elif (OUTPUT_TYPE == OUT_WINDOWS)
	fprintf(stderr, "Output type: Windows version\n");
#elif (OUTPUT_TYPE == OUT_X86)
	fprintf(stderr, "Output type: X86 version(s)\n");
#elif (OUTPUT_TYPE == OUT_LLASM)
	fprintf(stderr, "Output type: LLasm version\n");
#else
	#error unknown output type
#endif


	initialize_values();

	read_cfg();

	fprintf(stderr, "esp dword aligned: %s\n", (esp_dword_aligned)?"yes":"no");
	fprintf(stderr, "ebp dword aligned: %s\n", (ebp_dword_aligned)?"yes":"no");

	input_name = NULL;
	output_name = NULL;

	argv++;
	argc--;
	while (argc)
	{
		if ((*argv)[0] == '-')
		{
			if (strncasecmp(*argv, "--list_invalid_code_fixups=", 27) == 0)
			{
				list_invalid_code_fixups = 1;
				invalid_code_fixups_name = &((*argv)[27]);
			}
			else if (strncasecmp(*argv, "--list_data_to_code_fixups=", 27) == 0)
			{
				list_data_to_code_fixups = 1;
				data_to_code_fixups_name = &((*argv)[27]);
			}
			else if (strcasecmp(*argv, "--add_tracing") == 0)
			{
				add_tracing_to_code = 1;
			}
		}
		else
		{
			if (input_name == NULL) input_name = *argv;
			else if (output_name == NULL) output_name = *argv;
		}
		argv++;
		argc--;
	} ;

    if (list_invalid_code_fixups) list_data_to_code_fixups = 0;

	write_log_time(stderr);
	fprintf(stderr, "Loading EXE file...\n");

	ret = SRW_LoadFile(input_name);
	if (ret != 0)
	{
		fprintf(stderr, "Error: %i\n", ret);
		deinitialize_values();
		return 2;
	}

	fprintf(stderr, "\tSections in file: %i\n", num_sections);

#if (OUTPUT_TYPE != OUT_ORIG)
	write_log_time(stderr);
	fprintf(stderr, "Loading source code information...\n");

	ret = SR_LoadSCI();
	if (ret != 0)
	{
		fprintf(stderr, "Error: %i\n", ret);
		deinitialize_values();
		return 3;
	}
#endif

	write_log_time(stderr);
	fprintf(stderr, "Initial disassembly...\n");
	if ( SR_initial_disassembly() != 0 )
	{
		deinitialize_values();
		return 4;
	}

	write_log_time(stderr);
	fprintf(stderr, "Applying fixup...\n");
	if ( SR_apply_fixup_info() != 0 )
	{
		deinitialize_values();
		return 5;
	}

#if (OUTPUT_TYPE != OUT_ORIG)
	write_log_time(stderr);
	fprintf(stderr, "Full disassembly...\n");
	ret = SR_full_disassembly();
	if (ret != 0)
	{
		fprintf(stderr, "Error: %i\n", ret);
		deinitialize_values();
		return 6;
	}
#endif

	write_log_time(stderr);
	fprintf(stderr, "Writing output...\n");

	ret = SR_write_output(output_name);

	if (ret != 0)
	{
		fprintf(stderr, "Error: %i\n", ret);
		deinitialize_values();
		return 7;
	}

	write_log_time(stderr);
	fprintf(stderr, "Finishing...\n");

	deinitialize_values();

	return 0;
}
