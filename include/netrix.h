#ifndef __TRIX_NETRIX_H__
#define __TRIX_NETRIX_H__

#include "trixplug_netrix.h"


char *netrix_get ( char *url );
unsigned int netrix_execute ( char *url );
unsigned int netrix_check ( char *url );
t_fileinfo *netrix_open ( const char *url );

#endif
