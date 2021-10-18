#ifndef __TRIX_TRIXPLUG_C__
#define __TRIX_TRIXPLUG_C__

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>


#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif 

// defined in *.vcproj - easier for debugging plugins
//#define TRIXPLUG_DL_MEMORY

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "seer.h"
#include "workspace.h"
#include "util.h"
#include "ui.h"
#include "mem.h"
#include "trixplug.h"
#include "crypto.h"
#include "options.h"

int skip_plugin_fail = 0;
int plugin_signature_details = 2;


struct trix_functions *ft;

char *plugin_path = "plugins";
t_plugin_entry *plugins = NULL;

void (*trixplug_notification_target) (void*) = NULL;
void *trixplug_notification_object = NULL;

TRIXPLUG_PLUG_INIT;

t_plugin_info *trixplug_get ( t_plugin_info *info, int type ) 
{
	int pos = 0;

	if ( !info )
		return NULL;

	while ( info[pos].type )
	{
		if ( info[pos].type == type )
			return &info[pos];
		pos++;
	}
	return NULL;
}

unsigned int
trixplug_handle_load ( unsigned char **plugin, unsigned int length, unsigned char *desc )
{
	t_crypt_key pub_key[] = TRIX_PUBLIC_KEYS;
	char *signature = NULL;

	signature = crypto_check_integrity ( plugin, length, pub_key );
	if ( signature )
	{
		if ( plugin_signature_details >= 1 )
			printf ( "[TriX]  Signed by '%s' ", crypto_get_signer ( signature ) );
		if ( plugin_signature_details >= 2 )
			printf ( "for '%s' ", crypto_get_signee ( signature ) );
		if ( plugin_signature_details >= 3 )
			printf ( "key '%s' ", crypto_get_signkey ( signature ) );
		if ( plugin_signature_details >= 4 )
			printf ( "opt %08X", crypto_get_signopt ( signature ) );
		if ( plugin_signature_details >=1 )
			printf ( "\n" );
	}
	else
	{
#ifdef PPMODD
//		return E_FAIL;
#else
		if ( !skip_plugin_fail )
		{
			unsigned char *msg = "The plugin '%s' is not signed correctly.\nAre you sure you want to load it?\n(To disable this message, enable plugins.trixplug.skip_plugin_fail)";
			unsigned char *buffer = malloc ( strlen ( desc ) + strlen ( msg ) + 1 );
			sprintf ( buffer, msg , desc );

			if ( ui_dlg_bool ( buffer ) != E_OK )
			{
				free ( buffer );
				return E_FAIL;
			}

			free ( buffer );
		}
#endif
	}
	return crypto_unfold ( plugin, length, pub_key );
}

