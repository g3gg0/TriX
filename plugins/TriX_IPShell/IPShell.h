#ifndef __TRIX_IPSHELL_H__
#define __TRIX_IPSHELL_H__

#include "trixplug.h"

TRIXPLUGIN_API unsigned int trixplugin_init ( struct trix_functions *callbacks );
TRIXPLUGIN_API char *trixplugin_name ( void );
TRIXPLUGIN_API unsigned int trixplugin_vers ( void );



#define IPSHELL_HEADERS \
	""

#define IPSHELL_SYMBOLS \
	REGISTER_SYMBOL ( ipshell_vprintf_cb );\
	REGISTER_SYMBOL ( ipshell_fgets_cb );\

unsigned int ipshell_init ( );
unsigned int ipshell_connect ( );
unsigned int ipshell_disconnect ( );
unsigned int ipshell_cleanup ( );

int ipshell_vprintf_cb ( const char *format, va_list args );
char *ipshell_fgets_cb ( char * buffer, int length );
int ipshell_convert_newline ( unsigned char **string, unsigned int length );

#endif

