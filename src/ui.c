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

extern int debuglevel;
extern int errorlevel;

unsigned int ui_exiting = 0;
unsigned int ui_cb_enabled = 1;
unsigned char *ui_cb_vprintf = NULL;
unsigned char *ui_cb_fgets = NULL;

void ui_set_exiting ( )
{
	ui_exiting = 1;
}

unsigned int ui_register_cb ( unsigned char *vprintf_cb, unsigned char *fgets_cb )
{
	CHECK_AND_FREE ( ui_cb_vprintf );
	CHECK_AND_FREE ( ui_cb_vprintf );
	if ( vprintf_cb && fgets_cb )
	{
		ui_cb_vprintf = strdup ( vprintf_cb );
		ui_cb_fgets = strdup ( fgets_cb );
	}

	return E_OK;
}

unsigned int ui_set_callback_state ( unsigned int enabled )
{
	ui_cb_enabled = enabled;

	return E_OK;
}

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
		ui_printf_msg ( "\n" );
		ui_printf_msg ( "   %s\n", title );
		ui_printf_msg ( "---------------------------\n" );
		ui_printf_msg ( "\n" );
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
					ui_printf_msg ( " <%i.>  %s\n", entries, text[pos] );
			}
			else
				if ( !(options & UI_OPT_NOREPAINT) )
					ui_printf_msg ( "\n" );
			pos++;
		}
		if ( (options & UI_OPT_BACK) && !(options & UI_OPT_NOREPAINT) )
			ui_printf_msg ( "\n <0.>  Back\n\n" );
		else if ( (options & UI_OPT_QUIT) && !(options & UI_OPT_NOREPAINT) )
			ui_printf_msg ( "\n <0.>  Quit\n\n" );

		options |= UI_OPT_NOREPAINT;

		ret = ui_dlg_int ( "Your choice", 0, entries, 0, 0 );

		if ( ret == E_FAIL )
			return E_FAIL;

		if ( ui_exiting )
			return E_FAIL;

		if ((ret == 0 && ((options & UI_OPT_BACK) || (options & UI_OPT_QUIT)) ) ||
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


int ui_vprintf_func ( const char *format, va_list args )
{
	if ( ui_cb_vprintf && ui_cb_enabled )
	{
		int (*vprintf_cb) ( const char *format, va_list args ) = NULL;

		vprintf_cb = trixplug_get_global_plugin_symbol ( ui_cb_vprintf );

		if ( !vprintf_cb )
			ui_cb_vprintf = NULL;
		else
			return vprintf_cb ( format, args );
	}
#ifdef QT_CORE_LIB
	return qt_vprintf ( format, args );
#else
	return vprintf ( format, args );
#endif
}

int ui_printf_msg ( char *str, ... )
{
	va_list args;
	va_start ( args, str );

	ui_vprintf_func ( str, args );

	va_end ( args );
	return 0;
}


int ui_debug_msg ( int level, char *str, ... )
{
	va_list args;
	va_start ( args, str );

	if ( debuglevel & level )
	{
		ui_vprintf_func ( str, args );
		ui_printf_msg ( "\n" );
	}

	va_end ( args );
	return 0;
}

int ui_error_msg ( int level, char *str, ... )
{
	va_list args;
	va_start ( args, str );

	if ( errorlevel & level )
	{
		ui_vprintf_func ( str, args );
		ui_printf_msg ( "\n" );
	}

	va_end ( args );
	return 0;
}



unsigned int ui_dlg_string ( char *text, char **buf )
{
	int buf_len = 32768;

#ifdef QT_CORE_LIB
	if ( !ui_cb_fgets || !ui_cb_enabled )
	{
		if ( !buf || !text )
			return E_FAIL;

		CHECK_AND_FREE ((*buf));
		*buf = qt_dlg_string ( *buf, text );
		if ( *buf )
			return E_OK;
		return E_FAIL;
	}
#endif

	if ( !buf|| !text )
		return E_FAIL;

	if ( strlen ( text ) )
		ui_printf_msg ( "%s :  ", text );

	*buf = malloc ( buf_len );

	if ( ui_cb_fgets && ui_cb_enabled )
	{
		int (*fgets_cb) ( char * buffer, int length ) = NULL;

		fgets_cb = trixplug_get_global_plugin_symbol ( ui_cb_fgets );

		if ( !fgets_cb )
			ui_cb_fgets = NULL;

		// in case of an error, fallback
		if ( !fgets_cb || !fgets_cb ( *buf, buf_len ) )
		{
			int ret = E_FAIL;

			CHECK_AND_FREE ( (*buf) );
			ui_set_callback_state ( 0 );
			ret = ui_dlg_string ( text, buf );
			ui_set_callback_state ( 1 );

			return ret;
		}

		(*buf)[buf_len-1] = '\000';
		if ( (*buf)[strlen ( *buf )-1] == '\n' )
			(*buf)[strlen ( *buf )-1] = '\000';

		(*buf) = realloc ( *buf, strlen ( *buf ) + 1 );

		return E_OK;
	}


#ifndef QT_CORE_LIB		
	if ( !fgets ( *buf, buf_len, stdin ) )
	{
		free ( *buf );
		return E_FAIL;
	}

	(*buf)[buf_len-1] = '\000';
	(*buf)[buf_len-1] = '\000';
	if ( (*buf)[strlen ( *buf )-1] == '\n' )
		(*buf)[strlen ( *buf )-1] = '\000';
	(*buf) = realloc ( *buf, strlen ( *buf ) + 1 );

	return E_OK;
#endif
}

unsigned int ui_dlg_load_file ( char **filename, const char *msg, const char *ext )
{
#ifdef QT_CORE_LIB
   if ( !filename )
      return E_FAIL;

   CHECK_AND_FREE ((*filename));
   *filename = qt_dlg_load_file ( msg, ext );
   if ( *filename )
      return E_OK;
#endif
   return E_FAIL;
}

unsigned int ui_dlg_save_file ( char **filename, const char *msg, const char *ext )
{
#ifdef QT_CORE_LIB
   if ( !filename )
      return E_FAIL;

   CHECK_AND_FREE ((*filename));
   *filename = qt_dlg_save_file ( msg, ext );
   if ( *filename )
      return E_OK;
#endif
   return E_FAIL;
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
	ui_printf_msg ( "%s\n", msg );
	return E_OK;
#endif
}


unsigned int ui_dlg_box_set_keypress_ptr ( unsigned int id, int *ptr )
{
#if defined(QT_CORE_LIB) && defined(TRIX)
	return qt_dlg_box_set_keypress_ptr ( id, ptr );
#else
	return E_FAIL;
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
