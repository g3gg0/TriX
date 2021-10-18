

/* seer headers for seer.c */

#define SEER_AUTO_IMPORTS \
	"import void __MyDispatcher ( int *result, void *function, int *params, int paramcount, unsigned int options);\n"\
	"import char *__str ( char *str);\n"\
	"import unsigned int __seer_init ( );\n"\
	"import unsigned int __seer_execute ( scScript sc);\n"\
	"import int __seer_progresscallback ( int percent);\n"\
	"import unsigned int __seer_handle_include ( char **script);\n"\
	"import unsigned int __seer_run ( char *script);\n"\
	"import unsigned int __seer_add_header ( unsigned char *section, unsigned char *header);\n"\
	"import unsigned int __seer_add_symbol_int ( char *name, void *func);\n"\
	"import unsigned int __seer_del_header ( unsigned char *section);\n"\
	"import unsigned int __seer_del_symbol_int ( char *name);\n"\



#define SEER_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__MyDispatcher</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">result</font>,&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">function</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">params</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">paramcount</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__str</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">str</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__seer_init</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__seer_execute</font>&nbsp;(&nbsp;<font color=\"#000000\">scScript</font>&nbsp;<font color=\"#000000\">sc</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__seer_progresscallback</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">percent</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__seer_handle_include</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">script</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__seer_run</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">script</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__seer_add_header</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">section</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">header</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__seer_add_symbol_int</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>,&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">func</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__seer_del_header</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">section</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__seer_del_symbol_int</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for seer.c */

#define SEER_AUTO_REGISTER \
	scAddExtSymInt ( MyDispatcher );\
	scAddExtSymInt ( str );\
	scAddExtSymInt ( seer_init );\
	scAddExtSymInt ( seer_execute );\
	scAddExtSymInt ( seer_progresscallback );\
	scAddExtSymInt ( seer_handle_include );\
	scAddExtSymInt ( seer_run );\
	scAddExtSymInt ( seer_add_header );\
	scAddExtSymInt ( seer_add_symbol_int );\
	scAddExtSymInt ( seer_del_header );\
	scAddExtSymInt ( seer_del_symbol_int );\


/* seer function declaration for seer.c */
#ifndef HEADER_SKIP_DECLARATION
void MyDispatcher ();
void str ();
void seer_init ();
void seer_execute ();
void seer_progresscallback ();
void seer_handle_include ();
void seer_run ();
void seer_add_header ();
void seer_add_symbol_int ();
void seer_del_header ();
void seer_del_symbol_int ();

#endif
/* additional seer includes from seer_seer.h */

#define SEER_AUTO_MISC_PRE \
	"\n"\
	"typedef char *scScript;\n"\

#define SEER_AUTO_MISC_POST \


#define SEER_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">scScript</font>;"\
	"		</font>"\


#define SEER_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


