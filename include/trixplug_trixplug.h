
#ifndef __TRIX_TRIXPLUG_TRIXPLUG_H__
#define __TRIX_TRIXPLUG_TRIXPLUG_H__

/* trixplug struct */

struct trixplug_funcs
{
	t_plugin_entry *(*get_plugin_entry) ( unsigned char *name);
	t_plugin_entry *(*get_plugins) ( );
	t_plugin_info *(*get) ( t_plugin_info * info, int type);
	unsigned int (*add_plugin_header) ( t_plugin_info * trix_plugin_info, unsigned char *section, unsigned char *header);
	unsigned int (*add_plugin_symbol) ( t_plugin_info * trix_plugin_info, char *name, void *func);
	unsigned int (*init) ( );
	unsigned int (*load_plugin) ( char *name);
	unsigned int (*set_notification) ( void *target, void *object);
	unsigned int (*unload_plugin) ( unsigned char *name);
	void (*notify) ( );
	void *(*get_plugin_symbol) ( char *name, char *function);
 	void *(*get_global_plugin_symbol) ( char *function);
};

/* trixplug struct initializer */

#define TRIXPLUG_PLUG_INIT \
extern struct trix_functions *ft;\
struct trixplug_funcs trixplug_functions;\
unsigned int trixplug_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->trixplug = &trixplug_functions;\
	ft->trixplug->get = trixplug_get;\
	ft->trixplug->load_plugin = trixplug_load_plugin;\
	ft->trixplug->get_plugin_entry = trixplug_get_plugin_entry;\
	ft->trixplug->unload_plugin = trixplug_unload_plugin;\
	ft->trixplug->add_plugin_header = trixplug_add_plugin_header;\
	ft->trixplug->add_plugin_symbol = trixplug_add_plugin_symbol;\
	ft->trixplug->get_global_plugin_symbol = trixplug_get_global_plugin_symbol;\
	ft->trixplug->get_plugin_symbol = trixplug_get_plugin_symbol;\
	ft->trixplug->get_plugins = trixplug_get_plugins;\
	ft->trixplug->set_notification = trixplug_set_notification;\
	ft->trixplug->notify = trixplug_notify;\
	ft->trixplug->init = trixplug_init;\
\
	return E_OK;\
}

#endif
