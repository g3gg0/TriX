#ifndef __TRIX_ARMULATE_C__
#define __TRIX_ARMULATE_C__

/*! \file armulate.c
*/



#ifdef WIN32
#include <windows.h>
#endif

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"

#include "trixplug.h"
#include "arm.h"
#include "emu.h"
#include "armulate.h"

#include "mem.h"

ARM7TDMI *emustate = NULL;
t_workspace *current_ws = NULL;


breakpoint_t breakpoints[ARMULATE_BP_COUNT];

unsigned int pedantic = 0;
unsigned int write_verbosity = 1;
unsigned int read_verbosity = 1;
unsigned int exec_verbosity = 1;
unsigned int timeout_breakpoint = 500;
unsigned int instruction_trace = 0;

unsigned int async_mode = 0;

unsigned int abort_situation = 0;
unsigned int abort_address = 0;
unsigned int abort_memory = 0;
unsigned int abort_value = 0;
unsigned int abort_width = 0;
unsigned int abort_override = 0;

unsigned int program_counter = 0;
unsigned int (*armulate_exec_callback)(unsigned int) = 0;

extern int gdb_port;
extern unsigned char *armulate_gdb_remote;
extern unsigned char *armulate_gdb_status;
extern unsigned int armulate_gdb_log_tx;
extern unsigned int armulate_gdb_log_rx;


u32 armulate_cp15_cpuid = 0x41059461;
u32 armulate_cp15_cache_type = (7<<25) | (1<<24) | (4<<18) | (4<<15) | (2<<12) | (4<<6) | (4<<3) | (2<<0);
u32 armulate_cp15_tcm_size = (3<<18) | (3<<6) ;
u32 armulate_cp15_control_reg = 0x0;
u32 armulate_cp15_data_cachable = 0;
u32 armulate_cp15_instr_cachable = 0;
u32 armulate_cp15_bufferable = 0;
u32 armulate_cp15_data_perm_old = 0x0;
u32 armulate_cp15_instr_perm_old = 0x0;
u32 armulate_cp15_data_perm = 0x0;
u32 armulate_cp15_instr_perm = 0x0;
u32 armulate_cp15_prot_reg[8] = {0};


extern int run_task_ctrl;

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"


const char *ret_reasons[] = {
	"NO_ERR",
	"EXECUTED",
	"BP_REACHED",
	"SKIPPED",
	"INV_INSTR",
	"STOP_REQUEST",
	"TIMED_OUT",
	"DEADLOCK",
	"GENERAL EXCEPTION",
	"MEMORY READ EXCEPTION",
	"MEMORY WRITE EXCEPTION",
	"MEMORY EXEC EXCEPTION",
	"INVALID INSTRUCTION",
	"INVALID MODE"
};

