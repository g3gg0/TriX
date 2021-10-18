

// INSERT_DECLARATIONS

#define UI_OPT_BACK      0x00000001
#define UI_OPT_QUIT      0x00000002
#define UI_OPT_NOREPAINT 0x00000004

unsigned int UiDlgInt ( char *text, int min, int max ) { return __ui_dlg_int ( text, min, max, 0, 0 ); }
unsigned int UiDlgIntDef ( char *text, int min, int max, int def ) { return __ui_dlg_int ( text, min, max, 1, def ); }
unsigned int UiDlgBool ( char *text ) { return __ui_dlg_bool ( text ); }
unsigned int UiDlgString ( char *text, char **buf ) { return __ui_dlg_string ( text, buf ); }
unsigned int UiDlgMsg ( char *text, int type ) { return __ui_dlg_msg ( text, type ); }
unsigned int UiDlgMenu ( char *title, char **text, int options ) { return __ui_dlg_menu ( title, text, options ); }

unsigned int UiBoxCreate ( unsigned char *title ) { return __ui_dlg_box_create ( title ); }
unsigned int UiBoxRelease ( unsigned int id ) { return __ui_dlg_box_release (id); }
unsigned int UiBoxReleaseAll () { return __ui_dlg_box_release_all (); }
unsigned int UiBoxMsg ( unsigned int id, unsigned char *msg ) { return __ui_dlg_box_msg ( id, msg ) ; }
unsigned int UiBoxClear ( unsigned int id ) { return __ui_dlg_box_clear ( id ) ; }
unsigned int UiBoxSize ( unsigned int id, unsigned int width, unsigned int height ) { return __ui_dlg_box_size ( id, width, height ); }
