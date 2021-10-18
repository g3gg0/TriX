#ifndef __TRIX_OPTIONS_C__
#define __TRIX_OPTIONS_C__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "file_io.h"
#include "file.h"
#include "workspace.h"
#include "util.h"

#include "options.h"

#include "trixplug_funcs.h"
#include "mem.h"


OPTIONS_PLUG_INIT;


t_option_table *option_table = NULL;
void (*options_notification_target) (void*) = NULL;
void *options_notification_object = NULL;

/*!
 * return all *changed* options as a char*
 * \param read all settings when set to 1, else just non-default ones
 * \return settings (free please)
 */
char *options_read_settings_real ( unsigned int all )
{
	char *ret = strdup ( "" );
	static char buf[64];
	t_option_table *table = option_table;

	while ( table )
	{
		if ( !table->is_default || all )
		{
			if (!(table->type & OPT_IS_CB ))
			{
				ret = realloc ( ret, strlen ( ret ) + strlen ( table->name ) + 64 );
				strcat ( ret, table->name );
				strcat ( ret, " = " );
				if ( table->type == OPT_STR )
				{
					if ( !(*((char**)table->target)) )
					{
						strcat ( ret, "" );
					}
					else
					{
						ret = realloc ( ret, strlen ( ret ) + strlen ( *((char**)table->target) ) + 10 );
						strcat ( ret, *((char**)table->target) );
					}
				}
				else if ( table->type == OPT_HEX )
				{
					sprintf ( buf, "0x%08X", *((int*)table->target) );
					strcat ( ret, buf );
				}
				else
				{
					sprintf ( buf, "%i", *((int*)table->target) );
					strcat ( ret, buf );
				}
				strcat ( ret, "\n" );
			}
		}
		LIST_NEXT(table);
	}

	return ret;
}

/*!
 * return all *changed* options as a char*
 * \return settings (free please)
 */
char *options_read_settings ()
{
	return options_read_settings_real ( 0 );
}

/*!
 * return all options as a char*
 * \return settings (free please)
 */
char *options_read_all_settings ()
{
	return options_read_settings_real ( 1 );
}


/*!
 * return all valid options names as a char*
 * \return options (free please)
 */
char *options_get_options ()
{
	char *ret = strdup ( "" );
	t_option_table *table = option_table;

	while ( table )
	{
		ret = realloc ( ret, strlen ( ret ) + strlen ( table->name ) + 10 );
		strcat ( ret, table->name );
		strcat ( ret, "\n" );

		LIST_NEXT(table);
	}

	return ret;
}

/*!
 * set all options to their default value
 * \return E_OK or E_FAIL
 */
unsigned int options_default_all ()
{
	t_option_table *table = option_table;

	while ( table )
	{
		if ( table->type == OPT_STR )
		{
			if ( table->default_string )
			{
				if ( *((char**)table->target) && !table->is_default )
					free ( *((char**)table->target) );
				*((char**)table->target) = table->default_string;
			}
			else
			{
				*((char**)table->target) = NULL;
			}
		}
		else if (!(table->type & OPT_IS_CB))
		{
			*((int*)table->target) = table->default_value;
		}

		table->is_default = 1;

		LIST_NEXT(table);
	}
	options_notify ();
	return E_OK;
}

/*!
 * set all options AS DEFAULT value
 * \return E_OK or E_FAIL
 */
unsigned int options_apply_as_default_state ()
{
	t_option_table *table = option_table;

	while ( table )
	{
		table->is_default = 1;
		LIST_NEXT(table);
	}
	options_notify ();
	return E_OK;
}

/*!
 * set option to its default value
 * \return E_OK or E_FAIL
 */
unsigned int options_set_default ( char *option )
{
	t_option_table *table = option_table;

	while ( table )
	{
		if ( !strcmp ( table->name, option ) )
		{
			if ( table->type == OPT_STR )
			{
				if ( table->default_string )
				{
					if ( *((char**)table->target) && !table->is_default )
						free ( *((char**)table->target) );
					*((char**)table->target) = table->default_string;
				}
				else
					*((char**)table->target) = NULL;
			}
			else if (!(table->type & OPT_IS_CB))
			{
				*((int*)table->target) = table->default_value;
			}

			table->is_default = 1;
		}
		LIST_NEXT(table);
	}
	options_notify ();
	return E_OK;
}

