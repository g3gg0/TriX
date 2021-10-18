#include "emu.h"

#define Rd_t_g		get_ARM_reg(OPCODE_T&0x7)
#define Rd_t_s(x)	set_ARM_reg(OPCODE_T&0x7,x)

#define Rs_t_g		get_ARM_reg((OPCODE_T>>3)&0x7)
#define Rs_t_s(x)	set_ARM_reg((OPCODE_T>>3)&0x7,x)

#define Rn_t_g		get_ARM_reg((OPCODE_T>>6)&0x7)
#define Rn_t_s(x)	set_ARM_reg((OPCODE_T>>6)&0x7,x)

#define Rd_imm_t_g	get_ARM_reg((OPCODE_T>>8)&0x7)
#define Rd_imm_t_s(x)	set_ARM_reg((OPCODE_T>>8)&0x7,x)

#define Hd_t_g		get_ARM_reg((OPCODE_T&0x7)+8)
#define Hd_t_s(x)	set_ARM_reg((OPCODE_T&0x7)+8,x)

#define Hs_t_g		get_ARM_reg(((OPCODE_T>>3)&0x7)+8)
#define Hs_t_s(x)	set_ARM_reg(((OPCODE_T>>3)&0x7)+8,x)

#define Rd_nr		(OPCODE_T & 7)
#define Rs_nr		((OPCODE_T >> 3) & 7)



int tins_unknown (void)
{
	INSTRUCTION_TRACE();

	abort_situation = ARMULATE_ABORT_MEMORY_EX;
	return ARMULATE_INV_INSTR;
}

int tins_ror ( void )
{
	INSTRUCTION_TRACE();

	Hs_t_s ( Hs_t_g << Hd_t_g );

	instr_advance();
	return 1;
}

int tins_mov_lo_hi (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s (Hs_t_g);

	instr_advance();
	return 1;
}

int tins_mov_hi_lo (void)
{
	INSTRUCTION_TRACE();

	Hd_t_s (Rs_t_g);

	instr_advance ();
	return 1;
}

int tins_mov_hi_hi (void)
{
	INSTRUCTION_TRACE();

	Hd_t_s (Hs_t_g);

	instr_advance ();
	return 1;
}

int tins_add_lo_hi (void)
{
	u32 rm, rd, sum;

	INSTRUCTION_TRACE();

	rm = (OPCODE_T >> 3) & 7;
	rd = OPCODE_T & 7;

	if (OPCODE_T & 0x80)
		rd += 8;
	if (OPCODE_T & 0x40)
		rm += 8;

	sum = get_ARM_reg(rm) + get_ARM_reg(rd);
	SET_ADD_FLAGS (get_ARM_reg(rm), get_ARM_reg(rd), sum);
	set_ARM_reg(rd, sum);


	instr_advance ();
	return 1;
}

int tins_cmp_lo_hi (void)
{
	u32 rm, rn, cmp;
	INSTRUCTION_TRACE();

	rm = (OPCODE_T >> 3) & 7;
	rn = OPCODE_T & 7;
	if (OPCODE_T & 0x80)
		rn += 8;
	if (OPCODE_T & 0x40)
		rm += 8;

	cmp = get_ARM_reg(rn) - get_ARM_reg(rm);
	SET_SUB_FLAGS (get_ARM_reg(rn), get_ARM_reg(rm), cmp);

	instr_advance();
	return 1;
}

int tins_add_sp (void)
{
	INSTRUCTION_TRACE();

	if (OPCODE_T & 0x80)
		set_ARM_reg(13, get_ARM_reg(13) - ((OPCODE_T&0x7F)<<2));
	else
		set_ARM_reg(13, get_ARM_reg(13) + ((OPCODE_T&0x7F)<<2));

	instr_advance();
	return 1;
}

