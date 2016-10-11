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

#if !defined(_SR_DEFS_H_INCLUDED_)
#define _SR_DEFS_H_INCLUDED_

#include <inttypes.h>
#include <Judy.h>

#define MAX_SECTIONS 32

#define OUT_ORIG 0
#define OUT_DOS 1
#define OUT_X86 2
#define OUT_ARM_LINUX 4

#define OUTPUT_TYPE  OUT_X86


#if (OUTPUT_TYPE != OUT_ORIG)
    //#define _CHK_HACK 1
    #undef _CHK_HACK
#endif

#if (OUTPUT_TYPE == OUT_ARM_LINUX)
    //#define EMULATE_FPU 0
    #define EMULATE_FPU 1
#endif

typedef enum _output_type_ {
    OT_UNITIALIZED = -1,
    OT_UNKNOWN     =  0,
    OT_OFFSET      =  1,
    OT_INSTRUCTION =  2,
    OT_NONE        =  3         // used for label at section end
} output_type;

typedef enum _section_type_ {
    ST_CODE  = 0,
    ST_DATA  = 1,
    ST_STACK = 2,
    ST_UDATA = 3
} section_type;

typedef enum _fixup_type_ {
    FT_NORMAL   = 0,
    FT_SELFREL  = 1,
    FT_SEGMENT  = 2,
    FT_16BITOFS = 3,
    FT_SEGOFS32 = 4
} fixup_type;


typedef struct _fixup_data_ {
    uint_fast32_t sofs;         // source offset
    int_fast32_t tofs;          // target offset
    uint_fast32_t tsec;         // target section
    fixup_type    type;         // fixup type
} fixup_data;

typedef struct _output_data_ {
    uint_fast32_t ofs;          // output offset
    uint_fast32_t len;          // output length in memory (number of bytes - 1)
    char          *str;         // output string
    int           has_label;    // has label ?
    int           align;        // align directive
    output_type   type;         // output type: 0 - unknown (1 byte data), 1 - data offset (4 bytes), 2 - instruction
} output_data;

typedef struct _extrn_data_ {
    uint_fast32_t ofs;          // procedure offset
    char          *altaction;   // alternative action
    char          proc[1];      // external procedure name
} extrn_data, alias_data;

typedef struct _replace_data_ {
    uint_fast32_t ofs;          // instructions offset
    uint_fast32_t length;       // instructions length (bytes)
    char          instr[1];     // replacement code
} replace_data;

typedef struct _bound_data_ {
    uint_fast32_t ofs;          // bound offset
    int           begin;        // is begin bound ?
    int           end;          // is end bound ?
} bound_data;

typedef struct _region_data_ {
    uint_fast32_t begin_ofs;    // region begin offset
    uint_fast32_t end_ofs;      // region end offset
} region_data;

typedef struct _section_data_ {
    uint_fast32_t size;         // section size
    uint_fast32_t size_in_file; // section size stored in file
    uint_fast32_t start;        // section relocation base address
    uint8_t       *adr;         // section address in memory
    uint_fast32_t flags;        // section flags
    Pvoid_t fixup_list;         // list of fixups for section
    Pvoid_t output_list;        // list of output lines for section
    Pvoid_t entry_list;         // list of entries for section
    Pvoid_t code_list;          // list of valid code entries for section
    Pvoid_t label_list;         // list of displaced labels for section
    Pvoid_t noret_list;         // list of noret procedures for section
    Pvoid_t extrn_list;         // list of external procedures for section
    Pvoid_t alias_list;         // list of global aliases for section
    Pvoid_t replace_list;       // list of instruction replacements for section
    Pvoid_t _chk_list;          // list of procedures with _chk check for section
    Pvoid_t bound_list;         // list of bounds for section
    Pvoid_t region_list;        // list of code regions for section
    Pvoid_t nocode_list;        // list of invalid code entries for section
    Pvoid_t iflags_list;        // list of instruction flags for section
    Pvoid_t code16_list;        // list of 16-bit code areas
    Pvoid_t ua_ebp_list;        // list of unaligned ebp areas
    Pvoid_t ua_esp_list;        // list of unaligned esp areas
    Pvoid_t code2data_list;     // list of targets of references from instructions to data
    Pvoid_t data2code_list;     // list of targets of references from data to instructions
    section_type type;          // section type: 0 - code, 1 - data, 2 - stack, 3 - uninitialized data
    char name[12];              // section name
} section_data;

#endif
