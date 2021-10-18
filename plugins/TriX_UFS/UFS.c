#ifndef __TRIX_UFS_C__
#define __TRIX_UFS_C__

/*! \file ufs.c
 */



#ifdef WIN32
    #include <windows.h>
#endif

#include "defines.h"
#include "options.h"

#include "trixplug.h"
#include "ufs.h"


// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"


unsigned int preboot_read_timing = SPEED_PREBOOT_READ;   // my Twister: 60
unsigned int preboot_write_timing = SPEED_PREBOOT_WRITE;   // my Twister: 30.. 35
unsigned int boot_read_timing = SPEED_BOOT_READ;   // my Twister: 15.. 17
unsigned int boot_write_timing = SPEED_BOOT_WRITE;   // my Twister: 0.. 1
unsigned int flash_read_timing = SPEED_FLASH_READ;   // my Twister: 14.. 16
unsigned int flash_write_timing = SPEED_FLASH_WRITE;   // my Twister: 0.. 1
unsigned int timing_factor = 100;

unsigned int autoclose = 1;
unsigned int verbosity = 2;
unsigned int opened = 0;
unsigned int success_response = 1;

void *ufs_handle = NULL;

OPT_BEGIN
	OPTION( OPT_BOOL, "Automatically close driver after failed commands", "autoclose", &autoclose )
	OPTION( OPT_INT, "Verbosity of the prodigy routines (0=nothing, 1=just some errors, 2=all errors, 3=verbose )", "verbosity", &verbosity )
	OPTION( OPT_BOOL, "Wait for success response (0x00) after sending data. Required for TriX_Prodigize compatibility.", "success_response", &success_response )
	OPTION( OPT_INT, "Pre-Boot read timing (0=fast 255=slow)", "timings.preboot_read_timing", &preboot_read_timing )
	OPTION( OPT_INT, "Pre-Boot write timing (0=fast 255=slow)", "timings.preboot_write_timing", &preboot_write_timing )
	OPTION( OPT_INT, "Boot read timing (0=fast 255=slow)", "timings.boot_read_timing", &boot_read_timing )
	OPTION( OPT_INT, "Boot write timing (0=fast 255=slow)", "timings.boot_write_timing", &boot_write_timing )
	OPTION( OPT_INT, "Flash read timing (0=fast 255=slow)", "timings.flash_read_timing", &flash_read_timing )
	OPTION( OPT_INT, "Flash write timing (0=fast 255=slow)", "timings.flash_write_timing", &flash_write_timing )
	OPTION( OPT_INT, "Timing factor (100=normal, less=faster, more=slower)", "timings.timing_factor", &timing_factor )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "UFS" )
	PLUGIN_VERSION ( 0, 2 )
	PLUGIN_INIT ( ufs_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned int ufs_init ( )
{
	REGISTER_HEADER ( "trix_ufs", UFS_HEADERS );
    UFS_SYMBOLS

	printf ( "UFS v0.1 Plugin Loaded" );
    return E_OK;
}

unsigned int ufs_open ( )
{
	int ret = 0;

	if ( opened || ufs_handle )
		TUFSx_Close ( ufs_handle );

	ufs_handle = TUFSx_new ();

	ret = TUFSx_OpenFirst( ufs_handle );
	if ( ret )
	{
		printf ( "UFS open failed: %i, <%s>\n", ret, TUFSx_GetError ( ufs_handle, ret ) );
		return E_FAIL;
	}

	ret = TUFSx_Init( ufs_handle );
	if ( ret )
	{
		printf ( "UFS init failed: %i, <%s>\n", ret, TUFSx_GetError ( ufs_handle, ret ) );
		return E_FAIL;
	}

	opened = 1;
	return E_OK;
}

unsigned int ufs_close ( )
{
	TUFSx_Close ( ufs_handle );
	if ( ufs_handle )
		TUFSx_delete ( ufs_handle );

	opened = 0;
	ufs_handle = NULL;

	return E_OK;
}

unsigned int ufs_set_speeds ( )
{
	int i = 0;
	int res = 0;
	unsigned char speeds[6];

	speeds[0] = preboot_read_timing & 0xFF;
	speeds[1] = preboot_write_timing & 0xFF;
	speeds[2] = boot_read_timing & 0xFF;
	speeds[3] = boot_write_timing & 0xFF;
	speeds[4] = flash_read_timing & 0xFF;
	speeds[5] = flash_write_timing & 0xFF;

	for ( i=0; i<6; i++ )
	{
		res = ((int)speeds[i] * timing_factor) / 100;
		if ( res < 0x100 )
			speeds[i] = res;
		else
			speeds[i] = 0xFF;
	}

	res = TUFSx_SetSpeeds ( ufs_handle, speeds );
	if ( res )
	{
		if ( verbosity > 1 )
			printf ( "ERROR %i while set speeds()\n", res );
		return E_FAIL;
	}

	return E_OK;
}

unsigned int ufs_check_bootrom ( unsigned char *buf )
{
	unsigned long asic = 0;
	int ret = 0;

	if ( !opened )
		ufs_open ( );

	ufs_set_speeds ( );
	ret = TUFSx_BootLdr ( ufs_handle, buf );
	if ( ret )
	{
		if ( verbosity > 0 )
			printf ( "Phone boot failed: %i, <%s>\n", ret, TUFSx_GetError ( ufs_handle, ret ) );
		if ( autoclose )
			ufs_close ( );
		return E_FAIL;
	}

	return E_OK;
}

unsigned int ufs_boot_2nd ( unsigned char *config, unsigned char *loader, unsigned int length )
{
	int ret = 0;
	unsigned char config_size = 0;

	if ( !opened )
		ufs_open ( );

	ufs_set_speeds ( );
	ret = TUFSx_Boot2ndLdr ( ufs_handle, config, loader, length );
	if ( ret )
	{
		if ( verbosity > 0 )
			printf ( "2nd loader failed: %i, <%s>\n", ret, TUFSx_GetError ( ufs_handle, ret ) );
		if ( autoclose )
			ufs_close ( );
		return E_FAIL;
	}

	return E_OK;
}


unsigned int ufs_boot_3rd ( unsigned char *loader, unsigned int length )
{
	unsigned char zero_byte = 0;
	unsigned short err = 0;
	int ret = 0;

	if ( !opened )
		ufs_open ( );

	ufs_set_speeds ( );
	ret = TUFSx_FlashLdr ( ufs_handle, loader, length );
	if ( ret )
	{
		if ( verbosity > 0 )
			printf ( "3rd loader failed: %i, <%s>\n", ret, TUFSx_GetError ( ufs_handle, ret ) );
		if ( autoclose )
			ufs_close ( );
		return E_FAIL;
	}

	return E_OK;
}

#define PRODIGY_BLOCKSIZE 0xF000

unsigned int ufs_send_data ( unsigned char *buffer, unsigned int bytes )
{
	int bytes_written = 0;
	int bytes_count = 0;
	int ret = 0;

	if ( !opened )
		ufs_open ( );

	ufs_set_speeds ( );
	// write memory blockwise
	while ( bytes_written < bytes )
	{
		if ( (bytes-bytes_written) < PRODIGY_BLOCKSIZE )
			bytes_count = (bytes-bytes_written);
		else
			bytes_count = PRODIGY_BLOCKSIZE;

		ret = TUFSx_SendData ( ufs_handle, &buffer[bytes_written], bytes_count );
		if ( ret )
		{
			if ( verbosity > 0 )
				printf ( "data sending failed: %i, <%s>\n", ret, TUFSx_GetError ( ufs_handle, ret ) );
			if ( autoclose )
				ufs_close ( );
			return E_FAIL;
		}
		bytes_written = bytes_written + bytes_count;
	}

	return E_OK;
}

unsigned int ufs_read_data ( unsigned char *buffer, unsigned int bytes )
{
	unsigned char sec_buffer[PRODIGY_BLOCKSIZE];
	int bytes_read  = 0;
	int bytes_count = 0;

	if ( !opened )
		ufs_open ( );

	ufs_set_speeds ( );
	// read memory blockwise
	while ( bytes_read < bytes )
	{
		if ( (bytes-bytes_read) < PRODIGY_BLOCKSIZE )
			bytes_count = (bytes-bytes_read);
		else
			bytes_count = PRODIGY_BLOCKSIZE;

		// read block
		memset ( sec_buffer, 0x00, bytes_count );
		if ( TUFSx_ReceiveData ( ufs_handle, sec_buffer, bytes_count ) )
		{
			if ( verbosity > 0 )
				printf ( "data reading failed\n" );
			if ( autoclose )
				ufs_close ( );
			return E_FAIL;
		}
		memcpy ( &buffer[bytes_read], sec_buffer, bytes_count );
		bytes_read += bytes_count;
	}

	return E_OK;
}


unsigned short ufs_neg_checksum ( unsigned char *buffer, unsigned int bytes )
{
	unsigned int i = 0;
	unsigned short k = -1;

	for(i=0; i<bytes; i++)
		k = k - buffer[i];

	return k & 0xFFFF;
}

unsigned short ufs_pos_checksum ( unsigned char *buffer, unsigned int bytes )
{
	unsigned int i = 0;
	unsigned short k = 0;

	for ( i=0; i < bytes; i++)
		k = k + buffer[i];

	return k & 0xFFFF;
}


#endif

