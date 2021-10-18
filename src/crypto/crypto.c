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
#include "crypto_aes.h"
#include "crypto_sha1.h"

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
		return E_FAIL;

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



unsigned int
crypto_init_algokey ( unsigned char *algokey_data, unsigned int algokey_length, t_crypt_key *rsa_key )
{
	unsigned char *rsakey_data = NULL;
	int loops = 8192;
	int pos = 0;
	int rsakey_length = 0;
	int last_bit = 0;
	int next_last_bit = 0;


	if ( !algokey_data ||!rsa_key )
		return E_FAIL;

	rsakey_data = (unsigned char*)util_hexunpack ( rsa_key->primefac, &rsakey_length );

	if ( !rsakey_data )
		return E_FAIL;

	memset( algokey_data, 0xFF, algokey_length );

	// initialize random number generator with some random data
	srand ((unsigned int)time(NULL) ^ (unsigned int)rand() ^ (unsigned int)rsakey_data);

	// fill key with random data
	while ( pos < loops )
	{
		// fill key with random data, also depending on rsa key
		algokey_data[pos%algokey_length] += rand()%0xFF;
		algokey_data[pos%algokey_length] += rsakey_data[(pos+(rsakey_length/2))%rsakey_length];
		algokey_data[pos%algokey_length] ^= rsakey_data[pos%rsakey_length];
		algokey_data[pos%algokey_length] ^= rand()%0xFF;

		// rotate right the whole buffer to spread  randomness
		next_last_bit = algokey_data[pos%algokey_length] & 1;
		algokey_data[pos%algokey_length] >>= 1;
		algokey_data[pos%algokey_length] |= (last_bit<<7);
		last_bit = next_last_bit;

		pos++;
	}

	free ( rsakey_data );
	return E_OK;
}


