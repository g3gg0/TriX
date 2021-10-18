#ifndef __TRIX_TRIXCRYPT_C__
#define __TRIX_TRIXCRYPT_C__

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "fmt.h"
#include "crypto.h"
#include "fmt_trixcrypt.h"
#include "trixcrypt.h"

#include "trixplug_funcs.h"
#include "mem.h"

extern t_crypt_key decrypt_key;
extern t_crypt_key encrypt_key;

TRIXCRYPT_PLUG_INIT;

unsigned int trixcrypt_alloc_priv ( t_stage *stage )
{
	t_trixcrypt_priv *priv = NULL;

	if ( !stage )
		return E_FAIL;

	if ( stage->priv && !is_type ( stage->priv, "t_trixcrypt_priv" ) )
	{
		ERR ( 1, "trixcrypt_alloc_priv: already allocated a different priv data i this stage" );
		return E_FAIL;
	}

	if ( stage->priv )
		return E_OK;

	priv = ((t_trixcrypt_priv*) PRIV_MALLOC ( t_trixcrypt_priv ));
	if ( !priv ) 
	{
		ERR ( 1, "trixcrypt_alloc_priv: ERROR allocating private struct" );
		return E_FAIL;
	}

	stage->priv = (t_stage_priv*)priv;

	return E_OK;
}

unsigned int trixcrypt_set_decryptkey (  t_crypt_key *key )
{
	if ( !key )
		return E_FAIL;


	memcpy ( &decrypt_key, key, sizeof ( t_crypt_key ) );

	return E_OK;
}

unsigned int trixcrypt_set_encryptkey ( t_crypt_key *key )
{

	if (  !key )
		return E_FAIL;

	memcpy ( &encrypt_key, key, sizeof ( t_crypt_key ) );

	return E_OK;
}

unsigned int trixcrypt_set_cryptkeys ( t_crypt_key *encrypt_key, t_crypt_key *decrypt_key )
{
	if ( encrypt_key )
	{
		R( trixcrypt_set_encryptkey (  encrypt_key ) );
	}
	if ( decrypt_key )
	{
		R( trixcrypt_set_decryptkey (  decrypt_key ) );
	}
	return E_OK;
}

#endif
