#define DEST_REG		((OPCODE>>12)&0xF)
#define DEST_REG_get	get_ARM_reg((OPCODE>>12)&0xF)
#define DEST_REG_set(x)	set_ARM_reg((OPCODE>>12)&0xF, x)

#define BASE_REG_get	get_ARM_reg((OPCODE>>16)&0xF)
#define BASE_REG_set(x)	set_ARM_reg((OPCODE>>16)&0xF, x)

#define OP_REG_get		get_ARM_reg(OPCODE&0xF)
#define OP_REG_set(x)	set_ARM_reg(OPCODE&0xF, x)

#define SHFT_AMO_REG_get	get_ARM_reg((OPCODE>>8)&0xF)
#define SHFT_AMO_REG_set(x)	set_ARM_reg((OPCODE>>8)&0xF, x)

#define HDT_CALC_IMM_OFFSET	((OPCODE&0xF00)>>4)|(OPCODE&0xF)
#define S_BIT 0x00100000

#ifndef INT64
#define INT64 long long
#endif

#ifndef UINT64
#define UINT64 long long
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef boolean
#define boolean unsigned char
#endif


int doc_set_token ( void *doc, unsigned long address, const char *type, unsigned long value )
{
	return 0;
}

void ins_add_xref ( unsigned long addr, char *type, unsigned long src )
{
	doc_set_token ( pDoc, addr, type, src );
	doc_set_token ( pDoc, src, "XREF", addr );
}

__inline u32 DP_IMM_OPERAND (void)
{		
	u32 value = OPCODE;

#ifdef WIN32
	__asm {
		xor ebx, ebx
		mov ecx, value
		mov bl, cl
		and ecx, 0xF00
		shr ecx, 7		
		ror ebx, cl
		mov value, ebx
	}
#else
    printf ( "TODO: Not implemented yet\n" );
#endif
		
	return value;
}

//-------------Barrel-Shifter ("evil"(tm))----------------------------------------------

#define IMM_SHIFT	((OPCODE>>7)&0x1F)
#define REG_SHIFT	(SHFT_AMO_REG_get&0xFF)

__inline DP_REG_OPERAND (u32 shift)
{
	u32 shift_amount = shift;
	u32 op = OP_REG_get;
	switch ((OPCODE>>5)&0x3)	
	{							
		case 0: return (op << shift_amount);	
		case 1: return (op >> shift_amount);
		case 2: 
			if (shift_amount) {
				if (op&0x80000000)
					return ((0xFFFFFFFF<<(32-shift_amount))|(op>>shift_amount)); 
				else
					return (op >> shift_amount);
			}
			else {
				if (op&0x80000000) return 0xFFFFFFFF; else return 0;
			}
		case 3: 
			if (shift_amount) {
				return (op << (32-shift_amount)|(op>>shift_amount));
			}
			else
				return ((op>>1)|(CFLAG<<31));
	}
	return 0;
}

__inline DP_REG_OPERAND_C (u32 shift)
{
	u32 op = OP_REG_get;

	switch ((OPCODE>>5)&0x3)	
	{							
		case 0:
			CFLAG = (op&(0x80000000>>(shift-1))) ? (1):(0);
			return (op << shift);
		case 1: 
			if (shift) {
				CFLAG = (op&(1<<(shift-1))) ? (1):(0);
				return (op >> shift);
			} 
			else {
				CFLAG = op>>31; return 0;
			}
		case 2: 
			if (shift) {
				CFLAG = (op&(1<<(shift-1))) ? (1):(0);
				if (op&0x80000000)
					return ((0xFFFFFFFF<<(32-shift))|(op>>shift)); 
				else
					return (op >> shift);
			} else {
				CFLAG = op>>31;
				if (CFLAG) return 0xFFFFFFFF; else return 0;
			}
		case 3: 
			if (shift) {
				CFLAG = (op&(1<<(shift-1))) ? (1):(0);
				return (op << (32-shift)|(op>>shift));
			}
			else {
				if (CFLAG) {
					CFLAG = op&0x1;
					return ((op>>1)|0x80000000);
				}
				else {
					CFLAG = op&0x1;	
					return (op>>1);
				}
			}
	}
	return 0;
}

//--------------------------------------------------------------------------------------

//---------------------Flag macros------------------------------------------------------

#define TOPBIT 0x80000000

__inline void SET_FLAGS_FROM_CPSR ()
{
	if (CPSR & Z_BIT)
		ZFLAG = 1;
	else
		ZFLAG = 0;

	if (CPSR & C_BIT)
		CFLAG = 1;
	else
		CFLAG = 0;

	if (CPSR & N_BIT)
		NFLAG = 1;
	else
		NFLAG = 0;

	if (CPSR & V_BIT)
		VFLAG = 1;
	else
		VFLAG = 0;
}

__inline void SET_CPSR()
{
	if (ZFLAG)
		CPSR |= Z_BIT;
	else
		CPSR &= ~Z_BIT;

	if (CFLAG)
		CPSR |= C_BIT;
	else
		CPSR &= ~C_BIT;

	if (NFLAG)
		CPSR |= N_BIT;
	else
		CPSR &= ~N_BIT;

	if (VFLAG)
		CPSR |= V_BIT;
	else
		CPSR &= ~V_BIT;
}

__inline int OverflowFromAdd (u32 a, u32 b, u32 c )
{
	if ( ((a>>31) == (b>>31)) && ((c>>31) != (a>>31)) )
		return 1;

	return 0;
}

__inline int OverflowFromSub (u32 a, u32 b, u32 c )
{
	if ( ((a>>31) != (b>>31)) && ((c>>31) != (a>>31)) )
		return 1;

	return 0;
}

__inline int BorrowFrom (u32 a, u32 b)
{
	if ( a < b )
		return 1;

	return 0;
}

__inline int CarryFrom (u32 a, u32 b, u32 c)
{
	if ( (c < a) && (c < b) )
		return 1;

	return 0;
}

// Arithmetic Data Processing 
__inline void SET_SUB_FLAGS (u32 a, u32 b, u32 c)
{
	if (c) 
		ZFLAG = 0; 
	else 
		ZFLAG = 1;
	NFLAG = (c >> 31); 
	CFLAG = !BorrowFrom ( a, b );
	VFLAG = OverflowFromSub ( a, b, c );
	
	SET_CPSR();
}

__inline void SET_ADD_FLAGS (u32 a, u32 b, u32 c)
{
	if (c) 
		ZFLAG = 0; 
	else 
		ZFLAG = 1;
	NFLAG = (c >> 31); 
	CFLAG = CarryFrom ( a, b, c );
	VFLAG = OverflowFromAdd (a, b, c );
	
	SET_CPSR();
}


