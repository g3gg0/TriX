#ifndef __TRIX_FMT_TRIXCRYPT_H__
#define __TRIX_FMT_TRIXCRYPT_H__


typedef struct s_trixcrypt_priv t_trixcrypt_priv;
struct s_trixcrypt_priv
{
	STRUCT_HEADER;
	t_crypt_key encrypt_key;
	t_crypt_key decrypt_key;
};

#endif
