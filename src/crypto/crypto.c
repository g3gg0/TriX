#ifndef __TRIX_CRYPTO_C__
#define __TRIX_CRYPTO_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "seer.h"
#include "workspace.h"
#include "util.h"
#include "md5.h"
#include "crypto.h"

#include "trixplug_funcs.h"
#include "mem.h"

CRYPTO_PLUG_INIT;

char *
crypto_get_signkey ( char *signature )
{
	static char *buffer = NULL;

	if ( buffer )
		free ( buffer );
	buffer = util_split ( signature, '|', 0 );

	if ( buffer )
		buffer = strdup ( buffer );

	return buffer;
}

char *
crypto_get_signer ( char *signature )
{
	static char *buffer = NULL;

	if ( buffer )
		free ( buffer );
	buffer = util_split ( signature, '|', 1 );

	if ( buffer )
		buffer = strdup ( buffer );

	return buffer;
}


char *
crypto_get_signee ( char *signature )
{
	static char *buffer = NULL;

	if ( buffer )
		free ( buffer );
	buffer = util_split ( signature, '|', 2 );

	if ( buffer )
		buffer = strdup ( buffer );

	return buffer;
}

char *
crypto_get_signopt ( char *signature )
{
	static char *buffer = NULL;

	if ( buffer )
		free ( buffer );
	buffer = util_split ( signature, '|', 3 );

	if ( buffer )
		buffer = strdup ( buffer );

	return buffer;
}


t_crypt_key *
crypto_get_key ( unsigned char key_id, t_crypt_key *keys )
{
	int pos = 0;

	if ( !keys )
		return NULL;

	while ( keys[pos].id != 0xFF )
	{
		if ( keys[pos].id == key_id )
			return &keys[pos];
		pos++;
	}
	return NULL;
}

char *
crypto_get_keyname ( unsigned char key_id, t_crypt_key *keys )
{
	int pos = 0;

	if ( !keys )
		return NULL;

	while ( keys[pos].id != 0xFF )
	{
		if ( keys[pos].id == key_id )
			return keys[pos].name;
		pos++;
	}
	return NULL;
}

// remove signature header, maybe also unpack etc?
int
crypto_unfold ( char **in_data, int length, t_crypt_key *keys )
{
	int pos = 0;
	unsigned char *newbuf = NULL;

	if ( !in_data || !keys || length < 1 )
		return NULL;

	if ( !strncmp ( (*in_data), "/* AUTH: ", 9 ) )
	{
		while ( (*in_data)[pos] != '\n' && (*in_data)[pos] )
			pos++;
		if ( !(*in_data)[pos] )
			return E_OK;
		pos++;

		newbuf = malloc ( length - pos );
		if ( !newbuf )
			return E_FAIL;

		memcpy ( newbuf, &(*in_data)[pos], length - pos );
		free ( *in_data );
		*in_data = newbuf;
	}

	return E_OK;
}

//
// for now just check integrity 
//
char *
crypto_check_integrity ( char **in_data, int length, t_crypt_key *keys )
{
	MD5_DIGEST digest;
	unsigned char *buffer = NULL;
	unsigned char *data = *in_data;
	static unsigned char *tempbuf = NULL;
	int pos = 0;
	int bytes = 0;
	int auth_valid = 0;
	int checksum_valid = 0;
	unsigned char key_id = 0;
	t_crypt_key *key = NULL;


	if ( !data || !keys || length < 1 )
		return NULL;

	if ( !tempbuf )
		tempbuf = malloc ( 1 );

	/* use default key */
	key = &keys[0];


	if ( !strncmp ( data, "/* AUTH: ", 9 ) )
	{
		/* skip AUTH header */
		data += 9;
		length -= 9;

		/* get key index or signature */
		buffer = util_hex_get_buffer ( data, &bytes );

		if ( !buffer )
			return NULL;

		/* if it was key id */
		if ( bytes == 1 )
		{
			/* retrieve key */
			key_id = buffer[0];
			key = crypto_get_key ( key_id, keys );
			if ( !key )
			{
				free ( buffer );
				return NULL;
			}
			
			/* skip key id */
			data += 2;
			length -= 2;

			/* and spaces */
			while ( length && *data && *data == ' ' )
			{
				data++;
				length--;
			}

			free ( buffer );
			/* get signature */
			buffer = util_hex_get_buffer ( data, &bytes );
		}
		if ( !buffer || bytes == E_FAIL )
			return NULL;

		/* skip signature */
		data += 2*bytes;
		length -= 2*bytes;

		/* and spaces */
		while ( length && *data && *data == ' ' )
		{
			data++;
			length--;
		}

		/* some checks */
		if ( length < 2 || strncmp ( data, "*/\n", 2 ) )
		{
			free ( buffer );
			return NULL;
		}

		data += 3;
		length -= 3;

		buffer = realloc ( buffer, bytes + 16384 ); // 16k extra because RSA blocksize depends on key length
		bytes = crypto_crypt ( buffer, bytes, key );

		if ( bytes < 2*sizeof(digest)+1 )
		{
			free ( buffer );
			return NULL;
		}

		/* check code checksum */
		memset ( digest, 0x00, sizeof(digest) );
		md5_digest ( data, length, digest );
		if ( !memcmp ( digest, buffer, sizeof ( digest ) ) )
			checksum_valid = 1;

			
		/* check AUTH checksum */
		memset ( digest, 0x00, sizeof(digest) );
		md5_digest ( buffer, bytes-sizeof(digest), digest );
		if ( !memcmp ( digest, buffer+bytes - sizeof(digest), sizeof ( digest ) ) )
			auth_valid = 1;

		if ( checksum_valid && !auth_valid )
		{
			printf ( "[Crypto] FAKED AUTH DATA! Someone seems trying to fake the AUTH signature!\n" );
		}
		if ( !checksum_valid && !auth_valid )
		{
			printf ( "[Crypto] WRONG AUTH DATA! The AUTH signature has been modified!\n" );
		}
		if ( !checksum_valid && auth_valid )
		{
			printf ( "[Crypto] WRONG AUTH DATA! The code has been modified!\n" );
		}
		if ( checksum_valid && auth_valid )
		{
			tempbuf = realloc ( tempbuf, strlen ( crypto_get_keyname ( key_id, keys) ) + 1 + strlen ( buffer + sizeof(digest) ) + 1 );
			if ( !tempbuf )
				return NULL;
			strcpy ( tempbuf, crypto_get_keyname ( key_id, keys ) );
			strcat ( tempbuf, "|" );
			strcat ( tempbuf, buffer+sizeof ( digest ) );
			free ( buffer );
			return tempbuf;
		}

		free ( buffer );
	}

	return NULL;
}


