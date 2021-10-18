

/* seer headers for options.c */

#define OPTIONS_AUTO_IMPORTS \
	"import char *__options_read_settings_real ( unsigned int all);\n"\
	"import char *__options_read_settings ( );\n"\
	"import char *__options_read_all_settings ( );\n"\
	"import char *__options_get_options ( );\n"\
	"import unsigned int __options_default_all ( );\n"\
	"import unsigned int __options_apply_as_default_state ( );\n"\
	"import unsigned int __options_set_default ( char *option);\n"\
	"import unsigned int __options_apply_settings ( char *settings);\n"\
	"import unsigned int __options_has_default ( char *option);\n"\
	"import char *__options_get_description ( char *option);\n"\
	"import char *__options_get_string ( char *option);\n"\
	"import unsigned int __options_get_int ( char *option);\n"\
	"import unsigned int __options_get_bool ( char *option);\n"\
	"import unsigned int __options_set_int ( char *option, unsigned int value);\n"\
	"import unsigned int __options_set_string ( char *option, char *value);\n"\
	"import unsigned int __options_set_bool ( char *option, unsigned int value);\n"\
	"import unsigned int __options_get_type ( char *option);\n"\
	"import unsigned int __options_normalize_optionname ( char *name);\n"\
	"import unsigned int __options_normalize_pluginname ( char *name);\n"\
	"import unsigned int __options_delete_option ( char *optionname);\n"\
	"import unsigned int __options_add_option ( int type, char *optionname, char *description, void *target);\n"\
	"import unsigned int __options_import_script_options ( char *script, t_options * opt);\n"\
	"import unsigned int __options_delete_script_options ( char *script, t_options * opt);\n"\
	"import unsigned int __options_add_sysvar ( int type, char *sysvar, char *description, void *defaultval);\n"\
	"import unsigned int __options_import_plugin_options ( char *plugin, t_options * opt);\n"\
	"import unsigned int __options_delete_plugin_options ( char *plugin, t_options * opt);\n"\
	"import unsigned int __options_set_notification ( void *target, void *object);\n"\
	"import void __options_notify ( );\n"\
	"import unsigned int __options_load ( unsigned char *file);\n"\
	"import unsigned int __options_init ( );\n"\



#define OPTIONS_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__options_read_settings_real</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">all</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__options_read_settings</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__options_read_all_settings</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__options_get_options</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_default_all</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_apply_as_default_state</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_set_default</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_apply_settings</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">settings</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_has_default</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__options_get_description</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__options_get_string</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_get_int</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_get_bool</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_set_int</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">value</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_set_string</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">value</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_set_bool</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">value</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_get_type</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">option</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_normalize_optionname</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_normalize_pluginname</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_delete_option</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">optionname</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_add_option</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">optionname</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">description</font>,&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">target</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_import_script_options</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">script</font>,&nbsp;<font color=\"#000000\">t_options</font>&nbsp;*&nbsp;<font color=\"#000000\">opt</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_delete_script_options</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">script</font>,&nbsp;<font color=\"#000000\">t_options</font>&nbsp;*&nbsp;<font color=\"#000000\">opt</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_add_sysvar</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">sysvar</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">description</font>,&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">defaultval</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_import_plugin_options</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">plugin</font>,&nbsp;<font color=\"#000000\">t_options</font>&nbsp;*&nbsp;<font color=\"#000000\">opt</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_delete_plugin_options</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">plugin</font>,&nbsp;<font color=\"#000000\">t_options</font>&nbsp;*&nbsp;<font color=\"#000000\">opt</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_set_notification</font>&nbsp;(&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">target</font>,&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">object</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__options_notify</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_load</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">file</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__options_init</font>&nbsp;(&nbsp;);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for options.c */

