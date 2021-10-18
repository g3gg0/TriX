#ifndef __TRIX_TRIXCRYPT_H__
#define __TRIX_TRIXCRYPT_H__


unsigned int trixcrypt_set_decryptkey (  t_crypt_key *key );
unsigned int trixcrypt_set_encryptkey (  t_crypt_key *key );
unsigned int trixcrypt_alloc_priv ( t_stage *stage );
unsigned int trixcrypt_set_cryptkeys ( t_crypt_key *encrypt_key, t_crypt_key *decrypt_key );
unsigned int trixcrypt_copy_cryptkey ( t_crypt_key *dst, t_crypt_key *src );

#include "trixplug_trixcrypt.h"

#endif
