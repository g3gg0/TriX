

/* seer headers for symbols.c */

#define SYMBOLS_AUTO_IMPORTS \
	"import void __symbols_dump ( t_stage * stage);\n"\
	"import unsigned int __symbols_add ( t_stage * stage, const char *name, const int type, const char *data);\n"\
	"import unsigned int __symbols_remove ( t_stage * stage, const char *name);\n"\
	"import unsigned int __symbols_free_all ( t_stage * stage);\n"\
	"import t_symbols *__symbols_get ( t_stage * stage, const char *name);\n"\
	"import unsigned int __symbols_get_address ( t_stage * stage, const char *name);\n"\
	"import unsigned int __symbols_count ( t_stage * stage, const int type);\n"\



#define SYMBOLS_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__symbols_dump</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">stage</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__symbols_add</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">stage</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__symbols_remove</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">stage</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__symbols_free_all</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">stage</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_symbols</font>&nbsp;*<font color=\"#000000\">__symbols_get</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">stage</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__symbols_get_address</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">stage</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__symbols_count</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">stage</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for symbols.c */

#define SYMBOLS_AUTO_REGISTER \
	scAddExtSymInt ( symbols_dump );\
	scAddExtSymInt ( symbols_add );\
	scAddExtSymInt ( symbols_remove );\
	scAddExtSymInt ( symbols_free_all );\
	scAddExtSymInt ( symbols_get );\
	scAddExtSymInt ( symbols_get_address );\
	scAddExtSymInt ( symbols_count );\


/* seer function declaration for symbols.c */
#ifndef HEADER_SKIP_DECLARATION
void symbols_dump ();
void symbols_add ();
void symbols_remove ();
void symbols_free_all ();
void symbols_get ();
void symbols_get_address ();
void symbols_count ();

#endif
/* additional seer includes from symbols_seer.h */

#define SYMBOLS_AUTO_MISC_PRE \
	"\n"\
	"#define SYM_PTR 1\n"\
	"#define SYM_CALL 2\n"\
	"\n"\
	"typedef struct s_symbols\n"\
	"{\n"\
	"	STRUCT_HEADER_LIST(t_symbols);\n"\
	"    char *name;\n"\
	"    int type;\n"\
	"    char *data;\n"\
	"} t_symbols;\n"\
	"\n"\
	"\n"\


#define SYMBOLS_AUTO_MISC_POST \
	"\n"\
	"unsigned int SymbolAdd ( const char *name, const int type, const char *data ) { return __symbols_add ( GetWorkStage(0), name, type, data ); }\n"\
	"unsigned int SymbolGetInt ( const char *name ) { return __symbols_get_address ( GetWorkStage(0), name ); }\n"\


#define SYMBOLS_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;SYM_PTR&nbsp;1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;SYM_CALL&nbsp;2<br></font>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_symbols</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_symbols</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_symbols</font>;<br>"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define SYMBOLS_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">SymbolAdd</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__symbols_add</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkStage</font>(<font color=\"#0000FF\">0</font>),&nbsp;<font color=\"#000000\">name</font>,&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#000000\">data</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">SymbolGetInt</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__symbols_get_address</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkStage</font>(<font color=\"#0000FF\">0</font>),&nbsp;<font color=\"#000000\">name</font>&nbsp;);&nbsp;}<br>"\
	"		</font>"\


