#ifndef __TRIX_E32IMAGE_DEFLATE_C__
#define __TRIX_E32IMAGE_DEFLATE_C__

#include <stdlib.h>
#include <string.h>

#include "defines.h"

#include "e32image_deflate.h"

#include "mem.h"

const int fixedDecompressTree[] =
{
	  1,   2,
	  0,  -2,
	  1,   2,
	 -1,  -3,
	  1,   2,
	 -4,  -5,
	  1,   3,
	 -6,   1,
     -7,  -8,
	  1,   3,
	 -9,   1,
	-10, -11,
	  1,   2,
	-12, -13,
	  1,   2,
	-14, -15,
    -16,   1,
	-17,   1,
	-18,   1,
	-19,   1,
	-20,   1,
	  1,   3,
	-21,   1,
	-22, -23,
      1,   2,
	-24, -25,
	-26,   1,
	-27, -28
};

unsigned char xlat[MAX_FIXED_LENGTH];

unsigned char *bitstream_data = NULL;
unsigned int bitstream_length = 0;
unsigned int bitstream_current_bit = 0;

int bitstream_init ( const unsigned char *data, unsigned int length )
{
	if ( !data && !length )
		return E_FAIL;

	if ( bitstream_data )
		free ( bitstream_data );

	bitstream_data = (unsigned char*)malloc ( length );
	if ( !bitstream_data )
		return E_FAIL;

	memcpy ( bitstream_data, data, length );
	bitstream_length = length;
	bitstream_current_bit = 0;

	return E_OK;
}

void bitstream_free ()
{
	free ( bitstream_data );
	bitstream_data = NULL;
	bitstream_current_bit = 0;
	bitstream_length = 0;
}

void bitstream_rewind ()
{
	bitstream_current_bit = 0;
}

unsigned int bitstream_available_bits ( )
{
	return bitstream_length * 8 - bitstream_current_bit;
}

unsigned char bitstream_get_bit ( unsigned char *err )
{
	*err = 0;
	if ( !bitstream_data || bitstream_current_bit / 8 >= bitstream_length )
	{
		*err = 1;
		return 0;
	}

	return bitstream_data[bitstream_current_bit/8] >> 7 - (bitstream_current_bit++)%8 & 1;
}

/**
 returns bits from the bitstream. at the end it returns last bits and sets err
 @param length how many bits to read
 @param err set when an error occurs
 @returns read bits
 */
unsigned int bitstream_get_bits ( unsigned char length, unsigned char *err )
{
	unsigned char pos = 0;
	unsigned int value = 0;

	*err = 0;
	if ( !bitstream_data || length < 1 || length > 32 )
	{
		*err = 1;
		return 0;
	}

	while ( pos++ < length )
	{
		value <<= 1;
		value |= bitstream_get_bit ( err );
		if ( *err )
			return value >> 1;
	}

	return value;
}


int e32image_deflate_readcode ( const int *tree, unsigned char *err )
{
	int pos = 0;
	int code = 0;
	//unsigned char err = 0;

	while ( (code = tree[2*pos + bitstream_get_bit ( err )]) > 0 )
	{
		if ( *err )
		{
			printf ( "e32image_deflate_readcode: error reading bit\n" );
			return 0;
		}
		pos += code;
	}

	return -code;
}

unsigned char *e32image_deflate_readtree ( unsigned char *xlat, int length )
{
	int count = 0;
	unsigned char code = 0;
	unsigned char last = 0;
	unsigned char tmp;
	unsigned char *buf = NULL;

	unsigned char err = 0;

	int pos;

	if ( !xlat || !length )
		return NULL;

	buf = (unsigned char*) malloc ( length );
	
	if ( !buf )
		return NULL;

	for ( pos = 0; pos < length; )
	{
		code = e32image_deflate_readcode ( fixedDecompressTree, &err );
		if ( err )
		{
			printf ( "e32image_deflate_readtree: read error\n" );
			free ( buf );
			return NULL;
		}

		if ( code > 1 )
		{
			for ( ; count > 0; count-- )
				buf[pos++] = last;
			code -= 2;
			tmp = xlat[code];
			if ( code > 0 )
			{
				memmove ( xlat+1, xlat, code );
				//printf ( "e32image_deflate_readtree: Array.Copy\n" );
			}
			xlat[0] = last;
			last = tmp;
			buf[pos++] = last;
		}
		else
			count = count * 2 + code + 1;
	}

	return buf;
}



int e32image_deflate_build_decompress_tree ( unsigned char *lengths, unsigned int lenghts_length, int **tree )
{
	int minlength = MAX_FIXED_LENGTH;
	int maxlength = 0;
	int valcount = 0;
	int valsum = 0;
	int testval = 1 << MAX_FIXED_LENGTH;
	int i;

	int value = 0;
	int bit;
	int current = 0;
	int next = 1;

	int bits;
	int pos;
	int len;
	int tmp;

	if ( !lengths )
		return E_FAIL;

	// get max and min lengths
	for ( i = 0; i < lenghts_length; i++ )
	{
		if ( lengths[i] )
		{
			valcount++;
			valsum += lengths[i];
			testval -= 1 << MAX_FIXED_LENGTH - lengths[i];
			if ( lengths[i] > maxlength )
				maxlength = lengths[i];
			if ( lengths[i] < minlength )
				minlength = lengths[i];
		}
	}
	

	if ( testval || maxlength > MAX_FIXED_LENGTH )
		return E_FAIL;

	*tree = (int*)calloc ( (valcount - 1) * 2, sizeof ( int ) );
	
	//go through all lengths, beginning at min length
	for ( bits = minlength; bits <= maxlength; bits++ )
	{
		value <<= 1;
		for ( pos = 0; pos < lenghts_length; pos++ )
		{
			//find from table all lengths with current length
			if ( lengths[pos] == bits )
			{
				current = 0;
				for ( len = lengths[pos] - 1; len >= 0; len-- )
				{
					bit = value >> len & 1;
					if ( !len )
						(*tree)[current*2+bit] = -pos;
					else
					{
						if ( (*tree)[current*2+bit] == 0 )
							(*tree)[current*2+bit] = next++ - current;
						current += (*tree)[current*2+bit];
					}
				}
				value++;
			}
		}
	}

	tmp = valcount;
	for ( i = 0; i < (tmp-1)*2; i++ )
	{
		if ((*tree)[i] >= 0)
			valcount--;
	}

	if ( valcount != (lengths[0] ? 1 : 0 ) )
	{
		free ( *tree );
		return E_FAIL;
	}

	return E_OK;

}

