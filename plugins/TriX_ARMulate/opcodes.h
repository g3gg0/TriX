

#define DEST_REG		((OPCODE>>12)&0xF)

static u32 DEST_REG_get ()
{
	return get_ARM_reg((OPCODE>>12)&0xF);
}

static void DEST_REG_set( u32 value )
{
	int reg = (OPCODE>>12)&0xF;

	if ( instruction_trace )
	{
		unsigned char buffer[128];
		sprintf ( buffer, "  R%02d = 0x%08X\n", reg, value );\
		armulate_warn ( buffer );
	}
	set_ARM_reg(reg, value);
}

static u32 BASE_REG_get ()
{
	return get_ARM_reg((OPCODE>>16)&0xF);
}

static void BASE_REG_set( u32 value )
{
	int reg = (OPCODE>>16)&0xF;

	if ( instruction_trace )
	{
		unsigned char buffer[128];
		sprintf ( buffer, "  R%02d = 0x%08X\n", reg, value );\
		armulate_warn ( buffer );
	}
	set_ARM_reg(reg, value);
}

static u32 OP_REG_get ()
{
	return get_ARM_reg(OPCODE&0xF);
}

static void OP_REG_set( u32 value )
{
	int reg = OPCODE&0xF;

	if ( instruction_trace )
	{
		unsigned char buffer[128];
		sprintf ( buffer, "  R%02d = 0x%08X\n", reg, value );\
		armulate_warn ( buffer );
	}
	set_ARM_reg(reg, value);
}

static u32 SHFT_AMO_REG_get()
{
	return get_ARM_reg((OPCODE>>8)&0xF);
}

static void SHFT_AMO_REG_set( u32 value )
{
	int reg = (OPCODE>>8)&0xF;

	if ( instruction_trace )
	{
		unsigned char buffer[128];
		sprintf ( buffer, "  R%02d = 0x%08X\n", reg, value );\
		armulate_warn ( buffer );
	}
	set_ARM_reg(reg, value);
}

#define HDT_CALC_IMM_OFFSET	((OPCODE&0xF00)>>4)|(OPCODE&0xF)
#define S_BIT 0x00100000

#ifndef INT64
#define INT64 long long
#endif

#ifndef UINT64
#define UINT64 unsigned long long
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


__inline u32 DP_IMM_OPERAND (void)
{		
	u32 value = OPCODE;

    u32 ror_count = ((value & 0xF00) >> 7) & 0xFF;

    value &= 0xFF;
    value = (value>>ror_count) | (value<<(32-ror_count));
    value &= 0xFFFFFFFF;
		
	return value;
}

//-------------Barrel-Shifter ("evil"(tm))----------------------------------------------

#define IMM_SHIFT	((OPCODE>>7)&0x1F)
#define REG_SHIFT	(SHFT_AMO_REG_get()&0xFF)

__inline u32 DP_REG_OPERAND (u32 shift_amount, int immediate)
{
	u32 op = OP_REG_get();
	switch ((OPCODE>>5)&0x3)	
	{							
		case 0:
			return (op << shift_amount);	

		case 1:
			return (op >> shift_amount);

		case 2:
			if (shift_amount == 0)
			{
				return op;
			}
			else if (shift_amount < 32)
			{
				if (op&0x80000000)
					return ((0xFFFFFFFF<<(32-shift_amount))|(op>>shift_amount));
				else
					return (op >> shift_amount);
			}
			else
			{
				if (op&0x80000000)
					return 0xFFFFFFFF;
				else
					return 0;
			}

		case 3:
			if (shift_amount || !immediate)
				return (op << (32-shift_amount)|(op>>shift_amount));
			else
				return ((op>>1)|(CFLAG<<31));
	}
	return 0;
}

__inline u32 DP_REG_OPERAND_C (u32 shift_amount, int immediate)
{
	u32 op = OP_REG_get();

	switch ((OPCODE>>5)&0x3)	
	{							
		case 0:
			CFLAG_SET( (op&(0x80000000>>(shift_amount-1))) ? (1):(0));
			return (op << shift_amount);

		case 1:
			if (shift_amount == 0)
			{
				return op;
			}
			else if (shift_amount < 32)
			{
				CFLAG_SET( (op&(1<<(shift_amount-1))) ? (1):(0));
				return (op >> shift_amount);
			}
			else if (shift_amount == 32)
			{
				CFLAG_SET( (op & 1));
				return 0;
			}
			else
			{
				CFLAG_SET( 0);
				return 0;
			}

		case 2:
			if (shift_amount == 0)
			{
				return op;
			}
			else if (shift_amount < 32)
			{
				CFLAG_SET( (op&(1<<(shift_amount-1))) ? (1):(0));
				if (op&0x80000000)
					return ((0xFFFFFFFF<<(32-shift_amount))|(op>>shift_amount));
				else
					return (op >> shift_amount);
			}
			else
			{
				CFLAG_SET(op>>31);
				if (CFLAG)
					return 0xFFFFFFFF;
				else
					return 0;
			}

		case 3:
			// only immediate operations support RRX
			if (shift_amount || !immediate)
			{
				CFLAG_SET((op&(1<<(shift_amount-1))) ? (1):(0));
				return (op << (32-shift_amount)|(op>>shift_amount));
			}
			else
			{
				// thats an RRX
				if (CFLAG)
				{
					CFLAG_SET(op&0x1);
					return ((op>>1)|0x80000000);
				}
				else
				{
					CFLAG_SET(op&0x1);	
					return (op>>1);
				}
			}
	}
	return 0;
}

_inline u32 DP_REG_OPERAND_C_IMM()
{
	return DP_REG_OPERAND_C ( IMM_SHIFT, 1 );
}

_inline u32 DP_REG_OPERAND_C_REG()
{
	return DP_REG_OPERAND_C ( REG_SHIFT, 1 );
}

_inline u32 DP_REG_OPERAND_IMM()
{
	return DP_REG_OPERAND ( IMM_SHIFT, 1 );
}

_inline u32 DP_REG_OPERAND_REG()
{
	return DP_REG_OPERAND ( REG_SHIFT, 1 );
}

//--------------------------------------------------------------------------------------

//---------------------Flag macros------------------------------------------------------

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
		ZFLAG_SET(0);
	else
		ZFLAG_SET(1);
	NFLAG_SET((c >> 31));
	CFLAG_SET(!BorrowFrom ( a, b ));
	VFLAG_SET(OverflowFromSub ( a, b, c ));
	
	
}