/*!
 * set the settings given in parameter
 * \param settings
 * \return E_OK or E_FAIL
 */
unsigned int options_apply_settings ( char *settings )
{
	int pos = 0;
	int err = 0;
	int val = 0;
	int charpos = 0;
	int done = 0;
	char *line = NULL;
	char *optname = NULL;
	char *optval = NULL;

	optname = malloc ( 1 );
	optval = malloc ( 1 );

	while ( !done )
	{
		line = util_get_line ( settings, pos );
		if ( line )
		{
			// maximum line size
			optname = realloc ( optname, strlen(line) );
			optval = realloc ( optval, strlen(line) );

			charpos = 0;
			while ( line[charpos] && line[charpos] != '=' && line[charpos] != ' ' )
				charpos++;
			strncpy ( optname, line, charpos );
			optname[charpos] = '\000';

			while ( line[charpos] && ( line[charpos] == ' ' || line[charpos] == '=' ) )
				charpos++;
			strcpy ( optval, line + charpos );

			switch ( options_get_type ( optname ) )
			{
				case OPT_BOOL:
					if ( sscanf ( optval, "%i", &val ) == 1 )
						options_set_bool ( optname, val );
					break;
				case OPT_INT:
					if ( sscanf ( optval, "%i", &val ) == 1 )
						options_set_int ( optname, val );
					break;
				case OPT_HEX:
					val = util_hex_get ( optval, &err );
					if ( !err )
						options_set_int ( optname, val );
					break;
				case OPT_STR:
					options_set_string ( optname, optval );
					break;
				default:
					break;
			}
		}
		else 
			done = 1;
		pos++;
	}

	options_notify ();
	return E_OK;
}

/*!
 * set the settings given in parameter, the rest set to default
 * \param the option to check, e.g. "core.plugins.armada.mode"
 * \return 1 if true or 0 if false
 */
unsigned int options_has_default ( char *option )
{
	t_option_table *table = option_table;

	// if that option doesnt exist, say it has its default
	if ( !option )
		return 1;

	while ( table )
	{
		if ( !strcmp ( table->name, option ) )
			return table->is_default;

		LIST_NEXT(table);
	}

	// if that option doesnt exist, say it has its default
	return 1;
}


/*!
 * returns the description of the given option 
 * \param the description to get, e.g. "core.plugins.armada.mode"
 * \return NULL or the string
 */
char *options_get_description ( char *option )
{
	t_option_table *table = option_table;

	if ( !option )
		return NULL;

	while ( table )
	{
		if ( !strcmp ( table->name, option ) )
			return table->description;

		LIST_NEXT(table);
	}

	return NULL;
}

/*!
 * returns the value of the given option in case of a string
 * \param the option to get, e.g. "core.plugins.armada.mode"
 * \return NULL or the string
 */
char *options_get_string ( char *option )
{
	t_option_table *table = option_table;

	if ( !option )
		return NULL;

	while ( table )
	{
		if ( !strcmp ( table->name, option ))
		{
			if(table->type == OPT_STRCB)
			{
				char *(*opt_cb) (unsigned int write, unsigned int value) = table->target;
				return opt_cb(0, 0);
			}
			else if(table->type == OPT_STR)
			{
				if ( *((char**)table->target) )
					return *((char**)table->target);
				else
					return "";
			}
		}

		LIST_NEXT(table);
	}

	return NULL;
}


/*!
 * returns the value of the given option in case of a int/bool
 * \param the option to get, e.g. "core.plugins.armada.mode"
 * \return the value
 */
unsigned int options_get_int ( char *option )
{
	t_option_table *table = option_table;

	if ( !option )
		return E_FAIL;

	while ( table )
	{
		if ( !strcmp ( table->name, option ))
		{
			if ( ((table->type & OPT_TYPE) == OPT_BOOL || (table->type & OPT_TYPE) == OPT_INT || (table->type & OPT_TYPE) == OPT_HEX) )
			{
				if(table->type & OPT_IS_CB)
				{
					unsigned int (*opt_cb) (unsigned int write, unsigned int value) = table->target;
					return opt_cb(0, 0);
				}
				else
				{
					return *((unsigned int*)table->target);
				}
			}
		}
		LIST_NEXT(table);
	}

	return E_FAIL;
}

/*!
 * returns the value of the given option in case of a int/bool
 * \param the option to get, e.g. "core.plugins.armada.mode"
 * \return the value
 */
