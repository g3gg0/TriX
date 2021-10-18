
#include "Common.h"

#include <string.h>
#include <stdlib.h>


WORD ReverseWORD( WORD data )
{
	return (( data >> 8 ) | ( data << 8 ));
}

void
Str2Hex( BYTE *dst, BYTE *src )
{
	int i, size = 8, length;
	char data;

        length = strlen( (const char *)src );
	if( length > 16 )
                length = 16;
	memset( dst, 0, size );
	for( i = 0; i < length; i++ )
	{
		if(( i >> 1 ) > size )
			break;
		data = src[ i ];
		if(!(( data & 0xD0 ) ^ 0x40 ))
			data -= 7;
		dst[ i >> 1 ] |= ( data & 0x0F ) << ( 4 * (( i & 1 ) ^ 1 ));
	}
}

void
Hex2Str( BYTE *dst, BYTE *src )
{
	int i, length = 16;

	for( i = 1; i < length; i++ )
		dst[ i - 1 ] = (( src[ i >> 1 ] >> ( 4 * (( i & 1 ) ^ 1 ))) & 0x0F ) + 0x30;
	dst[ length - 1 ] = 0;
}

