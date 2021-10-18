

/* seer headers for project.c */

#define PROJECT_AUTO_IMPORTS \
	"import unsigned int __project_init ( );\n"\
	"import unsigned int __project_add_file ( t_project * p, char *file);\n"\
	"import t_project_files *__project_get_file ( t_project * p, unsigned int pos);\n"\
	"import unsigned int __project_del_file ( t_project * p, unsigned int pos);\n"\
	"import t_project *__project_load ( char *file);\n"\
	"import unsigned int __project_save ( t_project * p, char *file);\n"\



#define PROJECT_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__project_init</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__project_add_file</font>&nbsp;(&nbsp;<font color=\"#000000\">t_project</font>&nbsp;*&nbsp;<font color=\"#000000\">p</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">file</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_project_files</font>&nbsp;*<font color=\"#000000\">__project_get_file</font>&nbsp;(&nbsp;<font color=\"#000000\">t_project</font>&nbsp;*&nbsp;<font color=\"#000000\">p</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">pos</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__project_del_file</font>&nbsp;(&nbsp;<font color=\"#000000\">t_project</font>&nbsp;*&nbsp;<font color=\"#000000\">p</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">pos</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_project</font>&nbsp;*<font color=\"#000000\">__project_load</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">file</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__project_save</font>&nbsp;(&nbsp;<font color=\"#000000\">t_project</font>&nbsp;*&nbsp;<font color=\"#000000\">p</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">file</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for project.c */

#define PROJECT_AUTO_REGISTER \
	scAddExtSymInt ( project_init );\
	scAddExtSymInt ( project_add_file );\
	scAddExtSymInt ( project_get_file );\
	scAddExtSymInt ( project_del_file );\
	scAddExtSymInt ( project_load );\
	scAddExtSymInt ( project_save );\


/* seer function declaration for project.c */
#ifndef HEADER_SKIP_DECLARATION
void project_init ();
void project_add_file ();
void project_get_file ();
void project_del_file ();
void project_load ();
void project_save ();

#endif
/* additional seer includes from project_seer.h */

#define PROJECT_AUTO_MISC_PRE \
	"\n"\
	"typedef struct s_project_files\n"\
	"{\n"\
	"	STRUCT_HEADER_LIST(t_project_files);\n"\
	"	char *name;\n"\
	"    char *filename;\n"\
	"	char *content;\n"\
	"	char *options;\n"\
	"} t_project_files;\n"\
	"\n"\
	"typedef struct s_project\n"\
	"{\n"\
	"	char *projectname;		// a freely definable name for the project\n"\
	"    char *filename;			// the .txj project filename\n"\
	"	char *options;			// global option set\n"\
	"	int savemode;			// the file savemode (dry, partial, normal)\n"\
	"	int operationmode;		// the operation mode (simple, dct3, dct4)\n"\
	"	\n"\
	"	int num_files;				// number of loaded scripts\n"\
	"	t_project_files *files;	// loaded scripts\n"\
	"} t_project;\n"\


#define PROJECT_AUTO_MISC_POST \


#define PROJECT_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_project_files</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_project_files</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">filename</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">content</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">options</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_project_files</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_project</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">projectname</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;a&nbsp;freely&nbsp;definable&nbsp;name&nbsp;for&nbsp;the&nbsp;project<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">filename</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;the&nbsp;.txj&nbsp;project&nbsp;filename<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">options</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;global&nbsp;option&nbsp;set<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">savemode</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;the&nbsp;file&nbsp;savemode&nbsp;(dry,&nbsp;partial,&nbsp;normal)<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">operationmode</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;the&nbsp;operation&nbsp;mode&nbsp;(simple,&nbsp;dct3,&nbsp;dct4)<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">num_files</font>;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#808080\"><i>//&nbsp;number&nbsp;of&nbsp;loaded&nbsp;scripts<br></i></font>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_project_files</font>&nbsp;*<font color=\"#000000\">files</font>;&nbsp;<font color=\"#808080\"><i>//&nbsp;loaded&nbsp;scripts<br></i></font>"\
	"}&nbsp;<font color=\"#000000\">t_project</font>;<br>"\
	"		</font>"\


#define PROJECT_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


