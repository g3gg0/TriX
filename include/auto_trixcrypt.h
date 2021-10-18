

/* seer headers for trixcrypt.c */

#define TRIXCRYPT_AUTO_IMPORTS \
	"import unsigned int __trixcrypt_alloc_priv ( t_stage * stage);\n"\
	"import unsigned int __trixcrypt_copy_cryptkey ( t_crypt_key * dst, t_crypt_key * src);\n"\
	"import unsigned int __trixcrypt_set_decryptkey ( t_crypt_key * key);\n"\
	"import unsigned int __trixcrypt_set_encryptkey ( t_crypt_key * key);\n"\
	"import unsigned int __trixcrypt_set_cryptkeys ( t_crypt_key * encrypt_key, t_crypt_key * decrypt_key);\n"\



#define TRIXCRYPT_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__trixcrypt_alloc_priv</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">stage</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__trixcrypt_copy_cryptkey</font>&nbsp;(&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">dst</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">src</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__trixcrypt_set_decryptkey</font>&nbsp;(&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">key</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__trixcrypt_set_encryptkey</font>&nbsp;(&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">key</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__trixcrypt_set_cryptkeys</font>&nbsp;(&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">encrypt_key</font>,&nbsp;<font color=\"#000000\">t_crypt_key</font>&nbsp;*&nbsp;<font color=\"#000000\">decrypt_key</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for trixcrypt.c */

#define TRIXCRYPT_AUTO_REGISTER \
	scAddExtSymInt ( trixcrypt_alloc_priv );\
	scAddExtSymInt ( trixcrypt_copy_cryptkey );\
	scAddExtSymInt ( trixcrypt_set_decryptkey );\
	scAddExtSymInt ( trixcrypt_set_encryptkey );\
	scAddExtSymInt ( trixcrypt_set_cryptkeys );\


/* seer function declaration for trixcrypt.c */
#ifndef HEADER_SKIP_DECLARATION
void trixcrypt_alloc_priv ();
void trixcrypt_copy_cryptkey ();
void trixcrypt_set_decryptkey ();
void trixcrypt_set_encryptkey ();
void trixcrypt_set_cryptkeys ();

#endif
/* additional seer includes from trixcrypt_seer.h */

#define TRIXCRYPT_AUTO_MISC_PRE \


#define TRIXCRYPT_AUTO_MISC_POST \


#define TRIXCRYPT_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


#define TRIXCRYPT_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


