

/* seer headers for fmt.c */

#define FMT_AUTO_IMPORTS \
	"import t_fmt_handler *__fmt_get_handler ( char *name);\n"\
	"import unsigned int __fmt_decode ( t_stage * s, char *type);\n"\
	"import unsigned int __fmt_encode ( t_stage * s, t_stage * d);\n"\
	"import unsigned int __fmt_encode_diff ( t_stage * s1, t_stage * s2, t_stage * d);\n"\
	"import unsigned int __fmt_free_priv ( t_stage * s);\n"\
	"import unsigned int __fmt_unregister_handler ( char *name);\n"\
	"import unsigned int __fmt_register_handler ( t_fmt_handler * handler);\n"\
	"import unsigned int __fmt_add_seer_entry ( char *n, void *f, char **i, int p);\n"\
	"import unsigned int __fmt_add_seer ( t_fmt_handler * h, char **i);\n"\
	"import unsigned int __fmt_update ( );\n"\
	"import unsigned int __fmt_init ( );\n"\



#define FMT_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_fmt_handler</font>&nbsp;*<font color=\"#000000\">__fmt_get_handler</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_decode</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">type</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_encode</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">d</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_encode_diff</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s1</font>,&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s2</font>,&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">d</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_free_priv</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_unregister_handler</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_register_handler</font>&nbsp;(&nbsp;<font color=\"#000000\">t_fmt_handler</font>&nbsp;*&nbsp;<font color=\"#000000\">handler</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_add_seer_entry</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">n</font>,&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">f</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">i</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">p</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_add_seer</font>&nbsp;(&nbsp;<font color=\"#000000\">t_fmt_handler</font>&nbsp;*&nbsp;<font color=\"#000000\">h</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">i</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_update</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__fmt_init</font>&nbsp;(&nbsp;);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for fmt.c */

#define FMT_AUTO_REGISTER \
	scAddExtSymInt ( fmt_get_handler );\
	scAddExtSymInt ( fmt_decode );\
	scAddExtSymInt ( fmt_encode );\
	scAddExtSymInt ( fmt_encode_diff );\
	scAddExtSymInt ( fmt_free_priv );\
	scAddExtSymInt ( fmt_unregister_handler );\
	scAddExtSymInt ( fmt_register_handler );\
	scAddExtSymInt ( fmt_add_seer_entry );\
	scAddExtSymInt ( fmt_add_seer );\
	scAddExtSymInt ( fmt_update );\
	scAddExtSymInt ( fmt_init );\


/* seer function declaration for fmt.c */
#ifndef HEADER_SKIP_DECLARATION
void fmt_get_handler ();
void fmt_decode ();
void fmt_encode ();
void fmt_encode_diff ();
void fmt_free_priv ();
void fmt_unregister_handler ();
void fmt_register_handler ();
void fmt_add_seer_entry ();
void fmt_add_seer ();
void fmt_update ();
void fmt_init ();

#endif
/* additional seer includes from fmt_seer.h */

#define FMT_AUTO_MISC_PRE \
	"typedef char t_fmt_handler;\n"\


#define FMT_AUTO_MISC_POST \


#define FMT_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;<font color=\"#000000\">t_fmt_handler</font>;<br>"\
	"		</font>"\


#define FMT_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


