/* -----------------------------------------------------------------------------
 * decode.c
 *
 * Copyright (c) 2005, 2006, Vivek Mohan <vivek@sig9.com>
 * All rights reserved. See LICENSE
 * -----------------------------------------------------------------------------
 */

#include <string.h>
#include "types.h"
#include "input.h"
#include "opcmap.h"
#include "mnemonics.h"

/* The max number of prefixes to an instruction */
#define MAX_PREFIXES	15

/* register types */
#define T_NONE	0
#define T_GPR	1
#define T_MMX	2
#define T_CRG	3
#define T_DBG	4
#define T_SEG	5
#define T_XMM	6

struct map_entry* ud_me_db();
struct map_entry* ud_me_invalid();

/* -----------------------------------------------------------------------------
 * resolve_oprsize()- Resolves the size of operand depending on the current
 * disassembly mode, effective operand sizes, etc.
 * -----------------------------------------------------------------------------
 */
static unsigned int 
resolve_oprsize(register struct ud* u, unsigned int s)
{
  switch (s) {
	case SZ_V:	return (u->opr_mode);
	case SZ_Z:	return (u->opr_mode == 16) ? 16 : 32;
	case SZ_P:	return (u->opr_mode == 16) ? SZ_WP : SZ_DP;
	case SZ_MDQ:	return (u->opr_mode == 16) ? 32 : u->opr_mode;
	case SZ_RDQ:	return (u->dis_mode == 64) ? 64 : 32;
	default:	return s;
  }
}

/* -----------------------------------------------------------------------------
 * resolve_mnemonic()- Resolves the correct mnemonic that depends on the 
 * current effective operand or address mode.
 * -----------------------------------------------------------------------------
 */
static enum ud_mnemonic_code resolve_mnemonic_by_mode( struct ud* u )
{
  /* operand size == 32 */
  if ( u->opr_mode == 32 ) {
	switch ( u->mnemonic ) {
		case UD_Icbw: 	return UD_Icwde;
		case UD_Icwd: 	return UD_Icdq;
		case UD_Ilodsw: return UD_Ilodsd;
		case UD_Imovsw: return UD_Imovsd;
		case UD_Icmpsw: return UD_Icmpsd;
		case UD_Iinsw: 	return UD_Iinsd;
		case UD_Ioutsw: return UD_Ioutsd;
		case UD_Ipushfw:return UD_Ipushfd;
		case UD_Ipopfw: return UD_Ipopfd;
		case UD_Istosw: return UD_Istosd;
		case UD_Iscasw: return UD_Iscasd;
		case UD_Iiretw: return UD_Iiretd;
		case UD_Ipusha:	return UD_Ipushad;
		case UD_Ipopa:	return UD_Ipopad;
		default: 	break;
	}
  } /* operand size == 64*/
  else if ( u->opr_mode == 64 ) {
	switch( u->mnemonic ) {
		case UD_Icbw:	return UD_Icdqe;
		case UD_Icwd:	return UD_Icqo;
		case UD_Ilodsw: return UD_Ilodsq;
		case UD_Imovsw: return UD_Imovsq;
		case UD_Icmpsw: return UD_Icmpsq;
		case UD_Iinsw:	return UD_Iinsd;
		case UD_Ioutsw:	return UD_Ioutsd;
		case UD_Icmpxchg8b: return UD_Icmpxchg16b;		
		case UD_Istosw:	return UD_Istosq;
		case UD_Iscasw:	return UD_Iscasq;
		case UD_Iiretw:	return UD_Iiretq;
		case UD_Ipushfw:return UD_Ipushfq;
		case UD_Ipopfw:	return UD_Ipopfq;
		default: 	break; 
	} 
  }

  /* address == 32 */
  if ( u->adr_mode == 32 ) {
	switch( u->mnemonic ) {
		case UD_Ijcxz:	return UD_Ijecxz;
		default:	break; 
	} 
  } /* address == 64 */
  else if ( u->adr_mode == 64 ) {
	switch( u->mnemonic ) {
		case UD_Ijcxz:	return UD_Ijrcxz;
		default:	break; 
	} 
  }

  return u->mnemonic;
} 

