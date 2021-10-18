#ifndef __TRIX_HW_PUP_C__
#define __TRIX_HW_PUP_C__



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






unsigned int hwemu_pup_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	int okay = 0;
	int read = 1;
	int write = 0;
	unsigned int data = 0;
	unsigned char *sect = "PUP";
	unsigned char *desc = "";
	unsigned int offset = address - PUP_BASE;

	if ( type & MODE_WRITE )
	{
		read = 0;
		write = 1;
	}

	switch ( offset )
	{
		case 0x0002:
			desc = "IO A Output Set";
			break;

		case 0x0004:
			desc = "IO A Output Reset";
			break;

		case 0x0006:
			desc = "IO B Output Set";
			break;

		case 0x0008:
			desc = "IO B Output Reset";
			break;

		case 0x000A:
			desc = "IO A Readings";
			break;

		case 0x000C:
			desc = "IO B Readings";
			break;

		case 0x000E:
			desc = "IO A Direction";
			break;

		case 0x0010:
			desc = "IO B Direction";
			break;

		case 0x0012:
			desc = "IO A Pull Up States";
			break;

		case 0x0014:
			desc = "IO B Pull Up States";
			break;

		case 0x001E:
			desc = "Flasher Interface Lines";
			break;

		case 0x0022:
			desc = "Flasher Interface Read Register A";
			break;

		case 0x002C:
			desc = "IO C Output Set";
			break;

		case 0x002E:
			desc = "IO C Output Reset";
			break;

		case 0x0030:
			desc = "IO C Readings";
			break;

		case 0x0032:
			desc = "IO C Direction";
			break;

		case 0x0038:
			desc = "Flasher Interface Read Register B";
			break;

		default:
			data = 0x0000;
	}

	if ( type & MODE_WRITE )
	{
		if ( !okay )
			hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_PUP )
			printf ( "strh ( 0x%08X, 0x%04X ); // (%s+0x%04X %s)\n", address, value, sect, offset, desc );
	}
	else
	{
		if ( !okay )
			data = hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_PUP )
			printf ( "ldrh ( 0x%08X ); // -> 0x%04X (%s+0x%04X %s)\n", address, data, sect, offset, desc );
	}
	return data;
}


#endif
