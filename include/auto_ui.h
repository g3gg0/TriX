

/* seer headers for ui.c */

#define UI_AUTO_IMPORTS \
	"import void __ui_set_exiting ( );\n"\
	"import unsigned int __ui_register_cb ( unsigned char *vprintf_cb, unsigned char *fgets_cb);\n"\
	"import unsigned int __ui_set_callback_state ( unsigned int enabled);\n"\
	"import unsigned int __ui_dlg_msg ( char *text, int type);\n"\
	"import unsigned int __ui_dlg_tree ( t_treenode * tree);\n"\
	"import unsigned int __ui_dlg_dumpmem ( t_workspace * ws, int address);\n"\
	"import unsigned int __ui_dlg_dumpmem_set_addr ( int address);\n"\
	"import unsigned int __ui_dlg_dumpmem_get_addr ( );\n"\
	"import unsigned int __ui_dlg_dumpmem_visible ( );\n"\
	"import unsigned int __ui_dlg_dumpmem_finish ( );\n"\
	"import unsigned int __ui_dlg_menu ( char *title, char *text[], int options);\n"\
	"import unsigned int __ui_dlg_int ( char *text, int min, int max, int adv, int def);\n"\
	"import unsigned int __ui_dlg_bool ( char *text);\n"\
	"import int __ui_vprintf_func ( const char *format, va_list args);\n"\
	"import int __ui_printf_msg ( char *str, ...);\n"\
	"import int __ui_debug_msg ( int level, char *str, ...);\n"\
	"import int __ui_error_msg ( int level, char *str, ...);\n"\
	"import unsigned int __ui_dlg_string ( char *text, char **buf);\n"\
	"import unsigned int __ui_dlg_load_file ( char **filename, const char *msg, const char *ext);\n"\
	"import unsigned int __ui_dlg_save_file ( char **filename, const char *msg, const char *ext);\n"\
	"import unsigned int __ui_dlg_box_create ( unsigned char *title);\n"\
	"import unsigned int __ui_dlg_box_release ( unsigned int id);\n"\
	"import unsigned int __ui_dlg_box_release_all ( );\n"\
	"import unsigned int __ui_dlg_box_msg ( unsigned int id, unsigned char *msg);\n"\
	"import unsigned int __ui_dlg_box_set_keypress_ptr ( unsigned int id, int *ptr);\n"\
	"import unsigned int __ui_dlg_box_clear ( unsigned int id);\n"\
	"import unsigned int __ui_dlg_box_size ( unsigned int id, unsigned int width, unsigned int height);\n"\



#define UI_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__ui_set_exiting</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_register_cb</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">vprintf_cb</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">fgets_cb</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_set_callback_state</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">enabled</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_msg</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_tree</font>&nbsp;(&nbsp;<font color=\"#000000\">t_treenode</font>&nbsp;*&nbsp;<font color=\"#000000\">tree</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_dumpmem</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_dumpmem_set_addr</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_dumpmem_get_addr</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_dumpmem_visible</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_dumpmem_finish</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_menu</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">title</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>[],&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_int</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">min</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">max</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">adv</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">def</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_bool</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_vprintf_func</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">format</font>,&nbsp;<font color=\"#000000\">va_list</font>&nbsp;<font color=\"#000000\">args</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_printf_msg</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">str</font>,&nbsp;<font color=\"#800080\">...</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_debug_msg</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">level</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">str</font>,&nbsp;<font color=\"#800080\">...</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_error_msg</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">level</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">str</font>,&nbsp;<font color=\"#800080\">...</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_string</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">buf</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_load_file</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">filename</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">msg</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">ext</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_save_file</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">filename</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">msg</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">ext</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_box_create</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">title</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_box_release</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_box_release_all</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_box_msg</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">msg</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_box_set_keypress_ptr</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">ptr</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_box_clear</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__ui_dlg_box_size</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">width</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">height</font>);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for ui.c */

