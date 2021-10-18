#ifndef __TRIX_PRODIGIZE_C__
#define __TRIX_PRODIGIZE_C__

/*! \file prodigize.c
 */



#ifdef WIN32
    #include <windows.h>
#endif

#include "defines.h"
#include "options.h"

#include "trixplug.h"
#include "prodigize.h"
#include "interface.h"


// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"


/*
default speeds:
	0x05 - DCT4 Boot speed
	0x01 - DCT4 Conf speed
	0x80 - DCT4 Algo speed
	0x01 - LINDA Boot speed
	0x01 - LINDA Boot speed
	0x01 - LINDA Boot speed
*/
unsigned int dct4_boot_speed = 0x05;
unsigned int dct4_conf_speed = 0x01;
unsigned int dct4_algo_speed = 0x80;
unsigned int linda_boot_speed = 0x01;
unsigned int linda_conf_speed = 0x01;
unsigned int linda_algo_speed = 0x01;

unsigned int autoclose = 1;
unsigned int verbosity = 2;
unsigned int opened = 0;

OPT_BEGIN
	OPTION( OPT_BOOL, "Automatically close driver after failed commands", "autoclose", &autoclose )
	OPTION( OPT_INT, "Verbosity of the prodigy routines (0=nothing, 1=just some errors, 2=all errors, 3=verbose )", "verbosity", &verbosity )
	OPTION( OPT_INT, "DCT4 Boot speed (0-255)", "timings.dct4_boot_speed", &dct4_boot_speed )
	OPTION( OPT_INT, "DCT4 Conf speed (0-255)", "timings.dct4_conf_speed", &dct4_conf_speed )
	OPTION( OPT_INT, "DCT4 Algo speed (0-255)", "timings.dct4_algo_speed", &dct4_algo_speed )
	OPTION( OPT_INT, "LINDA Boot speed (0-255)", "timings.linda_boot_speed", &linda_boot_speed )
	OPTION( OPT_INT, "LINDA Conf speed (0-255)", "timings.linda_conf_speed", &linda_conf_speed )
	OPTION( OPT_INT, "LINDA Algo speed (0-255)", "timings.linda_algo_speed", &linda_algo_speed )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "Prodigize" )
	PLUGIN_VERSION ( 0, 2 )
	PLUGIN_INIT ( prodigize_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned int prodigize_init ( )
{
	REGISTER_HEADER ( "trix_prodigize", PRODIGIZE_HEADERS );
    PRODIGIZE_SYMBOLS

	printf ( "Prodigize v0.2 Plugin Loaded" );
    return E_OK;
}

unsigned int prodigize_open ( )
{
	if ( OpenPdDriver() != 1 )
	{
		printf ( "Prodigy init failed\n" );
		return E_FAIL;
	}

	opened = 1;
	return E_OK;
}

unsigned int prodigize_close ( )
{
	ClosePdDriver();
	opened = 0;

	return E_OK;
}
unsigned int prodigize_set_speeds ( )
{
	int res = 0;
	unsigned char speeds[6];

	speeds[0] = dct4_boot_speed & 0xFF;
	speeds[1] = dct4_conf_speed & 0xFF;
	speeds[2] = dct4_algo_speed & 0xFF;
	speeds[3] = linda_boot_speed & 0xFF;
	speeds[4] = linda_conf_speed & 0xFF;
	speeds[5] = linda_algo_speed & 0xFF;


	res = SetSpeeds ( speeds );
	if(res)
	{
		if ( verbosity > 1 )
			printf ( "ERROR %i while SetDefaultTimesA()\n", res );
		return E_FAIL;
	}

	return E_OK;
}

unsigned int prodigize_check_bootrom ( unsigned char *buf )
{

	if ( !opened )
		prodigize_open ( );

	prodigize_set_speeds ( );

	if ( CheckBootRomVersion ( buf ) != 1 )
	{
		if ( verbosity > 0 )
			printf ( "Phone boot failed\n" );
		if ( autoclose )
			prodigize_close ( );
		return E_FAIL;
	}

	return E_OK;
}

unsigned int prodigize_boot_2nd ( unsigned char *config, unsigned char *loader, unsigned int length )
{
	int i = 0;
	unsigned char config_size = 0;

	if ( !opened )
		prodigize_open ( );


	if ( LoadSecond ( config, &config_size, loader, length ) != 1 )
	{
		if ( verbosity > 0 )
			printf ( "2nd loader failed\n" );
		if ( autoclose )
			prodigize_close ( );
		return E_FAIL;
	}

	return E_OK;
}


unsigned int prodigize_boot_3rd ( unsigned char *loader, unsigned int length )
{
	unsigned short err = 0;

	if ( !opened )
		prodigize_open ( );

	if ( LoadThird ( &err, loader, length ) != 1 )
	{
		if ( verbosity > 0 )
			printf ( "3rd loader failed\n" );
		if ( autoclose )
			prodigize_close ( );
		return E_FAIL;
	}

	return E_OK;
}


unsigned int prodigize_send_data (  unsigned char *buffer, unsigned int bytes )
{
	if ( !opened )
		prodigize_open ( );

	if ( SendRawData ( buffer, bytes ) != 1 )
	{
		if ( verbosity > 0 )
			printf ( "data sending failed\n" );
		if ( autoclose )
			prodigize_close ( );
		return E_FAIL;
	}

	return E_OK;
/*
	int bytes_written = 0;
	int bytes_count = 0;

	if ( !opened )
		prodigize_open ( );

	// write memory blockwise
	while ( bytes_written < bytes )
	{
		if ( (bytes-bytes_written) < PRODIGY_BLOCKSIZE )
			bytes_count = (bytes-bytes_written);
		else
			bytes_count = PRODIGY_BLOCKSIZE;

		if ( SendRawData ( &buffer[bytes_written], bytes_count ) != 1 )
		{
			if ( verbosity > 0 )
				printf ( "data sending failed\n" );
			if ( autoclose )
				prodigize_close ( );
			return E_FAIL;
		}
		bytes_written = bytes_written + bytes_count;
	}
	return E_OK;
*/
}
#define PRODIGY_BLOCKSIZE 0xFD

unsigned int prodigize_read_data ( unsigned char *buffer, unsigned int bytes )
{
	unsigned char sec_buffer[PRODIGY_BLOCKSIZE];
	int bytes_read  = 0;
	int bytes_count = 0;

	if ( !opened )
		prodigize_open ( );

	// read memory blockwise
	while ( bytes_read < bytes )
	{
		if ( (bytes-bytes_read) < PRODIGY_BLOCKSIZE )
			bytes_count = (bytes-bytes_read);
		else
			bytes_count = PRODIGY_BLOCKSIZE;

		// read block
		if ( ReadRawData ( sec_buffer, bytes_count ) != 1 )
		{
			if ( verbosity > 0 )
				printf ( "data reading failed\n" );
			if ( autoclose )
				prodigize_close ( );
			return E_FAIL;
		}
		memcpy ( &buffer[bytes_read], sec_buffer, bytes_count );
		bytes_read = bytes_read + bytes_count;
	}

	return E_OK;
}

#endif

