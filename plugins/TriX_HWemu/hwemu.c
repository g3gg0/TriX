#ifndef __TRIX_HWEMU_C__
#define __TRIX_HWEMU_C__



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

#include "treenode.h"

#include "trixplug.h"


#include "hwemu.h"

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

#define PLUGIN_TITLE "HWemu"
#define PLUGIN_MAJOR 0
#define PLUGIN_MINOR 1

unsigned char teststring[32] = "";
unsigned int base_address = 0x01000000;

OPT_BEGIN
	OPTION( OPT_STR,  "This is a test", "teststring", &teststring )
	OPTION( OPT_HEX, "Base address of IO module", &base_address )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( PLUGIN_TITLE )
	PLUGIN_VERSION ( PLUGIN_MAJOR, PLUGIN_MINOR )
	PLUGIN_INIT ( hwemu_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned char outbuf[16384];
unsigned char tempbuf[1024];

unsigned int rcr = 0;
unsigned int rbr = 0;
unsigned int rpr = 0;


unsigned int hwemu_init ( )
{
	REGISTER_HEADER ( "trix_hwemu", HWEMU_HEADERS );
    HWEMU_SYMBOLS

	printf ( PLUGIN_TITLE " v%01i.%01i Plugin Loaded", PLUGIN_MAJOR, PLUGIN_MINOR  );

	outbuf[0] = 0x00;

    return E_OK;
}


unsigned int hwemu_register ( t_workspace *ws, unsigned int address )
{
	t_memmap *mm = NULL;

	if ( !ws || !ws->memmap )
		return E_FAIL;

	mm = ws->memmap;
	while ( mm )
	{
		if ( mm->start < address && mm->end > address )
			mm->segment->capture_plug = "hwemu_handler";

		LIST_NEXT(mm);
	}
	return E_OK;
}

unsigned int uart_read (t_workspace *ws, unsigned int addr)
{
	unsigned int data;

	switch ((addr & 0xfff) >> 2) 
	{
	case 0xd:		// RCR
		data = rcr;
		break;
	case 0xf:		// TCR
		data = 0;
		break;
	case 0x5:		// CSR
		data = (1 << 9) | (1 << 1) |	/* always empty and ready */
			(rcr ? ((1 << 3) | (1 << 0)) : 0);
		break;
	case 0x0:		// CR
	case 0x1:		// MR
	case 0x2:		// IER
	case 0x3:		// IDR
	case 0x4:		// IMR
	case 0x6:		// RHR
	case 0x7:		// THR
	case 0x8:		// BRGR
	case 0x9:		// RTOR
	case 0xa:		// TTGR
	case 0xb:		// RES1
	case 0xc:		// RPR
	case 0xe:		// TPR
//		printf ("uart_read(%s=0x%08x)\n",  uart_reg[(addr & 0xfff) >> 2], addr);
		break;


	}
	return (data);
}
unsigned int
uart_write (t_workspace *ws, unsigned int addr, unsigned int data)
{
	static unsigned int tx_buf = 0;

	switch (addr & 0xfff) 
	{
		case 0x0:		// CR
		case 0x4:		// MR
		case 0x8:		// IER
		case 0xC:		// IDR
		case 0x10:		// IMR
		case 0x14:		// CSR
		case 0x18:		// RHR
		case 0x20:		// BRGR
		case 0x24:		// RTOR
		case 0x28:		// TTGR
		case 0x2C:		// RES1
			break;

		case 0x1C:		// THR
			sprintf( tempbuf, "%c", data); 
			strcat ( outbuf, tempbuf );
			break;

		case 0x34:		// RCR
			rcr = data;
			break;

		case 0x30:		// RPR
			rpr = data;
			break;

		case 0x38:		// TPR
			tx_buf = data;
			break;

		case 0x3C:		// TCR
			for (; tx_buf && data > 0; data--) 
			{
				sprintf( tempbuf, "%c", v_get_b ( ws, tx_buf++));
				strcat ( outbuf, tempbuf );
			printf ( tempbuf );
			}
			tx_buf = 0;
			break;
		}
}



unsigned int hwemu_handler ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	unsigned int data = 0;

	switch ( address )
	{
		case 0xfffff100:	/* IVR */
			break;
		case 0xfffff108:	/* interrupt status register */
			break;
		case 0xfffff110:	/* IMR */
			break;
		case 0xfffff114:	/* CORE interrupt status register */
			break;
		case 0xfffff120:	/* IECR */
			break;
		case 0xfffff124:	/* IDCR */
			break;
		case 0xfff00000:	/* CPU ID */
			data = 0x40;
			break;
		case 0xfff00001:	/* CPU ID */
			data = 0x00;
			break;
		case 0xfff00002:	/* CPU ID */
			data = 0x00;
			break;
		case 0xfff00003:	/* CPU ID */
			data = 0x14;
			break;
		case 0xfffe00c0:	/* TIMER 1 BCR */
			break;
		case 0xfffe00c4:	/* TIMER 1 BMR */
			break;

		default:
			if ((address & 0xffffff00) == 0xfffff000)
				break;
			if ((address & 0xfffff000) == 0xfffd0000) 
			{
				if ( type & MODE_WRITE )
					return uart_write (ws,address, value);
				else
					return uart_read (ws,address);

			}
			if ((address & 0xfffff000) == 0xfffcc000) 
			{
				break;
			}
			if(address >= 0xfffe0000 && address < 0xfffe00c0)
			{
				break;
			}

	}


	return data;
}

#endif
