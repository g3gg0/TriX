//---------------------------------------------------------------------------
#ifndef TRIXPLUG_MODE
#include <vcl.h>
#include <dir.h>
#endif
#include <time.h>
#include <stdio.h>
#pragma hdrstop

#include "UFS.hpp"

#define STATE_TX_LINE   0x40
#define STATE_RX_LINE   0x20

#define UFS_FAST

#ifndef UFS_FAST
#define STATE_TIMEOUT   20
#define SPEED_PREBOOT_READ      0xFF
#define SPEED_PREBOOT_WRITE     0x80
#define SPEED_BOOT_READ         0x80
#define SPEED_BOOT_WRITE        0x0A
#define SPEED_FLASH_READ        0x40
#define SPEED_FLASH_WRITE       0x08
#else
#define STATE_TIMEOUT   3
#define SPEED_PREBOOT_READ      0x50
#define SPEED_PREBOOT_WRITE     0x10
#define SPEED_BOOT_READ         0x50
#define SPEED_BOOT_WRITE        0x02
#define SPEED_FLASH_READ        0x18
#define SPEED_FLASH_WRITE       0x02
#endif

#ifdef TRIXPLUG_MODE
extern "C" 
{
	extern unsigned int success_response;

	void *TUFSx_new ()
	{
		return (void*) new TUFSx();
	}

	void TUFSx_delete ( void *handle )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return;

		delete dev;
	}

	int TUFSx_Open( void *handle, unsigned char *sn )
	{		
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->Open( sn );
	}

	int TUFSx_OpenFirst( void *handle )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->OpenFirst(  );
	}

	int TUFSx_Init( void *handle )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->Init(  );
	}

	void TUFSx_Close( void *handle )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return;

		dev->Close(  );
	}

    int TUFSx_SetMode( void *handle, int ufsmode )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->SetMode( ufsmode );
	}

	int TUFSx_BootLdr( void *handle, DWORD *asicID )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->BootLdr( asicID );
	}

	int TUFSx_Boot2ndLdr( void *handle, unsigned char *config, unsigned char *fia, unsigned int length )
	{
		TUFSx *dev = (TUFSx*)handle;
		TFIA *ldr = new TFIA( fia, length );
		int ret = 0;

		if ( !dev )
		{
			delete ldr;
			return 1;
		}

		ret = dev->Boot2ndLdr( ldr, config );

		delete ldr;
		return ret;
	}

	int TUFSx_FlashLdr( void *handle, unsigned char *fia, unsigned int length )
	{
		TUFSx *dev = (TUFSx*)handle;
		TFIA *ldr = new TFIA( fia, length );
		int ret = 0;

		if ( !dev )
		{
			delete ldr;
			return 1;
		}

		ret = dev->FlashLdr( ldr );

		delete ldr;
		return ret;
	}

	int TUFSx_CBusLdr( void *handle, unsigned char *fia, unsigned int length )
	{
		TUFSx *dev = (TUFSx*)handle;
		TFIA *ldr = new TFIA( fia, length );
		int ret = 0;

		if ( !dev )
		{
			delete ldr;
			return 1;
		}

		ret = dev->CBusLdr( ldr );

		delete ldr;
		return ret;
	}

	int TUFSx_GetASK( void *handle, unsigned char *outdata )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->GetASK( outdata );
	}

	int TUFSx_WriteFlashRPL( void *handle, unsigned char *indata )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->WriteFlashRPL( indata );
	}

	int TUFSx_WriteUEMRPL( void *handle, unsigned char *indata )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->WriteUEMRPL( indata );
	}

	int TUFSx_HWInit( void *handle )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->HWInit(  );
	}

	int TUFSx_FlashCmd( void *handle, unsigned char cmd, unsigned char *indata, unsigned short size )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->FlashCmd( cmd, indata, size );
	}

	char *TUFSx_GetError ( void *handle, int rc )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return NULL;

		return dev->GetError( rc );
	}

	int TUFSx_SendData( void *handle, unsigned char *data, unsigned short size )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		// compatibility to TriX_Prodigize
		if ( success_response )
		{
			// Wait for Flash loader to be ready
			if( !dev->WaitState( STATE_TX_LINE, true ))
				return UFSERR_NOACK1;
		}

		if ( dev->SendData( data, size ) )
		{
			// compatibility to TriX_Prodigize
			if ( success_response )
			{
				// Wait for command to be prepared
				if( !dev->WaitState( STATE_TX_LINE, false ))
					return UFSERR_NOACK2;

				// Flash loader waits for 1 byte to start the command
				  if( !dev->ReceiveData( 1 ))
					  return UFSERR_EXCHANGEFAILED;

				// Wait for command to be prepared
				if( !dev->WaitState( STATE_TX_LINE, true ))
					return UFSERR_NOACK3;
			}
			return 0;
		}
		return 1;
	}

	int TUFSx_ReceiveData( void *handle, unsigned char *data, unsigned short size )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		if ( dev->ReceiveData( size ) )
		{
			memcpy ( data, dev->inbuf, size );
			return 0;
		}
		return 1;
	}

	int TUFSx_Exchange ( void *handle, unsigned char *out_data, DWORD out_size, unsigned char *in_data, DWORD *in_size )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		return dev->Exchange( out_data, out_size, in_data, in_size );
	}

	int TUFSx_SetSpeeds ( void *handle, unsigned char *speeds )
	{
		TUFSx *dev = (TUFSx*)handle;
		if ( !dev )
			return 1;

		dev->UFSSpeed[ UFSSPEED_PREBOOT ].read = speeds[0];
		dev->UFSSpeed[ UFSSPEED_PREBOOT ].write = speeds[1];
		dev->UFSSpeed[ UFSSPEED_BOOT ].read = speeds[2];
		dev->UFSSpeed[ UFSSPEED_BOOT ].write = speeds[3];
		dev->UFSSpeed[ UFSSPEED_FLASH ].read = speeds[4];
		dev->UFSSpeed[ UFSSPEED_FLASH ].write = speeds[5];

		return 0;
	}
}
#endif