OPT_BEGIN
	OPTION( OPT_BOOL, "Pedantic behaviour on memory access (throw exception)", "pedantic", &pedantic )
	OPTION( OPT_INT,  "Current exception state", "exception", &abort_situation )
	OPTION( OPT_INT,  "Memory Write verbosity (0=none, 1=on error, 2=always)", "write_verbosity", &write_verbosity )
	OPTION( OPT_INT,  "Memory Read verbosity (0=none, 1=on error, 2=always)", "read_verbosity", &read_verbosity )
	OPTION( OPT_INT,  "Memory Execute verbosity (0=none, 1=on error, 2=always)", "exec_verbosity", &exec_verbosity )
	OPTION( OPT_BOOL, "Instruction execution tracking", "instruction_trace", &instruction_trace )
	OPTION( OPT_INT,  "Timeout value for 'run until' functions. 10ms steps", "timeout_breakpoint", &timeout_breakpoint )
	OPTION( OPT_INT,  "(internal) Run task control", "run_task_ctrl", &run_task_ctrl )
	OPTION( OPT_BOOL, "(experimental) Run emulator in background (run_until only)", "async_mode", &async_mode )
	OPTION( OPT_BOOL, "GDB Stub: Port", "gdb.port", &gdb_port )
	OPTION( OPT_STR,  "GDB Stub: Status", "gdb.status", &armulate_gdb_status )
	OPTION( OPT_STR,  "GDB Stub: Remote", "gdb.remote", &armulate_gdb_remote )
	OPTION( OPT_BOOL, "GDB Stub: log TX packets", "gdb.log_tx", &armulate_gdb_log_tx )
	OPTION( OPT_INT,  "GDB Stub: log RX packets (1=trace except memory reads, 2=trace all)", "gdb.log_rx", &armulate_gdb_log_rx )

    OPTION( OPT_HEX,  "CP15: Register 0, {0,3-7}: ID Code Register", "cp15.cpuid", &armulate_cp15_cpuid )
    OPTION( OPT_HEX,  "CP15: Register 0, 1: Cache Type Register", "cp15.cache_type", &armulate_cp15_cache_type )
    OPTION( OPT_HEX,  "CP15: Register 0, 2: TCM Size Register", "cp15.tcm_size", &armulate_cp15_tcm_size )
    OPTION( OPT_HEX,  "CP15: Register 1: Control Register", "cp15.control_reg", &armulate_cp15_control_reg )
    OPTION( OPT_HEX,  "CP15: Register 2, 0: Cache Configuration Register (data)", "cp15.data_cachable", &armulate_cp15_data_cachable )
    OPTION( OPT_HEX,  "CP15: Register 2, 1: Cache Configuration Register (instruction)", "cp15.instr_cachable", &armulate_cp15_instr_cachable )
    OPTION( OPT_HEX,  "CP15: Register 3: Write Buffer Control Register", "cp15.bufferable", &armulate_cp15_bufferable )
    OPTION( OPT_HEX,  "CP15: Register 5, 0: Access Permission Register (data) old", "cp15.data_perm", &armulate_cp15_data_perm_old )
    OPTION( OPT_HEX,  "CP15: Register 5, 1: Access Permission Register (instruction) old", "cp15.instr_perm", &armulate_cp15_instr_perm_old )
    OPTION( OPT_HEX,  "CP15: Register 5, 2: Access Permission Register (data)", "cp15.data_perm", &armulate_cp15_data_perm )
    OPTION( OPT_HEX,  "CP15: Register 5, 3: Access Permission Register (instruction)", "cp15.instr_perm", &armulate_cp15_instr_perm )
    OPTION( OPT_HEX,  "CP15: Register 6, 0: Protection Region Register 0", "cp15.prot_reg[0]", &armulate_cp15_prot_reg[0] )
	OPTION( OPT_HEX,  "CP15: Register 6, 1: Protection Region Register 1", "cp15.prot_reg[1]", &armulate_cp15_prot_reg[1] )
	OPTION( OPT_HEX,  "CP15: Register 6, 2: Protection Region Register 2", "cp15.prot_reg[2]", &armulate_cp15_prot_reg[2] )
	OPTION( OPT_HEX,  "CP15: Register 6, 3: Protection Region Register 3", "cp15.prot_reg[3]", &armulate_cp15_prot_reg[3] )
	OPTION( OPT_HEX,  "CP15: Register 6, 4: Protection Region Register 4", "cp15.prot_reg[4]", &armulate_cp15_prot_reg[4] )
	OPTION( OPT_HEX,  "CP15: Register 6, 5: Protection Region Register 5", "cp15.prot_reg[5]", &armulate_cp15_prot_reg[5] )
	OPTION( OPT_HEX,  "CP15: Register 6, 6: Protection Region Register 6", "cp15.prot_reg[6]", &armulate_cp15_prot_reg[6] )
	OPTION( OPT_HEX,  "CP15: Register 6, 7: Protection Region Register 7", "cp15.prot_reg[7]", &armulate_cp15_prot_reg[7] )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "ARMulate" )
	PLUGIN_INIT ( armulate_init )
	PLUGIN_CLEANUP ( armulate_cleanup )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned int armulate_init ( )
{
	int pos = 0;

	REGISTER_HEADER ( "trix_armulate", ARMULATE_HEADERS );
	ARMULATE_SYMBOLS

	emustate = init_emu();

    for(pos = 0; pos < ARMULATE_BP_COUNT; pos++)
	{
		breakpoints[pos].address = E_FAIL;
		breakpoints[pos].flags = 0;
	}

	printf ( "ARMulate v1.0 Plugin Loaded" );
	return E_OK;
}

