#ifndef __TRIX_NOKIACRYPT_H__
#define __TRIX_NOKIACRYPT_H__

#include "trixplug.h"


#define NOKIACRYPT_HEADERS "\
import unsigned int __nokiacrypt_decode_splock ( unsigned char *imei, unsigned char *msid );\
import unsigned int __nokiacrypt_decode_msid ( unsigned char *msid );\
import unsigned int __nokiacrypt_decode_imei ( unsigned char *imei, unsigned char *msid );\
import unsigned int __nokiacrypt_decode_faid ( unsigned char *faid, unsigned char *msid );\
import unsigned int __nokiacrypt_encode_faid ( unsigned char *faid, unsigned char *msid );\
"

#define NOKIACRYPT_SYMBOLS \
	REGISTER_SYMBOL ( nokiacrypt_decode_splock ); \
	REGISTER_SYMBOL ( nokiacrypt_decode_msid ); \
	REGISTER_SYMBOL ( nokiacrypt_decode_imei ); \
	REGISTER_SYMBOL ( nokiacrypt_decode_faid ); \
	REGISTER_SYMBOL ( nokiacrypt_encode_faid ); \


unsigned int nokiacrypt_init ( );
unsigned int nokiacrypt_decode_splock ( unsigned char *imei, unsigned char *msid );
unsigned int nokiacrypt_decode_msid ( unsigned char *msid );
unsigned int nokiacrypt_decode_imei ( unsigned char *imei, unsigned char *msid );
unsigned int nokiacrypt_decode_faid ( unsigned char *faid, unsigned char *msid );
unsigned int nokiacrypt_encode_faid ( unsigned char *faid, unsigned char *msid );
#endif

