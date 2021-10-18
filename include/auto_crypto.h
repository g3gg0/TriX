

/* seer headers for crypto.c */

#define CRYPTO_AUTO_IMPORTS \
	"import char *__crypto_get_signkey ( char *signature);\n"\
	"import char *__crypto_get_signer ( char *signature);\n"\
	"import char *__crypto_get_signee ( char *signature);\n"\
	"import char *__crypto_get_signopt ( char *signature);\n"\
	"import t_crypt_key *__crypto_get_key ( unsigned char key_id, t_crypt_key * keys);\n"\
	"import char *__crypto_get_keyname ( unsigned char key_id, t_crypt_key * keys);\n"\
	"import char *__crypto_check_integrity ( char **in_data, int length, t_crypt_key * keys);\n"\
	"import char *__crypto_generate_cert ( char *data, int length, char *signature, t_crypt_key * key);\n"\
	"import unsigned char *__crypto_encrypt_buffer ( unsigned char *in_buffer, int *length, t_crypt_key * key);\n"\
	"import unsigned char *__crypto_decrypt_buffer ( unsigned char *in_buffer, int *length, t_crypt_key * key);\n"\
	"import unsigned int __crypto_generate_key ( int nbits, t_crypt_key * priv_key, t_crypt_key * pub_key);\n"\



#define CRYPTO_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__crypto_get_signkey</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">signature</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__crypto_get_signer</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">signature</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__crypto_get_signee</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">signature</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__crypto_get_signopt</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">signature</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*<font color=\"#000000\">__crypto_get_key</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;<font color=\"#000000\">key_id</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">keys</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__crypto_get_keyname</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;<font color=\"#000000\">key_id</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">keys</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__crypto_check_integrity</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">in_data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">keys</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__crypto_generate_cert</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">signature</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">key</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__crypto_encrypt_buffer</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">in_buffer</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">length</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">key</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__crypto_decrypt_buffer</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">in_buffer</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">length</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">key</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__crypto_generate_key</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">nbits</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">priv_key</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">pub_key</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for crypto.c */

#define CRYPTO_AUTO_REGISTER \
	scAddExtSymInt ( crypto_get_signkey );\
	scAddExtSymInt ( crypto_get_signer );\
	scAddExtSymInt ( crypto_get_signee );\
	scAddExtSymInt ( crypto_get_signopt );\
	scAddExtSymInt ( crypto_get_key );\
	scAddExtSymInt ( crypto_get_keyname );\
	scAddExtSymInt ( crypto_check_integrity );\
	scAddExtSymInt ( crypto_generate_cert );\
	scAddExtSymInt ( crypto_encrypt_buffer );\
	scAddExtSymInt ( crypto_decrypt_buffer );\
	scAddExtSymInt ( crypto_generate_key );\


/* seer function declaration for crypto.c */
#ifndef HEADER_SKIP_DECLARATION
void crypto_get_signkey ();
void crypto_get_signer ();
void crypto_get_signee ();
void crypto_get_signopt ();
void crypto_get_key ();
void crypto_get_keyname ();
void crypto_check_integrity ();
void crypto_generate_cert ();
void crypto_encrypt_buffer ();
void crypto_decrypt_buffer ();
void crypto_generate_key ();

#endif
/* additional seer includes from crypto_seer.h */

#define CRYPTO_AUTO_MISC_PRE \
	"\n"\
	"typedef struct s_crypt_key {\n"\
	"	unsigned char id;\n"\
	"	char *name;\n"\
	"	char *primefac;\n"\
	"	char *key;\n"\
	"} t_crypt_key;\n"\
	"\n"\
	"\n"\


#define CRYPTO_AUTO_MISC_POST \
	"\n"\
	"unsigned int CryptoGenerateKey ( int bits, t_crypt_key *priv_key, t_crypt_key *pub_key ) { return __crypto_generate_key ( bits, priv_key, pub_key ); }\n"\
	"char *CryptoCheckIntegrity ( char **data, int length, t_crypt_key *key ) { return __crypto_check_integrity ( data, length, key ) ;}\n"\
	"char *CryptoGenerateCert ( char *data, int length, char *signature, t_crypt_key *key ) { return __crypto_generate_cert ( data, length, signature, key );}\n"\
	"char *CryptoGetSigner ( char *signature ) { return __crypto_get_signer ( signature );}\n"\
	"char *CryptoGetSignee ( char *signature ) { return __crypto_get_signee ( signature );}\n"\
	"char *CryptoGetSignkey ( char *signature ) { return __crypto_get_signkey ( signature );}\n"\
	"\n"\


#define CRYPTO_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_crypt_key</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;<font color=\"#000000\">id</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">primefac</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">key</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_crypt_key</font>;<br>"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define CRYPTO_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">CryptoGenerateKey</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bits</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*<font color=\"#000000\">priv_key</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*<font color=\"#000000\">pub_key</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__crypto_generate_key</font>&nbsp;(&nbsp;<font color=\"#000000\">bits</font>,&nbsp;<font color=\"#000000\">priv_key</font>,&nbsp;<font color=\"#000000\">pub_key</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">CryptoCheckIntegrity</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*<font color=\"#000000\">key</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__crypto_check_integrity</font>&nbsp;(&nbsp;<font color=\"#000000\">data</font>,&nbsp;<font color=\"#000000\">length</font>,&nbsp;<font color=\"#000000\">key</font>&nbsp;)&nbsp;;}<br>"\
	"<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">CryptoGenerateCert</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">signature</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*<font color=\"#000000\">key</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__crypto_generate_cert</font>&nbsp;(&nbsp;<font color=\"#000000\">data</font>,&nbsp;<font color=\"#000000\">length</font>,&nbsp;<font color=\"#000000\">signature</font>,&nbsp;<font color=\"#000000\">key</font>&nbsp;);}<br>"\
	"<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">CryptoGetSigner</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">signature</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__crypto_get_signer</font>&nbsp;(&nbsp;<font color=\"#000000\">signature</font>&nbsp;);}<br>"\
	"<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">CryptoGetSignee</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">signature</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__crypto_get_signee</font>&nbsp;(&nbsp;<font color=\"#000000\">signature</font>&nbsp;);}<br>"\
	"<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">CryptoGetSignkey</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">signature</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__crypto_get_signkey</font>&nbsp;(&nbsp;<font color=\"#000000\">signature</font>&nbsp;);}<br>"\
	"<br>"\
	"		</font>"\