unsigned int options_get_bool ( char *option )
{
	return options_get_int ( option );
}

/*!
 * set option to given integer value
 * \param the option to set, e.g. "core.plugins.armada.mode"
 * \param value to set
 * \return E_OK or E_FAIL
 */
unsigned int options_set_int ( char *option, unsigned int value )
{
	t_option_table *table = option_table;

	if ( !option )
		return E_FAIL;

	while ( table )
	{
		if ( !strcmp ( table->name, option ))
		{
			if ( ((table->type & OPT_TYPE) == OPT_BOOL || (table->type & OPT_TYPE) == OPT_INT || (table->type & OPT_TYPE) == OPT_HEX) )
			{
				if(table->type & OPT_IS_CB)
				{
					unsigned int (*opt_cb) (unsigned int write, unsigned int value) = table->target;
					return opt_cb(1, value);
				}
				else
				{
					*((int*)table->target) = value;
					if ( value != table->default_value )
						table->is_default = 0;
					else
						table->is_default = 1;

					options_notify ();
					return E_OK;
				}
			}
		}
		LIST_NEXT(table);
	}

	return E_FAIL;
}

/*!
 * set option to given string value
 * \param the option to set, e.g. "core.plugins.armada.mode"
 * \param value to set
 * \return E_OK or E_FAIL
 */
unsigned int options_set_string ( char *option, char *value )
{
	t_option_table *table = option_table;

	if ( !option )
		return E_FAIL;

	while ( table )
	{
		if ( !strcmp ( table->name, option ))
		{
			if ( (table->type & OPT_TYPE) == OPT_STR )
			{
				if(table->type & OPT_IS_CB)
				{
					unsigned int (*opt_cb) (unsigned int write, char *value) = table->target;
					return opt_cb(1, value);
				}
				else
				{
					// if value is not default
					if (   (!table->default_string && strlen ( value ) > 0 ) 
						|| ( table->default_string && strcmp ( value, table->default_string )) )
					{
						if ( *((char**)table->target) && !table->is_default )
							free ( *((char**)table->target) );
						*((char**)table->target) = strdup ( value );
						table->is_default = 0;
					}
					else
					{
						if ( *((char**)table->target) && !table->is_default )
							free ( *((char**)table->target) );
						*((char**)table->target) = table->default_string;
						table->is_default = 1;
					}
					options_notify ();
					return E_OK;
				}
			}
		}

		LIST_NEXT(table);
	}

	return E_FAIL;
}

/*!
 * set option to given boolean value
 * \param the option to set, e.g. "core.plugins.armada.mode"
 * \param value to set
 * \return E_OK or E_FAIL
 */
unsigned int options_set_bool ( char *option, unsigned int value )
{
	return options_set_int ( option, value );
}

/*!
 * returns the type of the given option
 * \param the option to check, e.g. "core.plugins.armada.mode"
 * \return E_FAIL or the type
 */
unsigned int options_get_type ( char *option )
{
	t_option_table *table = option_table;

	if ( !option )
		return E_FAIL;

	while ( table )
	{
		if ( !strcmp ( table->name, option ) )
			return table->type & OPT_TYPE;

		LIST_NEXT(table);
	}

	return E_FAIL;
}


unsigned int options_normalize_optionname ( char *name )
{
	int pos = 0;

	if ( !name  )
		return E_FAIL;

	while ( name[pos] )
	{
		if ( ((name[pos] & ~0x20) < 0x40 || (name[pos] & ~0x20) > 0x5B) && name[pos] != '.' )
			name[pos] = '_';
		pos++;
	}

	return E_OK;
}

unsigned int options_normalize_pluginname ( char *name )
{
	int pos = 0;

	if ( !name  )
		return E_FAIL;

	while ( name[pos] )
	{
		if ( (name[pos] & ~0x20) < 0x40 || (name[pos] & ~0x20) > 0x5B )
			name[pos] = '_';
		pos++;
	}

	return E_OK;
}

/*!
 * delete the given option from the table
 * \param optionname
 * \return E_OK or E_FAIL
 */
