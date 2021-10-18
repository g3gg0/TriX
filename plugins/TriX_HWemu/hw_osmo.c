#ifndef __TRIX_HW_OSMO_C__
#define __TRIX_HW_OSMO_C__



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



#define JUST_PRINTABLE(value) (((value)>0x1F)?(value):0)

unsigned char *osmo_string = NULL;
unsigned int osmo_string_pos = 0;


unsigned int hwemu_osmodat_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	int okay = 0;
	int read = 1;
	int write = 0;
	unsigned int data = 0;
	unsigned char *sect = "OSMODAT";
	unsigned char *desc = "";
	unsigned int offset = address - OSMODAT_BASE;
	
	if ( !osmo_string )
		osmo_string = malloc ( 8192 );

	if ( type & MODE_WRITE )
	{
		read = 0;
		write = 1;
	}

	switch ( offset )
	{
		default:
			{
				if ( JUST_PRINTABLE((value >> 24) & 0xFF) != 0 )
					osmo_string[osmo_string_pos++] = JUST_PRINTABLE((value >> 24) & 0xFF);
				if ( JUST_PRINTABLE((value >> 16) & 0xFF) != 0 )
					osmo_string[osmo_string_pos++] = JUST_PRINTABLE((value >> 16) & 0xFF);
				if ( JUST_PRINTABLE((value >> 8) & 0xFF) != 0 )
					osmo_string[osmo_string_pos++] = JUST_PRINTABLE((value >> 8) & 0xFF);
				if ( JUST_PRINTABLE((value >> 0) & 0xFF) != 0 )
					osmo_string[osmo_string_pos++] = JUST_PRINTABLE((value >> 0) & 0xFF);

//				printf ( "OSMO DATA: 0x%08X\n", value );
				okay = 1;
				data = 0x0000;
			}
	}

	if ( type & MODE_WRITE )
	{
		if ( !okay )
			hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_OSMO )
			printf ( "strh ( 0x%08X, 0x%04X ); // (%s+0x%04X %s)\n", address, value, sect, offset, desc );
	}
	else
	{
		if ( !okay )
			data = hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_OSMO )
			printf ( "ldrh ( 0x%08X ); // -> 0x%04X (%s+0x%04X %s)\n", address, data, sect, offset, desc );
	}

	return data;
}


unsigned int hwemu_osmocmd_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	int okay = 0;
	int read = 1;
	int write = 0;
	unsigned int data = 0;
	unsigned char *sect = "OSMOCMD";
	unsigned char *desc = "";
	unsigned int offset = address - OSMOCMD_BASE;

	if ( !osmo_string )
		osmo_string = malloc ( 8192 );

	if ( type & MODE_WRITE )
	{
		read = 0;
		write = 1;
	}

	switch ( offset )
	{
		default:
			printf ( "OSMO CMD: %02X\n", value );
			if ( value == 0xEF )
			{
				osmo_string[osmo_string_pos] = '\000';
				printf ( "OSMO Message: <%s>\n", osmo_string );
				osmo_string_pos = 0;
			}
			else
			{
				osmo_string[osmo_string_pos] = '\000';
				printf ( "OSMO UNKNOWN Message: <%s>\n", osmo_string );
				osmo_string_pos = 0;
			}
			okay = 1;
			data = 0x0000;
	}

	if ( type & MODE_WRITE )
	{
		if ( !okay )
			hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_OSMO )
			printf ( "strh ( 0x%08X, 0x%04X ); // (%s+0x%04X %s)\n", address, value, sect, offset, desc );
	}
	else
	{
		if ( !okay )
			data = hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_OSMO )
			printf ( "ldrh ( 0x%08X ); // -> 0x%04X (%s+0x%04X %s)\n", address, data, sect, offset, desc );
	}

	return data;
}

#endif
