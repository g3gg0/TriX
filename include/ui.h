#ifndef __TRIX_UI_H__
#define __TRIX_UI_H__

#define UI_OPT_BACK      0x00000001
#define UI_OPT_QUIT      0x00000002
#define UI_OPT_NOREPAINT 0x00000004

#define UI_OPT_DLG_CHAR  0x80000000

#include "defines.h"
#include "segment.h"
#include "stage.h"
#include "symbols.h"

#include "file_io.h"
#include "file.h"
#include "treenode.h"
#include "workspace.h"
#include "treenode.h"
#include "trixplug_ui.h"

unsigned int __cdecl qt_dlg_tree ( t_treenode *tree );
unsigned int __cdecl qt_dlg_dumpmem ( t_workspace *ws, int address );
unsigned int __cdecl qt_dlg_dumpmem_set_addr ( int address );
unsigned int __cdecl qt_dlg_dumpmem_get_addr ( );
unsigned int __cdecl qt_dlg_dumpmem_finish ( );
unsigned int __cdecl qt_dlg_dumpmem_visible ( );

/* defined in defines.h
int ui_printf_msg ( char *str, ... );
int ui_debug_msg ( int level, char *str, ... );
int ui_error_msg ( int level, char *str, ... );
*/
void ui_set_exiting ( );
unsigned int ui_register_cb ( unsigned char *vprintf_cb, unsigned char *fgets_cb );
unsigned int ui_set_callback_state ( unsigned int enabled );
int ui_vprintf_func ( const char *format, va_list args );

unsigned int ui_dlg_load_file ( char **filename, const char *msg, const char *ext );
unsigned int ui_dlg_save_file ( char **filename, const char *msg, const char *ext );
unsigned int ui_dlg_msg ( char *text, int type );
unsigned int ui_dlg_int ( char *text, int min, int max, int adv, int def );
unsigned int ui_dlg_bool ( char *text );
unsigned int ui_dlg_string ( char *text, char **buf );

unsigned int ui_dlg_menu ( char *title, char *text[], int options );
unsigned int ui_dlg_tree ( t_treenode *tree );

unsigned int ui_dlg_dumpmem ( t_workspace *ws, int address );
unsigned int ui_dlg_dumpmem_set_addr ( int address );
unsigned int ui_dlg_dumpmem_get_addr ( );
unsigned int ui_dlg_dumpmem_finish ( );
unsigned int ui_dlg_dumpmem_visible ( );


unsigned int ui_dlg_box_create ( unsigned char *title );
unsigned int ui_dlg_box_release ( unsigned int id );
unsigned int ui_dlg_box_release_all (  );
unsigned int ui_dlg_box_msg ( unsigned int id, unsigned char *msg );
unsigned int ui_dlg_box_clear ( unsigned int id );
unsigned int ui_dlg_box_size ( unsigned int id, unsigned int width, unsigned int height );
unsigned int ui_dlg_box_set_keypress_ptr ( unsigned int id, int *ptr );

#endif