int tins_mov_imm (void)
{
	INSTRUCTION_TRACE();

	Rd_imm_t_s (OPCODE_T&0xFF);
	SET_DP_LOG_FLAGS(Rd_imm_t_g);

	instr_advance();
	return 1;
}

int tins_sp_rel_str (void)
{
	INSTRUCTION_TRACE();

	write_word (get_ARM_reg(13) + ((OPCODE_T&0xFF)<<2), Rd_imm_t_g, arm );

	instr_advance();
	return 1;
}

int tins_sp_rel_ldr (void)
{
	INSTRUCTION_TRACE();

	Rd_imm_t_s (read_word (get_ARM_reg(13) + ((OPCODE_T&0xFF)<<2), arm));

	instr_advance();
	return 1;
}

int tins_pc_rel_ldr (void)
{
	register addr = (get_ARM_reg(15)&~0x2) + ((OPCODE_T&0xFF)<<2);
	INSTRUCTION_TRACE();


	Rd_imm_t_s (read_word(addr, arm));
	
	instr_advance();
	return 1;
}

int tins_pc_add (void)
{
	INSTRUCTION_TRACE();

	Rd_imm_t_s ((get_ARM_reg(15) & ~2) + ((OPCODE_T&0xFF)<<2));

	instr_advance();
	return 1;
}

int tins_sp_add (void)
{
	INSTRUCTION_TRACE();

	Rd_imm_t_s (get_ARM_reg(13) + ((OPCODE_T&0xFF)<<2));

	instr_advance();
	return 1;
}

int tins_str_imm (void)
{
	INSTRUCTION_TRACE();

	write_word (Rs_t_g + (((OPCODE_T>>6)&0x1F)<<2), Rd_t_g, arm);

	instr_advance();
	return 1;
}

int tins_strh_imm (void)
{
	INSTRUCTION_TRACE();

	write_hword (Rs_t_g + (((OPCODE_T>>6)&0x1F)<<1), (u16)Rd_t_g, arm);

	instr_advance();
	return 1;
}

int tins_strb_imm (void)
{	
	INSTRUCTION_TRACE();

	write_byte (Rs_t_g + ((OPCODE_T>>6)&0x1F), (u8)Rd_t_g, arm);

	instr_advance();
	return 1;
}

int tins_ldr_imm (void)
{	
	INSTRUCTION_TRACE();

	Rd_t_s (read_word(Rs_t_g + (((OPCODE_T>>6)&0x1F)<<2), arm));

	instr_advance();
	return 1;
}

int tins_ldrh_imm (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s (read_hword (Rs_t_g + (((OPCODE_T>>6)&0x1F)<<1), arm));

	instr_advance();
	return 1;
}

int tins_ldrb_imm (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s (read_byte(Rs_t_g + ((OPCODE_T>>6)&0x1F), arm));

	instr_advance();
	return 1;
}

int tins_ldmia(void)
{
	int i, reglist = OPCODE_T & 0xFF;
	int addr = Rd_imm_t_g;
	unsigned int temp = 0;
	int update_base = 0;

	INSTRUCTION_TRACE();

	for (i=0; i<8; i++)
	{
		if (reglist & (1 << i))
		{
			if ( i != ((OPCODE_T>>8)&0x7) )
            {
				set_ARM_reg(i, read_word(Rd_imm_t_g, arm));
            }
			else
			{
				temp = read_word(Rd_imm_t_g, arm);
				update_base = 1;
			}
			Rd_imm_t_s (Rd_imm_t_g + 4);
		}
	}

	if ( update_base )
		Rd_imm_t_s (temp);


	instr_advance();
	return 1;
}

int tins_stmia(void)
{
	int i, reglist = OPCODE_T & 0xFF;
	int addr = Rd_imm_t_g;
	INSTRUCTION_TRACE();

	for (i=0; i<8; i++)
	{
		if (reglist & (1 << i))
		{
			write_word(Rd_imm_t_g, get_ARM_reg(i), arm);
			Rd_imm_t_s (Rd_imm_t_g + 4);
		}
	}

	instr_advance();
	return 1;
}

