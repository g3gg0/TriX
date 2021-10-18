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

#define TERM_SYMBOLS \
    REGISTER_SYMBOL ( term_open );\
    REGISTER_SYMBOL ( term_close );\
    REGISTER_SYMBOL ( term_set_control );\
    REGISTER_SYMBOL ( term_send );\
    REGISTER_SYMBOL ( term_receive );\

unsigned int term_open ( unsigned int port );
unsigned int term_close ();
unsigned int term_set_control ( unsigned int baud, unsigned char databits, unsigned char stopbits, unsigned char parity, unsigned char handshake );
unsigned int term_send ( const unsigned char *data, const unsigned int bytes );
unsigned int term_receive ( unsigned char *dest, const unsigned int bytes );
unsigned int term_init ( );

#endif

