/**
 *
 *  Copyright (C) 2016-2024 Roman Pauer
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <strings.h>
#ifdef USE_SPEEXDSP_RESAMPLER
#include <speex/speex_resampler.h>
#endif

#define ADLIB 1
#define ROLAND 2
#define EMU8000 3

#if (DRIVER==ADLIB)
#include "emu_opl2.h"
#define BASE_SAMPLE_RATE 49716
#define RESAMPLER_BUFFER_LEN 728
#elif (DRIVER==ROLAND)
#include "emu_mt32.h"
#define BASE_SAMPLE_RATE 32000
#define SYNTH_BUFFER_LEN 400
#define RESAMPLER_BUFFER_LEN 416
#elif (DRIVER==EMU8000)
#include "emu_awe32.h"
#define BASE_SAMPLE_RATE 44100
#define SYNTH_BUFFER_LEN 630
#define RESAMPLER_BUFFER_LEN 640
#endif


#define SEGMENT_VALUE 0x1000

#define X86_REG_8_HILO(regnum, hilo) x86.regs8[(hilo) + 4*(regnum)]
#define X86_REG_8(regnum) X86_REG_8_HILO((regnum) & 3, (regnum) >> 2)
#define X86_REG_16(regnum) x86.regs16[2*(regnum)]

#define PEEK_PREV_INSTRUCTION_BYTE (memory[x86.ip-1])
#define GET_INSTRUCTION_UINT8 (memory[x86.ip++])
#define GET_INSTRUCTION_INT8 (((int8_t *)memory)[x86.ip++])
#define READ_INSTRUCTION_UINT16_TO(dst) do { dst = *((uint16_t *)&(memory[x86.ip])); x86.ip += 2; } while (0)
#define READ_INSTRUCTION_UINT32_TO(dst) do { dst = *((uint32_t *)&(memory[x86.ip])); x86.ip += 4; } while (0)

#define GET_MEMORY_ADDR(addr) ((void *)&(memory[addr]))
#define READ_MEMORY_UINT16(addr) (*((uint16_t *)&(memory[addr])))
#define READ_MEMORY_UINT32(addr) (*((uint32_t *)&(memory[addr])))
#define WRITE_MEMORY_UINT16(addr, value) *((uint16_t *)&(memory[addr])) = (value)
#define WRITE_MEMORY_UINT32(addr, value) *((uint32_t *)&(memory[addr])) = (value)

enum {
    op_none,

    op_addw,
    op_adcw, // only when carry flag is set, otherwise op_addw is used
    op_subw,
    op_sbbw, // only when carry flag is set, otherwise op_subw is used
    op_xorw,
    op_incw,
    op_decw,
    op_negw,
    op_mulw,
    op_imulw,
    op_sarw,
    op_shrw,
    op_shlw,
    op_divw,

    op_addb,
    op_subb,
    op_xorb,
    op_incb,
    op_decb,
    op_negb,
    op_shrb,
    op_shlb,

    op_subd,
    op_xord,
};

static struct {
    union {
        uint32_t regs[10];
        uint16_t regs16[20];
        uint8_t regs8[40];
        struct {
            uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi, eip, eflags;
        };
        struct {
            uint16_t ax, ax2, cx, cx2, dx, dx2, bx, bx2, sp, sp2, bp, bp2, si, si2, di, di2, ip, ip2, flags, flags2;
        };
        struct {
            uint8_t al, ah, a3, a4, cl, ch, c3, c4, dl, dh, d3, d4, bl, bh, b3, b4;
        };
    };
    union {
        uint16_t segregs[6];
        struct {
            uint16_t es, cs, ss, ds, fs, gs;
        };
    };
    uint32_t modrm_reg;
    struct {
        int oper;
        union {
            uint32_t dwop[3];
            struct {
                uint32_t dwop1, dwop2, dwres;
            };
            struct {
                uint16_t wop1, wopx1, wop2, wopx2, wres, wres2;
            };
            struct {
                uint8_t bop1, bopx11, bopx12, bopx13, bop2, bopx21, bopx22, bopx3, bres, bresx1, bresx2, bresx3;
            };
        };
    } fl;
    uint16_t int66_offset;
    uint16_t resident_program_size;
    int terminate;
#if (DRIVER==ROLAND)
    uint8_t *lapc1_pat;
    int lapc1_pat_handle;
    uint32_t lapc1_pat_len, lapc1_pat_ofs;
#endif
} x86;

static uint8_t *memory;

#ifdef USE_SPEEXDSP_RESAMPLER
static SpeexResamplerState *resampler = NULL;
static uint_fast64_t resample_step;
static int resample_num_samples;
static int16_t resample_samples[2*RESAMPLER_BUFFER_LEN];
#endif

#if (DRIVER==ADLIB)
static uint_fast32_t sample_rate, num_samples_add, num_samples_left;
#elif (DRIVER==ROLAND) || (DRIVER==EMU8000)
static uint_fast32_t sample_rate, num_samples_left;
static uint_fast64_t position_add, current_position, samples_mul;
#ifndef USE_SPEEXDSP_RESAMPLER
static int16_t resample_samples[2*(SYNTH_BUFFER_LEN+1)];
#endif
#endif

static int initialize(void)
{
    memory = (uint8_t *)malloc(65536);
    if (memory == NULL) return 0;

    memset(memory, 0, 65536);

    memory[0] = 0xcd; // int 20h
    memory[1] = 0x20;
    memory[4] = 0xcf; // iret
    WRITE_MEMORY_UINT16(0x2c, SEGMENT_VALUE); // this is wrong value, but all segments should be the same in this emulator

    memset(&x86, 0, sizeof(x86));

    x86.esp = 0xfffe;
    x86.eip = 0x100;
    x86.eflags = 0x3202;
    x86.es = x86.cs = x86.ss = x86.ds = x86.fs = x86.gs = SEGMENT_VALUE;
    x86.int66_offset = 4;

    return 1;
}

static int loadfilebuffer(const void *filebuffer, unsigned int filelen)
{
    if (filebuffer == NULL) return 0;
    if ((filelen == 0) || (filelen > 60000)) return 0;

    memcpy(GET_MEMORY_ADDR(0x100), filebuffer, filelen);
    return 1;
}

/*static void loadfile(const char *filename)
{
    FILE *f;
    size_t filelen;

    f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    filelen = ftell(f);
    fseek(f, 0, SEEK_SET);

    fread(GET_MEMORY_ADDR(0x100), 1, filelen, f);
    fclose(f);
}*/

static void check_segment_value(uint16_t value)
{
    if (value != SEGMENT_VALUE)
    {
        fprintf(stderr, "Wrong segment value: 0x%x ip:0x%x\n", value, x86.ip);
        exit(1);
    }
}

static unsigned int x86_get_flag_c(void)
{
    switch (x86.fl.oper)
    {
        case 0:
        case op_incw:
        case op_decw:
        case op_incb:
        case op_decb:
            return (x86.eflags & 1);
        case op_addw:
            return (x86.fl.wres < x86.fl.wop1)?1:0;
        case op_adcw:
            return (x86.fl.wres <= x86.fl.wop1)?1:0;
        case op_subw:
            return (x86.fl.wres > x86.fl.wop1)?1:0;
        case op_sbbw:
            return (x86.fl.wres >= x86.fl.wop1)?1:0;
        case op_negw:
            return (x86.fl.wres != 0)?1:0;
        case op_mulw:
            return ((x86.fl.dwres & 0xffff0000) == 0)?0:1;
        case op_imulw:
            return (((x86.fl.dwres & 0xffff8000) == 0xffff8000) || ((x86.fl.dwres & 0xffff8000) == 0))?0:1;
        case op_sarw:
            return ((((int16_t)x86.fl.wop1) >> (x86.fl.bop2 - 1)) & 1)?1:0;
        case op_shrw:
            return ((x86.fl.wop1 >> (x86.fl.bop2 - 1)) & 1)?1:0;
        case op_shlw:
            return ((x86.fl.wop1 << (x86.fl.bop2 - 1)) & 0x8000)?1:0;
        case op_addb:
            return (x86.fl.bres < x86.fl.bop1)?1:0;
        case op_subb:
            return (x86.fl.bres > x86.fl.bop1)?1:0;
        case op_negb:
            return (x86.fl.bres != 0)?1:0;
        case op_shrb:
            return ((x86.fl.bop1 >> (x86.fl.bop2 - 1)) & 1)?1:0;
        case op_shlb:
            return ((x86.fl.bop1 << (x86.fl.bop2 - 1)) & 0x80)?1:0;
        case op_subd:
            return (x86.fl.dwres > x86.fl.dwop1)?1:0;
        case op_xorw:
        case op_divw:
        case op_xorb:
        case op_xord:
        default:
            return 0;
    };
}

static unsigned int x86_get_flag_o(void)
{
    switch (x86.fl.oper)
    {
        case 0:
            return (x86.eflags & 0x800)?1:0;
        case op_addw:
        case op_adcw:
            return (((x86.fl.wres ^ x86.fl.wop1) & (x86.fl.wres ^ x86.fl.wop2)) & 0x8000)?1:0;
        case op_subw:
        case op_sbbw:
            return (((x86.fl.wres ^ x86.fl.wop1) & (x86.fl.wop1 ^ x86.fl.wop2)) & 0x8000)?1:0;
        case op_incw:
        case op_negw:
            return (x86.fl.wres == 0x8000)?1:0;
        case op_decw:
            return (x86.fl.wres == 0x7fff)?1:0;
        case op_mulw:
            return ((x86.fl.dwres & 0xffff0000) == 0)?0:1;
        case op_imulw:
            return (((x86.fl.dwres & 0xffff8000) == 0xffff8000) || ((x86.fl.dwres & 0xffff8000) == 0))?0:1;
        case op_shrw:
            return ((x86.fl.bop2 == 1) && (x86.fl.wop1 & 0x8000))?1:0;
        case op_shlw:
            return ((x86.fl.bop2 == 1) && ((x86.fl.wop1 ^ x86.fl.wres) & 0x8000))?1:0;
        case op_addb:
            return (((x86.fl.bres ^ x86.fl.bop1) & (x86.fl.bres ^ x86.fl.bop2)) & 0x80)?1:0;
        case op_subb:
            return (((x86.fl.bres ^ x86.fl.bop1) & (x86.fl.bop1 ^ x86.fl.bop2)) & 0x8000)?1:0;
        case op_incb:
        case op_negb:
            return (x86.fl.bres == 0x80)?1:0;
        case op_decb:
            return (x86.fl.bres == 0x7f)?1:0;
        case op_shrb:
            return ((x86.fl.bop2 == 1) && (x86.fl.bop1 & 0x80))?1:0;
        case op_shlb:
            return ((x86.fl.bop2 == 1) && ((x86.fl.bop1 ^ x86.fl.bres) & 0x80))?1:0;
        case op_subd:
            return (((x86.fl.dwres ^ x86.fl.dwop1) & (x86.fl.dwop1 ^ x86.fl.dwop2)) & 0x80000000)?1:0;
        case op_xorw:
        case op_sarw:
        case op_divw:
        case op_xorb:
        case op_xord:
        default:
            return 0;
    };
}

