#ifndef __TRIX_HW_CTSI_C__
#define __TRIX_HW_CTSI_C__



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

unsigned int timer2_prescaler_counter = 0;
unsigned int timer2_prescaler = 0x1F;
unsigned int timer2_value = 0x80;



unsigned int hwemu_ctsi_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	int okay = 0;
	int read = 1;
	int write = 0;
	unsigned int data = 0;
	unsigned char *sect = "CTSI";
	unsigned char *desc = "";
	unsigned int offset = address - CTSI_BASE;

	if ( type & MODE_WRITE )
	{
		read = 0;
		write = 1;
	}

	switch ( offset )
	{
		case 0x0000:
			desc = "Powerup Reason, States";
			data = 0x0021;
			okay = 1;
			break;

		case 0x0002:
			desc = "System Type";
			data = system_type;
			if ( read )
				okay = 1;
			if ( write )
				system_type |= (value & 0x0078);
			break;

		case 0x0006:
			if ( write )
				desc = "Watchdog reset";
			else
				desc = "Watchdog read?!";
			break;

		case 0x0012:
			desc = "Interrupts?";
			break;

		case 0x0024:
			desc = "IRQ States";
			break;

		case 0x0028:
			desc = "FIQ States";
			break;

		case 0x0030:
		case 0x0032:
			desc = "Sleep Mode";
			break;

		case 0x0034:
		case 0x0036:
			desc = "LCD/Audio";
			break;

		case 0x00BA:
			desc = "FBUS Interrupts Ack";
			break;

		case 0x0100:
			desc = "IO D/E Interrupts Ack";
			break;

		case 0x0112:
			desc = "Timer 2 Prescaler Counter";
			okay = 1;
			data = timer2_prescaler_counter >> 2;
			timer2_prescaler_counter++;
			timer2_prescaler_counter %= (timer2_prescaler+1) << 2;
			break;

		case 0x0114:
			desc = "Timer 2 Prescaler";
			okay = 1;
			if ( read )
				data = timer2_prescaler;
			else
				timer2_prescaler = value;
			break;

		case 0x0116:
			desc = "Timer Value";
			okay = 1;

			// ignore lowest bit, so that two consecutive reads will read back the same value
			data = ((timer2_value & ~1)<<8);

			timer2_value++;
			timer2_value %= 0xFF;
			break;

		case 0x0118:
			desc = "Timer Destination";
			break;


		default:
			data = 0x0000;
	}

	if ( type & MODE_WRITE )
	{
		if ( !okay )
			hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_CTSI )
			printf ( "strh ( 0x%08X, 0x%04X ); // (%s+0x%04X %s)\n", address, value, sect, offset, desc );
	}
	else
	{
		if ( !okay )
			data = hwemu_default_handle ( ws, address, type, value );
		if ( verbosity & HWEMU_VERB_CTSI )
			printf ( "ldrh ( 0x%08X ); // -> 0x%04X (%s+0x%04X %s)\n", address, data, sect, offset, desc );
	}
	return data;
}



#endif