//---------------------------------------------------------------------------
TUFSx::TUFSx()
{
  handle = NULL;
  FlashID = NULL;

// Speed Init
  UFSSpeed[ UFSSPEED_PREBOOT ].read = SPEED_PREBOOT_READ;
  UFSSpeed[ UFSSPEED_PREBOOT ].write = SPEED_PREBOOT_WRITE;
  UFSSpeed[ UFSSPEED_BOOT ].read = SPEED_BOOT_READ;
  UFSSpeed[ UFSSPEED_BOOT ].write = SPEED_BOOT_WRITE;
  UFSSpeed[ UFSSPEED_FLASH ].read = SPEED_FLASH_READ;
  UFSSpeed[ UFSSPEED_FLASH ].write = SPEED_FLASH_WRITE;

  inbuf = (unsigned char *)malloc( 32768 );
}

TUFSx::~TUFSx()
{
  Close();

  free( inbuf );
}

void TUFSx::Close()
{
  if( handle != NULL )
    FT_Close( handle );
  handle = NULL;

  if( FlashID != NULL )
    free( FlashID );
  FlashID = NULL;

  FlashNum = 0;
}

//---------------------------------------------------------------------------
int TUFSx::Open( unsigned char *sn )
{
  if( handle != NULL )
    return UFSERR_OPEN;

  speed = UFSSPEED_PREBOOT;
  busstate = 0xFF;
  FlashID = NULL;
  FlashNum = 0;

  if( FT_OpenEx( sn, FT_OPEN_BY_SERIAL_NUMBER, &handle ) != FT_OK )
  {
    handle = NULL;
    return UFSERR_OPEN;
  }

  return UFSERR_OK;
}

int TUFSx::OpenFirst(  )
{
  FT_STATUS ftStatus;
  FT_DEVICE_LIST_INFO_NODE *ftDevList;
  DWORD numDevs, i;
  char text[512];
  char first_serial[64];

  strcpy ( first_serial, "" );

  ftStatus = FT_CreateDeviceInfoList( &numDevs );
  if( ftStatus == FT_OK )
  {
    if( numDevs != 0 )
    {
      ftDevList = (FT_DEVICE_LIST_INFO_NODE *)malloc( numDevs * sizeof( FT_DEVICE_LIST_INFO_NODE ));
      ftStatus = FT_GetDeviceInfoList( ftDevList, &numDevs );
      if( ftStatus == FT_OK )
      {
        for( i = 0; i < numDevs; i++ )
        {
          if( ftDevList[ i ].ID == UFS_ID )
          {
            sprintf( text, "%s , %s", ftDevList[ i ].SerialNumber, ftDevList[ i ].Description );
			if ( strlen ( first_serial ) == 0 )
				strcpy( first_serial, ftDevList[ i ].SerialNumber );
			printf ( text );
          }
        }
      }
      free( ftDevList );
    }
    else
      printf ( "No devices found!" );
  }
  else
    printf ( "Unable to get device list!" );

  return Open ( (unsigned char*)first_serial );
}