__inline void SET_ADD_FLAGS (u32 a, u32 b, u32 c)
{
	if (c)
		ZFLAG_SET(0);
	else
		ZFLAG_SET(1);
	NFLAG_SET((c >> 31));
	CFLAG_SET(CarryFrom ( a, b, c ));
	VFLAG_SET(OverflowFromAdd (a, b, c ));
	
	
}


// Logical Data Processing (value is supposed to be 32 bit)
__inline void SET_DP_LOG_FLAGS(u32 value)
{
	if (value)
		ZFLAG_SET(0);
	else
		ZFLAG_SET(1);

	NFLAG_SET((value>>31));

	
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

/*			if (arm->cpsr & THUMB_BIT)
				thumb = 1;
*/
			// TODO check the current mode and don't allow for SYSTEM and USER
			arm->cpsr = arm->spsr[arm->cpsr & 0x1F];
/*
			if ( thumb )
				arm->cpsr |= THUMB_BIT;
*/
			return 0;
		}
		else
			return 1;
	}
	else
		return 0;
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
	unsigned int value = OP_REG_get();
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

int ins_mcrmcr_match(u32 parm_cp_num, u32 parm_opcode1, u32 parm_crn, u32 parm_crm, u32 parm_opcode2)
{
	u32 opcode1 = (OPCODE >> 21) & 0x07;
	u32 crn = (OPCODE >> 16) & 0x0F;
	u32 rd = (OPCODE >> 12) & 0x0F;
	u32 cp_num = (OPCODE >> 8) & 0x0F;
	u32 opcode2 = (OPCODE >> 5) & 0x07;
	u32 crm = (OPCODE >> 0) & 0x0F;

	if(cp_num != parm_cp_num && parm_cp_num < 16)
	{
		return 0;
	}
	if(opcode1 != parm_opcode1 && parm_opcode1 < 8)
	{
		return 0;
	}
	if(crn != parm_crn && parm_crn < 16)
	{
		return 0;
	}
	if(crm != parm_crm && parm_crm < 16)
	{
		return 0;
	}
	if(opcode2 != parm_opcode2 && parm_opcode2 < 8)
	{
		return 0;
	}

	return 1;
}

