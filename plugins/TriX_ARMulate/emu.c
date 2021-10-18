
#ifdef WIN32
#include <windows.h>
#include <process.h>
#include "io.h" 
#else

#include <pthread.h>
pthread_t thread_id;
unsigned int _beginthread ( void *func, int opt, void *parm )
{
	return pthread_create ( &thread_id, NULL, func, parm );
}

int Sleep ( int ms )
{
	return usleep ( 1000 * ms );
}

#endif 

#include <stdlib.h>
#include <stdio.h> 


#include "arm.h"

extern int abort_situation;
extern unsigned int async_mode;
extern unsigned int timeout_breakpoint;
extern unsigned int (*armulate_exec_callback)(unsigned int);

extern u32 armulate_cp15_cpuid;
extern u32 armulate_cp15_cache_type;
extern u32 armulate_cp15_tcm_size;
extern u32 armulate_cp15_control_reg;

extern u32 armulate_cp15_prot_reg[8];
extern u32 armulate_cp15_data_perm_old;
extern u32 armulate_cp15_instr_perm_old;
extern u32 armulate_cp15_data_perm;
extern u32 armulate_cp15_instr_perm;

extern u32 armulate_cp15_data_cachable;
extern u32 armulate_cp15_instr_cachable;
extern u32 armulate_cp15_bufferable;

u32 armulate_mcr_int_waiting = 0;
u32 armulate_mcr_int_happened = 0;

u32 task_parm[3];
u32 run_task_ctrl = 0;
u32 (*instr_exec) (void);
void (*instr_advance)(void);
void swi(void);
void irq(void);
void fiq(void);


#include "emu.h"
#include "opcodes.h"
#include "topcodes.h"

#include "tables.h"
#include "ttables.h"


int armulate_is_bp ( unsigned int address );
int armulate_is_skipped ( unsigned int address );
void skip_next (void);
u32 read_aligned_word_instruction (u32 address, ARM7TDMI *emu);
u16 read_hword_instruction (u32 address, ARM7TDMI *emu);


void backup_cpustate ()
{
	memcpy ( &arm_backup, arm, sizeof (ARM7TDMI) );
}

void restore_cpustate ()
{
	memcpy ( arm, &arm_backup, sizeof (ARM7TDMI) );
}

void fill_instruction_pipe (void)  
{
	if ( abort_situation )
		return;
	if ( !arm )
		return;

	OPCODE = read_aligned_word_instruction (arm->gp_reg_USER [15], NULL);	

	if ( abort_situation )
		abort_situation = ARMULATE_ABORT_MEMORY_EX;
	else
		arm->gp_reg_USER [15] += 8;									 			
}   

void advance_instruction_pipe (void)
{
	if ( abort_situation )
		return;
	if ( !arm )
		return;

	if ( arm->pc_changed )
	{
		arm->pc_changed = 0;
		pc_changed ();
		return;
	}

	OPCODE = read_aligned_word_instruction (arm->gp_reg_USER [15] - 4, NULL); 

	if ( abort_situation )
		abort_situation = ARMULATE_ABORT_MEMORY_EX;
	else
		arm->gp_reg_USER [15] += 4; 
}

void tfill_instruction_pipe (void)
{
	if ( abort_situation )
		return;
	if ( !arm )
		return;

	OPCODE_T = read_hword_instruction (arm->gp_reg_USER [15], NULL);

	if ( abort_situation )
		abort_situation = ARMULATE_ABORT_MEMORY_EX;
	else
		arm->gp_reg_USER [15] += 4;
}

void tadvance_instruction_pipe (void)
{
	if ( abort_situation )
		return;

	if ( arm->pc_changed )
	{
		arm->pc_changed = 0;
		pc_changed ();
		return;
	}

	OPCODE_T = read_hword_instruction (arm->gp_reg_USER [15] - 2, NULL); 

	if ( abort_situation )
		abort_situation = ARMULATE_ABORT_MEMORY_EX;
	else
		arm->gp_reg_USER [15] += 2; 
}