/*********************************************/

int tins_lsl (void) 
{
	int depl = Rs_t_g;
	INSTRUCTION_TRACE();

	if (depl != 0)
	{
		CFLAG_SET(!!(Rd_t_g & (1 << (depl-1))));
		Rd_t_s (Rd_t_g << depl);
		SET_DP_LOG_FLAGS(Rd_t_g);
	}

	instr_advance();
	return 1;
}

int tins_lsl_imm (void) 
{
	INSTRUCTION_TRACE();

	Rd_t_s (Rs_t_g << ((OPCODE_T>>6)&0x1F));
	SET_DP_LOG_FLAGS(Rd_t_g);

	instr_advance();
	return 1;
}

int tins_lsr(void)
{
	int depl = Rs_t_g;
	INSTRUCTION_TRACE();

	if (depl != 0)
	{
		CFLAG_SET(!!(Rd_t_g & (1 << (32-depl))));
		Rd_t_s (Rd_t_g >> depl);
		SET_DP_LOG_FLAGS(Rd_t_g);
	}

	instr_advance();
	return 1;
}

int tins_lsr_imm (void) 
{
	int depl = (OPCODE_T>>6)&0x1F;

	INSTRUCTION_TRACE();

	if (depl == 0)
		depl = 32;
	CFLAG_SET(!!(Rs_t_g & (1 << (depl-1))));
	Rd_t_s (Rs_t_g >> depl);

	SET_DP_LOG_FLAGS(Rd_t_g);

	instr_advance();
	return 1;
}

int tins_asr(void)
{
	u32 shift_amount = Rs_t_g;

	INSTRUCTION_TRACE();

	CFLAG_SET(!!(Rs_t_g & (1 << (shift_amount-1))));
	if (Rd_t_g&0x80000000)
		Rd_t_s (((0xFFFFFFFF<<(32-shift_amount))|(Rd_t_g>>shift_amount))); 
	else
		Rd_t_s ((Rd_t_g >> shift_amount));

	SET_DP_LOG_FLAGS(Rd_t_g);

	instr_advance();
	return 1;
}

int tins_asr_imm (void)
{
	u32 shift_amount = (OPCODE_T>>6)&0x1F;

	INSTRUCTION_TRACE();

	if (Rs_t_g&0x80000000)
		Rd_t_s (((0xFFFFFFFF<<(32-shift_amount))|(OP_REG_get()>>shift_amount))); 
	else
		Rd_t_s ((Rs_t_g >> shift_amount));

	SET_DP_LOG_FLAGS(Rd_t_g);

	instr_advance();
	return 1;
}

int tins_add_3 (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s (Rs_t_g + Rn_t_g);
	SET_ADD_FLAGS (Rs_t_g, Rn_t_g, Rd_t_g);

	instr_advance();
	return 1;
}

int tins_sub_3 (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s (Rs_t_g - Rn_t_g);
	SET_SUB_FLAGS (Rs_t_g, Rn_t_g, Rd_t_g);

	instr_advance();
	return 1;
}

int tins_add_imm (void)
{
	u32 temp1 = Rd_imm_t_g;
	u32 temp2 = (OPCODE_T&0xFF);
	INSTRUCTION_TRACE();

	Rd_imm_t_s (temp1 + temp2);
	SET_ADD_FLAGS (temp1, temp2, Rd_imm_t_g);

	instr_advance();
	return 1;
}

int tins_cmp_imm (void)
{
	u32 temp2 = (OPCODE_T&0xFF);
	u32 temp  = Rd_imm_t_g - temp2;
	INSTRUCTION_TRACE();

	SET_SUB_FLAGS (Rd_imm_t_g, temp2, temp);

	instr_advance();
	return 1;
}

/************************************************/

