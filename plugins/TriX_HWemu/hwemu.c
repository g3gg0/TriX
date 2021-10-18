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

int verbosity = 0xFFFFFFFF;

int arch_type = 1;
int system_type = 0x03A6;
int upp_id = 0x1120;

int flash_manuf_code = 0x0089;
int flash_device_code = 0x8874; //0x8872;
int flash_cfi_device_code = 0x03; //0x8872;

int uem_pwr_key_pressed = 0;
int uem_charger_connected = 1;
int uem_adc_values[14] = { 
  0x01F9, 0x03EC, 0x01E6, 0x03E8, 0x02B8, 0x03F0, 0x0000, 0x03F3, 0x0303, 0x0002, 0x0004, 0x037B, 0x031B, 0x03F5 };
//0x01FB, 0x03EC, 0x01E5, 0x03E8, 0x02C0, 0x03F0, 0x0000, 0x03F3, 0x0305, 0x0002, 0x0004, 0x037B, 0x031C, 0x03F5 without charger
//0x01F8, 0x03EC, 0x01E5, 0x03E8, 0x02B6, 0x03F0, 0x0000, 0x03F3, 0x0301, 0x0174, 0x0004, 0x037C, 0x031C, 0x03F6 with charger
//0xC1B9, 0, 0, 0, 0, 0, 0x0985, 0, 0, 0, 0, 0, 0x0910 };


int hw_flash_bases[] = { 0x00000000, 0x01000000 };
int hw_faid_bases[] = { 0x0000012C, 0x0100002C };

int hw_lead3mmi_bases[] = { 0xFFFF0000, 0x06002000 };
int hw_dsppll_bases[] = { 0xFFFF0000, 0x06008000 };
int hw_mcupll_bases[] = { 0xFFFF0000, 0x06009000 };
int hw_accpll_bases[] = { 0xFFFF0000, 0x0600A000 };
int hw_dspmif_bases[] = { 0x0C305500, 0x0600B000 };
int hw_ebusc_bases[] = { 0xFFFF0000, 0x0600C000 };
int hw_fsig_bases[] = { 0xFFFF0000, 0x0600C800 };
int hw_testif_bases[] = { 0xFFFF0000, 0x0600F000 };
int hw_cbusc_bases[] = { 0x0C305300, 0xFFFF0000 };
int hw_sti_bases[] = { 0x0C305400, 0x0600D000 };
int hw_ctsi_bases[] = { 0x0C308000, 0x06010000 };
int hw_pup_bases[] = { 0x0C308200, 0x06010200 };
int hw_uif_bases[] = { 0x0C308400, 0x06010400 };
int hw_accif_bases[] = { 0xFFFF0000, 0x06010500 };
int hw_simif_bases[] = { 0xFFFF0000, 0x06010800 };
int hw_mfi_bases[] = { 0xFFFF0000, 0x06010900 };
int hw_brain_bases[] = { 0xFFFF0000, 0x06100000 };
int hw_osmodat_bases[] = { 0xFFFF0000, 0x04000000 };
int hw_osmocmd_bases[] = { 0xFFFF0000, 0x04FD0000 };

