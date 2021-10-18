
#ifndef __TRIX_TRIXPLUG_CRYPTO_H__
#define __TRIX_TRIXPLUG_CRYPTO_H__

/* trixplug struct */

struct crypto_funcs
{
	char *(*check_integrity) ( char **in_data, int length, t_crypt_key * keys);
	char *(*generate_cert) ( char *data, int length, char *signature, t_crypt_key * key);
	char *(*get_keyname) ( unsigned char key_id, t_crypt_key * keys);
	char *(*get_signee) ( char *signature);
	char *(*get_signer) ( char *signature);
	char *(*get_signkey) ( char *signature);
	char *(*get_signopt) ( char *signature);
	t_crypt_key *(*get_key) ( unsigned char key_id, t_crypt_key * keys);
	unsigned char *(*decrypt_buffer) ( unsigned char *in_buffer, int *length, t_crypt_key * key);
	unsigned char *(*encrypt_buffer) ( unsigned char *in_buffer, int *length, t_crypt_key * key);
	unsigned int (*generate_key) ( int nbits, t_crypt_key * priv_key, t_crypt_key * pub_key);
};

/* trixplug struct initializer */

#define CRYPTO_PLUG_INIT \
extern struct trix_functions *ft;\
struct crypto_funcs crypto_functions;\
unsigned int crypto_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->crypto = &crypto_functions;\
	ft->crypto->get_signkey = crypto_get_signkey;\
	ft->crypto->get_signer = crypto_get_signer;\
	ft->crypto->get_signee = crypto_get_signee;\
	ft->crypto->get_signopt = crypto_get_signopt;\
	ft->crypto->get_key = crypto_get_key;\
	ft->crypto->get_keyname = crypto_get_keyname;\
	ft->crypto->check_integrity = crypto_check_integrity;\
	ft->crypto->generate_cert = crypto_generate_cert;\
	ft->crypto->encrypt_buffer = crypto_encrypt_buffer;\
	ft->crypto->decrypt_buffer = crypto_decrypt_buffer;\
	ft->crypto->generate_key = crypto_generate_key;\
\
	return E_OK;\
}

#endif
