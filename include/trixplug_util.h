
#ifndef __TRIX_TRIXPLUG_UTIL_H__
#define __TRIX_TRIXPLUG_UTIL_H__

/* trixplug struct */

struct util_funcs
{
	char *(*block_get_offset_ptr) ( const object * o, const char *name, int occurence);
	char *(*block_get_segment_name_ptr) ( object * o, int occurence);
	char *(*block_get_val_ptr) ( const t_function2 * func, const char *name, int occurence);
	char *(*get_line) ( char *text, unsigned int line);
	char *(*hexpack) ( unsigned char *data, int bytes);
	char *(*hexunpack) ( char *data, int *length);
	char *(*split) ( char *string, unsigned char seperator, int which);
	int (*debug_msg) ( int level, char *str, ...);
	int (*error_msg) ( int level, char *str, ...);
	int (*printf_) ( char *str, ...);
	t_list *(*list_add_new) ( t_list * l1, int size, char *name);
	t_list *(*list_get_last) ( t_list * l);
	t_segment *(*v_get_segment) ( t_workspace * ws, unsigned int address);
	unsigned char *(*block_get_data_pointer) ( const t_function * func, const char *name);
	unsigned char *(*hex_get_buffer) ( char *data, int *len);
	unsigned char *(*v_get_ptr) ( t_workspace * ws, unsigned int address);
	unsigned char *(*v_get_str) ( t_workspace * ws, unsigned int address);
	unsigned char *(*v_get_str_raw) ( t_stage * s, unsigned int address);
	unsigned int (*block_get_number) ( const t_function2 * func, const char *name, int occurence);
	unsigned int (*block_get_segment_count) ( object * o);
	unsigned int (*block_get_size) ( t_function * func, const char *name);
	unsigned int (*check_pattern) ( t_workspace * ws, unsigned int offset, t_locator * loc);
	unsigned int (*check_pattern_raw) ( t_stage * s, unsigned int offset, t_locator * loc);
	unsigned int (*find_pattern) ( t_workspace * ws, t_locator * loc, unsigned int start, unsigned int end, int options);
	unsigned int (*free_object) ( object * o);
	unsigned int (*hex_get) ( char *data, int *err);
	unsigned int (*init) ( );
	unsigned int (*is_type) ( t_priv * data, char *type);
	unsigned int (*list_add) ( t_list * l1, t_list * l2);
	unsigned int (*list_count) ( t_list * l);
	unsigned int (*list_free) ( t_list * l);
	unsigned int (*quick_find_pattern) ( t_workspace * ws, char *pattern, char *mask, unsigned int size, unsigned int start, unsigned int end, int options);
	unsigned int (*simple_find_pattern) ( t_workspace * ws, char *pattern, char *mask, unsigned int size, int options);
	unsigned int (*str2int) ( const char *str);
	unsigned int (*swap_half) ( unsigned int v);
	unsigned int (*swap_word) ( unsigned int v);
	unsigned int (*v_arch_h) ( t_stage * s, int data);
	unsigned int (*v_arch_w) ( t_stage * s, int data);
	unsigned int (*v_cache_flush) ( );
	unsigned int (*v_cache_update) ( t_workspace * ws, t_memmap * mm);
	unsigned int (*v_get_areas) ( t_workspace * ws);
	unsigned int (*v_get_b) ( t_workspace * ws, unsigned int address);
	unsigned int (*v_get_b_raw) ( t_stage * s, unsigned int address);
	unsigned int (*v_get_end) ( t_workspace * ws);
	unsigned int (*v_get_h) ( t_workspace * ws, unsigned int address);
	unsigned int (*v_get_h_raw) ( t_stage * s, unsigned int address);
	unsigned int (*v_get_next_offset) ( t_workspace * ws, unsigned int offset);
	unsigned int (*v_get_prev_offset) ( t_workspace * ws, unsigned int offset);
	unsigned int (*v_get_size) ( t_workspace * ws);
	unsigned int (*v_get_start) ( t_workspace * ws);
	unsigned int (*v_get_strlen) ( t_workspace * ws, unsigned int address);
	unsigned int (*v_get_strlen_raw) ( t_stage * s, unsigned int address);
	unsigned int (*v_get_w) ( t_workspace * ws, unsigned int address);
	unsigned int (*v_get_w_raw) ( t_stage * s, unsigned int address);
	unsigned int (*v_memcpy) ( t_workspace * ws, unsigned int dest, unsigned int src, unsigned int length);
	unsigned int (*v_memcpy_get) ( t_workspace * ws, unsigned char *dest, unsigned int src, unsigned int length);
	unsigned int (*v_memcpy_get_raw) ( t_priv * s, unsigned char *dest, unsigned int src, unsigned int length);
	unsigned int (*v_memcpy_put) ( t_workspace * ws, unsigned int dest, unsigned char *src, unsigned int length);
	unsigned int (*v_set_b) ( t_workspace * ws, unsigned int address, unsigned int value);
	unsigned int (*v_set_h) ( t_workspace * ws, unsigned int address, unsigned int value);
	unsigned int (*v_set_w) ( t_workspace * ws, unsigned int address, unsigned int value);
	unsigned int (*v_valid) ( t_workspace * ws, unsigned int address);
	unsigned int (*v_valid_raw) ( t_priv * s, unsigned int address);
	void (*cache_disable) ( );
	void (*cache_enable) ( );
	void (*dump_bytes) ( unsigned char *data, int width, int bytes);
	void (*dump_object) ( object * o);
	void (*set_debuglevel) ( int lvl);
	void (*set_errorlevel) ( int lvl);
	void *(*smalloc) ( int size, char *name);
};