unsigned int
crypto_init_algoiv ( unsigned char *algoiv_data, unsigned int algoiv_length, t_crypt_key *rsa_key )
{
	unsigned char *rsakey_data = NULL;
	int loops = 8192;
	int pos = 0;
	int rsakey_length = 0;
	int last_bit = 0;
	int next_last_bit = 0;

	if ( !algoiv_data ||!rsa_key )
		return E_FAIL;

	rsakey_data = (unsigned char*)util_hexunpack ( rsa_key->primefac, &rsakey_length );

	if ( !rsakey_data )
		return E_FAIL;

	memset( algoiv_data, 0xFF, algoiv_length );

	// fill iv with key data
	while ( pos < loops )
	{
		algoiv_data[pos%algoiv_length] += rsakey_data[pos%rsakey_length];
		algoiv_data[pos%algoiv_length] ^= rsakey_data[(pos+(rsakey_length/2))%rsakey_length];

		// rotate right the whole buffer to spread randomness
		next_last_bit = algoiv_data[pos%algoiv_length] & 1;
		algoiv_data[pos%algoiv_length] >>= 1;
		algoiv_data[pos%algoiv_length] |= (last_bit<<7);
		last_bit = next_last_bit;

		pos++;
	}

	free ( rsakey_data );
	return E_OK;
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
	
	unsigned int total_size = 0;
	unsigned int header_size = 0;
	unsigned int payload_size = 0;

	// AES stuff
	unsigned int algo_ivlength = 16;
	unsigned int algo_blocksize = 16;
	unsigned int algo_keylength = 32;
	aes_context algo_ctx;


	t_crypt_header *header_struct = NULL;

	if ( !in_buffer || !key || !length || !*length )
		return NULL;

	// get the block size for encrypt operation
	in_blocksize = crypto_rsa_get_blocksize ( key, 0 );
	out_blocksize = crypto_rsa_get_blocksize ( key, 1 );

	if ( in_blocksize == 0 || in_blocksize == E_FAIL || out_blocksize == E_FAIL )
		return NULL;

	// get block count for header
	blocks = sizeof(t_crypt_header) / in_blocksize;
	if ( sizeof(t_crypt_header) % in_blocksize )
		blocks++;

	// build header buffer
	buffer = malloc ( blocks * in_blocksize );
	memset ( buffer, 0x00, blocks * in_blocksize  ); // replace with random bytes to make more secure?
	header_struct = (t_crypt_header*) buffer;

	// use big endian SET_WORD_RAW
	SET_WORD_RAW(&header_struct->header, 0, TRIXCRYPT_HEADER);
	SET_WORD_RAW(&header_struct->length, 0, *length);
	SET_WORD_RAW(&header_struct->trailer, 0, TRIXCRYPT_TRAILER);

	// initialize key and IV (IV is based on RSA key)
	crypto_init_algoiv ( algo_ctx.iv, algo_ivlength, key );
	crypto_init_algokey ( header_struct->keydata, algo_keylength, key );
	aes_setkey_enc( &algo_ctx, header_struct->keydata, algo_keylength * 8 );

	// calc the output buffer size for header plus data
	header_size = blocks * out_blocksize;
	payload_size = *length;

	if ( payload_size % algo_blocksize > 0 )
		payload_size += algo_blocksize - (payload_size % algo_blocksize);

	// new length is header blocks plus payload
	total_size = header_size + payload_size;
	blockdata = malloc ( total_size );
	memset ( blockdata, 0x00, total_size );

	// copy the source data
	memcpy ( blockdata + (blocks * out_blocksize), in_buffer, *length );


	// encrypt header with RSA
	while ( block < blocks && !util_script_is_aborted () )
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

	// start with AES encryption
	blocks = payload_size / algo_blocksize;
	block = 0;

	// encrypt data
	while ( block < blocks && !util_script_is_aborted () )
	{
		blockdata_ptr = blockdata + header_size + block * algo_blocksize;

		aes_crypt_cbc( &algo_ctx, AES_ENCRYPT, algo_blocksize, blockdata_ptr, blockdata_ptr );
//		aes256_encrypt_ecb(&aes_ctx, blockdata_ptr);

		block++;
	}

	*length = total_size;

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

	// AES stuff
	unsigned int algo_ivlength = 16;
	unsigned int algo_blocksize = 16;
	unsigned int algo_keylength = 32;
	aes_context algo_ctx;

	t_crypt_header *header_struct = NULL;


	if ( !in_buffer || !key || !length || !*length || !key->key || !key->primefac )
		return NULL;

	// get the block size for encrypt operation
	in_blocksize = crypto_rsa_get_blocksize ( key, 1 );
	out_blocksize = crypto_rsa_get_blocksize ( key, 0 );

	if ( out_blocksize < 1 || in_blocksize < 1 )
		return NULL;

	// get block count for header
	blocks = sizeof(t_crypt_header) / out_blocksize;
	if ( sizeof(t_crypt_header) % out_blocksize )
		blocks++;

	// input size does not fit the blocksize
	if ( *length < blocks * in_blocksize )
		return NULL;

	// get block count
	blockdata = malloc ( blocks * out_blocksize );
	memset ( blockdata, 0x00, blocks * out_blocksize );

	// save the buffer we now will operate on
	buffer = malloc ( blocks * in_blocksize );
	memcpy ( buffer, in_buffer, blocks * in_blocksize );

	// decrypt header with RSA
	while ( block < blocks )
	{
		blockdata_ptr = blockdata + block*out_blocksize;
		buffer_ptr = buffer + block*in_blocksize;
		
		this_blocksize = crypto_crypt ( buffer_ptr, in_blocksize, key );

		// fix the "alignment" problems due to shorter values with preceeding zeroes
		if ( out_blocksize - this_blocksize > 0)
		{
			memmove ( buffer_ptr + (out_blocksize-this_blocksize), buffer_ptr, this_blocksize );
			memset ( buffer_ptr, 0x00, out_blocksize-this_blocksize );
		}
		else if ( out_blocksize - this_blocksize < 0 )
		{
			// if blocksize somehow was too big, return
			free ( blockdata );
			free ( buffer );
			return NULL;
		}

		memcpy ( blockdata_ptr, buffer_ptr, out_blocksize );
		block++;
	}

	free ( buffer );

	header_struct = (t_crypt_header*)blockdata;

	if ( GET_WORD_RAW(&header_struct->header, 0) != TRIXCRYPT_HEADER )
	{
		free ( blockdata );
		return NULL;
	}

	if ( GET_WORD_RAW(&header_struct->trailer, 0) != TRIXCRYPT_TRAILER )
	{
		free ( blockdata );
		return NULL;
	}

	if ( GET_WORD_RAW(&header_struct->length, 0) > (*length - (blocks * in_blocksize)) )
	{
		free ( blockdata );
		return NULL;
	}

	if ( (*length - (blocks * in_blocksize)) % algo_blocksize )
	{
		free ( blockdata );
		return NULL;
	}

	// new buffer 
	buffer = malloc ( *length - (blocks * in_blocksize) );
	memcpy ( buffer, in_buffer + (blocks * in_blocksize), *length - (blocks * in_blocksize) );

	// initialize key and IV (IV is based on RSA key)
	crypto_init_algoiv ( algo_ctx.iv, algo_ivlength, key );
	aes_setkey_dec( &algo_ctx, header_struct->keydata, algo_keylength * 8 );

	// start with AES decryption
	blocks = (*length - (blocks * in_blocksize)) / algo_blocksize;
	block = 0;

	// decrypt data
	while ( block < blocks && !util_script_is_aborted () )
	{
		buffer_ptr = buffer + block * algo_blocksize;

		aes_crypt_cbc( &algo_ctx, AES_DECRYPT, algo_blocksize, buffer_ptr, buffer_ptr );

		block++;
	}

	*length = GET_WORD_RAW(&header_struct->length, 0);
	realloc ( buffer, *length );
	free ( blockdata );

	return buffer;
}

unsigned int crypto_generate_key ( int nbits, t_crypt_key *priv_key, t_crypt_key *pub_key )
{
	return rsa_generate_key ( nbits, priv_key, pub_key );
}

unsigned int crypto_generate_sha1 ( const unsigned char *data, unsigned int length, char *hash )
{
	SHA1Context sha;
	unsigned int i;

	SHA1Reset ( &sha );
	SHA1Input ( &sha, data, length );

	if ( !SHA1Result ( &sha ) || !hash )
		return E_FAIL;

	for ( i = 0; i < 5; i++ )
	{
		hash[ ( i << 2 ) + 0 ] = BYTEPOS ( sha.Message_Digest[i], 3);
		hash[ ( i << 2 ) + 1 ] = BYTEPOS ( sha.Message_Digest[i], 2);
		hash[ ( i << 2 ) + 2 ] = BYTEPOS ( sha.Message_Digest[i], 1);
		hash[ ( i << 2 ) + 3 ] = BYTEPOS ( sha.Message_Digest[i], 0);
	}

	return E_OK;
}

unsigned int crypto_generate_md5 ( const unsigned char *data, unsigned int length, char *hash )
{
	if ( !data || !length || !hash )
		return E_FAIL;

	md5_digest ( data, length, hash );

	return E_OK;
}

#endif