static int resolve_mnemonic( struct ud* u )
{
  /* the opcode map has hardcoded information about the
   * dependency of the instruction mnemonic on the mode.
   */
  if ( P_DEPM( u->mapen->prefix ) ) {
	u->mnemonic = resolve_mnemonic_by_mode( u );
  }
  /* far/near flags */
  u->br_far = 0;
  u->br_near = 0;
  /* readjust operand sizes for call/jmp instrcutions */
  if ( u->mnemonic == UD_Icall || u->mnemonic == UD_Ijmp ) {
	/* WP: 16bit pointer */
	if ( u->operand[ 0 ].size == SZ_WP ) {
		u->operand[ 0 ].size = 16;
		u->br_far = 1;
		u->br_near= 0;
	/* DP: 32bit pointer */
	} else if ( u->operand[ 0 ].size == SZ_DP ) {
		u->operand[ 0 ].size = 32;
		u->br_far = 1;
		u->br_near= 0;
	} else {
		u->br_far = 0;
		u->br_near= 1;
	}
  /* resolve 3dnow weirdness. */
  } else if ( u->mnemonic == UD_I3dnow ) {
	u->mnemonic = ud_map_get_3dnow( inp_curr( u ) );
  }
  /* SWAPGS is only valid in 64bits mode */
  if ( u->mnemonic == UD_Iswapgs && u->dis_mode != 64 ) {
	u->error = 1;
	return -1;
  }

  return 0;
}


/* -----------------------------------------------------------------------------
 * decode_a()- Decodes operands of the type seg:offset
 * -----------------------------------------------------------------------------
 */
static void 
decode_a(struct ud* u, struct ud_operand *op)
{
  if (u->opr_mode == 16) {	
	/* seg16:off16 */
	op->type = UD_OP_PTR;
	op->size = 32;
	op->lval.ptr.off = inp_uint16(u);
	op->lval.ptr.seg = inp_uint16(u);
  } else {
	/* seg16:off32 */
	op->type = UD_OP_PTR;
	op->size = 48;
	op->lval.ptr.off = inp_uint32(u);
	op->lval.ptr.seg = inp_uint16(u);
  }
}

/* -----------------------------------------------------------------------------
 * decode_gpr() - Returns decoded General Purpose Register 
 * -----------------------------------------------------------------------------
 */
static enum ud_type 
decode_gpr(register struct ud* u, unsigned int s, unsigned char rm)
{
  s = resolve_oprsize(u, s);
		
  switch (s) {
	case 64:
		return UD_R_RAX + rm;
	case SZ_DP:
	case 32:
		return UD_R_EAX + rm;
	case SZ_WP:
	case 16:
		return UD_R_AX  + rm;
	case  8:
		if (u->dis_mode == 64 && u->pfx_rex) {
			if (rm >= 4)
				return UD_R_SPL + (rm-4);
			return UD_R_AL + rm;
		} else return UD_R_AL + rm;
	default:
		return 0;
  }
}

/* -----------------------------------------------------------------------------
 * resolve_gpr64() - 64bit General Purpose Register-Selection. 
 * -----------------------------------------------------------------------------
 */
static enum ud_type 
resolve_gpr64(struct ud* u, enum map_operand_type gpr_op)
{
  if (gpr_op >= OP_rAXr8 && gpr_op <= OP_rDIr15)
	gpr_op = (gpr_op - OP_rAXr8) | (P_REX_B(u->pfx_rex) << 3);			
  else	gpr_op = (gpr_op - OP_rAX);

  if (u->opr_mode == 16)
	return gpr_op + UD_R_AX;
  if (u->dis_mode == 32 || 
	(u->opr_mode == 32 && ! (P_REX_W(u->pfx_rex) || u->default64))) {
	return gpr_op + UD_R_EAX;
  }

  return gpr_op + UD_R_RAX;
}

/* -----------------------------------------------------------------------------
 * resolve_gpr32 () - 32bit General Purpose Register-Selection. 
 * -----------------------------------------------------------------------------
 */
static enum ud_type 
resolve_gpr32(struct ud* u, enum map_operand_type gpr_op)
{
  gpr_op = gpr_op - OP_eAX;

  if (u->opr_mode == 16) 
	return gpr_op + UD_R_AX;

  return gpr_op +  UD_R_EAX;
}

/* -----------------------------------------------------------------------------
 * resolve_reg() - Resolves the register type 
 * -----------------------------------------------------------------------------
 */
static enum ud_type 
resolve_reg(struct ud* u, unsigned int type, unsigned char i)
{
  switch (type) {
	case T_MMX :	return UD_R_MM0  + (i & 7);
	case T_XMM :	return UD_R_XMM0 + i;
	case T_CRG :	return UD_R_CR0  + i;
	case T_DBG :	return UD_R_DR0  + i;
	case T_SEG :	return UD_R_ES   + (i & 7);
	case T_NONE:
	default:	return UD_NONE;
  }
}

