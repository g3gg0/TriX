#include "3way_algo.h"
#include "Common.h"
#include "key_table.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 12

static const UINT8 encodeseeds[] =
	{ 0xB1, 0x73, 0xE6, 0x5A, 0xAB, 0x47, 0x8E, 0x0D, 0x1A, 0x34, 0x68, 0x0B };


static void
CreateDecodeTable( UINT8 *ptr )
{
  UINT8 U8code = 0;
  UINT8 i, j;
  UINT8 tmphash[ TABLE_SIZE ];
  UINT8 hash[ 12 ][ 5 ] = {
    { 3, 4, 5, 7, 8 },
	{ 0, 2, 4, 5, 9 },
	{ 5, 6, 7, 9, 10 },
	{ 2, 4, 6, 7, 11 },
	{ 0, 7, 8, 9, 11 },
	{ 1, 4, 6, 8, 9 },
	{ 1, 2, 9, 10, 11 },
	{ 3, 6, 8, 10, 11 },
	{ 0, 1, 3, 4, 11 },
	{ 0, 1, 5, 8, 10 },
	{ 1, 2, 3, 5, 6 },
	{ 0, 2, 3, 7, 10 }
  };

  U8code = 0;
  for( i = 0; i < TABLE_SIZE; i++ )
  {
    tmphash[ i ] = 0;
	for( j = 0; j < 5; j++ )
	  tmphash[ i ] ^= ptr[ hash[ i ][ j ] ];
    U8code ^= ptr[ i ];
  }

  for( i = 0; i < TABLE_SIZE; i++ )
    tmphash[ i ] ^= U8code;

  memcpy( ptr, tmphash, TABLE_SIZE );
}

static UINT8 ReverseByte( UINT8 pU8byte )
{
  UINT8 U8new = 0;
  UINT8 i;

  for( i = 0; i < 8; i++ )
    U8new |= (( pU8byte >> ( 7 - i )) & 1 ) << i;

  return U8new;
}

static void
ReverseBits( UINT8 *ptr )
{
  UINT8 i;
  UINT8 tmphash[ TABLE_SIZE ];

  memcpy( tmphash, ptr, TABLE_SIZE );

  for ( i = 0; i < TABLE_SIZE; i++ )
    ptr[ TABLE_SIZE - i - 1 ] = ReverseByte( tmphash[ i ] );
}

static void
PrepareDecode( UINT8 seedvalue, UINT8 *streamptr, UINT8 *tableptr )
{
  UINT8 i;

  for( i = 0; i < TABLE_SIZE; i++ )
    streamptr[ i ] ^= tableptr[ i ] ^ ( seedvalue * ((( i >> 1 ) % 3 ) & 1 ));

  CreateDecodeTable( streamptr );
}

static void
RotateNTimes( UINT8 *ptr, UINT8 ntimes, UINT8 start )
{
  UINT8 i, j, U8prevbit = 0, U8bit = 0;

  for( i = 0; i < ntimes; i++ )
  {
    U8bit = ( ptr[ 3 + start ] & 1 );
    U8prevbit = U8bit;
    for( j = 0; j < 4; j ++ )
    {
      U8bit = ( ptr[ j + start ] & 1 );
      ptr[ j + start ] >>= 1;
      ptr[ j + start ] |= ( U8prevbit << 7 );
      U8prevbit = U8bit;
    }
  }
}

static void
Rotate( UINT8 *ptr, UINT8 start1, UINT8 start2 )
{
  RotateNTimes( ptr, 0x0A, start1 );
  RotateNTimes( ptr, 0x1F, start2 );
}

static void
Alter( UINT8 *streamptr )
{
  UINT8 l, i, m = TABLE_SIZE, k = 7, j = 3;
  UINT8 tmphash[ TABLE_SIZE ];

  memcpy( tmphash, streamptr, TABLE_SIZE );

  for( i = 0; i < TABLE_SIZE; i++ )
  {
    l = ~( tmphash[ k ] ) | ( tmphash[ j ] );
    m--;
    if( !k )
      k = TABLE_SIZE;
    k--;
    if( !j )
      j = TABLE_SIZE;
    j--;
    streamptr[ m ] ^= l;
  }
}

