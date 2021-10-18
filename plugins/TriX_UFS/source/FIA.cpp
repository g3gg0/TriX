//---------------------------------------------------------------------------

#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "FIA.h"

typedef struct {
  unsigned char num;
  unsigned char size;
  unsigned char ptr[ 1 ];
} TFIASec;

//---------------------------------------------------------------------------
TFIA::TFIA()
{
  handle = -1;
  dataBin.ptr = NULL;
  dataSec.ptr = NULL;
}

TFIA::~TFIA()
{
  Close();
}

void TFIA::Close()
{
  if( handle != -1 )
    close( handle );
  handle = -1;
  if( dataBin.ptr != NULL )
    free( dataBin.ptr );
  dataBin.ptr = NULL;
  if( dataSec.ptr != NULL )
    free( dataSec.ptr );
  dataSec.ptr = NULL;
}

int TFIA::Open( unsigned char *filename )
{
  if( handle != -1 )
    return FIAERR_OPEN;

  handle = open( filename, O_RDONLY | O_BINARY );

  if( handle == -1 )
    return FIAERR_OPEN;

  strcpy( filepath, filename );
  
  read( handle, &hdr, sizeof( TFIAHdr ));
  hdr.length = EndianDWORD( hdr.length );
  hdr.sections = EndianDWORD( hdr.sections );
  switch( hdr.type )
  {
  case FIA_2NDBOOT:
  case FIA_FLASH:
    dataSec.size = hdr.length - 4;
    if( dataSec.size > 0 )
    {
      dataSec.ptr = (unsigned char *)malloc( dataSec.size );
      read( handle, dataSec.ptr, dataSec.size );
    }
    dataBin.size = filelength( handle ) - hdr.length - 5;
    if( dataBin.size > 0 )
    {
      dataBin.ptr = (unsigned char *)malloc( dataBin.size );
      read( handle, dataBin.ptr, dataBin.size );
    }
  }
  handle = handle;
  return FIAERR_OK;
}

int TFIA::CheckASIC( unsigned short asicid )
{
  unsigned char data[ 256 ];
  int i, size;

  if( GetSection( 0xD4, data, &size ) == FIAERR_OK )
  {
    for( i = 0; i < size; i += 2 )
    {
      if( EndianWORD( *((unsigned short *)&data[ i ])) == asicid )
        break;
    }

    if( i < size )
      return FIAERR_OK;
  }
  else
    return FIAERR_SECTION;

  return FIAERR_NOASIC;
}

int TFIA::CheckFlashID( int flashid )
{
  int i, shift, rc = FIAERR_NOFLASHID;
  int curfid, size;
  char data[ 256 ];

  if( GetSection( 0xC3, data, &size ) == FIAERR_OK )
  {
    if( strstr( data, " BT " ) != NULL )
      return FIAERR_NOFLASHID;
  }

  i = 0;
  do
  {
    shift = 0x44 + ( i * 0x10 );
    if(( shift + 4 ) >= dataBin.size )
      break;
    if(( shift + 4 ) >= 0x400 )
      break;
    curfid = *((int *)&dataBin.ptr[ shift ]);
    if( curfid == flashid )
    {
      rc = FIAERR_OK;
      break;
    }
    i++;
  }while( curfid != 0x11111111 );

  return rc;
}

int TFIA::GetSection( unsigned char num, unsigned char *data, int *size )
{
  int i;
  TFIASec *sec;
  unsigned char *ptr;

  *size = 0;

  if( dataSec.ptr == NULL )
    return FIAERR_SECTION;

  ptr = dataSec.ptr;
  for( i = 0; i < (int)hdr.sections; i++ )
  {
    sec = (TFIASec *)ptr;
    if( sec->num == num )
    {
      *size = sec->size;
      memmove( data, sec->ptr, sec->size );
      break;
    }
    ptr += ( sec->size + sizeof( TFIASec ) - sizeof( sec->ptr ));
  }

  return FIAERR_OK;
}

unsigned short TFIA::EndianWORD( unsigned short i )
{
  unsigned short j;

  j = (( i >> 8 ) & 0x00FF );
  j |= (( i << 8 ) & 0xFF00 );

  return j;
}

unsigned long TFIA::EndianDWORD( unsigned long i )
{
  unsigned long j;

  j = (( i >> 24 ) & 0x000000FF );
  j |= (( i >> 8 ) & 0x0000FF00 );
  j |= (( i << 8 ) & 0x00FF0000 );
  j |= (( i << 24 ) & 0xFF000000 );

  return j;
}
