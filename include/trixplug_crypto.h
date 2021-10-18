
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
 	int (*unfold) ( char **in_data, int length, t_crypt_key * keys);
 	unsigned int (*init_algoiv) ( unsigned char *algoiv_data, unsigned int algoiv_length, t_crypt_key * rsa_key);
 	unsigned int (*init_algokey) ( unsigned char *algokey_data, unsigned int algokey_length, t_crypt_key * rsa_key);
 	unsigned int (*generate_sha1) ( const unsigned char *data, unsigned int length, char *hash);
 	unsigned int (*generate_md5) ( const unsigned char *data, unsigned int length, char *hash);
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
	ft->crypto->unfold = crypto_unfold;\
	ft->crypto->check_integrity = crypto_check_integrity;\
	ft->crypto->generate_cert = crypto_generate_cert;\
	ft->crypto->init_algokey = crypto_init_algokey;\
	ft->crypto->init_algoiv = crypto_init_algoiv;\
	ft->crypto->encrypt_buffer = crypto_encrypt_buffer;\
	ft->crypto->decrypt_buffer = crypto_decrypt_buffer;\
	ft->crypto->generate_key = crypto_generate_key;\
	ft->crypto->generate_sha1 = crypto_generate_sha1;\
	ft->crypto->generate_md5 = crypto_generate_md5;\
\
	return E_OK;\
}

#endif