u32 arm_exec (void)
{
	int ret = 0;
	int pos = 0;
	u32 (*handler)() = unknown_opcode;

	if ( armulate_is_bp ( get_ARM_reg(15) - 8 ) )
	{
		return ARMULATE_BP_REACHED;
	}

	if ( armulate_is_skipped ( get_ARM_reg(15) - 8 ) )
	{
		skip_next ();
		return ARMULATE_SKIPPED;
	}

    /* backup state in case of some abort, it will get restored below */
	backup_cpustate();

	pos = OPCODE_MASK;

	if(pos > 0x1000)
	{
		return ARMULATE_INV_INSTR;
	}

	if( get_ARM_reg(15) == 0xFF1F6A08 && (get_ARM_reg(0) > get_ARM_reg(5)))
	{
		return ARMULATE_BP_REACHED;
	}

	handler = opcode_handles[pos];

	switch (CONDITION_MASK) {

		case 0:
			if (ZFLAG) 
				ret = handler();
			break;
		case 1:
			if (!ZFLAG)
				ret = handler();
			break;
		case 2:
			if (CFLAG)
				ret = handler();
			break;
		case 3:
			if (!CFLAG)
				ret = handler();
			break;
		case 4:
			if (NFLAG)				
				ret = handler();
			break;
		case 5:
			if (!NFLAG)
				ret = handler();
			break;
		case 6:
			if (VFLAG)
				ret = handler();
			break;
		case 7:
			if (!VFLAG)				
				ret = handler();
			break;
		case 8:
			if (CFLAG && !ZFLAG)
				ret = handler();
			break;
		case 9:
			if (!CFLAG || ZFLAG)
				ret = handler();
			break;
		case 10:
			if (NFLAG == VFLAG)
				ret = handler();
			break;
		case 11:
			if (NFLAG != VFLAG)
				ret = handler();
			break;
		case 12:
			if (!ZFLAG && (NFLAG == VFLAG))
				ret = handler();
			break;
		case 13:
			if (ZFLAG || (NFLAG != VFLAG))
				ret = handler();
			break;
		case 14:
				ret = handler();
			break;
		default:
			ret = opcode_handles_special[pos]();
	}

	if ( abort_situation )
	{
		restore_cpustate();
		ret = ARMULATE_EXCEPTION | abort_situation;
		abort_situation = 0;
	}

	if ( !ret )
	{
		advance_instruction_pipe();
		ret = ARMULATE_EXECUTED;
	}
	return ret;
}

u32 thumb_exec (void)
{
	int ret = 0;

	if ( armulate_is_bp ( get_ARM_reg(15) - 4 ) )
		return ARMULATE_BP_REACHED;

	if ( armulate_is_skipped ( get_ARM_reg(15) - 4 ) )
	{
		skip_next ();
		return ARMULATE_SKIPPED;
	}

	backup_cpustate();
	ret = opcode_handles_t [OPCODE_MASK_T]();

	if ( abort_situation )
	{
		restore_cpustate();
		ret = ARMULATE_EXCEPTION | abort_situation;
		abort_situation = 0;
	}

	return ret;
}

void pc_changed ()
{
	if ( !arm )
		return;

	arm->pc_changed = 0;

	if ( CPSR & T_BIT )
	{
		tfill_instruction_pipe();
		instr_exec = thumb_exec;
		instr_advance = tadvance_instruction_pipe;
	} 
	else 
	{
		fill_instruction_pipe();
		instr_exec = arm_exec;
		instr_advance = advance_instruction_pipe;
	}
}

void free_emu ( )
{
	if ( arm )
	{
		free ( arm );
	}
}

ARM7TDMI *init_emu (void)
{
	arm = malloc (sizeof(ARM7TDMI));
	if ( !arm )
		return NULL;

	memset(arm, 0, sizeof(ARM7TDMI));
	arm->cpsr = SUPERVISOR_MODE | IRQ_BIT | FIQ_BIT;

	setup_tables ();

	return arm;
}

