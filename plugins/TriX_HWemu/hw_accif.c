#ifndef __TRIX_HW_ACCIF_C__
#define __TRIX_HW_ACCIF_C__



#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"

#include "mem.h"


#include "trixplug.h"

#include "hwemu.h"

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

struct trix_functions *ft;


unsigned int accif_data = 0;



unsigned int hwemu_accif_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	int okay = 0;
	int read = 1;
	int write = 0;
	unsigned int data = 0;
	unsigned char *sect = "ACCIF";
	unsigned char *desc = "";
	unsigned int offset = address - ACCIF_BASE;

	if ( type & MODE_WRITE )
	{
		read = 0;
		write = 1;
	}

	switch ( offset )
	{
		default:
			printf("Data access to ACCIF via 0x%08X\n", options_get_int("plugins.ARMulate.pc"));
			data = accif_data;
			accif_data += 0x12121212;
			okay = 1;
	}

	if ( type & MODE_WRITE )
	{
		if ( !okay )
			hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_ACCIF )
			printf ( "strh ( 0x%08X, 0x%04X ); // (%s+0x%04X %s)\n", address, value, sect, offset, desc );
	}
	else
	{
		if ( !okay )
			data = hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_ACCIF )
			printf ( "ldrh ( 0x%08X ); // -> 0x%04X (%s+0x%04X %s)\n", address, data, sect, offset, desc );
	}

	return data;
}

#endif
