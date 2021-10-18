#ifndef __TRIX_TRIXPLUG_H__
#define __TRIX_TRIXPLUG_H__

#if defined (WIN32) && defined(TRIXPLUG_DL_MEMORY)
#define TRIXPLUGIN_API __declspec(dllexport)
#define DL_OPEN MemoryLoadLibrary
#define DL_CLOSE MemoryFreeLibrary
#define DL_SYM  MemoryGetProcAddress
#elif defined (WIN32)
#define TRIXPLUGIN_API __declspec(dllexport)
#define DL_OPEN LoadLibrary
#define DL_CLOSE FreeLibrary
#define DL_SYM  GetProcAddress
#else
#define TRIXPLUGIN_API
#define DL_OPEN(x)   dlopen (x,RTLD_LAZY)
#define DL_CLOSE     dlclose
#define DL_SYM       dlsym
#endif

#define PLUGIN_ERROR(msg, val) do { printf ( "[e] %s (%s:%i)\n", msg, __FUNCTION__, __LINE__ ); return val; } while (0)


/* Interface versions.

	Major interface version must exactly be the same.
	Minor of the plugin may be lower than TriX's version.
#define TRIXPLUG_MAJOR  0x0002
#define TRIXPLUG_MINOR  0x0001
*/
#include "trixplug_version.h"



#define TRIXPLUG_IFACE_MAJOR(x)  ((x & 0xFFFF0000)>>16)
#define TRIXPLUG_IFACE_MINOR(x)  (x & 0x0000FFFF)
#define TRIXPLUG_INTERFACE         ((TRIXPLUG_MAJOR<<16) | TRIXPLUG_MINOR )

#define E_PLUGIN_INTERFACE_FAILURE -2
#define E_PLUGIN_ALREADY_LOADED  -3
#define E_PLUGIN_INVALID         -4
#define E_PLUGIN_UNKNOWN         -5

typedef unsigned int (*dll_init_func) (  );
typedef unsigned int (*dll_cleanup_func) (  );
typedef unsigned char *(*dll_lasterror_func) (  );

#define PLUGIN_INFO_NAME       0x0001
#define PLUGIN_INFO_INIT       0x0002
#define PLUGIN_INFO_CLEANUP    0x0003
#define PLUGIN_INFO_OPTIONS    0x0004
#define PLUGIN_INFO_FUNCS      0x0005
#define PLUGIN_INFO_INTERFACE  0x0006
#define PLUGIN_INFO_VERSION    0x0007
#define PLUGIN_INFO_LASTERROR  0x0008
#define PLUGIN_INFO_END        { 0, NULL, NULL }

#define PLUGIN_INFO_FINISH      PLUGIN_INFO_END }; 
#define PLUGIN_INFO_BEGIN      \
struct trix_functions *ft = 0;  \
t_plugin_info trix_plugin_info[] = { \
PLUGIN_INTERFACE \
PLUGIN_FUNCTIONS(ft)

#define PLUGIN_NAME(name)			{ PLUGIN_INFO_NAME, (void*)name, NULL },
#define PLUGIN_VERSION(major,minor) { PLUGIN_INFO_VERSION, (void*)major, (void*)minor },
#define PLUGIN_INIT(func)			{ PLUGIN_INFO_INIT, (void*)func, (void*)#func },
#define PLUGIN_CLEANUP(func)		{ PLUGIN_INFO_CLEANUP, (void*)func, (void*)#func },
#define PLUGIN_LASTERROR(func)   	{ PLUGIN_INFO_LASTERROR, (void*)func, (void*)#func },
#define PLUGIN_OPTION(opt)			{ PLUGIN_INFO_OPTIONS, (void*)opt, (void*)#opt },
#define PLUGIN_FUNCTIONS(ft)		{ PLUGIN_INFO_FUNCS, (void*)&ft, (void*)#ft },
#define PLUGIN_INTERFACE			{ PLUGIN_INFO_INTERFACE, (void*)TRIXPLUG_MAJOR, (void*)TRIXPLUG_MINOR },
#define PLUGIN_OPTIONS				PLUGIN_OPTION(script_options)

typedef struct s_plugin_info t_plugin_info;
struct s_plugin_info
{
	int type;
	void *param1;
	void *param2;
};

typedef struct s_seer_dict t_seer_dict;
struct s_seer_dict
{
	STRUCT_HEADER_LIST(t_seer_dict);
	char *name;
	void *value;
};


typedef struct s_plugin_entry t_plugin_entry;
struct s_plugin_entry
{
	STRUCT_HEADER_LIST(t_plugin_entry);
	t_plugin_info *info;
	char *pluginname;
	char *filename;
	int loaded;
	void *handle;
	t_seer_dict *headers;
	t_seer_dict *symbols;
};

#define TRIXPLUG_STUBS 

#define REGISTER_SYMBOL(a)   trixplug_add_plugin_symbol(trix_plugin_info,#a,&a)
#define REGISTER_HEADER(s,h) trixplug_add_plugin_header(trix_plugin_info,s,h)

#include "trixplug_trixplug.h"
#include "trixplug_funcs.h"


t_plugin_info *trixplug_get ( t_plugin_info *info, int type ) ;
t_plugin_entry *trixplug_get_plugin_entry ( unsigned char *name );
unsigned int trixplug_load_plugin ( char *name );
unsigned int trixplug_unload_plugin ( unsigned char *name );
unsigned int trixplug_add_plugin_header ( t_plugin_info *trix_plugin_info, unsigned char *section, unsigned char *header );
unsigned int trixplug_add_plugin_symbol ( t_plugin_info *trix_plugin_info, char *name, void *func );
void *trixplug_get_global_plugin_symbol ( char *function );
void *trixplug_get_plugin_symbol ( char *plugin, char *function );
t_plugin_entry *trixplug_get_plugins ();
unsigned int trixplug_set_notification ( void *target, void* object );
void trixplug_notify ( );
unsigned int trixplug_init ( );
unsigned int trixplug_handle_load ( unsigned char **plugin, unsigned int length, unsigned char *desc );

#endif

