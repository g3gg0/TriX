#ifndef __TRIX_HW_CBUSC_C__
#define __TRIX_HW_CBUSC_C__



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





unsigned int hwemu_cbusc_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	int okay = 0;
	int read = 1;
	int write = 0;
	unsigned int data = 0;
	unsigned char *sect = "CBUSC";
	unsigned char *desc = "";
	unsigned int offset = address - CBUSC_BASE;

	if ( type & MODE_WRITE )
	{
		read = 0;
		write = 1;
	}

	switch ( offset )
	{
		case 0x0000:
			desc = "Start/Length";
			break;

		case 0x001A:
		case 0x001C:
		case 0x001E:
		case 0x0020:
		case 0x0022:
		case 0x0024:
			desc = "FAID Data";
			break;

		case 0x0036:
			desc = "Chk Byte";
			break;

		default:
			data = 0x0000;
	}

	if ( type & MODE_WRITE )
	{
		if ( !okay )
			hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_CBUSC )
			printf ( "strh ( 0x%08X, 0x%04X ); // (%s+0x%04X %s)\n", address, value, sect, offset, desc );
	}
	else
	{
		if ( !okay )
			data = hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_CBUSC )
			printf ( "ldrh ( 0x%08X ); // -> 0x%04X (%s+0x%04X %s)\n", address, data, sect, offset, desc );
	}

	return data;
}

#endif
