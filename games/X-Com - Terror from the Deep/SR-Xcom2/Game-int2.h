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

#if !defined(_GAME_INT2_H_INCLUDED_)
#define _GAME_INT2_H_INCLUDED_

#define CARRY_FLAG     0xffffffff

#define EAX (tmp_regs.d.eax)
#define EBX (tmp_regs.d.ebx)
#define ECX (tmp_regs.d.ecx)
#define EDX (tmp_regs.d.edx)

#define ESI (tmp_regs.d.esi)
#define EDI (tmp_regs.d.edi)

#define AX (tmp_regs.w.ax)
#define BX (tmp_regs.w.bx)
#define CX (tmp_regs.w.cx)
#define DX (tmp_regs.w.dx)

#define SI (tmp_regs.w.si)
#define DI (tmp_regs.w.di)

#define AL (tmp_regs.h.al)
#define BL (tmp_regs.h.bl)
#define CL (tmp_regs.h.cl)
#define DL (tmp_regs.h.dl)

#define AH (tmp_regs.h.ah)
#define BH (tmp_regs.h.bh)
#define CH (tmp_regs.h.ch)
#define DH (tmp_regs.h.dh)

#define CS (seg_regs->cs)
#define DS (seg_regs->ds)
#define ES (seg_regs->es)
#define FS (seg_regs->fs)
#define GS (seg_regs->gs)
#define SS (seg_regs->ss)

#define SET_FLAG(x) {tmp_regs.d.cflag |= (x);}
#define CLEAR_FLAG(x) {tmp_regs.d.cflag &= ~(x);}


#define IN_EAX (in_regs->d.eax)
#define IN_EBX (in_regs->d.ebx)
#define IN_ECX (in_regs->d.ecx)
#define IN_EDX (in_regs->d.edx)

#define IN_ESI (in_regs->d.esi)
#define IN_EDI (in_regs->d.edi)

#define IN_AX (in_regs->w.ax)
#define IN_BX (in_regs->w.bx)
#define IN_CX (in_regs->w.cx)
#define IN_DX (in_regs->w.dx)

#define IN_SI (in_regs->w.si)
#define IN_DI (in_regs->w.di)

#define IN_AL (in_regs->h.al)
#define IN_BL (in_regs->h.bl)
#define IN_CL (in_regs->h.cl)
#define IN_DL (in_regs->h.dl)

#define IN_AH (in_regs->h.ah)
#define IN_BH (in_regs->h.bh)
#define IN_CH (in_regs->h.ch)
#define IN_DH (in_regs->h.dh)

#endif /* _GAME_INT2_H_INCLUDED_ */
