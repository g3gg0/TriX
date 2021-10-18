#ifndef __TRIX_MUTEX_H__
#define __TRIX_MUTEX_H__

typedef unsigned int t_mutex;
#define MUTEX_UNLOCKED 0
#define MUTEX_LOCKED   1

#include "trixplug_mutex.h"

t_mutex *mutex_create ( );
void mutex_release ( t_mutex *mutex );
void mutex_lock ( t_mutex *mutex );
unsigned int mutex_locked ( t_mutex *mutex );
void mutex_unlock ( t_mutex *mutex );



#endif