unsigned int armulate_cleanup ( )
{
	if(emustate)
	{
		free_emu(emustate);
		emustate = NULL;
	}
	options_delete_option("plugins.ARMulate.pc");
	return E_OK;
}

unsigned int armulate_opt_pc (unsigned int write, unsigned int value)
{
	return armulate_get_pc();
}

unsigned int gdb_listen ( );

unsigned int armulate_gdb_listen ( )
{
	return gdb_listen();
}

unsigned int armulate_setup ( t_workspace *ws, unsigned int start_address )
{
	if(emustate)
	{
		free_emu(emustate);
		emustate = NULL;
	}

	emustate = init_emu();
	emustate->memint = ws;


	options_delete_option("plugins.ARMulate.pc");
	options_add_option ( OPT_HEXCB, "plugins.ARMulate.pc", "Current ARMulate Program Counter (PC)", &armulate_opt_pc );
	
	armulate_set_pc ( start_address );

	return E_OK;
}

unsigned int armulate_run_until ( unsigned int address )
{
	return run_until ( address );
}

#define LR_HIST_SIZE 8192
unsigned int armulate_lr_hist[LR_HIST_SIZE];
unsigned int armulate_lr_hist_pos = 0;

void armulate_pc_changed(unsigned int address)
{
	unsigned int pos = (armulate_lr_hist_pos + LR_HIST_SIZE - 1) % LR_HIST_SIZE;

	/* now executing last LR address? */
	if(armulate_lr_hist[pos] == address)
	{
		armulate_lr_hist_pos = pos;
	}
}

void armulate_lr_set(unsigned int address)
{
	armulate_lr_hist[armulate_lr_hist_pos++] = address;
	armulate_lr_hist_pos %= LR_HIST_SIZE;
}

void armulate_lr_dump(unsigned int count)
{
	int i = 0;

	printf("[ARMULATE] BL Trace PC: 0x%08X <- ", armulate_get_pc());
	for(i = 0; i < count; i++)
	{
		unsigned int pos = (armulate_lr_hist_pos + LR_HIST_SIZE - i - 1) % LR_HIST_SIZE;

		printf("0x%08X <- ", armulate_lr_hist[pos] - 4);
	}
	printf("\r\n");
}

int armulate_irq()
{
	int reg = 0;
	return 0;
	printf ( "[ARMULATE] Interrupt. old regs: ");

	printf ( "CPSR: 0x%08X ", get_cpsr());
	for(reg = 0; reg < 0x10; reg++)
	{
		printf ( "R%i: 0x%08X ", reg, get_ARM_reg(reg));
	}
	printf ( "\r\n");

	return 0;
}


u8 read_byte (u32 address, ARM7TDMI *emu)
{
	u8 ret = 0;

    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if(armulate_is_bp_flag(address, ARMULATE_BP_READ))
	{
		abort_situation = ARMULATE_BP_REACHED;
		return 0;
	}

    if ( pedantic && !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X   BYTE <- 0x%08X - read %02X\n", armulate_get_pc (), address, abort_value );
			return abort_value;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = 0;
			abort_width = 1;
			abort_situation = ARMULATE_ABORT_MEMORY_RD;
		}
	}

	ret = v_get_b ( emu->memint, address );

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X   0x%02X  <- 0x%08X\n", armulate_get_pc (), ret, address );
	}

	return ret;
}


