#ifndef __TRIX_HWEMU_H__
#define __TRIX_HWEMU_H__

#include "trixplug.h"

TRIXPLUGIN_API unsigned int trixplugin_init ( struct trix_functions *callbacks );
TRIXPLUGIN_API char *trixplugin_name ( void );
TRIXPLUGIN_API unsigned int trixplugin_vers ( void );



#define HWEMU_HEADERS \
	"import unsigned int __hwemu_register ( t_workspace *ws, unsigned int address );\n" \


#define HWEMU_SYMBOLS \
	REGISTER_SYMBOL ( hwemu_register );\
	REGISTER_SYMBOL ( hwemu_handler );\

unsigned int hwemu_init ( );
unsigned int hwemu_register ( t_workspace *ws, unsigned int address );
unsigned int hwemu_handler ( t_workspace *ws, unsigned int address, unsigned char type, unsigned int value );

#endif