void copy_registers()
{
	if ( !arm )
		return;
	memcpy(arm->gp_reg_SAVE, arm->gp_reg_USER, 16*sizeof(u32));

	switch(arm->cpsr & 0x1F)
	{
		case USER_MODE:
			break;
		case FIQ_MODE:
			memcpy(arm->gp_reg_SAVE+8, arm->gp_reg_FIQ, 7*sizeof(u32));
			break;
		case IRQ_MODE:
			memcpy(arm->gp_reg_SAVE+13, arm->gp_reg_IRQ, 2*sizeof(u32));
			break;
		case SUPERVISOR_MODE:
			memcpy(arm->gp_reg_SAVE+13, arm->gp_reg_SUPERVISOR, 2*sizeof(u32));
			break;
		case ABORT_MODE:
			memcpy(arm->gp_reg_SAVE+13, arm->gp_reg_ABORT, 2*sizeof(u32));
			break;
		case UNDEFINED_MODE:
			memcpy(arm->gp_reg_SAVE+13, arm->gp_reg_UNDEFINED, 2*sizeof(u32));
			break;
		case SYSTEM_MODE:
			memcpy(arm->gp_reg_SAVE+13, arm->gp_reg_SYSTEM, 2*sizeof(u32));
			break;
	}
	arm->cpsr_SAVE = arm->cpsr;
}

u32 get_ARM_reg_mode(int reg, u32 mode)
{
	if ( !arm || reg < 0 || reg > 15 )
	{
		return 0;
	}

	if (reg <= 7 || reg == 15)
	{
		return arm->gp_reg_USER[reg];
	}
	else
	{
		switch (mode)
		{
			case USER_MODE:
				return arm->gp_reg_USER[reg];

			case FIQ_MODE:
				if (reg >= 8)
					return arm->gp_reg_FIQ[reg-8];
				else
					return arm->gp_reg_USER[reg];

			case IRQ_MODE:
				if (reg == 13 || reg == 14)
					return arm->gp_reg_IRQ[reg-13];
				else
					return arm->gp_reg_USER[reg];

			case SUPERVISOR_MODE:
				if (reg == 13 || reg == 14)
					return arm->gp_reg_SUPERVISOR[reg-13];
				else
					return arm->gp_reg_USER[reg];
				break;

			case ABORT_MODE:
				if (reg == 13 || reg == 14)
					return arm->gp_reg_ABORT[reg-13];
				else
					return arm->gp_reg_USER[reg];
				break;

			case UNDEFINED_MODE:
				if (reg == 13 || reg == 14)
					return arm->gp_reg_UNDEFINED[reg-13];
				else
					return arm->gp_reg_USER[reg];
				break;

			case SYSTEM_MODE:
				if (reg == 13 || reg == 14)
					return arm->gp_reg_SYSTEM[reg-13];
				else
					return arm->gp_reg_USER[reg];
				break;

			default:
				// Undefined
				return 0xDEADBEEF;
		}
	}
}

void set_ARM_mode ( int mode )
{
	if ( !arm )
		return;

	if ( mode == THUMB_MODE )
	{
		CPSR |= T_BIT;
	}
	else 
	{
		CPSR &= ~T_BIT;
	}
}

