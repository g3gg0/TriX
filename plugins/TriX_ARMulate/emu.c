
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
extern unsigned int timeout_breakpoint;

u32 (*instr_exec) (void);
void (*instr_advance)(void);
 

#include "emu.h"
#include "opcodes.h"
#include "topcodes.h"

#include "tables.h"
#include "ttables.h"


int armulate_is_bp ( unsigned int address );
int armulate_is_skipped ( unsigned int address );


void skip_next (void);

u32 arm_exec (void)
{
	int ret = 0;
	unsigned char buffer[1024];


	if ( armulate_is_bp ( get_ARM_reg(15) - 8 ) )
		return ARMULATE_BP_REACHED;

	if ( armulate_is_skipped ( get_ARM_reg(15) - 8 ) )
	{
		skip_next ();
		return ARMULATE_SKIPPED;
	}

	backup_cpustate();

	switch (CONDITION_MASK) {
	
		case 0:
				if (ZFLAG) 
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 1:
				if (!ZFLAG)
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 2:
				if (CFLAG)
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 3:
				if (!CFLAG)
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 4:
				if (NFLAG)				
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 5:
				if (!NFLAG)
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 6:
				if (VFLAG)
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 7:
				if (!VFLAG)				
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 8:
				if (CFLAG && !ZFLAG)
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 9:
				if (!CFLAG || ZFLAG)
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 10:
				if (NFLAG == VFLAG)
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 11:
				if (NFLAG != VFLAG)
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 12:
				if (!ZFLAG && (NFLAG == VFLAG))
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 13:
				if (ZFLAG || (NFLAG != VFLAG))
					ret = opcode_handles[OPCODE_MASK]();
				break;
		case 14:
				ret = opcode_handles[OPCODE_MASK]();
				break;
		default:
//				sprintf ( buffer, "0x%08X: SPECIAL INSTR\n", get_ARM_reg(15) - 4 );
//				armulate_warn ( buffer );
				ret = opcode_handles_special[OPCODE_MASK]();
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


int set_pointers ( t_emustate *state )
{
	if ( !state )
		return 1;

	arm         = state->arm;
	pDoc        = state->pDoc;
	meminterface= state->memint;

	return 0;
}

void free_emu ( t_emustate *state )
{
	if ( state )
	{
		free ( state->arm );
		free ( state );
	}
}

void clean_up (t_emustate *state )
{
}


t_emustate *init_emu (void)
{
	t_emustate *state = malloc ( sizeof (t_emustate) );
	state->arm = malloc (sizeof(ARM7TDMI));
	if ( !state->arm )
	    return NULL;

	memset(state->arm, 0, sizeof(ARM7TDMI));
	state->arm->cpsr = USER_MODE;
 
	set_pointers ( state );
	setup_tables ();

	return state;
}

void *get_memint ()
{
	return meminterface;
}

void place_emu (word pc, int is_thumb)
{
	if ( !arm )
		return;
	set_ARM_reg(15, pc);

}

void copy_registers()
{
	if ( !arm )
		return;
	memcpy(arm->gp_reg_SAVE, arm->gp_reg_USER, 16*sizeof(u32));

	switch(arm->cpsr & 0x1F){
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

u32 get_ARM_reg(int reg)
{
	if ( !arm || reg < 0 || reg > 15 )
		return 0;

	if (reg < 7)
		return arm->gp_reg_USER[reg];

	switch (arm->cpsr & 0x1F){
		case USER_MODE:
			return arm->gp_reg_USER[reg];
		case FIQ_MODE:
			if (reg <= 14)
				return arm->gp_reg_FIQ[reg-8];
			else
				return arm->gp_reg_USER[reg];
		case IRQ_MODE:
			if (13 <= reg && reg <= 14)
				return arm->gp_reg_IRQ[reg-13];
			else
				return arm->gp_reg_USER[reg];
		case SUPERVISOR_MODE:
			if (13 <= reg && reg <= 14)
				return arm->gp_reg_SUPERVISOR[reg-13];
			else
				return arm->gp_reg_USER[reg];
			break;
		case ABORT_MODE:
			if (13 <= reg && reg <= 14)
				return arm->gp_reg_ABORT[reg-13];
			else
				return arm->gp_reg_USER[reg];
			break;
		case UNDEFINED_MODE:
			if (13 <= reg && reg <= 14)
				return arm->gp_reg_UNDEFINED[reg-13];
			else
				return arm->gp_reg_USER[reg];
			break;
		case SYSTEM_MODE:
			if (13 <= reg && reg <= 14)
				return arm->gp_reg_SYSTEM[reg-13];
			else
				return arm->gp_reg_USER[reg];
			break;
		default:
			// Undefined
			return 0xDEADBEEF;
	}
}

void set_ARM_mode ( int mode )
{
    if ( !arm )
	return;
	
    if ( mode == THUMB_MODE )
	CPSR |= T_BIT;
    else 
	CPSR &= ~T_BIT;
}


void set_ARM_reg(int reg, u32 value)
{
	u32 val = value;

	if ( !arm || reg < 0 || reg > 15 )
		return;

	if (reg < 8 && reg > 0)
	{
		arm->gp_reg_USER[reg] = val;
		return;
	}

	if ( reg == 15 )
		val &= ~1;  // PC never contains T bit

	switch (arm->cpsr & 0x1F)
	{
		case USER_MODE:
			arm->gp_reg_USER[reg] = val;
			break;

		case FIQ_MODE:
			if (reg <= 14)
				arm->gp_reg_FIQ[reg-8] = val;
			else
				arm->gp_reg_USER[reg] = val;
			break;

		case IRQ_MODE:
			if (13 <= reg && reg <= 14)
				arm->gp_reg_IRQ[reg-13] = val;
			else
				arm->gp_reg_USER[reg] = val;
			break;

		case SUPERVISOR_MODE:
			if (13 <= reg && reg <= 14)
				arm->gp_reg_SUPERVISOR[reg-13] = val;
			else
				arm->gp_reg_USER[reg] = val;
			break;

		case ABORT_MODE:
			if (13 <= reg && reg <= 14)
				arm->gp_reg_ABORT[reg-13] = val;
			else
				arm->gp_reg_USER[reg] = val;
			break;

		case UNDEFINED_MODE:
			if (13 <= reg && reg <= 14)
				arm->gp_reg_UNDEFINED[reg-13] = val;
			else
				arm->gp_reg_USER[reg] = val;
			break;

		case SYSTEM_MODE:
			if (13 <= reg && reg <= 14)
				arm->gp_reg_SYSTEM[reg-13] = val;
			else
				arm->gp_reg_USER[reg] = val;
			break;

		default:
			// Undefined
			break;
	}

	if ( reg == 15 )
		arm->pc_changed = 1;

}

void dump_ARM_regs()
{
	FILE *f;
	int i;
	if ( !arm )
		return;
	
	if (!(f = fopen("regs.txt", "wt")))
		return;
	fprintf(f, "CPSR: %02X\n\n", arm->cpsr);
	fprintf(f, "User:\n");
	for (i=0; i<16; i++)
		fprintf(f, "\tR%02d: %8X\n", i, arm->gp_reg_USER[i]);

	fprintf(f, "FIQ:\n");
	for (i=0; i<7; i++)
		fprintf(f, "\tR%02d: %8X\n", i+8, arm->gp_reg_FIQ[i]);

	fprintf(f, "IRQ:\n");
	for (i=0; i<2; i++)
		fprintf(f, "\tR%02d: %8X\n", i+13, arm->gp_reg_IRQ[i]);

	fprintf(f, "SUPERVISOR:\n");
	for (i=0; i<2; i++)
		fprintf(f, "\tR%02d: %8X\n", i+13, arm->gp_reg_SUPERVISOR[i]);

	fprintf(f, "ABORT:\n");
	for (i=0; i<2; i++)
		fprintf(f, "\tR%02d: %8X\n", i+13, arm->gp_reg_ABORT[i]);

	fprintf(f, "UNDEFINED:\n");
	for (i=0; i<2; i++)
		fprintf(f, "\tR%02d: %8X\n", i+13, arm->gp_reg_UNDEFINED[i]);

	fprintf(f, "SYSTEM:\n");
	for (i=0; i<2; i++)
		fprintf(f, "\tR%02d: %8X\n", i+13, arm->gp_reg_SYSTEM[i]);

	fclose(f);
}

void setup_tables (void) 
{
	setup_handle_tables(); 
	setup_handle_tables_t();
	setup_string_tables();
}

char *get_cpu_mode_name()
{
	if ( !arm )
		return "";
	return cpu_mode_strings[arm->cpsr & 0x1F];
}

int emu_is_thumb ( void )
{
	if ( !arm )
		return 0;
	return (CPSR & THUMB_BIT);
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
	word code = (read_hword(addr, meminterface) << 16 ) | read_hword(addr+2, meminterface);
	if ((code & 0xF800E800) == 0xF000E800)
		return 4;
	else
		return 2;
}

void run_breakpoint_real (u32 breakpoint, int *ctrl)
{
	u32 pc, last_pc, delta;
	int status = 0;
	
	if ( !arm )
		return;
	if ( !ctrl )
		return;

	copy_registers();
	last_pc = get_ARM_reg(15);
	while (!*ctrl) 
	{
		status = instr_exec();

		pc = get_ARM_reg(15);
		if (CPSR & THUMB_BIT)
			delta = 4;
		else
			delta = 8;

		if ((pc-delta)==(breakpoint & ~1) || status == ARMULATE_BP_REACHED ) 
		{
			*ctrl = ARMULATE_BP_REACHED;
			return; 
		}
		if ( status == ARMULATE_INV_INSTR || (status&ARMULATE_EXCEPTION) )
		{
			*ctrl = status;
			return;
		}
		last_pc = pc;
	}
	*ctrl = 0;
}

void run_breakpoint_stub ( void *parm )
{
	u32 *p = parm;
	run_breakpoint_real ( p[0], (int*)p[1] );
}

int run_breakpoint (u32 breakpoint)
{
	int ctrl = 0;
	unsigned int tries = timeout_breakpoint;
	u32 buf[2];
	if ( !arm )
		return 0;

	copy_registers();
	buf[0] = breakpoint;
	buf[1] = (u32)&ctrl;

	_beginthread ( run_breakpoint_stub, 0, &buf );

	while ( !ctrl && tries-- )
		Sleep ( 10 );

	if ( ctrl )
		return ctrl;

	ctrl = ARMULATE_STOP_REQUEST;
	tries = 500;
	while ( ctrl != 0 && tries-- )
		Sleep ( 10 );

	if ( ctrl )
		return ARMULATE_DEADLOCK;

	return ARMULATE_TIMED_OUT;
}


void exec_next_real ( void *parm )
{
	int *ctrl = parm;
	int len, addr;
	int status = 0;
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
			run_breakpoint_real( addr + len, ctrl );
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
	else
	{
		addr = get_ARM_reg(15) - 8;

		if ( (OPCODE&0xF0000000) == 0xF0000000 )
			func = opcode_handles_special[OPCODE_MASK];
		else
			func = opcode_handles[OPCODE_MASK];

		if ( func == ins_blmi || func == ins_blpl || func == ins_blx  )
		{
			run_breakpoint_real( addr + 4, ctrl );
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

u32 exec_next ( void )
{
	int ctrl = 0;
	int tries = timeout_breakpoint;
	if ( !arm )
		return 0;

	_beginthread ( exec_next_real, 0, &ctrl );
	while ( !ctrl && tries-- )
		Sleep ( 10 );

	if ( ctrl )
		return ctrl;

	tries = timeout_breakpoint;
	while ( ctrl != 2 && tries-- )
		Sleep ( 10 );

	if ( ctrl )
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
		    instr_advance();
	}
	instr_advance ();
}

void fiq(void)
{
	if ( !arm )
		return;

	arm->spsr[FIQ_MODE] = arm->cpsr;
	arm->gp_reg_FIQ[6] = get_ARM_reg(15);

	arm->cpsr = (arm->cpsr & ~0x1F) | FIQ_MODE;

	set_ARM_mode ( ARM_MODE );
	set_ARM_reg(15, 0x1C);

	pc_changed ();
}

void irq(void)
{
	if ( !arm )
		return;

	arm->spsr[FIQ_MODE] = arm->cpsr;
	arm->gp_reg_IRQ[1] = get_ARM_reg(15);

	arm->cpsr = (arm->cpsr & ~0x1F) | IRQ_MODE;

	set_ARM_mode ( ARM_MODE );
	set_ARM_reg(15, 0x18);

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

	if (CFLAG) 
		CPSR |= C_BIT; 
	else 
		CPSR &= ~C_BIT; 

	if (VFLAG) 
		CPSR |= V_BIT; 
	else 
		CPSR &= ~V_BIT;

	if (NFLAG) 
		CPSR |= N_BIT; 
	else 
		CPSR &= ~N_BIT;

	if (ZFLAG) 
		CPSR |= Z_BIT; 
	else 
		CPSR &= ~Z_BIT;

	return arm->cpsr;
}

void set_cpsr (u32 value)			 
{ 
	if ( !arm )
		return 0xDEADBEEF;

	arm->cpsr = value;

	if (C_FLAG_SET) 
		CFLAG = 1; 
	else 
		CFLAG = 0;

	if (V_FLAG_SET) 
		VFLAG = 1; 
	else 
		VFLAG = 0; 

	if (N_FLAG_SET)
		NFLAG = 1; 
	else 
		NFLAG = 0; 

	if (Z_FLAG_SET) 
		ZFLAG = 1; 
	else 
		ZFLAG = 0; 
}

u32 get_instruction_pipe (u32 num)
{
	if ( !arm )
		return 0xDEADBEEF;
	return arm->op;
}

void set_mem32(u32 address, u32 val)
{
	write_word(address, val, meminterface);
}

void set_mem16(u32 address, u16 val)
{
	write_hword(address, val, meminterface);
}

void set_mem8(u32 address, u8 val)
{
	write_byte(address, val, meminterface);
}

u32 get_mem32(u32 address)
{
	return read_word(address, meminterface);
}

u16 get_mem16(u32 address)
{
	return read_hword(address, meminterface);
}

u8 get_mem8(u32 address)
{
	return read_byte(address, meminterface);
}

