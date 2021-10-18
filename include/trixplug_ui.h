
#ifndef __TRIX_TRIXPLUG_UI_H__
#define __TRIX_TRIXPLUG_UI_H__

/* trixplug struct */

struct ui_funcs
{
	int (*debug_msg) ( int level, char *str, ...);
	int (*error_msg) ( int level, char *str, ...);
	int (*printf_msg) ( char *str, ...);
	int (*vprintf_func) ( const char *format, va_list args);
	unsigned int (*dlg_bool) ( char *text);
	unsigned int (*dlg_box_clear) ( unsigned int id);
	unsigned int (*dlg_box_create) ( unsigned char *title);
	unsigned int (*dlg_box_msg) ( unsigned int id, unsigned char *msg);
	unsigned int (*dlg_box_release) ( unsigned int id);
	unsigned int (*dlg_box_release_all) ( );
	unsigned int (*dlg_box_size) ( unsigned int id, unsigned int width, unsigned int height);
	unsigned int (*dlg_dumpmem) ( t_workspace * ws, int address);
	unsigned int (*dlg_dumpmem_finish) ( );
	unsigned int (*dlg_dumpmem_get_addr) ( );
	unsigned int (*dlg_dumpmem_set_addr) ( int address);
	unsigned int (*dlg_dumpmem_visible) ( );
	unsigned int (*dlg_int) ( char *text, int min, int max, int adv, int def);
	unsigned int (*dlg_load_file) ( char **filename, const char *msg, const char *ext);
	unsigned int (*dlg_menu) ( char *title, char *text[], int options);
	unsigned int (*dlg_msg) ( char *text, int type);
	unsigned int (*dlg_save_file) ( char **filename, const char *msg, const char *ext);
	unsigned int (*dlg_string) ( char *text, char **buf);
	unsigned int (*dlg_tree) ( t_treenode * tree);
	unsigned int (*register_cb) ( unsigned char *vprintf_cb, unsigned char *fgets_cb);
	unsigned int (*set_callback_state) ( unsigned int enabled);
 	unsigned int (*dlg_box_set_keypress_ptr) ( unsigned int id, int *ptr);
 	void (*set_exiting) ( );
};

/* trixplug struct initializer */

#define UI_PLUG_INIT \
extern struct trix_functions *ft;\
struct ui_funcs ui_functions;\
unsigned int ui_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->ui = &ui_functions;\
	ft->ui->set_exiting = ui_set_exiting;\
	ft->ui->register_cb = ui_register_cb;\
	ft->ui->set_callback_state = ui_set_callback_state;\
	ft->ui->dlg_msg = ui_dlg_msg;\
	ft->ui->dlg_tree = ui_dlg_tree;\
	ft->ui->dlg_dumpmem = ui_dlg_dumpmem;\
	ft->ui->dlg_dumpmem_set_addr = ui_dlg_dumpmem_set_addr;\
	ft->ui->dlg_dumpmem_get_addr = ui_dlg_dumpmem_get_addr;\
	ft->ui->dlg_dumpmem_visible = ui_dlg_dumpmem_visible;\
	ft->ui->dlg_dumpmem_finish = ui_dlg_dumpmem_finish;\
	ft->ui->dlg_menu = ui_dlg_menu;\
	ft->ui->dlg_int = ui_dlg_int;\
	ft->ui->dlg_bool = ui_dlg_bool;\
	ft->ui->vprintf_func = ui_vprintf_func;\
	ft->ui->printf_msg = ui_printf_msg;\
	ft->ui->debug_msg = ui_debug_msg;\
	ft->ui->error_msg = ui_error_msg;\
	ft->ui->dlg_string = ui_dlg_string;\
	ft->ui->dlg_load_file = ui_dlg_load_file;\
	ft->ui->dlg_save_file = ui_dlg_save_file;\
	ft->ui->dlg_box_create = ui_dlg_box_create;\
	ft->ui->dlg_box_release = ui_dlg_box_release;\
	ft->ui->dlg_box_release_all = ui_dlg_box_release_all;\
	ft->ui->dlg_box_msg = ui_dlg_box_msg;\
	ft->ui->dlg_box_set_keypress_ptr = ui_dlg_box_set_keypress_ptr;\
	ft->ui->dlg_box_clear = ui_dlg_box_clear;\
	ft->ui->dlg_box_size = ui_dlg_box_size;\
\
	return E_OK;\
}

#endif
