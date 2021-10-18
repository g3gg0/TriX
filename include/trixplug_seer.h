
#ifndef __TRIX_TRIXPLUG_SEER_H__
#define __TRIX_TRIXPLUG_SEER_H__

/* trixplug struct */

struct seer_funcs
{
	char *(*str) ( char *str);
	unsigned int (*add_header) ( unsigned char *section, unsigned char *header);
	unsigned int (*add_symbol_int) ( char *name, void *func);
	unsigned int (*del_header) ( unsigned char *section);
	unsigned int (*del_symbol_int) ( char *name);
	unsigned int (*execute) ( scScript sc);
	unsigned int (*handle_include) ( char **script);
	unsigned int (*init) ( );
	unsigned int (*progresscallback) ( int percent);
	unsigned int (*run) ( char *script);
	void (*MyDispatcher) ( int *result, void *function, int *params, int paramcount, unsigned int options);
};

/* trixplug struct initializer */

#define SEER_PLUG_INIT \
extern struct trix_functions *ft;\
struct seer_funcs seer_functions;\
unsigned int seer_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->seer = &seer_functions;\
	ft->seer->MyDispatcher = MyDispatcher;\
	ft->seer->str = str;\
	ft->seer->init = seer_init;\
	ft->seer->execute = seer_execute;\
	ft->seer->progresscallback = seer_progresscallback;\
	ft->seer->handle_include = seer_handle_include;\
	ft->seer->run = seer_run;\
	ft->seer->add_header = seer_add_header;\
	ft->seer->add_symbol_int = seer_add_symbol_int;\
	ft->seer->del_header = seer_del_header;\
	ft->seer->del_symbol_int = seer_del_symbol_int;\
\
	return E_OK;\
}

#endif