/* -----------------------------------------------------------------------------
 * decode_imm() - Decodes Immediate values.
 * -----------------------------------------------------------------------------
 */
static void 
decode_imm(struct ud* u, unsigned int s, struct ud_operand *op)
{
  op->size = resolve_oprsize(u, s);
  op->type = UD_OP_IMM;

  switch (op->size) {
	case  8: op->lval.sbyte = inp_uint8(u);	  break;
	case 16: op->lval.uword = inp_uint16(u);  break;
	case 32: op->lval.udword = inp_uint32(u); break;
	case 64: op->lval.uqword = inp_uint64(u); break;
	default: return;
  }
}

/* -----------------------------------------------------------------------------
 * decode_modrm() - Decodes ModRM Byte
 * -----------------------------------------------------------------------------
 */
static void 
decode_modrm(struct ud* u, struct ud_operand *op, unsigned int s, 
	     unsigned char rm_type, struct ud_operand *opreg, 
	     unsigned char reg_size, unsigned char reg_type)
{
  unsigned char mod, rm, reg;

  inp_next(u);

  /* get mod, r/m and reg fields */
  mod = MODRM_MOD(inp_curr(u));
  rm  = (P_REX_B(u->pfx_rex) << 3) | MODRM_RM(inp_curr(u));
  reg = (P_REX_R(u->pfx_rex) << 3) | MODRM_REG(inp_curr(u));

  op->size = resolve_oprsize(u, s);

  /* if mod is 11b, then the UD_R_m specifies a gpr/mmx/sse/control/debug */
  if (mod == 3) {
	op->type = UD_OP_REG;
	if (rm_type == 	T_GPR)
		op->base = decode_gpr(u, op->size, rm);
	else	op->base = resolve_reg(u, rm_type, (P_REX_B(u->pfx_rex) << 3) | (rm&7));
  } 
  /* else its memory addressing */  
  else {
	op->type = UD_OP_MEM;

	/* 64bit addressing */
	if (u->adr_mode == 64) {

		op->base = UD_R_RAX + rm;

		/* get offset type */
		if (mod == 1)
			op->offset = 8;
		else if (mod == 2)
			op->offset = 32;
		else if (mod == 0 && (rm & 7) == 5) {			
			op->base = UD_R_RIP;
			op->offset = 32;
		} else  op->offset = 0;

		/* Scale-Index-Base (SIB) */
		if ((rm & 7) == 4) {
			inp_next(u);
			
			op->scale = (1 << SIB_S(inp_curr(u))) & ~1;
			op->index = UD_R_RAX + (SIB_I(inp_curr(u)) | (P_REX_X(u->pfx_rex) << 3));
			op->base  = UD_R_RAX + (SIB_B(inp_curr(u)) | (P_REX_B(u->pfx_rex) << 3));

			/* special conditions for base reference */
			if (op->index == UD_R_RSP) {
				op->index = UD_NONE;
				op->scale = UD_NONE;
			}

			if (op->base == UD_R_RBP || op->base == UD_R_R13) {
				if (mod == 0) 
					op->base = UD_NONE;
				if (mod == 1)
					op->offset = 8;
				else op->offset = 32;
			}
		}
	} 

	/* 32-Bit addressing mode */
	else if (u->adr_mode == 32) {

		/* get base */
		op->base = UD_R_EAX + rm;

		/* get offset type */
		if (mod == 1)
			op->offset = 8;
		else if (mod == 2)
			op->offset = 32;
		else if (mod == 0 && rm == 5) {
			op->base = UD_NONE;
			op->offset = 32;
		} else  op->offset = 0;

		/* Scale-Index-Base (SIB) */
		if ((rm & 7) == 4) {
			inp_next(u);

			op->scale = (1 << SIB_S(inp_curr(u))) & ~1;
			op->index = UD_R_EAX + (SIB_I(inp_curr(u)) | (P_REX_X(u->pfx_rex) << 3));
			op->base  = UD_R_EAX + (SIB_B(inp_curr(u)) | (P_REX_B(u->pfx_rex) << 3));

			if (op->index == UD_R_ESP) {
				op->index = UD_NONE;
				op->scale = UD_NONE;
			}

			/* special condition for base reference */
			if (op->base == UD_R_EBP) {
				if (mod == 0)
					op->base = UD_NONE;
				if (mod == 1)
					op->offset = 8;
				else op->offset = 32;
			}
		}
	} 

	/* 16bit addressing mode */
	else  {
		switch (rm) {
			case 0:	op->base = UD_R_BX; op->index = UD_R_SI; break;
			case 1: op->base = UD_R_BX; op->index = UD_R_DI; break;
			case 2: op->base = UD_R_BP; op->index = UD_R_SI; break;
			case 3: op->base = UD_R_BP; op->index = UD_R_DI; break;
			case 4:	op->base = UD_R_SI; break;
			case 5: op->base = UD_R_DI; break;
			case 6: op->base = UD_R_BP; break;
			case 7: op->base = UD_R_BX; break;
		}

		if (mod == 0 && rm == 6) {
			op->offset= 16;
			op->base = UD_NONE;
		}
		else if (mod == 1)
			op->offset = 8;
		else if (mod == 2) 
			op->offset = 16;
	}
  }  

  /* extract offset, if any */
  switch(op->offset) {
	case 8 : op->lval.ubyte  = inp_uint8(u);  break;
	case 16: op->lval.uword  = inp_uint16(u);  break;
	case 32: op->lval.udword = inp_uint32(u); break;
	case 64: op->lval.uqword = inp_uint64(u); break;
	default: break;
  }

  /* resolve register encoded in reg field */
  if (opreg) {
	opreg->type = UD_OP_REG;
	opreg->size = resolve_oprsize(u, reg_size);
	if (reg_type == T_GPR) 
		opreg->base = decode_gpr(u, opreg->size, reg);
	else opreg->base = resolve_reg(u, reg_type, reg);
  }
}