unsigned int trixplug_load_plugin ( char *name )
{
	dll_init_func plugin_init = NULL;
	t_plugin_info *plugin_info = NULL;
	t_plugin_info *pi = NULL;
	t_plugin_entry *entry = NULL;
	void *plugin_handle = NULL;
	char *filename = NULL;
	char *pluginname = NULL;
	unsigned int status = E_OK;
#ifdef WIN32
	t_workspace *ws = NULL;
#endif

	if ( !name )
	{
		ERR ( 1, "no plugin name given" );
		return E_FAIL;
	}

	if ( name[0] == '"' )
		name++;

	name = strdup ( name );
	if ( name[strlen(name)-1] == '"' )
		name[strlen(name)-1] = '\000';

	filename = malloc ( strlen ( name ) + strlen ( plugin_path ) + 10 );


	DBG ( DEBUG_PLUG, "Opening %s", name );

#if defined (WIN32) && defined(TRIXPLUG_DL_MEMORY)
	// per default look in plugins subdir
	sprintf ( filename, "%s"DIR_SEP_STR"%s.txp", plugin_path, name );
	workspace_skip_error ( 1 );
	ws = workspace_startup ( filename );

	if ( !ws )
	{
		// maybe its compressed?
		sprintf ( filename, "%s"DIR_SEP_STR"%s.txz", plugin_path, name );
		ws = workspace_startup ( filename );
	}

	if ( !ws )
	{
		// maybe its a http url or an absolute path?
		sprintf ( filename, "%s", name );
		ws = workspace_startup ( filename );
	}
	workspace_skip_error ( 0 );

	if ( ws )
	{
		/* try to find a MZ header */
		t_stage *pe = ws->memmap->stage;

		LIST_FFWD ( pe );
		while ( pe && v_get_h_raw ( pe, 0 ) != 0x5A4D ) // MZ
			LIST_PREV ( pe );

		if ( pe )
		{
			/* MZ header found. this plugin is not crypted or signed */
			unsigned char *buffer = malloc ( pe->segments->length );
			memcpy ( buffer, pe->segments->data, pe->segments->length );

			plugin_handle = DL_OPEN ( buffer );
		}
		else
		{
			/* no MZ header found. maybe plugin is signed. use last stage */
			if ( v_get_ptr(ws,0) )
			{
				unsigned char *buffer = malloc ( v_get_size ( ws ) );
				memcpy ( buffer, v_get_ptr(ws,0), v_get_size ( ws ) );

				if ( trixplug_handle_load ( &buffer, v_get_size ( ws ), name ) == E_OK )
				{
					plugin_handle = DL_OPEN ( buffer );
				}
			}
		}

		workspace_release ( ws );
	}

#else
	// TODO: load from memory for linux
	sprintf ( filename, "%s"DIR_SEP_STR"%s.txp", plugin_path, name );
	plugin_handle = DL_OPEN ( filename );
#endif

	if ( !plugin_handle )
	{
#ifdef WIN32
		ERR ( 1, "Could not open plugin '%s'. LastError: 0x%08X", name, GetLastError() );
#else
		ERR ( 1, "Could not open plugin '%s'.", name );
#endif
		free ( filename );
		free ( name );
		return E_FAIL;
	}

	free ( name );

	DBG ( DEBUG_PLUG, "Reading Symbols" );
	plugin_info = (t_plugin_info *) DL_SYM ( plugin_handle, "trix_plugin_info" );
	if ( !plugin_info )
	{
		ERR ( 1, "Plugin %s has no symbol named 'trix_plugin_info'", filename );
		free ( filename );
		DL_CLOSE ( plugin_handle );
		return E_FAIL;
	}

	printf ( "[Plugin]" );

	pi = trixplug_get ( plugin_info, PLUGIN_INFO_NAME );
	if ( !pi )
	{
		printf ( " - INVALID NAME -  " );
		free ( filename );
		DL_CLOSE ( plugin_handle );
		return E_FAIL;
	}
	pluginname = pi->param1;
	printf ( " - %s -  ", pluginname );


	//  check if plugin was already loaded
	if ( trixplug_get_plugin_entry ( pluginname ) )
	{
		printf ( "[STILL LOADED] - [OK]\n" );
		free ( filename );
		DL_CLOSE ( plugin_handle );
		return E_OK;
	}



	// check version
	pi = trixplug_get ( plugin_info, PLUGIN_INFO_INTERFACE );
	if ( !pi )
	{
		printf ( " - INVALID VERSION INFORMATION -  " );
		status = E_PLUGIN_INVALID;
	}

	if ( status == E_OK && 
		 (TRIXPLUG_MAJOR != (int)pi->param1)  ||
		 (TRIXPLUG_MINOR <  (int)pi->param2) )
	{
		printf ( " - Plugin: v%i.%i TriX: v%i.%i - ", (int)pi->param1, (int)pi->param2, TRIXPLUG_MAJOR, TRIXPLUG_MINOR );
		status = E_PLUGIN_INTERFACE_FAILURE;
	}


	// get function table
	if ( status == E_OK )
	{
		pi = trixplug_get ( plugin_info, PLUGIN_INFO_FUNCS );
		if ( !pi )
		{
			printf ( " - INVALID FUNCTION TABLE -  " );
			status = E_PLUGIN_INVALID;
		}
		else
			*((struct trix_functions**)pi->param1) = ft;
	}

	if ( status == E_OK )
	{
		pi = trixplug_get ( plugin_info, PLUGIN_INFO_OPTIONS );
		if ( pi && pi->param1 )
			options_import_plugin_options ( strdup(pluginname), pi->param1 );

		pi = trixplug_get ( plugin_info, PLUGIN_INFO_INIT );
		if ( !pi )
		{
			printf ( " - INVALID INIT FUNCTION -  " );
			status = E_PLUGIN_INVALID;
		}
	}

	if ( status == E_OK )
	{

		entry = LIST_NEW ( plugins, t_plugin_entry );
		if ( !plugins )
			plugins = entry;

		entry->pluginname = pluginname;
		entry->handle = plugin_handle;
		entry->info = plugin_info;
		entry->filename = filename;
		entry->headers = NULL;
		entry->symbols = NULL;
		entry->loaded = 0;


		plugin_init = pi->param1;
		status = plugin_init ( );
	}

	if ( status == E_OK )
		printf ( " - [OK]\n" );
	else if ( status == E_PLUGIN_INTERFACE_FAILURE )
		printf ( " [VERSION MISMATCH]\n" );
	else if ( status == E_PLUGIN_INVALID )
		printf ( " [CRITICAL: INVALID DATA]\n" );
	else if ( status == E_PLUGIN_ALREADY_LOADED )
		printf ( " [ALREADY LOADED]\n" );
	else
		printf ( " [UNKNOWN ERROR]\n" );

	if ( status == E_OK )
	{
		entry->loaded = 1;
		trixplug_notify();
	}
	else
	{
		DL_CLOSE ( plugin_handle );
		if ( entry )
			LIST_DEL_SIMPLE ( entry, plugins );
	}

	return status;
}