int tins_str(void)
{
	INSTRUCTION_TRACE();

	write_word(Rs_t_g + Rn_t_g, Rd_t_g, arm);

	instr_advance();
	return 1;
}

int tins_ldr(void)
{
	INSTRUCTION_TRACE();

	Rd_t_s (read_word(Rs_t_g + Rn_t_g, arm));

	instr_advance();
	return 1;
}

int tins_strh(void)
{
	INSTRUCTION_TRACE();

	write_hword(Rs_t_g + Rn_t_g, (u16)Rd_t_g, arm);

	instr_advance();
	return 1;
}

int tins_ldrh(void)
{
	INSTRUCTION_TRACE();

	Rd_t_s (read_hword(Rs_t_g + Rn_t_g, arm));

	instr_advance();
	return 1;
}

int tins_strb (void)
{
	INSTRUCTION_TRACE();

	write_byte(Rs_t_g + Rn_t_g, (u8)Rd_t_g, arm);

	instr_advance();
	return 1;
}

int tins_ldrb (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s (read_byte(Rs_t_g + Rn_t_g, arm));

	instr_advance();
	return 1;
}

int tins_ldrsb (void)
{
	u8 b = read_byte(Rs_t_g + Rn_t_g, arm);
	INSTRUCTION_TRACE();


	if (b & 0x80)
		Rd_t_s (0xFFFFFF00 | b);
	else
		Rd_t_s (b);

	instr_advance();
	return 1;
}

int tins_ldrsh (void)
{
	u16 hw = read_hword(Rs_t_g + Rn_t_g, arm);

	INSTRUCTION_TRACE();

	if (hw & 0x8000)
		Rd_t_s (0xFFFF0000 | hw);
	else
		Rd_t_s (hw);

	instr_advance();
	return 1;
}

/************************************************/

int tins_and (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s(Rd_t_g & Rs_t_g);
	SET_DP_LOG_FLAGS (Rd_t_g);

	instr_advance();
	return 1;
}

int tins_tst (void)
{
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (Rd_t_g & Rs_t_g);

	instr_advance();
	return 1;
}

int tins_eor (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s(Rd_t_g ^ Rs_t_g);
	SET_DP_LOG_FLAGS (Rd_t_g);

	instr_advance();
	return 1;
}

int tins_sbc (void)
{
/*
	u32 val = Rd_t_g - Rs_t_g - !CFLAG;

	SET_SUB_FLAGS (val, Rs_t_g+!CFLAG, Rd_t_g);
	Rd_t_s (val);
*/
	u32 val = Rd_t_g;
	INSTRUCTION_TRACE();


	Rd_t_s ( Rd_t_g - Rs_t_g - !CFLAG );
	SET_SUB_FLAGS (val, Rs_t_g+!CFLAG, Rd_t_g);

	instr_advance();
	return 1;
}

int tins_adc (void)
{
	u32 val = Rd_t_g;
	INSTRUCTION_TRACE();


	Rd_t_s(Rd_t_g + Rs_t_g + CFLAG);
	SET_ADD_FLAGS (val, Rs_t_g+CFLAG, Rd_t_g);

	instr_advance();
	return 1;
}

int tins_cmp (void)
{
	u32 temp  = Rd_t_g - Rs_t_g;
	INSTRUCTION_TRACE();


	SET_SUB_FLAGS (Rd_t_g, Rs_t_g, temp);

	instr_advance();
	return 1;
}

int tins_cmn (void)
{
	u32 temp  = Rd_t_g + Rs_t_g;

	INSTRUCTION_TRACE();

	SET_ADD_FLAGS (Rd_t_g, Rs_t_g, temp);

	instr_advance();
	return 1;
}

int tins_neg (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s  (0 - Rs_t_g);

	SET_SUB_FLAGS (0, Rs_t_g, Rd_t_g);

	instr_advance();
	return 1;
}