/* -----------------------------------------------------------------------------
 * decode_o() - Decodes offset
 * -----------------------------------------------------------------------------
 */
static void 
decode_o(struct ud* u, unsigned int s, struct ud_operand *op)
{
  switch (u->adr_mode) {
	case 64:
		op->offset = 64; 
		op->lval.uqword = inp_uint64(u); 
		break;
	case 32:
		op->offset = 32; 
		op->lval.udword = inp_uint32(u); 
		break;
	case 16:
		op->offset = 16; 
		op->lval.uword  = inp_uint16(u); 
		break;
	default:
		return;
  }
  op->type = UD_OP_MEM;
  op->size = resolve_oprsize(u, s);
}

/* -----------------------------------------------------------------------------
 * disasm_operands() - Disassembles Operands.
 * -----------------------------------------------------------------------------
 */
static int disasm_operands(register struct ud* u)
{


  /* mopXt = map entry, operand X, type; */
  enum map_operand_type mop1t = u->mapen->operand1.type;
  enum map_operand_type mop2t = u->mapen->operand2.type;
  enum map_operand_type mop3t = u->mapen->operand3.type;

  /* mopXs = map entry, operand X, size */
  unsigned int mop1s = u->mapen->operand1.size;
  unsigned int mop2s = u->mapen->operand2.size;
  unsigned int mop3s = u->mapen->operand3.size;

  /* iop = instruction operand */
  register struct ud_operand* iop = u->operand;
	
  switch(mop1t) {
	
	case OP_A :
		decode_a(u, &(iop[0]));
		break;
	
	/* M[b] ... */
	case OP_M :
		if (MODRM_MOD(inp_peek(u)) == 3)
			u->error= 1;
	/* E, G/P/V/I/CL/1/S */
	case OP_E :
		if (mop2t == OP_G) {
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_GPR);
			if (mop3t == OP_I)
				decode_imm(u, mop3s, &(iop[2]));
			else if (mop3t == OP_CL) {
				iop[2].type = UD_OP_REG;
				iop[2].base = UD_R_CL;
				iop[2].size = 8;
			}
		}
		else if (mop2t == OP_P)
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_MMX);
		else if (mop2t == OP_V)
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_XMM);
		else if (mop2t == OP_S)
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_SEG);
		else {
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, NULL, 0, T_NONE);
			if (mop2t == OP_CL) {
				iop[1].type = UD_OP_REG;
				iop[1].base = UD_R_CL;
				iop[1].size = 8;
			} else if (mop2t == OP_I1) {
				iop[1].type = UD_OP_CONST;
				u->operand[1].lval.udword = 1;
			} else if (mop2t == OP_I) {
				decode_imm(u, mop2s, &(iop[1]));
			}
		}
		break;

	/* G, E/PR[,I]/VR */
	case OP_G :

		if (mop2t == OP_M) {
			if (MODRM_MOD(inp_peek(u)) == 3)
				u->error= 1;
			decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_GPR);
		} else if (mop2t == OP_E) {
			decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_GPR);
			if (mop3t == OP_I)
				decode_imm(u, mop3s, &(iop[2]));
		} else if (mop2t == OP_PR) {
			decode_modrm(u, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_GPR);
			if (mop3t == OP_I)
				decode_imm(u, mop3s, &(iop[2]));
		} else if (mop2t == OP_VR) {
			if (MODRM_MOD(inp_peek(u)) != 3)
				u->error = 1;
			decode_modrm(u, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_GPR);
		} else if (mop2t == OP_W)
			decode_modrm(u, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_GPR);
		break;

	/* AL..BH, I/O/DX */
	case OP_AL : case OP_CL : case OP_DL : case OP_BL :
	case OP_AH : case OP_CH : case OP_DH : case OP_BH :

		iop[0].type = UD_OP_REG;
		iop[0].base = UD_R_AL + (mop1t - OP_AL);
		iop[0].size = 8;

		if (mop2t == OP_I)
			decode_imm(u, mop2s, &(iop[1]));
		else if (mop2t == OP_DX) {
			iop[1].type = UD_OP_REG;
			iop[1].base = UD_R_DX;
			iop[1].size = 16;
		}
		else if (mop2t == OP_O)
			decode_o(u, mop2s, &(iop[1]));
		break;

	/* rAX[r8]..rDI[r15], I/rAX..rDI/O */
	case OP_rAXr8 : case OP_rCXr9 : case OP_rDXr10 : case OP_rBXr11 :
	case OP_rSPr12: case OP_rBPr13: case OP_rSIr14 : case OP_rDIr15 :
	case OP_rAX : case OP_rCX : case OP_rDX : case OP_rBX :
	case OP_rSP : case OP_rBP : case OP_rSI : case OP_rDI :

		iop[0].type = UD_OP_REG;
		iop[0].base = resolve_gpr64(u, mop1t);

		if (mop2t == OP_I)
			decode_imm(u, mop2s, &(iop[1]));
		else if (mop2t >= OP_rAX && mop2t <= OP_rDI) {
			iop[1].type = UD_OP_REG;
			iop[1].base = resolve_gpr64(u, mop2t);
		}
		else if (mop2t == OP_O) {
			decode_o(u, mop2s, &(iop[1]));	
			iop[0].size = resolve_oprsize(u, mop2s);
		}
		break;

	/* AL[r8b]..BH[r15b], I */
	case OP_ALr8b : case OP_CLr9b : case OP_DLr10b : case OP_BLr11b :
	case OP_AHr12b: case OP_CHr13b: case OP_DHr14b : case OP_BHr15b :
	{
		ud_type_t gpr = (mop1t - OP_ALr8b) + UD_R_AL + 
						(P_REX_B(u->pfx_rex) << 3);
		if (UD_R_AH <= gpr && u->pfx_rex)
			gpr = gpr + 4;
		iop[0].type = UD_OP_REG;
		iop[0].base = gpr;
		if (mop2t == OP_I)
			decode_imm(u, mop2s, &(iop[1]));
		break;
	}

	/* eAX..eDX, DX/I */
	case OP_eAX : case OP_eCX : case OP_eDX : case OP_eBX :
	case OP_eSP : case OP_eBP : case OP_eSI : case OP_eDI :

		iop[0].type = UD_OP_REG;
		iop[0].base = resolve_gpr32(u, mop1t);
		if (mop2t == OP_DX) {
			iop[1].type = UD_OP_REG;
			iop[1].base = UD_R_DX;
			iop[1].size = 16;
		} else if (mop2t == OP_I)
			decode_imm(u, mop2s, &(iop[1]));
		break;

	/* ES..GS */
	case OP_ES : case OP_CS : case OP_DS :
	case OP_SS : case OP_FS : case OP_GS :

		/* in 64bits mode, only fs and gs are allowed */
		if (u->dis_mode == 64)
			if (mop1t != OP_FS && mop1t != OP_GS)
				u->error= 1;
		iop[0].type = UD_OP_REG;
		iop[0].base = (mop1t - OP_ES) + UD_R_ES;
		iop[0].size = 16;

		break;

	/* J */
	case OP_J :
		decode_imm(u, mop1s, &(iop[0]));		
		iop[0].type = UD_OP_JIMM;
		break ;

	/* PR, I */
	case OP_PR:
		if (MODRM_MOD(inp_peek(u)) != 3)
			u->error = 1;
		decode_modrm(u, &(iop[0]), mop1s, T_MMX, NULL, 0, T_NONE);
		if (mop2t == OP_I)
			decode_imm(u, mop2s, &(iop[1]));
		break; 

	/* VR, I */
	case OP_VR:
		if (MODRM_MOD(inp_peek(u)) != 3)
			u->error = 1;
		decode_modrm(u, &(iop[0]), mop1s, T_XMM, NULL, 0, T_NONE);
		if (mop2t == OP_I)
			decode_imm(u, mop2s, &(iop[1]));
		break; 

	/* P, Q[,I]/W/E[,I],VR */
	case OP_P :
		if (mop2t == OP_Q) {
			decode_modrm(u, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_MMX);
			if (mop3t == OP_I)
				decode_imm(u, mop3s, &(iop[2]));
		} else if (mop2t == OP_W) {
			decode_modrm(u, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_MMX);
		} else if (mop2t == OP_VR) {
			if (MODRM_MOD(inp_peek(u)) != 3)
				u->error = 1;
			decode_modrm(u, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_MMX);
		} else if (mop2t == OP_E) {
			decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_MMX);
			if (mop3t == OP_I)
				decode_imm(u, mop3s, &(iop[2]));
		}
		break;

	/* R, C/D */
	case OP_R :
		if (mop2t == OP_C)
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_CRG);
		else if (mop2t == OP_D)
			decode_modrm(u, &(iop[0]), mop1s, T_GPR, &(iop[1]), mop2s, T_DBG);
		break;

	/* C, R */
	case OP_C :
		decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_CRG);
		break;

	/* D, R */
	case OP_D :
		decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_DBG);
		break;

	/* Q, P */
	case OP_Q :
		decode_modrm(u, &(iop[0]), mop1s, T_MMX, &(iop[1]), mop2s, T_MMX);
		break;

	/* S, E */
	case OP_S :
		decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_SEG);
		break;

	/* W, V */
	case OP_W :
		decode_modrm(u, &(iop[0]), mop1s, T_XMM, &(iop[1]), mop2s, T_XMM);
		break;

	/* V, W[,I]/Q/M/E */
	case OP_V :
		if (mop2t == OP_W) {
			/* special cases for movlps and movhps */
			if (MODRM_MOD(inp_peek(u)) == 3) {
				if (u->mnemonic == UD_Imovlps)
					u->mnemonic = UD_Imovhlps;
				else
				if (u->mnemonic == UD_Imovhps)
					u->mnemonic = UD_Imovlhps;
			}
			decode_modrm(u, &(iop[1]), mop2s, T_XMM, &(iop[0]), mop1s, T_XMM);
			if (mop3t == OP_I)
				decode_imm(u, mop3s, &(iop[2]));
		} else if (mop2t == OP_Q)
			decode_modrm(u, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_XMM);
		else if (mop2t == OP_M) {
			if (MODRM_MOD(inp_peek(u)) == 3)
				u->error= 1;
			decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_XMM);
		} else if (mop2t == OP_E) {
			decode_modrm(u, &(iop[1]), mop2s, T_GPR, &(iop[0]), mop1s, T_XMM);
		} else if (mop2t == OP_PR) {
			decode_modrm(u, &(iop[1]), mop2s, T_MMX, &(iop[0]), mop1s, T_XMM);
		}
		break;

	/* DX, eAX/AL */
	case OP_DX :
		iop[0].type = UD_OP_REG;
		iop[0].base = UD_R_DX;
		iop[0].size = 16;

		if (mop2t == OP_eAX) {
			iop[1].type = UD_OP_REG;	
			iop[1].base = resolve_gpr32(u, mop2t);
		} else if (mop2t == OP_AL) {
			iop[1].type = UD_OP_REG;
			iop[1].base = UD_R_AL;
			iop[1].size = 8;
		}

		break;

	/* I, I/AL/eAX */
	case OP_I :
		decode_imm(u, mop1s, &(iop[0]));
		if (mop2t == OP_I)
			decode_imm(u, mop2s, &(iop[1]));
		else if (mop2t == OP_AL) {
			iop[1].type = UD_OP_REG;
			iop[1].base = UD_R_AL;
			iop[1].size = 16;
		} else if (mop2t == OP_eAX) {
			iop[1].type = UD_OP_REG;	
			iop[1].base = resolve_gpr32(u, mop2t);
		}
		break;

	/* O, AL/eAX */
	case OP_O :
		decode_o(u, mop1s, &(iop[0]));
		iop[1].type = UD_OP_REG;
		iop[1].size = resolve_oprsize(u, mop1s);
		if (mop2t == OP_AL)
			iop[1].base = UD_R_AL;
		else if (mop2t == OP_eAX)
			iop[1].base = resolve_gpr32(u, mop2t);
		else if (mop2t == OP_rAX)
			iop[1].base = resolve_gpr64(u, mop2t);		
		break;

	/* 3 */
	case OP_I3 :
		iop[0].type = UD_OP_CONST;
		iop[0].lval.sbyte = 3;
		break;

	/* ST(n), ST(n) */
	case OP_ST0 : case OP_ST1 : case OP_ST2 : case OP_ST3 :
	case OP_ST4 : case OP_ST5 : case OP_ST6 : case OP_ST7 :

		iop[0].type = UD_OP_REG;
		iop[0].base = (mop1t-OP_ST0) + UD_R_ST0;
		iop[0].size = 0;

		if (mop2t >= OP_ST0 && mop2t <= OP_ST7) {
			iop[1].type = UD_OP_REG;
			iop[1].base = (mop2t-OP_ST0) + UD_R_ST0;
			iop[1].size = 0;
		}
		break;

	/* AX */
	case OP_AX:
		iop[0].type = UD_OP_REG;
		iop[0].base = UD_R_AX;
		iop[0].size = 16;
		break;

	/* none */
	default :
		iop[0].type = iop[1].type = iop[2].type = UD_NONE;
  }

  return 0;
}