t_plugin_entry *trixplug_get_plugin_entry ( unsigned char *name )
{
	t_plugin_entry *plugin = plugins;

	while ( plugin )
	{
		if ( !strcmp ( plugin->pluginname, name ) )
			return plugin;
		LIST_NEXT ( plugin );
	}
	return NULL;
}

unsigned int trixplug_unload_plugin ( unsigned char *name )
{
	dll_cleanup_func plugin_cleanup = NULL;
	dll_lasterror_func plugin_lasterror = NULL;
	t_plugin_entry *plugin = NULL;
	t_plugin_info *pi = NULL;
	t_seer_dict *dict = NULL;

	plugin = trixplug_get_plugin_entry ( name );
	if ( !plugin )
		return E_FAIL;

	pi = trixplug_get ( plugin->info, PLUGIN_INFO_CLEANUP );
	if ( pi && pi->param1 )
	{
		plugin_cleanup = pi->param1;
		if ( plugin_cleanup () != E_OK )
		{
			unsigned char *error = "Cleanup failed (no reason given)";
			pi = trixplug_get ( plugin->info, PLUGIN_INFO_LASTERROR );
			if ( pi && pi->param1 )
			{
				plugin_lasterror = pi->param1;
				error = plugin_lasterror ();
			}

			printf ( "Unloading Plugin '%s' failed: '%s'\n", name, error );
			return E_FAIL;
		}
	}

	dict = plugin->headers;
	while ( dict )
	{
		seer_del_header ( dict->name );
		LIST_NEXT ( dict );
	}

	dict = plugin->symbols;
	while ( dict )
	{
		seer_del_symbol_int ( dict->name );
		LIST_NEXT ( dict );
	}

	pi = trixplug_get ( plugin->info, PLUGIN_INFO_OPTIONS );
	if ( pi && pi->param1 )
		options_delete_plugin_options ( plugin->pluginname, pi->param1 );


	DL_CLOSE ( plugin->handle );

	LIST_DEL_SIMPLE ( plugin, plugins );
	trixplug_notify();

	return E_OK;
}