int tins_orr (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s  (Rd_t_g | Rs_t_g);

	SET_DP_LOG_FLAGS (Rd_t_g);

	instr_advance();
	return 1;
}

int tins_bic (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s  (Rd_t_g & ~Rs_t_g);

	SET_DP_LOG_FLAGS (Rd_t_g);

	instr_advance();
	return 1;
}

int tins_mvn (void)
{
	INSTRUCTION_TRACE();

	Rd_t_s (~Rs_t_g);

	SET_DP_LOG_FLAGS (Rd_t_g);

	instr_advance();
	return 1;

}

int tins_mul(void)
{
	INSTRUCTION_TRACE();

	Rd_t_s  (Rd_t_g * Rs_t_g);

	SET_DP_LOG_FLAGS (Rd_t_g);

	instr_advance();
	return 1;
}

int tins_add_short_imm (void)
{
	u32 temp = Rs_t_g;
	u32 op = ((OPCODE_T>>6)&0x7);

	INSTRUCTION_TRACE();

	Rd_t_s (Rs_t_g + op);
	SET_ADD_FLAGS (temp, op, Rd_t_g);

	instr_advance();
	return 1;
}

int tins_sub_short_imm (void)
{
	u32 temp = Rs_t_g;
	u32 op = ((OPCODE_T>>6)&0x7);
	INSTRUCTION_TRACE();


	Rd_t_s (Rs_t_g - op);
	SET_SUB_FLAGS (temp, op, Rd_t_g);

	instr_advance();
	return 1;
}

int tins_sub_imm (void)
{
	u32 temp1 = Rd_imm_t_g;
	u32 temp2 = (OPCODE_T&0xFF);
	INSTRUCTION_TRACE();


	Rd_imm_t_s (temp1 - temp2);
	SET_SUB_FLAGS (temp1, temp2, Rd_imm_t_g);

	instr_advance();
	return 1;
}

int tins_push (void)
{
	int i;

	INSTRUCTION_TRACE();

	if (OPCODE_T & 0x100) 
	{
		set_ARM_reg ( 13, get_ARM_reg(13) - 4 );
		write_word ( get_ARM_reg(13), get_ARM_reg(14), arm );
	}
			
	for (i=7; i>=0; i--) 
	{	
		if (OPCODE_T & (1<<i)) 
		{
			set_ARM_reg ( 13, get_ARM_reg(13) - 4 );
			write_word ( get_ARM_reg(13), get_ARM_reg(i), arm );
		}
	}

	instr_advance();
	return 1;
}

int tins_pop (void)
{
	int i, new_pc;
	INSTRUCTION_TRACE();


	for (i=0; i<8; i++) 
	{	
		if (OPCODE_T & (1<<i)) 
		{
			set_ARM_reg(i, read_word (get_ARM_reg(13), arm));
			set_ARM_reg(13, get_ARM_reg(13) + 4);
		}
	}

	if (OPCODE_T & 0x100) 
	{
		new_pc = read_word (get_ARM_reg(13), arm);

		if ( !(new_pc & 1)  )
			set_ARM_mode ( ARM_MODE );
		set_ARM_reg(15, new_pc & ~1 );

		set_ARM_reg(13, get_ARM_reg(13) + 4);
	}
			
	instr_advance();
	return 1;
}


