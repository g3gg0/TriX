

/* seer headers for util.c */

#define UTIL_AUTO_IMPORTS \
	"import unsigned int __util_init ( );\n"\
	"import void __util_set_debuglevel ( int lvl);\n"\
	"import void __util_set_errorlevel ( int lvl);\n"\
	"import int __util_script_is_aborted ( );\n"\
	"import char *__util_get_line ( char *text, unsigned int line);\n"\
	"import void *__util_smalloc_init ( void *ptr, int size, char *name);\n"\
	"import void *__util_smalloc ( int size, char *name);\n"\
	"import void __util_dump_bytes ( unsigned char *data, int width, int bytes);\n"\
	"import unsigned int __v_cache_update ( t_workspace * ws, t_memmap * mm);\n"\
	"import unsigned int __v_cache_flush ( );\n"\
	"import unsigned int __v_get_size ( t_workspace * ws);\n"\
	"import t_segment *__v_get_segment ( t_workspace * ws, unsigned int address);\n"\
	"import unsigned int __v_get_areas ( t_workspace * ws);\n"\
	"import unsigned int __v_get_start ( t_workspace * ws);\n"\
	"import unsigned int __v_get_end ( t_workspace * ws);\n"\
	"import unsigned int __v_set_b ( t_workspace * ws, unsigned int address, unsigned int value);\n"\
	"import unsigned int __v_set_h ( t_workspace * ws, unsigned int address, unsigned int value);\n"\
	"import unsigned int __v_set_w ( t_workspace * ws, unsigned int address, unsigned int value);\n"\
	"import unsigned int __v_set_b_raw ( t_stage * s, unsigned int address, unsigned int value);\n"\
	"import unsigned int __v_set_h_raw ( t_stage * s, unsigned int address, unsigned int value);\n"\
	"import unsigned int __v_set_w_raw ( t_stage * s, unsigned int address, unsigned int value);\n"\
	"import unsigned int __v_get_b ( t_workspace * ws, unsigned int address);\n"\
	"import unsigned int __v_get_h ( t_workspace * ws, unsigned int address);\n"\
	"import unsigned int __v_get_w ( t_workspace * ws, unsigned int address);\n"\
	"import unsigned int __v_get_b_raw ( t_stage * s, unsigned int address);\n"\
	"import unsigned int __v_get_h_raw ( t_stage * s, unsigned int address);\n"\
	"import unsigned int __v_get_w_raw ( t_stage * s, unsigned int address);\n"\
	"import unsigned int __v_get_strlen ( t_workspace * ws, unsigned int address);\n"\
	"import unsigned char *__v_get_str ( t_workspace * ws, unsigned int address);\n"\
	"import unsigned int __v_get_strlen_raw ( t_stage * s, unsigned int address);\n"\
	"import unsigned char *__v_get_str_raw ( t_stage * s, unsigned int address);\n"\
	"import unsigned char *__v_get_ptr ( t_workspace * ws, unsigned int address);\n"\
	"import unsigned int __v_valid ( t_workspace * ws, unsigned int address);\n"\
	"import unsigned int __v_get_next_offset ( t_workspace * ws, unsigned int offset);\n"\
	"import unsigned int __v_get_prev_offset ( t_workspace * ws, unsigned int offset);\n"\
	"import unsigned int __v_arch_h ( t_stage * s, int data);\n"\
	"import unsigned int __v_arch_w ( t_stage * s, int data);\n"\
	"import unsigned int __v_valid_raw ( t_priv * s, unsigned int address);\n"\
	"import unsigned int __v_memcpy_get_raw ( t_priv * s, unsigned char *dest, unsigned int src, unsigned int length);\n"\
	"import unsigned int __v_memcpy ( t_workspace * ws, unsigned int dest, unsigned int src, unsigned int length);\n"\
	"import unsigned int __v_memcpy_get ( t_workspace * ws, unsigned char *dest, unsigned int src, unsigned int length);\n"\
	"import unsigned int __v_memcpy_put ( t_workspace * ws, unsigned int dest, unsigned char *src, unsigned int length);\n"\
	"import unsigned int __util_swap_half ( unsigned int v);\n"\
	"import unsigned int __util_swap_word ( unsigned int v);\n"\
	"import unsigned int __is_type ( t_priv * data, char *type);\n"\
	"import unsigned int __util_hex_get ( char *data, int *err);\n"\
	"import unsigned char *__util_hex_get_buffer ( char *data, int *len);\n"\
	"import char *__util_hexunpack ( char *data, int *length);\n"\
	"import char *__util_hexpack ( unsigned char *data, int bytes);\n"\
	"import t_list *__util_list_get_last ( t_list * l);\n"\
	"import unsigned int __util_list_add ( t_list * l1, t_list * l2);\n"\
	"import t_list *__util_list_add_new ( t_list * l1, int size, char *name);\n"\
	"import unsigned int __util_list_free ( t_list * l);\n"\
	"import unsigned int __util_list_count ( t_list * l);\n"\
	"import unsigned int __util_check_pattern ( t_workspace * ws, unsigned int offset, t_locator * loc);\n"\
	"import unsigned int __util_check_pattern_raw ( t_stage * s, unsigned int offset, t_locator * loc);\n"\
	"import unsigned int __util_quick_find_pattern ( t_workspace * ws, char *pattern, char *mask, unsigned int size, unsigned int start, unsigned int end, int options);\n"\
	"import unsigned int __util_simple_find_pattern ( t_workspace * ws, char *pattern, char *mask, unsigned int size, int options);\n"\
	"import unsigned int __util_find_pattern ( t_workspace * ws, t_locator * loc, unsigned int start, unsigned int end, int options);\n"\
	"import unsigned int __util_block_get_size ( t_function * func, const char *name);\n"\
	"import unsigned int __util_block_get_segment_count ( object * o);\n"\
	"import char *__util_block_get_segment_name_ptr ( object * o, int occurence);\n"\
	"import unsigned int __util_str2int ( const char *str);\n"\
	"import unsigned int __util_block_get_number ( const t_function2 * func, const char *name, int occurence);\n"\
	"import char *__util_block_get_val_ptr ( const t_function2 * func, const char *name, int occurence);\n"\
	"import char *__util_block_get_offset_ptr ( const object * o, const char *name, int occurence);\n"\
	"import unsigned char *__util_block_get_data_pointer ( const t_function * func, const char *name);\n"\
	"import unsigned int __util_free_object ( object * o);\n"\
	"import void __util_dump_object ( object * o);\n"\
	"import char *__util_split ( char *string, unsigned char seperator, int which);\n"\
	"import void __util_cache_enable ( );\n"\
	"import void __util_cache_disable ( );\n"\