char *
crypto_generate_cert ( char *data, int length, char *signature, t_crypt_key *key )
{
	MD5_DIGEST digest;
	unsigned char *buffer = NULL;
	unsigned char *tempbuf = NULL;
	int pos = 0;
	int bytes = 1;
	int status = 0;

	if ( !data || !signature || !key || length < 1  )
		return NULL;

	memset ( digest, 0x00, 16 );
	md5_digest ( data, length, digest );

	bytes = sizeof(digest) + strlen ( signature ) + 1 + sizeof(digest);
	while ( bytes & 0x07 )
		bytes++;

	buffer = malloc ( bytes + 16384 ); // 16k extra because RSA blocksize depends on key length
	if ( !buffer )
		return NULL;

	memset ( buffer, 0x00, bytes );
	memcpy ( buffer, digest, sizeof(digest));
	memcpy ( buffer+sizeof(digest), signature, strlen ( signature )+1 );
	memset ( digest, 0x00, 16 );
	md5_digest ( buffer, bytes - sizeof(digest), digest );

	memcpy ( (buffer+bytes) - sizeof(digest), digest, sizeof(digest));

	bytes = crypto_crypt ( buffer, bytes, key );

	tempbuf = util_hexpack ( buffer, bytes );
	free ( buffer );

	return tempbuf;
}

unsigned char *
crypto_encrypt_buffer ( unsigned char *in_buffer, int *length, t_crypt_key *key )
{
	int in_blocksize = 0;
	int out_blocksize = 0;
	int blocks = 0;
	int block = 0;
	int this_blocksize = 0;
	unsigned char *buffer = NULL;
	unsigned char *blockdata = NULL;
	unsigned char *blockdata_ptr = NULL;
	unsigned char *buffer_ptr = NULL;

	if ( !in_buffer || ! key || !length || !*length )
		return NULL;

	// get the block size for encrypt operation
	in_blocksize = crypto_rsa_get_blocksize ( key, 0 );
	out_blocksize = crypto_rsa_get_blocksize ( key, 1 );

	if ( in_blocksize == 0 || in_blocksize == E_FAIL || out_blocksize == E_FAIL )
		return NULL;

	// get block count
	blocks = (*length + 0x10) / in_blocksize;

	if ( (*length + 0x10) % in_blocksize )
		blocks++;

	// allocate the output buffer
	blockdata = malloc ( blocks * out_blocksize );
	memset ( blockdata, 0x00, blocks * out_blocksize  );

	// replicate the input buffer with information header
	buffer = malloc ( blocks * in_blocksize );
	memset ( buffer, 0x00, blocks * in_blocksize  ); // replace with random bytes to make more secure?
	memcpy ( buffer + 0x10, in_buffer, *length );


	buffer[0] = 1;
	buffer[1] = (in_blocksize & 0xFF000000)>> 24;
	buffer[2] = (in_blocksize & 0x00FF0000)>> 16;
	buffer[3] = (in_blocksize & 0x0000FF00)>> 8;
	buffer[4] = (in_blocksize & 0x000000FF)>> 0;
	buffer[5] = (out_blocksize & 0xFF000000)>> 24;
	buffer[6] = (out_blocksize & 0x00FF0000)>> 16;
	buffer[7] = (out_blocksize & 0x0000FF00)>> 8;
	buffer[8] = (out_blocksize & 0x000000FF)>> 0;
	buffer[9] = (*length & 0xFF000000)>> 24;
	buffer[10] = (*length & 0x00FF0000)>> 16;
	buffer[11] = (*length & 0x0000FF00)>> 8;
	buffer[12] = (*length & 0x000000FF)>> 0;
	buffer[13] = 0x55;
	buffer[14] = 0xAA;
	buffer[15] = 0x55;


	// now update the returned length
	*length = blocks * out_blocksize;

	while ( block < blocks )
	{
		blockdata_ptr = blockdata + block * out_blocksize;
		buffer_ptr = buffer + block*in_blocksize;
		
		memcpy ( blockdata_ptr, buffer_ptr, in_blocksize );

		this_blocksize = crypto_crypt ( blockdata_ptr, in_blocksize, key );

		if ( this_blocksize == E_FAIL )
		{
			free ( buffer );
			free ( blockdata );
			return NULL;
		}

		if ( out_blocksize-this_blocksize > 0)
		{
			memmove ( blockdata_ptr + (out_blocksize-this_blocksize), blockdata_ptr, this_blocksize );
			memset ( blockdata_ptr, 0x00, out_blocksize-this_blocksize );
		}
		else if ( out_blocksize-this_blocksize < 0)
			return NULL;
		block++;
	}

	return blockdata;
}

