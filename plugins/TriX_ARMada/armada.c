#ifndef __TRIX_ARMADA_C__
#define __TRIX_ARMADA_C__

/*! \file armada.c
 */



#ifdef WIN32
    #include <windows.h>
#endif

#include "defines.h"
#include "seer.h"
#include "segment.h"
#include "stage.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"
#include "options.h"

#include "trixplug.h"
#include "armada.h"

#include "mem.h"

t_workspace *current_ws = NULL;
unsigned char *teststring = NULL;
int testbool = 0;
int bytes_patched = 0;

int initCompiler ( void );
char *compile ( char *text );

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

OPT_BEGIN
	OPTION( OPT_BOOL, "Bool Test Variable", "testbool", &testbool )
	OPTION( OPT_STR,  "String Test Variable", "teststring", &teststring )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "ARMada" )
	PLUGIN_INIT ( armada_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH





unsigned int armada_init ( )
{
    REGISTER_HEADER ( "trix_armada", ARMADA_HEADERS );
    ARMADA_SYMBOLS

	printf ( "ARMada v2.6 Plugin Loaded" );
    return E_OK;
}

unsigned int armada_initialize ( )
{
	initCompiler ();
	return E_OK;
}

unsigned int armada_compile ( t_workspace *ws, unsigned char *source, int output )
{
	char* ret  = NULL;
	
	current_ws = ws;

	armada_initialize ();
	bytes_patched = 0;
	ret = compile ( source );

	if ( output )
		printf ( "%s\n", ret );


	return E_OK;
}

unsigned int armada_inject_callback ( unsigned char *buf, unsigned int address, unsigned int length  )
{
	bytes_patched += length;

	v_memcpy_put ( current_ws, address, buf, length ); 
	return E_OK;
}

unsigned int armada_get_patched_bytes ( )
{
	return bytes_patched;
}

#endif