void set_ARM_reg_mode(int reg, u32 value, u32 mode)
{
	u32 val = value;

	if ( !arm || reg < 0 || reg > 15 )
		return;

	if ( reg == 15 )
	{
		val &= ~1;  // PC never contains T bit
	}

	if (reg <= 7 || reg == 15)
	{
		arm->gp_reg_USER[reg] = val;
	}
	else
	{
		switch (mode)
		{
			case USER_MODE:
				arm->gp_reg_USER[reg] = val;
				break;

			case FIQ_MODE:
				if (reg >= 8)
					arm->gp_reg_FIQ[reg-8] = val;
				else
					arm->gp_reg_USER[reg] = val;
				break;

			case IRQ_MODE:
				if (reg == 13 || reg == 14)
					arm->gp_reg_IRQ[reg-13] = val;
				else
					arm->gp_reg_USER[reg] = val;
				break;

			case SUPERVISOR_MODE:
				if (reg == 13 || reg == 14)
					arm->gp_reg_SUPERVISOR[reg-13] = val;
				else
					arm->gp_reg_USER[reg] = val;
				break;

			case ABORT_MODE:
				if (reg == 13 || reg == 14)
					arm->gp_reg_ABORT[reg-13] = val;
				else
					arm->gp_reg_USER[reg] = val;
				break;

			case UNDEFINED_MODE:
				if (reg == 13 || reg == 14)
					arm->gp_reg_UNDEFINED[reg-13] = val;
				else
					arm->gp_reg_USER[reg] = val;
				break;

			case SYSTEM_MODE:
				if (reg == 13 || reg == 14)
					arm->gp_reg_SYSTEM[reg-13] = val;
				else
					arm->gp_reg_USER[reg] = val;
				break;

			default:
				// Undefined
				break;
		}
	}

	if ( reg == 15 )
	{
		arm->pc_changed = 1;
	}
}

void set_ARM_reg(int reg, u32 value)
{
	if(reg == 14)
	{
		armulate_lr_set(value);
	}
	set_ARM_reg_mode(reg, value, arm->cpsr & 0x1F);
}

u32 get_ARM_reg(int reg)
{
	return get_ARM_reg_mode(reg, arm->cpsr & 0x1F);
}


void setup_tables (void) 
{
	setup_handle_tables(); 
	setup_handle_tables_t();
	setup_string_tables();
}

u32 exec_step (void)
{ 
	if ( !arm )
		return 0;
	copy_registers();
	return instr_exec();
}

int get_instruction_length(int addr)
{
	u32 code = (read_hword(addr, NULL) << 16 ) | read_hword(addr+2, NULL);

	if ((code & 0xF800E800) == 0xF000E800)
	{
		return 4;
	}
	else
	{
		return 2;
	}
}

void run_until_real (u32 breakpoint, u32 *ctrl, unsigned int (*cb)(unsigned int))
{
	u32 pc, delta;
	int status = 0;
	int fiq_flag = 0;
	int irq_flag = 0;
	int fiq_wait = 0;
	int irq_wait = 0;
	if ( !arm || !ctrl )
		return;

	copy_registers();

	while (!(*ctrl)) 
	{
		int pos = 0;

		if (CPSR & THUMB_BIT)
		{
			pc = get_ARM_reg(15) - 4;
		}
		else
		{
			pc = get_ARM_reg(15) - 8;
		}

		/* call the per-instruction callback */
		if(cb)
		{
			unsigned int ret = cb(pc);

			switch(ret)
			{
				case ARMULATE_CB_NO_ACTION:
					break;
				case ARMULATE_CB_IRQ:
					irq_flag = 1;
					irq_wait = 0;
					break;
				case ARMULATE_CB_FIQ:
					fiq_flag = 1;
					fiq_wait = 0;
					break;
				default:
					*ctrl = ret;
					return; 
			}
		}

		if(fiq_flag)
		{
			/* special hack - signal CP15 code (ins_mcr) that there was an interrupt */
			armulate_mcr_int_happened = 1;

			if(!(CPSR & FIQ_BIT))
			{
				if(fiq_wait++ > 32)
				{
					fiq_wait = 0;
					fiq_flag = 0;
					fiq();
				}
			}
		}

		if(irq_flag)
		{
			/* special hack - signal CP15 code (ins_mcr) that there was an interrupt */
			armulate_mcr_int_happened = 1;

			if(!(CPSR & IRQ_BIT))
			{
				if(irq_wait++ > 32)
				{
					irq_wait = 0;
					irq_flag = 0;
					irq();
				}
			}
		}

		status = instr_exec();

		if (CPSR & THUMB_BIT)
		{
			pc = get_ARM_reg(15) - 4;
		}
		else
		{
			pc = get_ARM_reg(15) - 8;
		}

		armulate_pc_changed(pc);

		/* check if exec-until address is reached or there is a breakpoint */
		if(pc == (breakpoint & ~1) || armulate_is_bp(pc))
		{
			status = ARMULATE_BP_REACHED;
		}

		if (status == ARMULATE_BP_REACHED ) 
		{
			*ctrl = ARMULATE_BP_REACHED;
			return; 
		}

		if ( status == ARMULATE_INV_INSTR || (status & ARMULATE_EXCEPTION) )
		{
			*ctrl = status;
			return;
		}
	}
	*ctrl = 0;
}

