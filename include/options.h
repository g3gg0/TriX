#ifndef __TRIX_OPTIONS_H__
#define __TRIX_OPTIONS_H__

#define OPT_BOOL   0x0001
#define OPT_INT    0x0002
#define OPT_HEX    0x0003
#define OPT_STR    0x0004
#define OPT_LAST   0x0005
#define OPT_END    { 0, NULL, NULL, NULL }

#define OPT_BEGIN     t_options script_options[] = {
#define OPT_FINISH    OPT_END }; 

#define OPTION(t,d,n,g)  { t, d, n, g },

typedef struct s_options t_options;
struct s_options
{
	int type;
	char *description;
	char *name;
	void *target;
};

typedef struct s_option_table t_option_table;
struct s_option_table
{
	STRUCT_HEADER_LIST(t_option_table);
	char *name;
	char *description;
	void *target;

	int is_default;

	int type;

	unsigned char *default_string;
	int default_value;
};

#include "trixplug_options.h"

char *options_read_settings_real ( unsigned int all );
char *options_read_settings ( );
char *options_read_all_settings ( );
char *options_get_options ( );
unsigned int options_default_all ( );
unsigned int options_set_default ( char *option );
unsigned int options_apply_as_default_state ( );
unsigned int options_apply_settings ( char *settings );
unsigned int options_has_default ( char *option );
char *options_get_string ( char *option );
unsigned int options_get_int ( char *option );
unsigned int options_get_bool ( char *option );
unsigned int options_set_int ( char *option, unsigned int value );
unsigned int options_set_string ( char *option, char *value );
unsigned int options_set_bool ( char *option, unsigned int value );
unsigned int options_get_type ( char *option );
unsigned int options_normalize_pluginname ( char *name );
unsigned int options_normalize_optionname ( char *name );
unsigned int options_delete_option ( char *optionname );
unsigned int options_add_option ( int type, char *optionname, char *description, void *target );
unsigned int options_import_script_options ( char *scriptname, t_options * opt );
unsigned int options_delete_script_options ( char *scriptname, t_options * opt );
unsigned int options_import_plugin_options ( char *pluginname, t_options * opt );
unsigned int options_delete_plugin_options ( char *pluginname, t_options * opt );
char *options_get_description ( char *option );
unsigned int options_add_sysvar ( int type, char *sysvar, char *description, unsigned int defaultval );
unsigned int options_set_notification ( void *target, void* object );
void options_notify ();
unsigned int options_load ( unsigned char *file );
unsigned int options_init ();

#define options_add_core_option( type, module, variable, desc ) \
	options_add_option ( type, "core." module "." #variable, desc, &variable );



#endif
