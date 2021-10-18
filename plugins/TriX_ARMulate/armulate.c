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
#include "armulate.h"

#include "mem.h"

t_emustate *state = NULL;
t_workspace *current_ws = NULL;
unsigned int breakpoints[8192] = { E_FAIL };

unsigned int pedantic = 0;
unsigned int write_verbosity = 0;
unsigned int read_verbosity = 0;
unsigned int exec_verbosity = 0;
unsigned int timeout_breakpoint = 500;
unsigned int instruction_trace = 0;

unsigned int abort_situation = 0;
unsigned int abort_address = 0;
unsigned int abort_memory = 0;
unsigned int abort_value = 0;
unsigned int abort_width = 0;
unsigned int abort_override = 0;


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
	OPTION( OPT_INT, "Current exception state", "exception", &abort_situation )
	OPTION( OPT_INT, "Memory Write verbosity (0=none, 1=on error, 2=always)", "write_verbosity", &write_verbosity )
	OPTION( OPT_INT, "Memory Read verbosity (0=none, 1=on error, 2=always)", "read_verbosity", &read_verbosity )
	OPTION( OPT_INT, "Memory Execute verbosity (0=none, 1=on error, 2=always)", "exec_verbosity", &exec_verbosity )
	OPTION( OPT_BOOL, "Instruction execution tracking", "instruction_trace", &instruction_trace )
	OPTION( OPT_INT, "Timeout value for 'run until' functions. 10ms steps", "timeout_breakpoint", &timeout_breakpoint )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "ARMulate" )
	PLUGIN_INIT ( armulate_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned int armulate_init ( )
{
	REGISTER_HEADER ( "trix_armulate", ARMULATE_HEADERS );
    ARMULATE_SYMBOLS

	printf ( "ARMulate v1.0 Plugin Loaded" );
    return E_OK;
}


#ifdef WIN32
typedef unsigned char  byte;
typedef unsigned short hword;
typedef unsigned long  word;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed char  s8;
typedef signed short s16;
typedef signed long  s32;
#endif

unsigned int armulate_setup ( t_workspace *ws, unsigned int start_address )
{
	state = init_emu ();

	state->memint = ws;
	set_pointers ( state );

	armulate_set_pc ( start_address );

	return E_OK;
}


unsigned int armulate_run_until ( unsigned int address )
{
	return run_breakpoint ( address );
}


u8   read_byte (u32 address, void*i)
{
	if ( pedantic && !v_valid ( i, address ) )
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

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X   BYTE  <- 0x%08X\n", armulate_get_pc (), address );

	return v_get_b ( i, address );
}


u16  read_hword (u32 address, void*i)
{
	if ( pedantic && !v_valid ( i, address ) )
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

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X  HWORD <- 0x%08X\n", armulate_get_pc (), address );

	return v_get_h ( i, address );
}


u16  read_hword_instruction (u32 address, void*i)
{
	if ( !v_valid ( i, address ) )
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

	if ( exec_verbosity == ARMULATE_VERB_ALWAYS ||
		( exec_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X  HWORD <- 0x%08X (EXEC)\n", armulate_get_pc (), address );

	return v_get_h ( i, address );
}


u32  read_word (u32 address, void*i)
{
	if ( pedantic && !v_valid ( i, address ) )
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

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X   WORD  <- 0x%08X\n", armulate_get_pc (), address );

	return v_get_w ( i, address );
}

/*

u32  read_word_instruction (u32 address, void*i)
{
	if ( !v_valid ( i, address ) )
	{
		abort_situation = ARMULATE_ABORT_MEMORY_RD;
		printf ( "ARMULATE: at 0x%08X   WORD  <- 0x%08X\n", armulate_get_pc (), address );
	}

	return v_get_w ( i, address );
}
*/

u16  read_aligned_hword (u32 address, void*i)
{
	if ( pedantic && !v_valid ( i, address ) )
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

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X  HWORD <- 0x%04X\n", armulate_get_pc (), address );

	return v_get_h ( i, address & ~3 );
}


u32  read_aligned_word (u32 address, void*i)
{
	if ( pedantic && !v_valid ( i, address ) )
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

	if ( read_verbosity == ARMULATE_VERB_ALWAYS ||
		( read_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X   WORD <- 0x%08X\n", armulate_get_pc (), address );

	return v_get_w ( i, address & ~3 );
}


u32  read_aligned_word_instruction (u32 address, void*i)
{
	if ( pedantic && !v_valid ( i, address ) )
	{
		abort_situation = ARMULATE_ABORT_MEMORY_RD;
		printf ( "ARMULATE: at 0x%08X   WORD <- 0x%08X\n", armulate_get_pc (), address );
	}

	if ( exec_verbosity == ARMULATE_VERB_ALWAYS ||
		( exec_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X   WORD <- 0x%08X (EXEC)\n", armulate_get_pc (), address );

	return v_get_w ( i, address & ~3 );
}


void write_byte (word address, byte data, void*i)
{
	if ( pedantic && !v_valid ( i, address ) )
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
		( write_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X   0x%02X -> 0x%08X\n", armulate_get_pc (), data, address );

	v_set_b ( i, address, data );
}


void write_hword (u32 address, u16 data, void*i)
{
	if ( pedantic && !v_valid ( i, address ) )
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
		( write_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X   0x%04X -> 0x%08X\n", armulate_get_pc (), data, address );

	v_set_h ( i, address, data );
}


void write_word (u32 address, u32 data, void*i)
{
	if ( pedantic && !v_valid ( i, address ) )
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
		( write_verbosity == ARMULATE_VERB_ONERR && !v_valid ( i, address ) ) )
		printf ( "ARMULATE: at 0x%08X   0x%08X -> 0x%08X\n", armulate_get_pc (), data, address );

	v_set_w ( i, address, data );
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
		return get_reg ( 15 ) - 4;
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
		return 1;

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


unsigned int armulate_set_bps ( unsigned int *address )
{
	int pos = 0;

	if ( !address )
		return E_FAIL;

	while ( v_valid ( (t_workspace*)get_memint(), address[pos]) )
	{
		breakpoints[pos] = address[pos];
		pos++;
	}
	breakpoints[pos] = E_FAIL;

	return E_OK;
}


unsigned int armulate_is_bp ( unsigned int address )
{
	int pos = 0;

	if ( !breakpoints )
		return 0;

	while ( v_valid ( (t_workspace*)get_memint(), breakpoints[pos]) )
	{
		if ( breakpoints[pos] == address )
			return 1;
		pos++;
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
		return NULL;
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



#endif