/* -----------------------------------------------------------------------------
 * clear_insn() - clear instruction pointer 
 * -----------------------------------------------------------------------------
 */
static int clear_insn(register struct ud* u)
{
  u->error = 0;
  u->pfx_seg = 0;
  u->pfx_opr = 0;
  u->pfx_adr = 0;
  u->pfx_lock = 0;
  u->pfx_repne = 0;
  u->pfx_rep = 0;
  u->pfx_seg = 0;
  u->pfx_rex = 0;
  u->pfx_insn= 0;
  u->mnemonic = UD_Inone;
  u->mapen = NULL;

  memset( &u->operand[ 0 ], 0, sizeof( struct ud_operand ) );
  memset( &u->operand[ 1 ], 0, sizeof( struct ud_operand ) );
  memset( &u->operand[ 2 ], 0, sizeof( struct ud_operand ) );
 
  return 0;
}

static int do_prefixes( struct ud* u )
{
  int have_pfx = 1;
  int i;
  uint8_t last_pfx = -1;
  if ( u->error ) return -1; /* if in error state, bail out */

  for ( i = 0; have_pfx ; ++i ) {

	uint8_t curr;
	/* Get next byte. */
  	inp_next(u); if ( u->error ) return -1;
	curr = inp_curr( u );
	/* Rex prefixes in 64bit mode */
	if ( u->dis_mode == 64 && ( curr & 0xF0 ) == 0x40 ) {
		u->pfx_rex = curr;	
	} else {
		switch ( curr )  
		{
		/* TBD: Need to find out the behavior in the case of multiple
		 * segment prefixes.
		 */
		case 0x2E : 
			u->pfx_seg = UD_R_CS; 
			break;
		case 0x36 : 	
			u->pfx_seg = UD_R_SS; 
			break;
		case 0x3E : 
			u->pfx_seg = UD_R_DS; 
			break;
		case 0x26 : 
			u->pfx_seg = UD_R_ES; 
			break;
		case 0x64 : 
			u->pfx_seg = UD_R_FS; 
			break;
		case 0x65 : 
			u->pfx_seg = UD_R_GS; 
			break;
		case 0x67 : /* adress-size override prefix */ 
			u->pfx_adr = 0x67;
			break;
		case 0xF0 : 
			u->pfx_lock= 0xF0;
			break;
		case 0x66 : { /* operand-size override, and SSE modifier */
			/* if there was already an F2, F3 prefix, 66 becomes
			 * in effective.
			 */
			if ( u->pfx_insn != 0xF2 &&  u->pfx_insn != 0xF3 ) {
				u->pfx_insn = 0x66;
			} 
			/* operand size prefix */
			u->pfx_opr = 0x66;			 
			break;
			}
		/* 0xF2 is an SSE instruction modifier */
		case 0xF2 : {
			u->pfx_insn = 0xF2;
			u->pfx_repne= 0xF2; 
			break;
			}
		/* 0xF3 is an SSE instruction modifier */
		case 0xF3 : {
			u->pfx_insn = 0xF3;
			u->pfx_rep  = 0xF3; 
			break;
			}
		default : {
			/* No more prefixes */
			have_pfx = 0;
			}
		}
	}

	/* check if we reached max instruction length */
	if ( i == 14 ) {
		u->error = 1;
		break;
	}
	/* we keep the last prefix for checking 0x66 insn modifier. */	
	last_pfx = curr;
  }

  /* return status */
  if ( u->error ) return -1; 

  /* rewind back one byte in stream, since the above loop stopped 
   * with a non-prefix byte. 
   */
  inp_back(u);

  return 0;
}