int ins_mrc (void )
{
	u32 opcode1 = (OPCODE >> 21) & 0x07;
	u32 crn = (OPCODE >> 16) & 0x0F;
	u32 rd = (OPCODE >> 12) & 0x0F;
	u32 cp_num = (OPCODE >> 8) & 0x0F;
	u32 opcode2 = (OPCODE >> 5) & 0x07;
	u32 crm = (OPCODE >> 0) & 0x0F;

	INSTRUCTION_TRACE();

	/* system control processor - cpuid */
	if(ins_mcrmcr_match(15, 0, 0, 0, 0))
	{
		/* return ARM946 */
		set_ARM_reg ( rd, armulate_cp15_cpuid );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - cache type */
	if(ins_mcrmcr_match(15, 0, 0, 0, 1))
	{
		/* return 8Kib Caches */
		set_ARM_reg ( rd, armulate_cp15_cache_type );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - TCM size register */
	if(ins_mcrmcr_match(15, 0, 0, 0, 2))
	{
		/* return 4KiB TCM size */
		set_ARM_reg ( rd, armulate_cp15_tcm_size );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - control register */
	if(ins_mcrmcr_match(15, 0, 1, 0, 0))
	{
		char buffer[128];

		/* return 4KiB TCM size */
		set_ARM_reg ( rd, armulate_cp15_control_reg );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - memory protection ranges */
	if(ins_mcrmcr_match(15, 0, 6, 0xFF, 0))
	{
		set_ARM_reg ( rd, armulate_cp15_prot_reg[crm] );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - data access permission */
	if( ins_mcrmcr_match(15, 0, 5, 0, 0))
	{
		set_ARM_reg ( rd, armulate_cp15_data_perm_old );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - instr access permission */
	if(ins_mcrmcr_match(15, 0, 5, 0, 1))
	{
		set_ARM_reg ( rd, armulate_cp15_instr_perm_old );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - data access permission */
	if(ins_mcrmcr_match(15, 0, 5, 0, 2))
	{
		set_ARM_reg ( rd, armulate_cp15_data_perm );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - instr access permission */
	if(ins_mcrmcr_match(15, 0, 5, 0, 3))
	{
		set_ARM_reg ( rd, armulate_cp15_instr_perm );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - data cacheable bits */
	if(ins_mcrmcr_match(15, 0, 2, 0, 0))
	{
		set_ARM_reg ( rd, armulate_cp15_data_cachable );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - instruction cacheable bits */
	if(ins_mcrmcr_match(15, 0, 2, 0, 1))
	{
		set_ARM_reg ( rd, armulate_cp15_instr_cachable );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - bufferable bits */
	if(ins_mcrmcr_match(15, 0, 3, 0, 0))
	{
		set_ARM_reg ( rd, armulate_cp15_bufferable );
		instr_advance();
		return ARMULATE_EXECUTED;
	}
	//return ARMULATE_BP_REACHED;
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_mcr (void )
{
    char buffer[128];
	u32 opcode1 = (OPCODE >> 21) & 0x07;
	u32 crn = (OPCODE >> 16) & 0x0F;
	u32 rd = (OPCODE >> 12) & 0x0F;
	u32 cp_num = (OPCODE >> 8) & 0x0F;
	u32 opcode2 = (OPCODE >> 5) & 0x07;
	u32 crm = (OPCODE >> 0) & 0x0F;

	INSTRUCTION_TRACE();

	/* system control processor - control register */
	if(ins_mcrmcr_match(15, 0, 1, 0, 0))
	{
        u32 old_reg = armulate_cp15_control_reg;
		armulate_cp15_control_reg = get_ARM_reg ( rd );

		sprintf ( buffer, "[ARMulate] CP15 control reg set at 0x%08X: 0x%08X\n", armulate_get_pc(), armulate_cp15_control_reg );
		armulate_warn(buffer);
        if((old_reg ^ armulate_cp15_control_reg) & 0x04)
        {
            if(armulate_cp15_control_reg & 0x04)
            {
                sprintf ( buffer, "[ARMulate] 'Control Register: Data cache enable' ENABLED at 0x%08X\n", armulate_get_pc() );
            }
            else
            {
                sprintf ( buffer, "[ARMulate] 'Control Register: Data cache enable' DISABLED at 0x%08X\n", armulate_get_pc() );
            }
		    armulate_warn(buffer);
        }

        if((old_reg ^ armulate_cp15_control_reg) & 0x1000)
        {
            if(armulate_cp15_control_reg & 0x1000)
            {
                sprintf ( buffer, "[ARMulate] 'Control Register: Instruction cache enable' ENABLED at 0x%08X\n", armulate_get_pc() );
            }
            else
            {
                sprintf ( buffer, "[ARMulate] 'Control Register: Instruction cache enable' DISABLED at 0x%08X\n", armulate_get_pc() );
            }
		    armulate_warn(buffer);
        }

		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - memory protection ranges */
	if(ins_mcrmcr_match(15, 0, 6, 0xFF, 0))
	{
		armulate_cp15_prot_reg[crm] = get_ARM_reg ( rd );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - data access permission */
	if(ins_mcrmcr_match(15, 0, 5, 0, 0))
	{
		armulate_cp15_data_perm_old = get_ARM_reg ( rd );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - instr access permission */
	if(ins_mcrmcr_match(15, 0, 5, 0, 1))
	{
		armulate_cp15_instr_perm_old = get_ARM_reg ( rd );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - data access permission */
	if(ins_mcrmcr_match(15, 0, 5, 0, 2))
	{
		armulate_cp15_data_perm = get_ARM_reg ( rd );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - instr access permission */
	if(ins_mcrmcr_match(15, 0, 5, 0, 3))
	{
		armulate_cp15_instr_perm = get_ARM_reg ( rd );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - data cacheable bits */
	if(ins_mcrmcr_match(15, 0, 2, 0, 0))
	{
		armulate_cp15_data_cachable = get_ARM_reg ( rd );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - instruction cacheable bits */
	if(ins_mcrmcr_match(15, 0, 2, 0, 1))
	{
		armulate_cp15_instr_cachable = get_ARM_reg ( rd );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - bufferable bits */
	if(ins_mcrmcr_match(15, 0, 3, 0, 0))
	{
		armulate_cp15_bufferable = get_ARM_reg ( rd );
		instr_advance();
		return ARMULATE_EXECUTED;
	}

	/* system control processor - wait for interrupt */
	if(ins_mcrmcr_match(15, 0, 1, 0, 0) || ins_mcrmcr_match(15, 0, 15, 8, 2))
	{
		if(!armulate_mcr_int_waiting)
		{
			/* this is the first time, reset interrupt flag */
            armulate_mcr_int_happened = 0;
            armulate_mcr_int_waiting = 1;
		    return ARMULATE_EXECUTED;
		}
        else
        {
		    /* check if interrupt happened and we are done */
		    if(armulate_mcr_int_happened)
		    {
                armulate_mcr_int_happened = 0;
			    armulate_mcr_int_waiting = 0;
			    instr_advance();
			    return ARMULATE_EXECUTED;
		    }
            else
            {
                /* no interrupt happened, dont continue */
			    return ARMULATE_EXECUTED;
            }
        }
	}

	if(ins_mcrmcr_match(15, 0, 7, 5, 0))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Flush instruction cache' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
    }
    if(ins_mcrmcr_match(15, 0, 7, 5, 1))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Flush instruction cache single entry (MVA)' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 5, 2))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Flush instruction cache single entry (set/way)' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 6, 0))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Flush data cache' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 6, 1))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Flush data cache single entry (MVA)' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 6, 2))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Flush data cache single entry (set/way)' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 7, 0))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Invalidate both instruction and data caches or unified cache' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 7, 1))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Invalidate unified cache line' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 7, 2))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Invalidate unified cache line' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 10, 0))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Clean entire data cache' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 10, 1) || ins_mcrmcr_match(15, 0, 7, 10, 2))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Clean data cache entry' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 13, 1))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Prefetch instruction cache line' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	} 
    if(ins_mcrmcr_match(15, 0, 7, 14, 1) || ins_mcrmcr_match(15, 0, 7, 14, 2))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Clean and flush data cache entry' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	}
    if(ins_mcrmcr_match(15, 0, 7, 11, 0) || ins_mcrmcr_match(15, 0, 7, 11, 1) || ins_mcrmcr_match(15, 0, 7, 11, 2))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Clean unified cache' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	}
    if(ins_mcrmcr_match(15, 0, 7, 14, 0) || ins_mcrmcr_match(15, 0, 7, 14, 1) || ins_mcrmcr_match(15, 0, 7, 14, 2))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Clean and invalidate data cache' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	}
    if(ins_mcrmcr_match(15, 0, 7, 15, 0) || ins_mcrmcr_match(15, 0, 7, 15, 1) || ins_mcrmcr_match(15, 0, 7, 15, 2))
	{
        sprintf ( buffer, "[ARMulate] 'Cache Control: Clean and invalidate unified cache' at 0x%08X\n", armulate_get_pc() );
		armulate_warn(buffer);
		instr_advance();
		return ARMULATE_EXECUTED;
	}

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

	if ( OP_REG_get() & 1 )
		set_ARM_mode ( THUMB_MODE );

	set_ARM_reg( 15, OP_REG_get() & ~1 );

	instr_advance ();
	return ARMULATE_EXECUTED;
}


int ins_blx (void)
{
	INSTRUCTION_TRACE();
	set_ARM_reg ( 14,  get_ARM_reg (15) - 4 );

	if ( OP_REG_get() & 1 )
		set_ARM_mode ( THUMB_MODE );

	set_ARM_reg( 15, OP_REG_get() & ~1 );

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_mul(void)
{
	INSTRUCTION_TRACE();
	BASE_REG_set(OP_REG_get() * SHFT_AMO_REG_get());

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_muls(void)
{
	INSTRUCTION_TRACE();
	BASE_REG_set(OP_REG_get() * SHFT_AMO_REG_get());
	SET_DP_LOG_FLAGS(BASE_REG_get());

	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_mla(void)
{
	INSTRUCTION_TRACE();
	BASE_REG_set ((OP_REG_get() * SHFT_AMO_REG_get()) + DEST_REG_get());
	
	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_mlas(void)
{
	INSTRUCTION_TRACE();
	BASE_REG_set ((OP_REG_get() * SHFT_AMO_REG_get()) + DEST_REG_get());
	SET_DP_LOG_FLAGS(BASE_REG_get());
	
	check_S_bit();

	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_smull(void)
{
	INT64 temp64 = (INT64)OP_REG_get();
	INSTRUCTION_TRACE();

	temp64 *= ((s32)SHFT_AMO_REG_get());

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32) & 0xFFFFFFFF));

	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_smulls(void)
{
	INT64 temp64 = (INT64)OP_REG_get();

	INSTRUCTION_TRACE();
	temp64 *= ((s32)SHFT_AMO_REG_get());

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32) & 0xFFFFFFFF));

	if (!temp64)					ZFLAG_SET(1); else ZFLAG_SET(0);
	if (temp64&0x8000000000000000)	NFLAG_SET(1); else NFLAG_SET(0);

	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_umull(void)
{
	UINT64 temp64 = (UINT64)OP_REG_get();
	INSTRUCTION_TRACE();

	temp64 *= SHFT_AMO_REG_get();

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32) & 0xFFFFFFFF));

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_umulls(void)
{
	UINT64 temp64 = (UINT64)OP_REG_get();
	INSTRUCTION_TRACE();

	temp64 *= SHFT_AMO_REG_get();

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF));

	if (!temp64)					ZFLAG_SET(1); else ZFLAG_SET(0);
	if (temp64&0x8000000000000000)	NFLAG_SET(1); else NFLAG_SET(0);

	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_smlal(void)
{
	INT64 temp64 = (INT64)OP_REG_get();
	INT64 operand = (INT64)DEST_REG_get();
	INSTRUCTION_TRACE();

	operand |= ((INT64)BASE_REG_get())<<32;
		
	temp64 = (temp64 * ((INT64)SHFT_AMO_REG_get())) + operand;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF));
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_smlals(void)
{
	INT64 temp64 = (INT64)OP_REG_get();
	INT64 operand = (INT64)DEST_REG_get();
	INSTRUCTION_TRACE();
	operand |= ((INT64)BASE_REG_get())<<32;
		
	temp64 = (temp64 * ((INT64)SHFT_AMO_REG_get())) + operand;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF));

	if (!temp64)					ZFLAG_SET(1); else ZFLAG_SET(0);
	if (temp64&0x8000000000000000)	NFLAG_SET(1); else NFLAG_SET(0);

	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_umlal(void)
{
	UINT64 temp64 = (UINT64)OP_REG_get();
	UINT64 operand = (UINT64)DEST_REG_get();
	INSTRUCTION_TRACE();
	operand |= ((UINT64)BASE_REG_get())<<32;

	temp64 = (temp64 * ((UINT64)SHFT_AMO_REG_get())) + operand;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF));
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_umlals(void)
{
	UINT64 temp64 = (UINT64)OP_REG_get();
	UINT64 operand = (UINT64)DEST_REG_get();
	INSTRUCTION_TRACE();
	operand |= ((UINT64)BASE_REG_get())<<32;

	temp64 = (temp64 * ((UINT64)SHFT_AMO_REG_get())) + operand;

	DEST_REG_set ((u32)(temp64 & 0xFFFFFFFF));
	BASE_REG_set ((u32)((temp64>>32)&0xFFFFFFFF));

	if (!temp64)					ZFLAG_SET(1); else ZFLAG_SET(0);
	if (temp64&0x8000000000000000)	NFLAG_SET(1); else NFLAG_SET(0);

	check_S_bit();

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
	DEST_REG_set (BASE_REG_get() & DP_REG_OPERAND_IMM());
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_and_reg(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & DP_REG_OPERAND_REG());
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_and_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & DP_IMM_OPERAND());
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ands(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & DP_REG_OPERAND_C_IMM());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_ands_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & DP_REG_OPERAND_C_REG());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_ands_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// EOR

int ins_eor(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() ^ DP_REG_OPERAND_IMM());
	
	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_eor_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() ^ DP_REG_OPERAND_REG());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_eor_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() ^ DP_IMM_OPERAND());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_eors(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() ^ DP_REG_OPERAND_C_IMM());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_eors_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() ^ DP_REG_OPERAND_C_REG());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_eors_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() ^ DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// SUB

