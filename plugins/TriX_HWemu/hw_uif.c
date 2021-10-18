#ifndef __TRIX_HW_UIF_C__
#define __TRIX_HW_UIF_C__



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






unsigned int hwemu_uif_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	int okay = 0;
	int read = 1;
	int write = 0;
	unsigned int data = 0;
	unsigned char *sect = "UIF";
	unsigned char *desc = "";
	unsigned int offset = address - UIF_BASE;

	if ( type & MODE_WRITE )
	{
		read = 0;
		write = 1;
	}

	switch ( offset )
	{
		case 0x0000:
			desc = "IO D Input/Output";
			break;

		case 0x0002:
			desc = "IO D Direction";
			break;

		case 0x0004:
			desc = "IO D Interrupt mask";
			break;

		case 0x000A:
			desc = "IO D Pull Up States";
			break;

		case 0x000C:
			desc = "IO E Input/Output";
			break;

		case 0x000E:
			desc = "IO E Direction";
			break;

		case 0x0010:
			desc = "IO E Interrupt mask";
			break;

		case 0x0016:
			desc = "IO E Pull Up States";
			break;

		case 0x001A:
			desc = "Graph Write CMD";
			if ( verbosity & HWEMU_VERB_SPI )
					printf ( "spi_send_cmd ( 0x%02X );\n", value );
			break;

		case 0x001C:
			desc = "Graph Write DATA";
			if ( verbosity & HWEMU_VERB_SPI )
					printf ( "    spi_send_data ( 0x%02X );\n", value );
			break;

		case 0x0020:
			desc = "CBUS Transfer count";
			break;

		case 0x0022:
			desc = "CBUS Flags";
			okay = 1;
			if ( read )
				data = 0x0400;

			if ( write && (value & 0x0100) )
			{
				unsigned int cbus_dev = (value>>5)&0x07;
				unsigned int cbus_reg = value & 0x1F;
				unsigned int cbus_len = v_get_h ( ws, UIF_BASE+0x0020 ) & 0x1F;

				data = hwemu_cbus_handle ( ws, MODE_READ, cbus_dev, cbus_reg, 0, cbus_len );
				printf ( "// CBUS Read  Dev: 0x%02X, Reg: 0x%02X\n", cbus_dev, cbus_reg ); 
				v_set_h ( ws, UIF_BASE+0x0026, data );
				v_set_h ( ws, UIF_BASE+0x0024, data>>16 );
			}

			if ( write )
				v_set_h ( ws, address, value );

			break;

		case 0x0024:
			desc = "CBUS Data High";
			break;

		case 0x0026:
			{
				unsigned int cbus_dev = (v_get_h ( ws, UIF_BASE+0x0022 )>>5)&0x07;
				unsigned int cbus_reg = v_get_h ( ws, UIF_BASE+0x0022 ) & 0x1F;
				unsigned int cbus_val = (v_get_h ( ws, UIF_BASE+0x0024 ) << 16 ) | value;
				unsigned int cbus_len = v_get_h ( ws, UIF_BASE+0x0020 ) & 0x1F;

				desc = "CBUS Data Low";

				if ( write )
				{
					
					if ( verbosity & HWEMU_VERB_UIF )
					{
						if ( v_get_h ( ws, UIF_BASE+0x0022 ) & 0x0200 )
							printf ( "// CBUS Write Dev: 0x%02X, Reg: 0x%02X, Data: 0x%04X Len: 0x%02X\n", cbus_dev, cbus_reg, cbus_val, cbus_len ); 
						else
							printf ( "// CBUS Write Dev: 0x%02X, Reg: 0x%02X, Data: 0x%04X Len: 0x%02X (no write flag?)\n", cbus_dev, cbus_reg, cbus_val, cbus_len ); 
					}
					hwemu_cbus_handle ( ws, MODE_WRITE, cbus_dev, cbus_reg, cbus_val, cbus_len ); 
				}

				if ( (verbosity & HWEMU_VERB_UIF) && read && (v_get_h ( ws, UIF_BASE+0x0022 ) & 0x0100) )
					printf ( "// CBUS Read: 0x%04X\n", (v_get_h ( ws, UIF_BASE+0x0024 ) << 16 ) | v_get_h ( ws, UIF_BASE+0x0026 ) ); 
			}
			break;

		case 0x002A:
			desc = "Graph Ready States";
			okay = 1;
			data = 0x0007;
			break;

		case 0x002E:
			desc = "Graph Interface Setup";
			break;

		case 0x0030:
			desc = "System Clocks";
			break;

		case 0x003A:
			desc = "Audio Codec Clock";
			break;

		default:
			data = 0x0000;
	}

	if ( type & MODE_WRITE )
	{
		if ( !okay )
			hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_UIF )
			printf ( "strh ( 0x%08X, 0x%04X ); // (%s+0x%04X %s)\n", address, value, sect, offset, desc );
	}
	else
	{
		if ( !okay )
			data = hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_UIF )
			printf ( "ldrh ( 0x%08X ); // -> 0x%04X (%s+0x%04X %s)\n", address, data, sect, offset, desc );
	}
	return data;
}



#endif