#define OPTIONS_AUTO_REGISTER \
	scAddExtSymInt ( options_read_settings_real );\
	scAddExtSymInt ( options_read_settings );\
	scAddExtSymInt ( options_read_all_settings );\
	scAddExtSymInt ( options_get_options );\
	scAddExtSymInt ( options_default_all );\
	scAddExtSymInt ( options_apply_as_default_state );\
	scAddExtSymInt ( options_set_default );\
	scAddExtSymInt ( options_apply_settings );\
	scAddExtSymInt ( options_has_default );\
	scAddExtSymInt ( options_get_description );\
	scAddExtSymInt ( options_get_string );\
	scAddExtSymInt ( options_get_int );\
	scAddExtSymInt ( options_get_bool );\
	scAddExtSymInt ( options_set_int );\
	scAddExtSymInt ( options_set_string );\
	scAddExtSymInt ( options_set_bool );\
	scAddExtSymInt ( options_get_type );\
	scAddExtSymInt ( options_normalize_optionname );\
	scAddExtSymInt ( options_normalize_pluginname );\
	scAddExtSymInt ( options_delete_option );\
	scAddExtSymInt ( options_add_option );\
	scAddExtSymInt ( options_import_script_options );\
	scAddExtSymInt ( options_delete_script_options );\
	scAddExtSymInt ( options_add_sysvar );\
	scAddExtSymInt ( options_import_plugin_options );\
	scAddExtSymInt ( options_delete_plugin_options );\
	scAddExtSymInt ( options_set_notification );\
	scAddExtSymInt ( options_notify );\
	scAddExtSymInt ( options_load );\
	scAddExtSymInt ( options_init );\


/* seer function declaration for options.c */
#ifndef HEADER_SKIP_DECLARATION
void options_read_settings_real ();
void options_read_settings ();
void options_read_all_settings ();
void options_get_options ();
void options_default_all ();
void options_apply_as_default_state ();
void options_set_default ();
void options_apply_settings ();
void options_has_default ();
void options_get_description ();
void options_get_string ();
void options_get_int ();
void options_get_bool ();
void options_set_int ();
void options_set_string ();
void options_set_bool ();
void options_get_type ();
void options_normalize_optionname ();
void options_normalize_pluginname ();
void options_delete_option ();
void options_add_option ();
void options_import_script_options ();
void options_delete_script_options ();
void options_add_sysvar ();
void options_import_plugin_options ();
void options_delete_plugin_options ();
void options_set_notification ();
void options_notify ();
void options_load ();
void options_init ();

#endif
/* additional seer includes from options_seer.h */

#define OPTIONS_AUTO_MISC_PRE \
	"\n"\
	"\n"\
	"#define OPT_BOOL   0x0001\n"\
	"#define OPT_INT    0x0002\n"\
	"#define OPT_HEX    0x0003\n"\
	"#define OPT_STR    0x0004\n"\
	"#define OPT_LAST   0x0005\n"\
	"#define OPT_INTCB  0x0006\n"\
	"#define OPT_STRCB  0x0007\n"\
	"#define OPT_END    { 0, NULL, NULL, NULL }\n"\
	"#define OPT_BEGIN  t_options script_options[] = {\n"\
	"#define OPT_FINISH OPT_END }; export script_options;\n"\
	"#define OPTION(t,d,n,g)  { t, d, n, g },\n"\
	"\n"\
	"typedef struct s_options\n"\
	"{\n"\
	"	int type;\n"\
	"	char *description;\n"\
	"	char *name;\n"\
	"	void *target;\n"\
	"} t_options;\n"\


#define OPTIONS_AUTO_MISC_POST \


#define OPTIONS_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;OPT_BOOL&nbsp;&nbsp;&nbsp;0x0001<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPT_INT&nbsp;&nbsp;&nbsp;&nbsp;0x0002<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPT_HEX&nbsp;&nbsp;&nbsp;&nbsp;0x0003<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPT_STR&nbsp;&nbsp;&nbsp;&nbsp;0x0004<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPT_LAST&nbsp;&nbsp;&nbsp;0x0005<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPT_INTCB&nbsp;&nbsp;0x0006<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPT_STRCB&nbsp;&nbsp;0x0007<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPT_END&nbsp;&nbsp;&nbsp;&nbsp;{&nbsp;0,&nbsp;NULL,&nbsp;NULL,&nbsp;NULL&nbsp;}<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPT_BEGIN&nbsp;&nbsp;t_options&nbsp;script_options[]&nbsp;=&nbsp;{<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPT_FINISH&nbsp;OPT_END&nbsp;};&nbsp;export&nbsp;script_options;<br></font>"\
	"<font color=\"#008000\">#define&nbsp;OPTION(t,d,n,g)&nbsp;&nbsp;{&nbsp;t,&nbsp;d,&nbsp;n,&nbsp;g&nbsp;},<br></font>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_options</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">description</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">target</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_options</font>;<br>"\
	"		</font>"\


#define OPTIONS_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"		</font>"\


