#ifndef __TRIX_FMT_TRIXCRYPT_C__
#define __TRIX_FMT_TRIXCRYPT_C__

#include <stdlib.h>
#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "crypto.h"
#include "fmt_trixcrypt.h"
#include "options.h"

#include "mem.h"

t_crypt_key decrypt_key;
t_crypt_key encrypt_key;

unsigned int
fmt_trixcrypt_free ( t_stage * s )
{
    if ( !s )
        return E_OK;

    segment_release_all ( s->segments );
    s->segments = NULL;

    return E_OK;
}

char *
fmt_trixcrypt_encode_data ( char *buf, unsigned int length, unsigned int *c_length, t_crypt_key *key )
{
	unsigned char *buffer = NULL;
	unsigned char *encoded = NULL;
	unsigned int encoded_length = 0;

/*
	printf ( "before crypt:\n" );
	util_dump_bytes ( buf, 16, length );
	printf ( "\n" );
*/	


	encoded_length = length;
	encoded = crypto_encrypt_buffer ( buf, &encoded_length, key );
	if ( !encoded )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) encryption failed !!\n", __FUNCTION__ );
        return NULL;
    }

	buffer = malloc ( encoded_length + 0x10 );
	if ( !buffer )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) malloc failed !!\n", __FUNCTION__ );
        return NULL;
    }

	memset ( buffer, 0x00, encoded_length + 0x10 );
	memcpy ( buffer, "[TriXCrypT-RSA-]", 16 );
	memcpy ( buffer + 0x10, encoded, encoded_length );

	free ( encoded );

	*c_length = encoded_length + 0x10;

	/*
	printf ( "after crypt:\n" );
	util_dump_bytes ( buffer, 16, *c_length );
	printf ( "\n" );
	*/

	return buffer;
}


char *
fmt_trixcrypt_decode_data ( char *buf, unsigned int length, unsigned int *o_length, t_crypt_key *key )
{
	unsigned char *decoded = NULL;
	int decoded_length = 0;

/*	
	printf ( "before decrypt:\n" );
	util_dump_bytes ( buf, 16, length );
	printf ( "\n" );
*/

	if ( length < 16 || memcmp ( buf, "[TriXCrypT-RSA-]", 16 ) )
        return NULL;

	decoded_length = length-0x10;
	decoded = crypto_decrypt_buffer ( buf+0x10, &decoded_length, key );
	if ( !decoded )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) decryption failed !!\n", __FUNCTION__ );
        return NULL;
    }

	*o_length = decoded_length;

/*
	printf ( "after decrypt:\n" );
	util_dump_bytes ( decoded, 16, *o_length );
	printf ( "\n" );
*/
	return decoded;
}

unsigned int
fmt_trixcrypt_decode ( t_stage * source, t_stage * target )
{
    char *decoded = NULL;
    int length = 0;
	t_trixcrypt_priv *priv = NULL;

	
	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );

    if ( !target )
        target = source->next;

	if ( !source || !target || !source->segments || !source->segments->data /*|| !target->priv */ )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }


	decoded = fmt_trixcrypt_decode_data ( source->segments->data, source->segments->length, &length, /*&(priv->decrypt_key)*/ &decrypt_key );
	if ( decoded )
		decoded = realloc ( decoded, length + 1 );

    if ( !decoded )
    {
		DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }

    target->name = "DECODED";
    target->length = length;
    target->segments = segment_create (  );
    target->segments->name = "DATA";
    target->segments->data = decoded;
    target->segments->start = 0;
    target->segments->end = length;
    target->segments->length = length;
    target->parser = "TRIXCRYPT";
    target->type = "TRIXCRYPT";

	// in case of text files, make sure it will end with an NULL byte
	target->segments->data[target->segments->length] = '\000'; 

    target->flags = S_MODIFIED;

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;
}

unsigned int
fmt_trixcrypt_encode ( t_stage * source, t_stage * target )
{
    unsigned int length = 0;
    char *encoded = NULL;
	t_trixcrypt_priv *priv = NULL;

	DBG ( DEBUG_FMT, " => %s ( ) called\n", __FUNCTION__ );
    if ( !target )
        target = source->next;

	if ( !source || !target || !source->segments || !source->segments->data /* || !source->priv*/ )
    {
	    DBG ( DEBUG_FMT, " -> %s ( ) failed !!\n", __FUNCTION__ );
        return E_FAIL;
    }

    if ( segment_count ( source->segments ) != 1 )
    {
	    DBG ( DEBUG_FMT, " -> %s ( %s, %s ) failed !!\n", __FUNCTION__, source->name, target->name );
        return E_FAIL;
    }


	encoded = fmt_trixcrypt_encode_data ( GET_PTR ( source, 0 ), GET_SIZE ( source ), &length, /*&(priv->encrypt_key)*/ &encrypt_key );
    if ( !encoded || !length )
        return E_FAIL;

    target->segments = segment_create (  );
    target->segments->name = "DATA";
    target->segments->start = 0;
    target->segments->end = length;
    target->segments->length = length;
    target->segments->data = encoded;

	target->name = "RAW";
    target->length = length;
    target->parser = "TRIXCRYPT";
    target->type = "TRIXCRYPT";

	/* set the parser again (see file_format() ). all parsers should do this. shouldn't they? */
	source->parser = "TRIXCRYPT";
	source->type = "TRIXCRYPT";

	DBG ( DEBUG_FMT, " ## %s ( %s, %s ) done\n", __FUNCTION__, source->name, target->name );
    return E_OK;

}

t_fmt_handler trixcrypt_handler = {
    "TRIXCRYPT",
    "ENCRYPTION",
	"fmt_trixcrypt_decode",
    fmt_trixcrypt_decode,
	"fmt_trixcrypt_encode",
    fmt_trixcrypt_encode,
	"fmt_trixcrypt_free",
    fmt_trixcrypt_free,
	NULL,
	NULL
};

unsigned int
fmt_trixcrypt_init (  )
{
//	crypto_generate_key ( 384, &decrypt_key, &encrypt_key  );

	fmt_register_handler ( &trixcrypt_handler );

	options_add_core_option ( OPT_STR, "fmt.trixcrypt", decrypt_key.key, "Decryption key" );
	options_add_core_option ( OPT_STR, "fmt.trixcrypt", decrypt_key.primefac, "Key primefact" );
	options_add_core_option ( OPT_STR, "fmt.trixcrypt", encrypt_key.key, "Encryption key" );
	options_add_core_option ( OPT_STR, "fmt.trixcrypt", encrypt_key.primefac, "Key primefact" );

	return E_OK;
}

#endif