int TUFSx::Init()
{
  DWORD size;
  unsigned char auth1data[] = "\x4C\x1D\x1D\x1F\x02\x0D\x00";

  mode = UFSMODE_ATMEL;

  memset( BootVer, 0, sizeof( BootVer ));
  memset( FWVer, 0, sizeof( FWVer ));

  FT_ResetDevice( handle );
  FT_SetTimeouts( handle, 10000, 10000 );
//  FT_SetLatencyTimer( handle, 16 );
  FT_SetUSBParameters( handle, 32768, 32768 );
  FT_SetEventNotification( handle, FT_EVENT_RXCHAR, NULL );
  FT_SetDataCharacteristics( handle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE );
  FT_SetFlowControl( handle, FT_FLOW_NONE, 17, 19 );

  FT_SetBaudRate( handle, FT_BAUD_9600 );

  FT_ClrDtr( handle );
  FT_ClrRts( handle );
  FT_SetRts( handle );
  FT_ClrRts( handle );
  FT_SetRts( handle );
  FT_SetDtr( handle );

  FT_Purge( handle, FT_PURGE_TX | FT_PURGE_RX );

// Boot UFS
  size = 2;
  Exchange( (unsigned char*)"\x3F", 1, inbuf, &size );
  AtmelID = EndianWORD( *((unsigned short *)&inbuf[ 0 ]) );
  if( AtmelID != 0x62CF )
    return UFSERR_UFSBOOT;

// Get Serial Number
  size = 3;
  Exchange( (unsigned char*)"\x55\x1C", 2, inbuf, &size );
  SerNum = 65000L * inbuf[ 1 ];
  size = 3;
  Exchange( (unsigned char*)"\x54", 1, inbuf, &size );
  SerNum += EndianWORD( *((unsigned short *)&inbuf[ 0 ]) );

// Get Boot version
  size = 32;
  Exchange( (unsigned char*)"\x56", 1, inbuf, &size );
  memmove( BootVer, inbuf, size );

// Get FirmWare version
  size = 32;
  Exchange( (unsigned char*)"\x41", 1, inbuf, &size );
  memmove( FWVer, inbuf, size );

// Authorization
  auth1data[ 6 ] = CheckSum( &auth1data[ 1 ], 5, true );
  size = 1;
  Exchange( auth1data, 7, inbuf, &size );

  if( inbuf[ 0 ] != 0x3E )
    return UFSERR_UFSAUTH1;

  size = 3;
  Exchange( (unsigned char*)"\x31\x7F\x78", 3, inbuf, &size );
  if( inbuf[ 0 ] == 0x3F )
    return UFSERR_UFSAUTH2;

// UFS Initialization done, switch to UART mode

  size = 0;
  Exchange( (unsigned char*)"\x42\x10", 2, inbuf, &size );

  FT_SetBaudRate( handle, FT_BAUD_115200 );

  size = 1;
  Exchange( (unsigned char*)"\x58", 1, inbuf, &size );

  size = 1;
  Exchange( (unsigned char*)"\x32", 1, inbuf, &size );
  if( inbuf[ 0 ] == 0 )
    return UFSERR_UFSCABLE;

  FT_ClrDtr( handle );

  FT_SetLatencyTimer( handle, 5 );

  mode = UFSMODE_UART;

  return UFSERR_OK;
}

int TUFSx::SetMode( int ufsmode )
{
  DWORD size;

  FT_ResetDevice( handle );

  FT_Purge( handle, FT_PURGE_TX | FT_PURGE_RX );
  FT_SetDtr( handle );
  FT_Purge( handle, FT_PURGE_TX | FT_PURGE_RX );

//  FT_SetTimeouts( handle, 500, 8000 );
  FT_SetFlowControl( handle, FT_FLOW_NONE, 17, 19 );

  size = 1;
  Exchange( (unsigned char*)"\x78", 1, inbuf, &size );
//  FT_SetTimeouts( handle, 6000, 8000 );
  mode = UFSMODE_ATMEL;
  Sleep( 100 );

  switch( ufsmode )
  {
  case UFSMODE_FBUS:
    size = 0;
    Exchange( (unsigned char*)"\x42\x10", 2, inbuf, &size );

    FT_SetBaudRate( handle, FT_BAUD_115200 );

    size = 1;
    Exchange( (unsigned char*)"\x32", 1, inbuf, &size );
    size = 1;
    Exchange( (unsigned char*)"\x68\x01", 2, inbuf, &size );
    size = 1;
    Exchange( (unsigned char*)"\x67", 1, inbuf, &size );

//    FT_SetFlowControl( handle, FT_FLOW_NONE, 17, 19 );

    FT_Purge( handle, FT_PURGE_RX | FT_PURGE_TX );
    FT_SetDtr( handle );
    FT_Purge( handle, FT_PURGE_RX | FT_PURGE_TX );

    size = 1;
    Exchange( (unsigned char*)"\x68\x00", 2, inbuf, &size );
    size = 1;
    Exchange( (unsigned char*)"\x66", 0, inbuf, &size );

    FT_SetDtr( handle );
    FT_Purge( handle, FT_PURGE_RX | FT_PURGE_TX );

//    FT_SetFlowControl( handle, FT_FLOW_NONE, 17, 19 );
//    FT_SetDivisor( handle, 5242906L );
//    FT_SetDataCharacteristics( handle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE );

    FT_ResetDevice( handle );
    FT_Purge( handle, FT_PURGE_RX | FT_PURGE_TX );

    size = 1;
    Exchange( (unsigned char*)"\xA5", 1, inbuf, &size );
    if( inbuf[ 0 ] != 0xC3 )
      return UFSERR_NOACK,

    FT_ClrDtr( handle );
    FT_Purge( handle, FT_PURGE_RX | FT_PURGE_TX );

    Sleep( 1000 );
    mode = ufsmode;
    break;
  case UFSMODE_FLASH:
    FT_SetTimeouts( handle, 8000, 8000 );

    size = 0;
    Exchange( (unsigned char*)"\x42\x00", 2, inbuf, &size );

    FT_SetDivisor( handle, 0x0001 ); // 3MBits/s

    FT_SetFlowControl( handle, FT_FLOW_DTR_DSR, 17, 19 );
    FT_Purge( handle, FT_PURGE_RX | FT_PURGE_TX );

    size = 1;
    Exchange( (unsigned char*)"\x32", 1, inbuf, &size );

	size = 1;
    Exchange( (unsigned char*)"\x67", 1, inbuf, &size );

    Sleep( 100 );
    mode = ufsmode;
    break;
  }

  return UFSERR_OK;
}

