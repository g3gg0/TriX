#ifndef __TRIX_DISARM_C__
#define __TRIX_DISARM_C__

/*! \file disarm.c
 */



#ifdef WIN32
//    #include <windows.h>
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
#include "arm.h"
#include "disarm.h"

#include "mem.h"



t_workspace *current_ws = NULL;
unsigned int *breakpoints = NULL;
unsigned char *teststring = NULL;
unsigned int testbool = 0;

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

OPT_BEGIN
/*
	OPTION( OPT_BOOL, "Bool Test Variable", "testbool", &testbool )
	OPTION( OPT_STR,  "String Test Variable", "teststring", &teststring )
*/
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "DisARM" )
	PLUGIN_INIT ( disarm_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned int disarm_init ( )
{
    REGISTER_HEADER ( "trix_disarm", DISARM_HEADERS );
    DISARM_SYMBOLS

	printf ( "DisARM v1.0 Plugin Loaded" );
    return E_OK;
}


unsigned int disarm_address ( t_workspace *ws, unsigned char *dest, unsigned int address, int is_thumb )
{
	return print_instruction ( dest, address, NULL, address, is_thumb, (void *)ws );
}

typedef unsigned char  byte;
typedef unsigned short hword;
typedef unsigned long  word;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed char  s8;
typedef signed short s16;
typedef signed long  s32;

u8   read_byte (u32 address, void*i)
{
	return v_get_b ( i, address );
}

u16  read_hword (u32 address, void*i)
{
	return v_get_h ( i, address );
}

u32  read_word (u32 address, void*i)
{
	return v_get_w ( i, address );
}

u16  read_aligned_hword (u32 address, void*i)
{
	return v_get_h ( i, address & ~3 );
}

u32  read_aligned_word (u32 address, void*i)
{
	return v_get_w ( i, address & ~3 );
}

void write_byte (word address, byte data, void*i)
{
	v_set_b ( i, address, data );
}

void write_hword (u32 address, u16 data, void*i)
{
	v_set_h ( i, address, data );
}

void write_word (u32 address, u32 data, void*i)
{
	v_set_w ( i, address, data );
}

#endif