static void
Crypt_3way( UINT8 *data, const UINT8 *seed, UINT8 *hash )
{
  UINT8 i;

  for ( i = 0; i < TABLE_SIZE; i++ )
  {
    PrepareDecode( seed[ i ], data, hash );
	if( i == TABLE_SIZE - 1 )
	{
	  ReverseBits( data );
	  break;
	}
    Rotate( data, 0, 8 );
    Alter( data );
    Rotate( data, 8, 0 );
  }
}

static void
Decrypt_3way( UINT8 *data, const UINT8 *eseed, UINT8 *ehash )
{
  UINT8 seed[ TABLE_SIZE ], hash[ TABLE_SIZE ];

  memmove( seed, eseed, TABLE_SIZE );
  memmove( hash, ehash, TABLE_SIZE );

  ReverseBits( seed );

  CreateDecodeTable( hash );
  ReverseBits( hash );

  Crypt_3way( data, seed, hash );
}

void
DecodeMSID( UINT8 *pmsid )
{
  UINT8 seeds[ TABLE_SIZE ];

  memmove( seeds, encodeseeds, TABLE_SIZE );
  if( pmsid[ 0 ] < 0x80 )
    memset( seeds, 0, TABLE_SIZE );

  Decrypt_3way( &pmsid[ 1 ], seeds, key_table[ pmsid[ 0 ] ] );
}

void
EncodeFAID( UINT8 *pfaid, UINT8 *pmsid )
{
  UINT8 seeds[ TABLE_SIZE ], hash[ TABLE_SIZE ];
  UINT8 faidID;

  faidID = pmsid[ 9 ];
//  memset( pfaid, 0, TABLE_SIZE );
  memmove( hash, key_table[ faidID ], TABLE_SIZE );
//  *((UINT32*)pfaid) = *((UINT32*)&pmsid[ 1 ]) ^ 0x8960CA17;
  *((UINT32*)hash) ^= *((UINT32*)&pmsid[ 5 ]);
//  if(( pmsid[ 10 ] != 0 ) && ( pmsid[ 11 ] != 0 ))
//  *((UINT32*)&pfaid[ 4 ]) = ~*((UINT32*)pfaid);

  memmove( seeds, encodeseeds, TABLE_SIZE );
  if( faidID < 0x80 )
    memset( seeds, 0, TABLE_SIZE );

  Crypt_3way( pfaid, seeds, hash );
}

void
DecodeFAID( UINT8 *pfaid, UINT8 *pmsid )
{
  UINT8 seeds[ TABLE_SIZE ], hash[ TABLE_SIZE ];
  UINT8 faidID;

  faidID = pmsid[ 9 ];
  memmove( hash, key_table[ faidID ], TABLE_SIZE );
  *((UINT32*)hash) ^= *((UINT32*)&pmsid[ 5 ]);

  memmove( seeds, encodeseeds, TABLE_SIZE );
  if( pmsid[ 0 ] < 0x80 )
    memset( seeds, 0, TABLE_SIZE );

  Decrypt_3way( pfaid, seeds, hash );
}

void
DecodeIMEI( UINT8 *pimei, UINT8 *pmsid )
{
  UINT8 seeds[ TABLE_SIZE ], hash[ TABLE_SIZE ];
  UINT8 imeiID;

  imeiID = pmsid[ 10 ];
  memmove( hash, key_table[ imeiID ], TABLE_SIZE );
  *((UINT32*)hash) ^= *((UINT32*)&pmsid[ 5 ]);

  memmove( seeds, encodeseeds, TABLE_SIZE );
  if( pmsid[ 0 ] < 0x80 )
    memset( seeds, 0, TABLE_SIZE );

  Decrypt_3way( pimei, seeds, hash );
}

void
DecodeSPLock( UINT8 *pimei, UINT8 *pmsid )
{
  UINT8 seeds[ TABLE_SIZE ], hash[ TABLE_SIZE ];
  UINT8 imeiID;

  imeiID = pmsid[ 11 ];
  memmove( hash, key_table[ imeiID ], TABLE_SIZE );
  *((UINT32*)hash) ^= *((UINT32*)&pmsid[ 5 ]);

  memmove( seeds, encodeseeds, TABLE_SIZE );
  if( pmsid[ 0 ] < 0x80 )
    memset( seeds, 0, TABLE_SIZE );

  Decrypt_3way( pimei, seeds, hash );
}