int TUFSx::BootLdr( DWORD *asicID )
{
  DWORD size, i;
  /*
  typedef struct {
    unsigned char cmd;
    unsigned char speed;
    unsigned char receivesize;
    unsigned char sendbyte;
  }TBootInit;
  */
	unsigned char BootInit[4];

	speed = UFSSPEED_PREBOOT;

	SetMode( UFSMODE_FLASH );

	if( !WaitState( STATE_TX_LINE, true ))
		return UFSERR_NOACK;

	for ( i=0; i<10; i++ )
	{
		BootInit[0] = 0x62;
		BootInit[1] = UFSSpeed[ UFSSPEED_PREBOOT ].read;
		BootInit[2] = 2;
		BootInit[3] = 0x93;

		size = 4;
		Exchange( BootInit, 4, inbuf, &size );

		if( inbuf[ 0 ] == 0x92 )
		{
#ifdef TRIXPLUG_MODE
			memcpy ( asicID, inbuf, 4 );
#else
			*asicID = EndianWORD( *((unsigned short *)&inbuf[ 1 ]) );
#endif
			return UFSERR_OK;
		}
		Sleep ( 100 );
	}

#ifdef TRIXPLUG_MODE
	memcpy ( asicID, inbuf, 4 );
#else
	*asicID = EndianDWORD( *((unsigned long *)&inbuf[ 0 ]) );
#endif

	return UFSERR_NOACK;
}

bool TUFSx::WaitState( unsigned char mask, bool state )
{
  time_t start_time;

  start_time = time( NULL );
  do
  {
    if( !ReceiveData( 0 ))
      return false;
    if( !state )
    {
      if(( ~busstate ) & mask )
        return true;
    }
    else
    {
      if( busstate & mask )
        return true;
    }
    Sleep( 50 );
  }while(( time( NULL ) - start_time ) <= STATE_TIMEOUT );

  return false;
}

bool TUFSx::ReceiveData( unsigned short size )
{
	/*
  typedef struct {
    unsigned char cmd;
    unsigned char sendbyte;
    unsigned char speed;
    unsigned char msbsize; // ?
    unsigned short size;
  } TExCmd;
  */
  DWORD receivesize;
  unsigned char excmd[6];

  excmd[0] = 0x77;
  excmd[1] = ( busstate & STATE_RX_LINE )? 0xFF: 0x00;
  excmd[2] = UFSSpeed[ speed ].read;
  excmd[3] = 0x00;
  excmd[4] = (size >> 8) & 0xFF;
  excmd[5] = size & 0xFF;

  receivesize = size + 2;
  Exchange( excmd, 6, inbuf, &receivesize );

  if( (unsigned short)receivesize == ( size + 2 ))
  {
    if( inbuf[ receivesize - 1 ] == 0x72 )
    {
      busstate = inbuf[ receivesize - 2 ];
      return true;
    }
    else
      return false; // Bad acknowledge!
  }

  return false;
}