int ins_sub(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() - DP_REG_OPERAND_IMM());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sub_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() - DP_REG_OPERAND_REG());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sub_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() - DP_IMM_OPERAND());
	
	instr_advance(); return ARMULATE_EXECUTED;
}


int ins_subs(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();
	DEST_REG_set (op1 - op2);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get());

	check_S_bit();
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_subs_reg (void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_REG();
	INSTRUCTION_TRACE();
	DEST_REG_set (op1 - op2);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_subs_imm(void)
{
	u32 op1, op2;
	INSTRUCTION_TRACE();
	
	op1 = BASE_REG_get();
	op2 = DP_IMM_OPERAND();

	DEST_REG_set (op1 - op2);	
	SET_SUB_FLAGS(op1, op2, DEST_REG_get());

	check_S_bit();

	instr_advance();	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// RSB

int ins_rsb(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND_IMM() - BASE_REG_get());
	
	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_rsb_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND_REG() - BASE_REG_get());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsb_imm(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_IMM_OPERAND() - BASE_REG_get());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsbs(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsbs_reg (void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_REG();
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsbs_imm(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// ADD

int ins_add(void)
{
	INSTRUCTION_TRACE();

	DEST_REG_set (BASE_REG_get() + DP_REG_OPERAND_IMM());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_add_reg(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() + DP_REG_OPERAND_REG());
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_add_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() + DP_IMM_OPERAND());
	
	instr_advance();	return ARMULATE_EXECUTED;
}

int ins_adds(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adds_reg (void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_REG();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adds_imm(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// ADC

int ins_adc(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() + DP_REG_OPERAND_IMM() + CFLAG);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_adc_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() + DP_REG_OPERAND_REG() + CFLAG);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_adc_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() + DP_IMM_OPERAND() + CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adcs(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2 + CFLAG);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get());

	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adcs_reg (void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_REG();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2 + CFLAG);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_adcs_imm(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 + op2 + CFLAG);
	SET_ADD_FLAGS(op1, op2, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// SBC

int ins_sbc(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() - DP_REG_OPERAND_IMM() - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbc_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() - DP_REG_OPERAND_REG() - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbc_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() + DP_IMM_OPERAND() - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbcs(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 - op2 - !CFLAG);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbcs_reg (void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_REG();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 - op2 - !CFLAG);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_sbcs_imm(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op1 - op2 - !CFLAG);
	SET_SUB_FLAGS(op1, op2, DEST_REG_get());
	
	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------

// RSC

int ins_rsc(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND_IMM() - BASE_REG_get() - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsc_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND_REG() - BASE_REG_get() - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rsc_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_IMM_OPERAND() - BASE_REG_get() - !CFLAG);
	
	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rscs(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1 - !CFLAG);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get());

	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rscs_reg (void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_REG();
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1 - !CFLAG);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get());

	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

int ins_rscs_imm(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	DEST_REG_set (op2 - op1 - !CFLAG);
	SET_SUB_FLAGS(op2, op1, DEST_REG_get());

	check_S_bit();

	instr_advance(); return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// TST

int ins_tst(void)
{
	u32 temp = BASE_REG_get() & DP_REG_OPERAND_C_IMM();
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_tst_reg (void)
{
	u32 temp = BASE_REG_get() & DP_REG_OPERAND_C_REG();
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_tst_imm(void)
{
	u32 temp = BASE_REG_get() & DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance();
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// TEQ

int ins_teq(void)
{
	u32 temp = BASE_REG_get() ^ DP_REG_OPERAND_C_IMM();
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_teq_reg (void)
{
	u32 temp = BASE_REG_get() ^ DP_REG_OPERAND_C_REG();
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_teq_imm(void)
{
	u32 temp = BASE_REG_get() ^ DP_IMM_OPERAND();
	INSTRUCTION_TRACE();

	SET_DP_LOG_FLAGS (temp);

	instr_advance();
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// CMP

int ins_cmp(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	SET_SUB_FLAGS (op1, op2, op1 - op2);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_cmp_reg(void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_REG();
	INSTRUCTION_TRACE();

	SET_SUB_FLAGS (op1, op2, op1 - op2);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_cmp_imm(void)	
{
	u32 op1 = BASE_REG_get();
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
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	SET_ADD_FLAGS (op1, op2, op1 + op2);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_cmn_reg (void)
{
	u32 op1 = BASE_REG_get();
	u32 op2 = DP_REG_OPERAND_REG();
	INSTRUCTION_TRACE();

	SET_ADD_FLAGS (op1, op2, op1 + op2);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_cmn_imm(void)
{
	u32 op1 = BASE_REG_get();
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
	DEST_REG_set (BASE_REG_get() | DP_REG_OPERAND_IMM());
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_orr_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() | DP_REG_OPERAND_REG());
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_orr_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() | DP_IMM_OPERAND());
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_orrs(void)
{	
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() | DP_REG_OPERAND_C_IMM());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_orrs_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() | DP_REG_OPERAND_C_REG());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_orrs_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() | DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// MOV

int ins_mov(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND_IMM());

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_mov_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND_REG());

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
	DEST_REG_set (DP_REG_OPERAND_C_IMM());
	SET_DP_LOG_FLAGS(DEST_REG_get());

	check_S_bit();

	instr_advance();	
	return ARMULATE_EXECUTED;	
}

int ins_movs_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_REG_OPERAND_C_REG());
	SET_DP_LOG_FLAGS(DEST_REG_get());

	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_movs_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get());

	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// BIC

int ins_bic(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & (~DP_REG_OPERAND_IMM()));
	
	instr_advance();	
	return ARMULATE_EXECUTED;
}

int ins_bic_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & (~DP_REG_OPERAND_REG()));
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_bic_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & (~DP_IMM_OPERAND()));
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_bics(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & (~DP_REG_OPERAND_C_IMM()));
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_bics_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & (~DP_REG_OPERAND_C_REG()));
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_bics_imm(void)	
{
	INSTRUCTION_TRACE();
	DEST_REG_set (BASE_REG_get() & (~DP_IMM_OPERAND()));
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

//---------------------------------------------------------------------

// MVN

int ins_mvn(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_REG_OPERAND_IMM());
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_mvn_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_REG_OPERAND_REG());
	
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
	DEST_REG_set (~DP_REG_OPERAND_C_IMM());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_mvns_reg (void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_REG_OPERAND_C_REG());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_mvns_imm(void)
{
	INSTRUCTION_TRACE();
	DEST_REG_set (~DP_IMM_OPERAND());
	SET_DP_LOG_FLAGS(DEST_REG_get());
	
	check_S_bit();

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
	unsigned int valid[] = {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
	INSTRUCTION_TRACE();

	// control field
	if ( flags & 0x01 )
	{
		if ( !(cpsr_new & 0x10) || !valid[cpsr_new & 0x0F] )
		{
			set_abort ( ARMULATE_ABORT_INV_MODE );
			return ARMULATE_ABORT_INV_MODE;
		}
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
	unsigned int valid[] = {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
	INSTRUCTION_TRACE();

	// control field
	if ( flags & 0x01 )
	{
		if ( !(spsr_new & 0x10) || !valid[spsr_new & 0x0F] )
		{
			set_abort ( ARMULATE_ABORT_INV_MODE );
			return ARMULATE_ABORT_INV_MODE;
		}
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
	unsigned int valid[] = {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
	INSTRUCTION_TRACE();

	// control field
	if ( flags & 0x01 )
	{
		if ( !(cpsr_new & 0x10) || !valid[cpsr_new & 0x0F] )
		{
			set_abort ( ARMULATE_ABORT_INV_MODE );
			return ARMULATE_ABORT_INV_MODE;
		}
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

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_msr_spsr_imm(void)	
{
	unsigned char flags = (OPCODE>>16)&0xF;
	unsigned int spsr_new = DP_IMM_OPERAND();
	unsigned int valid[] = {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
	INSTRUCTION_TRACE();

	// control field
	if ( flags & 0x01 )
	{
		if ( !(spsr_new & 0x10) || !valid[spsr_new & 0x0F] )
		{
			set_abort ( ARMULATE_ABORT_INV_MODE );
			return ARMULATE_ABORT_INV_MODE;
		}
		else
		{
			arm->spsr[CPSR&0x1F] &= ~0x000000FF;
			arm->spsr[CPSR&0x1F] |= spsr_new & 0x000000FF;
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
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();
	
	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get() + offset, arm) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get() + offset, arm) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get() + offset, arm));

	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() + offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_pre_down (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get() - offset, arm) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get() - offset, arm) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get() - offset, arm));

	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() - offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_post_up (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get(), arm) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get(), arm) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get(), arm));

	BASE_REG_set(BASE_REG_get() + offset);		

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_post_down (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get(), arm) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get(), arm) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get(), arm));

	BASE_REG_set(BASE_REG_get() - offset);		

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_imm_pre_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	register addr = BASE_REG_get() + offset;
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (addr, arm) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (addr, arm) & ~1 );
	}
	else
		DEST_REG_set (read_word (addr, arm));

	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() + offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_imm_pre_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get() - offset, arm) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get() - offset, arm) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get() - offset, arm));

	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() - offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_imm_post_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get(), arm) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get(), arm) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get(), arm));

	BASE_REG_set(BASE_REG_get() + offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_ldr_imm_post_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	if ( DEST_REG == 15 )
	{
		if ( read_word (BASE_REG_get(), arm) & 1 )
			set_ARM_mode ( THUMB_MODE );

		DEST_REG_set (read_word (BASE_REG_get(), arm) & ~1 );
	}
	else
		DEST_REG_set (read_word (BASE_REG_get(), arm));

	BASE_REG_set(BASE_REG_get() - offset);

	instr_advance ();
	return ARMULATE_EXECUTED;
}