unsigned int trixplug_add_plugin_header ( t_plugin_info *trix_plugin_info, unsigned char *section, unsigned char *header )
{
	t_plugin_entry *plugin = NULL;
	t_plugin_info *pi = NULL;
	t_seer_dict *dict = NULL;

	pi = trixplug_get ( trix_plugin_info, PLUGIN_INFO_NAME );
	if ( !pi )
		return E_FAIL;

	plugin = trixplug_get_plugin_entry ( pi->param1 );
	if ( !plugin )
		return E_FAIL;


	dict = LIST_NEW ( plugin->headers, t_seer_dict );
	if ( !plugin->headers )
		plugin->headers = dict;

	dict->name = section;
	dict->value = header;

	return seer_add_header ( section, header );
}

unsigned int trixplug_add_plugin_symbol ( t_plugin_info *trix_plugin_info, char *name, void *func )
{
	t_plugin_entry *plugin = NULL;
	t_plugin_info *pi = NULL;
	t_seer_dict *dict = NULL;

	pi = trixplug_get ( trix_plugin_info, PLUGIN_INFO_NAME );
	if ( !pi )
		return E_FAIL;

	plugin = trixplug_get_plugin_entry ( pi->param1 );
	if ( !plugin )
		return E_FAIL;


	dict = LIST_NEW ( plugin->symbols, t_seer_dict );
	if ( !plugin->symbols )
		plugin->symbols = dict;

	dict->name = name;
	dict->value = func;

	return seer_add_symbol_int ( name, func );
}

void *trixplug_get_global_plugin_symbol ( char *function )
{
	t_plugin_info *trix_plugin_info = NULL;
	t_seer_dict *dict = NULL;
	t_plugin_entry *plugin = plugins;

	if ( !function )
		return NULL;

	while ( plugin )
	{
		dict = plugin->symbols;
		while ( dict )
		{
			if ( !strcmp ( dict->name, function ) )
				return dict->value;
			LIST_NEXT(dict);
		}
		LIST_NEXT ( plugin );
	}


	return NULL;
}

void *trixplug_get_plugin_symbol ( char *name, char *function )
{
	t_plugin_info *trix_plugin_info = NULL;
	t_plugin_entry *plugin = NULL;
	t_seer_dict *dict = NULL;

	if ( !name || !function )
		return NULL;

	plugin = trixplug_get_plugin_entry ( name );
	if ( !plugin )
		return NULL;

	dict = plugin->symbols;
	while ( dict )
	{
		if ( !strcmp ( dict->name, function ) )
			return dict->value;
		LIST_NEXT(dict);
	}

	return NULL;
}

t_plugin_entry *trixplug_get_plugins ()
{
	return plugins;
}



/*!
 * set notifier in case of an changed option
 * \param target function in form 'void target (void *object)' to call
 * \param object the parameter to the function
 * \return E_OK 
 */
unsigned int trixplug_set_notification ( void *target, void* object )
{
	trixplug_notification_target = target;
	trixplug_notification_object = object;

	return E_OK;
}

/*!
 * notify registered function
 */
void trixplug_notify ()
{
	if ( trixplug_notification_target )
		trixplug_notification_target ( trixplug_notification_object );

}



unsigned int trixplug_init ( )
{
	ft = malloc ( sizeof ( struct trix_functions ) );
	if ( !ft )
		return E_FAIL;
	memset ( ft, 0x00, sizeof ( struct trix_functions ) );

	ft->version = TRIXPLUG_INTERFACE;
	ft->debug_msg = ui_debug_msg;
	ft->error_msg = ui_error_msg;
	ft->print_msg = ui_printf_msg;

	TRIXPLUG_INIT_ALL;

	options_add_core_option ( OPT_BOOL, "trixplug", skip_plugin_fail, "Skip warning dialog if signature failed" );
	options_add_core_option ( OPT_BOOL, "trixplug", plugin_signature_details, "Detail level when showing signature (0-4)" );

/*
#ifdef WIN32
	printf ( "TriXPluG using DLL mode" );
#else
	printf ( "TriXPluG using .so mode" );
#endif
*/
	return E_OK;
}


#endif
