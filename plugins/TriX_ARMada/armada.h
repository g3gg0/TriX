#ifndef __TRIX_ARMADA_H__
#define __TRIX_ARMADA_H__

#include "trixplug.h"

TRIXPLUGIN_API unsigned int trixplugin_init ( struct trix_functions *callbacks );
TRIXPLUGIN_API char *trixplugin_name ( void );
TRIXPLUGIN_API unsigned int trixplugin_vers ( void );


#define ARMADA_HEADERS "import unsigned int __armada_compile ( t_workspace *ws, unsigned char *source, int output );\n"\
    "import unsigned int __armada_initialize ( );\n"\
    "import unsigned int __armada_get_patched_bytes ( );\n"\
	"unsigned int ARMada_compile ( unsigned char *source, int output ) { return __armada_compile ( GetWorkspace (), source, output ): }\n"\
	"unsigned int ARMada_inject_file ( char *file ) { \
		unsigned char *arm_code = NULL; \
		t_workspace *arm_file = WorkspaceStartup ( file );\
		if ( !arm_file ) return E_FAIL;\
		arm_code = strdup ( __v_get_str ( arm_file, 0x00 ) );\
		return ARMada_compile ( arm_code, 1 );\
	}"\


#define ARMADA_SYMBOLS \
	REGISTER_SYMBOL ( armada_compile );\
	REGISTER_SYMBOL ( armada_get_patched_bytes );\
    REGISTER_SYMBOL ( armada_initialize );\

unsigned int armada_compile ( t_workspace *ws, unsigned char *source, int output );
unsigned int armada_initialize ( );
unsigned int armada_init ( );
unsigned int armada_get_patched_bytes ( );

#endif