int tins_bcond (void)
{
	int cond = 0;
	INSTRUCTION_TRACE();

	switch ((OPCODE_T >> 8) & 15)
	{
		case 0:
			/* EQ */
			cond = Z_FLAG_SET;
			break;
		case 1:
			/* NE */
			cond = Z_FLAG_CLEAR;
			break;
		case 2:
			/* CS/HS */
			cond = C_FLAG_SET;
			break;
		case 3:
			/* CC/LO */
			cond = C_FLAG_CLEAR;
			break;
		case 4:
			/* MI */
			cond = N_FLAG_SET;
			break;
		case 5:
			/* PL */
			cond = N_FLAG_CLEAR;
			break;
		case 6:
			/* VS */
			cond = V_FLAG_SET;
			break;
		case 7:
			/* VC */
			cond = V_FLAG_CLEAR;
			break;
		case 8:
			/* HI */
			cond = C_FLAG_SET && Z_FLAG_CLEAR;
			break;
		case 9:
			/* LS */
			cond = C_FLAG_CLEAR || Z_FLAG_SET;
			break;
		case 10:
			/* GE */
			cond = N_EQU_V_FLAG;
			break;
		case 11:
			/* LT */
			cond = N_NEQ_V_FLAG;
			break;
		case 12:
			/* GT */
			cond = Z_FLAG_CLEAR && N_EQU_V_FLAG;
			break;
		case 13:
			/* LE */
			cond = Z_FLAG_SET || N_NEQ_V_FLAG;
			break;
		case 14:
			/* AL */
			cond = 1;
			break;
		case 15:
			/* (NV) Shouldn't be here */
			cond = 0;
			abort_situation = ARMULATE_ABORT_UNKNOWN_INSTR;
			return ARMULATE_INV_INSTR;
			break;
	}

	if (cond) 
	{	
		if (OPCODE_T & 0x80)
			set_ARM_reg(15, get_ARM_reg(15) + (((OPCODE_T&0xFF)<<1)-0x200) );
		else
			set_ARM_reg(15, get_ARM_reg(15) + ((OPCODE_T&0x7F)<<1) );
	}

	instr_advance();
	return 1;
}

int tins_bl (void)
{
	int H_field = (OPCODE_T >> 11) & 0x03;
	u32 next_instr = (get_ARM_reg(15) - 2);

	INSTRUCTION_TRACE();

	if ( H_field == 0x02 ) 
	{
		if (OPCODE_T & 0x400)
			set_ARM_reg(14, get_ARM_reg(15) + (((OPCODE_T&0x7FF)-0x800)<<12) );
		else
			set_ARM_reg(14, get_ARM_reg(15) + ((OPCODE_T&0x7FF)<<12) );
	}
	else if ( H_field == 0x03 ) 
	{
		set_ARM_reg(15, get_ARM_reg(14) + ((OPCODE_T&0x7FF)<<1) );
		set_ARM_reg(14, next_instr | 1);
	}
	else if ( H_field == 0x01 ) 
	{
		set_ARM_reg(15, (get_ARM_reg(14) + ((OPCODE_T&0x7FF)<<1)) & ~0x03 );
		set_ARM_reg(14, next_instr | 1);
	}
	else 
	{
		abort_situation = ARMULATE_ABORT_UNKNOWN_INSTR;
		return ARMULATE_INV_INSTR;
	}

	instr_advance ();
	return 1;
}

int tins_bx (void)
{
	u32 temp, reg;
	INSTRUCTION_TRACE();


	reg = (OPCODE_T >> 3) & 0xf; 
	temp = get_ARM_reg ( reg );

	// BLX variant
	if ( OPCODE_T & 0x0080 )
		set_ARM_reg(14, get_ARM_reg(15) | 1);

	if (temp & 1)
		set_ARM_reg(15, temp & ~1);
	else
	{
		set_ARM_mode ( ARM_MODE );
		set_ARM_reg(15, temp & ~3);
	}

	instr_advance ();
	return 1;
}

int tins_b (void)
{
	INSTRUCTION_TRACE();

	if (OPCODE_T & 0x0400)
		set_ARM_reg(15, get_ARM_reg(15) + (((OPCODE_T&0x7FF)-0x800)<<1) );
	else
		set_ARM_reg(15, get_ARM_reg(15) + ((OPCODE_T&0x7FF)<<1) );

	instr_advance ();
	return 1;
}

int tins_swi (void)
{
	INSTRUCTION_TRACE();

	swi ();

	return 1;
}

