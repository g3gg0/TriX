
#ifndef __TRIX_TRIXPLUG_MUTEX_H__
#define __TRIX_TRIXPLUG_MUTEX_H__

/* trixplug struct */

struct mutex_funcs
{
	t_mutex *(*create) ( );
	unsigned int (*locked) ( t_mutex * mutex);
	void (*lock) ( t_mutex * mutex);
	void (*release) ( t_mutex * mutex);
	void (*unlock) ( t_mutex * mutex);
};

/* trixplug struct initializer */

#define MUTEX_PLUG_INIT \
extern struct trix_functions *ft;\
struct mutex_funcs mutex_functions;\
unsigned int mutex_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->mutex = &mutex_functions;\
	ft->mutex->create = mutex_create;\
	ft->mutex->release = mutex_release;\
	ft->mutex->lock = mutex_lock;\
	ft->mutex->locked = mutex_locked;\
	ft->mutex->unlock = mutex_unlock;\
\
	return E_OK;\
}

#endif