//----------------------------------------------------------------

int ins_ldrb_pre_up (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	DEST_REG_set ((u32)read_byte (BASE_REG_get() + offset, arm));
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() + offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrb_pre_down (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	DEST_REG_set ((u32)read_byte (BASE_REG_get() - offset, arm));
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() - offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrb_post_up (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get(), arm));
	BASE_REG_set(BASE_REG_get() + offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrb_post_down (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get(), arm));
	BASE_REG_set(BASE_REG_get() - offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrb_imm_pre_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get() + offset, arm));
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() + offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrb_imm_pre_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get() - offset, arm));
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() - offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrb_imm_post_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get(), arm));
	BASE_REG_set(BASE_REG_get() + offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrb_imm_post_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();
	DEST_REG_set ((u32)read_byte (BASE_REG_get(), arm));
	BASE_REG_set(BASE_REG_get() - offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_str_pre_up (void)
{
	u32 offset = DP_REG_OPERAND_IMM();	
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get() + offset, DEST_REG_get(), arm);
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() + offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_str_pre_down (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get() - offset, DEST_REG_get(), arm);
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() - offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_str_post_up (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get(), DEST_REG_get(), arm);
	BASE_REG_set(BASE_REG_get() + offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_str_post_down (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get(), DEST_REG_get(), arm);
	BASE_REG_set(BASE_REG_get() - offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_str_imm_pre_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get() + offset, DEST_REG_get(), arm);
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() + offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_str_imm_pre_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get() - offset, DEST_REG_get(), arm);
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() - offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_str_imm_post_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get(), DEST_REG_get(), arm);
	BASE_REG_set(BASE_REG_get() + offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_str_imm_post_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_word (BASE_REG_get(), DEST_REG_get(), arm);
	BASE_REG_set(BASE_REG_get() - offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strb_pre_up (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get() + offset, (u8)DEST_REG_get(), arm);
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() + offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strb_pre_down (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get() - offset, (u8)DEST_REG_get(), arm);
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() - offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strb_post_up (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get(), (u8)DEST_REG_get(), arm);
	BASE_REG_set(BASE_REG_get() + offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strb_post_down (void)
{
	u32 offset = DP_REG_OPERAND_IMM();
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get(), (u8)DEST_REG_get(), arm);
	BASE_REG_set(BASE_REG_get() - offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strb_imm_pre_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get() + offset, (u8)DEST_REG_get(), arm);
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() + offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strb_imm_pre_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get() - offset, (u8)DEST_REG_get(), arm);
	if (OPCODE&0x200000)
		BASE_REG_set(BASE_REG_get() - offset);	
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strb_imm_post_up (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get(), (u8)DEST_REG_get(), arm);
	BASE_REG_set(BASE_REG_get() + offset);
	
	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strb_imm_post_down (void)
{
	u32 offset = (OPCODE&0xFFF);
	INSTRUCTION_TRACE();

	write_byte (BASE_REG_get(), (u8)DEST_REG_get(), arm);
	BASE_REG_set(BASE_REG_get() - offset);
	
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
		DEST_REG_set ((u32)read_hword (BASE_REG_get() + offset, arm));
		if (OPCODE&0x00200000)
			BASE_REG_set(BASE_REG_get() + offset);	
	}
	else
	{
		DEST_REG_set ((u32)read_hword (BASE_REG_get() - offset, arm));
		if (OPCODE&0x00200000)
			BASE_REG_set(BASE_REG_get() - offset);	
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrh_post (void)
{
	u32 offset = get_ARM_reg(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	DEST_REG_set (read_hword (BASE_REG_get(), arm));

	if (OPCODE&0x800000)
		BASE_REG_set(BASE_REG_get() + offset);
	else
		BASE_REG_set(BASE_REG_get() - offset);

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
			DEST_REG_set ((s32)read_byte (BASE_REG_get() + offset, arm));
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get() + offset);	
		}
		else
		{
			DEST_REG_set ((s32)read_byte (BASE_REG_get() - offset, arm));
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get() - offset);	
		}
	}
	else
	{
		DEST_REG_set ((s32)read_byte (BASE_REG_get(), arm));
		if (OPCODE&0x800000)
			BASE_REG_set(BASE_REG_get() + offset);
		else
			BASE_REG_set(BASE_REG_get() - offset);
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
			temp = read_hword (BASE_REG_get() + offset, arm);
			
			if (temp&0x8000)
				DEST_REG_set (0 - temp);
			else
				DEST_REG_set (temp);
			
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get() + offset);	
		}
		else
		{
			temp = read_hword (BASE_REG_get() - offset, arm);
			
			if (temp&0x8000)
				DEST_REG_set (0 - temp);
			else
				DEST_REG_set (temp);

			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get() - offset);	
		}
	}
	else
	{
		temp = read_hword (BASE_REG_get(), arm);

		if (temp&0x8000)
			DEST_REG_set (0 - temp);
		else
			DEST_REG_set (temp);

		if (OPCODE&0x800000)
			BASE_REG_set(BASE_REG_get() + offset);
		else
			BASE_REG_set(BASE_REG_get() - offset);
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
		write_hword (BASE_REG_get()+offset, (u16)DEST_REG_get(), arm);
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get() + offset);	
	}
	else
	{
		write_hword (BASE_REG_get()-offset, (u16)DEST_REG_get(), arm);
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get() - offset);	
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strh_post (void)
{
	u32 offset = get_ARM_reg(OPCODE&0xF);
	INSTRUCTION_TRACE();


	write_hword (BASE_REG_get(), (u16)DEST_REG_get(), arm);

	if (OPCODE&0x800000)
		BASE_REG_set(BASE_REG_get() + offset);
	else
		BASE_REG_set(BASE_REG_get() - offset);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrh_imm_pre (void)
{
	u32 offset = ((OPCODE&0xF00)>>4)|(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	if (OPCODE&0x800000)
	{
		DEST_REG_set (read_hword (BASE_REG_get() + offset, arm));
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get() + offset);
	}
	else
	{
		DEST_REG_set (read_hword (BASE_REG_get() - offset, arm));
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get() - offset);
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrh_imm_post (void)
{
	u32 offset = ((OPCODE&0xF00)>>4)|(OPCODE&0xF);
	INSTRUCTION_TRACE();

	
	DEST_REG_set (read_hword (BASE_REG_get(), arm));
	if (OPCODE&0x800000)
		BASE_REG_set(BASE_REG_get() + offset);
	else
		BASE_REG_set(BASE_REG_get() - offset);

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
			DEST_REG_set ((s32)read_byte (BASE_REG_get() + offset, arm));
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get() + offset);
		}
		else
		{
			DEST_REG_set ((s32)read_byte (BASE_REG_get() - offset, arm));
			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get() - offset);
		}
	}
	else
	{
		DEST_REG_set ((s32)read_byte (BASE_REG_get(), arm));
		if (OPCODE&0x800000)
			BASE_REG_set(BASE_REG_get() + offset);
		else
			BASE_REG_set(BASE_REG_get() - offset);
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
			temp = read_hword (BASE_REG_get() + offset, arm);
			
			if (temp&0x8000)
				DEST_REG_set (0 - temp);
			else
				DEST_REG_set (temp);

			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get() + offset);
		}
		else
		{
			temp = read_hword (BASE_REG_get() - offset, arm);

			if (temp&0x8000)
				DEST_REG_set (0 - temp);
			else
				DEST_REG_set (temp);

			if (OPCODE&0x200000)
				BASE_REG_set(BASE_REG_get() - offset);
		}
	}
	else
	{
		temp = read_hword (BASE_REG_get(), arm);

		if (temp&0x8000)
			DEST_REG_set (0 - temp);
		else
			DEST_REG_set (temp);

		if (OPCODE&0x800000)
			BASE_REG_set(BASE_REG_get() + offset);
		else
			BASE_REG_set(BASE_REG_get() - offset);
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
		write_hword (BASE_REG_get()+offset, (u16)DEST_REG_get(), arm);
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get() + offset);	
	}
	else
	{
		write_hword (BASE_REG_get()-offset, (u16)DEST_REG_get(), arm);
		if (OPCODE&0x200000)
			BASE_REG_set(BASE_REG_get() - offset);	
	}

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_strh_imm_post (void)
{
	u32 offset = ((OPCODE&0xF00)>>4)|(OPCODE&0xF);
	INSTRUCTION_TRACE();

	write_hword (BASE_REG_get(), (u16)DEST_REG_get(), arm);

	if (OPCODE&0x800000)
		BASE_REG_set(BASE_REG_get() + offset);
	else
		BASE_REG_set(BASE_REG_get() - offset);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrb (void)
{
	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;
	boolean S_bit = (OPCODE&0x00000040)?TRUE:FALSE;

	u32 offset = BASE_REG_get();

	int dir_val = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
    {
		dir_val = ((unsigned char)((OPCODE&0xF00)>>4)|(OPCODE&0xF));
    }
	else
    {
		dir_val = OP_REG_get();
    }

	// Up
	if (U_bit)
    {
		dir_val = dir_val;
    }
	else
    {
		dir_val = -dir_val;
    }

	// Pre-indexed / offset addressing
	if ( P_bit )
    {
		offset += dir_val;
    }

	if (S_bit)
    {
		DEST_REG_set ( ((signed char)read_byte ( offset, arm )) );
    }
	else
    {
		DEST_REG_set ( read_byte ( offset, arm ) );
    }

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
	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 offset = BASE_REG_get();

	int dir_val = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
    {
		dir_val = ((unsigned char)((OPCODE&0xF00)>>4)|(OPCODE&0xF));
    }
	else
    {
		dir_val = OP_REG_get();
    }

	// Up
	if (U_bit)
    {
        dir_val = dir_val;
    }
	else
    {
		dir_val = -dir_val;
    }

	// Pre-indexed / offset addressing
	if ( P_bit )
    {
		offset += dir_val;
    }

	write_byte ( offset, (u8)DEST_REG_get(), arm );

	// Post-indexed
	if ( !P_bit )
    {
		offset += dir_val;
    }

	if ( W_bit || !P_bit )
    {
		BASE_REG_set ( offset );
    }

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrh (void)
{
	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;
	boolean S_bit = (OPCODE&0x00000040)?TRUE:FALSE;

	u32 offset = BASE_REG_get();

	int dir_val = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
    {
		dir_val = ((unsigned char)((OPCODE&0xF00)>>4)|(OPCODE&0xF));
    }
	else
    {
		dir_val = OP_REG_get();
    }

	// Up
	if (U_bit)
    {
		dir_val = dir_val;
    }
	else
    {
		dir_val = -dir_val;
    }

	// Pre-indexed / offset addressing
	if ( P_bit )
    {
		offset += dir_val;
    }

	if (S_bit)
    {
		DEST_REG_set ( ((signed short)read_hword ( offset, arm )) );
    }
	else
    {
		DEST_REG_set ( read_hword ( offset, arm ) );
    }

	// Post-indexed
	if ( !P_bit )
    {
		offset += dir_val;
    }

	if ( W_bit || !P_bit )
    {
		BASE_REG_set ( offset );
    }

	instr_advance();
	return ARMULATE_EXECUTED;
}


int ins_strh (void)
{
	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 offset = BASE_REG_get();

	int dir_val = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
    {
		dir_val = ((unsigned char)((OPCODE&0xF00)>>4)|(OPCODE&0xF));
    }
	else
    {
		dir_val = OP_REG_get();
    }

	// Up
	if (U_bit)
    {
		dir_val = dir_val;
    }
	else
    {
		dir_val = -dir_val;
    }

	// Pre-indexed / offset addressing
	if ( P_bit )
    {
		offset += dir_val;
    }

    write_hword ( offset, DEST_REG_get(), arm );

	// Post-indexed
	if ( !P_bit )
    {
		offset += dir_val;
    }

	if ( W_bit || !P_bit )
    {
		BASE_REG_set ( offset );
    }

	instr_advance();
	return ARMULATE_EXECUTED;
}


int ins_stm (void)
{
	int i = 0;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean S_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 list_reg = OPCODE & 0x0000FFFF;

	int offset = BASE_REG_get();
	int dir_val = 0;
	int reg_start = 0;
	int reg_end = 15;
	int reg_step = 1;

	INSTRUCTION_TRACE();

	// Up
	if (U_bit)
	{
		dir_val = 4;
	}
	else
	{
		dir_val = -4;
	}

	for (i=0; i<16; i++)
	{
		int reg;

		if (U_bit)
		{
			reg = i;
		}
		else
		{
			reg = 15 - i;
		}

		if (list_reg&(0x1<<reg))
		{
			unsigned int val = 0;

			// Pre-indexed / offset addressing
			if (P_bit)
			{
				offset += dir_val;
			}

			if(S_bit)
			{
				/* get the register of the mode in SPSR register */
				val = get_ARM_reg_mode(reg, (arm->spsr[(arm->cpsr & 0x1F)] & 0x1F));
			}
			else
			{
				val = get_ARM_reg ( reg );
			}

			write_word ( offset, val, arm );

			// Post-indexed
			if ( !P_bit )
			{
				offset += dir_val;
			}
		}
	}

	if ( W_bit )
		BASE_REG_set ( offset );

	instr_advance();
	return ARMULATE_EXECUTED;
}


int ins_ldm (void)
{
	u32 i = 0;
	u32 data = 0;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean S_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 list_reg = OPCODE & 0x0000FFFF;

	int offset = BASE_REG_get();
	int dir_val = 0;

	INSTRUCTION_TRACE();


	// Up
	if (U_bit)
	{
		dir_val = 4;
	}
	else
	{
		dir_val = -4;
	}

	for (i=0; i<16; i++)
	{
		int reg;

		if (U_bit)
		{
			reg = i;
		}
		else
		{
			reg = 15-i;
		}

		if (list_reg & (1<<reg))
		{
			// Pre-indexed / offset addressing
			if ( P_bit )
			{
				offset += dir_val;
			}

			data = read_word ( offset, arm );

			// if PC gets loaded
			if ( reg == 15 )
			{
				set_ARM_mode ( ( data & 1 )?THUMB_MODE:ARM_MODE );
				data &= ~1;
			}

			if(S_bit)
			{
				/* set the register of the mode in SPSR register */
				set_ARM_reg_mode ( reg, data, (arm->spsr[(arm->cpsr & 0x1F)] & 0x1F) );
				if ( reg == 15 )
				{
					arm->pc_changed = 1;
				}
			}
			else
			{
				set_ARM_reg ( reg, data );
			}

			// Post-indexed
			if ( !P_bit )
			{
				offset += dir_val;
			}
		}
	}

	/* finally update base reg (if S was set, this is in old mode) */
	if ( W_bit )
	{
		BASE_REG_set ( offset );
	}

	/* when PC is restored, restore CPSR too */
	if(S_bit && (list_reg & (1<<15)))
	{
		arm->cpsr = arm->spsr[arm->cpsr & 0x1F];		
	}

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_swi (void)
{
	INSTRUCTION_TRACE();

	swi ();

	return ARMULATE_EXECUTED;
}

int ins_swp (void)
{
	u32 temp = 0;

	INSTRUCTION_TRACE();

	temp = read_word (BASE_REG_get(), arm);
	write_word (BASE_REG_get(), OP_REG_get(), arm);
	DEST_REG_set (temp);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_swpb (void)
{
	u8 temp = 0;

	INSTRUCTION_TRACE();

	temp = read_byte (BASE_REG_get(), arm);
	write_byte (BASE_REG_get(), (u8)OP_REG_get(), arm);
	DEST_REG_set (temp);

	instr_advance();
	return ARMULATE_EXECUTED;
}

int ins_ldrd (void)
{
	u32 temp = 0;
	u32 update_base = 0;
	u32 i = 0;
	u32 n = 0;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 base_reg = (OPCODE>>16)& 0xF;
	u32 dest_reg = (OPCODE>>12)& 0xF;

	char offset_val = (char)((OPCODE & 0x00000F00)>>4) | (OPCODE & 0x0000000F);
	int offset = 0;
	int dir_val  = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
    {
		offset = offset_val;
    }
	else
    {
		offset = get_ARM_reg (OPCODE & 0x0000000F);
    }

	// Up
	if (U_bit)
    {
		dir_val = 4;
    }
	else
    {
		dir_val = -4;
    }

	for (i = dest_reg; i < (dest_reg + 2); i++)
	{
		// Pre-indexed / offset addressing
		if (P_bit && W_bit)
        {
			BASE_REG_set ( BASE_REG_get() + dir_val );
        }

		if (i != base_reg)
        {
			set_ARM_reg ( i, read_word (BASE_REG_get()+offset, arm));
        }
		else
		{
			temp = read_word (BASE_REG_get()+offset, arm);
			update_base = 1;
		}

		// Post-indexed
		if ( !P_bit )
        {
			BASE_REG_set(BASE_REG_get() + dir_val);
        }
		else
        {
			offset += dir_val;
        }
	}

	if (update_base)
    {
		BASE_REG_set (temp);
    }

	instr_advance ();
	return ARMULATE_EXECUTED;
}

int ins_strd (void)
{
	u32 i = 0;

	boolean P_bit = (OPCODE&0x01000000)?TRUE:FALSE;
	boolean U_bit = (OPCODE&0x00800000)?TRUE:FALSE;
	boolean I_bit = (OPCODE&0x00400000)?TRUE:FALSE;
	boolean W_bit = (OPCODE&0x00200000)?TRUE:FALSE;

	u32 base_reg = (OPCODE>>16)& 0xF;
	u32 dest_reg = (OPCODE>>12)& 0xF;

	char offset_val = (char)((OPCODE & 0x00000F00)>>4) | (OPCODE & 0x0000000F);
	int offset = 0;
	int dir_val = 0;

	INSTRUCTION_TRACE();

	// Immediate
	if (I_bit)
    {
		offset = offset_val;
    }
	else
    {
		offset = get_ARM_reg (OPCODE & 0x0000000F);
    }

	// Up
	if (U_bit)
    {
		dir_val = 4;
    }
	else
    {
		dir_val = -4;
    }


	for (i=dest_reg; i<dest_reg+2; i++)
	{
		// Pre-indexed / offset addressing
		if (P_bit && W_bit)
        {
			BASE_REG_set ( BASE_REG_get() + dir_val );
        }

		write_word ( BASE_REG_get()+offset, get_ARM_reg ( i ), arm );

		// Post-indexed
		if ( !P_bit )
        {
			BASE_REG_set(BASE_REG_get() + dir_val);
        }
		else
        {
			offset += dir_val;
        }
	}

	instr_advance ();
	return ARMULATE_EXECUTED;
}