u16 read_hword (u32 address, ARM7TDMI *emu)
{
	u16 ret = 0;

    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if(armulate_is_bp_flag(address, ARMULATE_BP_READ))
	{
		abort_situation = ARMULATE_BP_REACHED;
		return 0;
	}

	if ( pedantic && !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X  HWORD <- 0x%08X - read %04X\n", armulate_get_pc (), address, abort_value );
			return abort_value;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = 0;
			abort_width = 2;
			abort_situation = ARMULATE_ABORT_MEMORY_RD;
		}
	}

	ret = v_get_h ( emu->memint, address );

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X  0x%04X <- 0x%08X\n", armulate_get_pc (), ret, address );
	}

	return ret;
}


u16 read_hword_instruction (u32 address, ARM7TDMI *emu)
{
	u16 ret = 0;

    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if ( !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X  HWORD <- 0x%08X - read %04X\n", armulate_get_pc (), address, abort_value );
			return abort_value;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = 0;
			abort_width = 2;
			abort_situation = ARMULATE_ABORT_MEMORY_RD;
		}
	}

	ret = v_get_h ( emu->memint, address );

	if ( exec_verbosity == ARMULATE_VERB_ALWAYS ||
		( exec_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X  0x%04X <- 0x%08X (EXEC, called from 0x%08X)\n", armulate_get_pc (), ret, address, (armulate_get_reg ( 14 ) & ~1) - 4 );
	}

	return ret;
}


u32 read_word (u32 address, ARM7TDMI *emu)
{
	u32 ret = 0;

    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if(armulate_is_bp_flag(address, ARMULATE_BP_READ))
	{
		abort_situation = ARMULATE_BP_REACHED;
		return 0;
	}

	if ( pedantic && !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X   WORD <- 0x%08X - read %08X\n", armulate_get_pc (), address, abort_value );
			return abort_value;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = 0;
			abort_width = 4;
			abort_situation = ARMULATE_ABORT_MEMORY_RD;
		}
	}

	ret = v_get_w ( emu->memint, address );

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X   0x%08X  <- 0x%08X\n", armulate_get_pc (), ret, address );
	}

	return ret;
}

u16 read_aligned_hword (u32 address, ARM7TDMI *emu)
{
	u16 ret = 0;

    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if(armulate_is_bp_flag(address, ARMULATE_BP_READ))
	{
		abort_situation = ARMULATE_BP_REACHED;
		return 0;
	}

	if ( pedantic && !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X  HWORD <- 0x%08X - read %04X\n", armulate_get_pc (), address, abort_value );
			return abort_value;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = 0;
			abort_width = 2;
			abort_situation = ARMULATE_ABORT_MEMORY_RD;
		}
	}

	ret = v_get_h ( emu->memint, address & ~3 );

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X  0x%04X <- 0x%08X\n", armulate_get_pc (), ret, address );
	}

	return ret;
}

u32 read_aligned_word (u32 address, ARM7TDMI *emu)
{
	u32 ret = 0;

    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if(armulate_is_bp_flag(address, ARMULATE_BP_READ))
	{
		abort_situation = ARMULATE_BP_REACHED;
		return 0;
	}

	if ( pedantic && !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X   WORD <- 0x%08X - read %08X\n", armulate_get_pc (), address, abort_value );
			return abort_value;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = 0;
			abort_width = 4;
			abort_situation = ARMULATE_ABORT_MEMORY_RD;
		}
	}

	ret = v_get_w ( emu->memint, address & ~3 );

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X   0x%08X <- 0x%08X\n", armulate_get_pc (), ret, address );
	}

	return ret;
}

u32 read_aligned_word_instruction (u32 address, ARM7TDMI *emu)
{
	u32 ret = 0;

    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if ( !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X  WORD <- 0x%08X - read %08X\n", armulate_get_pc (), address, abort_value );
			return abort_value;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = 0;
			abort_width = 2;
			abort_situation = ARMULATE_ABORT_MEMORY_RD;
		}
	}

	ret = v_get_w ( emu->memint, address & ~3 );

	if ( exec_verbosity == ARMULATE_VERB_ALWAYS ||
		( exec_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X   0x%08X <- 0x%08X (EXEC)\n", armulate_get_pc (), ret, address );
	}

	return ret;
}