// Logical Data Processing (value is supposed to be 32 bit)
__inline void SET_DP_LOG_FLAGS(u32 value)
{
	if (value) 
		ZFLAG = 0; 
	else 
		ZFLAG = 1;

	NFLAG = (value>>31);

	SET_CPSR();
}
//--------------------------------------------------------------------------------------

/***********************************************************************************************/
/*									Opcodes													   */
/***********************************************************************************************/

int unknown_opcode(void ) 
{
	INSTRUCTION_TRACE();
	abort_situation = ARMULATE_ABORT_UNKNOWN_INSTR;
	return ARMULATE_INV_INSTR;
}


int ins_stc2 (void ) 
{
	INSTRUCTION_TRACE();
	instr_advance(); 
	return ARMULATE_EXECUTED;
}


int ins_ldc2 (void ) 
{
	INSTRUCTION_TRACE();
	instr_advance(); 
	return ARMULATE_EXECUTED;
}


int ins_clz (void ) 
{
	unsigned int value = OP_REG_get;
	int zeroes = 0;

	INSTRUCTION_TRACE();
	for ( zeroes=0; zeroes<0x20; zeroes++ )
	{
		if ( value & 0x80000000 )
			break;
		value <<= 1;
	}

	DEST_REG_set ( zeroes );

	instr_advance(); 
	return ARMULATE_EXECUTED;
}


int ins_mrc (void ) 
{
	INSTRUCTION_TRACE();
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_mcr (void ) 
{
	INSTRUCTION_TRACE();
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_cdp (void ) 
{
	INSTRUCTION_TRACE();
	instr_advance(); 
	return ARMULATE_EXECUTED;
}


int ins_bpl (void)
{
	INSTRUCTION_TRACE();
	if ( ((OPCODE>>28) & 0xF) == 0xF )
	{
		set_ARM_reg ( 14,  get_ARM_reg (15) - 4 );
		set_ARM_mode ( THUMB_MODE );
		set_ARM_reg( 15, get_ARM_reg (15) + ((OPCODE&0x7FFFFF)<<2) );
	}
	else
	{
		set_ARM_reg( 15, get_ARM_reg (15) + ((OPCODE&0x7FFFFF)<<2) );
	}

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_bmi (void)
{
	INSTRUCTION_TRACE();
	if ( ((OPCODE>>28) & 0xF) == 0xF )
	{
		set_ARM_reg ( 14,  get_ARM_reg (15) - 4 );
		set_ARM_mode ( THUMB_MODE );
		set_ARM_reg( 15, get_ARM_reg (15) + (((OPCODE&0xFFFFFF)<<2)-0x4000000) );
	}
	else
	{
		set_ARM_reg( 15, get_ARM_reg (15) + (((OPCODE&0xFFFFFF)<<2)-0x4000000) );
	}

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_blpl (void)
{
	INSTRUCTION_TRACE();
	if ( ((OPCODE>>28) & 0xF) == 0xF )
	{
		set_ARM_reg ( 14,  get_ARM_reg (15) - 4 );
		set_ARM_mode ( THUMB_MODE );
		set_ARM_reg( 15, get_ARM_reg (15) + ((OPCODE&0xFFFFFF)<<2) + 2 );
	}
	else
	{
		set_ARM_reg ( 14,  get_ARM_reg (15) - 4 );
		set_ARM_reg( 15, get_ARM_reg (15) + ((OPCODE&0xFFFFFF)<<2) );
	}

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_blmi (void)
{
	INSTRUCTION_TRACE();
	if ( ((OPCODE>>28) & 0xF) == 0xF )
	{
		set_ARM_reg ( 14,  get_ARM_reg (15) - 4 );
		set_ARM_mode ( THUMB_MODE );
		set_ARM_reg( 15, get_ARM_reg (15) + (((OPCODE&0xFFFFFF)<<2)-0x4000000) + 2 );
	}
	else
	{
		set_ARM_reg ( 14,  get_ARM_reg (15) - 4 );
		set_ARM_reg( 15, get_ARM_reg (15) + (((OPCODE&0xFFFFFF)<<2)-0x4000000) );
	}

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_bx (void)
{
	INSTRUCTION_TRACE();
	if ( OP_REG_get & 1 )
		set_ARM_mode ( THUMB_MODE );

	set_ARM_reg( 15, OP_REG_get & ~1 );

	instr_advance ();
	return ARMULATE_EXECUTED;
}


int ins_blx (void)
{
	INSTRUCTION_TRACE();
	set_ARM_reg ( 14,  get_ARM_reg (15) - 4 );

	if ( OP_REG_get & 1 )
		set_ARM_mode ( THUMB_MODE );

	set_ARM_reg( 15, OP_REG_get & ~1 );

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_mul(void)
{
	INSTRUCTION_TRACE();
	BASE_REG_set(OP_REG_get * SHFT_AMO_REG_get);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_muls(void)
{
	INSTRUCTION_TRACE();
	BASE_REG_set(OP_REG_get * SHFT_AMO_REG_get);
	SET_DP_LOG_FLAGS(BASE_REG_get);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_mla(void)
{
	INSTRUCTION_TRACE();
	BASE_REG_set ((OP_REG_get * SHFT_AMO_REG_get) + DEST_REG_get);
	
	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_mlas(void)
{
	INSTRUCTION_TRACE();
	BASE_REG_set ((OP_REG_get * SHFT_AMO_REG_get) + DEST_REG_get);
	SET_DP_LOG_FLAGS(BASE_REG_get);
	
	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_mull(void)
{
	INT64 temp64 = (INT64)OP_REG_get;
	INSTRUCTION_TRACE();

	temp64 *= ((s32)SHFT_AMO_REG_get);

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32) & 0xFFFFFFFF)); 

	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_mulls(void)
{
	INT64 temp64 = (INT64)OP_REG_get;

	INSTRUCTION_TRACE();
	temp64 *= ((s32)SHFT_AMO_REG_get);

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32) & 0xFFFFFFFF)); 

	if (!temp64)					ZFLAG = 1; else ZFLAG = 0;
	if (temp64&0x8000000000000000)	NFLAG = 1; else NFLAG = 0;

	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_mull_unsigned(void)
{
	UINT64 temp64 = (UINT64)OP_REG_get;
	INSTRUCTION_TRACE();

	temp64 *= SHFT_AMO_REG_get;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32) & 0xFFFFFFFF)); 

	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_mulls_unsigned(void)
{
	UINT64 temp64 = (UINT64)OP_REG_get;
	INSTRUCTION_TRACE();

	temp64 *= SHFT_AMO_REG_get;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF)); 

	if (!temp64)					ZFLAG = 1; else ZFLAG = 0;
	if (temp64&0x8000000000000000)	NFLAG = 1; else NFLAG = 0;

	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_mlal(void)
{
	INT64 temp64 = (INT64)OP_REG_get;
	INT64 operand = (INT64)DEST_REG_get;
	INSTRUCTION_TRACE();

	operand |= ((INT64)BASE_REG_get)<<32;
		
	temp64 = (temp64 * ((INT64)SHFT_AMO_REG_get)) + operand;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF)); 
	
	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_mlals(void)
{
	INT64 temp64 = (INT64)OP_REG_get;
	INT64 operand = (INT64)DEST_REG_get;
	INSTRUCTION_TRACE();
	operand |= ((INT64)BASE_REG_get)<<32;
		
	temp64 = (temp64 * ((INT64)SHFT_AMO_REG_get)) + operand;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF)); 

	if (!temp64)					ZFLAG = 1; else ZFLAG = 0;
	if (temp64&0x8000000000000000)	NFLAG = 1; else NFLAG = 0;

	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_mlal_unsigned(void)
{
	UINT64 temp64 = (UINT64)OP_REG_get;
	UINT64 operand = (UINT64)DEST_REG_get;
	INSTRUCTION_TRACE();
	operand |= ((UINT64)BASE_REG_get)<<32;

	temp64 = (temp64 * ((UINT64)SHFT_AMO_REG_get)) + operand;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF)); 
	
	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_mlals_unsigned(void)
{
	UINT64 temp64 = (UINT64)OP_REG_get;
	UINT64 operand = (UINT64)DEST_REG_get;
	INSTRUCTION_TRACE();
	operand |= ((UINT64)BASE_REG_get)<<32;

	temp64 = (temp64 * ((UINT64)SHFT_AMO_REG_get)) + operand;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF)); 

	if (!temp64)					ZFLAG = 1; else ZFLAG = 0;
	if (temp64&0x8000000000000000)	NFLAG = 1; else NFLAG = 0;

	instr_advance();	
	return ARMULATE_EXECUTED;
}

