
#ifndef __TRIX_TRIXPLUG_NETRIX_H__
#define __TRIX_TRIXPLUG_NETRIX_H__

/* trixplug struct */

struct netrix_funcs
{
	char *(*get) ( char *url);
	t_fileinfo *(*open) ( const char *url);
	unsigned int (*check) ( char *url);
	unsigned int (*execute) ( char *url);
};

/* trixplug struct initializer */

#define NETRIX_PLUG_INIT \
extern struct trix_functions *ft;\
struct netrix_funcs netrix_functions;\
unsigned int netrix_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->netrix = &netrix_functions;\
	ft->netrix->check = netrix_check;\
	ft->netrix->open = netrix_open;\
	ft->netrix->execute = netrix_execute;\
	ft->netrix->get = netrix_get;\
\
	return E_OK;\
}

#endif