unsigned char *
crypto_decrypt_buffer ( unsigned char *in_buffer, int *length, t_crypt_key *key )
{
	int in_blocksize = 0;
	int out_blocksize = 0;
	int blocks = 0;
	int block = 0;
	int this_blocksize = 0;
	unsigned char *buffer = NULL;
	unsigned char *blockdata = NULL;
	unsigned char *blockdata_ptr = NULL;
	unsigned char *buffer_ptr = NULL;

	if ( !in_buffer || !key || !length || !*length || !key->key || !key->primefac )
		return NULL;

	// get the block size for encrypt operation
	in_blocksize = crypto_rsa_get_blocksize ( key, 1 );
	out_blocksize = crypto_rsa_get_blocksize ( key, 0 );

	// input size does not fit the blocksize
	if ( *length % in_blocksize )
		return NULL;

	// get block count
	blocks = *length / in_blocksize;
	blockdata = malloc ( blocks * out_blocksize );
	memset ( blockdata, 0x00, blocks * out_blocksize );

	// save the buffer we now will operate on
	buffer = malloc ( *length );
	memcpy ( buffer, in_buffer, *length );

	while ( block < blocks )
	{
		blockdata_ptr = blockdata + block*out_blocksize;
		buffer_ptr = buffer + block*in_blocksize;
		
		this_blocksize = crypto_crypt ( buffer_ptr, in_blocksize, key );
//		util_dump_bytes ( buffer_ptr, 16, this_blocksize );

		// fix the "alignment" problems due to shorter values with preceeding zeroes
		if ( out_blocksize - this_blocksize > 0)
		{
			memmove ( blockdata_ptr + (out_blocksize-this_blocksize), blockdata_ptr, this_blocksize );
			memset ( blockdata_ptr, 0x00, out_blocksize-this_blocksize );
		}
		else if ( out_blocksize - this_blocksize < 0 )
		{
			// if blocksize somehow was too big, return
			free ( buffer );
			return NULL;
		}

		if ( block == 0 )
		{
			// some checks to ensure it was crypted with same key
			if ( buffer[0] != 1 || buffer[13] != 0x55 || buffer[14] != 0xAA || buffer[15] != 0x55 
			     || out_blocksize != (buffer[1]<<24 | buffer[2]<<16 | buffer[3]<<8 | buffer[4]) 
				 || in_blocksize != (buffer[5]<<24 | buffer[6]<<16 | buffer[7]<<8 | buffer[8]) )
			{
				free ( buffer );
				return NULL;
			}

			// get the resulting length
			*length = (buffer[9]<<24 | buffer[10]<<16 | buffer[11]<<8 | buffer[12]);
			if ( *length < 0 )
			{
				free ( buffer );
				return NULL;
			}
		}

		memcpy ( blockdata_ptr, buffer_ptr, out_blocksize );
		block++;
	}

	memmove ( blockdata, blockdata + 0x10, *length );
	realloc ( blockdata, *length );
	free ( buffer );

	return blockdata;
}

unsigned int crypto_generate_key ( int nbits, t_crypt_key *priv_key, t_crypt_key *pub_key )
{
	return rsa_generate_key ( nbits, priv_key, pub_key );
}

#endif
