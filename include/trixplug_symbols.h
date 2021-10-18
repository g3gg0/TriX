
#ifndef __TRIX_TRIXPLUG_SYMBOLS_H__
#define __TRIX_TRIXPLUG_SYMBOLS_H__

/* trixplug struct */

struct symbols_funcs
{
	t_symbols *(*get) ( t_stage * stage, const char *name);
	unsigned int (*add) ( t_stage * stage, const char *name, const int type, const char *data);
	unsigned int (*count) ( t_stage * stage, const int type);
	unsigned int (*free_all) ( t_stage * stage);
	unsigned int (*get_address) ( t_stage * stage, const char *name);
	unsigned int (*remove) ( t_stage * stage, const char *name);
	void (*dump) ( t_stage * stage);
};

/* trixplug struct initializer */

#define SYMBOLS_PLUG_INIT \
extern struct trix_functions *ft;\
struct symbols_funcs symbols_functions;\
unsigned int symbols_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->symbols = &symbols_functions;\
	ft->symbols->dump = symbols_dump;\
	ft->symbols->add = symbols_add;\
	ft->symbols->remove = symbols_remove;\
	ft->symbols->free_all = symbols_free_all;\
	ft->symbols->get = symbols_get;\
	ft->symbols->get_address = symbols_get_address;\
	ft->symbols->count = symbols_count;\
\
	return E_OK;\
}

#endif