static int do_mode( struct ud* u )
{
  /* if in error state, bail out */
  if ( u->error ) return -1; 

  /* propagate perfix effects */
  if ( u->dis_mode == 64 ) {  /* set 64bit-mode flags */

	/* Check validity of  instruction m64 */
	if ( P_INV64( u->mapen->prefix ) ) {
		u->error = 1;
		return -1;
	}

	/* effective rex prefix is the  effective mask for the 
	 * instruction hard-coded in the opcode map.
	 */
	u->pfx_rex = u->pfx_rex & P_REX_MASK( u->mapen->prefix ); 

	/* whether this instruction has a default operand size of 
	 * 64bit, also hardcoded into the opcode map.
	 */
	u->default64 = P_DEF64( u->mapen->prefix ); 
	/* calculate effective operand size */
	if ( P_REX( u->mapen->prefix ) && P_REX_W( u->pfx_rex ) ) {
		u->opr_mode = 64;
	} else if ( u->pfx_opr ) {
		u->opr_mode = 16;
	} else {
		/* unless the default opr size of instruction is 64,
		 * the effective operand size in the absence of rex.w
		 * prefix is 32.
		 */
		u->opr_mode = ( u->default64 ) ? 64 : 32;
	}

	/* calculate effective address size */
	u->adr_mode = (u->pfx_adr) ? 32 : 64;
  } else if ( u->dis_mode == 32 ) { /* set 32bit-mode flags */
	u->opr_mode = ( u->pfx_opr ) ? 16 : 32;
	u->adr_mode = ( u->pfx_adr ) ? 16 : 32;
  } else if ( u->dis_mode == 16 ) { /* set 16bit-mode flags */
	u->opr_mode = ( u->pfx_opr ) ? 32 : 16;
	u->adr_mode = ( u->pfx_adr ) ? 32 : 16;
  }

  /* These flags determine which operand to apply the operand size
   * cast to.
   */
  u->c1 = ( P_C1( u->mapen->prefix ) ) ? 1 : 0;
  u->c2 = ( P_C2( u->mapen->prefix ) ) ? 1 : 0;
  u->c3 = ( P_C3( u->mapen->prefix ) ) ? 1 : 0;

  return 0;
}