static unsigned int x86_get_flag_s(void)
{
    switch (x86.fl.oper)
    {
        case 0:
            return (x86.eflags & 0x80)?1:0;
        case op_addw:
        case op_adcw:
        case op_subw:
        case op_sbbw:
        case op_xorw:
        case op_incw:
        case op_decw:
        case op_negw:
        case op_sarw:
        case op_shrw:
        case op_shlw:
            return (x86.fl.wres & 0x8000)?1:0;
        case op_addb:
        case op_subb:
        case op_xorb:
        case op_incb:
        case op_decb:
        case op_negb:
        case op_shrb:
        case op_shlb:
            return (x86.fl.bres & 0x80)?1:0;
        case op_subd:
        case op_xord:
            return (x86.fl.dwres & 0x80000000)?1:0;
        case op_mulw:
        case op_imulw:
        case op_divw:
        default:
            return 0;
    };
}

static unsigned int x86_get_flag_z(void)
{
    switch (x86.fl.oper)
    {
        case 0:
            return (x86.eflags & 0x40)?1:0;
        case op_addw:
        case op_adcw:
        case op_subw:
        case op_sbbw:
        case op_xorw:
        case op_incw:
        case op_decw:
        case op_negw:
        case op_sarw:
        case op_shrw:
        case op_shlw:
            return (x86.fl.wres == 0)?1:0;
        case op_addb:
        case op_subb:
        case op_xorb:
        case op_incb:
        case op_decb:
        case op_negb:
        case op_shrb:
        case op_shlb:
            return (x86.fl.bres == 0)?1:0;
        case op_subd:
        case op_xord:
            return (x86.fl.dwres == 0)?1:0;
        case op_mulw:
        case op_imulw:
        case op_divw:
        default:
            return 0;
    };
}

static unsigned int x86_get_flag_p(void)
{
    switch (x86.fl.oper)
    {
        case 0:
            return (x86.eflags & 0x04)?1:0;
        case op_addw:
        case op_adcw:
        case op_subw:
        case op_sbbw:
        case op_xorw:
        case op_incw:
        case op_decw:
        case op_negw:
        case op_sarw:
        case op_shrw:
        case op_shlw:

        case op_addb:
        case op_subb:
        case op_xorb:
        case op_incb:
        case op_decb:
        case op_negb:
        case op_shrb:
        case op_shlb:

        case op_subd:
        case op_xord:
            {
                uint8_t parity = x86.fl.bres;
                parity = parity | (parity >> 4);
                parity = parity | (parity >> 2);
                parity = parity | (parity >> 1);
                return (parity & 1);
            }
        case op_mulw:
        case op_imulw:
        case op_divw:
        default:
            return 0;
    };
}

static unsigned int x86_get_flag_a(void)
{
    switch (x86.fl.oper)
    {
        case 0:
            return (x86.eflags & 0x10)?1:0;
        case op_addw:
        case op_adcw:
        case op_subw:
        case op_sbbw:
            return ((x86.fl.wop1 ^ x86.fl.wop2) ^ x86.fl.wres) & 0x10;

        case op_incw:
        case op_decw:
        case op_negw:
        case op_sarw:
        case op_shrw:
        case op_shlw:
            fprintf(stderr, "todo: x86_get_flag_a\n");
            return 0;

        case op_addb:
        case op_subb:
            return ((x86.fl.bop1 ^ x86.fl.bop2) ^ x86.fl.bres) & 0x10;

        case op_incb:
        case op_decb:
        case op_negb:
        case op_shrb:
        case op_shlb:

        case op_subd:

            fprintf(stderr, "todo: x86_get_flag_a\n");
            return 0;
        case op_xorw:
        case op_mulw:
        case op_imulw:
        case op_divw:
        case op_xorb:
        case op_xord:
        default:
            return 0;
    };
}

static void x86_calculate_carry_flag(void)
{
    if (x86.fl.oper)
    {
        x86.eflags = (x86.eflags & 0xfffffffe) | x86_get_flag_c();
        x86.fl.oper = 0;
    }
}

static void x86_calculate_flags(void)
{
    if (x86.fl.oper != 0)
    {
        unsigned int newflags;
        newflags = x86_get_flag_c() | (x86_get_flag_o() << 11) | (x86_get_flag_s() << 7) | (x86_get_flag_z() << 6) | (x86_get_flag_p() << 2) | (x86_get_flag_a() << 4);
        x86.flags = (x86.flags & ~(0x08d5)) | newflags;
        x86.fl.oper = 0;
    }
}

static uint16_t *x86_decode_modrm16(void)
{
    uint16_t offset;
    uint8_t modrm, mod, rm;
    modrm = GET_INSTRUCTION_UINT8;
    mod = modrm >> 6;
    x86.modrm_reg = (modrm >> 3) & 7;
    rm = modrm & 7;

    switch (mod)
    {
        case 0:
            if (rm != 6)
            {
                offset = 0;
                break;
            }
            else
            {
                READ_INSTRUCTION_UINT16_TO(offset);
                return (uint16_t *)GET_MEMORY_ADDR(offset);
            }
        case 1:
            offset = (int16_t)GET_INSTRUCTION_INT8;
            break;
        case 2:
            READ_INSTRUCTION_UINT16_TO(offset);
            break;
        case 3:
            return &(X86_REG_16(rm));
    }

    switch (rm)
    {
        case 0:
            offset += x86.bx + x86.si;
            break;
        case 1:
            offset += x86.bx + x86.di;
            break;
        case 2:
            offset += x86.bp + x86.si;
            break;
        case 3:
            offset += x86.bp + x86.di;
            break;
        case 4:
            offset += x86.si;
            break;
        case 5:
            offset += x86.di;
            break;
        case 6:
            offset += x86.bp;
            break;
        case 7:
            offset += x86.bx;
            break;
    }

    return (uint16_t *)GET_MEMORY_ADDR(offset);
}

static uint8_t *x86_decode_modrm8(void)
{
    uint16_t offset;
    uint8_t modrm, mod, rm;
    modrm = GET_INSTRUCTION_UINT8;
    mod = modrm >> 6;
    x86.modrm_reg = (modrm >> 3) & 7;
    rm = modrm & 7;

    switch (mod)
    {
        case 0:
            if (rm != 6)
            {
                offset = 0;
                break;
            }
            else
            {
                READ_INSTRUCTION_UINT16_TO(offset);
                return (uint8_t *)GET_MEMORY_ADDR(offset);
            }
        case 1:
            offset = (int16_t)GET_INSTRUCTION_INT8;
            break;
        case 2:
            READ_INSTRUCTION_UINT16_TO(offset);
            break;
        case 3:
            return &(X86_REG_8(rm));
    }

    switch (rm)
    {
        case 0:
            offset += x86.bx + x86.si;
            break;
        case 1:
            offset += x86.bx + x86.di;
            break;
        case 2:
            offset += x86.bp + x86.si;
            break;
        case 3:
            offset += x86.bp + x86.di;
            break;
        case 4:
            offset += x86.si;
            break;
        case 5:
            offset += x86.di;
            break;
        case 6:
            offset += x86.bp;
            break;
        case 7:
            offset += x86.bx;
            break;
    }

    return (uint8_t *)GET_MEMORY_ADDR(offset);
}

