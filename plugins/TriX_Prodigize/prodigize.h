#ifndef __TRIX_PRODIGIZE_H__
#define __TRIX_PRODIGIZE_H__

#include "trixplug.h"


unsigned int prodigize_init ( );
unsigned int prodigize_open ( );
unsigned int prodigize_close ( );
unsigned int prodigize_check_bootrom ( unsigned char *buf );
unsigned int prodigize_boot_2nd ( unsigned char *config, unsigned char *loader, unsigned int length );
unsigned int prodigize_boot_3rd ( unsigned char *loader, unsigned int length );
unsigned int prodigize_send_data (  unsigned char *buffer, unsigned int bytes );
unsigned int prodigize_read_data (  unsigned char *buffer, unsigned int bytes );
unsigned int prodigize_set_speeds ();

#define PRODIGIZE_HEADERS "\
import unsigned int __prodigize_open ( );\
import unsigned int __prodigize_close ( );\
import unsigned int __prodigize_check_bootrom ( unsigned char *buf );\
import unsigned int __prodigize_boot_2nd ( unsigned char *config, unsigned char *loader, unsigned int length );\
import unsigned int __prodigize_boot_3rd ( unsigned char *loader, unsigned int length );\
import unsigned int __prodigize_send_data (  unsigned char *buffer, unsigned int bytes );\
import unsigned int __prodigize_read_data (  unsigned char *buffer, unsigned int bytes );\
import unsigned int __prodigize_set_speeds ();\
"

#define PRODIGIZE_SYMBOLS \
	REGISTER_SYMBOL ( prodigize_open ); \
	REGISTER_SYMBOL ( prodigize_close ); \
	REGISTER_SYMBOL ( prodigize_check_bootrom ); \
	REGISTER_SYMBOL ( prodigize_boot_2nd ); \
	REGISTER_SYMBOL ( prodigize_boot_3rd ); \
	REGISTER_SYMBOL ( prodigize_send_data ); \
	REGISTER_SYMBOL ( prodigize_read_data ); \
	REGISTER_SYMBOL ( prodigize_set_speeds ); \


#endif

