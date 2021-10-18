

/* seer headers for mutex.c */

#define MUTEX_AUTO_IMPORTS \
	"import t_mutex *__mutex_create ( );\n"\
	"import void __mutex_release ( t_mutex * mutex);\n"\
	"import void __mutex_lock ( t_mutex * mutex);\n"\
	"import unsigned int __mutex_locked ( t_mutex * mutex);\n"\
	"import void __mutex_unlock ( t_mutex * mutex);\n"\



#define MUTEX_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_mutex</font>&nbsp;*<font color=\"#000000\">__mutex_create</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__mutex_release</font>&nbsp;(&nbsp;<font color=\"#000000\">t_mutex</font>&nbsp;*&nbsp;<font color=\"#000000\">mutex</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__mutex_lock</font>&nbsp;(&nbsp;<font color=\"#000000\">t_mutex</font>&nbsp;*&nbsp;<font color=\"#000000\">mutex</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mutex_locked</font>&nbsp;(&nbsp;<font color=\"#000000\">t_mutex</font>&nbsp;*&nbsp;<font color=\"#000000\">mutex</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__mutex_unlock</font>&nbsp;(&nbsp;<font color=\"#000000\">t_mutex</font>&nbsp;*&nbsp;<font color=\"#000000\">mutex</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for mutex.c */

#define MUTEX_AUTO_REGISTER \
	scAddExtSymInt ( mutex_create );\
	scAddExtSymInt ( mutex_release );\
	scAddExtSymInt ( mutex_lock );\
	scAddExtSymInt ( mutex_locked );\
	scAddExtSymInt ( mutex_unlock );\


/* seer function declaration for mutex.c */
#ifndef HEADER_SKIP_DECLARATION
void mutex_create ();
void mutex_release ();
void mutex_lock ();
void mutex_locked ();
void mutex_unlock ();

#endif
/* additional seer includes from mutex_seer.h */

#define MUTEX_AUTO_MISC_PRE \
	"\n"\
	"typedef int t_mutex;\n"\
	"\n"\


#define MUTEX_AUTO_MISC_POST \


#define MUTEX_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">t_mutex</font>;<br>"\
	"<br>"\
	"		</font>"\


#define MUTEX_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


