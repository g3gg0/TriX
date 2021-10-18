#ifndef __TRIX_TERM_C__
#define __TRIX_TERM_C__

/*! \file term.c
 */

/*
    note:
        term at this state only works with windows !
*/

//#include <stdio.h>
#ifdef WIN32
    #include <windows.h>
	HANDLE hCom;
#endif

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"

#include "trixplug.h"
#include "term.h"

#include "mem.h"



// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

#define TERM_TITLE      "Terminal"
#define TERM_MAJOR      0
#define TERM_MINOR      01

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( TERM_TITLE )
	PLUGIN_VERSION ( TERM_MAJOR, TERM_MINOR )
	PLUGIN_INIT ( term_init )
PLUGIN_INFO_FINISH



unsigned int term_init ( )
{
	REGISTER_HEADER ( "trix_term", TERM_HEADERS );
    TERM_SYMBOLS

	printf ( TERM_TITLE " v%01i.%01i Plugin Loaded", TERM_MAJOR, TERM_MINOR  );

    return E_OK;
}


#ifdef WIN32
unsigned int
term_open ( unsigned int port )
{
	char portstr[32];

	if ( hCom )
	{
		ERR ( DEBUG_UNKNOWN, "##%s: COM port already open?\n", __FUNCTION__ );
		return E_FAIL;
	}

	sprintf ( portstr, "\\\\.\\COM%d", port );
	hCom = CreateFile ( (LPCWSTR)portstr,
	                    GENERIC_READ | GENERIC_WRITE,
	                    0,
	                    0,
	                    OPEN_EXISTING,
	                    0, //FILE_FLAG_OVERLAPPED,
	                    0 );

    if ( hCom == INVALID_HANDLE_VALUE )
    {
		ERR ( DEBUG_UNKNOWN, "##%s: could not open COM port %d\n", __FUNCTION__, port );
		hCom = NULL;
		return E_FAIL;
    }

    DBG ( DEBUG_UNKNOWN, "##%s: COM port opened with success\n", __FUNCTION__ );

	return E_OK;
}

unsigned int
term_close ()
{
    if ( !hCom )
    {
        ERR ( DEBUG_UNKNOWN, "##%s: COM port was not open!\n", __FUNCTION__ );
        return E_FAIL;
    }

	CloseHandle ( hCom );
	hCom = NULL;

    DBG ( DEBUG_UNKNOWN, "##%s: COM port closed with success\n", __FUNCTION__ );

	return E_OK;
}

/* device control settings */
unsigned int
term_set_control ( unsigned int baud, unsigned char databits, unsigned char stopbits, unsigned char parity, unsigned char handshake )
{
    /*
        todo:
            - error checking
            - handshake
            - parity
            - stopbits
            - let the user configure timeouts ?
    */

    COMMCONFIG lpCC;
    COMMTIMEOUTS timeouts;

    if ( !hCom )
    {
        ERR ( DEBUG_UNKNOWN, "##%s: COM port was not open!\n", __FUNCTION__ );
        return E_FAIL;
    }

    GetCommState ( hCom, &lpCC.dcb );

    lpCC.dcb.BaudRate = CBR_115200;
    lpCC.dcb.ByteSize = databits;
    lpCC.dcb.StopBits = ONESTOPBIT;
    lpCC.dcb.Parity = NOPARITY;

    lpCC.dcb.fDtrControl = DTR_CONTROL_DISABLE;
    lpCC.dcb.fRtsControl = RTS_CONTROL_DISABLE;
    SetCommState ( hCom, &lpCC.dcb );


    // set predefined timeouts
    timeouts.ReadIntervalTimeout = 20;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 3000;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(hCom, &timeouts))
    {
        ERR ( DEBUG_UNKNOWN, "##%s: setting timeouts didn't work!\n", __FUNCTION__ );
        return E_FAIL;
    }

    DBG ( DEBUG_UNKNOWN, "##%s: COM port control set\n", __FUNCTION__ );

    return E_OK;
}

unsigned int
term_set_dtr ( unsigned char state )
{
    /* todo :-) */
    return E_OK;
}

unsigned int
term_set_rts ( unsigned char state )
{
    /* todo :-) */
    return E_OK;
}

unsigned int
term_send ( const unsigned char *data, const unsigned int bytes )
{
    unsigned int bytesWritten = 0;

    if ( !hCom )
    {
        ERR ( DEBUG_UNKNOWN, "##%s: COM port was not open!\n", __FUNCTION__ );
        return E_FAIL;
    }

    WriteFile ( hCom, data, bytes, &bytesWritten, 0 );

    if ( bytes != bytesWritten )
    {
        ERR ( DEBUG_UNKNOWN, "##%s: only sent %d bytes of %d\n", __FUNCTION__, bytesWritten, bytes );
        return E_FAIL;
    }

    DBG ( DEBUG_UNKNOWN, "##%s: sent of %d bytes successfull\n", __FUNCTION__, bytes );

	return E_OK;
}

unsigned int
term_receive ( unsigned char *dest, const unsigned int bytes )
{
    unsigned int bytesRead = 0;

    if ( !hCom )
    {
        ERR ( DEBUG_UNKNOWN, "##%s: COM port was not open!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( !ReadFile ( hCom, dest, bytes, &bytesRead, 0 ) )
    {
        ERR ( DEBUG_UNKNOWN, "##%s: ReadFile returned error!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( bytes == bytesRead )
    {
        ERR ( DEBUG_UNKNOWN, "##%s: only received %d bytes of %d\n", __FUNCTION__, bytesRead, bytes );
        return E_FAIL;
    }

    DBG ( DEBUG_UNKNOWN, "##%s: receiving of %d bytes successfull\n", __FUNCTION__, bytes );

	return E_OK;
}

#else
/* linux implementation */

unsigned int 
term_receive ( unsigned char *dest, const unsigned int bytes )
{
    ERR ( DEBUG_UNKNOWN, "term works only under windows for now\n" );
    return E_FAIL;
}

unsigned int 
term_send ( const unsigned char *data, const unsigned int bytes )
{
    ERR ( DEBUG_UNKNOWN, "term works only under windows for now\n" );
    return E_FAIL;
}

unsigned int 
term_set_control ( unsigned int baud, unsigned char databits, unsigned char stopbits, unsigned char parity, unsigned char handshake )
{
    ERR ( DEBUG_UNKNOWN, "term works only under windows for now\n" );
    return E_FAIL;
}

unsigned int 
term_open ( unsigned int port )
{
    ERR ( DEBUG_UNKNOWN, "term works only under windows for now\n" );
    return E_FAIL;
}

unsigned int 
term_close ( )
{
    ERR ( DEBUG_UNKNOWN, "term works only under windows for now\n" );
    return E_FAIL;
}

#endif

#endif