int *literaltree;
int *distancetree;

int e32image_deflate_init ( unsigned char *data )
{
	int i;
	unsigned char *tmp;
	
	xlat[0] = 0;

	for ( i = 0; i < MAX_FIXED_LENGTH; i++ )
		xlat[i] = i + 1;

	

	// read lengths table/tree
	if ( !(tmp = e32image_deflate_readtree ( xlat, LITERALS_COUNT )) )
		return E_FAIL;
	if ( e32image_deflate_build_decompress_tree ( tmp, LITERALS_COUNT, &literaltree ) != E_OK )
		return E_FAIL;
	free ( tmp );

	if ( !(tmp = e32image_deflate_readtree ( xlat, DISTANCES_COUNT ) ) )
		return E_FAIL;
	if ( e32image_deflate_build_decompress_tree ( tmp, DISTANCES_COUNT, &distancetree ) != E_OK )
		return E_FAIL;
	free ( tmp );

	return E_OK;
}

/* free both trees */
void e32image_deflate_free ( )
{
	free ( literaltree );
	free ( distancetree );
}


int e32image_deflate_extendedcode ( unsigned int code, unsigned char *err )
{
	unsigned char bits;
	int ret;

	if ( code > 7 )
	{
		bits = (unsigned char)((code >> 2) - 1);
		code -= bits << 2;
		ret = code << bits | bitstream_get_bits ( bits, err );
		if ( *err )
		{
			printf ( "e32image_deflate_extendedcode: error getting bits\n" );
		}
		return ret;
	}

	return code;
}

unsigned int e32image_deflate_decode (unsigned char **buffer, unsigned int total_size, unsigned char *data )
{
	unsigned int code;
	int pos = 0;
	int distance;
	int length;

	unsigned char err = 0;

	unsigned char *buf = (unsigned char*)malloc ( bitstream_available_bits() );
	*buffer = buf;

	//total_size = bitstream_available_bits() / 8;

	while ( pos < total_size )
	{
		code = e32image_deflate_readcode ( literaltree, &err );
		if ( err )
		{
			free ( buf );
			*buffer = NULL;
			return E_FAIL;
		}
		if ( code == END_OF_STREAM )
			break;
		if ( code < 0x100 )
			buf[pos++] = (unsigned char)(code & 0xFF);
		else
		{
			length = e32image_deflate_extendedcode ( code - 0x100, &err ) + 3;
			if ( err )
			{
				free ( buf );
				*buffer = NULL;
				return E_FAIL;
			}
			
			code = e32image_deflate_readcode ( distancetree, &err );
			if ( err )
			{
				free ( buf );
				*buffer = NULL;
				return E_FAIL;
			}

			distance = e32image_deflate_extendedcode ( code, &err ) + 1;
			if ( err )
			{
				free ( buf );
				*buffer = NULL;
				return E_FAIL;
			}

			while ( length-- > 0 )
				buf[pos] = buf[pos++ - distance];
		}
	}

	buf = (unsigned char*)realloc ( buf, pos );

	return E_OK;
}

void bitstream_test_print ( )
{
	int i = 0;
	unsigned char err = 0;
	int value = 0;

	printf ( "-- bitstream test --\n\n" );

	while ( i++ < 8*20 && !err )
	{
		value = value << 1 | bitstream_get_bit ( &err );
		if ( i % 8 == 0 && i )
		{
			printf ( "%02X ", value );
			value = 0;
		}
	}

	printf ( "\n" );

	if ( err )
		printf ( "error getting bit\n" );

	printf ( "\n---\n\n" );

	i = 0;
	err = 0;
	
	while ( i++ < 20 && !err )
		printf ( "%02X ", bitstream_get_bits ( 8, &err ) );

	printf ( "\n" );
	if ( err )
		printf ( "error getting bits\n" );

	printf ( "--------------------\n\n" );
}

unsigned char *e32image_deflate_decompress (unsigned char *data, unsigned int data_length, unsigned int unpacked_length )
{
	unsigned char *buf = NULL;

	if ( bitstream_init ( data, data_length ) == E_FAIL )
		return NULL;

	//bitstream_test_print ();
	//return NULL;

	if ( e32image_deflate_init ( data ) == E_FAIL )
	{
		e32image_deflate_free();
		bitstream_free();
		return NULL;
	}
	if ( e32image_deflate_decode ( &buf, unpacked_length, data ) == E_FAIL )
	{
		e32image_deflate_free();
		bitstream_free();
		return NULL;
	}

	e32image_deflate_free();
	bitstream_free();

	return buf;
}




#endif
