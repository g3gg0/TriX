#ifndef __TRIX_HW_FLASH_C__
#define __TRIX_HW_FLASH_C__



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


/* Intel StrataFlash */

#define STATE_NORMAL         0x00
#define STATE_PROGRAM        0x01
#define STATE_READ_ID        0x02
#define STATE_READ_STATUS    0x03
#define STATE_BLOCK_ERASE    0x04
#define STATE_CONFIG         0x05
#define STATE_LOCKBITS       0x06
#define STATE_PROTECTION     0x07
#define STATE_BUFFER         0x08
#define STATE_READ_QUERY     0x09

#define CMD_RESET            0xFF
#define CMD_READ_ID          0x90
#define CMD_READ_QUERY       0x98
#define CMD_READ_STATUS      0x70
#define CMD_CLEAR_STATUS     0x50
#define CMD_WRITE_BUFFER     0xE8
#define CMD_BLOCK_ERASE      0x20
#define CMD_PROGRAM1         0x40
#define CMD_PROGRAM2         0x10
#define CMD_SUSPEND          0xB0
#define CMD_RESUME           0xD0
#define CMD_CONFIG           0xB8
#define CMD_LOCKBITS         0x60
#define CMD_PROTECTION       0xC0


unsigned char *clr_block[0x10000];

extern int flash_device_code;
extern int flash_manuf_code;
extern int flash_cfi_device_code;


unsigned int hwemu_flash_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	int okay = 0;
	int read = 1;
	int write = 0;
	unsigned int data = 0;
	unsigned char *sect = "FLASH";
	unsigned char *desc = "";
	unsigned int offset = address - FLASH_BASE;

	unsigned int flash_next_state = STATE_NORMAL;
	static unsigned int flash_state = STATE_NORMAL;
