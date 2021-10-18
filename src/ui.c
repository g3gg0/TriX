#ifndef __TRIX_UI_C__
#define __TRIX_UI_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "treenode.h"
#include "workspace.h"
#include "ui.h"

#include "util.h"


#include "trixplug_funcs.h"
#include "mem.h"

UI_PLUG_INIT;



unsigned int ui_dlg_msg ( char *text, int type )
{
#ifdef QT_CORE_LIB
	return qt_dlg_msg ( text, type );
#else
	return E_OK;
#endif
}

unsigned int ui_dlg_tree ( t_treenode *tree )
{
#ifdef QT_CORE_LIB 
	return qt_dlg_tree ( tree );
#else
	return E_OK;
#endif
}



unsigned int ui_dlg_dumpmem ( t_workspace *ws, int address )
{
#if defined QT_CORE_LIB && defined TRIX
	return qt_dlg_dumpmem ( ws, address );
#else
	return E_OK;
#endif
}

unsigned int ui_dlg_dumpmem_set_addr (int address )
{
#if defined QT_CORE_LIB && defined TRIX
	return qt_dlg_dumpmem_set_addr (  address );
#else
	return E_OK;
#endif
}

unsigned int ui_dlg_dumpmem_get_addr ( )
{
#if defined QT_CORE_LIB && defined TRIX
	return qt_dlg_dumpmem_get_addr (  );
#else
	return 0;
#endif
}


unsigned int ui_dlg_dumpmem_visible ( )
{
#if defined QT_CORE_LIB && defined TRIX
	return qt_dlg_dumpmem_visible (  );
#else
	return 0;
#endif
}

unsigned int ui_dlg_dumpmem_finish (  )
{
#if defined QT_CORE_LIB && defined TRIX
	return qt_dlg_dumpmem_finish (  );
#else
	return E_OK;
#endif
}


unsigned int ui_dlg_menu ( char *title, char *text[], int options )
{
	int entries = 0;
	unsigned int ret = 0;
	int pos = 0;
	int done = 0;

	if ( !text )
		return E_FAIL;

	if ( title && !(options & UI_OPT_NOREPAINT) )
	{
		printf ( "\n" );
		printf ( "   %s\n", title );
		printf ( "---------------------------\n" );
		printf ( "\n" );
	}

	while ( !done )
	{
		pos = 0;
		while ( text[pos] )
		{
			if ( text[pos][0] != '\000' )
			{
				entries++;
				if ( !(options & UI_OPT_NOREPAINT) )
					printf ( " <%i.>  %s\n", entries, text[pos] );
			}
			else
				if ( !(options & UI_OPT_NOREPAINT) )
					printf ( "\n" );
			pos++;
		}
		if ( (options & UI_OPT_BACK) && !(options & UI_OPT_NOREPAINT) )
			printf ( "\n <0.>  Back\n\n" );
		else if ( (options & UI_OPT_QUIT) && !(options & UI_OPT_NOREPAINT) )
			printf ( "\n <0.>  Quit\n\n" );

		options |= UI_OPT_NOREPAINT;

		ret = ui_dlg_int ( "Your choice", 0, entries, 0, 0 );

		if ( ret == E_FAIL )
			return E_FAIL;

		if ( (ret == 0 && ((options & UI_OPT_BACK) || (options & UI_OPT_QUIT)) ) ||
			 (ret <= entries && ret > 0 ) )
			done = 1;
	}
	return ret;
}

unsigned int ui_dlg_int ( char *text, int min, int max, int adv, int def )
{
	unsigned int val = 0;
	char *buf = NULL;
	
	if ( adv )
	{
		buf = malloc ( 32 );
		sprintf ( buf, "%i", def );
	}

	R (ui_dlg_string ( text, &buf ));

	if ( !buf )
		return E_FAIL;

	if ( sscanf ( buf, "%i", &val ) != 1 )
	{
		free ( buf );
		return E_FAIL;
	}

	if ( val < min || val > max )
		return E_FAIL;

	return val;
}

unsigned int ui_dlg_bool ( char *text )
{
#ifdef QT_CORE_LIB
	return qt_dlg_bool ( text );
#else
	int val = 0;
	char *buf = NULL;

	R (ui_dlg_string ( text, &buf ));

	if ( !strcasecmp ( "y", buf ) || !strcasecmp ( "yes", buf ) )
		return E_OK;

	return E_FAIL;
#endif
}

unsigned int ui_dlg_string ( char *text, char **buf )
{
#ifdef QT_CORE_LIB
	if ( !buf || !text )
		return E_FAIL;

	CHECK_AND_FREE ((*buf));
	*buf = qt_dlg_string ( *buf, text );
	if ( *buf )
		return E_OK;
	return E_FAIL;
#else

	if ( !buf || !text )
		return E_FAIL;

	printf ( "%s :  ", text );

	*buf = malloc ( 32768 );
	if ( ! fgets ( *buf, 32768, stdin ) )
	{
		free ( *buf );
		return E_FAIL;
	}

	(*buf)[strlen ( *buf )-1] = 0;
	(*buf) = realloc ( *buf, strlen ( *buf ) + 1 );

	return E_OK;
#endif
}



unsigned int ui_dlg_box_create ( unsigned char *title )
{
#ifdef QT_CORE_LIB
	return qt_dlg_box_create ( title );
#else
	return E_FAIL;
#endif
}

unsigned int ui_dlg_box_release ( unsigned int id )
{
#ifdef QT_CORE_LIB
	return qt_dlg_box_release ( id );
#else
	return E_FAIL;
#endif
}

unsigned int ui_dlg_box_release_all (  )
{
#ifdef QT_CORE_LIB
	return qt_dlg_box_release_all ( );
#else
	return E_FAIL;
#endif
}

unsigned int ui_dlg_box_msg ( unsigned int id, unsigned char *msg )
{
#ifdef QT_CORE_LIB
	return qt_dlg_box_msg ( id, msg );
#else
	printf ( "%s\n", msg );
	return E_OK;
#endif
}

unsigned int ui_dlg_box_clear ( unsigned int id )
{
#ifdef QT_CORE_LIB
	return qt_dlg_box_clear ( id );
#else
	return E_FAIL;
#endif
}

unsigned int ui_dlg_box_size ( unsigned int id, unsigned int width, unsigned int height )
{
#ifdef QT_CORE_LIB
	return qt_dlg_box_size ( id, width, height );
#else
	return E_FAIL;
#endif
}


#endif