static int gen_hex( struct ud *u )
{
  unsigned int i;
  unsigned char *src_ptr = inp_sess( u );
  char* src_hex;

  /* bail out if in error stat. */
  if ( u->error ) return -1; 
  /* output buffer pointe */
  src_hex = ( char* ) u->insn_hexcode;
  /* for each byte used to decode instruction */
  for ( i = 0; i < u->inp_ctr; ++i, ++src_ptr) {
	sprintf( src_hex, "%02x", *src_ptr & 0xFF );
	src_hex += 2;
  }
  return 0;
}

/* =============================================================================
 * ud_decode() - Instruction decoder. Returns the number of bytes decoded.
 * =============================================================================
 */
unsigned int ud_decode( struct ud* u )
{
  inp_start(u);

  if ( clear_insn( u ) ) {
	; /* error */
  } else if ( do_prefixes( u ) != 0 ) {
	; /* error */
  } else if ( ud_search_map( u ) != 0 ) {
	; /* error */
  } else if ( do_mode( u ) != 0 ) {
	; /* error */
  } else if ( disasm_operands( u ) != 0 ) {
	; /* error */
  } else if ( resolve_mnemonic( u ) != 0 ) {
	; /* error */
  }

  /* Handle decode error. */
  if ( u->error ) {
	/* clear out the decode data. */
	clear_insn( u );
	/* mark the sequence of bytes as invalid. */
	u->mapen = ud_me_invalid();
	u->mnemonic = u->mapen->mnemonic;
  } 

  u->insn_offset = u->pc; /* set offset of instruction */
  u->insn_fill = 0;	  /* set translation buffer index to 0 */
  u->pc += u->inp_ctr;	  /* move program counter by bytes decoded */
  gen_hex( u );		  /* generate hex code */

  /* return number of bytes disassembled. */
  return u->inp_ctr;
}
