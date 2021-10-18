

/* seer headers for nokia.c */

#define NOKIA_AUTO_IMPORTS \
	"import unsigned int __nokia_init ( );\n"\
	"import unsigned int __nokia_reserve_ram ( unsigned int size);\n"\
	"import unsigned int __nokia_release_ram ( unsigned int id);\n"\
	"import unsigned int __nokia_reserve_eeprom ( unsigned int size);\n"\
	"import unsigned int __nokia_release_eeprom ( unsigned int id);\n"\
	"import unsigned int __nokia_is_section_name_relative ( unsigned char *name);\n"\
	"import unsigned int __nokia_get_section_dest ( t_workspace * ws, unsigned char *name);\n"\
	"import unsigned int __nokia_export_segments ( t_workspace * ws, object * func);\n"\
	"import unsigned int __nokia_fill_section_data ( t_workspace * ws, object * func, unsigned int offset, unsigned int base);\n"\
	"import unsigned int __nokia_create_func2 ( t_workspace * ws, object * func, unsigned int offset, unsigned int base);\n"\
	"import unsigned int __nokia_create_func ( t_workspace * ws, t_function * func, unsigned int offset);\n"\



#define NOKIA_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_init</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_reserve_ram</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_release_ram</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_reserve_eeprom</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_release_eeprom</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_is_section_name_relative</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_get_section_dest</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_export_segments</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">object</font>&nbsp;*&nbsp;<font color=\"#000000\">func</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_fill_section_data</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">object</font>&nbsp;*&nbsp;<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">base</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_create_func2</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">object</font>&nbsp;*&nbsp;<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">base</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__nokia_create_func</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_function</font>&nbsp;*&nbsp;<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for nokia.c */

#define NOKIA_AUTO_REGISTER \
	scAddExtSymInt ( nokia_init );\
	scAddExtSymInt ( nokia_reserve_ram );\
	scAddExtSymInt ( nokia_release_ram );\
	scAddExtSymInt ( nokia_reserve_eeprom );\
	scAddExtSymInt ( nokia_release_eeprom );\
	scAddExtSymInt ( nokia_is_section_name_relative );\
	scAddExtSymInt ( nokia_get_section_dest );\
	scAddExtSymInt ( nokia_export_segments );\
	scAddExtSymInt ( nokia_fill_section_data );\
	scAddExtSymInt ( nokia_create_func2 );\
	scAddExtSymInt ( nokia_create_func );\


/* seer function declaration for nokia.c */
#ifndef HEADER_SKIP_DECLARATION
void nokia_init ();
void nokia_reserve_ram ();
void nokia_release_ram ();
void nokia_reserve_eeprom ();
void nokia_release_eeprom ();
void nokia_is_section_name_relative ();
void nokia_get_section_dest ();
void nokia_export_segments ();
void nokia_fill_section_data ();
void nokia_create_func2 ();
void nokia_create_func ();

#endif
/* additional seer includes from nokia_seer.h */

#define NOKIA_AUTO_MISC_PRE \
	"\n"\
	"\n"\
	"\n"\


#define NOKIA_AUTO_MISC_POST \
	"unsigned int NokiaCreateFunc ( t_function2 *func, unsigned int offset ) { return __nokia_create_func2 ( GetWorkspace(), func, offset, 0 ); }\n"\
	"unsigned int NokiaCreateFuncRebased ( t_function2 *func, unsigned int offset, unsigned int base ) { return __nokia_create_func2 ( GetWorkspace(), func, offset, base ); }\n"\
	"\n"\


#define NOKIA_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define NOKIA_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">NokiaCreateFunc</font>&nbsp;(&nbsp;<font color=\"#000000\">t_function2</font>&nbsp;*<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__nokia_create_func2</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">func</font>,&nbsp;<font color=\"#000000\">offset</font>,&nbsp;<font color=\"#0000FF\">0</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">NokiaCreateFuncRebased</font>&nbsp;(&nbsp;<font color=\"#000000\">t_function2</font>&nbsp;*<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">base</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__nokia_create_func2</font>&nbsp;(&nbsp;<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">func</font>,&nbsp;<font color=\"#000000\">offset</font>,&nbsp;<font color=\"#000000\">base</font>&nbsp;);&nbsp;}<br>"\
	"<br>"\
	"		</font>"\