/* trixplug struct initializer */

#define UTIL_PLUG_INIT \
extern struct trix_functions *ft;\
struct util_funcs util_functions;\
unsigned int util_plug_init ( ) \
{\
	if ( !ft )\
		return E_FAIL;\
	ft->util = &util_functions;\
	ft->util->init = util_init;\
	ft->util->set_debuglevel = util_set_debuglevel;\
	ft->util->set_errorlevel = util_set_errorlevel;\
	ft->util->get_line = util_get_line;\
	ft->util->smalloc = util_smalloc;\
	ft->util->dump_bytes = util_dump_bytes;\
	ft->util->v_cache_update = v_cache_update;\
	ft->util->v_cache_flush = v_cache_flush;\
	ft->util->v_get_size = v_get_size;\
	ft->util->v_get_segment = v_get_segment;\
	ft->util->v_get_areas = v_get_areas;\
	ft->util->v_get_start = v_get_start;\
	ft->util->v_get_end = v_get_end;\
	ft->util->v_set_b = v_set_b;\
	ft->util->v_set_h = v_set_h;\
	ft->util->v_set_w = v_set_w;\
	ft->util->v_get_b = v_get_b;\
	ft->util->v_get_h = v_get_h;\
	ft->util->v_get_w = v_get_w;\
	ft->util->v_get_b_raw = v_get_b_raw;\
	ft->util->v_get_h_raw = v_get_h_raw;\
	ft->util->v_get_w_raw = v_get_w_raw;\
	ft->util->v_get_strlen = v_get_strlen;\
	ft->util->v_get_str = v_get_str;\
	ft->util->v_get_strlen_raw = v_get_strlen_raw;\
	ft->util->v_get_str_raw = v_get_str_raw;\
	ft->util->v_get_ptr = v_get_ptr;\
	ft->util->v_valid = v_valid;\
	ft->util->v_get_next_offset = v_get_next_offset;\
	ft->util->v_get_prev_offset = v_get_prev_offset;\
	ft->util->v_arch_h = v_arch_h;\
	ft->util->v_arch_w = v_arch_w;\
	ft->util->v_valid_raw = v_valid_raw;\
	ft->util->v_memcpy_get_raw = v_memcpy_get_raw;\
	ft->util->v_memcpy = v_memcpy;\
	ft->util->v_memcpy_get = v_memcpy_get;\
	ft->util->v_memcpy_put = v_memcpy_put;\
	ft->util->swap_half = util_swap_half;\
	ft->util->swap_word = util_swap_word;\
	ft->util->is_type = is_type;\
	ft->util->hex_get = util_hex_get;\
	ft->util->hex_get_buffer = util_hex_get_buffer;\
	ft->util->hexunpack = util_hexunpack;\
	ft->util->hexpack = util_hexpack;\
	ft->util->debug_msg = util_debug_msg;\
	ft->util->error_msg = util_error_msg;\
	ft->util->printf_ = util_printf_;\
	ft->util->list_get_last = util_list_get_last;\
	ft->util->list_add = util_list_add;\
	ft->util->list_add_new = util_list_add_new;\
	ft->util->list_free = util_list_free;\
	ft->util->list_count = util_list_count;\
	ft->util->check_pattern = util_check_pattern;\
	ft->util->check_pattern_raw = util_check_pattern_raw;\
	ft->util->quick_find_pattern = util_quick_find_pattern;\
	ft->util->simple_find_pattern = util_simple_find_pattern;\
	ft->util->find_pattern = util_find_pattern;\
	ft->util->block_get_size = util_block_get_size;\
	ft->util->block_get_segment_count = util_block_get_segment_count;\
	ft->util->block_get_segment_name_ptr = util_block_get_segment_name_ptr;\
	ft->util->str2int = util_str2int;\
	ft->util->block_get_number = util_block_get_number;\
	ft->util->block_get_val_ptr = util_block_get_val_ptr;\
	ft->util->block_get_offset_ptr = util_block_get_offset_ptr;\
	ft->util->block_get_data_pointer = util_block_get_data_pointer;\
	ft->util->free_object = util_free_object;\
	ft->util->dump_object = util_dump_object;\
	ft->util->split = util_split;\
	ft->util->cache_enable = util_cache_enable;\
	ft->util->cache_disable = util_cache_disable;\
\
	return E_OK;\
}

#endif
