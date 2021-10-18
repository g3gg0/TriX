
#ifndef __TRIX_TRIXPLUG_TRIXCRYPT_H__
#define __TRIX_TRIXPLUG_TRIXCRYPT_H__

/* trixplug struct */

struct trixcrypt_funcs
{
	unsigned int (*alloc_priv) ( t_stage * stage);
	unsigned int (*set_cryptkeys) ( t_crypt_key * encrypt_key, t_crypt_key * decrypt_key);
	unsigned int (*set_decryptkey) ( t_crypt_key * key);
	unsigned int (*set_encryptkey) ( t_crypt_key * key);
	unsigned int (*copy_cryptkey) ( t_crypt_key * dst, t_crypt_key * src);
};

/* trixplug struct initializer */

#define TRIXCRYPT_PLUG_INIT \
extern struct trix_functions *ft;\
struct trixcrypt_funcs trixcrypt_functions;\
unsigned int trixcrypt_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->trixcrypt = &trixcrypt_functions;\
	ft->trixcrypt->alloc_priv = trixcrypt_alloc_priv;\
	ft->trixcrypt->copy_cryptkey = trixcrypt_copy_cryptkey;\
	ft->trixcrypt->set_decryptkey = trixcrypt_set_decryptkey;\
	ft->trixcrypt->set_encryptkey = trixcrypt_set_encryptkey;\
	ft->trixcrypt->set_cryptkeys = trixcrypt_set_cryptkeys;\
\
	return E_OK;\
}

#endif
