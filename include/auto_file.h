

/* seer headers for file.c */

#define FILE_AUTO_IMPORTS \
	"import unsigned int __file_set_options ( unsigned int options);\n"\
	"import unsigned int __file_get_options ( );\n"\
	"import t_fileinfo *__file_open ( char *filename);\n"\
	"import unsigned int __file_flush_diff ( t_stage * s, t_stage * d);\n"\
	"import unsigned int __file_flush ( t_stage * s);\n"\
	"import unsigned int __file_sync ( t_stage * s);\n"\
	"import unsigned int __file_format ( t_fileinfo * fi, char *format);\n"\
	"import unsigned int __file_write ( char *filename, t_fileinfo * fi);\n"\
	"import unsigned int __file_release ( t_fileinfo * fi);\n"\
	"import unsigned int __file_release_all ( t_fileinfo * fi);\n"\



#define FILE_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_set_options</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_get_options</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*<font color=\"#000000\">__file_open</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">filename</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_flush_diff</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">d</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_flush</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_sync</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_format</font>&nbsp;(&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*&nbsp;<font color=\"#000000\">fi</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">format</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_write</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">filename</font>,&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*&nbsp;<font color=\"#000000\">fi</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_release</font>&nbsp;(&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*&nbsp;<font color=\"#000000\">fi</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__file_release_all</font>&nbsp;(&nbsp;<font color=\"#000000\">t_fileinfo</font>&nbsp;*&nbsp;<font color=\"#000000\">fi</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for file.c */

#define FILE_AUTO_REGISTER \
	scAddExtSymInt ( file_set_options );\
	scAddExtSymInt ( file_get_options );\
	scAddExtSymInt ( file_open );\
	scAddExtSymInt ( file_flush_diff );\
	scAddExtSymInt ( file_flush );\
	scAddExtSymInt ( file_sync );\
	scAddExtSymInt ( file_format );\
	scAddExtSymInt ( file_write );\
	scAddExtSymInt ( file_release );\
	scAddExtSymInt ( file_release_all );\


/* seer function declaration for file.c */
#ifndef HEADER_SKIP_DECLARATION
void file_set_options ();
void file_get_options ();
void file_open ();
void file_flush_diff ();
void file_flush ();
void file_sync ();
void file_format ();
void file_write ();
void file_release ();
void file_release_all ();

#endif
/* additional seer includes from file_seer.h */

#define FILE_AUTO_MISC_PRE \
	"\n"\
	"#define FILE_NORMAL    0x00\n"\
	"#define FILE_DIFF      0x01\n"\
	"#define FILE_DRYMODE   0x02\n"\
	"\n"\


#define FILE_AUTO_MISC_POST \


#define FILE_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;FILE_NORMAL&nbsp;&nbsp;&nbsp;&nbsp;0x00<br></font>"\
	"<font color=\"#008000\">#define&nbsp;FILE_DIFF&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0x01<br></font>"\
	"<font color=\"#008000\">#define&nbsp;FILE_DRYMODE&nbsp;&nbsp;&nbsp;0x02<br></font>"\
	"<br>"\
	"		</font>"\


#define FILE_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