/*-------------------------------------------------------------
				Data Processing
--------------------------------------------------------------*/

// AND

int ins_and(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & DP_REG_OPERAND(IMM_SHIFT));
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_and_reg(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & DP_REG_OPERAND(REG_SHIFT));
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_and_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & DP_IMM_OPERAND());
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_ands(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & DP_REG_OPERAND_C(IMM_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_ands_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & DP_REG_OPERAND_C(REG_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}
int ins_ands_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// EOR

int ins_eor(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get ^ DP_REG_OPERAND(IMM_SHIFT));
	
	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_eor_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get ^ DP_REG_OPERAND(REG_SHIFT));
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_eor_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get ^ DP_IMM_OPERAND());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_eors(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get ^ DP_REG_OPERAND_C(IMM_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_eors_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get ^ DP_REG_OPERAND_C(REG_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_eors_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get ^ DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// SUB

int ins_sub(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get - DP_REG_OPERAND(IMM_SHIFT));
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sub_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get - DP_REG_OPERAND(REG_SHIFT));
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sub_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get - DP_IMM_OPERAND());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int check_S_bit()
{
	INSTRUCTION_TRACE();
	if ( OPCODE & S_BIT )
	{
		// For return from INTs
		if (DEST_REG == 15)
		{
			unsigned char thumb = 0;

			if (arm->cpsr & THUMB_BIT)
				thumb = 1;

			// TODO check the current mode and don't allow for SYSTEM and USER
			arm->cpsr = arm->spsr[arm->cpsr & 0x1F];

			if ( thumb )
				arm->cpsr |= THUMB_BIT;

			return 0;
		}
		else
			return 1;
	}
	else
		return 0;
}

int ins_subs(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();
	DEST_REG_set (op1 - op2);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get);

	check_S_bit();
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_subs_reg (void) 
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(REG_SHIFT);
	INSTRUCTION_TRACE();
	DEST_REG_set (op1 - op2);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get);
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_subs_imm(void)
{
	u32 op1, op2;
	INSTRUCTION_TRACE();
	
	op1 = BASE_REG_get;
	op2 = DP_IMM_OPERAND();

	DEST_REG_set (op1 - op2);	
	SET_SUB_FLAGS(op1, op2, DEST_REG_get);

	check_S_bit();

	instr_advance();	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// RSB

int ins_rsb(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND(IMM_SHIFT) - BASE_REG_get);
	
	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_rsb_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND(REG_SHIFT) - BASE_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsb_imm(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_IMM_OPERAND() - BASE_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsbs(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsbs_reg (void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(REG_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsbs_imm(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// ADD

int ins_add(void)
{
	INSTRUCTION_TRACE();

	DEST_REG_set (BASE_REG_get + DP_REG_OPERAND(IMM_SHIFT));
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_add_reg(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get + DP_REG_OPERAND(REG_SHIFT));
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_add_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get + DP_IMM_OPERAND());
	
	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_adds(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adds_reg (void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(REG_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adds_imm(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// ADC

int ins_adc(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get + DP_REG_OPERAND(IMM_SHIFT) + CFLAG);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_adc_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get + DP_REG_OPERAND(REG_SHIFT) + CFLAG);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_adc_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get + DP_IMM_OPERAND() + CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adcs(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2 + CFLAG);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adcs_reg (void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(REG_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2 + CFLAG);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adcs_imm(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2 + CFLAG);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// SBC

int ins_sbc(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get - DP_REG_OPERAND(IMM_SHIFT) - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbc_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get - DP_REG_OPERAND(REG_SHIFT) - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbc_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get + DP_IMM_OPERAND() - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbcs(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 - op2 - !CFLAG);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbcs_reg (void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(REG_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 - op2 - !CFLAG);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbcs_imm(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 - op2 - !CFLAG);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// RSC

int ins_rsc(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND(IMM_SHIFT) - BASE_REG_get - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsc_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND(REG_SHIFT) - BASE_REG_get - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsc_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_IMM_OPERAND() - BASE_REG_get - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rscs(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1 - !CFLAG);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get);

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rscs_reg (void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(REG_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1 - !CFLAG);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get);

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rscs_imm(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1 - !CFLAG);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get);

	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// TST

int ins_tst(void)
{
	u32 temp = BASE_REG_get & DP_REG_OPERAND_C(IMM_SHIFT);
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_tst_reg (void)
{
	u32 temp = BASE_REG_get & DP_REG_OPERAND_C(REG_SHIFT);
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_tst_imm(void)
{
	u32 temp = BASE_REG_get & DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// TEQ

int ins_teq(void)
{
	u32 temp = BASE_REG_get ^ DP_REG_OPERAND_C(IMM_SHIFT);
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_teq_reg (void)
{
	u32 temp = BASE_REG_get ^ DP_REG_OPERAND_C(REG_SHIFT);
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_teq_imm(void)
{
	u32 temp = BASE_REG_get ^ DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// CMP

int ins_cmp(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	SET_SUB_FLAGS (op1, op2, op1 - op2);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_cmp_reg(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(REG_SHIFT);
	INSTRUCTION_TRACE();

	SET_SUB_FLAGS (op1, op2, op1 - op2);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_cmp_imm(void)	
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	SET_SUB_FLAGS(op1, op2, op1 - op2);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// CMN

int ins_cmn(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	SET_ADD_FLAGS (op1, op2, op1 + op2);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_cmn_reg (void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_REG_OPERAND(REG_SHIFT);
	INSTRUCTION_TRACE();

	SET_ADD_FLAGS (op1, op2, op1 + op2);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_cmn_imm(void)
{
	u32 op1 = BASE_REG_get;
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	SET_ADD_FLAGS(op1, op2, op1 + op2);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// ORR

int ins_orr(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get | DP_REG_OPERAND(IMM_SHIFT));
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_orr_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get | DP_REG_OPERAND(REG_SHIFT));
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_orr_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get | DP_IMM_OPERAND());
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_orrs(void)
{	
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get | DP_REG_OPERAND_C(IMM_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_orrs_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get | DP_REG_OPERAND_C(REG_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_orrs_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get | DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// MOV

int ins_mov(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND(IMM_SHIFT));

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_mov_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND(REG_SHIFT));

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_mov_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_IMM_OPERAND());

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_movs(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND_C(IMM_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);

	instr_advance();	
	return ARMULATE_EXECUTED;	
}

int ins_movs_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND_C(REG_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_movs_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// BIC

int ins_bic(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & (~DP_REG_OPERAND(IMM_SHIFT)));
	
	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_bic_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & (~DP_REG_OPERAND(REG_SHIFT)));
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_bic_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & (~DP_IMM_OPERAND()));
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_bics(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & (~DP_REG_OPERAND_C(IMM_SHIFT)));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_bics_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & (~DP_REG_OPERAND_C(REG_SHIFT)));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_bics_imm(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get & (~DP_IMM_OPERAND()));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// MVN

int ins_mvn(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_REG_OPERAND(IMM_SHIFT));
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_mvn_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_REG_OPERAND(REG_SHIFT));
	
	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_mvn_imm(void)	
{	
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_IMM_OPERAND());
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_mvns(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_REG_OPERAND_C(IMM_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_mvns_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_REG_OPERAND_C(REG_SHIFT));
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_mvns_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get);
	
	instr_advance();	
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// MRS-MSR

int ins_mrs_cpsr(void)
{	
	INSTRUCTION_TRACE();
	DEST_REG_set (CPSR);
	
	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_msr_cpsr(void)
{
	unsigned char flags = (OPCODE>>16)&0xF;
	unsigned int cpsr_new = get_ARM_reg(OPCODE&0xF);
	INSTRUCTION_TRACE();

	// control field
	if ( flags & 0x01 )
	{
		if ( !(cpsr_new & 0x10) ) 
			set_abort ( ARMULATE_ABORT_INV_MODE );
		else
		{
			CPSR &= ~0x000000FF;
			CPSR |= cpsr_new & 0x000000FF;
		}
	}
	// extension field
	if ( flags & 0x02 )
	{
		CPSR &= ~0x0000FF00;
		CPSR |= cpsr_new & 0x0000FF00;
	}
	// status field
	if ( flags & 0x04 )
	{
		CPSR &= ~0x00FF0000;
		CPSR |= cpsr_new & 0x00FF0000;
	}
	// flags field
	if ( flags & 0x08 )
	{
		CPSR &= ~0xFF000000;
		CPSR |= cpsr_new & 0xFF000000;
	}

	SET_FLAGS_FROM_CPSR ();

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_mrs_spsr(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (arm->spsr[CPSR&0x1F]);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_msr_spsr(void)
{
	unsigned char flags = (OPCODE>>16)&0xF;
	unsigned int spsr_new = get_ARM_reg(OPCODE&0xF);
	INSTRUCTION_TRACE();

	// control field
	if ( flags & 0x01 )
	{
		if ( !(spsr_new & 0x10) ) 
			set_abort ( ARMULATE_ABORT_INV_MODE );
		else
		{
			arm->spsr [CPSR&0x1F] &= ~0x000000FF;
			arm->spsr [CPSR&0x1F] |= spsr_new & 0x000000FF;
		}
	}
	// extension field
	if ( flags & 0x02 )
	{
		arm->spsr [CPSR&0x1F] &= ~0x0000FF00;
		arm->spsr [CPSR&0x1F] |= spsr_new & 0x0000FF00;
	}
	// status field
	if ( flags & 0x04 )
	{
		arm->spsr [CPSR&0x1F] &= ~0x00FF0000;
		arm->spsr [CPSR&0x1F] |= spsr_new & 0x00FF0000;
	}
	// flags field
	if ( flags & 0x08 )
	{
		arm->spsr [CPSR&0x1F] &= ~0xFF000000;
		arm->spsr [CPSR&0x1F] |= spsr_new & 0xFF000000;
	}

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_msr_cpsr_imm(void)
{
	unsigned char flags = (OPCODE>>16)&0xF;
	unsigned int cpsr_new = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	// control field
	if ( flags & 0x01 )
	{
		if ( !(cpsr_new & 0x10) ) 
			set_abort ( ARMULATE_ABORT_INV_MODE );
		else
		{
			CPSR &= ~0x000000FF;
			CPSR |= cpsr_new & 0x000000FF;
		}
	}
	// extension field
	if ( flags & 0x02 )
	{
		CPSR &= ~0x0000FF00;
		CPSR |= cpsr_new & 0x0000FF00;
	}
	// status field
	if ( flags & 0x04 )
	{
		CPSR &= ~0x00FF0000;
		CPSR |= cpsr_new & 0x00FF0000;
	}
	// flags field
	if ( flags & 0x08 )
	{
		CPSR &= ~0xFF000000;
		CPSR |= cpsr_new & 0xFF000000;
	}

	SET_FLAGS_FROM_CPSR ();

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_msr_spsr_imm(void)	
{
	unsigned char flags = (OPCODE>>16)&0xF;
	unsigned int spsr_new = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	// control field
	if ( flags & 0x01 )
	{
		if ( !(spsr_new & 0x10) ) 
			set_abort ( ARMULATE_ABORT_INV_MODE );
		else
		{
			arm->spsr [CPSR&0x1F] &= ~0x000000FF;
			arm->spsr [CPSR&0x1F] |= spsr_new & 0x000000FF;
		}
	}
	// extension field
	if ( flags & 0x02 )
	{
		arm->spsr [CPSR&0x1F] &= ~0x0000FF00;
		arm->spsr [CPSR&0x1F] |= spsr_new & 0x0000FF00;
	}
	// status field
	if ( flags & 0x04 )
	{
		arm->spsr [CPSR&0x1F] &= ~0x00FF0000;
		arm->spsr [CPSR&0x1F] |= spsr_new & 0x00FF0000;
	}
	// flags field
	if ( flags & 0x08 )
	{
		arm->spsr [CPSR&0x1F] &= ~0xFF000000;
		arm->spsr [CPSR&0x1F] |= spsr_new & 0xFF000000;
	}


	instr_advance(); 
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

/*---------------------------------------------------------------------
					DATA TRANSFER
----------------------------------------------------------------------*/

int ins_ldr_pre_up (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();
	
	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get + offset, meminterface) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get + offset, meminterface) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get + offset, meminterface));

	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get + offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_pre_down (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get - offset, meminterface) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get - offset, meminterface) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get - offset, meminterface));

	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get - offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_post_up (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get, meminterface) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get, meminterface) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get, meminterface));

	BASE_REG_set(BASE_REG_get + offset);		

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_post_down (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get, meminterface) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get, meminterface) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get, meminterface));

	BASE_REG_set(BASE_REG_get - offset);		

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_imm_pre_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	register addr = BASE_REG_get + offset;
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (addr, meminterface) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (addr, meminterface) & ~1 );
	}
	else
		DEST_REG_set (read_word (addr, meminterface));

	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get + offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_imm_pre_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get - offset, meminterface) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get - offset, meminterface) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get - offset, meminterface));

	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get - offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_imm_post_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get, meminterface) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get, meminterface) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get, meminterface));

	BASE_REG_set(BASE_REG_get + offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_imm_post_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get, meminterface) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get, meminterface) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get, meminterface));

	BASE_REG_set(BASE_REG_get - offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

//----------------------------------------------------------------

int ins_ldrb_pre_up (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set ((u32)read_byte (BASE_REG_get + offset, meminterface));
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get + offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_ldrb_pre_down (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	DEST_REG_set ((u32)read_byte (BASE_REG_get - offset, meminterface));
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get - offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_ldrb_post_up (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get, meminterface));
	BASE_REG_set(BASE_REG_get + offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_ldrb_post_down (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get, meminterface));
	BASE_REG_set(BASE_REG_get - offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_ldrb_imm_pre_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get + offset, meminterface));
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get + offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_ldrb_imm_pre_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get - offset, meminterface));
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get - offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_ldrb_imm_post_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get, meminterface));
	BASE_REG_set(BASE_REG_get + offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_ldrb_imm_post_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get, meminterface));
	BASE_REG_set(BASE_REG_get - offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_str_pre_up (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);	
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get + offset, DEST_REG_get, meminterface);
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get + offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_str_pre_down (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get - offset, DEST_REG_get, meminterface);
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get - offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_str_post_up (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get, DEST_REG_get, meminterface); 
	BASE_REG_set(BASE_REG_get + offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_str_post_down (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get, DEST_REG_get, meminterface); 
	BASE_REG_set(BASE_REG_get - offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_str_imm_pre_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get + offset, DEST_REG_get, meminterface);
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get + offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_str_imm_pre_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get - offset, DEST_REG_get, meminterface);
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get - offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_str_imm_post_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get, DEST_REG_get, meminterface); 
	BASE_REG_set(BASE_REG_get + offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_str_imm_post_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get, DEST_REG_get, meminterface);
	BASE_REG_set(BASE_REG_get - offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_strb_pre_up (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get + offset, (u8)DEST_REG_get, meminterface); 
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get + offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_strb_pre_down (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get - offset, (u8)DEST_REG_get, meminterface); 
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get - offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_strb_post_up (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get, (u8)DEST_REG_get, meminterface); 
	BASE_REG_set(BASE_REG_get + offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_strb_post_down (void)
{
	u32 offset = DP_REG_OPERAND(IMM_SHIFT);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get, (u8)DEST_REG_get, meminterface); 
	BASE_REG_set(BASE_REG_get - offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_strb_imm_pre_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get + offset, (u8)DEST_REG_get, meminterface);
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get + offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_strb_imm_pre_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get - offset, (u8)DEST_REG_get, meminterface);
	if (OPCODE&0x200000) 
		BASE_REG_set(BASE_REG_get - offset);	
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_strb_imm_post_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get, (u8)DEST_REG_get, meminterface); 
	BASE_REG_set(BASE_REG_get + offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_strb_imm_post_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get, (u8)DEST_REG_get, meminterface); 
	BASE_REG_set(BASE_REG_get - offset);
	
	instr_advance(); 
	return ARMULATE_EXECUTED;
}

