

/* seer headers for netrix.c */

#define NETRIX_AUTO_IMPORTS \
	"import unsigned int __netrix_check ( char *url);\n"\
	"import t_fileinfo *__netrix_open ( const char *url);\n"\
	"import unsigned int __netrix_execute ( char *url);\n"\
	"import char *__netrix_get ( char *url);\n"\



#define NETRIX_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__netrix_check</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">url</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*<font color=\"#000000\">__netrix_open</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">url</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__netrix_execute</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">url</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__netrix_get</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">url</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for netrix.c */

#define NETRIX_AUTO_REGISTER \
	scAddExtSymInt ( netrix_check );\
	scAddExtSymInt ( netrix_open );\
	scAddExtSymInt ( netrix_execute );\
	scAddExtSymInt ( netrix_get );\


/* seer function declaration for netrix.c */
#ifndef HEADER_SKIP_DECLARATION
void netrix_check ();
void netrix_open ();
void netrix_execute ();
void netrix_get ();

#endif
/* additional seer includes from netrix_seer.h */

#define NETRIX_AUTO_MISC_PRE \
	"\n"\
	"\n"\


#define NETRIX_AUTO_MISC_POST \
	"\n"\
	"unsigned int NetrixExecute ( char *url ) { return __netrix_execute ( url ); }\n"\
	"char *NetrixGet ( char *url ) { return __netrix_get ( url ); }\n"\


#define NETRIX_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define NETRIX_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">NetrixExecute</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">url</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__netrix_execute</font>&nbsp;(&nbsp;<font color=\"#000000\">url</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">NetrixGet</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">url</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__netrix_get</font>&nbsp;(&nbsp;<font color=\"#000000\">url</font>&nbsp;);&nbsp;}<br>"\
	"		</font>"\