#define UI_AUTO_REGISTER \
	scAddExtSymInt ( ui_set_exiting );\
	scAddExtSymInt ( ui_register_cb );\
	scAddExtSymInt ( ui_set_callback_state );\
	scAddExtSymInt ( ui_dlg_msg );\
	scAddExtSymInt ( ui_dlg_tree );\
	scAddExtSymInt ( ui_dlg_dumpmem );\
	scAddExtSymInt ( ui_dlg_dumpmem_set_addr );\
	scAddExtSymInt ( ui_dlg_dumpmem_get_addr );\
	scAddExtSymInt ( ui_dlg_dumpmem_visible );\
	scAddExtSymInt ( ui_dlg_dumpmem_finish );\
	scAddExtSymInt ( ui_dlg_menu );\
	scAddExtSymInt ( ui_dlg_int );\
	scAddExtSymInt ( ui_dlg_bool );\
	scAddExtSymInt ( ui_vprintf_func );\
	scAddExtSymInt ( ui_printf_msg );\
	scAddExtSymInt ( ui_debug_msg );\
	scAddExtSymInt ( ui_error_msg );\
	scAddExtSymInt ( ui_dlg_string );\
	scAddExtSymInt ( ui_dlg_load_file );\
	scAddExtSymInt ( ui_dlg_save_file );\
	scAddExtSymInt ( ui_dlg_box_create );\
	scAddExtSymInt ( ui_dlg_box_release );\
	scAddExtSymInt ( ui_dlg_box_release_all );\
	scAddExtSymInt ( ui_dlg_box_msg );\
	scAddExtSymInt ( ui_dlg_box_set_keypress_ptr );\
	scAddExtSymInt ( ui_dlg_box_clear );\
	scAddExtSymInt ( ui_dlg_box_size );\


/* seer function declaration for ui.c */
#ifndef HEADER_SKIP_DECLARATION
void ui_set_exiting ();
void ui_register_cb ();
void ui_set_callback_state ();
void ui_dlg_msg ();
void ui_dlg_tree ();
void ui_dlg_dumpmem ();
void ui_dlg_dumpmem_set_addr ();
void ui_dlg_dumpmem_get_addr ();
void ui_dlg_dumpmem_visible ();
void ui_dlg_dumpmem_finish ();
void ui_dlg_menu ();
void ui_dlg_int ();
void ui_dlg_bool ();
void ui_vprintf_func ();
void ui_printf_msg ();
void ui_debug_msg ();
void ui_error_msg ();
void ui_dlg_string ();
void ui_dlg_load_file ();
void ui_dlg_save_file ();
void ui_dlg_box_create ();
void ui_dlg_box_release ();
void ui_dlg_box_release_all ();
void ui_dlg_box_msg ();
void ui_dlg_box_set_keypress_ptr ();
void ui_dlg_box_clear ();
void ui_dlg_box_size ();

#endif
/* additional seer includes from ui_seer.h */

#define UI_AUTO_MISC_PRE \
	"\n"\
	"typedef int va_list;\n"\
	"\n"\
	"\n"\


#define UI_AUTO_MISC_POST \
	"\n"\
	"#define UI_OPT_BACK      0x00000001\n"\
	"#define UI_OPT_QUIT      0x00000002\n"\
	"#define UI_OPT_NOREPAINT 0x00000004\n"\
	"\n"\
	"\n"\
	"unsigned int UiDlgInt ( char *text, int min, int max ) { return __ui_dlg_int ( text, min, max, 0, 0 ); }\n"\
	"unsigned int UiDlgIntDef ( char *text, int min, int max, int def ) { return __ui_dlg_int ( text, min, max, 1, def ); }\n"\
	"unsigned int UiDlgBool ( char *text ) { return __ui_dlg_bool ( text ); }\n"\
	"unsigned int UiDlgString ( char *text, char **buf ) { return __ui_dlg_string ( text, buf ); }\n"\
	"unsigned int UiDlgLoadFile ( char **filename, const char *msg, const char *ext ) { return __ui_dlg_load_file ( filename, msg, ext ); }\n"\
	"unsigned int UiDlgSaveFile ( char **filename, const char *msg, const char *ext ) { return __ui_dlg_save_file ( filename, msg, ext ); }\n"\
	"unsigned int UiDlgMsg ( char *text, int type ) { return __ui_dlg_msg ( text, type ); }\n"\
	"unsigned int UiDlgMenu ( char *title, char **text, int options ) { return __ui_dlg_menu ( title, text, options ); }\n"\
	"\n"\
	"unsigned int UiBoxCreate ( unsigned char *title ) { return __ui_dlg_box_create ( title ); }\n"\
	"unsigned int UiBoxRelease ( unsigned int id ) { return __ui_dlg_box_release (id); }\n"\
	"unsigned int UiBoxReleaseAll () { return __ui_dlg_box_release_all (); }\n"\
	"unsigned int UiBoxMsg ( unsigned int id, unsigned char *msg ) { return __ui_dlg_box_msg ( id, msg ) ; }\n"\
	"unsigned int UiBoxClear ( unsigned int id ) { return __ui_dlg_box_clear ( id ) ; }\n"\
	"unsigned int UiBoxSize ( unsigned int id, unsigned int width, unsigned int height ) { return __ui_dlg_box_size ( id, width, height ); }\n"\


