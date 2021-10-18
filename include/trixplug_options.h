
#ifndef __TRIX_TRIXPLUG_OPTIONS_H__
#define __TRIX_TRIXPLUG_OPTIONS_H__

/* trixplug struct */

struct options_funcs
{
	char *(*get_description) ( char *option);
	char *(*get_options) ( );
	char *(*get_string) ( char *option);
	char *(*read_all_settings) ( );
	char *(*read_settings) ( );
	char *(*read_settings_real) ( unsigned int all);
	unsigned int (*add_option) ( int type, char *optionname, char *description, void *target);
	unsigned int (*add_sysvar) ( int type, char *sysvar, char *description, void *defaultval);
	unsigned int (*apply_as_default_state) ( );
	unsigned int (*apply_settings) ( char *settings);
	unsigned int (*default_all) ( );
	unsigned int (*delete_option) ( char *optionname);
	unsigned int (*delete_plugin_options) ( char *plugin, t_options * opt);
	unsigned int (*delete_script_options) ( char *script, t_options * opt);
	unsigned int (*get_bool) ( char *option);
	unsigned int (*get_int) ( char *option);
	unsigned int (*get_type) ( char *option);
	unsigned int (*has_default) ( char *option);
	unsigned int (*import_plugin_options) ( char *plugin, t_options * opt);
	unsigned int (*import_script_options) ( char *script, t_options * opt);
	unsigned int (*init) ( );
	unsigned int (*load) ( unsigned char *file);
	unsigned int (*normalize_optionname) ( char *name);
	unsigned int (*normalize_pluginname) ( char *name);
	unsigned int (*set_bool) ( char *option, unsigned int value);
	unsigned int (*set_default) ( char *option);
	unsigned int (*set_int) ( char *option, unsigned int value);
	unsigned int (*set_notification) ( void *target, void *object);
	unsigned int (*set_string) ( char *option, char *value);
	void (*notify) ( );
};

/* trixplug struct initializer */

#define OPTIONS_PLUG_INIT \
extern struct trix_functions *ft;\
struct options_funcs options_functions;\
unsigned int options_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->options = &options_functions;\
	ft->options->read_settings_real = options_read_settings_real;\
	ft->options->read_settings = options_read_settings;\
	ft->options->read_all_settings = options_read_all_settings;\
	ft->options->get_options = options_get_options;\
	ft->options->default_all = options_default_all;\
	ft->options->apply_as_default_state = options_apply_as_default_state;\
	ft->options->set_default = options_set_default;\
	ft->options->apply_settings = options_apply_settings;\
	ft->options->has_default = options_has_default;\
	ft->options->get_description = options_get_description;\
	ft->options->get_string = options_get_string;\
	ft->options->get_int = options_get_int;\
	ft->options->get_bool = options_get_bool;\
	ft->options->set_int = options_set_int;\
	ft->options->set_string = options_set_string;\
	ft->options->set_bool = options_set_bool;\
	ft->options->get_type = options_get_type;\
	ft->options->normalize_optionname = options_normalize_optionname;\
	ft->options->normalize_pluginname = options_normalize_pluginname;\
	ft->options->delete_option = options_delete_option;\
	ft->options->add_option = options_add_option;\
	ft->options->import_script_options = options_import_script_options;\
	ft->options->delete_script_options = options_delete_script_options;\
	ft->options->add_sysvar = options_add_sysvar;\
	ft->options->import_plugin_options = options_import_plugin_options;\
	ft->options->delete_plugin_options = options_delete_plugin_options;\
	ft->options->set_notification = options_set_notification;\
	ft->options->notify = options_notify;\
	ft->options->load = options_load;\
	ft->options->init = options_init;\
\
	return E_OK;\
}

#endif