void write_byte (u32 address, u8 data, ARM7TDMI *emu)
{
    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if(armulate_is_bp_flag(address, ARMULATE_BP_WRITE))
	{
		abort_situation = ARMULATE_BP_REACHED;
		return;
	}

	if ( pedantic && !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X   0x%02X -> 0x%08X\n", armulate_get_pc (), data, address );
			return;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = data;
			abort_width = 1;
			abort_situation = ARMULATE_ABORT_MEMORY_WR;
		}
	}

	if ( write_verbosity == ARMULATE_VERB_ALWAYS ||
		( write_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X   0x%02X -> 0x%08X\n", armulate_get_pc (), data, address );
	}

	v_set_b ( emu->memint, address, data );
}


void write_hword (u32 address, u16 data, ARM7TDMI *emu)
{
    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if(armulate_is_bp_flag(address, ARMULATE_BP_WRITE))
	{
		abort_situation = ARMULATE_BP_REACHED;
		return;
	}

	if ( pedantic && !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X   0x%04X -> 0x%08X\n", armulate_get_pc (), data, address );
			return;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = data;
			abort_width = 2;
			abort_situation = ARMULATE_ABORT_MEMORY_WR;
		}
	}

	if ( write_verbosity == ARMULATE_VERB_ALWAYS ||
		( write_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X   0x%04X -> 0x%08X\n", armulate_get_pc (), data, address );
	}

	v_set_h ( emu->memint, address, data );
}


void write_word (u32 address, u32 data, ARM7TDMI *emu)
{
    /* if no context given use the global */
    if(emu == NULL)
    {
        emu = emustate;
    }

	if(armulate_is_bp_flag(address, ARMULATE_BP_WRITE))
	{
		abort_situation = ARMULATE_BP_REACHED;
		return;
	}

	if ( pedantic && !v_valid ( emu->memint, address ) )
	{
		if ( abort_override && abort_memory == address )
		{
			abort_override = 0;
			printf ( "ARMULATE: OVERRIDE at 0x%08X   0x%08X -> 0x%08X\n", armulate_get_pc (), data, address );
			return;
		}
		else
		{
			abort_address = armulate_get_pc ();
			abort_memory = address;
			abort_value = data;
			abort_width = 4;
			abort_situation = ARMULATE_ABORT_MEMORY_WR;
		}
	}

	if ( write_verbosity == ARMULATE_VERB_ALWAYS ||
		( write_verbosity == ARMULATE_VERB_ONERR && !v_valid ( emu->memint, address ) ) )
	{
		printf ( "ARMULATE: at 0x%08X   0x%08X -> 0x%08X\n", armulate_get_pc (), data, address );
	}

	v_set_w ( emu->memint, address, data );
}

void set_abort ( int mode )
{
	abort_address = armulate_get_pc ();
	abort_memory = 0;
	abort_value = 0;
	abort_width = 0;
	abort_situation = mode;
}

unsigned int armulate_get_pc ( void )
{
	if ( is_thumb_state () )
	{
		return get_reg ( 15 ) - 4;
	}
	return get_reg ( 15 ) - 8;
}

unsigned int armulate_set_pc ( unsigned int address )
{
	if ( address & 1 )
	{
		set_ARM_mode ( THUMB_MODE );
		set_ARM_reg ( 15, address & ~1 );
	}
	else
	{
		set_ARM_mode ( ARM_MODE );
		set_ARM_reg ( 15, address & ~3 );
	}

	pc_changed ();
	return E_OK;
}

unsigned int armulate_is_thumb ( void )
{
	if ( is_thumb_state () )
	{
		return 1;
	}

	return 0;
}

unsigned int armulate_get_reg ( unsigned int reg )
{
	return get_reg ( reg );
}

void armulate_set_reg ( unsigned int reg, unsigned int value )
{
	set_reg ( reg, value );
}

unsigned int armulate_get_cpsr (void)
{
	return get_cpsr (  );
}