bool TUFSx::SendData( unsigned char *data, unsigned short size )
{
	/*
  typedef struct {
    unsigned char cmd;
    unsigned char speed;
    unsigned char msbsize; // ?
    unsigned short size;
  } TSendCmd;
  */
  DWORD receivesize, fullsize;
  unsigned char *sendcmd = NULL;

  fullsize = 5 + size;
  sendcmd = (unsigned char *)malloc( fullsize );
  if ( sendcmd == NULL )
    return false;

  sendcmd[0] = 0x74;
  sendcmd[1] = UFSSpeed[ speed ].write;
  sendcmd[2] = 0x00;
  sendcmd[3] = (size >> 8) & 0xFF;
  sendcmd[4] = size & 0xFF;

  memcpy( &sendcmd[5], data, size );

  receivesize = 2;
  Exchange( sendcmd, fullsize, inbuf, &receivesize );

  free( sendcmd );
  if( receivesize == 2 )
  {
    if( inbuf[ receivesize - 1 ] == 0x72 )
    {
      busstate = inbuf[ receivesize - 2 ];
      return true;
    }
    else
      return false; // Bad acknowledge!
  }

  return false;
}

int TUFSx::Boot2ndLdr( TFIA *ldr, unsigned char *config )
{
  unsigned char *data;
  unsigned char i, cs;
  int size;
  bool rc;

  if( ldr == NULL )
    return UFSERR_OPEN;

  size = ldr->dataBin.size + 5;
  data = (unsigned char *)malloc( size );
  if ( !data )
	  return UFSERR_EXCHANGEFAILED;

  data[ 0 ] = 0xFF;
  data[ 1 ] = 0xFF;
  data[ 2 ] = 0xFF;
  data[ 3 ] = (ldr->dataBin.size >> 8) & 0xFF;
  data[ 4 ] = ldr->dataBin.size & 0xFF;
  memcpy( &data[ 5 ], ldr->dataBin.ptr, ldr->dataBin.size );

  rc = SendData( data, size );
  free( data );

  if( !rc )
    return UFSERR_EXCHANGEFAILED;

  speed = UFSSPEED_BOOT;

  // Wait for 2nd boot loader response
  // 2nd boot loader waits for 3 bytes to continue.
  for( i = 0; i < 3; i++ )
  {
    if( !WaitState( STATE_TX_LINE, ( i & 1 ) ))
      return UFSERR_NOACK;
    if( !ReceiveData( 1 ))
      return UFSERR_EXCHANGEFAILED;
  }

  //Wait for Config to get ready
  if( !WaitState( STATE_TX_LINE, true ))
    return UFSERR_NOACK;

  // Getting first 3 bytes of Config
  if( !ReceiveData( 3 ))
    return UFSERR_EXCHANGEFAILED;

  if(( inbuf[ 0 ] == 0x00 ) || ( inbuf[ 0 ] == 0xFF ))
    return UFSERR_BADACK;

  FlashCfg = inbuf[ 1 ];
  FlashNum = inbuf[ 2 ];
  cs = ~CheckSum( &inbuf[ 1 ], 2, false );

  if ( config )
  	memcpy ( config, inbuf, 2 );

  // Get the rest
  size = ( 11 * FlashNum ) + sizeof( ExtBUSC ) + 1;
  if( !ReceiveData( size ))
    return UFSERR_EXCHANGEFAILED;

  if ( config )
  	memcpy ( &config[2], inbuf, size );
  
  cs = ~( cs + ~CheckSum( inbuf, size - 1, false ));
  if( cs != inbuf[ size - 1 ] )
    return UFSERR_BADCS;


  // FlashIDs
  if( FlashID != NULL )
    free( FlashID );
  FlashID = (TFlashID*)malloc( sizeof(TFlashID) * FlashNum );
  for( i = 0; i < FlashNum; i++ )
  {
	  FlashID[ i ].address = inbuf[11*i+0];
	  // endianess!!
	  FlashID[ i ].ID = (inbuf[11*i+1] << 24) | (inbuf[11*i+2] << 16) | (inbuf[11*i+3] << 8) | inbuf[11*i+4];
	  //FlashID[ i ].ID = (inbuf[11*i+4] << 24) | (inbuf[11*i+3] << 16) | (inbuf[11*i+2] << 8) | inbuf[11*i+1];
	  FlashID[ i ].data1[0] = inbuf[11*i+5];
	  FlashID[ i ].data1[1] = inbuf[11*i+6];
	  FlashID[ i ].rev = inbuf[11*i+7];
	  FlashID[ i ].data2[0] = inbuf[11*i+8];
	  FlashID[ i ].sector = (inbuf[11*i+9] << 8) | inbuf[11*i+10];
	  //FlashID[ i ].sector = (inbuf[11*i+10] << 8) | inbuf[11*i+9];
  }

  memmove( ExtBUSC, &inbuf[ ( sizeof( TFlashID ) * FlashNum ) ], sizeof( ExtBUSC ));

  return UFSERR_OK;
}

