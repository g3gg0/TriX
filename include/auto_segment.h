

/* seer headers for segment.c */

#define SEGMENT_AUTO_IMPORTS \
	"import t_segment *__segment_duplicate ( t_segment * s);\n"\
	"import t_segment *__segment_find_by_num ( t_segment * s, unsigned int num);\n"\
	"import t_segment *__segment_find_by_end_and_name ( t_segment * s, unsigned int addr, char *name);\n"\
	"import t_segment *__segment_find_by_end ( t_segment * s, unsigned int addr);\n"\
	"import t_segment *__segment_find_by_start ( t_segment * s, unsigned int addr);\n"\
	"import t_segment *__segment_find_by_name ( t_segment * s, char *name);\n"\
	"import unsigned int __segment_count ( t_segment * s);\n"\
	"import t_segment *__segment_get_last ( t_segment * s);\n"\
	"import t_segment *__segment_add ( t_segment * s);\n"\
	"import t_segment *__segment_create ( );\n"\
	"import unsigned int __segment_dump ( t_segment * s);\n"\
	"import unsigned int __segment_release ( t_segment * s);\n"\
	"import unsigned int __segment_release_all ( t_segment * s);\n"\



#define SEGMENT_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__segment_duplicate</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__segment_find_by_num</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">num</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__segment_find_by_end_and_name</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">addr</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__segment_find_by_end</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">addr</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__segment_find_by_start</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">addr</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__segment_find_by_name</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__segment_count</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__segment_get_last</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__segment_add</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__segment_create</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__segment_dump</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__segment_release</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__segment_release_all</font>&nbsp;(&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for segment.c */

#define SEGMENT_AUTO_REGISTER \
	scAddExtSymInt ( segment_duplicate );\
	scAddExtSymInt ( segment_find_by_num );\
	scAddExtSymInt ( segment_find_by_end_and_name );\
	scAddExtSymInt ( segment_find_by_end );\
	scAddExtSymInt ( segment_find_by_start );\
	scAddExtSymInt ( segment_find_by_name );\
	scAddExtSymInt ( segment_count );\
	scAddExtSymInt ( segment_get_last );\
	scAddExtSymInt ( segment_add );\
	scAddExtSymInt ( segment_create );\
	scAddExtSymInt ( segment_dump );\
	scAddExtSymInt ( segment_release );\
	scAddExtSymInt ( segment_release_all );\


/* seer function declaration for segment.c */
#ifndef HEADER_SKIP_DECLARATION
void segment_duplicate ();
void segment_find_by_num ();
void segment_find_by_end_and_name ();
void segment_find_by_end ();
void segment_find_by_start ();
void segment_find_by_name ();
void segment_count ();
void segment_get_last ();
void segment_add ();
void segment_create ();
void segment_dump ();
void segment_release ();
void segment_release_all ();

#endif
/* additional seer includes from segment_seer.h */

#define SEGMENT_AUTO_MISC_PRE \
	"\n"\
	"typedef struct s_segment {\n"\
	"	STRUCT_HEADER_LIST(t_segment);\n"\
	"    char *name;\n"\
	"    unsigned int start;\n"\
	"    unsigned int end;\n"\
	"    unsigned int length;\n"\
	"    unsigned char *data;\n"\
	"    void *priv;\n"\
	"} t_segment;\n"\
	"\n"\


#define SEGMENT_AUTO_MISC_POST \


#define SEGMENT_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_segment</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_segment</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">start</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">end</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">priv</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_segment</font>;<br>"\
	"<br>"\
	"		</font>"\


#define SEGMENT_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


