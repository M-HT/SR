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

#if !defined(_GAME_INT3_H_INCLUDED_)
#define _GAME_INT3_H_INCLUDED_

#define CARRY_FLAG     0x01
#define PARITY_FLAG    0x04
#define AUXILIARY_FLAG 0x10
#define ZERO_FLAG      0x40
#define SIGN_FLAG      0x80

#define EAX (regs->d.eax)
#define EBX (regs->d.ebx)
#define ECX (regs->d.ecx)
#define EDX (regs->d.edx)

#define ESI (regs->d.esi)
#define EDI (regs->d.edi)
#define EBP (regs->d.ebp)

#define AX (regs->w.ax)
#define BX (regs->w.bx)
#define CX (regs->w.cx)
#define DX (regs->w.dx)

#define SI (regs->w.si)
#define DI (regs->w.di)
#define SP (regs->w.sp)
#define BP (regs->w.bp)

#define FLAGS (regs->w.flags)

#define AL (regs->h.al)
#define BL (regs->h.bl)
#define CL (regs->h.cl)
#define DL (regs->h.dl)

#define AH (regs->h.ah)
#define BH (regs->h.bh)
#define CH (regs->h.ch)
#define DH (regs->h.dh)

#define DS (regs->w.ds)
#define ES (regs->w.es)
#define FS (regs->w.fs)
#define SS (regs->w.ss)


#define SET_FLAG(x) {FLAGS |= (x);}
#define CLEAR_FLAG(x) {FLAGS &= ~(x);}

#endif /* _GAME_INT3_H_INCLUDED_ */