#define UTIL_AUTO_IMPORTS_HTML \
	"		<font face=\"Monospace\">"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_init</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__util_set_debuglevel</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">lvl</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__util_set_errorlevel</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">lvl</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_script_is_aborted</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__util_get_line</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">text</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">line</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__util_smalloc_init</font>&nbsp;(&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">ptr</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;*<font color=\"#000000\">__util_smalloc</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__util_dump_bytes</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">width</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_cache_update</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_memmap</font>&nbsp;*&nbsp;<font color=\"#000000\">mm</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_cache_flush</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_size</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_segment</font>&nbsp;*<font color=\"#000000\">__v_get_segment</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_areas</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_start</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_end</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_set_b</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">value</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_set_h</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">value</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_set_w</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">value</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_set_b_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">value</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_set_h_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">value</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_set_w_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">value</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_b</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_h</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_w</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_b_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_h_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_w_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_strlen</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__v_get_str</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_strlen_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__v_get_str_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__v_get_ptr</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_valid</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_next_offset</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_get_prev_offset</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_arch_h</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">data</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_arch_w</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">data</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_valid_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_priv</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">address</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_memcpy_get_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_priv</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">dest</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">src</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_memcpy</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">dest</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">src</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_memcpy_get</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">dest</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">src</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__v_memcpy_put</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">dest</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">src</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_swap_half</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">v</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_swap_word</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">v</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__is_type</font>&nbsp;(&nbsp;<font color=\"#000000\">t_priv</font>&nbsp;*&nbsp;<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">type</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_hex_get</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">err</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__util_hex_get_buffer</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">len</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__util_hexunpack</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;*<font color=\"#000000\">length</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__util_hexpack</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_list</font>&nbsp;*<font color=\"#000000\">__util_list_get_last</font>&nbsp;(&nbsp;<font color=\"#000000\">t_list</font>&nbsp;*&nbsp;<font color=\"#000000\">l</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_list_add</font>&nbsp;(&nbsp;<font color=\"#000000\">t_list</font>&nbsp;*&nbsp;<font color=\"#000000\">l1</font>,&nbsp;<font color=\"#000000\">t_list</font>&nbsp;*&nbsp;<font color=\"#000000\">l2</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#000000\">t_list</font>&nbsp;*<font color=\"#000000\">__util_list_add_new</font>&nbsp;(&nbsp;<font color=\"#000000\">t_list</font>&nbsp;*&nbsp;<font color=\"#000000\">l1</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_list_free</font>&nbsp;(&nbsp;<font color=\"#000000\">t_list</font>&nbsp;*&nbsp;<font color=\"#000000\">l</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_list_count</font>&nbsp;(&nbsp;<font color=\"#000000\">t_list</font>&nbsp;*&nbsp;<font color=\"#000000\">l</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_check_pattern</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>,&nbsp;<font color=\"#000000\">t_locator</font>&nbsp;*&nbsp;<font color=\"#000000\">loc</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_check_pattern_raw</font>&nbsp;(&nbsp;<font color=\"#000000\">t_stage</font>&nbsp;*&nbsp;<font color=\"#000000\">s</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>,&nbsp;<font color=\"#000000\">t_locator</font>&nbsp;*&nbsp;<font color=\"#000000\">loc</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_quick_find_pattern</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">pattern</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">mask</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_simple_find_pattern</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">pattern</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">mask</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_find_pattern</font>&nbsp;(&nbsp;<font color=\"#000000\">t_workspace</font>&nbsp;*&nbsp;<font color=\"#000000\">ws</font>,&nbsp;<font color=\"#000000\">t_locator</font>&nbsp;*&nbsp;<font color=\"#000000\">loc</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_block_get_size</font>&nbsp;(&nbsp;<font color=\"#000000\">t_function</font>&nbsp;*&nbsp;<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_block_get_segment_count</font>&nbsp;(&nbsp;<font color=\"#000000\">object</font>&nbsp;*&nbsp;<font color=\"#000000\">o</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__util_block_get_segment_name_ptr</font>&nbsp;(&nbsp;<font color=\"#000000\">object</font>&nbsp;*&nbsp;<font color=\"#000000\">o</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">occurence</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_str2int</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">str</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_block_get_number</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#000000\">t_function2</font>&nbsp;*&nbsp;<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">occurence</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__util_block_get_val_ptr</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#000000\">t_function2</font>&nbsp;*&nbsp;<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">occurence</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__util_block_get_offset_ptr</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#000000\">object</font>&nbsp;*&nbsp;<font color=\"#000000\">o</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">occurence</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__util_block_get_data_pointer</font>&nbsp;(&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#000000\">t_function</font>&nbsp;*&nbsp;<font color=\"#000000\">func</font>,&nbsp;<font color=\"#800000\">const</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">__util_free_object</font>&nbsp;(&nbsp;<font color=\"#000000\">object</font>&nbsp;*&nbsp;<font color=\"#000000\">o</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__util_dump_object</font>&nbsp;(&nbsp;<font color=\"#000000\">object</font>&nbsp;*&nbsp;<font color=\"#000000\">o</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">__util_split</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">string</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;<font color=\"#000000\">seperator</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">which</font>);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__util_cache_enable</font>&nbsp;(&nbsp;);<br>"\
	"<font color=\"#000080\"><b>import</b></font>&nbsp;<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">__util_cache_disable</font>&nbsp;(&nbsp;);<br>"\
	"<br>"\
	"		</font>"\

/* seer function registration for util.c */

#define UTIL_AUTO_REGISTER \
	scAddExtSymInt ( util_init );\
	scAddExtSymInt ( util_set_debuglevel );\
	scAddExtSymInt ( util_set_errorlevel );\
	scAddExtSymInt ( util_script_is_aborted );\
	scAddExtSymInt ( util_get_line );\
	scAddExtSymInt ( util_smalloc_init );\
	scAddExtSymInt ( util_smalloc );\
	scAddExtSymInt ( util_dump_bytes );\
	scAddExtSymInt ( v_cache_update );\
	scAddExtSymInt ( v_cache_flush );\
	scAddExtSymInt ( v_get_size );\
	scAddExtSymInt ( v_get_segment );\
	scAddExtSymInt ( v_get_areas );\
	scAddExtSymInt ( v_get_start );\
	scAddExtSymInt ( v_get_end );\
	scAddExtSymInt ( v_set_b );\
	scAddExtSymInt ( v_set_h );\
	scAddExtSymInt ( v_set_w );\
	scAddExtSymInt ( v_set_b_raw );\
	scAddExtSymInt ( v_set_h_raw );\
	scAddExtSymInt ( v_set_w_raw );\
	scAddExtSymInt ( v_get_b );\
	scAddExtSymInt ( v_get_h );\
	scAddExtSymInt ( v_get_w );\
	scAddExtSymInt ( v_get_b_raw );\
	scAddExtSymInt ( v_get_h_raw );\
	scAddExtSymInt ( v_get_w_raw );\
	scAddExtSymInt ( v_get_strlen );\
	scAddExtSymInt ( v_get_str );\
	scAddExtSymInt ( v_get_strlen_raw );\
	scAddExtSymInt ( v_get_str_raw );\
	scAddExtSymInt ( v_get_ptr );\
	scAddExtSymInt ( v_valid );\
	scAddExtSymInt ( v_get_next_offset );\
	scAddExtSymInt ( v_get_prev_offset );\
	scAddExtSymInt ( v_arch_h );\
	scAddExtSymInt ( v_arch_w );\
	scAddExtSymInt ( v_valid_raw );\
	scAddExtSymInt ( v_memcpy_get_raw );\
	scAddExtSymInt ( v_memcpy );\
	scAddExtSymInt ( v_memcpy_get );\
	scAddExtSymInt ( v_memcpy_put );\
	scAddExtSymInt ( util_swap_half );\
	scAddExtSymInt ( util_swap_word );\
	scAddExtSymInt ( is_type );\
	scAddExtSymInt ( util_hex_get );\
	scAddExtSymInt ( util_hex_get_buffer );\
	scAddExtSymInt ( util_hexunpack );\
	scAddExtSymInt ( util_hexpack );\
	scAddExtSymInt ( util_list_get_last );\
	scAddExtSymInt ( util_list_add );\
	scAddExtSymInt ( util_list_add_new );\
	scAddExtSymInt ( util_list_free );\
	scAddExtSymInt ( util_list_count );\
	scAddExtSymInt ( util_check_pattern );\
	scAddExtSymInt ( util_check_pattern_raw );\
	scAddExtSymInt ( util_quick_find_pattern );\
	scAddExtSymInt ( util_simple_find_pattern );\
	scAddExtSymInt ( util_find_pattern );\
	scAddExtSymInt ( util_block_get_size );\
	scAddExtSymInt ( util_block_get_segment_count );\
	scAddExtSymInt ( util_block_get_segment_name_ptr );\
	scAddExtSymInt ( util_str2int );\
	scAddExtSymInt ( util_block_get_number );\
	scAddExtSymInt ( util_block_get_val_ptr );\
	scAddExtSymInt ( util_block_get_offset_ptr );\
	scAddExtSymInt ( util_block_get_data_pointer );\
	scAddExtSymInt ( util_free_object );\
	scAddExtSymInt ( util_dump_object );\
	scAddExtSymInt ( util_split );\
	scAddExtSymInt ( util_cache_enable );\
	scAddExtSymInt ( util_cache_disable );\


/* seer function declaration for util.c */
#ifndef HEADER_SKIP_DECLARATION
void util_init ();
void util_set_debuglevel ();
void util_set_errorlevel ();
void util_script_is_aborted ();
void util_get_line ();
void util_smalloc_init ();
void util_smalloc ();
void util_dump_bytes ();
void v_cache_update ();
void v_cache_flush ();
void v_get_size ();
void v_get_segment ();
void v_get_areas ();
void v_get_start ();
void v_get_end ();
void v_set_b ();
void v_set_h ();
void v_set_w ();
void v_set_b_raw ();
void v_set_h_raw ();
void v_set_w_raw ();
void v_get_b ();
void v_get_h ();
void v_get_w ();
void v_get_b_raw ();
void v_get_h_raw ();
void v_get_w_raw ();
void v_get_strlen ();
void v_get_str ();
void v_get_strlen_raw ();
void v_get_str_raw ();
void v_get_ptr ();
void v_valid ();
void v_get_next_offset ();
void v_get_prev_offset ();
void v_arch_h ();
void v_arch_w ();
void v_valid_raw ();
void v_memcpy_get_raw ();
void v_memcpy ();
void v_memcpy_get ();
void v_memcpy_put ();
void util_swap_half ();
void util_swap_word ();
void is_type ();
void util_hex_get ();
void util_hex_get_buffer ();
void util_hexunpack ();
void util_hexpack ();
void util_list_get_last ();
void util_list_add ();
void util_list_add_new ();
void util_list_free ();
void util_list_count ();
void util_check_pattern ();
void util_check_pattern_raw ();
void util_quick_find_pattern ();
void util_simple_find_pattern ();
void util_find_pattern ();
void util_block_get_size ();
void util_block_get_segment_count ();
void util_block_get_segment_name_ptr ();
void util_str2int ();
void util_block_get_number ();
void util_block_get_val_ptr ();
void util_block_get_offset_ptr ();
void util_block_get_data_pointer ();
void util_free_object ();
void util_dump_object ();
void util_split ();
void util_cache_enable ();
void util_cache_disable ();

#endif
/* additional seer includes from util_seer.h */

#define UTIL_AUTO_MISC_PRE \
	"\n"\
	"#define LOC_UNIQUE    1\n"\
	"#define LOC_FORWARD   0\n"\
	"#define LOC_BACKWARD  2\n"\
	"\n"\
	"#define DEBUG_FMT       0x0001\n"\
	"#define DEBUG_SEER      0x0002\n"\
	"#define DEBUG_ARM       0x0004\n"\
	"#define DEBUG_NOKIA     0x0008\n"\
	"#define DEBUG_LIST      0x0010\n"\
	"#define DEBUG_UNKNOWN   0x0020\n"\
	"\n"\
	"\n"\
	"typedef char* t_function2;\n"\
	"typedef char* func;\n"\
	"typedef char* object;\n"\
	"\n"\
	"typedef struct s_list\n"\
	"{\n"\
	"	STRUCT_HEADER_LIST(t_list);\n"\
	"} t_list;\n"\
	"\n"\
	"typedef struct s_priv\n"\
	"{\n"\
	"	STRUCT_HEADER;\n"\
	"} t_priv;\n"\
	"\n"\
	"typedef struct s_locator\n"\
	"{\n"\
	"    unsigned char *func_name;\n"\
	"    unsigned char *pattern;\n"\
	"    unsigned char *mask;\n"\
	"    unsigned int length;\n"\
	"    unsigned int options;\n"\
	"} t_locator;\n"\
	"\n"\
	"typedef struct s_function_block\n"\
	"{\n"\
	"    char *name;\n"\
	"    unsigned char *data;\n"\
	"    unsigned int length;\n"\
	"} t_function_block;\n"\
	"\n"\
	"typedef struct s_function_relative_data\n"\
	"{\n"\
	"    unsigned int offset;\n"\
	"    int type;\n"\
	"    char *dest;\n"\
	"} t_function_relative_data;\n"\
	"\n"\
	"typedef struct s_function\n"\
	"{\n"\
	"    char *name;\n"\
	"    t_function_block *blocks;\n"\
	"    t_function_relative_data *rel_data;\n"\
	"} t_function;\n"\
	"\n"\
	"\n"\


#define UTIL_AUTO_MISC_POST \
	"\n"\
	"unsigned int FindPattern(t_locator *loc, unsigned int start, unsigned int end, int options) { return __util_find_pattern(GetWorkspace(), loc, start, end, options); }\n"\
	"unsigned int QuickFindPattern(char *pattern, char *mask, unsigned int size, unsigned int start, unsigned int end, int options) { return __util_quick_find_pattern(GetWorkspace(),pattern, mask, size, start, end, options); }\n"\
	"unsigned int SimpleFindPattern(char *pattern, char *mask, unsigned int size, unsigned int options) { return __util_simple_find_pattern(GetWorkspace(), pattern, mask, size, options); }\n"\
	"unsigned int Valid(int x) { return __v_valid (GetWorkspace(), x); }\n"\
	"unsigned int GetSize() { return __v_get_size (GetWorkspace()); }\n"\
	"void CacheDisable() { __util_cache_disable(); }\n"\
	"void CacheEnable() { __util_cache_enable(); }\n"\
	"void DumpBytes(unsigned char *data, int width, unsigned int bytes) { __util_dump_bytes(data, width, bytes); }\n"\
	"unsigned int GetByte(unsigned int x) { return __v_get_b (GetWorkspace(), x); }\n"\
	"unsigned int GetHalf(unsigned int x) { return __v_get_h (GetWorkspace(), x); }\n"\
	"unsigned int GetWord(unsigned int x) { return __v_get_w (GetWorkspace(), x); }\n"\
	"unsigned int SetByte(unsigned int x,unsigned int y) { return __v_set_b (GetWorkspace(), x, y); }\n"\
	"unsigned int SetHalf(unsigned int x,unsigned int y) { return __v_set_h (GetWorkspace(), x, y); }\n"\
	"unsigned int SetWord(unsigned int x,unsigned int y) { return __v_set_w (GetWorkspace(), x, y); }\n"\
	"unsigned int MemCpy(unsigned int x,unsigned int y,unsigned int z) { return __v_memcpy (GetWorkspace(),x,y,z); }\n"\
	"unsigned int MemCpyGet(unsigned char *x,unsigned int y,unsigned int z) { return __v_memcpy_get (GetWorkspace(),x,y,z); }\n"\
	"unsigned int MemCpyPut(unsigned int x,unsigned char *y,unsigned int z) { return __v_memcpy_put (GetWorkspace(),x,y,z); }\n"\
	"unsigned char *GetPtr(unsigned int x) { return __v_get_ptr (GetWorkspace(), x); }\n"\
	"unsigned char *GetStr(unsigned int x) { return __v_get_str (GetWorkspace(), x); }\n"\
	"unsigned int GetStrLen(unsigned int x) { return __v_get_strlen (GetWorkspace(), x); }\n"\
	"unsigned int SH(unsigned int x) { return __util_swap_half (x); }\n"\
	"unsigned int SW(unsigned int x) { return __util_swap_word (x); }\n"\
	"void SetDebugLevel ( int debuglevel ) { __util_set_debuglevel ( debuglevel ); }\n"\
	"void SetErrorLevel ( int errorlevel ) { __util_set_errorlevel ( errorlevel ); }\n"\
	"unsigned int Str2Int ( char *str ) { return __util_str2int ( str ); }\n"\
	"\n"\
	"void DrawBytes ( unsigned int data, int width, unsigned int bytes )\n"\
	"{\n"\
	"	unsigned int pos = 0;\n"\
	"	int col = 0;\n"\
	"	unsigned char byte = 0;\n"\
	"\n"\
	"	say ( \"-------- print bytes at 0x%08X -------\n\", data );\n"\
	"\n"\
	"	while ( pos < bytes )\n"\
	"	{\n"\
	"		say ( \"%08X: \", pos );\n"\
	"		col = 0;\n"\
	"		while ( col < width  )\n"\
	"		{\n"\
	"			if ( col+pos < bytes )\n"\
	"				say ( \"%02X \", GetByte (data+pos+col) );\n"\
	"			else\n"\
	"				say ( \"   \" );\n"\
	"			col++;\n"\
	"		}\n"\
	"\n"\
	"		col = 0;\n"\
	"		while ( col < width )\n"\
	"		{\n"\
	"			if ( col+pos < bytes )\n"\
	"			{\n"\
	"				byte = GetByte (data+pos+col);\n"\
	"				if ( byte < 0x80 && byte > 0x20 )\n"\
	"					say ( \"%c\", byte );\n"\
	"				else\n"\
	"					say ( \".\" );\n"\
	"			}\n"\
	"			else\n"\
	"				say ( \" \" );\n"\
	"\n"\
	"			col++;\n"\
	"		}\n"\
	"		say ( \"\n\" );\n"\
	"		pos += width;\n"\
	"	}\n"\
	"\n"\
	"	say ( \"\n\" );\n"\
	"	say ( \"---------- printed all bytes ---------\n\" );\n"\
	"	return;\n"\
	"}\n"\


#define UTIL_AUTO_MISC_PRE_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;LOC_UNIQUE&nbsp;&nbsp;&nbsp;&nbsp;1<br></font>"\
	"<font color=\"#008000\">#define&nbsp;LOC_FORWARD&nbsp;&nbsp;&nbsp;0<br></font>"\
	"<font color=\"#008000\">#define&nbsp;LOC_BACKWARD&nbsp;&nbsp;2<br></font>"\
	"<br>"\
	"<font color=\"#008000\">#define&nbsp;DEBUG_FMT&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0x0001<br></font>"\
	"<font color=\"#008000\">#define&nbsp;DEBUG_SEER&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0x0002<br></font>"\
	"<font color=\"#008000\">#define&nbsp;DEBUG_ARM&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0x0004<br></font>"\
	"<font color=\"#008000\">#define&nbsp;DEBUG_NOKIA&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0x0008<br></font>"\
	"<font color=\"#008000\">#define&nbsp;DEBUG_LIST&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;0x0010<br></font>"\
	"<font color=\"#008000\">#define&nbsp;DEBUG_UNKNOWN&nbsp;&nbsp;&nbsp;0x0020<br></font>"\
	"<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">char</font>*&nbsp;<font color=\"#000000\">t_function2</font>;<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">char</font>*&nbsp;<font color=\"#000000\">func</font>;<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#800000\">char</font>*&nbsp;<font color=\"#000000\">object</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_list</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER_LIST</font>(<font color=\"#000000\">t_list</font>);<br>"\
	"}&nbsp;<font color=\"#000000\">t_list</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_priv</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">STRUCT_HEADER</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_priv</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_locator</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">func_name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">pattern</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">mask</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_locator</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_function_block</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">length</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_function_block</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_function_relative_data</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">offset</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">type</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">dest</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_function_relative_data</font>;<br>"\
	"<br>"\
	"<font color=\"#000080\"><b>typedef</b></font>&nbsp;<font color=\"#000080\"><b>struct</b></font>&nbsp;<font color=\"#000000\">s_function</font><br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">name</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_function_block</font>&nbsp;*<font color=\"#000000\">blocks</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">t_function_relative_data</font>&nbsp;*<font color=\"#000000\">rel_data</font>;<br>"\
	"}&nbsp;<font color=\"#000000\">t_function</font>;<br>"\
	"<br>"\
	"<br>"\
	"		</font>"\


#define UTIL_AUTO_MISC_POST_HTML \
	"		<font face=\"Monospace\">"\
	"<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">FindPattern</font>(<font color=\"#000000\">t_locator</font>&nbsp;*<font color=\"#000000\">loc</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__util_find_pattern</font>(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">loc</font>,&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#000000\">options</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">QuickFindPattern</font>(<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">pattern</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">mask</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__util_quick_find_pattern</font>(<font color=\"#000000\">GetWorkspace</font>(),<font color=\"#000000\">pattern</font>,&nbsp;<font color=\"#000000\">mask</font>,&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#000000\">start</font>,&nbsp;<font color=\"#000000\">end</font>,&nbsp;<font color=\"#000000\">options</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">SimpleFindPattern</font>(<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">pattern</font>,&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">mask</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">options</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__util_simple_find_pattern</font>(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">pattern</font>,&nbsp;<font color=\"#000000\">mask</font>,&nbsp;<font color=\"#000000\">size</font>,&nbsp;<font color=\"#000000\">options</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">Valid</font>(<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_valid</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">GetSize</font>()&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_get_size</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>());&nbsp;}<br>"\
	"<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">CacheDisable</font>()&nbsp;{&nbsp;<font color=\"#000000\">__util_cache_disable</font>();&nbsp;}<br>"\
	"<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">CacheEnable</font>()&nbsp;{&nbsp;<font color=\"#000000\">__util_cache_enable</font>();&nbsp;}<br>"\
	"<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">DumpBytes</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">width</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>)&nbsp;{&nbsp;<font color=\"#000000\">__util_dump_bytes</font>(<font color=\"#000000\">data</font>,&nbsp;<font color=\"#000000\">width</font>,&nbsp;<font color=\"#000000\">bytes</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">GetByte</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_get_b</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">GetHalf</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_get_h</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">GetWord</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_get_w</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">SetByte</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>,<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">y</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_set_b</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>,&nbsp;<font color=\"#000000\">y</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">SetHalf</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>,<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">y</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_set_h</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>,&nbsp;<font color=\"#000000\">y</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">SetWord</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>,<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">y</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_set_w</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>,&nbsp;<font color=\"#000000\">y</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">MemCpy</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>,<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">y</font>,<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">z</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_memcpy</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),<font color=\"#000000\">x</font>,<font color=\"#000000\">y</font>,<font color=\"#000000\">z</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">MemCpyGet</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">x</font>,<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">y</font>,<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">z</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_memcpy_get</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),<font color=\"#000000\">x</font>,<font color=\"#000000\">y</font>,<font color=\"#000000\">z</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">MemCpyPut</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>,<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">y</font>,<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">z</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_memcpy_put</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),<font color=\"#000000\">x</font>,<font color=\"#000000\">y</font>,<font color=\"#000000\">z</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">GetPtr</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_get_ptr</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">GetStr</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_get_str</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">GetStrLen</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__v_get_strlen</font>&nbsp;(<font color=\"#000000\">GetWorkspace</font>(),&nbsp;<font color=\"#000000\">x</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">SH</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__util_swap_half</font>&nbsp;(<font color=\"#000000\">x</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">SW</font>(<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">x</font>)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__util_swap_word</font>&nbsp;(<font color=\"#000000\">x</font>);&nbsp;}<br>"\
	"<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">SetDebugLevel</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">debuglevel</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000000\">__util_set_debuglevel</font>&nbsp;(&nbsp;<font color=\"#000000\">debuglevel</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">SetErrorLevel</font>&nbsp;(&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">errorlevel</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000000\">__util_set_errorlevel</font>&nbsp;(&nbsp;<font color=\"#000000\">errorlevel</font>&nbsp;);&nbsp;}<br>"\
	"<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">Str2Int</font>&nbsp;(&nbsp;<font color=\"#800000\">char</font>&nbsp;*<font color=\"#000000\">str</font>&nbsp;)&nbsp;{&nbsp;<font color=\"#000080\"><b>return</b></font>&nbsp;<font color=\"#000000\">__util_str2int</font>&nbsp;(&nbsp;<font color=\"#000000\">str</font>&nbsp;);&nbsp;}<br>"\
	"<br>"\
	"<font color=\"#800000\">void</font>&nbsp;<font color=\"#000000\">DrawBytes</font>&nbsp;(&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">data</font>,&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">width</font>,&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">bytes</font>&nbsp;)<br>"\
	"{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">pos</font>&nbsp;=&nbsp;<font color=\"#0000FF\">0</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">int</font>&nbsp;<font color=\"#000000\">col</font>&nbsp;=&nbsp;<font color=\"#0000FF\">0</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#800000\">unsigned</font>&nbsp;<font color=\"#800000\">char</font>&nbsp;<font color=\"#000000\">byte</font>&nbsp;=&nbsp;<font color=\"#0000FF\">0</font>;<br>"\
	"<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;--------&nbsp;print&nbsp;bytes&nbsp;at&nbsp;0x%08X&nbsp;-------\n&quot;</font>,&nbsp;<font color=\"#000000\">data</font>&nbsp;);<br>"\
	"<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">while</font>&nbsp;(&nbsp;<font color=\"#000000\">pos</font>&nbsp;&lt;&nbsp;<font color=\"#000000\">bytes</font>&nbsp;)<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;%08X:&nbsp;&quot;</font>,&nbsp;<font color=\"#000000\">pos</font>&nbsp;);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">col</font>&nbsp;=&nbsp;<font color=\"#0000FF\">0</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">while</font>&nbsp;(&nbsp;<font color=\"#000000\">col</font>&nbsp;&lt;&nbsp;<font color=\"#000000\">width</font>&nbsp;&nbsp;)<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>if</b></font>&nbsp;(&nbsp;<font color=\"#000000\">col</font>+<font color=\"#000000\">pos</font>&nbsp;&lt;&nbsp;<font color=\"#000000\">bytes</font>&nbsp;)<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;%02X&nbsp;&quot;</font>,&nbsp;<font color=\"#000000\">GetByte</font>&nbsp;(<font color=\"#000000\">data</font>+<font color=\"#000000\">pos</font>+<font color=\"#000000\">col</font>)&nbsp;);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>else</b></font><br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;&nbsp;&nbsp;&nbsp;&quot;</font>&nbsp;);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">col</font>++;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}<br>"\
	"<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">col</font>&nbsp;=&nbsp;<font color=\"#0000FF\">0</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">while</font>&nbsp;(&nbsp;<font color=\"#000000\">col</font>&nbsp;&lt;&nbsp;<font color=\"#000000\">width</font>&nbsp;)<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>if</b></font>&nbsp;(&nbsp;<font color=\"#000000\">col</font>+<font color=\"#000000\">pos</font>&nbsp;&lt;&nbsp;<font color=\"#000000\">bytes</font>&nbsp;)<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;{<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">byte</font>&nbsp;=&nbsp;<font color=\"#000000\">GetByte</font>&nbsp;(<font color=\"#000000\">data</font>+<font color=\"#000000\">pos</font>+<font color=\"#000000\">col</font>);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>if</b></font>&nbsp;(&nbsp;<font color=\"#000000\">byte</font>&nbsp;&lt;&nbsp;<font color=\"#008080\">0x80</font>&nbsp;&amp;&amp;&nbsp;<font color=\"#000000\">byte</font>&nbsp;&gt;&nbsp;<font color=\"#008080\">0x20</font>&nbsp;)<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;%c&quot;</font>,&nbsp;<font color=\"#000000\">byte</font>&nbsp;);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>else</b></font><br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;.&quot;</font>&nbsp;);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>else</b></font><br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;&nbsp;&quot;</font>&nbsp;);<br>"\
	"<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">col</font>++;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;\n&quot;</font>&nbsp;);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">pos</font>&nbsp;+=&nbsp;<font color=\"#000000\">width</font>;<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}<br>"\
	"<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;\n&quot;</font>&nbsp;);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000000\">say</font>&nbsp;(&nbsp;<font color=\"#FF0000\">&quot;----------&nbsp;printed&nbsp;all&nbsp;bytes&nbsp;---------\n&quot;</font>&nbsp;);<br>"\
	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"#000080\"><b>return</b></font>;<br>"\
	"}<br>"\
	"		</font>"\


