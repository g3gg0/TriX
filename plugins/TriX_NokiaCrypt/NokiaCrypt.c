#ifndef __TRIX_NOKIACRYPT_C__
#define __TRIX_NOKIACRYPT_C__

#include "defines.h"
#include "options.h"

#include "trixplug.h"
#include "NokiaCrypt.h"

#include "3way_algo.h"

/*
 by Al:
--------
Only few (30bits, ~5%) bytes & bits from Claudia data is used, the others (482bits, ~95%) is just garbage.

Encryption mode: 0x20 (like RPL and Loaders);

Decrypted:
Used bytes & bits in format [<byte offset>][<bits used>]:

    * [0x04][7..0] - Flash Cipher key LSB;
    * [0x06][2..0] - SYSC algorithm ID;
    * [0x07][0] - unknown;
    * [0x0E][3..0] - unknown;
    * [0x1F][0] - unknown;
    * [0x23][3..0] - these bits set to SYSC and we have them as 4 lower bits on GetSysFlags Secure ROM function return, but from where is the higher bits?;
      if bit 0 or 1 is set here - Basic FTD will be enabled;
    * [0x2D][3..0] - unknown;
    * [0x35][0] - unknown;
    * [0x3B][7..0] - Flash Cipher key MSB;


Example (decoded Claudia at pdata array):

Code:
        data_dword = 0xA8000000 |

          (((DWORD)pdata[ 0x35 ] & 0x01 )) |
          (((DWORD)pdata[ 0x2D ] & 0x0F ) << 1 ) |
          (((DWORD)pdata[ 0x07 ] & 0x01 ) << 5 ) |
          (((DWORD)pdata[ 0x1F ] & 0x01 ) << 6 ) |

          (((DWORD)pdata[ 0x0E ] & 0x0F ) << 7 ) |
          (((DWORD)pdata[ 0x06 ] & 0x07 ) << 24 ); // FAID algo ID

        flashCipher =
          ((( pdata[ 0x3B ] ^ 0x17 ) << 24 ) |
          (( pdata[ 0x04 ] ^ 0xCA ) << 16 ) |
          ( 0xBA << 8 ) |
          ( 0xE9 ^ (((DWORD)pdata[ 0x23 ] & 0x0F ) << 1 ))); // last 2 bits at [0x23] will enable Basic FTD if set;
        text.sprintf( "Claudia MSID: %08X 00XXXXXX %08X", flashCipher, data_dword ); 
*/

// include all the TriXPluG function/init stuff
TRIXPLUG_STUBS
#include "trixplug_wrapper.h"


OPT_BEGIN
//	OPTION( OPT_INT, "Timing factor (100=normal, less=faster, more=slower)", "timings.timing_factor", &timing_factor )
OPT_FINISH

PLUGIN_INFO_BEGIN
	PLUGIN_NAME ( "NokiaCrypt" )
	PLUGIN_VERSION ( 0, 1 )
	PLUGIN_INIT ( nokiacrypt_init )
	PLUGIN_OPTIONS
PLUGIN_INFO_FINISH


unsigned int nokiacrypt_init ( )
{
	REGISTER_HEADER ( "trix_nokiacrypt", NOKIACRYPT_HEADERS );
    NOKIACRYPT_SYMBOLS

	printf ( "NokiaCrypt v0.1 Plugin Loaded" );
    return E_OK;
}


unsigned int nokiacrypt_decode_msid ( unsigned char *msid )
{
	DecodeMSID( msid );
	return E_OK;
}

unsigned int nokiacrypt_encode_faid ( unsigned char *faid, unsigned char *msid )
{
	EncodeFAID ( faid, msid );
	return E_OK;
}

unsigned int nokiacrypt_decode_imei ( unsigned char *imei, unsigned char *msid )
{
	DecodeIMEI ( imei, msid );
	return E_OK;
}

unsigned int nokiacrypt_decode_faid ( unsigned char *faid, unsigned char *msid )
{
	DecodeFAID ( faid, msid );
	return E_OK;
}

unsigned int nokiacrypt_decode_splock ( unsigned char *imei, unsigned char *msid )
{
	DecodeSPLock ( imei, msid );
	return E_OK;
}


#endif

