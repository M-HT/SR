/**
 *
 *  Copyright (C) 2016-2026 Roman Pauer
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

#if !defined(_GAME_INT_H_INCLUDED_)
#define _GAME_INT_H_INCLUDED_

#define CARRY_FLAG     0x01
#define PARITY_FLAG    0x04
#define AUXILIARY_FLAG 0x10
#define ZERO_FLAG      0x40
#define SIGN_FLAG      0x80

#define EAX (regs->_eax.e)
#define EBX (regs->_ebx.e)
#define ECX (regs->_ecx.e)
#define EDX (regs->_edx.e)

#define ESI (regs->_esi.e)
#define EDI (regs->_edi.e)
#define ESP (regs->_esp.e)
#define EBP (regs->_ebp.e)

#define EIP (regs->_eip.e)

#define EFLAGS (regs->_eflags.e)

#define AX (regs->_eax.w.x)
#define BX (regs->_ebx.w.x)
#define CX (regs->_ecx.w.x)
#define DX (regs->_edx.w.x)

#define SI (regs->_esi.w.x)
#define DI (regs->_edi.w.x)
#define SP (regs->_esp.w.x)
#define BP (regs->_ebp.w.x)

#define IP (regs->_eip.w.x)

#define FLAGS (regs->_eflags.w.x)

#define AL (regs->_eax.b.l)
#define BL (regs->_ebx.b.l)
#define CL (regs->_ecx.b.l)
#define DL (regs->_edx.b.l)

#define AH (regs->_eax.b.h)
#define BH (regs->_ebx.b.h)
#define CH (regs->_ecx.b.h)
#define DH (regs->_edx.b.h)

#define PTR_EAX (regs->_eax.p)
#define PTR_EBX (regs->_ebx.p)
#define PTR_ECX (regs->_ecx.p)
#define PTR_EDX (regs->_edx.p)

#define PTR_ESI (regs->_esi.p)
#define PTR_EDI (regs->_edi.p)
#define PTR_ESP (regs->_esp.p)
#define PTR_EBP (regs->_ebp.p)

#define PTR_EIP (regs->_eip.p)


#define SET_FLAG(x) {EFLAGS |= (x);}
#define CLEAR_FLAG(x) {EFLAGS &= ~(x);}

#endif /* _GAME_INT_H_INCLUDED_ */