int TUFSx::FlashLdr( TFIA *ldr )
{
  unsigned char *data;
  int size;
  bool rc;

  if( ldr == NULL )
    return UFSERR_OPEN;

  size = ldr->dataBin.size + 2;
  data = (unsigned char *)malloc( size );

  if ( !data )
	  return NULL;

#ifdef OLD_LOADERS
  ldr->dataBin.ptr[ 0x20 ] = FlashCfg;
  ldr->dataBin.ptr[ 0x21 ] = FlashNum;
  for( i = 0; i < FlashNum; i++ )
  {
	  ldr->dataBin.ptr[ 0x22 + ( 8 * i ) + 0 ] = FlashID[i].address;
	  ldr->dataBin.ptr[ 0x22 + ( 8 * i ) + 1 ] = (FlashID[i].ID >> 24) & 0xFF;
	  ldr->dataBin.ptr[ 0x22 + ( 8 * i ) + 2 ] = (FlashID[i].ID >> 16) & 0xFF;
	  ldr->dataBin.ptr[ 0x22 + ( 8 * i ) + 3 ] = (FlashID[i].ID >> 8) & 0xFF;
	  ldr->dataBin.ptr[ 0x22 + ( 8 * i ) + 4 ] = FlashID[i].ID & 0xFF;
	  ldr->dataBin.ptr[ 0x22 + ( 8 * i ) + 5 ] = FlashID[i].data1[0];
	  ldr->dataBin.ptr[ 0x22 + ( 8 * i ) + 6 ] = FlashID[i].data1[1];;
	  ldr->dataBin.ptr[ 0x22 + ( 8 * i ) + 7 ] = FlashID[i].rev;
  }
#endif


  *((unsigned short *)(&data[ 0 ])) = EndianWORD( ldr->dataBin.size );
  memmove( &data[ 2 ], ldr->dataBin.ptr, ldr->dataBin.size );

  // Wait for 2nd boot loader to get ready
  if( !WaitState( STATE_TX_LINE, true ))
  {
    free( data );
    return UFSERR_NOACK;
  }

  rc = SendData( data, size );
  free( data );

  if( !rc )
    return UFSERR_EXCHANGEFAILED;

  speed = UFSSPEED_FLASH;

  // Wait for Flash loader response
  if( !WaitState( STATE_TX_LINE, false ))
    return UFSERR_NOACK;
  // Flash loader waits for 1 byte to start
  if( !ReceiveData( 1 ))
    return UFSERR_EXCHANGEFAILED;
  // Wait for Flash loader to get ready
  if( !WaitState( STATE_TX_LINE, true ))
    return UFSERR_NOACK;

  return UFSERR_OK;
}