void exec_next_real (u32 breakpoint, u32 *ctrl, unsigned int (*cb)(unsigned int))
{
	int len = 0;
	int addr = 0;
	int status = ARMULATE_EXECUTED;
	u32 last_pc = 0;
	u32 (*func)(void);

	if ( !arm || !ctrl )
		return;

	if ( CPSR & THUMB_BIT )
	{
		addr = get_ARM_reg(15) - 4;
		func = opcode_handles_t[OPCODE_MASK_T];

		if ( func == tins_bl )
		{
			len = get_instruction_length(addr);
			run_until_real( addr + len, ctrl, cb );
		} 
		else 
		{
			status = exec_step();
			if ( status == ARMULATE_INV_INSTR || (status & ARMULATE_EXCEPTION) )
			{
				*ctrl = status;
				return;
			}
		}
	}
	else
	{
		addr = get_ARM_reg(15) - 8;

		if ( (OPCODE&0xF0000000) == 0xF0000000 )
		{
			func = opcode_handles_special[OPCODE_MASK];
		}
		else
		{
			func = opcode_handles[OPCODE_MASK];
		}

		if ( func == ins_blmi || func == ins_blpl || func == ins_blx  )
		{
			run_until_real( addr + 4, ctrl, cb );
		}
		else 
		{
			status = exec_step();
			if ( status == ARMULATE_INV_INSTR || (status&ARMULATE_EXCEPTION) )
			{
				*ctrl = status;
				return;
			}
		}
	}
	*ctrl = ARMULATE_EXECUTED;
}

void run_until_stub ( void *parm )
{
	u32 *parms = parm;
	u32 breakpoint = parms[0];
	u32 *ctrl = parms[1];
	unsigned int (*cb)(unsigned int) = parms[2];

	run_until_real (breakpoint, ctrl, cb);
}


void exec_next_stub ( void *parm )
{
	u32 *parms = parm;
	u32 breakpoint = parms[0];
	u32 *ctrl = parms[1];
	unsigned int (*cb)(unsigned int) = parms[2];

	exec_next_real (breakpoint, ctrl, cb);
}


u32 run_until (u32 breakpoint)
{
	unsigned int tries = 0;

	if ( !arm )
		return 0;

	run_task_ctrl = 0;

	task_parm[0] = breakpoint;
	task_parm[1] = (u32)&run_task_ctrl;
	task_parm[2] = (u32)armulate_exec_callback;

	_beginthread ( run_until_stub, 0, &task_parm );

	if(async_mode)
	{
		return ARMULATE_BP_REACHED;
	}

	while ( !run_task_ctrl && (tries++ < timeout_breakpoint) && !armulate_aborted() )
	{
		Sleep ( 10 );
	}

	if ( run_task_ctrl )
		return run_task_ctrl;

	run_task_ctrl = ARMULATE_STOP_REQUEST;
	tries = 50;
	while ( run_task_ctrl != 0 && tries-- )
		Sleep ( 100 );

	if ( run_task_ctrl )
		return ARMULATE_DEADLOCK;

	return ARMULATE_TIMED_OUT;
}

