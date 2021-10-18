
typedef struct s_crypt_key {
	unsigned char id;
	char *name;
	char *primefac;
	char *key;
} t_crypt_key;

// INSERT_DECLARATIONS

unsigned int CryptoGenerateKey ( int bits, t_crypt_key *priv_key, t_crypt_key *pub_key ) { return __crypto_generate_key ( bits, priv_key, pub_key ); }
unsigned int CryptoGenerateSHA1 ( const unsigned char *data, unsigned int length, char *hash ) { return __crypto_generate_sha1 ( data, length, hash ); }
unsigned int CryptoGenerateMD5 ( const unsigned char *data, unsigned int length, char *hash ) { return __crypto_generate_md5 ( data, length, hash ); }
char *CryptoCheckIntegrity ( char **data, int length, t_crypt_key *key ) { return __crypto_check_integrity ( data, length, key ) ;}
char *CryptoGenerateCert ( char *data, int length, char *signature, t_crypt_key *key ) { return __crypto_generate_cert ( data, length, signature, key );}
char *CryptoGetSigner ( char *signature ) { return __crypto_get_signer ( signature );}
char *CryptoGetSignee ( char *signature ) { return __crypto_get_signee ( signature );}
char *CryptoGetSignkey ( char *signature ) { return __crypto_get_signkey ( signature );}