int TUFSx::GetASK( unsigned char *outdata )
{
  unsigned char *data;
  int size, rval;

  size = 20;
  data = (unsigned char *)malloc( size );

  memmove( &data[ 0 ], IMEIESN, 8 );
  memmove( &data[ 8 ], "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 8 );
  *((unsigned long *)&data[ 16 ]) = EndianDWORD( time( NULL ));

  rval = FlashCmd( 0x1D, data, 20 );
  free( data );

  if( rval != UFSERR_OK )
    return rval;

  // Wait for command to be processed
  if( !WaitState( STATE_TX_LINE, true ))
    return UFSERR_NOACK;

  // Have to wait here some
  Sleep( 50 );

  // Get ASK Data
  if( !ReceiveData( 76 ))
    return UFSERR_EXCHANGEFAILED;

  // Check checksum of received data
  if( inbuf[ 75 ] != CheckSum( &inbuf[ 1 ], 74, false ))
    return UFSERR_BADCS;

  memmove( outdata, &inbuf[ 1 ], 74 );

  return UFSERR_OK;
}

int TUFSx::WriteUEMRPL( unsigned char *indata )
{
  int rval;

  // Write UEM RPL
  rval = FlashCmd( 0x1C, indata, 88 );
  if( rval != UFSERR_OK )
    return rval;

  // Wait for Flash loader to get ready
  if( !WaitState( STATE_TX_LINE, true ))
    return UFSERR_NOACK;

  return UFSERR_OK;
}

int TUFSx::WriteFlashRPL( unsigned char *indata )
{
  int rval;

  // Write Flash RPL
  rval = FlashCmd( 0x1E, indata, 40 );
  if( rval != UFSERR_OK )
    return rval;

  // Wait for Flash loader to get ready
  if( !WaitState( STATE_TX_LINE, true ))
    return UFSERR_NOACK;

  return UFSERR_OK;
}

int TUFSx::HWInit()
{
  int rval;

  // HW Init
  rval = FlashCmd( 0x2F );
  if( rval != UFSERR_OK )
    return rval;

  Sleep( 50 );

  if( !ReceiveData( 3 ))
    return UFSERR_EXCHANGEFAILED;
  if( inbuf[ 0 ] != 0x9E )
    return UFSERR_BADACK;
  if( inbuf[ 2 ] != CheckSum( &inbuf[ 1 ], 1, false ))
    return UFSERR_BADCS;

  return UFSERR_OK;
}

int TUFSx::CBusLdr( TFIA *ldr )
{
  unsigned char *data;
  int size, i, rval;
  bool rc;

  if( ldr == NULL )
    return UFSERR_OPEN;

  rval = HWInit();
  if( rval != UFSERR_OK )
    return rval;

  size = 8;
  data = (unsigned char *)malloc( size );

  ldr->GetSection( 0xD7, &data[ 0 ], &i ); // Loading address
  data[ 4 ] = CheckSum( ldr->dataBin.ptr, ldr->dataBin.size, true );
  data[ 5 ] = 0x00;
  *((unsigned short *)(&data[ 6 ])) = EndianWORD( ldr->dataBin.size ); // Size

  rval = FlashCmd( 0x13, data, size );
  free( data );

  if( rval != UFSERR_OK )
    return rval;

  // Wait for command to be processed
  if( !WaitState( STATE_TX_LINE, true ))
    return UFSERR_NOACK;

  // Send data
  rc = SendData( ldr->dataBin.ptr, ldr->dataBin.size );

  if( !rc )
    return UFSERR_EXCHANGEFAILED;

  if( !ReceiveData( 1 ))
    return UFSERR_EXCHANGEFAILED;
  if( !WaitState( STATE_TX_LINE, false ))
    return UFSERR_NOACK;
  if( !ReceiveData( 1 ))
    return UFSERR_EXCHANGEFAILED;
/*
  Sleep( 200 );

// Start CBUS loader
  if( !ReceiveData( 1 ))
    return UFSERR_EXCHANGEFAILED;

  if( !ReceiveData( 1 ))
    return UFSERR_EXCHANGEFAILED;
  if( inbuf[ 0 ] != 0x34 )
    return UFSERR_BADACK;

  Sleep( 100 );

// Receive data
  if( !ReceiveData( 9 ))
    return UFSERR_EXCHANGEFAILED;
  if( inbuf[ 8 ] != 0x42 )
    return UFSERR_BADACK;

  memmove( IMEIESN, inbuf, 8 );

  // Wait for flash loader to get ready
  if( !WaitState( STATE_TX_LINE, true ))
    return UFSERR_NOACK;
/*/
  if( !WaitState( STATE_TX_LINE, true ))
    return UFSERR_NOACK;
  if( !ReceiveData( 10 ))
    return UFSERR_EXCHANGEFAILED;
  if( inbuf[ 0 ] != 0x5A )
    return UFSERR_BADACK;
  if( inbuf[ 9 ] != CheckSum( &inbuf[ 1 ], 8, false ))
    return UFSERR_BADCS;

  memmove( IMEIESN, &inbuf[ 1 ], 8 );

  return UFSERR_OK;
}

int TUFSx::FlashCmd( unsigned char cmd, unsigned char *indata, unsigned short size )
{
  unsigned char *data;
  int fullsize;
  bool rc;

  fullsize = size + 2;
  data = (unsigned char *)malloc( fullsize );

  data[ 0 ] = cmd;
  if( indata != NULL )
    memmove( &data[ 1 ], indata, size );
  data[ size + 1 ] = CheckSum( &data [ 1 ], size, false );

  // Wait for Flash loader to be ready
  if( !WaitState( STATE_TX_LINE, true ))
    return UFSERR_NOACK;

  // Send command data
  rc = SendData( data, fullsize );

  free( data );

  if( !rc )
    return UFSERR_EXCHANGEFAILED;

  // Wait for command to be prepared
  if( !WaitState( STATE_TX_LINE, false ))
    return UFSERR_NOACK;

  // Flash loader waits for 1 byte to start the command
  if( !ReceiveData( 1 ))
    return UFSERR_EXCHANGEFAILED;


  return UFSERR_OK;
}

int TUFSx::Exchange( unsigned char *out_data, DWORD out_size, unsigned char *in_data, DWORD *in_size )
{
  FT_STATUS ftStatus;
  DWORD bytes;

  if ( out_data )
  {
	  ftStatus = FT_Write( handle, out_data, out_size, &bytes );

	  if( ftStatus != FT_OK )
		return ftStatus;
	  if( out_size != bytes )
	  {
		ftStatus = FT_OTHER_ERROR;
		*in_size = 0;
		return ftStatus;
	  }
  }

  if ( in_data && in_size )
  {
		if( *in_size == 0 )
		{
			Sleep( 50 );
			return ftStatus;
		}

		bytes = *in_size;
		ftStatus = FT_Read( handle, in_data, bytes, in_size );
		if( ftStatus != FT_OK )
			return ftStatus;
		if( bytes != *in_size )
		{
			ftStatus = FT_OTHER_ERROR;
			*in_size = 0;
			return ftStatus;
		}
  }  
  return ftStatus;
}

unsigned char TUFSx::CheckSum( unsigned char *data, DWORD size, bool add )
{
  DWORD i;
  unsigned char sum;

  sum = 0;
  for( i = 0; i < size; i++ )
    sum += data[ i ];

  sum = ~sum;
  if( add )
    sum++;

  return sum;
}

char *TUFSx::GetError( int rc )
{
  char *errtext;
  static char unktext[ 256 ];

  switch( rc )
  {
  case UFSERR_OK:
    errtext = "OK";
    break;
  case UFSERR_OPEN:
    errtext = "Open failed!";
    break;
  case UFSERR_UFSBOOT:
    errtext = "UFS booting failed!";
    break;
  case UFSERR_UFSAUTH1:
    errtext = "UFS authentication failed!";
    break;
  case UFSERR_UFSAUTH2:
    errtext = "UFS authentication step 2 failed!";
    break;
  case UFSERR_UFSCABLE:
    errtext = "cable too long!";
    break;
  case UFSERR_EXCHANGEFAILED:
    errtext = "send/receive failed!";
    break;
  case UFSERR_NOACK:
    sprintf( unktext, "no acknowledge! (%02X)", busstate );
    errtext = unktext;
    break;
  case UFSERR_NOACK1:
    sprintf( unktext, "no acknowledge 1 (TX HIGH)! (%02X)", busstate );
    errtext = unktext;
    break;
  case UFSERR_NOACK2:
    sprintf( unktext, "no acknowledge 2 (TX LOW)! (%02X)", busstate );
    errtext = unktext;
    break;
  case UFSERR_NOACK3:
    sprintf( unktext, "no acknowledge 3 (TX HIGH)! (%02X)", busstate );
    errtext = unktext;
    break;
  case UFSERR_BADACK:
    sprintf( unktext, "bad acknowledge! (%02X%02X)", inbuf[ 0 ], inbuf[ 1 ] );
    errtext = unktext;
    break;
  case UFSERR_BADCS:
    errtext = "bad checksum!";
    break;
  case UFSERR_NOFIA:
    errtext = "unable to find loader!";
    break;
  default:
    sprintf( unktext, "Unknown %lu (%02X%02X)", rc, inbuf[ 0 ], inbuf[ 1 ] );
    errtext = unktext;
    break;
  }

  return errtext;
}

unsigned short TUFSx::EndianWORD( unsigned short i )
{
  unsigned short j;

  j = (( i >> 8 ) & 0x00FF );
  j |= (( i << 8 ) & 0xFF00 );

  return j;
}

unsigned long TUFSx::EndianDWORD( unsigned long i )
{
  unsigned long j;

  j = (( i >> 24 ) & 0x000000FF );
  j |= (( i >> 8 ) & 0x0000FF00 );
  j |= (( i << 8 ) & 0x00FF0000 );
  j |= (( i << 24 ) & 0xFF000000 );

  return j;
}

TFIA *TUFSx::SearchFIA( char *mask, unsigned char type, unsigned short asicID, int flashid )
{
#ifndef TRIXPLUG_MODE
  struct ffblk fd;
  int done, rc = UFSERR_NOFIA;
  AnsiString filename, fullpath;
  TFIA *ldr;

  ldr = new TFIA();

  fullpath = ExtractFilePath( Application->ExeName );
  fullpath += "Flash\\";

  filename = fullpath + mask;
  done = findfirst( filename.c_str(), &fd, 0 );
  while( !done )
  {
    filename = fullpath + fd.ff_name;
    if( ldr->Open( filename.c_str() ) == FIAERR_OK )
    {
      if( ldr->hdr.type == type )
      {
        if( ldr->CheckASIC( asicID ) == FIAERR_OK )
        {
          if( flashid != -1 )
          {
            if( ldr->CheckFlashID( flashid ) == FIAERR_OK )
            {
              rc = UFSERR_OK;
              break;
            }
          }
          else
          {
            rc = UFSERR_OK;
            break;
          }
        }
      }
      ldr->Close();
    }
    done = findnext( &fd );
  }

  if( rc != UFSERR_OK )
  {
    delete ldr;
    ldr = NULL;
  }

  return ldr;
#else
	return NULL;
#endif
}