void armulate_set_cpsr ( unsigned int value )
{
	set_cpsr ( value );
}

unsigned int armulate_exec_step (  )
{
	return exec_step ();
}

unsigned int armulate_exec_next ( void )
{
	return exec_next ();
}

unsigned int armulate_add_bp ( unsigned int address )
{
	int pos = 0;

	while ((breakpoints[pos].flags != 0) && (pos < (ARMULATE_BP_COUNT - 2)))
	{
		pos++;
	}

	if(pos < (ARMULATE_BP_COUNT - 1))
	{
		breakpoints[pos].address = address;
		breakpoints[pos].flags = ARMULATE_BP_EXEC;
	}

	return E_OK;
}

unsigned int armulate_set_bps ( unsigned int *bps )
{
	int pos = 0;

	if ( !bps )
		return E_FAIL;
	
	/* delete all existing breakpoints */
    for(pos = 0; pos < ARMULATE_BP_COUNT; pos++)
	{
		breakpoints[pos].address = E_FAIL;
		breakpoints[pos].flags = 0;
	}
 
	pos = 0;
	while ((bps[pos] != E_FAIL) && (pos < (ARMULATE_BP_COUNT - 1)))
	{
		breakpoints[pos].address = bps[pos];
		breakpoints[pos].flags = ARMULATE_BP_EXEC;
		pos++;
	}

	return E_OK;
}

unsigned int armulate_is_bp_flag ( unsigned int address, unsigned int flags )
{
	int pos = 0;

    for(pos = 0; pos < ARMULATE_BP_COUNT; pos++)
	{
		/* if address is E_FAIL, there is no valid entry after this one anymore. break */
		if(breakpoints[pos].address == E_FAIL)
		{
			return 0;
		}
		if (((breakpoints[pos].flags & flags) != 0) && (breakpoints[pos].address & ~1) == address )
		{
			return 1;
		}
	}

	return 0;
}

unsigned int armulate_is_bp ( unsigned int address )
{
	int pos = 0;

    for(pos = 0; pos < ARMULATE_BP_COUNT; pos++)
	{
		/* if address is E_FAIL, there is no valid entry after this one anymore. break */
		if(breakpoints[pos].address == E_FAIL)
		{
			return 0;
		}
		if ((breakpoints[pos].flags != 0) && (breakpoints[pos].address & ~1) == address )
		{
			return 1;
		}
	}

	return 0;
}

unsigned int armulate_is_skipped ( unsigned int address )
{
	return 0;
}

const char *armulate_reason ( int reason )
{
	if ( reason >= sizeof ( ret_reasons )/sizeof(ret_reasons[0]) )
	{
		return NULL;
	}

	return ret_reasons[reason];
}

unsigned int armulate_get_abort_address ( )
{
	return abort_address;
}

unsigned int armulate_get_abort_memory ( )
{
	return abort_memory;
}

unsigned int armulate_get_abort_value ( )
{
	return abort_value;
}

unsigned int armulate_get_abort_width ( )
{
	return abort_width;
}

unsigned int armulate_get_abort_override ( )
{
	return abort_override;
}

unsigned int armulate_get_abort_situation ( )
{
	return abort_situation;
}

// setters
void armulate_set_abort_address ( unsigned int val )
{
	abort_address = val;
}

void armulate_set_abort_memory ( unsigned int val )
{
	abort_memory = val;
}

void armulate_set_abort_value ( unsigned int val )
{
	abort_value = val;
}

void armulate_set_abort_width ( unsigned int val )
{
	abort_width = val;
}

void armulate_set_abort_override ( unsigned int val )
{
	abort_override = val;
}

void armulate_set_abort_situation ( unsigned int val )
{
	abort_situation = val;
}

void armulate_warn ( unsigned char *buffer )
{
	printf ( buffer );
}

void armulate_set_exec_callback (unsigned int (*cb)(unsigned int) )
{
	armulate_exec_callback = cb;
}

int armulate_aborted()
{
	return util_script_is_aborted();
}


#endif