OPT_BEGIN
	OPTION( OPT_HEX, "Verbosity flags", "verbosity", &verbosity )
	OPTION( OPT_INT, "CPU Architecture (0=TIKU, 1=ASIC11)", "arch_type", &arch_type )
	OPTION( OPT_HEX, "System Type (CTSI+0x0002)", "system_type", &system_type )
	OPTION( OPT_HEX, "UPP ID (DSPMIF+0x0000)", "upp_id", &upp_id )
	OPTION( OPT_HEX, "Flash Manufacturer", "flash_manuf_code", &flash_manuf_code )
	OPTION( OPT_HEX, "Flash Device Code", "flash_device_code", &flash_device_code )
	OPTION( OPT_HEX, "Flash CFI Device Code", "flash_cfi_device_code", &flash_cfi_device_code )
	OPTION( OPT_BOOL, "UEM Power Key", "uem.pwr_key", &uem_pwr_key_pressed )
	OPTION( OPT_BOOL, "UEM Charger Connected", "uem.charger", &uem_charger_connected )
	OPTION( OPT_HEX, "UEM ADC Chan 0 Value", "uem.adc.chan_0", &uem_adc_values[0] )
	OPTION( OPT_HEX, "UEM ADC Chan 1 Value", "uem.adc.chan_1", &uem_adc_values[1] )
	OPTION( OPT_HEX, "UEM ADC Chan 2 Value", "uem.adc.chan_2", &uem_adc_values[2] )
	OPTION( OPT_HEX, "UEM ADC Chan 3 Value", "uem.adc.chan_3", &uem_adc_values[3] )
	OPTION( OPT_HEX, "UEM ADC Chan 4 Value", "uem.adc.chan_4", &uem_adc_values[4] )
	OPTION( OPT_HEX, "UEM ADC Chan 5 Value", "uem.adc.chan_5", &uem_adc_values[5] )
	OPTION( OPT_HEX, "UEM ADC Chan 6 Value", "uem.adc.chan_6", &uem_adc_values[6] )
	OPTION( OPT_HEX, "UEM ADC Chan 7 Value", "uem.adc.chan_7", &uem_adc_values[7] )
	OPTION( OPT_HEX, "UEM ADC Chan 8 Value", "uem.adc.chan_8", &uem_adc_values[8] )
	OPTION( OPT_HEX, "UEM ADC Chan 9 Value", "uem.adc.chan_9", &uem_adc_values[9] )
	OPTION( OPT_HEX, "UEM ADC Chan 10 Value", "uem.adc.chan_10", &uem_adc_values[10] )
	OPTION( OPT_HEX, "UEM ADC Chan 11 Value", "uem.adc.chan_11", &uem_adc_values[11] )
	OPTION( OPT_HEX, "UEM ADC Chan 12 Value", "uem.adc.chan_12", &uem_adc_values[12] )
	OPTION( OPT_HEX, "UEM ADC Chan 13 Value", "uem.adc.chan_13", &uem_adc_values[13] )
//	OPTION( OPT_STR,  "String Test Variable", "teststring", &teststring )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( PLUGIN_TITLE )
	PLUGIN_VERSION ( PLUGIN_MAJOR, PLUGIN_MINOR )
	PLUGIN_INIT ( hwemu_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned char outbuf[16384];
unsigned char tempbuf[1024];


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
		if ( mm->start <= address && mm->end > address )
			mm->segment->capture_plug = "hwemu_handler";

		LIST_NEXT(mm);
	}
	return E_OK;
}

unsigned int hwemu_default_handle ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	unsigned int data = 0;

	switch ( type & WIDTH_MASK )
	{
		case WIDTH_WORD:
			if ( type & MODE_WRITE )
				v_set_w ( ws, address, value );
			else
				data = v_get_w ( ws, address );
			break;

		case WIDTH_HALF:
			if ( type & MODE_WRITE )
				v_set_h ( ws, address, value );
			else
				data = v_get_h ( ws, address );
			break;

		case WIDTH_BYTE:
			if ( type & MODE_WRITE )
				v_set_b ( ws, address, value );
			else
				data = v_get_b ( ws, address );
			break;
	}

	return data;
}




unsigned int hwemu_handler ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value )
{
	unsigned int data = 0;

	if ( (address & 0xFFFFFE00) == CTSI_BASE )
		return hwemu_ctsi_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == UIF_BASE )
		return hwemu_uif_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == STI_BASE )
		return hwemu_sti_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == PUP_BASE )
		return hwemu_pup_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == CBUSC_BASE )
		return hwemu_cbusc_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == ACCIF_BASE )
		return hwemu_accif_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == ACCPLL_BASE )
		return hwemu_accpll_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == BRAIN_BASE )
		return hwemu_brain_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == DSPMIF_BASE )
		return hwemu_dspmif_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == DSPPLL_BASE )
		return hwemu_dsppll_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == EBUSC_BASE )
		return hwemu_ebusc_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == FSIG_BASE )
		return hwemu_fsig_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == LEAD3MMI_BASE )
		return hwemu_lead3mmi_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == MCUPLL_BASE )
		return hwemu_mcupll_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == MFI_BASE )
		return hwemu_mfi_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == SIMIF_BASE )
		return hwemu_simif_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == TESTIF_BASE )
		return hwemu_testif_handle ( ws, address, type, value );
	else if ( (address & 0xFFFFFF00) == OSMOCMD_BASE )
		return hwemu_osmocmd_handle ( ws, address, type, value );
	else if ( (address & 0xFF000000) == OSMODAT_BASE )
		return hwemu_osmodat_handle ( ws, address, type, value );
	else if ( (address & 0xF7000000) == FLASH_BASE )
		return hwemu_flash_handle ( ws, (address & 0xF7FFFFFF), type, value );
	else
		return hwemu_default_handle ( ws, address, type, value );
	
	return data;
}

#endif
