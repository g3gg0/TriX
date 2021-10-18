

/* seer headers for mem.c */

#define MEM_AUTO_IMPORTS \
	"import unsigned int __mem_init ( );\n"\
	"import unsigned int __mem_get_hash ( unsigned int hash);\n"\
	"import void *__mem_allocate ( unsigned int bytes, const char *func, int line);\n"\
	"import t_mem_list *__mem_get_entry ( void *ptr);\n"\
	"import unsigned int __mem_delete_old_entry ( unsigned int address);\n"\
	"import void *__mem_allocate_type ( unsigned int bytes, const char *func, int line, char *type);\n"\
	"import void *__mem_strduplicate ( char *text, const char *func, int line);\n"\
	"import void *__mem_calloc ( unsigned int count, unsigned int size, const char *func, int line);\n"\
	"import unsigned int __mem_check ( t_mem_list * entry, unsigned int final);\n"\
	"import unsigned int __mem_check_overwrite ( void *ptr);\n"\
	"import unsigned int __mem_check_overwrites ( );\n"\
	"import unsigned int __mem_check_all ( );\n"\
	"import unsigned int __mem_set_state_mark ( unsigned int mark, unsigned char *prefix);\n"\
	"import unsigned int __mem_get_state_mark ( unsigned int *mark, unsigned char *prefix);\n"\
	"import unsigned int __mem_release_tagged ( );\n"\
	"import void *__mem_reallocate ( void *ptr, unsigned int bytes, const char *func, int line);\n"\
	"import void __mem_release ( void *ptr, const char *func, int line);\n"\
	"import unsigned int __mem_is_valid ( void *ptr);\n"\
	"import void *__mem_reallocate_seer ( void *ptr, unsigned int bytes);\n"\
	"import void *__mem_allocate_seer ( unsigned int bytes);\n"\
	"import void *__mem_calloc_seer ( unsigned int count, unsigned int size);\n"\
	"import void *__mem_strduplicate_seer ( char *text);\n"\
	"import void __mem_release_seer ( char *buf);\n"\
	"import void *__mem_allocate_passthru ( unsigned int bytes);\n"\
	"import void __mem_release_passthru ( char *buf);\n"\



#define MEM_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_init</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_get_hash</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">hash</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_allocate</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">line</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_mem_list</font>&nbsp;*<font color=\"#000000\">__mem_get_entry</font>&nbsp;(&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">ptr</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_delete_old_entry</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_allocate_type</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">line</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">type</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_strduplicate</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">line</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_calloc</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">count</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">line</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_check</font>&nbsp;(&nbsp;<font color=\"#000000\">t_mem_list</font>&nbsp;*&nbsp;<font color=\"#000000\">entry</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">final</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_check_overwrite</font>&nbsp;(&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">ptr</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_check_overwrites</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_check_all</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_set_state_mark</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">mark</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">prefix</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_get_state_mark</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">mark</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">prefix</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_release_tagged</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_reallocate</font>&nbsp;(&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">ptr</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">line</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__mem_release</font>&nbsp;(&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">ptr</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">line</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__mem_is_valid</font>&nbsp;(&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">ptr</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_reallocate_seer</font>&nbsp;(&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">ptr</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_allocate_seer</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_calloc_seer</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">count</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_strduplicate_seer</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__mem_release_seer</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">buf</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__mem_allocate_passthru</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__mem_release_passthru</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">buf</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for mem.c */

#define MEM_AUTO_REGISTER \
	scAddExtSymInt ( mem_init );\
	scAddExtSymInt ( mem_get_hash );\
	scAddExtSymInt ( mem_allocate );\
	scAddExtSymInt ( mem_get_entry );\
	scAddExtSymInt ( mem_delete_old_entry );\
	scAddExtSymInt ( mem_allocate_type );\
	scAddExtSymInt ( mem_strduplicate );\
	scAddExtSymInt ( mem_calloc );\
	scAddExtSymInt ( mem_check );\
	scAddExtSymInt ( mem_check_overwrite );\
	scAddExtSymInt ( mem_check_overwrites );\
	scAddExtSymInt ( mem_check_all );\
	scAddExtSymInt ( mem_set_state_mark );\
	scAddExtSymInt ( mem_get_state_mark );\
	scAddExtSymInt ( mem_release_tagged );\
	scAddExtSymInt ( mem_reallocate );\
	scAddExtSymInt ( mem_release );\
	scAddExtSymInt ( mem_is_valid );\
	scAddExtSymInt ( mem_reallocate_seer );\
	scAddExtSymInt ( mem_allocate_seer );\
	scAddExtSymInt ( mem_calloc_seer );\
	scAddExtSymInt ( mem_strduplicate_seer );\
	scAddExtSymInt ( mem_release_seer );\
	scAddExtSymInt ( mem_allocate_passthru );\
	scAddExtSymInt ( mem_release_passthru );\


/* seer function declaration for mem.c */
#ifndef HEADER_SKIP_DECLARATION
void mem_init ();
void mem_get_hash ();
void mem_allocate ();
void mem_get_entry ();
void mem_delete_old_entry ();
void mem_allocate_type ();
void mem_strduplicate ();
void mem_calloc ();
void mem_check ();
void mem_check_overwrite ();
void mem_check_overwrites ();
void mem_check_all ();
void mem_set_state_mark ();
void mem_get_state_mark ();
void mem_release_tagged ();
void mem_reallocate ();
void mem_release ();
void mem_is_valid ();
void mem_reallocate_seer ();
void mem_allocate_seer ();
void mem_calloc_seer ();
void mem_strduplicate_seer ();
void mem_release_seer ();
void mem_allocate_passthru ();
void mem_release_passthru ();

#endif
/* additional seer includes from mem_seer.h */

#define MEM_AUTO_MISC_PRE \
	"\n"\
	"\n"\
	"typedef struct s_mem_list\n"\
	"{\n"\
	"	STRUCT_HEADER_LIST(t_mem_list);\n"\
	"	unsigned int bytes;\n"\
	"	unsigned int address;\n"\
	"	char *caller;\n"\
	"	char *type;\n"\
	"	int line;\n"\
	"	int freed;\n"\
	"} t_mem_list;\n"\
	"\n"\


#define MEM_AUTO_MISC_POST \


#define MEM_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_mem_list</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_mem_list</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">caller</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">type</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">line</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">freed</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_mem_list</font>;<br>"\
	"<br>"\
	"		</font>"\


#define MEM_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