#define UI_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">va_list</font>;<br>"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define UI_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;UI_OPT_BACK&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0x00000001<br></font>"\
	"<font color=\"#008000\">#define&nbsp;UI_OPT_QUIT&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0x00000002<br></font>"\
	"<font color=\"#008000\">#define&nbsp;UI_OPT_NOREPAINT&nbsp;0x00000004<br></font>"\
	"<br>"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiDlgInt</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">min</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">max</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_int</font>&nbsp;(&nbsp;<font color=\"#000000\">text</font>,&nbsp;<font color=\"#000000\">min</font>,&nbsp;<font color=\"#000000\">max</font>,&nbsp;<font color=\"#0000FF\">0</font>,&nbsp;<font color=\"#0000FF\">0</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiDlgIntDef</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">min</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">max</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">def</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_int</font>&nbsp;(&nbsp;<font color=\"#000000\">text</font>,&nbsp;<font color=\"#000000\">min</font>,&nbsp;<font color=\"#000000\">max</font>,&nbsp;<font color=\"#0000FF\">1</font>,&nbsp;<font color=\"#000000\">def</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiDlgBool</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_bool</font>&nbsp;(&nbsp;<font color=\"#000000\">text</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiDlgString</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">buf</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_string</font>&nbsp;(&nbsp;<font color=\"#000000\">text</font>,&nbsp;<font color=\"#000000\">buf</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiDlgLoadFile</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">filename</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">msg</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">ext</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_load_file</font>&nbsp;(&nbsp;<font color=\"#000000\">filename</font>,&nbsp;<font color=\"#000000\">msg</font>,&nbsp;<font color=\"#000000\">ext</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiDlgSaveFile</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">filename</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">msg</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">ext</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_save_file</font>&nbsp;(&nbsp;<font color=\"#000000\">filename</font>,&nbsp;<font color=\"#000000\">msg</font>,&nbsp;<font color=\"#000000\">ext</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiDlgMsg</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_msg</font>&nbsp;(&nbsp;<font color=\"#000000\">text</font>,&nbsp;<font color=\"#000000\">type</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiDlgMenu</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">title</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;**<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_menu</font>&nbsp;(&nbsp;<font color=\"#000000\">title</font>,&nbsp;<font color=\"#000000\">text</font>,&nbsp;<font color=\"#000000\">options</font>&nbsp;);&nbsp;}<br>"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiBoxCreate</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">title</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_box_create</font>&nbsp;(&nbsp;<font color=\"#000000\">title</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiBoxRelease</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_box_release</font>&nbsp;(<font color=\"#000000\">id</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiBoxReleaseAll</font>&nbsp;()&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_box_release_all</font>&nbsp;();&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiBoxMsg</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">msg</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_box_msg</font>&nbsp;(&nbsp;<font color=\"#000000\">id</font>,&nbsp;<font color=\"#000000\">msg</font>&nbsp;)&nbsp;;&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiBoxClear</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_box_clear</font>&nbsp;(&nbsp;<font color=\"#000000\">id</font>&nbsp;)&nbsp;;&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">UiBoxSize</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">id</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">width</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">height</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__ui_dlg_box_size</font>&nbsp;(&nbsp;<font color=\"#000000\">id</font>,&nbsp;<font color=\"#000000\">width</font>,&nbsp;<font color=\"#000000\">height</font>&nbsp;);&nbsp;}<br>"\
	"		</font>"\