static void x86_interrupt(uint8_t intnum)
{
    switch (intnum)
    {
        case 0x21:
            switch (x86.ah)
            {
                case 0x25: // DOS 1+ - SET INTERRUPT VECTOR
                    if (x86.al == 0x66)
                    {
                        x86.int66_offset = x86.dx;
                    }
                    else
                    {
                        fprintf(stderr, "Unhandled interrupt 21 function 25: 0x%x ip: 0x%x\n", x86.al, x86.ip);
                        exit(1);
                    }
                    break;
                case 0x31: // DOS 2+ - TERMINATE AND STAY RESIDENT
                    x86.resident_program_size = x86.dx << 4;
                    x86.terminate = 1;
                    break;
                case 0x35: // DOS 2+ - GET INTERRUPT VECTOR
                    if (x86.al == 0x66)
                    {
                        x86.bx = x86.int66_offset;
                    }
                    else
                    {
                        fprintf(stderr, "Unhandled interrupt 21 function 35: 0x%x ip: 0x%x\n", x86.al, x86.ip);
                        exit(1);
                    }
                    break;
#if (DRIVER==ROLAND)
                case 0x3d: // DOS 2+ - OPEN - OPEN EXISTING FILE
                    x86_calculate_flags();
                    if (strcmp((char *)GET_MEMORY_ADDR(x86.dx), "sound\\lapc1.pat") == 0)
                    {
                        if (x86.lapc1_pat_handle == 0)
                        {
                            if (x86.al == 0)
                            {
                                if (x86.lapc1_pat != NULL)
                                {
                                    x86.lapc1_pat_handle = 5;
                                    x86.lapc1_pat_ofs = 0;

                                    x86.flags &= 0xfffe; // clear carry flag
                                    x86.ax = x86.lapc1_pat_handle;
                                }
                                else
                                {
                                    x86.flags |= 1; // set carry flag
                                    x86.ax = 2; // file not found
                                }
                            }
                            else
                            {
                                fprintf(stderr, "Unhandled interrupt 21 function 3d: 0x%x ip: 0x%x\n", x86.al, x86.ip);
                                exit(1);
                            }
                        }
                        else
                        {
                            x86.flags |= 1; // set carry flag
                            x86.ax = 4; // too many open files (no handles available)
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Unhandled interrupt 21 function 3d: 0x%x ip: 0x%x\n", x86.al, x86.ip);
                        exit(1);
                    }
                    break;
                case 0x3e: // DOS 2+ - CLOSE - CLOSE FILE
                    x86_calculate_flags();
                    if ((x86.bx == x86.lapc1_pat_handle) && (x86.lapc1_pat_handle != 0))
                    {
                        x86.lapc1_pat_handle = 0;

                        x86.flags &= 0xfffe; // clear carry flag
                    }
                    else
                    {
                        x86.flags |= 1; // set carry flag
                        x86.ax = 6; // invalid handle
                    }
                    break;
                case 0x3f: // DOS 2+ - READ - READ FROM FILE OR DEVICE
                    x86_calculate_flags();
                    if ((x86.bx == x86.lapc1_pat_handle) && (x86.lapc1_pat_handle != 0))
                    {
                        int readsize;

                        if (x86.lapc1_pat_len - x86.lapc1_pat_ofs >= x86.cx)
                        {
                            readsize = x86.cx;
                            memcpy(GET_MEMORY_ADDR(x86.dx), x86.lapc1_pat + x86.lapc1_pat_ofs, readsize);
                            x86.lapc1_pat_ofs += readsize;
                        }
                        else
                        {
                            readsize = x86.lapc1_pat_len - x86.lapc1_pat_ofs;
                            memcpy(GET_MEMORY_ADDR(x86.dx), x86.lapc1_pat + x86.lapc1_pat_ofs, readsize);
                            x86.lapc1_pat_ofs += readsize;
                        }

                        x86.flags &= 0xfffe; // clear carry flag
                        x86.ax = readsize;
                    }
                    else
                    {
                        x86.flags |= 1; // set carry flag
                        x86.ax = 6; // invalid handle
                    }
                    break;
#endif
               case 0x49: // DOS 2+ - FREE MEMORY
                    // ignore
                    x86_calculate_flags();
                    x86.flags &= 0xfffe; // clear carry flag
                    break;
                default:
                    fprintf(stderr, "Unhandled interrupt 21 function: 0x%x ip: 0x%x\n", x86.ah, x86.ip);
                    exit(1);
            }
            break;
        default:
            fprintf(stderr, "Unhandled interrupt: 0x%x ip: 0x%x\n", intnum, x86.ip);
            exit(1);
    }
}

static void x86_port_in8(uint16_t port)
{
    switch (port)
    {
#if (DRIVER==ADLIB)
        case 0x388:
            x86.al = emu_opl2_read_388();
            break;
        case 0x389:
            x86.al = emu_opl2_read_389();
            break;
#elif (DRIVER==ROLAND)
        case 0x330:
            x86.al = emu_mt32_read_330();
            break;
        case 0x331:
            x86.al = emu_mt32_read_331();
            break;
#endif
        default:
            fprintf(stderr, "Unhandled port in: 0x%x ip: 0x%x\n", port, x86.ip);
            exit(1);
    };
}

static void x86_port_in16(uint16_t port)
{
    switch (port)
    {
#if (DRIVER==EMU8000)
        case 0x620:
        case 0x621:
        case 0x622:
        case 0x623:
        case 0xa20:
        case 0xa21:
        case 0xa22:
        case 0xa23:
        case 0xe20:
        case 0xe21:
        case 0xe22:
        case 0xe23:
            x86.ax = emu_awe32_read16(port);
            break;
#endif
        default:
            fprintf(stderr, "Unhandled port in: 0x%x ip: 0x%x\n", port, x86.ip);
            exit(1);
    };
}

static void x86_port_out8(uint16_t port)
{
    switch (port)
    {
#if (DRIVER==ADLIB)
        case 0x388:
            emu_opl2_write_388(x86.al);
            break;
        case 0x389:
            emu_opl2_write_389(x86.al);
            break;
#elif (DRIVER==ROLAND)
        case 0x330:
            emu_mt32_write_330(x86.al);
            break;
        case 0x331:
            emu_mt32_write_331(x86.al);
            break;
#endif
        default:
            fprintf(stderr, "Unhandled port out: 0x%x ip: 0x%x\n", port, x86.ip);
            exit(1);
    };
}

static void x86_port_out16(uint16_t port)
{
    switch (port)
    {
#if (DRIVER==EMU8000)

        case 0x620:
        case 0x621:
        case 0x622:
        case 0x623:
        case 0xa20:
        case 0xa21:
        case 0xa22:
        case 0xa23:
        case 0xe20:
        case 0xe21:
        case 0xe22:
        case 0xe23:
            emu_awe32_write16(port, x86.ax);
            break;
#endif
        default:
            fprintf(stderr, "Unhandled port out: 0x%x ip: 0x%x\n", port, x86.ip);
            exit(1);
    };
}

static void x86_push(uint16_t value)
{
    x86.sp -= 2;
    WRITE_MEMORY_UINT16(x86.sp, value);
}

static void x86_push32(uint32_t value)
{
    x86.sp -= 4;
    WRITE_MEMORY_UINT32(x86.sp, value);
}

static uint16_t x86_pop(void)
{
    uint16_t value;
    value = READ_MEMORY_UINT16(x86.sp);
    x86.sp += 2;
    return value;
}

static uint32_t x86_pop32(void)
{
    uint32_t value;
    value = READ_MEMORY_UINT32(x86.sp);
    x86.sp += 4;
    return value;
}

static void emulator(void)
{
    uint32_t tempval32;
    uint32_t *tempptr32;
    uint16_t tempval16;
    uint16_t *tempptr16;
    uint8_t tempval8;
    uint8_t *tempptr8;

    while (1)
    {
        switch (GET_INSTRUCTION_UINT8)
        {
            case 0x00: // ADD r/m8,reg8                 ; 00 /r
                tempptr8 = x86_decode_modrm8();
                x86.fl.oper = op_addb;
                x86.fl.bop1 = *tempptr8;
                x86.fl.bop2 = X86_REG_8(x86.modrm_reg);
                *tempptr8 = x86.fl.bres = x86.fl.bop1 + x86.fl.bop2;
                break;
            case 0x01: // ADD r/m16,reg16               ; o16 01 /r
                tempptr16 = x86_decode_modrm16();
                x86.fl.oper = op_addw;
                x86.fl.wop1 = *tempptr16;
                x86.fl.wop2 = X86_REG_16(x86.modrm_reg);
                *tempptr16 = x86.fl.wres = x86.fl.wop1 + x86.fl.wop2;
                break;
            case 0x02: // ADD reg8,r/m8                 ; 02 /r
                tempval8 = *x86_decode_modrm8();
                x86.fl.oper = op_addb;
                x86.fl.bop1 = X86_REG_8(x86.modrm_reg);
                x86.fl.bop2 = tempval8;
                X86_REG_8(x86.modrm_reg) = x86.fl.bres = x86.fl.bop1 + x86.fl.bop2;
                break;
            case 0x03: // ADD reg16,r/m16               ; o16 03 /r
                tempval16 = *x86_decode_modrm16();
                x86.fl.oper = op_addw;
                x86.fl.wop1 = X86_REG_16(x86.modrm_reg);
                x86.fl.wop2 = tempval16;
                X86_REG_16(x86.modrm_reg) = x86.fl.wres = x86.fl.wop1 + x86.fl.wop2;
                break;
            case 0x04: // ADD AL,imm8                   ; 04 ib
                x86.fl.oper = op_addb;
                x86.fl.bop1 = x86.al;
                x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                x86.al = x86.fl.bres = x86.fl.bop1 + x86.fl.bop2;
                break;
            case 0x05: // ADD AX,imm16                  ; o16 05 iw
                x86.fl.oper = op_addw;
                x86.fl.wop1 = x86.ax;
                READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                x86.ax = x86.fl.wres = x86.fl.wop1 + x86.fl.wop2;
                break;

            case 0x11: // ADC r/m16,reg16               ; o16 11 /r
                tempptr16 = x86_decode_modrm16();
                tempval32 = x86_get_flag_c();
                x86.fl.oper = tempval32 ? op_adcw : op_addw;
                x86.fl.wop1 = *tempptr16;
                x86.fl.wop2 = X86_REG_16(x86.modrm_reg);
                *tempptr16 = x86.fl.wres = x86.fl.wop1 + x86.fl.wop2 + tempval32;
                break;

            case 0x06: // PUSH ES                       ; 06
                x86_push(x86.es);
                break;
            case 0x0e: // PUSH CS                       ; 0E
                x86_push(x86.cs);
                break;
            case 0x1e: // PUSH DS                       ; 1E
                x86_push(x86.ds);
                break;
            case 0x50: // PUSH reg16                    ; o16 50+r
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
                x86_push(X86_REG_16(PEEK_PREV_INSTRUCTION_BYTE - 0x50));
                break;
            case 0x68: // PUSH imm16                    ; o16 68 iw
                READ_INSTRUCTION_UINT16_TO(tempval16);
                x86_push(tempval16);
                break;
            case 0x6a: // PUSH imm8                     ; 6A ib
                x86_push((int16_t)GET_INSTRUCTION_INT8);
                break;
            case 0x9c: // PUSHF                         ; 9C
                x86_push(x86.flags);
                break;

            case 0x07: // POP ES                        ; 07
                tempval16 = x86_pop();
                check_segment_value(tempval16);
                x86.es = tempval16;
                break;
            case 0x1f: // POP DS                        ; 1F
                tempval16 = x86_pop();
                check_segment_value(tempval16);
                x86.ds = tempval16;
                break;
            case 0x58: // POP reg16                     ; o16 58+r
            case 0x59:
            case 0x5a:
            case 0x5b:
            case 0x5c:
            case 0x5d:
            case 0x5e:
            case 0x5f:
                X86_REG_16(PEEK_PREV_INSTRUCTION_BYTE - 0x58) = x86_pop();
                break;

            case 0x08: // OR r/m8,reg8                  ; 08 /r
                tempptr8 = x86_decode_modrm8();
                x86.fl.oper = op_xorb;
                x86.fl.bop1 = *tempptr8;
                x86.fl.bop2 = X86_REG_8(x86.modrm_reg);
                *tempptr8 = x86.fl.bres = x86.fl.bop1 | x86.fl.bop2;
                break;
            case 0x09: // OR r/m16,reg16                ; o16 09 /r
                tempptr16 = x86_decode_modrm16();
                x86.fl.oper = op_xorw;
                x86.fl.wop1 = *tempptr16;
                x86.fl.wop2 = X86_REG_16(x86.modrm_reg);
                *tempptr16 = x86.fl.wres = x86.fl.wop1 | x86.fl.wop2;
                break;
            case 0x0a: // OR reg8,r/m8                  ; 0A /r
                tempval8 = *x86_decode_modrm8();
                x86.fl.oper = op_xorb;
                x86.fl.bop1 = X86_REG_8(x86.modrm_reg);
                x86.fl.bop2 = tempval8;
                X86_REG_8(x86.modrm_reg) = x86.fl.bres = x86.fl.bop1 | x86.fl.bop2;
                break;
            case 0x0b: // OR reg16,r/m16                ; o16 0B /r
                tempval16 = *x86_decode_modrm16();
                x86.fl.oper = op_xorw;
                x86.fl.wop1 = X86_REG_16(x86.modrm_reg);
                x86.fl.wop2 = tempval16;
                X86_REG_16(x86.modrm_reg) = x86.fl.wres = x86.fl.wop1 | x86.fl.wop2;
                break;
            case 0x0c: // OR AL,imm8                    ; 0C ib
                x86.fl.oper = op_xorb;
                x86.fl.bop1 = x86.al;
                x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                x86.al = x86.fl.bres = x86.fl.bop1 | x86.fl.bop2;
                break;
            case 0x24: // AND AL,imm8                   ; 24 ib
                x86.fl.oper = op_xorb;
                x86.fl.bop1 = x86.al;
                x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                x86.al = x86.fl.bres = x86.fl.bop1 & x86.fl.bop2;
                break;
            case 0x25: // AND AX,imm16                  ; o16 25 iw
                x86.fl.oper = op_xorw;
                x86.fl.wop1 = x86.ax;
                READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                x86.ax = x86.fl.wres = x86.fl.wop1 & x86.fl.wop2;
                break;
            case 0xa8: // TEST AL,imm8                  ; A8 ib
                x86.fl.oper = op_xorb;
                x86.fl.bop1 = x86.al;
                x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                x86.fl.bres = x86.fl.bop1 & x86.fl.bop2;
                break;
            case 0xa9: // TEST AX,imm16                 ; o16 A9 iw
                x86.fl.oper = op_xorw;
                x86.fl.wop1 = x86.ax;
                READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                x86.fl.wres = x86.fl.wop1 & x86.fl.wop2;
                break;

            case 0x26: // segment override - ES
            case 0x2e: // segment override - CS
                // ignore
                break;

            case 0x19: // SBB r/m16,reg16               ; o16 19 /r
                tempptr16 = x86_decode_modrm16();
                tempval32 = x86_get_flag_c();
                x86.fl.oper = tempval32 ? op_sbbw : op_subw;
                x86.fl.wop1 = *tempptr16;
                x86.fl.wop2 = X86_REG_16(x86.modrm_reg);
                *tempptr16 = x86.fl.wres = x86.fl.wop1 - (x86.fl.wop2 + tempval32);
                break;
            case 0x1b: // SBB reg16,r/m16               ; o16 1B /r
                tempval16 = *x86_decode_modrm16();
                tempval32 = x86_get_flag_c();
                x86.fl.oper = tempval32 ? op_sbbw : op_subw;
                x86.fl.wop1 = X86_REG_16(x86.modrm_reg);
                x86.fl.wop2 = tempval16;
                X86_REG_16(x86.modrm_reg) = x86.fl.wres = x86.fl.wop1 - (x86.fl.wop2 + tempval32);
                break;
            case 0x29: // SUB r/m16,reg16               ; o16 29 /r
                tempptr16 = x86_decode_modrm16();
                x86.fl.oper = op_subw;
                x86.fl.wop1 = *tempptr16;
                x86.fl.wop2 = X86_REG_16(x86.modrm_reg);
                *tempptr16 = x86.fl.wres = x86.fl.wop1 - x86.fl.wop2;
                break;
            case 0x2a: // SUB reg8,r/m8                 ; 2A /r
                tempval8 = *x86_decode_modrm8();
                x86.fl.oper = op_subb;
                x86.fl.bop1 = X86_REG_8(x86.modrm_reg);
                x86.fl.bop2 = tempval8;
                X86_REG_8(x86.modrm_reg) = x86.fl.bres = x86.fl.bop1 - x86.fl.bop2;
                break;
            case 0x2b: // SUB reg16,r/m16               ; o16 2B /r
                tempval16 = *x86_decode_modrm16();
                x86.fl.oper = op_subw;
                x86.fl.wop1 = X86_REG_16(x86.modrm_reg);
                x86.fl.wop2 = tempval16;
                X86_REG_16(x86.modrm_reg) = x86.fl.wres = x86.fl.wop1 - x86.fl.wop2;
                break;
            case 0x2c: // SUB AL,imm8                   ; 2C ib
                x86.fl.oper = op_subb;
                x86.fl.bop1 = x86.al;
                x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                x86.al = x86.fl.bres = x86.fl.bop1 - x86.fl.bop2;
                break;
            case 0x2d: // SUB AX,imm16                  ; o16 2D iw
                x86.fl.oper = op_subw;
                x86.fl.wop1 = x86.ax;
                READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                x86.ax = x86.fl.wres = x86.fl.wop1 - x86.fl.wop2;
                break;
            case 0x39: // CMP r/m16,reg16               ; o16 39 /r
                tempval16 = *x86_decode_modrm16();
                x86.fl.oper = op_subw;
                x86.fl.wop1 = tempval16;
                x86.fl.wop2 = X86_REG_16(x86.modrm_reg);
                x86.fl.wres = x86.fl.wop1 - x86.fl.wop2;
                break;
            case 0x3a: // CMP reg8,r/m8                 ; 3A /r
                tempval8 = *x86_decode_modrm8();
                x86.fl.oper = op_subb;
                x86.fl.bop1 = X86_REG_8(x86.modrm_reg);
                x86.fl.bop2 = tempval8;
                x86.fl.bres = x86.fl.bop1 - x86.fl.bop2;
                break;
            case 0x3b: // CMP reg16,r/m16               ; o16 3B /r
                tempval16 = *x86_decode_modrm16();
                x86.fl.oper = op_subw;
                x86.fl.wop1 = X86_REG_16(x86.modrm_reg);
                x86.fl.wop2 = tempval16;
                x86.fl.wres = x86.fl.wop1 - x86.fl.wop2;
                break;
            case 0x3c: // CMP AL,imm8                   ; 3C ib
                x86.fl.oper = op_subb;
                x86.fl.bop1 = x86.al;
                x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                x86.fl.bres = x86.fl.bop1 - x86.fl.bop2;
                break;
            case 0x3d: // CMP AX,imm16                  ; o16 3D iw
                x86.fl.oper = op_subw;
                x86.fl.wop1 = x86.ax;
                READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                x86.fl.wres = x86.fl.wop1 - x86.fl.wop2;
                break;

            case 0x32: // XOR reg8,r/m8                 ; 32 /r
                tempval8 = *x86_decode_modrm8();
                x86.fl.oper = op_xorb;
                x86.fl.bop1 = X86_REG_8(x86.modrm_reg);
                x86.fl.bop2 = tempval8;
                X86_REG_8(x86.modrm_reg) = x86.fl.bres = x86.fl.bop1 ^ x86.fl.bop2;
                break;
            case 0x33: // XOR reg16,r/m16               ; o16 33 /r
                tempval16 = *x86_decode_modrm16();
                x86.fl.oper = op_xorw;
                x86.fl.wop1 = X86_REG_16(x86.modrm_reg);
                x86.fl.wop2 = tempval16;
                X86_REG_16(x86.modrm_reg) = x86.fl.wres = x86.fl.wop1 ^ x86.fl.wop2;
                break;
            case 0x34: // XOR AL,imm8                   ; 34 ib
                x86.fl.oper = op_xorb;
                x86.fl.bop1 = x86.al;
                x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                x86.al = x86.fl.bres = x86.fl.bop1 ^ x86.fl.bop2;
                break;

            case 0x40: // INC reg16                     ; o16 40+r
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
                x86_calculate_carry_flag();
                x86.fl.oper = op_incw;
                x86.fl.wop1 = X86_REG_16(PEEK_PREV_INSTRUCTION_BYTE - 0x40);
                X86_REG_16(PEEK_PREV_INSTRUCTION_BYTE - 0x40) = x86.fl.wres = x86.fl.wop1 + 1;
                break;

            case 0x48: // DEC reg16                     ; o16 48+r
            case 0x49:
            case 0x4a:
            case 0x4b:
            case 0x4c:
            case 0x4d:
            case 0x4e:
            case 0x4f:
                x86_calculate_carry_flag();
                x86.fl.oper = op_decw;
                x86.fl.wop1 = X86_REG_16(PEEK_PREV_INSTRUCTION_BYTE - 0x48);
                X86_REG_16(PEEK_PREV_INSTRUCTION_BYTE - 0x48) = x86.fl.wres = x86.fl.wop1 - 1;
                break;

            case 0x66:
                switch (GET_INSTRUCTION_UINT8)
                {
                    case 0x25: // AND EAX,imm32                 ; o32 25 id
                        x86.fl.oper = op_xord;
                        x86.fl.dwop1 = x86.eax;
                        READ_INSTRUCTION_UINT32_TO(x86.fl.dwop2);
                        x86.eax = x86.fl.dwres = x86.fl.dwop1 & x86.fl.dwop2;
                        break;

                    case 0x33: // XOR reg32,r/m32               ; o32 33 /r
                        tempval32 = *(uint32_t *)x86_decode_modrm16();
                        x86.fl.oper = op_xord;
                        x86.fl.dwop1 = x86.regs[x86.modrm_reg];
                        x86.fl.dwop2 = tempval32;
                        x86.regs[x86.modrm_reg] = x86.fl.dwres = x86.fl.dwop1 ^ x86.fl.dwop2;
                        break;

                    case 0x50: // PUSH reg32                    ; o32 50+r
                    case 0x51:
                    case 0x52:
                    case 0x53:
                    case 0x54:
                    case 0x55:
                    case 0x56:
                    case 0x57:
                        x86_push32(x86.regs[PEEK_PREV_INSTRUCTION_BYTE - 0x50]);
                        break;
                    case 0x58: // POP reg32                     ; o32 58+r
                    case 0x59:
                    case 0x5a:
                    case 0x5b:
                    case 0x5c:
                    case 0x5d:
                    case 0x5e:
                    case 0x5f:
                        x86.regs[PEEK_PREV_INSTRUCTION_BYTE - 0x58] = x86_pop32();
                        break;

                    case 0x81:
                        tempptr32 = (uint32_t *)x86_decode_modrm16();
                        switch (x86.modrm_reg)
                        {
                            case 4: // AND r/m32,imm32               ; o32 81 /4 id
                                x86.fl.oper = op_xord;
                                x86.fl.dwop1 = *tempptr32;
                                READ_INSTRUCTION_UINT32_TO(x86.fl.dwop2);
                                *tempptr32 = x86.fl.wres = x86.fl.dwop1 & x86.fl.dwop2;
                                break;
                            default:
                                fprintf(stderr, "Unhandled instruction 66 81: 0x%x\n", x86.ip-2);
                                exit(1);
                        }
                        break;

                    case 0x83:
                        tempptr32 = (uint32_t *)x86_decode_modrm16();
                        switch (x86.modrm_reg)
                        {
                            case 7: // CMP r/m32,imm8                ; o32 83 /7 ib
                                x86.fl.oper = op_subd;
                                x86.fl.dwop1 = *tempptr32;
                                x86.fl.dwop2 = (int32_t)GET_INSTRUCTION_INT8;
                                x86.fl.dwres = x86.fl.dwop1 - x86.fl.dwop2;
                                break;
                            default:
                                fprintf(stderr, "Unhandled instruction 66 83: 0x%x\n", x86.ip-2);
                                exit(1);
                        }
                        break;

                    case 0x89: // MOV r/m32,reg32               ; o32 89 /r
                        tempptr32 = (uint32_t *)x86_decode_modrm16();
                        *tempptr32 = x86.regs[x86.modrm_reg];
                        break;
                    case 0x8b: // MOV reg32,r/m32               ; o32 8B /r
                        tempval32 = *(uint32_t *)x86_decode_modrm16();
                        x86.regs[x86.modrm_reg] = tempval32;
                        break;
                    case 0xa1: // MOV AX,memoffs16              ; o16 A1 ow/od
                        READ_INSTRUCTION_UINT16_TO(tempval16);
                        x86.eax = READ_MEMORY_UINT32(tempval16);
                        break;
                    case 0xa3: // MOV memoffs16,AX              ; o16 A3 ow/od
                        READ_INSTRUCTION_UINT16_TO(tempval16);
                        WRITE_MEMORY_UINT32(tempval16, x86.eax);
                        break;

                    case 0xc7:
                        tempptr32 = (uint32_t *)x86_decode_modrm16();
                        switch (x86.modrm_reg)
                        {
                            case 0: // MOV r/m32,imm32               ; o32 C7 /0 id
                                READ_INSTRUCTION_UINT32_TO(*tempptr32);
                                break;
                            default:
                                fprintf(stderr, "Unhandled instruction 66 c7: 0x%x\n", x86.ip-2);
                                exit(1);
                        }
                        break;

                    default:
                        fprintf(stderr, "Unhandled instruction 66: 0x%x\n", x86.ip-2);
                        exit(1);
                };
                break;

            case 0x69: // IMUL reg16,r/m16,imm16        ; o16 69 /r iw
                tempval16 = *x86_decode_modrm16();
                x86.fl.oper = op_imulw;
                x86.fl.wop1 = tempval16;
                READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                X86_REG_16(x86.modrm_reg) = x86.fl.dwres = ((int16_t)x86.fl.wop1) * (int32_t)((int16_t)x86.fl.wop2);
                break;
            case 0x6b: // IMUL reg16,r/m16,imm8         ; o16 6B /r ib
                tempval16 = *x86_decode_modrm16();
                x86.fl.oper = op_imulw;
                x86.fl.wop1 = tempval16;
                x86.fl.wop2 = (int16_t)GET_INSTRUCTION_INT8;
                X86_REG_16(x86.modrm_reg) = x86.fl.dwres = ((int16_t)x86.fl.wop1) * (int32_t)((int16_t)x86.fl.wop2);
                break;

            case 0x72: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (x86_get_flag_c()) // jb
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x73: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (!(x86_get_flag_c())) // jnb
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x74: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (x86_get_flag_z()) // jz
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x75: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (!(x86_get_flag_z())) // jnz
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x76: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (x86_get_flag_c() || x86_get_flag_z()) // jbe
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x77: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (!(x86_get_flag_c() || x86_get_flag_z())) // ja
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x79: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (!(x86_get_flag_s())) // jns
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x7c: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (x86_get_flag_s() ^ x86_get_flag_o()) // jl
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x7d: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (!(x86_get_flag_s() ^ x86_get_flag_o())) // jge
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x7e: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (x86_get_flag_z() || (x86_get_flag_s() ^ x86_get_flag_o())) // jle
                {
                    x86.ip += tempval16;
                }
                break;
            case 0x7f: // Jcc imm                       ; 70+cc rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                if (!(x86_get_flag_z() || (x86_get_flag_s() ^ x86_get_flag_o()))) // jg
                {
                    x86.ip += tempval16;
                }
                break;

            case 0x80:
                tempptr8 = x86_decode_modrm8();
                switch (x86.modrm_reg)
                {
                    case 0: // ADD r/m8,imm8                 ; 80 /0 ib
                        x86.fl.oper = op_addb;
                        x86.fl.bop1 = *tempptr8;
                        x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                        *tempptr8 = x86.fl.bres = x86.fl.bop1 + x86.fl.bop2;
                        break;
                    case 1: // OR r/m8,imm8                  ; 80 /1 ib
                        x86.fl.oper = op_xorb;
                        x86.fl.bop1 = *tempptr8;
                        x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                        *tempptr8 = x86.fl.bres = x86.fl.bop1 | x86.fl.bop2;
                        break;
                    case 4: // AND r/m8,imm8                 ; 80 /4 ib
                        x86.fl.oper = op_xorb;
                        x86.fl.bop1 = *tempptr8;
                        x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                        *tempptr8 = x86.fl.bres = x86.fl.bop1 & x86.fl.bop2;
                        break;
                    case 6: // XOR r/m8,imm8                 ; 80 /6 ib
                        x86.fl.oper = op_xorb;
                        x86.fl.bop1 = *tempptr8;
                        x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                        *tempptr8 = x86.fl.bres = x86.fl.bop1 ^ x86.fl.bop2;
                        break;
                    case 7: // CMP r/m8,imm8                 ; 80 /7 ib
                        x86.fl.oper = op_subb;
                        x86.fl.bop1 = *tempptr8;
                        x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                        x86.fl.bres = x86.fl.bop1 - x86.fl.bop2;
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction 80: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0x81:
                tempptr16 = x86_decode_modrm16();
                switch (x86.modrm_reg)
                {
                    case 0: // ADD r/m16,imm16               ; o16 81 /0 iw
                        x86.fl.oper = op_addw;
                        x86.fl.wop1 = *tempptr16;
                        READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                        *tempptr16 = x86.fl.wres = x86.fl.wop1 + x86.fl.wop2;
                        break;
                    case 4: // AND r/m16,imm16               ; o16 81 /4 iw
                        x86.fl.oper = op_xorw;
                        x86.fl.wop1 = *tempptr16;
                        READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                        *tempptr16 = x86.fl.wres = x86.fl.wop1 & x86.fl.wop2;
                        break;
                    case 7: // CMP r/m16,imm16               ; o16 81 /7 iw
                        x86.fl.oper = op_subw;
                        x86.fl.wop1 = *tempptr16;
                        READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                        x86.fl.wres = x86.fl.wop1 - x86.fl.wop2;
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction 81: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0x83:
                tempptr16 = x86_decode_modrm16();
                switch (x86.modrm_reg)
                {
                    case 0: // ADD r/m16,imm8                ; o16 83 /0 ib
                        x86.fl.oper = op_addw;
                        x86.fl.wop1 = *tempptr16;
                        x86.fl.wop2 = (int16_t)GET_INSTRUCTION_INT8;
                        *tempptr16 = x86.fl.wres = x86.fl.wop1 + x86.fl.wop2;
                        break;
                    case 2: // ADC r/m16,imm8                ; o16 83 /2 ib
                        tempval32 = x86_get_flag_c();
                        x86.fl.oper = tempval32 ? op_adcw : op_addw;
                        x86.fl.wop1 = *tempptr16;
                        x86.fl.wop2 = (int16_t)GET_INSTRUCTION_INT8;
                        *tempptr16 = x86.fl.wres = x86.fl.wop1 + x86.fl.wop2 + tempval32;
                        break;
                    case 3: // SBB r/m16,imm8                ; o16 83 /3 ib
                        tempval32 = x86_get_flag_c();
                        x86.fl.oper = tempval32 ? op_sbbw : op_subw;
                        x86.fl.wop1 = *tempptr16;
                        x86.fl.wop2 = (int16_t)GET_INSTRUCTION_INT8;
                        *tempptr16 = x86.fl.wres = x86.fl.wop1 - (x86.fl.wop2 + tempval32);
                        break;
                    case 4: // AND r/m16,imm8                ; o16 83 /4 ib
                        x86.fl.oper = op_xorw;
                        x86.fl.wop1 = *tempptr16;
                        x86.fl.wop2 = (int16_t)GET_INSTRUCTION_INT8;
                        *tempptr16 = x86.fl.wres = x86.fl.wop1 & x86.fl.wop2;
                        break;
                    case 7: // CMP r/m16,imm8                ; o16 83 /7 ib
                        x86.fl.oper = op_subw;
                        x86.fl.wop1 = *tempptr16;
                        x86.fl.wop2 = (int16_t)GET_INSTRUCTION_INT8;
                        x86.fl.wres = x86.fl.wop1 - x86.fl.wop2;
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction 83: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0x88: // MOV r/m8,reg8                 ; 88 /r
                tempptr8 = x86_decode_modrm8();
                *tempptr8 = X86_REG_8(x86.modrm_reg);
                break;
            case 0x89: // MOV r/m16,reg16               ; o16 89 /r
                tempptr16 = x86_decode_modrm16();
                *tempptr16 = X86_REG_16(x86.modrm_reg);
                break;
            case 0x8a: // MOV reg8,r/m8                 ; 8A /r
                tempval8 = *x86_decode_modrm8();
                X86_REG_8(x86.modrm_reg) = tempval8;
                break;
            case 0x8b: // MOV reg16,r/m16               ; o16 8B /r
                tempval16 = *x86_decode_modrm16();
                X86_REG_16(x86.modrm_reg) = tempval16;
                break;
            case 0x8c: // MOV r/m16,segreg              ; o16 8C /r
                tempptr16 = x86_decode_modrm16();
                *tempptr16 = x86.segregs[x86.modrm_reg];
                break;
            case 0x8d: // LEA reg16,mem                 ; o16 8D /r
                tempptr16 = x86_decode_modrm16();
                X86_REG_16(x86.modrm_reg) = (uintptr_t)tempptr16 - (uintptr_t)memory;
                break;
            case 0x8e: // MOV segreg,r/m16              ; o16 8E /r
                tempval16 = *x86_decode_modrm16();
                check_segment_value(tempval16);
                x86.segregs[x86.modrm_reg] = tempval16;
                break;
            case 0xa1: // MOV AX,memoffs16              ; o16 A1 ow/od
                READ_INSTRUCTION_UINT16_TO(tempval16);
                x86.ax = READ_MEMORY_UINT16(tempval16);
                break;
            case 0xa3: // MOV memoffs16,AX              ; o16 A3 ow/od
                READ_INSTRUCTION_UINT16_TO(tempval16);
                WRITE_MEMORY_UINT16(tempval16, x86.ax);
                break;
            case 0xb0: // MOV reg8,imm8                 ; B0+r ib
            case 0xb1:
            case 0xb2:
            case 0xb3:
                tempval8 = PEEK_PREV_INSTRUCTION_BYTE - 0xb0;
                X86_REG_8_HILO(tempval8, 0) = GET_INSTRUCTION_UINT8;
                break;
            case 0xb4: // MOV reg8,imm8                 ; B0+r ib
            case 0xb5:
            case 0xb6:
            case 0xb7:
                tempval8 = PEEK_PREV_INSTRUCTION_BYTE - 0xb4;
                X86_REG_8_HILO(tempval8, 1) = GET_INSTRUCTION_UINT8;
                break;
            case 0xb8: // MOV reg16,imm16               ; o16 B8+r iw
            case 0xb9:
            case 0xba:
            case 0xbb:
            case 0xbc:
            case 0xbd:
            case 0xbe:
            case 0xbf:
                READ_INSTRUCTION_UINT16_TO(X86_REG_16(PEEK_PREV_INSTRUCTION_BYTE - 0xb8));
                break;

            case 0x98: // CBW                           ; o16 98
                x86.ax = ((int16_t)(int8_t)x86.al);
                break;
            case 0x99: // CWD                           ; o16 99
                x86.dx = ((int32_t)(int16_t)x86.ax) >> 16;
                break;

            case 0x9d: // POPF                          ; 9D
                x86.flags = (x86.flags & ~(0x0cd5)) | (x86_pop() & 0x0cd5);
                x86.fl.oper = op_none;
                break;

            case 0xc0:
                tempptr8 = x86_decode_modrm8();
                switch (x86.modrm_reg)
                {
                    case 4: // SHL r/m8,imm8                 ; C0 /4 ib
                        tempval8 = GET_INSTRUCTION_UINT8 & 0x1f;
                        if (tempval8)
                        {
                            x86.fl.oper = op_shlb;
                            x86.fl.bop1 = *tempptr8;
                            x86.fl.bop2 = tempval8;
                            *tempptr8 = x86.fl.bres = x86.fl.bop1 << x86.fl.bop2;
                        }
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction c0: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xc1:
                tempptr16 = x86_decode_modrm16();
                switch (x86.modrm_reg)
                {
                    case 4: // SHL r/m16,imm8                ; o16 C1 /4 ib
                        tempval8 = GET_INSTRUCTION_UINT8 & 0x1f;
                        if (tempval8)
                        {
                            x86.fl.oper = op_shlw;
                            x86.fl.wop1 = *tempptr16;
                            x86.fl.bop2 = tempval8;
                            *tempptr16 = x86.fl.wres = x86.fl.wop1 << x86.fl.bop2;
                        }
                        break;
                    case 5: // SHR r/m16,imm8                ; o16 C1 /5 ib
                        tempval8 = GET_INSTRUCTION_UINT8 & 0x1f;
                        if (tempval8)
                        {
                            x86.fl.oper = op_shrw;
                            x86.fl.wop1 = *tempptr16;
                            x86.fl.bop2 = tempval8;
                            *tempptr16 = x86.fl.wres = x86.fl.wop1 >> x86.fl.bop2;
                        }
                        break;
                    case 7: // SAR r/m16,imm8                ; o16 C1 /7 ib
                        tempval8 = GET_INSTRUCTION_UINT8 & 0x1f;
                        if (tempval8)
                        {
                            x86.fl.oper = op_sarw;
                            x86.fl.wop1 = *tempptr16;
                            x86.fl.bop2 = tempval8;
                            *tempptr16 = x86.fl.wres = ((int16_t)x86.fl.wop1) >> x86.fl.bop2;
                        }
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction c1: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xc2: // RET imm16                     ; C2
                READ_INSTRUCTION_UINT16_TO(tempval16);
                x86.ip = x86_pop();
                x86.sp += tempval16;
                break;

            case 0xc3: // RET                           ; C3
                x86.ip = x86_pop();
                break;

            case 0xc4: // LES reg16,mem                 ; o16 C4 /r
                tempptr16 = x86_decode_modrm16();
                check_segment_value(tempptr16[1]);
                x86.es = tempptr16[1];
                X86_REG_16(x86.modrm_reg) = *tempptr16;
                break;

            case 0xc6:
                tempptr8 = x86_decode_modrm8();
                switch (x86.modrm_reg)
                {
                    case 0: // MOV r/m8,imm8                 ; C6 /0 ib
                        *tempptr8 = GET_INSTRUCTION_UINT8;
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction c7: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xc7:
                tempptr16 = x86_decode_modrm16();
                switch (x86.modrm_reg)
                {
                    case 0: // MOV r/m16,imm16               ; o16 C7 /0 iw
                        READ_INSTRUCTION_UINT16_TO(*tempptr16);
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction c7: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xc8: // ENTER imm,imm                 ; C8 iw ib
                READ_INSTRUCTION_UINT16_TO(tempval16);
                tempval8 = GET_INSTRUCTION_UINT8;
                if (tempval8 == 0)
                {
                    x86_push(x86.bp);
                    x86.bp = x86.sp;
                    x86.sp -= tempval16;
                }
                else
                {
                    fprintf(stderr, "Unhandled enter instruction: 0x%x, 0x%x - 0x%x\n", tempval16, tempval8, x86.ip-2);
                }
                break;
            case 0xc9: // LEAVE                         ; C9
                x86.sp = x86.bp;
                x86.bp = x86_pop();
                break;

            case 0xcd: // INT imm8                      ; CD ib
                x86_interrupt(GET_INSTRUCTION_UINT8);
                if (x86.terminate)
                {
                    x86.terminate = 0;
                    return;
                }
                break;
            case 0xcf: // IRET                          ; CF
                x86.ip = x86_pop();
                check_segment_value(x86_pop());
                x86.fl.oper = op_none;
                x86.flags = x86_pop();
                break;

            case 0xd0:
                tempptr8 = x86_decode_modrm8();
                switch (x86.modrm_reg)
                {
                    case 5: // SHR r/m8,1                    ; D0 /5
                        x86.fl.oper = op_shrb;
                        x86.fl.bop1 = *tempptr8;
                        x86.fl.bop2 = 1;
                        *tempptr8 = x86.fl.bres = x86.fl.bop1 >> 1;
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction d0: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xd1:
                tempptr16 = x86_decode_modrm16();
                switch (x86.modrm_reg)
                {
                    case 4: // SHL r/m16,1                   ; o16 D1 /4
                        x86.fl.oper = op_shlw;
                        x86.fl.wop1 = *tempptr16;
                        x86.fl.bop2 = 1;
                        *tempptr16 = x86.fl.wres = x86.fl.wop1 << 1;
                        break;
                    case 7: // SAR r/m16,1                   ; o16 D1 /7
                        x86.fl.oper = op_sarw;
                        x86.fl.wop1 = *tempptr16;
                        x86.fl.bop2 = 1;
                        *tempptr16 = x86.fl.wres = ((int16_t)x86.fl.wop1) >> 1;
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction d1: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xe2: // LOOP imm                      ; E2 rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                x86.cx--;
                if (x86.cx)
                {
                    x86.ip += tempval16;
                }
                break;
            case 0xe8: // CALL imm                      ; E8 rw/rd
                READ_INSTRUCTION_UINT16_TO(tempval16);
                x86_push(x86.ip);
                x86.ip += tempval16;
                break;
            case 0xe9: // JMP imm                       ; E9 rw/rd
                READ_INSTRUCTION_UINT16_TO(tempval16);
                x86.ip += tempval16;
                break;
            case 0xeb: // JMP SHORT imm                 ; EB rb
                tempval16 = (int16_t)GET_INSTRUCTION_INT8;
                x86.ip += tempval16;
                break;

            case 0xec: // IN AL,DX                      ; EC
                x86_port_in8(x86.dx);
                break;
            case 0xed: // IN AX,DX                      ; ED
                x86_port_in16(x86.dx);
                break;
            case 0xee: // OUT DX,AL                     ; EE
                x86_port_out8(x86.dx);
                break;
            case 0xef: // OUT DX,AX                     ; EF
                x86_port_out16(x86.dx);
                break;

            case 0xf3:
                switch (GET_INSTRUCTION_UINT8)
                {
                    case 0xa5: // REP MOVSW
                        tempval16 = (x86.flags & 0x0400) ? -2 : 2;
                        do
                        {
                            WRITE_MEMORY_UINT16(x86.di, READ_MEMORY_UINT16(x86.si));
                            x86.si += tempval16;
                            x86.di += tempval16;
                            x86.cx--;
                        } while (x86.cx != 0);
                        break;
                    case 0xab: // REP STOSW
                        tempval16 = (x86.flags & 0x0400) ? -2 : 2;
                        do
                        {
                            WRITE_MEMORY_UINT16(x86.di, x86.ax);
                            x86.di += tempval16;
                            x86.cx--;
                        } while (x86.cx != 0);
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction f3: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xf6:
                tempptr8 = x86_decode_modrm8();
                switch (x86.modrm_reg)
                {
                    case 0: // TEST r/m8,imm8                ; F6 /0 ib
                        x86.fl.oper = op_xorb;
                        x86.fl.bop1 = *tempptr8;
                        x86.fl.bop2 = GET_INSTRUCTION_UINT8;
                        x86.fl.bres = x86.fl.bop1 & x86.fl.bop2;
                        break;
                    case 3: // NEG r/m8                      ; F6 /3
                        x86.fl.oper = op_negb;
                        x86.fl.bop1 = *tempptr8;
                        *tempptr8 = x86.fl.bres = - (int8_t)x86.fl.bop1;
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction f6: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xf7:
                tempptr16 = x86_decode_modrm16();
                switch (x86.modrm_reg)
                {
                    case 0: // TEST r/m16,imm16              ; o16 F7 /0 iw
                        x86.fl.oper = op_xorw;
                        x86.fl.wop1 = *tempptr16;
                        READ_INSTRUCTION_UINT16_TO(x86.fl.wop2);
                        x86.fl.wres = x86.fl.wop1 & x86.fl.wop2;
                        break;
                    case 3: // NEG r/m16                     ; o16 F7 /3
                        x86.fl.oper = op_negw;
                        x86.fl.wop1 = *tempptr16;
                        *tempptr16 = x86.fl.wres = - (int16_t)x86.fl.wop1;
                        break;
                    case 4: // MUL r/m16                     ; o16 F7 /4
                        x86.fl.oper = op_mulw;
                        x86.fl.wop1 = x86.ax;
                        x86.fl.wop2 = *tempptr16;
                        x86.fl.dwres = x86.fl.wop1 * (uint32_t)x86.fl.wop2;
                        x86.ax = (uint16_t)x86.fl.dwres;
                        x86.dx = (uint16_t)(x86.fl.dwres >> 16);
                        break;
                    case 5: // IMUL r/m16                    ; o16 F7 /5
                        x86.fl.oper = op_imulw;
                        x86.fl.wop1 = x86.ax;
                        x86.fl.wop2 = *tempptr16;
                        x86.fl.dwres = ((int16_t)x86.fl.wop1) * (int32_t)((int16_t)x86.fl.wop2);
                        x86.ax = (uint16_t)x86.fl.dwres;
                        x86.dx = (uint16_t)(x86.fl.dwres >> 16);
                        break;
                    case 6: // DIV r/m16                     ; o16 F7 /6
                        x86.fl.oper = op_divw;
                        x86.fl.dwop1 = x86.ax | (((uint32_t)x86.dx) << 16);
                        x86.fl.wop2 = *tempptr16;
                        x86.ax = x86.fl.dwop1 / x86.fl.wop2;
                        x86.dx = x86.fl.dwop1 % x86.fl.wop2;
                        x86.fl.dwres = x86.ax | (((uint32_t)x86.dx) << 16);
                        break;
                    case 7: // IDIV r/m16                    ; o16 F7 /7
                        x86.fl.oper = op_divw;
                        x86.fl.dwop1 = x86.ax | (((uint32_t)x86.dx) << 16);
                        x86.fl.wop2 = *tempptr16;
                        x86.ax = ((int32_t)x86.fl.dwop1) / ((int16_t)x86.fl.wop2);
                        x86.dx = ((int32_t)x86.fl.dwop1) % ((int16_t)x86.fl.wop2);
                        x86.fl.dwres = x86.ax | (((uint32_t)x86.dx) << 16);
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction f7: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xfa: // CLI                           ; FA
                // do nothing
                break;

            case 0xfb: // STI                           ; FB
                // do nothing
                break;

            case 0xfe:
                tempptr8 = x86_decode_modrm8();
                switch (x86.modrm_reg)
                {
                    case 0: // INC r/m8                      ; FE /0
                        x86_calculate_carry_flag();
                        x86.fl.oper = op_incb;
                        x86.fl.bop1 = *tempptr8;
                        *tempptr8 = x86.fl.bres = x86.fl.bop1 + 1;
                        break;
                    case 1: // DEC r/m8                      ; FE /1
                        x86_calculate_carry_flag();
                        x86.fl.oper = op_decb;
                        x86.fl.bop1 = *tempptr8;
                        *tempptr8 = x86.fl.bres = x86.fl.bop1 - 1;
                        break;
                    case 7: // exit emulator
                        return;
                    default:
                        fprintf(stderr, "Unhandled instruction fe: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            case 0xff:
                tempptr16 = x86_decode_modrm16();
                switch (x86.modrm_reg)
                {
                    case 0: // INC r/m16                     ; o16 FF /0
                        x86_calculate_carry_flag();
                        x86.fl.oper = op_incw;
                        x86.fl.wop1 = *tempptr16;
                        *tempptr16 = x86.fl.wres = x86.fl.wop1 + 1;
                        break;
                    case 1: // DEC r/m16                     ; o16 FF /1
                        x86_calculate_carry_flag();
                        x86.fl.oper = op_decw;
                        x86.fl.wop1 = *tempptr16;
                        *tempptr16 = x86.fl.wres = x86.fl.wop1 - 1;
                        break;
                    case 4: // JMP r/m16                     ; o16 FF /4
                        x86.ip = *tempptr16;
                        break;
                    case 5: // JMP FAR mem                   ; o16 FF /5
                        check_segment_value(tempptr16[1]);
                        x86.ip = *tempptr16;
                        break;
                    case 6: // PUSH r/m16                    ; o16 FF /6
                        x86_push(*tempptr16);
                        break;
                    default:
                        fprintf(stderr, "Unhandled instruction ff: 0x%x\n", x86.ip-2);
                        exit(1);
                }
                break;

            default:
                fprintf(stderr, "Unhandled instruction: 0x%x\n", x86.ip-1);
                exit(1);
        };
    };
}

static void RunInt66(uint16_t param)
{
    x86.eax = param;

    x86.esp = 0xfffe;
    x86.eip = x86.int66_offset;
    x86.eflags = 0x3202;
    x86.fl.oper = op_none;

    WRITE_MEMORY_UINT16(0xfffe, 0xfffe); // exit from emulator
    x86_push(x86.flags);
    x86_push(SEGMENT_VALUE);
    x86_push(0xfffe);

    emulator();
}

static void InitMusic(void)
{
    // SoundConfig (18 bytes)
    WRITE_MEMORY_UINT16(0xee, 0); // SoundDriver
    WRITE_MEMORY_UINT16(0xf0, 0x220); // SoundBasePort
    WRITE_MEMORY_UINT16(0xf2, 7); // SoundIRQ
    WRITE_MEMORY_UINT16(0xf4, 1); // SoundDMA
#if (DRIVER==ADLIB)
    WRITE_MEMORY_UINT16(0xf6, 0); // MusicDriver
    WRITE_MEMORY_UINT16(0xf8, 0x388); // MusicBasePort
#elif (DRIVER==ROLAND)
    WRITE_MEMORY_UINT16(0xf6, 1); // MusicDriver
    WRITE_MEMORY_UINT16(0xf8, 0x330); // MusicBasePort
#elif (DRIVER==EMU8000)
    WRITE_MEMORY_UINT16(0xf6, 3); // MusicDriver
    WRITE_MEMORY_UINT16(0xf8, 0x620); // MusicBasePort
#else
    WRITE_MEMORY_UINT16(0xf6, 4); // MusicDriver
#endif
    WRITE_MEMORY_UINT16(0xfa, 2); // SoundChannels
    WRITE_MEMORY_UINT16(0xfc, 0); // SoundSwapStereo
    WRITE_MEMORY_UINT16(0xfe, 0); // Reserved

    x86.edx = 0xee | (SEGMENT_VALUE << 16); // address of SoundConfig

    RunInt66(0x100);
}

static void StopMusic(void)
{
    RunInt66(0x101);
}

static void ProcessSequence(void)
{
    RunInt66(0x103);
}

static int PlaySequenceBuffer(const void *seqbuffer, unsigned int seqlen)
{
    if (seqbuffer == NULL) return 0;
    if (seqlen == 0) seqlen = 33000;
    if (x86.resident_program_size + seqlen > 65000) return 0;

    memcpy(GET_MEMORY_ADDR(x86.resident_program_size), seqbuffer, seqlen);

    x86.edx = x86.resident_program_size | (SEGMENT_VALUE << 16);

    RunInt66(0x104);

    return 1;
}

/*static void PlaySequence(const char *filename)
{
    FILE *f;
    size_t filelen;

    f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    filelen = ftell(f);
    fseek(f, 0, SEEK_SET);

    fread(GET_MEMORY_ADDR(x86.resident_program_size), 1, filelen, f);
    fclose(f);

    x86.edx = x86.resident_program_size | (SEGMENT_VALUE << 16);

    RunInt66(0x104);
}*/

static void RewindSequence(void)
{
    x86.edx = x86.resident_program_size | (SEGMENT_VALUE << 16);

    RunInt66(0x104);
}

static uint32_t IsSequencePlaying(void)
{
    RunInt66(0x105);
    return x86.eax;
}

static void SetSequenceVolume(uint16_t volume)
{
    x86.ebx = volume;
    RunInt66(0x106);
}

/*static uint16_t GetSequenceVolume(void)
{
    RunInt66(0x107);
    return x86.ax;
}*/

static void StopSequence(void)
{
    RunInt66(0x108);
}

/*static void FadeOutSequence(void)
{
    RunInt66(0x109);
}*/

static void SetTimerFrequency(uint16_t frequency)
{
    x86.ebx = frequency;
    RunInt66(0x10d);
}


int emu_x86_initialize(unsigned int rate, char const *drivers_cat, char const *mt32_roms, char const *awe32_rom, int opl3_emulator, int resampling_quality)
{
    FILE *f;
    uint32_t num_files, file_offset, file_len;
    unsigned int file_counter;
    void *driver_com;
    uint32_t driver_com_len;
#if (DRIVER==ROLAND)
    void *lapc1_pat;
    uint32_t lapc1_pat_len;
#endif
    uint8_t name_len;
    uint8_t name[256];

    memory = NULL;

    f = fopen(drivers_cat, "rb");
    if (f == NULL) return 0;

    driver_com = NULL;
#if (DRIVER==ROLAND)
    lapc1_pat = NULL;
#endif
    if (fread(&num_files, 4, 1, f) != 1) goto emu_x86_initialize_error_1;
    num_files >>= 3;

    for (file_counter = 0; file_counter < num_files; file_counter++)
    {
        if (fseek(f, 8 * file_counter, SEEK_SET)) goto emu_x86_initialize_error_1;
        if (fread(&file_offset, 4, 1, f) != 1) goto emu_x86_initialize_error_1;
        if (fread(&file_len, 4, 1, f) != 1) goto emu_x86_initialize_error_1;

        if (file_len == 0) continue;

        if (fseek(f, file_offset, SEEK_SET)) goto emu_x86_initialize_error_1;
        if (fread(&name_len, 1, 1, f) != 1) goto emu_x86_initialize_error_1;
        if (name_len == 0) continue;
        if (fread(name, 1, name_len, f) != name_len) goto emu_x86_initialize_error_1;
        name[name_len] = 0;

#if (DRIVER==ADLIB)
        if (strcasecmp((char *)name, "amusic.com") == 0)
        {
            driver_com_len = file_len;
            driver_com = malloc(file_len);
            if (driver_com == NULL) goto emu_x86_initialize_error_1;
            if (fread(driver_com, 1, file_len, f) != file_len) goto emu_x86_initialize_error_1;

            break;
        }
#elif (DRIVER==ROLAND)
        if (strcasecmp((char *)name, "rmusic.com") == 0)
        {
            driver_com_len = file_len;
            driver_com = malloc(file_len);
            if (driver_com == NULL) goto emu_x86_initialize_error_1;
            if (fread(driver_com, 1, file_len, f) != file_len) goto emu_x86_initialize_error_1;

            if (lapc1_pat != NULL) break;
        }
        else if (strcasecmp((char *)name, "lapc1.pat") == 0)
        {
            lapc1_pat_len = file_len;
            lapc1_pat = malloc(file_len);
            if (lapc1_pat == NULL) goto emu_x86_initialize_error_1;
            if (fread(lapc1_pat, 1, file_len, f) != file_len) goto emu_x86_initialize_error_1;

            if (driver_com != NULL) break;
        }
#elif (DRIVER==EMU8000)
        if (strcasecmp((char *)name, "awemusic.com") == 0)
        {
            driver_com_len = file_len;
            driver_com = malloc(file_len);
            if (driver_com == NULL) goto emu_x86_initialize_error_1;
            if (fread(driver_com, 1, file_len, f) != file_len) goto emu_x86_initialize_error_1;

            break;
        }
#endif
    }

    fclose(f);

    if (driver_com == NULL) goto emu_x86_initialize_error_2;
#if (DRIVER==ROLAND)
    if (lapc1_pat == NULL) goto emu_x86_initialize_error_2;

    x86.lapc1_pat = (uint8_t *)lapc1_pat;
    x86.lapc1_pat_len = lapc1_pat_len;
#endif

    if (!initialize()) goto emu_x86_initialize_error_2;

    if (!loadfilebuffer(driver_com, driver_com_len)) goto emu_x86_initialize_error_2;

    free(driver_com);
    driver_com = NULL;

    emulator();

    if (x86.int66_offset == 0) goto emu_x86_initialize_error_2;

#if (DRIVER==ADLIB)
#ifdef USE_SPEEXDSP_RESAMPLER
    if ((resampling_quality > 0) && (rate != BASE_SAMPLE_RATE))
    {
        int err;

        resampler = speex_resampler_init(2, BASE_SAMPLE_RATE, rate, SPEEX_RESAMPLER_QUALITY_DESKTOP, &err);
        if ((resampler == NULL) || (err != RESAMPLER_ERR_SUCCESS))
        {
            resampler = NULL;
        }
        else
        {
            resample_step = ((((uint64_t)BASE_SAMPLE_RATE) << 32) / rate);
        }
    }
    else resampler = NULL;

    if (resampler != NULL) emu_opl2_init(BASE_SAMPLE_RATE, opl3_emulator);
    else
#endif
    emu_opl2_init(rate, opl3_emulator);
#elif (DRIVER==ROLAND) || (DRIVER==EMU8000)
#if (DRIVER==ROLAND)
    if (emu_mt32_init(BASE_SAMPLE_RATE, mt32_roms)) goto emu_x86_initialize_error_2;
#elif (DRIVER==EMU8000)
    if (emu_awe32_init(BASE_SAMPLE_RATE, awe32_rom)) goto emu_x86_initialize_error_2;
#endif

#ifdef USE_SPEEXDSP_RESAMPLER
    if ((resampling_quality > 0) && (rate != BASE_SAMPLE_RATE))
    {
        int err;

        resampler = speex_resampler_init(2, BASE_SAMPLE_RATE, rate, SPEEX_RESAMPLER_QUALITY_DESKTOP, &err);
        if ((resampler == NULL) || (err != RESAMPLER_ERR_SUCCESS))
        {
            resampler = NULL;
        }
        else
        {
            resample_step = ((((uint64_t)BASE_SAMPLE_RATE) << 32) / rate);
        }
    }
    else resampler = NULL;
#endif
#endif

    InitMusic();

#if (DRIVER==ROLAND)
    free(lapc1_pat);
    lapc1_pat = NULL;
    x86.lapc1_pat = NULL;
#endif

#if (DRIVER==ADLIB)
    SetTimerFrequency(70);

#ifdef USE_SPEEXDSP_RESAMPLER
    if (resampler != NULL) sample_rate = BASE_SAMPLE_RATE;
    else
#endif
    sample_rate = rate;
    num_samples_add = (sample_rate << 10) / 70;
    num_samples_left = 0;
#elif (DRIVER==ROLAND) || (DRIVER==EMU8000)
#if (DRIVER==ROLAND)
    SetTimerFrequency(80);
#elif (DRIVER==EMU8000)
    SetTimerFrequency(70);
#endif

    sample_rate = rate;
    position_add = (((uint64_t)BASE_SAMPLE_RATE) << 32) / sample_rate;
    samples_mul = (((uint64_t)sample_rate) << 32) / BASE_SAMPLE_RATE;
#endif

    return 1;

emu_x86_initialize_error_1:
    fclose(f);
emu_x86_initialize_error_2:
    if (driver_com != NULL) free(driver_com);
#if (DRIVER==ROLAND)
    if (lapc1_pat != NULL) free(lapc1_pat);
#endif
    return 0;
}

int emu_x86_setvolume(unsigned char volume)
{
    SetSequenceVolume(volume);
    return 1;
}

int emu_x86_playsequence(void const *sequence, int size)
{
    int retval;

    if (size <= 0) return 0;
    retval = PlaySequenceBuffer(sequence, size);
#ifdef USE_SPEEXDSP_RESAMPLER
    resample_num_samples = 0;
#endif
#if (DRIVER==ADLIB)
    num_samples_left = num_samples_add;
#elif (DRIVER==ROLAND) || (DRIVER==EMU8000)
    num_samples_left = SYNTH_BUFFER_LEN;

    if (BASE_SAMPLE_RATE > sample_rate)
    {
        current_position = ((((uint64_t)(BASE_SAMPLE_RATE - sample_rate)) << 32) / BASE_SAMPLE_RATE) + (((uint64_t)1) << 32);
    }
    else
    {
        current_position = (((uint64_t)1) << 32);
    }

#ifdef USE_SPEEXDSP_RESAMPLER
    if (resampler == NULL)
#endif
    if (sample_rate != BASE_SAMPLE_RATE)
    {
#if (DRIVER==ROLAND)
        emu_mt32_getsamples(&(resample_samples[2]), SYNTH_BUFFER_LEN);
#elif (DRIVER==EMU8000)
        emu_awe32_getsamples(&(resample_samples[2]), SYNTH_BUFFER_LEN);
#endif
    }
#endif
    return retval;
}

int emu_x86_getdata(void *buffer, int size)
{
    int numsamples, samples_read;
    int16_t *buf;

    numsamples = size >> 2;
    samples_read = 0;
    buf = (int16_t *)buffer;

#if (DRIVER==ADLIB)
#ifdef USE_SPEEXDSP_RESAMPLER
    if (resampler != NULL)
    {
        while (numsamples != 0)
        {
            int samples_left, samples_toread;
            spx_uint32_t in_len, out_len;
            int err;
            unsigned int index;

            samples_left = num_samples_left >> 10;
            if (samples_left == 0)
            {
                if (!IsSequencePlaying()) break;
                ProcessSequence();

                num_samples_left += num_samples_add;
                samples_left = num_samples_left >> 10;
            }

            samples_toread = (((numsamples + 1) * resample_step) >> 32) + 8;
            if (samples_toread & 7) samples_toread += 8 - (samples_toread & 7);
            if (samples_toread > RESAMPLER_BUFFER_LEN) samples_toread = RESAMPLER_BUFFER_LEN;

            samples_toread -= resample_num_samples;
            if (samples_toread > samples_left) samples_toread = samples_left;

            emu_opl2_getsamples(&(resample_samples[2 * resample_num_samples]), samples_toread);

            resample_num_samples += samples_toread;
            num_samples_left -= samples_toread << 10;

            in_len = resample_num_samples;
            out_len = numsamples;

            err = speex_resampler_process_interleaved_int(resampler, resample_samples, &in_len, buf, &out_len);
            if (err != RESAMPLER_ERR_SUCCESS) break;

            buf += 2 * out_len;
            samples_read += out_len;
            numsamples -= out_len;

            if (in_len)
            {
                for (index = 0; in_len + index < resample_num_samples; index++)
                {
                    resample_samples[2 * index] = resample_samples[2 * (in_len + index)];
                    resample_samples[2 * index + 1] = resample_samples[2 * (in_len + index) + 1];
                }

                resample_num_samples -= in_len;
            }
        }
    }
    else
#endif
    while (numsamples != 0)
    {
        int samples_left, samples_toread;

        samples_left = num_samples_left >> 10;
        if (samples_left == 0)
        {
            if (!IsSequencePlaying()) break;
            ProcessSequence();

            num_samples_left += num_samples_add;
            samples_left = num_samples_left >> 10;
        }

        samples_toread = (numsamples <= samples_left)?numsamples:samples_left;

        emu_opl2_getsamples(buf, samples_toread);

        buf += 2 * samples_toread;
        samples_read += samples_toread;
        numsamples -= samples_toread;

        num_samples_left -= samples_toread << 10;
    }
#elif (DRIVER==ROLAND) || (DRIVER==EMU8000)
#ifdef USE_SPEEXDSP_RESAMPLER
    if (resampler != NULL)
    {
        while (numsamples != 0)
        {
            int samples_toread;
            spx_uint32_t in_len, out_len;
            int err;
            unsigned int index;

            if (num_samples_left == 0)
            {
                if (!IsSequencePlaying()) break;
                ProcessSequence();

                num_samples_left = SYNTH_BUFFER_LEN;
            }

            samples_toread = (((numsamples + 1) * resample_step) >> 32) + 8;
            if (samples_toread & 7) samples_toread += 8 - (samples_toread & 7);
            if (samples_toread > RESAMPLER_BUFFER_LEN) samples_toread = RESAMPLER_BUFFER_LEN;

            samples_toread -= resample_num_samples;
            if (samples_toread > num_samples_left) samples_toread = num_samples_left;

#if (DRIVER==ROLAND)
            emu_mt32_getsamples(&(resample_samples[2 * resample_num_samples]), samples_toread);
#elif (DRIVER==EMU8000)
            emu_awe32_getsamples(&(resample_samples[2 * resample_num_samples]), samples_toread);
#endif

            resample_num_samples += samples_toread;
            num_samples_left -= samples_toread;

            in_len = resample_num_samples;
            out_len = numsamples;

            err = speex_resampler_process_interleaved_int(resampler, resample_samples, &in_len, buf, &out_len);
            if (err != RESAMPLER_ERR_SUCCESS) break;

            buf += 2 * out_len;
            samples_read += out_len;
            numsamples -= out_len;

            if (in_len)
            {
                for (index = 0; in_len + index < resample_num_samples; index++)
                {
                    resample_samples[2 * index] = resample_samples[2 * (in_len + index)];
                    resample_samples[2 * index + 1] = resample_samples[2 * (in_len + index) + 1];
                }

                resample_num_samples -= in_len;
            }
        }
    }
    else
#endif
    if (sample_rate == BASE_SAMPLE_RATE)
    {
        while (numsamples != 0)
        {
            int samples_toread;

            if (num_samples_left == 0)
            {
                if (!IsSequencePlaying()) break;
                ProcessSequence();

                num_samples_left = SYNTH_BUFFER_LEN;
            }

            samples_toread = (numsamples <= num_samples_left)?numsamples:num_samples_left;

#if (DRIVER==ROLAND)
            emu_mt32_getsamples(buf, samples_toread);
#elif (DRIVER==EMU8000)
            emu_awe32_getsamples(buf, samples_toread);
#endif

            buf += 2 * samples_toread;
            samples_read += samples_toread;
            numsamples -= samples_toread;

            num_samples_left -= samples_toread;
        }
    }
    else
    {
        while (numsamples != 0)
        {
            int position, samples_toread;

            position = current_position >> 32;
            if (position >= SYNTH_BUFFER_LEN)
            {
                if (!IsSequencePlaying()) break;
                ProcessSequence();

                resample_samples[0] = resample_samples[2*SYNTH_BUFFER_LEN];
                resample_samples[1] = resample_samples[2*SYNTH_BUFFER_LEN+1];

#if (DRIVER==ROLAND)
                emu_mt32_getsamples(&(resample_samples[2]), SYNTH_BUFFER_LEN);
#elif (DRIVER==EMU8000)
                emu_awe32_getsamples(&(resample_samples[2]), SYNTH_BUFFER_LEN);
#endif

                current_position -= (((uint64_t)SYNTH_BUFFER_LEN) << 32);
                position = current_position >> 32;
            }

            samples_toread = (((SYNTH_BUFFER_LEN - 1) - position) * samples_mul) >> 32;
            if (numsamples < samples_toread) samples_toread = numsamples;

            samples_read += samples_toread;
            numsamples -= samples_toread;

            for (; samples_toread != 0; samples_toread--)
            {
                buf[0] = resample_samples[2*position] + (((resample_samples[2*(position + 1)] - resample_samples[2*position]) * ((int32_t)(((uint32_t)current_position) >> 16))) >> 16);
                buf[1] = resample_samples[2*position+1] + (((resample_samples[2*(position + 1)+1] - resample_samples[2*position+1]) * ((int32_t)(((uint32_t)current_position) >> 16))) >> 16);

                buf += 2;

                current_position += position_add;
                position = current_position >> 32;
            }

            while ((numsamples != 0) && (position < SYNTH_BUFFER_LEN))
            {
                buf[0] = resample_samples[2*position] + (((resample_samples[2*(position + 1)] - resample_samples[2*position]) * ((int32_t)(((uint32_t)current_position) >> 16))) >> 16);
                buf[1] = resample_samples[2*position+1] + (((resample_samples[2*(position + 1)+1] - resample_samples[2*position+1]) * ((int32_t)(((uint32_t)current_position) >> 16))) >> 16);

                buf += 2;

                samples_read++;
                numsamples--;

                current_position += position_add;
                position = current_position >> 32;
            }
        }
    }
#endif

    return samples_read << 2;
}

int emu_x86_rewindsequence(void)
{
    RewindSequence();
#ifdef USE_SPEEXDSP_RESAMPLER
    resample_num_samples = 0;
#endif
    return 1;
}

int emu_x86_stopsequence(void)
{
    StopSequence();
    return 1;
}

void emu_x86_shutdown(void)
{
    StopMusic();
    free(memory);
    memory = NULL;
    x86.int66_offset = 0;
#ifdef USE_SPEEXDSP_RESAMPLER
    if (resampler != NULL)
    {
        speex_resampler_destroy(resampler);
        resampler = NULL;
    }
#endif
#if (DRIVER==ROLAND)
    emu_mt32_shutdown();
#elif (DRIVER==EMU8000)
    emu_awe32_shutdown();
#endif
}