unsigned int options_delete_option ( char *optionname )
{
	t_option_table *opt = option_table;

	if ( !optionname || ! opt  )
		return E_FAIL;

	if ( options_get_type ( optionname ) == E_FAIL )
		return E_FAIL;

	while ( opt )
	{
		if ( !strcmp ( opt->name, optionname ) )
		{
			LIST_DEL_SIMPLE(opt, option_table);
			free (opt->name);

			if ( opt->type == OPT_STR )
			{
				if ( !opt->is_default )
					CHECK_AND_FREE ( *((char**)opt->target) );
				*((char**)opt->target) = opt->default_string;
			}
			else if(!(opt->type & OPT_IS_CB))
			{
				*((int*)opt->target) = opt->default_value;
			}

			free ( opt );
			options_notify ();
			return E_OK;
		}
		LIST_NEXT(opt);
	}

	return E_OK;
}

/*!
 * add the given option to the table
 * \param type
 * \param optionname
 * \param description
 * \param pointer to option target
 * \return E_OK or E_FAIL
 */
unsigned int options_add_option ( int type, char *optionname, char *description, void *target )
{
	t_option_table *opt = NULL;

	if ( !optionname || ! description || !target || !type || type >= OPT_LAST )
		return E_FAIL;

	if ( options_get_type ( optionname ) != E_FAIL )
		return E_FAIL;

	opt = LIST_NEW ( option_table, t_option_table );
	if ( !opt )
		return E_FAIL;


	opt->name = strdup ( optionname );
	opt->description = strdup ( description );
	opt->target = target;
	opt->type = type;

	opt->is_default = 1;

	opt->default_string = NULL;
	opt->default_value = 0;

	if ( type == OPT_STR )
	{
		if ( !(*((char**)target)) )
			opt->default_string = NULL;
		else
			opt->default_string = *((char**)target);
	}
	else
		opt->default_value = *((int*)target);


	if ( !option_table )
		option_table = opt;

	options_notify ();
	return E_OK;
}

/*!
 * add the options provided by the script
 * \param scriptname
 * \param pointer to options struct
 * \return E_OK or E_FAIL
 */
unsigned int
options_import_script_options ( char *script, t_options *opt )
{
	int pos = 0;
	char *optionname = strdup ( "" );
	char *scriptname = NULL;

	if ( !script )
		scriptname = strdup ( "unnamed_script" );
	else
		scriptname = strdup ( script );

	options_normalize_pluginname ( scriptname );

	if ( !opt )
	{
		free ( scriptname );
		free ( optionname );
		return E_FAIL;
	}
	while ( opt[pos].type && opt[pos].type < OPT_LAST )
	{
		optionname = realloc ( optionname, strlen ( scriptname ) + strlen ( opt[pos].name ) + 15 );
		sprintf ( optionname, "scripts.%s.%s", scriptname, opt[pos].name );
		options_add_option ( opt[pos].type, optionname, opt[pos].description, opt[pos].target );
		pos++;
	}
	free ( scriptname );
	free ( optionname );

	return E_OK;
}


/*!
 * delete the options provided by the script
 * \param scriptname
 * \param pointer to options struct
 * \return E_OK or E_FAIL
 */
unsigned int
options_delete_script_options ( char *script, t_options *opt )
{
	int pos = 0;
	char *optionname = strdup ( "" );
	char *scriptname = NULL;

	if ( !script )
		scriptname = strdup ( "unnamed_script" );
	else
		scriptname = strdup ( script );

	options_normalize_pluginname ( scriptname );

	if ( !opt )
	{
		free ( scriptname );
		free ( optionname );
		return E_FAIL;
	}
	while ( opt[pos].type && opt[pos].type < OPT_LAST )
	{

		optionname = realloc ( optionname, strlen ( scriptname ) + strlen ( opt[pos].name ) + 15 );
		sprintf ( optionname, "scripts.%s.%s", scriptname, opt[pos].name );
		options_delete_option ( optionname );

		pos++;
	}
	free ( scriptname );
	free ( optionname );

	return E_OK;
}
/////////////////////////

/*!
 * add some internal variable, used e.g. for communication between scripts
 * \param type
 * \param sysvar the variable name
 * \param description if not NULL, a char* to description
 * \param defaultval if not NULL, the defaultvalue
 * \return E_OK or E_FAIL
 */