//-----------------------------------------------------------------------

int ins_ldrh_pre (void)
{
	u32 offset = get_ARM_reg(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	if (OPCODE&0x00800000) 
	{
		DEST_REG_set ((u32)read_hword (BASE_REG_get + offset, meminterface));
		if (OPCODE&0x00200000) 
			BASE_REG_set(BASE_REG_get + offset);	
	}
	else 
	{
		DEST_REG_set ((u32)read_hword (BASE_REG_get - offset, meminterface));
		if (OPCODE&0x00200000) 
			BASE_REG_set(BASE_REG_get - offset);	
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrh_post (void)
{
	u32 offset = get_ARM_reg(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	DEST_REG_set (read_hword (BASE_REG_get, meminterface));

	if (OPCODE&0x800000) 
		BASE_REG_set(BASE_REG_get + offset); 
	else 
		BASE_REG_set(BASE_REG_get - offset);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrsb (void)
{
	u32 offset = get_ARM_reg(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	if (OPCODE&0x1000000) 
	{
		if (OPCODE&0x800000) 
		{
			DEST_REG_set ((s32)read_byte (BASE_REG_get + offset, meminterface));
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get + offset);	
		}
		else 
		{
			DEST_REG_set ((s32)read_byte (BASE_REG_get - offset, meminterface));
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get - offset);	
		}
	}
	else 
	{
		DEST_REG_set ((s32)read_byte (BASE_REG_get, meminterface));
		if (OPCODE&0x800000) 
			BASE_REG_set(BASE_REG_get + offset); 
		else 
			BASE_REG_set(BASE_REG_get - offset);
	}
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrsh (void)
{
	u32 offset = get_ARM_reg(OPCODE&0xF);
	u16 temp;
	INSTRUCTION_TRACE();


	if (OPCODE&0x1000000) 
	{
		if (OPCODE&0x800000) 
		{
			temp = read_hword (BASE_REG_get + offset, meminterface);
			
			if (temp&0x8000) 
				DEST_REG_set (0 - temp);
			else 
				DEST_REG_set (temp);
			
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get + offset);	
		}
		else 
		{
			temp = read_hword (BASE_REG_get - offset, meminterface);
			
			if (temp&0x8000) 
				DEST_REG_set (0 - temp);
			else
				DEST_REG_set (temp);

			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get - offset);	
		}
	}
	else 
	{
		temp = read_hword (BASE_REG_get, meminterface);

		if (temp&0x8000)
			DEST_REG_set (0 - temp);
		else 
			DEST_REG_set (temp);

		if (OPCODE&0x800000) 
			BASE_REG_set(BASE_REG_get + offset); 
		else 
			BASE_REG_set(BASE_REG_get - offset);
	}
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strh_pre (void)
{
	u32 offset = get_ARM_reg(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	if (OPCODE&0x800000) 
	{
		write_hword (BASE_REG_get+offset, (u16)DEST_REG_get, meminterface);
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get + offset);	
	}
	else 
	{
		write_hword (BASE_REG_get-offset, (u16)DEST_REG_get, meminterface);
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get - offset);	
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strh_post (void)
{
	u32 offset = get_ARM_reg(OPCODE&0xF);
	INSTRUCTION_TRACE();


	write_hword (BASE_REG_get, (u16)DEST_REG_get, meminterface);

	if (OPCODE&0x800000) 
		BASE_REG_set(BASE_REG_get + offset); 
	else 
		BASE_REG_set(BASE_REG_get - offset);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrh_imm_pre (void)
{
	u32 offset = ((OPCODE&0xF00)>>4)|(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	if (OPCODE&0x800000) 
	{
		DEST_REG_set (read_hword (BASE_REG_get + offset, meminterface));
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get + offset);
	}
	else 
	{
		DEST_REG_set (read_hword (BASE_REG_get - offset, meminterface));
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get - offset);
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrh_imm_post (void)
{
	u32 offset = ((OPCODE&0xF00)>>4)|(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	DEST_REG_set (read_hword (BASE_REG_get, meminterface));
	if (OPCODE&0x800000) 
		BASE_REG_set(BASE_REG_get + offset); 
	else 
		BASE_REG_set(BASE_REG_get - offset);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrsb_imm (void)
{
	u32 offset = ((OPCODE&0xF00)>>4)|(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	if (OPCODE&0x1000000) 
	{
		if (OPCODE&0x800000) 
		{
			DEST_REG_set ((s32)read_byte (BASE_REG_get + offset, meminterface));
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get + offset);
		}
		else 
		{
			DEST_REG_set ((s32)read_byte (BASE_REG_get - offset, meminterface));
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get - offset);
		}
	}
	else 
	{
		DEST_REG_set ((s32)read_byte (BASE_REG_get, meminterface));
		if (OPCODE&0x800000) 
			BASE_REG_set(BASE_REG_get + offset); 
		else 
			BASE_REG_set(BASE_REG_get - offset);
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrsh_imm (void)
{
	u32 offset = ((OPCODE&0xF00)>>4)|(OPCODE&0xF);
	u16 temp;
	INSTRUCTION_TRACE();

	
	if (OPCODE&0x1000000) 
	{
		if (OPCODE&0x800000) 
		{
			temp = read_hword (BASE_REG_get + offset, meminterface);
			
			if (temp&0x8000) 
				DEST_REG_set (0 - temp);
			else 
				DEST_REG_set (temp);

			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get + offset);
		}
		else 
		{
			temp = read_hword (BASE_REG_get - offset, meminterface);

			if (temp&0x8000) 
				DEST_REG_set (0 - temp);
			else
				DEST_REG_set (temp);

			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get - offset);
		}
	}
	else 
	{
		temp = read_hword (BASE_REG_get, meminterface);

		if (temp&0x8000)
			DEST_REG_set (0 - temp);
		else
			DEST_REG_set (temp);

		if (OPCODE&0x800000) 
			BASE_REG_set(BASE_REG_get + offset); 
		else 
			BASE_REG_set(BASE_REG_get - offset);
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strh_imm_pre (void)
{
	u32 offset = ((OPCODE&0xF00)>>4)|(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	if (OPCODE&0x800000) 
	{
		write_hword (BASE_REG_get+offset, (u16)DEST_REG_get, meminterface);
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get + offset);	
	}
	else 
	{
		write_hword (BASE_REG_get-offset, (u16)DEST_REG_get, meminterface);
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get - offset);	
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strh_imm_post (void)
{
	u32 offset = ((OPCODE&0xF00)>>4)|(OPCODE&0xF);
	INSTRUCTION_TRACE();

	write_hword (BASE_REG_get, (u16)DEST_REG_get, meminterface);

	if (OPCODE&0x800000) 
		BASE_REG_set(BASE_REG_get + offset); 
	else 
		BASE_REG_set(BASE_REG_get - offset);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrb (void)
{
	int i;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;
	boolean S_bit = (OPCODE&0x00000040)?TRUE:FALSE;

	u32 offset = BASE_REG_get;

	int dir_val = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
		dir_val = ((signed char)((OPCODE&0xF00)>>4)|(OPCODE&0xF));
	else
		dir_val = OP_REG_get;

	// Up
	if (U_bit) 
		dir_val = dir_val;
	else
		dir_val = -dir_val;

	// Pre-indexed / offset addressing
	if ( P_bit )
		offset += dir_val;

	if (S_bit)
		DEST_REG_set ( ((signed char)read_byte ( offset, meminterface )) );
	else
		DEST_REG_set ( read_hword ( offset, meminterface ) );

	// Post-indexed
	if ( !P_bit )
		offset += dir_val;

	if ( W_bit || !P_bit )
		BASE_REG_set ( offset ); 

	instr_advance();
	return ARMULATE_EXECUTED;
}


int ins_strb (void)
{
	int i;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 offset = BASE_REG_get;

	int dir_val = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
		dir_val = ((signed char)((OPCODE&0xF00)>>4)|(OPCODE&0xF));
	else
		dir_val = OP_REG_get;

	// Up
	if (U_bit) 
		dir_val = dir_val;
	else
		dir_val = -dir_val;

	// Pre-indexed / offset addressing
	if ( P_bit )
		offset += dir_val;

	write_byte ( offset, DEST_REG_get, meminterface );

	// Post-indexed
	if ( !P_bit )
		offset += dir_val;

	if ( W_bit || !P_bit )
		BASE_REG_set ( offset ); 

	instr_advance();
	return ARMULATE_EXECUTED;
}



int ins_ldrh (void)
{
	int i;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;
	boolean S_bit = (OPCODE&0x00000040)?TRUE:FALSE;

	u32 offset = BASE_REG_get;

	int dir_val = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
		dir_val = ((signed char)((OPCODE&0xF00)>>4)|(OPCODE&0xF));
	else
		dir_val = OP_REG_get;

	// Up
	if (U_bit) 
		dir_val = dir_val;
	else
		dir_val = -dir_val;

	// Pre-indexed / offset addressing
	if ( P_bit )
		offset += dir_val;

	if (S_bit)
		DEST_REG_set ( ((signed short)read_hword ( offset, meminterface )) );
	else
		DEST_REG_set ( read_hword ( offset, meminterface ) );

	// Post-indexed
	if ( !P_bit )
		offset += dir_val;

	if ( W_bit || !P_bit )
		BASE_REG_set ( offset ); 

	instr_advance();
	return ARMULATE_EXECUTED;
}


int ins_strh (void)
{
	int i;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 offset = BASE_REG_get;

	int dir_val = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
		dir_val = ((signed char)((OPCODE&0xF00)>>4)|(OPCODE&0xF));
	else
		dir_val = OP_REG_get;

	// Up
	if (U_bit) 
		dir_val = dir_val;
	else
		dir_val = -dir_val;

	// Pre-indexed / offset addressing
	if ( P_bit )
		offset += dir_val;

	write_word ( offset, DEST_REG_get, meminterface );

	// Post-indexed
	if ( !P_bit )
		offset += dir_val;

	if ( W_bit || !P_bit )
		BASE_REG_set ( offset ); 

	instr_advance();
	return ARMULATE_EXECUTED;
}


int ins_stm (void)
{
	int i;
	unsigned char txtbuf[1024];

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean S_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 list_reg = OPCODE & 0x0000FFFF;

	int offset = BASE_REG_get;
	int dir_val = 0;
	int reg_start = 0;
	int reg_end = 15;
	int reg_step = 1;

	INSTRUCTION_TRACE();

	// Up
	if (U_bit) 
		dir_val = 4;
	else
		dir_val = -4;

	for (i=0; i<16; i++)
	{
		int reg;

		if (U_bit) 
			reg = i;
		else
			reg = 15-i;

		if (list_reg&(0x1<<reg)) 
		{
			// Pre-indexed / offset addressing
			if ( P_bit )
				offset += dir_val;

			write_word ( offset, get_ARM_reg ( reg ), meminterface );

			// Post-indexed
			if ( !P_bit )
				offset += dir_val;
		}
	}

	if ( W_bit )
		BASE_REG_set ( offset ); 
/*
	{
	int i;
	u32 offset=0;
	int n=0;


	u32 P_bit = OPCODE&0x01000000;
	u32 U_bit = OPCODE&0x800000;
	u32 W_bit = OPCODE&0x200000;
	u32 base_reg = (OPCODE>>16)& 0xF;
	u32 list_reg = OPCODE & 0x0000FFFF;
	INSTRUCTION_TRACE();

	armulate_warn ( "OLD\n" );

	// P bit set 
	if (P_bit) 
	{
		// U bit set 
		if (U_bit) 
		{
			for (i=0; i<16; i++)
			{
				if (list_reg&(0x1<<i)) 
				{
					if (W_bit) 
						BASE_REG_set(BASE_REG_get + 4); 
					else 
						offset += 4;

			sprintf ( txtbuf, "0x%08X Set 0x%08X to 0x%08X\n", armulate_get_pc(), BASE_REG_get + offset, get_ARM_reg ( i ) );
			armulate_warn ( txtbuf );
					write_word (BASE_REG_get + offset, get_ARM_reg(i), meminterface);
				}
			}
		}
		// U bit not set 
		else 
		{
			for (i=15; i>=0; i--) 
			{
				if (list_reg&(0x1<<i)) 
				{
					if (W_bit) 
						BASE_REG_set(BASE_REG_get - 4); 
					else 
						offset += 4;

			sprintf ( txtbuf, "0x%08X Set 0x%08X to 0x%08X\n", armulate_get_pc(), BASE_REG_get - offset, get_ARM_reg ( i ) );
			armulate_warn ( txtbuf );
					write_word (BASE_REG_get - offset, get_ARM_reg(i), meminterface);
				}
			}
		}	
	}
	// P bit not set 
	else 
	{
		// U bit set 
		if (U_bit) 
		{
			for (i=0; i<16; i++) 
			{
				if (list_reg&(0x1<<i)) 
				{
			sprintf ( txtbuf, "0x%08X Set 0x%08X to 0x%08X\n", armulate_get_pc(), BASE_REG_get+offset, get_ARM_reg ( i ) );
			armulate_warn ( txtbuf );
					write_word (BASE_REG_get+offset, get_ARM_reg(i), meminterface);

					if (W_bit) 
						BASE_REG_set(BASE_REG_get + 4); 
					else 
						offset += 4;
				}
			}
		}
		// U bit not set 
		else 
		{
			for (i=15; i>=0; i--) 
			{
				if (list_reg&(0x1<<i)) 
				{
			sprintf ( txtbuf, "0x%08X Set 0x%08X to 0x%08X\n", armulate_get_pc(), BASE_REG_get-offset, get_ARM_reg ( i ) );
			armulate_warn ( txtbuf );
					write_word (BASE_REG_get-offset, get_ARM_reg(i), meminterface);

					if (W_bit) 
						BASE_REG_set(BASE_REG_get - 4); 
					else 
						offset += 4;
				}
			}
		}	
	}
}
*/
	instr_advance();
	return ARMULATE_EXECUTED;
}


int ins_ldm (void)
{
	u32 i;
	u32 data = 0;
	unsigned char txtbuf[1024];

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean S_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 list_reg = OPCODE & 0x0000FFFF;

	int offset = BASE_REG_get;
	int dir_val = 0;

	INSTRUCTION_TRACE();


	// Up
	if (U_bit) 
		dir_val = 4;
	else
		dir_val = -4;

	for (i=0; i<16; i++)
	{
		int reg;

		if (U_bit) 
			reg = i;
		else
			reg = 15-i;

		if (list_reg&(0x1<<reg)) 
		{
			// Pre-indexed / offset addressing
			if ( P_bit )
				offset += dir_val;

			data = read_word ( offset, meminterface );

			// if PC gets loaded
			if ( reg == 15 )
			{
				set_ARM_mode ( ( data & 1 )?THUMB_MODE:ARM_MODE );
				data &= ~1;
			}

			set_ARM_reg ( reg, data );

			// Post-indexed
			if ( !P_bit )
				offset += dir_val;
		}
	}

	if ( W_bit )
		BASE_REG_set ( offset ); 
/*	
	{
	u32 temp;
	u32 update_base = 0;
	u32 i;
	s32 offset=0;
	u32 n=0;

	u32 P_bit = OPCODE&0x01000000;
	u32 U_bit = OPCODE&0x00800000;
	u32 W_bit = OPCODE&0x00200000;
	u32 base_reg = (OPCODE>>16)& 0xF;
	u32 list_reg = OPCODE & 0x0000FFFF;

	INSTRUCTION_TRACE();

	// P bit set 
	if (P_bit) 
	{
		// U bit set
		if (U_bit) 
		{
			for (i=0; i<16; i++) 
			{
				if (list_reg & (1<<i) ) 
				{
					if (W_bit) 
						BASE_REG_set ( BASE_REG_get + 4 ); 
					else 
						offset += 4;

					if (i != base_reg) 
					{
			sprintf ( txtbuf, "0x%08X read from 0x%08X to reg %i\n", armulate_get_pc(), BASE_REG_get+offset, i );
			armulate_warn ( txtbuf );
						if ( i != 15 )
							set_ARM_reg(i, read_word (BASE_REG_get+offset, meminterface));
						else
						{
							if ( read_word (BASE_REG_get+offset, meminterface) & 1 )
								set_ARM_mode ( THUMB_MODE );
							set_ARM_reg(i, read_word (BASE_REG_get+offset, meminterface) & ~1);
						}
					}
					else 
					{
						temp = read_word (BASE_REG_get+offset, meminterface); 
						update_base = 1;
					}
				}
			}

		}
		// U bit not set
		else 
		{
			for (i=15; i>=0; i--) 
			{
				if (list_reg&(1<<i)) 
				{
					if (W_bit) 
						BASE_REG_set(BASE_REG_get - 4); 
					else 
						offset += 4;

					if (i != base_reg) 
					{
			sprintf ( txtbuf, "0x%08X read from 0x%08X to reg %i\n", armulate_get_pc(), BASE_REG_get-offset, i );
			armulate_warn ( txtbuf );
						if ( i != 15 )
							set_ARM_reg(i, read_word (BASE_REG_get-offset, meminterface));
						else
						{
							if ( read_word (BASE_REG_get-offset, meminterface) & 1 )
								set_ARM_mode ( THUMB_MODE );
							set_ARM_reg(i, read_word (BASE_REG_get-offset, meminterface) & ~1);
						}
					}
					else 
					{
						temp = read_word (BASE_REG_get+offset, meminterface); 
						update_base = 1;
					}
				}
			}
		}	
	}
	// P bit not set 
	else 
	{
		// U bit set
		if (U_bit) 
		{
			for (i=0; i<16; i++) 
			{
				if (list_reg&(0x1<<i))
				{
					if (i != base_reg) 
					{
			sprintf ( txtbuf, "0x%08X read from 0x%08X to reg %i\n", armulate_get_pc(), BASE_REG_get+offset, i );
			armulate_warn ( txtbuf );
						if ( i != 15 )
							set_ARM_reg(i, read_word (BASE_REG_get+offset, meminterface));
						else
						{
							if ( read_word (BASE_REG_get+offset, meminterface) & 1 )
								set_ARM_mode ( THUMB_MODE );
							set_ARM_reg(i, read_word (BASE_REG_get+offset, meminterface) & ~1);
						}
					}
					else 
					{
						temp = read_word (BASE_REG_get+offset, meminterface); 
						update_base = 1;
					}

					if (W_bit) 
						BASE_REG_set(BASE_REG_get + 4); 
					else 
						offset += 4;
				}
			}
		}
		// U bit not set
		else
		{
			for (i=15; i>=0; i--) 
			{
				if (list_reg&(1<<i)) 
				{
					if (i != base_reg) 
					{
			sprintf ( txtbuf, "0x%08X read from 0x%08X to reg %i\n", armulate_get_pc(), BASE_REG_get-offset, i );
			armulate_warn ( txtbuf );
						if ( i != 15 )
							set_ARM_reg(i, read_word (BASE_REG_get-offset, meminterface));
						else
						{
							if ( read_word (BASE_REG_get-offset, meminterface) & 1 )
								set_ARM_mode ( THUMB_MODE );
							set_ARM_reg(i, read_word (BASE_REG_get-offset, meminterface) & ~1);
						}
					}
					else 
					{
						temp = read_word (BASE_REG_get+offset, meminterface); 
						update_base = 1;
					}
					if (W_bit) 
						BASE_REG_set(BASE_REG_get - 4); 
					else 
						offset += 4;
				}
			}
		}	
	}

	if (update_base) 
		BASE_REG_set (temp);
	}

*/
	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_swi (void)
{
	INSTRUCTION_TRACE();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_swp (void)
{
	u32 temp;
	temp = read_word (BASE_REG_get, meminterface);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get, OP_REG_get, meminterface);
	DEST_REG_set (temp);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}

int ins_swpb (void)
{
	u8 temp;
	INSTRUCTION_TRACE();

	temp = read_byte (BASE_REG_get, meminterface);
	write_byte (BASE_REG_get, (u8)OP_REG_get, meminterface);
	DEST_REG_set (temp);

	instr_advance(); 
	return ARMULATE_EXECUTED;
}



int ins_ldrd (void)
{
	u32 temp;
	u32 update_base = 0;
	u32 i;
	u32 n=0;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 base_reg = (OPCODE>>16)& 0xF;
	u32 dest_reg = (OPCODE>>12)& 0xF;

	char offset_val = ((OPCODE & 0x00000F00)>>4) | (OPCODE & 0x0000000F);
	int offset = 0;
	int dir_val  = 0;
	INSTRUCTION_TRACE();


	// Immediate
	if (I_bit)
		offset = offset_val;
	else
		offset = get_ARM_reg (OPCODE & 0x0000000F);

	// Up
	if (U_bit) 
		dir_val = 4;
	else
		dir_val = -4;


	for (i=dest_reg; i<dest_reg+2; i++) 
	{
		// Pre-indexed / offset addressing
		if (P_bit && W_bit) 
			BASE_REG_set ( BASE_REG_get + dir_val ); 

		if (i != base_reg) 
			set_ARM_reg ( i, read_word (BASE_REG_get+offset, meminterface));
		else 
		{
			temp = read_word (BASE_REG_get+offset, meminterface); 
			update_base = 1;
		}

		// Post-indexed
		if ( !P_bit )
			BASE_REG_set(BASE_REG_get + dir_val); 
		else
			offset += dir_val;

	}

	if (update_base) 
		BASE_REG_set (temp);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_strd (void)
{
	u32 i;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 base_reg = (OPCODE>>16)& 0xF;
	u32 dest_reg = (OPCODE>>12)& 0xF;

	char offset_val = ((OPCODE & 0x00000F00)>>4) | (OPCODE & 0x0000000F);
	int offset = 0;
	int dir_val  = 0;
	INSTRUCTION_TRACE();


	// Immediate
	if (I_bit)
		offset = offset_val;
	else
		offset = get_ARM_reg (OPCODE & 0x0000000F);

	// Up
	if (U_bit) 
		dir_val = 4;
	else
		dir_val = -4;


	for (i=dest_reg; i<dest_reg+2; i++) 
	{
		// Pre-indexed / offset addressing
		if (P_bit && W_bit) 
			BASE_REG_set ( BASE_REG_get + dir_val ); 

		write_word ( BASE_REG_get+offset, get_ARM_reg ( i ), meminterface );

		// Post-indexed
		if ( !P_bit )
			BASE_REG_set(BASE_REG_get + dir_val); 
		else
			offset += dir_val;

	}

	instr_advance ();
	return ARMULATE_EXECUTED;
}