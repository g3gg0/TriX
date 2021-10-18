

/* seer headers for file_io.c */

#define FILE_IO_AUTO_IMPORTS \
	"import t_fileinfo *__file_io_create_empty ( );\n"\
	"import t_fileinfo *__file_io_open ( const char *path, const char *filename);\n"\
	"import unsigned int __file_io_write ( const char *filename, const t_fileinfo * fi);\n"\
	"import unsigned int __file_io_release ( t_fileinfo * fi);\n"\
	"import unsigned int __file_io_release_all ( t_fileinfo * fi);\n"\



#define FILE_IO_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*<font color=\"#000000\">__file_io_create_empty</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*<font color=\"#000000\">__file_io_open</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">path</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">filename</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_io_write</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">filename</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*&nbsp;<font color=\"#000000\">fi</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_io_release</font>&nbsp;(&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*&nbsp;<font color=\"#000000\">fi</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_io_release_all</font>&nbsp;(&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*&nbsp;<font color=\"#000000\">fi</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for file_io.c */

#define FILE_IO_AUTO_REGISTER \
	scAddExtSymInt ( file_io_create_empty );\
	scAddExtSymInt ( file_io_open );\
	scAddExtSymInt ( file_io_write );\
	scAddExtSymInt ( file_io_release );\
	scAddExtSymInt ( file_io_release_all );\


/* seer function declaration for file_io.c */
#ifndef HEADER_SKIP_DECLARATION
void file_io_create_empty ();
void file_io_open ();
void file_io_write ();
void file_io_release ();
void file_io_release_all ();

#endif
/* additional seer includes from file_io_seer.h */

#define FILE_IO_AUTO_MISC_PRE \
	"\n"\
	"typedef struct s_fileinfo {\n"\
	"	STRUCT_HEADER_LIST(t_fileinfo);\n"\
	"    char *filename;\n"\
	"    char *path;\n"\
	"    t_stage *stages;\n"\
	"    t_stage *orig;\n"\
	"    void *priv;\n"\
	"    unsigned int options;\n"\
	"} t_fileinfo;\n"\
	"\n"\


#define FILE_IO_AUTO_MISC_POST \


#define FILE_IO_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_fileinfo</font>&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_fileinfo</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">filename</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">path</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">stages</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*<font color=\"#000000\">orig</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">priv</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_fileinfo</font>;<br>"\
	"<br>"\
	"		</font>"\


#define FILE_IO_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