u32 exec_next ( void )
{
	unsigned int tries = 0;

	if ( !arm )
		return 0;

	run_task_ctrl = 0;

	task_parm[0] = 0;
	task_parm[1] = (u32)&run_task_ctrl;
	task_parm[2] = (u32)armulate_exec_callback;

	_beginthread ( exec_next_stub, 0, &task_parm );

	if(async_mode)
	{
		return ARMULATE_BP_REACHED;
	}

	while ( !run_task_ctrl && (tries++ < timeout_breakpoint) && !armulate_aborted() )
	{
		Sleep ( 10 );
	}

	if ( run_task_ctrl )
		return run_task_ctrl;

	run_task_ctrl = ARMULATE_STOP_REQUEST;
	tries = 50;
	while ( run_task_ctrl != 0 && tries-- )
		Sleep ( 100 );

	if ( run_task_ctrl )
		return ARMULATE_DEADLOCK;

	return ARMULATE_TIMED_OUT;
}

void skip_next (void)
{
	u32 (*func)(void);
	if ( !arm )
		return;

	if ( CPSR & THUMB_BIT )
	{
		func = opcode_handles_t[OPCODE_MASK_T];
		if (func == tins_bl)
		{
			instr_advance();
		}
	}
	instr_advance ();
}

void fiq(void)
{
	if ( !arm )
		return;

	arm->spsr[FIQ_MODE] = arm->cpsr;
	arm->gp_reg_FIQ[6] = get_ARM_reg(15);

	if ( CPSR & THUMB_BIT )
	{
		arm->gp_reg_FIQ[6] = (get_ARM_reg(15) - 2) | 1;
	}
	else
	{
		arm->gp_reg_FIQ[6] = get_ARM_reg(15) - 4;
	}

	arm->cpsr = FIQ_MODE | IRQ_BIT | FIQ_BIT;

	/* if alternate vectors are selected, use appropriate vector */
	if(armulate_cp15_control_reg & (1<<13))
	{
		set_ARM_reg(15, 0xFFFF001C);
	}
	else
	{
		set_ARM_reg(15, 0x1C);
	}

	pc_changed ();
}

void irq(void)
{
	if ( !arm )
		return;

	armulate_irq();

	arm->spsr[IRQ_MODE] = arm->cpsr;

	if ( CPSR & THUMB_BIT )
	{
		arm->gp_reg_IRQ[1] = (get_ARM_reg(15) - 2) | 1;
	}
	else
	{
		arm->gp_reg_IRQ[1] = get_ARM_reg(15) - 4;
	}

	arm->cpsr = IRQ_MODE | IRQ_BIT;

	/* if alternate vectors are selected, use appropriate vector */
	if(armulate_cp15_control_reg & (1<<13))
	{
		set_ARM_reg(15, 0xFFFF0018);
	}
	else
	{
		set_ARM_reg(15, 0x18);
	}

	pc_changed ();
}

void swi(void)
{
	if ( !arm )
		return;

	arm->spsr[SUPERVISOR_MODE] = arm->cpsr;
	if ( CPSR & THUMB_BIT )
		arm->gp_reg_SUPERVISOR[1] = (get_ARM_reg(15) - 2) | 1;
	else
		arm->gp_reg_SUPERVISOR[1] = get_ARM_reg(15) - 4;

	arm->cpsr = SUPERVISOR_MODE | IRQ_BIT;

	/* if alternate vectors are selected, use appropriate vector */
	if(armulate_cp15_control_reg & (1<<13))
	{
		set_ARM_reg(15, 0xFFFF0008);
	}
	else
	{
		set_ARM_reg(15, 0x08);
	}

	pc_changed ();
}

int is_thumb_state()
{
	if ( arm && (CPSR & THUMB_BIT) )
		return 1;
	return 0;
}

u32 get_reg (u32 i)                
{ 
	return get_ARM_reg(i);
}

void set_reg (u32 index, u32 value)
{ 
	set_ARM_reg ( index, value);
}

u32 get_cpsr (void)					 
{ 
	if ( !arm )
		return 0xDEADBEEF;

	return arm->cpsr;
}

void set_cpsr (u32 value)			 
{ 
	if ( !arm )
		return;

	arm->cpsr = value;
}

u32 get_instruction_pipe (u32 num)
{
	if ( !arm )
		return 0xDEADBEEF;
	return arm->op;
}
