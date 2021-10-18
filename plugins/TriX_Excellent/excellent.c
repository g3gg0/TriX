#ifndef __TRIX_EXCELLENT_C__
#define __TRIX_EXCELLENT_C__

/*! \file armulate.c
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
#include "mem.h"

#include "trixplug.h"
#include "excellent.h"


// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"

int use_color = 0;

OPT_BEGIN
	OPTION( OPT_INT, "Generate/Use color tags", "use_color", &use_color )
OPT_FINISH


PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "Excellent" )
	PLUGIN_INIT ( excellent_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned int excellent_init ( )
{
	REGISTER_HEADER ( "trix_excellent", EXCELLENT_HEADERS );
    EXCELLENT_SYMBOLS

	printf ( "EXCELlent v1.0 Plugin Loaded" );
    return E_OK;
}


unsigned int excellent_start_doc ( char *buffer )
{
	int col = 0;
	char buf[1024];
	sprintf ( buffer, DOC_START );

	if ( use_color )
	{
		for ( col = 0; col <256; col++)
		{
			sprintf ( buf, DOC_STYLE_START, col );
			strcat ( buffer, buf );
			sprintf ( buf, "%02X%02X%02X", col, col, col );
			strcat ( buffer, buf );
			strcat ( buffer, DOC_STYLE_END );
		}
	}
	strcat ( buffer, DOC_START2 );
	return 0;
}

unsigned int excellent_end_doc ( char *buffer )
{
	strcat ( buffer, DOC_END );
	return 0;
}

unsigned int excellent_start_cell ( char *buffer, char *type, int color )
{
	char buf[1024];

	if ( use_color )
		sprintf ( buf, CELL_START_COLOR, color, type );
	else
		sprintf ( buf, CELL_START, type );
	strcat ( buffer, buf );
	return 0;
}

unsigned int excellent_start_row ( char *buffer )
{
	strcat ( buffer, ROW_START );
	return 0;
}

unsigned int excellent_end_cell ( char *buffer )
{
	strcat ( buffer, CELL_END );
	return 0;
}

unsigned int excellent_end_row ( char *buffer )
{
	strcat ( buffer, ROW_END );
	return 0;
}

char *excellent_create_doc (  )
{
	int x_count = 255;
	int y_count = 1024;
	char *buffer = malloc ( strlen ( DOC_START ) + strlen ( DOC_START2 ) + strlen ( DOC_END ) + y_count*(strlen(ROW_START)+strlen(ROW_END)) + y_count*x_count*(strlen(CELL_START_COLOR)+strlen(CELL_END)+6) +  256*(strlen (DOC_STYLE_START) + strlen (DOC_STYLE_END) + 7 ) );
	if ( !buffer )
	{
		printf ( "could not allocate memory\n" );
		return NULL;
	}
	excellent_start_doc ( buffer );

	return buffer;
}


char *excellent_generate ( t_workspace *ws, unsigned int start, int width, int x_count, int y_count )
{
	int x = 0;
	int y = 0;
	char *buffer = malloc ( strlen ( DOC_START ) + strlen ( DOC_START2 ) + strlen ( DOC_END ) + y_count*(strlen(ROW_START)+strlen(ROW_END)) + y_count*x_count*(strlen(CELL_START_COLOR)+strlen(CELL_END)+6) +  256*(strlen (DOC_STYLE_START) + strlen (DOC_STYLE_END) + 7 ) );
	char *current = NULL;
	unsigned int pos = 0;
	char *value = malloc ( 256 );

	if ( !buffer )
	{
		printf ( "could not allocate memory\n" );
		return NULL;
	}
	excellent_start_doc ( buffer );

	for ( y=0; y<y_count; y++ )
	{
		printf ( "Row: %i\n", y );

		// dirty but fast ;)
		current = &buffer[pos];
		pos += strlen ( current );
		current = &buffer[pos];

		excellent_start_row ( current );

		for ( x=0; x<x_count; x++ )
		{
			if ( x < 256 )
			{
				excellent_start_cell ( current, "Number", v_get_b ( ws, start + y*x_count + x ) );

				// dirty but fast ;)
				pos += strlen ( current );
				current = &buffer[pos];

				sprintf ( value, "%i", v_get_b ( ws, start + y*x_count + x ) );
				strcat ( current, value );

				// dirty but fast ;)
				pos += strlen ( current );
				current = &buffer[pos];

				excellent_end_cell ( current );
			}
		}

		// dirty but fast ;)
		pos += strlen ( current );
		current = &buffer[pos];

		excellent_end_row ( current );
	}
	

	excellent_end_doc ( current );

	return buffer;
}

#endif

