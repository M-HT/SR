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

#if !defined(_SR_VARS_H_INCLUDED_)
#define _SR_VARS_H_INCLUDED_

#include <udis86.h>

#include "SR_defs.h"

#if defined(DEFINE_VARIABLES)
	#define EXTERNAL_VARIABLE
#else
	#define EXTERNAL_VARIABLE extern
#endif

EXTERNAL_VARIABLE section_data section[MAX_SECTIONS];   // array of sections
EXTERNAL_VARIABLE unsigned int num_sections;            // number of sections
EXTERNAL_VARIABLE uint8_t *SR_CodeBase;                 // allocated data

EXTERNAL_VARIABLE uint_fast32_t EIPObjectNum;           // initial section
EXTERNAL_VARIABLE uint_fast32_t EIP;                    // initial offset

EXTERNAL_VARIABLE ud_t ud_obj;                          // udis86 object

EXTERNAL_VARIABLE int list_invalid_code_fixups;
EXTERNAL_VARIABLE char *invalid_code_fixups_name;

EXTERNAL_VARIABLE int list_data_to_code_fixups;
EXTERNAL_VARIABLE char *data_to_code_fixups_name;

EXTERNAL_VARIABLE int add_tracing_to_code;

EXTERNAL_VARIABLE int esp_dword_aligned;
EXTERNAL_VARIABLE int ebp_dword_aligned;

EXTERNAL_VARIABLE Pvoid_t import_list;

#include "judy_helpers.h"

int SR_get_section_reladr(uint_fast32_t Address, uint_fast32_t *SecNum, uint_fast32_t *RelAdr);

void SR_get_label(char *cbuf, uint_fast32_t Address);

int SRW_LoadFile(const char *fname);

int SR_LoadSCI(void);

int SR_initial_disassembly(void);
int SR_apply_fixup_info(void);
int SR_full_disassembly(void);
int SR_write_output(const char *fname);



output_data *SR_disassemble_offset_init_output(unsigned int SecNum, uint_fast32_t offset, unsigned int length);

void SR_disassemble_add_address(unsigned int Entry, uint32_t address);
void SR_disassemble_align_fixup(const fixup_data *fixup);
int SR_disassemble_convert_cjump(char *dst, const char *modifier, uint_fast32_t address, extrn_data *extrn);
int SR_disassemble_convert_fixup(const char *ostr, char *dst, fixup_data *fixup, extrn_data *extrn, int Entry, uint_fast32_t offset, int decoded_length);
int SR_disassemble_find_noret(uint_fast32_t address);

extrn_data *SR_disassemble_find_proc(unsigned int Entry, uint_fast32_t address);

int SR_disassemble_offset_windows(unsigned int Entry, uint_fast32_t offset);
int SR_disassemble_offset_win32(unsigned int Entry, uint_fast32_t offset);
int SR_disassemble_region_llasm(unsigned int Entry, region_data *region);

int SR_disassemble_llasm_instruction(unsigned int Entry, output_data *output, uint_fast32_t flags_to_write, uint_fast32_t *pflags_write, uint_fast32_t *pflags_read, int *plast_instruction);

#undef EXTERNAL_VARIABLE

#endif