//	static int flash_manuf_code = 0x0089;
//	static int flash_device_code = 0x8872;
	static int flash_rcd = 0x8000;


	if ( type & MODE_WRITE )
	{
		read = 0;
		write = 1;
	}

	// normal read commands
	if ( read )
	{
		switch ( flash_state )
		{
			case STATE_LOCKBITS:
				flash_next_state = STATE_LOCKBITS;
				// although lock bits command was started, reads work correctly
			case STATE_NORMAL:
				// all addresses behind 0x00000020 will be handled on function exit
				// if the address is < 0x00000020
				if ( address < 0x20 )
				{
					desc = "ARM IV Table";
					data = 0xE59FF018;
					okay = 1;
				}
				break;

			case STATE_READ_ID:
				desc = "Read ID IA/ID";
				flash_next_state = STATE_READ_ID;
				if ( (offset & 0xFFFF) == 0 )
					data = flash_manuf_code;
				else if ( (offset & 0xFFFF) == 2 )
					data = flash_device_code;
				else if ( (offset & 0xFFFF) == 10 )
					data = flash_rcd;
				else
					printf ( "HWEmu Flash: unknown address 0x%02X\n", address );

				okay = 1;
				break;

			case STATE_READ_QUERY:
				desc = "Read Query";
				switch ( offset & 0xFFFF )
				{
					case 0x2A:
						desc = "Read Query: Primary Vendor-Specific Offset";
						data = (0x0040 >> 1); // byte offset 0x40
						break;

					case 0x40:
					case 0x41:
						desc = "Read Query: Primary Vendor-Specific Identification 'P'";
						data = 'P';
						break;

					case 0x42:
					case 0x43:
						desc = "Read Query: Primary Vendor-Specific Identification 'R'";
						data = 'R';
						break;

					case 0x44:
					case 0x45:
						desc = "Read Query: Primary Vendor-Specific Identification 'I'";
						data = 'I';
						break;

					case 0xBA:
					case 0xBB:
						desc = "Read Query: Primary Vendor-Specific FlashID?";
						data = flash_cfi_device_code;
						break;
				}
				flash_next_state = STATE_READ_QUERY;
				okay = 1;
				break;

			case STATE_READ_STATUS:
				desc = "Read Status";
				flash_next_state = STATE_READ_STATUS;
				data = 0x0080;
				okay = 1;
				break;

			default:
				desc = "Unhandled state in read command";
				break;
		}
	}
	else
	{
		switch ( flash_state )
		{
			case STATE_READ_ID:
			case STATE_READ_QUERY:
			case STATE_READ_STATUS:
			case STATE_NORMAL:
				switch ( value )
				{
					case CMD_RESET:
						desc = "Read Array/Reset";
						flash_next_state = STATE_NORMAL;
						okay = 1;
						break;

					case CMD_READ_ID:
						desc = "Read Identifier Codes";
						flash_next_state = STATE_READ_ID;
						okay = 1;
						break;

					case CMD_READ_STATUS:
						desc = "Read Status Register";
						flash_next_state = STATE_READ_STATUS;
						okay = 1;
						break;

					case CMD_CLEAR_STATUS:
						desc = "Clear Status Register";
						flash_next_state = STATE_NORMAL;
						okay = 1;
						break;

					case CMD_BLOCK_ERASE:
						desc = "Block Erase";
						flash_next_state = STATE_BLOCK_ERASE;
						okay = 1;
						break;

					case CMD_PROGRAM1:
					case CMD_PROGRAM2:
						desc = "Program";
						flash_next_state = STATE_PROGRAM;
						okay = 1;
						break;

					case CMD_SUSPEND:
						desc = "Suspend";
						flash_next_state = STATE_NORMAL;
						okay = 1;
						break;

					case CMD_RESUME:
						desc = "Resume";
						flash_next_state = STATE_NORMAL;
						okay = 1;
						break;

					case CMD_CONFIG:
						desc = "Set Read Configuration";
						flash_next_state = STATE_CONFIG;
						flash_rcd = offset;
						okay = 1;
						break;

					case CMD_LOCKBITS:
						desc = "Set/Clear Lockbits";
						flash_next_state = STATE_LOCKBITS;
						okay = 1;
						break;

					case CMD_PROTECTION:
						desc = "Protection Program";
						flash_next_state = STATE_PROTECTION;
						okay = 1;
						break;

					case CMD_WRITE_BUFFER:
						desc = "Write to Buffer";
						flash_next_state = STATE_BUFFER;
						okay = 1;
						break;

					case CMD_READ_QUERY:
						desc = "Read Query";
						flash_next_state = STATE_READ_QUERY;
						okay = 1;
						break;


					default:
						desc = "Unknown";
						flash_next_state = STATE_NORMAL;
						okay = 1;
						break;
				}
				break;

			case STATE_PROGRAM:
				desc = "Programm WA/WD";
				v_set_h ( ws, address, v_get_h ( ws, address ) & value );
				flash_next_state = STATE_READ_STATUS;
				okay = 1;
				break;

			case STATE_BLOCK_ERASE:
				desc = "Block Erase BA";
				memset ( clr_block, 0xFF, 0x10000 );
				v_memcpy_put ( ws, address, clr_block, 0x10000 );
				flash_next_state = STATE_READ_STATUS;
				okay = 1;
				break;

			case STATE_CONFIG:
				desc = "Set Config";
				flash_next_state = STATE_NORMAL;
				okay = 1;
				break;

			case STATE_LOCKBITS:
				if ( value == 0x01 )
					desc = "Set Lockbits";
				else if ( value == 0xD0 )
					desc = "Clear Lockbits";
				else if ( value == 0x2F )
					desc = "Lock-Down Lockbits";
				else 
					desc = "Unknown Lockbits command";
				flash_next_state = STATE_NORMAL;
				okay = 1;
				break;

			case STATE_PROTECTION:
				desc = "Protection Program";
				flash_next_state = STATE_NORMAL;
				okay = 1;
				break;

			case STATE_BUFFER:
				desc = "Write to Buffer";
				printf ( "HWEmu Flash: Not supported yet\n" );
				flash_next_state = STATE_NORMAL;
				okay = 1;
				break;

			default:
				desc = "Unhandled state in write command";
				break;
		}
	}

	if ( type & MODE_WRITE )
	{
		if ( !okay )
			hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_FLASH )
			printf ( "strh ( 0x%08X, 0x%04X ); // (%s+0x%04X %s)\n", address, value, sect, offset, desc );
	}
	else
	{
		if ( flash_state != STATE_NORMAL || flash_next_state != STATE_NORMAL )
		{
			if ( !okay )
				data = hwemu_default_handle ( ws, address, type, value );
			if ( verbosity & HWEMU_VERB_FLASH )
				printf ( "ldrh ( 0x%08X ); // -> 0x%04X (%s+0x%04X %s)\n", address, data, sect, offset, desc );
		}
		else
			data = hwemu_default_handle ( ws, address, type, value );
	}

	flash_state = flash_next_state;

	return data;
}

#endif
