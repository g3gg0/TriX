#ifndef __TRIX_MUTEX_C__
#define __TRIX_MUTEX_C__

#ifdef WIN32
#include "windows.h"
#else
#include <pthread.h>
typedef pthread_mutex_t CRITICAL_SECTION;
#endif

#include "defines.h"
#include "mutex.h"
#include "trixplug_funcs.h"

MUTEX_PLUG_INIT;

CRITICAL_SECTION *mutex_cs = NULL;

t_mutex *mutex_create ( )
{
	unsigned int *mutex = NULL;

	if ( !mutex_cs )
	{
		mutex_cs = mem_allocate_passthru ( sizeof (CRITICAL_SECTION ) );
		if ( !mutex_cs )
			return NULL;
#ifdef WIN32
		InitializeCriticalSection( mutex_cs );
#else
		 pthread_mutex_init ( mutex_cs, NULL );
#endif
	}
	mutex = mem_allocate_passthru ( sizeof ( int ) );
	if ( !mutex )
		return NULL;

	*mutex = MUTEX_UNLOCKED;

	return mutex;
}

void mutex_release ( t_mutex *mutex )
{
	if ( !mutex )
		return;

	mem_release_passthru ( mutex );
}

void mutex_lock ( t_mutex *mutex )
{
	unsigned int unlocked = 0;

	if ( !mutex )
		return;

	while ( !unlocked )
	{
#ifdef WIN32
		EnterCriticalSection ( mutex_cs );
#else
		pthread_mutex_lock ( mutex_cs );
#endif

		if ( *mutex == MUTEX_UNLOCKED )
		{
			unlocked = 1;
			*mutex = MUTEX_LOCKED;
		}
#ifdef WIN32
		LeaveCriticalSection ( mutex_cs );
		if ( !unlocked )
			Sleep ( 100 );
#else
		pthread_mutex_unlock ( mutex_cs );
		if ( !unlocked )
			usleep ( 10000 );
#endif
	}

	return;

}

unsigned int mutex_locked ( t_mutex *mutex )
{
	unsigned int locked = 1;

	if ( !mutex )
		return 0;
#ifdef WIN32
	EnterCriticalSection ( mutex_cs );
#endif

	if ( *mutex == MUTEX_UNLOCKED )
		locked = 0;
#ifdef WIN32
	LeaveCriticalSection ( mutex_cs );
#endif

	return locked;
}

void mutex_unlock ( t_mutex *mutex )
{
	if ( !mutex )
		return;

#ifdef WIN32
	EnterCriticalSection ( mutex_cs );
#endif

	*mutex = MUTEX_UNLOCKED;

#ifdef WIN32
	LeaveCriticalSection ( mutex_cs );
#endif
}

#endif
