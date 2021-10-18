#ifndef __TRIX_UFS_H__
#define __TRIX_UFS_H__

#include "trixplug.h"

#define STATE_TIMEOUT   15
#define SPEED_PREBOOT_READ      60
#define SPEED_PREBOOT_WRITE     35
#define SPEED_BOOT_READ         25
#define SPEED_BOOT_WRITE        2
#define SPEED_FLASH_READ        25
#define SPEED_FLASH_WRITE       2


void *TUFSx_new ();
void TUFSx_delete ( void *handle );

int TUFSx_Open( void *handle, unsigned char *sn );
int TUFSx_OpenFirst( void *handle );
int TUFSx_Init( void *handle );
void TUFSx_Close( void *handle );
int TUFSx_SetMode( void *handle, int ufsmode );
int TUFSx_BootLdr( void *handle, DWORD *asicID );
int TUFSx_Boot2ndLdr( void *handle, unsigned char *config, unsigned char *fia, unsigned int length );
int TUFSx_FlashLdr( void *handle, unsigned char *fia, unsigned int length );
int TUFSx_GetASK( void *handle, unsigned char *outdata );
int TUFSx_WriteFlashRPL( void *handle, unsigned char *indata );
int TUFSx_WriteUEMRPL( void *handle, unsigned char *indata );
int TUFSx_HWInit( void *handle );
int TUFSx_CBusLdr( void *handle, unsigned char *fia, unsigned int length );
int TUFSx_FlashCmd( void *handle, unsigned char cmd, unsigned char *indata, unsigned short size );
char *TUFSx_GetError ( void *handle, int rc );
int TUFSx_SendData( void *handle, unsigned char *data, unsigned short size );
int TUFSx_ReceiveData( void *handle, unsigned char *data, unsigned short size );
int TUFSx_Exchange ( void *handle, unsigned char *out_data, DWORD out_size, unsigned char *in_data, DWORD *in_size );
int TUFSx_SetSpeeds ( void *handle, unsigned char *speeds );

unsigned int ufs_init ( );
unsigned int ufs_open ( );
unsigned int ufs_close ( );
unsigned int ufs_check_bootrom ( unsigned char *buf );
unsigned int ufs_boot_2nd ( unsigned char *config, unsigned char *loader, unsigned int length );
unsigned int ufs_boot_3rd ( unsigned char *loader, unsigned int length );
unsigned int ufs_send_data (  unsigned char *buffer, unsigned int bytes );
unsigned int ufs_read_data (  unsigned char *buffer, unsigned int bytes );
unsigned int ufs_set_speeds ();
unsigned short ufs_pos_checksum ( unsigned char *buffer, unsigned int bytes );
unsigned short ufs_neg_checksum ( unsigned char *buffer, unsigned int bytes );


#define UFS_HEADERS "\
import unsigned int __ufs_open ( );\
import unsigned int __ufs_close ( );\
import unsigned int __ufs_check_bootrom ( unsigned char *buf );\
import unsigned int __ufs_boot_2nd ( unsigned char *config, unsigned char *loader, unsigned int length );\
import unsigned int __ufs_boot_3rd ( unsigned char *loader, unsigned int length );\
import unsigned int __ufs_send_data (  unsigned char *buffer, unsigned int bytes );\
import unsigned int __ufs_read_data (  unsigned char *buffer, unsigned int bytes );\
import unsigned int __ufs_set_speeds ();\
import unsigned short __ufs_pos_checksum ( unsigned char *buffer, unsigned int bytes );\
import unsigned short __ufs_neg_checksum ( unsigned char *buffer, unsigned int bytes );\
"

#define UFS_SYMBOLS \
	REGISTER_SYMBOL ( ufs_open ); \
	REGISTER_SYMBOL ( ufs_close ); \
	REGISTER_SYMBOL ( ufs_check_bootrom ); \
	REGISTER_SYMBOL ( ufs_boot_2nd ); \
	REGISTER_SYMBOL ( ufs_boot_3rd ); \
	REGISTER_SYMBOL ( ufs_send_data ); \
	REGISTER_SYMBOL ( ufs_read_data ); \
	REGISTER_SYMBOL ( ufs_set_speeds ); \
	REGISTER_SYMBOL ( ufs_pos_checksum ); \
	REGISTER_SYMBOL ( ufs_neg_checksum ); \


#endif