unsigned int
options_add_sysvar ( int type, char *sysvar, char *description, void *defaultval )
{
	int pos = 0;
	char *optionname = NULL;
	static char *sysvarprefix =  "sysvar.";
	void *target = NULL;

	if ( !sysvar )
		return E_FAIL;

	optionname = malloc ( strlen ( sysvar )  + strlen ( sysvarprefix ) + 1 );
	sprintf ( optionname, "%s%s", sysvarprefix, sysvar );
	options_normalize_optionname ( optionname );

	switch ( type )
	{
		case OPT_BOOL:
		case OPT_INT:
		case OPT_HEX:
			target = malloc ( sizeof ( int ) );
			*((unsigned int*)target) = (unsigned int) defaultval;
			break;

		case OPT_STR:
			target = malloc ( sizeof ( char * ) );
			if ( description )
				*((char**)target) = strdup ( ((char*)defaultval) );
			else
				*((char**)target) = NULL;

			break;
		default:
			return E_FAIL;
			break;
	}

	if ( options_add_option ( type, optionname, description, target ) != E_OK )
	{
		free ( target );
		return E_FAIL;
	}

	return E_OK;
}

/*!
 * add the options provided by the plugin
 * \param pluginname
 * \param pointer to options struct
 * \return E_OK or E_FAIL
 */
unsigned int
options_import_plugin_options ( char *plugin, t_options *opt )
{
	int pos = 0;
	char *optionname = strdup ( "" );
	char *pluginname = NULL;

	if ( !plugin )
		pluginname = strdup ( "unnamed_script" );
	else
		pluginname = strdup ( plugin );

	options_normalize_pluginname ( pluginname );

	if ( !opt )
	{
		free ( pluginname );
		free ( optionname );
		return E_FAIL;
	}

	while ( opt[pos].type && opt[pos].type < OPT_LAST )
	{

		optionname = realloc ( optionname, strlen ( pluginname ) + strlen ( opt[pos].name ) + 15 );
		sprintf ( optionname, "plugins.%s.%s", pluginname, opt[pos].name );
		options_add_option ( opt[pos].type, optionname, opt[pos].description, opt[pos].target );
		pos++;
	}

	return E_OK;
}


/*!
 * delete the options provided by the plugin
 * \param pluginname
 * \param pointer to options struct
 * \return E_OK or E_FAIL
 */
unsigned int
options_delete_plugin_options ( char *plugin, t_options *opt )
{
	int pos = 0;
	char *optionname = strdup ( "" );
	char *pluginname = NULL;

	if ( !plugin )
		pluginname = strdup ( "unnamed_script" );
	else
		pluginname = strdup ( plugin );

	options_normalize_pluginname ( pluginname );


	if ( !opt )
	{
		free ( pluginname );
		free ( optionname );
		return E_FAIL;
	}

	while ( opt[pos].type && opt[pos].type < OPT_LAST )
	{
		optionname = realloc ( optionname, strlen ( pluginname ) + strlen ( opt[pos].name ) + 15 );
		sprintf ( optionname, "plugins.%s.%s", pluginname, opt[pos].name );
		options_delete_option ( optionname );

		pos++;
	}
	free ( pluginname );
	free ( optionname );


	return E_OK;
}


/*!
 * set notifier in case of an changed option
 * \param target function in form 'void target (void *object)' to call
 * \param object the parameter to the function
 * \return E_OK 
 */
unsigned int options_set_notification ( void *target, void* object )
{
	options_notification_target = target;
	options_notification_object = object;

	return E_OK;
}

/*!
 * notify registered function
 */
void options_notify ()
{
	if ( options_notification_target )
		options_notification_target ( options_notification_object );

}

/*!
 * load option file
 * \param filename of options fil
 * \return E_OK 
 */
unsigned int options_load ( unsigned char *file )
{
	unsigned char *options = NULL;
	t_workspace *ws = NULL;

	ws = workspace_startup ( file );

	if ( !ws || v_get_size(ws) == E_FAIL )
		return E_FAIL;

	options = malloc ( v_get_size(ws) + 2 );
	if ( !options )
		return E_FAIL;

	v_memcpy_get ( ws, options, v_get_start(ws), v_get_size(ws) );
	options[v_get_size(ws)] = '\000';

	return options_apply_settings ( options );
}

/*!
 * load default options
 */
unsigned int options_init ()
{
	workspace_skip_error ( 1 );
	options_load ( ".trix.conf" );
	options_load ( "trix.conf" );
	workspace_skip_error ( 0 );
	options_apply_as_default_state ();

	return E_OK;
}

#endif
