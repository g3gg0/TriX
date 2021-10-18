#ifndef __TRIX_TERM_H__
#define __TRIX_TERM_H__

#include "trixplug.h"

TRIXPLUGIN_API unsigned int trixplugin_init ( struct trix_functions *callbacks );
TRIXPLUGIN_API char *trixplugin_name ( void );
TRIXPLUGIN_API unsigned int trixplugin_vers ( void );


#define TERM_HEADERS "import int __term_open ( unsigned int port );\n"\
    "import unsigned int __term_close ( );\n"\
    "import unsigned int __term_set_control ( unsigned int baud, unsigned char databits, unsigned char stopbits, unsigned char parity, unsigned char handshake );\n\n"\
    "import unsigned int __term_send ( const unsigned char *data, const unsigned int bytes );\n"\
    "import unsigned int __term_receive ( unsigned char *dest, const unsigned int bytes  );\n\n"\

#define TERM_SYMBOLS seer_add_symbol ( term_open );\
    seer_add_symbol ( term_close );\
    seer_add_symbol ( term_set_control );\
    seer_add_symbol ( term_send );\
    seer_add_symbol ( term_receive );\

unsigned int term_open ( unsigned int port );
unsigned int term_close ();
unsigned int term_set_control ( unsigned int baud, unsigned char databits, unsigned char stopbits, unsigned char parity, unsigned char handshake );
unsigned int term_send ( const unsigned char *data, const unsigned int bytes );
unsigned int term_receive ( unsigned char *dest, const unsigned int bytes );
unsigned int term_init ( );

#endif

